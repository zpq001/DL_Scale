//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Parse.h
//! \ingroup	util
//! \brief		Parsing of simple data types from string to variable.
//!
//! This module contains functions to parse simple data types from a string into a variable.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.25 $
//
//==================================================================================================

#ifndef _RB_Parse__h
#define _RB_Parse__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

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

//! Bit definitions for bit set returned by MAC address parsing function.
typedef enum {
	//! Unconfigured MAC address parsed (00:00:00:00:00:00)
	RB_PARSE_MACADDRESSBITSETVALUE_UNCONFIGURED = 1,
	//! Multicast MAC address parsed
	//! The least-significant bit of the first octet of the MAC address is set if it is a
	//! multicast MAC address.
	RB_PARSE_MACADDRESSBITSETVALUE_MULTICAST = 2

} RB_PARSE_tMACAddressBitSetValues;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
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
RB_DECL_FUNC bool RB_PARSE_Long(const char** ppInput, int32_t* pValue);


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
RB_DECL_FUNC bool RB_PARSE_ULong(const char** ppInput, uint32_t* pValue);


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
RB_DECL_FUNC bool RB_PARSE_Double(const char** ppInput, float64* pValue);


//--------------------------------------------------------------------------------------------------
// RB_PARSE_Hex
//--------------------------------------------------------------------------------------------------
//! \brief	Read uint32_t hex value from string, like sscanf().
//!
//! Warning! Only values in the range 0x00000000..0xffffffff are processed properly.
//!	Optional 0x at start of hex string is discarded.
//!
//! \param	ppInput		Pointer to input string to parse, points past uint32_t value at return
//! \param	pValue		Returned int32_t value
//! \return
//!     - false	No uint32_t value could be scanned
//!     - true	Value could be scanned
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_PARSE_Hex(const char** ppInput, uint32_t* pValue);


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
RB_DECL_FUNC bool RB_PARSE_MacAddress(const char** ppInput, uint8_t* macAddr, uint8_t* macflags);


//--------------------------------------------------------------------------------------------------
// RB_PARSE_IpAddress
//--------------------------------------------------------------------------------------------------
//! \brief	Read IP address from string.
//!
//! \attention: The input string must be in this format: "192.127.0.1"
//!
//! \param	ppInput	The string containing a IP address in this format: e.g. "192.127.0.1"
//! \param	pIpAddr	IP address as uint32_t value (address will be in network byte order = big endian)
//! \return
//!     - false	No valid IP address could be scanned
//!     - true	Valid IP address could be scanned
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_PARSE_IpAddress(const char** ppInput, uint32_t* pIpAddr);


#ifdef __cplusplus
}
#endif

#endif // _RB_Parse__h
