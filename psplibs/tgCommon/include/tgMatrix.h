/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMatrix.h
*   Matrix math functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

#ifndef _TGMATRIX_H_
#define _TGMATRIX_H_

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

#define TG_MATRIX_ZERO		{{{ {{{ 0.f, 0.f, 0.f, 0.f }}}, {{{ 0.f, 0.f, 0.f, 0.f }}}, {{{ 0.f, 0.f, 0.f, 0.f }}}, {{{ 0.f, 0.f, 0.f, 0.f }}} }}}	//initailise zero matrix
#define TG_MATRIX_IDENTITY	{{{ {{{ 1.f, 0.f, 0.f, 0.f }}}, {{{ 0.f, 1.f, 0.f, 0.f }}}, {{{ 0.f, 0.f, 1.f, 0.f }}}, {{{ 0.f, 0.f, 0.f, 1.f }}} }}}	//initailise identity matrix

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgVector.h"

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

//4x4 matrix 
typedef struct _TgMatrix
{
	union
	{
		struct						
		{
			//4-vector 
			TgVector x, y, z, w;	
		};							//4 vector representation

		f32 m[4][4];				//2d array representation
	};							
} TgMatrix;

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// tgMatrixCopy
// Description: copy matrix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixCopy(TgMatrix *pDest, const TgMatrix *pSrc);

//-----------------------------------------------------------------------------
// tgMatrixSetIdentity
// Description: set matrix to identity
// Parameters: 	pDest - destination matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixSetIdentity(TgMatrix *pDest);

//-----------------------------------------------------------------------------
// tgMatrixMulMatrix
// Description: matrix multiply
// Parameters: 	pDest - destination matrix pointer
//			pSrc1 - first source matrix pointer
//			pSrc2 - second source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixMulMatrix(TgMatrix *pDest, const TgMatrix *pSrc1, const TgMatrix *pSrc2);

//-----------------------------------------------------------------------------
// tgMatrixMulVector
// Description: matrix vector multiply
// Parameters: 	pDest - destination vector pointer
//			pSrc1 - source matrix pointer
//			pSrc2 - source vector pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixMulVector(TgVector *pDest, const TgMatrix *pSrc1, const TgVector *pSrc2);

//-----------------------------------------------------------------------------
// tgMatrixTranslateF
// Description: create translation matrix
// Parameters: 	pDest - destination matrix pointer
//			x - x translation value
//			y - y translation value
//			z - z translation value
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranslateF(TgMatrix *pDest, f32 x, f32 y, f32 z);

//-----------------------------------------------------------------------------
// tgMatrixTranslateV
// Description: create translation matrix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - translation vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranslateV(TgMatrix *pDest, const TgVector *pSrc);

//-----------------------------------------------------------------------------
// tgMatrixRotateX
// Description: create x-axis rotation matrix
// Parameters: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateX(TgMatrix *pDest, f32 ang);

//-----------------------------------------------------------------------------
// tgMatrixRotateY
// Description: create y-axis rotation matrix
// Parameters: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateY(TgMatrix *pDest, f32 ang);

//-----------------------------------------------------------------------------
// tgMatrixRotateZ
// Description: create z-axis rotation matrix
// Parameters: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateZ(TgMatrix *pDest, f32 ang);

//-----------------------------------------------------------------------------
// tgMatrixRotateAxisF
// Description: create arbitrary axis rotation matrix
// Parameters: 
//		pDest - destination matrix pointer.
//		x - Rotation axis x value.
//		y - Rotation axis y value.
//		z - Rotation axis z value.
//		ang -   angle in radians
// Returns:
// Note:
//----------------------------------------------------------------------------- 
void tgMatrixRotateAxisF(TgMatrix *pDest, f32 x, f32 y, f32 z, f32 ang);

//-----------------------------------------------------------------------------
// tgMatrixRotateAxisV
// Description: create arbitrary axis rotation matrix
// Parameters: 	pDest - destination matrix pointer
//			pAxis - axis vector
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateAxisV(TgMatrix *pDest, const TgVector *pAxis, f32 ang);

//-----------------------------------------------------------------------------
// tgMatrixScaleF
// Description: create scale matrix
// Parameters: 	pDest - destination matrix pointer
//				x - scale factor in x
//				y - scale factor in y
//				z - scale factor in z
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixScaleF(TgMatrix *pDest, f32 x, f32 y, f32 z);

//-----------------------------------------------------------------------------
// tgMatrixScaleV
// Description: create scale matrix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - scale vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixScaleV(TgMatrix *pDest, const TgVector *pSrc);

//-----------------------------------------------------------------------------
// tgMatrixLookAt
// Description: create look-at camera matrix
// Parameters: 	pDest - destination matrix pointer
//			pCenter - center position
//			pEye - eye position
//			pUp - up vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixLookAt(TgMatrix *pDest, const TgVector *pCenter, const TgVector *pEye,
					const TgVector *pUp);

//-----------------------------------------------------------------------------
// tgMatrixInverse
// Description: inverse matix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixInverse(TgMatrix *pDest, const TgMatrix *pSrc);

//-----------------------------------------------------------------------------
// tgMatrixTranspose
// Description: transpose matix
// Parameters: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranspose(TgMatrix *pDest, const TgMatrix *pSrc);

//-----------------------------------------------------------------------------
// tgMatrixOrtho
// Description: create orthogonal viewing matrix
// Parameters: 	pDest - destination matrix pointer
//			left - left co-ordinate
//			right - right co-ordinate
//			bottom - bottom co-ordinate
//			top - top co-ordinate
//			nearz - near co-ordinate
//			farz - far co-ordinate
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixOrtho(TgMatrix *pDest, f32 left, f32 right, f32 bottom,
						 f32 top, f32 nearz, f32 farz);

//-----------------------------------------------------------------------------
// tgMatrixPerspective
// Description: create perspective viewing matrix
// Parameters: 	pDest - destination matrix pointer
//			fov - field of view in radians
//			aspect - aspect ratio
//			nearz - near co-ordinate
//			farz - far co-ordinate
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixPerspective(TgMatrix *pDest, f32 fov, f32 aspect,
						 f32 nearz, f32 farz);

#ifdef __cplusplus
}
#endif

#endif //_TGMATRIX_H_
