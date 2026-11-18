/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgVector.h
*   Vector maths functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

#ifndef _TGVECTOR_H_
#define _TGVECTOR_H_

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgTypes.h"

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

// set a vector
#define TG_VECTOR_SET(name, _x, _y, _z, _w) name.x = _x; name.y = _y; \
											name.z = _z; name.w = _w
// a zero vector
#define TG_VECTOR_ZERO	{{{ 0.f, 0.f, 0.f, 1.f }}}
// a one vector
#define TG_VECTOR_ONE	{{{ 1.f, 1.f, 1.f, 1.f }}}


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

//vector structure
typedef struct _TgVector
{
	union
	{
		struct
		{	
			//4-vector component
			f32	x, y, z, w;	
		};					//component representation

		f32	v[4];			//array representation
	};
} TgVector;

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// tgVectorCopy
// Description: copy vector
// Parameters: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc.xyzw
//-----------------------------------------------------------------------------
void tgVectorCopy(TgVector *pDest, const TgVector *pSrc);

//-----------------------------------------------------------------------------
// tgVectorAdd
// Description: add vectors
// Parameters: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw + pSrc2.xyzw
//-----------------------------------------------------------------------------
void tgVectorAdd(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2);

//-----------------------------------------------------------------------------
// tgVectorSub
// Description: subtract vectors
// Parameters: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw - pSrc2.xyzw
//-----------------------------------------------------------------------------
void tgVectorSub(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2);

//-----------------------------------------------------------------------------
// tgVectorLerp
// Description: vector interpolate
// Parameters: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
//			t - interpolation amount
// Returns: 
// Note: pDest.xyzw = pSrc1.xyzw * (1-t) + pSrc2.xyzw * t
//-----------------------------------------------------------------------------
void tgVectorLerp(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2, f32 t);

//-----------------------------------------------------------------------------
// tgVectorDot
// Description: vector dot product
// Parameters: 	pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: dot product of pSrc1 and pSrc2
// Note: 
//-----------------------------------------------------------------------------
f32 tgVectorDot(const TgVector *pSrc1, const TgVector *pSrc2);

//-----------------------------------------------------------------------------
// tgVectorCross
// Description: vector cross product
// Parameters: 	pDest - destination vector pointer
//			pSrc1 - first source vector pointer
//			pSrc2 - second source vector pointer
// Returns: 
// Note: pDest.xyz = pSrc1.xyz x pSrc2.xyz
//-----------------------------------------------------------------------------
void tgVectorCross(TgVector *pDest, const TgVector *pSrc1, const TgVector *pSrc2);

//-----------------------------------------------------------------------------
// tgVectorScale
// Description: vector scale
// Parameters: 	pDest - destination vector pointer
//			pSrc - source vector pointer
//			scale - scale factor
// Returns: 
// Note: pDest.xyzw = pSrc.xyzw * scale
//-----------------------------------------------------------------------------
void tgVectorScale(TgVector *pDest, const TgVector *pSrc, f32 scale);

//-----------------------------------------------------------------------------
// tgVectorNormalize
// Description: vector normalize
// Parameters: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: length of pSrc
// Note: pDest = norm(pSrc)
//-----------------------------------------------------------------------------
f32 tgVectorNormalize(TgVector *pDest, const TgVector *pSrc);

//-----------------------------------------------------------------------------
// tgVectorLength
// Description: vector length
// Parameters: 	pSrc - source vector pointer
// Returns: length of pSrc
// Note:
//-----------------------------------------------------------------------------
f32 tgVectorLength(const TgVector *pSrc);

//-----------------------------------------------------------------------------
// tgVectorNegate
// Description: vector negation
// Parameters: 	pDest - destination vector pointer
//			pSrc - source vector pointer
// Returns: 
// Note: pDest.xyz = -pSrc.xyz
//-----------------------------------------------------------------------------
void tgVectorNegate(TgVector *pDest, const TgVector *pSrc);

#ifdef __cplusplus
}
#endif

#endif //_TGVECTOR_H_
