//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Format.h
//! \ingroup	util
//! \brief		Convert a variable of simple type to a string output.
//!
//! This module contains functions to write a simple type to a string with predefined length.
//! If the string length is insufficient, an empty string is returned with returnvalue false.
//!
//!
//! MT-SICS Floating Point Format Representation (from MT-SICS manual):
//!
//! > MT-SICS uses two formats: Normal format (e.g. 1530.12) and scientific format (e.g. 1.53012E+03).
//!
//! > Normal floating point and scientific format can be used as input of all floating point parameters
//!
//! > Normal format outputs as many decimal places as needed but always at least one decimal, e.g.
//!   0.1 / 10.0 / 12.34
//!   The value zero is always output in normal format, i.e. 0.0.
//!
//! > In case of small numbers, normal format automatically changes to scientific format as soon as
//!   values are smaller than +/-0.0001, e.g. 0.001 / 0.0001 / 9E-05 / 1E-005
//!
//! > In case of large numbers, normal format automatically changes to scientific format as soon as
//!   values are larger than +/-999999.9, e.g. 100000.0 / 999999.9 / 1E+006
//!
//! > Scientific format outputs a fixed number of 9 or 17 decimal places (according to IEEE754), e.g.
//!   float32 = 9 decimals: 6.000000000E+03
//!   float64 = 17 decimals: 5.38635290000000000E-010
//!
//! > Scientific format shall output a fixed number of 2 or 3 exponent digits (according to IEEE754), e.g.
//!   float32 = 2 exponent digits: 6.000000000E+03
//!   float64 = 3 exponent digits: 5.38635290000000000E-010
//!   If the number of exponent digits cannot be chosen (e.g. when using printf()), the number may be
//!   compiler/system dependent
//!
//! > MT-SICS commands output normal format
//!
//! > MT-SICS-XP01xx/02xx commands output scientific format
//!
//! > MT-SICS-XP/XM/XA/XU output normal format
//!
//! > For all commands, values can be input be either normal format or scientific format
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/12/08 10:47:43MEZ $
// $Revision: 1.35 $
//
//==================================================================================================

#ifndef _RB_Format__h
#define _RB_Format__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

//#include "RB_Typedefs.h"

#include "comm.h"
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
// RB_FORMAT_Long
//--------------------------------------------------------------------------------------------------
//! \brief	Convert int32_t value to string.
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	Length of string, 0 and empty string if output string is too short
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_Long(char* pOutput, int32_t value, size_t bufLen);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_ULong
//--------------------------------------------------------------------------------------------------
//! \brief	Convert uint32_t value to string.
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	Length of string, 0 and empty string if output string is too short
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_ULong(char* pOutput, uint32_t value, size_t bufLen);

//--------------------------------------------------------------------------------------------------
// RB_FORMAT_Hex
//--------------------------------------------------------------------------------------------------
//! \brief	Convert uint32_t value to hex string.
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \param	digits		Number of hex digits, range 1..8
//! \param	leading_0x	Output a leading 0x in front of string
//! \return	Length of string, 0 and empty string if output string is too short
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_Hex(char* pOutput, uint32_t value, size_t bufLen, size_t digits, bool leading_0x);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_Double
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 value to string with decimal places. Value is arithmetically rounded
//!			according to decimal places.
//!
//!	Attention!	Starting with Rainbow version 1.9.7-3 the meaning of decimal places changed:
//!				Old: dp  0	-->	12345.				New: decPlaces  0	--> 12345
//!				Old: dp -1	-->	12345				New: decPlaces <0	--> 12345.
//!
//! \param	pOutput     Output string  buffer
//! \param	decPlaces	Decimal places, range 0..15 (0 = no decimal places)
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= "\0")
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_Double(char* pOutput, int decPlaces, float64 value, size_t bufLen);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_DoubleToEFormat
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 value to string with E format NOT in SICS Format
//!			Value will be displayed and rounded according to decPlaces:
//!
//!			Example: value = 1.06
//!			decPlaces -1	--> 1.E0
//!			decPlaces  0	--> 1E0
//!			decPlaces  1	--> 1.1E0
//!			decPlaces  2	--> 1.06E0
//!
//!			Example: value = 10000.9
//!			decPlaces -2    --> 1.00E4
//!
//!
//! \param	pOutput		Output string buffer
//! \param	dp			Decimal places, range 0..15 (-1 = no decimal place)
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= "\0")
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_DoubleToEFormat(char* pOutput, int dp, float64 value, size_t bufLen);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_DoubleToGFormatSICS
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 input value into "normal format" string or into "scientific format" string
//!			according to MT-SICS specification. In case of normal format the overall number of digits
//!			output in the string will follow the value of digits.
//!			In case of E format, we output 1 place befor comma and 9/17 digits after comma.
//!			if digits <= 9 9 places after comma will be printed out, otherwise 17 places
//!			Due to float precision - 17 significant digits guaranteed (which makes 16 dp after comma
//!			- and some lost of precision during processing of the values, last 2-3 digits may differ
//!			from ideal values.
//!
//!			Small values -> normal format, digits is overall number of significant digits
//!			(before and after comma!)
//!			We always output at least one place
//!			Negative digits does not make sense in this context. In case of negative digits,
//!			Number with no places after comma will be printed out
//!
//!			In case of normal format we always output at least all digits before and one decimal place
//!			after comma, even in digits is smaller than number digits befor comma
//!
//!			Example: value = 1.06
//!			digits  -1   --> 1.1
//!			digits   0   --> 1.1
//!			digits   1   --> 1.1
//!			digits   2   --> 1.1
//!			digits   >2  --> 1.06
//!
//!			Example: value = 12.3456
//!			digits   0   --> 12.3
//!			digits   1   --> 12.3
//!			digits   2   --> 12.3
//!			digits   3   --> 12.3
//!			digits   4   --> 12.34
//!			digits   5   --> 12.346
//!			digits   6   --> 12.3456
//!
//!
//!			Values < 0.0001 or > 1000000  --> SICK E Format
//!
//!			Example: value = 1000000.9
//!			digits 2    --> 1.000000900E+06
//!			digits 10   --> 1.00000090000000000E+06
//!
//! \param	ppOutput	Output string buffer pointer, points to end of string after return, i.e. '\\0'
//! \param	value		Input value
//! \param	digits		For scientific format digits indicate nr of decimnal places after comma
//!						the sum of significant places will be digits+1
//!						for normal format digits is the number of all significant places
//!						e.g. digits = 6, value = 12.345678 -> 12.3456
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_FORMAT_DoubleToGFormatSICS(char** ppOutput, float64 value, int16_t digits);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_Float32ToEFormatSICS
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float32 input value into "scientific format" string according to
//!			MT-SICS specification. Value is arithmetically rounded according to precision (9 decimal places)
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_Float32ToEFormatSICS(char* pOutput, float32 value, size_t bufLen);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_Float64ToEFormatSICS
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 input value into "scientific format" string according to
//!			MT-SICS specification. Value is arithmetically rounded according to precision (9 decimal places)
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_Float64ToEFormatSICS(char* pOutput, float64 value, size_t bufLen);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_MacAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Write MAC address in hex to string.
//!
//! \param	pOutput	The string containing a MAC address in this format: e.g. "00:0a:0b:0c:0d:0e"
//! \param	macAddr	Buffer where the address is stored.
//! \return	Length of string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_MacAddress(char* pOutput, const uint8_t* macAddr);


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_IpAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Write IP address to string.
//!
//! \param	pOutput		The string containing a IP address in this format: e.g. "172.24.48.1"
//! \param	pIpAddr		IP address as uint32_t value (containing IP address in network byte order = big endian)
//! \param	zeroFill	Fill zeroes, e.g. "172.024.048.001"
//! \return	Length of string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_FORMAT_IpAddress(char* pOutput, const uint32_t* pIpAddr, bool zeroFill);


#ifdef __cplusplus
}
#endif

#endif // _RB_Format__h
