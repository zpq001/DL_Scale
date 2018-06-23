//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Format.c
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
// $Date: 2016/12/20 11:03:24MEZ $
// $Revision: 1.65 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Format"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Format.h"
// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Math.h"

#include "RB_String.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

//==================================================================================================
//  L O C A L   C O N S T A N T S
//==================================================================================================

//! Power of 10 table
static const int32_t POWER_OF_10[] = {
	1L,                 // 0
	10L,                // 1
	100L,               // 2
	1000L,              // 3
	10000L,             // 4
	100000L,            // 5
	1000000L,           // 6
	10000000L,          // 7
	100000000L,         // 8
	1000000000L         // 9
};


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static size_t DoubleToEFormat(char* pOutput, int dp, float64 value, size_t bufLen, bool sicsFormat);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_FORMAT_Long
//--------------------------------------------------------------------------------------------------
//! \brief	Convert int32_t value to string.
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output string is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_Long(char* pOutput, int32_t value, size_t bufLen)
{
	int i;
	size_t j = 0;
	char digit;
	int blanking = 1;
	const char maxNegLong[] = "2147483648";

	// bufLen must be greater than 0
	if (bufLen == 0) {
		return 0;			// Buffer length insufficient
	}

	// Process sign
	if (value < 0L) {
		value = -value;
		pOutput[j++] = '-';
	}

	for (i = 9; i >= 0; i--) {
		// String length may not be greater then bufLen
		if (j == bufLen - 1) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Handle special case: -2147483648 = -2^31 = 0x8000'0000
		// This value cannot be converted to positive value
		if (value < 0L) {
			pOutput[j] = maxNegLong[j-1];
			j++;
		}
		else {
			// Write digit value
			digit = (char)((value / POWER_OF_10[i]) % 10L);
			if (digit > '\0' || i == 0 || blanking == 0) {
				pOutput[j++] = (char)(digit + '0');
				blanking = 0;
			}
		}
	}

	// Append trailing zero
	pOutput[j] = '\0';

	// Return length of string
	return j;
}


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_ULong
//--------------------------------------------------------------------------------------------------
//! \brief	Convert uint32_t value to string.
//!
//! \param	pOutput		Output string buffer
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_ULong(char* pOutput, uint32_t value, size_t bufLen)
{
	int i;
	size_t j = 0;
	char digit;
	int blanking = 1;

	// bufLen must be greater than 0
	if (bufLen == 0) {
		return 0;			// Buffer length insufficient
	}

	for (i = 9; i >=0; i--) {
		// String length may not be greater then bufLen
		if (j == bufLen - 1) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Write digit value
		digit = (char)((value / (uint32_t)POWER_OF_10[i]) % 10UL);
		if (digit > '\0' || i == 0 || blanking == 0) {
			pOutput[j++] = (char)(digit + '0');
			blanking = 0;
		}
	}

	// Append trailing zero
	pOutput[j] = '\0';

	// Return length of string
	return j;
}


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
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_Hex(char* pOutput, uint32_t value, size_t bufLen, size_t digits, bool leading_0x)
{
	size_t i;
	size_t j = 0;
	size_t len;
	const char hexdigits[] = "0123456789ABCDEF";

	// bufLen must be greater than 0
	if (bufLen == 0) {
		return 0;			// Buffer length insufficient
	}

	// Restrict digits to 1..8
	if (digits == 0) {
		digits = 1;
	}
	if (digits > 8) {
		digits = 8;
	}

	// Check for proper string length
	len = digits;
	if (leading_0x)	{
		len += 2u;
	}
	if (len >= bufLen) {
		pOutput[0] = '\0';	// Return empty string
		return 0;			// buffer length insufficient
	}

	// Output leading 0x
	if (leading_0x)	{
		pOutput[j++] = '0';
		pOutput[j++] = 'x';
	}

	// Output trailing zero
	j = len;
	pOutput[j--] = '\0';

	// Output hex nibbles
	for (i = 0; i < digits; i++) {
		// Write hex nibble
		pOutput[j--] = hexdigits[value % 16uL];
		value = value / 16uL;
	}

	// Return length of string
	return len;
}


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
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_Double(char* pOutput, int decPlaces, float64 value, size_t bufLen)
{
	float64 roundingValue = (float64)0.5;
	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	int dp;
	int roundingPos;
	int digit = 0;
	int i = 0;
	size_t j = 0;		// String index
	int k = 0;
	int count = 0;		// Digit count
	bool blanking = false;
	bool isNegative = false;
	bool isStrValZero = true;	// true if string value is all zeroes, i.e. 0

	// bufLen must be greater than 0
	if (bufLen == 0) {
		return 0;			// Buffer length insufficient
	}

	// Start changed with RB 1.9.7-3
	if (decPlaces < 0) {
		dp = 0;
	}
	else if (decPlaces == 0) {
		dp = -1;
	}
	else {
		dp = decPlaces;
	}
	// End changed with RB 1.9.7-3

	roundingPos = dp;

	if (!FLT64_isfinite(valueTemp)) {
		const char *text;
		// value is one of +/-NaN, +/-Inf

		isNegative = signbit(valueTemp); // sign of value
		if (isnan(valueTemp)) {
			text = (isNegative) ? "-NaN" : "NaN";
		}
		else {
			text = (isNegative) ? "-INF" : "INF";
		}

		if (bufLen >= (strlen(text) + 1)) {
			RB_STRING_strncpymax(pOutput, text, bufLen);
			return strlen(pOutput);
		}
		else {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}
	}

	// Process sign
	if (valueTemp < (float64)0.0L) {
		valueTemp = -valueTemp;
		pOutput[j++] = '-';
		isNegative = true;
	}

	// Round value
	if (dp < 0) {
		roundingPos = 0;
	}
	for (i = roundingPos; i > 0; i--) {
		roundingValue *= (float64)0.1L;
	}
	valueTemp += roundingValue;

	// Limit value to 9.999...1.000
	while (valueTemp >= (float64)10.0L) {
		valueTemp = valueTemp * (float64)0.1L;
		k++;
	}

	// Set blanking of leading zeroes
	blanking = (k > 0);

	// Digit count
	if (dp >= 0) {
		count = dp + k + 1;
	}
	else {
		count = k + 1;
	}
	// Compose output string
	for (i = (count - 1); i >= 0; i--) {

		// String length may not be greater then bufLen - 1
		if (j >= bufLen - 1) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Output digit
		digit = (int)valueTemp % 10;
		if (digit > 0 || i <= dp || !blanking) {
			pOutput[j++] = (char)(digit + '0');
			blanking = false;
		}

		// Check if string is all zeroes, for negative zero test, i.e. "-0.0000"
		if (digit != 0)	{
			isStrValZero = false;
		}

		// String length may not be greater then bufLen
		if (j == bufLen) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Output decimal point
		if (i == dp) {
			pOutput[j++] = '.';
		}
		valueTemp = (valueTemp - (float64)digit) * (float64)10.0L;
	}

	// Append trailing zero
	pOutput[j] = '\0';

	// Handle case, where output is "-0.0000", correct to "0.0000"
	if (isStrValZero && isNegative) {
		i = 0;
		// Remove minus, move string one position down
		while (pOutput[i]) {
			pOutput[i] = pOutput[i+1];
			i++;
		}
		// Adjust string length
		j--;
	}

	// Return length of string
	return j;
}


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
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_DoubleToEFormat(char* pOutput, int dp, float64 value, size_t bufLen)
{
	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	return DoubleToEFormat(pOutput, dp, valueTemp, bufLen, false);
}


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_DoubleToGFormatSICS
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 input value into "normal format" string or into "scientific format" string
//!			according to MT-SICS specification. In case of normal format the overall number of digits
//!			output in the string will follow the value of digits.
//!			In case of E format, we output 1 place before comma and 9/17 digits after comma.
//!			if digits <= 9 9 places after comma will be printed out, otherwise 17 places
//!			Due to float precision - 17 significant digits guaranteed (which makes 16 dp after comma)
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
//!			after comma, even in digits is smaller than number digits before comma
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
//!			Values < 0.0001 or > 1000000  --> SICS E Format
//!
//!			Example: value = 1000000.9
//!			digits 2    --> 1.000000900E+06
//!			digits 10   --> 1.00000090000000000E+06
//!
//! \param	ppOutput	Output string buffer pointer, points to end of string after return, i.e. '\0'
//! \param	value		Input value
//! \param	digits		For scientific format digits indicate nr of decimal places after comma
//!						the sum of significant places will be digits+1
//!						for normal format digits is the number of all significant places
//!						e.g. digits = 6, value = 12.345678 -> 12.3456
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
//lint -e818 Pointer could be declared as pointing to const
void RB_FORMAT_DoubleToGFormatSICS(char** ppOutput, float64 value, int16_t digits)
{
	char valStr[32];
	char* pValStr;
	int16_t expo = 0;
	int dp;

	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	float64 valueAbs = fabs(valueTemp);

	// Use E-Format, if very big or very small value and not zero
	// Use E-Format float32 with 9 digits precision
	if ((valueTemp != (float64)0.0L) &&
		(digits < 10) &&
		((valueAbs >= 1E6L) || (valueAbs < 0.000099995L)))
	{
		RB_FORMAT_Float32ToEFormatSICS(valStr, (float32)valueTemp, sizeof(valStr));
	}
	// Use E-Format float64 with 17 digits precision
	else if ((valueTemp != (float64)0.0L) &&
			 (digits >= 10) &&
			((valueAbs >= 1E6L) || (valueAbs < 0.00009999999999995L)))
	{
		RB_FORMAT_Float64ToEFormatSICS(valStr, valueTemp, sizeof(valStr));
	}

	// Use normal format in other cases
	else {

		expo = (int16_t)RB_MATH_ilog10(valueAbs);
		// The number of decimal places needs to be (overall nr digits) - (nr digits before comma)
		// We want to output at least 1 decimal place
		if (expo >= 0) {
			dp =  digits - ((expo) + 1);
		}
		else {
			dp = digits - 1;			// only one digit before comma (0.)
		}

		if(dp < 1) {
			dp = 1;
		}

		RB_FORMAT_Double(valStr, dp, valueTemp, sizeof(valStr));

		pValStr = &valStr[0] + strlen(valStr) - 1;
		// remove trailing zeros
		while ((*pValStr == '0') && (*(pValStr-1) != '.')) {
			*pValStr-- = '\0';
		}
	}

	// Copy string to output
	RB_STRING_strncpymax(*ppOutput, valStr, sizeof(valStr));
}
//lint +e818 Pointer could be declared as pointing to const


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
size_t RB_FORMAT_Float32ToEFormatSICS(char* pOutput, float32 value, size_t bufLen)
{
	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	return DoubleToEFormat(pOutput, 9, valueTemp, bufLen, true);
}


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
size_t RB_FORMAT_Float64ToEFormatSICS(char* pOutput, float64 value, size_t bufLen)
{
	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	return DoubleToEFormat(pOutput, 17, valueTemp, bufLen, true);
}


//--------------------------------------------------------------------------------------------------
// RB_FORMAT_MacAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Write MAC address in hex to string.
//!
//! \param	pOutput	The string containing a MAC address in this format: e.g. "00:0a:0b:0c:0d:0e"
//! \param	macAddr	Buffer where the address is stored.
//! \return	Length of string
//--------------------------------------------------------------------------------------------------
size_t RB_FORMAT_MacAddress(char* pOutput, const uint8_t* macAddr)
{
	int i;
	size_t len = 0;

	for (i = 0; i < 6; i++) {
		if (!RB_FORMAT_Hex(&pOutput[len], (uint32_t)macAddr[i], (size_t)3, (size_t)2, false)) {
			return 0;
		}
		len = len + 2;

		// Skip last colon
		if (i < 5) {
			pOutput[len++] = ':';
		}
	}
	return len;
}


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
size_t RB_FORMAT_IpAddress(char* pOutput, const uint32_t* pIpAddr, bool zeroFill)
{
	char ipStr[16] = "";
	int i;
	size_t len = 0;

	for (i = 3; i >= 0; i--) {
		len += RB_FORMAT_ULong(&ipStr[len], *pIpAddr >> (i * 8) & 255, (size_t)4);

		// Fill zeroes, if requested
		if (zeroFill) {
			RB_STRING_AlignRight(&ipStr[(3 - i) * 4], 3u);
			if (ipStr[(3 - i) * 4 + 0] == ' ') ipStr[(3 - i) * 4 + 0] = '0';
			if (ipStr[(3 - i) * 4 + 1] == ' ') ipStr[(3 - i) * 4 + 1] = '0';
		}
		len = strlen(ipStr);

		// Skip last dot
		if (i > 0) {
			ipStr[len++] = '.';
		}
	}
	RB_STRING_strncpymax(pOutput, ipStr, sizeof(ipStr));
	return strlen(ipStr);
}



//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// DoubleToEFormat
//--------------------------------------------------------------------------------------------------
//! \brief	Convert float64 value to string with E format. Value is arithmetically rounded
//!			according to dp. According to sicsFormat parameter SICS format can be displayed
//!
//!			No SICS Format:
//!
//!			Example: value = 10.6
//!			decPlaces -1	--> 1.E1
//!			decPlaces  0	--> 1E1
//!			decPlaces  1	--> 1.1E1
//!			decPlaces  2	--> 1.06E1
//!
//!			SICS Format:
//!
//!			Example: value = 10.6
//!			decPlaces -1	--> 1.E+01
//!			decPlaces  0	--> 1E+01
//!			decPlaces  1	--> 1.1E+01
//!			decPlaces  2	--> 1.06E+01
//!
//! \param	pOutput		Output string buffer
//! \param	dp			Decimal places, range 0..17 (-1 = no decimal place)
//! \param	value		Input value
//! \param	bufLen		Buffer length of argument 'pOutput'
//! \param	sicsFormat	SICS format, if true
//! \return	In case of successful conversion:	return length of string, pOutput will be filled
//!			In case output buffer is too short: return 0 and pOutput will be filled with empty string (= '\0')
//!			In case bufLen == 0:				return 0, pOutput will not be changed
//--------------------------------------------------------------------------------------------------
static size_t DoubleToEFormat(char* pOutput, int dp, float64 value, size_t bufLen, bool sicsFormat)
{
	float64 roundingValue = (float64)0.5L;
	int roundingPos;
	// Required as on TMS320 float64 is passed by reference and real value is evaluated during first assignment
	float64 valueTemp = value;
	int digit = 0;
	int i = 0;
	size_t j = 0;		// String index
	int manCount = 0;	// Digit count of mantissa
	int expo = 0;		// Exponent value
	size_t expLen = 0;	// String length of exponent
	bool isNegative = false;

	// bufLen must be greater than 0
	if (bufLen == 0) {
		return 0;			// Buffer length insufficient
	}

	isNegative = signbit(valueTemp); // sign of value

	if (dp < 0) {
		dp = 0;
	}
	else if (dp == 0) {
		dp = -1;
	}

	roundingPos = dp;

	if (sicsFormat) {
		// Evaluate exponent length
		if (dp > 9) {
			expLen = 5;			// E+100..E+999 needs 5 char
		}
		else {
			expLen = 4;			// E+10..E+99   needs 4 char
		}
		// Check bufLen
		if (bufLen < (size_t)dp + expLen + 2u) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}
	}
	else {
		// bufLen must be >= 4 char
		if (bufLen < 4U) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}
	}

	if (!FLT64_isfinite(valueTemp)) {
		const char *text;
		// value is one of +/-NaN, +/-Inf

		if (isnan(valueTemp)) {
			text = (isNegative) ? "-NaN" : "NaN";
		}
		else {
			text = (isNegative) ? "-INF" : "INF";
		}

		if (bufLen >= (strlen(text) + 1)) {
			RB_STRING_strncpymax(pOutput, text, bufLen);
			return strlen(pOutput);
		}
		else {
			pOutput[0] = '\0';  // Return empty string
			return 0;
		}
	}

	// Process value sign
	if (isNegative) {
		valueTemp = -valueTemp;
		pOutput[j++] = '-';
	}

	// Round value and get exponent
	if (valueTemp != (float64)0.0L) {

		// Get exponent of unrounded value for rounding
		expo = RB_MATH_ilog10(valueTemp);

		// Round value
		if (dp < 0) {
			roundingPos = 0;
		}
		for (i = (roundingPos - expo); i > 0; i--) {
			roundingValue *= (float64)0.1L;
		}
		valueTemp += roundingValue;

		// Get exponent of rounded value and limit value to 9.999...1.000
		expo = 0;
		while (valueTemp < (float64)1.0L) {
			valueTemp = valueTemp * (float64)10.0L;
			expo--;
		}
		while (valueTemp >= (float64)10.0L) {
			valueTemp = valueTemp * (float64)0.1L;
			expo++;
		}
	}

	// Digit count of mantissa
	if (dp >= 0)
		manCount = dp + 1;
	else
		manCount = 1;

	if (!sicsFormat) {
		// Adjust exponent string length
		if (abs(expo) < 10) {
			expLen = 2;			// E0..E9     needs 2 char
		}
		else if (abs(expo) < 100) {
			expLen = 3;			// E10..E99   needs 3 char
		}
		else {
			expLen = 4;			// E100..E999 needs 4 char
		}
		if (expo < 0) {
			expLen++;			// Add length for minus sign
		}

		// Test string length, we need at least 1 mantissa digit + "E" + expLen + trailing zero
		if (expLen + 2U >= (size_t)bufLen) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}
	}
	// Compose mantissa output string
	for (i = (manCount - 1); i >= 0; i--) {

		// String length may not be greater then bufLen - expLen
		if (j >= (bufLen - expLen)) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Output digit
		digit = (int)valueTemp % 10;
		pOutput[j++] = (char)(digit + '0');

		// String length may not be greater then bufLen - expLen
		if (j >= (bufLen - expLen)) {
			pOutput[0] = '\0';	// Return empty string
			return 0;			// String length insufficient
		}

		// Output decimal point
		if (i == dp) {
			pOutput[j++] = '.';
		}
		valueTemp = (valueTemp - (float64)digit) * (float64)10.0L;
	}

	// Compose exponent output string
	pOutput[j++] = 'E';
	if (sicsFormat) {
		if (expo >= 0) {
			pOutput[j++] = '+';
		}
		else {
			pOutput[j++] = '-';
		}

		if ((abs(expo) < 100) && dp > 9) {		// dp >= 10 => 3 digit exponent, else 2
			pOutput[j++] = '0';
		}
		if (abs(expo) < 10) {
			pOutput[j++] = '0';
		}
		RB_FORMAT_Long(&pOutput[j], (int32_t)abs(expo), (size_t)expLen);
	}
	else {
		RB_FORMAT_Long(&pOutput[j], (int32_t)expo, (size_t)expLen);
	}

	// Return length of string
	return strlen(pOutput);
}


