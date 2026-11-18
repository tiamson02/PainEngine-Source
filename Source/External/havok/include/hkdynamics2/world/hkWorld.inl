/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkCollisionEnvironment* hkWorld::getCollisionEnvironment()
{
	return m_collisionEnvironment;
}


inline hkBroadPhase* hkWorld::getBroadPhase()
{
	return m_broadPhase;
}

inline hkCollisionDispatcher* hkWorld::getCollisionDispatcher()
{
	return m_collisionDispatcher;
}

inline hkTypedBroadPhaseHandler* hkWorld::getBroadPhaseHandler()
{
	return m_typedBroadPhaseHandler;
}

inline const hkVector4& hkWorld::getGravity()
{
	return m_gravity;
}


inline const hkSimulationIsland* hkWorld::getFixedIsland() const
{
	return m_fixedIsland;
}

inline const hkArray<hkSimulationIsland*>& hkWorld::getActiveSimulationIslands() const
{
	return m_activeSimulationIslands;
}

inline const hkArray<hkSimulationIsland*>& hkWorld::getInactiveSimulationIslands() const
{
	return m_inactiveSimulationIslands;
}


inline void hkWorld::requestMerge( hkEntity* entityA, hkEntity* entityB )
{

//	HK_ASSERT2( entityA->getSimulationIsland() != entityB->getSimulationIsland(), "internal error");
//	HK_ASSERT2( (entityA->getSimulationIsland() != m_fixedIsland) && (entityB->getSimulationIsland() != m_fixedIsland), "internal error");

	m_mergeList.setSize( m_mergeList.getSize() + 1 );
	m_mergeList[m_mergeList.getSize() - 1].m_entityA = entityA;
	m_mergeList[m_mergeList.getSize() - 1].m_entityB = entityB;

}


inline hkReal hkWorld::getHighFrequencyDeactivationPeriod() const
{
	return m_highFrequencyDeactivationPeriod;
}

inline void hkWorld::setHighFrequencyDeactivationPeriod( hkReal period )
{
	m_lowFrequencyDeactivationPeriod = period;
}

inline hkReal hkWorld::getLowFrequencyDeactivationPeriod() const
{
	return m_lowFrequencyDeactivationPeriod;
}

inline void hkWorld::setLowFrequencyDeactivationPeriod( hkReal period )
{
	m_lowFrequencyDeactivationPeriod = period;
}

inline hkWorld::SimulationState hkWorld::setRemovalPermission(hkWorld::SimulationState state)
{
	SimulationState oldState = static_cast<SimulationState>(m_simulationState);
	m_simulationState = state;
	return oldState;
}

inline const hkArray<hkPhantom*>& hkWorld::getPhantoms() const
{
	return m_phantoms;
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
