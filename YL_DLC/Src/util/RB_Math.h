//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Math.h
//! \ingroup	util
//! \brief		Basic mathematical operations
//!
//! The implementation is optimized for low code size and high speed at the
//! expense of truncation to integers.
//! If code size and high speed is not important then use the functions in math.h.
//!
//! (c) Copyright Mettler-Toledo. All Rights Reserved.
//! \author		Peter Hertach
//
// $Date: 2016/11/10 08:58:39MEZ $
// $Revision: 1.25 $
//
//==================================================================================================

#ifndef _RB_MATH__h
#define _RB_MATH__h


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.

//#include "RB_Typedefs.h"

#include "comm.h"
//==================================================================================================
//  S U P P O R T   F O R   M I X E D   C / C + +
//==================================================================================================

#ifdef __cplusplus
extern "C" {
#endif


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Pi
extern const RB_DECL_CONST float64 RB_MATH_PI;


//==================================================================================================
//  G L O B A L   F U N C T I O N   D E C L A R A T I O N
//==================================================================================================

//----------------------------------------------------------------------------
// RB_MATH_isqrt
//----------------------------------------------------------------------------
//! \brief	Integer square root
//!
//! \param	radicand    Value whose square root is returned
//!
//! \return	Square root of radicand
//----------------------------------------------------------------------------
RB_DECL_FUNC uint32_t RB_MATH_isqrt(uint32_t radicand);

//----------------------------------------------------------------------------
// RB_MATH_ipow
//----------------------------------------------------------------------------
//! \brief	Power function for integer exponent
//!        Any base raised to the power 0 equals 1
//!
//! \param	x     Base
//! \param	n     Exponent
//!
//! \return	Value of x raised to the power n
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_ipow(float64 x, int32_t n);

//----------------------------------------------------------------------------
// RB_MATH_ipow10
//----------------------------------------------------------------------------
//! \brief	Base-10 power function for integer exponent
//!
//! \param	n     Exponent
//!
//! \return	Value of 10 raised to the power n
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_ipow10(int32_t n);

//----------------------------------------------------------------------------
// RB_MATH_ilog2
//----------------------------------------------------------------------------
//! \brief	Base-2 logarithm of magnitude of x rounded towards -infinity
//!
//! \param	x     Value whose base-2 logarithm is returned
//!
//! \return	Base-2 logarithm of magnitude of x rounded towards -infinity
//----------------------------------------------------------------------------
RB_DECL_FUNC int32_t RB_MATH_ilog2(float64 x);

//----------------------------------------------------------------------------
// RB_MATH_ilog10
//----------------------------------------------------------------------------
//! \brief	Base-10 logarithm of magnitude of x rounded towards -infinity
//!
//! \param	x     Value whose base-10 logarithm is returned
//!
//! \return	Base-10 logarithm of magnitude of x rounded towards -infinity
//----------------------------------------------------------------------------
RB_DECL_FUNC int32_t RB_MATH_ilog10(float64 x);

//----------------------------------------------------------------------------
// RB_MATH_frexp10
//----------------------------------------------------------------------------
//! \brief	Split floating point number into sign bit, mantissa and exponent
//!
//! Return sign bit s, mantissa m (1 <= m < 10) and integer exponent e such that
//! x = (-1)^s * m * 10^e.
//! If the return value is false then s, m, e are undefined.
//!
//! Remark: x=0 is handled by s=m=n=0
//!
//! \param	x     Value to be split into mantissa, exponent and sign bit
//! \param	s     Returned sign bit
//! \param	m     Returned mantissa
//! \param	e     Returned exponent
//
//! \return	True if split was successful.
//!         False otherwise.
//----------------------------------------------------------------------------
RB_DECL_FUNC bool RB_MATH_frexp10(float64 x, int *s, float64 *m, int32_t *e);

//----------------------------------------------------------------------------
// RB_MATH_Round
//----------------------------------------------------------------------------
//! \brief	Round to given resolution
//!
//! The rounded value equals round(value/resolution)*resolution,
//! where round(x) rounds to the nearest integer. The rounding point is adjusted
//! by 2^12 epsilon to prevent wrong rounding of inexact floats, e.g. 0.49999999999987.
//! Errors are propagated on each float operation. In case this function is used to
//! round a weight to its according resolution, there are about twelve operations taken
//! on the value and the resolution. So we correct the rounding point by about 2^12
//! epsilon, for better readability the factor of 5000.0 is used.
//! If the negative magnitude of x is smaller than the smallest value of int64_t
//! then no rounding is performed. RB_MATH_Round must not be used for mathematical
//! correct rounding, use round(value/resolution)*resolution in this case.
//!
//! \par Useful links
//!
//! - The floating point guide:
//!   http://floating-point-gui.de
//!
//! - What Every Computer Scientist Should Know About Floating-Point Arithmetic:
//!   http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
//!
//! \param	value        Value to be rounded
//! \param	resolution   Rounding resolution, must not be zero
//!
//! \return	Rounded value
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_Round(float64 value, float64 resolution);

//----------------------------------------------------------------------------
// RB_MATH_RoundPow2
//----------------------------------------------------------------------------
//! \brief	Round to nearest integer which is a power of 2 for a positive exponent
//!
//! \param	value    Value to be rounded
//!
//! \return	Rounded value
//----------------------------------------------------------------------------
RB_DECL_FUNC uint32_t RB_MATH_RoundPow2(uint32_t value);

//----------------------------------------------------------------------------
// RB_MATH_RoundStep
//----------------------------------------------------------------------------
//! \brief	Round to nearest number of magnitude k*10^n
//!        where k is 0, 1, 2 or 5 and n is an integer
//!
//! \param	value    Value to be rounded
//!
//! \return	Rounded value
//!
//! Examples:
//!     value  -> Rounded value
//!     0.0    ->    0.0   (k=0, n=any)
//!     0.0014 ->    0.001 (k=1, n=-3)
//!     0.015  ->    0.02  (k=2, n=-2)
//!     0.35   ->    0.5   (k=5, n=-1)
//!     7.49   ->    5.0   (k=5, n=0)
//!   -74.9    ->  -50.0   (k=5, n=1)
//!    75.0    ->  100.0   (k=1, n=2)
//!
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_RoundStep(float64 value);

//----------------------------------------------------------------------------
// RB_MATH_RoundUpStep
//----------------------------------------------------------------------------
//! \brief	Round to nearest number of magnitude k*10^n greater or equal to
//!        magnitude of value, where k is 0, 1, 2 or 5 and n is an integer
//!
//! \param	value    Value to be rounded
//!
//! \return	Rounded value
//!
//! Examples:
//!     value  -> Rounded value
//!     0.0    ->    0.0   (k=0, n=any)
//!     0.001  ->    0.001 (k=1, n=-3)
//!     0.011  ->    0.02  (k=2, n=-2)
//!     0.21   ->    0.5   (k=5, n=-1)
//!     0.51   ->    1.0   (k=1, n=0)
//!   -10.1    ->  -20.0   (k=2, n=1)
//!
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_RoundUpStep(float64 value);

//----------------------------------------------------------------------------
// RB_MATH_RoundDownStep
//----------------------------------------------------------------------------
//! \brief	Round to nearest number of magnitude k*10^n smaller or equal to
//!        magnitude of value, where k is 0, 1, 2 or 5 and n is an integer
//!
//! \param	value    Value to be rounded
//!
//! \return	Rounded value
//!
//! Examples:
//!     value  -> Rounded value
//!     0.0    ->    0.0   (k=0, n=any)
//!     0.001  ->    0.001 (k=1, n=-3)
//!     0.019  ->    0.01  (k=1, n=-2)
//!     0.49   ->    0.2   (k=2, n=-1)
//!     9.9    ->    5.0   (k=5, n=0)
//!   -19.0    ->  -10.0   (k=1, n=1)
//!
//----------------------------------------------------------------------------
RB_DECL_FUNC float64 RB_MATH_RoundDownStep(float64 value);

//----------------------------------------------------------------------------
// RB_MATH_Vander
//----------------------------------------------------------------------------
//! \brief	Solve vandermonde system
//!
//! Solve the linear equation system V * p = f for the unknown vector p
//! where
//! V is a matrix with elements V_kj = x_k ^ j for k,j = 0,..,n,
//! f is a vector with elements f_k            for k   = 0,..,n,
//! p is a vector with elements p_k            for k   = 0,..,n.
//!
//! Polynomial interpolation leads to a vandermonde system:
//! Find coefficients of the n-th order polynomial
//! p(x) = p_0 + p_1*x + p_2*x^2 + .. + p_n*x^n
//! such that, given x_k and f_k, p(x_k) = f_k for k = 0,..,n.
//!
//! \param	x       Input:  Array of length n+1 where x[k]  = x_k for k = 0,..,n
//!                         All array elements must be distinct
//! \param	fp      Input:  Array of length n+1 where fp[k] = f_k for k = 0,..,n
//!                 Output: Array of length n+1 where fp[k] = p_k for k = 0,..,n
//! \param	n       Input:  Order of polynomial
//!
//! \return	True if solving the vandermonde system was successful.
//!         False otherwise.
//!
//! Reference: Matrix Computations, second Edition, G. H. Golub
//----------------------------------------------------------------------------
RB_DECL_FUNC bool RB_MATH_Vander(const float64 *x, float64 *fp, int32_t n);

//----------------------------------------------------------------------------
// RB_MATH_LeastSquaresLine
//----------------------------------------------------------------------------
//! \brief	Fit data points to a straight line in the least square sense
//!
//! Given the data points (x_k, y_k) find coefficient a and b which minimize
//! r^2 = sum_{k=0,..,n-1}{ (y_k - a - b * x_k)^2 }
//!
//! \param	x   Input: Array of length n where x[k] = x_k for k = 0,..,n-1
//! \param	y   Input: Array of length n where y[k] = y_k for k = 0,..,n-1
//! \param	n   Input: Number of data points
//! \param	a   Output: Fitted parameter a
//! \param	b   Output: Fitted parameter b
//! \param	r2  Output: Squared residue r^2
//!
//! \return	None
//!
//! Reference: Numerical Recipes in C, second Edition, W. H. Press, page 665
//----------------------------------------------------------------------------
RB_DECL_FUNC void RB_MATH_LeastSquaresLine(const float64 *x, const float64 *y, int32_t n, float64 *a, float64 *b, float64 *r2);

//----------------------------------------------------------------------------
// RB_MATH_LeastSquaresParabola
//----------------------------------------------------------------------------
//! \brief	Fit data points to a parabola in the least square sense
//!
//! Given the data points (x_k, y_k) find coefficient a, b and c which minimize
//! r^2 = sum_{k=0,..,n-1}{ (y_k - a - b * x_k - c * x_k^2)^2 }
//!
//! \param	x   Input: Array of length n where x[k] = x_k for k = 0,..,n-1
//! \param	y   Input: Array of length n where y[k] = y_k for k = 0,..,n-1
//! \param	n   Input: Number of data points
//! \param	a   Output: Fitted parameter a
//! \param	b   Output: Fitted parameter b
//! \param	c   Output: Fitted parameter c
//! \param	r2  Output: Squared residue r^2
//!
//! \return	None
//!
//! Attention: Algorithm is susceptible to roundoff error.
//----------------------------------------------------------------------------
RB_DECL_FUNC void RB_MATH_LeastSquaresParabola(const float64 *x, const float64 *y, int32_t n, float64 *a, float64 *b, float64 *c, float64 *r2);

//----------------------------------------------------------------------------
// RB_MATH_AlmostEqualAbs
//----------------------------------------------------------------------------
//! \brief	Check if two values are approximately equal using absolute error
//!
//! \param	x        Value to be compared
//! \param	y        Value to be compared
//! \param	e        Admissible absolute error between x and y
//!
//! \return	True if x approximately equals y.
//!         False otherwise.
//----------------------------------------------------------------------------
RB_DECL_FUNC bool RB_MATH_AlmostEqualAbs(float64 x, float64 y, float64 e);

//----------------------------------------------------------------------------
// RB_MATH_AlmostEqualRel
//----------------------------------------------------------------------------
//! \brief	Check if two values are approximately equal using relative error
//!
//! \param	x        Value to be compared
//! \param	y        Value to be compared
//! \param	e        Admissible relative error between x and y
//!
//! \return	True if x approximately equals y.
//!         False otherwise.
//----------------------------------------------------------------------------
RB_DECL_FUNC bool RB_MATH_AlmostEqualRel(float64 x, float64 y, float64 e);

#ifdef __cplusplus
}
#endif

#endif // _RB_MATH__h
