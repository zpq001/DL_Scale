//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Type.c
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
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author	Werner Langenegger
//!
// $Date: 2017/02/23 15:13:49MEZ $
// $Revision: 1.230 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Type"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Type.h"

#if defined(RB_CONFIG_USE_TYPE) && (RB_CONFIG_USE_TYPE == RB_CONFIG_YES)

#include <ctype.h>
#include <string.h>
#include "RB_CmdM.h"
#include "RB_Date.h"
#include "RB_Format.h"
#include "RB_Math.h"
//#include "RB_Pack.h"
#include "RB_Parse.h"
#include "RB_String.h"


//lint -e616 Control flows into case/default
//lint -e925 Cast from pointer to pointer
//lint -e929 Cast from pointer to pointer
//lint -e737 Loss of sign
//lint -e740 Unusual pointer cast
//lint -e825 Case fallthough
//lint -e929 Cast from pointer to pointer

//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================

// Alignment of data types in Tigris-2 packed format
#define T2_ALIGN_D8				1u
#define T2_ALIGN_D16			1u
#define T2_ALIGN_D32			1u
//#define T2_ALIGN_D64			1u
#define T2_ALIGN_F32			1u
#define T2_ALIGN_F64			1u
#define T2_ALIGN_PTR			1u
#define T2_ALIGN_STRUCT			1u

// Size of data types in Tigris-2 packed format (octets == 8 bit)
#define T2_OCTETS_D8			1u
#define T2_OCTETS_D16			2u
#define T2_OCTETS_D32			4u
//#define T2_OCTETS_D64			4u
#define T2_OCTETS_F32			4u
#define T2_OCTETS_F64			8u
#define T2_OCTETS_PTR			4u

#ifndef RB_CONFIG_TYPE_FUNCTION_TABLE_CONTENT
	#define RB_CONFIG_TYPE_FUNCTION_TABLE_CONTENT
#endif

#ifndef RB_CONFIG_TYPE_TABLE_CONTENT
	#define RB_CONFIG_TYPE_TABLE_CONTENT
#endif

#if CHAR_BIT % 8 != 0
#error Only machines where the bits contained in a byte are a multiple of 8 are supported!
#endif


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================
//! Type definition table
static const char* const RB_TYPE_Table[] = {
	"c=c",
	"a=a",
	"s=s",
	"j=j",
	"k=k",
	"K=K",
	"i=i",
	"n=n",
	"l=l",
	"m=m",
	"u=u",
	"v=v",
	"w=w",
	"x=x",
	"y=y",
	"z=z",
	"f=f",
	"e=e",
	"g=g",
	"d=d",
	"E=E",
	"G=G",
	RB_CONFIG_TYPE_TABLE_CONTENT
	""				// Last definition must be an empty string
};

//! Type function table, i.e. functions which are called during type parsing
static const RB_TYPE_tFunction RB_TYPE_FunctionTbl[] = {
	RB_CONFIG_TYPE_FUNCTION_TABLE_CONTENT
	(bool(*)(void*))0 };



//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static RB_TYPE_tStatus RB_TYPE_ExecuteFunction(const char* pType, void*  pVariable);
static RB_TYPE_tStatus RB_TYPE_ParseType      (const char** ppInput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, void* pStruct, size_t* pMaxAlignment, bool pack);
static RB_TYPE_tStatus RB_TYPE_ParseStruct    (const char** ppInput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, void* pStruct, size_t* pMaxAlignment, bool pack);
static RB_TYPE_tStatus RB_TYPE_ParseCharacter (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer);
static RB_TYPE_tStatus RB_TYPE_ParseIdentifier(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer);
static RB_TYPE_tStatus RB_TYPE_ParseString    (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer);
static RB_TYPE_tStatus RB_TYPE_ParseI         (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer, size_t octets);
static RB_TYPE_tStatus RB_TYPE_ParseU         (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer, size_t octets, bool hex);
#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
static RB_TYPE_tStatus RB_TYPE_ParseFloat     (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer);
static RB_TYPE_tStatus RB_TYPE_ParseDouble    (const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer);
#endif

static const char* RB_TYPE_FindType(const char* typeName, size_t* lenOfTypeName);

static RB_TYPE_tStatus RB_TYPE_ToStringType(char** ppOutput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, size_t* pMaxAlignment, bool pack);
static RB_TYPE_tStatus RB_TYPE_ToStringStruct(char** ppOutput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, size_t* pMaxAlignment, bool pack);

static RB_TYPE_tStatus RB_TYPE_InfoOfType  (const char** ppType, size_t* pSize, size_t* pMaxAlignment, bool pack);
static RB_TYPE_tStatus RB_TYPE_InfoOfStruct(const char** ppType, size_t* pSize, size_t* pMaxAlignment, bool pack);
static RB_TYPE_tStatus RB_TYPE_InfoOfString(const char* pType, size_t* pSize, size_t* pMaxAlignment, bool pack);

static RB_TYPE_tStatus RB_TYPE_CopyOfType(const char** ppType, RB_PACK_tRecBuffer *dstBuffer, RB_PACK_tRecBufferConst *srcBuffer, RB_TYPEDEFS_tEndianess reqEndianess, bool pack);
static RB_TYPE_tStatus RB_TYPE_CopyOfStruct(const char** ppType, RB_PACK_tRecBuffer *dstBuffer, RB_PACK_tRecBufferConst *srcBuffer, RB_TYPEDEFS_tEndianess reqEndianess, bool pack);

static void RB_TYPE_Align(size_t* value, size_t alignTo, size_t* pMaxAlignment);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TYPE_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize the data type interpreter
//!
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_TYPE_Initialize(void)
{
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Parse
//--------------------------------------------------------------------------------------------------
//! \brief		Parse input string to variable with specified type.
//!
//! The input value(s) are checked for minimum and maximum values or length
//!
//! \attention	In case of strings where the string length equals the maximum length, the string
//!				in pVariable isn't terminated with '\\0'. This is due to NVMem restrictions for
//!				Tigris-2 backward compatibility. These restrictions state that it's not allowed to
//!				add an extra character '\\0' if the string already occupies the size of the NVMem
//!				storage area.
//!				Example with MT-SICS command D "This is a text that does not fit12345678":
//!				\see RB_CONFIG_CMD1_D_MAXLENGTH = 40 => type definition = "s40"
//!				In case of a call of command D with a string that is exactly 40 characters long
//!				RB_TYPE_Parse() will return an unterminated string. Thus, the caller has
//!				to add '\\0' manually. For an example implementation see \see RB_CMD1_D().
//!
//! \param		pInput			Input string to parse
//! \param		pTypeName		Type to parse, name of predefined type in typeTable
//! \param		pVariable		Location where parsed data are stored
//! \return		RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_TYPE_tStatus RB_TYPE_Parse(const char* pInput, const char* pTypeName, void* pVariable)
{
	return RB_TYPE_ParseDefinition(pInput, RB_TYPE_Definition(pTypeName), pVariable);
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseDefinition
//--------------------------------------------------------------------------------------------------
//! \brief		Parse input string to variable with specified type definition (not type name).
//!
//! The input value(s) are checked for minimum and maximum values or length
//!
//! \attention	In case of strings where the string length equals the maximum length, the string
//!				in pVariable isn't terminated with '\\0'. This is due to NVMem restrictions for
//!				Tigris-2 backward compatibility. These restrictions state that it's not allowed to
//!				add an extra character '\\0' if the string already occupies the size of the NVMem
//!				storage area.
//!				Example with MT-SICS command D "This is a text that does not fit12345678":
//!				\see RB_CONFIG_CMD1_D_MAXLENGTH = 40 => type definition = "s40"
//!				In case of a call of command D with a string that is exactly 40 characters long
//!				RB_TYPE_ParseDefinition() will return an unterminated string. Thus, the caller has
//!				to add '\\0' manually. For an example implementation see \see RB_CMD1_D().
//!
//! \param		pInput			Input string to parse
//! \param		pTypeDefinition	Type definition to parse, e.g. "S{n5 1;s30}"
//! \param		pVariable		Location where parsed data are stored
//! \return		RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_TYPE_tStatus RB_TYPE_ParseDefinition(const char* pInput, const char* pTypeDefinition, void* pVariable)
{
	RB_TYPE_tStatus typeSts;
	size_t maxAlignment = 0;

	RB_PACK_tRecBuffer varBuffer;
	varBuffer.index = 0;
	varBuffer.pBufferStart = pVariable;

	// Parse input to variable and validate it
	typeSts = RB_TYPE_ParseType(&pInput, &pTypeDefinition, &varBuffer, 0, &maxAlignment, false);
	if (typeSts == RB_TYPE_OK) {
		// Skip trailing blanks
		while (*pInput == ' ')
			pInput++;

		// Return error, if we have some input left
		if (*pInput != '\0')
			return RB_TYPE_ERROR_PARSER;
	}

	return typeSts;
}


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
RB_TYPE_tStatus RB_TYPE_ToString(char* pOutput, const char* pTypeName, void* pVariable)
{
	return RB_TYPE_DefinitionToString(pOutput, RB_TYPE_Definition(pTypeName), pVariable);
}


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
RB_TYPE_tStatus RB_TYPE_DefinitionToString(char* pOutput, const char* pTypeDefinition, void* pVariable)
{
	size_t maxAlignment = 0;
	RB_PACK_tRecBuffer varBuffer;
	varBuffer.index = 0;
	varBuffer.pBufferStart = pVariable;

	return RB_TYPE_ToStringType(&pOutput, &pTypeDefinition, &varBuffer, &maxAlignment, false);
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Definition
//--------------------------------------------------------------------------------------------------
//! \brief	Return the type definition from a type name.
//!
//! \param	pTypeName		Type name, e.g. "XP0301"
//! \return	pTypeDefinition, empty string if no typedefinition found ("").
//--------------------------------------------------------------------------------------------------
const char* RB_TYPE_Definition(const char* pTypeName)
{
	const char *pType;
	size_t lenOfTypeName;

	// Find type in table
	pType = RB_TYPE_FindType(pTypeName, &lenOfTypeName);

	// If type found, we parse input value(s)
	if (pType) {
		// Point behind of typename
		pType += lenOfTypeName + 1;
		return pType;
	}
	else {
		return "";
	}
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Pack
//--------------------------------------------------------------------------------------------------
//! \brief	Copy data of a specified type definition from an unpacked to a packed variable.
//!
//! \attention: Precondition *pSrc and *pDst must be correctly aligned to target alignment requirements
//! and Tigris-2 alignment requirement (Tigris-2 alignment requirement is max. 4).
//!
//! \param	pTypeDefinition		Type definition to parse, e.g. "S{n5 1;s30}"
//! \param	pPacked	    		Address of packed variable
//! \param	pUnpacked    		Address of unpacked variable
//! \param	packedEndianess		Endianess of packed data
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_TYPE_tStatus RB_TYPE_Pack(const char* pTypeDefinition, void* pPacked, const void* pUnpacked, RB_TYPEDEFS_tEndianess packedEndianess)
{

	RB_PACK_tRecBuffer dstBuffer;
	RB_PACK_tRecBufferConst srcBuffer;

	dstBuffer.index = 0;
	dstBuffer.pBufferStart = pPacked;

	srcBuffer.index = 0;
	srcBuffer.pBufferStart = pUnpacked;

	//                                          packed       unpacked
	return RB_TYPE_CopyOfType(&pTypeDefinition, &dstBuffer, &srcBuffer, packedEndianess, true);
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Unpack
//--------------------------------------------------------------------------------------------------
//! \brief	Copy data of a specified type definition from a packed to an unpacked variable.
//!
//! \attention: Precondition *pSrc and *pDst must be correctly aligned to target alignment requirements
//! and Tigris-2 alignment requirement (Tigris-2 alignment requirement is max. 4).
//!
//! \param	pTypeDefinition		Type definition to parse, e.g. "S{n5 1;s30}"
//! \param	pPacked	    		Address of packed variable
//! \param	pUnpacked    		Address of unpacked variable
//! \param	packedEndianess		Endianess of packed data
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
RB_TYPE_tStatus RB_TYPE_Unpack(const char* pTypeDefinition, const void* pPacked, void* pUnpacked, RB_TYPEDEFS_tEndianess packedEndianess)
{
	RB_PACK_tRecBuffer dstBuffer;
	RB_PACK_tRecBufferConst srcBuffer;

	dstBuffer.index = 0;
	dstBuffer.pBufferStart = pUnpacked;

	srcBuffer.index = 0;
	srcBuffer.pBufferStart = pPacked;

	//                                          unpacked     packed
	return RB_TYPE_CopyOfType(&pTypeDefinition, &dstBuffer, &srcBuffer, packedEndianess, false);
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_SizeOfPacked
//--------------------------------------------------------------------------------------------------
//! \brief	Return size of the specified type in a packed struct.
//!
//! \param	pTypeDefinition		Type definition, e.g. "S{n5 1;s30}"
//! \return
//!	 - >0: Size of type
//!	 - -1: Type not found or error in typedef
//!	 - -2: Size not determinable, e.g. no max string length defined
//--------------------------------------------------------------------------------------------------
int RB_TYPE_SizeOfPacked(const char* pTypeDefinition)
{
	RB_TYPE_tStatus sts;
	size_t size = 0;
	size_t maxAlignment = 0;

	sts = RB_TYPE_InfoOfType(&pTypeDefinition, &size, &maxAlignment, true);
	if (sts == RB_TYPE_ERROR_STR_LEN) {
		return -2;
	} else if (sts != RB_TYPE_OK) {
		return -1;
	}
	return (int)size;
}


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
int RB_TYPE_SizeOfUnpacked(const char* pTypeDefinition)
{
	RB_TYPE_tStatus sts;
	size_t size = 0;
	size_t maxAlignment = 0;

	sts = RB_TYPE_InfoOfType(&pTypeDefinition, &size, &maxAlignment, false);
	if (sts == RB_TYPE_ERROR_STR_LEN) {
		return -2;
	} else if (sts != RB_TYPE_OK) {
		return -1;
	}
	return (int)size;
}

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
size_t RB_TYPE_AlignmentOfUnpacked(const char* pTypeDefinition)
{
	RB_TYPE_tStatus sts;
	size_t size = 0;
	size_t maxAlignment = 0;

	sts = RB_TYPE_InfoOfType(&pTypeDefinition, &size, &maxAlignment, false);
	if (sts != RB_TYPE_OK) {
		return 0;
	}
	return maxAlignment;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_GetTablePtr
//--------------------------------------------------------------------------------------------------
//! \brief	Return the pointer to the type definition table, defined in RB_Config.h
//!
//! \return	Address of table
//--------------------------------------------------------------------------------------------------
const char* const * RB_TYPE_GetTablePtr(void)
{
	return RB_TYPE_Table;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_GetTableSize
//--------------------------------------------------------------------------------------------------
//! \brief	Return the size of the type definition table, defined in RB_Config.h
//!
//! \return	Size of table, i.e. number of table entries
//--------------------------------------------------------------------------------------------------
int RB_TYPE_GetTableSize(void)
{
	return (RB_ARRAY_SIZE(RB_TYPE_Table) -1);
}



//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseType
//--------------------------------------------------------------------------------------------------
//! \brief	Parse specific type to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	ppType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \param	pStruct			Pointer to actually parsed struct
//! \param	pMaxAlignment	Maximum alignment used in type, variable must be zero, when called
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseType(const char** ppInput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, void* pStruct, size_t* pMaxAlignment, bool pack)
{
	RB_TYPE_tStatus typeSts;
	const char *pType = *ppType;
	const char *pTblType;
	char parseType;
	size_t lenOfTypeName;
	size_t size = 0;
	size_t alignMax = 0;

	// Check if typestring is empty
	if (pType[0] == '\0')
		return RB_TYPE_NOT_DEFINED;

	// Get type character and skip it
	parseType = pType[0];
	pType++;
	(*ppType)++;

	// Parse input to variable
	switch (parseType) {
		case RB_TYPE_CHAR:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseCharacter (ppInput, pType, varBuffer);

		case RB_TYPE_IDENT:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseIdentifier(ppInput, pType, varBuffer);

		case RB_TYPE_STRING:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseString    (ppInput, pType, varBuffer);

		case RB_TYPE_I8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseI         (ppInput, pType, varBuffer, 1);

		case RB_TYPE_I16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			return RB_TYPE_ParseI         (ppInput, pType, varBuffer, 2);

		case RB_TYPE_I32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			return RB_TYPE_ParseI         (ppInput, pType, varBuffer, 4);

		case RB_TYPE_U8:
		case RB_TYPE_D8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 1, false);

		case RB_TYPE_U16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 2, false);

		case RB_TYPE_U32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 4, false);

		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H8:
		case RB_TYPE_X8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 1, true);

		case RB_TYPE_H16:
		case RB_TYPE_X16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 2, true);

		case RB_TYPE_H32:
		case RB_TYPE_X32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			return RB_TYPE_ParseU         (ppInput, pType, varBuffer, 4, true);
		#endif

		#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_FLOAT_E:
		case RB_TYPE_FLOAT_G:
		case RB_TYPE_FLOAT:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, pMaxAlignment);
			return RB_TYPE_ParseFloat     (ppInput, pType, varBuffer);

		case RB_TYPE_DOUBLE_E:
		case RB_TYPE_DOUBLE_G:
		case RB_TYPE_DOUBLE:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, pMaxAlignment);
			return RB_TYPE_ParseDouble    (ppInput, pType, varBuffer);
		#endif

		case RB_TYPE_PPARAM:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_PTR : RB_ALIGN_PTR, pMaxAlignment);
			*(const void **)((uintptr_t)varBuffer->pBufferStart + varBuffer->index) = *ppInput;	// Point to remaining input line
			// Skip remaining input to avoid error return
			while (**ppInput)
				*ppInput += 1;
			return RB_TYPE_OK;

		case RB_TYPE_STRUCT:
			alignMax = pack ? T2_ALIGN_STRUCT : RB_ALIGN_STRUCT;
			if (RB_TYPE_InfoOfStruct(&pType, &size, &alignMax, pack) != RB_TYPE_OK) {		// Recursion !
				return RB_TYPE_ERROR_TYPEDEF;
			}
			RB_TYPE_Align(&varBuffer->index, alignMax, pMaxAlignment);
			typeSts = RB_TYPE_ParseStruct(ppInput, ppType, varBuffer, (void *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), pMaxAlignment, pack);
			RB_TYPE_Align(&varBuffer->index, alignMax, pMaxAlignment);
			return typeSts;

		case RB_TYPE_FUNCTION:
			return RB_TYPE_ExecuteFunction(pType, pStruct);

		case RB_TYPE_TYPEDEF:
			// Find type in table
			pTblType = RB_TYPE_FindType(pType , &lenOfTypeName);
			// If type found, we parse input value(s)
			if (pTblType) {
				// Point after typename and '='
				pTblType = pTblType + lenOfTypeName + 1;
				return RB_TYPE_ParseType(ppInput, &pTblType, varBuffer, 0, pMaxAlignment, pack);       // Recursion !
			}
			else {
				return RB_TYPE_NOT_DEFINED; // Type not defined
			}

		default: return RB_TYPE_ERROR_TYPEDEF;
	} // switch
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseStruct
//--------------------------------------------------------------------------------------------------
//! \brief	Parse struct to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	ppType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \param	pMaxAlignment	Maximum alignment used in type, variable must be zero, when called
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseStruct(const char** ppInput, const char** ppType, RB_PACK_tRecBuffer* varBuffer, void* pStruct, size_t* pMaxAlignment, bool pack)
{
	#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
	const char *typeItem;
	#endif
	const char *pType = *ppType;
	RB_TYPE_tStatus sts;

	if (*pType != '{')
		return RB_TYPE_ERROR_TYPEDEF;

	// Point behind '{'
	pType++;

	// Loop over all struct elements
	while (*pType != '}') {

		// Skip spaces in type definition
		while (*pType == ' ')
			pType++;

		#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
		// Check for struct element name
		typeItem = strchr(pType, '=');
		if (typeItem) {
			// Set pointer behind '='
			typeItem++;
			pType = typeItem;
		}
		#endif

		// Parse struct element into variable
		sts = RB_TYPE_ParseType(ppInput, &pType, varBuffer, pStruct, pMaxAlignment, pack);		// Recursion !
		if (sts != RB_TYPE_OK)
			return sts;     // Error return

		// Check for separator or end of line char
		if (!(**ppInput == ' ' || **ppInput == '\0'))
			return RB_TYPE_ERROR_PARSER;

		// Skip struct element definition
		while ((*pType != ';') && (*pType != '}'))
			pType++;

		// Skip ';' in struct type definition
		if (*pType == ';')
			pType++;
	}

	// Point behind '}'
	pType++;
	*ppType = pType;

	// Struct must be terminated by ";" or NUL
	if ((*pType != ';') && (*pType != '\0'))
		return RB_TYPE_ERROR_TYPEDEF;

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseCharacter
//--------------------------------------------------------------------------------------------------
//! \brief	Parse character to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseCharacter(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer)
{
	const char* pTypeStr = pType;
	int32_t min;
	int32_t max;
	int value;

	// Skip spaces
	while (**ppInput == ' ')
		*ppInput += 1;

	// Read character
	value = (int)**ppInput;

	// Get max and min values, if available
	if (!RB_PARSE_Long(&pTypeStr, &max))
		max = 127L;
	if (!RB_PARSE_Long(&pTypeStr, &min))
		min = -128L;

	// Check type definition
	if (max > 127L)
		return RB_TYPE_ERROR_TYPEDEF;
	if (min < -128L)
		return RB_TYPE_ERROR_TYPEDEF;
	if (min > max)
		return RB_TYPE_ERROR_TYPEDEF;

	// Check max range
	if (value > max)
		return RB_TYPE_ERROR_RANGE;

   // Check min range
	if (value < min)
		return RB_TYPE_ERROR_RANGE;

	// Copy character to variable
	*(char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index) = (char)value;

	// Adjust pointers
	varBuffer->index += sizeof(char);
	*ppInput += 1;

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseIdentifier
//--------------------------------------------------------------------------------------------------
//! \brief	Parse identifier to variable.
//!
//! \param	ppInput			Input string to parse, allowed char's are A..Z, a..z, 0..9, _ and @
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseIdentifier(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer)
{
	const char* pTypeStr = pType;
	const char* input = *ppInput;
	char* variable = (char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
	uint32_t min;
	uint32_t max;
	uint32_t len = 0L;

	// Skip spaces
	while (*input == ' ')
		input++;

	// Get max and min values, if available
	if (!RB_PARSE_ULong(&pTypeStr, &max)) {
		// String type definition needs a maxlen definition
		return RB_TYPE_ERROR_TYPEDEF;
	}
	if (!RB_PARSE_ULong(&pTypeStr, &min))
		min = 0UL;

	// Check type definition
	if (min > max)
		return RB_TYPE_ERROR_TYPEDEF;

	// Copy identifier
	while(len < max) {
		if ((isalnum(*input)) || (*input == '_') || (*input == '@')) {
			// Copy characters to variable
			*variable++ = *input++;
			len++;
		}
		else {
			// Character not allowed for identifier
			break;
		}
	} // while

	// Terminate string, if not equal maxlen
	if (len < max) {
		*variable = '\0';
	}

	// Check min range
	if (len < min)
		return RB_TYPE_ERROR_RANGE;

	// Adjust pointers
	varBuffer->index += max * sizeof(char);	// Skip entire string space in struct
	*ppInput = input;

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseString
//--------------------------------------------------------------------------------------------------
//! \brief	Parse string to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseString(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer)
{
	const char* pTypeStr = pType;
	const char* input = *ppInput;
	char* variable = (char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
	uint32_t min;
	uint32_t max;
	int insideQuote = 0;
	uint32_t len = 0L;

	// Skip spaces
	while (*input == ' ')
		input++;

	// Get max and min values, if available
	if (!RB_PARSE_ULong(&pTypeStr, &max)) {
		// String type definition needs a maxlen definition
		return RB_TYPE_ERROR_TYPEDEF;
	}
	if (!RB_PARSE_ULong(&pTypeStr, &min))
		min = 0UL;

	// Check type definition
	if (min > max)
		return RB_TYPE_ERROR_TYPEDEF;

	// Copy string and substitute '\x' with one character
	if (*input == '"')
	{
		input++;                    // Skip leading quote
		insideQuote = 1;

		while(insideQuote) {
			switch(*input) {
				// End quote of parameter string
				case '"':
					// Terminate string, if not equal maxlen
					if (len < max) {
						*variable = '\0';
					}
					input++;
					insideQuote = 0;
					break;

				// Replace backslash anychar with anychar
				case '\\':
					input++;    // Skip '\'
					if (*input != '\0') {
						*variable++ = *input++;
						len++;
					}
					break;

				// End of input string -> Error, no matching quote !
				case '\0':
					return RB_TYPE_ERROR_TYPEDEF;

				// Copy characters to variable
				default:
					*variable++ = *input++;
					len++;
					break;
			} // switch

			// Check maxlen
			if (len > max) {
				return RB_TYPE_ERROR_RANGE;
			}
		} // while
	}
	else {
		return RB_TYPE_ERROR_PARSER;	// Not a quoted string
	}

	// Check min range
	if (len < min)
		return RB_TYPE_ERROR_RANGE;

	// Adjust pointers
	varBuffer->index += max * sizeof(char);	// Skip entire string space in struct
	*ppInput = input;

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseI
//--------------------------------------------------------------------------------------------------
//! \brief	Parse integer value to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \param	octets			Size of value in octets [1, 2, 4] (octet = 8 bits)
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseI(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer, size_t octets)
{
	const char* pTypeStr = pType;
	int32_t min;
	int32_t max;
	int32_t rangeLow;
	int32_t rangeHigh;
	int32_t value;
	size_t bytes;

	// Adjust for different size of char
	bytes = octets / (CHAR_BIT / 8);
	if (bytes < 1) {
		bytes = 1;
	}

	// Read value
	if (RB_PARSE_Long(ppInput, &value) == 0)
		return RB_TYPE_ERROR_PARSER;

	switch(octets) {
		case 1:
			min = -128L;
			max =  127L;
			break;
		case 2:
			min = -32768L;
			max =  32767L;
			break;
		default: // case 4
			min = LONG_MIN;
			max = LONG_MAX;
	}

	// Get rangeHigh and rangeLow values, if available
	if (!RB_PARSE_Long(&pTypeStr, &rangeHigh))
		rangeHigh = max;
	if (!RB_PARSE_Long(&pTypeStr, &rangeLow))
		rangeLow = min;

	// Check type definition
	if (rangeHigh > max)
		return RB_TYPE_ERROR_TYPEDEF;
	if (rangeLow < min)
		return RB_TYPE_ERROR_TYPEDEF;
	if (rangeLow > rangeHigh)
		return RB_TYPE_ERROR_TYPEDEF;

	// Check rangeHigh range
	if (value > rangeHigh)
		return RB_TYPE_ERROR_RANGE;

	// Check rangeLow range
	if (value < rangeLow)
		return RB_TYPE_ERROR_RANGE;

	// Copy value

	memcpy((void *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), &value, bytes);

	// Adjust pointers
	varBuffer->index += bytes;

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseU
//--------------------------------------------------------------------------------------------------
//! \brief	Parse unsigned integer value to variable.
//!
//! \param	ppInput      	Input string to parse
//! \param	pType     		Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \param	octets			Size of value in bytes [1, 2, 4] (number of octets, octet = 8 bits)
//! \param	hex				true, if hex conversion
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseU(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer, size_t octets, bool hex)
{
	const char* pTypeStr = pType;
	uint32_t max;
	uint32_t rangeLow;
	uint32_t rangeHigh;
	uint32_t value;
	size_t bytes;

	// Adjust for different size of char
	bytes = octets / (CHAR_BIT / 8);
	if (bytes < 1) {
		bytes = 1;
	}

	// Read value
	if (hex) {
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		if (RB_PARSE_Hex(ppInput, &value) == 0) {
			return RB_TYPE_ERROR_PARSER;
		}
		#else
		return RB_TYPE_ERROR_PARSER;
		#endif
	}
	else {
		if (RB_PARSE_ULong(ppInput, &value) == 0) {
			return RB_TYPE_ERROR_PARSER;
		}
	}

	switch(octets) {
		case 1:
			max = 255uL;
			break;
		case 2:
			max = 65535uL;
			break;
		default: // case 4
			max = ULONG_MAX;
	}

	// Get max and min values, if available
	if (!RB_PARSE_ULong(&pTypeStr, &rangeHigh))
		rangeHigh = max;
	if (!RB_PARSE_ULong(&pTypeStr, &rangeLow))
		rangeLow = 0UL;

	// Check type definition
	if (rangeHigh > max)
		return RB_TYPE_ERROR_TYPEDEF;
	if (rangeLow > rangeHigh)
		return RB_TYPE_ERROR_TYPEDEF;

	// Check rangeHigh
	if (value > rangeHigh)
		return RB_TYPE_ERROR_RANGE;

   // Check rangeLow
	if (value < rangeLow)
		return RB_TYPE_ERROR_RANGE;

	// Copy value
	memcpy((void *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), &value, bytes);

	// Adjust pointers
	varBuffer->index += bytes;

	return RB_TYPE_OK;
}


#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseFloat
//--------------------------------------------------------------------------------------------------
//! \brief	Parse float32 value to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseFloat(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer)
{
	float64 min;
	float64 max;
	const char *pTypeStr = pType;
	float64 value;
	float32 valueFloat;

	// Read value
	if (RB_PARSE_Double(ppInput, &value) == 0)
		return RB_TYPE_ERROR_PARSER;

	// Get max and min values, if available
	if (!RB_PARSE_Double(&pTypeStr, &max))
		max = (float64)FLT32_MAX;
	if (!RB_PARSE_Double(&pTypeStr, &min))
		min = (float64)-FLT32_MAX;

	// Check type definition
	if (max > (float64)FLT32_MAX)
		return RB_TYPE_ERROR_TYPEDEF;
	if (min < (float64)-FLT32_MAX)
		return RB_TYPE_ERROR_TYPEDEF;
	if (min > max)
		return RB_TYPE_ERROR_TYPEDEF;

	// Check max range
	if (value > max)
		return RB_TYPE_ERROR_RANGE;

   // Check min range
	if (value < min)
		return RB_TYPE_ERROR_RANGE;

	// Copy value to variable
	valueFloat = (float32)value;
	memcpy((void *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), &valueFloat, sizeof(float32));

	// Adjust variable pointer
	varBuffer->index += sizeof(float32);

	return RB_TYPE_OK;
}
#endif


#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
//--------------------------------------------------------------------------------------------------
// RB_TYPE_ParseDouble
//--------------------------------------------------------------------------------------------------
//! \brief	Parse float64 value to variable.
//!
//! \param	ppInput			Input string to parse
//! \param	pType			Type definition string
//! \param	varBuffer		Location where parsed data will be stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ParseDouble(const char** ppInput, const char* pType, RB_PACK_tRecBuffer *varBuffer)
{
	float64 min;
	float64 max;
	const char *pTypeStr = pType;
	float64 value;

	// Read value
	if (RB_PARSE_Double(ppInput, &value) == 0)
		return RB_TYPE_ERROR_PARSER;

	// Get max and min values, if available
	if (!RB_PARSE_Double(&pTypeStr, &max))
		max = (float64)LDBL_MAX;
	if (!RB_PARSE_Double(&pTypeStr, &min))
		min = (float64)-LDBL_MAX;

	// Check type definition
	if (min > max)
		return RB_TYPE_ERROR_TYPEDEF;

	// Check max range
	if (value > max)
		return RB_TYPE_ERROR_RANGE;

	// Check min range
	if (value < min)
		return RB_TYPE_ERROR_RANGE;

	// Copy value to variable
	memcpy((void *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), &value, sizeof(float64));

	// Adjust variable pointer
	varBuffer->index += sizeof(float64);

	return RB_TYPE_OK;
}
#endif


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ToStringType
//--------------------------------------------------------------------------------------------------
//! \brief	Write specified type to output string.
//!
//! \param	ppOutput		Output string pointer, points to end of string after return, i.e. '\\0'
//! \param	ppType			Type to parse
//! \param	varBuffer		Location where data to be formatted resides
//! \param	pMaxAlignment	Maximum alignment used in type, variable must be zero, when called
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ToStringType(char** ppOutput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, size_t* pMaxAlignment, bool pack)
{
	RB_TYPE_tStatus typeSts;
	const char *pType = *ppType;
	const char *pTblType;
	char parseType;
	char ch;
	size_t typeLen;
	size_t len = 0;
	size_t size = 0;
	size_t alignMax = 0;

	// Get type character and skip it
	parseType = pType[0];
	pType++;
	(*ppType)++;

	// Output variable to string
	// Simple types need a leading space
	*(*ppOutput)++ = ' ';
	switch (parseType) {

		case RB_TYPE_CHAR:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			*(*ppOutput)++ = *(char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
			**ppOutput     = '\0';
			varBuffer->index += sizeof(char);
			break;

		case RB_TYPE_IDENT:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
                        // Get maxlen of identifier
			if (RB_TYPE_InfoOfString(pType, &len, &alignMax, pack) != RB_TYPE_OK) {
				return RB_TYPE_ERROR_TYPEDEF;
			}
			while (len > 0) {
				ch = *(char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
				varBuffer->index += sizeof(char);
				if (ch == '\0') {
					while (len > 1) {
						varBuffer->index += sizeof(char);
						len--;
					}
					break;
				}
				*(*ppOutput)++ = ch;
				len--;
			}
			*(*ppOutput)   = '\0';
			break;

		case RB_TYPE_STRING:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
                        // Get maxlen of string
			if (RB_TYPE_InfoOfString(pType, &len, &alignMax, pack) != RB_TYPE_OK) {
				return RB_TYPE_ERROR_TYPEDEF;
			}
			*(*ppOutput)++ = '"';
			while (len > 0) {
				ch = *(char *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
				varBuffer->index += sizeof(char);
				if (ch == '\0') {
					while (len > 1) {
						varBuffer->index += sizeof(char);
						len--;
					}
					break;
				}
				*(*ppOutput)++ = ch;
				len--;
			}
			*(*ppOutput)++ = '"';
			*(*ppOutput)   = '\0';
			break;

		case RB_TYPE_I8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			RB_FORMAT_Long(*ppOutput, (int32_t)*(int8_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 5);
			varBuffer->index += sizeof(int8_t);
			break;

		case RB_TYPE_U8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			RB_FORMAT_ULong(*ppOutput, (uint32_t)*(uint8_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 4);
			varBuffer->index += sizeof(uint8_t);
			break;

		case RB_TYPE_D8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
				{
				uint32_t val = (uint32_t)*(uint8_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index);
				if (val < 10u)
					{
					*(*ppOutput)++ = '0'; // add leading 0 for values 0..9
					*(*ppOutput)   = '\0';
					}
				RB_FORMAT_ULong(*ppOutput, val, 4);
				}
			varBuffer->index += sizeof(uint8_t);
			break;

		case RB_TYPE_I16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			RB_FORMAT_Long(*ppOutput, (int32_t)*(int16_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 7);
			varBuffer->index += sizeof(int16_t);
			break;

		case RB_TYPE_U16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			RB_FORMAT_ULong(*ppOutput, (uint32_t)*(uint16_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 6);
			varBuffer->index += sizeof(uint16_t);
			break;

		case RB_TYPE_I32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			RB_FORMAT_Long(*ppOutput, *(int32_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 12);
			varBuffer->index += sizeof(int32_t);
			break;

		case RB_TYPE_U32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			RB_FORMAT_ULong(*ppOutput, *(uint32_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 11);
			varBuffer->index += sizeof(uint32_t);
			break;

		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H8:
		case RB_TYPE_X8:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			RB_FORMAT_Hex(*ppOutput, (uint32_t)*(uint8_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 5u, 2u, (parseType == RB_TYPE_X8));
			varBuffer->index += sizeof(uint8_t);
			break;

		case RB_TYPE_H16:
		case RB_TYPE_X16:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			RB_FORMAT_Hex(*ppOutput, (uint32_t)*(uint16_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 7u, 4u, (parseType == RB_TYPE_X16));
			varBuffer->index += sizeof(uint16_t);
			break;

		case RB_TYPE_H32:
		case RB_TYPE_X32:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			RB_FORMAT_Hex(*ppOutput,*(uint32_t *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 11u, 8u, (parseType == RB_TYPE_X32));
			varBuffer->index += sizeof(uint32_t);
			break;
		#endif

		#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_FLOAT:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, pMaxAlignment);
			RB_FORMAT_DoubleToEFormat(*ppOutput, 7, (float64)*(float32 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 20);
			varBuffer->index += sizeof(float32);
			break;

		case RB_TYPE_FLOAT_E:	// Scientific format e
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, pMaxAlignment);
			RB_FORMAT_Float32ToEFormatSICS(*ppOutput, *(float32 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 22);
			varBuffer->index += sizeof(float32);
			break;

		case RB_TYPE_FLOAT_G:	// Normal format g
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, pMaxAlignment);
			RB_FORMAT_DoubleToGFormatSICS(ppOutput, (float64)*(float32 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 7);
			varBuffer->index += sizeof(float32);
			break;

		case RB_TYPE_DOUBLE:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, pMaxAlignment);
			RB_FORMAT_DoubleToEFormat(*ppOutput, 14, *(float64 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 25);
			varBuffer->index += sizeof(float64);
			break;

		case RB_TYPE_DOUBLE_E:	// Scientific format E
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, pMaxAlignment);
			RB_FORMAT_Float64ToEFormatSICS(*ppOutput, *(float64 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 28);
			varBuffer->index += sizeof(float64);
			break;

		case RB_TYPE_DOUBLE_G:	// Normal format G
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, pMaxAlignment);
			RB_FORMAT_DoubleToGFormatSICS(ppOutput, *(float64 *)((uintptr_t)varBuffer->pBufferStart + varBuffer->index), 16);
			varBuffer->index += sizeof(float64);
			break;
		#endif

		case RB_TYPE_PPARAM:
			RB_TYPE_Align(&varBuffer->index, pack ? T2_ALIGN_PTR : RB_ALIGN_PTR, pMaxAlignment);
			*(*ppOutput)++ = '*';
			*(*ppOutput)++ = '\0';
			break;

		default:
			// Complex types need no leading space, remove previously set space for simple types
			*--(*ppOutput) = '\0';
			switch (parseType) {
				case RB_TYPE_STRUCT:
					alignMax = RB_ALIGN_STRUCT;
					if (RB_TYPE_InfoOfStruct(&pType, &size, &alignMax, pack) != RB_TYPE_OK) {		// Recursion !
						return RB_TYPE_ERROR_TYPEDEF;
					}
					RB_TYPE_Align(&varBuffer->index, alignMax, pMaxAlignment);
					typeSts = RB_TYPE_ToStringStruct(ppOutput, ppType, varBuffer, pMaxAlignment, pack);
					RB_TYPE_Align(&varBuffer->index, alignMax, pMaxAlignment);
					return typeSts;

				case RB_TYPE_FUNCTION:
					return RB_TYPE_OK;

				case RB_TYPE_TYPEDEF:
					// Find type in table
					pTblType = RB_TYPE_FindType(pType , &typeLen);
					// If type found, we output type element value(s)
					if (pTblType) {
						// Point after typename and '='
						pTblType = pTblType + typeLen + 1;
						return RB_TYPE_ToStringType(ppOutput, &pTblType, varBuffer, pMaxAlignment, pack);       // Recursion !
					}
					else {
						return RB_TYPE_NOT_DEFINED; // Type not defined
					}
				default: return RB_TYPE_ERROR_TYPEDEF;
			} // switch
	} // switch

	*ppOutput += strlen(*ppOutput);
	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ToStringStruct
//--------------------------------------------------------------------------------------------------
//! \brief	Write specified type to output string.
//!
//! \param	ppOutput      	Output string pointer, points to end of string after return, i.e. '\\0'
//! \param	ppType     		Type to parse
//! \param	varBuffer		Location where data to be formatted resides
//! \param	pMaxAlignment	Maximum alignment used in type, variable must be zero, when called
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ToStringStruct(char** ppOutput, const char** ppType, RB_PACK_tRecBuffer *varBuffer, size_t* pMaxAlignment, bool pack)
{
	#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
	const char *typeItem;
	#endif
	const char *pType = *ppType;
	size_t alignMax = 0;
	RB_TYPE_tStatus sts;

	if (*pType != '{')
		return RB_TYPE_ERROR_TYPEDEF;

	// Point behind '{'
	pType++;

	// Loop over all struct elements
	while (*pType != '}') {

		// Skip spaces in type definition
		while (*pType == ' ') {
			pType++;
		}

		#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
		// Check for struct element name
		typeItem = strchr(pType, '=');
		if (typeItem) {
			// Set pointer behind '='
			typeItem++;
			pType = typeItem;
		}
		#endif

		// Write struct element into string
		sts = RB_TYPE_ToStringType(ppOutput, &pType, varBuffer, pMaxAlignment, pack);          // Recursion !
		if (sts != RB_TYPE_OK) {
			return sts;     // Error return
		}

		// Evaluate maximum alignment
		if (alignMax > *pMaxAlignment) {
			*pMaxAlignment = alignMax;
		}

		// Skip struct element definition
		while ((*pType != ';') && (*pType != '}')) {
			pType++;
		}

		// Skip ';' in struct type definition
		if (*pType == ';') {
			pType++;
		}
	}

	// Point behind '}'
	pType++;
	*ppType = pType;

	// Struct must be terminated by ";" or NUL
	if ((*pType != ';') && (*pType != '\0')) {
		return RB_TYPE_ERROR_TYPEDEF;
	}

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_InfoOfType
//--------------------------------------------------------------------------------------------------
//! \brief	Return size and maximum alignment of the specified type.
//!
//! \param	ppType			Type definition string
//! \param	pSize			Size of type in bytes, variable must be zero, when called
//! \param	pMaxAlignment	Maximum alignment used in type, variable must be zero, when called
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_InfoOfType(const char** ppType, size_t* pSize, size_t* pMaxAlignment, bool pack)
{
	const char *pType = *ppType;
	const char *pTblType;
	char parseType;
	size_t lenOfTypeName;
	size_t size = 0;
	size_t alignMax = 0;

	// Get type character and skip it
	parseType = pType[0];
	pType++;
	(*ppType)++;

	// Evaluate info of type element
	switch (parseType) {
		case RB_TYPE_CHAR:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			*pSize += pack ?  T2_OCTETS_D8 : sizeof(char);
			break;

		case RB_TYPE_IDENT:
		case RB_TYPE_STRING:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			if (RB_TYPE_InfoOfString(pType, pSize, pMaxAlignment, pack) != RB_TYPE_OK)
				return RB_TYPE_ERROR_STR_LEN;
			break;

		case RB_TYPE_I8:
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H8:
		case RB_TYPE_X8:
		#endif
		case RB_TYPE_U8:
		case RB_TYPE_D8:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_D8 : sizeof(uint8_t);
			break;

		case RB_TYPE_I16:
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H16:
		case RB_TYPE_X16:
		#endif
		case RB_TYPE_U16:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_D16 : sizeof(uint16_t);
			break;

		case RB_TYPE_I32:
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H32:
		case RB_TYPE_X32:
		#endif
		case RB_TYPE_U32:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_D32 : sizeof(uint32_t);
			break;

		#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_FLOAT:
		case RB_TYPE_FLOAT_E:
		case RB_TYPE_FLOAT_G:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_F32 : sizeof(float32);
			break;

		case RB_TYPE_DOUBLE:
		case RB_TYPE_DOUBLE_E:
		case RB_TYPE_DOUBLE_G:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_F64 : sizeof(float64);
			break;
		#endif

		case RB_TYPE_PPARAM:
			RB_TYPE_Align(pSize, pack ? T2_ALIGN_PTR : RB_ALIGN_PTR, pMaxAlignment);
			*pSize += pack ? T2_OCTETS_PTR : sizeof(void*);
			break;

		case RB_TYPE_STRUCT:
			alignMax = pack ? T2_ALIGN_STRUCT : RB_ALIGN_STRUCT;
			if (RB_TYPE_InfoOfStruct(ppType, &size, &alignMax, pack) != RB_TYPE_OK) {		// Recursion !
				return RB_TYPE_ERROR_TYPEDEF;
			}
			RB_TYPE_Align(pSize, alignMax, pMaxAlignment);
			*pSize += size;

			// Do not consider trailing padding in packed structs.
			// If packed, do not add trailing padding but leave size as is.
			// If unpacked, add trailing padding to make the size dividable by the alignment requirement
			RB_TYPE_Align(pSize, pack ? 1 : alignMax, pMaxAlignment);
			break;

		case RB_TYPE_FUNCTION:
			break;

		case RB_TYPE_TYPEDEF:
			// Find type in table
			pTblType = RB_TYPE_FindType(pType , &lenOfTypeName);
			// If type found, we parse input value(s)
			if (pTblType) {
				// Point after typename and '='
				pTblType = pTblType + lenOfTypeName + 1;
				if (RB_TYPE_InfoOfType(&pTblType, pSize, pMaxAlignment, pack) != RB_TYPE_OK) {	// Recursion !
					return RB_TYPE_ERROR_TYPEDEF;  // Error in type definition
				}
				else {
					break;
				}
			}
			else {
				return RB_TYPE_NOT_DEFINED;
			}

		default: return RB_TYPE_ERROR_TYPEDEF;  // Error in type definition
	} // switch

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_InfoOfStruct
//--------------------------------------------------------------------------------------------------
//! \brief	Return size and maximum alignment of the specified struct.
//!
//! \param	ppType			Type definition string
//! \param	pSize			Size of type in bytes
//! \param	pMaxAlignment	Maximum alignment used in struct
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_InfoOfStruct(const char** ppType, size_t* pSize, size_t* pMaxAlignment, bool pack)
{
	#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
	const char *typeItem;
	#endif
	const char *pType = *ppType;
	size_t alignMax = 0;
	RB_TYPE_tStatus retValue;

	if (*pType != '{')
		return RB_TYPE_ERROR_TYPEDEF;

	// Point behind '{'
	pType++;

	// Loop over all struct elements
	while (*pType != '}') {

		// Skip spaces in type definition
		while (*pType == ' ') {
			pType++;
		}

		#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
		// Check for struct element name
		typeItem = strchr(pType, '=');
		if (typeItem) {
			// Set pointer behind '='
			typeItem++;
			pType = typeItem;
		}
		#endif

		// Evaluate info of type element
		retValue = RB_TYPE_InfoOfType(&pType, pSize, &alignMax, pack);         // Recursion !
		if (retValue != RB_TYPE_OK) {
			return retValue;        // Error return
		}

		// Evaluate maximum alignment
		if (alignMax > *pMaxAlignment) {
			*pMaxAlignment = alignMax;
		}

		// Skip struct element definition
		while ((*pType != ';') && (*pType != '}')) {
			pType++;
		}

		// Skip ';' in struct type definition
		if (*pType == ';') {
			pType++;
		}
	}

	// Point behind '}'
	pType++;
	*ppType = pType;

	// Struct must be terminated by ";" or NUL
	if ((*pType != ';') && (*pType != '\0')) {
		return RB_TYPE_ERROR_TYPEDEF;
	}

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_InfoOfString
//--------------------------------------------------------------------------------------------------
//! \brief	Return size and maximum alignment of the specified string.
//!
//! \param	ppType			Type definition string
//! \param	pSize			Size of type in bytes
//! \param	pMaxAlignment	Maximum alignment used in string
//! \param	pack			true = packed, false = unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_InfoOfString(const char* pType, size_t* pSize, size_t* pMaxAlignment, bool pack)
{
	const char* pTypeStr = pType;
	uint32_t max;
	int rangeCheck;

	*pMaxAlignment = pack ? T2_ALIGN_D8 : RB_ALIGN_D8;

	// Get maxlen value, must be available to check size
	rangeCheck = RB_PARSE_ULong(&pTypeStr, &max);

	// Check type definition
	if (rangeCheck < 1)
		return RB_TYPE_ERROR_STR_LEN;  // Size not determinable, no maxlen value defined

	// Update size
	*pSize = *pSize + (pack ? T2_OCTETS_D8 * max : max);

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_CopyOfType
//--------------------------------------------------------------------------------------------------
//! \brief	Copy the specified type.
//!
//! \param	ppType			Type definition string
//! \param	dstBuffer		Destination of copy operation
//! \param	srcBuffer		Source of copy operation
//! \param	reqEndianess	Endianess of Tigris2 buffer
//! \param	pack			true = unpacked-->Tigris2, false = Tigris2-->unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_CopyOfType(const char** ppType, RB_PACK_tRecBuffer *dstBuffer, RB_PACK_tRecBufferConst *srcBuffer, RB_TYPEDEFS_tEndianess reqEndianess, bool pack)

{
	const char *pType = *ppType;
	const char *pTblType;
	char parseType;
	size_t lenOfTypeName;
	size_t size = 0;
	size_t alignMaxSrc = 0;
	size_t alignMaxDst = 0;
	size_t i = 0;
	bool sign = false; // Set to true if a signed integral type is processed

	// Get type character and skip it
	parseType = pType[0];
	pType++;
	(*ppType)++;

	// Evaluate info of type element
	switch (parseType) {
		case RB_TYPE_CHAR:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_D8 : T2_ALIGN_D8, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_D8, sizeof(char), sign, pack);
			break;

		case RB_TYPE_IDENT:
		case RB_TYPE_STRING:
			if (RB_TYPE_InfoOfString(pType, &size, &alignMaxSrc, pack ? false : true) != RB_TYPE_OK) {
				return RB_TYPE_ERROR_STR_LEN;
			}
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_D8 : T2_ALIGN_D8, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, &alignMaxDst);

			if (!pack) {
				// When unpacking data from Tigris II format to CPU format:
				// Write terminating zero to avoid not terminated string.
				// If stringsize is equal buffer size, e.g. if typespecification is s18 and string
				// ... is actually 18 char long, there is no terminating zero stored in NVMem.
				// ATTENTION! Buffersize for strings must always be one char longer than typespecification!
				*((char*)dstBuffer->pBufferStart + dstBuffer->index + size) = '\0';
			}
			for (i = 0; i < size; i++) {
				RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_D8, sizeof(char), sign, pack);
			}
			break;

		case RB_TYPE_I8:
			sign = true;
			//lint -fallthrough
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H8:
		case RB_TYPE_X8:
		#endif
		case RB_TYPE_U8:
		case RB_TYPE_D8:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_D8 : T2_ALIGN_D8, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_D8 : RB_ALIGN_D8, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_D8, sizeof(uint8_t), sign, pack);
			break;

		case RB_TYPE_I16:
			sign = true;
			//lint -fallthrough
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H16:
		case RB_TYPE_X16:
		#endif
		case RB_TYPE_U16:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_D16 : T2_ALIGN_D16, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_D16 : RB_ALIGN_D16, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_D16, sizeof(uint16_t), sign, pack);
			break;

		case RB_TYPE_I32:
			sign = true;
			//lint -fallthrough
		#if defined(RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED) && (RB_CONFIG_TYPE_HEX_VALUES_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_H32:
		case RB_TYPE_X32:
		#endif
		case RB_TYPE_U32:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_D32 : T2_ALIGN_D32, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_D32 : RB_ALIGN_D32, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_D32, sizeof(uint32_t), sign, pack);
			break;

		#if defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) && (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_YES)
		case RB_TYPE_FLOAT:
		case RB_TYPE_FLOAT_E:
		case RB_TYPE_FLOAT_G:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_F32 : T2_ALIGN_F32, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_F32 : RB_ALIGN_F32, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_F32, sizeof(float32), sign, pack);
			break;

		case RB_TYPE_DOUBLE:
		case RB_TYPE_DOUBLE_E:
		case RB_TYPE_DOUBLE_G:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_F64 : T2_ALIGN_F64, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_F64 : RB_ALIGN_F64, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_F64, sizeof(float64), sign, pack);
			break;
		#endif

		case RB_TYPE_PPARAM:
			RB_TYPE_Align(&srcBuffer->index, pack ? RB_ALIGN_PTR : T2_ALIGN_PTR, &alignMaxSrc);
			RB_TYPE_Align(&dstBuffer->index, pack ? T2_ALIGN_PTR : RB_ALIGN_PTR, &alignMaxDst);
			RB_PACK_CopyOfSimpleType(dstBuffer, srcBuffer, reqEndianess, T2_OCTETS_PTR, sizeof(void *), sign, pack);
			break;

		case RB_TYPE_STRUCT: {
				const char *pTypeOriginal = *ppType;
				// Align source pointer
				alignMaxSrc = pack ? RB_ALIGN_STRUCT : T2_ALIGN_STRUCT;
				if (RB_TYPE_InfoOfStruct(&pType, &size, &alignMaxSrc, pack ? false : true) != RB_TYPE_OK) {		// Recursion !
					return RB_TYPE_ERROR_TYPEDEF;
				}
				pType = pTypeOriginal;
				RB_TYPE_Align(&srcBuffer->index, alignMaxSrc, &alignMaxSrc);

				// Align destination pointer
				alignMaxDst = pack ? T2_ALIGN_STRUCT : RB_ALIGN_STRUCT;
				if (RB_TYPE_InfoOfStruct(&pType, &size, &alignMaxDst, pack ? true : false) != RB_TYPE_OK) {		// Recursion !
					return RB_TYPE_ERROR_TYPEDEF;
				}
				RB_TYPE_Align(&dstBuffer->index, alignMaxDst, &alignMaxDst);

				// Copy struct, consume ppType
				RB_TYPE_CopyOfStruct(ppType, dstBuffer, srcBuffer, reqEndianess, pack);

				// Do not consider trailing padding in packed structs.
				// If pack, do not skip trailing padding in destination.
				// if unpack, do not skip trailing padding in source
				RB_TYPE_Align(&srcBuffer->index, pack ? alignMaxSrc : 1, &alignMaxSrc);
				// Align destination pointer
				RB_TYPE_Align(&dstBuffer->index, pack ? 1 : alignMaxDst, &alignMaxDst);
			}
			break;

		case RB_TYPE_FUNCTION:
			break;

		case RB_TYPE_TYPEDEF:
			// Find type in table
			pTblType = RB_TYPE_FindType(pType , &lenOfTypeName);
			// If type found, we parse input value(s)
			if (pTblType) {
				// Point after typename and '='
				pTblType = pTblType + lenOfTypeName + 1;
				if (RB_TYPE_CopyOfType(&pTblType, dstBuffer, srcBuffer, reqEndianess, pack) != RB_TYPE_OK) {	// Recursion !
					return RB_TYPE_ERROR_TYPEDEF;  // Error in type definition
				}
				else {
					break;
				}
			}
			else {
				return RB_TYPE_NOT_DEFINED;
			}

		default: return RB_TYPE_ERROR_TYPEDEF;  // Error in type definition
	} // switch

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_CopyOfStruct
//--------------------------------------------------------------------------------------------------
//! \brief	Copy the specified struct, with all it's elements.
//!
//! \param	ppType			Type definition string
//! \param	dstBuffer		Destination of copy operation
//! \param	srcBuffer		Source of copy operation
//! \param	reqEndianess	Endianess of Tigris2 buffer
//! \param	pack			true = unpacked-->Tigris2, false = Tigris2-->unpacked
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_CopyOfStruct(const char** ppType, RB_PACK_tRecBuffer *dstBuffer, RB_PACK_tRecBufferConst *srcBuffer, RB_TYPEDEFS_tEndianess reqEndianess, bool pack)
{
	#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
	const char *typeItem;
	#endif
	const char *pType = *ppType;
	RB_TYPE_tStatus retValue;

	if (*pType != '{')
		return RB_TYPE_ERROR_TYPEDEF;

	// Point behind '{'
	pType++;

	// Loop over all struct elements
	while (*pType != '}') {

		// Skip spaces in type definition
		while (*pType == ' ') {
			pType++;
		}

		#if defined(RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED) && (RB_CONFIG_TYPE_STRUCT_ELEM_NAMES_SUPPORTED == RB_CONFIG_YES)
		// Check for struct element name
		typeItem = strchr(pType, '=');
		if (typeItem) {
			// Set pointer behind '='
			typeItem++;
			pType = typeItem;
		}
		#endif

		// Copy type element
		retValue = RB_TYPE_CopyOfType(&pType, dstBuffer, srcBuffer, reqEndianess, pack);         // Recursion !
		if (retValue != RB_TYPE_OK) {
			return retValue;        // Error return
		}

		// Skip struct element definition
		while ((*pType != ';') && (*pType != '}')) {
			pType++;
		}

		// Skip ';' in struct type definition
		if (*pType == ';') {
			pType++;
		}
	}

	// Point behind '}'
	pType++;
	*ppType = pType;

	// Struct must be terminated by ";" or NUL
	if ((*pType != ';') && (*pType != '\0')) {
		return RB_TYPE_ERROR_TYPEDEF;
	}

	return RB_TYPE_OK;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_ExecuteFunction
//--------------------------------------------------------------------------------------------------
//! \brief	Execute function.
//!
//! \param	pTypeStr    	Type definition string
//! \param	pVariable   	Location where parsed data are stored
//! \return	RB_TYPE_tStatus
//--------------------------------------------------------------------------------------------------
static RB_TYPE_tStatus RB_TYPE_ExecuteFunction(const char* pTypeStr, void* pVariable)
{
	int32_t functionNumber;

	// Read function number
	if (RB_PARSE_Long(&pTypeStr, &functionNumber) == 0)
		return RB_TYPE_ERROR_PARSER;

	// Execute function
	if (RB_TYPE_FunctionTbl[functionNumber](pVariable))
		return RB_TYPE_OK;
	else
		return RB_TYPE_ERROR_RANGE;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_FindType
//--------------------------------------------------------------------------------------------------
//! \brief	Find type definition by name in type definition table.
//!
//! \param	typeName        Type to parse
//! \param	lenOfTypeName   Returned length of typename
//! \return	pointer to type definition
//--------------------------------------------------------------------------------------------------
static const char* RB_TYPE_FindType(const char* pTypeName, size_t* pLenOfTypeName)
{
	int type = 0;
	size_t typeNameLen;

	while (strlen(RB_TYPE_Table[type]) > 0U) {
		// Find first occurrence of '=', ';', '}' or ' '
		*pLenOfTypeName = strcspn(RB_TYPE_Table[type], "=;} ");
		typeNameLen = strcspn(pTypeName, "=;} ");
		if (*pLenOfTypeName == typeNameLen) {
			// Look for identical type name
			if (strncmp(RB_TYPE_Table[type], pTypeName, *pLenOfTypeName) == 0)
					// Found, return pointer to table entry
					return RB_TYPE_Table[type];
		}
		type++;
	}
	return NULL;
}


//--------------------------------------------------------------------------------------------------
// RB_TYPE_Align
//--------------------------------------------------------------------------------------------------
//! \brief	Align value to the next aligned value.
//!
//! \param	value			Returned value adjusted to next alignment
//! \param	alignTo			Align value [1, 2, 4, 8]
//! \param	pMaxAlignment	Maximum alignment
//! \return	pointer to type definition
//--------------------------------------------------------------------------------------------------
static void RB_TYPE_Align(size_t* value, size_t alignTo, size_t* pMaxAlignment)
{
	// Avoid NULL pointer (0 results in NULL pointer)
	if (alignTo == 0)
		return;

	*value = ((*value) + (alignTo - 1u)) & ~(alignTo - 1u);

	// Adjust alignment
	if (alignTo > *pMaxAlignment) {
		*pMaxAlignment = alignTo;
	}
}


#endif // RB_CONFIG_USE_TYPE
