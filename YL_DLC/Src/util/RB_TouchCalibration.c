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
//! For more information about touch calibration refer to:
//! http://www.maximintegrated.com/app-notes/index.mvp/id/5296
//!
//! <tt>
//! To transform a touch controller point (x, y) to a display point (xd, yd), the equations are
//!
//! xd = x*A + y*B + C
//! yd = x*D + y*E + F
//!
//! To get the six unknowns A,B,C,D,E and F, three pairs of calibration points are needed:
//!
//! (x1, y1), (x1d, y1d)
//! (x2, y2), (x2d, y2d)
//! (x3, y3), (x3d, y3d)
//!
//! Then the equations are:
//!
//! x1d = x1*A + y1*B + C
//! x2d = x2*A + y2*B + C
//! x3d = x3*A + y3*B + C
//!
//! y1d = x1*D + y1*E + F
//! y2d = x2*D + y2*E + F
//! y3d = x3*D + y3*E + F
//!
//! Or written in matrix form:
//!
//! | x1d |       | A |
//! | x2d | = Z * | B |
//! | x3d |       | C |
//!
//! | y1d |       | D |
//! | y2d | = Z * | E |
//! | y3d |       | F |
//!
//! where
//!
//!     | x1 y1 1 |
//! Z = | x2 y2 1 |
//!     | x3 y3 1 |
//!
//! Then the unknown coefficients A,B,C,D,E and F are:
//!
//! | A |          | x1d |
//! | B | = Z^-1 * | x2d |
//! | C |          | x3d |
//!
//! | D |          | y1d |
//! | E | = Z^-1 * | y2d |
//! | F |          | y3d |
//!
//! where
//!
//!                |   y2-y3     y3-y1     y1-y2   |
//! Z^-1 = 1/Div * |   x3-x2     x1-x3     x2-x1   |
//!                | x2y3-x3y2 x3y1-x1y3 x1y2-x2y1 |
//!
//! and
//!
//! Div = x1y2-x1y3-x2y1+x2y3+x3y1-x3y2
//!
//! Which results in the calibration coefficients
//!
//! A = ((y2-y3)x1d     + (y3-y1)x2d     + (y1-y2)x3d     ) / Div
//! B = ((x3-x2)x1d     + (x1-x3)x2d     + (x2-x1)x3d     ) / Div
//! C = ((x2y3-x3y2)x1d + (x3y1-x1y3)x2d + (x1y2-x2y1)x3d ) / Div
//! D = ((y2-y3)y1d     + (y3-y1)y2d     + (y1-y2)y3d     ) / Div
//! E = ((x3-x2)y1d     + (x1-x3)y2d     + (x2-x1)y3d     ) / Div
//! F = ((x2y3-x3y2)y1d + (x3y1-x1y3)y2d + (x1y2-x2y1)y3d ) / Div
//! Div = (y2-y3)x1 + (y3-y1)x2 +(y1-y2)x3
//! </tt>
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Silvan Sturzenegger
//
// $Date: 2016/11/10 08:58:41MEZ $
// $Revision: 1.7 $
//
//==================================================================================================

//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_TouchCalibration"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Config.h"

#if defined(RB_CONFIG_USE_TOUCHCALIBRATION) && (RB_CONFIG_USE_TOUCHCALIBRATION == RB_CONFIG_YES)

#include "RB_TouchCalibration.h"


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
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
bool RB_TOUCHCALIBRATION_CalculateCoefficients(
		RB_TOUCHCALIBRATION_tConversionCoefficients* pCoeffs,
		const RB_TOUCHCALIBRATION_tCalibrationPoints* pPoints)
{
	float Div; // The common divider for all coefficients
	float x1t,x2t,x3t,y1t,y2t,y3t;
	float x1d,x2d,x3d,y1d,y2d,y3d;

	// Intermediary values, to make the calculations more readable
	// This also forces floating point calculations
	x1t = (float)pPoints->touch1.x;
	x2t = (float)pPoints->touch2.x;
	x3t = (float)pPoints->touch3.x;
	y1t = (float)pPoints->touch1.y;
	y2t = (float)pPoints->touch2.y;
	y3t = (float)pPoints->touch3.y;
	x1d = (float)pPoints->display1.x;
	x2d = (float)pPoints->display2.x;
	x3d = (float)pPoints->display3.x;
	y1d = (float)pPoints->display1.y;
	y2d = (float)pPoints->display2.y;
	y3d = (float)pPoints->display3.y;

	// Calculate the common divider first
	Div =	(y2t - y3t) * x1t +
			(y3t - y1t) * x2t +
			(y1t - y2t) * x3t;

	if (Div == 0) {
		return false;
	}

	pCoeffs->A =	(y2t - y3t) * x1d +
					(y3t - y1t) * x2d +
					(y1t - y2t) * x3d;
	pCoeffs->A = pCoeffs->A / Div;

	pCoeffs->B =	(x3t - x2t) * x1d +
					(x1t - x3t) * x2d +
					(x2t - x1t) * x3d;
	pCoeffs->B = pCoeffs->B / Div;

	pCoeffs->C =	((x2t * y3t) - (x3t * y2t)) * x1d +
					((x3t * y1t) - (x1t * y3t)) * x2d +
					((x1t * y2t) - (x2t * y1t)) * x3d;
	pCoeffs->C = pCoeffs->C / Div;

	pCoeffs->D =	(y2t - y3t) * y1d +
					(y3t - y1t) * y2d +
					(y1t - y2t) * y3d;
	pCoeffs->D = pCoeffs->D / Div;

	pCoeffs->E =	(x3t - x2t) * y1d +
					(x1t - x3t) * y2d +
					(x2t - x1t) * y3d;
	pCoeffs->E = pCoeffs->E / Div;

	pCoeffs->F =	((x2t * y3t) - (x3t * y2t)) * y1d +
					((x3t * y1t) - (x1t * y3t)) * y2d +
					((x1t * y2t) - (x2t * y1t)) * y3d;
	pCoeffs->F = pCoeffs->F / Div;
	return true;
}


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
void RB_TOUCHCALIBRATION_ConvertPoint(
		RB_TOUCHCALIBRATION_tPoint* pDisplay,
		const RB_TOUCHCALIBRATION_tPoint* pTouch,
		const RB_TOUCHCALIBRATION_tConversionCoefficients* pCoeffs)
{
	pDisplay->x = (int32_t)(pTouch->x * pCoeffs->A + pTouch->y * pCoeffs->B + pCoeffs->C);
	pDisplay->y = (int32_t)(pTouch->x * pCoeffs->D + pTouch->y * pCoeffs->E + pCoeffs->F);
}


#endif // RB_CONFIG_USE_TOUCHCALIBRATION
