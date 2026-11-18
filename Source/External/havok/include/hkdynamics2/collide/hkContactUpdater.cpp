/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/collide/hkContactUpdater.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/collide/hkDynamicsContactMgr.h>
#include <hkconstraintsolver2/constraint/contact/hkContactPointProperties.h>


void HK_CALL hkContactUpdater::defaultFrictionUpdateCallback( hkContactUpdateEvent& event )
{
	for ( int j = 0; j < event.m_contactPointIds.getSize(); ++j )
	{
		hkContactPointProperties* prop = event.m_contactMgr.getContactPointProperties(event.m_contactPointIds[j]);

		const hkMaterial* materialA = static_cast<const hkMaterial*>( event.m_collidableA.getResponse() );
		const hkMaterial* materialB = static_cast<const hkMaterial*>( event.m_collidableB.getResponse() );

		prop->setFriction( hkMaterial::getCombinedFriction( materialA->getFriction(), materialB->getFriction() ) );

	}
}

static inline void fireIslandContacts( hkRigidBody* rb, hkSimulationIsland* island, hkContactUpdater::ContactUpdateCallback cb )
{
	for ( int i = 0; i < island->getCollisionCandidates().getSize(); ++i )
	{

		hkCollidable* a = island->getCollisionCandidates()[i].getCollidableA();
		hkCollidable* b = island->getCollisionCandidates()[i].getCollidableB();

		if ( ( a->getOwner() == rb ) ||
			 ( b->getOwner() == rb ) )
		{
			if ( island->getCollisionCandidates()[i].m_contactMgr != HK_NULL )
			{
				hkContactUpdateEvent event(static_cast<hkDynamicsContactMgr&>(*island->getCollisionCandidates()[i].m_contactMgr), *a, *b);
				event.m_contactMgr.getAllContactPointIds(event.m_contactPointIds);
				event.m_callbackFiredFrom = rb;

				cb( event );
			}
		}
	}
}

void hkContactUpdater::updateContacts( hkRigidBody* rb, hkContactUpdater::ContactUpdateCallback cb )
{
	HK_ASSERT2(rb->getOwner() != HK_NULL, "You are trying to update contact points for a rigid body which has not been added to the world");
	HK_ASSERT(rb->getSimulationIsland() != HK_NULL );
	
	if ( !rb->isFixed() )
	{
		fireIslandContacts( rb, rb->getSimulationIsland(), cb );
	}
	else
	{
		// have to go through ALL islands to update contacts to update a fixed body
		hkWorld* world = rb->getOwner();
		{
			for ( int i = 0; i < world->getActiveSimulationIslands().getSize(); ++i )
			{
				fireIslandContacts( rb, world->getActiveSimulationIslands()[i], cb );
			}
		}
		{
			for ( int i = 0; i < world->getInactiveSimulationIslands().getSize(); ++i )
			{
				fireIslandContacts( rb, world->getInactiveSimulationIslands()[i], cb );
			}
		}
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
