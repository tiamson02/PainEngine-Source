/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


hkWorld* hkSimulationIsland::getWorld()
{
	return m_world;
}



inline bool hkSimulationIsland::isActive() const
{
	return m_active;
}

inline bool hkSimulationIsland::isFixed() const
{
	return m_storageIndex == -1;
}

inline void hkSimulationIsland::addNewBroadPhasePair( hkBroadPhaseHandlePair& pair )
{
	m_newBroadPhasePairs.pushBack( pair );
}

inline void hkSimulationIsland::addOldBroadPhasePair( hkBroadPhaseHandlePair& pair )
{
	m_oldBroadPhasePairs.pushBack( pair );
}


inline const hkArray<hkEntity*>& hkSimulationIsland::getEntities() const 
{ 
	return m_entities; 
}

inline const hkArray<hkAction*>& hkSimulationIsland::getActions() const 
{ 
	return m_actions; 
}

inline const hkArray<hkConstraint*>& hkSimulationIsland::getConstraints() const 
{ 
	return m_constraints; 
}

int hkSimulationIsland::getStorageIndex()
{
	return m_storageIndex;
}

inline hkArray<hkCollidablePair>& hkSimulationIsland::getCollisionCandidates()
{
	return m_collisionCandidates;
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
