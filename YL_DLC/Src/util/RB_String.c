//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_String.c
//! \ingroup	util
//! \brief		Functions for string conversions.
//!
//! This module contains the functions for string conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger / Peter Lutz
//!
//! The string related functions listed below are allowed to be used from the ANSI C <string.h> file.
//! \attention 	The string related ANSI C functions not listed below should not be used and
//!				the implementations in this file should be used instead.
//! \code
//!		char *strchr(const char *, int);
//!		char *strrchr(const char *, int);
//!		int strcmp(const char *, const char *);
//!		int strncmp(const char *, const char *, size_t);
//!		int strcoll(const char *, const char *);
//!		size_t strlen(const char *);
//!		size_t strspn(const char *s, const char *accept);
//!		size_t strcspn(const char *s, const char *reject);
//!		char *strpbrk(const char *s, const char *accept);
//!		char *strstr(const char *haystack, const char *needle);
//!		char *strtok(char *, const char *);
//! \endcode
//
// $Date: 2016/12/19 15:47:24MEZ $
// $Revision: 1.57 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_String"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_String.h"
// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

#include <string.h>


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_STRING_AlignLeft
//--------------------------------------------------------------------------------------------------
//! \brief	Left align string in field, i.e. fill right side with spaces
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating '\\0' after adjustment
//! \return	none
//!
//! \par Example code:
//! \code
//! #define FIELD_WIDTH		15
//! char ExampleStr[16] = "Example";
//!
//! RB_STRING_AlignLeft(ExampleStr, FIELD_WIDTH);
//!
//!	//                    0123456789012345
//!	// ExampleStr before "Example"
//! // ExampleStr after  "Example        "
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_STRING_AlignLeft(char* pStr, size_t rightPos)
{
	size_t srcLen = strlen(pStr);
	int diff = (int)rightPos - (int)srcLen;
	size_t c;

	// Check, if we can align
	if (diff <= 0)
		return;

	// Fill spaces
	for (c = srcLen; c < rightPos; c++) {
		pStr[c] = ' ';
	}

	pStr[rightPos] = (char)'\0';
}


//--------------------------------------------------------------------------------------------------
// RB_STRING_AlignRight
//--------------------------------------------------------------------------------------------------
//! \brief	Right align string, i.e. fill left side with spaces
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating '\\0' after adjustment
//! \return	none
//!
//! \par Example code:
//! \code
//! #define FIELD_WIDTH		15
//! char ExampleStr[16] = "Example";
//!
//! RB_STRING_AlignRight(ExampleStr, FIELD_WIDTH);
//!
//!	//                    0123456789012345
//!	// ExampleStr before "Example"
//! // ExampleStr after  "        Example"
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_STRING_AlignRight(char* pStr, size_t rightPos)
{
	size_t srcLen = strlen(pStr);
	int diff = (int)rightPos - (int)srcLen;
	int c;

	// Check, if we can align
	if (diff <= 0)
		return;

	// Move string
	for (c = (int)srcLen; c >= 0; c--) {
		pStr[(int)(c + diff)] = pStr[c];
	}

	// Fill spaces
	for (c = 0; c < diff; c++) {
		pStr[c] = (char)' ';
	}
}


//--------------------------------------------------------------------------------------------------
// RB_STRING_AlignCenter
//--------------------------------------------------------------------------------------------------
//! \brief	Center string in field, i.e. fill left side with spaces, right side is not filled.
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Field size for centering
//! \return	none
//!
//! \attention	Right side is not filled with spaces, use RB_STRING_AlignLeft if needed!
//!
//! \par Example code:
//! \code
//! #define FIELD_WIDTH		15                          //  0123456789012345
//! char ExampleStr[16] = "Example";                    // "Example"
//!
//! RB_STRING_AlignCenter(ExampleStr, FIELD_WIDTH);     // "    Example"
//! RB_STRING_AlignLeft(ExampleStr, FIELD_WIDTH);       // "    Example    "
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_STRING_AlignCenter(char* pStr, size_t rightPos)
{
	size_t srcLen = strlen(pStr);
	int diff = ((int)rightPos - (int)srcLen) / 2;
	int c;

	// Check, if we can align
	if (diff <= 0)
		return;

	// Move string
	for (c = (int)srcLen; c >= 0; c--) {
		pStr[(int)(c + diff)] = pStr[c];
	}

	// Fill spaces
	for (c = 0; c < diff; c++) {
		pStr[c] = (char)' ';
	}
}


//--------------------------------------------------------------------------------------------------
// RB_STRING_Align
//--------------------------------------------------------------------------------------------------
//! \brief	Align string in field
//!
//! For details on alignment, see explicit alignment functions.
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating '\\0' after adjustment
//! \param	alignment	Alignment
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_STRING_Align(char *pStr, size_t rightPos, RB_STRING_tAlignment alignment)
{
	switch (alignment)
	{
		case RB_STRING_ALIGN_LEFT:
			RB_STRING_AlignLeft(pStr, rightPos);
			break;

		case RB_STRING_ALIGN_RIGHT:
			RB_STRING_AlignRight(pStr, rightPos);
			break;

		case RB_STRING_ALIGN_CENTER:
		default:
			RB_STRING_AlignCenter(pStr, rightPos);
			break;
	}
}


//--------------------------------------------------------------------------------------------------
// RB_STRING_strncpymax
//--------------------------------------------------------------------------------------------------
//! \brief	Copy to string with maximum size, add always a terminating zero
//!
//! \param	dst		Destination string
//! \param	src		Source string
//! \param	siz		Maximum size of destination string, including terminating zero
//! \return	Pointer to destination string (same as parameter dst)
//--------------------------------------------------------------------------------------------------
char* RB_STRING_strncpymax(char* dst, const char* src, size_t siz)
{
	char* dstStart = dst;

	if (dst && siz)	{
		if (src)
			while (*src && --siz)
				*dst++ = *src++;
		*dst = (char)'\0';  // termination
	}
	return dstStart;
}


//--------------------------------------------------------------------------------------------------
// RB_STRING_strncatmax
//--------------------------------------------------------------------------------------------------
//! \brief	Append to string with maximum size, add always a terminating zero
//!
//! \param	dst		Destination and source string
//! \param	src		String to append to destination string
//! \param	siz		Maximum size of destination string, including terminating zero
//! \return	Pointer to destination string (same as parameter dst)
//--------------------------------------------------------------------------------------------------
char* RB_STRING_strncatmax(char *dst, const char *src, size_t siz)
	{
	char* dstStart = dst;
	if (dst && siz && src)
		{
		while (*dst && --siz)
			dst++;
		if (siz)
			while (*src && --siz)
				*dst++ = *src++;
		*dst = (char)'\0';
		}
	return dstStart;
	}


//--------------------------------------------------------------------------------------------------
// RB_STRING_AddBackslashToQuotes
//--------------------------------------------------------------------------------------------------
//! \brief	Copies string with maximum size, prefixes all quotation marks in source string with
//!			C-like backslash '\' and always adds a terminating zero
//!
//! String "this "is" a text" results in "this \"is\" a text".
//!
//! \param	pDst	Destination string
//! \param	pSrc	Source string
//! \param	max		Maximum size of destination string, including terminating '\\0'
//! \return	Length of string, 0 and empty string if output string is to short
//--------------------------------------------------------------------------------------------------
size_t RB_STRING_AddBackslashToQuotes(char* pDst, const char* pSrc, size_t max)
{
	char* pDstStart = pDst;
	size_t ret = 0;

	if (pDst && max) {
		if (pSrc) {
			while (*pSrc && --max) {
				if (*pSrc == '\"') {
					if (--max) {       // only add backslash and quote if there is space for both
						*pDst++ = '\\';
						*pDst++ = *pSrc++;
					}
				}
				else {
					*pDst++ = *pSrc++;
				}
			}
			if (*pSrc == '\0') {
				ret = 1;               // signal that source fits into destination
			}
			else {
				pDst = pDstStart;      // reset destination to return empty string
			}
		}
		*pDst = (char)'\0';            // termination
	}

	if (ret > 0) {                     // get length of string if source fits into destination
		ret = strlen(pDstStart);
	}

	return (ret);
}
