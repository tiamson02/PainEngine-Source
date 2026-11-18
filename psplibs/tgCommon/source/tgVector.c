/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgVector.c
*   Vector maths functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgVector.h"
#include "tgMath.h"

//DOM-IGNORE-BEGIN

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgVectorCopy
// Description: copy vector
// Params: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc.xyzw
//-----------------------------------------------------------------------------
void tgVectorCopy(TgVector *pDest, const TgVector *pSrc)
{
	pDest->x = pSrc->x;
	pDest->y = pSrc->y;
	pDest->z = pSrc->z;
	pDest->w = pSrc->w;
}

//-----------------------------------------------------------------------------
// tgVectorAdd
// Description: add vectors
// Params: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw + pSrc2.xyzw
//-----------------------------------------------------------------------------
void tgVectorAdd(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2)
{
	pDest->x = pSrc1->x + pSrc2->x;
	pDest->y = pSrc1->y + pSrc2->y;
	pDest->z = pSrc1->z + pSrc2->z;
	pDest->w = pSrc1->w + pSrc2->w;
}

//-----------------------------------------------------------------------------
// tgVectorSub
// Description: subtract vectors
// Params: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw - pSrc2.xyzw
//-----------------------------------------------------------------------------
void tgVectorSub(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2)
{
	pDest->x = pSrc1->x - pSrc2->x;
	pDest->y = pSrc1->y - pSrc2->y;
	pDest->z = pSrc1->z - pSrc2->z;
	pDest->w = pSrc1->w - pSrc2->w;
}

//-----------------------------------------------------------------------------
// tgVectorLerp
// Description: vector interpolate
// Params: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
//			t - interpolation amount
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw * (1-t) + pSrc2.xyzw * t
//-----------------------------------------------------------------------------
void tgVectorLerp(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2, f32 t)
{
	pDest->x = pSrc1->x * (1.f - t) + pSrc2->x * t;
	pDest->y = pSrc1->y * (1.f - t) + pSrc2->y * t;
	pDest->z = pSrc1->z * (1.f - t) + pSrc2->z * t;
	pDest->w = pSrc1->w * (1.f - t) + pSrc2->w * t;
}

//-----------------------------------------------------------------------------
// tgVectorDot
// Description: vector dot product
// Params: 	pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: dot product of pSrc1 and pSrc2
// Note: 
//-----------------------------------------------------------------------------
f32 tgVectorDot(const TgVector *pSrc1, const TgVector *pSrc2)
{
	return pSrc1->x * pSrc2->x + pSrc1->y * pSrc2->y + pSrc1->z * pSrc2->z;
}

//-----------------------------------------------------------------------------
// tgVectorCross
// Description: vector cross product
// Params: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyz = pSrc1.xyz x pSrc2.xyz
//-----------------------------------------------------------------------------
void tgVectorCross(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2)
{
	pDest->x = pSrc1->y * pSrc2->z - pSrc1->z * pSrc2->y;
	pDest->y = pSrc1->z * pSrc2->x - pSrc1->x * pSrc2->z;
	pDest->z = pSrc1->x * pSrc2->y - pSrc1->y * pSrc2->x;
	pDest->w = 0.f;
}

//-----------------------------------------------------------------------------
// tgVectorScale
// Description: vector scale
// Params: 	pDest - destination vector pointer
//			pSrc - source vector pointer
//			scale - scale factor
// Returns: 
// Note: pDest.xyzw = pSrc.xyzw * scale
//-----------------------------------------------------------------------------
void tgVectorScale(TgVector *pDest, const TgVector *pSrc, f32 scale)
{
	pDest->x = pSrc->x * scale;
	pDest->y = pSrc->y * scale;
	pDest->z = pSrc->z * scale;
	pDest->w = pSrc->w * scale;
}

//-----------------------------------------------------------------------------
// tgVectorNormalize
// Description: vector normalize
// Params: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: length of pSrc
// Note: pDest = norm(pSrc)
//-----------------------------------------------------------------------------
f32 tgVectorNormalize(TgVector *pDest, const TgVector *pSrc)
{
	f32 len, ooLen;

	pDest->w = 0.f;

	len = tgSqrt(pSrc->x * pSrc->x + pSrc->y * pSrc->y + pSrc->z * pSrc->z);

	if(len > TGMATH_EPSILON)
	{
		ooLen = 1.f / len;

		pDest->x = pSrc->x * ooLen;
		pDest->y = pSrc->y * ooLen;
		pDest->z = pSrc->z * ooLen;
		return len;
	}
	else
	{
		pDest->x = 0.f;
		pDest->y = 0.f;
		pDest->z = 0.f;
		return 0.f;
	}
}

//-----------------------------------------------------------------------------
// tgVectorLength
// Description: vector length
// Params: 	pSrc - source vector pointer
// Returns: length of pSrc
// Note:
//-----------------------------------------------------------------------------
f32 tgVectorLength(const TgVector *pSrc)
{
	f32 dot;

	dot = pSrc->x * pSrc->x + pSrc->y * pSrc->y + pSrc->z * pSrc->z;

	return tgSqrt(dot);
}

//-----------------------------------------------------------------------------
// tgVectorNegate
// Description: vector negation
// Params: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: 
// Note: pDest.xyz = -pSrc.xyz
//-----------------------------------------------------------------------------
void tgVectorNegate(TgVector *pDest, const TgVector *pSrc)
{
	pDest->x = -pSrc->x;
	pDest->y = -pSrc->y;
	pDest->z = -pSrc->z;
	pDest->w = 0.f;
}
//DOM-IGNORE-END
