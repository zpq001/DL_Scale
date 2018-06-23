//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_RTOS.h
//! \ingroup	util
//! \brief		Interface to Rainbow for the RTOS package
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Andri Toggenburger
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.21 $
//
//==================================================================================================

#ifndef _RB_RTOS__h
#define _RB_RTOS__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_RTOS) && (RB_CONFIG_USE_RTOS == RB_CONFIG_YES)

#include "RB_Typedefs.h"
#include "RB_Version.h"

#if defined(RB_ENV_BSP_LPC3000)
	#include "RB_InterruptController.h"
#endif


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//! Mapping to RB_Version infrastructure
typedef RB_VERSION_tPackageInfo RB_DECL_TYPE RTOS_tVersionPackageInfo;

#define RTOS_VERSIONINSTALLRBPLUSINFO(INFO)		RB_VERSION_SetPackageInfo(INFO)

#define RTOS_DECL_FUNC							RB_DECL_FUNC
#define RTOS_ATTR_THREAD_SAFE					RB_ATTR_THREAD_SAFE

#if defined(RB_ENV_BSP_LPC3000)
	// Mapping to RB Interrupt Controller for LPC3000
	// For other BSPs we do not need this, as we can use the VIC register to get the ISR function pointer.
	#define RTOS_LPC3000_IRQHANDLER()			RB_INTERRUPTCONTROLLER_IRQHandler()
#endif

#ifdef __cplusplus
}
#endif

#endif // USE_RTOS

#endif // _RB_RTOS__h
