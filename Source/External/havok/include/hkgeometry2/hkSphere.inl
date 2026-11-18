/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// The sphere class is a utility class for storing information representing a 
/// sphere. This constructor creates a sphere with the given center and radius .
/// \param pt Center of the sphere  
/// \param radius Radius of the sphere  
inline hkSphere::hkSphere()
	: m_pos(hkVector4(0,0,0))
{
	m_pos(3) = 1.0f;
}

/// The sphere class is a utility class for storing information representing a 
/// sphere. This constructor creates a sphere with the given center and radius .
/// \param pt Center of the sphere  
/// \param radius Radius of the sphere  
inline hkSphere::hkSphere(const hkVector4& pt, hkReal radius)
{
	m_pos = pt;
	m_pos(3) = radius;
}

/// This function returns the radius of the sphere.
inline hkReal hkSphere::getRadius() const
{
	return m_pos(3);
}

/// This function changes the radius of the sphere .
/// \param newRadius The new radius  
inline void hkSphere::setRadius(hkReal newRadius)
{
	m_pos(3) = newRadius;
}

/// This function returns the position of the center of the sphere.
inline const hkVector4& hkSphere::getPosition() const
{
	return m_pos;
}

/// This function sets the position of the center of the sphere.
/// \param newPos The desired center of the sphere  
inline void hkSphere::setPosition(const hkVector4& newPos)
{
	m_pos(0) = newPos(0);	// do not override the z component
	m_pos(1) = newPos(1);
	m_pos(2) = newPos(2);
}

inline void hkSphere::setPositionAndRadius( const hkVector4& newPos )
{
	m_pos = newPos;
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
