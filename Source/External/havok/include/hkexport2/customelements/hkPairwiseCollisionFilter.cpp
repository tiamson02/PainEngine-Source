/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/customelements/hkPairwiseCollisionFilter.h>


hkPairwiseCollisionFilter::~hkPairwiseCollisionFilter()
{
	// remove self as listener from any entities.

	// temporary - get the hkArray from the map
	hkArray< hkPair<CollisionPair, int> >& pairList = m_disabledPairs.m_storage;

	for ( int i = 0; i < pairList.getSize(); ++i )
	{
		if ( pairList[i].first.m_a->getEntityListeners().indexOf(this) < 0 )
		{
			pairList[i].first.m_a->removeEntityListener( this );
		}
		if ( pairList[i].first.m_b->getEntityListeners().indexOf(this) < 0 )
		{
			pairList[i].first.m_b->removeEntityListener( this );
		}
	}
}


void hkPairwiseCollisionFilter::disableCollisionPair(hkEntity* a, hkEntity* b)
{
	CollisionPair pair(a, b);
	if (m_disabledPairs.find(pair) != HK_NULL)
	{
		HK_ASSERT2(0, "Collision pair already disabled");
	}
	else
	{
		m_disabledPairs[pair] = 1;

		// add to 'entityA's listeners if this filter is not there
		if ( a->getEntityListeners().indexOf(this) < 0 )
		{
			a->addEntityListener(this);
		}

		// add to 'entityA's listeners if this filter is not there
		if ( b->getEntityListeners().indexOf(this) < 0 )
		{
			b->addEntityListener(this);
		}
	}

}

void hkPairwiseCollisionFilter::enableCollisionPair(hkEntity* a, hkEntity* b)
{
	CollisionPair pair(a, b);

	if (m_disabledPairs.erase(pair) == 0)
	{
		HK_ASSERT2(0, "Collision pair already enabled");
	}
	else
	{
		{
			// remove from 'entityA's listeners if this filter is there
			const hkArray<hkEntityListener*>& listeners = a->getEntityListeners();
			int i = listeners.indexOf(this);
			if (i >= 0)
			{
				a->removeEntityListener(this);
			}
		}

		{
			// remove from 'entityB's listeners if this filter is there
			const hkArray<hkEntityListener*>& listeners = b->getEntityListeners();
			int i = listeners.indexOf(this);
			if (i >= 0)
			{
				 b->removeEntityListener(this);
			}
		}
	}
}

bool hkPairwiseCollisionFilter::isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const
{
	hkEntity* entityA = static_cast<hkEntity*>(a.getOwner());
	hkEntity* entityB = static_cast<hkEntity*>(b.getOwner());
	CollisionPair pair(entityA, entityB);
	return m_disabledPairs.find(pair) == HK_NULL;
}

void hkPairwiseCollisionFilter::entityRemovedCallback(hkEntity* entity)
{
	// temporary implementation - iterate through the internal array

	int i = 0;
	while (i < m_disabledPairs.m_storage.getSize())
	{
		const hkEntity* entityA = m_disabledPairs.m_storage[i].first.m_a;
		const hkEntity* entityB = m_disabledPairs.m_storage[i].first.m_b;

		if ( (entityA == entity) || (entityB == entity) )
		{
			// entity has been deleted - remove pair that references it
			m_disabledPairs.m_storage.removeAt(i);
		}
		else
		{
			i++;
		}
	}	
	entity->removeEntityListener( this );
}

void hkPairwiseCollisionFilter::entityDeletedCallback( hkEntity* entity )
{
}

int hkPairwiseCollisionFilter::getNumDisabledPairs() const
{
	const int numPairs = m_disabledPairs.m_storage.getSize();
	return numPairs;
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
