//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Unit.h
//! \ingroup	util
//! \brief		Functions for weight unit conversions.
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
//! \author		Pius Derungs, Matthias Klaey, Martin Heusser
//
// $Date: 2017/04/03 17:56:54MESZ $
// $Revision: 1.52 $
//
//==================================================================================================

#ifndef _WP_Unit__h
#define _WP_Unit__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include	"RB_Config.h"

#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)

#include "WP_Typedefs.h"

#if defined(WP_FX_CUSTOM_UNITS)
	#include "WP_Data.h"
#endif

//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Status
typedef enum {
	WP_UNIT_OK			= 0,	//!< Operation succeded
	WP_UNIT_RANGE_FAIL	= 1,	//!< Operation failed, parameter out of range
	WP_UNIT_STORE_FAIL	= 2		//!< Operation failed, parameter can not be stored
} RB_DECL_TYPE WP_UNIT_tStatus;


//! Unit enumeration. FixUnits are the standard weighing units WP_UNIT_g ... WP_UNIT_lboz which has a
//! fixed unit factor. Custom units are the four units WP_UNIT_pieces ... WP_UNIT_custom_2
typedef enum {
	WP_UNIT_g			= 0,	//!< Gram
	WP_UNIT_kg			= 1,	//!< Kilogram
	WP_UNIT_t			= 2,	//!< Ton
	WP_UNIT_mg			= 3,	//!< Milligram
	WP_UNIT_ug			= 4,	//!< Microgram
	WP_UNIT_ct			= 5,	//!< Carat
	WP_UNIT_N			= 6,	//!< Newton
	WP_UNIT_lb			= 7,	//!< Pound
	WP_UNIT_oz			= 8,	//!< Ounce
	WP_UNIT_ozt			= 9,	//!< Troy ounce
	WP_UNIT_GN			= 10,	//!< Grain
	WP_UNIT_dwt			= 11,	//!< Pennyweight
	WP_UNIT_mo			= 12,	//!< Momme
	WP_UNIT_msg			= 13,	//!< Mesghal
	WP_UNIT_htl			= 14,	//!< Hong Kong tael
	WP_UNIT_stl			= 15,	//!< Singapore tael
	WP_UNIT_ttl			= 16,	//!< Taiwan tael
	WP_UNIT_tical		= 17,	//!< Tical
	WP_UNIT_tola		= 18,	//!< Tola
	WP_UNIT_baht		= 19,	//!< Baht
	WP_UNIT_lboz		= 20,	//!< lb:oz

	WP_UNIT_none		= 25,	//!< No unit

	WP_UNIT_pieces		= 26,	//!< Pieces
	WP_UNIT_percent		= 27,	//!< Percent
	WP_UNIT_custom_1	= 28,	//!< # Custom unit 1
	WP_UNIT_custom_2	= 29,	//!< # Custom unit 2
	WP_UNIT_currency_1	= 30,	//!< Currency 1
	WP_UNIT_currency_2	= 31	//!< Currency 2
} RB_DECL_TYPE WP_UNIT_tUnit;

//! Defines the last enumeration of fixed units for boundary tests or loops
#define WP_UNIT_LAST_FIX WP_UNIT_lboz

//! Unit selectors
typedef enum {
	WP_UNIT_SICS		= 0,		//!< Active unit for MT-SICS
	WP_UNIT_DISPLAY		= 1,		//!< Active display unit
	WP_UNIT_1			= 2,		//!< Active unit 1
	WP_UNIT_2			= 3			//!< Active unit 2
} RB_DECL_TYPE WP_UNIT_tUnitSelection;


//! Definition of TD-parameters for the unit module
typedef struct {
	WP_UNIT_tUnit 	defUnit;							//!< Definition unit (default: 0, related to XP0317)
	int32_t			defDP;								//!< Decimal places of definition unit (default: 2, related to XP0338,XP0339)
	uint16_t		defStep[RB_CONFIG_WP_RANGES_MAX];	//!< Step of definition unit in increasing order (default: 1, related to XP0338,XP0339)
	WP_tFloat		dpReduction;						//!< Reduction of reduced units (default: 1.0)
	uint32_t		reducedUnitMask;					//!< Mask of reduced units (default: 0xFFFFFFFF, related to XP0904, XP0334)
	uint32_t		validDisplayUnitMask;				//!< Mask of valid units (default: 0xFFFFFFFF, related to XP0320)
	WP_UNIT_tUnit 	defDisplayUnit;						//!< Default display unit (related to XP0320)
	uint32_t		validHostUnitMask;					//!< Mask of valid units (default: 0xFFFFFFFF, related to XP0321)
	WP_UNIT_tUnit 	defHostUnit;						//!< Default host unit (related to XP0321)
	#if defined(WP_FX_2ND_RANGE_DETECTOR)
	WP_UNIT_tUnit 	rngDet2DefUnit;						//!< Definition unit for 2nd range detector
	uint32_t		rngDet2UnitMask;					//!< Unit mask for 2nd range detector
	#endif
} RB_DECL_TYPE WP_UNIT_tTDParams;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_UNIT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module UNIT
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_UNIT_Initialize(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module UNIT
//!
//! \param	pUnitParams		Pointer to a set of typedefinition parameters for the module UNIT
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_UNIT_SetTDParams(const WP_UNIT_tTDParams* pUnitParams);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the typedefinition parameterset of the module UNIT
//!
//! \return	Pointer to the typedefinition parameterset
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const WP_UNIT_tTDParams* WP_UNIT_GetTDParams(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetBaseResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the base resolution in grams
//!
//! The base resolution corresponds to the smallest value defined in XP0338 and XP0339 parameters.
//! No corrections are applied according to UserStep, service mode or reduced units.
//!
//! \note The base resolution can be a fraction number (i.e. 0.00204..) if balance is defined in lb
//!
//! \return	Base resolution in gram
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetBaseResolution(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition unit defined as XP0317
//!
//! \return	Definition unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetDefinitionUnit(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionStep
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition step defined as part of XP0339
//!
//! \return	Definition step
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_UNIT_GetDefinitionStep(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionDP
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition decimal places defined as part of XP0339
//!
//! \return	Definition decimal places
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int WP_UNIT_GetDefinitionDP(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetHostUnitMask
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for host
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t WP_UNIT_GetHostUnitMask(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefaultHostUnit
//--------------------------------------------------------------------------------------------------
//! \brief	get default unit for host
//!
//! \return	Default host unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetDefaultHostUnit(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDisplayUnitMask
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for display
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t WP_UNIT_GetDisplayUnitMask(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefaultDisplayUnit
//--------------------------------------------------------------------------------------------------
//! \brief	get default unit for display
//!
//! \return	Default display unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetDefaultDisplayUnit(void);


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
RB_DECL_FUNC WP_UNIT_tStatus WP_UNIT_SetUserStepFactor(uint16_t stepFactor);


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
RB_DECL_FUNC uint16_t WP_UNIT_GetUserStepFactor(void);


#if defined(WP_FX_2ND_RANGE_DETECTOR)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitMaskRangeDetector2
//--------------------------------------------------------------------------------------------------
//! \brief	get unit mask for 2nd range detector
//!
//! \return	Unit mask
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint32_t WP_UNIT_GetUnitMaskRangeDetector2(void);
#endif

#if defined(WP_FX_2ND_RANGE_DETECTOR)
//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetDefinitionUnitRangeDetector2
//--------------------------------------------------------------------------------------------------
//! \brief	Get the definition unit of 2nd range detector
//!
//! \return	Definition unit of 2nd range detector
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetDefinitionUnitRangeDetector2(void);
#endif

//--------------------------------------------------------------------------------------------------
// WP_UNIT_ActivateUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Set the actual unit for the corresponding unit selection
//!
//! Set the actual unit and store it permanently
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	unit		Unit to be set as actual unit for unit selection
//! \return				Status (WP_OK, WP_RANGE_FAIL or WP_STORE_FAIL)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tStatus WP_UNIT_ActivateUnit(WP_UNIT_tUnitSelection unitSel, WP_UNIT_tUnit unit);

//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual unit for the corresponding unit selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual unit of the specific unit selection
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetActiveUnit(WP_UNIT_tUnitSelection unitSel);

//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveUnitText
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual unit text for the corresponding unit selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual unit text of the specific unit selection (never NULL, pointer to "")
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* WP_UNIT_GetActiveUnitText(WP_UNIT_tUnitSelection unitSel);

//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveDigitResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual digit resolution for the corresponding unit selection
//!
//! This function takes into consideration the settings of the SysOperatingMode (e.g. SERVICE)
//! and the unit specific ReductionFactor
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Actual digit resolution of the specific unit selection
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetActiveDigitResolution(WP_UNIT_tUnitSelection unitSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveDP
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual decimal places for the corresponding unit selection
//!
//! This function takes into acount the settings of the SysOperatingMode (e.g. SERVICE)
//! and the unit specific ReductionFactor
//!
//! \param	unitSel	Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \return	Decimal places location of the specific unit selection
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC int WP_UNIT_GetActiveDP(WP_UNIT_tUnitSelection unitSel);


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
RB_DECL_FUNC WP_tFloat WP_UNIT_GetActiveRangeFactor(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveRangeStep
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual step for the corresponding unit and range selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	rngSel		Range selection, to get the actual step factor for
//! \return	Actual step of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_UNIT_GetActiveRangeStep(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetActiveRangeDecade
//--------------------------------------------------------------------------------------------------
//! \brief	Get the actual range decade for the corresponding unit and range selection
//!
//! \param	unitSel		Unit selection, range WP_UNIT_SICS..WP_UNIT_2
//! \param	rngSel		Range status, to get the actual range decade for
//! \return	Actual range decade of the specific unit and range selections
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_UNIT_GetActiveRangeDecade(WP_UNIT_tUnitSelection unitSel, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the conversion factor for the corresponding unit (incl. custom unit)
//!
//! \param	unit	Unit, to get the conversion factor for
//! \return	Conversion factor of the specific unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetUnitFactor(WP_UNIT_tUnit unit);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitDigitResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the digit resolution for the corresponding unit (incl. custom unit)
//!
//! \param	unit	Unit, to get the digit resolution for.
//! \return	Digit resolution of the specific unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetUnitDigitResolution(WP_UNIT_tUnit unit);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeResolution
//--------------------------------------------------------------------------------------------------
//! \brief	Get the resolution for the corresponding unit and range (incl. custom unit)
//!
//! \param	unit	Unit, to get the digit resolution for.
//! \param	rngSel	Range selection, to get the actual step factor for
//! \return	Digit resolution of the specific unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetUnitRangeResolution(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel);


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
RB_DECL_FUNC int WP_UNIT_GetUnitDP(WP_UNIT_tUnit unit);


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
RB_DECL_FUNC uint16_t WP_UNIT_GetUnitRangeStep(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeFactor
//--------------------------------------------------------------------------------------------------
//! \brief	Get the step factor for the corresponding unit and range specifications
//!
//! Only fix units supports multiple ranges with a range factor, all other return 1.0
//!
//! \param	unit		Unit, to get the step factor for
//! \param	rngSel		Range status, to get the step factor for
//! \return	Step factor of the specific unit and range specifications
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_GetUnitRangeFactor(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitRangeDecade
//--------------------------------------------------------------------------------------------------
//! \brief	Get the step decade for the corresponding unit and range specifications
//!
//! \param	unit		Unit, to get the step decade for
//! \param	rngSel		Range status, to get the range decade for
//! \return	Step decade of the specific unit and range specifications
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC uint16_t WP_UNIT_GetUnitRangeDecade(WP_UNIT_tUnit unit, WP_tRangeIndex rngSel);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_CalculateDeflection
//--------------------------------------------------------------------------------------------------
//! \brief	Calculate deflection value for e.g. SNR command
//!
//! The deflection value depends on the resolution of balance:
//!     - Resolution >= 1g:     Deflection = 5 * Resolution
//!     - Resolution >= 0.1g:   Deflection = 10 * Resolution
//!     - Resolution >= 0.01g:  Deflection = 100 * Resolution
//!     - Resolution <  0.01g:  Deflection = 1000 * Resolution
//!
//! \return	Deflection value in gram
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_tFloat WP_UNIT_CalculateDeflection(void);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetUnitText
//--------------------------------------------------------------------------------------------------
//! \brief	Return the unit text (incl. custom unit text)
//!
//! \param	unit	Unit, to get the unitstring for
//! \return	Pointer to the unitstring of the specific unit (pointer to "" in case of invalid unit)
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const char* WP_UNIT_GetUnitText(WP_UNIT_tUnit unit);


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
RB_DECL_FUNC bool WP_UNIT_SetCustomUnitSpecs(WP_UNIT_tUnit unit, const WP_DATA_tCustomUnit* pUnitSpecs);
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
RB_DECL_FUNC const WP_DATA_tCustomUnit* WP_UNIT_GetCustomUnitSpecs(WP_UNIT_tUnit unit);
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
RB_DECL_FUNC bool WP_UNIT_IsCustomUnitTextValid(const char* pCustomUnitText);
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
RB_DECL_FUNC bool WP_UNIT_IsUnitValid(WP_UNIT_tUnitSelection unitSel, WP_UNIT_tUnit unit);


//--------------------------------------------------------------------------------------------------
// WP_UNIT_GetFirstValidUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Return first valid fix unit
//!
//! \param	unitSel		Unit selection, to use for the check
//! \return	unit
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_GetFirstValidUnit(WP_UNIT_tUnitSelection unitSel);


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
RB_DECL_FUNC WP_UNIT_tUnit WP_UNIT_ParseUnit(const char* unitStr);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_WP
#endif // _WP_Unit__h
