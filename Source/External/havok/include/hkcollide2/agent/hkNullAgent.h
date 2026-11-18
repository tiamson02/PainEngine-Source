/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_NULL_AGENT_H
#define HK_COLLIDE2_NULL_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>

	/// A null collision agent does nothing and can be used to disable collisions between objects.
/// It is used as a default agent by the hkCollisionDispatcher
/// if no appropriate hkCollisionAgent can be found based on the shape types of a pair of objects.
class hkNullAgent : public hkCollisionAgent
{
	public:

			/// This function does nothing in this agent.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result){}

			/// This function does nothing in this agent.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env ){ return false; }

			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, hkClosestPointsDetails& pointDetails); 
	
			// hkCollisionAgent interface implementation.
		virtual void cleanup(const hkCollisionEnvironment& env){}

			/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createNullAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);

};

#endif // HK_COLLIDE2_NULL_AGENT_H

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
