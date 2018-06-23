//==================================================================================================
//                                            Rainbow
//==================================================================================================
//
//! \file		util/RB_Math.c
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
// $Revision: 1.30 $
//
//==================================================================================================


//==================================================================================================
//  M O D U L E   N A M E
//==================================================================================================

#define RB_MODULE_NAME "RB_Math"


//==================================================================================================
//  I N C L U D E D   F I L E S
//==================================================================================================

#include "RB_Math.h"
// This module is automatically enabled/disabled and has no RB_CONFIG_USE, no check is needed here.


//==================================================================================================
//  G L O B A L   D E F I N I T I O N S
//==================================================================================================

//! Pi
const float64 RB_MATH_PI = 3.141592653589793238;


//==================================================================================================
//  F O R W A R D   D E C L A R A T I O N S
//==================================================================================================

static float64 upow(float64 x, uint32_t n);
static float64 compose(int s, float64 m, int32_t e);


//==================================================================================================
//  G L O B A L   F U N C T I O N   I M P L E M E N T A T I O N
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
uint32_t RB_MATH_isqrt(uint32_t radicand)
{
    #define NUM_BITS_UINT32 32 // number of bits of uint32_t

    uint32_t lower = 0u; // lower limit of square root
    uint32_t upper = (((uint32_t)1u) << (NUM_BITS_UINT32/2u));  // upper limit of square root of a uint32_t radicand
    uint32_t middle;
    uint32_t i = 0u;

    // bisection method
    do
    {
        middle = (lower + upper) >> 1u; // mean value
        if (middle * middle <= radicand)
        {
            lower = middle;
        }
        else
        {
            upper = middle;
        }
    } while (i++ < (NUM_BITS_UINT32/2u));

    return middle;
}

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
float64 RB_MATH_ipow(float64 x, int32_t n)
{
    if (n >= 0)
    {
        return upow(x,(uint32_t)n);
    }
    else if (x != 0.0)
    {
        return upow(1.0/x,(uint32_t)(-n));
    }
    else
    {
        // power of zero for a negative exponent is undefined because division by zero is implied
        return((float64)(FLT64_INFINITY));
    }
}

//----------------------------------------------------------------------------
// RB_MATH_ipow10
//----------------------------------------------------------------------------
//! \brief	Base-10 power function for integer exponent
//!
//! \param	n     Exponent
//!
//! \return	Value of 10 raised to the power n
//----------------------------------------------------------------------------
float64 RB_MATH_ipow10(int32_t n)
{
    float64 base;

    if  (n >= 0)
    {
        base = 10.0;
    }
    else
    {
        n = -n;
        base = 0.1;
    }

    return upow(base,(uint32_t)n);
}

//----------------------------------------------------------------------------
// RB_MATH_ilog2
//----------------------------------------------------------------------------
//! \brief	Base-2 logarithm of magnitude of x rounded towards -infinity
//!
//! \param	x     Value whose base-2 logarithm is returned
//!
//! \return	Base-2 logarithm of magnitude of x rounded towards -infinity
//----------------------------------------------------------------------------
int32_t RB_MATH_ilog2(float64 x)
{
    if (!FLT64_isfinite(x)) // handle NaN, +inf, -inf
    {
        return INT32_MAX;
    }
    else if (x == 0.0) // handle 0.0
    {
        return INT32_MIN;
    }
    else
    {
        // m = frexpf64(x,&e) returns m and e such that x=m*2^e, where 0.5 <= fabs(m) < 1.0
        int exponent = 0;
        frexpf64(x, &exponent);
        return exponent-1;
    }
}

//----------------------------------------------------------------------------
// RB_MATH_ilog10
//----------------------------------------------------------------------------
//! \brief	Base-10 logarithm of magnitude of x rounded towards -infinity
//!
//! \param	x     Value whose base-10 logarithm is returned
//!
//! \return	Base-10 logarithm of magnitude of x rounded towards -infinity
//----------------------------------------------------------------------------
int32_t RB_MATH_ilog10(float64 x)
{
    if (!FLT64_isfinite(x)) // handle NaN, +inf, -inf
    {
        return INT32_MAX;
    }
    else if (x == 0.0) // handle 0.0
    {
        return INT32_MIN;
    }
    else
    {
        int sign = 0;
        float64 mantissa = 0.0;
        int32_t exponent = 0;

        RB_MATH_frexp10(x, &sign, &mantissa, &exponent);
        return exponent;
    }
}

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
bool RB_MATH_frexp10(float64 x, int *s, float64 *m, int32_t *e)
{
    int sign;
    float64 mantissa;
    int32_t exponent;

    // handle NaN, +inf, -inf
    if (!FLT64_isfinite(x))
    {
        return false;
    }

    // handle x=0.0
    if (x == 0.0)
    {
        sign = 0;
        mantissa = 0.0;
        exponent = 0;
    }
    else
    {
        if (x < 0.0) // handle sign bit
        {
            x = -x;
            sign = 1;
        }
        else
        {
            sign = 0;
        }

        // Estimate exponent and calculate resulting mantissa.
        //
        // Let x = b*2^n, where 1 <= b < 2 and n is an integer.
        // Then: log10(x) = log10(b) + n*log10(2)
        //
        // Use approximation log10(2) ~= 1233/(2^12) and ignore log10(b).
        // Then: log10(x) ~= n * 1233/(2^12)
        exponent = (RB_MATH_ilog2(x)*1233)/4096; // estimated exponent
        {
            // split exponent into a sum e = e1 + e2 (necessary to handle subnormal numbers)
            const int32_t e1 = exponent/2;
            const int32_t e2 = exponent - e1;
            mantissa = x * RB_MATH_ipow10(-e1);
			mantissa *= RB_MATH_ipow10(-e2); // resulting mantissa
        }

        // adjust exponent until mantissa is in valid range
        while (mantissa >= 10.0)
        {
            mantissa *= 0.1;
            exponent++;
        }

        while (mantissa < 1.0)
        {
            mantissa *= 10.0;
            exponent--;
        }
    }

    if (s)
    {
        *s = sign;
    }

    if (m)
    {
        *m = mantissa;
    }

    if (e)
    {
        *e = exponent;
    }

    return true;
}

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
float64 RB_MATH_Round(float64 value, float64 resolution)
{
    float64 rounded;

    // account for resolution
    rounded = value/resolution;

    // handle NaN, +inf, -inf
    if (!FLT64_isfinite(rounded))
    {
        return value; // no rounding
    }

    // consider positive values
    if (rounded > 0.0)
    {
        rounded = -rounded;         // change sign
        resolution = -resolution;   // account for sign change
    }

    // handle too small values
    if (rounded < INT64_MIN)
    {
        return value; // no rounding
    }

    // shift by half of rounding grid distance
	// ---------------------------------------
	// additional shift by 5000 times epsilon to prevent wrong rounding of inexact floats near
	// the rounding point, e.g. 0.49999999999987. See function header for details why 5000.
	// The correction will shift the rounding point by about 10E-13 digits.
	// A lot of Internet forums propose solutions like value = ceil(value - 0.4999999999999).
	// Example using TA command on a 3 place balance:
	// Input value					Output value
	// 1.33349999999999000 g		   1.333 g
	// 1.33349999999999900 g		   1.334 g
	// 6000.33349999999990 g		6000.333 g
	// 6000.33349999999999 g		6000.334 g
    rounded -= 0.5 + (5000.0 * FLT64_EPSILON);

    // round towards zero
    rounded = (float64)((int64_t)rounded);

    // account for resolution and sign
    return rounded * resolution;
}

//----------------------------------------------------------------------------
// RB_MATH_RoundPow2
//----------------------------------------------------------------------------
//! \brief	Round to nearest integer which is a power of 2 for a positive exponent
//!
//! \param	value    Value to be rounded
//!
//! \return	Rounded value
//----------------------------------------------------------------------------
uint32_t RB_MATH_RoundPow2(uint32_t value)
{
    uint32_t roundValue; // value rounded to 2^n for n >= 1
    uint32_t n = 1u;     // base-2 exponent of rounded value

    // Basic idea:
    // If the 2 most significant bits both equal 1 then round up, otherwise round down.

    // find the 2 most significant bits
    while (value > 3u)
    {
        value >>= 1u; // divide by 2, round towards zero
        n++;
    }

    // Round down.
    // Round value to largest power of 2 which is not larger then value.
    // Exception: value < 2 is rounded to 2.
    roundValue = (1u << n);

    if (value == 3u)
    {
        // Round up.
        // Round value to smallest power of 2 which is larger then value.
        // Exception: If resulting rounded value would not be representable then
        //            round to largest representable power of 2.
        uint32_t tmp = (roundValue << 1u); // in case of overflow, tmp equals 0
        if (tmp)
        {
          // no overflow occurred
          roundValue = tmp;
        }
        // else: roundValue already contains largest representable power of 2
    }

    return roundValue;
}

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
float64 RB_MATH_RoundStep(float64 value)
{
    int isNeg;    // indicate if value is negative
    int32_t exponent; // exponent
    float64 mantissa; // mantissa

    if (!RB_MATH_frexp10(value, &isNeg, &mantissa, &exponent))
    {
        return value;
    }

    // round to nearest number
    if (mantissa >= 7.5)
    {
        mantissa = 10.0;
    }
    else if (mantissa >= 3.5)
    {
        mantissa = 5.0;
    }
    else if (mantissa >= 1.5)
    {
        mantissa = 2.0;
    }
    else if (mantissa >= 1.0)
    {
        mantissa = 1.0;
    }
    // else: mantissa=0.0

    return compose(isNeg, mantissa, exponent);
}

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
float64 RB_MATH_RoundUpStep(float64 value)
{
    int isNeg;    // indicate if value is negative
    int32_t exponent; // exponent
    float64 mantissa; // mantissa

    if (!RB_MATH_frexp10(value, &isNeg, &mantissa, &exponent))
    {
        return value;
    }

    // round towards infinity
    if (mantissa > 5.0)
    {
        mantissa = 10.0;
    }
    else if (mantissa > 2.0)
    {
        mantissa = 5.0;
    }
    else if (mantissa > 1.0)
    {
        mantissa = 2.0;
    }
    // else: mantissa=1.0 or mantissa=0.0

    return compose(isNeg, mantissa, exponent);
}

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
float64 RB_MATH_RoundDownStep(float64 value)
{
    int isNeg;        // indicate if value is negative
    int32_t exponent; // exponent
    float64 mantissa; // mantissa

    if (!RB_MATH_frexp10(value, &isNeg, &mantissa, &exponent))
    {
        return value;
    }

    // round towards zero
    if (mantissa >= 5.0)
    {
        mantissa = 5.0;
    }
    else if (mantissa >= 2.0)
    {
        mantissa = 2.0;
    }
    else if (mantissa >= 1.0)
    {
        mantissa = 1.0;
    }
    // else: mantissa=0.0

    return compose(isNeg, mantissa, exponent);
}

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
bool RB_MATH_Vander(const float64 *x, float64 *fp, int32_t n)
{
    int32_t i,k;
    float64 * const c = fp;
    float64 * const p = fp;
    bool success = true;

    // Calculate coefficients c_k of interpolating Newton polynomial
    // p(x) = c_0*N_0(x) + c_1*N_1(x) + .. + c_n*N_n(x)
    // where the Newton basis polynomials are defined as
    // N_0(x) = 1, N_(k+1)(x) = (x-x_k)*N_k(x).
    // The c_k can be calculated as the divided difference of fp[].
    for (k = 0; k < n; k++)
    {
        for (i = n; i > k; i--)
        {
            float64 denom = x[i] - x[(i - 1) - k];
            if ((denom < FLT64_EPSILON) && (denom > -FLT64_EPSILON)) {
                // singular matrix, set element to zero
                c[i] = (float64)0.0;
                success = false;
            } else {
                c[i] = (c[i] - c[i-1]) / denom;
            }
        }
    }

    // generate coefficients p_k from c_k
    for (k = n-1; k >= 0; k--)
    {
        for (i = k; i < n; i++)
        {
            p[i] -= p[i+1] * x[k];
        }
    }

    return success;
}

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
void RB_MATH_LeastSquaresLine(const float64 *x, const float64 *y, int32_t n, float64 *a, float64 *b, float64 *r2)
{
    int32_t k;   // loop variable
    float64 sx;  // sum of x[]
    float64 sy;  // sum of y[]
    float64 sxn; // mean of x[]
    float64 t;   // substitution t[k] = x[k] - sxn to improve numerical stability
    float64 st2; // sum of t[k]^2
    float64 e;

    // accumulate sums
    sx = 0.0;
    sy = 0.0;
    for (k = 0; k < n; k++)
    {
        sx += x[k];
        sy += y[k];
    }

    sxn = sx/n; // mean of x[]

    *b = 0.0;
    st2 = 0.0;
    for (k = 0; k < n; k++)
    {
        t = x[k] - sxn;
        st2 += t*t;
        *b += t*y[k];
    }

    if (st2) *b /= st2;
    *a = (sy-sx*(*b))/n;

    // calculate residue
    *r2 = 0.0;
    for (k = 0; k < n; k++)
    {
        e = (y[k] - (*a)) - (*b)*x[k];
        *r2 += e*e;
    }
}

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
void RB_MATH_LeastSquaresParabola(const float64 *x, const float64 *y, int32_t n, float64 *a, float64 *b, float64 *c, float64 *r2)
{
    int32_t k;   // loop variable

    float64 x1,x2,x3,x4,det,e;

    float64 sx1  = 0.0;
    float64 sx2  = 0.0;
    float64 sx3  = 0.0;
    float64 sx4  = 0.0;
    float64 sy   = 0.0;
    float64 syx  = 0.0;
    float64 syx2 = 0.0;

    for (k = 0; k < n; k++)
    {
        x1 = x[k];          // x^1
        x2 = x[k] * x1;     // x^2
        x3 = x[k] * x2;     // x^3
        x4 = x[k] * x3;     // x^4

        sx1 += x1;          // accumulate x^1
        sx2 += x2;          // accumulate x^2
        sx3 += x3;          // accumulate x^3
        sx4 += x4;          // accumulate x^4

        sy   += y[k];       // accumulate y
        syx  += y[k] * x1;  // accumulate y*x^1
        syx2 += y[k] * x2;  // accumulate y*x^2
    }

    det = (((n*sx2*sx4 + 2.0*sx1*sx2*sx3) - sx2*sx2*sx2) - sx1*sx1*sx4) - n*sx3*sx3;

    *a = ((((sx2*sx4*sy + sx2*sx3*syx  + sx1*sx3*syx2) - sx2*sx2*syx2) - sx1*sx4*syx)  - sx3*sx3*sy) / det;
    *b = ((((n*sx4*syx  + sx1*sx2*syx2 + sx2*sx3*sy)   - sx2*sx2*syx)  - sx1*sx4*sy)   - n*sx3*syx2) / det;
    *c = ((((n*sx2*syx2 + sx1*sx3*sy   + sx1*sx2*syx)  - sx2*sx2*sy)   - sx1*sx1*syx2) - n*sx3*syx ) / det;

    // calculate residue
    *r2 = 0.0;
    for (k = 0; k < n; k++)
    {
        e = y[k] - ((*a) + x[k] * ((*b) + x[k] * (*c)));
        *r2 += e*e;
    }
}

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
bool RB_MATH_AlmostEqualAbs(float64 x, float64 y, float64 e)
{
    float64 d = x - y;

    // fabs(d)
    if (d < 0.0) {
        d = -d;
    }

    if (d <= e) {
        return true;
    } else {
        return false;
    }
}

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
bool RB_MATH_AlmostEqualRel(float64 x, float64 y, float64 e)
{
    float64 d = x - y;
    float64 m;

    // fabs(d)
    if (d < 0.0) {
        d = -d;
    }

    // fabs(x)
    if (x < 0.0) {
        x = -x;
    }

    // fabs(y)
    if (y < 0.0) {
        y = -y;
    }

    // m = max(|x|,|y|)
    if (x >= y) {
        m = x;
    } else {
        m = y;
    }

    // relative error
    if (d <= m * e) {
        return true;
    } else {
        return false;
    }
}

//==================================================================================================
//  L O C A L   F U N C T I O N   I M P L E M E N T A T I O N
//==================================================================================================

//----------------------------------------------------------------------------
// upow
//----------------------------------------------------------------------------
//! \brief	Power function for nonnegative integer exponent
//!        Any base raised to the power 0 equals 1
//!
//! \param	x     Base
//! \param	n     Exponent
//!
//! \return	Value of x raised to the power n
//----------------------------------------------------------------------------
static float64 upow(float64 x, uint32_t n)
{
    // exponentiation by squaring
    float64 result = 1.0;
    while (n)
    {
        if (n & 1) // n is odd
        {
            result *= x;
        }
        x *= x;
        n >>= 1; // divide by 2, round towards zero
    }
    return result;
}

//----------------------------------------------------------------------------
// compose
//----------------------------------------------------------------------------
//! \brief	Compose floating point number from sign bit, mantissa and exponent
//!
//! \param	s     Sign bit
//! \param	m     Mantissa
//! \param	e     Exponent
//
//! \return	Composed floating point number (-1)^s * m * 10^e
//----------------------------------------------------------------------------
static float64 compose(int s, float64 m, int32_t e)
{
    float64 x;

    x = m*RB_MATH_ipow10(e);

    // consider the sign bit
    if (s)
    {
        x = -x;
    }

    return x;
}

