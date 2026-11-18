/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_BV_TREE_SHAPE_AGENT_H
#define HK_COLLIDE2_BV_TREE_SHAPE_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>
#include <hkcollide2/shape/hkShapeCollection.h>

class hkCollisionDispatcher;


	/// This agent deals with collisions between hkBVTreeShapes and other shapes.  It traverses the bounding volume tree and dispatches 
	/// collision agents for those child shapes that are found to be collision candidates with the other shape.
class hkBvTreeShapeAgent : public hkCollisionAgent
{
	public:
			
			
			/// Registers this agent with the collision dispatcher.
		static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);
		
			// hkCollisionAgent interface implementation.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);

		// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, class hkClosestPointsDetails& details );

			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

			// hkCollisionAgent interface implementation.
		virtual void cleanup(	const hkCollisionEnvironment& env);


	public:
			// internal method
		typedef hkCollisionAgent* ( HK_CALL *agentCreateFunc) ( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env );

	protected:

		static hkCollisionAgent* HK_CALL defaultAgentCreate( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env );


		void prepareCollisionPartners(const hkCollidable& collidableA, const hkCollidable& collidableB, 
									const hkCollisionEnvironment& environment, agentCreateFunc agentFunc = defaultAgentCreate );

		/// Constructor, called by the agent creation function.
		hkBvTreeShapeAgent();

		///Destructor, called by cleanup().
		~hkBvTreeShapeAgent();

		struct hkBvAgentEntryInfo
		{
			hkShapeCollection::hkShapeKey m_key;
			hkCollisionAgent*	m_collisionAgent;	

			hkShapeCollection::hkShapeKey& getKey(){ return m_key; }

			void setKey( hkShapeCollection::hkShapeKey& key) { m_key = key; }
		};

		hkArray<hkBvAgentEntryInfo> m_collisionPartners;

		hkAabb	m_cachedAabb;

		/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createBvTreeShapeAgent( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
		
		/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createShapeBvAgent(	const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );
};

#endif // HK_COLLIDE2_BV_TREE_SHAPE_AGENT_H

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
