//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Pack.c
//! \ingroup	util
//! \brief		Swap and copy of types
//!
//! Swap and copy for simple type operations, excluded from RB_Type for small systems.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Werner Langenegger
//
// $Date: 2017/02/07 07:19:55MEZ $
// $Revision: 1.29 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Pack"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Pack.h"
// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

#include "RB_Debug.h"

#include <string.h>


//==================================================================================================
//  L O C A L   T Y P E S
//==================================================================================================

//! Type for changing endianess
typedef struct {
	uint8_t  b0;
	uint8_t  b1;
	uint8_t  b2;
	uint8_t  b3;
	uint8_t  b4;
	uint8_t  b5;
	uint8_t  b6;
	uint8_t  b7;
} tSwapEndianess;



//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
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
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, sizeof(int), (void*)MyInt);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, sizeof(int32_t), (void*)MyLong);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, sizeof(float32), (void*)MyFloat);
//! RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, sizeof(float64), (void*)MyDouble);
//! \endcode
//--------------------------------------------------------------------------------------------------
void RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_tEndianess reqEndianess, uint16_t size, void* pVariable)
{
	uint8_t  temp;
	tSwapEndianess* pSwap;
	RB_TYPEDEFS_tEndianess actEndianess;

#if (RB_ENDIANNESS == RB_ENDIANESS_LITTLE)
	actEndianess = RB_TYPEDEFS_ENDIANESS_LITTLE;
#elif (RB_ENDIANNESS == RB_ENDIANESS_BIG)
	actEndianess = RB_TYPEDEFS_ENDIANESS_BIG;
#endif

	if (reqEndianess == actEndianess) {
		// Nothing to swap
		return;
	}

	pSwap = (tSwapEndianess*)pVariable;
	switch (size) {
		case 2:
			temp =      pSwap->b0;
			pSwap->b0 = pSwap->b1;
			pSwap->b1 = temp;
			break;

		case 4:
			temp =      pSwap->b0;
			pSwap->b0 = pSwap->b3;
			pSwap->b3 = temp;
			temp =      pSwap->b1;
			pSwap->b1 = pSwap->b2;
			pSwap->b2 = temp;
			break;

		case 8:
			temp =      pSwap->b0;
			pSwap->b0 = pSwap->b7;
			pSwap->b7 = temp;
			temp =      pSwap->b1;
			pSwap->b1 = pSwap->b6;
			pSwap->b6 = temp;
			temp =      pSwap->b2;
			pSwap->b2 = pSwap->b5;
			pSwap->b5 = temp;
			temp =      pSwap->b3;
			pSwap->b3 = pSwap->b4;
			pSwap->b4 = temp;
			break;

		// Other values make no sense, we do nothing
		default:
			break;
	}
}


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
void RB_PACK_CopyOfSimpleType(RB_PACK_tRecBuffer *pDst, RB_PACK_tRecBufferConst *pSrc, RB_TYPEDEFS_tEndianess packedEndianess, size_t nT2octets, size_t nTargetBytes, bool sign, bool pack)
{
#if CHAR_BIT > 8
	// 1 byte multiple of 8 bits.
	// On target machine, less bytes than octets are used up by the value to be copied.
	// Must split target bytes into single octets or combine single octets to target bytes respectively

	if (pack) {
		// Target --> Tigris-2. Split target bytes into single T2 octets.
		// Every octet in the source buffer will be placed in a single byte in the destination buffer (to lower 8 bits of byte).

		size_t i, k;
		char *pDestination = (char *)((uintptr_t)pDst->pBufferStart + pDst->index);
		char tmpBuf[sizeof(float64)]; // large enough to fit all simple types

		// Copy source bytes to be swapped to little endian byte order
		if (sizeof(tmpBuf) < nTargetBytes) {
			RB_DEBUG_FAIL("unpacked simple type too large");
			return;
		}
		memcpy(tmpBuf, (void *)((uintptr_t)pSrc->pBufferStart + pSrc->index), nTargetBytes);

		// Make sure that target bytes are now in little endian order. This is required for next steps.
		RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, nTargetBytes, tmpBuf);
		// Copied target source value is now in little endian byte order

		// Split up source value's bytes, such that each octet of source value occupies one byte's lower 8 bits in the destination buffer.
		for (i = 0; i < nTargetBytes; i++) {
			size_t remOctets = nT2octets - i * (CHAR_BIT / 8); // remaining T2 format octets to process
			// Octets to process in inner loop to split target byte
			size_t currentOctets = remOctets >= (CHAR_BIT / 8) ? (CHAR_BIT / 8) : remOctets;
			// Split up target byte in octets
			// Take least significant octet first
			for (k = 0; k < currentOctets; k++) {
				*(char *)((uintptr_t)pDst->pBufferStart + pDst->index) = 0;
				*(char *)((uintptr_t)pDst->pBufferStart + pDst->index) = *(char *)((uintptr_t)pDst->pBufferStart + pDst->index) | ((tmpBuf[i] >> (k * 8)) & 0xff);
				pDst->index++; // set next destination T2 buffer entry
			}
			pSrc->index++; // consume source unpacked buffer
		}

		// Tigris 2 format destination buffer is now in little endian order
		// Swap endianess to desired T2 target buffer endianess
		if (packedEndianess != RB_TYPEDEFS_ENDIANESS_LITTLE) {
			// Swap T2 buffer to big endian order if required so.
			uint8_t tmp;
			for (i = 0; i < nT2octets / 2; i++) {
				tmp = pDestination[i];
				pDestination[i] = pDestination[nT2octets - i - 1];
				pDestination[nT2octets - i - 1] = tmp;
			}
		}

	} else {
		// Tigris-2 --> Target. Combine single T2 octets to fill up all octets in a target byte.
		// Only use the lower 8 bits of every byte in the T2 format source buffer to get the data octets.
		// Assemble target bytes by combining the octets from the source to form bytes in the destination buffer.

		size_t i, k;
		char *pDestination = (char *)((uintptr_t)pDst->pBufferStart + pDst->index);
		bool msBitIsOne = false;

		char tmpBuf[sizeof(float64) * (CHAR_BIT / 8)]; // large enough to fit all octets of simple types

		// Copy packed source to be endianess swapped
		if (sizeof(tmpBuf) < nT2octets) {
			RB_DEBUG_FAIL("T2 packed simple type too large");
			return;
		}
		memcpy(tmpBuf, (void *)((uintptr_t)pSrc->pBufferStart + pSrc->index), nT2octets);

		// Swap T2 source buffer to be in little endian order
		if (packedEndianess != RB_TYPEDEFS_ENDIANESS_LITTLE) {
			// Swap T2 buffer to little endian order
			uint8_t tmp;
			for (i = 0; i < nT2octets / 2; i++) {
				tmp = tmpBuf[i];
				tmpBuf[i] = tmpBuf[nT2octets - i - 1];
				tmpBuf[nT2octets - i - 1] = tmp;
			}
		}
		// T2 packed buffer in tmpBuf is now in little endian order

		// Check if most significant bit of T2 value is a 0 or a 1.
		// This is required for sign extension of signed int types where not a whole target byte will be used up
		// as tmpBuf is in little endian order, the last index will be
		// the most significant octet.
		if (tmpBuf[nT2octets - 1] & 0x80) {
			msBitIsOne = true;
		} else {
			msBitIsOne = false;
		}
		// msBitIsOne contains the most significant bit of the T2 packed value

		// Combine octets in source to form one destination (target) byte
		for (i = 0; i < nTargetBytes; i++) {
			size_t remOctets = nT2octets - i * (CHAR_BIT / 8); // remaining T2 format octets to process
			// Octets to process in inner loop to fill target byte
			size_t currentOctets = remOctets >= (CHAR_BIT / 8) ? (CHAR_BIT / 8) : remOctets;
			if (sign && msBitIsOne && (remOctets < (CHAR_BIT / 8))) {
				// Cannot fill a whole target byte with remaining T2 octets
				// Must extend sign of value in target byte if type is signed integral and
				// its most significant bit is a 1.
				// Set all unused most significant bits to 1 to extend sign
				*(char *)((uintptr_t)pDst->pBufferStart + pDst->index) = 0xFFFFFFFF << (remOctets * 8)  ;
			} else {
				// no signed type or positive number. Initialize to 0.
				*(char *)((uintptr_t)pDst->pBufferStart + pDst->index) = 0;
			}
			for (k = 0; k < currentOctets; k++) {
				// T2 data is in little endian order, T2 octets at lower address will be placed as lower 8 bits in target byte.
				// Subsequent octets will be shifted left by 8 and ored until target byte is filled
				// Least significant T2 octet will be first
				*(char *)((uintptr_t)pDst->pBufferStart + pDst->index) = *(char *)((uintptr_t)pDst->pBufferStart + pDst->index) | ((tmpBuf[i * (CHAR_BIT / 8) + k] & 0xff) << (k * 8));
				pSrc->index++; // consume source packed buffer
			}
			pDst->index++; // set next destination unpacked buffer entry
		}

		// Target format pDst buffer is now in little endian order
		// Swap destination buffer to target's endianness
		RB_PACK_SwapEndianessOfSimpleType(RB_TYPEDEFS_ENDIANESS_LITTLE, (uint16_t)nTargetBytes, pDestination);
	}
#else
	// 1 byte == 8 bits == 1 octet
	RB_UNUSED(nT2octets);
	RB_UNUSED(pack);
	RB_UNUSED(sign);

	memmove((void *)((uintptr_t)pDst->pBufferStart + (uintptr_t)pDst->index), (void *)((uintptr_t)pSrc->pBufferStart + (uintptr_t)pSrc->index), nTargetBytes);
	RB_PACK_SwapEndianessOfSimpleType(packedEndianess, (uint16_t)nTargetBytes, (void *)((uintptr_t)pDst->pBufferStart + (uintptr_t)pDst->index));
	pSrc->index += nTargetBytes;
	pDst->index += nTargetBytes;
#endif // CHAR_BIT > 8
}

