/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgCamera.h>

hkgCamera* (*hkgCamera::create)();

float hkgCamera::DEFAULT_FROM[3] = {4,10,2};
float hkgCamera::DEFAULT_TO[3] = {0,0,0};
float hkgCamera::DEFAULT_UP[3] = {0,0,1};
float hkgCamera::DEFAULT_FOV = 45.0f;

const float g_sourceFrustumPlanePntRight[8][4] = 
{
/*0*/{ -1.0f, -1.0f,  -1.0f, 1.0f}, // xyzw
/*1*/{  1.0f, -1.0f,  -1.0f, 1.0f}, // Xyzw
/*2*/{ -1.0f,  1.0f,  -1.0f, 1.0f}, // xYzw
/*3*/{  1.0f,  1.0f,  -1.0f, 1.0f}, // XYzw
/*4*/{ -1.0f, -1.0f,  1.0f, 1.0f}, // xyZw
/*5*/{  1.0f, -1.0f,  1.0f, 1.0f}, // XyZw
/*6*/{ -1.0f,  1.0f,  1.0f, 1.0f}, // xYZw
/*7*/{  1.0f,  1.0f,  1.0f, 1.0f} // XYZw
};

const float g_sourceFrustumPlanePntLeft[8][4] = 
{
/*0*/{ -1.0f, -1.0f,  1.0f, 1.0f}, // xyzw
/*1*/{  1.0f, -1.0f,  1.0f, 1.0f}, // Xyzw
/*2*/{ -1.0f,  1.0f,  1.0f, 1.0f}, // xYzw
/*3*/{  1.0f,  1.0f,  1.0f, 1.0f}, // XYzw
/*4*/{ -1.0f, -1.0f,  -1.0f, 1.0f}, // xyZw
/*5*/{  1.0f, -1.0f,  -1.0f, 1.0f}, // XyZw
/*6*/{ -1.0f,  1.0f,  -1.0f, 1.0f}, // xYZw
/*7*/{  1.0f,  1.0f,  -1.0f, 1.0f} // XYZw
};


hkgCamera::~hkgCamera()
{

}

void hkgCamera::pan(float x_amount, float y_amount, bool move_focus)
{
	float dir[3];
	hkgVec3Sub( dir, m_from, m_to );
	float len2 = hkgVec3Length(dir);

	float move[3];
	move[0] = 0.0f;
	move[1] = 0.0f;
	move[2] = 0.0f;

	if (x_amount != 0.0f) 
	{
		float right[3];
		getRight( right );
		hkgVec3Mult( move, right, len2 * x_amount );
	}
	if (y_amount != 0.0f) 
	{
		hkgVec3MultAdd( move, m_up, len2 * y_amount );
	}

	hkgVec3Add( m_from, m_from, move );

	if (move_focus)
		hkgVec3Add( m_to, m_to, move);

	computeModelView();
	if (m_mode == HKG_CAMERA_COMPUTE_ORTHOGRAPHIC)
		computeProjection();
}

void hkgCamera::dolly(float z_amount, bool move_focus )
{
	float dir[3];
	
	hkgVec3Sub( dir, m_from, m_to );
	hkgVec3Mult( dir, dir, z_amount );
	hkgVec3Add( m_from, dir, m_from );

	if (move_focus)
		hkgVec3Add( m_to, dir, m_to );

	computeModelView();
	if (m_mode == HKG_CAMERA_COMPUTE_ORTHOGRAPHIC)
		computeProjection();
}

void hkgCamera::rotateAboutTo(float angle, const float axis[3], bool upLock)
{
	float r[4];
	hkgQuatFromAngleAxis( r, angle, axis );

	float dir[3];

	hkgVec3Sub( dir, m_from, m_to );
	hkgQuatRotateVec3( dir, r, dir );
	
	if (!upLock)
		hkgQuatRotateVec3( m_up, r, m_up );

	hkgVec3Add( m_from, m_to, dir );

	computeModelView();
	if (m_mode == HKG_CAMERA_COMPUTE_ORTHOGRAPHIC)
		computeProjection();
}

void hkgCamera::rotateAboutFrom(float angle, const float axis[3], bool upLock)
{

	float r[4];
	hkgQuatFromAngleAxis(r, angle, axis);
	
	float dir[3];
	hkgVec3Sub( dir, m_to, m_from );

	hkgQuatRotateVec3( dir, r, dir );

	if (!upLock)
		hkgQuatRotateVec3( m_up, r, m_up );

	hkgVec3Add( m_to, m_from, dir );

	computeModelView();
	if (m_mode == HKG_CAMERA_COMPUTE_ORTHOGRAPHIC)
		computeProjection();
}


static void _trackballPointToVector(int x, int y, 
								 unsigned int w, unsigned int h, 
								 float v[3], bool leftHanded)
{

  // project x, y onto a hemi-sphere centered within width, height. 
  v[0] = (2.0f * x - w) / w;
  v[1] = (2.0f * y - h) / h;
  
  const float d = hkg_sqrt(v[0] * v[0] + v[1] * v[1]);
  
  v[2] = hkg_cos((HKG_PI / 2.0f) * ((d < 1.0f) ? d : 1.0f));
  
  if (leftHanded)
	  v[2] *= -1;
    
  hkgVec3Normalize(v);

}

static void _trackballAxisAngle(int x, int y, 
					unsigned int width, unsigned int height,
					float lastPos[3], float axis[3], float &angle, bool leftHanded)
{
  float curPos[3];
  float d[3];

  _trackballPointToVector( x, y, width, height, curPos, leftHanded);
  
  hkgVec3Sub( d, curPos, lastPos );

  angle = ( HKG_PI * 0.5f ) * hkgVec3Length(d); 

 // if (leftHanded)
//	hkgVec3Cross( axis, curPos, lastPos );
 // else
	hkgVec3Cross( axis, lastPos, curPos );

  hkgVec3Copy( lastPos, curPos );
}

void hkgCamera::startTrackball( unsigned int winWidth, unsigned int winHeight, int mouseX, int mouseY )
{
	_trackballPointToVector(mouseX, mouseY, winWidth, winHeight, m_trackballVec, m_handedness == HKG_CAMERA_HANDEDNESS_LEFT);
}


void hkgCamera::moveTrackball( unsigned int winWidth, unsigned int winHeight, int mouseX, int mouseY )
{
	float axis[3];
	float angle;

	_trackballAxisAngle(mouseX, mouseY, winWidth, winHeight, m_trackballVec, axis, angle, m_handedness == HKG_CAMERA_HANDEDNESS_LEFT);

	float temp[16]; 

	// as we just use the rotational part of the view mat, and we know 
	// it is orthoganal, we can just use the transpose as the inverse.
	hkgMat4Transpose( temp, m_viewMat );	

	hkgVec3Rotate( axis, temp );
	hkgVec3Normalize( axis );
	
	rotateAboutTo(-angle, axis, false);
}

void hkgCamera::computePerspective( float _fov, float aspect, float znear, float zfar )
{
	HK_ASSERT( (znear >= 0) && (zfar >= 0) && "Negative plane distances arte not allowed (GameCube restricition)");
	
	const float fov = _fov * 3.141592654f / 180.0f;
	const float h = 2.0f * znear * hkg_tan( fov/2.0f );
	const float w = h * aspect;
	
	const float l = -w/2.0f;
	const float r = w/2.0f;
	const float b = -h/2.0f;
	const float t = h/2.0f;
	const float n = znear;
	const float f = zfar;

	const float A = ( r + l ) / ( r - l );
	const float B = ( t + b ) / ( t - b );
	const float C = -( f + n ) / ( f - n );
	const float D = -( 2*f*n ) / ( f - n );
	const float E = (2*n) / ( r - l);
	const float F = (2*n) / ( t - b);

	hkgMat4Zero( m_projMat );

	m_projMat[0] = E;
	m_projMat[5] = F;
	
	if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
	{
		m_projMat[8] = A;
		m_projMat[9] = B;
		m_projMat[10] = C;
	    m_projMat[11] = -1;
	}
	else
	{
		m_projMat[8] = -A;
		m_projMat[9] = -B;
		m_projMat[10] = -C;
		m_projMat[11] = 1;
	}

	m_projMat[14] = D;

	m_camInvDirty = true;
	m_frustumPlanesDirty = true;
	
	m_near = znear;
	m_far = zfar;
	m_aspect = aspect;
	m_fov = _fov;

	m_mode = HKG_CAMERA_PERSPECTIVE;
}

void hkgCamera::computeOrtho( float left, float right, float bottom, float top, float znear, float zfar)
{
	HK_ASSERT( (znear >= 0) && (zfar >= 0) && "Negative plane distances arte not allowed (GameCube restricition)");
	
	const float rl = right - left;
	const float tb = top - bottom;
	const float fn = zfar - znear;

	const float A = 2.0f / rl;
	const float B = 2.0f / tb;
	const float C = -2.0f / fn;

	const float D =  -(right + left) / rl;
	const float E =  -(top + bottom) / tb;
	const float F =  -(zfar + znear) / fn;
	
	hkgMat4Zero( m_projMat );

	m_projMat[0] = A;
	m_projMat[5] = B;
	
	if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
	{
		m_projMat[10] = C;
	}
	else
	{
		m_projMat[10] = -C;
	}

	m_projMat[12] = D;
	m_projMat[13] = E;
	m_projMat[14] = F;
	m_projMat[15] = 1;

	m_camInvDirty = true;
	m_frustumPlanesDirty = true;
	
	m_onear = znear;
	m_ofar = zfar;
	m_oleft = left;
	m_oright = right;
	m_otop = top;
	m_obottom = bottom;

	if (m_mode == HKG_CAMERA_PERSPECTIVE) // as can be computeortho too
		m_mode = HKG_CAMERA_ORTHOGRAPHIC; 
}

void hkgCamera::computeModelView(bool forceIdentity)
{
#if defined(HK_COMPILER_MSVC)
	// msvc miscompiles this code in release
	__asm { emms }
#endif

	m_camInvDirty = true;
	m_frustumPlanesDirty = true;
	
	if (forceIdentity)
	{
		hkgMat4Identity(m_viewMat);
		return;
	}

	orthogonalize();

	float eye[3];
	hkgVec3Copy(eye, m_from);

	const float* cent = m_to;

	float f[3];

	if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
		hkgVec3Sub( f, cent, eye );
	else
		hkgVec3Sub( f, eye, cent );

	hkgVec3Normalize( f );
	hkgVec3Normalize( m_up );

	float s[3];
	float u[3];

	hkgVec3Cross( s, f, m_up );
	hkgVec3Cross( u, s, f );

	m_viewMat[0] = s[0];  m_viewMat[4] = s[1];  m_viewMat[8] = s[2]; 
	m_viewMat[1] = u[0];  m_viewMat[5] = u[1];  m_viewMat[9] = u[2];	
	m_viewMat[2] = -f[0]; m_viewMat[6] = -f[1]; m_viewMat[10] = -f[2];

	m_viewMat[3] = 0.0f; m_viewMat[7] = 0.0f; m_viewMat[11] = 0.0f;
	
	// this is then the outcome of doing a post translate of -eye :
	hkgVec3Scale( eye, -1 );

	m_viewMat[12] = (m_viewMat[0]*eye[0]) + (m_viewMat[4]*eye[1]) + (m_viewMat[8] *eye[2]);
	m_viewMat[13] = (m_viewMat[1]*eye[0]) + (m_viewMat[5]*eye[1]) + (m_viewMat[9] *eye[2]);
	m_viewMat[14] = (m_viewMat[2]*eye[0]) + (m_viewMat[6]*eye[1]) + (m_viewMat[10]*eye[2]);

	m_viewMat[15] = 1.0f;
}

void hkgCamera::computeOrthoParams()
{
	float pov[3];

	orthogonalize();

	hkgVec3Sub( pov, m_from, m_to);

	const float fov = m_fov * HKG_PI / 180.0f;
	const float dist = hkgVec3Length( pov );

	const float h = 2.0f * dist * hkg_tan( fov/2.0f );
	const float w = h * m_aspect;
	
	m_oleft		= -w/2.0f;
	m_oright	= w/2.0f;
	m_obottom	= -h/2.0f;
	m_otop		= h/2.0f;
	m_onear		= m_near;
	m_ofar		= m_far;
}

void hkgCamera::computeCameraInverse() const
{
	// have to invert (model * proj)

	float viewInv[16];
	float projInv[16];

	computeInverseProjectionMatrix( projInv );
	computeInverseViewMatrix( viewInv );

	hkgMat4Mult(m_camInv, viewInv, projInv );
	m_camInvDirty = false;
}	

void hkgCamera::unProject( int x, int y, float depth, int winWidth, int winHeight, float result[3] ) const
{
	float cs[4];
	cs[0] = ((2 * x)/(float)winWidth) - 1.0f;
	cs[1] = ((2 * y)/(float)winHeight) - 1.0f;
	cs[2] = (2*depth) - 1.0f;
	cs[3] = 1.0f;

	if (m_camInvDirty)
		computeCameraInverse();

	float ws[4];
	hkgVec4Transform( ws, cs, m_camInv );

	result[0] = ws[0]/ws[3];
	result[1] = ws[1]/ws[3];
	result[2] = ws[2]/ws[3];

}

void hkgCamera::project( float x, float y, float z, int winWidth, int winHeight, float result[3] ) const
{
	float ws[4];
	ws[0] = x;
	ws[1] = y;
	ws[2] = z;
	ws[3] = 1.0f;

	float w2c[16];
	hkgMat4Mult( w2c, m_projMat, m_viewMat);

	float cs[4];
	hkgVec4Transform( cs, ws, w2c );

	result[0] = cs[0]/cs[3];
	result[1] = cs[1]/cs[3];
	result[2] = cs[2]/cs[3];
	
	// convert:

	const float one[] = {1.0f, 1.0f, 1.0f};
	hkgVec3Add( result, one );
	hkgVec3Scale( result, 0.5f );

	// ranges now in 0-->1 for window,
	// want to convert x,y into window coords

	result[0] *= winWidth;
	result[1] *= winHeight;
}

void hkgCamera::unProjectExact( int x, int y, float depth, int winWidth, int winHeight,  float result[3] ) const
{
	double cs[4];
	cs[0] = ((2 * x)/(double)winWidth) - 1.0f;
	cs[1] = ((2 * y)/(double)winHeight) - 1.0f;
	cs[2] = (2*(double)depth) - 1.0f;
	cs[3] = 1.0f;

	// have to invert (model * proj)

	double viewInvD[16];
	double projInvD[16];

	float viewInvF[16];
	float projInvF[16];
	
	double inv[16];

	computeInverseProjectionMatrix( projInvF );
	computeInverseViewMatrix( viewInvF );
	
	hkgMat4FloatToDouble( projInvD, projInvF );
	hkgMat4FloatToDouble( viewInvD, viewInvF );
	
	hkgMat4MultD(inv, viewInvD, projInvD );

	double ws[4];
	hkgVec4TransformD( ws, cs, inv );

	result[0] = static_cast<float>( ws[0]/ws[3] );
	result[1] = static_cast<float>( ws[1]/ws[3] );
	result[2] = static_cast<float>( ws[2]/ws[3] );
}


/*
#define PRINTF_MAT4(x) printf(	"\t| %.6f %.6f %.6f %.6f |\n"\
								"\t| %.6f %.6f %.6f %.6f |\n"\
								"\t| %.6f %.6f %.6f %.6f |\n"\
		 						"\t| %.6f %.6f %.6f %.6f |\n\n", \
								(x)[0],(x)[4],(x)[8],(x)[12], \
								(x)[1],(x)[5],(x)[9],(x)[13], \
								(x)[2],(x)[6],(x)[10],(x)[14], \
								(x)[3],(x)[7],(x)[11],(x)[15] );
*/

void hkgCamera::computeInverseProjectionMatrix( float inv[16] ) const
{

	// to save having to do a 4x4 matrix inversion, we
	// can just compute directly the two inverses and mult.

	hkgMat4Zero( inv );

	//ORTHO:
	switch (m_mode)
	{
	case HKG_CAMERA_PERSPECTIVE: 
		{
			const float fov = m_fov * HKG_PI / 180.0f;
			const float h = 2.0f * m_near * hkg_tan( fov/2.0f );
			const float w = h * m_aspect;
			
			const float l = -w/2.0f;
			const float r = w/2.0f;
			const float b = -h/2.0f;
			const float t = h/2.0f;
			const float n = m_near;
			const float f = m_far;

			const float A = (r - l) / (2*n);
			const float B = (t - b) / (2*n);
			const float C = -(f - n) / (2*f*n);

			const float D = (r + l) / (2*n);
			const float E = (t + b) / (2*n);
			const float F = (f + n) / (2*f*n);

			inv[0] = A;
			inv[5] = B;
			inv[11] = C;

			inv[12] = D;
			inv[13] = E;

			if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
				inv[14] = -1;
			else
				inv[14] = 1;

			inv[15] = F;
		}
		break;


	case HKG_CAMERA_COMPUTE_ORTHOGRAPHIC: 
		// no break;
	case HKG_CAMERA_ORTHOGRAPHIC: 
	
		{
			const float A = (m_oright - m_oleft) / 2.0f;
			const float B = (m_otop - m_obottom)/ 2.0f;
			const float C = (m_ofar - m_onear) / -2.0f;

			const float D = (m_oright + m_oleft) / 2.0f;
			const float E = (m_otop + m_obottom)/ 2.0f;
			const float F = (m_ofar + m_onear) / -2.0f;

			inv[0] = A;
			inv[5] = B; 

			if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
			{
				inv[10] = C;
				inv[14] = F;
			}			
			else
			{
				inv[10] = -C;	
				inv[14] = -F;
			}

			inv[12] = D;
			inv[13] = E;
			inv[15] = 1;

		}
		break;
	}
}



void hkgCamera::computeInverseViewMatrix( float inv[16] ) const
{
	// Model view is easy to invert as it is orthonormal in our system
	// Transpose the initial rotation and translate by the inverse of the original translation:

	inv[0] = m_viewMat[0];
	inv[1] = m_viewMat[4];
	inv[2] = m_viewMat[8];
	inv[3] = 0;

	inv[4] = m_viewMat[1];
	inv[5] = m_viewMat[5];
	inv[6] = m_viewMat[9];
	inv[7] = 0;

	inv[8] = m_viewMat[2];
	inv[9] = m_viewMat[6];
	inv[10] = m_viewMat[10];
	inv[11] = 0;


	// For [12]:
	// --> viewInv[12]*s[0] + viewInv[13]*s[1] + viewInv[14]*s[2] + 1*m_viewMat[12] == 0
	// where m_viewMat[12], == s[0]e[0] + s[1]e[1] + s[2]e[2]
	// so viewInv[12] = -e[0], viewInv[13] == -e[1], etc
	// and e == -m_from, 
	
	inv[12] = m_from[0];
	inv[13] = m_from[1];
	inv[14] = m_from[2];

	inv[15] = 1.0f;
}




void hkgCamera::computeFrustumPlanes() const
{
	if (m_camInvDirty)
		computeCameraInverse();
	
	float transformedFrustumPlanePnt[8][4];

//	printf(" Computed points:\n");
    for( int i = 0; i < 8; i++ )
	{
        if (m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
			hkgVec4Transform( transformedFrustumPlanePnt[i], g_sourceFrustumPlanePntRight[i], m_camInv);
		else
			hkgVec4Transform( transformedFrustumPlanePnt[i], g_sourceFrustumPlanePntLeft[i], m_camInv);
		
		hkgVec3Div( transformedFrustumPlanePnt[i], transformedFrustumPlanePnt[i][HKG_W]); // rescale
//		printf(" [%.3f %.3f %.3f]\n", transformedFrustumPlanePnt[i][0],transformedFrustumPlanePnt[i][1], transformedFrustumPlanePnt[i][2]);
	}

    hkgPlaneFrom3Points( m_frustumPlanes[0], transformedFrustumPlanePnt[2], 
        transformedFrustumPlanePnt[3], transformedFrustumPlanePnt[6] ); // Top

    hkgPlaneFrom3Points( m_frustumPlanes[1],transformedFrustumPlanePnt[1], 
        transformedFrustumPlanePnt[0], transformedFrustumPlanePnt[4] ); // Bottom

	hkgPlaneFrom3Points( m_frustumPlanes[2], transformedFrustumPlanePnt[2], 
		transformedFrustumPlanePnt[6], transformedFrustumPlanePnt[4] ); // Left

	hkgPlaneFrom3Points( m_frustumPlanes[3], transformedFrustumPlanePnt[7], 
		transformedFrustumPlanePnt[3], transformedFrustumPlanePnt[5] ); // Right

	hkgPlaneFrom3Points( m_frustumPlanes[4], transformedFrustumPlanePnt[0], 
        transformedFrustumPlanePnt[1], transformedFrustumPlanePnt[2] ); // Near

    hkgPlaneFrom3Points( m_frustumPlanes[5], transformedFrustumPlanePnt[6], 
        transformedFrustumPlanePnt[7], transformedFrustumPlanePnt[5] ); // Far

//	printf("Computed planes :\n");
//	for (int j =0; j < 6; ++j)
//		printf("[%.3f %.3f %.3f %.3f]\n", m_frustumPlanes[j][0],m_frustumPlanes[j][1],m_frustumPlanes[j][2],m_frustumPlanes[j][3]);
		
	
	m_frustumPlanesDirty = false;
}



/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
