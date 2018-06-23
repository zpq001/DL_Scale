//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Date.c
//! \ingroup	util
//! \brief		Functions for date conversions.
//!
//! This module contains the functions for date conversions.
//! The calculation is valid for dates between 1970 and 2099.
//!
//! Note that this module also contains RB_CLOCK_... types and functions. This is due to an
//! architectural change between RB 1.1x and RB 1.2x when 'drv' was split into 'drv' and 'sys'.
//! As a consequence, the RB_Clock module got split into 'util' and 'dev' . Alternatively, a new
//! module RB_DateTime could have been introduced. In order to keep backward compatbility, it was
//! decided to simply move the RB_CLOCK_... types and functions into this module RB_Date.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger / Peter Lutz
//
// $Date: 2016/12/19 15:30:36MEZ $
// $Revision: 1.79 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Date"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Date.h"

#if defined(RB_CONFIG_USE_DATE) && (RB_CONFIG_USE_DATE == RB_CONFIG_YES)

#include <ctype.h>
#include <string.h>
#include "RB_String.h"
#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
	#include "RB_Type.h"
#endif
#if defined(RB_CONFIG_USE_TEXT) && (RB_CONFIG_USE_TEXT == RB_CONFIG_YES)
	#include "RB_Text.h"
#endif

#if !defined(RB_CONFIG_USE_TIME) || (RB_CONFIG_USE_TIME == RB_CONFIG_NO)
	#error RB_CONFIG_USE_TIME must be defined and set to RB_CONFIG_YES in RB_Config.h
#endif


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)

static int GetNumExt(const char ** h);
static const char* GetAMPMString (uint8_t hours);
static uint8_t GetAMPMHours (uint8_t hours);

#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_DATE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_DATE_Initialize(void)
{
}


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_Set
//--------------------------------------------------------------------------------------------------
//! \brief	Set clock struct
//!
//! \param	pClock      Address of struct, where data are set
//! \param	day         Day,    range 1..31
//! \param	month       Month,  range 1..12
//! \param	year        Year,   range 1970..2099
//! \param	hr          Hour,   range 0..23
//! \param	min         Minute, range 0..59
//! \param	sec         Second, range 0..59
//! \param	hundredth   1/100s, range 0..99
//! \return	true, if date is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_CLOCK_Set(RB_CLOCK_tClock* pClock, uint8_t day, uint8_t month, uint16_t year, uint8_t hr, uint8_t min, uint8_t sec, uint8_t hundredth)
{
	pClock->date.day       = day;
	pClock->date.month     = month;
	pClock->date.year      = year;
	pClock->time.hour      = hr;
	pClock->time.minute    = min;
	pClock->time.second    = sec;
	pClock->time.hundredth = hundredth;
	return(RB_CLOCK_IsValid(pClock));
}


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set clock struct to begin of UNIX time, i.e. 1-Jan-1970 00:00:00
//!
//! \param	pClock		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
void RB_CLOCK_SetBeginOfUNIX(RB_CLOCK_tClock* pClock)
{
	pClock->date.day       = 1u;
	pClock->date.month     = 1u;
	pClock->date.year      = 1970u;
	pClock->time.hour      = 0u;
	pClock->time.minute    = 0u;
	pClock->time.second    = 0u;
	pClock->time.hundredth = 0u;
}


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate data and time
//!
//! \param	pClock  Clock value to check
//! \return	true, if date and time are valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_CLOCK_IsValid(const RB_CLOCK_tClock* pClock)
{
	if (RB_DATE_IsValid(&pClock->date))
	{
		if (RB_TIME_IsValid(&pClock->time)) return(true);
	}
	return(false);
}


#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// RB_CLOCK_ToString
//--------------------------------------------------------------------------------------------------
//! \brief	Return values of clock as a string
//!
//! \param	pStr        Returned string
//! \param	pClock      Clock input values
//! \param	pFormat     Date and Time format string
//!                     weekday:    %Wn = n chars of string (without n means all), "MONDAY"
//!                     		    %wn = n chars of string (without n means all), "Monday"
//!                     day:        %d = 1..31, %D = 01..31
//!                     month:      %b = 1..12, %B = 01..12
//!                                 %Nn = n chars of string (without n means all), "APRIL"
//!                                 %nn = n chars of string (without n means all), "April"
//!                     year:       %y = 00..99, %Y = 1970..2099
//!                     hour:       %h = 0..23, %H = 00..23, %r = 1..12, %R = 01..12
//!                                 %T = AM/PM notation
//!                     minute:     %m = 0..59, %M = 00..59
//!                     seconds:    %s = 0..59, %S = 00..59
//!
//! \par Example code:
//! \code
//! RB_CLOCK_ToString(clockStr, &clock, "%W3 %d.%N3 %Y  %H:%M:%S");
//! \endcode
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_CLOCK_ToString(char* pStr, const RB_CLOCK_tClock* pClock, const char* pFormat)
{
	size_t i;
	char* pUC;
	bool upperCase;

    if (pStr == NULL || pClock == NULL)
    {
        return;
    }

    if (pFormat != NULL)
    {
        while (*pFormat)
        {
			upperCase = false;
			pUC = pStr;
            if (*pFormat == '%')
            {   // formatting character
                ++pFormat; // skip %
                switch (*pFormat++)
                {
                    case 'W':
						upperCase = true;
                        // no break
                        //lint -fallthrough
                    case 'w':
                        RB_STRING_strncpymax(pStr, RB_DATE_GetWeekdayString(&pClock->date), (size_t)(1 + GetNumExt(&pFormat)));
						if (upperCase) {
							for (i = 0; i < strlen(pUC); i++)
								pUC[i] = (char)toupper(pUC[i]);
						}
                        break;

                    case 'D':
                        if (pClock->date.day < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'd':
                        RB_FORMAT_ULong(pStr, (uint32_t) pClock->date.day, 3);
                        break;

                    case 'B':
                        if (pClock->date.month < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'b':
                        RB_FORMAT_ULong(pStr, (uint32_t) pClock->date.month, 3);
                        break;

                    case 'N':
						upperCase = true;
                        // no break
                        //lint -fallthrough
                    case 'n':
                        RB_STRING_strncpymax(pStr, RB_DATE_GetMonthString(&pClock->date), (size_t)(1 + GetNumExt(&pFormat)));
						if (upperCase) {
							for (i = 0; i < strlen(pUC); i++)
								pUC[i] = (char)toupper(pUC[i]);
						}
                        break;

                    case 'Y':
                        RB_FORMAT_ULong(pStr, (uint32_t)pClock->date.year, 5);
                        break;

                    case 'y':
                        if ((pClock->date.year % 100) < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        RB_FORMAT_ULong(pStr, (uint32_t)(pClock->date.year % 100), 3);
                        break;

                    case 'T': RB_STRING_strncpymax(pStr, GetAMPMString(pClock->time.hour), 3);
                        break;

                    case 'H':
                        if (pClock->time.hour < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'h':
                        RB_FORMAT_ULong(pStr, (uint32_t)pClock->time.hour, 3);
                        break;

                    case 'R':
                        if (GetAMPMHours(pClock->time.hour) < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'r':
                        RB_FORMAT_ULong(pStr, (uint32_t)GetAMPMHours(pClock->time.hour), 3);
                        break;

                    case 'M':
                        if (pClock->time.minute < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'm':
                        RB_FORMAT_ULong(pStr, (uint32_t)pClock->time.minute, 3);
                        break;

                    case 'S':
                        if (pClock->time.second < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 's':
                        RB_FORMAT_ULong(pStr, (uint32_t)pClock->time.second, 3);
                        break;

                    case '\0':
                        *pStr = '\0';
                        --pFormat;
                        break;

                    default:
                        --pFormat;
                        *pStr++ = *pFormat++;
                        *pStr = '\0';
                        break;
                } // end switch
                // skip to end of string since some above functions do not increment pStr
                while (*pStr)
                {
                    pStr++;
                }
            } // end if (*pFormat == '%')
            else
            {
                *pStr++ = *pFormat++;
            }
        } // end while
    } // end if
    *pStr = '\0';
}
#endif

//--------------------------------------------------------------------------------------------------
// RB_DATE_Set
//--------------------------------------------------------------------------------------------------
//! \brief	Set date struct
//!
//! \param	pDate       Address of struct, where data are set
//! \param	day         Day,    range 1..31
//! \param	month       Month,  range 1..12
//! \param	year        Year,   range 1970..2099
//! \return	true, if date is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_DATE_Set(RB_DATE_tDate* pDate, uint8_t day, uint8_t month, uint16_t year)
{
    pDate->day   = day;
    pDate->month = month;
    pDate->year  = year;
	return(RB_DATE_IsValid(pDate));
}


//--------------------------------------------------------------------------------------------------
// RB_DATE_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set date struct to begin of UNIX time, i.e. 1-Jan-1970
//!
//! \param	pDate		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
void RB_DATE_SetBeginOfUNIX(RB_DATE_tDate* pDate)
{
	pDate->day       = 1u;
	pDate->month     = 1u;
	pDate->year      = 1970u;
}


//--------------------------------------------------------------------------------------------------
// RB_DATE_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate data
//!
//! The calculation is valid for dates between 1970 and 2099
//!
//! \param	pDate   Date value to check
//! \return	true, if date is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_DATE_IsValid(const RB_DATE_tDate* pDate)
{
    bool valid;

    valid = ((pDate->year >= 1970) && (pDate->year <= 2099) && (pDate->day >= 1));

    switch (pDate->month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            valid = (valid && (pDate->day <= 31));
            break;

        case 4:
        case 6:
        case 9:
        case 11:
            valid = (valid && (pDate->day <= 30));
            break;

        case 2:
            valid = (valid && (pDate->day <= (((pDate->year % 4u) == 0u) ? 29u : 28u)));
            break;

        default:
            valid = false;
            break;
    }
    return(valid);
}


#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// RB_DATE_ToString
//--------------------------------------------------------------------------------------------------
//! \brief	Return values of date as formatted string
//!
//! \param	pStr        Returned string, which must be long enough to hold formatted date string!
//! \param	pDate       Date input values
//! \param	pFormat     Format string
//!                     weekday:    %Wn = n chars of string (without n means all), "MONDAY"
//!                     		    %wn = n chars of string (without n means all), "Monday"
//!                     day:        %d = 1..31, %D = 01..31
//!                     month:      %b = 1..12, %B = 01..12
//!                                 %Nn = n chars of string (without n means all), "APRIL"
//!                                 %nn = n chars of string (without n means all), "April"
//!                     year:       %y = 00..99, %Y = 1970..2099
//!
//! \par Example code:
//! \code
//! RB_DATE_ToString(dateStr, &date, "%W3 %d.%N3 %Y");
//! \endcode
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_DATE_ToString(char* pStr, const RB_DATE_tDate* pDate, const char* pFormat)
{
	size_t i;
	char* pUC;
	bool upperCase;

    if (pStr == NULL || pDate == NULL)
    {
        return;
    }
    if (pFormat != NULL)
    {
        while (*pFormat)
        {
			upperCase = false;
			pUC = pStr;
            if (*pFormat == '%')
            {   // formatting character
                ++pFormat; // skip %
                switch (*pFormat++)
                {
                    case 'W':
						upperCase = true;
                        // no break
                        //lint -fallthrough
                    case 'w':
                        RB_STRING_strncpymax(pStr, RB_DATE_GetWeekdayString(pDate), (size_t)(1 + GetNumExt(&pFormat)));
						if (upperCase) {
							for (i = 0; i < strlen(pUC); i++)
								pUC[i] = (char)toupper(pUC[i]);
						}
                        break;

                    case 'D':
                        if (pDate->day < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'd':
                        RB_FORMAT_ULong(pStr, (uint32_t) pDate->day, 3);
                        break;

                    case 'B':
                        if (pDate->month < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        // no break
                        //lint -fallthrough
                    case 'b':
                        RB_FORMAT_ULong(pStr, (uint32_t) pDate->month, 3);
                        break;

                    case 'N':
						upperCase = true;
                        // no break
                        //lint -fallthrough
                    case 'n':
                        RB_STRING_strncpymax(pStr, RB_DATE_GetMonthString(pDate), (size_t)(1 + GetNumExt(&pFormat)));
						if (upperCase) {
							for (i = 0; i < strlen(pUC); i++)
								pUC[i] = (char)toupper(pUC[i]);
						}
                        break;

                    case 'Y':
                        RB_FORMAT_ULong(pStr, (uint32_t)pDate->year, 5);
                        break;

                    case 'y':
                        if ((pDate->year % 100) < 10)
                        {
                            *pStr = '0';
                            ++pStr;
                        }
                        RB_FORMAT_ULong(pStr, (uint32_t)(pDate->year % 100), 3);
                        break;

                    case '\0':
                        *pStr = '\0';
                        --pFormat;
                        break;

                    default:
                        --pFormat;
                        *pStr++ = *pFormat++;
                        *pStr = '\0';
                        break;
                } // end switch
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
#endif


//--------------------------------------------------------------------------------------------------
// RB_DATE_ToDays
//--------------------------------------------------------------------------------------------------
//!
//! \brief	Computes the number of days passed since the the begin of 1970, so called Unix time
//!
//! Converts Gregorian date to days since 1970-01-01.
//! Assumes input in normal date format, i.e. 1980-12-31
//! => year=1980, mon=12, day=31
//!
//! This algorithm was first published by Gauss (I think). This implementation is copied out of
//! linux/arch/i386/kernel/time.c. The original name of the function was mktime().
//!
//! WARNING: This function will overflow on 2106-02-07 06:28:16 on machines were int32_t is 32-bit!
//!
//! \param	pDate  	date to be converted in passed days
//! \return	 passed days since begin of 1970
//--------------------------------------------------------------------------------------------------
uint32_t RB_DATE_ToDays(const RB_DATE_tDate * pDate)
{
	int32_t day   = pDate->day;
	int32_t month = pDate->month;
	int32_t year  = pDate->year;

	if (0L >= (month -= 2L))  // 1..12 -> 11,12,1..10
	{
		month += 12L; // Puts Feb last since it has leap day
		year  -= 1L;
	}
	return (uint32_t)((year/4L - year/100L) + year/400L + 367L*month/12L + day + year*365L) - 719499uL;
}


//--------------------------------------------------------------------------------------------------
// RB_DATE_ToSeconds
//--------------------------------------------------------------------------------------------------
//!
//! \brief	Computes the number of seconds passed since the the begin of 1970, so called Unix time
//!
//! Converts Gregorian date to seconds since 1970-01-01 00:00:00.
//! Assumes input in normal date format, i.e. 1980-12-31 23:59:59
//! => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
//!
//! This algorithm was first published by Gauss (I think). This implementation is copied out of
//! linux/arch/i386/kernel/time.c. The original name of the function was mktime().
//!
//! WARNING: This function will overflow on 2106-02-07 06:28:16 on machines were int32_t is 32-bit!
//!
//! \param	pTimeStamp  	date and time to be converted in passed seconds
//! \return	 Passed seconds since begin of 1970
//--------------------------------------------------------------------------------------------------
uint32_t RB_DATE_ToSeconds(const RB_DATE_tDate * pTimeStamp)
{
	return RB_DATE_ToDays(pTimeStamp) * 86400uL;
}


//--------------------------------------------------------------------------------------------------
// RB_DATE_GetMonthString
//--------------------------------------------------------------------------------------------------
//! \brief	Return month of date as string
//!
//! \param	pDate	Date input values
//!
//! \return	Month string
//!
//! Note: QC# 6369 - the use here of the static char MonthString[4] as return value was
//! introduced due to compiler options in Code Composer Studio Version: 5.5.0.00077 with
//! opt_level=3 (Interprocedure Optimizations) and opt_for_speed=4. This compiler setting did not
//! yield correct compile end results with the previous implementation of this function
//! RB_DATE_GetMonthString. This "compiler malfunction" was detected in the TMS320 configuration of
//! ExampleLightPlus.
//!
//--------------------------------------------------------------------------------------------------
const char* RB_DATE_GetMonthString(const RB_DATE_tDate* pDate)
{
	#if defined(RB_CONFIG_USE_TEXT) && (RB_CONFIG_USE_TEXT == RB_CONFIG_YES)
	return (RB_TEXT_Get((RB_TEXT_tId)((int)TID_January + pDate->month - 1)));
	#else
	static const char monthStrings[] = "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec";
	size_t siz = 4;
	static char MonthString[4];
	RB_STRING_strncpymax(MonthString, &monthStrings[siz * (pDate->month - 1)], siz);
	return (MonthString);
	#endif // RB_CONFIG_USE_TEXT == RB_CONFIG_YES
}


//--------------------------------------------------------------------------------------------------
// RB_DATE_GetWeekdayString
//--------------------------------------------------------------------------------------------------
//! \brief	Return weekday of date as string
//!
//! \param	pDate	Date input values
//!
//! \return	Weekday string
//!
//! Note: QC# 6369 - the use here of the static char WeekdayString[4] as return value was
//! introduced due to compiler options in Code Composer Studio Version: 5.5.0.00077 with
//! opt_level=3 (Interprocedure Optimizations) and opt_for_speed=4. This compiler setting did not
//! yield correct compile end results with the previous implementation of this function
//! RB_DATE_GetWeekdayString. This "compiler malfunction" was detected in the TMS320 configuration of
//! ExampleLightPlus.
//!
//--------------------------------------------------------------------------------------------------
const char* RB_DATE_GetWeekdayString(const RB_DATE_tDate* pDate)
{
	#if defined(RB_CONFIG_USE_TEXT) && (RB_CONFIG_USE_TEXT == RB_CONFIG_YES)
	return (RB_TEXT_Get((RB_TEXT_tId)((int)TID_Monday + RB_DATE_GetWeekday(pDate))));
	#else
	static const char dayStrings[] = "Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0Sun";
	static char WeekdayString[4];
	size_t siz = 4;
	RB_STRING_strncpymax(WeekdayString, &dayStrings[siz * RB_DATE_GetWeekday(pDate)], siz);
	return (WeekdayString);
	#endif // RB_CONFIG_USE_TEXT == RB_CONFIG_YES
}


//--------------------------------------------------------------------------------------------------
//! \brief	Computes the weekday for the given date
//!
//! Computes the weekday of the week for the given date using the Zeller Congruence.
//! It returns a positive value from 0 to 6 for the weekday
//! The Zeller Congruence maps Saturday as weekday 0.  Most applications treat Sunday as weekday 0.
//! The parameter ZELLER_OFFSET is used to convert the weekday of the week from Zeller to local.
//! In the Rainbow project the value 0 corresponds to Monday
//!
//! Offset from Zeller to local
#define ZELLER_OFFSET   (5L)
//!
//! \param	pDate      Pointer to date to be evaluated
//! \return	weekday as number. [0 for Monday, ......, 6 for Sunday]
//--------------------------------------------------------------------------------------------------
uint8_t RB_DATE_GetWeekday(const RB_DATE_tDate* pDate)
{
    int32_t   	yr ;
   	int32_t     mn ;
	int32_t		k;
	int32_t		j;
	int32_t 	dayofweek;
	int32_t		d;


    yr = pDate->year;
    mn = pDate->month;
	d = pDate->day;

    // January or February?
    // --------------------
    if (mn < 3L)
    {
        // Yes, make these part of last year
        // ---------------------------------
        mn += 12L ;
        yr -= 1L ;
    }
	k = yr % 100L;
	j = yr / 100L;

	// Saturday = 0, Friday = 6
	dayofweek = (int32_t)(((d + (((mn + 1L) * 26L) / 10L) + k + (k / 4L) + (j / 4L)) - (2L * j)) % 7L);

	// Monday = 0, Sunday = 6
	return (uint8_t)(dayofweek + ZELLER_OFFSET) % 7;




}

//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// GetNumExt
//--------------------------------------------------------------------------------------------------
//! \brief	If possible returns an ascii character converted to an integer else 20
//!
//! \param	char        pointer to an ascii character string
//!
//! \return	integer
//--------------------------------------------------------------------------------------------------
static int GetNumExt(const char ** h)
{
    int retval = 20;
    char ch = **h;
    if (isdigit((int)ch))
    {
        (*h)++;
        retval = ch - '0';
    }
    return(retval);
}

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


//--------------------------------------------------------------------------------------------------
// RB_DATE_IsDateEqual
//--------------------------------------------------------------------------------------------------
//! \brief	Check two dates for equality
//!
//!	\param date1	date to be checked for equality
//!	\param date2	date to be checked for equality
//!
//! \return true 	if dates equal
//!			false 	otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_DATE_IsDateEqual(RB_DATE_tDate date1, RB_DATE_tDate date2)
{
	bool equal = false;

	if(date1.year == date2.year &&
	   date1.month == date2.month &&
	   date1.day == date2.day)
	{
		equal = true;
	}

	return equal;
}


#endif // RB_CONFIG_USE_TYPE
#endif // RB_CONFIG_USE_DATE
