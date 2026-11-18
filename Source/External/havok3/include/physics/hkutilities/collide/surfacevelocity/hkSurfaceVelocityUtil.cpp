/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities/collide/surfacevelocity/hkSurfaceVelocityUtil.h>
#include <hkdynamics/collide/hkResponseModifier.h>
#include <hkdynamics/entity/hkRigidBody.h>

hkSurfaceVelocityUtil::hkSurfaceVelocityUtil(hkRigidBody* body, const hkVector4& surfaceVelocityWorld)
{
	this->addReference();

	m_rigidBody = body;
	m_surfaceVelocity = surfaceVelocityWorld;

	m_rigidBody->addCollisionListener( this );
	m_rigidBody->addEntityListener( this );
}


// The hkCollisionListener interface implementation
void hkSurfaceVelocityUtil::contactPointAddedCallback(hkContactPointAddedEvent& event)
{
	hkRigidBody *bodyA = hkGetRigidBody(event.m_bodyA.getRootCollidable());
	hkRigidBody *bodyB = hkGetRigidBody(event.m_bodyB.getRootCollidable());

	hkDynamicsContactMgr* mgr = &event.m_internalContactMgr;
	if ( !mgr )
	{
		mgr = bodyA->findContactMgrTo(bodyB);
	}

	if ( !mgr )
	{
		return;
	}

	if ( bodyA->getMaterial().getResponseType() != hkMaterial::RESPONSE_SIMPLE_CONTACT )
	{
		return;
	}
	if ( bodyB->getMaterial().getResponseType() != hkMaterial::RESPONSE_SIMPLE_CONTACT )
	{
		return;
	}
	hkResponseModifier::setSurfaceVelocity( mgr, m_rigidBody, &m_surfaceVelocity );
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
