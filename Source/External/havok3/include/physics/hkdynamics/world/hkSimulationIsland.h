/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SIMULATION_ISLAND_H
#define HK_DYNAMICS2_SIMULATION_ISLAND_H

#include <hkinternal/collide/agent3/machine/nn/hkAgentNnTrack.h>
#include <hkcollide/dispatch/broadphase/hkBroadPhaseListener.h>
#include <hkdynamics/constraint/hkSimpleConstraintOwner.h>
#include <hkconstraintsolver/accumulator/hkRigidAccumulator.h>
#include <hkdynamics/world/hkMultiThreadLock.h>

class hkUnionFind;

class hkAction;
class hkEntity;
class hkConstraintInstance;
class hkStepInfo;
class hkBroadPhase;
class hkWorld;
class hkCollisionDispatcher;
struct hkWorldDynamicsStepInfo;
class hkBroadPhaseHandlePair;
struct hkCollisionInput;
struct hkProcessCollisionOutput;

///	These objects are internally managed by hkWorld. They are created and destroyed continuously during simulation.
/// At any one time, you can view the contents of the simulation islands, by calling hkWorld::getActiveSimulationIslands
/// and hkWorld::getInactiveSimulationIslands. However you must NOT keep any handles to the simulation islands, and you
/// must not alter anything in the simulation islands.
class hkSimulationIsland : public hkSimpleConstraintOwner
{
	public:
			
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SIMISLAND);

			/// Get the list of entities in the simulation island
		inline const hkArray<hkEntity*>& getEntities() const;

			/// Get the list of actions in the simulation island
		inline const hkArray<hkAction*>& getActions() const;

			/// Returns true if the island is active. 
			/// Tech info: returns true if the island is active in the next integration step. 
			///            To see whether the island actually was integrated in the last PSI or TOI step,
			///            check hkSimulationIsland::m_isInActiveIslandsArray.
		inline hkBool isActive() const;

			/// Returns true is the island is fixed
		inline hkBool isFixed() const;

			/// Get the world
		inline hkWorld* getWorld();

			/// Gets the memory usage required to complete the next integration step on this island.
			/// This function is called internally to prevent the memory usage breaching the critical memory limit in
			/// hkMemory.
		inline int getMemUsageForIntegration();

		


		void calcStatistics( hkStatisticsCollector* collector) const;


		//
		// The remainder of the file is for Internal use only
		//

	protected:

		friend class hkWorld;
		friend class hkWorldOperationUtil;
		friend class hkWorldCallbackUtil;
		friend class hkDiscreteSimulation;
		friend class hkContinuousSimulation;

		
		hkSimulationIsland( hkWorld* world );
		~hkSimulationIsland();
			
			// Properly sets storage indices and pointers on the entity. Updates island's data with entity's motion info.
		void internalAddEntity(hkEntity* entity);

			// Properly sets storage indices and pointers on the entity. Updates island's data with entity's motion info.
		void internalRemoveEntity(hkEntity* entity);

			// hkConstraintOwner interface implementation
		virtual void removeConstraint( hkConstraintInstance* constraint);

		void addAction( hkAction* act );
		void removeAction( hkAction* act );

	protected:

		hkBool isFullyConnected( hkUnionFind& checkConnectivityOut );


	public:
			// Checks deactivators for all entities and says (return hkBool value) whether the island is OK for deactivation.
		hkBool shouldDeactivate( const hkStepInfo& stepInfo );


	public:

		inline hkMultiThreadLock& getMultiThreadLock();
		inline const hkMultiThreadLock& getMultiThreadLock() const;

		hkWorld*	m_world;

			// Storage index of the island: either in hkWorld::m_activeSimulationIslands or hkWorld::m_inactiveSimulationIslands
			// depending on m_isInActiveIslandsArray
		hkObjectIndex m_storageIndex;

			// If the island is dirty, than the island is also stored in the hkWorld::m_dirtySimulationIslands
			// dirty islands are cleaned up in hkWorldOperationUtil::cleanupDirtyIslands
		hkObjectIndex m_dirtyListIndex;

		hkUchar m_highFrequencyDeactivationCounter;
		hkUchar m_lowFrequencyDeactivationCounter;

		hkBool m_splitCheckRequested;
		hkBool m_actionListCleanupNeeded;

			// desired Activation State
		hkBool m_active;

			// see m_storageIndex
		hkBool m_isInActiveIslandsArray; 

		hkMultiThreadLock m_multiThreadLock;

		hkReal m_timeSinceLastHighFrequencyCheck;
		hkReal m_timeSinceLastLowFrequencyCheck;

			// the entities (note: optimized for single entities)
		hkInplaceArray<hkEntity*,1> m_entities;

			// for narrow phase collision agents
		hkAgentNnTrack m_agentTrack;

		hkArray<hkAction*> m_actions;

		hkTime m_timeOfDeactivation;

	public:
#	if defined HK_DEBUG
		static hkBool debugOnly_isSameIslandOrToBeMerged( hkWorld* world, hkSimulationIsland*islandA, hkSimulationIsland*islandB );
#	endif
		static hkSimulationIsland* getActiveSimulationIsland( hkEntity* entityA, hkEntity* entityB);

		void isValid();
		inline int getStorageIndex();
};

#include <hkdynamics/world/hkSimulationIsland.inl>

#endif // HK_DYNAMICS2_SIMULATION_ISLAND_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
