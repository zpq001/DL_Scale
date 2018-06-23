//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Unit.c
//! \ingroup	util
//! \brief		Functions for unit conversions.
//!
//! This module contains the functions for weight unit conversions. It supports SI units as well as
//! a large set of nonmetric units and a set of four user definable units. Service mode with enhanced
//! resolution is also supported if the module WP_State is already used. Additional support for user
//! step factor used typically for 1d/10d key is included. The user step is adapted if the
//! resolution is decreased (M23 or setting M110 to a negative value).
//! Increased display resolutions are also supported (setting M110 to a positive value). This is
//! achieved by automatically increasing the number of decimal places and adapting the unit step.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Pius Derungs, Matthias Klaey, Martin Heusser, Silvan Sturzenegger
//
// $Date: 2017/08/11 13:32:06MESZ $
// $Revision: 1.112 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "WP_Unit"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "WP_Unit.h"

#if defined(WP_FX_BASIC)

#if !defined(RB_CONFIG_USE_STATE) || (RB_CONFIG_USE_STATE == RB_CONFIG_NO)
	#error RB_CONFIG_USE_STATE must be set to RB_CONFIG_YES to use this module
#endif

#include "WP_Data.h"
#include "RB_State.h"
#include "RB_String.h"
#include "RB_Math.h"
#include <ctype.h>
#include <string.h>

#if defined(WP_FX_CUSTOM_UNITS)
	#include "RB_ASCII.h"
#endif


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================
//! Maximum number of fix units
#define UNIT_FIX_MAX			(WP_UNIT_LAST_FIX + 1)

// Set default behavior of unit dwt to NO (normal behavior according to NTEP)
#ifndef RB_CONFIG_WP_UNIT_DWT_INCREASE_RESOLUTION
	#define RB_CONFIG_WP_UNIT_DWT_INCREASE_RESOLUTION	RB_CONFIG_NO
#endif

// Set default maximum decimal places checked in WP_UNIT_IsUnitValid
#ifndef RB_CONFIG_WP_UNIT_MAX_DECIMAL_PLACES
	#define RB_CONFIG_WP_UNIT_MAX_DECIMAL_PLACES	6
#endif

// Set default minimum decimal places checked in WP_UNIT_IsUnitValid
#ifndef RB_CONFIG_WP_UNIT_MIN_DECIMAL_PLACES
	#define RB_CONFIG_WP_UNIT_MIN_DECIMAL_PLACES	(-3)
#endif

//! Set default maximum length of unit text, including terminating zero
#ifndef RB_CONFIG_WP_UNIT_TEXT_MAXLEN
	#define RB_CONFIG_WP_UNIT_TEXT_MAXLEN			8
#endif


//==================================================================================================
//  L O C A L   T Y P E S
//==================================================================================================
//! Definition of unit descriptor struct for one range
typedef struct {
	uint16_t	step;		//!< Step (1, 2, 5, 10, 20, ....)
	int8_t		dp;			//!< Number of decimal places (2 corresponds to 0.01)
	uint8_t		blanking;	//!< Number of dp to suppress (i.e 1 for step=20)
	} WP_UNIT_tDesc;


//==================================================================================================
//  L O C A L   C O N S T A N T S
//==================================================================================================
// From the C Std: When a float64 is demoted to float32 or a int32_t float64 to float64 or float32, if
// the value being converted is outside the range of values that can be represented, the
// behavior is undefined. If the value being converted is in the range of values that can
// be represented but cannot be represented exactly, the result is either the nearest higher
// or nearest lower value, chosen in an implementation-defined manner.

//! Conversion: 1 [gram] = cnvFactor [unit]
static const WP_tFloat cnvFactorTable[UNIT_FIX_MAX] = {
	(WP_tFloat) 1.00000000000000E+00,	//!< Gram
	(WP_tFloat) 1.00000000000000E-03,	//!< Kilogram
	(WP_tFloat) 1.00000000000000E-06,	//!< Ton
	(WP_tFloat) 1.00000000000000E+03,	//!< Milligram
	(WP_tFloat) 1.00000000000000E+06,	//!< Microgram
	(WP_tFloat) 5.00000000000000E+00,	//!< Carat
	(WP_tFloat) 9.80655000000000E-03,	//!< Newton -- !!! DEPRECATED
	(WP_tFloat) 2.20462262184878E-03,	//!< Pound
	(WP_tFloat) 3.52739619495804E-02,	//!< Ounce
	(WP_tFloat) 3.21507465686280E-02,	//!< Troy ounce
	(WP_tFloat) 1.54323583529414E+01,	//!< Grain
	(WP_tFloat) 6.43014931372560E-01,	//!< Pennyweight
	(WP_tFloat) 2.66666666666667E-01,	//!< Momme
	(WP_tFloat) 2.17000000000000E-01,	//!< Mesghal
	(WP_tFloat) 2.67172513291833E-02,	//!< Hong Kong tael
	(WP_tFloat) 2.64554714621853E-02,	//!< Singapore tael
	(WP_tFloat) 2.66666666666667E-02,	//!< Taiwan tael
	(WP_tFloat) 6.12395000000000E-02,	//!< Tical -- !!! DEPRECATED
	(WP_tFloat) 8.57353241830079E-02,	//!< Tola
	(WP_tFloat) 6.59630606860158E-02,	//!< Bath
	(WP_tFloat) 3.52739619495804E-02	//!< lb:oz -- calculation same as Ounce
	};

//! Fix unit text table
static const char* const unitTextTable[] = {
	"g",			//!< 0 Gram
	"kg",			//!< 1 Kilogram
	"t",			//!< 2 Ton
	"mg",			//!< 3 Milligram
	"ug",			//!< 4 Microgram
	"ct",			//!< 5 Carat
	"N",			//!< 6 Newton
	"lb",			//!< 7 Pound
	"oz",			//!< 8 Ounce
	"ozt",			//!< 9 Troy ounce
#if defined(RB_CONFIG_WP_DISPLAY_WEIGHT_ATTRIBUTES_OHAUS) && (RB_CONFIG_WP_DISPLAY_WEIGHT_ATTRIBUTES_OHAUS == RB_CONFIG_YES)
	"Grain",		//!< 10 Grain
#else
	"GN",			//!< 10 Grain
#endif
	"dwt",			//!< 11 Pennyweight
	"mom",			//!< 12 Momme
	"msg",			//!< 13 Mesghal
#if defined(RB_CONFIG_WP_DISPLAY_WEIGHT_ATTRIBUTES_OHAUS) && (RB_CONFIG_WP_DISPLAY_WEIGHT_ATTRIBUTES_OHAUS == RB_CONFIG_YES)
	"HKt",			//!< 14 Hong Kong tael (OHAUS variant)
	"SGt",			//!< 15 Singapore tael (OHAUS variant)
	"TWt",			//!< 16 Taiwan tael (OHAUS variant)
	"ti",			//!< 17 Tical (OHAUS variant)
#else
	"tlh",			//!< 14 Hong Kong tael
	"tls",			//!< 15 Singapore tael
	"tlt",			//!< 16 Taiwan tael
	"tcl",			//!< 17 Tical
#endif
	"tola",			//!< 18 Tola
	"baht",			//!< 19 Baht
	"lb:oz",		//!< 20 lb:oz
	""				//!< 21 empty string
	};

//! Readability factor encoding according to M23
static const uint16_t readabilityEncoding[6] = {1u, 10u, 100u, 1000u, 2u, 5u};

//! Display resolution factor encoding according to M110
static const uint16_t displayResolutionEncoding[7] = {1u, 2u, 5u, 10u, 20u, 50u, 100u};


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================
static int EvaluateFirstSignificantDecimal(WP_tFloat value);
static void SetDescriptorOnBaseDescriptor(WP_tFloat value, int dp, WP_UNIT_tDesc *desc);

#if defined(WP_FX_CUSTOM_UNITS)
static bool IsCustomUnitValid(WP_UNIT_tUnit unit);
static int GetCustomUnitIndex(WP_UNIT_tUnit unit);
#endif

static int GetDpCorrection(WP_UNIT_tUnit unit);
static uint16_t RoundStep(uint16_t step, WP_UNIT_tUnit unit);


//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================
//! Base resolution of the balance in grams (derived from typedefinitions)
static WP_tFloat	BaseResolution;

//! Typedefinition parameters for the module UNIT
static WP_UNIT_tTDParams UnitParams;

//! Unit descriptor table
static WP_UNIT_tDesc FixUnits[UNIT_FIX_MAX][RB_CONFIG_WP_RANGES_MAX];

//! Dynamically evaluated index of finest range (= highest range index used)
static int FinestRangeIndex;

// Declare local variable if WP_DATA parameter not available
#if !defined(WP_FX_CMD_M21)
	static WP_DATA_tUnitSelection		UnitSelection[RB_CONFIG_WP_UNIT_MAX_ACTIVATED];
	#define UNIT_SELECTION_DATA			UnitSelection
#else
	#define UNIT_SELECTION_DATA			WP_DATA.M21
#endif

// Declare local variable if WP_DATA parameter not available
#if defined(WP_FX_CUSTOM_UNITS)
	#if !defined(WP_FX_CMD_M22)
		static WP_DATA_tCustomUnit		CustomUnit[RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS + 1]; // +1 needed to reserve space for s8 format
		#define CUSTOMUNIT_DATA			CustomUnit
	#else
		#define CUSTOMUNIT_DATA			WP_DATA.M22
	#endif
#endif

// Declare local variable if WP_DATA parameter not available
#if !defined(WP_FX_CMD_M23)
	static WP_DATA_tReadability			Readability;
	#define READABILITY_DATA			Readability
#else
	#define READABILITY_DATA			WP_DATA.M23
#endif

// Declare local variable if WP_DATA parameter not available
#if !defined(WP_FX_CMD_M110)
	static WP_DATA_tDisplayResolution	DisplayResolution;
	#define DISPLAY_RESOLUTION_DATA		DisplayResolution
#else
	#define DISPLAY_RESOLUTION_DATA		WP_DATA.M110
#endif


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_UNIT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module UNIT
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_UNIT_Initialize(void)
	{
	WP_UNIT_tTDParams defaultTD;
	RB_RESET_STRUCT(defaultTD);					// Set all to zero
	defaultTD.defDP = 2;						// Two decimal places
	defaultTD.defStep[0] = 1u;					// One range with step 1
	WP_UNIT_SetTDParams(&defaultTD);			// Setup all local variables
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module UNIT
//!
//! \param	pUnitParams		Pointer to a set of typedefinition parameters for the module UNIT
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_UNIT_SetTDParams(const WP_UNIT_tTDParams* pUnitParams)
	{
	int i;
	int unit;
	WP_tFloat stepFactor[RB_CONFIG_WP_RANGES_MAX];

	// Copy parameter to local data
	UnitParams = *pUnitParams;

	// Patch definition units as always valid unit
	UnitParams.validDisplayUnitMask |= (1uL << UnitParams.defUnit);
	UnitParams.validHostUnitMask |= (1uL << UnitParams.defUnit);

	// evaluate finest range (always the last valid)
	FinestRangeIndex = 0;
	for (i=0; (i<(int)RB_ARRAY_SIZE(UnitParams.defStep)) && (UnitParams.defStep[i] > 0); i++)
		{
		FinestRangeIndex = i;
		}

	// Calculate stepfactortable
	for (i=0; i<(int)RB_ARRAY_SIZE(stepFactor); i++)
		{
		stepFactor[i] = (WP_tFloat)UnitParams.defStep[i] / (WP_tFloat)UnitParams.defStep[FinestRangeIndex];
		}

	// Calculate BaseResolution (always in Gram)
	BaseResolution = (WP_tFloat)UnitParams.defStep[FinestRangeIndex] * (WP_tFloat)RB_MATH_ipow10(-UnitParams.defDP);
	BaseResolution /= cnvFactorTable[UnitParams.defUnit];

	// Process service mode (half resolution except for defUnit=microgram and milligram
	#if defined(RB_CONFIG_WP_SERVICE_RESOLUTION) && (RB_CONFIG_WP_SERVICE_RESOLUTION == RB_CONFIG_YES)
	if (RB_STATE_GetOperatingMode() == RB_STATE_OPMODE_SERVICE)
		{
		if ((UnitParams.defUnit != WP_UNIT_mg) && (UnitParams.defUnit != WP_UNIT_ug))
			{
			BaseResolution *= (WP_tFloat)0.5;
			}
		}
	#endif

	// Prepare step and formatting table for all fix units
	for (unit = 0; unit < (int)RB_ARRAY_SIZE(FixUnits); unit++)
		{
		WP_tFloat unitResolution = BaseResolution * cnvFactorTable[unit];
		int dp;

		// Process reduced units (dpReduction=10.0 result in resolution * 10
		if (UnitParams.reducedUnitMask & (1uL << unit))
			unitResolution *= UnitParams.dpReduction;

		// According to Einstein (Projectname MT-LabTec) document "Table of units.pdf", chapter 2.6.5,
		// footnote 5: Increase resolution for unit pennyweight.
		#if defined(RB_CONFIG_WP_UNIT_DWT_INCREASE_RESOLUTION) && (RB_CONFIG_WP_UNIT_DWT_INCREASE_RESOLUTION == RB_CONFIG_YES)
		if (unit == (int)WP_UNIT_dwt)
			{
			unitResolution *= (WP_tFloat)0.8;
			}
		#endif

		dp = EvaluateFirstSignificantDecimal(unitResolution);
		for (i=0; i<(int)RB_ARRAY_SIZE(FixUnits[0]); i++)
			{
			SetDescriptorOnBaseDescriptor(unitResolution * stepFactor[i], dp, &FixUnits[unit][i]);
			if (unit == (int)WP_UNIT_lboz)
				{
				// Special rounding to 4,40,.. instead of 5,50,.. for oz-part of lb:oz
				if ((dp <= 0) && (FixUnits[unit][i].step ==    5u)) FixUnits[unit][i].step =    4u;
				if ((dp == 1) && (FixUnits[unit][i].step ==   50u)) FixUnits[unit][i].step =   40u;
				if ((dp == 2) && (FixUnits[unit][i].step ==  500u)) FixUnits[unit][i].step =  400u;
				if ((dp == 3) && (FixUnits[unit][i].step == 5000u)) FixUnits[unit][i].step = 4000u;
				}
			}
		}

	// Check unit selection
	for (unit = 0; unit < (int)RB_ARRAY_SIZE(UNIT_SELECTION_DATA); unit++)
		{
		if (!WP_UNIT_IsUnitValid((WP_UNIT_tUnitSelection)unit, (WP_UNIT_tUnit)UNIT_SELECTION_DATA[unit]))
			UNIT_SELECTION_DATA[unit] = UnitParams.defUnit;
		}

	// Check some parameters
	if (READABILITY_DATA >= (int)RB_ARRAY_SIZE(readabilityEncoding))
		READABILITY_DATA = 0;
	if (DISPLAY_RESOLUTION_DATA >= (int)RB_ARRAY_SIZE(displayResolutionEncoding) ||
		DISPLAY_RESOLUTION_DATA <= -(int)RB_ARRAY_SIZE(displayResolutionEncoding))
		DISPLAY_RESOLUTION_DATA = 0;
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the typedefinition parameterset of the module UNIT
//!
//! \return	Pointer to the typedefinition parameterset
//--------------------------------------------------------------------------------------------------
const WP_UNIT_tTDParams* WP_UNIT_GetTDParams(void)
	{
	return(&UnitParams);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetBaseResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the base resolution in grams
//!
//! The base resolution corresponds to the smallest value defined in XP0338 and XP0339 parameters.
//! Also service mode is considered. No corrections are applied according to UserStep or reduced units.
//!
//! \note The base resolution can be a odd value (i.e. 0.00204..) if balance is defined in lb's
//!
//! \return	Base resolution in gram
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetBaseResolution(void)
	{
	return(BaseResolution);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition unit defined as XP0317
//!
//! \return	Definition unit
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetDefinitionUnit(void)
	{
	return(UnitParams.defUnit);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionDP
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition decimal places defined as part of XP0339
//!
//! \return	Definition decimal places
//--------------------------------------------------------------------------------------------------
int WP_UNIT_GetDefinitionDP(void)
	{
	return(UnitParams.defDP);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionStep
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition step defined as part of XP0339
//!
//! \return	Definition step
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetDefinitionStep(void)
	{
	return(UnitParams.defStep[0]);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetHostUnitMask
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for host
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
uint32_t WP_UNIT_GetHostUnitMask(void)
	{
	return(UnitParams.validHostUnitMask);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefaultHostUnit
//--------------------------------------------------------------------------------------------------
//! \brief	get default unit for host
//!
//! \return	Default host unit
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetDefaultHostUnit(void)
	{
	return(UnitParams.defHostUnit);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDisplayUnitMask
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for display
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
uint32_t WP_UNIT_GetDisplayUnitMask(void)
	{
	return(UnitParams.validDisplayUnitMask);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefaultDisplayUnit
//--------------------------------------------------------------------------------------------------
//! \brief	get default unit for display
//!
//! \return	Default display unit
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetDefaultDisplayUnit(void)
	{
	return(UnitParams.defDisplayUnit);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_SetUserStepFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Set the actual user step factor (used for 1d/10d ...)
//!
//! This function only sets the M23 parameter (readability), it cannot be used to change M110
//! (display resolution).
//!
//! \param	stepFactor		Step factor to be set (only 1, 10, 100 and 1000 is valid)
//! \return	WP_UNIT_tStatus
//!			- WP_UNIT_OK
//!			- WP_UNIT_RANGE_FAIL, i.e. stepFactor out of limit
//!			- WP_UNIT_STORE_FAIL, i.e. stepFactor can not be stored
//--------------------------------------------------------------------------------------------------
WP_UNIT_tStatus WP_UNIT_SetUserStepFactor(uint16_t stepFactor)
	{
	int i;
	WP_UNIT_tStatus sts = WP_UNIT_RANGE_FAIL;
	for (i=0; i<(int)RB_ARRAY_SIZE(readabilityEncoding); i++)
		{
		if (readabilityEncoding[i] == stepFactor)
			{
			WP_DATA_tReadability index = (WP_DATA_tReadability)i;
			#if defined(WP_FX_CMD_M23)
			// When using the WP_DATA_StoreParameter function M110 is automatically reset.
			switch (WP_DATA_StoreParameter(RB_PARAM_M23, &index, 0, 0))
			{
				default:
				case WP_OK:			sts = WP_UNIT_OK;			break;
				case WP_RANGE_FAIL:	sts = WP_UNIT_RANGE_FAIL;	break;
				case WP_STORE_FAIL:	sts = WP_UNIT_STORE_FAIL;	break;
			}
			#else
			// Reset M110, M23 and M110 cannot be active at the same time.
			#if defined(WP_FX_CMD_M110)
			WP_DATA_tDisplayResolution res = 0;
			switch (WP_DATA_StoreParameter(RB_PARAM_M110, &res, 0, 0))
			{
				default:
				case WP_OK:			sts = WP_UNIT_OK;			break;
				case WP_RANGE_FAIL:	sts = WP_UNIT_RANGE_FAIL;	break;
				case WP_STORE_FAIL:	sts = WP_UNIT_STORE_FAIL;	break;
			}
			#else
			DISPLAY_RESOLUTION_DATA = res;
			sts = WP_UNIT_OK;
			#endif
			if (sts == WP_UNIT_OK) {
				READABILITY_DATA = index;
			}
			#endif
			break; // leave for loop
			}
		}
	return sts;
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUserStepFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual user step factor (used by 1d/10d ...)
//!
//! This function returns one of two factors, depending on which parameter is active:
//! - The factor set with M23.
//! - The factor set with M110, if it is <= 0, i.e. decreased display resolution.
//! If M110 is set to a positive value (i.e. increased display resolution) zero is returned instead,
//! indicating that the user step factor is not valid.
//!
//! \return	Actual user step factor
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetUserStepFactor(void)
{
	uint16_t userStepFactor;
	// M23 and M110 are mutually exclusive, only one can be active at a time.
	if (READABILITY_DATA > 0) {
		userStepFactor = readabilityEncoding[READABILITY_DATA];
	} else if (DISPLAY_RESOLUTION_DATA < 0) {
		userStepFactor = displayResolutionEncoding[-DISPLAY_RESOLUTION_DATA];
	} else if (DISPLAY_RESOLUTION_DATA > 0) {
		// Set it to zero to indicate that the step factor is not valid because the display
		// resolution is increased.
		userStepFactor = 0u;
	} else {
		// If M23 and M110 are set to zero the default factor of one is active.
		userStepFactor = 1u;
	}
	return userStepFactor;
}


#if defined(WP_FX_2ND_RANGE_DETECTOR)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitMaskRangeDetector2
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for 2nd range detector
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
uint32_t WP_UNIT_GetUnitMaskRangeDetector2(void)
	{
	return(UnitParams.rngDet2UnitMask);
	}
#endif


#if defined(WP_FX_2ND_RANGE_DETECTOR)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionUnitRangeDetector2
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition unit of 2nd range detector
//!
//! \return	Definition unit of 2nd range detector
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetDefinitionUnitRangeDetector2(void)
	{
	return(UnitParams.rngDet2DefUnit);
	}
#endif


//--------------------------------------------------------------------------------------------------
// WP_UNIT_ActivateUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Set the actual unit for the corresponding unit selection
//!
//! Set the actual unit and store it permanently in RB_PARAM_M21
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	unit		Unit to be set as actual unit for unit selection
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
WP_tStatus WP_UNIT_ActivateUnit(WP_UNIT_tUnitSelection unitSel, WP_UNIT_tUnit unit)
	{
	#if defined(RB_PARAM_M21) && defined(RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM) && (RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM == RB_CONFIG_YES)
	WP_DATA_tUnitSelection m21val = (WP_DATA_tUnitSelection)unit;
	return(WP_DATA_StoreParameter(RB_PARAM_M21, &m21val, 0, (int)unitSel));
	#else
	UNIT_SELECTION_DATA[(int)unitSel] = (WP_DATA_tUnitSelection)unit;
	return(WP_OK);
	#endif
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetFirstValidUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Return first valid fix unit
//!
//! \param	unitSel		Unit selection, to use for the check
//! \return	unit
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetFirstValidUnit(WP_UNIT_tUnitSelection unitSel)
	{
	WP_UNIT_tUnit unit;
	for (unit = WP_UNIT_g; unit <= WP_UNIT_LAST_FIX; unit++)
		{
		if (WP_UNIT_IsUnitValid(unitSel, unit))
			{
			return(unit);
			}
		}
	return(UnitParams.defUnit); // Definition unit is always valid
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual unit for the corresponding unit selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual unit of the specific unit selection
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_GetActiveUnit(WP_UNIT_tUnitSelection unitSel)
	{
	return ((WP_UNIT_tUnit)UNIT_SELECTION_DATA[unitSel]);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveUnitText
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual unit text for the corresponding unit selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual unit text of the specific unit selection
//--------------------------------------------------------------------------------------------------
const char* WP_UNIT_GetActiveUnitText(WP_UNIT_tUnitSelection unitSel)
	{
	return (WP_UNIT_GetUnitText(WP_UNIT_GetActiveUnit(unitSel)));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveDigitResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual digit resolution for the corresponding unit selection
//!
//! This function takes into consideration the settings of the SysOperatingMode (e.g. SERVICE)
//! and the unit specific reduction factor
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual digit resolution of the specific unit selection
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetActiveDigitResolution(WP_UNIT_tUnitSelection unitSel)
	{
	return(WP_UNIT_GetUnitDigitResolution(WP_UNIT_GetActiveUnit(unitSel)));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveDP
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual decimal places for the corresponding unit selection
//!
//! This function takes into consideration the settings of the SysOperatingMode (e.g. SERVICE)
//! and the unit specific reduction factor
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Decimal places location of the specific unit selection
//--------------------------------------------------------------------------------------------------
int WP_UNIT_GetActiveDP(WP_UNIT_tUnitSelection unitSel)
	{
	return (WP_UNIT_GetUnitDP(WP_UNIT_GetActiveUnit(unitSel)));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveRangeFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual step factor for the corresponding unit and range selection
//!
//! Returns factor for step (range 1.0 or higher). The returned value is 1.0 for custom units, unit-none
//! and in service mode. The factor is always 1.0 for the finest range.
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	rngSel		Range selection, to get the actual step factor for
//! \return	Actual step factor of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetActiveRangeFactor(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel)
	{
	return(WP_UNIT_GetUnitRangeFactor(WP_UNIT_GetActiveUnit(unitSel), rngSel));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveRangeStep
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual step for the corresponding unit and range selection
//!
//! This function takes into consideration the settings of the SysOperatingMode (e.g. SERVICE)
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	rngSel		Range selection, to get the actual step factor for
//! \return	Actual step of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetActiveRangeStep(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel)
	{
	return(WP_UNIT_GetUnitRangeStep(WP_UNIT_GetActiveUnit(unitSel), rngSel));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveRangeDecade
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual range decade for the corresponding unit and range selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	rngSel		Range status, to get the actual range decade for
//! \return	Actual range decade of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetActiveRangeDecade(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel)
	{
	return(WP_UNIT_GetUnitRangeDecade(WP_UNIT_GetActiveUnit(unitSel), rngSel));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the conversion factor for the corresponding unit (incl. custom unit)
//!
//! \param	unit	Unit, to get the conversion factor for
//! \return	Conversion factor of the specific unit
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetUnitFactor(WP_UNIT_tUnit unit)
	{
	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	// Return factor for fixed units
	if (unit <= WP_UNIT_LAST_FIX)
		return cnvFactorTable[unit];

	#if defined(WP_FX_CUSTOM_UNITS)
	if (IsCustomUnitValid(unit))
		return CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].factor;
	#endif

	return(WP_FLT_ONE);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitDigitResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the digit resolution for the corresponding unit (incl. custom unit)
//!
//! \param	unit	Unit, to get the digit resolution for.
//! \return	Digit	resolution of the specific unit
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetUnitDigitResolution(WP_UNIT_tUnit unit)
	{
	return(WP_UNIT_GetUnitRangeResolution(unit, (WP_tRangeIndex)FinestRangeIndex));
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the resolution for the corresponding unit and range (incl. custom unit)
//!
//! \param	unit	Unit, to get the digit resolution for.
//! \param	rngSel	Range selection, to get the actual step factor for
//! \return	Digit resolution of the specific unit
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetUnitRangeResolution(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel)
	{
	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	if (unit <= WP_UNIT_LAST_FIX)
		{
		WP_tFloat resolution = (WP_tFloat)WP_UNIT_GetUnitRangeStep(unit, rngSel);
		resolution *= (WP_tFloat)RB_MATH_ipow10(-WP_UNIT_GetUnitDP(unit));
		return resolution;
		}

	#if defined(WP_FX_CUSTOM_UNITS)
	if (IsCustomUnitValid(unit))
		return CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].resolution;
	#endif

	return(WP_FLT_ONE);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitDP
//--------------------------------------------------------------------------------------------------
//! \brief	Get the decimal places for the corresponding unit
//!
//! The decimal places is the number of decimal digits for the given unit (i.e for a resolution of
//! 0.005g the decimal places is 3)
//! If the display resolution is increased (M110) then the decimal places may have to be increased
//! as well so the weight can be formatted correctly.
//!
//! \param	unit	Unit, to get the decimal places for
//! \return	Decimal places of the specific unit
//--------------------------------------------------------------------------------------------------
int WP_UNIT_GetUnitDP(WP_UNIT_tUnit unit)
{
	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	if (unit <= WP_UNIT_LAST_FIX) {
		int dp = FixUnits[unit][FinestRangeIndex].dp;
		// For increased display resolutions dp may have to be increased.
		if (DISPLAY_RESOLUTION_DATA > 0) {
			dp += GetDpCorrection(unit);
		}
		return dp;
	}

	#if defined(WP_FX_CUSTOM_UNITS)
	if (IsCustomUnitValid(unit))
		return EvaluateFirstSignificantDecimal(CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].resolution);
	#endif

	return 0;
}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeStep
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual step for the corresponding unit and range selection
//!
//! The step can be changed using M23 and M110.
//! The step always has to be equal to or larger than 1, therefore the number of decimal places is
//! automatically increased if the step is getting too small.
//!
//! \param	unit		Unit, to get the actual step factor for
//! \param	rngSel		Range selection, to get the actual step factor for
//! \return	Actual step of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetUnitRangeStep(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel)
{
	uint16_t rngStep = 1U; // Default for all non-fix units

	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	if (unit <= WP_UNIT_LAST_FIX) {
		switch (RB_STATE_GetOperatingMode()) {
			case RB_STATE_OPMODE_SERVICE:
				// Service mode: Always use step of finest range and ignore user step settings (minStep)
				// HPQC#5793 Corrected step for service mode (typically 5 instead of 1)
				rngStep = FixUnits[unit][FinestRangeIndex].step;
				break;
			default:
			{
				uint16_t minStep;
				uint16_t userStep = WP_UNIT_GetUserStepFactor();
				rngStep = FixUnits[unit][rngSel].step;
				if (READABILITY_DATA != 0) {
					#if defined(RB_CONFIG_WP_USER_STEP_MAXIMUM_VALUE) && (RB_CONFIG_WP_USER_STEP_MAXIMUM_VALUE == RB_CONFIG_YES)
					if (FixUnits[unit][FinestRangeIndex].step > userStep) {
						minStep = FixUnits[unit][FinestRangeIndex].step;
					} else {
						minStep = userStep;
					}
					#else
					minStep = (uint16_t)(FixUnits[unit][FinestRangeIndex].step * userStep); // max. 50'000
					#endif
					if (rngStep < minStep) {
						rngStep = minStep;
					}
				} else if (DISPLAY_RESOLUTION_DATA < 0) {
					rngStep *= userStep;
				} else if (DISPLAY_RESOLUTION_DATA > 0) {
					WP_tFloat step = (WP_tFloat)rngStep / (WP_tFloat)displayResolutionEncoding[DISPLAY_RESOLUTION_DATA];
					// If the step would be smaller than one then dp is automatically adapted.
					step *= (WP_tFloat)RB_MATH_ipow10(GetDpCorrection(unit));
					rngStep = (uint16_t)step;
				}
				// The calculated step can have weird values and has to be rounded appropriately.
				// E.g.
				// 2 * 2 = 4  -> 5
				// 5 * 5 = 25 -> 20
				rngStep = RoundStep(rngStep, unit);
				break;
			}
		}
	}
	return(rngStep);
}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the step factor for the corresponding unit and range specifications
//!
//! Only fix units supports multiple ranges with a range factor, all other returns fix 1.0
//! \param	unit		Unit, to get the step factor for
//! \param	rngSel		Range status, to get the step factor for
//! \return	Step factor of the specific unit and range specifications
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_GetUnitRangeFactor(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel)
	{
	WP_tFloat factor = WP_FLT_ONE;

	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	// Fix units only, 1.0 for all other units
	if (unit <= WP_UNIT_LAST_FIX)
		{
		factor = (WP_tFloat)WP_UNIT_GetUnitRangeStep(unit, rngSel) / (WP_tFloat)FixUnits[unit][FinestRangeIndex].step;
		}
	return(factor);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeDecade
//--------------------------------------------------------------------------------------------------
//! \brief	Get the step decade for the corresponding unit and range specifications
//!
//! \param	unit		Unit, to get the step decade for
//! \param	rngSel		Range status, to get the range decade for
//! \return	rangeDecade Number of decimal places that can be suppressed
//--------------------------------------------------------------------------------------------------
uint16_t WP_UNIT_GetUnitRangeDecade(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel)
	{
	uint16_t step = WP_UNIT_GetUnitRangeStep(unit, rngSel);
	uint16_t blanking = ((step < 10U) ? 0 : ((step < 100U) ? 1U : ((step < 1000U) ? 2U : 3U)));
	return(blanking);
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_CalculateDeflection
//--------------------------------------------------------------------------------------------------
//! \brief	Calculate deflection value for e.g. SNR command
//!
//! The deflection value depends on the resolution of balance:
//!     - BaseResolution >= 1g:     Deflection =    5 * BaseResolution
//!     - BaseResolution >= 0.1g:   Deflection =   10 * BaseResolution
//!     - BaseResolution >= 0.01g:  Deflection =  100 * BaseResolution
//!     - BaseResolution <  0.01g:  Deflection = 1000 * BaseResolution
//!
//! \return	Deflection value in gram
//--------------------------------------------------------------------------------------------------
WP_tFloat WP_UNIT_CalculateDeflection(void)
	{
	int8_t dp = FixUnits[0][0].dp;
	WP_tFloat threshold;
	if      (dp <= 0) threshold = (WP_tFloat)5.0 * BaseResolution;
	else if (dp >= 3) threshold = (WP_tFloat)1000.0 * BaseResolution;
	else                    threshold = (WP_tFloat)1.0;

	return threshold;
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitText
//--------------------------------------------------------------------------------------------------
//! \brief	Return the unit text (incl. custom unit text)
//!
//! \param	unit	Unit, to get the unitstring for
//! \return	Pointer to the unitstring of the specific unit (pointer to "" in case of invalid unit)
//--------------------------------------------------------------------------------------------------
const char* WP_UNIT_GetUnitText(WP_UNIT_tUnit unit)
	{
	if (unit <= WP_UNIT_LAST_FIX)
		return unitTextTable[unit];

	#if defined(WP_FX_CUSTOM_UNITS)
	if (IsCustomUnitValid(unit))
		return &CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].unitText[0];
	#endif

	return "";
	}


#if defined(WP_FX_CUSTOM_UNITS)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_SetCustomUnitSpecs
//--------------------------------------------------------------------------------------------------
//! \brief	Set the specifications for a specific custom unit
//!
//! The range of the unit selection is WP_UNIT_pieces..WP_UNIT_currency_2 according to configured
//! RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS, otherwise no action is taken and false is returned.
//!
//! \param	unit		Unit, to set the specification for
//! \param	pUnitSpecs	Pointer to the specification data of the specific custom unit
//! \return	<c>true</c> if unit is valid, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_UNIT_SetCustomUnitSpecs(WP_UNIT_tUnit unit, const WP_DATA_tCustomUnit* pUnitSpecs)
	{
	if (pUnitSpecs)
		{
		if (IsCustomUnitValid(unit))
			{
			#if defined(RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM) && (RB_CONFIG_WP_PARAM_USE_RUNTIME_PARAM == RB_CONFIG_YES)
			if (WP_DATA_StoreParameter(RB_PARAM_M22, pUnitSpecs, 0, GetCustomUnitIndex(unit)) == WP_OK)
				return(true);
			#else
			CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)] = *pUnitSpecs;
			return(WP_OK);
			#endif
			}
		}
	return false;
	}
#endif


#if defined(WP_FX_CUSTOM_UNITS)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetCustomUnitSpecs
//--------------------------------------------------------------------------------------------------
//! \brief	Get the specifications for a specific custom unit
//!
//! The range of the unit selection is WP_UNIT_pieces..WP_UNIT_currency_2 according to configured
//! RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS, otherwise NULL is returned.
//!
//! \param	unit	Unit, to get the specification for
//! \return	Pointer to customUnit specification of the specific custom unit or NULL
//--------------------------------------------------------------------------------------------------
const WP_DATA_tCustomUnit* WP_UNIT_GetCustomUnitSpecs(WP_UNIT_tUnit unit)
	{
	if (!IsCustomUnitValid(unit))
		return NULL;

	return &CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)];
	}
#endif


#if defined(WP_FX_CUSTOM_UNITS)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_IsCustomUnitTextValid
//--------------------------------------------------------------------------------------------------
//! \brief	Check if the custom unit text is valid
//!
//! Custom unit text must not be equal or similar to a predefined unit text
//!
//! \param	pCustomUnitText		Custom unit text, to be checked
//! \return	<c>true</c> if unit is valid, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_UNIT_IsCustomUnitTextValid(const char* pCustomUnitText)
	{
	char cuText[RB_CONFIG_WP_UNIT_TEXT_MAXLEN];
	WP_UNIT_tUnit unit;
	size_t cuLen;

	// Copy text to local variable and remove leading and trailing blanks
	// Skip leading blanks
	while (*pCustomUnitText == RB_ASCII_SP)
		pCustomUnitText++;

	// copy unit and convert it to lower case
	for (cuLen = 0; (cuLen < (RB_ARRAY_SIZE(cuText) - 1)) && (*pCustomUnitText > RB_ASCII_SP); cuLen++)
		{
		cuText[cuLen] = (char)tolower((int)*pCustomUnitText);
		pCustomUnitText++;
		}
	cuText[cuLen] = RB_ASCII_NUL;

	// Empty unit is always valid as custom unit
	if (cuLen == 0)
		return true;

	// Check against all unit texts in table (lower case compare)
	for (unit = WP_UNIT_g; unit <= WP_UNIT_LAST_FIX; unit++)
		{
		const char *pUnit = unitTextTable[unit];
		size_t i;
		size_t match = 0;
		if (strlen(pUnit) != cuLen)
			continue; // next unit if different length's

		for (i=0; i<cuLen; i++, pUnit++)
			{
			if (cuText[i] == (char)tolower((int)*pUnit))
				match++;
			}

		if (match == cuLen)
			return(false); // match found
		}
	return true;
}
#endif


//--------------------------------------------------------------------------------------------------
// WP_UNIT_IsUnitValid
//--------------------------------------------------------------------------------------------------
//! \brief	Check if the unit is selectable
//!
//! Fix units are valid if the corresponding unit mask (XP0320, XP0321) is set. UNIT_none and the
//! definition unit are always valid. Custom units, PCS and percent are only valid if their specs
//! are set with a factor different from zero.
//!
//! \param	unitSel		Unit selection, to use for the check
//! \param	unit	Unit, to be checked
//! \return	<c>true</c> if unit is valid, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_UNIT_IsUnitValid(WP_UNIT_tUnitSelection unitSel, WP_UNIT_tUnit unit)
	{
	int decimals = WP_UNIT_GetUnitDP(unit);

	if ((int)unitSel >= RB_CONFIG_WP_UNIT_MAX_ACTIVATED)
		return false;

	// Unit-none: process as definition unit
	if (unit == WP_UNIT_none)
		unit = UnitParams.defUnit;

	// Test fix units (note: definition unit is always set the masks)
	if (unit <= WP_UNIT_LAST_FIX)
		{
		// Only RB_CONFIG_WP_UNIT_MIN_DECIMAL_PLACES .. RB_CONFIG_WP_UNIT_MAX_DECIMAL_PLACES decimal places are valid
		if ((decimals < RB_CONFIG_WP_UNIT_MIN_DECIMAL_PLACES) ||(decimals > RB_CONFIG_WP_UNIT_MAX_DECIMAL_PLACES))
			return false;
		else
			{
			uint32_t mask = 0;
			switch (unitSel)
				{
				case WP_UNIT_SICS:    mask = UnitParams.validHostUnitMask; break;
				case WP_UNIT_DISPLAY: mask = UnitParams.validDisplayUnitMask; break;
				case WP_UNIT_1:       mask = UnitParams.validHostUnitMask; break;
				case WP_UNIT_2:       mask = UnitParams.validHostUnitMask; break;
				}
			return ((mask & (1UL << unit)) ? true : false);
			}
		}

	#if defined(WP_FX_CUSTOM_UNITS)
	// Test custom units --> valid if CUSTOMUNIT_DATA.formula != none
	if (IsCustomUnitValid(unit))
		{
		if (CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].formula != WP_DATA_CU_FORMULA_NONE)
			return (true);
		}
	#endif

	return false;
	}


//--------------------------------------------------------------------------------------------------
// WP_UNIT_ParseUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Parse a string for a valid unitstring (incl. custom unit)
//!
//! Only leading blanks are skipped
//!
//! \param	unitStr		String, to be converted into a unit enumerator
//! \return	Unit enumerator
//--------------------------------------------------------------------------------------------------
WP_UNIT_tUnit WP_UNIT_ParseUnit(const char* unitStr)
	{
	WP_UNIT_tUnit unit;

	// Skip leading blanks
	while (*unitStr == ' ') unitStr++;

	// Check for no unit
	if (*unitStr == 0)
		return WP_UNIT_none;

	// Search in fixed unit text
	for (unit = WP_UNIT_g; unit <= WP_UNIT_LAST_FIX; unit++)
		if (strcmp(unitTextTable[unit], unitStr) == 0)
			return unit;

	#if defined(WP_FX_CUSTOM_UNITS)
	// Search in custom unit text
	for (unit = WP_UNIT_pieces; IsCustomUnitValid(unit); unit++)
		if (strcmp(&CUSTOMUNIT_DATA[GetCustomUnitIndex(unit)].unitText[0], unitStr) == 0)
			return unit;
	#endif

	return WP_UNIT_none;
	}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// EvaluateFirstSignificantDecimal
//--------------------------------------------------------------------------------------------------
//! \brief	Evaluates the first significant decimal place
//!
//! This function evaluates the first significant decimal place for a given WP_tFloat value.
//! 0.001 results in 3 decimal places, 0.0123456789 results in 2 decimal places.
//!
//! \param	value		value, must be positive
//! \return	Result		exponent of value
//--------------------------------------------------------------------------------------------------
static int EvaluateFirstSignificantDecimal(WP_tFloat value)
	{
	float64 mantissa;
	int32_t exponent;
	int e = 0; // default

	// Split value into mantissa and exponent:
	//    value = (-1)^signbit * mantissa * 10^exponent
	//    where 1 <= mantissa < 10
	if (RB_MATH_frexp10((double)value, NULL, &mantissa, &exponent))
	{
		e = (int)(-exponent);

		// We must use the same constant as used in SetDescriptorOnBaseDescriptor
		if ((WP_tFloat)mantissa >= (WP_tFloat)5.25)
		{
			e--;
		}
	}
	return e;
	}


//--------------------------------------------------------------------------------------------------
// SetDescriptorOnBaseDescriptor
//--------------------------------------------------------------------------------------------------
//! \brief	Evaluate fix unit descriptor based on a other descriptor
//!
//! This function evaluates step, and blanking for a given float32 value based on fix decimal places.
//! Step result in values of 1, 2, 5, 10, 20, 50, 100, ... 5000.
//! Decimal places are set to the same as base decimal places.
//! Blanking ist set to 1 for steps between 10 and 50, 2 for steps between 100 and 500, ...
//!
//! \param	value		Original value, must be positive (typ. 1 ... 500)
//! \param	dp			Number of decimal places
//! \param	desc		Pointer to unit descriptor
//--------------------------------------------------------------------------------------------------
static void SetDescriptorOnBaseDescriptor(WP_tFloat value, int dp, WP_UNIT_tDesc *desc)
	{
	uint8_t  blanking;
	float32 v = (float32)(value * (WP_tFloat)RB_MATH_ipow10(dp));
	uint16_t step;

	// Evaluate step for given value. Threshold values are about 5% higher to allow some near
	// unitfactors to have the same resolution (i.e TL-S, TL-T). This will increase the resolution
	// of the balance up to 5% for some specific configuration of definition unit and displayed unit.
	// Calculation: 1+1.05*(2-1), 2+1.05(5-2), 5+1.05*(10-5)
	if      (v <    1.05F) step = 1U;
	else if (v <    2.15F) step = 2U;
	else if (v <    5.25F) step = 5U;
	else if (v <   10.50F) step = 10U;
	else if (v <   21.50F) step = 20U;
	else if (v <   52.50F) step = 50U;
	else if (v <  105.00F) step = 100U;
	else if (v <  215.00F) step = 200U;
	else if (v <  525.00F) step = 500U;
	else if (v < 1050.00F) step = 1000U;
	else if (v < 2150.00F) step = 2000U;
	else                   step = 5000U;

	// Evaluate blanking for given step
	if      (step < 10)   blanking = 0;
	else if (step < 100)  blanking = 1;
	else if (step < 1000) blanking = 2;
	else                  blanking = 3;

	desc->dp = (int8_t)dp;
	desc->step     = step;
	desc->blanking = blanking;
	return;
	}


#if defined(WP_FX_CUSTOM_UNITS)
//--------------------------------------------------------------------------------------------------
// IsCustomUnitValid
//--------------------------------------------------------------------------------------------------
//! \brief	Check if unit a valid custom unit
//!
//! \param	unit		Unit of type WP_UNIT_tUnit
//! \return	<c>true</c> if unit is valid, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
static bool IsCustomUnitValid(WP_UNIT_tUnit unit)
	{
	int index = (int)unit - (int)WP_UNIT_pieces;
	if ((index >= 0) && (index < RB_CONFIG_WP_UNIT_MAX_CUSTOM_UNITS))
		return true;
	return false;
	}
#endif


#if defined(WP_FX_CUSTOM_UNITS)
//--------------------------------------------------------------------------------------------------
// GetCustomUnitIndex
//--------------------------------------------------------------------------------------------------
//! \brief	Get index of cutom unit
//!
//! \param	unit		Unit of type WP_UNIT_tUnit
//! \return	index		Index of cutom unit
//--------------------------------------------------------------------------------------------------
static int GetCustomUnitIndex(WP_UNIT_tUnit unit)
	{
	return (int)unit - (int)WP_UNIT_pieces;
	}
#endif


//--------------------------------------------------------------------------------------------------
// GetDpCorrection
//--------------------------------------------------------------------------------------------------
//! \brief	Get the decimal places correction if a higher display resolution is configured
//!
//! Example:
//! By default the unit has a step of 5. If the display resolution is increased the following
//! dp corrections are returned:
//! \verbatim
//! factor, step,   dp correction
//! 2       2       0
//! 5       1       0
//! 10      0.5     1
//! 20      0.2     1
//! 50      0.1     1
//! 100     0.05    2
//! \endverbatim
//!
//! \param	unit		Unit of type WP_UNIT_tUnit
//! \return	dp correction, normally 0, >0 for higher resolutions
//--------------------------------------------------------------------------------------------------
static int GetDpCorrection(WP_UNIT_tUnit unit)
{
	if (DISPLAY_RESOLUTION_DATA > 0) {
		WP_tFloat step = (WP_tFloat)FixUnits[unit][FinestRangeIndex].step;
		step /= (WP_tFloat)displayResolutionEncoding[DISPLAY_RESOLUTION_DATA];
		if (step < WP_FLT_ONE) {
			return EvaluateFirstSignificantDecimal(step);
		}
	}
	return 0;
}


//--------------------------------------------------------------------------------------------------
// RoundStep
//--------------------------------------------------------------------------------------------------
//! \brief	Round to the nearest step which is 0, 1, 2 or 5 times 10*n
//!
//! The rounding is not mathematically exact because integers are used and it is only used for some
//! special cases where it doesn't matter.
//! The two cases (and their variations) which have to be rounded are:
//! 2 * 2 = 4 -> round to 5
//! 5 * 5 = 25 -> round to 20
//!
//! Therefore it is rounded according to the following table:
//! \verbatim
//! 0 -> 0
//! 1 -> 1
//! 2 -> 2
//! 3 -> 5
//! 4 -> 5
//! 5 -> 5
//! 6 -> 5
//! 7 -> 10
//! 8 -> 10
//! 9 -> 10
//! \endverbatim
//!
//! The unit lb:oz requires special handling. For the oz part a step of 5 is rounded to 4 instead
//! because 16 isn't divisible by 5.
//!
//! \param	step	Step to be rounded
//! \param	unit	Unit of type WP_UNIT_tUnit
//! \return	Rounded value
//--------------------------------------------------------------------------------------------------
static uint16_t RoundStep(uint16_t step, WP_UNIT_tUnit unit)
{
	uint16_t mult = 1u;
	int stepDigits = 0u;
	// Only the most significant digit is of relevance
	while (step >= 10u) {
		step = step / 10u;
		mult *= 10u;
		stepDigits++;
	}
	if (step <= 2u) {
		return (uint16_t)(step * mult);
	} else if (step <= 6u) {
		// Special rounding to 4,40,.. instead of 5,50,.. for oz-part of lb:oz
		if (unit == WP_UNIT_lboz) {
			if (WP_UNIT_GetUnitDP(unit) <= stepDigits) {
				return (uint16_t)(4u * mult);
			}
		}
		return (uint16_t)(5u * mult);
	} else {
		return (uint16_t)(10u * mult);
	}
}


#endif // WP_FX_BASIC
