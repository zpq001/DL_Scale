//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_TouchCalibration.h
//! \ingroup	util
//! \brief		Convert touch input to usable coordinates
//!
//! This module uses a 3-point touch calibration matrix to convert the touch input values to
//! calibrated coordinates which can be used by the RB_PointingDevice module.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Silvan Sturzenegger
//
// $Date: 2016/11/10 08:58:41MEZ $
// $Revision: 1.6 $
//
//==================================================================================================

#ifndef _RB_TouchCalibration__h
#define _RB_TouchCalibration__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_TOUCHCALIBRATION) && (RB_CONFIG_USE_TOUCHCALIBRATION == RB_CONFIG_YES)


//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   T Y P E S
//==================================================================================================

//! One touch calibration point
typedef struct {
	int32_t x;		//!< X coordinate on the display in pixels
	int32_t y;		//!< Y coordinate on the display in pixels
} RB_DECL_TYPE RB_TOUCHCALIBRATION_tPoint;

//! A set of three touch calibration points
typedef struct {
	RB_TOUCHCALIBRATION_tPoint display1;	//!< 1st display position calibration point
	RB_TOUCHCALIBRATION_tPoint touch1;		//!< 1st raw touch input calibration point
	RB_TOUCHCALIBRATION_tPoint display2;	//!< 2nd display position calibration point
	RB_TOUCHCALIBRATION_tPoint touch2;		//!< 2nd raw touch input calibration point
	RB_TOUCHCALIBRATION_tPoint display3;	//!< 3rd display position calibration point
	RB_TOUCHCALIBRATION_tPoint touch3;		//!< 3rd raw touch input calibration point
} RB_DECL_TYPE RB_TOUCHCALIBRATION_tCalibrationPoints;

//! The conversion coefficients, calculated from the calibration matrix
typedef struct {
	float A;	//!< 1st coefficient
	float B;	//!< 2nd coefficient
	float C;	//!< 3rd coefficient
	float D;	//!< 4th coefficient
	float E;	//!< 5th coefficient
	float F;	//!< 6th coefficient
} RB_DECL_TYPE RB_TOUCHCALIBRATION_tConversionCoefficients;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// RB_TOUCHCALIBRATION_CalculateCoefficients
//--------------------------------------------------------------------------------------------------
//! \brief	Calculate the coefficients to convert the touch data from the given calibration matrix
//!
//! This function has to be called once during touch controller initialization to get the correct
//! coefficients from the 3 points in the touch calibration matrix.
//!
//! \param	pCoeffs			output	The calculated conversion coefficients
//! \param	pPoints			input	The three calibration points
//! \return	bool			true if successful
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC bool RB_TOUCHCALIBRATION_CalculateCoefficients(
		RB_TOUCHCALIBRATION_tConversionCoefficients* pCoeffs,
		const RB_TOUCHCALIBRATION_tCalibrationPoints* pPoints);


//--------------------------------------------------------------------------------------------------
// RB_TOUCHCALIBRATION_ConvertPoint
//--------------------------------------------------------------------------------------------------
//! \brief	Convert raw touch input values into display coordinates using the given coefficients
//!
//! \param	pDisplay		output	The calibrated display point in pixels
//! \param	pTouch			input	The raw input from the touch controller
//! \param	pCoeffs			input	The conversion coefficients
//! \return	none
//--------------------------------------------------------------------------------------------------
RB_DECL_FUNC void RB_TOUCHCALIBRATION_ConvertPoint(
		RB_TOUCHCALIBRATION_tPoint* pDisplay,
		const RB_TOUCHCALIBRATION_tPoint* pTouch,
		const RB_TOUCHCALIBRATION_tConversionCoefficients* pCoeffs);

#ifdef __cplusplus
}
#endif

#endif // RB_CONFIG_USE_TOUCHCALIBRATION
#endif // _RB_TouchCalibration__h
