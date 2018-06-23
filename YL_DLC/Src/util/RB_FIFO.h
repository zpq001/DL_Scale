//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_FIFO.h
//! \ingroup	util
//! \brief		Functions for a fifo buffer.
//!
//! RB_FIFO is outdated and is wrapped to RB_Queue with element size = 1 byte
//!
//! \attention Same FIFO must never be used with multiple sinks!
//!
//! All fifo functions are threadsave, i.e you can put characters from an interrupt routine
//! and get characters from the main program without any special protection.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger, Martin Heusser
//!
//! \attention RB_FIFO is deprecated, use RB_Queue instead of RB_FIFO. Most functions in RB_FIFO
//! are directly routed to corresponding functions in RB_Queue, some functions are implemented as
//! wrapper functions (RB_FIFO_Put, RB_FIFO_TryPut and RB_FIFO_Get).
//!
//
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.39 $
//
//==================================================================================================

#ifndef _RB_FIFO__h
#define _RB_FIFO__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Typedefs.h"
#include "RB_Queue.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! FIFO control block (wrapped to RB_Queue)
#define RB_FIFO_tFifo	RB_QUEUE_tQueue


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_FIFO_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of fifo structure. All data is lost.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \param	buffer      input   Pointer to fifo data buffer, i.e. uint8_t[]
//! \param	length      input   Length of fifo data buffer
//! \param	lowLimit    input   Lower level, e.g. for Xon
//! \param	highLimit   input   Higher level, e.g. for Xoff
//! \return	none
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_Initialize(fifo, buffer, length, lowLimit, highLimit)	\
		RB_QUEUE_Initialize(fifo, buffer, length, sizeof(uint8_t), lowLimit, highLimit) // RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Clear
//--------------------------------------------------------------------------------------------------
//! \brief	Clear fifo buffer. All data is lost.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	none
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_Clear(fifo)	RB_QUEUE_Clear(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_TryPut
//--------------------------------------------------------------------------------------------------
//! \brief	Try to put an octet into fifo. If fifo is full then octet is dropped.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \param	c           input   Octet to put into fifo data buffer
//! \return	true if successful, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_FIFO_TryPut(RB_FIFO_tFifo *fifo, uint8_t c) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Put
//--------------------------------------------------------------------------------------------------
//! \brief	Put an octet into fifo.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \param	c           input   Octet to put into fifo data buffer
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_FIFO_Put(RB_FIFO_tFifo *fifo, uint8_t c) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Get an octet from fifo.
//!
//! \attention Check with RB_FIFO_Level or RB_FIFO_IsEmpty before using RB_FIFO_Get.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	octet from fifo
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint8_t RB_FIFO_Get(RB_FIFO_tFifo *fifo) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_FIFO_IsEmpty
//--------------------------------------------------------------------------------------------------
//! \brief	Test if fifo is empty.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	true if fifo is empty, false otherwise
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_IsEmpty(fifo)	RB_QUEUE_IsEmpty(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_IsFull
//--------------------------------------------------------------------------------------------------
//! \brief	Test if fifo is full.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	true if fifo is full, false otherwise
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_IsFull(fifo)	RB_QUEUE_IsFull(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Free
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many octets can be put into fifo.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	Number of free octets in fifo
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_Free(fifo)	RB_QUEUE_Free(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Level
//--------------------------------------------------------------------------------------------------
//! \brief	Test how many octets are in the fifo.
//
//! \param	fifo        input   Pointer to fifo data structure
//! \return	Number of octets in fifo
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_Level(fifo)	RB_QUEUE_Level(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_IsLow
//--------------------------------------------------------------------------------------------------
//! \brief	Test if fifo fill level is less or equal lowLimit. Used for Xon.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	true, if fifo fill level is less or equal lowLimit
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_IsLow(fifo)	RB_QUEUE_IsLow(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


//--------------------------------------------------------------------------------------------------
// RB_FIFO_IsHigh
//--------------------------------------------------------------------------------------------------
//! \brief	Test if fifo fill level is greater or equal highLimit. Used for Xoff.
//!
//! \param	fifo        input	Pointer to fifo data structure
//! \return	true, if fifo fill level is greater or equal highLimit
//--------------------------------------------------------------------------------------------------
#define RB_FIFO_IsHigh(fifo)	RB_QUEUE_IsHigh(fifo)	// RB_ATTR_THREAD_SAFE, direct wrapped to RB_Queue


#ifdef __cplusplus
}
#endif

#endif // _RB_FIFO__h
