//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Access.c
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
// $Date: 2016/12/20 11:03:23MEZ $
// $Revision: 1.41 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Access"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_ACCESS) && (RB_CONFIG_USE_ACCESS == RB_CONFIG_YES)

#include "RB_Access.h"

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! This is a set of all accessrights, 1 = enabled
static RB_ACCESS_tAccess EnabledRights = (RB_ACCESS_tAccess)0;


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_ACCESS_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_ACCESS_Initialize(void)
{
	EnabledRights = (RB_ACCESS_tAccess)0;
}


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
void RB_ACCESS_EnableAccessright(RB_ACCESS_tAccess right)
{
	EnabledRights |= right;
}


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
void RB_ACCESS_DisableAccessright(RB_ACCESS_tAccess right)
{
	EnabledRights &= ~(unsigned int)right;
}


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_IsAccessible
//--------------------------------------------------------------------------------------------------
//! \brief	Test of one or more accessright(s)
//!
//! \param	right	Right(s) to test
//! \return	true if accessright is enabled
//--------------------------------------------------------------------------------------------------
bool RB_ACCESS_IsAccessible(RB_ACCESS_tAccess right)
{
	#ifndef NDEBUG
	if (EnabledRights == (RB_ACCESS_tAccess)0)
	{
		RB_DEBUG_WARN("No access rights currently enabled at all");
	}
	#endif // NDEBUG

	return ((EnabledRights & right) != (RB_ACCESS_tAccess)0);
}


//--------------------------------------------------------------------------------------------------
// RB_ACCESS_IsNoneAccessible
//--------------------------------------------------------------------------------------------------
//! \brief	Test if no accessright(s) are enabled
//!
//! \return	true if NO accessright is enabled
//--------------------------------------------------------------------------------------------------
bool RB_ACCESS_IsNoneAccessible(void)
{
	return ((EnabledRights == (RB_ACCESS_tAccess)0) ? true : false);
}


#endif // RB_CONFIG_USE_ACCESS
