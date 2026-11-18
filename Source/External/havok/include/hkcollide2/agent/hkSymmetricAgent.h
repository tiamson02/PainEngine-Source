/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_SYMMETRIC_AGENT_H
#define HK_COLLIDE2_SYMMETRIC_AGENT_H

class hkCollidable;

#include <hkcollide2/contact/hkCollisionResults.h>
#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkcollide2/manifold/hkCollisionFeature.h>

	/// A symmetric agent is used to wrap a non symmetric agent with its mirrored type. The collision dispatcher
	/// uses this class when creating agents that deal with two different shape types - for instance, the hkSphereTriangleAgent.
template<typename AGENT>
class hkSymmetricAgent : public AGENT
{
	public:
		inline hkSymmetricAgent(){}

			/// Called by the appropriate createAgent() function.
		inline hkSymmetricAgent( const hkCollidable& A, const hkCollidable& B );

			/// Called by the appropriate createAgent() function.
		inline hkSymmetricAgent( const hkCollidable& A, const hkCollidable& B, const hkCollisionEnvironment& env );

			/// hkCollisionAgent interface forward call.
		virtual void cleanup(	const hkCollisionEnvironment& env);

			// hkCollisionAgent interface forward call.
		virtual void processCollision(	const hkCollidable& collA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkCollisionResult& result);

			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails); 
	
		// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

};

#include <hkcollide2/agent/hkSymmetricAgent.inl>

#endif // HK_COLLIDE2_SYMMETRIC_AGENT_H

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
