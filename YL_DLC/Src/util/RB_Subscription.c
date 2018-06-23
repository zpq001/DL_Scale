//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Subscription.c
//! \ingroup	util
//! \brief		Subscriber module of the observer pattern.
//!
//! This module contains the function to subscribe to a subject in the observer pattern.
//! It has functions to subscribe, unsubscribe, unsubscribeAll or notify.
//!
//! RB_SUBSCRIPTION_AttachOnce() functionality, why not implemented
//! The implementation would need a new subscription element with a RB_OS_tMsgQueue* and a bool once.
//! This would double the memory amount and needs reprogramming of all modules with subscription.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/12/19 14:27:34MEZ $
// $Revision: 1.45 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Subscription"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)

#include "RB_Subscription.h"

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Root of subscription control block list
const RB_SUBSCRIPTION_tCB* pFirstControlBlock = NULL;


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_SUBSCRIPTION_Initialize(void)
{
	pFirstControlBlock = NULL;
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Create
//--------------------------------------------------------------------------------------------------
//! \brief	Create a subscription control block
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	pSubscriberList		Subscriber list, i.e. pointer to array of message queue	pointers
//! \param	subscriberListSize	Subscriber list size, i.e. max number of subscribers
//! \param	msgSource			Message source, sent with notify()
//! \return	none
//!
//! \code
//! // Event subscriptions
//! static RB_SUBSCRIPTION_tCB SubscriptionControlBlock;
//! static RB_OS_tMsgQueue* SubscriberList[RB_CONFIG_POWER_NUM_OF_SUBSCRIBER];
//!
//! RB_SUBSCRIPTION_Create(&SubscriptionControlBlock,
//!						   SubscriberList,
//!						   RB_CONFIG_POWER_NUM_OF_SUBSCRIBER,
//!						   RB_IDENTIFIERS_SRC_POWER);
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_SUBSCRIPTION_Create(const RB_SUBSCRIPTION_tCB* pSubCtrBlock)
{
	RB_RESET_STRUCT(*(pSubCtrBlock->pControlData));
	RB_SUBSCRIPTION_DetachAll(pSubCtrBlock);

	// Add to list if not already in list. Note: pNext was set to zero by RB_RESET_STRUCT above
	if (pFirstControlBlock)
		{
		const RB_SUBSCRIPTION_tCB* pCB = pFirstControlBlock;
		while (pCB->pControlData->pNext)
			{
			if (pCB->pControlData->pNext == pSubCtrBlock)
				break;
			pCB = pCB->pControlData->pNext;
			}
		pCB->pControlData->pNext = pSubCtrBlock; // Append to end of list
		}
	else
		{
		// First entry
		pFirstControlBlock = pSubCtrBlock;
		}
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Attach
//--------------------------------------------------------------------------------------------------
//! \brief	Attach a subscriber	to the subscriber list
//!
//! The number of possible subscriber is defined in RB_SUBSCRIPTION_Initialize()
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	pMsgQueue			Subscriber message queue pointer
//! \return	true = OK, false = Too many subscriber -> increase number of subscriber
//--------------------------------------------------------------------------------------------------
bool RB_SUBSCRIPTION_Attach(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, RB_OS_tMsgQueue* pMsgQueue)
{
	return RB_SUBSCRIPTION_AttachWithCallback(pSubCtrBlock, pMsgQueue, NULL);
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_AttachWithCallback
//--------------------------------------------------------------------------------------------------
//! \brief	Attach a subscriber	to the subscriber list with callback function
//!
//! The number of possible subscriber is defined in RB_SUBSCRIPTION_Initialize()
//! The callback will be invoked on subscription notification.
//!
//! This callback could e.g. set an RTOS event to make a pending RTOS task ready when the
//! subscription has been signaled.
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	pMsgQueue			Subscriber message queue pointer
//! \param	pNotifyCallback		Callback function that will be invoked on notification.
//! \return	true = OK, false = Too many subscribers or no callback support
//--------------------------------------------------------------------------------------------------
bool RB_SUBSCRIPTION_AttachWithCallback(const RB_SUBSCRIPTION_tCB* pSubCtrBlock,
														RB_OS_tMsgQueue* pMsgQueue,
														RB_SUBSCRIPTION_tNotifyCallback pNotifyCallback)
{
	uint16_t i;

	// Throw error if we want to add a callback but the subscription does not support callbacks
	if ((pNotifyCallback != NULL) && (pSubCtrBlock->pCallbackList == NULL)) {
		RB_DEBUG_WARN("current pSubCtrBlock does not support callbacks - provide pSubCtrBlock->pCallbackList");
		return false;
	}

	// Avoid float64 subscription of same MsgQueue, i.e. remove previously installed MsgQueue
	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		if (pSubCtrBlock->pSubList[i] == pMsgQueue) {
			pSubCtrBlock->pSubList[i] = NULL;
		}
	}

	// Insert MsgQueue in subscriber list
	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		if (pSubCtrBlock->pSubList[i] == NULL) {
			pSubCtrBlock->pSubList[i] = pMsgQueue;

			// Install notification callback if subscription supports adding a callback
			if (pSubCtrBlock->pCallbackList) {
				pSubCtrBlock->pCallbackList[i] = pNotifyCallback;
			}
			return true;
		}
	}

	// List too short
	RB_DEBUG_WARN("Too many subscribers");
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Detach
//--------------------------------------------------------------------------------------------------
//! \brief	Detach, i.e. remove a subscriber from the subscriber list
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	pMsgQueue			Subscriber message queue pointer
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
bool RB_SUBSCRIPTION_Detach(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, const RB_OS_tMsgQueue* pMsgQueue)
{
	uint16_t i;

	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		if (pSubCtrBlock->pSubList[i] == pMsgQueue) {
			pSubCtrBlock->pSubList[i] = NULL;
			if (pSubCtrBlock->pCallbackList) {
				pSubCtrBlock->pCallbackList[i] = NULL;
			}
			return true;
		}
	}
	RB_DEBUG_WARN("No such subscriber");
	return false;
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_DetachAll
//--------------------------------------------------------------------------------------------------
//! \brief	Detach, i.e. remove all subscribers from the subscriber list
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_SUBSCRIPTION_DetachAll(const RB_SUBSCRIPTION_tCB* pSubCtrBlock)
{
	uint16_t i;

	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		pSubCtrBlock->pSubList[i] = NULL;
		if (pSubCtrBlock->pCallbackList) {
			pSubCtrBlock->pCallbackList[i] = NULL;
		}
	}
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Notify
//--------------------------------------------------------------------------------------------------
//! \brief	Send notification to all subscribers
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	event				Event, which is sent to all subscriber
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_SUBSCRIPTION_Notify(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, RB_OS_tEvent event)
{
	uint16_t i;
	RB_OS_tStatus sts;

	// Send message to all subscriber
	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		if (pSubCtrBlock->pSubList[i] != NULL) {
			sts = RB_OS_MsgQueuePostEvent(pSubCtrBlock->pSubList[i], pSubCtrBlock->msgSource, event);
			if (sts == RB_OS_MSG_QUEUE_FULL) {
				RB_DEBUG_WARN("Notify() results in full message queue!");
				pSubCtrBlock->pControlData->lostMsgCount++;
				pSubCtrBlock->pControlData->totalLostCount++;
			}

			// Invoke callback if subscription supports callbacks and a callback was installed
			if((pSubCtrBlock->pCallbackList != NULL) && (pSubCtrBlock->pCallbackList[i] != NULL)) {
				pSubCtrBlock->pCallbackList[i](pSubCtrBlock);
			}
		}
	} // for
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
uint16_t RB_SUBSCRIPTION_GetMsgLostCount(const RB_SUBSCRIPTION_tCB* pSubCtrBlock)
{
	uint16_t lostMsgCount = pSubCtrBlock->pControlData->lostMsgCount;
	pSubCtrBlock->pControlData->lostMsgCount = 0U;
	return lostMsgCount;
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetAttachCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of attached subscriber
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	Number of attached subscriber
//--------------------------------------------------------------------------------------------------
uint16_t RB_SUBSCRIPTION_GetAttachCount(const RB_SUBSCRIPTION_tCB* pSubCtrBlock)
{
	size_t i;
	uint16_t attachCount = 0;
	for (i = 0; i < pSubCtrBlock->subListSize; i++) {
		if (pSubCtrBlock->pSubList[i] != NULL) {
			attachCount++;
		}
	}
	return attachCount;
}


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetFirstControlBlock
//--------------------------------------------------------------------------------------------------
//! \brief	Get the pointer to first subscription control block
//!
//! \return	Pointer to subscription control block or NULL if not available
//--------------------------------------------------------------------------------------------------
const RB_SUBSCRIPTION_tCB* RB_SUBSCRIPTION_GetFirstControlBlock(void)
{
	return pFirstControlBlock;
}


#endif // RB_CONFIG_USE_SUBSCRIPTION
