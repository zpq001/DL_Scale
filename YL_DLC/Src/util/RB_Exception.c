//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Exception.c
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
// $Date: 2017/05/18 15:30:59MESZ $
// $Revision: 1.69 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Exception"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_EXCEPTION) && (RB_CONFIG_USE_EXCEPTION == RB_CONFIG_YES)

#include "RB_Exception.h"

#include "RB_Debug.h"
#include "RB_OS.h"

#if defined(RB_CONFIG_EXCEPTION_LOG_SIZE) && (RB_CONFIG_EXCEPTION_LOG_SIZE > 0)
	#define SUPPORT_EXCEPTION_LOG
	#include "RB_Queue.h"
#endif

#if !defined(RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER)
	#define RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER	2
#endif

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)
	#if !defined(RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER)
		#error RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER must be defined in RB_Config.h
	#endif
	#if defined(RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER) && (RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER > 0)
		#define SUPPORT_SUBSCRIBE
	#endif
#endif

#ifdef SUPPORT_SUBSCRIBE
	#include "RB_Subscription.h"
#endif

#ifndef NDEBUG
	#include "RB_Format.h"
	#include <string.h>
#endif


//==================================================================================================
//  L O C A L   T Y P E S
//==================================================================================================

#ifdef SUPPORT_EXCEPTION_LOG
typedef struct {
	RB_OS_tSource source;
	RB_OS_tEvent event;
} tException;
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Message queue control block
static RB_OS_tMsgQueue 		s_msgQueue;

//! Message queue buffer
static RB_OS_tMessage		s_msgBuf[RB_CONFIG_EXCEPTION_MESSAGE_QUEUE_SIZE];

//! Lost message counter
static uint16_t				s_msgLostCounter;

#ifdef SUPPORT_SUBSCRIBE
//! Event subscriptions. Note: SubscriptionControlBlock must not be const here to adapt message source
static RB_OS_tMsgQueue* SubscriberList[RB_CONFIG_EXCEPTION_NUM_OF_SUBSCRIBER];
static RB_SUBSCRIPTION_tCD SubscriptionControlData;
static RB_SUBSCRIPTION_tCB SubscriptionControlBlock = {SubscriberList,
		RB_ARRAY_SIZE(SubscriberList),
		RB_IDENTIFIERS_SRC_EXCEPTION,
		&SubscriptionControlData,
		RB_MODULE_NAME,
		NULL};
#endif

//! Total number of exceptions
static int s_exceptionCounter;

#ifdef SUPPORT_EXCEPTION_LOG
//! Queue to log received exceptions
static RB_QUEUE_tQueue s_exceptionLog;
//! Buffer for the logged exceptions
static tException s_exceptionLogBuffer[RB_CONFIG_EXCEPTION_LOG_SIZE];
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_EXCEPTION_Initialize(void)
{
	s_msgLostCounter = 0;
	s_exceptionCounter = 0;
	RB_OS_MsgQueueCreate(&s_msgQueue, s_msgBuf, RB_CONFIG_EXCEPTION_MESSAGE_QUEUE_SIZE, RB_MODULE_NAME);

#ifdef SUPPORT_EXCEPTION_LOG
	RB_QUEUE_Initialize(&s_exceptionLog,
						s_exceptionLogBuffer,
						RB_CONFIG_EXCEPTION_LOG_SIZE,
						sizeof(tException),
						0,
						0);
#endif

#ifdef SUPPORT_SUBSCRIBE
	RB_SUBSCRIPTION_Create(&SubscriptionControlBlock);
#endif
}


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
RB_OS_tStatus RB_EXCEPTION_Post(RB_OS_tSource source, RB_OS_tEvent event)
{
	RB_OS_tStatus sts;

	s_exceptionCounter++;
#ifdef SUPPORT_EXCEPTION_LOG
	// Add the new exception to the log if there is any space left
	if (!RB_QUEUE_IsFull(&s_exceptionLog)) {
		tException exception;
		exception.source = source;
		exception.event = event;
		RB_QUEUE_Put(&s_exceptionLog, &exception);
	}
#endif

#ifndef NDEBUG
	{
		char msg[40] = "Source/Event = ";
		size_t len;
		RB_FORMAT_ULong(&msg[strlen(msg)], (uint32_t)source, 8u);
		len = strlen(msg);
		msg[len++] = (char)'/';
		msg[len] = (char)'\0';
		RB_FORMAT_ULong(&msg[len], (uint32_t)event, 8u);
		RB_DEBUG_FAIL(msg);
	}
#endif

	sts = RB_OS_MsgQueuePostEvent(&s_msgQueue, source, event);
	if (sts == RB_OS_MSG_QUEUE_FULL) {
		if (s_msgLostCounter < 9999u) {
			s_msgLostCounter++;
		}
	}

#ifdef SUPPORT_SUBSCRIBE
	// Send message with original source to subscribers
	SubscriptionControlBlock.msgSource = source;
	RB_SUBSCRIPTION_Notify(&SubscriptionControlBlock, (RB_OS_tEvent)event);
	// Reset the source, otherwise the ZZ09 function may try to use the subscription control block
	// of RB_Exception to send events because the source ID matches.
	SubscriptionControlBlock.msgSource = RB_IDENTIFIERS_SRC_EXCEPTION;
#endif

	return (sts);
}


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
RB_OS_tStatus RB_EXCEPTION_Accept(RB_OS_tMessage* pException)
{
	return (RB_OS_MsgQueueAccept(&s_msgQueue, pException));
}


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
RB_OS_tStatus RB_EXCEPTION_Pend(RB_OS_tMessage* pException)
{
	return (RB_OS_MsgQueuePend(&s_msgQueue, pException));
}


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
uint16_t RB_EXCEPTION_GetMsgLostCount(void)
{
	uint16_t lost = s_msgLostCounter;
	s_msgLostCounter = 0;
	return (lost);
}


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
bool RB_EXCEPTION_Subscribe(RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_Attach(&SubscriptionControlBlock, pMsgQueue);
#else
	RB_UNUSED(pMsgQueue);
	return false;
#endif
}


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
bool RB_EXCEPTION_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_Detach(&SubscriptionControlBlock, pMsgQueue);
#else
	RB_UNUSED(pMsgQueue);
	return false;
#endif
}


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetSubscriptionMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
uint16_t RB_EXCEPTION_GetSubscriptionMsgLostCount(void)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_GetMsgLostCount(&SubscriptionControlBlock);
#else
	return 0u;
#endif
}


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetExceptionCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of exceptions and reset counter
//!
//! \return	Number of exceptions
//--------------------------------------------------------------------------------------------------
int RB_EXCEPTION_GetExceptionCount(void)
{
	int exceptionCount = s_exceptionCounter;
	s_exceptionCounter = 0;
	return exceptionCount;
}


//--------------------------------------------------------------------------------------------------
// RB_EXCEPTION_GetExceptionFromLog
//--------------------------------------------------------------------------------------------------
//! \brief	Get the oldest exception from the exception log and delete it
//!
//! \return	false if log is empty
//--------------------------------------------------------------------------------------------------
bool RB_EXCEPTION_GetExceptionFromLog(RB_OS_tSource* pSource, RB_OS_tEvent* pEvent)
{
#ifdef SUPPORT_EXCEPTION_LOG
	tException exception;
	if (RB_QUEUE_Get(&s_exceptionLog, &exception)) {
		*pSource = exception.source;
		*pEvent = exception.event;
		return true;
	}
#else
	RB_UNUSED(pSource);
	RB_UNUSED(pEvent);
#endif
	return false;
}


#endif // RB_CONFIG_USE_EXCEPTION
