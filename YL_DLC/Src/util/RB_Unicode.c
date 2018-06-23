//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Unicode.c
//! \ingroup	util
//! \brief		Support for UTF8 string handling and conversions to UCS2 and 8-bit encodings
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser, Lukas Schwarz, Bram Scheidegger
//
//  Please refer to the overall-description in the corresponding header-file
//
//
// ----- Idea: implementation of conversion tables > U+FFFF -----
// Currently, the conversion tables support 16bit Unicode code points. In case conversion tables
// with code points > U+FFFF need to be supported, the Unicode module can be extended as follows:
// 1. Add configuration option to switch between 16 bit and 32 bit conversion tables. Set default
//    to 16 bit.
// 2. Adjust RB_UNICODE_tEightbitExtToUnicodeTable definition according conversion table size setting.
// 3. Enclose conversion tables with preprocessor macros: if conversion table size is set to 16 bit,
//    only the 16 bit conversion tables are active. If conversion table size is set to 32 bit,
//    only the 32 bit conversion tables are active.
// 4. Adjust methods in Unicode module dealing with conversion tables to account for 32 bit
//    conversion tables (use preprocessor for case distinction).
//
// $Date: 2017/06/14 19:48:42MESZ $
// $Revision: 1.44 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Unicode"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_UNICODE) && (RB_CONFIG_USE_UNICODE == RB_CONFIG_YES)

#include "RB_Unicode.h"


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_UNICODE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_UNICODE_Initialize(void)
{
	// Nothing to do
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8charlenBytes
//--------------------------------------------------------------------------------------------------
//! \brief	Determine length in bytes of a UTF8 character
//!
//! In case s is a UTF8 string and not only a UTF8 character, this method will return the length in
//! bytes of the first character of the UTF8 string. Subsequent bytes are NOT checked.
//! Note: this method returns also the correct result on platforms where 1 byte is 16 bit
//!
//! \param	s	Pointer to UTF8 character
//! \return		Length in bytes of a UTF8 character. 0 if first byte of UTF8 character
//!				is invalid (it contains the header).
//--------------------------------------------------------------------------------------------------
size_t RB_UNICODE_UTF8charlenBytes(const RB_UNICODE_tUTF8 *s)
{
	// Fundamental idea: analyze first byte to determine the UTF-8 character length in bytes On a
	// platform, where 1 byte = 16 bits, this calculation is correct as well. Reason: the
	// "numOfBytes" calculated in this method corresponds to the number of RB_UNICODE_tUTF8 elements
	// required to store the character. 1 RB_UNICODE_tUTF8 element corresponds to 1 byte, even on a
	// TMS 320 because the non-existing type "uint8_t" is simulated using a "uint16_t" on a TMS 320

	size_t numOfBytes = 0;
	RB_UNICODE_tUTF8 utfByte1 = *s;

	if ((utfByte1 & 0xFE) == 0xFC)
	{	// The UTF-8 character consists of 6 bytes:
		// 1111 110x | 10xx xxxx | 10xx xxxx | 10xx xxxx | 10xx xxxx | 10xx xxxx
		numOfBytes = 6;
	}
	else if ((utfByte1 & 0xFC) == 0xF8)
	{	// The UTF-8 character consists of 5 bytes:
		// 1111 10xx | 10xx xxxx | 10xx xxxx | 10xx xxxx | 10xx xxxx
		numOfBytes = 5;
	}
	else if ((utfByte1 & 0xF8) == 0xF0)
	{	// The UTF-8 character consists of 4 bytes:
		// 1111 0xxx | 10xx xxxx | 10xx xxxx | 10xx xxxx x
		numOfBytes = 4;
	}
	else if ((utfByte1 & 0xF0) == 0xE0)
	{	// The UTF-8 character consists of 3 bytes:
		// 1110 xxxx | 10xx xxxx | 10xx xxxx
		numOfBytes = 3;
	}
	else if ((utfByte1 & 0xE0) == 0xC0)
	{	// The UTF-8 character consists of 2 bytes:
		// 110x xxxx | 10xx xxxx
		numOfBytes = 2;
	}
	else if ((utfByte1 & 0x80) == 0x0)
	{	// the UTF-8 character consists of 1 byte 0xxxxxxx
		// This case includes the null character, as it is a valid character of size 1
		numOfBytes = 1;
	}
	else
	{
		// Invalid character (e.g. starting with 0xff)
		numOfBytes = 0;
	}

	return numOfBytes;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8strlenBytes
//--------------------------------------------------------------------------------------------------
//! \brief	Determine length in bytes of UTF8 string
//!
//! A UTF8 string in C is NULL terminated. This method searches for a NULL termination character and
//! returns the number of bytes until (not including) the NULL character is encountered.
//! Note: this method returns also the correct result on platforms where 1 byte is 16 bit, because
//! internally this method counts the number of RB_UNICODE_tUTF8 characters.
//!
//! \param	s	Pointer to UTF8 string
//! \return		Length in bytes of a UTF8 string (not including terminal NULL character)
//--------------------------------------------------------------------------------------------------
size_t RB_UNICODE_UTF8strlenBytes(const RB_UNICODE_tUTF8 *s)
{
	size_t counter = 0;

	while (*s++)
		counter++;

	return counter;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8strlenChars
//--------------------------------------------------------------------------------------------------
//! \brief	Determine length in characters of UTF8 string
//!
//! A UTF8 string in C is NULL terminated. This method searches for a NULL termination character and
//! returns the number of UTF8 characters until (not including) the NULL character is encountered.
//! UTF8 input string validity is not checked.
//!
//! \param	s	Pointer to UTF8 string
//! \return		Length in characters of a UTF8 string (not including terminal NULL character).
//!				Returns 0 in case an invalid UTF8 character (according to first byte) is
//!				encountered.
//--------------------------------------------------------------------------------------------------
size_t RB_UNICODE_UTF8strlenChars(const RB_UNICODE_tUTF8 *s)
{
	// Fundamental idea: iterate through string. Determine length of current character and skip it.
	// Counter tracks number of iterations.

	size_t counter = 0;
	size_t charBytes = 0;

	while (*s)
	{
		charBytes = RB_UNICODE_UTF8charlenBytes(s);
		if (charBytes == 0)
		{
			// Error from RB_UNICODE_UTF8charlenbytes. A UTF8 char has a least length 1
			// Return 0 to pass error to user. Stop method execution to avoid infinite loop.
			return 0;
		}
		counter++;
		s += charBytes;
	}

	return counter;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UCS2strlenBytes
//--------------------------------------------------------------------------------------------------
//! \brief	Determine length in bytes of UCS2 string
//!
//! A UCS2 string in C is NULL terminated (0x0000, 16 bit). This method searches for a NULL
//! termination character and returns the number of bytes until (not including) the NULL character
//! is encountered.
//! Note: this method returns also the correct result on platforms where 1 byte is 16 bit, because
//! internally this method takes the size of RB_UNICODE_tUCS2 elements into account.
//!
//! \param	s	Pointer to UCS2 string
//! \return		Length in bytes of a UCS2 string (not including terminal NULL character)
//--------------------------------------------------------------------------------------------------
size_t RB_UNICODE_UCS2strlenBytes(const RB_UNICODE_tUCS2 *s)
{
	return RB_UNICODE_UCS2strlenChars(s) * sizeof(RB_UNICODE_tUCS2);
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UCS2strlenChars
//--------------------------------------------------------------------------------------------------
//! \brief	Determine length in characters of UCS2 string
//!
//! A UCS2 string in C is NULL terminated (0x0000, 16 bit). This method searches for a NULL
//! termination character and returns the number of characters until (not including) the NULL
//! character is  encountered.
//!
//! \param	s	Pointer to UCS2 string
//! \return		Length in characters of a UCS2 string (not including terminal NULL character).
//--------------------------------------------------------------------------------------------------
size_t RB_UNICODE_UCS2strlenChars(const RB_UNICODE_tUCS2 *s)
{
	size_t counter = 0;

	while (*s++)
		// Iterate until we find a 16bit (RB_UNICODE_tUCS2) NULL character
		counter++;

	return counter;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8charIsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Determine if given character is a valid UTF8 character
//!
//! A valid UTF8 character has a leading byte defining the character length followed by the defined
//! number of additional bytes. The additional bytes must have the format 10xxxxxx.
//!
//! \param	utf8	Pointer to UTF8 string
//! \return			True if UTF8 character is valid, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_UTF8charIsValid(const RB_UNICODE_tUTF8 *utf8)
{
	bool success = false;
	size_t byteLength = 0;

	byteLength = RB_UNICODE_UTF8charlenBytes(utf8);

	// (byte length == 0) ==> UTF8 header byte invalid
	if (byteLength > 0)
	{
		// Skip first byte as RB_UNICODE_UTF8charlenBytes already analyzed it
		byteLength--;
		utf8++;

		success = true;
		while (byteLength > 0 && success)
		{
			success = (((*utf8) & 0xC0) == 0x80);
			utf8++;
			byteLength--;
		}
	}

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8stringIsValid
//--------------------------------------------------------------------------------------------------
//! \brief	Determine if given string is a valid UTF8 string
//!
//! A valid UTF8 string consists of a NULL terminated sequence of valid UTF8 characters.
//!
//! \param	utf8	Pointer to UTF8 string
//! \return			True if UTF8 string is valid, false otherwise
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_UTF8stringIsValid(const RB_UNICODE_tUTF8 *utf8)
{
	bool success = true;

	while ((*utf8 != 0) && success)
	{
		success = RB_UNICODE_UTF8charIsValid(utf8);
		utf8 += RB_UNICODE_UTF8charlenBytes(utf8);
	}

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8strncatmax
//--------------------------------------------------------------------------------------------------
//! \brief	Concatenate two UTF8 strings
//!
//! Concatenate two UTF8 strings and store them in the dst buffer. The resulting string is null
//! terminated and a pointer to it returned by this function. If the UTF8 src string contains an
//! invalid character according to its header, concatenation is aborted. This situation can be
//! detected by comparing input and output length (see below).
//!
//! RB_UNICODE_UTF8strncatmax writes at most siz bytes to dst. If the dst buffer is too small to fit
//! the entire concatenated string, this function writes as many characters as possible to dst. It
//! is guaranteed that no partial UTF8 characters are written to dst. Example:
//!
//! Consider a UTF8 character which is 3 bytes long and the remaining buffer size is 3 bytes as
//! well. 1 byte is reserved for null termination, leaving 2 bytes of effective storage space.
//! Because  there is not enough room to fit the entire 3 byte UTF8 character, it is omitted.
//! RB_UNICODE_UTF8strncatmax NULL terminates the string immediately and returns.
//!
//! To determine whether the concatenation was successful (no cut-off), one can compare the input
//! and output length. Example:
//!
//! \code
//! size_t expectedLen = RB_UNICODE_UTF8strlenBytes(src) + RB_UNICODE_UTF8strlenBytes(dst);
//! RB_UNICODE_tUTF8* concatStr = RB_UNICODE_UTF8strncatmax(dst, src, 20);
//! if (RB_UNICODE_UTF8strlenBytes(concatStr) != expectedLen)
//!		RB_DEBUG_WARN("Concatenation failed, cut-off");
//! \endcode
//!
//! \param	dst		Destination buffer where concatenated string is stored
//! \param	src		Source buffer (UTF8 string to append to dst)
//! \param	siz		dst buffer size in bytes (at most siz bytes are written to dst)
//! \return			Pointer to concatenated string
//--------------------------------------------------------------------------------------------------
RB_UNICODE_tUTF8* RB_UNICODE_UTF8strncatmax(RB_UNICODE_tUTF8 *dst, const RB_UNICODE_tUTF8 *src, size_t siz)
{
	// Fundamental idea: jump to end of string in dst and append one UTF8 character after another to
	// dst. Decrement siz to keep track of the remaining buffer space in dst.

	size_t i = 0;
	size_t dstLen = 0;
	size_t charBytes = 0;
	RB_UNICODE_tUTF8 *dstStart = dst;


	if (siz > 0)
		// Reserve space for null termination (1 byte)
		siz--;

	dstLen = RB_UNICODE_UTF8strlenBytes(dst);
	if (dstLen < siz)
	{
		siz -= dstLen;
		dst += dstLen;

		// Iterate until there are no more UTF8 characters in src or buffer full
		while (*src != 0)
		{
			charBytes = RB_UNICODE_UTF8charlenBytes(src);
			if (charBytes == 0)
				// Character invalid, abort to avoid infinite loop
				break;

			if (charBytes <= siz)
			{
				// We have enough room for this UTF8 character
				siz -= charBytes;
				for (i = 0; i < charBytes; i++)
				{
					// Copy UTF8 character
					*dst = *src;
					dst++;
					src++;
				}
			}
			else
			{
				// Not enough room for another character ==> stop
				break;
			}
		}

		// Add null termination
		*dst = 0;
	}

	return dstStart;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UTF8strncpymax
//--------------------------------------------------------------------------------------------------
//! \brief	Create a copy of a UTF8 string
//!
//! Copy a UTF8 string from src to destination. The resulting string will be NULL terminated and
//! this function will write at most siz bytes to dst (including NULL).
//!
//! \param	dst		Destination buffer to store copy of UTF8 string
//! \param	src		Pointer to source UTF8 string
//! \param	siz		Destination buffer size in bytes
//! \return			Pointer to beginning of copy of UTF8 string
//--------------------------------------------------------------------------------------------------
RB_UNICODE_tUTF8* RB_UNICODE_UTF8strncpymax(RB_UNICODE_tUTF8 *dst, const RB_UNICODE_tUTF8 *src, size_t siz)
{
	RB_UNICODE_tUTF8* dstStart = dst;

	// If buffer size is at least 1, we can simply convert dst into an empty string
	// and use the append function to copy the src to destination.
	if (siz)
	{
		*dst = 0;
		RB_UNICODE_UTF8strncatmax(dst, src, siz);
	}

	return dstStart;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UCS2strncatmax
//--------------------------------------------------------------------------------------------------
//! \brief	Concatenate two UCS2 strings
//!
//! Concatenate two UCS2 strings and store them in the dst buffer. The resulting string is null
//! terminated and a pointer to it returned by this function.
//!
//! \param	dst		Destination buffer where concatenated string is stored
//! \param	src		Source buffer (UCS2 string to append to dst)
//! \param	siz		Destination buffer size in characters (RB_UNICODE_tUCS2)
//!					!!! WARNING !!! In general, one RB_UNICODE_tUCS2 character is NOT 1 byte
//! \return			Pointer to resulting string
//--------------------------------------------------------------------------------------------------
RB_UNICODE_tUCS2* RB_UNICODE_UCS2strncatmax(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUCS2 *src, size_t siz)
{
	// Principal idea: 1) Find end of string in dst. 2) If there is still buffer space left,
	// append as much of src until buffer size is exceeded or end of string in src.
	// Note: cannot use RB_STRING functions because the null character is represented by
	// 2 bytes: 0x00 0x00

	size_t dstStrLen = 0;
	RB_UNICODE_tUCS2* dstStart = dst;


	// === Step 1: goto end of dst and adapt remaining buffer size ===
	dstStrLen = RB_UNICODE_UCS2strlenChars(dst);
	if (siz <= dstStrLen)
		// Corner case: if siz is less than dst string size, dst should not change
		return dstStart;

	// Advance ptr of dst to end of string and adapt remaining buffer size
	dst += dstStrLen;
	siz -= dstStrLen;

	// At this point:
	// 1) Siz will be at least 1 ==> important for NULL termination
	// 2) dst points to location, where we can start appending


	// === Step 2: append ===

	// Copy src to destination until a) end of string in src or
	// b) no more space left (except for null)
	while (*src && siz > 1)
	{
		*dst = *src;
		dst++;
		src++;
		siz--;
	}

	// At this point:
	// a) siz is at least one
	// b) dst points to the location, where NULL should be inserted (pointer has already
	//    been advanced by the while loop above)

	// Append null termination
	*dst = 0;

	return dstStart;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_UCS2strncpymax
//--------------------------------------------------------------------------------------------------
//! \brief	Create a copy of a UCS2 string
//!
//! Copy a UCS2 string from src to destination. The resulting string will be NULL terminated and
//! this function will write at most siz RB_UNICODE_tUCS2 characters to dst (including NULL).
//!
//! \param	dst		Destination buffer to store copy of UCS2 string
//! \param	src		Pointer to source UCS2 string
//! \param	siz		Destination buffer size in characters (RB_UNICODE_tUCS2)
//!					!!! WARNING !!! In general, one RB_UNICODE_tUCS2 character is NOT one byte
//! \return			Pointer to copy of string
//--------------------------------------------------------------------------------------------------
RB_UNICODE_tUCS2* RB_UNICODE_UCS2strncpymax(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUCS2 *src, size_t siz)
{
	RB_UNICODE_tUCS2* dstStart = dst;

	// If buffer size is at least 1, we can simply convert dst into an empty string
	// and use the append function to copy the src to dst.
	if (siz)
	{
		*dst = 0;
		RB_UNICODE_UCS2strncatmax(dst, src, siz);
	}

	return dstStart;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUCS2CharacterToEightbitCharacter
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UCS2 character to an eightbit character.
//!
//! Convert a UCS2 character to an eightbit character using the given conversion table. If the
//! character is not present in the conversion table, it is replaced by a "?" and this method
//! returns false.
//!
//! \param	dst			Buffer to store converted character
//! \param	ucs2Char	Source character to be converted
//! \param	convTable	Conversion table
//! \return				True if conversion succeeded, false if the character could not be
//!						found in the conversion table provided
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUCS2CharacterToEightbitCharacter(char *dst, const RB_UNICODE_tUCS2 *ucs2Char, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{

	bool success = false;
	int  searchPosition = 0;
	RB_UNICODE_tUCS2 ucs2 = *ucs2Char;

	// Initialize dst with ?.
	// This stays only if the character cannot be found in the conversion table
	*dst = '?';

	if (ucs2 < 0x80)
	{
		// Unicode characters in the range 0x00 ... 0x7f correspond to the ANSI code standard.
		// We just can cast them to char and do not have to search for them in the conversion table
		*dst = (char)ucs2;
		success = true;
	}
	else
	{
		// try to find the UCS2-code in the conversion table
		for (searchPosition = 0; searchPosition < 128 && !success; searchPosition++)
		{
			if (ucs2 == convTable[searchPosition])
			{
				// the conversion table contains only the characters 0x80 ... 0xff therefore we have
				// to add  0x80 to the position found to get the 8-bit character
				*dst = (char)(searchPosition + 0x80);
				success = true;
			}
		}
	}

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertEightbitCharacterToUCS2Character
//--------------------------------------------------------------------------------------------------
//! \brief	Convert an eightbit character to a UCS2 character using the given conversion table
//!
//! \param	dst				Buffer to store converted character
//! \param	eightBitChar	Source character to be converted
//! \param	convTable		Conversion table
//--------------------------------------------------------------------------------------------------
void RB_UNICODE_ConvertEightbitCharacterToUCS2Character(RB_UNICODE_tUCS2 *dst, const char *eightBitChar, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	unsigned char eightBit = (unsigned char) *eightBitChar;

	if (eightBit < 128)
		*dst = (RB_UNICODE_tUCS2)eightBit;
	else
		*dst = convTable[eightBit - 128];
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUCS2StringToEightbitString
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UCS2 string to an eightbit string.
//!
//! Convert a UCS2 string to an eightbit string using the given conversion table. If at least one
//! character of the string is not present in the conversion table, it is replaced by a "?" and this
//! method returns false. The resulting string will always be NULL terminated.
//!
//! \param	dst			Buffer to store converted string
//! \param	ucs2String	Source string to be converted
//! \param	maxsiz		Buffer size of "dst" in bytes
//! \param	convTable	Conversion table
//! \return				True if conversion succeeded, false if at least one character could not be
//!						found in the conversion table or buffer too small.
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUCS2StringToEightbitString(char *dst, const RB_UNICODE_tUCS2 *ucs2String, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	size_t bufPos = 0;
	char tmpChar;
	bool success = true;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	while (*ucs2String != 0)
	{
		if (bufPos >= maxsiz)
		{
			// No more buffer space available, mark conversion as failed
			success = false;
			break;
		}

		if (!RB_UNICODE_ConvertUCS2CharacterToEightbitCharacter(&tmpChar, ucs2String, convTable))
			// Mark conversion as failed, but continue converting the other characters
			success = false;


		// Write converted character, move src forward and increase dst buffer position
		dst[bufPos] = tmpChar;
		bufPos++;
		ucs2String++;
	}

	// Null terminate string (space already reserved)
	dst[bufPos] = '\0';

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertEightbitStringToUCS2String
//--------------------------------------------------------------------------------------------------
//! \brief	Convert an eightbit string to a UCS2 string using the given conversion table
//!
//! The resulting string will always be NULL terminated.
//!
//! \param	dst				Buffer to store converted string
//! \param	eightbitString	Source string to be converted
//! \param	maxsiz			Destination buffer size in characters (RB_UNICODE_tUCS2)
//!							!!! WARNING !!! In general, one RB_UNICODE_tUCS2 character is NOT one byte
//! \param	convTable		Conversion table
//! \return					True if conversion succeeded, false dst buffer too small
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertEightbitStringToUCS2String(RB_UNICODE_tUCS2 *dst, const char *eightbitString, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	size_t bufPos = 0;
	RB_UNICODE_tUCS2 tmpChar;
	bool success = true;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	// Loop until end of eightbit string
	while (*eightbitString)
	{
		if (bufPos >= maxsiz)
		{
			// No more buffer space available, mark conversion as failed
			success = false;
			break;
		}

		// Convert current character
		RB_UNICODE_ConvertEightbitCharacterToUCS2Character(&tmpChar, eightbitString, convTable);


		// Write converted character and move src forward
		dst[bufPos] = tmpChar;
		bufPos++;
		eightbitString++;
	}


	// Null terminate string (space already reserved)
	dst[bufPos] = 0;

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUCS2CharacterToUTF8Character
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UCS2 encoded character to a UTF8 encoded Unicode character
//!
//! \param	utf8	Destination buffer to store UTF8 character
//! \param	ucs2	UCS2 character to be converted
//! \param	maxsiz	utf8 buffer size (converted UTF8 character can be up to 3 bytes)
//! \return			False if utf8 buffer too small to store conversion result, true otherwise
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUCS2CharacterToUTF8Character(RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tUCS2 *ucs2, size_t maxsiz)
{
	RB_UNICODE_tUCS2 ucs2Char = *ucs2;


	if (ucs2Char >= 0x0800)
	{
		// Number of UTF8 bytes required: 3 bytes.
		// UTF8 target format: 1110 xxxx | 10xx xxxx | 10xx xxxx

		if (maxsiz < 3)
			// Remaining buffer too small
			return false;

		*utf8++ = (RB_UNICODE_tUTF8)(((ucs2Char >> 12) & 0x000F) | 0x00E0);
		*utf8++ = (RB_UNICODE_tUTF8)(((ucs2Char >>  6) & 0x003F) | 0x0080);
		*utf8++ = (RB_UNICODE_tUTF8)(((ucs2Char      ) & 0x003F) | 0x0080);
	}
	else if (ucs2Char >= 0x0080)
	{
		// Number of UTF8 bytes required: 2 bytes.
		// UTF8 target format: 110x xxxx | 10xx xxxx

		if (maxsiz < 2)
			// Remaining buffer too small
			return false;

		*utf8++ = (RB_UNICODE_tUTF8)(((ucs2Char >>  6) & 0x001F) | 0x00C0);
		*utf8++ = (RB_UNICODE_tUTF8)(((ucs2Char      ) & 0x003F) | 0x0080);
	}
	else
	{
		// Number of UTF8 bytes required: 1 byte.
		// UTF8 target format: 0xxx xxxx

		if (maxsiz < 1)
			// Remaining buffer too small
			return false;

		*utf8++ = (RB_UNICODE_tUTF8)(ucs2Char & 0x007F);
	}

	return true;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUTF8CharacterToUCS2Character
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UTF8 encoded character to a UCS2 encoded Unicode character
//!
//! This conversion fails if either the UTF8 character is invalid or the UTF8 character is too large
//! to be stored into RB_UNICODE_tUCS2. If the conversion failed, the dst character is set to '?'.
//!
//! \param	dst		Destination buffer to store UCS2 character
//! \param	utf8	UTF8 character to be converted
//! \return			True if conversion succeeded, false if conversion failed (see above)
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUTF8CharacterToUCS2Character(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUTF8 *utf8)
{
	RB_UNICODE_tUTF8 utfByte1 = 0;
	RB_UNICODE_tUTF8 utfByte2 = 0;
	RB_UNICODE_tUTF8 utfByte3 = 0;
	bool success = false;
	size_t utf8ByteLength = 0;


	// Check if arguments are valid
	if (!RB_UNICODE_UTF8charIsValid(utf8))
	{
		*dst = (RB_UNICODE_tUCS2) '?';
		return false;
	}

	// Determine byte length of UTF8 character
	utf8ByteLength = RB_UNICODE_UTF8charlenBytes(utf8);


	if (utf8ByteLength == 1)
	{
		// The UTF-8 character consists of 1 byte 0xxx xxxx  -  we just cast the character to
		// RB_UNICODE_tUCS2
		utfByte1 = *utf8;
		*dst = (RB_UNICODE_tUCS2)utfByte1;
		success = true;
	}
	else if (utf8ByteLength == 2)
	{
		// The UTF-8 character consists of 2 bytes: 110x xxxx | 10xx xxxx
		utfByte1 = *utf8;
		utfByte2 = *(++utf8);
		*dst = ((utfByte1 & 0x1F) << 6) | (utfByte2 & 0x3F);
		success = true;
	}
	else if (utf8ByteLength == 3)
	{
		// The UTF-8 character consists of 3 bytes: 1110x xxx | 10xx xxxx | 10xx xxxx
		utfByte1 = *utf8;
		utfByte2 = *(++utf8);
		utfByte3 = *(++utf8);
		*dst = ((utfByte1 & 0x0F) << 12) | ((utfByte2 & 0x3F) << 6) | (utfByte3 & 0x3F);
		success = true;
	}
	else
	{
		// The UTF-8 character consists of more than 3 bytes and can not be converted to UCS2 format
		// Replace with ?
		*dst = (RB_UNICODE_tUCS2) '?';
		success = false;
	}


	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUCS2StringToUTF8String
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UCS2 encoded string to a UTF8 encoded string
//!
//! The resulting string is always NULL terminated. Furthermore, this function guarantees that only
//! whole characters will be converted, i.e. a UTF8 character is an atomic unit.
//!
//! \param	utf8	Destination buffer for converted string
//! \param	ucs2	UCS2 string to be converted
//! \param	maxsiz	Buffer size of utf8 (in bytes)
//! \return			False if utf8 buffer too small, true otherwise
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUCS2StringToUTF8String(RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tUCS2 *ucs2, size_t maxsiz)
{
	size_t currUTF8Pos = 0;
	size_t utf8CharLen = 0;
	bool success = true;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	while ((*ucs2 != 0) && success)
	{
		if (currUTF8Pos >= maxsiz)
		{
			// Conversion failed because buffer too small
			success = false;
		}
		else
		{
			// Character conversion will be successful only if dst buffer (utf8) is large enough
			success = RB_UNICODE_ConvertUCS2CharacterToUTF8Character(utf8, ucs2, maxsiz - currUTF8Pos);

			// Modify pointers only if RB_UNICODE_ConvertUCS2CharacterToUTF8Character succeeded
			if (success)
			{
				// Determine length of just converted character and move all pointers forward
				// utf8: point to next character to be converted
				// ucs2: point to next free position in buffer
				utf8CharLen = RB_UNICODE_UTF8charlenBytes(utf8);
				utf8 += utf8CharLen;
				currUTF8Pos += utf8CharLen;
				ucs2++;
			}
		}
	}

	// Add NULL termination, use reserved space
	*utf8 = 0;

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUTF8StringToUCS2String
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UTF8 encoded string to a UCS2 encoded string
//!
//! The resulting string is always NULL terminated. If a character could not be converted, it is
//! replaced by '?'.
//!
//! \param	ucs2	Destination buffer for UCS2 string
//! \param	utf8	UTF8 encoded source string
//! \param	maxsiz	Destination buffer size in characters (RB_UNICODE_tUCS2)
//!					!!! WARNING !!! In general, one RB_UNICODE_tUCS2 character is NOT one byte
//! \return			False if conversion failed due to insufficient buffer space or an invalid
//!					UTF8 character
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUTF8StringToUCS2String(RB_UNICODE_tUCS2 *ucs2, const RB_UNICODE_tUTF8 *utf8, size_t maxsiz)
{
	size_t ucs2BufPosition = 0;
	size_t utf8CharLen = 0;
	bool success = true;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	while (*utf8 != 0)
	{
		if (ucs2BufPosition >= maxsiz)
		{
			// Not enough space to fit another UCS2 character
			success = false;
			break;
		}

		// RB_UNICODE_UTF8charIsValid ==> RB_UNICODE_UTF8charlenBytes > 0
		if (!RB_UNICODE_UTF8charIsValid(utf8))
		{
			success = false;
			*ucs2 = (RB_UNICODE_tUCS2) '?';
			ucs2++;
			ucs2BufPosition++;
			break;
		}

		if (!RB_UNICODE_ConvertUTF8CharacterToUCS2Character(ucs2, utf8))
			// Mark conversion as failed
			success = false;


		// Move pointer forward even if conversion above was unsuccessful. This can happen even if
		// the UTF8 src character was valid, e.g. if utf8 character cannot be stored in a UCS2
		// character because the Unicode code point is > U+FFFF ==> try the next character.
		utf8CharLen = RB_UNICODE_UTF8charlenBytes(utf8);
		utf8 += utf8CharLen;
		ucs2++;
		ucs2BufPosition++;
	}

	// Add null termination, use previously reserved space
	*ucs2 = 0;

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUTF8CharacterToEightbitCharacter
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UTF8 character into an eightbit character using the conversion table provided
//!
//! If the character is not found in the conversion table, dst is set to '?'.
//!
//! \param	dst			Destination buffer to store converted string
//! \param	utf8		UTF8 encoded source string
//! \param	convTable	Conversion table
//! \return				True if conversion succeeded, false if character could not be found in
//!						the conversion table.
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUTF8CharacterToEightbitCharacter(char *dst, const RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	// Note: the UTF8 ==> Eightbit conversion is implemented by first converting UTF8 ==> UCS2 and
	// then UCS2 ==> Eightbit. This way, we can use the already existing functions. This conversion
	// will not work if the UTF8 character is not representable by UCS2. However, the
	// RB_UNICODE_tEightbitExtToUnicodeTable currently stores 16 bit Unicode code points and hence this
	// issue is not a problem.

	RB_UNICODE_tUCS2 ucs2Buf;
	bool success;

	success = RB_UNICODE_ConvertUTF8CharacterToUCS2Character(&ucs2Buf, utf8);
	if (success)
		success = RB_UNICODE_ConvertUCS2CharacterToEightbitCharacter(dst, &ucs2Buf, convTable);

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertEightbitCharacterToUTF8Character
//--------------------------------------------------------------------------------------------------
//! \brief	Convert an eightbit character into a UTF8 character using the conversion table provided
//!
//! If there is insufficient buffer space to store the UTF8 character in the target buffer, the
//! buffer will not be modified.
//!
//! \param	dst			Destination buffer to store converted character
//! \param	utf8		Eightbit source character
//! \param	maxsiz		Destination buffer size in bytes
//! \param	convTable	Conversion table
//! \return				True if conversion succeeded, false if dst buffer too small
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertEightbitCharacterToUTF8Character(RB_UNICODE_tUTF8 *dst, const char *eightBitChar, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	RB_UNICODE_tUCS2 ucs2Buf;

	RB_UNICODE_ConvertEightbitCharacterToUCS2Character(&ucs2Buf, eightBitChar, convTable);
	return RB_UNICODE_ConvertUCS2CharacterToUTF8Character(dst, &ucs2Buf, maxsiz);
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertUTF8StringToEightbitString
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a UTF8 string to an eightbit string.
//!
//! Convert a UTF8 string to an eightbit string using the given conversion table. If at least one
//! character of the string is not present in the conversion table, it is replaced by a "?" and this
//! method returns false. The resulting string is always NULL terminated.
//!
//! \param	dst			Buffer to store converted string
//! \param	utf8		Source string to be converted
//! \param	maxsiz		Buffer size of "dst" in bytes
//! \param	convTable	Conversion table
//! \return				True if conversion succeeded, false if at least one character could not be
//!						found in the conversion table.
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertUTF8StringToEightbitString(char *dst, const RB_UNICODE_tUTF8 *utf8, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	size_t bufPos = 0;
	size_t utf8CharLen = 0;
	bool success = true;
	char tmpChar;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	while (*utf8 != 0)
	{
		if (bufPos >= maxsiz)
		{
			// No more buffer space available, mark conversion as failed
			success = false;
			break;
		}

		// RB_UNICODE_UTF8charIsValid ==> RB_UNICODE_UTF8charlenBytes > 0
		if (!RB_UNICODE_UTF8charIsValid(utf8))
		{
			success = false;
			dst[bufPos] = '?';
			bufPos++;
			break;
		}

		// Apply conversion
		if (!RB_UNICODE_ConvertUTF8CharacterToEightbitCharacter(&tmpChar, utf8, convTable))
			// Mark conversion as failed
			success = false;

		// Move pointer forward even if conversion above was unsuccessful
		// ==> try the next character.
		utf8CharLen = RB_UNICODE_UTF8charlenBytes(utf8);
		dst[bufPos] = tmpChar;
		bufPos++;
		utf8 += utf8CharLen;
	}

	// Null terminate string (space already reserved)
	dst[bufPos] = '\0';

	return success;
}


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertEightbitStringToUTF8String
//--------------------------------------------------------------------------------------------------
//! \brief	Convert an eightbit string to a UTF8 string using the given conversion table
//!
//! The resulting string will always be NULL terminated.
//!
//! \param	dst				Buffer to store converted string
//! \param	eightbitString	Source string to be converted
//! \param	maxsiz			Destination buffer size in bytes
//! \param	convTable		Conversion table
//! \return					True if conversion succeeded, false dst buffer too small
//--------------------------------------------------------------------------------------------------
bool RB_UNICODE_ConvertEightbitStringToUTF8String(RB_UNICODE_tUTF8 *dst, const char *eightbitString, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable)
{
	bool success = false;
	size_t utf8CharSize = 0;


	// Abort if maxsiz == 0 to avoid writing anything to the target buffer.
	if (maxsiz == 0)
		return false;

	// Reserve space for null termination only when maxsiz > 0.
	maxsiz--;


	while (*eightbitString)
	{
		// RB_UNICODE_ConvertEightbitCharacterToUTF8Character returns false only when buffer too
		// small to fit the entire UTF8 encoded character into the remaining buffer
		success = RB_UNICODE_ConvertEightbitCharacterToUTF8Character(dst, eightbitString, maxsiz, convTable);
		if (!success)
			break;


		// Move src and dst forward
		utf8CharSize = RB_UNICODE_UTF8charlenBytes(dst);
		dst += utf8CharSize;
		eightbitString++;

		// Ensure that on subtraction there is no underflow
		if (maxsiz > utf8CharSize)
			maxsiz -= utf8CharSize;
		else
			maxsiz = 0;
	}


	// Null terminate string (space already reserved)
	*dst = 0;

	return success;
}


//==================================================================================================
//  D E F A U L T   C O N V E R S I O N   T A B L E S
//==================================================================================================

//! CP437 (MS DOS)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0437 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x00C7,		//!< 0x80
	0x00FC,		//!< 0x81   // <ue> character
	0x00E9,		//!< 0x82
	0x00E2,		//!< 0x83
	0x00E4,		//!< 0x84
	0x00E0,		//!< 0x85
	0x00E5,		//!< 0x86
	0x00E7,		//!< 0x87
	0x00EA,		//!< 0x88
	0x00EB,		//!< 0x89
	0x00E8,		//!< 0x8A
	0x00EF,		//!< 0x8B
	0x00EE,		//!< 0x8C
	0x00EC,		//!< 0x8D
	0x00C4,		//!< 0x8E    // <Ae> character
	0x00C5,		//!< 0x8F

	0x00C9,		//!< 0x90
	0x00E6,		//!< 0x91
	0x00C6,		//!< 0x92
	0x00F4,		//!< 0x93
	0x00F6,		//!< 0x94
	0x00F2,		//!< 0x95
	0x00FB,		//!< 0x96
	0x00F9,		//!< 0x97
	0x00FF,		//!< 0x98
	0x00D6,		//!< 0x99
	0x00DC,		//!< 0x9A
	0x00A2,		//!< 0x9B
	0x00A3,		//!< 0x9C
	0x00A5,		//!< 0x9D
	0x20A7,		//!< 0x9E
	0x0192,		//!< 0x9F

	0x00E1,		//!< 0xA0
	0x00ED,		//!< 0xA1
	0x00F3,		//!< 0xA2
	0x00FA,		//!< 0xA3
	0x00F1,		//!< 0xA4
	0x00D1,		//!< 0xA5
	0x00AA,		//!< 0xA6
	0x00BA,		//!< 0xA7
	0x00BF,		//!< 0xA8
	0x2310,		//!< 0xA9
	0x00AC,		//!< 0xAA
	0x00BD,		//!< 0xAB
	0x00BC,		//!< 0xAC
	0x00A1,		//!< 0xAD
	0x00AB,		//!< 0xAE
	0x00BB,		//!< 0xAF

	0x2591,		//!< 0xB0
	0x2592,		//!< 0xB1
	0x2593,		//!< 0xB2
	0x2502,		//!< 0xB3
	0x2524,		//!< 0xB4
	0x2561,		//!< 0xB5
	0x2562,		//!< 0xB6
	0x2556,		//!< 0xB7
	0x2555,		//!< 0xB8
	0x2563,		//!< 0xB9
	0x2551,		//!< 0xBA
	0x2557,		//!< 0xBB
	0x255D,		//!< 0xBC
	0x255C,		//!< 0xBD
	0x255B,		//!< 0xBE
	0x2510,		//!< 0xBF

	0x2514,		//!< 0xC0
	0x2534,		//!< 0xC1
	0x252C,		//!< 0xC2
	0x251C,		//!< 0xC3
	0x2500,		//!< 0xC4
	0x253C,		//!< 0xC5
	0x255E,		//!< 0xC6
	0x255F,		//!< 0xC7
	0x255A,		//!< 0xC8
	0x2554,		//!< 0xC9
	0x2569,		//!< 0xCA
	0x2566,		//!< 0xCB
	0x2560,		//!< 0xCC
	0x2550,		//!< 0xCD
	0x256C,		//!< 0xCE
	0x2567,		//!< 0xCF

	0x2568,		//!< 0xD0
	0x2564,		//!< 0xD1
	0x2565,		//!< 0xD2
	0x2559,		//!< 0xD3
	0x2558,		//!< 0xD4
	0x2552,		//!< 0xD5
	0x2553,		//!< 0xD6
	0x256B,		//!< 0xD7
	0x256A,		//!< 0xD8
	0x2518,		//!< 0xD9
	0x250C,		//!< 0xDA
	0x2588,		//!< 0xDB
	0x2584,		//!< 0xDC
	0x258C,		//!< 0xDD
	0x2590,		//!< 0xDE
	0x2580,		//!< 0xDF

	0x03B1,		//!< 0xE0
	0x00DF,		//!< 0xE1
	0x0393,		//!< 0xE2
	0x03C0,		//!< 0xE3
	0x03A3,		//!< 0xE4
	0x03C3,		//!< 0xE5
	0x00B5,		//!< 0xE6
	0x03C4,		//!< 0xE7
	0x03A6,		//!< 0xE8
	0x0398,		//!< 0xE9
	0x03A9,		//!< 0xEA
	0x03B4,		//!< 0xEB
	0x221E,		//!< 0xEC
	0x03C6,		//!< 0xED
	0x03B5,		//!< 0xEE
	0x2229,		//!< 0xEF

	0x2261,		//!< 0xF0
	0x00B1,		//!< 0xF1
	0x2265,		//!< 0xF2
	0x2264,		//!< 0xF3
	0x2320,		//!< 0xF4
	0x2321,		//!< 0xF5
	0x00F7,		//!< 0xF6
	0x2248,		//!< 0xF7
	0x00B0,		//!< 0xF8
	0x2219,		//!< 0xF9
	0x00B7,		//!< 0xFA
	0x221A,		//!< 0xFB
	0x207F,		//!< 0xFC
	0x00B2,		//!< 0xFD
	0x25A0,		//!< 0xFE
	0x00A0,		//!< 0xFF
};

//! CP852 (Latin 2)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0852 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x00C7,		//!< 0x80 latin capital letter c with cedilla
	0x00FC,		//!< 0x81 latin small letter u with diaeresis
	0x00E9,		//!< 0x82 latin small letter e with acute
	0x00E2,		//!< 0x83 latin small letter a with circumflex
	0x00E4,		//!< 0x84 latin small letter a with diaeresis
	0x016F,		//!< 0x85 latin small letter u with ring above
	0x0107,		//!< 0x86 latin small letter c with acute
	0x00E7,		//!< 0x87 latin small letter c with cedilla
	0x0142,		//!< 0x88 latin small letter l with stroke
	0x00EB,		//!< 0x89 latin small letter e with diaeresis
	0x0150,		//!< 0x8A latin capital letter o with double acute
	0x0151,		//!< 0x8B latin small letter o with double acute
	0x00EE,		//!< 0x8C latin small letter i with circumflex
	0x0179,		//!< 0x8D latin capital letter z with acute
	0x00C4,		//!< 0x8E latin capital letter a with diaeresis
	0x0106,		//!< 0x8F latin capital letter c with acute

	0x00C9,		//!< 0x90 latin capital letter e with acute
	0x0139,		//!< 0x91 latin capital letter l with acute
	0x013A,		//!< 0x92 latin small letter l with acute
	0x00F4,		//!< 0x93 latin small letter o with circumflex
	0x00F6,		//!< 0x94 latin small letter o with diaeresis
	0x013D,		//!< 0x95 latin capital letter l with caron
	0x013E,		//!< 0x96 latin small letter l with caron
	0x015A,		//!< 0x97 latin capital letter s with acute
	0x015B,		//!< 0x98 latin small letter s with acute
	0x00D6,		//!< 0x99 latin capital letter o with diaeresis
	0x00DC,		//!< 0x9A latin capital letter u with diaeresis
	0x0164,		//!< 0x9B latin capital letter t with caron
	0x0165,		//!< 0x9C latin small letter t with caron
	0x0141,		//!< 0x9D latin capital letter l with stroke
	0x00D7,		//!< 0x9E multiplication sign
	0x010D,		//!< 0x9F latin small letter c with caron

	0x00E1,		//!< 0xA0 latin small letter a with acute
	0x00ED,		//!< 0xA1 latin small letter i with acute
	0x00F3,		//!< 0xA2 latin small letter o with acute
	0x00FA,		//!< 0xA3 latin small letter u with acute
	0x0104,		//!< 0xA4 latin capital letter a with ogonek
	0x0105,		//!< 0xA5 latin small letter a with ogonek
	0x017D,		//!< 0xA6 latin capital letter z with caron
	0x017E,		//!< 0xA7 latin small letter z with caron
	0x0118,		//!< 0xA8 latin capital letter e with ogonek
	0x0119,		//!< 0xA9 latin small letter e with ogonek
	0x00AC,		//!< 0xAA not sign
	0x017A,		//!< 0xAB latin small letter z with acute
	0x010C,		//!< 0xAC latin capital letter c with caron
	0x015F,		//!< 0xAD latin small letter s with cedilla
	0x00AB,		//!< 0xAE left-pointing double angle quotation mark
	0x00BB,		//!< 0xAF right-pointing double angle quotation mark

	0x2591,		//!< 0xB0 light shade
	0x2592,		//!< 0xB1 medium shade
	0x2593,		//!< 0xB2 dark shade
	0x2502,		//!< 0xB3 box drawings light vertical
	0x2524,		//!< 0xB4 box drawings light vertical and left
	0x00C1,		//!< 0xB5 latin capital letter a with acute
	0x00C2,		//!< 0xB6 latin capital letter a with circumflex
	0x011A,		//!< 0xB7 latin capital letter e with caron
	0x015E,		//!< 0xB8 latin capital letter s with cedilla
	0x2563,		//!< 0xB9 box drawings double vertical and left
	0x2551,		//!< 0xBA box drawings double vertical
	0x2557,		//!< 0xBB box drawings double down and left
	0x255D,		//!< 0xBC box drawings double up and left
	0x017B,		//!< 0xBD latin capital letter z with dot above
	0x017C,		//!< 0xBE latin small letter z with dot above
	0x2510,		//!< 0xBF box drawings light down and left

	0x2514,		//!< 0xC0 box drawings light up and right
	0x2534,		//!< 0xC1 box drawings light up and horizontal
	0x252C,		//!< 0xC2 box drawings light down and horizontal
	0x251C,		//!< 0xC3 box drawings light vertical and right
	0x2500,		//!< 0xC4 box drawings light horizontal
	0x253C,		//!< 0xC5 box drawings light vertical and horizontal
	0x0102,		//!< 0xC6 latin capital letter a with breve
	0x0103,		//!< 0xC7 latin small letter a with breve
	0x255A,		//!< 0xC8 box drawings double up and right
	0x2554,		//!< 0xC9 box drawings double down and right
	0x2569,		//!< 0xCA box drawings double up and horizontal
	0x2566,		//!< 0xCB box drawings double down and horizontal
	0x2560,		//!< 0xCC box drawings double vertical and right
	0x2550,		//!< 0xCD box drawings double horizontal
	0x256C,		//!< 0xCE box drawings double vertical and horizontal
	0x00A4,		//!< 0xCF currency sign

	0x0111,		//!< 0xD0 latin small letter d with stroke
	0x0110,		//!< 0xD1 latin capital letter d with stroke
	0x010E,		//!< 0xD2 latin capital letter d with caron
	0x00CB,		//!< 0xD3 latin capital letter e with diaeresis
	0x010F,		//!< 0xD4 latin small letter d with caron
	0x0147,		//!< 0xD5 latin capital letter n with caron
	0x00CD,		//!< 0xD6 latin capital letter i with acute
	0x00CE,		//!< 0xD7 latin capital letter i with circumflex
	0x011B,		//!< 0xD8 latin small letter e with caron
	0x2518,		//!< 0xD9 box drawings light up and left
	0x250C,		//!< 0xDA box drawings light down and right
	0x2588,		//!< 0xDB full block
	0x2584,		//!< 0xDC lower half block
	0x0162,		//!< 0xDD latin capital letter t with cedilla
	0x016E,		//!< 0xDE latin capital letter u with ring above
	0x2580,		//!< 0xDF upper half block

	0x00D3,		//!< 0xE0 latin capital letter o with acute
	0x00DF,		//!< 0xE1 latin small letter sharp s
	0x00D4,		//!< 0xE2 latin capital letter o with circumflex
	0x0143,		//!< 0xE3 latin capital letter n with acute
	0x0144,		//!< 0xE4 latin small letter n with acute
	0x0148,		//!< 0xE5 latin small letter n with caron
	0x0160,		//!< 0xE6 latin capital letter s with caron
	0x0161,		//!< 0xE7 latin small letter s with caron
	0x0154,		//!< 0xE8 latin capital letter r with acute
	0x00DA,		//!< 0xE9 latin capital letter u with acute
	0x0155,		//!< 0xEA latin small letter r with acute
	0x0170,		//!< 0xEB latin capital letter u with double acute
	0x00FD,		//!< 0xEC latin small letter y with acute
	0x00DD,		//!< 0xED latin capital letter y with acute
	0x0163,		//!< 0xEE latin small letter t with cedilla
	0x00B4,		//!< 0xEF acute accent

	0x00AD,		//!< 0xF0 soft hyphen
	0x02DD,		//!< 0xF1 double acute accent
	0x02DB,		//!< 0xF2 ogonek
	0x02C7,		//!< 0xF3 caron
	0x02D8,		//!< 0xF4 breve
	0x00A7,		//!< 0xF5 section sign
	0x00F7,		//!< 0xF6 division sign
	0x00B8,		//!< 0xF7 cedilla
	0x00B0,		//!< 0xF8 degree sign
	0x00A8,		//!< 0xF9 diaeresis
	0x02D9,		//!< 0xFA dot above
	0x0171,		//!< 0xFB latin small letter u with double acute
	0x0158,		//!< 0xFC latin capital letter r with caron
	0x0159,		//!< 0xFD latin small letter r with caron
	0x25A0,		//!< 0xFE black square
	0x00A0,		//!< 0xFF no-break space
};

//! CP860 (Portuguese)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0860 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x00C7,		//!< 0x80 latin capital letter c with cedilla
	0x00FC,		//!< 0x81 latin small letter u with diaeresis
	0x00E9,		//!< 0x82 latin small letter e with acute
	0x00E2,		//!< 0x83 latin small letter a with circumflex
	0x00E3,		//!< 0x84 latin small letter a with tilde
	0x00E0,		//!< 0x85 latin small letter a with grave
	0x00C1,		//!< 0x86 latin capital letter a with acute
	0x00E7,		//!< 0x87 latin small letter c with cedilla
	0x00EA,		//!< 0x88 latin small letter e with circumflex
	0x00CA,		//!< 0x89 latin capital letter e with circumflex
	0x00E8,		//!< 0x8A latin small letter e with grave
	0x00CD,		//!< 0x8B latin capital letter i with acute
	0x00D4,		//!< 0x8C latin capital letter o with circumflex
	0x00EC,		//!< 0x8D latin small letter i with grave
	0x00C3,		//!< 0x8E latin capital letter a with tilde
	0x00C2,		//!< 0x8F latin capital letter a with circumflex

	0x00C9,		//!< 0x90 latin capital letter e with acute
	0x00C0,		//!< 0x91 latin capital letter a with grave
	0x00C8,		//!< 0x92 latin capital letter e with grave
	0x00F4,		//!< 0x93 latin small letter o with circumflex
	0x00F5,		//!< 0x94 latin small letter o with tilde
	0x00F2,		//!< 0x95 latin small letter o with grave
	0x00DA,		//!< 0x96 latin capital letter u with acute
	0x00F9,		//!< 0x97 latin small letter u with grave
	0x00CC,		//!< 0x98 u#latin capital letter i with grave
	0x00D5,		//!< 0x99 latin capital letter o with tilde
	0x00DC,		//!< 0x9A latin capital letter u with diaeresis
	0x00A2,		//!< 0x9B cent sign
	0x00A3,		//!< 0x9C pound sign
	0x00D9,		//!< 0x9D latin capital letter u with grave
	0x20A7,		//!< 0x9E peseta sign
	0x00D3,		//!< 0x9F latin capital letter o with acute

	0x00E1,		//!< 0xA0 latin small letter a with acute
	0x00ED,		//!< 0xA1 latin small letter i with acute
	0x00F3,		//!< 0xA2 latin small letter o with acute
	0x00FA,		//!< 0xA3 latin small letter u with acute
	0x00F1,		//!< 0xA4 latin small letter n with tilde
	0x00D1,		//!< 0xA5 latin capital letter n with tilde
	0x00AA,		//!< 0xA6 feminine ordinal indicator
	0x00BA,		//!< 0xA7 masculine ordinal indicator
	0x00BF,		//!< 0xA8 inverted question mark
	0x00D2,		//!< 0xA9 latin capital letter o with grave
	0x00AC,		//!< 0xAA not sign
	0x00BD,		//!< 0xAB vulgar fraction one half
	0x00BC,		//!< 0xAC vulgar fraction one quarter
	0x00A1,		//!< 0xAD inverted exclamation mark
	0x00AB,		//!< 0xAE left-pointing double angle quotation mark
	0x00BB,		//!< 0xAF right-pointing double angle quotation mark

	0x2591,		//!< 0xB0 light shade
	0x2592,		//!< 0xB1 medium shade
	0x2593,		//!< 0xB2 dark shade
	0x2502,		//!< 0xB3 box drawings light vertical
	0x2524,		//!< 0xB4 box drawings light vertical and left
	0x2561,		//!< 0xB5 box drawings vertical single and left double
	0x2562,		//!< 0xB6 box drawings vertical double and left single
	0x2556,		//!< 0xB7 box drawings down double and left single
	0x2555,		//!< 0xB8 box drawings down single and left double
	0x2563,		//!< 0xB9 box drawings double vertical and left
	0x2551,		//!< 0xBA box drawings double vertical
	0x2557,		//!< 0xBB box drawings double down and left
	0x255D,		//!< 0xBC box drawings double up and left
	0x255C,		//!< 0xBD box drawings up double and left single
	0x255B,		//!< 0xBE box drawings up single and left double
	0x2510,		//!< 0xBF box drawings light down and left

	0x2514,		//!< 0xC0 box drawings light up and right
	0x2534,		//!< 0xC1 box drawings light up and horizontal
	0x252C,		//!< 0xC2 box drawings light down and horizontal
	0x251C,		//!< 0xC3 box drawings light vertical and right
	0x2500,		//!< 0xC4 box drawings light horizontal
	0x253C,		//!< 0xC5 box drawings light vertical and horizontal
	0x255E,		//!< 0xC6 box drawings vertical single and right double
	0x255F,		//!< 0xC7 box drawings vertical double and right single
	0x255A,		//!< 0xC8 box drawings double up and right
	0x2554,		//!< 0xC9 box drawings double down and right
	0x2569,		//!< 0xCA box drawings double up and horizontal
	0x2566,		//!< 0xCB box drawings double down and horizontal
	0x2560,		//!< 0xCC box drawings double vertical and right
	0x2550,		//!< 0xCD box drawings double horizontal
	0x256C,		//!< 0xCE box drawings double vertical and horizontal
	0x2567,		//!< 0xCF box drawings up single and horizontal double

	0x2568,		//!< 0xD0 box drawings up double and horizontal single
	0x2564,		//!< 0xD1 box drawings down single and horizontal double
	0x2565,		//!< 0xD2 box drawings down double and horizontal single
	0x2559,		//!< 0xD3 box drawings up double and right single
	0x2558,		//!< 0xD4 box drawings up single and right double
	0x2552,		//!< 0xD5 box drawings down single and right double
	0x2553,		//!< 0xD6 box drawings down double and right single
	0x256B,		//!< 0xD7 box drawings vertical double and horizontal single
	0x256A,		//!< 0xD8 box drawings vertical single and horizontal double
	0x2518,		//!< 0xD9 box drawings light up and left
	0x250C,		//!< 0xDA box drawings light down and right
	0x2588,		//!< 0xDB full block
	0x2584,		//!< 0xDC lower half block
	0x258C,		//!< 0xDD left half block
	0x2590,		//!< 0xDE right half block
	0x2580,		//!< 0xDF upper half block

	0x03B1,		//!< 0xE0 greek small letter alpha
	0x00DF,		//!< 0xE1 latin small letter sharp s
	0x0393,		//!< 0xE2 greek capital letter gamma
	0x03C0,		//!< 0xE3 greek small letter pi
	0x03A3,		//!< 0xE4 greek capital letter sigma
	0x03C3,		//!< 0xE5 greek small letter sigma
	0x00B5,		//!< 0xE6 micro sign
	0x03C4,		//!< 0xE7 greek small letter tau
	0x03A6,		//!< 0xE8 greek capital letter phi
	0x0398,		//!< 0xE9 greek capital letter theta
	0x03A9,		//!< 0xEA greek capital letter omega
	0x03B4,		//!< 0xEB greek small letter delta
	0x221E,		//!< 0xEC infinity
	0x03C6,		//!< 0xED greek small letter phi
	0x03B5,		//!< 0xEE greek small letter epsilon
	0x2229,		//!< 0xEF intersection

	0x2261,		//!< 0xF0 identical to
	0x00B1,		//!< 0xF1 plus-minus sign
	0x2265,		//!< 0xF2 greater-than or equal to
	0x2264,		//!< 0xF3 less-than or equal to
	0x2320,		//!< 0xF4 top half integral
	0x2321,		//!< 0xF5 bottom half integral
	0x00F7,		//!< 0xF6 division sign
	0x2248,		//!< 0xF7 almost equal to
	0x00B0,		//!< 0xF8 degree sign
	0x2219,		//!< 0xF9 bullet operator
	0x00B7,		//!< 0xFA middle dot
	0x221A,		//!< 0xFB square root
	0x207F,		//!< 0xFC superscript latin small letter n
	0x00B2,		//!< 0xFD superscript two
	0x25A0,		//!< 0xFE black square
	0x00A0,		//!< 0xFF no-break space
};

//! CP1251 (Cyrillic)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP1251 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x0402,		//!< 0x80 cyrillic capital letter dje
	0x0403,		//!< 0x81 cyrillic capital letter gje
	0x201A,		//!< 0x82 single low-9 quotation mark
	0x0453,		//!< 0x83 cyrillic small letter gje
	0x201E,		//!< 0x84 double low-9 quotation mark
	0x2026,		//!< 0x85 horizontal ellipsis
	0x2020,		//!< 0x86 dagger
	0x2021,		//!< 0x87 double dagger
	0x20AC,		//!< 0x88 euro sign
	0x2030,		//!< 0x89 per mille sign
	0x0409,		//!< 0x8A cyrillic capital letter lje
	0x2039,		//!< 0x8B single left-pointing angle quotation mark
	0x040A,		//!< 0x8C cyrillic capital letter nje
	0x040C,		//!< 0x8D cyrillic capital letter kje
	0x040B,		//!< 0x8E cyrillic capital letter tshe
	0x040F,		//!< 0x8F cyrillic capital letter dzhe

	0x0452,		//!< 0x90 cyrillic small letter dje
	0x2018,		//!< 0x91 left single quotation mark
	0x2019,		//!< 0x92 right single quotation mark
	0x201C,		//!< 0x93 left double quotation mark
	0x201D,		//!< 0x94 right double quotation mark
	0x2022,		//!< 0x95 bullet
	0x2013,		//!< 0x96 en dash
	0x2014,		//!< 0x97 em dash
	0x0098,		//!< 0x98 undefined, 1:1 conversion
	0x2122,		//!< 0x99 trade mark sign
	0x0459,		//!< 0x9A cyrillic small letter lje
	0x203A,		//!< 0x9B single right-pointing angle quotation mark
	0x045A,		//!< 0x9C cyrillic small letter nje
	0x045C,		//!< 0x9D cyrillic small letter kje
	0x045B,		//!< 0x9E cyrillic small letter tshe
	0x045F,		//!< 0x9F cyrillic small letter dzhe

	0x00A0,		//!< 0xA0 no-break space
	0x040E,		//!< 0xA1 cyrillic capital letter short u
	0x045E,		//!< 0xA2 cyrillic small letter short u
	0x0408,		//!< 0xA3 cyrillic capital letter je
	0x00A4,		//!< 0xA4 currency sign
	0x0490,		//!< 0xA5 cyrillic capital letter ghe with upturn
	0x00A6,		//!< 0xA6 broken bar
	0x00A7,		//!< 0xA7 section sign
	0x0401,		//!< 0xA8 cyrillic capital letter io
	0x00A9,		//!< 0xA9 copyright sign
	0x0404,		//!< 0xAA cyrillic capital letter ukrainian ie
	0x00AB,		//!< 0xAB left-pointing double angle quotation mark
	0x00AC,		//!< 0xAC not sign
	0x00AD,		//!< 0xAD soft hyphen
	0x00AE,		//!< 0xAE registered sign
	0x0407,		//!< 0xAF cyrillic capital letter yi

	0x00B0,		//!< 0xB0 degree sign
	0x00B1,		//!< 0xB1 plus-minus sign
	0x0406,		//!< 0xB2 cyrillic capital letter byelorussian-ukrainian i
	0x0456,		//!< 0xB3 cyrillic small letter byelorussian-ukrainian i
	0x0491,		//!< 0xB4 cyrillic small letter ghe with upturn
	0x00B5,		//!< 0xB5 micro sign
	0x00B6,		//!< 0xB6 pilcrow sign
	0x00B7,		//!< 0xB7 middle dot
	0x0451,		//!< 0xB8 cyrillic small letter io
	0x2116,		//!< 0xB9 numero sign
	0x0454,		//!< 0xBA cyrillic small letter ukrainian ie
	0x00BB,		//!< 0xBB right-pointing double angle quotation mark
	0x0458,		//!< 0xBC cyrillic small letter je
	0x0405,		//!< 0xBD cyrillic capital letter dze
	0x0455,		//!< 0xBE cyrillic small letter dze
	0x0457,		//!< 0xBF cyrillic small letter yi

	0x0410,		//!< 0xC0 cyrillic capital letter a
	0x0411,		//!< 0xC1 cyrillic capital letter be
	0x0412,		//!< 0xC2 cyrillic capital letter ve
	0x0413,		//!< 0xC3 cyrillic capital letter ghe
	0x0414,		//!< 0xC4 cyrillic capital letter de
	0x0415,		//!< 0xC5 cyrillic capital letter ie
	0x0416,		//!< 0xC6 cyrillic capital letter zhe
	0x0417,		//!< 0xC7 cyrillic capital letter ze
	0x0418,		//!< 0xC8 cyrillic capital letter i
	0x0419,		//!< 0xC9 cyrillic capital letter short i
	0x041A,		//!< 0xCA cyrillic capital letter ka
	0x041B,		//!< 0xCB cyrillic capital letter el
	0x041C,		//!< 0xCC cyrillic capital letter em
	0x041D,		//!< 0xCD cyrillic capital letter en
	0x041E,		//!< 0xCE cyrillic capital letter o
	0x041F,		//!< 0xCF cyrillic capital letter pe

	0x0420,		//!< 0xD0 cyrillic capital letter er
	0x0421,		//!< 0xD1 cyrillic capital letter es
	0x0422,		//!< 0xD2 cyrillic capital letter te
	0x0423,		//!< 0xD3 cyrillic capital letter u
	0x0424,		//!< 0xD4 cyrillic capital letter ef
	0x0425,		//!< 0xD5 cyrillic capital letter ha
	0x0426,		//!< 0xD6 cyrillic capital letter tse
	0x0427,		//!< 0xD7 cyrillic capital letter che
	0x0428,		//!< 0xD8 cyrillic capital letter sha
	0x0429,		//!< 0xD9 cyrillic capital letter shcha
	0x042A,		//!< 0xDA cyrillic capital letter hard sign
	0x042B,		//!< 0xDB cyrillic capital letter yeru
	0x042C,		//!< 0xDC cyrillic capital letter soft sign
	0x042D,		//!< 0xDD cyrillic capital letter e
	0x042E,		//!< 0xDE cyrillic capital letter yu
	0x042F,		//!< 0xDF cyrillic capital letter ya

	0x0430,		//!< 0xE0 cyrillic small letter a
	0x0431,		//!< 0xE1 cyrillic small letter be
	0x0432,		//!< 0xE2 cyrillic small letter ve
	0x0433,		//!< 0xE3 cyrillic small letter ghe
	0x0434,		//!< 0xE4 cyrillic small letter de
	0x0435,		//!< 0xE5 cyrillic small letter ie
	0x0436,		//!< 0xE6 cyrillic small letter zhe
	0x0437,		//!< 0xE7 cyrillic small letter ze
	0x0438,		//!< 0xE8 cyrillic small letter i
	0x0439,		//!< 0xE9 cyrillic small letter short i
	0x043A,		//!< 0xEA cyrillic small letter ka
	0x043B,		//!< 0xEB cyrillic small letter el
	0x043C,		//!< 0xEC cyrillic small letter em
	0x043D,		//!< 0xED cyrillic small letter en
	0x043E,		//!< 0xEE cyrillic small letter o
	0x043F,		//!< 0xEF cyrillic small letter pe

	0x0440,		//!< 0xF0 cyrillic small letter er
	0x0441,		//!< 0xF1 cyrillic small letter es
	0x0442,		//!< 0xF2 cyrillic small letter te
	0x0443,		//!< 0xF3 cyrillic small letter u
	0x0444,		//!< 0xF4 cyrillic small letter ef
	0x0445,		//!< 0xF5 cyrillic small letter ha
	0x0446,		//!< 0xF6 cyrillic small letter tse
	0x0447,		//!< 0xF7 cyrillic small letter che
	0x0448,		//!< 0xF8 cyrillic small letter sha
	0x0449,		//!< 0xF9 cyrillic small letter shcha
	0x044A,		//!< 0xFA cyrillic small letter hard sign
	0x044B,		//!< 0xFB cyrillic small letter yeru
	0x044C,		//!< 0xFC cyrillic small letter soft sign
	0x044D,		//!< 0xFD cyrillic small letter e
	0x044E,		//!< 0xFE cyrillic small letter yu
	0x044F,		//!< 0xFF cyrillic small letter ya
};

//! CP1252 (Latin 1)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP1252 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x20AC,		//!< 0x80 euro sign
	0x0081,		//!< 0x81 undefined, 1:1 conversion
	0x201A,		//!< 0x82 single low-9 quotation mark
	0x0192,		//!< 0x83 latin small letter f with hook
	0x201E,		//!< 0x84 double low-9 quotation mark
	0x2026,		//!< 0x85 horizontal ellipsis
	0x2020,		//!< 0x86 dagger
	0x2021,		//!< 0x87 double dagger
	0x02C6,		//!< 0x88 modifier letter circumflex accent
	0x2030,		//!< 0x89 per mille sign
	0x0160,		//!< 0x8A latin capital letter s with caron
	0x2039,		//!< 0x8B single left-pointing angle quotation mark
	0x0152,		//!< 0x8C latin capital ligature oe
	0x008D,		//!< 0x8D undefined, 1:1 conversion
	0x017D,		//!< 0x8E latin capital letter z with caron
	0x008F,		//!< 0x8F undefined, 1:1 conversion

	0x0090,		//!< 0x90 undefined, 1:1 conversion
	0x2018,		//!< 0x91 left single quotation mark
	0x2019,		//!< 0x92 right single quotation mark
	0x201C,		//!< 0x93 left double quotation mark
	0x201D,		//!< 0x94 right double quotation mark
	0x2022,		//!< 0x95 bullet
	0x2013,		//!< 0x96 en dash
	0x2014,		//!< 0x97 em dash
	0x02DC,		//!< 0x98 small tilde
	0x2122,		//!< 0x99 trade mark sign
	0x0161,		//!< 0x9A latin small letter s with caron
	0x203A,		//!< 0x9B single right-pointing angle quotation mark
	0x0153,		//!< 0x9C latin small ligature oe
	0x009D,		//!< 0x9D undefined, 1:1 conversion
	0x017E,		//!< 0x9E latin small letter z with caron
	0x0178,		//!< 0x9F latin capital letter y with diaeresis

	0x00A0,		//!< 0xA0 no-break space
	0x00A1,		//!< 0xA1 inverted exclamation mark
	0x00A2,		//!< 0xA2 cent sign
	0x00A3,		//!< 0xA3 pound sign
	0x00A4,		//!< 0xA4 currency sign
	0x00A5,		//!< 0xA5 yen sign
	0x00A6,		//!< 0xA6 broken bar
	0x00A7,		//!< 0xA7 section sign
	0x00A8,		//!< 0xA8 diaeresis
	0x00A9,		//!< 0xA9 copyright sign
	0x00AA,		//!< 0xAA feminine ordinal indicator
	0x00AB,		//!< 0xAB left-pointing double angle quotation mark
	0x00AC,		//!< 0xAC not sign
	0x00AD,		//!< 0xAD soft hyphen
	0x00AE,		//!< 0xAE registered sign
	0x00AF,		//!< 0xAF macron

	0x00B0,		//!< 0xB0 degree sign
	0x00B1,		//!< 0xB1 plus-minus sign
	0x00B2,		//!< 0xB2 superscript two
	0x00B3,		//!< 0xB3 superscript three
	0x00B4,		//!< 0xB4 acute accent
	0x00B5,		//!< 0xB5 micro sign
	0x00B6,		//!< 0xB6 pilcrow sign
	0x00B7,		//!< 0xB7 middle dot
	0x00B8,		//!< 0xB8 cedilla
	0x00B9,		//!< 0xB9 superscript one
	0x00BA,		//!< 0xBA masculine ordinal indicator
	0x00BB,		//!< 0xBB right-pointing double angle quotation mark
	0x00BC,		//!< 0xBC vulgar fraction one quarter
	0x00BD,		//!< 0xBD vulgar fraction one half
	0x00BE,		//!< 0xBE vulgar fraction three quarters
	0x00BF,		//!< 0xBF inverted question mark

	0x00C0,		//!< 0xC0 latin capital letter a with grave
	0x00C1,		//!< 0xC1 latin capital letter a with acute
	0x00C2,		//!< 0xC2 latin capital letter a with circumflex
	0x00C3,		//!< 0xC3 latin capital letter a with tilde
	0x00C4,		//!< 0xC4 latin capital letter a with diaeresis
	0x00C5,		//!< 0xC5 latin capital letter a with ring above
	0x00C6,		//!< 0xC6 latin capital letter ae
	0x00C7,		//!< 0xC7 latin capital letter c with cedilla
	0x00C8,		//!< 0xC8 latin capital letter e with grave
	0x00C9,		//!< 0xC9 latin capital letter e with acute
	0x00CA,		//!< 0xCA latin capital letter e with circumflex
	0x00CB,		//!< 0xCB latin capital letter e with diaeresis
	0x00CC,		//!< 0xCC latin capital letter i with grave
	0x00CD,		//!< 0xCD latin capital letter i with acute
	0x00CE,		//!< 0xCE latin capital letter i with circumflex
	0x00CF,		//!< 0xCF latin capital letter i with diaeresis

	0x00D0,		//!< 0xD0 latin capital letter eth
	0x00D1,		//!< 0xD1 latin capital letter n with tilde
	0x00D2,		//!< 0xD2 latin capital letter o with grave
	0x00D3,		//!< 0xD3 latin capital letter o with acute
	0x00D4,		//!< 0xD4 latin capital letter o with circumflex
	0x00D5,		//!< 0xD5 latin capital letter o with tilde
	0x00D6,		//!< 0xD6 latin capital letter o with diaeresis
	0x00D7,		//!< 0xD7 multiplication sign
	0x00D8,		//!< 0xD8 latin capital letter o with stroke
	0x00D9,		//!< 0xD9 latin capital letter u with grave
	0x00DA,		//!< 0xDA latin capital letter u with acute
	0x00DB,		//!< 0xDB latin capital letter u with circumflex
	0x00DC,		//!< 0xDC latin capital letter u with diaeresis
	0x00DD,		//!< 0xDD latin capital letter y with acute
	0x00DE,		//!< 0xDE latin capital letter thorn
	0x00DF,		//!< 0xDF latin small letter sharp s

	0x00E0,		//!< 0xE0 latin small letter a with grave
	0x00E1,		//!< 0xE1 latin small letter a with acute
	0x00E2,		//!< 0xE2 latin small letter a with circumflex
	0x00E3,		//!< 0xE3 latin small letter a with tilde
	0x00E4,		//!< 0xE4 latin small letter a with diaeresis
	0x00E5,		//!< 0xE5 latin small letter a with ring above
	0x00E6,		//!< 0xE6 latin small letter ae
	0x00E7,		//!< 0xE7 latin small letter c with cedilla
	0x00E8,		//!< 0xE8 latin small letter e with grave
	0x00E9,		//!< 0xE9 latin small letter e with acute
	0x00EA,		//!< 0xEA latin small letter e with circumflex
	0x00EB,		//!< 0xEB latin small letter e with diaeresis
	0x00EC,		//!< 0xEC latin small letter i with grave
	0x00ED,		//!< 0xED latin small letter i with acute
	0x00EE,		//!< 0xEE latin small letter i with circumflex
	0x00EF,		//!< 0xEF latin small letter i with diaeresis

	0x00F0,		//!< 0xF0 latin small letter eth
	0x00F1,		//!< 0xF1 latin small letter n with tilde
	0x00F2,		//!< 0xF2 latin small letter o with grave
	0x00F3,		//!< 0xF3 latin small letter o with acute
	0x00F4,		//!< 0xF4 latin small letter o with circumflex
	0x00F5,		//!< 0xF5 latin small letter o with tilde
	0x00F6,		//!< 0xF6 latin small letter o with diaeresis
	0x00F7,		//!< 0xF7 division sign
	0x00F8,		//!< 0xF8 latin small letter o with stroke
	0x00F9,		//!< 0xF9 latin small letter u with grave
	0x00FA,		//!< 0xFA latin small letter u with acute
	0x00FB,		//!< 0xFB latin small letter u with circumflex
	0x00FC,		//!< 0xFC latin small letter u with diaeresis
	0x00FD,		//!< 0xFD latin small letter y with acute
	0x00FE,		//!< 0xFE latin small letter thorn
	0x00FF,		//!< 0xFF latin small letter y with diaeresis
};

//! ISO-8859-1 (Latin 1)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_1 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x0080,		//!< 0x80 <control>
	0x0081,		//!< 0x81 <control>
	0x0082,		//!< 0x82 <control>
	0x0083,		//!< 0x83 <control>
	0x0084,		//!< 0x84 <control>
	0x0085,		//!< 0x85 <control>
	0x0086,		//!< 0x86 <control>
	0x0087,		//!< 0x87 <control>
	0x0088,		//!< 0x88 <control>
	0x0089,		//!< 0x89 <control>
	0x008A,		//!< 0x8A <control>
	0x008B,		//!< 0x8B <control>
	0x008C,		//!< 0x8C <control>
	0x008D,		//!< 0x8D <control>
	0x008E,		//!< 0x8E <control>
	0x008F,		//!< 0x8F <control>

	0x0090,		//!< 0x90 <control>
	0x0091,		//!< 0x91 <control>
	0x0092,		//!< 0x92 <control>
	0x0093,		//!< 0x93 <control>
	0x0094,		//!< 0x94 <control>
	0x0095,		//!< 0x95 <control>
	0x0096,		//!< 0x96 <control>
	0x0097,		//!< 0x97 <control>
	0x0098,		//!< 0x98 <control>
	0x0099,		//!< 0x99 <control>
	0x009A,		//!< 0x9A <control>
	0x009B,		//!< 0x9B <control>
	0x009C,		//!< 0x9C <control>
	0x009D,		//!< 0x9D <control>
	0x009E,		//!< 0x9E <control>
	0x009F,		//!< 0x9F <control>

	0x00A0,		//!< 0xA0 no-break space
	0x00A1,		//!< 0xA1 inverted exclamation mark
	0x00A2,		//!< 0xA2 cent sign
	0x00A3,		//!< 0xA3 pound sign
	0x00A4,		//!< 0xA4 currency sign
	0x00A5,		//!< 0xA5 yen sign
	0x00A6,		//!< 0xA6 broken bar
	0x00A7,		//!< 0xA7 section sign
	0x00A8,		//!< 0xA8 diaeresis
	0x00A9,		//!< 0xA9 copyright sign
	0x00AA,		//!< 0xAA feminine ordinal indicator
	0x00AB,		//!< 0xAB left-pointing double angle quotation mark
	0x00AC,		//!< 0xAC not sign
	0x00AD,		//!< 0xAD soft hyphen
	0x00AE,		//!< 0xAE registered sign
	0x00AF,		//!< 0xAF macron

	0x00B0,		//!< 0xB0 degree sign
	0x00B1,		//!< 0xB1 plus-minus sign
	0x00B2,		//!< 0xB2 superscript two
	0x00B3,		//!< 0xB3 superscript three
	0x00B4,		//!< 0xB4 acute accent
	0x00B5,		//!< 0xB5 micro sign
	0x00B6,		//!< 0xB6 pilcrow sign
	0x00B7,		//!< 0xB7 middle dot
	0x00B8,		//!< 0xB8 cedilla
	0x00B9,		//!< 0xB9 superscript one
	0x00BA,		//!< 0xBA masculine ordinal indicator
	0x00BB,		//!< 0xBB right-pointing double angle quotation mark
	0x00BC,		//!< 0xBC vulgar fraction one quarter
	0x00BD,		//!< 0xBD vulgar fraction one half
	0x00BE,		//!< 0xBE vulgar fraction three quarters
	0x00BF,		//!< 0xBF inverted question mark

	0x00C0,		//!< 0xC0 latin capital letter a with grave
	0x00C1,		//!< 0xC1 latin capital letter a with acute
	0x00C2,		//!< 0xC2 latin capital letter a with circumflex
	0x00C3,		//!< 0xC3 latin capital letter a with tilde
	0x00C4,		//!< 0xC4 latin capital letter a with diaeresis
	0x00C5,		//!< 0xC5 latin capital letter a with ring above
	0x00C6,		//!< 0xC6 latin capital letter ae
	0x00C7,		//!< 0xC7 latin capital letter c with cedilla
	0x00C8,		//!< 0xC8 latin capital letter e with grave
	0x00C9,		//!< 0xC9 latin capital letter e with acute
	0x00CA,		//!< 0xCA latin capital letter e with circumflex
	0x00CB,		//!< 0xCB latin capital letter e with diaeresis
	0x00CC,		//!< 0xCC latin capital letter i with grave
	0x00CD,		//!< 0xCD latin capital letter i with acute
	0x00CE,		//!< 0xCE latin capital letter i with circumflex
	0x00CF,		//!< 0xCF latin capital letter i with diaeresis

	0x00D0,		//!< 0xD0 latin capital letter eth
	0x00D1,		//!< 0xD1 latin capital letter n with tilde
	0x00D2,		//!< 0xD2 latin capital letter o with grave
	0x00D3,		//!< 0xD3 latin capital letter o with acute
	0x00D4,		//!< 0xD4 latin capital letter o with circumflex
	0x00D5,		//!< 0xD5 latin capital letter o with tilde
	0x00D6,		//!< 0xD6 latin capital letter o with diaeresis
	0x00D7,		//!< 0xD7 multiplication sign
	0x00D8,		//!< 0xD8 latin capital letter o with stroke
	0x00D9,		//!< 0xD9 latin capital letter u with grave
	0x00DA,		//!< 0xDA latin capital letter u with acute
	0x00DB,		//!< 0xDB latin capital letter u with circumflex
	0x00DC,		//!< 0xDC latin capital letter u with diaeresis
	0x00DD,		//!< 0xDD latin capital letter y with acute
	0x00DE,		//!< 0xDE latin capital letter thorn
	0x00DF,		//!< 0xDF latin small letter sharp s

	0x00E0,		//!< 0xE0 latin small letter a with grave
	0x00E1,		//!< 0xE1 latin small letter a with acute
	0x00E2,		//!< 0xE2 latin small letter a with circumflex
	0x00E3,		//!< 0xE3 latin small letter a with tilde
	0x00E4,		//!< 0xE4 latin small letter a with diaeresis
	0x00E5,		//!< 0xE5 latin small letter a with ring above
	0x00E6,		//!< 0xE6 latin small letter ae
	0x00E7,		//!< 0xE7 latin small letter c with cedilla
	0x00E8,		//!< 0xE8 latin small letter e with grave
	0x00E9,		//!< 0xE9 latin small letter e with acute
	0x00EA,		//!< 0xEA latin small letter e with circumflex
	0x00EB,		//!< 0xEB latin small letter e with diaeresis
	0x00EC,		//!< 0xEC latin small letter i with grave
	0x00ED,		//!< 0xED latin small letter i with acute
	0x00EE,		//!< 0xEE latin small letter i with circumflex
	0x00EF,		//!< 0xEF latin small letter i with diaeresis

	0x00F0,		//!< 0xF0 latin small letter eth
	0x00F1,		//!< 0xF1 latin small letter n with tilde
	0x00F2,		//!< 0xF2 latin small letter o with grave
	0x00F3,		//!< 0xF3 latin small letter o with acute
	0x00F4,		//!< 0xF4 latin small letter o with circumflex
	0x00F5,		//!< 0xF5 latin small letter o with tilde
	0x00F6,		//!< 0xF6 latin small letter o with diaeresis
	0x00F7,		//!< 0xF7 division sign
	0x00F8,		//!< 0xF8 latin small letter o with stroke
	0x00F9,		//!< 0xF9 latin small letter u with grave
	0x00FA,		//!< 0xFA latin small letter u with acute
	0x00FB,		//!< 0xFB latin small letter u with circumflex
	0x00FC,		//!< 0xFC latin small letter u with diaeresis
	0x00FD,		//!< 0xFD latin small letter y with acute
	0x00FE,		//!< 0xFE latin small letter thorn
	0x00FF,		//!< 0xFF latin small letter y with diaeresis
};

//! ISO-8859-2 (Latin 2)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_2 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x0080,		//!< 0x80 <control>
	0x0081,		//!< 0x81 <control>
	0x0082,		//!< 0x82 <control>
	0x0083,		//!< 0x83 <control>
	0x0084,		//!< 0x84 <control>
	0x0085,		//!< 0x85 <control>
	0x0086,		//!< 0x86 <control>
	0x0087,		//!< 0x87 <control>
	0x0088,		//!< 0x88 <control>
	0x0089,		//!< 0x89 <control>
	0x008A,		//!< 0x8A <control>
	0x008B,		//!< 0x8B <control>
	0x008C,		//!< 0x8C <control>
	0x008D,		//!< 0x8D <control>
	0x008E,		//!< 0x8E <control>
	0x008F,		//!< 0x8F <control>

	0x0090,		//!< 0x90 <control>
	0x0091,		//!< 0x91 <control>
	0x0092,		//!< 0x92 <control>
	0x0093,		//!< 0x93 <control>
	0x0094,		//!< 0x94 <control>
	0x0095,		//!< 0x95 <control>
	0x0096,		//!< 0x96 <control>
	0x0097,		//!< 0x97 <control>
	0x0098,		//!< 0x98 <control>
	0x0099,		//!< 0x99 <control>
	0x009A,		//!< 0x9A <control>
	0x009B,		//!< 0x9B <control>
	0x009C,		//!< 0x9C <control>
	0x009D,		//!< 0x9D <control>
	0x009E,		//!< 0x9E <control>
	0x009F,		//!< 0x9F <control>

	0x00A0,		//!< 0xA0 no-break space
	0x0104,		//!< 0xA1 latin capital letter a with ogonek
	0x02D8,		//!< 0xA2 breve
	0x0141,		//!< 0xA3 latin capital letter l with stroke
	0x00A4,		//!< 0xA4 currency sign
	0x013D,		//!< 0xA5 latin capital letter l with caron
	0x015A,		//!< 0xA6 latin capital letter s with acute
	0x00A7,		//!< 0xA7 section sign
	0x00A8,		//!< 0xA8 diaeresis
	0x0160,		//!< 0xA9 latin capital letter s with caron
	0x015E,		//!< 0xAA latin capital letter s with cedilla
	0x0164,		//!< 0xAB latin capital letter t with caron
	0x0179,		//!< 0xAC latin capital letter z with acute
	0x00AD,		//!< 0xAD soft hyphen
	0x017D,		//!< 0xAE latin capital letter z with caron
	0x017B,		//!< 0xAF latin capital letter z with dot above

	0x00B0,		//!< 0xB0 degree sign
	0x0105,		//!< 0xB1 latin small letter a with ogonek
	0x02DB,		//!< 0xB2 ogonek
	0x0142,		//!< 0xB3 latin small letter l with stroke
	0x00B4,		//!< 0xB4 acute accent
	0x013E,		//!< 0xB5 latin small letter l with caron
	0x015B,		//!< 0xB6 latin small letter s with acute
	0x02C7,		//!< 0xB7 caron
	0x00B8,		//!< 0xB8 cedilla
	0x0161,		//!< 0xB9 latin small letter s with caron
	0x015F,		//!< 0xBA latin small letter s with cedilla
	0x0165,		//!< 0xBB latin small letter t with caron
	0x017A,		//!< 0xBC latin small letter z with acute
	0x02DD,		//!< 0xBD double acute accent
	0x017E,		//!< 0xBE latin small letter z with caron
	0x017C,		//!< 0xBF latin small letter z with dot above

	0x0154,		//!< 0xC0 latin capital letter r with acute
	0x00C1,		//!< 0xC1 latin capital letter a with acute
	0x00C2,		//!< 0xC2 latin capital letter a with circumflex
	0x0102,		//!< 0xC3 latin capital letter a with breve
	0x00C4,		//!< 0xC4 latin capital letter a with diaeresis
	0x0139,		//!< 0xC5 latin capital letter l with acute
	0x0106,		//!< 0xC6 latin capital letter c with acute
	0x00C7,		//!< 0xC7 latin capital letter c with cedilla
	0x010C,		//!< 0xC8 latin capital letter c with caron
	0x00C9,		//!< 0xC9 latin capital letter e with acute
	0x0118,		//!< 0xCA latin capital letter e with ogonek
	0x00CB,		//!< 0xCB latin capital letter e with diaeresis
	0x011A,		//!< 0xCC latin capital letter e with caron
	0x00CD,		//!< 0xCD latin capital letter i with acute
	0x00CE,		//!< 0xCE latin capital letter i with circumflex
	0x010E,		//!< 0xCF latin capital letter d with caron

	0x0110,		//!< 0xD0 latin capital letter d with stroke
	0x0143,		//!< 0xD1 latin capital letter n with acute
	0x0147,		//!< 0xD2 latin capital letter n with caron
	0x00D3,		//!< 0xD3 latin capital letter o with acute
	0x00D4,		//!< 0xD4 latin capital letter o with circumflex
	0x0150,		//!< 0xD5 latin capital letter o with double acute
	0x00D6,		//!< 0xD6 latin capital letter o with diaeresis
	0x00D7,		//!< 0xD7 multiplication sign
	0x0158,		//!< 0xD8 latin capital letter r with caron
	0x016E,		//!< 0xD9 latin capital letter u with ring above
	0x00DA,		//!< 0xDA latin capital letter u with acute
	0x0170,		//!< 0xDB latin capital letter u with double acute
	0x00DC,		//!< 0xDC latin capital letter u with diaeresis
	0x00DD,		//!< 0xDD latin capital letter y with acute
	0x0162,		//!< 0xDE latin capital letter t with cedilla
	0x00DF,		//!< 0xDF latin small letter sharp s

	0x0155,		//!< 0xE0 latin small letter r with acute
	0x00E1,		//!< 0xE1 latin small letter a with acute
	0x00E2,		//!< 0xE2 latin small letter a with circumflex
	0x0103,		//!< 0xE3 latin small letter a with breve
	0x00E4,		//!< 0xE4 latin small letter a with diaeresis
	0x013A,		//!< 0xE5 latin small letter l with acute
	0x0107,		//!< 0xE6 latin small letter c with acute
	0x00E7,		//!< 0xE7 latin small letter c with cedilla
	0x010D,		//!< 0xE8 latin small letter c with caron
	0x00E9,		//!< 0xE9 latin small letter e with acute
	0x0119,		//!< 0xEA latin small letter e with ogonek
	0x00EB,		//!< 0xEB latin small letter e with diaeresis
	0x011B,		//!< 0xEC latin small letter e with caron
	0x00ED,		//!< 0xED latin small letter i with acute
	0x00EE,		//!< 0xEE latin small letter i with circumflex
	0x010F,		//!< 0xEF latin small letter d with caron

	0x0111,		//!< 0xF0 latin small letter d with stroke
	0x0144,		//!< 0xF1 latin small letter n with acute
	0x0148,		//!< 0xF2 latin small letter n with caron
	0x00F3,		//!< 0xF3 latin small letter o with acute
	0x00F4,		//!< 0xF4 latin small letter o with circumflex
	0x0151,		//!< 0xF5 latin small letter o with double acute
	0x00F6,		//!< 0xF6 latin small letter o with diaeresis
	0x00F7,		//!< 0xF7 division sign
	0x0159,		//!< 0xF8 latin small letter r with caron
	0x016F,		//!< 0xF9 latin small letter u with ring above
	0x00FA,		//!< 0xFA latin small letter u with acute
	0x0171,		//!< 0xFB latin small letter u with double acute
	0x00FC,		//!< 0xFC latin small letter u with diaeresis
	0x00FD,		//!< 0xFD latin small letter y with acute
	0x0163,		//!< 0xFE latin small letter t with cedilla
	0x02D9,		//!< 0xFF dot above
};

//! ISO-8859-5 (Latin/Cyrillic)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_5 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x0080,		//!< 0x80 undefined, replace with <space>
	0x0081,		//!< 0x81 undefined, replace with <space>
	0x0082,		//!< 0x82 undefined, replace with <space>
	0x0083,		//!< 0x83 undefined, replace with <space>
	0x0084,		//!< 0x84 undefined, replace with <space>
	0x0085,		//!< 0x85 undefined, replace with <space>
	0x0086,		//!< 0x86 undefined, replace with <space>
	0x0087,		//!< 0x87 undefined, replace with <space>
	0x0088,		//!< 0x88 undefined, replace with <space>
	0x0089,		//!< 0x89 undefined, replace with <space>
	0x008A,		//!< 0x8A undefined, replace with <space>
	0x008B,		//!< 0x8B undefined, replace with <space>
	0x008C,		//!< 0x8C undefined, replace with <space>
	0x008D,		//!< 0x8D undefined, replace with <space>
	0x008E,		//!< 0x8E undefined, replace with <space>
	0x008F,		//!< 0x8F undefined, replace with <space>

	0x0090,		//!< 0x90 undefined, replace with <space>
	0x0091,		//!< 0x91 undefined, replace with <space>
	0x0092,		//!< 0x92 undefined, replace with <space>
	0x0093,		//!< 0x93 undefined, replace with <space>
	0x0094,		//!< 0x94 undefined, replace with <space>
	0x0095,		//!< 0x95 undefined, replace with <space>
	0x0096,		//!< 0x96 undefined, replace with <space>
	0x0097,		//!< 0x97 undefined, replace with <space>
	0x0098,		//!< 0x98 undefined, replace with <space>
	0x0099,		//!< 0x99 undefined, replace with <space>
	0x009A,		//!< 0x9A undefined, replace with <space>
	0x009B,		//!< 0x9B undefined, replace with <space>
	0x009C,		//!< 0x9C undefined, replace with <space>
	0x009D,		//!< 0x9D undefined, replace with <space>
	0x009E,		//!< 0x9E undefined, replace with <space>
	0x009F,		//!< 0x9F undefined, replace with <space>

	0x00A0,		//!< 0xA0 no-break space
	0x0401,		//!< 0xA1 cyrillic capital letter io
	0x0402,		//!< 0xA2 cyrillic capital letter dje
	0x0403,		//!< 0xA3 cyrillic capital letter gje
	0x0404,		//!< 0xA4 cyrillic capital letter ukrainian ie
	0x0405,		//!< 0xA5 cyrillic capital letter dze
	0x0406,		//!< 0xA6 cyrillic capital letter byelorussian-ukrainian i
	0x0407,		//!< 0xA7 cyrillic capital letter yi
	0x0408,		//!< 0xA8 cyrillic capital letter je
	0x0409,		//!< 0xA9 cyrillic capital letter lje
	0x040A,		//!< 0xAA cyrillic capital letter nje
	0x040B,		//!< 0xAB cyrillic capital letter tshe
	0x040C,		//!< 0xAC cyrillic capital letter kje
	0x00AD,		//!< 0xAD soft hyphen
	0x040E,		//!< 0xAE cyrillic capital letter short u
	0x040F,		//!< 0xAF cyrillic capital letter dzhe

	0x0410,		//!< 0xB0 cyrillic capital letter a
	0x0411,		//!< 0xB1 cyrillic capital letter be
	0x0412,		//!< 0xB2 cyrillic capital letter ve
	0x0413,		//!< 0xB3 cyrillic capital letter ghe
	0x0414,		//!< 0xB4 cyrillic capital letter de
	0x0415,		//!< 0xB5 cyrillic capital letter ie
	0x0416,		//!< 0xB6 cyrillic capital letter zhe
	0x0417,		//!< 0xB7 cyrillic capital letter ze
	0x0418,		//!< 0xB8 cyrillic capital letter i
	0x0419,		//!< 0xB9 cyrillic capital letter short i
	0x041A,		//!< 0xBA cyrillic capital letter ka
	0x041B,		//!< 0xBB cyrillic capital letter el
	0x041C,		//!< 0xBC cyrillic capital letter em
	0x041D,		//!< 0xBD cyrillic capital letter en
	0x041E,		//!< 0xBE cyrillic capital letter o
	0x041F,		//!< 0xBF cyrillic capital letter pe

	0x0420,		//!< 0xC0 cyrillic capital letter er
	0x0421,		//!< 0xC1 cyrillic capital letter es
	0x0422,		//!< 0xC2 cyrillic capital letter te
	0x0423,		//!< 0xC3 cyrillic capital letter u
	0x0424,		//!< 0xC4 cyrillic capital letter ef
	0x0425,		//!< 0xC5 cyrillic capital letter ha
	0x0426,		//!< 0xC6 cyrillic capital letter tse
	0x0427,		//!< 0xC7 cyrillic capital letter che
	0x0428,		//!< 0xC8 cyrillic capital letter sha
	0x0429,		//!< 0xC9 cyrillic capital letter shcha
	0x042A,		//!< 0xCA cyrillic capital letter hard sign
	0x042B,		//!< 0xCB cyrillic capital letter yeru
	0x042C,		//!< 0xCC cyrillic capital letter soft sign
	0x042D,		//!< 0xCD cyrillic capital letter e
	0x042E,		//!< 0xCE cyrillic capital letter yu
	0x042F,		//!< 0xCF cyrillic capital letter ya

	0x0430,		//!< 0xD0 cyrillic small letter a
	0x0431,		//!< 0xD1 cyrillic small letter be
	0x0432,		//!< 0xD2 cyrillic small letter ve
	0x0433,		//!< 0xD3 cyrillic small letter ghe
	0x0434,		//!< 0xD4 cyrillic small letter de
	0x0435,		//!< 0xD5 cyrillic small letter ie
	0x0436,		//!< 0xD6 cyrillic small letter zhe
	0x0437,		//!< 0xD7 cyrillic small letter ze
	0x0438,		//!< 0xD8 cyrillic small letter i
	0x0439,		//!< 0xD9 cyrillic small letter short i
	0x043A,		//!< 0xDA cyrillic small letter ka
	0x043B,		//!< 0xDB cyrillic small letter el
	0x043C,		//!< 0xDC cyrillic small letter em
	0x043D,		//!< 0xDD cyrillic small letter en
	0x043E,		//!< 0xDE cyrillic small letter o
	0x043F,		//!< 0xDF cyrillic small letter pe

	0x0440,		//!< 0xE0 cyrillic small letter er
	0x0441,		//!< 0xE1 cyrillic small letter es
	0x0442,		//!< 0xE2 cyrillic small letter te
	0x0443,		//!< 0xE3 cyrillic small letter u
	0x0444,		//!< 0xE4 cyrillic small letter ef
	0x0445,		//!< 0xE5 cyrillic small letter ha
	0x0446,		//!< 0xE6 cyrillic small letter tse
	0x0447,		//!< 0xE7 cyrillic small letter che
	0x0448,		//!< 0xE8 cyrillic small letter sha
	0x0449,		//!< 0xE9 cyrillic small letter shcha
	0x044A,		//!< 0xEA cyrillic small letter hard sign
	0x044B,		//!< 0xEB cyrillic small letter yeru
	0x044C,		//!< 0xEC cyrillic small letter soft sign
	0x044D,		//!< 0xED cyrillic small letter e
	0x044E,		//!< 0xEE cyrillic small letter yu
	0x044F,		//!< 0xEF cyrillic small letter ya

	0x2116,		//!< 0xF0 numero sign
	0x0451,		//!< 0xF1 cyrillic small letter io
	0x0452,		//!< 0xF2 cyrillic small letter dje
	0x0453,		//!< 0xF3 cyrillic small letter gje
	0x0454,		//!< 0xF4 cyrillic small letter ukrainian ie
	0x0455,		//!< 0xF5 cyrillic small letter dze
	0x0456,		//!< 0xF6 cyrillic small letter byelorussian-ukrainian i
	0x0457,		//!< 0xF7 cyrillic small letter yi
	0x0458,		//!< 0xF8 cyrillic small letter je
	0x0459,		//!< 0xF9 cyrillic small letter lje
	0x045A,		//!< 0xFA cyrillic small letter nje
	0x045B,		//!< 0xFB cyrillic small letter tshe
	0x045C,		//!< 0xFC cyrillic small letter kje
	0x00A7,		//!< 0xFD section sign
	0x045E,		//!< 0xFE cyrillic small letter short u
	0x045F,		//!< 0xFF cyrillic small letter dzhe
};

//! ISO-8859-15 (Latin 9)
const RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_15 =
{
//	Unicode:	8-bit Code:
//	--------	-----------
//
	0x0080,		//!< 0x80 <control>
	0x0081,		//!< 0x81 <control>
	0x0082,		//!< 0x82 <control>
	0x0083,		//!< 0x83 <control>
	0x0084,		//!< 0x84 <control>
	0x0085,		//!< 0x85 <control>
	0x0086,		//!< 0x86 <control>
	0x0087,		//!< 0x87 <control>
	0x0088,		//!< 0x88 <control>
	0x0089,		//!< 0x89 <control>
	0x008A,		//!< 0x8A <control>
	0x008B,		//!< 0x8B <control>
	0x008C,		//!< 0x8C <control>
	0x008D,		//!< 0x8D <control>
	0x008E,		//!< 0x8E <control>
	0x008F,		//!< 0x8F <control>

	0x0090,		//!< 0x90 <control>
	0x0091,		//!< 0x91 <control>
	0x0092,		//!< 0x92 <control>
	0x0093,		//!< 0x93 <control>
	0x0094,		//!< 0x94 <control>
	0x0095,		//!< 0x95 <control>
	0x0096,		//!< 0x96 <control>
	0x0097,		//!< 0x97 <control>
	0x0098,		//!< 0x98 <control>
	0x0099,		//!< 0x99 <control>
	0x009A,		//!< 0x9A <control>
	0x009B,		//!< 0x9B <control>
	0x009C,		//!< 0x9C <control>
	0x009D,		//!< 0x9D <control>
	0x009E,		//!< 0x9E <control>
	0x009F,		//!< 0x9F <control>

	0x00A0,		//!< 0xA0 no-break space
	0x00A1,		//!< 0xA1 inverted exclamation mark
	0x00A2,		//!< 0xA2 cent sign
	0x00A3,		//!< 0xA3 pound sign
	0x20AC,		//!< 0xA4 euro sign
	0x00A5,		//!< 0xA5 yen sign
	0x0160,		//!< 0xA6 latin capital letter s with caron
	0x00A7,		//!< 0xA7 section sign
	0x0161,		//!< 0xA8 s with caron
	0x00A9,		//!< 0xA9 copyright sign
	0x00AA,		//!< 0xAA feminine ordinal indicator
	0x00AB,		//!< 0xAB left-pointing double angle quotation mark
	0x00AC,		//!< 0xAC not sign
	0x00AD,		//!< 0xAD soft hyphen
	0x00AE,		//!< 0xAE registered sign
	0x00AF,		//!< 0xAF macron

	0x00B0,		//!< 0xB0 degree sign
	0x00B1,		//!< 0xB1 plus-minus sign
	0x00B2,		//!< 0xB2 superscript two
	0x00B3,		//!< 0xB3 superscript three
	0x017D,		//!< 0xB4 latin capital letter z with caron
	0x00B5,		//!< 0xB5 micro sign
	0x00B6,		//!< 0xB6 pilcrow sign
	0x00B7,		//!< 0xB7 middle dot
	0x017E,		//!< 0xB8 z with caron
	0x00B9,		//!< 0xB9 superscript one
	0x00BA,		//!< 0xBA masculine ordinal indicator
	0x00BB,		//!< 0xBB right-pointing double angle quotation mark
	0x0152,		//!< 0xBC latin capital letter ce
	0x0153,		//!< 0xBD latin letter ce
	0x0178,		//!< 0xBE latin capital letter y with diaeresis
	0x00BF,		//!< 0xBF inverted question mark

	0x00C0,		//!< 0xC0 latin capital letter a with grave
	0x00C1,		//!< 0xC1 latin capital letter a with acute
	0x00C2,		//!< 0xC2 latin capital letter a with circumflex
	0x00C3,		//!< 0xC3 latin capital letter a with tilde
	0x00C4,		//!< 0xC4 latin capital letter a with diaeresis
	0x00C5,		//!< 0xC5 latin capital letter a with ring above
	0x00C6,		//!< 0xC6 latin capital letter ae
	0x00C7,		//!< 0xC7 latin capital letter c with cedilla
	0x00C8,		//!< 0xC8 latin capital letter e with grave
	0x00C9,		//!< 0xC9 latin capital letter e with acute
	0x00CA,		//!< 0xCA latin capital letter e with circumflex
	0x00CB,		//!< 0xCB latin capital letter e with diaeresis
	0x00CC,		//!< 0xCC latin capital letter i with grave
	0x00CD,		//!< 0xCD latin capital letter i with acute
	0x00CE,		//!< 0xCE latin capital letter i with circumflex
	0x00CF,		//!< 0xCF latin capital letter i with diaeresis

	0x00D0,		//!< 0xD0 latin capital letter eth
	0x00D1,		//!< 0xD1 latin capital letter n with tilde
	0x00D2,		//!< 0xD2 latin capital letter o with grave
	0x00D3,		//!< 0xD3 latin capital letter o with acute
	0x00D4,		//!< 0xD4 latin capital letter o with circumflex
	0x00D5,		//!< 0xD5 latin capital letter o with tilde
	0x00D6,		//!< 0xD6 latin capital letter o with diaeresis
	0x00D7,		//!< 0xD7 multiplication sign
	0x00D8,		//!< 0xD8 latin capital letter o with stroke
	0x00D9,		//!< 0xD9 latin capital letter u with grave
	0x00DA,		//!< 0xDA latin capital letter u with acute
	0x00DB,		//!< 0xDB latin capital letter u with circumflex
	0x00DC,		//!< 0xDC latin capital letter u with diaeresis
	0x00DD,		//!< 0xDD latin capital letter y with acute
	0x00DE,		//!< 0xDE latin capital letter thorn
	0x00DF,		//!< 0xDF latin small letter sharp s

	0x00E0,		//!< 0xE0 latin small letter a with grave
	0x00E1,		//!< 0xE1 latin small letter a with acute
	0x00E2,		//!< 0xE2 latin small letter a with circumflex
	0x00E3,		//!< 0xE3 latin small letter a with tilde
	0x00E4,		//!< 0xE4 latin small letter a with diaeresis
	0x00E5,		//!< 0xE5 latin small letter a with ring above
	0x00E6,		//!< 0xE6 latin small letter ae
	0x00E7,		//!< 0xE7 latin small letter c with cedilla
	0x00E8,		//!< 0xE8 latin small letter e with grave
	0x00E9,		//!< 0xE9 latin small letter e with acute
	0x00EA,		//!< 0xEA latin small letter e with circumflex
	0x00EB,		//!< 0xEB latin small letter e with diaeresis
	0x00EC,		//!< 0xEC latin small letter i with grave
	0x00ED,		//!< 0xED latin small letter i with acute
	0x00EE,		//!< 0xEE latin small letter i with circumflex
	0x00EF,		//!< 0xEF latin small letter i with diaeresis

	0x00F0,		//!< 0xF0 latin small letter eth
	0x00F1,		//!< 0xF1 latin small letter n with tilde
	0x00F2,		//!< 0xF2 latin small letter o with grave
	0x00F3,		//!< 0xF3 latin small letter o with acute
	0x00F4,		//!< 0xF4 latin small letter o with circumflex
	0x00F5,		//!< 0xF5 latin small letter o with tilde
	0x00F6,		//!< 0xF6 latin small letter o with diaeresis
	0x00F7,		//!< 0xF7 division sign
	0x00F8,		//!< 0xF8 latin small letter o with stroke
	0x00F9,		//!< 0xF9 latin small letter u with grave
	0x00FA,		//!< 0xFA latin small letter u with acute
	0x00FB,		//!< 0xFB latin small letter u with circumflex
	0x00FC,		//!< 0xFC latin small letter u with diaeresis
	0x00FD,		//!< 0xFD latin small letter y with acute
	0x00FE,		//!< 0xFE latin small letter thorn
	0x00FF,		//!< 0xFF latin small letter y with diaeresis
};


//--------------------------------------------------------------------------------------------------
#endif // RB_CONFIG_USE_UNICODE
