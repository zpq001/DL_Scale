//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Queue.c
//! \ingroup	util
//! \brief		This module implements Queue functionalities.
//!
//! The supported functions of the Queue module is similar to the FIFO module.
//! In comparison to the FIFO module that only supports 1-Byte elements (characters) the
//! Queue module supports any size elements.
//!
//! All functions of RB_Queue are interrupt safe.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Christian Zingg. Martin Heusser
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.28 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Queue"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Queue.h"
// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of queue structure.
//!
//! Initialize the queue and assign the buffer to it. The size of the buffer must be the size of
//! one element multiplied with the number of elements.
//!
//! \attention An already initialized queue will loose all data if it is initialized again.
//!
//! \param	pQueue				input	Pointer to queue data structure
//! \param	pBuffer				input	Pointer to queue data buffer, size must be numOfElements * lengthPerElement
//! \param	numOfElements		input	Number of elements that maximum can be stored in queue
//! \param	lengthPerElement	input	Length in bytes of a single queue element
//! \param	lowLimit			input	Lower level, e.g. for Xon
//! \param	highLimit			input	Higher level, e.g. for Xoff
//!
//! \return	none
//!
//! \par Example code:
//! \code
//!
//! #define EVENT_QUEUE_SIZE 24
//! typedef struct { uint16_t event; uint16_t message} tEvent;
//!
//! static RB_QUEUE_tQueue eventQueue; // queue data structure
//! static tEvent eventBuffer[EVENT_QUEUE_SIZE]; // queue data buffer
//!
//! RB_QUEUE_Initialize(&eventQueue, &eventBuffer, EVENT_QUEUE_SIZE, sizeof(tEvent), 3, 21);
//!
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_QUEUE_Initialize(RB_QUEUE_tQueue* pQueue, void* pBuffer, uint16_t numOfElements, uint16_t lengthPerElement, uint16_t lowLimit, uint16_t highLimit) RB_ATTR_THREAD_SAFE
	{
	RB_ENTER_CRITICAL_SECTION;
	pQueue->pBuf        = pBuffer;			// pointer to queue buffer
	pQueue->numElements = numOfElements;	// number of elements that can be stored to queue
	pQueue->lenElement  = lengthPerElement;	// size in bytes of an element in queue
	pQueue->inp         = 0u;				// Input byte index into buffer [0 to (pQueue->len - 1) valid]
	pQueue->out         = 0u;				// Output byte index into buffer [0 to (pQueue->len - 1) valid]
	pQueue->cnt         = 0u;				// Element counter   [0 to pQueue->numElements valid]
	pQueue->low         = lowLimit;			// low limit (Xon)   [0 to pQueue->numElements valid]
	pQueue->high        = highLimit;		// high limit (Xoff) [0 to pQueue->numElements valid]
	pQueue->pName       = NULL;				// Name of queue (only used from RB_OS)
	RB_LEAVE_CRITICAL_SECTION;
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_SetName
//--------------------------------------------------------------------------------------------------
//! \brief	Set the name of the queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//! \param	pName				input	Pointer to name
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_QUEUE_SetName(RB_QUEUE_tQueue* pQueue, const char *pName) RB_ATTR_THREAD_SAFE
	{
	pQueue->pName = pName;
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_GetName
//--------------------------------------------------------------------------------------------------
//! \brief	Get the name of the queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	pointer to name (never NULL)
//--------------------------------------------------------------------------------------------------
const char* RB_QUEUE_GetName(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return((pQueue->pName) ? pQueue->pName : "unknown");
	}

//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Clear
//--------------------------------------------------------------------------------------------------
//! \brief	Clear queue. All data are lost.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_QUEUE_Clear(RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	RB_ENTER_CRITICAL_SECTION;
	pQueue->inp  = 0u;
	pQueue->out  = 0u;
	pQueue->cnt  = 0u;
	RB_LEAVE_CRITICAL_SECTION;
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Put
//--------------------------------------------------------------------------------------------------
//! \brief	Puts an element into queue. If queue is full 'false' is returned.
//!
//! \attention If queue is full the element is dropped. (overfill not possible)
//!
//! \param	pQueue				input	Pointer to queue data structure
//! \param	pQueueElement		input	Pointer to queue element
//!
//! \return	true if element was put successfully, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_Put(RB_QUEUE_tQueue* pQueue, const void* pQueueElement) RB_ATTR_THREAD_SAFE
	{
	bool success = false;
	union {const void *cpV; const uint8_t *cpU8;} src; // Union to convert const void* to const uint8_t*
	uint16_t i = pQueue->lenElement;
	src.cpV = pQueueElement;

	RB_ENTER_CRITICAL_SECTION;
	if (pQueue->cnt < pQueue->numElements)
		{
		uint8_t *dst = &pQueue->pBuf[pQueue->inp * i];
		// Copy element to queue buffer. Inline copy is used for speed (much faster than memcpy).
		while (i--)
			*dst++ = *src.cpU8++;
		pQueue->cnt++;
		pQueue->inp++;
		if (pQueue->inp >= pQueue->numElements)
			pQueue->inp = 0;
		success = true;
		}
	RB_LEAVE_CRITICAL_SECTION;

	return(success);
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Gets an element from queue.
//!
//! This function reads from queue. If an element is available it is copied to the
//! pQueueElement pointer and true is returned. If no element is in queue the function
//! returns false.
//!
//! \param	pQueue				input	Pointer to queue data structure
//! \param	pQueueElement		output	Pointer to queue element
//!
//! \return	true if an element was read from queue and copied to 'pQueueElement', false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_Get(RB_QUEUE_tQueue* pQueue, void* pQueueElement) RB_ATTR_THREAD_SAFE
	{
	bool success = false;
	union {void *pV; uint8_t *pU8;} dst; // Union to convert void* to uint8_t*
	uint16_t i = pQueue->lenElement;
	dst.pV = pQueueElement;

	RB_ENTER_CRITICAL_SECTION;
	if (pQueue->cnt > 0)
		{
		const uint8_t *src = &pQueue->pBuf[pQueue->out * i];
		// Copy queue buffer to element. Inline copy is used for speed (much faster than memcpy).
		while (i--)
			*dst.pU8++ = *src++;
		pQueue->cnt--;
		pQueue->out++;
		if (pQueue->out >= pQueue->numElements)
			pQueue->out = 0;
		success = true;
		}
	RB_LEAVE_CRITICAL_SECTION;

	return(success);
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsEmpty
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue is empty.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true if queue is empty, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_IsEmpty(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return((bool)(pQueue->cnt == 0));
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsFull
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue is full.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true if queue is full, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_IsFull(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return((bool)(pQueue->cnt == pQueue->numElements));
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Free
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many elements can be put into queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	Number of free elements in queue
//--------------------------------------------------------------------------------------------------
uint16_t RB_QUEUE_Free(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return(pQueue->numElements - pQueue->cnt);
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Level
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many elements are in the queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	Number of elements in queue
//--------------------------------------------------------------------------------------------------
uint16_t RB_QUEUE_Level(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return(pQueue->cnt);
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsLow
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue fill level is less or equal lowLimit. Used for Xon.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true, if queue fill level is less or equal lowLimit
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_IsLow(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return((bool)(pQueue->cnt <= pQueue->low));
	}


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsHigh
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue fill level is greater or equal highLimit. Used for Xoff.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true, if queue fill level is greater or equal highLimit
//--------------------------------------------------------------------------------------------------
bool RB_QUEUE_IsHigh(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE
	{
	return((bool)(pQueue->cnt >= pQueue->high));
	}


//--------------------------------------------------------------------------------------------------
