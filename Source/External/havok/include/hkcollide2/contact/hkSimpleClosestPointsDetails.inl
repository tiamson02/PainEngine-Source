/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkSimpleClosestPointsDetails::resetResults()
{
	m_closestIndex = -1;
	m_closestDistance = hkMath::HK_REAL_MAX;
	m_contactPoints.clear();
}

inline hkSimpleClosestPointsDetails::hkSimpleClosestPointsDetails(	const hkCollidable& colA,
																	const hkCollidable& colB,
																	const hkCollisionEnvironment& colEnv )
:	m_collidableA(colA),
	m_collidableB(colB)
{
	resetResults();
	m_collisionAgent = colEnv.m_dispatcher->getNewCollisionAgent(m_collidableA, m_collidableB, colEnv);
}


inline void hkSimpleClosestPointsDetails::destroy( const hkCollisionEnvironment& env )
{
	if (m_collisionAgent)
	{
		m_collisionAgent->cleanup( env );
		m_collisionAgent = HK_NULL;
	}
	delete this;
}


inline hkSimpleClosestPointsDetails::~hkSimpleClosestPointsDetails( )
{
}

inline hkReal hkSimpleClosestPointsDetails::getClosestDistance() const
{
	return m_closestDistance;
}

inline	int	hkSimpleClosestPointsDetails::getClosestIndex() const
{
	return m_closestIndex;
}

inline const hkArray<hkContactPoint>& hkSimpleClosestPointsDetails::getContactPoints() const
{
	return m_contactPoints;
}


inline void hkSimpleClosestPointsDetails::updateClosestPoints( const hkCollisionEnvironment& env )
{
	
	resetResults();

	if (m_collisionAgent)
	{
		m_collisionAgent->getClosestPoints(
					m_collidableA,
					m_collidableB,
					env,*this);
	}

}

inline const hkCollidable& hkSimpleClosestPointsDetails::getCollidableA() const
{
	return m_collidableA;
}


inline const hkCollidable& hkSimpleClosestPointsDetails::getCollidableB() const
{
	return m_collidableB;
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
