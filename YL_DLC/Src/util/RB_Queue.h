//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Queue.h
//! \ingroup	util
//! \brief		This module implements Queue functionalities.
//!
//! RB_Queue supports first in - first out (FIFO) buffers with any element size and depth.
//!
//! \attention The element size used in Put and Get must be the same size as used during Initialize.
//! \attention Variable element sizes are not supported.
//!
//! All functions of RB_Queue are thread safe.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Christian Zingg. Martin Heusser
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.23 $
//
//==================================================================================================

#ifndef _RB_Queue__h
#define _RB_Queue__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Queue control block
typedef struct {
	uint8_t*		pBuf;			//!< Data buffer
	uint16_t		numElements;	//!< number of elements that can be stored to queue
	uint16_t		lenElement;		//!< Length of a single queue element
	uint16_t 		inp;			//!< Input pointer
	uint16_t 		out;			//!< Output pointer
	uint16_t 		cnt;			//!< Number of elements in queue
	uint16_t 		low;			//!< Low limit, --> SW-Flowcontrol (Xon)
	uint16_t 		high;			//!< High limit, --> SW-Flowcontrol (Xoff)
	const char*		pName;			//!< Name
} RB_DECL_TYPE RB_QUEUE_tQueue;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
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
RB_DECL_FUNC void RB_QUEUE_Initialize(RB_QUEUE_tQueue* pQueue, void* pBuffer, uint16_t numOfElements, uint16_t lengthPerElement, uint16_t lowLimit, uint16_t highLimit) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC void RB_QUEUE_SetName(RB_QUEUE_tQueue* pQueue, const char *pName) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_GetName
//--------------------------------------------------------------------------------------------------
//! \brief	Get the name of the queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	pointer to name (never NULL)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* RB_QUEUE_GetName(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Clear
//--------------------------------------------------------------------------------------------------
//! \brief	Clear queue. All data are lost.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_QUEUE_Clear(RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_QUEUE_Put(RB_QUEUE_tQueue* pQueue, const void* pQueueElement) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Gets an element from queue.
//!
//! This function reads from queue. If an element is available it is copied to the
//! queueElement pointer and true is returned. If no element is in queue the function
//! returns false.
//!
//! \param	pQueue				input	Pointer to queue data structure
//! \param	pQueueElement		output	Pointer to queue element
//!
//! \return	true if an element was read from queue and copied to 'queueElement', false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_QUEUE_Get(RB_QUEUE_tQueue* pQueue, void* pQueueElement) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsEmpty
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue is empty.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true if queue is empty, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_QUEUE_IsEmpty(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsFull
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue is full.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true if queue is full, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_QUEUE_IsFull(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Free
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many elements can be put into queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	Number of free elements in queue
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_QUEUE_Free(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_Level
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many elements are in the queue.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	Number of elements in queue
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_QUEUE_Level(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsLow
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue fill level is less or equal lowLimit. Used for Xon.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true, if queue fill level is less or equal lowLimit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_QUEUE_IsLow(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_QUEUE_IsHigh
//--------------------------------------------------------------------------------------------------
//! \brief	Test if queue fill level is greater or equal highLimit. Used for Xoff.
//!
//! \param	pQueue				input	Pointer to queue data structure
//!
//! \return	true, if queue fill level is greater or equal highLimit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_QUEUE_IsHigh(const RB_QUEUE_tQueue* pQueue) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif // _RB_Queue__h
