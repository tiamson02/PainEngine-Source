/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/***
 *** INLINE FUNCTION DEFINITIONS
 ***/

inline hkPlane::hkPlane() 
{
	m_normal = hkVector4( 0,1,0,0 );
}

inline hkPlane::hkPlane(const hkVector4& normal, hkReal dist)
{
	m_normal = normal;
	m_normal(3) = dist;
}


/// The distance is measured from the origin to the hkPlane, in the direction of the normal.
inline hkReal hkPlane::getDistance() const
{
	return m_normal(3);
}

/// The return vaue is positive if the point is on positive side of the hkPlane.
/// \param pt Distance is calculated between pt and the hkPlane  
inline hkReal hkPlane::getDistance(const hkVector4& pt) const
{
	hkReal dot = pt.dot3(m_normal);
	return  dot - m_normal(3);
}

/// \param newDist The distance from the origin to the hkPlane, in the direction of the normal. 
inline void hkPlane::setDistance(hkReal newDist)
{
	m_normal(3) = newDist;
}

// Returns the normal to the hkPlane.
inline const hkVector4& hkPlane::getNormal() const
{
	return m_normal;
}

// Change the normal for the hkPlane.
/// \param newNorm Specifies the new normal for the hkPlane.  
inline void hkPlane::setNormal(const hkVector4& newNorm)
{
	m_normal(0) = newNorm(0);
	m_normal(1) = newNorm(1);
	m_normal(2) = newNorm(2);
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
