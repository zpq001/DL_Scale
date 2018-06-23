//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Power.c
//! \ingroup	util
//! \brief		Interface to power and battery status.
//!
//! This module contains the interface to the power and battery status.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/12/19 14:27:34MEZ $
// $Revision: 1.62 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Power"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_POWER) && (RB_CONFIG_USE_POWER == RB_CONFIG_YES)

#include "RB_Power.h"

#include "RB_Debug.h"
#include "RB_OS.h"

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)
	#if !defined(RB_CONFIG_POWER_NUM_OF_SUBSCRIBER)
		#error RB_CONFIG_POWER_NUM_OF_SUBSCRIBER must be defined in RB_Config.h
	#endif
	#if defined(RB_CONFIG_POWER_NUM_OF_SUBSCRIBER) && (RB_CONFIG_POWER_NUM_OF_SUBSCRIBER > 0)
		#define SUPPORT_SUBSCRIBE
	#endif
#endif

#ifdef SUPPORT_SUBSCRIBE
	#include "RB_Subscription.h"
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Power state
static RB_POWER_tState powerState = RB_POWER_OFF;

#ifdef SUPPORT_SUBSCRIBE
//! Event subscriptions
static RB_OS_tMsgQueue* SubscriberList[RB_CONFIG_POWER_NUM_OF_SUBSCRIBER];
static RB_SUBSCRIPTION_tCD SubscriptionControlData;
static const RB_SUBSCRIPTION_tCB SubscriptionControlBlock = {SubscriberList,
		RB_ARRAY_SIZE(SubscriberList),
		RB_IDENTIFIERS_SRC_POWER,
		&SubscriptionControlData,
		RB_MODULE_NAME,
		NULL};
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_POWER_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_POWER_Initialize(void)
{
#ifdef SUPPORT_SUBSCRIBE
	RB_SUBSCRIPTION_Create(&SubscriptionControlBlock);
#endif

	powerState = RB_POWER_STARTUP;
}


//--------------------------------------------------------------------------------------------------
// RB_POWER_Subscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller subscribes to messages (events)
//!
//! The number of possible subscriber is defined in RB_Config.h (RB_CONFIG_POWER_NUM_OF_SUBSCRIBER)
//!
//! \param	pMsgQueue	Message queue pointer, to send messages (events)
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
bool RB_POWER_Subscribe(RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	if (RB_SUBSCRIPTION_Attach(&SubscriptionControlBlock,pMsgQueue)) {
		return true;
	}
	RB_DEBUG_WARN("Too many subscriber");
#else
	RB_UNUSED(pMsgQueue);
#endif
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_POWER_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
bool RB_POWER_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue)
{
#ifdef SUPPORT_SUBSCRIBE
	if (RB_SUBSCRIPTION_Detach(&SubscriptionControlBlock,pMsgQueue)) {
			return true;
	}
	RB_DEBUG_WARN("No such subscriber");
#else
	RB_UNUSED(pMsgQueue);
#endif
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_POWER_SetState
//--------------------------------------------------------------------------------------------------
//! \brief	Set power state and send message to subscriber
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_POWER_SetState(RB_POWER_tState state)
{
	// Check for state change
	if (powerState != state) {

		// Set new state
		powerState = state;

#ifdef SUPPORT_SUBSCRIBE
		// Send new state to subscriber
		RB_SUBSCRIPTION_Notify(&SubscriptionControlBlock, (RB_OS_tEvent)powerState);
#endif

	} // if
}


//--------------------------------------------------------------------------------------------------
// RB_POWER_GetState
//--------------------------------------------------------------------------------------------------
//! \brief	Get power state
//!
//! \return	RB_POWER_tState
//--------------------------------------------------------------------------------------------------
RB_POWER_tState RB_POWER_GetState(void)
{
	return powerState;
}


//--------------------------------------------------------------------------------------------------
// RB_POWER_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
uint16_t RB_POWER_GetMsgLostCount(void)
{
#ifdef SUPPORT_SUBSCRIBE
	return RB_SUBSCRIPTION_GetMsgLostCount(&SubscriptionControlBlock);
#else
	return 0;
#endif
}


#endif // RB_CONFIG_USE_POWER
