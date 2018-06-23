//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/WP_Weight.h
//! \ingroup	util
//! \brief		Functions for weight conversions.
//!
//! This module contains the type Weight and the functions for weight conversions.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Pius Derungs, Martin Heusser
//
// $Date: 2016/11/10 08:58:42MEZ $
// $Revision: 1.48 $
//
//==================================================================================================

#ifndef _WP_Weight__h
#define _WP_Weight__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_WP) && (RB_CONFIG_USE_WP == RB_CONFIG_YES)

#include <stddef.h>
#include "WP_Typedefs.h"
#include "WP_Unit.h"
#include "WP_State.h"


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! Weightvalue status
typedef enum {
	WP_WEIGHT_STS_UNDEFINED = 0,		//!< Weight is undefined
	WP_WEIGHT_STS_UNDERLOAD,			//!< Weight is below absolute range
	WP_WEIGHT_STS_TYPEUNDER,			//!< Weight is below type range
	WP_WEIGHT_STS_OVERLOAD,				//!< Weight is above absolute range
	WP_WEIGHT_STS_TYPEOVER,				//!< Weight is above type range
	WP_WEIGHT_STS_AVAILABLE,			//!< Weight is available
	WP_WEIGHT_STS_INEXACT,				//!< Weight is available but inexact (Minweighing)
	WP_WEIGHT_STS_STARTUP				//!< Weight is available but init zero not already done
} RB_DECL_TYPE WP_WEIGHT_tWgtStatus;

//! Weightvalue status
typedef enum {
	WP_WEIGHT_TARE_NO = 0,				//!< Not tared
	WP_WEIGHT_TARE_DONE,				//!< Tare executed
	WP_WEIGHT_TARE_PRETARED				//!< Pretared
} RB_DECL_TYPE WP_WEIGHT_tTareStatus;

//! Output type of rounded weight value
typedef enum {
	WP_WEIGHT_TYPE_NONE	= 0,			//!< Value is not a weight value
	WP_WEIGHT_TYPE_NET,					//!< Net value by tare == 0
	WP_WEIGHT_TYPE_NET_T,				//!< Net value by tare != 0
	WP_WEIGHT_TYPE_NET_PT,				//!< Net value by pretare != 0
	WP_WEIGHT_TYPE_GROSS,				//!< Gross value by tare == 0
	WP_WEIGHT_TYPE_GROSS_T,				//!< Gross value by tare != 0
	WP_WEIGHT_TYPE_GROSS_PT,			//!< Gross value by pretare != 0
	WP_WEIGHT_TYPE_T,					//!< Tare value
	WP_WEIGHT_TYPE_PT					//!< Pretare value
} RB_DECL_TYPE WP_WEIGHT_tWgtType;

//! Item selection of rounded weight information
typedef enum {
	WP_WEIGHT_SEL_TARE = 0,				//!< Item selection is tare value
	WP_WEIGHT_SEL_NET,					//!< Item selection is net value
	WP_WEIGHT_SEL_GROSS					//!< Item selection is gross value
} RB_DECL_TYPE WP_WEIGHT_tValSelection;

//! Method to build gross, net and tare value
typedef enum {
	WP_WEIGHT_METH_INDEPENDEND = 0,		//!< Each value taken from weight-struct
	WP_WEIGHT_METH_RECALC_NET,			//!< Recalculate net weight from rounded gross and tare
	WP_WEIGHT_METH_RECALC_GROSS			//!< Recalculate gross weight from rounded net and tare
} RB_DECL_TYPE WP_WEIGHT_tMethSelection;

//! Stability specification
typedef struct {
	WP_tFloat				width;			//!< Stability width in gram
	unsigned int			count;			//!< Stability count
} RB_DECL_TYPE WP_WEIGHT_tStability;

//! Weight value item
typedef struct {
	WP_tFloat				value;			//!< Weight value in gram
	WP_tRangeIndex			range;			//!< Range of weight value
	#if defined(WP_FX_2ND_RANGE_DETECTOR)
	WP_tRangeIndex			range2;			//!< Range of weight value (2nd range detector)
	#endif
} RB_DECL_TYPE WP_WEIGHT_tWgtItem;

//! Tare value item
typedef struct {
	WP_tFloat				value;			//!< Tare value in gram
	WP_tRangeIndex			range;			//!< Range of tare value
	#if defined(WP_FX_2ND_RANGE_DETECTOR)
	WP_tRangeIndex			range2;			//!< Range of weight value (2nd range detector)
	#endif
	WP_WEIGHT_tTareStatus	status;			//!< Status of tare value  (no, done, pretared)
} RB_DECL_TYPE WP_WEIGHT_tTareItem;

//! Qualified weight type, after PostProcessing (unit always grams)
typedef struct {
	WP_tFloat				signalWgt;		//!< Weight from signal processing in gram
	WP_tFloat				signalDrift;	//!< Weight drift from signal processing in gram per sec
	WP_tFloat				temperature;	//!< Cell temperature
	WP_WEIGHT_tWgtItem		gross;			//!< Gross in gram = signalWgt - zero
	WP_WEIGHT_tWgtItem		net;			//!< Net in gram = Gross - Tare
	WP_WEIGHT_tTareItem		tare;			//!< Tare in gram
	WP_WEIGHT_tStability	stability;		//!< Stability of weight
	WP_WEIGHT_tWgtStatus	status;			//!< Status of weight (over, under, avail, ...)
	#if defined(WP_FX_2ND_RANGE_DETECTOR)
	WP_WEIGHT_tWgtStatus	status2;		//!< Status of weight value (2nd range detector)
	#endif
	uint16_t				valUpdateCnt;	//!< Value update counter
	bool					centerOfZero;	//!< Center of zero indicator (within 0.25 e around gross zero)
	#if defined(RB_CONFIG_WP_SENSOR_TIMESTAMP) && (RB_CONFIG_WP_SENSOR_TIMESTAMP == RB_CONFIG_YES)
	uint32_t				timestamp;		//!< Time stamp
	#endif
} RB_DECL_TYPE WP_WEIGHT_tWeight;

//! Converted and rounded weight, ready for output composer (formatter)
typedef struct {
	WP_tFloat				value;			//!< Weight value rounded in unit
	WP_tFloat				resolution;		//!< Resolution of rounded value in unit
	WP_WEIGHT_tWgtStatus	valStatus;		//!< Status of rounded value (avail, over, under, ...)
	WP_WEIGHT_tWgtType		valType;		//!< Type of rounded weight (net, gross, tare, ...)
	WP_UNIT_tUnit			unit;			//!< Unit of rounded weight
	WP_STATE_tLFTMode		lftMode;		//!< LFT-mode of rounded weight (e = xd)
	WP_tRangeIndex			range;			//!< Range of value
	bool					stbFlag;		//!< Stabilityflag of rounded weight
	bool					centerOfZero;	//!< Center of zero indicator (within 0.25 e around gross zero)
	int						dp;				//!< Decimal places, e.g. ...-2, -1, 0, 1, 2, etc.
	uint16_t				rangeDecade;	//!< Number of decades of rounding step
} RB_DECL_TYPE WP_WEIGHT_tRoundedWeight;

//! Definition of TD-parameters for the module WEIGHT
typedef struct {
	WP_tFloat			outStabilityWidth;	//!< Width for the output-stability detection in gram
	bool				recalculateGross;	//!< TRUE if gross is recalculated from rounded net and tare
} RB_DECL_TYPE WP_WEIGHT_tTDParams;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_Initialize
//--------------------------------------------------------------------------------------------------
//! \brief	Initialization of the module WEIGHT
//!
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_WEIGHT_Initialize(void);


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_SetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Set typedefinition parameters for the module WEIGHT
//!
//! \param	pWeightParams	Pointer to a set of type definition parameters for the module WEIGHT
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_WEIGHT_SetTDParams(const WP_WEIGHT_tTDParams* pWeightParams);


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_GetTDParams
//--------------------------------------------------------------------------------------------------
//! \brief	Get the type definition parameter set of the module WEIGHT
//!
//! \return	Pointer to the type definition parameter set
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC const WP_WEIGHT_tTDParams* WP_WEIGHT_GetTDParams(void);


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsWgtSigStable
//--------------------------------------------------------------------------------------------------
//! \brief	Check if weight is stable
//!
//! \param	pWgt	Pointer to weight to be checked
//! \return	<c>true</c> if weight is available and stable, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_WEIGHT_IsWgtSigStable(const WP_WEIGHT_tWeight* pWgt) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsRecalculateGrossWeightSet
//--------------------------------------------------------------------------------------------------
//! \brief	Check if gross weight should be recalculated from net and tare
//!
//! \return	<c>true</c> if gross weight is recalculated from net and tare, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_WEIGHT_IsRecalculateGrossWeightSet(void);


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_IsAvailable
//--------------------------------------------------------------------------------------------------
//! \brief	Check if a specific weight status represents an available weight
//!
//! \param	wgtSts  Weight status to be checked
//! \return	<c>true</c> if specific weight status represents an available weight, otherwise <c>false</c>
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool WP_WEIGHT_IsAvailable(WP_WEIGHT_tWgtStatus wgtSts) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC bool WP_WEIGHT_IsValidAndWeighing(WP_WEIGHT_tWgtStatus wgtSts) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_ConvertToActualUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a weight item to a rounded weight item fitting the actual unit settings
//!
//! \attention RB_ATTR_THREAD_SAFE
//! only applies when settings related to ranges and units are NOT changed by application, commands or WP restart during call.
//!
//! \param	pWeight		Pointer to weight to be converted (input)
//! \param	pRndWgt		Pointer to converted and rounded weight (output)
//! \param	unitSel		Selection of actual output unit (display,unit1/2/3)
//! \param	valSel		Selection of value item to be converted (tare/net/gross)
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_WEIGHT_ConvertToActualUnit(const WP_WEIGHT_tWeight* pWeight,
								   WP_WEIGHT_tRoundedWeight* pRndWgt,
								   WP_UNIT_tUnitSelection unitSel,
								   WP_WEIGHT_tValSelection valSel) RB_ATTR_THREAD_SAFE;


//--------------------------------------------------------------------------------------------------
// WP_WEIGHT_ConvertToUnit
//--------------------------------------------------------------------------------------------------
//! \brief	Convert a weight item to a rounded weight item fitting a specific conversion unit
//!
//! \attention RB_ATTR_THREAD_SAFE
//! only applies when settings related to ranges and units are NOT changed by application, commands or WP restart during call.
//!
//! \param	pWeight		Pointer to weight to be converted (input)
//! \param	pRndWgt		Pointer to converted and rounded weight (output)
//! \param	unit		Conversion unit
//! \param	valSel		Selection of value item to be converted (tare/net/gross)
//! \return	None
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void WP_WEIGHT_ConvertToUnit(const WP_WEIGHT_tWeight* pWeight,
							 WP_WEIGHT_tRoundedWeight* pRndWgt,
							 WP_UNIT_tUnit unit,
							 WP_WEIGHT_tValSelection valSel) RB_ATTR_THREAD_SAFE;


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
RB_DECL_FUNC void WP_WEIGHT_ConvertGrossNetTare(const WP_WEIGHT_tWeight* pWeight,
							 WP_WEIGHT_tRoundedWeight* pRndGross,
							 WP_WEIGHT_tRoundedWeight* pRndNet,
							 WP_WEIGHT_tRoundedWeight* pRndTare,
							 WP_UNIT_tUnit unit);


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
RB_DECL_FUNC size_t WP_WEIGHT_GetWeightStr(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* wgtStr, size_t maxBufLen);

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
RB_DECL_FUNC size_t WP_WEIGHT_GetSicsStr(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* sicsStr, size_t maxBufLen);


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
//! \param	maxBufLen	Max length of string (incl.'\\0', i.e. buffersize)
//! \param	showDot		Show dot as last character
//! \return	Number of chars in string
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC size_t WP_WEIGHT_WeightToString(const WP_WEIGHT_tRoundedWeight* pRndWgt, char* wgtStr, size_t maxBufLen, bool showDot);


#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_WP
#endif // _WP_Weight__h
