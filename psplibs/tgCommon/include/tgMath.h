/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMath.h
*   General math functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

#ifndef _TGMATH_H_
#define _TGMATH_H_

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgTypes.h"

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

// Useful maths constants
#define TGMATH_EPSILON	1e-6f						// small threshold value when avoiding problems with floating point inaccuracy
#define TGMATH_MAXFLOAT	3.40282347e+38f				// maximum float value 
#define TGMATH_E		2.7182818284590452354f		// natural log base
#define TGMATH_LOG2E	1.4426950408889634074f		// log2(e)
#define TGMATH_LOG10E	0.43429448190325182765f		// log10(e)
#define TGMATH_LN2		0.69314718055994530942f		// ln(2)
#define TGMATH_LN10		2.30258509299404568402f		// ln(10)
#define TGMATH_PI		3.14159265358979323846f		// pi
#define TGMATH_TWOPI	6.28318530717958647692f		// pi*2
#define TGMATH_PI_2		1.57079632679489661923f		// pi/2
#define TGMATH_PI_4		0.78539816339744830962f		// pi/4
#define TGMATH_3PI_4	2.3561944901923448370f		// pi*3/4
#define TGMATH_SQRTPI	1.77245385090551602792981f	// sqrt(pi)
#define TGMATH_1_PI		0.31830988618379067154f		// 1/pi
#define TGMATH_2_PI		0.63661977236758134308f		// 2/pi
#define TGMATH_2_SQRTPI	1.12837916709551257390f		// 2/sqrt(pi)
#define TGMATH_SQRT2	1.41421356237309504880f		// sqrt(2)
#define TGMATH_SQRT1_2	0.70710678118654752440f		// sqrt(1/2)
#define TGMATH_LN2LO	1.9082149292705877000e-10f	// upper bits of ln(2)
#define TGMATH_LN2HI	6.9314718036912381649e-1f	// lower bits of ln(2)
#define TGMATH_SQRT3	1.73205080756887719000f		// sqrt(3)
#define TGMATH_INVLN10	0.43429448190325182765f		// 1 / ln(10) 
#define TGMATH_LOG2_E	0.693147180559945309417f	// ln(2)
#define TGMATH_INVLN2	1.4426950408889633870f		// 1 / ln(2) 

#define TGMATH_MIN(a, b) ((a) < (b)) ? (a) : (b)	//return minimum value
#define TGMATH_MAX(a, b) ((a) > (b)) ? (a) : (b)	//return maximum value


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// tgCos
// Description: cosine
// Parameters: 	ang - angle in rads
// Returns: cosine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgCos(f32 ang);

//-----------------------------------------------------------------------------
// tgCosApprox
// Description: cosine approximation
// Parameters: 	ang - angle in rads
// Returns: approximate value of cosine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgCosApprox(f32 ang);

//-----------------------------------------------------------------------------
// tgSin
// Description: sine
// Parameters: 	ang - angle in rads
// Returns: sine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgSin(f32 ang);

//-----------------------------------------------------------------------------
// tgSinApprox
// Description: sine approximation
// Parameters: 	ang - angle in rads
// Returns: approximate value of sine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgSinApprox(f32 ang);

//-----------------------------------------------------------------------------
// tgTan
// Description: tangent
// Parameters: 	ang - angle in rads
// Returns: tangent of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgTan(f32 ang);

//-----------------------------------------------------------------------------
// tgAcos
// Description: arc-cosine
// Parameters: 	x - cosine of angle
// Returns: angle in rads
// Note: 
//-----------------------------------------------------------------------------
f32 tgAcos(f32 x);

//-----------------------------------------------------------------------------
// tgAsin
// Description: arc-sine
// Parameters: 	x - sine of angle
// Returns: angle in rads
// Note: 
//-----------------------------------------------------------------------------
f32 tgAsin(f32 x);

//-----------------------------------------------------------------------------
// tgAtan
// Description: arc-tangent
// Parameters: 	x - tangent of angle
// Returns: angle in rads
// Note: 
//-----------------------------------------------------------------------------
f32 tgAtan(f32 x);

//-----------------------------------------------------------------------------
// tgAtanApprox
// Description: arc-tangent approximation
// Parameters: 	x - tangent of angle
// Returns: angle in rads
// Note: -1 < x < 1
//-----------------------------------------------------------------------------
f32 tgAtanApprox(f32 x);

//-----------------------------------------------------------------------------
// tgSqrt
// Description: square root
// Parameters: 	x - value
// Returns: square root of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgSqrt(f32 x);

//-----------------------------------------------------------------------------
// tgFtoi
// Description: float to int
// Parameters: 	x - value
// Returns: x as integer
// Note: 
//-----------------------------------------------------------------------------
s32 tgFtoi(f32 x);

//-----------------------------------------------------------------------------
// tgItof
// Description: int to float
// Parameters: 	x - value
// Returns: x as float
// Note: 
//-----------------------------------------------------------------------------
f32 tgItof(s32 x);

//-----------------------------------------------------------------------------
// tgFabs
// Description: absolute value
// Parameters: 	x - value
// Returns: absolute value of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgFabs(f32 x);

//-----------------------------------------------------------------------------
// tgExp
// Description: expoential
// Parameters: 	x - value
// Returns: e raised to the power of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgExp(f32 x);

//-----------------------------------------------------------------------------
// tgExpApprox
// Description: expoential approximation
// Parameters: 	x - value
// Returns: approximate value of e raised to the power of x
// Note: accurate to within approx 0.25%
//-----------------------------------------------------------------------------
f32 tgExpApprox(f32 x);

//-----------------------------------------------------------------------------
// tgPow
// Description: power
// Parameters: 	x - value
//			y - power
// Returns: x raised to the power of y
// Note: 
//-----------------------------------------------------------------------------
f32 tgPow(f32 x, f32 y);

//-----------------------------------------------------------------------------
// tgPowApprox
// Description: power approximation
// Parameters: 	x - value
//			y - power
// Returns: approximate value of x raised to the power of y
// Note: x must be <= 1, doesn't work for very small values of x
//-----------------------------------------------------------------------------
f32 tgPowApprox(f32 x, f32 y);

#ifdef __cplusplus
}
#endif

#endif //_TGMATH_H_
