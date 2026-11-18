/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkEntity::hkEntity( const hkEntityConstructionInfo& info )
:	m_collidable( info.m_shape, HK_NULL, HK_NULL, HK_NULL, HK_BROAD_PHASE_ENTITY)
{
	//HK_ASSERT2(info.m_shape != HK_NULL, "Cannot create an entity without a shape");
	m_collidable.setOwner( this );
	m_material.m_primaryResponseType = info.m_collisionResponse;
	m_collidable.setResponse( &this->m_material );

	m_simulationIsland = HK_NULL;
	m_fixed = false;
	m_processContactCallbackDelay = info.m_processContactCallbackDelay;
	m_collidable.setCollisionFilterInfo( info.m_collisionFilterInfo );

	m_owner = HK_NULL;

	m_deactivator = HK_NULL;

	if (info.m_shape != HK_NULL)
	{
		info.m_shape->addReference(); // TODO
	}

	m_storageIndex = -1;	// This is used by the simulation islands, and is not initialised correctly until the entity has been added to the world.

//	HK_RECOVERABLE_ASSERT( (int(this) & 0x3f) == 0, "The memory system did not align the hkEntity on a 64 cache line" );
//	HK_RECOVERABLE_ASSERT_END();
}

inline void hkEntity::getConstructionInfo(hkEntityConstructionInfo& info)
{
	info.m_collisionFilterInfo = m_collidable.getCollisionFilterInfo();
	info.m_collisionResponse = m_material.m_primaryResponseType;
	info.m_processContactCallbackDelay = m_processContactCallbackDelay;
	info.m_shape = m_collidable.getShape();
}

inline hkPropertyValue hkEntity::getProperty( hkUint32 type ) const
{
	for (int i = 0; i < m_properties.getSize(); ++i)
	{
		if (m_properties[i].m_key == type)
		{
			return m_properties[i].m_value;
		}
	}

	hkPropertyValue returnValue;
	returnValue.asPtr = HK_NULL;

	return returnValue;
}

inline bool hkEntity::hasProperty( hkUint32 type ) const
{
	for (int i = 0; i < m_properties.getSize(); ++i)
	{
		if (m_properties[i].m_key == type)
		{
			return true;
		}
	}
	return false;
}

inline hkCollidable& hkEntity::getCollidable()
{
	return m_collidable;
}

inline hkMotion* hkEntity::getMotion()
{
	return m_motion;
}

inline hkSimulationIsland* hkEntity::getSimulationIsland()
{	
	return m_simulationIsland;
}

inline void hkEntity::setSimulationIsland( hkSimulationIsland* si)
{
	m_simulationIsland = si;
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

inline hkUint16 hkEntity::getProcessContactCallbackDelay()
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

inline int hkEntity::getStorageIndex()
{
	return m_storageIndex;
}

inline void hkEntity::setStorageIndex(int index)
{
	m_storageIndex = index;
}

inline hkEntityDeactivator* hkEntity::getDeactivator()
{
	return m_deactivator;
}

inline void hkEntity::setDeactivator( hkEntityDeactivator* deactivator )
{
	if ( deactivator != HK_NULL )
	{
		deactivator->addReference();
	}

	if ( m_deactivator != HK_NULL )
	{
		m_deactivator->removeReference();
	}

	m_deactivator = deactivator;
}

inline bool hkEntity::isFixed() const
{
	return m_fixed;
}

inline hkWorld* hkEntity::getOwner()
{
	return m_owner;
}

inline bool hkEntity::setShape(hkShape* shape)
{
	bool setOk1 = shape != HK_NULL;

	HK_ASSERT2(setOk1, "Cannot setShape to NULL.");

	bool setOk2 = getCollidable().getShape() == HK_NULL;

	// Check if hkCollidable already contained an hkShape.
	HK_ASSERT2(setOk2, "Cannot setShape as hkEntity's hkCollidable already has an hkShape.");

	bool setOk3 = getOwner() == HK_NULL;

	HK_ASSERT2(setOk3, "Cannot setShape as hkEntity is already added to an hkWorld.");

	setOk1 = setOk1 && setOk2 && setOk3;

	if (setOk1)
	{
		getCollidable().setShape(shape);
		shape->addReference();
	}
	return setOk1;
}

inline void hkEntity::setOwner( hkWorld* owner )
{
	m_owner = owner;
}

inline void hkEntity::cleanupEntityListeners()
{
	hkArray<hkEntityListener*>& listen = m_entityListeners;

	int end = listen.getSize();
	int i = 0;
	while( i < end && listen[i] != HK_NULL )
	{
		i++;
	}

	int shiftTarget = i;
	i++;
	while( i < end )
	{

		if( listen[i] != HK_NULL )
		{
			listen[shiftTarget] = listen[i];
			shiftTarget++;
		}

		i++;
	}

	listen.setSize( shiftTarget );
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
