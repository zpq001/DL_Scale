//==================================================================================================
//                                            Rainbow
//==================================================================================================
//! \file		util/RB_Unicode.h
//! \ingroup	util
//! \brief		Support for UTF8 string handling and conversions to UCS2 and 8-bit encodings
//!
//!
//! This module offers a set of standard string processing operations, for example length
//! determination or string concatenation. Furthermore, it offers functions to convert between
//! eightbit, UTF8 and UCS2 encodings.
//!
//!
//! \par Background knowledge
//!
//! Eightbit encoding
//! A string is stored as a set of characters (char*), which is the usual string representation
//! in C. The first 7 bits are defined in the ANSI standard (ASCII table). For characters with value
//! 0x80 and higher, the translation to a character depends on the code page.
//!
//! Unicode UCS2 encoding
//! The Unicode code points are stored as 16 bit values and hence code points from 0x0000 to 0xFFFF
//! can be represented by the UCS2 encoding. These are the most frequently used characters and is
//! called the Basic Multilingual Plane (BMP). UCS2 characters are stored as uint16_t.
//!
//! Unicode UTF8 encoding
//! Unicode code points are stored as a sequence of bytes with variable length. The first byte
//! defines how many bytes will follow to represent the particular code point. The first 128
//! characters of Unicode correspond one-to-one with ASCII. Using the UTF8 encoding, they have the
//! same binary representation as when using the eightbit encoding.
//!
//!
//! \par Functionality overview
//!
//! This module offers the following groups of functionality:
//! - String and character length calculation for UCS2 and UTF8
//!
//! - UTF8 and UCS2 string copy and concatenation
//!
//! - Conversion between eightbit, UCS2 and UTF8 encoding
//!     - Several conversion tables are provided as well
//!
//!
//! UTF16 encoding
//! UTF16 encoding is currently not supported by the Unicode module. UCS2 is currently sufficient
//! because it supports all major Chinese characters and the graphics package uses UCS2 only.
//!
//!
//! \par Implementation details
//!
//! - NULL pointers:
//!   Most methods do not check for null pointers (like in string.h)
//!
//! - UCS2 buffer capacity:
//!   The buffer capacity for buffers of type RB_UNICODE_tUCS2 are expected to be in "number of
//!   characters". In general, this does not correspond to the number of bytes as 1 byte is usually
//!   8 bits and one RB_UNICODE_tUCS2 character uses 16 bits.
//!
//! - 1 byte != 8 bits:
//!   There are certain platforms where 1 byte is not equal 8 bits (e.g. TMS 320). The code should
//!   be able to deal with this, but it has NOT been tested. Furthermore, the C# test script cannot
//!   deal with this either.
//!
//!
//! \par Useful links
//!
//!
//! - Unicode character lookup:
//!   http://www.fileformat.info/info/unicode/char/search.htm
//!
//! - Code pages:
//!   http://www.unicode.org/Public/MAPPINGS/
//!
//!
//!
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Martin Heusser, Lukas Schwarz, Bram Scheidegger
//
// $Date: 2016/12/20 14:00:06MEZ $
// $Revision: 1.33 $
//
//==================================================================================================

#ifndef _RB_UNICODE__h
#define _RB_UNICODE__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_UNICODE) && (RB_CONFIG_USE_UNICODE == RB_CONFIG_YES)

#include "RB_Typedefs.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L    D E F I N I T I O N S
//==================================================================================================

//! The type of the UTF-8 strings.
typedef uint8_t RB_DECL_TYPE RB_UNICODE_tUTF8;

//! The type of the UCS-2 characters.
typedef uint16_t RB_DECL_TYPE RB_UNICODE_tUCS2;

//! The type for the Unicode to 8-bit conversion tables. They just contain values from 0x80 to 0xff
//! for the special "non-ascii" characters
typedef RB_UNICODE_tUCS2 RB_DECL_TYPE RB_UNICODE_tEightbitExtToUnicodeTable[128];


//==================================================================================================
//  G L O B A L   C O N S T A N T S
//==================================================================================================
//
// Default tables according to http://www.unicode.org/Public/MAPPINGS/
//
// Attention
// ---------
// For compatibility reasons, some products use conversion tables which differ from the standard
// tables below. Thus, for the use in a product specific software, you may have to create product
// specific tables.
// Proceed as follows to create product specific tables:
//  1. Decide which table is the best suitable
//  2. Copy-paste the table form RB_Unicode.c into an application specific module
//  3. Rename and modify the table according to the product requirements
//  4. Give feedback to the Rainbow team by creating a defect in HPQC or sending an email
//
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0437;					//!< CP437 (MS DOS)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0852;					//!< CP852 (Latin 2)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP0860;					//!< CP860 (Portuguese)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP1251;					//!< CP1251 (Cyrillic)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_CP1252;					//!< CP1252 (Latin 1)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_1;				//!< ISO-8859-1 (Latin 1)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_2;				//!< ISO-8859-2 (Latin 2)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_5;				//!< ISO-8859-5 (Latin/Cyrillic)
extern const RB_DECL_CONST RB_UNICODE_tEightbitExtToUnicodeTable RB_UNICODE_DefaultConversionTable_ISO8859_15;				//!< ISO-8859-15 (Latin 9)


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_UNICODE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_UNICODE_Initialize(void) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC size_t RB_UNICODE_UTF8charlenBytes(const RB_UNICODE_tUTF8 *s) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC size_t RB_UNICODE_UTF8strlenBytes(const RB_UNICODE_tUTF8 *s) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC size_t RB_UNICODE_UTF8strlenChars(const RB_UNICODE_tUTF8 *s) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC size_t RB_UNICODE_UCS2strlenBytes(const RB_UNICODE_tUCS2 *s) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC size_t RB_UNICODE_UCS2strlenChars(const RB_UNICODE_tUCS2 *s) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_UTF8charIsValid(const RB_UNICODE_tUTF8 *utf8) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_UTF8stringIsValid(const RB_UNICODE_tUTF8 *utf8) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_UNICODE_tUTF8* RB_UNICODE_UTF8strncatmax(RB_UNICODE_tUTF8 *dst, const RB_UNICODE_tUTF8 *src, size_t siz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_UNICODE_tUTF8* RB_UNICODE_UTF8strncpymax(RB_UNICODE_tUTF8 *dst, const RB_UNICODE_tUTF8 *src, size_t siz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_UNICODE_tUCS2* RB_UNICODE_UCS2strncatmax(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUCS2 *src, size_t siz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC RB_UNICODE_tUCS2* RB_UNICODE_UCS2strncpymax(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUCS2 *src, size_t siz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUCS2CharacterToEightbitCharacter(char *dst, const RB_UNICODE_tUCS2 *ucs2Char, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_UNICODE_ConvertEightbitCharacterToUCS2Character
//--------------------------------------------------------------------------------------------------
//! \brief	Convert an eightbit character to a UCS2 character using the given conversion table
//!
//! \param	dst				Buffer to store converted character
//! \param	eightBitChar	Source character to be converted
//! \param	convTable		Conversion table
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_UNICODE_ConvertEightbitCharacterToUCS2Character(RB_UNICODE_tUCS2 *dst, const char *eightBitChar, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUCS2StringToEightbitString(char *dst, const RB_UNICODE_tUCS2 *ucs2String, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertEightbitStringToUCS2String(RB_UNICODE_tUCS2 *dst, const char *eightbitString, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUCS2CharacterToUTF8Character(RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tUCS2 *ucs2, size_t maxsiz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUTF8CharacterToUCS2Character(RB_UNICODE_tUCS2 *dst, const RB_UNICODE_tUTF8 *utf8) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUCS2StringToUTF8String(RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tUCS2 *ucs2, size_t maxsiz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUTF8StringToUCS2String(RB_UNICODE_tUCS2 *ucs2, const RB_UNICODE_tUTF8 *utf8, size_t maxsiz) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUTF8CharacterToEightbitCharacter(char *dst, const RB_UNICODE_tUTF8 *utf8, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertEightbitCharacterToUTF8Character(RB_UNICODE_tUTF8 *dst, const char *eightBitChar, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertUTF8StringToEightbitString(char *dst, const RB_UNICODE_tUTF8 *utf8, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool RB_UNICODE_ConvertEightbitStringToUTF8String(RB_UNICODE_tUTF8 *dst, const char *eightbitString, size_t maxsiz, const RB_UNICODE_tEightbitExtToUnicodeTable convTable) RB_ATTR_THREAD_SAFE;


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_UNICODE
#endif // _RB_UNICODE__h
