/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONVEX_SWEEP_CONVEX_AGENT_H
#define HK_COLLIDE2_CONVEX_SWEEP_CONVEX_AGENT_H

#include <hkcollide2/agent/hkConvexConvexAgent.h>

class hkCollisionDispatcher;

	/// You must register this agent if you are using the hkConvexSweepShape. 
	/// This agent is not automatically registered by hkAgentRegisterUtil::registerAllAgents()
class hkConvexSweepConvexAgent : public hkConvexConvexAgent
{
	public:

			/// Registers the agent with the collision dispatcher.
		static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);

			/// hkCollisionAgent interface implementation.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);
			
	public:
			/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createConvexSweepConvexAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env);
	
};

#endif // HK_COLLIDE2_CONVEX_SWEEP_CONVEX_AGENT_H

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
