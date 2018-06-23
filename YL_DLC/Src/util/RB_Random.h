//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Random.h
//! \ingroup	util
//! \brief		This module includes pseudo random sequence generator from numerical recipies.
//!
//! \attention
//! The module must be initialized by setting up a good random seed (set seed by RB_RANDOM_Seed) before usage.
//! A good random seed shall be different after every system restart and shall be as random itself as possible (use hardware
//! such as RTC, A/D converter with noise etc. to get a good random seed). Do NOT use the system ticker if possible.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author	George Fankhauser
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.25 $
//
//==================================================================================================

#ifndef _RB_Random__h
#define _RB_Random__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_RANDOM) && (RB_CONFIG_USE_RANDOM == RB_CONFIG_YES)

#include "RB_Sysdefs.h"


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


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================


//--------------------------------------------------------------------------------------------------
// RB_RANDOM_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_RANDOM_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_RANDOM_Seed
//--------------------------------------------------------------------------------------------------
//! \brief	Seed (initialize) random sequence
//!
//! Same seeds will produce same sequences (PRNG principle)
//!
//! \param	seed	input	select random sequence; use \ref modules_RB_RTC or \ref RB_Timer to
//!							make this unpredictable
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_RANDOM_Seed(int32_t seed);

//--------------------------------------------------------------------------------------------------
// RB_RANDOM_GetInt
//--------------------------------------------------------------------------------------------------
//! \brief	Get next PRN as integer between 0 and max
//!
//! \param	max	input	maximum PRN value to return
//! \return	PRN
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t RB_RANDOM_GetInt(uint32_t max);

//--------------------------------------------------------------------------------------------------
// RB_RANDOM_GetFloat
//--------------------------------------------------------------------------------------------------
//! \brief	Get next PRN as float32 between 0.0 and 1.0
//!
//! \return	PRN
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC float32 RB_RANDOM_GetFloat(void);

#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_RANDOM
#endif // _RB_Random__h
