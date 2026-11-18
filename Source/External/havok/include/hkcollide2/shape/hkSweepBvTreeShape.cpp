/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/shape/hkSweepBvTreeShape.h>

#include <hkcollide2/broadphase/hk3AxisSweep.h>

#include <hkcollide2/algorithms/boundingvolume/hkAabbUtil.h>

#include <hkcollide2/shape/hkRayCastCallback.h>



hkSweepBvTreeShape::hkSweepBvTreeShape( hkShapeCollection* collection )
: hkBvTreeShape( collection ) 
{
	// query aabb of the shape collection needed to provide the total size of the sweep and prune space

	collection->addReference();

	hkAabb aabb;
	m_collection = collection;
	collection->getAabb(hkTransform::getIdentity(),0.f,aabb);

	m_3dSweep = new hk3AxisSweep ( aabb.m_min , aabb.m_max );

	
	int numchilds = collection->getNumChildShapes();


	hkArray<hkBroadPhaseHandle*>	objectHandles;
	m_objectHandles.setSize(numchilds);
	objectHandles.setSize(numchilds);

	int i;
	for (i=0;i<numchilds;i++)
	{
		objectHandles[i] = &m_objectHandles[i];
	}


	{
		// query all the children to get their aabb, and add them to the sweep and prune space
		for (i=0;i<numchilds;i++)
		{
			hkArray<hkBroadPhaseHandlePair>	newPairs;
			hkAabb aabb;
			hkShapeCollection::AllocBuffer HK_ALIGNED_VARIABLE( shapeBuffer, 16 );
			hkReal	tolerance = 0.0f;

			collection->getChildShape(collection->getKeyAt(i),shapeBuffer)->getAabb(hkTransform::getIdentity(),tolerance,aabb);
			m_3dSweep->addObject( objectHandles[i] , aabb , newPairs );

		}
	}
}

hkSweepBvTreeShape::~hkSweepBvTreeShape()
{
	m_collection->removeReference();
	delete m_3dSweep;
}


void hkSweepBvTreeShape::getAabb( const hkTransform& localToWorld, hkReal tolerance, hkAabb& out ) const
{
	m_shapeCollection->getAabb(localToWorld,tolerance,out);
}

void hkSweepBvTreeShape::querySphere( const hkSphere &sphereQuery, hkArray<hkShapeCollection::hkShapeKey>& hits ) const
{
	//turn sphere into an aabb and query single obb
	const hkVector4&	pos = sphereQuery.getPosition();
	hkVector4 halfExtents;
	halfExtents.setAll(sphereQuery.getRadius());
	hkReal	tolerance = 0.0f;
	hkTransform	obbTrans;
	obbTrans.setIdentity();
	obbTrans.setTranslation(pos);
	queryObb( obbTrans , halfExtents , tolerance , hits );
}

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the obb (defined by obbTransform and obbExtent).
void hkSweepBvTreeShape::queryObb( const hkTransform& obbTransform, const hkVector4& obbExtent, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const
{
	HK_INTERNAL_TIMER_BEGIN("hkSweepBvTreeShape::queryObb");
	//turn obb into an aabb and query single aabb
	hkAabb aabb;
	hkArray<hkBroadPhaseHandlePair>	pairsOut;

	hkAabbUtil::calcAabb( obbTransform , obbExtent, tolerance , aabb );

	m_3dSweep->querySingleAabb( aabb , pairsOut );

	for (int i=0;i<pairsOut.getSize();i++)
	{
		const hkBroadPhaseHandlePair& pair = pairsOut[i];
		int index=(pair.m_b - &m_objectHandles[0]);
		hits.pushBack(m_shapeCollection->getKeyAt(index));
	}

	HK_INTERNAL_TIMER_END();


}

			/// Returns the hkShapeKey for all shapes in the hkShapeCollection that intersect with the obb (defined by obbTransform and obbExtent).
void hkSweepBvTreeShape::queryAabb( const hkAabb& aabb, hkReal tolerance, hkArray<hkShapeCollection::hkShapeKey>& hits ) const
{
	HK_INTERNAL_TIMER_BEGIN("hkSweepBvTreeShape::queryAabb");
	//turn obb into an aabb and query single aabb

	hkBroadPhaseHandle objectHandle;
	objectHandle.m_id = (void*)-1;

	hkInplaceArray<hkBroadPhaseHandlePair,128>	pairsOut;

	m_3dSweep->querySingleAabb( aabb , pairsOut );

	for (int i=0;i<pairsOut.getSize();i++)
	{
		const hkBroadPhaseHandlePair& pair = pairsOut[i];
		int index=(pair.m_b - &m_objectHandles[0]);

		hits.pushBack(m_shapeCollection->getKeyAt(index));
	}

	HK_INTERNAL_TIMER_END();


}


class hkSweepRayCastCallback : public hkRayCastCallback
{
public:
	hkSweepRayCastCallback (
		hk3AxisSweep*	sweepSpace , 
		const hkBroadPhaseHandle* broadphaseHandle, 
		const hkShapeCollection* collection, 
		const hkVector4& fromWorld,
		const hkVector4& toWorld,
		const hkShape::hkRayResults& initResults  );
	
	virtual hkReal	castPrimitive(const hkBroadPhaseHandle* broadphaseHandle);

	bool	hasHit();
private:
	hk3AxisSweep*	m_3dSweep;
	const hkBroadPhaseHandle* m_broadphaseHandle;
	const hkShapeCollection*	m_shapeCollection;
	bool	m_hasHit;
};


hkSweepRayCastCallback ::hkSweepRayCastCallback (	hk3AxisSweep*	sweepSpace , 
		const hkBroadPhaseHandle* broadphaseHandle, 
		const hkShapeCollection* collection, const hkVector4& fromWorld,const hkVector4& toWorld,const hkShape::hkRayResults& initResults  )
: hkRayCastCallback(fromWorld,toWorld,initResults),
m_3dSweep( sweepSpace ), m_broadphaseHandle (broadphaseHandle) , m_shapeCollection(collection),m_hasHit(false)
{

}

hkReal	hkSweepRayCastCallback ::castPrimitive(const hkBroadPhaseHandle* broadphaseHandle)
{
	//need to get the childshape to do the actual query !
	int index = broadphaseHandle - m_broadphaseHandle;
	hkShapeCollection::hkShapeKey key = m_shapeCollection->getKeyAt(index);
	if (m_shapeCollection->childShapeCastRay(key,m_fromWorld,m_toWorld,m_rayResults))
	{
		m_hasHit = true;
		m_rayResults.m_primary = key.m_primary;
		m_rayResults.m_secondary = key.m_secondary;
	}
	return m_rayResults.m_mindist;
}

bool	hkSweepRayCastCallback::hasHit()
{
	return m_hasHit;
}

int hkSweepBvTreeShape::castRay(const hkVector4& fromLocal,const hkVector4& toLocal,hkRayResults& results) const
{

	bool hasHit = false;

	hkSweepRayCastCallback  HK_ALIGNED_VARIABLE(raycallback,16) = hkSweepRayCastCallback( m_3dSweep, &m_objectHandles[0], getShapeCollection() , fromLocal , toLocal , results );
	
	m_3dSweep->castPrimitive( fromLocal , toLocal, raycallback );
	if ( raycallback.hasHit() )
	{
		results = raycallback.getRayResults();
		hasHit = 1;
	}
	
	return hasHit;
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
