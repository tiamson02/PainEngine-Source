/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkgRay::hkgRay()
{
	hkgVec3Zero( m_from );
	m_dir[0] = 0.0f;
	m_dir[1] = 0.0f;
	m_dir[2] = 1.0f;
}


inline hkgRay::hkgRay(const float from[3], const float dir[3])
{
	hkgVec3Copy( m_dir, dir );
	hkgVec3Copy( m_from, from );
}

inline float* hkgRay::getFromPtr()
{
	return m_from;
}

inline const float* hkgRay::getFromPtr() const
{
	return m_from;
}

inline void hkgRay::getFrom(float f[3]) const
{
	hkgVec3Copy( f, m_from );
}

inline float* hkgRay::getDirPtr()
{
	return m_dir;
}

inline const float* hkgRay::getDirPtr() const
{
	return m_dir;
}

inline void hkgRay::getDir(float d[3]) const
{
	hkgVec3Copy( d, m_dir );
}

inline void hkgRay::transform( float t[16] )
{
	hkgVec3Rotate( m_dir, t );
	float temp[3];
	hkgVec3Copy( temp, m_from );
	hkgVec3Transform( m_from, temp, t );
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
