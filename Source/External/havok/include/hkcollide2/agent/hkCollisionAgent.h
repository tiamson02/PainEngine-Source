/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLISION_AGENT_H
#define HK_COLLIDE2_COLLISION_AGENT_H

#include <hkmath2/hkMath.h>

class hkCollidable;
class hkCollisionDispatcher;

struct hkCollisionEnvironment;
struct hkCollisionResult;
class hkClosestPointsDetails;

	/// An interface for classes that handle pairwise narrowphase collision detection. 
	/// Each hkCollisionAgent implementation handles collision detection for a different combination of hkShape types. 
	/// You must register a collision agent with the collision dispatcher if you want to use it. 
class hkCollisionAgent : public hkBaseObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT);

			/// Performs the collision detection.
		virtual void processCollision( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkCollisionResult& result ) = 0;

			/// Checks whether the two objects are penetrating. 
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env ) = 0;

			/// the closest distances are reported in the env.m_collisionResult
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, class hkClosestPointsDetails& details ) =0;
		
			/// Called if the agent is no longer being used.
		virtual void cleanup( const hkCollisionEnvironment& env ){}

	protected:
			/// Called by the cleanup function.
		inline virtual ~hkCollisionAgent() {}

			/// Called by a static creation function that is registered with the hkCollisionDispatcher.
		inline hkCollisionAgent() {}
};

#endif // HK_COLLIDE2_COLLISION_AGENT_H

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
