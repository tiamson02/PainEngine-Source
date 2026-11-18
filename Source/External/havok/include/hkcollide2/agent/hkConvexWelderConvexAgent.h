/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_WELDER_CONVEX_AGENT_H
#define HK_COLLIDE2_CONVEX_WELDER_CONVEX_AGENT_H

#include <hkcollide2/agent/hkBvTreeShapeAgent.h>

class hkCollisionDispatcher;
class hkContactPoint;
class hkSimplerClosestPointDetails;

	/// This agent handles collisions between hkConvexWelderShape and other shapes.
	/// trying as best as it can to eliminate the "internal edge" problem
class hkConvexWelderConvexAgent : public hkBvTreeShapeAgent
{
	public:

			/// Registers this agent with the collision dispatcher.
		static void registerAgent(hkCollisionDispatcher* dispatcher);

			// hkCollisionAgent interface implementation.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
		
			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails); 
	
			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

	protected:

			/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* createConvexWelderAAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
		static hkCollisionAgent* createConvexWelderBAgent(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

		inline bool checkForCulling( const hkContactPoint& culler, const hkContactPoint& cullee ) const;
		int checkAllForCulling( const hkContactPoint& culler, hkSimplerClosestPointDetails& candidatesForCulling, int startIdx, int endIdx ) const; 

};



#endif // HK_COLLIDE2_CONVEX_WELDER_AGENT_H

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
