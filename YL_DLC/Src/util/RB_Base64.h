//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Base64.h
//! \ingroup	util
//! \brief		Contains functions to perform Base64 encoding/decoding
//!
//!	Refer to  http://www.php-einfach.de/base64_generator.php  for online Base64 encoding/decoding
//!
//! This Module ist tested against the test vectors given in http://tools.ietf.org/html/rfc4648#page-12
//!
//! The encoding process represents 24-bit groups of input bits as output
//!	strings of 4 encoded characters.  Proceeding from left to right, a
//!	24-bit input group is formed by concatenating 3 8-bit input groups.
//! These 24 bits are then treated as 4 concatenated 6-bit groups, each
//!	of which is translated into a single character in the base 64 alphabet.
//!
//! \verbatim
//!  +----------------+-------------------+-------------------+-------------------+
//!  | INPUT BYTES    |   B Y T E  1      |   B Y T E  2      |   B Y T E  3      |
//!  +----------------+-------------------+-------------------+-------------------+
//!  | INPUT BITS     | 7 6 5 4 3 2   1 0 | 7 6 5 4   3 2 1 0 | 7 6   5 4 3 2 1 0 |
//!  +----------------+-------------+-----+---------+---------+-----+-------------+
//!  | OUTPUT BITS    | 5 4 3 2 1 0 | 5 4   3 2 1 0 | 5 4 3 2   1 0 | 5 4 3 2 1 0 |
//!  +----------------+-------------+---------------+---------------+-------------+
//!  | OUTPUT BYTES   | B Y T E  1  |  B Y T E  2   |  B Y T E  3   | B Y T E  4  |
//!  +----------------+-------------+---------------+---------------+-------------+
//! \endverbatim
//!
//!
//! \b   Usage:
//!
//!		Since the RB_BASE64_Encode() function returns a longer output string than the number of bytes in the input string,
//!		the user has to allocate sufficient memory to store the Base64 encoded string.
//!		Using the encoding example below we see that the string "Hello World" consists of 11 characters.
//!		If we round up the number of characters to the next multiple of 3 (in our case 12) and multiply
//!		it by 4/3 we get a required length of 16 characters for the output string.
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Yves Schenker
//
// $Date: 2016/11/10 08:58:38MEZ $
// $Revision: 1.32 $
//
//==================================================================================================

#ifndef _base64__h
#define _base64__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_BASE64) && (RB_CONFIG_USE_BASE64 == RB_CONFIG_YES)

#include <string.h>


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Get size of base64 encoded string given the unencoded data size (bytes).
//! This includes space for a terminating zero after the base64 encoding
//! Size of base64 encoded data is calculated as follows:
//! - if input is dividable by 3, 4 output bytes will be written for 3 input bytes + 1 term. zero.
//! - if input is not dividable by 3, 4 output bytes per complete group of 3 input bytes will be written + 4 bytes for incomplete group of 3 + 1 term. zero.
#define RB_BASE64_GET_ENCODED_SIZE_B(DECODED_SIZE_B)  (((DECODED_SIZE_B) % 3) ? (size_t)((((DECODED_SIZE_B)/3)*4) + 4 + 1) : (size_t)(((DECODED_SIZE_B)/3)*4 + 1))


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//!	Return values for encoding and decoding functions
typedef	enum
{
	RB_BASE64_OPERATION_SUCCESSFUL		=  1,	//!< Operation successfull
	RB_BASE64_OUTPUT_BUFFER_TO_SMALL	= -1,	//!< Not enough space in target buffer to perform operation
	RB_BASE64_SIZE_NOT_DIVIDABLE_BY_4	= -2	//!< Size of buffer is unexpected
} RB_DECL_TYPE RB_BASE64_tStatus;

//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_BASE64_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_BASE64_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_BASE64_Encode
//--------------------------------------------------------------------------------------------------
//! \brief	Encodes a buffer of arbitrary length to a Base64 formatted string.
//!
//! A terminating zero will be added after the encoded data.
//!
//! \param	outBuffer		output	Pointer to base64 encoded output buffer (zero terminated base64 encoded string will be placed here).
//! \param	inBuffer		input	Pointer to binary input buffer (binary data to be encoded)
//! \param	outBufferSize	input	Number of bytes reserved for output string buffer  (must fit base64 encoding, including terminating zero).
//! \param	inDataSize		input	Number of bytes in inBuffer (size of binary data to encode).
//! \param	encodedLength	output	Length of base64 encoded data, not including the terminating zero.
//!
//! \return	Status of the Base64 encode procedure
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_BASE64_tStatus RB_BASE64_Encode(char* outBuffer, const char* inBuffer, size_t outBufferSize, size_t inDataSize, size_t* encodedLength);


//--------------------------------------------------------------------------------------------------
// RB_BASE64_Decode
//--------------------------------------------------------------------------------------------------
//! \brief	Decodes binary data of arbitrary length from a Base64 formatted string.
//!
//! No terminating zero will be written after the decoded payload data (since data can be arbitrary binary data).
//!
//! \param	outBuffer		output	Pointer to output buffer (destination, where decoded binary data will be stored to)
//! \param	inBuffer		input	Pointer to input buffer (source, where Base64 encoded data string resides)
//! \param	outBufferSize	input	Number of bytes reserved for outBuffer (max. size of decoded binary data)
//! \param	inDataSize		input	Number of bytes reserved for inBuffer (number of base64 encoded characters to decode, must be multiple of 4
//!									do not count terminating zero of base64 encoded string)
//! \param	decodedLength	output	Size of actually decoded binary output data
//!
//! \return	Status of the Base64 decode procedure
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_BASE64_tStatus RB_BASE64_Decode(char* outBuffer, const char* inBuffer, size_t outBufferSize, size_t inDataSize, size_t* decodedLength);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_BASE64
#endif // _base64__h
