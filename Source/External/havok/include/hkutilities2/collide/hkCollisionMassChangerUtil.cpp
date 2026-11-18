/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/collide/hkCollisionMassChangerUtil.h>
#include <hkdynamics2/collide/hkResponseModifier.h>
#include <hkdynamics2/entity/hkRigidBody.h>

	/// Will call setRelativeMasses() in hkResponseModifier
	/// this allows an object to interact differently with one object than with another
	/// (ie it may appear to be fixed to one object, and not to another)
	/// This is an entity listener, with 2 rigidbodies used to decide when to 
	/// change the mass and what to change it to. You may implement a world listener
	/// and a look-up table to change the response of any object in contact with any
	/// other object
hkCollisionMassChangerUtil::hkCollisionMassChangerUtil( hkRigidBody* bodyA, hkRigidBody* bodyB, float inverseMassA, float inverseMassB )
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	
	m_inverseMassA = inverseMassA;
	m_inverseMassB = inverseMassB;

	m_bodyA->addCollisionListener( this );
	m_bodyA->addEntityListener( this );
}

hkCollisionMassChangerUtil::~hkCollisionMassChangerUtil()
{
	if(m_bodyA)
	{
		m_bodyA->removeCollisionListener( this );
		m_bodyA->removeEntityListener( this );
	}
}


void hkCollisionMassChangerUtil::contactPointAddedCallback( hkContactPointAddedEvent& event )
{
	if ( event.m_collidableA.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}
	if ( event.m_collidableB.getResponse()->m_primaryResponseType != hkMaterial::HK_SIMPLE_CONTACT_RESPONSE )
	{
		return;
	}

	const hkRigidBody* bodyA = static_cast<hkRigidBody*>(event.m_collidableA.getOwner());
	const hkRigidBody* bodyB = static_cast<hkRigidBody*>(event.m_collidableB.getOwner());

	// The bodies could be in either order so we have to check both cases
	if ( ( ( bodyA == m_bodyA ) && (bodyB == m_bodyB ) ) ||
		( ( bodyB == m_bodyA) && (bodyA == m_bodyB ) ) )
	{
		hkResponseModifier::setInvMassScalingForContact( &event.m_contactMgr, m_bodyA, m_bodyB, m_inverseMassA, m_inverseMassB );
	}
}

			// The hkCollisionListener interface implementation
void hkCollisionMassChangerUtil::contactProcessCallback( hkContactProcessEvent& event)
{
}

void hkCollisionMassChangerUtil::entityDeletedCallback( hkEntity* entity )
{
	HK_ASSERT2(entity == m_bodyA, "hkCollisionMassChangerUtil recieved an unexpected entity deleted callback");
	entity->removeCollisionListener( this );
	entity->removeEntityListener( this );
	m_bodyA = HK_NULL;
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
