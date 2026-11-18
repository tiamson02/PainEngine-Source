/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline int hkProcessCollisionData::getNumContactPoints() const
{
	return int(m_firstFreeContactPoint - &m_contactPoints[0]);
}

inline hkProcessCdPoint* hkProcessCollisionData::getFirstContactPoint()
{
	return &m_contactPoints[0];
}

inline hkProcessCdPoint* hkProcessCollisionData::getEnd()
{
	return m_firstFreeContactPoint;
}

inline hkProcessCdPoint& hkProcessCollisionData::getContactPoint( int i )
{
	return m_contactPoints[i];
}

inline hkBool hkProcessCollisionData::isEmpty() const
{
	return m_firstFreeContactPoint == &m_contactPoints[0];
}

inline hkBool hkProcessCollisionData::hasToi() const
{
	return m_toi != hkMath::HK_REAL_MAX;
}

inline hkContactPoint& hkProcessCollisionData::getToiContactPoint()
{
	return m_toiContactPoint;
}

inline hkTime hkProcessCollisionData::getToi() const
{
	return m_toi;
}

inline hkContactPointMaterial& hkProcessCollisionData::getToiMaterial()
{
	return m_toiMaterial;
}


hkProcessCollisionData::hkProcessCollisionData(  ): m_toi( hkMath::HK_REAL_MAX )
{
}


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
