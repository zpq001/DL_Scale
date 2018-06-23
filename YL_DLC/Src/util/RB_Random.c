//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		RB_Random.c
//! \ingroup	util
//! \brief		This module includes random number (PRN) generators from numerical recipies.
//!
//! \attention
//! The module must be initialized by setting up a good random seed (set seed by RB_RANDOM_Seed) before usage.
//! A good random seed shall be different after every system restart and shall be as random itself as possible (use hardware
//! such as RTC, A/D converter with noise etc. to get a good random seed). Do NOT use the system ticker if possible.
//!
//! From numerical recipies, chpt. 7.1
//! Two algorithms are wrapped in RB_Random:
//! - ran3: Knuth's algorithm, returns a random between 0.0 and 1.0; idum to be set to any negative
//!   value for setting and resetting sequence
//! See also the books explanation at http://www.nrbook.com/a/bookcpdf.php (or buy the book!)
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		George Fankhauser
//
// $Date: 2016/12/19 14:27:34MEZ $
// $Revision: 1.35 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Random"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_RANDOM) && (RB_CONFIG_USE_RANDOM == RB_CONFIG_YES)

#include "RB_Random.h"

#include "RB_Debug.h"


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

static bool seeded = false;
static bool warningSent = false;
static int32_t ran3_idum;


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

static float32 ran3(int32_t *idum)
{
	static int inext,inextp;
	static int32_t ma[56];
	static int iff=0;
	int32_t mj,mk;
	int i,ii,k;

	if (*idum < 0 || iff == 0) {
		iff=1;
		mj=MSEED-(*idum < 0 ? -*idum : *idum);
		mj %= MBIG;
		ma[55]=mj;
		mk=1;
		for (i=1;i<=54;i++) {
			ii=(21*i) % 55;
			ma[ii]=mk;
			mk=mj-mk;
			if (mk < MZ) mk += MBIG;
			mj=ma[ii];
		}
		for (k=1;k<=4;k++)
			for (i=1;i<=55;i++) {
				ma[i] -= ma[1+(i+30) % 55];
				if (ma[i] < MZ) ma[i] += MBIG;
			}
		inext=0;
		inextp=31;
		*idum=1;
	}
	if (++inext == 56) inext=1;
	if (++inextp == 56) inextp=1;
	mj=ma[inext]-ma[inextp];
	if (mj < MZ) mj += MBIG;
	ma[inext]=mj;
	return (float32)(mj*FAC);
}


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_RANDOM_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialize module
//!
//--------------------------------------------------------------------------------------------------
void RB_RANDOM_Initialize(void)
	{
	}


//--------------------------------------------------------------------------------------------------
// RB_RANDOM_Seed
//--------------------------------------------------------------------------------------------------
//! \brief	Seed (initialize) random sequence
//!
//! Same seeds will produce same sequences (PRNG principle)
//!
//! \param	seed	input	select random sequence; use \ref modules_RB_RTC or \ref RB_Timer to make this unpredictable
//! \return	none
//--------------------------------------------------------------------------------------------------
void RB_RANDOM_Seed(int32_t seed)
{
	ran3_idum = -100000*seed; // idum for ran3 needs to be large, any value is acceptable
	seeded = true;
}

//--------------------------------------------------------------------------------------------------
// RB_RANDOM_GetInt
//--------------------------------------------------------------------------------------------------
//! \brief	Get next PRN as integer between 0 and max
//!
//! \param	max	input	maximum PRN value to return
//! \return	PRN
//--------------------------------------------------------------------------------------------------
uint32_t RB_RANDOM_GetInt(uint32_t max)
{
	if (!seeded && !warningSent) {
		// No seed set up. Must call RB_RANDOM_Seed at application startup
		// with some meaningful random value to set up a proper seed.
		RB_DEBUG_WARN("Random number requested but NO SEED SET UP");
		warningSent = true;
	}
	return (uint32_t)(max * ran3(&ran3_idum));
}

//--------------------------------------------------------------------------------------------------
// RB_RANDOM_GetFloat
//--------------------------------------------------------------------------------------------------
//! \brief	Get next PRN as float32 between 0.0 and 1.0
//!
//! \return	PRN
//--------------------------------------------------------------------------------------------------
float32 RB_RANDOM_GetFloat(void)
{
	if (!seeded && !warningSent) {
		// No seed set up. Must call RB_RANDOM_Seed at application startup
		// with some meaningful random value to set up a proper seed.
		RB_DEBUG_WARN("Random number requested but NO SEED SET UP");
		warningSent = true;
	}
	return ran3(&ran3_idum);
}

#endif // RB_CONFIG_USE_RANDOM
