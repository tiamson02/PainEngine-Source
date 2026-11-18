/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgQuaternion.c
*   Quaternion maths functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgQuaternion.h"
#include "tgMath.h"

//DOM-IGNORE-BEGIN

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgQuaternionCopy
// Description: vector negation
// Params: 	pDest - destination quat pointer
//			pSrc - source quat pointer
// Returns: 
// Note: pDest = pSrc
//-----------------------------------------------------------------------------
void tgQuaternionCopy(TgQuaternion *pDest, const TgQuaternion *pSrc)
{
	pDest->x = pSrc->x;
	pDest->y = pSrc->y;
	pDest->z = pSrc->z;
	pDest->w = pSrc->w;
}

//-----------------------------------------------------------------------------
// tgQuaternionMul
// Description: quaternion multiply
// Params: 	pDest - destination quat pointer
//			pSrc1 - source1 quat pointer
//			pSrc2 - source2 quat pointer
// Returns: 
// Note: pDest = pSrc1*pSrc2
//-----------------------------------------------------------------------------
void tgQuaternionMul(	TgQuaternion*		pDest,
						const TgQuaternion	*pSrc1,
						const TgQuaternion	*pSrc2 )
{
						// cross product  + V1*w2   + w1*C2
	pDest->x =	pSrc1->y*pSrc2->z - pSrc1->z*pSrc2->y + pSrc1->x*pSrc2->w + pSrc1->w*pSrc2->x;
	pDest->y =	pSrc1->z*pSrc2->x - pSrc1->x*pSrc2->z + pSrc1->y*pSrc2->w + pSrc1->w*pSrc2->y;
	pDest->z =	pSrc1->x*pSrc2->y - pSrc1->y*pSrc2->x + pSrc1->z*pSrc2->w + pSrc1->w*pSrc2->z;
	pDest->w =	pSrc1->w*pSrc2->w - pSrc1->x*pSrc2->x - pSrc1->y*pSrc2->y - pSrc1->z*pSrc2->z;
}


//-----------------------------------------------------------------------------
// tgQuaternionSlerp
// Description: quaternion slerp
// Params: 	pDest - destination quat pointer
//			pSrc1 - first source quat pointer
//			pSrc2 - second source quat pointer
//			t - slerp amount
// Returns: 
// Note: pDest = slerp(pSrc1, pSrc2, t)
//-----------------------------------------------------------------------------
void tgQuaternionSlerp(TgQuaternion *pDest, const TgQuaternion *pSrc1,
						 const TgQuaternion *pSrc2, f32 t)
{
	f32 c = pSrc1->x * pSrc2->x + pSrc1->y * pSrc2->y + pSrc1->z * pSrc2->z
			+ pSrc1->w * pSrc2->w;

	s32 shortest = 0;

	if(c < 0.f)
	{
		c = - c;
		shortest = 1;
	}

	if(c < 1.f)
	{
		f32 angle = tgAcos(c);
		f32 s = tgSin(angle);
		
		if(s != 0.f)
		{
			f32 r1 = tgSin((1 - t) * angle) / s;
			f32 r2 = tgSin(t * angle) / s;
			
			if(shortest)
				r2 = -r2;
			
			pDest->x = pSrc1->x * r1 + pSrc2->x * r2;
			pDest->y = pSrc1->y * r1 + pSrc2->y * r2;
			pDest->z = pSrc1->z * r1 + pSrc2->z * r2;
			pDest->w = pSrc1->w * r1 + pSrc2->w * r2;
			return;
		}
	}
	
	pDest->x = pSrc2->x;
	pDest->y = pSrc2->y;
	pDest->z = pSrc2->z;
	pDest->w = pSrc2->w;
}

//-----------------------------------------------------------------------------
// tgQuaternionToMatrix
// Description: convert quaternion to matrix
// Params: 	pDest - destination matrix pointer
//			pSrc - source quat pointer
// Returns: 
// Note: pDest = matrix(pSrc)
//-----------------------------------------------------------------------------
void tgQuaternionToMatrix(TgMatrix *pDest, const TgQuaternion *pSrc)
{
	f32 fTx  = 2.0f * pSrc->x;
	f32 fTy  = 2.0f * pSrc->y;
	f32 fTz  = 2.0f * pSrc->z;
	f32 fTwx = fTx * pSrc->w;
	f32 fTwy = fTy * pSrc->w;
	f32 fTwz = fTz * pSrc->w;
	f32 fTxx = fTx * pSrc->x;
	f32 fTxy = fTy * pSrc->x;
	f32 fTxz = fTz * pSrc->x;
	f32 fTyy = fTy * pSrc->y;
	f32 fTyz = fTz * pSrc->y;
	f32 fTzz = fTz * pSrc->z;

	pDest->m[0][0] = 1.0f - (fTyy + fTzz);
	pDest->m[0][1] = fTxy + fTwz;
	pDest->m[0][2] = fTxz - fTwy;
	pDest->m[0][3] = 0.0f;

	pDest->m[1][0] = fTxy - fTwz;
	pDest->m[1][1] = 1.0f - (fTxx + fTzz);
	pDest->m[1][2] = fTyz + fTwx;
	pDest->m[1][3] = 0.0f;

	pDest->m[2][0] = fTxz + fTwy;
	pDest->m[2][1] = fTyz - fTwx;
	pDest->m[2][2] = 1.0f - (fTxx + fTyy);
	pDest->m[2][3] = 0.0f;

	pDest->m[3][0] = 0.0f;
	pDest->m[3][1] = 0.0f;
	pDest->m[3][2] = 0.0f;
	pDest->m[3][3] = 1.0f;
}
//DOM-IGNORE-END

