/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/collide/hkSurfaceVelocityUtil.h>
#include <hkdynamics2/collide/hkResponseModifier.h>
#include <hkdynamics2/entity/hkRigidBody.h>

	/// Will call setSurfaceVelocity() in hkResponseModifier, which gives the 
	/// surface a conveyor-belt like behavior
	/// This is an entity listener, with 2 rigidbodies used to decide when to 
	/// change the mass and what to change it to. You may implement a world listener
	/// and a look-up table to change the response of any object in contact with any
	/// other object
hkSurfaceVelocityUtil::hkSurfaceVelocityUtil( hkRigidBody* body, const hkVector4& surfaceVelocityWorld )
{
	this->addReference();

	m_rigidBody = body;
	m_surfaceVelocity = surfaceVelocityWorld;

	m_rigidBody->addCollisionListener( this );
	m_rigidBody->addEntityListener( this );
}


	// The hkCollisionListener interface implementation
void hkSurfaceVelocityUtil::contactPointAddedCallback( hkContactPointAddedEvent& event )
{
	if ( event.m_collidableA.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}
	if ( event.m_collidableB.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}
	hkResponseModifier::setSurfaceVelocity( &event.m_contactMgr, m_rigidBody, &m_surfaceVelocity );
}

			// The hkCollisionListener interface implementation
void hkSurfaceVelocityUtil::contactProcessCallback( hkContactProcessEvent& event)
{
}

void hkSurfaceVelocityUtil::entityDeletedCallback( hkEntity* entity )
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
