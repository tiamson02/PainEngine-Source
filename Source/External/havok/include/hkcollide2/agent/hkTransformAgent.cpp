/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/agent/hkTransformAgent.h>
#include <hkcollide2/shape/hkTransformShape.h>
#include <hkcollide2/agent/hkSymmetricAgent.h>

#include <hkmath2/linear/hkVector4Util.h>
#include <hkcollide2/manifold/hkCollisionFeature.h>

void HK_CALL hkTransformAgent::registerAgent(hkCollisionDispatcher* dispatcher)
{
	dispatcher->registerCollisionAgent(&createTransformAAgent, HK_SHAPE_TRANSFORM, HK_SHAPE_ALL);
	dispatcher->registerCollisionAgent(&createTransformBAgent, HK_SHAPE_ALL, HK_SHAPE_TRANSFORM);
}


hkCollisionAgent* HK_CALL hkTransformAgent::createTransformAAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
	hkTransformAgent* agent = new hkTransformAgent( collA, collB, env );
	return agent;
}

void hkTransformAgent::cleanup(	const hkCollisionEnvironment& environment)
{
	m_childAgent->cleanup( environment );
	delete this;
}

hkTransformAgent::hkTransformAgent(const hkCollidable& collAIn, const hkCollidable& collB, const hkCollisionEnvironment& env)
{
	const hkTransformShape* tShapeA = static_cast<const hkTransformShape*>(collAIn.m_shape);
	const hkShape* childShape = tShapeA->getChildShape();

	hkMotionState ms = *collAIn.getMotionState();

	ms.m_sceneGraphTransform1.setMul(*collAIn.getTransform(), tShapeA->getTransform());

	hkCollidable collA = collAIn;
	
	collA.m_motionState = &ms;
	collA.m_shape = tShapeA->getChildShape();

	m_childAgent = env.m_dispatcher->getNewCollisionAgent( collA, collB, env );
}


hkCollisionAgent* HK_CALL hkTransformAgent::createTransformBAgent(const hkCollidable& collA, const hkCollidable& collB, 
														const hkCollisionEnvironment& env)
{
	hkTransformAgent* agent = new hkSymmetricAgent<hkTransformAgent>(collA, collB, env);
	return agent;
}


void hkTransformAgent::processCollision(const hkCollidable& collAin, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result)
{
	HK_INTERNAL_TIMER_BEGIN( "transform" );

	const hkTransformShape* tShapeA = static_cast<const hkTransformShape*>(collAin.m_shape);
	
	hkMotionState ms = *collAin.getMotionState();

	ms.m_sceneGraphTransform1.setMul(*collAin.getTransform(), tShapeA->getTransform());

	hkCollidable collA = collAin;

	collA.m_motionState = &ms;
	collA.m_shape = tShapeA->getChildShape();

	m_childAgent->processCollision( collA, collB, env, result );

	HK_INTERNAL_TIMER_END();
}

		// hkCollisionAgent interface implementation.
void hkTransformAgent::getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails)
{
	HK_INTERNAL_TIMER_BEGIN( "transform/get closest points" );

	const hkTransformShape* tShapeA = static_cast<const hkTransformShape*>(collidableA.m_shape);
	
	hkMotionState ms = *collidableA.getMotionState();

	ms.m_sceneGraphTransform1.setMul(*collidableA.getTransform(), tShapeA->getTransform());

	hkCollidable collA = collidableA;

	collA.m_motionState = &ms;
	collA.m_shape = tShapeA->getChildShape();

	m_childAgent->getClosestPoints( collA, collidableB, env, pointDetails);

	HK_INTERNAL_TIMER_END();
}


			/// hkAgent interface implementation
bool hkTransformAgent::arePenetrating( const hkCollidable& collAin, const hkCollidable& collB, const hkCollisionEnvironment& env )
{
	HK_INTERNAL_TIMER_BEGIN( "transform/are penetrating" );

	const hkTransformShape* tShapeA = static_cast<const hkTransformShape*>(collAin.m_shape);
	
	hkMotionState ms = *collAin.getMotionState();
	ms.m_sceneGraphTransform1.setMul(*collAin.getTransform(), tShapeA->getTransform());


	hkCollidable collA = collAin;

	collA.m_motionState = &ms;
	collA.m_shape = tShapeA->getChildShape();

	HK_INTERNAL_TIMER_END();
	return m_childAgent->arePenetrating( collA, collB, env );
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
