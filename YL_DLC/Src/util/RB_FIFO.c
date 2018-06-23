//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_FIFO.c
//! \ingroup	util
//! \brief		Functions for a fifo buffer.
//!
//! RB_FIFO is outdated and is wrapped to RB_Queue with element size = 1 byte.
//! Most functionality is wrapped by preprocessor to RB_Queue. Only RB_FIFO_Get, RB_FIFO_Put and
//! RB_FIFO_TryPut is implemented as a procedural function for compatibility.
//!
//! \attention Same FIFO must never be used with multiple sinks!
//!
//! All fifo functions are threadsave, i.e you can put characters from an interrupt routine
//! and get characters from the main program without any special protection.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger, Martin Heusser
//
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.42 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_FIFO"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_FIFO.h"
// This module is mandatory and has no RB_CONFIG_USE, no check is needed here.


//==================================================================================================
// G L O B A L    F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_FIFO_Get
//--------------------------------------------------------------------------------------------------
//! \brief	Get an octet from fifo.
//!
//! Needs to be implemented as procedural wrapper since result must be passed in different form.
//!
//! \attention Check with RB_FIFO_Level or RB_FIFO_IsEmpty before using RB_FIFO_Get.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \return	octet from fifo
//--------------------------------------------------------------------------------------------------
uint8_t RB_FIFO_Get(RB_FIFO_tFifo *fifo) RB_ATTR_THREAD_SAFE
    {
	uint8_t c;
	RB_QUEUE_Get((RB_QUEUE_tQueue*)fifo, &c);
	return(c);
    }

//--------------------------------------------------------------------------------------------------
// RB_FIFO_TryPut
//--------------------------------------------------------------------------------------------------
//! \brief	Try to put an octet into fifo. If fifo is full then octet is dropped.
//!
//! Needs to be implemented as procedural wrapper since result must be passed in different form.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \param	c           input   Octet to put into fifo data buffer
//! \return	true if successful, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_FIFO_TryPut(RB_FIFO_tFifo *fifo, uint8_t c) RB_ATTR_THREAD_SAFE
    {
    return RB_QUEUE_Put((RB_QUEUE_tQueue*)fifo, &c);
    }


//--------------------------------------------------------------------------------------------------
// RB_FIFO_Put
//--------------------------------------------------------------------------------------------------
//! \brief	Put an octet into fifo.
//!
//! Needs to be implemented as procedural wrapper since result must be passed in different form.
//!
//! \param	fifo        input   Pointer to fifo data structure
//! \param	c           input   Octet to put into fifo data buffer
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_FIFO_Put(RB_FIFO_tFifo *fifo, uint8_t c) RB_ATTR_THREAD_SAFE
    {
	RB_QUEUE_Put((RB_QUEUE_tQueue*)fifo, &c);
    }


//--------------------------------------------------------------------------------------------------
