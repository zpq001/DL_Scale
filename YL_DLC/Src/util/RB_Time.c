//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Time.c
//! \ingroup	util
//! \brief		Functions for time conversions.
//!
//! This module contains the functions for time conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger / Peter Lutz
//
// $Date: 2017/02/02 07:46:05MEZ $
// $Revision: 1.59 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Time"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Time.h"

#if defined(RB_CONFIG_USE_TIME) && (RB_CONFIG_USE_TIME == RB_CONFIG_YES)

#include <string.h>
#include "RB_String.h"
#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
	#include "RB_Type.h"
#endif


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)

static const char* GetAMPMString (uint8_t hours);
static uint8_t GetAMPMHours (uint8_t hours);
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TIME_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_TIME_Initialize(void)
{
}


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
bool RB_TIME_Set(RB_TIME_tTime* pTime, uint8_t hr, uint8_t min, uint8_t sec, uint8_t hundredth)
{
    pTime->hour      = hr;
    pTime->minute    = min;
    pTime->second    = sec;
    pTime->hundredth = hundredth;
	return(RB_TIME_IsValid(pTime));
}


//--------------------------------------------------------------------------------------------------
// RB_TIME_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set time struct to begin of UNIX time, i.e. 00:00:00
//!
//! \param	pTime		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
void RB_TIME_SetBeginOfUNIX(RB_TIME_tTime* pTime)
{
	pTime->hour      = 0u;
	pTime->minute    = 0u;
	pTime->second    = 0u;
	pTime->hundredth = 0u;
}


//--------------------------------------------------------------------------------------------------
// RB_TIME_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate time
//!
//! \param	pTime   Time value to check
//! \return	true, if time is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_TIME_IsValid(const RB_TIME_tTime* pTime)
{
    // Check for proper range
    return ((pTime->hour       <= 23)
         && (pTime->minute     <= 59)
         && (pTime->second     <= 59)
         && (pTime->hundredth  <= 99)
        );
}


#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
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
void RB_TIME_ToString(char* pStr, const RB_TIME_tTime* pTime, const char* pFormat)
{
    if (pStr == NULL || pTime == NULL)
    {
        return;
    }
    if (pFormat != NULL)
    {
        while (*pFormat)
        {
            if (*pFormat == '%')
            {   // formatting character
                ++pFormat; // skip %
                switch (*pFormat)
                {
                    case 'T': RB_STRING_strncpymax(pStr, GetAMPMString(pTime->hour), 3);
                        break;

                    case 'H':
                        if (pTime->hour < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'h':
                        RB_FORMAT_ULong(pStr, (uint32_t)pTime->hour, 3);
                        break;

                    case 'R':
                        if (GetAMPMHours(pTime->hour) < 10)
                        {
                            RB_STRING_strncpymax(pStr, "0", 100);
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'r':
                        RB_FORMAT_ULong(pStr, (uint32_t)GetAMPMHours(pTime->hour), 3);
                        break;

                    case 'M':
                        if (pTime->minute < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'm':
                        RB_FORMAT_ULong(pStr, (uint32_t)pTime->minute, 3);
                        break;

                    case 'S':
                        if (pTime->second < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 's':
                        RB_FORMAT_ULong(pStr, (uint32_t)pTime->second, 3);
                        break;

                    case 0  :
                        *pStr = '\0';
                        --pFormat;
                        break;

                    default:
                        --pFormat;
                        *pStr++ = *pFormat++;
                        *pStr = '\0';
                        break;
                } // end switch
                ++pFormat;
                while (*pStr)
                {
                    pStr++;
                }
            } // end if (*pFormat == '%')
            else
            {
                *pStr++ = *pFormat++;
            }
        }
    }
    *pStr = '\0';
}
#endif // RB_CONFIG_USE_TYPE


//--------------------------------------------------------------------------------------------------
// RB_TIME_ToSeconds
//--------------------------------------------------------------------------------------------------
//!
//! \brief	Computes the number of seconds
//!
//! \param	pTime		Time to be converted in seconds
//! \return	Seconds
//--------------------------------------------------------------------------------------------------
uint32_t RB_TIME_ToSeconds(const RB_TIME_tTime* pTime)
{
	return((uint32_t)pTime->hour * 3600uL + (uint32_t)pTime->minute * 60uL + (uint32_t)pTime->second);
}



//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================


#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// GetAMPMString
//--------------------------------------------------------------------------------------------------
//! \brief	Returns a string with "AM" if hours < 12 else with "PM"
//!
//! \param	hours       hours to be checked
//!
//! \return	integer
//--------------------------------------------------------------------------------------------------
static const char* GetAMPMString (uint8_t hours)
{
    if (hours < 12)
    {
        return("AM");   // 00..11 --> AM
    }
    return("PM");       // 12..23 --> PM
}

//--------------------------------------------------------------------------------------------------
// GetAMPMHours
//--------------------------------------------------------------------------------------------------
//! \brief	Returns converted 24h format hours to AM/PM format hours
//!
//! \param	hours       hours to be checked
//!
//! \return	integer
//--------------------------------------------------------------------------------------------------
static uint8_t GetAMPMHours (uint8_t hours)
{
    if (hours < 1)
    {
        return(12);
    }
    if (hours < 13)
    {
        return(hours);
    }
    return((uint8_t)(hours-12));
}
#endif // RB_CONFIG_USE_TYPE


#endif // RB_CONFIG_USE_TIME
