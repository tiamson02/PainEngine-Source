/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMatrix.c
*   Matrix math functions
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      sidwell     initial implementation
*/

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgMatrix.h"
#include "tgMath.h"
#include "tgVector.h"

//DOM-IGNORE-BEGIN

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgMatrixCopy
// Description: copy matrix
// Params: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixCopy(TgMatrix *pDest, const TgMatrix *pSrc)
{
	s32 i, j;

	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j ++)
			pDest->m[i][j] = pSrc->m[i][j];
}

//-----------------------------------------------------------------------------
// tgMatrixSetIdentity
// Description: set matrix to identity
// Params: 	pDest - destination matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixSetIdentity(TgMatrix *pDest)
{
	s32 i, j;

	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j ++)
			if(i == j)
				pDest->m[i][j] = 1.f;
			else
				pDest->m[i][j] = 0.f;
}

//-----------------------------------------------------------------------------
// tgMatrixMulMatrix
// Description: matrix multiply
// Params: 	pDest - destination matrix pointer
//			pSrc1 - first source matrix pointer
//			pSrc2 - second source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixMulMatrix(TgMatrix *pDest, const TgMatrix *pSrc1, const TgMatrix *pSrc2)
{
	s32			i, j, k;
	TgMatrix	tmp;

	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j ++)
		{
			tmp.m[j][i] = 0.f;
			for(k = 0; k < 4; k ++)
				tmp.m[j][i] += pSrc1->m[k][i] * pSrc2->m[j][k];
		}

	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j ++)
			pDest->m[i][j] = tmp.m[i][j];
}

//-----------------------------------------------------------------------------
// tgMatrixMulVector
// Description: matrix vector multiply
// Params: 	pDest - destination vector pointer
//			pSrc1 - source matrix pointer
//			pSrc2 - source vector pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixMulVector(TgVector *pDest, const TgMatrix *pSrc1, const TgVector *pSrc2)
{
	s32 i, j;
	TgVector	tmp;

	tgVectorCopy(&tmp, pSrc2);

	for(i = 0; i < 4; i ++)
	{
		pDest->v[i] = 0.f;
		for(j = 0; j < 4; j ++)
			pDest->v[i] += pSrc1->m[j][i] * tmp.v[j];
	}
}

//-----------------------------------------------------------------------------
// tgMatrixTranslateF
// Description: create translation matrix
// Params: 	pDest - destination matrix pointer
//			x - x translation value
//			y - y translation value
//			z - z translation value
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranslateF(TgMatrix *pDest, f32 x, f32 y, f32 z)
{
	tgMatrixSetIdentity(pDest);

	pDest->m[3][0] = x;
	pDest->m[3][1] = y;
	pDest->m[3][2] = z;
}

//-----------------------------------------------------------------------------
// tgMatrixTranslateV
// Description: create translation matrix
// Params: 	pDest - destination matrix pointer
//			pSrc - translation vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranslateV(TgMatrix *pDest, const TgVector *pSrc)
{
	tgMatrixSetIdentity(pDest);

	pDest->m[3][0] = pSrc->x;
	pDest->m[3][1] = pSrc->y;
	pDest->m[3][2] = pSrc->z;
}

//-----------------------------------------------------------------------------
// tgMatrixRotateX
// Description: create x-axis rotation matrix
// Params: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateX(TgMatrix *pDest, f32 ang)
{
	f32 cos, sin;

	tgMatrixSetIdentity(pDest);

	cos = tgCos(ang);
	sin = tgSin(ang);

	pDest->m[1][1] = cos; 
	pDest->m[1][2] = sin; 
	
	pDest->m[2][1] = -sin;
	pDest->m[2][2] = cos;
}

//-----------------------------------------------------------------------------
// tgMatrixRotateY
// Description: create y-axis rotation matrix
// Params: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateY(TgMatrix *pDest, f32 ang)
{
	f32 cos, sin;

	tgMatrixSetIdentity(pDest);

	cos = tgCos(ang);
	sin = tgSin(ang);

	pDest->m[0][0] = cos;
	pDest->m[0][2] = -sin;
	
	pDest->m[2][0] = sin;
	pDest->m[2][2] = cos;
}

//-----------------------------------------------------------------------------
// tgMatrixRotateZ
// Description: create z-axis rotation matrix
// Params: 	pDest - destination matrix pointer
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateZ(TgMatrix *pDest, f32 ang)
{
	f32 cos, sin;

	tgMatrixSetIdentity(pDest);

	cos = tgCos(ang);
	sin = tgSin(ang);

	pDest->m[0][0] = cos;
	pDest->m[0][1] = sin;

	pDest->m[1][0] = -sin;
	pDest->m[1][1] = cos;
}

//-----------------------------------------------------------------------------
// tgMatrixRotateAxisF
// Description: create arbitrary axis rotation matrix
// Params: 	pDest - destination matrix pointer
//			x - axis x value
//			y - axis y value
//			z - axis z value
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateAxisF(TgMatrix *pDest, f32 x, f32 y, f32 z, f32 ang)
{
	f32 cos, sin, t, sx, sy, sz, tx, ty, tz;

	cos = tgCos(ang);
	sin = tgSin(ang);
	t = 1.f - cos;
	sx = sin * x;
	sy = sin * y;
	sz = sin * z;
	tx = t * x;
	ty = t * y;
	tz = t * z;

	pDest->m[0][0] = tx * x + cos;
	pDest->m[0][1] = tx * y + sz;
	pDest->m[0][2] = tx * z - sy;
	pDest->m[0][3] = 0.f;

	pDest->m[1][0] = tx * y - sz;
	pDest->m[1][1] = ty * y + cos;
	pDest->m[1][2] = ty * z + sx;
	pDest->m[1][3] = 0.f;
	
	pDest->m[2][0] = tx * z + sy;
	pDest->m[2][1] = ty * z - sx;
	pDest->m[2][2] = tz * z + cos;
	pDest->m[2][3] = 0.f;

	pDest->m[3][0] = 0.f;
	pDest->m[3][1] = 0.f;
	pDest->m[3][2] = 0.f;
	pDest->m[3][3] = 1.f;
}

//-----------------------------------------------------------------------------
// tgMatrixRotateAxisV
// Description: create arbitrary axis rotation matrix
// Params: 	pDest - destination matrix pointer
//			pAxis - axis vector
//			ang - angle in radians
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixRotateAxisV(TgMatrix *pDest, const TgVector *pAxis, f32 ang)
{
	f32 cos, sin, t, sx, sy, sz, tx, ty, tz;

	cos = tgCos(ang);
	sin = tgSin(ang);
	t = 1.f - cos;
	sx = sin * pAxis->x;
	sy = sin * pAxis->y;
	sz = sin * pAxis->z;
	tx = t * pAxis->x;
	ty = t * pAxis->y;
	tz = t * pAxis->z;

	pDest->m[0][0] = tx * pAxis->x + cos;
	pDest->m[0][1] = tx * pAxis->y + sz;
	pDest->m[0][2] = tx * pAxis->z - sy;
	pDest->m[0][3] = 0.f;

	pDest->m[1][0] = tx * pAxis->y - sz;
	pDest->m[1][1] = ty * pAxis->y + cos;
	pDest->m[1][2] = ty * pAxis->z + sx;
	pDest->m[1][3] = 0.f;
	
	pDest->m[2][0] = tx * pAxis->z + sy;
	pDest->m[2][1] = ty * pAxis->z - sx;
	pDest->m[2][2] = tz * pAxis->z + cos;
	pDest->m[2][3] = 0.f;

	pDest->m[3][0] = 0.f;
	pDest->m[3][1] = 0.f;
	pDest->m[3][2] = 0.f;
	pDest->m[3][3] = 1.f;
}

//-----------------------------------------------------------------------------
// tgMatrixScaleF
// Description: create scale matrix
// Params: 	pDest - destination matrix pointer
//			x - scale factor in x
//			y - scale factor in y
//			z - scale factor in z
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixScaleF(TgMatrix *pDest, f32 x, f32 y, f32 z)
{
	tgMatrixSetIdentity(pDest);

	pDest->m[0][0] = x;
	pDest->m[1][1] = y;
	pDest->m[2][2] = z;
}

//-----------------------------------------------------------------------------
// tgMatrixScaleV
// Description: create scale matrix
// Params: 	pDest - destination matrix pointer
//			pSrc - scale vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixScaleV(TgMatrix *pDest, const TgVector *pSrc)
{
	tgMatrixSetIdentity(pDest);

	pDest->m[0][0] = pSrc->x;
	pDest->m[1][1] = pSrc->y;
	pDest->m[2][2] = pSrc->z;
}

//-----------------------------------------------------------------------------
// tgMatrixLookAt
// Description: create look-at camera matrix
// Params: 	pDest - destination matrix pointer
//			pCenter - center position
//			pEye - eye position
//			pUp - up vector
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixLookAt(TgMatrix *pDest, const TgVector *pCenter, const TgVector *pEye,
					const TgVector *pUp)
{
	TgVector	F, Up, s, u;
	TgMatrix	t;

	tgVectorSub(&F, pCenter, pEye);
	tgVectorNormalize(&F, &F);

	tgVectorNormalize(&Up, pUp);

	tgVectorCross(&s, &F, &Up);
	tgVectorNormalize(&s, &s);

	tgVectorCross(&u, &s, &F);
	tgVectorNormalize(&u, &u);

	pDest->m[0][0] = s.x;
	pDest->m[0][1] = u.x;
	pDest->m[0][2] = -F.x;
	pDest->m[0][3] = 0.f;

	pDest->m[1][0] = s.y;
	pDest->m[1][1] = u.y;
	pDest->m[1][2] = -F.y;
	pDest->m[1][3] = 0.f;

	pDest->m[2][0] = s.z;
	pDest->m[2][1] = u.z;
	pDest->m[2][2] = -F.z;
	pDest->m[2][3] = 0.f;

	pDest->m[3][0] = 0.f;
	pDest->m[3][1] = 0.f;
	pDest->m[3][2] = 0.f;
	pDest->m[3][3] = 1.f;

	tgMatrixTranslateF(&t, -pEye->x, -pEye->y, -pEye->z);
	tgMatrixMulMatrix(pDest, pDest, &t);
}

//-----------------------------------------------------------------------------
// tgMatrixInverse
// Description: inverse matix
// Params: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixInverse(TgMatrix *pDest, const TgMatrix *pSrc)
{
	f32		det;			// determinant
	f32		tmp[12];		// temp array for pairs
	TgMatrix	transpose;	// transposed source matrix
	s32			i, j;

	tgMatrixTranspose(&transpose, pSrc);

	// calculate pairs for first 8 elements (cofactors)
	tmp[0] = transpose.m[2][2] * transpose.m[3][3];
	tmp[1] = transpose.m[2][3] * transpose.m[3][2];
	tmp[2] = transpose.m[2][1] * transpose.m[3][3];
	tmp[3] = transpose.m[2][3] * transpose.m[3][1];
	tmp[4] = transpose.m[2][1] * transpose.m[3][2];
	tmp[5] = transpose.m[2][2] * transpose.m[3][1];
	tmp[6] = transpose.m[2][0] * transpose.m[3][3];
	tmp[7] = transpose.m[2][3] * transpose.m[3][0];
	tmp[8] = transpose.m[2][0] * transpose.m[3][2];
	tmp[9] = transpose.m[2][2] * transpose.m[3][0];
	tmp[10] = transpose.m[2][0] * transpose.m[3][1];
	tmp[11] = transpose.m[2][1] * transpose.m[3][0];

	// calculate first 8 elements (cofactors)
	pDest->m[0][0]  = tmp[0] * transpose.m[1][1] + tmp[3] * transpose.m[1][2] + tmp[4] * transpose.m[1][3];
	pDest->m[0][0] -= tmp[1] * transpose.m[1][1] + tmp[2] * transpose.m[1][2] + tmp[5] * transpose.m[1][3];
	pDest->m[0][1]  = tmp[1] * transpose.m[1][0] + tmp[6] * transpose.m[1][2] + tmp[9] * transpose.m[1][3];
	pDest->m[0][1] -= tmp[0] * transpose.m[1][0] + tmp[7] * transpose.m[1][2] + tmp[8] * transpose.m[1][3];
	pDest->m[0][2]  = tmp[2] * transpose.m[1][0] + tmp[7] * transpose.m[1][1] + tmp[10] * transpose.m[1][3];
	pDest->m[0][2] -= tmp[3] * transpose.m[1][0] + tmp[6] * transpose.m[1][1] + tmp[11] * transpose.m[1][3];
	pDest->m[0][3]  = tmp[5] * transpose.m[1][0] + tmp[8] * transpose.m[1][1] + tmp[11] * transpose.m[1][2];
	pDest->m[0][3] -= tmp[4] * transpose.m[1][0] + tmp[9] * transpose.m[1][1] + tmp[10] * transpose.m[1][2];
	pDest->m[1][0]  = tmp[1] * transpose.m[0][1] + tmp[2] * transpose.m[0][2] + tmp[5] * transpose.m[0][3];
	pDest->m[1][0] -= tmp[0] * transpose.m[0][1] + tmp[3] * transpose.m[0][2] + tmp[4] * transpose.m[0][3];
	pDest->m[1][1]  = tmp[0] * transpose.m[0][0] + tmp[7] * transpose.m[0][2] + tmp[8] * transpose.m[0][3];
	pDest->m[1][1] -= tmp[1] * transpose.m[0][0] + tmp[6] * transpose.m[0][2] + tmp[9] * transpose.m[0][3];
	pDest->m[1][2]  = tmp[3] * transpose.m[0][0] + tmp[6] * transpose.m[0][1] + tmp[11] * transpose.m[0][3];
	pDest->m[1][2] -= tmp[2] * transpose.m[0][0] + tmp[7] * transpose.m[0][1] + tmp[10] * transpose.m[0][3];
	pDest->m[1][3]  = tmp[4] * transpose.m[0][0] + tmp[9] * transpose.m[0][1] + tmp[10] * transpose.m[0][2];
	pDest->m[1][3] -= tmp[5] * transpose.m[0][0] + tmp[8] * transpose.m[0][1] + tmp[11] * transpose.m[0][2];

	// calculate pairs for second 8 elements (cofactors)
	tmp[0] = transpose.m[0][2] * transpose.m[1][3];
	tmp[1] = transpose.m[0][3] * transpose.m[1][2];
	tmp[2] = transpose.m[0][1] * transpose.m[1][3];
	tmp[3] = transpose.m[0][3] * transpose.m[1][1];
	tmp[4] = transpose.m[0][1] * transpose.m[1][2];
	tmp[5] = transpose.m[0][2] * transpose.m[1][1];
	tmp[6] = transpose.m[0][0] * transpose.m[1][3];
	tmp[7] = transpose.m[0][3] * transpose.m[1][0];
	tmp[8] = transpose.m[0][0] * transpose.m[1][2];
	tmp[9] = transpose.m[0][2] * transpose.m[1][0];
	tmp[10] = transpose.m[0][0] * transpose.m[1][1];
	tmp[11] = transpose.m[0][1] * transpose.m[1][0];

	// calculate second 8 elements (cofactors)
    pDest->m[2][0]  = tmp[0] * transpose.m[3][1] + tmp[3] * transpose.m[3][2] + tmp[4] * transpose.m[3][3];
	pDest->m[2][0] -= tmp[1] * transpose.m[3][1] + tmp[2] * transpose.m[3][2] + tmp[5] * transpose.m[3][3];
	pDest->m[2][1]  = tmp[1] * transpose.m[3][0] + tmp[6] * transpose.m[3][2] + tmp[9] * transpose.m[3][3];
	pDest->m[2][1] -= tmp[0] * transpose.m[3][0] + tmp[7] * transpose.m[3][2] + tmp[8] * transpose.m[3][3];
	pDest->m[2][2]  = tmp[2] * transpose.m[3][0] + tmp[7] * transpose.m[3][1] + tmp[10] * transpose.m[3][3];
	pDest->m[2][2] -= tmp[3] * transpose.m[3][0] + tmp[6] * transpose.m[3][1] + tmp[11] * transpose.m[3][3];
	pDest->m[2][3]  = tmp[5] * transpose.m[3][0] + tmp[8] * transpose.m[3][1] + tmp[11] * transpose.m[3][2];
	pDest->m[2][3] -= tmp[4] * transpose.m[3][0] + tmp[9] * transpose.m[3][1] + tmp[10] * transpose.m[3][2];
	pDest->m[3][0]  = tmp[2] * transpose.m[2][2] + tmp[5] * transpose.m[2][3] + tmp[1] * transpose.m[2][1];
	pDest->m[3][0] -= tmp[4] * transpose.m[2][3] + tmp[0] * transpose.m[2][1] + tmp[3] * transpose.m[2][2];
	pDest->m[3][1]  = tmp[8] * transpose.m[2][3] + tmp[0] * transpose.m[2][0] + tmp[7] * transpose.m[2][2];
	pDest->m[3][1] -= tmp[6] * transpose.m[2][2] + tmp[9] * transpose.m[2][3] + tmp[1] * transpose.m[2][0];
	pDest->m[3][2]  = tmp[6] * transpose.m[2][1] + tmp[11] * transpose.m[2][3] + tmp[3] * transpose.m[2][0];
	pDest->m[3][2] -= tmp[10] * transpose.m[2][3] + tmp[2] * transpose.m[2][0] + tmp[7] * transpose.m[2][1];
	pDest->m[3][3]  = tmp[10] * transpose.m[2][2] + tmp[4] * transpose.m[2][0] + tmp[9] * transpose.m[2][1];
	pDest->m[3][3] -= tmp[8] * transpose.m[2][1] + tmp[11] * transpose.m[2][2] + tmp[5] * transpose.m[2][0];

	// calculate determinant
	det = 1.f / (transpose.m[0][0] * pDest->m[0][0] + transpose.m[0][1] * pDest->m[0][1] + transpose.m[0][2] * pDest->m[0][2] + transpose.m[0][3] * pDest->m[0][3]);

	// calculate matrix inverse
	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j++)
			pDest->m[i][j] *= det;
}

//-----------------------------------------------------------------------------
// tgMatrixTranspose
// Description: transpose matix
// Params: 	pDest - destination matrix pointer
//			pSrc - source matrix pointer
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixTranspose(TgMatrix *pDest, const TgMatrix *pSrc)
{
	TgMatrix	tmp;
	s32			i, j;

	tgMatrixCopy(&tmp, pSrc);

	for(i = 0; i < 4; i ++)
		for(j = 0; j < 4; j ++)
			pDest->m[i][j] = tmp.m[j][i];
}

//-----------------------------------------------------------------------------
// tgMatrixOrtho
// Description: create orthogonal viewing matrix
// Params: 	pDest - destination matrix pointer
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
						 f32 top, f32 nearz, f32 farz)
{
	pDest->m[0][0] = 2.f / (right - left);
	pDest->m[0][1] = 0.f;
	pDest->m[0][2] = 0.f;
	pDest->m[0][3] = 0.f;

	pDest->m[1][0] = 0.f;
	pDest->m[1][1] = 2.f / (top - bottom);
	pDest->m[1][2] = 0.f;
	pDest->m[1][3] = 0.f;

	pDest->m[2][0] = 0.f;
	pDest->m[2][1] = 0.f;
	pDest->m[2][2] = -2.f / (farz - nearz);
	pDest->m[2][3] = 0.f;

	pDest->m[3][0] = (right + left) / (right - left);
	pDest->m[3][1] = (top + bottom) / (top - bottom);
	pDest->m[3][2] = (farz + nearz) / (farz - nearz);
	pDest->m[3][3] = 1.f;
}

//-----------------------------------------------------------------------------
// tgMatrixPerspective
// Description: create perspective viewing matrix
// Params: 	pDest - destination matrix pointer
//			fov - field of view in radians
//			aspect - aspect ratio
//			nearz - near co-ordinate
//			farz - far co-ordinate
// Returns: 
// Note: 
//-----------------------------------------------------------------------------
void tgMatrixPerspective(TgMatrix *pDest, f32 fov, f32 aspect,
						 f32 nearz, f32 farz)
{
	f32 c, s, halfFov;

	halfFov = fov * 0.5f;
	
	c = tgCos(halfFov);
	s = tgSin(halfFov);

	pDest->m[0][0] = (c / s) / aspect;
	pDest->m[0][1] = 0.f;
	pDest->m[0][2] = 0.f;
	pDest->m[0][3] = 0.f;

	pDest->m[1][0] = 0.f;
	pDest->m[1][1] = (c / s);
	pDest->m[1][2] = 0.f;
	pDest->m[1][3] = 0.f;

	pDest->m[2][0] = 0.f;
	pDest->m[2][1] = 0.f;
	pDest->m[2][2] = -(farz + nearz) / (farz - nearz);
	pDest->m[2][3] = -1.f;

	pDest->m[3][0] = 0.f;
	pDest->m[3][1] = 0.f;
	pDest->m[3][2] = (-2.f * farz * nearz) / (farz - nearz);
	pDest->m[3][3] = 0.f;
}
//DOM-IGNORE-END
