/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/simulation/hkConvexSweepConvexAgent.h>
#include <hkutilities2/simulation/hkConvexSweepShape.h>
#include <hkcollide2/hkCollide2.h>

void HK_CALL hkConvexSweepConvexAgent::registerAgent(hkCollisionDispatcher* dispatcher)
{
	dispatcher->registerCollisionAgent(createConvexSweepConvexAgent, HK_SHAPE_USER7, HK_SHAPE_CONVEX);
	dispatcher->registerCollisionAgent(createConvexSweepConvexAgent, HK_SHAPE_CONVEX, HK_SHAPE_USER7);
	dispatcher->registerCollisionAgent(createConvexSweepConvexAgent, HK_SHAPE_USER7, HK_SHAPE_USER7);
}


hkCollisionAgent* HK_CALL hkConvexSweepConvexAgent::createConvexSweepConvexAgent(	const hkCollidable& collidableA, 
																				const hkCollidable& collidableB, 
																				const hkCollisionEnvironment& environment)
{
	hkConvexSweepConvexAgent* agent = new hkConvexSweepConvexAgent();
	return agent;
}


void hkConvexSweepConvexAgent::processCollision(	const hkCollidable& collidableA,
						const hkCollidable& collidableB, 
						const hkCollisionEnvironment& environment,
						hkCollisionResult& result)
{
	if ( collidableA.m_shape->getPrimaryType() == HK_SHAPE_USER7 )
	{
		if (static_cast<const hkConvexSweepShape*>( collidableA.m_shape )->m_wantReset == true )
		{
			m_cache.reset();
		}
	}

	if ( collidableB.m_shape->getPrimaryType() == HK_SHAPE_USER7 )
	{
		if (static_cast<const hkConvexSweepShape*>( collidableB.m_shape )->m_wantReset == true )
		{
			m_cache.reset();
		}
	}

	hkConvexConvexAgent::processCollision( collidableA, collidableB, environment, result );
	
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
