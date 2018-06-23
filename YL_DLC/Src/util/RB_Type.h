//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Type.h
//! \ingroup	util
//! \brief		Data type interpreter.
//!
//! This module does the interpretation of data types. It replaces stdio C functions printf() and
//! scanf() and their derivatives. Other than stdio, this module is RAM optimized and additionally
//! provides value range checking when parsing values from strings.
//!
//! However, it is up to the application developer whether sprintf, sscanf, ... should be used in
//! a specific project in addition to RB_Type.
//!
//!	sprintf/sscanf on IAR
//! 	One can configure the capabilities of the printf and scanf library (see doc/user/Raibow-IAR-ARM.pdf)
//! 	The printf library uses ro memory from ~1kB (tiny) to ~7kB (full) depending on the capabilities (fully optimized for size).
//! 	The scanf library uses ro memory from ~2kB (small) to ~9kB (full) depending on the capabilities (fully optimized for size).
//! 	If the full scanf and the full printf library is chosen, it will consume ~14kB of ro memory.
//!
//!	Design considerations:
//!		- Use as little RAM as possible
//!		- Use as little string variables as possible in functions
//!
//! \attention:
//! For string parameters converted from Tigris II format (e.g. type s7), a terminating zero is automatically added after the string payload.
//! This means that the caller must reserve an additional byte for the terminating zero (e.g. for type s7, 8 bytes must be reserved since a terminating zero will be written after the 7th character).
//! This does not apply to internal members of struct types. In struct types, there will no terminating zero be written for string members except if the string is the last member of the struct.
//! Examples:
//!   s7 --> buffer must fit 8 characters (char buf[8]) since it must fit the terminating zero.
//!   S{s7;n} --> struct must have these members: (char string[7], uint8_t number),
//!               since the string is internal member of the struct. No terminating zero will be present after strings which are internal members of structs.
//!   S{n;s7} --> struct must have these members: (uint8_t number, char string[8]), since string is last member of struct.
//!
//! \attention:
//! A special case must be considered when several parameters (containing a string as last element) are placed in an array one after each other such that multiple parameter
//! instances can be read out/written by using the functions RB_PARAM_GetArray/RB_PARAM_SetArray. In this case, NO extra space for a terminating zero must be reserved in the buffers.
//! But an extra array element must be reserved at the end of the array.
//! Example:
//!   Array of S{n;s7} --> structs must have these members: (uint8_t number, char string[7]). If e.g. 2 array elements are required, we must declare 3 array elements.
//!                        No terminating zero will be present after the strings who use up the whole size (e.g. 7 payload characters for s7).
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author	Werner Langenegger
//
// $Date: 2017/08/22 09:56:31MESZ $
// $Revision: 1.102 $
//
//==================================================================================================

#ifndef  _RB_Type__h
#define  _RB_Type__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================
#define RB_CONFIG_YES 1 
#define RB_CONFIG_USE_TYPE 1
// SP needs enums and prototypes below, but don't include RB_Config.h when compiling SP libraries.
//#ifdef RB_WITHIN_SP_LIB
//	#undef  RB_CONFIG_YES
//	#undef  RB_CONFIG_USE_TYPE
//	#define RB_CONFIG_YES 1
//	#define RB_CONFIG_USE_TYPE RB_CONFIG_YES
//	#include "RB_Typedefs.h"
//#else
//	#include "RB_Config.h"
//#endif

#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)

#include <stdlib.h>
#include "RB_Format.h"
//#include "RB_Pack.h"
#include "RB_Parse.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

// Default defines, if not defined in RB_Config_Common
#ifndef RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED
	#define RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED		RB_CONFIG_YES
#endif
#ifndef RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED
	#define RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED			RB_CONFIG_YES
#endif
#ifndef RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED
	#define RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED	RB_CONFIG_YES
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Definition of return values
typedef enum {
	RB_TYPE_OK				= 0,	//!< No Error
	RB_TYPE_ERROR_PARSER,			//!< Error during parsing
	RB_TYPE_ERROR_RANGE,			//!< Value out of range
	RB_TYPE_ERROR_TYPEDEF,			//!< Error in type definition (syntax)
	RB_TYPE_NOT_DEFINED,			//!< No type definition found
	RB_TYPE_ERROR_STR_LEN,			//!< Error stringlength
	RB_TYPE_ERROR_ALIGNMENT 		//!< Wrong alignment (type definition or var pointer)
} RB_DECL_TYPE RB_TYPE_tStatus;

//! Type identifiers
typedef enum {
	// \verbatim Simple types											Syntax, [] = optional, <..> = value	\endverbatim
	RB_TYPE_CHAR		= 'c',	//!< \verbatim character				c[<max>[ <min>]]					\endverbatim
	RB_TYPE_IDENT		= 'a',	//!< \verbatim identifier (alfanum)		a<maxlen>[ <minlen>]				\endverbatim
	RB_TYPE_STRING		= 's',	//!< \verbatim string (quoted)			s<maxlen>[ <minlen>]				\endverbatim
//	RB_TYPE_BIT_16		= 'q',	//!< \verbatim bitset 16 bit			q[<max>[ <min>]]					\endverbatim
//	RB_TYPE_BIT_32		= 'r',	//!< \verbatim bitset 32 bit			r[<max>[ <min>]]					\endverbatim
	RB_TYPE_I8			= 'j',	//!< \verbatim signed int 8 bit			j[<max>[ <min>]]					\endverbatim
	RB_TYPE_U8			= 'k',	//!< \verbatim unsigned int 8 bit		k[<max>[ <min>]]					\endverbatim
	RB_TYPE_D8			= 'K',	//!< \verbatim same as k, output of 0..9 with leading 0 (00, 01, .. 09, 10)	\endverbatim
	RB_TYPE_I16			= 'i',	//!< \verbatim signed int 16 bit		i[<max>[ <min>]]					\endverbatim
	RB_TYPE_U16			= 'n',	//!< \verbatim unsigned int 16 bit		n[<max>[ <min>]]					\endverbatim
	RB_TYPE_I32			= 'l',	//!< \verbatim signed long 32 bit		l[<max>[ <min>]]					\endverbatim
	RB_TYPE_U32			= 'm',	//!< \verbatim unsigned long 32 bit		m[<max>[ <min>]]					\endverbatim
	RB_TYPE_H8			= 'u',	//!< \verbatim unsigned hex 8 bit		u[<max>[ <min>]]  hh format			\endverbatim
	RB_TYPE_H16			= 'v',	//!< \verbatim unsigned hex 16 bit		v[<max>[ <min>]]  hhhh format		\endverbatim
	RB_TYPE_H32			= 'w',	//!< \verbatim unsigned hex 32 bit		w[<max>[ <min>]]  hhhhhhhh format	\endverbatim
	RB_TYPE_X8			= 'x',	//!< \verbatim unsigned hex 8 bit		x[<max>[ <min>]]  0xhh format		\endverbatim
	RB_TYPE_X16			= 'y',	//!< \verbatim unsigned hex 16 bit		y[<max>[ <min>]]  0xhhhh format		\endverbatim
	RB_TYPE_X32			= 'z',	//!< \verbatim unsigned hex 32 bit		z[<max>[ <min>]]  0xhhhhhhhh format	\endverbatim
	RB_TYPE_FLOAT		= 'f',	//!< \verbatim floating point 32 bit	f[<max>[ <min>]]					\endverbatim
	RB_TYPE_FLOAT_E		= 'e',	//!< \verbatim same as 'f', but output in "scientific format", see MT-SICS	\endverbatim
	RB_TYPE_FLOAT_G		= 'g',	//!< \verbatim same as 'f', but output in "normal format",     see MT-SICS	\endverbatim
	RB_TYPE_DOUBLE		= 'd',	//!< \verbatim floating point 64 bit	d[<max>[ <min>]]					\endverbatim
	RB_TYPE_DOUBLE_E	= 'E',	//!< \verbatim same as 'd', but output in "scientific format", see MT-SICS 	\endverbatim
	RB_TYPE_DOUBLE_G	= 'G',	//!< \verbatim same as 'd', but output in "normal format",     see MT-SICS	\endverbatim
	RB_TYPE_PPARAM		= '*',	//!< \verbatim pointer to params		* = Pointer to remaining parameters	\endverbatim
								//!< must be last element in struct !
	// Complex types
//	RB_TYPE_ENUM		= 'N',	//!< \verbatim enumeration				N{name[=<value>];...}				\endverbatim
//	RB_TYPE_ARRAY		= 'A',	//!< \verbatim array					A<size><typedef>					\endverbatim
	RB_TYPE_STRUCT		= 'S',	//!< \verbatim struct					S{[<name>=]<typedef>;...}			\endverbatim
	RB_TYPE_FUNCTION	= 'F',	//!< \verbatim function					F<functionNum>						\endverbatim
	RB_TYPE_TYPEDEF		= 'T' 	//!< \verbatim predefined type			T<typename>							\endverbatim
} RB_DECL_TYPE RB_TYPE_tTypeIdent;


// Type function prototype
//! Provide variable pointer, return true = Ok, false will return parser error
typedef RB_DECL_TYPE bool (*RB_TYPE_tFunction)(void*);



//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TYPE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the data type interpreter
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TYPE_Initialize(void);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Parse
//--------------------------------------------------------------------------------------------------
//! \brief		Parse input string to variable with specified type.
//!
//! The input value(s) are checked for minimum and maximum values or length
//!
//! \attention	In case of strings where the string length equals the maximum length, the string
//!				in pVariable isn't terminated with zero. This is due to NVMem restrictions for
//!				Tigris-2 backward compatibility. These restrictions state that it's not allowed to
//!				add an extra character terminating zero if the string already occupies the size of the NVMem
//!				storage area.
//!				Example with MT-SICS command D "This is a text that does not fit12345678":
//!				\see RB_CONFIG_CMD1_D_MAXLENGTH = 40 => type definition = "s40"
//!				In case of a call of command D with a string that is exactly 40 characters long
//!				RB_TYPE_Parse() will return an unterminated string. Thus, the caller has
//!				to add terminating zero manually. For an example implementation see \see RB_CMD1_D().
//!
//! \param		pInput			Input string to parse
//! \param		pTypeName		Type to parse, name of predefined type in typeTable
//! \param		pVariable		Location where parsed data are stored
//! \return		RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_Parse(const char* pInput, const char* pTypeName, void* pVariable);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseDefinition
//--------------------------------------------------------------------------------------------------
//! \brief		Parse input string to variable with specified type definition (not type name).
//!
//! The input value(s) are checked for minimum and maximum values or length
//!
//! \attention	In case of strings where the string length equals the maximum length, the string
//!				in pVariable isn't terminated with zero. This is due to NVMem restrictions for
//!				Tigris-2 backward compatibility. These restrictions state that it's not allowed to
//!				add an extra character terminating zero if the string already occupies the size of the NVMem
//!				storage area.
//!				Example with MT-SICS command D "This is a text that does not fit12345678":
//!				\see RB_CONFIG_CMD1_D_MAXLENGTH = 40 => type definition = "s40"
//!				In case of a call of command D with a string that is exactly 40 characters long
//!				RB_TYPE_ParseDefinition() will return an unterminated string. Thus, the caller has
//!				to add terminating zero manually. For an example implementation see \see RB_CMD1_D().
//!
//! \param		pInput			Input string to parse
//! \param		pTypeDefinition	Type definition to parse, e.g. "S{n5 1;s30}"
//! \param		pVariable		Location where parsed data are stored
//! \return		RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_ParseDefinition(const char* pInput, const char* pTypeDefinition, void* pVariable);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ToString
//--------------------------------------------------------------------------------------------------
//! \brief	Write data of a predefined type name to output string.
//!
//! \param	pOutput			Output string
//! \param	pTypeName		Type to parse, name of predefined type in typeTable
//! \param	pVariable		Location of data of that type
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_ToString(char* pOutput, const char* pTypeName, void* pVariable);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_DefinitionToString
//--------------------------------------------------------------------------------------------------
//! \brief	Write data of a specified type definition to output string (not type name).
//!
//! \param	pOutput      	Output string
//! \param	pTypeDefinition	Type definition to parse, e.g. "S{n5 1;s30}"
//! \param	pVariable    	Location of data of that type
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_DefinitionToString(char* pOutput, const char* pTypeDefinition, void* pVariable);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Definition
//--------------------------------------------------------------------------------------------------
//! \brief	Return the type definition from a type name.
//!
//! \param	pTypeName		Type name, e.g. "XP0301"
//! \return	pTypeDefinition, empty string if no typedefinition found ("").
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* RB_TYPE_Definition(const char* pTypeName);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Pack
//--------------------------------------------------------------------------------------------------
//! \brief	Copy data of a specified type definition from an unpacked to a packed variable.
//!
//! \attention: Precondition *pSrc and *pDst must be correctly aligned to target alignment requirements
//! and Tigris-2 alignment requirement (Tigris-2 alignment requirement is max. 4).
//!
//! \param	pTypeDefinition	Type definition to parse, e.g. "S{n5 1;s30}"
//! \param	pPacked	    	Address of packed variable
//! \param	pUnpacked    	Address of unpacked variable
//! \param	packedEndianess	Endianess of packed data
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_Pack(const char* pTypeDefinition, void* pPacked, const void* pUnpacked, RB_TYPEDEFS_tEndianess packedEndianess);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Unpack
//--------------------------------------------------------------------------------------------------
//! \brief	Copy data of a specified type definition from a packed to an unpacked variable.
//!
//! \attention: Precondition *pSrc and *pDst must be correctly aligned to target alignment requirements
//! and Tigris-2 alignment requirement (Tigris-2 alignment requirement is max. 4).
//!
//! \param	pTypeDefinition	Type definition to parse, e.g. "S{n5 1;s30}"
//! \param	pPacked	    	Address of packed variable
//! \param	pUnpacked    	Address of unpacked variable
//! \param	packedEndianess	Endianess of packed data
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC RB_TYPE_tStatus RB_TYPE_Unpack(const char* pTypeDefinition, const void* pPacked, void* pUnpacked, RB_TYPEDEFS_tEndianess packedEndianess);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_SizeOfPacked
//--------------------------------------------------------------------------------------------------
//! \brief	Return size of the specified type in a packed struct, including trailing fillers.
//!
//! \param	pTypeDefinition		Type definition, e.g. "S{n5 1;s30}"
//! \return
//!	 - >0: Size of type
//!	 - -1: Type not found or error in typedef
//!	 - -2: Size not determinable, e.g. no max string length defined
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int RB_TYPE_SizeOfPacked(const char* pTypeDefinition);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_SizeOfUnpacked
//--------------------------------------------------------------------------------------------------
//! \brief	Return size of the specified type in a unpacked struct, including trailing fillers.
//!
//! \param	pTypeDefinition		Type definition, e.g. "S{n5 1;s30}"
//! \return
//!	 - >0: Size of type
//!	 - -1: Type not found or error in typedef
//!	 - -2: Size not determinable, e.g. no max string length defined
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int RB_TYPE_SizeOfUnpacked(const char* pTypeDefinition);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_AlignmentOfUnpacked
//--------------------------------------------------------------------------------------------------
//! \brief	Return alignment requirement of type
//!
//! \param	pTypeDefinition		Type definition, e.g. "S{n5 1;s30}"
//! \return
//!	 - >0: Alignment requirement of unpacked type
//!	 - ==0: Error
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t RB_TYPE_AlignmentOfUnpacked(const char* pTypeDefinition);


//--------------------------------------------------------------------------------------------------
// RB_TYPE_GetTablePtr
//--------------------------------------------------------------------------------------------------
//! \brief	Return the pointer to the type definition table, defined in RB_Config.h
//!
//! \return	Address of table
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* const * RB_TYPE_GetTablePtr(void) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// RB_TYPE_GetTableSize
//--------------------------------------------------------------------------------------------------
//! \brief	Return the size of the type definition table, defined in RB_Config.h
//!
//! \return	Size of table, i.e. number of table entries
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int RB_TYPE_GetTableSize(void) RB_ATTR_THREAD_SAFE;


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_TYPE
#endif // _RB_Type__h
