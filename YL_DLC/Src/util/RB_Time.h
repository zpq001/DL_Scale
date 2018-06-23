//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Time.h
//! \ingroup	util
//! \brief		Functions for time conversions.
//!
//! This module contains the functions for time conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger / Peter Lutz
//
// $Date: 2016/11/10 08:58:41MEZ $
// $Revision: 1.40 $
//
//==================================================================================================

#ifndef _RB_Time__h
#define _RB_Time__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_TIME) && (RB_CONFIG_USE_TIME == RB_CONFIG_YES)

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

//! Time type
typedef struct {
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
    uint8_t  hundredth;
} RB_DECL_TYPE RB_TIME_tTime;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TIME_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TIME_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_TIME_Set
//--------------------------------------------------------------------------------------------------
//! \brief	Set time struct
//!
//! \param	pTime       Address of struct, where data are set
//! \param	hr          Hour,   range 0..23
//! \param	min         Minute, range 0..59
//! \param	sec         Second, range 0..59
//! \param	hundredth   1/100s, range 0..99
//! \return	true, if time is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_TIME_Set(RB_TIME_tTime* pTime, uint8_t hr, uint8_t min, uint8_t sec, uint8_t hundredth);


//--------------------------------------------------------------------------------------------------
// RB_TIME_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set time struct to begin of UNIX time, i.e. 00:00:00
//!
//! \param	pTime		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TIME_SetBeginOfUNIX(RB_TIME_tTime* pTime);


//--------------------------------------------------------------------------------------------------
// RB_TIME_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate time
//!
//! \param	pTime       Time value to check
//! \return	true, if time is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_TIME_IsValid(const RB_TIME_tTime* pTime);


//--------------------------------------------------------------------------------------------------
// RB_TIME_ToString
//--------------------------------------------------------------------------------------------------
//! \brief	Return values of time as formatted string
//!
//! \param	pStr        Returned string, which must be long enough to hold formatted time string!
//! \param	pTime       Time input values
//! \param	pFormat     Format string:
//!                     hour:       %h = 0..23, %H = 00..23, %r = 1..12, %R = 01..12
//!                                 %T = AM/PM notation
//!                     minute:     %m = 0..59, %M = 00..59
//!                     seconds:    %s = 0..59, %S = 00..59
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TIME_ToString(char* pStr, const RB_TIME_tTime* pTime, const char* pFormat);


//--------------------------------------------------------------------------------------------------
// RB_TIME_ToSeconds
//--------------------------------------------------------------------------------------------------
//!
//! \brief	Computes the number of seconds
//!
//! \param	pTime		Time to be converted in seconds
//! \return	Seconds
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t RB_TIME_ToSeconds(const RB_TIME_tTime* pTime);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_TIME
#endif // _RB_Time__h
