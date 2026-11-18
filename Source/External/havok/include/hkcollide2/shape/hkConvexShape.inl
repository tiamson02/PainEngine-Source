/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkConvexShape::hkConvexShape(hkReal radius)
{
	m_radius = radius;
}

/// The radius is used to create a thin "shell" around the object that is used as the shape's surface for collision detection purposes. 
/// This can improve performance, as calculations for shapes that are actually interpenetrating are much more time-consuming than for interpenetrating shells. 
inline hkReal hkConvexShape::getRadius() const
{
	return m_radius;
}

inline void hkConvexShape::setRadius(hkReal radius)
{
	m_radius = radius;
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
