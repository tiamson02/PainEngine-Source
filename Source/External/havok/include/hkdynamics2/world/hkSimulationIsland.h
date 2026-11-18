/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SIMULATION_ISLAND_H
#define HK_DYNAMICS2_SIMULATION_ISLAND_H

#include <hkcollide2/dispatch/hkCollidablePair.h>
#include <hkcollide2/dispatch/broadphase/hkBroadPhaseListener.h>
#include <hkdynamics2/constraint/hkSimpleConstraintOwner.h>

#include <hkbase/sort/hkUnionFind.h>

class hkAction;
class hkEntity;
class hkConstraint;
class hkStepInfo;
class hkBroadPhase;
class hkWorld;
class hkCollisionDispatcher;
struct hkDynamicsStepInfo;
class hkBroadPhaseHandlePair;
struct hkCollisionEnvironment;

///	These objects are internally managed by hkWorld. They are created and destroyed continuously during simulation.
/// At any one time, you can view the contents of the simulation islands, by calling hkWorld::getActiveSimulationIslands
/// and hkWorld::getInactiveSimulationIslands. However you must NOT keep any handles to the simulation islands, and you
/// must not alter anything in the simulation islands.
class hkSimulationIsland : public hkSimpleConstraintOwner, public hkBroadPhaseListener
{
	public:
			
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SIMISLAND)

			/// Get the list of entities in the simulation island
		inline const hkArray<hkEntity*>& getEntities() const;

			/// Get the list of actions in the simulation island
		inline const hkArray<hkAction*>& getActions() const;

			/// Get the list of constraints in the simulation island
		inline const hkArray<hkConstraint*>& getConstraints() const;

			/// Returns true if the island is active
		inline bool isActive() const;

			/// Returns true is the island is fixed
		inline bool isFixed() const;

			/// Get the owner of the world
		inline hkWorld* getWorld();

			/// gets the memory usage required to complete the next integration step on this island
			/// totalMemUse will return the total memory required.  
			/// primaryScratchUse report how much of the fixed size scratch pad is used
			/// newMemUse is the amount of memory that will be requested from the memory manager when
			/// integration is next performed on this island. 
			/// Use newMemUse to pass in any predicted increase of static buffer size allocated during 
			/// a previous islands integration.
			/// In most cases the input value of newMemUse will be subtracted from output value
		void getMemUsageForIntegration( int& totalMemUseOut, int& primaryScratchUseOut, int& newMemUseInOut );


			/// Advanced use only. Get the collision candidates of the simulation island
		inline hkArray<hkCollidablePair>& getCollisionCandidates();


		//
		// The remainder of the file is for Internal use only
		//

	protected:

		friend class hkWorld;
		friend class hkWorldCallbackUtil;
		friend class hkAddRemoveBroadPhaseListener;

		
		hkSimulationIsland( hkWorld* world );
		~hkSimulationIsland();

		void addEntity(hkEntity* entity);
		void removeEntity(hkEntity* entity);

		void addConstraint( hkConstraint* constraint );
		void removeConstraint( hkConstraint* constraint );

		void addAction( hkAction* act );
		void removeAction( hkAction* act );



		void addAndRemovePairs(hkTypedBroadPhaseHandlePair* newPairs, int numNewPairs, 
									hkTypedBroadPhaseHandlePair* deletedPairs, int numDeletedPairs );


		inline void addNewBroadPhasePair( hkBroadPhaseHandlePair& pair );
		inline void addOldBroadPhasePair( hkBroadPhaseHandlePair& pair );

	protected:
		

		virtual void addCollisionPair( hkTypedBroadPhaseHandlePair& pair );

		virtual void removeCollisionPair( hkTypedBroadPhaseHandlePair& pair );

		void createConnectivityGraph( hkArray<hkUnionFind::Edge>& edges );

		inline void addNewBroadPhasePair(const hkBroadPhaseHandlePair& n);

	public:

		void collideNarrowPhase( hkDynamicsStepInfo& env );

		void findPairCollision( hkDynamicsStepInfo& env, hkCollidablePair* p, struct hkCollisionResult& result, hkReal threshold );
		void collideNarrowPhaseBackstep( hkDynamicsStepInfo& env );

		void simulate(const hkDynamicsStepInfo& stepInfo);

		bool shouldDeactivate( const hkStepInfo& stepInfo );

		void collide( hkBroadPhase& broadPhase, hkDynamicsStepInfo& env );

		HK_FORCE_INLINE	void collideBroadPhase( hkBroadPhase& broadPhase, hkDynamicsStepInfo& env );

	private:

		HK_FORCE_INLINE void applyActions( const hkStepInfo& stepInfo );

		hkWorld*	m_world;
		int  m_storageIndex;

		bool m_splitCheckRequested:8;
		bool m_active:8;
		int  m_deactivationCounter:8;
		bool m_broadPhaseSynchronised:8;


		hkReal m_timeSinceLastHighFrequencyCheck;
		hkReal m_timeSinceLastLowFrequencyCheck;

		// the entities (note: optimized for single entities)
		hkInplaceArray<hkEntity*,1> m_entities;

		hkArray<hkAction*> m_actions;

		// the constraints: Note: a single object laying on the floor has 1 constraint
		hkInplaceArray<hkConstraint*,1> m_constraints;

		
		// for narrow phase
		hkArray<hkCollidablePair> m_collisionCandidates;


		// used to delay the creation of collision agents until necessary
		hkArray<hkBroadPhaseHandlePair> m_newBroadPhasePairs;


		// Used for adding fixed entities
		hkArray<hkBroadPhaseHandlePair> m_tempStorageBroadPhasePairs;
		hkArray<hkBroadPhaseHandlePair> m_oldBroadPhasePairs;

		// temporary pointers used in broad phase merging
		const hkCollisionEnvironment* m_currentCollisionEnvironment;
		hkCollidablePair* m_oldCandidates;
		hkCollidablePair* m_oldCandidatesEnd;
		hkCollidablePair* m_destCandidates;
	public:


		void isValid();
		inline int getStorageIndex();

};

HK_COMPILE_TIME_ASSERT( sizeof( hkSimulationIsland ) <= 180 );

#include <hkdynamics2/world/hkSimulationIsland.inl>


#endif // HK_DYNAMICS2_SIMULATION_ISLAND_H

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
