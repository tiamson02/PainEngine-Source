/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkutilities2/collide/hkContactCollector.h>
#include <hkcollide2/contact/hkCollisionContactPoint.h>
#include <hkcollide2/contact/hkCollisionResults.h>
#include <hkcollide2/hkCollidable.h>
#include <hkdynamics2/entity/hkEntity.h>
		

hkContactCollector::~hkContactCollector()
{
	for (int i = 0; i < m_entities.getSize(); ++i )
	{
		removeFromEntity( m_entities[i] );
	}
}

void hkContactCollector::flipContactPoints( hkRigidBody* body )
{
	for (int i = 0; i < m_contactPoints.getSize(); i++ )
	{
		ContactPoint& cp = m_contactPoints[i];
		if ( cp.m_bodyB == body ) 
		{
			cp.m_normal.setNeg4( cp.m_normal );
			cp.m_bodyB = cp.m_bodyA;
			cp.m_bodyA = body;
		}
	}
}

		/// reset all contact points
void hkContactCollector::reset()
{
	m_contactPoints.clear();
}

	/// the hkCollisionListener interface implementation
void hkContactCollector::contactPointAddedCallback(	  hkContactPointAddedEvent& event)
{
}

	/// the hkCollisionListener interface implementation
void hkContactCollector::contactPointRemovedCallback( hkContactPointRemovedEvent& event)
{
}


void hkContactCollector::addToEntity( hkEntity* entity )
{
	entity->addCollisionListener( this );
	entity->addEntityListener( this );
	m_entities.pushBack(entity);
}

void hkContactCollector::removeFromEntity( hkEntity* entity )
{
	HK_ASSERT2( m_entities.indexOf( entity ) != -1, "Trying to remove a contact collector from an entity to which it has not been added");
	m_entities.removeAt( m_entities.indexOf( entity ) );
	entity->removeCollisionListener( this );
	entity->removeEntityListener( this );
}


	/// the hkCollisionListener interface implementation
void hkContactCollector::contactProcessCallback( hkContactProcessEvent& event)
{
	int numNewPoints = event.m_collisionResult.m_contactPoints.getSize();
	ContactPoint* cp = m_contactPoints.expandBy( numNewPoints );
	for (int i = 0; i < numNewPoints; i++ )
	{
		hkCollisionContactPoint& ccp = event.m_collisionResult.m_contactPoints[i];
		cp->m_bodyA = static_cast<hkRigidBody*>(event.m_collidableA.getOwner());
		cp->m_bodyB = static_cast<hkRigidBody*>(event.m_collidableB.getOwner());
		cp->m_normal = ccp.m_normal;
		cp->m_position = ccp.m_position;
		cp->m_distance = ccp.m_distance;
		cp++;
	}
	HK_ASSERT2( m_contactPoints.getSize() < 1024, "Warning: Too many contact points gathered, are you sure you called hkContactCollector::reset() every frame ??" );
}


void hkContactCollector::entityDeletedCallback( hkEntity* entity )
{
	removeFromEntity( entity );
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
