/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/action/hkArrayAction.h>
#include <hkdynamics2/world/hkWorld.h>

hkArrayAction::hkArrayAction(const hkArray<hkEntity*>& bodies)
{
	m_bodies = bodies;
	for (int i = 0; i < m_bodies.getSize(); i++)
	{
		hkEntity* ent = m_bodies[i];
		ent->addReference();
		ent->addEntityListener( this );
	}
}

hkArrayAction::~hkArrayAction()
{
	for (int i = 0; i < m_bodies.getSize(); i++)
	{
		hkEntity* ent = m_bodies[i];
		ent->removeEntityListener( this );
		ent->removeReference();
	}
}


void hkArrayAction::addEntity(hkEntity* entity)
{
	m_bodies.pushBack(entity);
	entity->addReference();
	entity->addEntityListener( this );
}


void hkArrayAction::getEntities( hkArray<hkEntity*>& entitiesOut )
{
	int i;
	entitiesOut.setSize(m_bodies.getSize());
	for (i=0;i<m_bodies.getSize();i++)
	{
		entitiesOut[i]=m_bodies[i];
	}
}


void hkArrayAction::removeEntity(hkEntity* entity)
{
	//find index,swap with last, and popback
	HK_ASSERT2(m_bodies.indexOf(entity)>=0,"in removeEntity: entity not in ArrayAction");
	m_bodies.removeAt(m_bodies.indexOf(entity));
	entity->removeEntityListener( this );
	entity->removeReference();
}


void hkArrayAction::entityRemovedCallback(hkEntity* entity) 
{
	//TODO
	if ( getOwner() )
	{
		getOwner()->removeAction(this);
	}
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
