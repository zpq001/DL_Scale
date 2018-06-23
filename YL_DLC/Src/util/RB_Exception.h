//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Exception.h
//! \ingroup	util
//! \brief		Functions for the exception handling.
//!
//! This module contains the functions for the exception handling.
//! The exception is posted into a message queue, which must be periodically looked up by the
//! main program loop for exceptions with the function RB_EXCEPTION_Accept().
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2017/02/23 15:13:49MEZ $
// $Revision: 1.40 $
//
//==================================================================================================

#ifndef _RB_Exception__h
#define _RB_Exception__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_EXCEPTION) && (RB_CONFIG_USE_EXCEPTION == RB_CONFIG_YES)

#include "RB_Typedefs.h"
#include "RB_OS.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_EXCEPTION_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Post
//--------------------------------------------------------------------------------------------------
//! \brief	Post an exception, if exception queue is not full, i.e. no waiting if queue is full.
//!
//! \param	source		Source of exception
//! \param	event		Event(= exception) occurred
//! \return	RB_OS_tStatus
//!		- RB_OS_OK					Exception placed in queue
//!		- RB_OS_MSG_QUEUE_FULL		Exception queue is full, nothing done
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_OS_tStatus RB_EXCEPTION_Post(RB_OS_tSource source, RB_OS_tEvent event);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Accept
//--------------------------------------------------------------------------------------------------
//! \brief	Receive an exception, return with RB_OS_NO_MSG_AVAILABLE if no exception is available.
//!
//! \param	pException	Exception
//! \return	RB_OS_tStatus
//!		- RB_OS_OK					Exception received, i.e. copied to pException
//!		- RB_OS_NO_MSG_AVAILABLE	No exception available
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_OS_tStatus RB_EXCEPTION_Accept(RB_OS_tMessage* pException);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Pend
//--------------------------------------------------------------------------------------------------
//! \brief	Pend for an exception, wait if no exception is available.
//!
//! \param	pException	Exception
//! \return	RB_OS_tStatus
//!		- RB_OS_OK					Exception received, i.e. copied to pException
//!		- RB_OS_NO_MSG_AVAILABLE	No exception available
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_OS_tStatus RB_EXCEPTION_Pend(RB_OS_tMessage* pException);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost exception messages and reset counter
//!
//! \return	Number of lost exception messages
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_EXCEPTION_GetMsgLostCount(void);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Subscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller subscribes to messages (events)
//!
//! The number of possible subscriber is defined in RB_Config.h (RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER)
//!
//! \param	pMsgQueue	Message queue pointer, to send messages (events)
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_EXCEPTION_Subscribe(RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_EXCEPTION_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetSubscriptionMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_EXCEPTION_GetSubscriptionMsgLostCount(void);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetExceptionCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of exceptions and reset counter
//!
//! \return	Number of exceptions
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int RB_EXCEPTION_GetExceptionCount(void);


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetExceptionFromLog
//--------------------------------------------------------------------------------------------------
//! \brief	Get the oldest exception from the exception log and delete it
//!
//! \return	false if log is empty
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_EXCEPTION_GetExceptionFromLog(RB_OS_tSource* pSource, RB_OS_tEvent* pEvent);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_EXCEPTION
#endif // _RB_Exception__h
