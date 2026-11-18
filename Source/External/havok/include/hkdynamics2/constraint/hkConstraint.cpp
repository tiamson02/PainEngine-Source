/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/constraint/hkConstraint.h>
#include <hkdynamics2/constraint/hkConstraintOwner.h>


HK_COMPILE_TIME_ASSERT(sizeof(hkConstraint) == 32);

void hkConstraint::entityAddedCallback(hkEntity* entity)
{
}

void hkConstraint::entityRemovedCallback(hkEntity* entity)
{
	if ( m_owner != HK_NULL )
	{
		m_owner->removeConstraint( this );
	}
}

hkConstraint::~hkConstraint()
{
	if (m_bodies[0] != HK_NULL)
	{
		m_bodies[0]->removeEntityListener(this); 
		m_bodies[0]->removeReference(); 
	}
	if (m_bodies[1] != HK_NULL)
	{
		m_bodies[1]->removeEntityListener(this);
		m_bodies[1]->removeReference();
	}
}

void hkConstraint::setEntityA(hkEntity* entity )
{
	HK_ASSERT2( m_owner == HK_NULL, "You cannot change EntityA if the constraints is already added to the world" );

	if ( entity != HK_NULL )
	{
		entity->addReference();
		entity->addEntityListener(this);
	}

	if ( m_bodies[0] )
	{
		m_bodies[0]->removeEntityListener(this);
		m_bodies[0]->removeReference();
	}
	m_bodies[0] = entity;
}

void hkConstraint::setEntityB(hkEntity* entity )
{
	HK_ASSERT2( m_owner == HK_NULL, "You cannot change EntityB if the constraints is already added to the world" );

	if (entity != HK_NULL)
	{
		entity->addReference();
		entity->addEntityListener(this);
	}

	if ( m_bodies[1] )
	{
		m_bodies[1]->removeEntityListener(this);
		m_bodies[1]->removeReference();
	}
	m_bodies[1] = entity;
}

void hkConstraint::entityDeletedCallback( hkEntity* entity )
{
	HK_ASSERT2(0, "A constraint recieved an entity deleted callback. The entity should not have been deleted if the constraint was still alive.");
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
