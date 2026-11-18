/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_MULTI_SPHERE_TRIANGLE_AGENT_H
#define HK_COLLIDE2_MULTI_SPHERE_TRIANGLE_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>
#include <hkcollide2/shape/hkMultiSphereShape.h>
#include <hkcollide2/algorithms/util/hkCollideTriangleUtil.h>

class hkCollisionDispatcher;

	/// This agent deals with collisions between hkMultiSphereShapes and hkTriangles.
class hkMultiSphereTriangleAgent : public hkCollisionAgent
{
    public:

			///Registers the agent with the collision dispatcher.
        static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);

			// hkCollisionAgent interface implementation.
        virtual inline void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);

			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env );

			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails); 
	
			// hkCollisionAgent interface implementation.
		virtual void cleanup( const hkCollisionEnvironment& env);

	protected:

		/// Constructor, called by the agent creation functions.
		hkMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

		/// Agent creation function used by the hkCollisionDispatcher. 
        static hkCollisionAgent* HK_CALL createMultiSphereTriangleAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
        
		/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createTriangleMultiSphereAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

	private:
		hkReal			m_invTriangleNormalLength;
		hkContactPointId m_contactPointId[hkMultiSphereShape::MAX_SPHERES];
		hkCollideTriangleUtil::ClosestPointTriangleCache m_closestPointTriangleCache;

};

#endif // HK_COLLIDE2_MULTI_SPHERE_TRIANGLE_AGENT_H

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
