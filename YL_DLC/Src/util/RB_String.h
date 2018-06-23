//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_String.h
//! \ingroup	util
//! \brief		Functions for string conversions.
//!
//! This module contains the functions for string conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger / Peter Lutz / Martin Heusser
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
// $Revision: 1.50 $
//
//==================================================================================================

#ifndef _RB_String__h
#define _RB_String__h


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
//  G L O B A L   T Y P E S
//==================================================================================================

//! Text alignment
typedef	enum
{
	RB_STRING_ALIGN_LEFT		= 0,		//!< Left alignment
	RB_STRING_ALIGN_RIGHT,					//!< Right alignment
	RB_STRING_ALIGN_CENTER					//!< Center alignment
} RB_DECL_TYPE RB_STRING_tAlignment;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_STRING_AlignLeft
//--------------------------------------------------------------------------------------------------
//! \brief	Left align string in field, i.e. fill right side with spaces
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating zero after adjustment
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
RB_DECL_FUNC void RB_STRING_AlignLeft(char *pStr, size_t rightPos);


//--------------------------------------------------------------------------------------------------
// RB_STRING_AlignRight
//--------------------------------------------------------------------------------------------------
//! \brief	Right align string, i.e. fill left side with spaces
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating zero after adjustment
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
RB_DECL_FUNC void RB_STRING_AlignRight(char *pStr, size_t rightPos);


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
RB_DECL_FUNC void RB_STRING_AlignCenter(char *pStr, size_t rightPos);


//--------------------------------------------------------------------------------------------------
// RB_STRING_Align
//--------------------------------------------------------------------------------------------------
//! \brief	Align string in field
//!
//! For details on alignment, see explicit alignment functions.
//!
//! \param	pStr		Pointer to modified string, aligned left at begin
//! \param	rightPos	Right position of terminating zero after adjustment
//! \param	alignment	Alignment
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_STRING_Align(char *pStr, size_t rightPos, RB_STRING_tAlignment alignment);


//--------------------------------------------------------------------------------------------------
// RB_STRING_strncpymax
//--------------------------------------------------------------------------------------------------
//! \brief	Copy to string with maximum size, add always a terminating zero
//!
//! \param	dst		Destination string
//! \param	src		Source string
//! \param	siz		Maximum size of destination string, including terminating zero
//! \return	Pointer to destination string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC char* RB_STRING_strncpymax(char *dst, const char *src, size_t siz);


//--------------------------------------------------------------------------------------------------
// RB_STRING_strncatmax
//--------------------------------------------------------------------------------------------------
//! \brief	Append to string with maximum size, add always a terminating zero
//!
//! \param	dst		Destination string
//! \param	src		Source string
//! \param	siz		Maximum size of destination string, including terminating zero
//! \return	Pointer to destination string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC char* RB_STRING_strncatmax(char *dst, const char *src, size_t siz);


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
RB_DECL_FUNC size_t RB_STRING_AddBackslashToQuotes(char* pDst, const char* pSrc, size_t max);


#ifdef __cplusplus
}
#endif

#endif // _RB_String__h
