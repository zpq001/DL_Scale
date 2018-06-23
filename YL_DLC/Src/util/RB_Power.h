//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Power.h
//! \ingroup	util
//! \brief		Interface to power and battery status.
//!
//! This module contains the interface to the power and battery status.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.37 $
//
//==================================================================================================

#ifndef _RB_Power__h
#define _RB_Power__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================
#include "RB_Config.h"

#if defined(RB_CONFIG_USE_POWER) && (RB_CONFIG_USE_POWER == RB_CONFIG_YES)

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

//! Power state
typedef enum {
	RB_POWER_OFF = 0,			//!< Power is off
	RB_POWER_STANDBY,			//!< Device is in standby mode
	RB_POWER_STARTUP,			//!< Transition from standby to on
	RB_POWER_ON_LINE,			//!< Power is on, line operated
	RB_POWER_ON_BATTERY,		//!< Power is on, battery operated
	RB_POWER_SHUTDOWN 			//!< Transition from on to standby
} RB_DECL_TYPE RB_POWER_tState;



//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_POWER_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_POWER_Initialize(void);


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
RB_DECL_FUNC bool RB_POWER_Subscribe(RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_POWER_Unsubscribe
//--------------------------------------------------------------------------------------------------
//! \brief	Caller unsubscribes to messages (events)
//!
//! \param	pMsgQueue	Message queue pointer, to cancel sending messages (events)
//! \return	true = OK, false = No such subscriber
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_POWER_Unsubscribe(const RB_OS_tMsgQueue* pMsgQueue);


//--------------------------------------------------------------------------------------------------
// RB_POWER_SetState
//--------------------------------------------------------------------------------------------------
//! \brief	Set power state
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_POWER_SetState(RB_POWER_tState state);


//--------------------------------------------------------------------------------------------------
// RB_POWER_GetState
//--------------------------------------------------------------------------------------------------
//! \brief	Get power state
//!
//! \return	RB_POWER_tState
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_POWER_tState RB_POWER_GetState(void);


//--------------------------------------------------------------------------------------------------
// RB_POWER_GetMsgLostCount
//--------------------------------------------------------------------------------------------------
//! \brief	Get the number of lost messages and reset counter
//!
//! \return	Number of lost messages
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t RB_POWER_GetMsgLostCount(void);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_POWER
#endif // _RB_Power__h
