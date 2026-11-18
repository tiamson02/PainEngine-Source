/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMath.c
*   General math functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/


//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

#define COSF(x)		cosf(x)
#define SINF(x)		sinf(x)
#define TANF(x)		tanf(x)
#define ACOSF(x)	acosf(x)
#define ASINF(x)	asinf(x)
#define ATANF(x)	atanf(x)
#define EXPF(x)		expf(x)
#define POWF(x, y)	powf(x, y)

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include <math.h>
#include "tgMath.h"

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef union
{
	s32	i;
	f32	f;
} IntFloat;

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//DOM-IGNORE-BEGIN

//-----------------------------------------------------------------------------
// tgCos
// Description: cosine
// Params: 	ang - angle in rads
// Returns: cosine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgCos(f32 ang)
{
	return COSF(ang);
}

//-----------------------------------------------------------------------------
// tgCosApprox
// Description: cosine approximation
// Params: 	ang - angle in rads
// Returns: approximate value of cosine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgCosApprox(f32 ang)
{
	// based on code posted by Colin Hughes
	const f32 fk0 = 6.283185f;
	const f32 fk1 = -41.34167f;
	const f32 fk2 = 81.60223f;
	const f32 fk3 = -76.57496f;
	const f32 fk4 = 39.71066f;

	f32 f1, f2, f3;

	f1 = tgFabs(ang);

	f1 = f1 * (f32)(TGMATH_1_PI) * 0.5f;
	f1 = f1 - (f32)((s32)(f1));

	f1 = tgFabs(f1 - 0.5f);
	f1 = f1 - 0.25f;

	f3 = f1 * f1;
	f2 = f1 * fk0;
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk1);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk2);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk3);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk4);

	return f2;
}

//-----------------------------------------------------------------------------
// tgSin
// Description: sine
// Params: 	ang - angle in rads
// Returns: sine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgSin(f32 ang)
{
	return SINF(ang);
}

//-----------------------------------------------------------------------------
// tgSinApprox
// Description: sine approximation
// Params: 	ang - angle in rads
// Returns: approximate value of sine of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgSinApprox(f32 ang)
{
	// based on code posted by Colin Hughes
	const f32 fk0 = 6.283185f;
	const f32 fk1 = -41.34167f;
	const f32 fk2 = 81.60223f;
	const f32 fk3 = -76.57496f;
	const f32 fk4 = 39.71066f;

	f32 f1, f2, f3;

	f1 = tgFabs(ang - (f32)(TGMATH_PI_2));

	f1 = f1 * (f32)(TGMATH_1_PI) * 0.5f;
	f1 = f1 - (f32)((s32)(f1));

	f1 = tgFabs(f1 - 0.5f);
	f1 = f1 - 0.25f;

	f3 = f1 * f1;
	f2 = f1 * fk0;
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk1);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk2);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk3);
	f1 = f1 * f3;
	f2 = f2 + (f1 * fk4);

	return f2;
}

//-----------------------------------------------------------------------------
// tgTan
// Description: tangent
// Params: 	ang - angle in rads
// Returns: tangent of angle
// Note: 
//-----------------------------------------------------------------------------
f32 tgTan(f32 ang)
{
	return TANF(ang);
}

//-----------------------------------------------------------------------------
// tgAcos
// Description: arc-cosine
// Params: 	x - cosine of angle
// Returns: angle in rads
// Note: 
//-----------------------------------------------------------------------------
f32 tgAcos(f32 x)
{
	return ACOSF(x);
}

//-----------------------------------------------------------------------------
// tgAsin
// Description: arc-sine
// Params: 	x - sine of angle
// Returns: angle in rads
// Note: 
//-----------------------------------------------------------------------------
f32 tgAsin(f32 x)
{
	return ASINF(x);
}

//-----------------------------------------------------------------------------
// tgAtanApprox
// Description: arc-tangent approximation
// Parameters: 	x - tangent of angle
// Returns: angle in rads
// Note: -1 < x < 1
//-----------------------------------------------------------------------------
f32 tgAtan(f32 x)
{
	return ATANF(x);
}

//-----------------------------------------------------------------------------
// tgAtanApprox
// Description: arc-tangent approximation
// Params: 	x - tangent of angle
// Returns: angle in rads
// Note: -1 < x < 1
//-----------------------------------------------------------------------------
f32 tgAtanApprox(f32 x)
{
	// based on code posted by Martin Griffiths
	f32 fx2 = x * x;
	f32 fx3 = fx2 * x;
	return (x + 0.43157974f * fx3) / (1.0f + 0.76443945f * fx2 + 0.05831938f * fx3 * x);
}

//-----------------------------------------------------------------------------
// tgSqrt
// Description: square root
// Params: 	x - value
// Returns: square root of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgSqrt(f32 x)
{
	f32 y;
	__asm__ __volatile__ ("sqrt.s %0, %1" : "=f" (y) : "f" (x));
	return y;
}

//-----------------------------------------------------------------------------
// tgFtoi
// Description: float to int
// Params: 	x - value
// Returns: x as integer
// Note: 
//-----------------------------------------------------------------------------
s32 tgFtoi(f32 x)
{
	return (int)x;
}

//-----------------------------------------------------------------------------
// tgItof
// Description: int to float
// Params: 	x - value
// Returns: x as float
// Note: 
//-----------------------------------------------------------------------------
f32 tgItof(s32 x)
{
	return (f32)x;
}

//-----------------------------------------------------------------------------
// tgFabs
// Description: absolute value
// Params: 	x - value
// Returns: absolute value of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgFabs(f32 x)
{
	f32 y;
	__asm__ __volatile__ ("abs.s %0, %1" : "=f" (y) : "f" (x));
	return y;
}

//-----------------------------------------------------------------------------
// tgExp
// Description: expoential
// Params: 	x - value
// Returns: e raised to the power of x
// Note: 
//-----------------------------------------------------------------------------
f32 tgExp(f32 x)
{
	return EXPF(x);
}

//-----------------------------------------------------------------------------
// tgExpApprox
// Description: expoential approximation
// Params: 	x - value
// Returns: approximate value of e raised to the power of x
// Note: accurate to within approx 0.25%
//-----------------------------------------------------------------------------
f32 tgExpApprox(f32 x)
{
	// based on code posted by Matthew Jones
	const f32 fk = 12102203.0f;
	const f32 fc0 = 1065353216.0f;
	const f32 fc1 = 1067450368.0f;
	const f32 fc2 = 1069547520.0f;
	const f32 fc3 = 1071644672.0f;
	const f32 fd0 = 0.24037740f;
	const f32 fd1 = 0.20213249f;
	const f32 fd2 = 0.16957040f;
	const f32 fd3 = 0.14292926f;

	f32	fkx = fk * x;
	f32	fkxc0 = fkx + fc0;
	f32	fkxc1 = fkx + fc1;
	f32	fkxc2 = fkx + fc2;
	f32	fkxc3 = fkx + fc3;

	IntFloat	i0, i1, i2, i3;
	
	i0.i = (s32)fkxc0;
	i1.i = (s32)fkxc1;
	i2.i = (s32)fkxc2;
	i3.i = (s32)fkxc3;

	fkxc0 = i0.f;
	fkxc1 = i1.f;
	fkxc2 = i2.f;
	fkxc3 = i3.f;

	fkxc0 *= fd0;
	fkxc1 *= fd1;
	fkxc2 *= fd2;
	fkxc3 *= fd3;

	return fkxc0 + fkxc1 + fkxc2 + fkxc3;
}

//-----------------------------------------------------------------------------
// tgPow
// Description: power
// Params: 	x - value
//			y - power
// Returns: x raised to the power of y
// Note: 
//-----------------------------------------------------------------------------
f32 tgPow(f32 x, f32 y)
{
	return POWF(x, y);
}

//-----------------------------------------------------------------------------
// tgPowApprox
// Description: power approximation
// Params: 	x - value
//			y - power
// Returns: approximate value of x raised to the power of y
// Note: x must be <= 1, doesn't work for very small values of x
//-----------------------------------------------------------------------------
f32 tgPowApprox(f32 x, f32 y)
{
	// based on code posted by Matthew Jones
	const f32	fOneAsFloat = 1065353216.0f;
	f32			fMagic = fOneAsFloat - fOneAsFloat * y;
	IntFloat	ifX;
	f32			fX;

	ifX.f = x;
	fX = (f32)ifX.i;

	fX *= y;
	fX += fMagic;

	ifX.i = (s32)fX;
	fX = ifX.f;

	return fX;
}
//DOM-IGNORE-END
