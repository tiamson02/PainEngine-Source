/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/shape/hkAabbCastShape.h>
#include <hkcollide2/contact/hkClosestPointsDetails.h>
#include <hkcollide2/algorithms/continuous/hkAabbConvexCast.h>
#include <hkcollide2/agent/hkAabbCastAabbCastAgent.h>

hkAabbCastAabbCastAgent::hkAabbCastAabbCastAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{

}


hkAabbCastAabbCastAgent::~hkAabbCastAabbCastAgent()
{

}


void hkAabbCastAabbCastAgent::registerAgent(hkCollisionDispatcher* dispatcher)
{
	dispatcher->registerCollisionAgent(&createAabbCastAabbCastAgent, HK_SHAPE_AABBCAST, HK_SHAPE_AABBCAST);
}


hkCollisionAgent* hkAabbCastAabbCastAgent::createAabbCastAabbCastAgent(	const hkCollidable& collidableA, 
														const hkCollidable& collidableB, 
														const hkCollisionEnvironment& environment)
{
	hkAabbCastAabbCastAgent* agent = new hkAabbCastAabbCastAgent(collidableA, collidableB, environment);
	return agent;
}


void hkAabbCastAabbCastAgent::cleanup( const hkCollisionEnvironment& environment )
{
	// delete agent
	delete this;
}

void hkAabbCastAabbCastAgent::processCollision(	const hkCollidable& collidableA,
						const hkCollidable& collidableB, 
						const hkCollisionEnvironment& environment,
						hkCollisionResult& result)
{

	// Process collision is not used for character controllers

}


void hkAabbCastAabbCastAgent::getClosestPoints(	const hkCollidable& collidableA,
						const hkCollidable& collidableB, 
						const hkCollisionEnvironment& environment,
						hkClosestPointsDetails& pointDetails)
{
	HK_TIMER_BEGIN("aabb-cast aabb-cast getClosestPoints", this);

	// collidable A
	const hkAabbCastShape* aabbCastShapeA = static_cast<const hkAabbCastShape*>(collidableA.getShape());
	const hkVector4& characterBoxHalfExtentsA = aabbCastShapeA->getHalfExtents();
	bool ignorePenetrationA = aabbCastShapeA->getIgnorePenetration();
	
	const hkAabbCastShape* aabbCastShapeB = static_cast<const hkAabbCastShape*>(collidableB.getShape());
	const hkVector4& characterBoxHalfExtentsB = aabbCastShapeB->getHalfExtents();
	bool ignorePenetration = aabbCastShapeB->getIgnorePenetration();

	hkInplaceArray<hkVector4, 6>	planeEquations(6);
	for (unsigned int i=0; i<3; i++)
	{
		hkVector4& plane = planeEquations[i];
		hkVector4& oppPlane = planeEquations[i+3];

		plane.set(0,0,0,0);
		oppPlane.set(0,0,0,0);

		plane(i) = 1;
		plane(3) = -characterBoxHalfExtentsB(i);
		oppPlane(i) = -1;
		oppPlane(3) = -characterBoxHalfExtentsB(i);
	}

	
	const hkVector4& fromWorldA = aabbCastShapeA->getFromWorld();
	const hkVector4& toWorldA = aabbCastShapeA->getToWorld();
	const hkVector4& fromWorldB = aabbCastShapeB->getFromWorld();
	const hkVector4& toWorldB = aabbCastShapeB->getToWorld();

	// Transform cast into space from A to B
	// Take into account that B may also be 'casting'
	hkVector4 fromWorld;
	fromWorld.setSub4(fromWorldA, fromWorldB);

	hkVector4 toWorld;
	toWorld.setSub4(toWorldA, fromWorldB);
	toWorld.add4(fromWorldB);
	toWorld.sub4(toWorldB);

	hkShape::hkRayResults& rayResults = aabbCastShapeA->getRayResults();
	
	// Do the cast
	{
		HK_TIMER_BEGIN( "cast aabb", HK_NULL );
		int res = hkAabbConvexCast::castAabb( fromWorld , toWorld , hkTransform::getIdentity().getRotation() , planeEquations , characterBoxHalfExtentsA , ignorePenetration,  rayResults );
		HK_TIMER_END();
		
		if (res)
		{
			hkClosestPointsDetails::ClosestPointEvent pointEvent;
			pointEvent.m_collidableA = &collidableA;
			pointEvent.m_collidableB = &collidableB;
			pointEvent.m_distance = rayResults.m_mindist;
			
			pointEvent.m_pointInWorld.setInterpolate4( fromWorld , toWorld , rayResults.m_mindist );
			pointEvent.m_normalInWorld = rayResults.m_unnormalizedHitNormal;
	
			pointDetails.addClosestPointCallback( pointEvent );
		}
		
	}
	HK_TIMER_END();

}


bool hkAabbCastAabbCastAgent::arePenetrating(const  hkCollidable& collidableA, const 	hkCollidable& collidableB, const hkCollisionEnvironment& environment )
{
	HK_TIMER_BEGIN("aabb-cast aabb-cast arePenetrating", this);

	// collidable A
	const hkAabbCastShape* aabbCastShapeA = static_cast<const hkAabbCastShape*>(collidableA.getShape());
	const hkVector4& characterBoxHalfExtentsA = aabbCastShapeA->getHalfExtents();
	bool ignorePenetrationA = aabbCastShapeA->getIgnorePenetration();
	
	const hkAabbCastShape* aabbCastShapeB = static_cast<const hkAabbCastShape*>(collidableB.getShape());
	const hkVector4& characterBoxHalfExtentsB = aabbCastShapeB->getHalfExtents();
	bool ignorePenetration = aabbCastShapeB->getIgnorePenetration();

	hkInplaceArray<hkVector4, 6>	planeEquations(6);
	for (unsigned int i=0; i<3; i++)
	{
		hkVector4& plane = planeEquations[i];
		hkVector4& oppPlane = planeEquations[i+3];

		plane.set(0,0,0,0);
		oppPlane.set(0,0,0,0);

		plane(i) = 1;
		plane(3) = -characterBoxHalfExtentsB(i);
		oppPlane(i) = -1;
		oppPlane(3) = -characterBoxHalfExtentsB(i);
	}

	
	const hkVector4& fromWorldA = aabbCastShapeA->getFromWorld();
	const hkVector4& toWorldA = aabbCastShapeA->getToWorld();
	const hkVector4& fromWorldB = aabbCastShapeB->getFromWorld();
	const hkVector4& toWorldB = aabbCastShapeB->getToWorld();

	// Transform cast into space from A to B
	// Take into account that B may also be 'casting'
	hkVector4 fromWorld;
	fromWorld.setSub4(fromWorldA, fromWorldB);

	hkVector4 toWorld;
	toWorld.setSub4(toWorldA, fromWorldB);
	toWorld.add4(fromWorldB);
	toWorld.sub4(toWorldB);

	hkShape::hkRayResults& rayResults = aabbCastShapeA->getRayResults();

	// Do the cast
	HK_TIMER_BEGIN( "cast aabb", HK_NULL );
	int res = hkAabbConvexCast::castAabb( fromWorld , toWorld , hkTransform::getIdentity().getRotation() , planeEquations , characterBoxHalfExtentsA , ignorePenetration,  rayResults );
	HK_TIMER_END();

	HK_TIMER_END();

	// If res returns true => penetrating
	if (res)
	{
		return true;
	}

	return false;
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
