//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Pack.h
//! \ingroup	util
//! \brief		Swap and copy of types
//!
//! Swap and copy for simple type operations, excluded from RB_Type for small systems.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2016/11/10 08:58:40MEZ $
// $Revision: 1.21 $
//
//==================================================================================================

#ifndef _RB_Pack__h
#define _RB_Pack__h


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

// Types used for recursion when copying / parsing / formating types.
typedef struct {
	void *pBufferStart; // Start of buffer (where buffer is not constant). Do NOT modify once set up.
	size_t index; // Current index into buffer. Address of current position in buffer: ((uintptr_t)pBufferStart + index)
} RB_DECL_TYPE RB_PACK_tRecBuffer;

typedef struct {
	const void *pBufferStart; // Start of buffer (where buffer is constant). Do NOT modify once set up.
	size_t index; // Current index into buffer. Address of current position in buffer: ((uintptr_t)pBufferStart + index)
} RB_DECL_TYPE RB_PACK_tRecBufferConst;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_PACK_SwapEndianessOfSimpleType
//--------------------------------------------------------------------------------------------------
//! \brief	Reverse the byte order in memory, i.e convert little to big endian and vice versa.
//!
//! \param	reqEndianess    Requested endianess, if same as CPU, no endianess swap is done
//! \param	size            Size of data in bytes, only 2 or 4 or 8 are allowed
//! \param	pVariable       Address of data (int, int32_t, float32, float64)
//!
//! \par Example code:
//! \code
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPE_LITTLE_ENDIAN, sizeof(int), (void*)MyInt);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPE_LITTLE_ENDIAN, sizeof(int32_t), (void*)MyLong);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPE_LITTLE_ENDIAN, sizeof(float32), (void*)MyFloat);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPE_LITTLE_ENDIAN, sizeof(float64), (void*)MyDouble);
//! \endcode
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_tEndianess reqEndianess, uint16_t size, void* pVariable);


//--------------------------------------------------------------------------------------------------
// RB_PACK_CopyOfSimpleType
//--------------------------------------------------------------------------------------------------
//! \brief	Copy simple type from target format to Tigris2 compatible format or vice versa
//!
//! Will also work on machines where 1 byte contains a multiple of 8 bits.
//! On such machines, every Tigris2 format payload octet will occupy the lower 8 bits of a separate target byte.
//! The function will split up target bytes into T2 octets or combine T2 octets to form target bytes.
//!
//! \param	pDst			Destination of copy operation
//! \param	pSrc			Source of copy operation
//! \param	packedEndianess	Endianess of Tigris2 buffer
//! \param		nT2octets		Number of Tigris2 format octets used by simple type
//! \param	nTargetBytes	Number of target bytes used by simple type
//! \param	sign			true = signed integer data type, false = unsigned integer data type
//! \param	pack			true = unpacked-->Tigris2, false = Tigris2-->unpacked
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_PACK_CopyOfSimpleType(RB_PACK_tRecBuffer *pDst, RB_PACK_tRecBufferConst *pSrc, RB_TYPEDEFS_tEndianess packedEndianess, size_t nT2octets, size_t nTargetBytes, bool sign, bool pack);


#ifdef __cplusplus
}
#endif

#endif //  _RB_Pack__h
