/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/action/hkBinaryAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/world/hkWorld.h>

hkBinaryAction::hkBinaryAction(hkRigidBody* rigidBodyA, hkRigidBody* rigidBodyB)
:	m_bodyA(rigidBodyA),
	m_bodyB(rigidBodyB)
{
	if (m_bodyA == HK_NULL)
	{
		HK_WARN("hkBinaryAction: bodyA is a NULL pointer, you can use hkWorld::getFixedRigidBody()");	
	}
	else
	{
		m_bodyA->addReference();
		m_bodyA->addEntityListener(this);	
	}

	if (m_bodyB == HK_NULL)
	{
		HK_WARN("hkBinaryAction: bodyB is a NULL pointer, you can use hkWorld::getFixedRigidBody()");	
	}
	else
	{
		m_bodyB->addReference();
		m_bodyB->addEntityListener(this);
	}
}

void hkBinaryAction::entityRemovedCallback(hkEntity* entity) 
{
	if ( getOwner() != HK_NULL )
	{
		getOwner()->removeAction( this );
	}
}

void hkBinaryAction::getEntities( hkArray<hkEntity*>& entitiesOut )
{
	entitiesOut.pushBack( m_bodyA );
	entitiesOut.pushBack( m_bodyB );
}

void hkBinaryAction::setRigidBodyA(hkRigidBody* rigidBodyA)
{
	HK_ASSERT2(getOwner() == HK_NULL, "hkBinaryAction is already added to an hkWorld. m_bodyA cannot be changed.");	
	HK_ASSERT2(rigidBodyA != HK_NULL, "rigidBodyA is a NULL pointer. You can use hkWorld::getFixedRigidBody().");	

	//
	// If m_bodyA is being changed, remove the old hkRigidBody reference and listener.
	//
	if (m_bodyA != HK_NULL)
	{
		HK_WARN("m_bodyA is not NULL. This hkBinaryAction already has an hkRigidBody in m_bodyA.");

		m_bodyA->removeEntityListener(this);	
		m_bodyA->removeReference();
		m_bodyA = HK_NULL;
	}

	//
	// Add reference and listener for the new hkRigidBody.
	//
	m_bodyA = rigidBodyA;
	m_bodyA->addReference();
	m_bodyA->addEntityListener(this);	
}

void hkBinaryAction::setRigidBodyB(hkRigidBody* rigidBodyB)
{
	HK_ASSERT2(getOwner() == HK_NULL, "hkBinaryAction is already added to an hkWorld. m_bodyB cannot be changed.");	
	HK_ASSERT2(rigidBodyB != HK_NULL, "rigidBodyB is a NULL pointer. You can use hkWorld::getFixedRigidBody().");	

	//
	// If m_bodyB is being changed, remove the old hkRigidBody reference and listener.
	//
	if (m_bodyB != HK_NULL)
	{
		HK_WARN("m_bodyB is not NULL. This hkBinaryAction already has an hkRigidBody in m_bodyB.");

		m_bodyB->removeEntityListener(this);	
		m_bodyB->removeReference();
		m_bodyB = HK_NULL;
	}

	//
	// Add reference and listener for the new hkRigidBody.
	//
	m_bodyB = rigidBodyB;
	m_bodyB->addReference();
	m_bodyB->addEntityListener(this);
}

hkBinaryAction::~hkBinaryAction()
{
	if (m_bodyA != HK_NULL)
	{
		m_bodyA->removeEntityListener(this);
		m_bodyA->removeReference();
		m_bodyA = HK_NULL;
	}

	if (m_bodyB != HK_NULL)
	{
		m_bodyB->removeEntityListener(this);
		m_bodyB->removeReference();
		m_bodyB = HK_NULL;
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
