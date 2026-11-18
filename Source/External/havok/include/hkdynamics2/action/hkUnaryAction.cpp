/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/action/hkUnaryAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/world/hkWorld.h>

hkUnaryAction::hkUnaryAction(hkRigidBody* body)
:	m_body(body)
{
	if (m_body == HK_NULL)
	{
		HK_WARN("hkUnaryAction: body is a NULL pointer");
	}
	else
	{
		m_body->addEntityListener(this);
		m_body->addReference();
	}
}

void hkUnaryAction::entityRemovedCallback(hkEntity* entity) 
{
	// Remove self from physics.
	if ( getOwner() != HK_NULL )
	{
		getOwner()->removeAction(this);
	}
}

hkUnaryAction::~hkUnaryAction()
{
	if (m_body != HK_NULL)
	{
		m_body->removeEntityListener(this);
		m_body->removeReference();
		m_body = HK_NULL;
	}
}

// NB: Only intended to be called pre-simualtion i.e. before the hkUnaryAction is 
// added to an hkWorld.
void hkUnaryAction::setRigidBody(hkRigidBody* rigidBody)
{
	HK_ASSERT2(getOwner() == HK_NULL, "This hkUnaryAction is already added to an hkWorld.");	
	HK_ASSERT2(rigidBody != HK_NULL, "rigidBody is a NULL pointer. You can use hkWorld::getFixedRigidBody().");	
	if(m_body != HK_NULL)
	{
		HK_WARN("m_body is not NULL. This hkUnaryAction already had an hkRigidBody.");

		m_body->removeEntityListener(this);
		m_body->removeReference();
		m_body = HK_NULL;
	}

	m_body = rigidBody;
	m_body->addEntityListener(this);
	m_body->addReference();
}

void hkUnaryAction::getEntities( hkArray<hkEntity*>& entitiesOut )
{
	entitiesOut.pushBack( m_body );
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
