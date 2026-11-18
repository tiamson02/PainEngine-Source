/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>

#include <hkdynamics2/phantom/hkCharacterPhantom.h>
#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkdynamics2/motion/rigid/hkRigidMotion.h>
#include <hkcollide2/contact/hkSimpleClosestPointsDetails.h>
#include <hkdynamics2/world/hkWorld.h>

	
hkCharacterPhantom::hkCharacterPhantom(	const hkCharacterPhantomConstructionInfo& info )
:	hkAabbPhantom(info)
{
	m_collidable.m_shape = info.m_shape;
	m_collidable.m_shape->addReference();
	m_collidable.m_motionState = &m_motionState;

	setTransform( info.m_transform );	
}


hkCharacterPhantom::~hkCharacterPhantom()
{
	m_collidable.m_shape->removeReference();
}


int hkCharacterPhantom::updateCollisions( hkCollisionEnvironment& env )
{
	hkReal minDist = hkMath::HK_REAL_MAX;

	int i = 0;
	const int a = m_collisionDetails.getSize();
	int	index = -1;

	for ( i = a - 1; i >= 0; i-- )
	{
		hkSimpleClosestPointsDetails* pointDetails = m_collisionDetails[i];

		pointDetails->updateClosestPoints( env );

		//if at least one point has been found, index is non-negative

		if ( pointDetails->getClosestIndex() >= 0 )
		{
			if (index >= 0)
			{
				if ( pointDetails->getClosestDistance() < minDist )
				{
					minDist = pointDetails->getClosestDistance();
					index = i;
				}
			} else
			{
				minDist = pointDetails->getClosestDistance();
				index = i;
			}
		}

	}

	return index;
}

	

void hkCharacterPhantom::addOverlappingCollidable( hkCollidable& collidable )
{
	if ( collidable.getShape() != HK_NULL )
	{
		if (hkAabbPhantom::addFilteredOverlappingCollidable( collidable ))
		{
			hkSimpleClosestPointsDetails* destElem = new hkSimpleClosestPointsDetails( collidable, m_collidable, *m_owner->getCollisionEnvironment() );
			m_collisionDetails.pushBack(destElem);
		}
	}
}

void hkCharacterPhantom::removeOverlappingCollidable( hkCollidable& collidable )
{
	if ( collidable.getShape() != HK_NULL )
	{
		if (hkAabbPhantom::removeFilteredOverlappingCollidable( collidable ))
		{
			for ( int i = m_collisionDetails.getSize() - 1; i >= 0; i-- )
			{
				if ( &m_collisionDetails[i]->getCollidableA() == &collidable )
				{
					m_collisionDetails[i]->destroy( *m_owner->getCollisionEnvironment() );
					m_collisionDetails[i] = m_collisionDetails[m_collisionDetails.getSize()-1];
					m_collisionDetails.popBack();
					break;
				}
			}
		} 
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
