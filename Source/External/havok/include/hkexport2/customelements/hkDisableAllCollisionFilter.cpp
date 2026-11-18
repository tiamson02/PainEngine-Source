/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkdynamics2/hkDynamics2.h>
#include <hkexport2/customelements/hkDisableAllCollisionFilter.h>



hkDisableAllCollisionFilter::~hkDisableAllCollisionFilter()
{
	// remove self as listener from any entities.

	for ( int i = 0; i < m_disabledEntities.getSize(); i++ )
	{
		if ( m_disabledEntities[i]->getEntityListeners().indexOf(this) >= 0 )
		{
			m_disabledEntities[i]->removeEntityListener( this );
		}
	}
}

bool hkDisableAllCollisionFilter::isCollisionEnabled(const hkCollidable& a,const hkCollidable& b) const
{
	for (int i=0; i < m_disabledEntities.getSize(); i++)
	{
		const hkCollidable* stored_collidable = &(m_disabledEntities[i]->getCollidable());
		if ((stored_collidable == &a) || (stored_collidable == &b))
		{
			return false;
		}
	}
	return true;
}


bool hkDisableAllCollisionFilter::addEntityToFilter( hkEntity* new_entity)
{
	if (!new_entity)
	{
		return false;
	}
	for (int i=0; i < m_disabledEntities.getSize(); i++)
	{
		if (m_disabledEntities[i] == new_entity)
		{
			return false;
		}
	}
	m_disabledEntities.pushBack(new_entity);

	// add to 'new_entity's listeners if this filter is not there
	if ( new_entity->getEntityListeners().indexOf(this) < 0 )
	{
		new_entity->addEntityListener(this);
	}
	return true;
}

bool hkDisableAllCollisionFilter::removeEntityFromFilter( hkEntity* new_entity)
{
	if (!new_entity)
	{
		return false;
	}
	int i = 0;
	while (i < m_disabledEntities.getSize())
	{
		const hkEntity* stored_entity = m_disabledEntities[i];
		
		if (stored_entity == new_entity)
		{
			m_disabledEntities.removeAt(i);
			return true;
		}
		else
		{
			i++;
		}
	}	
	return false;
}

void hkDisableAllCollisionFilter::entityRemovedCallback(hkEntity* entity)
{
	if (entity)
	{
		removeEntityFromFilter(entity);
		entity->removeEntityListener( this );
	}
}

void hkDisableAllCollisionFilter::entityDeletedCallback( hkEntity* entity )
{
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
