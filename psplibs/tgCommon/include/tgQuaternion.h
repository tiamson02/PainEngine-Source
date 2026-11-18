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

#ifndef _TGQUATERNION_H_
#define _TGQUATERNION_H_

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgMatrix.h"

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

//quaternion structure
typedef struct _TgQuaternion
{
	union
	{
		struct
		{
			//quaternion component
			f32	x, y, z, w;	
		};					//component representation 

		f32	v[4];			//array representation
	};
} TgQuaternion;

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// tgQuaternionCopy
// Description: vector negation
// Parameters: 	pDest - destination quat pointer
//			pSrc - source quat pointer
// Returns: 
// Note: pDest = pSrc
//-----------------------------------------------------------------------------
void tgQuaternionCopy(TgQuaternion *pDest, const TgQuaternion *pSrc);

//-----------------------------------------------------------------------------
// tgQuaternionMul
// Description: quaternion multiply
// Parameters: 	pDest - destination quat pointer
//			pSrc1 - source1 quat pointer
//			pSrc2 - source2 quat pointer
// Returns: 
// Note: pDest = pSrc1*pSrc2
//-----------------------------------------------------------------------------
void tgQuaternionMul(TgQuaternion *pDest, const TgQuaternion *pSrc1, const TgQuaternion *pSrc2);

//-----------------------------------------------------------------------------
// tgQuaternionSlerp
// Description: quaternion slerp
// Parameters: 	pDest - destination quat pointer
//			pSrc1 - first source quat pointer
//			pSrc2 - second source quat pointer
//			t - slerp amount
// Returns: 
// Note: pDest = slerp(pSrc1, pSrc2, t)
//-----------------------------------------------------------------------------
void tgQuaternionSlerp(TgQuaternion *pDest, const TgQuaternion *pSrc1,
						 const TgQuaternion *pSrc2, f32 t);

//-----------------------------------------------------------------------------
// tgQuaternionToMatrix
// Description: convert quaternion to matrix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - source quat pointer
// Returns: 
// Note: pDest = matrix(pSrc)
//-----------------------------------------------------------------------------
void tgQuaternionToMatrix(TgMatrix *pDest, const TgQuaternion *pSrc);

#ifdef __cplusplus
}
#endif

#endif //_TGQUATERNION_H_
