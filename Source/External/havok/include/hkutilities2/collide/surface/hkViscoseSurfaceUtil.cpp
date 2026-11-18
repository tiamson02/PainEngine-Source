/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/collide/surface/hkViscoseSurfaceUtil.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/collide/hkResponseModifier.h>

//
// Please do not change this file
//

hkViscoseSurfaceUtil::hkViscoseSurfaceUtil( hkRigidBody* entity )
{
	HK_ASSERT2( entity->getOwner() == HK_NULL, "You can only create a hkViscoseSurfaceUtil BEFORE you add an entity to the world");

	this->addReference();

	m_entity = entity;

	HK_ASSERT2( entity->getCollidable().getResponse()->m_primaryResponseType == hkMaterial::HK_SIMPLE_CONTACT_RESPONSE, "The response type of the entity must be HK_SIMPLE_CONTACT_RESPONSE" );

	entity->setProcessContactCallbackDelay(0);
	entity->addCollisionListener( this );
	entity->addEntityListener( this );
}



			// The hkCollisionListener interface implementation
void hkViscoseSurfaceUtil::contactProcessCallback( hkContactProcessEvent& event)
{
	if ( event.m_collidableA.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}
	if ( event.m_collidableB.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}
	hkResponseModifier::setLowSurfaceViscosity( &event.m_contactMgr );
}

void hkViscoseSurfaceUtil::entityDeletedCallback( hkEntity* entity )
{		
	entity->removeCollisionListener( this );
	entity->removeEntityListener( this );
	this->removeReference();
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
