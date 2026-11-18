/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_CONVEX_AGENT_H
#define HK_COLLIDE2_CONVEX_CONVEX_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>
#include <hkcollide2/algorithms/gjk/hkGjkCache.h>
#include <hkcollide2/manifold/hkClosestPointManifold.h>

class hkCollisionDispatcher;

	/// A hkConvexConvexAgent handles collision detection between pairs of hkConvexShapes.
class hkConvexConvexAgent : public hkCollisionAgent
{
	public:
			

			/// Registers the agent with the collision dispatcher.
		static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);

			/// hkCollisionAgent interface implementation.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
			
			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails); 
		
			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

			// hkCollisionAgent interface implementation.
		virtual void cleanup(	const hkCollisionEnvironment& env);

	public:
			/// Constructor, called by the createConvexConvexAgent() function.
		hkConvexConvexAgent();

			/// Destructor, called by cleanup().
		~hkConvexConvexAgent();

			/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createConvexConvexAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
 
	public:
			// get a single point in a raw format
		bool  getClosestPoint( const hkCollidable& collidableA, const hkCollidable& collidableB, struct hkGjkOut& output,hkReal&	dist,hkVector4& normalInWorld,	const hkCollisionEnvironment& environment);
		HK_FORCE_INLINE bool  getClosestPointInl( const hkCollidable& collidableA, const hkCollidable& collidableB, struct hkGjkOut& output,hkReal&	dist,hkVector4& normalInWorld,	const hkCollisionEnvironment& environment);

	public:
		hkVector4	m_oldNormal;

		hkGjkCache m_cache;
		hkClosestPointManifold m_bridgeManifold;

	
};

#endif // HK_COLLIDE2_CONVEX_CONVEX_AGENT_H

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
