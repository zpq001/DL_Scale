//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_GraphicBuffer.h
//! \ingroup	util
//! \brief		Utility module to allocate framebuffers
//!
//! This module is used to manage the framebuffers needed by the different GUI libraries and
//! graphic controllers.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Silvan Sturzenegger
//
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.9 $
//
//==================================================================================================

#ifndef _RB_GRAPHICBUFFER__h
#define _RB_GRAPHICBUFFER__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_GRAPHICCONTROLLER) && (RB_CONFIG_USE_GRAPHICCONTROLLER == RB_CONFIG_YES)


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_GRAPHICBUFFER_GetBuffer
//--------------------------------------------------------------------------------------------------
//! \brief	Get a pointer to the start of the frame buffer.
//!
//! \return	pointer to the frame buffer
//!			(void pointer because the type is dependent on the GUI library)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void* RB_GRAPHICBUFFER_GetBuffer(void);

#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_GRAPHICCONTROLLER

#endif // _RB_GRAPHICBUFFER__h

