/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_COLLISION_DISPATCHER_H
#define HK_COLLIDE2_COLLISION_DISPATCHER_H

#include <hkcollide2/dispatch/hkCollidablePair.h>
#include <hkcollide2/agent/hkCollisionAgent.h>
#include <hkcollide2/shape/hkShapeTypes.h>

struct hkCollisionEnvironment;
class hkContactMgrFactory;

#define HK_MAX_RESPONSE_TYPE 8

/// Use this class to create a collision agents between two collidables
/// At initialization time, agents are registered with this class.
/// This class is generally used internally. For an example of its use, see the implementation
/// of hkCharacterPhantom
class hkCollisionDispatcher : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO)

		typedef hkCollisionAgent* (HK_CALL *CollisionAgentCreationFunction)(const hkCollidable& collidableA,
																	const hkCollidable& collidableB,
																	const hkCollisionEnvironment& environment);


	public:

		hkCollisionDispatcher( CollisionAgentCreationFunction defaultCollisionAgent, hkContactMgrFactory* defaultContactMgrFactory );

			/*
			**	Agents
			*/

			/// returns a new collision agent for a collideable pair
		hkCollisionAgent* getNewCollisionAgent(	const hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env) const;

			/// register an agent for a given pair of objects
		void registerCollisionAgent( CollisionAgentCreationFunction f, hkShapeType typeA, hkShapeType typeB );

			/// get the agent creation function registered for a given pair.  usefull for swapping the registered function  
		CollisionAgentCreationFunction getCollisionAgentCreationFunction( hkShapeType typeA, hkShapeType typeB );

			/// register this agent with all other shapes in the table, f goes along 'rows' and symmetricF along 'columns'
		void registerCollisionAgentWithAll(CollisionAgentCreationFunction f, CollisionAgentCreationFunction symmetricF, hkShapeType typeA );

			/*
			**	Contact Mgrs
			*/

			/// Get a contact manager factory - used to create a contact manager.
		hkContactMgrFactory* getContactMgrFactory(const hkCollisionResponse& responseA, const hkCollisionResponse& responseB) const;

			/// register a contact factory
		void registerContactMgrFactory( hkContactMgrFactory*, int responseA, int responseB );

			/// register a contact factory with all other types: Note overrides existing entries
		void registerContactMgrFactoryWithAll( hkContactMgrFactory*, int responseA );


	protected:

		~hkCollisionDispatcher();

	private:

		CollisionAgentCreationFunction* m_agentCreationTable;
		int m_numTypes;

		CollisionAgentCreationFunction	m_defaultCollisionAgent;

		hkContactMgrFactory*			m_contactMgrFactory[HK_MAX_RESPONSE_TYPE][HK_MAX_RESPONSE_TYPE];
};


#endif // HK_COLLIDE2_COLLISION_DISPATCHER_H

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
