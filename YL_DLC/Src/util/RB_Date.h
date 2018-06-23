//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Date.h
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
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.46 $
//
//==================================================================================================

#ifndef _RB_Date__h
#define _RB_Date__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_DATE) && (RB_CONFIG_USE_DATE == RB_CONFIG_YES)

#include "RB_Typedefs.h"
#include "RB_Time.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Date type
typedef struct {
	uint8_t  day;
	uint8_t  month;
	uint16_t year;
} RB_DECL_TYPE RB_DATE_tDate;

//! Clock type, i.e. timestamp
typedef struct {
	RB_DATE_tDate date;
	RB_TIME_tTime time;
} RB_DECL_TYPE RB_CLOCK_tClock;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_DATE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DATE_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_Set
//--------------------------------------------------------------------------------------------------
//! \brief	Set clock struct
//!
//! \param	pClock		Address of struct, where data are set
//! \param	day			Day, 	range 1..31
//! \param	month		Month, 	range 1..12
//! \param	year		Year, 	range 1970..2099
//! \param	hr			Hour, 	range 0..23
//! \param	min			Minute,	range 0..59
//! \param	sec			Second,	range 0..59
//! \param	hundredth	1/100s,	range 0..99
//! \return	true, if date is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_CLOCK_Set(RB_CLOCK_tClock* pClock, uint8_t day, uint8_t month, uint16_t year, uint8_t hr, uint8_t min, uint8_t sec, uint8_t hundredth);


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set clock struct to begin of UNIX time, i.e. 1-Jan-1970 00:00:00
//!
//! \param	pClock		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_CLOCK_SetBeginOfUNIX(RB_CLOCK_tClock* pClock);


//--------------------------------------------------------------------------------------------------
// RB_CLOCK_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate data and time
//!
//! \param	pClock	Clock value to check
//! \return	true, if date and time are valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_CLOCK_IsValid(const RB_CLOCK_tClock* pClock);


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
RB_DECL_FUNC void RB_CLOCK_ToString(char* pStr, const RB_CLOCK_tClock* pClock, const char* pFormat);

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
RB_DECL_FUNC bool RB_DATE_Set(RB_DATE_tDate* pDate, uint8_t day, uint8_t month, uint16_t year);


//--------------------------------------------------------------------------------------------------
// RB_DATE_SetBeginOfUNIX
//--------------------------------------------------------------------------------------------------
//! \brief	Set date struct to begin of UNIX time, i.e. 1-Jan-1970
//!
//! \param	pDate		Address of struct, where data are set
//! \return
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_DATE_SetBeginOfUNIX(RB_DATE_tDate* pDate);


//--------------------------------------------------------------------------------------------------
// RB_DATE_IsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Validate data
//!
//! \param	pDate   Date value to check
//! \return	true, if date is valid, i.e. in range, false otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_DATE_IsValid(const RB_DATE_tDate* pDate);


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
RB_DECL_FUNC void RB_DATE_ToString(char* pStr, const RB_DATE_tDate* pDate, const char* pFormat);


//--------------------------------------------------------------------------------------------------
// RB_DATE_GetWeekdayString
//--------------------------------------------------------------------------------------------------
//! \brief	Return weekday of date as string
//!
//! \param	pDate       Date input values
//!
//! \return	Weekday string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* RB_DATE_GetWeekdayString(const RB_DATE_tDate* pDate);


//--------------------------------------------------------------------------------------------------
// RB_DATE_GetMonthString
//--------------------------------------------------------------------------------------------------
//! \brief	Return month of date as string
//!
//! \param	pDate       Date input values
//!
//! \return	Month string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* RB_DATE_GetMonthString(const RB_DATE_tDate* pDate);


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
//! \param	pDate  date to be converted in passed days
//! \return	 passed days since begin of 1970
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t RB_DATE_ToDays(const RB_DATE_tDate * pDate);


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
RB_DECL_FUNC uint32_t RB_DATE_ToSeconds(const RB_DATE_tDate * pTimeStamp);

//--------------------------------------------------------------------------------------------------
//! \brief	Computes the weekday for the given date
//!
//! Computes the weekday of the week for the given date using the Zeller Congruence.
//! It returns a positive value from 0 (Monday) to 6 (Sunday) for the weekday
//!
//! \param	pDate      Pointer to date to be evaluated
//! \return	weekday as number. [0 for Monday, ......, 6 for Sunday]
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint8_t RB_DATE_GetWeekday(const RB_DATE_tDate* pDate);


//--------------------------------------------------------------------------------------------------
// RB_DATE_IsDateEqual
//--------------------------------------------------------------------------------------------------
//! \brief	Check two dates for equality
//
//! \return true 	if dates equal
//!			false 	otherwise
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_DATE_IsDateEqual(RB_DATE_tDate date1, RB_DATE_tDate date2);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_DATE
#endif // _RB_Date__h
