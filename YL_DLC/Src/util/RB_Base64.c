//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Base64.c
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
//!		Using the encoding example below we see that the input string "Hello World" consists of 11 bytes.
//!		If we round up the number of bytes to the next multiple of 3 (in our case 12) and multiply
//!		it by 4/3 we get a required length of 16 characters for the base64 encoded output string.
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Yves Schenker
//
// $Date: 2016/12/20 11:03:24MEZ $
// $Revision: 1.40 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Base64"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_BASE64) && (RB_CONFIG_USE_BASE64 == RB_CONFIG_YES)

#include "RB_Base64.h"

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

// Base64 coding table. Maps Base64 values to ASCII characters
// Input  = Array index    = Base64 value
// Output = Array contents = ASCII character
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Base64 decoding table. Maps ASCII characters to Base64Values
// Array index + 43 = ASCII value
// If an ASCII character is not used in the Base64 alphabet we use 99 as a dummy Base64 value.
// Input  = Array index + 43 = ASCII character value
// Output = Array contents   = Base64 value
static const uint8_t db64[]={62,99,99,99,63,52,53,54,55,56,57,58,59,60,61,99,99,99,99,99,99,99,
						0 , 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,
						22,23,24,25,99,99,99,99,99,99,26,27,28,29,30,31,32,33,34,35,36,37,
						38,39,40,41,42,43,44,45,46,47,48,49,50,51};

//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static void RB_BASE64_EncodeBlock(char* out, const char* in, size_t len);
static void RB_BASE64_DecodeBlock(char* out, const char* in);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_BASE64_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
void RB_BASE64_Initialize(void)
{
	// Nothing to do.
}


//--------------------------------------------------------------------------------------------------
// RB_BASE64_Encode
//--------------------------------------------------------------------------------------------------
//! \brief	Encodes a buffer of arbitrary length to a base64 formatted string.
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
RB_BASE64_tStatus RB_BASE64_Encode(char* outBuffer, const char* inBuffer, size_t outBufferSize, size_t inDataSize, size_t* encodedLength)
{
	size_t actualInPosition    = 0;
	size_t actualOutPosition   = 0;
	size_t InPositionIncrement = 0;

	// inDataSize	paddedLength
	// 0			0
	// 1			4
	// 2			4
	// 3			4
	// 4			8
	// 5			8
	// 6			8
	// 7			12
	size_t paddedLength = ((inDataSize + ((3 - (inDataSize % 3)) % 3))/3)*4;

	// Return encoded length size to caller
	*encodedLength = paddedLength;

	// Check if target buffer is big enough to hold the encoded data + terminating zero
	if (paddedLength + 1 > outBufferSize) {
		RB_DEBUG_WARN("Encode: target buffer not large enough!");
		return RB_BASE64_OUTPUT_BUFFER_TO_SMALL;
	}

	// Encode block after block until the end of the in inBuffer is reached
	while (actualInPosition < inDataSize) {
		// There is still something to encode

		// Compute Base64 block length
		if (inDataSize - actualInPosition >= 3) {
			// There is still 3 or more bytes to encode
			InPositionIncrement = 3;
		}
		else {
			// There is less than 3 bytes to encode left
			InPositionIncrement = inDataSize - actualInPosition; // remaining input bytes
		}

		// Encode the input block and write encoded block to outBuffer
		// InPositionIncrement is in range [1, 3].
		RB_BASE64_EncodeBlock(outBuffer + actualOutPosition, inBuffer + actualInPosition, InPositionIncrement);

		// Increment positions
		actualInPosition  = actualInPosition  + InPositionIncrement;
		actualOutPosition = actualOutPosition + 4; // 3 input bytes yield 4 encoded output bytes

	} // end while

	// Terminate encoded string
	outBuffer[actualOutPosition] = '\0';

	return RB_BASE64_OPERATION_SUCCESSFUL;
}


//--------------------------------------------------------------------------------------------------
// RB_BASE64_Decode
//--------------------------------------------------------------------------------------------------
//! \brief	Decodes binary data of arbitrary length from a base64 formatted string.
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
RB_BASE64_tStatus RB_BASE64_Decode(char* outBuffer, const char* inBuffer, size_t outBufferSize, size_t inDataSize, size_t* decodedLength)
{
	size_t actualInPosition  = 0;
	size_t actualOutPosition = 0;
	size_t paddingCounter    = 0;

	// Coarse check if outBuffer is big enough to hold the encoded data - With no respect to padding
	// 4 encoded characters will lead to 3 decoded characters
	*decodedLength = (inDataSize / 4) * 3;
	if( *decodedLength > outBufferSize ) {
		RB_DEBUG_WARN("Decode: target buffer not large enough!");
		return RB_BASE64_OUTPUT_BUFFER_TO_SMALL;
	}

	if( inDataSize % 4 != 0 ) {
		// Size of base64 encoding is not dividable by 4!
		RB_DEBUG_WARN("Decode: unexpected length of base64 encoding - should be dividable by 4!");
		return RB_BASE64_SIZE_NOT_DIVIDABLE_BY_4;
	}

	// Decode block after block until the end of the in inBuffer is reached
	while( actualInPosition < inDataSize ) {

		// Decode the input block of Base64 characters and write decoded block to outBuffer
		RB_BASE64_DecodeBlock(outBuffer + actualOutPosition, inBuffer + actualInPosition);

		// Increment positions
		actualOutPosition = actualOutPosition + 3;
		actualInPosition  = actualInPosition  + 4;
	}

	// Find number of padded characters. Last 2 characters of encoded string
	// may be padding ("=") character.
	if ((int)actualInPosition - 1 >= 0) {
		if (inBuffer[actualInPosition - 1] == '=') {
			paddingCounter++;
		}
		if ((int)actualInPosition - 2 >= 0) {
			if (inBuffer[actualInPosition - 2] == '=') {
				paddingCounter++;
			}
		}
	}

	// Update output message length - With respect to padding
	*decodedLength = *decodedLength - paddingCounter;

	return RB_BASE64_OPERATION_SUCCESSFUL;
}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_BASE64_EncodeBlock
//--------------------------------------------------------------------------------------------------
//! \brief	Encodes the input block of binary data with the length of 1 to 3 bytes to the output block
//!			with the length of 4 bytes in the Base64 format.
//! When the number of bytes to encode is not divisible by 3 (that is, if there are only one or two bytes of input for the last block),
//! then the following action is performed: Add extra bytes with value zero so there are three bytes,
//! and perform the conversion to base64.
//! If there was only one significant input byte, only the first two base64 digits are significant,
//! and if there were two significant input bytes, the first three base64 digits are significant. '=' characters
//! will be added to make the last block contain four base64 characters.
//!
//! No terminating zeroes are added by this function.
//!
//! \param	out		output	Pointer to output block (available size must be 4)
//! \param	in		input	Pointer to input block (available size must be equal to "len" value).
//! \param	len		input	Number of bytes to be encoded (consumed from input buffer).
//!							Allowed values are in range [1,3].
//--------------------------------------------------------------------------------------------------
static void RB_BASE64_EncodeBlock(char* out, const char* in, size_t len)
{
	const uint8_t* pIn = (const uint8_t*) in;
	char fillBuffer[3];

	if (len != 3) {
		// Input block size is not 3. According to the base64 specs, we must fill
		// the unused bytes with 0 before encoding.
		RB_RESET_ARRAY(fillBuffer);
		memcpy(fillBuffer, in, len);
		pIn = (const uint8_t*) fillBuffer; // use fillBuffer for further calculation
	}

	// Output byte 0 and 1 are always used
	out[0] = cb64[pIn[0] >> 2 & 0x3f];
	out[1] = cb64[((pIn[0] & 0x03) << 4) | ((pIn[1] & 0xf0) >> 4)];

	// Add padding for output byte 2 and 3 if required (len < 3)
	// 1 input byte --> 2 output bytes + 2 padding
	// 2 input bytes --> 3 output bytes + 1 padding
	// 3 input bytes --> 4 output bytes + 0 padding
	out[2] = (len > 1 ? cb64[((pIn[1] & 0x0f) << 2) | ((pIn[2] & 0xc0) >> 6)] : '='); // adds padding for second byte if required.
	out[3] = (len > 2 ? cb64[pIn[2] & 0x3f] : '='); // adds padding for third byte if required
}


//--------------------------------------------------------------------------------------------------
// RB_BASE64_DecodeBlock
//--------------------------------------------------------------------------------------------------
//! \brief	Decodes the input block in the Base64 format with the length of 4 bytes to the output
//!			block of binary data with the length of 3 bytes.
//!
//! No terminating zeroes are added by this function.
//!
//! Function deals with padding characters ("=") that were added by base64 encode functions to get a
//! a base64 encoding whith a length which is a multiple of 4.
//!
//! \param	out		output	Pointer to output block (size must be 3 since decoded base64 block may have size 1 to 3)
//! \param	in		input	Pointer to input block (size must be 4 --> size of a base64 block)
//--------------------------------------------------------------------------------------------------
static void RB_BASE64_DecodeBlock(char* out, const char* in)
{
	uint8_t inTemp[4];
	int i;

	// loop for every input character
	for (i=0; i<(int)RB_ARRAY_SIZE(inTemp); i++){
		// Perform decode if we have a valid Base64 character
		if( (in[i] >= 43) && (in[i]<= 122) ) {
			inTemp[i] = db64[(int)in[i] - 43];
		}
		else {
			inTemp[i] = 99; // dummy character
			RB_DEBUG_WARN("DecodeBlock processed an invalid base64 character");
		}
	}

	// Decode
	out[0] = ((inTemp[0] << 2) & 0xff) | inTemp[1] >> 4;
	out[1] = ((inTemp[1] << 4) & 0xff) | inTemp[2] >> 2;
	out[2] = ((inTemp[2] << 6) & 0xc0) | inTemp[3];
}


#endif // RB_CONFIG_USE_BASE64
