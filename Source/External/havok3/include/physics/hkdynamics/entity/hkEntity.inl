/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkMotion* hkEntity::getMotion()
{
	return m_motion;
}

inline hkSimulationIsland* hkEntity::getSimulationIsland() const
{	
	return m_simulationIsland;
}

hkAction* hkEntity::getAction(int i)
{
	return m_actions[i];
}

int hkEntity::getNumActions() const
{
	return m_actions.getSize();
}

inline int hkEntity::getNumConstraints() const
{
	return m_constraintsMaster.getSize() + m_constraintsSlave.getSize();
}

inline hkConstraintInstance* hkEntity::getConstraint( int i )
{
	HK_ASSERT2(0, i >=0 && i < getNumConstraints(), "Constraint index out of range.");
	return i < m_constraintsMaster.getSize() ? m_constraintsMaster[i].m_constraint : m_constraintsSlave[i - m_constraintsMaster.getSize()];
}

inline const hkConstraintInstance* hkEntity::getConstraint( int i ) const
{
	HK_ASSERT2(0, i >=0 && i < getNumConstraints(), "Constraint index out of range.");
	return i < m_constraintsMaster.getSize() ? m_constraintsMaster[i].m_constraint : m_constraintsSlave[i - m_constraintsMaster.getSize()];
}

inline const hkArray<hkEntityListener*>& hkEntity::getEntityListeners() const
{
	return m_entityListeners;
}

inline const hkArray<hkEntityActivationListener*>& hkEntity::getEntityActivationListeners() const
{
	return m_activationListeners;
}

inline const hkArray<hkCollisionListener*>& hkEntity::getCollisionListeners() const
{
	return m_collisionListeners;
}

inline hkUint16 hkEntity::getProcessContactCallbackDelay() const
{
	return m_processContactCallbackDelay;
}

inline void hkEntity::setProcessContactCallbackDelay( hkUint16 delay )
{
	m_processContactCallbackDelay = delay;
}

inline hkMaterial& hkEntity::getMaterial()
{
	return m_material;
}

inline hkLinkedCollidable* hkEntity::getLinkedCollidable()
{
	return &m_collidable;
}

inline const hkMaterial& hkEntity::getMaterial() const
{
	return m_material;
}

inline hkEntityDeactivator* hkEntity::getDeactivator()
{
	return m_deactivator;
}

inline const hkEntityDeactivator* hkEntity::getDeactivator() const
{
	return m_deactivator;
}

inline hkBool hkEntity::isFixed() const
{
	return m_fixed;
}

hkBool hkEntity::isFixedOrKeyframed() const
{
	return m_isFixedOrKeyframed;
}

inline hkUint32 hkEntity::getUid() const
{
	return m_uid;
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
