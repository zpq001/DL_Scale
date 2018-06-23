//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_State.h
//! \ingroup	util
//! \brief		This module contains general system information.
//!
//! System information like operating mode are store in RB_State. No configuration for this module
//! is needed.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Heusser Martin
//
// $Date: 2016/11/10 08:58:41MEZ $
// $Revision: 1.11 $
//
//==================================================================================================

#ifndef _RB_State__h
#define _RB_State__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_STATE) && (RB_CONFIG_USE_STATE == RB_CONFIG_YES)

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Definition of Operating modes
typedef enum {
	RB_STATE_OPMODE_USER			= 0,	//!< USER mode
	RB_STATE_OPMODE_PRODUCTION		= 1,	//!< PRODUCTION mode
	RB_STATE_OPMODE_SERVICE			= 2,	//!< SERVICE mode
	RB_STATE_OPMODE_DIAGNOSTIC		= 3		//!< DIAGNOSTIC mode
} RB_DECL_TYPE RB_STATE_tOperatingMode;

//! Definition of state parameter and data
typedef struct {
	RB_STATE_tOperatingMode			mode;	//!< Operating mode
} RB_DECL_TYPE RB_STATE_tData;



//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_STATE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STATE_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_STATE_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set type data parameter
//!
//! \param	pParams		parameters to set
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STATE_SetTDParams(const RB_STATE_tData *pParams);


//--------------------------------------------------------------------------------------------------
// RB_STATE_GetOperatingMode
//--------------------------------------------------------------------------------------------------
//! \brief	This function gets the operating mode
//!
//! \return	Value of actual operating mode
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_STATE_tOperatingMode RB_STATE_GetOperatingMode(void);


//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_STATE
#endif // _RB_State__h
