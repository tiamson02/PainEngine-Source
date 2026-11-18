/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

HK_FORCE_INLINE	const hkVector4&	hkAabbCastShape::getFromWorld() const
{
	return m_fromWorld;
}


HK_FORCE_INLINE const hkVector4&	hkAabbCastShape::getToWorld() const
{
	return m_toWorld;
}

HK_FORCE_INLINE	void	hkAabbCastShape::setFromWorld( const hkVector4& fromWorld )
{
	m_fromWorld = fromWorld;
	hkShape::hkRayResults newResults;
	m_rayResults = newResults;
}

HK_FORCE_INLINE void	hkAabbCastShape::setToWorld( const hkVector4& toWorld )
{
	m_toWorld = toWorld;
	hkShape::hkRayResults newResults;
	m_rayResults = newResults;
}
		
HK_FORCE_INLINE const hkVector4&	hkAabbCastShape::getHalfExtents() const
{
	return m_halfExtents;
}

HK_FORCE_INLINE void hkAabbCastShape::setHalfExtents(const hkVector4& extents)
{
	m_halfExtents = extents;
}

HK_FORCE_INLINE void	hkAabbCastShape::setAabb(const hkAabb& aabb)
{
	m_aabb = aabb;
}

HK_FORCE_INLINE  hkShape::hkRayResults&	hkAabbCastShape ::getRayResults() const
{
	return m_rayResults;
}


HK_FORCE_INLINE void	hkAabbCastShape ::setIgnorePenetration( bool ignorePenetration)
{
	m_ignorePenetration = ignorePenetration;
}

HK_FORCE_INLINE bool	hkAabbCastShape ::getIgnorePenetration() const
{
	return m_ignorePenetration ;
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
