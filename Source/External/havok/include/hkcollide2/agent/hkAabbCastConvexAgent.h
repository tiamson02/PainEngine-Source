/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_AABBCAST_CONVEX_AGENT_H
#define HK_COLLIDE2_AABBCAST_CONVEX_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>

class hkCollisionDispatcher;

	/// This agent handles collisions between hkAabbCastShapes and hkConvexShapes.
	/// The agent performs a continuous aabbcast. The collidableB is expected to containt an hkAabbCastShape.
class hkAabbCastConvexAgent : public hkCollisionAgent
{
    public:
		
		///Registers the agent with the collision dispatcher.
        static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);

			// hkCollisionAgent interface implementation.
        virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);

			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

			// the closest distances are reported in the env.m_collisionResult
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, class hkClosestPointsDetails& details );
		
			// hkCollisionAgent interface implementation.
		virtual void cleanup( const hkCollisionEnvironment& env);

	protected:

		/// Constructor, called by the agent creation functions.
		hkAabbCastConvexAgent(const  hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

		/// Agent creation function used by the hkCollisionDispatcher. 
        static hkCollisionAgent* HK_CALL createAabbCastConvexAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
        
		/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createConvexAabbCastAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

	protected:
		hkInplaceArray<hkVector4,12>	m_planeEquations;
		
		// cached transform to detect movement, so we have to refresh the planeequations
		hkTransform			m_oldTrans;

		bool				m_initialized;
		
		
};

#endif // HK_COLLIDE2_AABBCAST_CONVEX_AGENT_H

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
