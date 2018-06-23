//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Weight.c
//! \ingroup	util
//! \brief		Functions for weight conversions.
//!
//! This module contains the functions for weight conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Pius Derungs, Martin Heusser
//
// $Date: 2017/04/25 11:00:10MESZ $
// $Revision: 1.92 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "WP_Weight"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "WP_Weight.h"

#if defined(WP_FX_BASIC)

#include "WP_Data.h"

#if !defined(RB_CONFIG_USE_TYPE) || (RB_CONFIG_USE_TYPE == RB_CONFIG_NO)
	#error RB_CONFIG_USE_TYPE must be defined and set to RB_CONFIG_YES in RB_Config.h
#endif

#include "RB_Debug.h"
#include "RB_Format.h"
#include "RB_Math.h"
#include "RB_String.h"

#include <string.h>


//==================================================================================================
//  L O C A L   D E F I N I T I O N S
//==================================================================================================
#define SPACE_TERM		1		//!< Space needed for terminating zero
#define SPACE_DOT		1		//!< Space needed for decimal point
#define SPACE_COLON		1		//!< Space needed for colon (lb:oz only)
#define SPACE_OUNCE		2		//!< Space needed for ounces (lb:oz only)

//==================================================================================================
//  L O C A L   V A R I A B L E S
//==================================================================================================

//! Typedefinition parameters for the module WEIGHT
static WP_WEIGHT_tTDParams WeightParams;


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module WEIGHT
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_WEIGHT_Initialize(void)
	{
	}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module WEIGHT
//!
//! \param	pWeightParams	Pointer to a set of type definition parameters for the module WEIGHT
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_WEIGHT_SetTDParams(const WP_WEIGHT_tTDParams* pWeightParams)
	{
	WeightParams = *pWeightParams;
	}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the type definition parameter set of the module WEIGHT
//!
//! \return	Pointer to the type definition parameter set
//--------------------------------------------------------------------------------------------------
const WP_WEIGHT_tTDParams* WP_WEIGHT_GetTDParams(void)
{
	return (&WeightParams);
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsWgtSigStable
//--------------------------------------------------------------------------------------------------
//! \brief	Check if weight is stable
//!
//! \param	pWgt	Pointer to weight to be checked
//! \return	<c>true</c> if weight is available and stable, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_WEIGHT_IsWgtSigStable(const WP_WEIGHT_tWeight* pWgt)
{
	return (pWgt && WP_WEIGHT_IsAvailable(pWgt->status) && pWgt->stability.count > 0U);
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsRecalculateGrossWeightSet
//--------------------------------------------------------------------------------------------------
//! \brief	Check if gross weight should be recalculated from net and tare
//!
//! \return	<c>true</c> if gross weight is recalculated from net and tare, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_WEIGHT_IsRecalculateGrossWeightSet(void)
{
	return WeightParams.recalculateGross;
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsAvailable
//--------------------------------------------------------------------------------------------------
//! \brief	Check if a specific weight status represents an available weight
//!
//! \param	wgtSts  Weight status to be checked
//! \return	<c>true</c> if specific weight status represents an available weight, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_WEIGHT_IsAvailable(WP_WEIGHT_tWgtStatus wgtSts)
{
	switch (wgtSts)
		{
		case WP_WEIGHT_STS_AVAILABLE:
		case WP_WEIGHT_STS_INEXACT:
			return(true);
		default:
			break;
		}
	return(false);
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsValidAndWeighing
//--------------------------------------------------------------------------------------------------
//! \brief	Check if a specific weight status represents a valid weight and the balance is weighing
//!
//! As a addition to WP_WEIGHT_IsAvailable(), this function also takes care about fatal errors and
//! balance operating state. The functions returns true only when all the following condition are met:
//! - wgtSts == WP_WEIGHT_STS_AVAILABLE || wgtSts == WP_WEIGHT_STS_INEXACT
//! - WP_STATE_GetFatalError() == 0
//! - WP_STATE_GetBalOpState() == WP_STATE_BAL_OP_WEIGHING
//!
//! \param	wgtSts  Weight status to be checked
//! \return	<c>true</c> if specific weight status represents an valid weight, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
bool WP_WEIGHT_IsValidAndWeighing(WP_WEIGHT_tWgtStatus wgtSts) RB_ATTR_THREAD_SAFE
{
	if (WP_STATE_GetFatalError() == 0)
		if (WP_STATE_GetBalOpState() == WP_STATE_BAL_OP_WEIGHING)
			return(WP_WEIGHT_IsAvailable(wgtSts));
	return(false);
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_ConvertToActualUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a weight item to a rounded weight item fitting the actual unit settings
//!
//! \param	pWeight		input	Pointer to weight to be converted
//! \param	pRndWgt		output	Pointer to converted and rounded weight
//! \param	unitSel		input	Selection of actual output unit
//! \param	valSel		input	Selection of value item to be converted
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_WEIGHT_ConvertToActualUnit(const WP_WEIGHT_tWeight* pWeight,
								   WP_WEIGHT_tRoundedWeight* pRndWgt,
								   WP_UNIT_tUnitSelection unitSel,
								   WP_WEIGHT_tValSelection valSel)
{
	WP_WEIGHT_ConvertToUnit(pWeight, pRndWgt, WP_UNIT_GetActiveUnit(unitSel), valSel);
}

//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_ConvertToUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a weight item to a rounded weight item fitting a specific conversion unit
//!
//! \param	pWeight		input	Pointer to weight to be converted
//! \param	pRndWgt		output	Pointer to converted and rounded weight
//! \param	unitSel		input	Selection of actual output unit
//! \param	valSel		input	Selection of value item to be converted
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_WEIGHT_ConvertToUnit(const WP_WEIGHT_tWeight* pWeight,
							 WP_WEIGHT_tRoundedWeight* pRndWgt,
							 WP_UNIT_tUnit unit,
							 WP_WEIGHT_tValSelection valSel)
	{

	pRndWgt->value = (WP_tFloat)0.0;
	pRndWgt->valType = WP_WEIGHT_TYPE_NONE;
	pRndWgt->dp = 0;
	pRndWgt->rangeDecade = 0;
	pRndWgt->stbFlag = false;
	pRndWgt->centerOfZero = pWeight->centerOfZero;
	pRndWgt->unit = unit;
	pRndWgt->valStatus = pWeight->status;

	// process LFT mode (CustomUnits are not approvable)
	pRndWgt->lftMode = WP_STATE_GetLFTMode();
	if (pRndWgt->lftMode >= WP_STATE_LFTMODE_1_D_APPR && unit >= WP_UNIT_pieces)
		pRndWgt->lftMode = WP_STATE_LFTMODE_NOT_APPR;

	if ((pRndWgt->valStatus != WP_WEIGHT_STS_UNDEFINED) || (valSel == WP_WEIGHT_SEL_TARE))
		{
		WP_tRangeIndex range;
		WP_tFloat factor            = WP_UNIT_GetUnitFactor(unit);
		WP_tRangeIndex tareRange    = pWeight->tare.range;
		WP_tRangeIndex grossRange   = pWeight->gross.range;
		WP_tRangeIndex netRange     = pWeight->net.range;
		WP_WEIGHT_tWgtStatus status = pWeight->status;

		#if defined(WP_FX_2ND_RANGE_DETECTOR)
		if (((1uL << (int)unit) & WP_UNIT_GetUnitMaskRangeDetector2()) != 0)
			{
			// Use range and status from 2nd range detector
			tareRange  = pWeight->tare.range2;
			grossRange = pWeight->gross.range2;
			netRange   = pWeight->net.range2;
			status     = pWeight->status2;
			}
		#endif

		// *** Adjust the weightvalue, weighttype, weightstate and approvalstate to the valueselection
		switch (valSel)
			{
			case WP_WEIGHT_SEL_TARE:
				range = tareRange;
				pRndWgt->value = pWeight->tare.value;
				pRndWgt->valStatus = WP_WEIGHT_STS_AVAILABLE;
				status = WP_WEIGHT_STS_AVAILABLE;
				if (pWeight->tare.status == WP_WEIGHT_TARE_PRETARED)
					{
					pRndWgt->valType = WP_WEIGHT_TYPE_PT;
					if (pRndWgt->lftMode >= WP_STATE_LFTMODE_1_D_APPR)
						pRndWgt->lftMode = WP_STATE_LFTMODE_NOT_APPR;	// PT is not approvable
					}
				else
					{
					pRndWgt->valType = WP_WEIGHT_TYPE_T;
					}
				break;

			case WP_WEIGHT_SEL_GROSS:
				range = grossRange;
				pRndWgt->value = pWeight->gross.value;
				switch (pWeight->tare.status)
					{
					case WP_WEIGHT_TARE_DONE:
						pRndWgt->valType = WP_WEIGHT_TYPE_GROSS_T;
						if (pRndWgt->valStatus == WP_WEIGHT_STS_INEXACT)
							pRndWgt->valStatus = WP_WEIGHT_STS_AVAILABLE;
						break;
					case WP_WEIGHT_TARE_PRETARED:
						pRndWgt->valType = WP_WEIGHT_TYPE_GROSS_PT;
						if (pRndWgt->valStatus == WP_WEIGHT_STS_INEXACT)
							pRndWgt->valStatus = WP_WEIGHT_STS_AVAILABLE;
						break;
					default:
						pRndWgt->valType = WP_WEIGHT_TYPE_GROSS;
						break;
					}
				break;

			default:	// WP_WEIGHT_SEL_NET
				range = netRange;
				pRndWgt->value = pWeight->net.value;
				switch (pWeight->tare.status)
					{
					case WP_WEIGHT_TARE_DONE:
						pRndWgt->valType = WP_WEIGHT_TYPE_NET_T;
						break;
					case WP_WEIGHT_TARE_PRETARED:
						pRndWgt->valType = WP_WEIGHT_TYPE_NET_PT;
						break;
					default:
						pRndWgt->valType = WP_WEIGHT_TYPE_NET;
						break;
					}
				break;
			}

		pRndWgt->valStatus = status;
		pRndWgt->range = range;

		// *** Conversion formula according to the conversion unit
		pRndWgt->resolution = WP_UNIT_GetUnitRangeResolution(unit,range);

		#if defined(WP_FX_CUSTOM_UNITS)
			{
			const WP_DATA_tCustomUnit* cuSpec = WP_UNIT_GetCustomUnitSpecs(unit);
			if (cuSpec)
				{
				// cuSpec available
				switch (cuSpec->rounding)
					{
					case WP_DATA_CU_ROUND_NONE:
						break;

					case WP_DATA_CU_ROUND_BASE:
						pRndWgt->value = (WP_tFloat)RB_MATH_Round((float64)pRndWgt->value, (float64)WP_UNIT_GetBaseResolution());
						break;

					default:
					case WP_DATA_CU_ROUND_RANGE:
						pRndWgt->value = (WP_tFloat)RB_MATH_Round((float64)pRndWgt->value, (float64)WP_UNIT_GetUnitRangeResolution(WP_UNIT_g, range));
						break;
					}

				switch (cuSpec->formula)
					{
					case WP_DATA_CU_FORMULA_MUL:
						// Output = (FACTOR * WEIGHT) + OFFSET
						pRndWgt->value *= cuSpec->factor;
						break;

					case WP_DATA_CU_FORMULA_DIV:
							{
							// Output = (FACTOR / WEIGHT) + OFFSET
							WP_tFloat lim = WP_UNIT_GetBaseResolution() * (WP_tFloat)0.5;
							if ((pRndWgt->value < lim) && (pRndWgt->value > -lim))
								{
								// Weight is zero, so the result is also zero
								pRndWgt->value = WP_FLT_ZERO;
								}
							else
								{
								// Calculate current 'DIV'-resolution 'currStepu'.
								// based on the current loaded weight and the base resolution
								WP_tFloat baseResol = WP_UNIT_GetBaseResolution();
								WP_tFloat cuFactor = (cuSpec->factor < WP_FLT_ZERO)? -cuSpec->factor : cuSpec->factor;
								WP_tFloat currLoad = (pRndWgt->value < WP_FLT_ZERO)? -pRndWgt->value : pRndWgt->value;
								WP_tFloat currStepu = cuFactor * baseResol / (currLoad * (currLoad + baseResol));

								// if current 'DIV'-resolution greater than defined 'DIV'-resolution then take current resolution
								if ( currStepu > pRndWgt->resolution )
									{
									// Round 'currStepu' upward! (steps 1, 2, 5 or 10)
									int32_t exponent = RB_MATH_ilog10(currStepu);
									WP_tFloat pow10Exp = (WP_tFloat)RB_MATH_ipow10(exponent);
									WP_tFloat mantisse_u = currStepu / pow10Exp;
									WP_tFloat mantisse_r = WP_FLT_ONE;
									if (mantisse_u <= WP_FLT_ONE)
										mantisse_r = WP_FLT_ONE;
									else if (mantisse_u <= (WP_tFloat)2.0)
										mantisse_r = (WP_tFloat)2.0;
									else if (mantisse_u <= (WP_tFloat)5.0)
										mantisse_r = (WP_tFloat)5.0;
									else
										{
										exponent++;
										pow10Exp = (WP_tFloat)RB_MATH_ipow10(exponent);
										}
									pRndWgt->resolution = mantisse_r * pow10Exp;
									}
								pRndWgt->value = cuSpec->factor / pRndWgt->value;
								}
							}
						break;

					default:
					case WP_DATA_CU_FORMULA_NONE:
						break;
					}
				// Build absolute value of factor for the stability check below (HPQC#5816)
				if (factor < WP_FLT_ZERO)
					factor = -factor;

				pRndWgt->value += cuSpec->offset;
				}
			else
				{
				pRndWgt->value *= factor;
				}
			}
		#else
		pRndWgt->value *= factor;
		#endif

		// *** Rounding according to the resolution
		pRndWgt->value = (WP_tFloat)RB_MATH_Round((float64)pRndWgt->value, (float64)pRndWgt->resolution);

		// *** Evaluation of output-stability
		if (valSel == WP_WEIGHT_SEL_TARE)
			{
			// Tare is always stable
			pRndWgt->stbFlag = true;
			}
		else
			{
			if (pWeight->stability.width * factor < WeightParams.outStabilityWidth * pRndWgt->resolution)
				{
				// check if stabilitylevel of outputresolution is reached
				pRndWgt->stbFlag = true;
				}
			else
				{
				// else take signalstability
				pRndWgt->stbFlag = (pWeight->stability.count > 0U);
				}

			}
		// Update dp and rangedecade according to unit and range
		pRndWgt->dp = WP_UNIT_GetUnitDP(unit);
		pRndWgt->rangeDecade = WP_UNIT_GetUnitRangeDecade(unit,range);
		}
	}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_ConvertGrossNetTare
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a weight item to rounded gross, net and tare weights
//!
//! \param	pWeight		Pointer to weight to be converted (input)
//! \param	pRndGross	Pointer to converted and rounded gross (output)
//! \param	pRndNet		Pointer to converted and rounded net (output)
//! \param	pRndTare	Pointer to converted and rounded tare (output)
//! \param	unit		Conversion unit
//! \return	None
//--------------------------------------------------------------------------------------------------
void WP_WEIGHT_ConvertGrossNetTare(const WP_WEIGHT_tWeight* pWeight,
							 WP_WEIGHT_tRoundedWeight* pRndGross,
							 WP_WEIGHT_tRoundedWeight* pRndNet,
							 WP_WEIGHT_tRoundedWeight* pRndTare,
							 WP_UNIT_tUnit unit)
{
	if (pRndGross && pRndNet && pRndTare)
	{
		WP_WEIGHT_tWeight weight = *pWeight;
		if (WP_WEIGHT_IsRecalculateGrossWeightSet())
		{
			#if defined(WP_FX_CNFPAR_STD)
			// Put tare range into net range to round tare in net range
			if (WP_DATA.XP0907 == 2)
			{
				// Round Tare to the same number of decimal places as Net
				weight.tare.range = weight.net.range;
				#if defined(WP_FX_2ND_RANGE_DETECTOR)
				// Use range and status from 2nd range detector
				weight.tare.range2 = weight.net.range2;
				#endif
			}
			#endif
			WP_WEIGHT_ConvertToUnit(&weight, pRndTare,  unit, WP_WEIGHT_SEL_TARE);
			WP_WEIGHT_ConvertToUnit(&weight, pRndNet,   unit, WP_WEIGHT_SEL_NET);
			*pRndGross = *pRndNet; // Transfer all attributes of N to G
			pRndGross->value = pRndNet->value + pRndTare->value; // Recalculate gross value
			pRndGross->valType = (WP_WEIGHT_tWgtType)((int)pRndGross->valType + ((int)WP_WEIGHT_TYPE_GROSS - (int)WP_WEIGHT_TYPE_NET));
			#if defined(WP_FX_CNFPAR_STD)
			if (WP_DATA.XP0907 == 2)
			{
				// Gross and Tare have all the same number of decimal places as Net
				pRndTare->rangeDecade = pRndNet->rangeDecade;
			}
			else
			#endif
			{
				// Gross has same number of decimal places as weight in smallest range
				pRndGross->rangeDecade = pRndNet->rangeDecade < pRndTare->rangeDecade ? pRndNet->rangeDecade : pRndTare->rangeDecade;
			}
		}
		else
		{
			WP_WEIGHT_ConvertToUnit(&weight, pRndTare,  unit, WP_WEIGHT_SEL_TARE);
			WP_WEIGHT_ConvertToUnit(&weight, pRndNet,   unit, WP_WEIGHT_SEL_NET);
			WP_WEIGHT_ConvertToUnit(&weight, pRndGross, unit, WP_WEIGHT_SEL_GROSS);
		}
	}
	else
	{
		RB_DEBUG_FAIL("Invalid struct reference (NULL)");
	}
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_GetWeightStr
//--------------------------------------------------------------------------------------------------
//! \brief	Get weight value as formatted string
//!
//! Weight string with appended blanks according to the rangedecade of reduced resolution. The
//! output format can be configured by RB_CONFIG_WP_SHOW_REDUCED_DIGITS and
//! RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS within RB_Config.h
//!
//! Example of weight strings with RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS missing or configured
//! to RB_CONFIG_NO (normal case)
//!
//! \verbatim
//!   "123.45"   normal weight
//!   "123.5 "   weight with reduced resolution (1d/10d)
//!   "123   "   weight with reduced resolution (1d/100d)
//!   "120   "   weight with reduced resolution (1d/1000d)
//! \endverbatim
//!
//! Example of weight strings with RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS configured to RB_CONFIG_YES
//!
//! \verbatim
//!   "123.45"   normal weight
//!   "123.5 "   weight with reduced resolution (1d/10d)
//!   "123.  "   weight with reduced resolution (1d/100d)
//!   "120.  "   weight with reduced resolution (1d/1000d)
//! \endverbatim
//!
//! Example of weight strings with RB_CONFIG_WP_SHOW_REDUCED_DIGITS configured RB_CONFIG_YES
//!
//! \verbatim
//!   "123.45"   normal weight
//!   "123.50"   weight with reduced resolution (1d/10d)
//!   "123.00"   weight with reduced resolution (1d/100d)
//!   "120.00"   weight with reduced resolution (1d/1000d)
//! \endverbatim
//!
//!
//! \param	pRndWgt		Pointer to rounded and converted weight (input)
//! \param	wgtStr		Pointer to stringbuffer to be filled up with range-formatted weight string
//! \param	maxBufLen	Max length of string buffer (incl.'\\0', i.e. buffersize)
//! \return	Number of characters in string
//--------------------------------------------------------------------------------------------------
size_t WP_WEIGHT_GetWeightStr(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* wgtStr, size_t maxBufLen)
{
	#if  defined(RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS) && (RB_CONFIG_WP_SHOW_DOT_WITHOUT_DECIMALS == RB_CONFIG_YES)
	return(WP_WEIGHT_WeightToString(pRndWgt, wgtStr, maxBufLen, true));
	#else
	return(WP_WEIGHT_WeightToString(pRndWgt, wgtStr, maxBufLen, false));
	#endif
}


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_GetSicsStr
//--------------------------------------------------------------------------------------------------
//! \brief	Get weight as SICS-string for S, SI, SIR
//!
//! Example of SICS-strings:
//!
//! \verbatim
//! "S S    123.45 g"   for stable weights
//! "S D    123.45 g"   for unstable weights
//! "S S    123.5  g"   for stable weights with reduced resolution
//! "S D    123.5  g"   for unstable weights with reduced resolution
//! "S +"               for overload
//! "S -"               for underload
//! "S I"               weight not available
//! \endverbatim
//!
//!
//! \param	pRndWgt		Pointer to rounded and converted weight (input)
//! \param	sicsStr		Pointer to stringbuffer to be filled up with range-formatted weight string
//! \param	maxBufLen	Max length of string buffer (for weight string with unit -> min maxBufLen = 15 + RB_CONFIG_WP_UNIT_TEXT_MAXLEN)
//! \return	Number of characters in string
//--------------------------------------------------------------------------------------------------
size_t WP_WEIGHT_GetSicsStr(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* sicsStr, size_t maxBufLen)
{
	if (!sicsStr || maxBufLen < 1U)
		return (0);

	sicsStr[0] = '\0';
	if (maxBufLen < 4U)
		return (0);

	sicsStr[0] = 'S';
	sicsStr[1] = ' ';
	sicsStr[2] = 'I';
	sicsStr[3] = '\0';

	if (!pRndWgt)
		return (0);	// pRndWgt not available --> "S I"

	if (pRndWgt->valType < WP_WEIGHT_TYPE_NET || pRndWgt->valType >= WP_WEIGHT_TYPE_T )
		return (0);	// valType not available for SICS-string --> "S I"

	switch(pRndWgt->valStatus) {
		case WP_WEIGHT_STS_OVERLOAD:
		case WP_WEIGHT_STS_TYPEOVER:
			sicsStr[2] = '+';
			break;

		case WP_WEIGHT_STS_UNDERLOAD:
		case WP_WEIGHT_STS_TYPEUNDER:
			sicsStr[2] = '-';
			break;

		case WP_WEIGHT_STS_AVAILABLE:
		case WP_WEIGHT_STS_INEXACT:
			if (maxBufLen >= 20U) {
				int i;
				size_t wgtLen;
				char wgtStr[11]; // 10 + 1 for termination
				sicsStr[2] = (char)((pRndWgt->stbFlag)? 'S':'D');
				sicsStr[3] = ' ';
				wgtLen = WP_WEIGHT_WeightToString(pRndWgt, wgtStr, 11, true);
				for (i = 13; i > 3; i--) {
					if (wgtLen > 0)
						sicsStr[i] = wgtStr[--wgtLen];
					else
						sicsStr[i] = ' ';
				}
				sicsStr[14] = ' ';
				sicsStr[15] = '\0';
				RB_STRING_strncpymax(&sicsStr[15], WP_UNIT_GetUnitText(pRndWgt->unit), maxBufLen - 15);
			}
			break;

		default: // valStatus undefined --> "S I"
			break;
	}

	return (strlen(sicsStr));
}


//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_WeightToString
//--------------------------------------------------------------------------------------------------
//! \brief	Get weight value as string
//!
//! Weight string with appended blanks according to the rangedecade of reduced resolution
//! Example of weight strings:  "123.45"   normal weight
//!                             "123.5 "   weight with reduced resolution (1d/10d)
//!                             "123   "   weight with reduced resolution (1d/100d , showDot = false)
//!                             "123.  "   weight with reduced resolution (1d/100d , showDot = true)
//!                             "120   "   weight with reduced resolution (1d/1000d, showDot = false)
//!                             "120.  "   weight with reduced resolution (1d/1000d, showDot = true)
//!
//! \param	pRndWgt		Pointer to rounded and converted weight (input)
//! \param	wgtStr		Pointer to stringbuffer to be filled up with range-formatted weight string
//! \param	maxBufLen	Max length of string (incl.'\\0', i.e. buffersize)
//! \param	showDot		Show dot as last character
//! \return	Number of chars in string
//--------------------------------------------------------------------------------------------------
size_t WP_WEIGHT_WeightToString(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* wgtStr, size_t maxBufLen, bool showDot)
{

#if !defined(RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED) || (RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED == RB_CONFIG_NO)
	#error RB_CONFIG_TYPE_FLOATING_POINT_SUPPORTED must be set to RB_CONFIG_YES to use this function
#endif

	size_t strPos = 0;

	if (!wgtStr)
		return(0);

	if (pRndWgt && WP_WEIGHT_IsAvailable(pRndWgt->valStatus)) {
		int dp = pRndWgt->dp;
		int blnk = pRndWgt->rangeDecade;

		// limit decimal places to positive values
		if (dp < 0) {
			dp = 0;
		}
		// limit blanking to decimal places, e.g. "123.45" with 1000d only results in 2 blanks "120.  "
		//                                   or "11:02.33" with 1000d only results in 2 blanks "11:00.  "
		if (blnk > dp) {
			blnk = dp;
		}
		// limit blanking to positive values
		if (blnk < 0) {
			blnk = 0;
		}

		// format weight string depending on unit
		switch (pRndWgt->unit)
			{
			// lb:oz
			// Actual value in WP_WEIGHT_tRoundedWeight is converted and rounded as ounce (oz)
			case WP_UNIT_lboz: {
				int32_t lb = 0;
				size_t ozPos = 0;
				WP_tFloat oz = pRndWgt->value;

				// get and put sign
				if (oz < WP_FLT_ZERO) {
					wgtStr[strPos] = '-';
					oz = -oz;
				}
				// the first digit is reserved for the sign
				else
				{
					wgtStr[strPos] = ' ';
				}
				strPos++;

				// extract pounds from ounces and recalculate ounces (1 lb = 16 oz)
				lb = (int32_t)oz / 16;
				//lint -e790 // Info: Suspicious truncation, integral to float32
				oz = (WP_tFloat)100.0 + (oz - (WP_tFloat)(16 * lb)); // oz now in range 100.0 .. 115.x
				//lint +e790

				// put pounds and ":" after it
				strPos += RB_FORMAT_Long(wgtStr + strPos, lb, maxBufLen - strPos);
				ozPos = strPos;

				// check space for colon and ounces -> 4 characters ":XX" plus terminating '\0'
				if (strPos > (maxBufLen - ((size_t)(SPACE_COLON + SPACE_OUNCE + SPACE_TERM)))) {
					break;
				}
				// check if reduced decimal places must be used
				while ((dp > 0) && (((int)strPos + dp + SPACE_COLON + SPACE_OUNCE + SPACE_DOT + SPACE_TERM) > ((int)maxBufLen)))
				{
					if (blnk > 0) blnk--; // reduce blanking if any
					dp--; // reduce decimal places
				}

				// put ounces
				strPos += RB_FORMAT_Double(wgtStr+strPos, dp, (float64)oz, maxBufLen - strPos);
				wgtStr[ozPos] = ':'; // overwrite "1"
				break;
			}

			// other units than lb:oz
			default:
				// Do normal conversion. In case of overflow, try to reduce decimals. If this does
				// already not fit into the buffer, try to use exponential format.
				// Multiple iterations may be needed to find the correct formatting.
				//   Actual values: pRndWgt->value, maxBufLen, dp, blnk
				{
				float64 value = (float64)pRndWgt->value;
				strPos = RB_FORMAT_Double(wgtStr, dp, value, maxBufLen);
				while (strPos == 0)
					{
					if (dp > 0)
						{
						if (blnk > 0) blnk--; // reduce blanking if any
						dp--; // reduce decimal places
						strPos = RB_FORMAT_Double(wgtStr, dp, value, maxBufLen);
						}
					else
						{
						// Try to use exponential format
						blnk = 0;
						dp = ((int)maxBufLen - 5) + 1; // Space for "1.2E3" + 1 (for first dp--)
						while ((strPos == 0) && (dp > 0))
							{
							dp--; // reduce decimal places
							strPos = RB_FORMAT_DoubleToEFormat(wgtStr, dp, value, maxBufLen);
							}
						if (strPos == 0)
							{
							RB_STRING_strncpymax(wgtStr, "***", maxBufLen); // Occurs only if maxBufLen < 6
							strPos = strlen(wgtStr);
							}
						}
					}
				}
				break;
			} // switch (pRndWgt->unit)

		// blank reduced units depending on RB_Config
		#if !defined(RB_CONFIG_WP_SHOW_REDUCED_DIGITS) || (RB_CONFIG_WP_SHOW_REDUCED_DIGITS == RB_CONFIG_NO)
		if (strPos > (size_t)blnk)
			{
			strPos -= (size_t)blnk;
			if (!showDot && (blnk > 0) && (wgtStr[strPos-1] == '.'))
				{
				wgtStr[strPos-1] = ' ';
				strPos--; // Adjust length
				}
			}
		else
			{
			blnk = (int)strPos;
			strPos = 0;
			}


		while (blnk-- && (strPos < maxBufLen)) {
			wgtStr[strPos++] = ' ';
		}
		#else
		RB_UNUSED(showDot);
		#endif
	}
	wgtStr[strPos] = '\0';

	return((size_t)strPos);
}


//--------------------------------------------------------------------------------------------------
#endif // WP_FX_BASIC
