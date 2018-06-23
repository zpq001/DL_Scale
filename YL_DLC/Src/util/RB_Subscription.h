//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Subscription.h
//! \ingroup	util
//! \brief		Subscriber module of the observer pattern.
//!
//! This module contains the function to subscribe to a subject in the observer pattern.
//! It has functions to attach, detach, detachAll or notify.
//!
//! RB_SUBSCRIPTION_AttachOnce() functionality, why not implemented
//! The implementation would need a new subscription element with a RB_OS_tMsgQueue* and a bool once.
//! This would double the memory amount and needs reprogramming of all modules with subscription.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:41MEZ $
// $Revision: 1.33 $
//
//==================================================================================================

#ifndef _RB_Subscription__h
#define _RB_Subscription__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_SUBSCRIPTION) && (RB_CONFIG_USE_SUBSCRIPTION == RB_CONFIG_YES)

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

struct RB_SUBSCRIPTION_tCB;
struct RB_SUBSCRIPTION_tCD;

//! Callback function prototype. The callback (if installed) will be called on subscription notification.
//! Parameter: the subscription control block where the callback was invoked for.
typedef RB_DECL_TYPE void (*RB_SUBSCRIPTION_tNotifyCallback)(const struct RB_SUBSCRIPTION_tCB* pSubCtrBlock);

//! Subscription control block (constant part)
typedef struct RB_SUBSCRIPTION_tCB{
	RB_OS_tMsgQueue** 					pSubList;		//! Subscriber list, i.e. array of RB_OS_tMsgQueue*
	size_t								subListSize;	//! Array size of subscriber list
	RB_OS_tSource 						msgSource;		//! Used to keep source of message
	struct RB_SUBSCRIPTION_tCD*			pControlData;	//! Pointer to control data
	const char*							pName;			//! Name of module

	//! Optional fields. Must be at end of struct declaration to be automatically initialized to 0
	//! when providing initializer for struct (e.g. constant declaration)

	RB_SUBSCRIPTION_tNotifyCallback*	pCallbackList;	//! Must be provided (!= NULL) if callbacks shall be supported:
														//! List of subscriber specific callbacks
														//! to be invoked on notification. The number
														//! of elements must be subListSize (same size as pSubList)
} RB_DECL_TYPE RB_SUBSCRIPTION_tCB;

//! Subscription control data (variable part)
typedef struct RB_SUBSCRIPTION_tCD {
	const struct RB_SUBSCRIPTION_tCB*	pNext;			//! Pointer to next subscription block
	uint16_t							lostMsgCount;	//! Counter for lost messages
	uint16_t							totalLostCount;	//! Total of lost messages
} RB_DECL_TYPE RB_SUBSCRIPTION_tCD;





//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_SUBSCRIPTION_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Create
//--------------------------------------------------------------------------------------------------
//! \brief	Create a subscription control block
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	none
//!
//! \code
//! // Defining data for event subscriptions
//! static RB_OS_tMsgQueue* SubscriberList[NUM_OF_SUBSCRIBER];
//! static RB_SUBSCRIPTION_tCD SubscriptionControlData;
//! static const RB_SUBSCRIPTION_tCB SubscriptionControlBlock = {SubscriberList,
//! 		RB_ARRAY_SIZE(SubscriberList),
//! 		RB_IDENTIFIERS_SRC_KEYBOARD,
//! 		&SubscriptionControlData,
//! 		RB_MODULE_NAME};
//!
//! // Register subscription in subscription module
//! RB_SUBSCRIPTION_Create(&SubscriptionControlBlock);
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_SUBSCRIPTION_Create(const RB_SUBSCRIPTION_tCB* pSubCtrBlock);


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
RB_DECL_FUNC bool RB_SUBSCRIPTION_Attach(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, RB_OS_tMsgQueue* pMsgQueue);


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
RB_DECL_FUNC bool RB_SUBSCRIPTION_AttachWithCallback(const RB_SUBSCRIPTION_tCB* pSubCtrBlock,
														RB_OS_tMsgQueue* pMsgQueue,
														RB_SUBSCRIPTION_tNotifyCallback pNotifyCallback);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Detach
//--------------------------------------------------------------------------------------------------
//! \brief	Detach, i.e. remove a subscriber from the subscriber list
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	pMsgQueue			Subscriber message queue pointer
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_SUBSCRIPTION_Detach(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, const RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_DetachAll
//--------------------------------------------------------------------------------------------------
//! \brief	Detach, i.e. remove all subscribers from the subscriber list
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_SUBSCRIPTION_DetachAll(const RB_SUBSCRIPTION_tCB* pSubCtrBlock);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_Notify
//--------------------------------------------------------------------------------------------------
//! \brief	Send notification to all subscribers
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \param	event				Event, which is sent to all subscriber
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_SUBSCRIPTION_Notify(const RB_SUBSCRIPTION_tCB* pSubCtrBlock, RB_OS_tEvent event);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_SUBSCRIPTION_GetMsgLostCount(const RB_SUBSCRIPTION_tCB* pSubCtrBlock);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetAttachCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of attached subscriber
//!
//! \param	pSubCtrBlock		Pointer to subscription control block
//! \return	Number of attached subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_SUBSCRIPTION_GetAttachCount(const RB_SUBSCRIPTION_tCB* pSubCtrBlock);


//--------------------------------------------------------------------------------------------------
// RB_SUBSCRIPTION_GetFirstControlBlock
//--------------------------------------------------------------------------------------------------
//! \brief	Get the pointer to first subscription control block
//!
//! \return	Pointer to subscription control block or NULL if not available
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const RB_SUBSCRIPTION_tCB* RB_SUBSCRIPTION_GetFirstControlBlock(void);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_SUBSCRIPTION
#endif // _RB_Subscription__h
