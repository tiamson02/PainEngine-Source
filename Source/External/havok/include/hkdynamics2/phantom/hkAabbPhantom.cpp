/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>

#include <hkdynamics2/phantom/hkAabbPhantom.h>
#include <hkdynamics2/phantom/hkPhantomConstructionInfo.h>
#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkcollide2/shape/hkAabbPhantomShape.h>
#include <hkcollide2/algorithms/boundingvolume/hkAabb.h>
#include <hkcollide2/broadphase/hkBroadPhase.h>
#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandler.h>
#include <hkcollide2/dispatch/hkCollisionDispatcher.h>
#include <hkdynamics2/world/hkBroadPhaseTypes.h>
#include <hkdynamics2/world/hkWorld.h>


hkAabbPhantom::hkAabbPhantom( const hkPhantomConstructionInfo& info )
:	hkPhantom( info )
{
	m_collisionFilter = info.m_collisionFilter;
	if (m_collisionFilter)
	{
		m_collisionFilter->addReference();
	}
}

hkAabbPhantom::~hkAabbPhantom()
{
	if (m_collisionFilter)
	{
		m_collisionFilter->removeReference();
	}
}

hkCollidable* hkAabbPhantom::castRay( const hkVector4& fromWorld, const hkVector4& toWorld, hkVector4& hitNormalWorld, float& mindist )
{

	hkCollidable* hitObject = HK_NULL;

	hkShape::hkRayResults rayResults;
	
	for (int i = 0; i < m_overlappingCollidables.getSize(); i++)
	{
		hkCollidable& col = *m_overlappingCollidables[i];
		
		const hkShape* shape = col.getShape();
		if ( shape != HK_NULL )
		{
			const hkTransform* trans = col.getTransform();
			hkVector4 shapefrom,shapeto;
			shapefrom.setTransformedInversePos( *trans, fromWorld);
			shapeto.setTransformedInversePos( *trans, toWorld);
				
			int hit = shape->castRay( shapefrom, shapeto, rayResults);
			if (hit!=0)
			{
				hitObject = &col;
			}
		}
	}
	
	if ( hitObject != HK_NULL )
	{
		const hkTransform* trans = hitObject->getTransform();
		hitNormalWorld.setRotatedDir( trans->getRotation() , rayResults.m_unnormalizedHitNormal);
		hitNormalWorld.normalize3();
		mindist = rayResults.m_mindist;
	}
	
	return hitObject;
}

void hkAabbPhantom::addOverlappingCollidable( hkCollidable& collidable )
{
	addFilteredOverlappingCollidable( collidable );
}

void hkAabbPhantom::removeOverlappingCollidable( hkCollidable& collidable )
{
	removeFilteredOverlappingCollidable( collidable );
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
