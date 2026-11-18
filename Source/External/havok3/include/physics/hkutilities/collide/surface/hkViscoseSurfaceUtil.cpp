/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities/collide/surface/hkViscoseSurfaceUtil.h>
#include <hkdynamics/entity/hkRigidBody.h>
#include <hkdynamics/collide/hkResponseModifier.h>

//
// Please do not change this file
//

hkViscoseSurfaceUtil::hkViscoseSurfaceUtil( hkRigidBody* entity )
{
	HK_ASSERT2(0x6740aa92,  entity->getWorld() == HK_NULL, "You can only create a hkViscoseSurfaceUtil BEFORE you add an entity to the world");

	this->addReference();

	m_entity = entity;

	HK_ASSERT2(0x42166b07, entity->getMaterial().getResponseType() == hkMaterial::RESPONSE_SIMPLE_CONTACT, "The response type of the entity must be hkMaterial::RESPONSE_SIMPLE_CONTACT" );

	entity->setProcessContactCallbackDelay(0);
	entity->addCollisionListener( this );
	entity->addEntityListener( this );
}



			// The hkCollisionListener interface implementation
void hkViscoseSurfaceUtil::contactProcessCallback( hkContactProcessEvent& event)
{
	if ( static_cast<hkEntity*>(event.m_collidableA.getOwner())->getMaterial().getResponseType() != hkMaterial::RESPONSE_SIMPLE_CONTACT )
	{
		return;
	}
	if ( static_cast<hkEntity*>(event.m_collidableB.getOwner())->getMaterial().getResponseType() != hkMaterial::RESPONSE_SIMPLE_CONTACT )
	{
		return;
	}
	hkResponseModifier::setLowSurfaceViscosity( &event.m_internalContactMgr );
}

void hkViscoseSurfaceUtil::entityDeletedCallback( hkEntity* entity )
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
