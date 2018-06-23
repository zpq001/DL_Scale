//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Parse.c
//! \ingroup	util
//! \brief		Parsing of simple data types from string to variable.
//!
//! This module contains functions to parse simple data types from a string into a variable.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/12/19 14:27:34MEZ $
// $Revision: 1.31 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Parse"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Parse.h"
// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Debug.h"

#include <ctype.h>
#include <string.h>


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_PARSE_Long
//--------------------------------------------------------------------------------------------------
//! \brief	Read int32_t value from string, like sscanf().
//!
//! Warning! Only values in the range -2147483648..2147483647 are processed properly.
//!
//! \param	ppInput		Pointer to input string to parse, points past int32_t value at return
//! \param	pValue		Returned int32_t value
//! \return
//!     - false	No int32_t value could be scanned
//!     - true	Value could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_Long(const char** ppInput, int32_t* pValue)
{
	bool valueScanOk = false;
	bool valueIsNegativ = false;
	bool parsing = true;
	bool firstChar = true;

	// Check input string
	if ((*ppInput == NULL) || (strlen(*ppInput) == 0U))
		return false;

	// Skip spaces in input
	while (**ppInput == ' ')
		(*ppInput)++;

	// Parse input string
	*pValue = 0L;
	while (parsing) {
		switch (**ppInput) {
			case '+' :
			case '-' :
				if (firstChar) {
					valueIsNegativ = (**ppInput == '-');
				}
				else {
					return false;
				}
				break;

			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				// Check if value * 10 is out of range
				if (*pValue > 214748364L) {
					return false;
				}
				*pValue = *pValue * 10L + (int32_t)(**ppInput - '0');
				// Range check: if value is negative -> out of range
				if ((*pValue < 0) && !((*pValue == LONG_MIN) && valueIsNegativ)) {
					return false;
				}
				valueScanOk = true;
				break;

			default:
				parsing = false;
				break;
		} // switch
		(*ppInput)++;
		firstChar = false;
	} // while
	(*ppInput)--;

	// Adjust sign
	if (valueIsNegativ)
		*pValue = -*pValue;

	return valueScanOk;
}


//--------------------------------------------------------------------------------------------------
// RB_PARSE_ULong
//--------------------------------------------------------------------------------------------------
//! \brief	Read uint32_t value from string, like sscanf().
//!
//! Warning! Only values in the range 0..4294967295 are processed properly.
//!
//! \param	ppInput		Pointer to input string to parse, points past uint32_t value at return
//! \param	pValue		Returned int32_t value
//! \return
//!     - false	No uint32_t value could be scanned
//!     - true	Value could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_ULong(const char** ppInput, uint32_t* pValue)
{
	bool valueScanOk = false;
	bool parsing = true;
	bool firstChar = true;
	uint32_t digit;

	// Check input string
	if ((*ppInput == NULL) || (strlen(*ppInput) == 0U))
		return false;

	// Skip spaces in input
	while (**ppInput == ' ')
		(*ppInput)++;

	// Parse input string
	*pValue = 0UL;
	while (parsing) {
		switch (**ppInput) {
			case '+' :
				if (!firstChar) {
					return false;
				}
				break;

			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				//lint -e571 Suspicious cast
				digit = (uint32_t)(**ppInput - '0');
				// Range check
				if ((*pValue > 429496729uL) || (*pValue == 429496729uL && digit > 5uL)) {
					return false;
				}
				*pValue = *pValue * 10UL + digit;
				//lint +e571 Suspicious cast
				valueScanOk = true;
				break;

			default:
				parsing = false;
				break;
		} // switch
		(*ppInput)++;
		firstChar = false;
	} // while
	(*ppInput)--;

	return valueScanOk;
}


//--------------------------------------------------------------------------------------------------
// RB_PARSE_Double
//--------------------------------------------------------------------------------------------------
//! \brief	Read float64 value from string, like sscanf().
//!
//! \param	ppInput		Pointer to input string to parse, points past float64 value at return
//! \param	pValue		Returned float64 value
//! \return
//!		- false	No float64 value could be scanned
//!		- true	Value could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_Double(const char** ppInput, float64* pValue)
{
	int exponent = 0;
	int expCorr = 0;
	bool valueScanOk = false;
	bool valueIsNegativ = false;
	bool expIsNegativ = false;
	bool decPointParsed = false;
	bool expInputParsed = false;
	bool expSignParsed = false;
	bool parsing = true;
	bool firstChar = true;
	bool firstExpChar = false;

	// Check input string
	if ((*ppInput == NULL) || (strlen(*ppInput) == 0U))
		return false;

	// Skip spaces in input
	while (**ppInput == ' ')
		(*ppInput)++;

	// Parse input string
	*pValue = 0.0;
	while (parsing) {
		switch (**ppInput) {
			case '+' :
			case '-' :
				if (firstChar) {
					valueIsNegativ = (**ppInput == '-');
				}
				else if (expInputParsed && !expSignParsed) {
					expIsNegativ = (**ppInput == '-');
					expSignParsed = true;
				}
				else {
					return false;
				}
				break;

			case 'e' :
			case 'E' :
				if (!expInputParsed) {
					expInputParsed = true;
					expSignParsed = false;
					firstExpChar = true;
				}
				else {
					return false;
				}
				break;

			case '.' :
				if (expInputParsed) {
					return false;
				}
				if (!decPointParsed)
					decPointParsed = true;
				else
					return false;
				break;

			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				if (! expInputParsed) {
					*pValue = *pValue * (float64)10.0L + (float64)(**ppInput - '0');
					valueScanOk = true;
					if (decPointParsed)
						expCorr--;
				}
				else {
					exponent = exponent * 10 + (**ppInput - '0');
					firstExpChar = false;
				}
				break;

			default:
				if (firstExpChar) {
					return false;
				}
				parsing = false;
				break;
		} // switch
		(*ppInput)++;
		firstChar = false;
	}
	(*ppInput)--;

	// Calculate exponent adjustment
	if (expIsNegativ)
		exponent = -exponent;
	exponent = exponent + expCorr;

	// Adjust value according to positive exponent
	while (exponent > 0) {
		*pValue = *pValue * (float64)10.0L;
		exponent--;
	}

	// Adjust value according to negative exponent
	while (exponent < 0) {
		*pValue = *pValue * (float64)0.1L;
		exponent++;
	}

	// Adjust sign
	if (valueIsNegativ)
		*pValue = -*pValue;

	return valueScanOk;
}


//--------------------------------------------------------------------------------------------------
// RB_PARSE_Hex
//--------------------------------------------------------------------------------------------------
//! \brief	Read uint32_t hex value from string, like sscanf().
//!
//! Warning! Only values in the range 0x00000000..0xffffffff are processed properly.
//!	Optional 0x at start of hex string is discarded.
//!
//! \param	ppInput		Pointer to input string to parse, points past uint32_t value at return
//! \param	pValue		Returned uint32_t value
//! \return
//!     - false	No uint32_t value could be scanned
//!     - true	Value could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_Hex(const char** ppInput, uint32_t* pValue)
{
	bool valueScanOk = false;
	bool parsing = true;
	uint32_t digit;

	// Check input string
	if ((*ppInput == NULL) || (strlen(*ppInput) == 0U))
		return false;

	// Skip spaces in input
	while (**ppInput == ' ')
		(*ppInput)++;

	// Discard leading 0x
	if (**ppInput == '0') {
		(*ppInput)++;
		if (**ppInput == 'x') {
			(*ppInput)++;
		}
		else {
			(*ppInput)--;
		}
	}

	// Parse input string
	*pValue = 0UL;
	while (parsing) {
		switch (**ppInput) {
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				//lint -e571 Suspicious cast
				digit = (uint32_t)(**ppInput - '0');
				break;

			case 'A' :
			case 'B' :
			case 'C' :
			case 'D' :
			case 'E' :
			case 'F' :
				//lint -e571 Suspicious cast
				digit = (uint32_t)((**ppInput - 'A') + 10);
				break;

			case 'a' :
			case 'b' :
			case 'c' :
			case 'd' :
			case 'e' :
			case 'f' :
				//lint -e571 Suspicious cast
				digit = (uint32_t)((**ppInput - 'a') + 10);
				break;

			default:
				parsing = false;
				continue;
		} // switch

		// Range check (multiplying with 16 for additional digit must not give overflow)
		//  Max value is decimal 4294967295 (or 0xFFFFFFFF)
		if (*pValue > 268435455) {
			return false;
		}

		// Add digit to value
		*pValue = *pValue * 16UL + digit;
		//lint +e571 Suspicious cast
		valueScanOk = true;
		(*ppInput)++;
	} // while

	return valueScanOk;
}


//--------------------------------------------------------------------------------------------------
// RB_PARSE_MacAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Read MAC address in hex from string.
//!
//! \attention: The destination buffer must be large enough to hold at least 6 octets.
//! \attention: The input string must be in this format: "00:0a:0b:0c:0d:0e", 17 characters.
//!
//! \param	ppInput		The string containing a MAC address in this format: e.g. "00:0a:0b:0c:0d:0e"
//! \param	macAddr		Buffer where the address will be parsed to. Octets will have same order as in input.
//! \param	macflags	Output. Additional information about the parsed MAC address is encoded here.
//!						See RB_PARSE_tMACAddressBitSetValues for possible values.
//! \return
//!     - false	No valid MAC address could be scanned or the MAC address is unconfigured = "00:00:00:00:00:00".
//!     - true	Valid MAC address could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_MacAddress(const char** ppInput, uint8_t* macAddr, uint8_t* macflags)
{
	int i;
	uint32_t value;
	bool isAllZeroAddress = true;

	for (i = 0; i < 6; i++) {
		// Avoid wrong input in last char, e.g. "01:23:45:67:89:fg"
		if (!isxdigit(*(*ppInput+1)))
			return false;
		if (!RB_PARSE_Hex(ppInput, &value)) {
			return false;
		}
		if (value > 255) {
			return false;
		}
		macAddr[i] = (uint8_t)value;

		// Skip last check, check only ':' in between values
		if (i < 5) {
			// Check for ':'
			if (**ppInput == ':') {
				(*ppInput)++;
			}
			else {
				return false;
			}
		}
	}

	*macflags = 0;
	// Check Multicast Address (first octet, lowest bit)
	if (macAddr[0] & 0x01) {
		RB_DEBUG_INFO("Parsed a Multicast MAC address");
		*macflags |= (RB_PARSE_MACADDRESSBITSETVALUE_MULTICAST & 0xFF);
	}

	// Check whether the MAC address is unconfigured (all zeroes address)
	for(i = 0; i < 6; i++) {
		if (macAddr[i] != 0x00) {
			isAllZeroAddress = false;
		}
	}
	if (isAllZeroAddress) {
		RB_DEBUG_INFO("Parsed an all-zero MAC address");
		*macflags |= (RB_PARSE_MACADDRESSBITSETVALUE_UNCONFIGURED & 0xFF);
	}

	return true;
}


//--------------------------------------------------------------------------------------------------
// RB_PARSE_IpAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Read IP address from string.
//!
//! \attention: The input string must be in this format: "172.24.48.1"
//!
//! \param	ppInput	The string containing a IP address in this format: e.g. "172.24.48.1"
//! \param	pIpAddr	IP address as uint32_t value (address will be in network byte order = big endian)
//! \return
//!     - false	No valid IP address could be scanned
//!     - true	Valid IP address could be scanned
//--------------------------------------------------------------------------------------------------
bool RB_PARSE_IpAddress(const char** ppInput, uint32_t* pIpAddr)
{
	int i;
	uint32_t value = 0UL;

	*pIpAddr = 0UL;
	for (i = 0; i < 4; i++) {
		if (!RB_PARSE_ULong(ppInput, &value)) {
			return false;
		}
		if (value > 255) {
			return false;
		}
		*pIpAddr = *pIpAddr * 256 + value;

		// Skip last check, check only '.' in between values
		if (i < 3) {
			// Check for '.'
			if (**ppInput == '.') {
				(*ppInput)++;
			}
			else {
				return false;
			}
		}
	}
	return true;
}

