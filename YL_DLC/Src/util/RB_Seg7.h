//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Seg7.h
//! \ingroup	util
//! \brief		Convert ASCII characters to 7-segment code.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.28 $
//
//==================================================================================================

#ifndef _RB_Seg7__h
#define _RB_Seg7__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_DISPLAY) && (RB_CONFIG_USE_DISPLAY == RB_CONFIG_YES) && \
    defined(RB_CONFIG_USE_SEG7) && (RB_CONFIG_USE_SEG7 == RB_CONFIG_YES)

#include "RB_Typedefs.h"


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
// RB_SEG7_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_SEG7_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_SEG7_GetCode
//--------------------------------------------------------------------------------------------------
//! \brief	Return 7-segment code for an ASCII character, range	0x1A..0x7F/0xFF, otherwise return 0.
//!
//! \param	ascii	input	ASCII character
//! \return	7-segment code
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint8_t RB_SEG7_GetCode(char ascii);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_DISPLAY && RB_CONFIG_USE_SEG7
#endif // _RB_Seg7__h