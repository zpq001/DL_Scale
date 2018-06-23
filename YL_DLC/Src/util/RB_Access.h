//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Access.h
//! \ingroup	util
//! \brief		Accessrights for parameters, commands, etc.
//!
//! This module manages the global accessrights for parameters, commands, etc.
//! The user of the Rainbow modules may add his/her own accessrights in the file RB_Config.h
//! The accessrights are implemented as bitset of rights, i.e. multiple rights may be enabled or
//! disabled at the same time. See example below.
//!
//! \par Example code:
//! \code
//!	RB_ACCESS_EnableAccessright(RB_CONFIG_ACCESS_SICS + RB_CONFIG_ACCESS_XP);
//! \endcode
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.29 $
//
//==================================================================================================

#ifndef _RB_ACCESS__h
#define _RB_ACCESS__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_ACCESS) && (RB_CONFIG_USE_ACCESS == RB_CONFIG_YES)

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

//! Accessrights, may be combined, i.e. added
typedef uint16_t RB_DECL_TYPE RB_ACCESS_tAccess;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_ACCESS_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_ACCESS_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_EnableAccessright
//--------------------------------------------------------------------------------------------------
//! \brief	Enable one or more accessright(s)
//!
//! The accessrights are implemented as set of rights, i.e. multiple rights may be enabled at the
//! same time. See example in file header!
//!
//! \param	right	Right(s) to enable
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_ACCESS_EnableAccessright(RB_ACCESS_tAccess right);


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_DisableAccessright
//--------------------------------------------------------------------------------------------------
//! \brief	Disable one or more accessright(s)
//!
//! The accessrights are implemented as set of rights, i.e. multiple rights may be disabled at the
//! same time. See example in file header!
//!
//! \param	right	Right(s) to disable
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_ACCESS_DisableAccessright(RB_ACCESS_tAccess right);


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_IsAccessible
//--------------------------------------------------------------------------------------------------
//! \brief	Test of one or more accessright(s)
//!
//! \param	right	Right(s) to test
//! \return	true if accessright is enabled
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_ACCESS_IsAccessible(RB_ACCESS_tAccess right);


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_IsNoneAccessible
//--------------------------------------------------------------------------------------------------
//! \brief	Test if no accessright(s) are enabled
//!
//! \return	true if NO accessright is enabled
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_ACCESS_IsNoneAccessible(void);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_ACCESS

#endif

