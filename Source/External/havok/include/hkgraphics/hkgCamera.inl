/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkgCamera::getFrom(float f[3]) const
{ 
	hkString::memCpy( f, m_from, sizeof(float)*3 );	
}

inline void hkgCamera::setFrom(const float f[3])
{
	hkString::memCpy( m_from, f, sizeof(float)*3 );		
}

inline void hkgCamera::getTo(float t[3]) const					
{ 
	hkString::memCpy( t, m_to, sizeof(float)*3 );		
}

inline void hkgCamera::setTo(const float t[3])	
{ 
	hkString::memCpy( m_to, t, sizeof(float)*3 );					
}

inline void hkgCamera::getUp(float u[3]) const					
{ 
	hkString::memCpy( u, m_up, sizeof(float)*3 );				
}
	
inline void hkgCamera::setUp(const float u[3])	
{ 
	hkString::memCpy( m_up, u, sizeof(float)*3);			
}

inline void hkgCamera::getDir(float dir[3]) const
{
	hkgVec3Sub( dir, m_to, m_from);
	hkgVec3Normalize( dir );
}

inline void hkgCamera::getRight(float r[3]) const
{
	float d[3];
	getDir(d);

	if ( m_handedness == HKG_CAMERA_HANDEDNESS_RIGHT)
		hkgVec3Cross( r, d, m_up );
	else //  m_handedness == HKG_CAMERA_HANDEDNESS_LEFT
		hkgVec3Cross( r, m_up, d );
}

inline float hkgCamera::getFOV() const				
{ 
	return m_fov;		
}

inline void hkgCamera::setFOV(float f)			
{ 
	m_fov = f;		
}

inline float hkgCamera::getNear() const				
{ 
	return m_near;
}

inline void hkgCamera::setNear(float f)		
{ 
	HK_ASSERT( f >= 0.0f && "Negative near plane not allowed (GameCube restriction)");
	m_near = f;	
}

inline float hkgCamera::getFar() const				
{ 
	return m_far;
}

inline void hkgCamera::setFar(float f)			
{ 
	HK_ASSERT( f >= 0.0f && "Negative far plane not allowed (GameCube restriction)");
	m_far = f;		
}

inline float hkgCamera::getAspect() const				
{ 
	return m_aspect;	
}

inline void hkgCamera::setAspect(float f)		
{ 
	m_aspect = f;		
}

inline float* hkgCamera::getUpPtr()
{
	return m_up;
}

inline float* hkgCamera::getToPtr()
{
	return m_to;
}

inline float* hkgCamera::getFromPtr()
{
	return m_from;
}

inline float	hkgCamera::getOrthoLeft() const
{	
	return m_oleft;
}

inline float	hkgCamera::getOrthoRight() const
{
	return m_oright;
}

inline float	hkgCamera::getOrthoTop() const
{
	return m_otop;
}

inline float	hkgCamera::getOrthoBottom() const
{
	return m_obottom;	
}

inline float	hkgCamera::getOrthoNear() const
{
	return m_onear;
}

inline float	hkgCamera::getOrthoFar() const
{
	return m_ofar;
}

inline void		hkgCamera::setOrthoLeft(float l)
{
	m_oleft = l;
}

inline void		hkgCamera::setOrthoRight(float r)
{
	m_oright = r;
}

inline void		hkgCamera::setOrthoTop(float t)
{
	m_otop = t;
}

inline void 	hkgCamera::setOrthoBottom(float b)
{
	m_obottom = b;	
}

inline void		hkgCamera::setOrthoNear(float n)
{
	HK_ASSERT( n >= 0.0f && "Negative near plane not allowed (GameCube restriction)");
	m_onear = n;
}

inline void		hkgCamera::setOrthoFar(float f)
{
	HK_ASSERT( f >= 0.0f && "Negative far plane not allowed (GameCube restriction)");
	m_ofar = f;	
}

inline float* hkgCamera::getProjectionMatrix()
{
	return m_projMat;
}

inline float* hkgCamera::getViewMatrix()
{
	return m_viewMat;
}

inline float* hkgCamera::getCameraInverseMatrix()
{
	if (m_camInvDirty)
		computeCameraInverse();

	return m_camInv;
}

inline float* hkgCamera::getFrustumPlane(unsigned int i)
{
	HK_ASSERT( i < 6 );

	if (m_frustumPlanesDirty)
		computeFrustumPlanes();

	return m_frustumPlanes[i];
}

inline bool hkgCamera::sphereVisible(const float c[3], float radius) const
{
	if (m_frustumPlanesDirty)
		computeFrustumPlanes();

	for (unsigned int planeID = 0; planeID < 6; ++planeID)
	{
		float v = hkgVec3Dot( c, m_frustumPlanes[planeID] );
		if ( (v - m_frustumPlanes[planeID][HKG_D]) > radius )
			return false;
	}

	return true;
}

inline void hkgCamera::orthogonalize()
{
	// make sure up is orthoganal
	float d[3];
	getDir(d);
	float a[3];
	hkgVec3Cross( a, m_up, d );
	hkgVec3Cross( m_up, d, a );
}

inline void hkgCamera::computeProjection()
{
	switch (m_mode)
	{
	case HKG_CAMERA_PERSPECTIVE: 
		computePerspective( m_fov, m_aspect, m_near, m_far);
		break;
	case HKG_CAMERA_ORTHOGRAPHIC: 
		computeOrtho(m_oleft, m_oright, m_obottom, m_otop, m_onear, m_ofar);
		break;
	case HKG_CAMERA_COMPUTE_ORTHOGRAPHIC: 
		computeOrthoParams();
		computeOrtho(m_oleft, m_oright, m_obottom, m_otop, m_onear, m_ofar);
		break;
	default: break;
	}
}

inline hkgCamera::hkgCamera(const float* from, const float* to, const float* up, float fov)
:	m_mode(HKG_CAMERA_PERSPECTIVE),
	m_handedness(HKG_CAMERA_HANDEDNESS_RIGHT),
	m_fov(fov),
	m_near(HKG_CAMERA_DEFAULT_NEAR_PLANE),
	m_far(HKG_CAMERA_DEFAULT_FAR_PLANE),
	m_aspect(HKG_CAMERA_DEFAULT_ASPECT),
	m_camInvDirty(true), m_frustumPlanesDirty(true)
{
	const unsigned int size3 = sizeof(float)*3;
	hkString::memCpy( m_from, from, size3);
	hkString::memCpy( m_to, to, size3);
	hkString::memCpy( m_up, up, size3);
	
	orthogonalize();
	
	computeProjection();
	computeModelView();
}

inline HKG_CAMERA_PROJECTION_MODE hkgCamera::getProjectionMode() const
{
	return m_mode;
}

inline void hkgCamera::setProjectionMode(HKG_CAMERA_PROJECTION_MODE m)
{
	m_mode = m;
	computeProjection();
}

inline HKG_CAMERA_HANDEDNESS_MODE hkgCamera::getHandednessMode() const
{
	return m_handedness;
}

inline void hkgCamera::setHandednessMode(HKG_CAMERA_HANDEDNESS_MODE m)
{
	m_handedness = m;

	computeModelView();
	computeProjection();
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
