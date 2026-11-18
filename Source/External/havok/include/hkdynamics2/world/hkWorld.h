/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_WORLD_H
#define HK_DYNAMICS2_WORLD_H

#include <hkmath2/hkMath.h>
#include <hkdynamics2/common/hkDynamicsStepInfo.h>

//the following include is needed, as otherwise a forward declaration
//of an enum would have to occur (hkSolverType)
#include <hkdynamics2/world/hkWorldConstructionInfo.h>
#include <hkdynamics2/collide/hkCollisionListener.h>

//the following include is needed as it includes the define of the current version number
#include <hkbase/config/hkConfigVersion.h>

class hkEntity;
class hkAction;
class hkRigidBody;
class hkConstraint;
class hkEntityListener;
class hkCollisionDispatcher;
class hkSimulationIsland;
class hkCollisionFilter;
class hkContactMgrFactory;
class hkBroadPhase;
class hkCollidable;
struct hkCollisionEnvironment;

class hkActionListener;
class hkEntityListener;
class hkConstraintListener;
class hkWorldPostSimulationListener;
class hkWorldPostDetectionListener;
class hkWorldDeletionListener;
class hkCollisionListener;
class hkIslandActivationListener;
class hkTypedBroadPhaseHandler;
class hkPhantomBroadPhaseListener;
class hkIslandPostSimulationListener;
class hkIslandPostDetectionListener;
class hkPhantom;
class hkSecondaryScratchPad;
class hkStepInfo;
class hkCollidablePair;
class hkWorldPairwiseBackstepFilter;


/// The hkWorld is a container for the simulation's physical objects.  It also steps the simulation forward in time.
/// You add elements (including rigid bodies, actions, constraints, and listeners) to the simulation by adding them to the hkWorld.
class hkWorld : public hkReferencedObject
{

	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_WORLD)

			/// Creates a world with the given construction information
		hkWorld(const hkWorldConstructionInfo& info, unsigned int sdkversion = HAVOK_VERSION_NUMBER );

			/// Get construction info from this instance
		void getConstructionInfo(hkWorldConstructionInfo& info);

			//
			//	Entity addition and removal 
			//

			/// Adds the specified entity to the world and returns it
		hkEntity* addEntity( hkEntity* entity );

			/// efficiently add a group of entities at the same time.  Will not be as efficient when
			/// adding a small number of entities, in that case use addEntity.  Only for non-fixed entities
		void addNonFixedEntityBatch( hkArray< hkEntity* > &entityBatch );

			/// Removes the specified entity from the world 
			/// During a simulation step the removal of the entity is delayed until it is safe to do so.
			/// returns true if removed immediately false if delayed
		bool removeEntity( hkEntity* entity );

			/// efficiently remove a group of entities at the same time.  Will not be as efficient when
			/// removing a small number of entities, in that case use removeEntity.  Only for non-fixed entities
		void removeNonFixedEntityBatch( hkArray< hkEntity* > &entityBatch );

			/// Gets the world's fixed rigid body. This is a fixed body that you can use
			/// to constrain objects to a position in world space.
		hkRigidBody* getFixedRigidBody();


			//
			//	Constraint addition and removal
			//

			/// Adds the specified constraint to the world
		hkConstraint* addConstraint( hkConstraint* constraint );

			/// Removes the specified constraint from the world
			/// returns true if removed immediately false if delayed
		bool removeConstraint( hkConstraint* constraint );



			//
			// Actions
			//
			
			/// Adds the specified action to the world.
		hkAction* addAction( hkAction* action ); 

			/// Remove the specified action from the world.
		void removeAction( hkAction* action );	


			//
			// Phantoms
			//

			/// Add the phantom to the world
		hkPhantom* addPhantom( hkPhantom* phantom );

			/// Add multiple phatoms to the world
		void addPhantomBatch( const hkArray<hkPhantom*>& phantoms );


			/// Remove the phantom from the world
		void removePhantom( hkPhantom* phantom );

			/// Remove mulitple phantoms from the world
		void removePhantomBatch( const hkArray<hkPhantom*>& phantoms );


			/// Get a list of all the phantoms which have been added to the world
		inline const hkArray<hkPhantom*>& getPhantoms() const;


			//
			// Gravity
			//

			/// Sets the gravity value for the world.
		void setGravity( const hkVector4& gravity);

			/// Gets the world's gravity value.
		inline const hkVector4& getGravity();


			//
			// Simulation Control
			//

			/// Steps all subspaces by the desired delta time.
			/// This will increase the current time by deltaTime.
		void stepDeltaTime(hkReal deltaTime);
		
			/// gets the memory usage required to complete the next integration step
			/// totalMemUse will return the total memory required.  
			/// primaryScratchUse reports how much of the fixed-size scratch pad is used
			/// newMemUse is the amount of memory that will be requested from the memory manager when
			/// integration is next performed
		void getMemUsageForIntegration( int& totalMemUse, int& primaryScratchUse, int& newMemUse );


			//
			// Event Handling
			//

			/// Adds the specified listener to the world
		void addActionListener( hkActionListener* worldListener );

			/// Removes the specified listener from the world
		void removeActionListener( hkActionListener* worldListener );

			/// Adds the specified listener to the world
		void addConstraintListener( hkConstraintListener* worldListener );

			/// Removes the specified listener from the world
		void removeConstraintListener( hkConstraintListener* worldListener );

			/// Adds the specified listener to the world
		void addEntityListener( hkEntityListener* worldListener );

			/// Removes the specified listener from the world
		void removeEntityListener( hkEntityListener* worldListener );

			/// Adds a listener which listens to ALL entity activations.
		void addIslandActivationListener( hkIslandActivationListener* worldListener );

			/// Removes the specified listener from the world
		void removeIslandActivationListener( hkIslandActivationListener* worldListener );

			/// Adds the specified listener to the world
		void addWorldPostDetectionListener( hkWorldPostDetectionListener* worldListener );

			/// Removes the specified listener from the world
		void removeWorldPostDetectionListener( hkWorldPostDetectionListener* worldListener );

			/// Adds the specified listener to the world
		void addWorldPostSimulationListener( hkWorldPostSimulationListener* worldListener );

			/// Removes the specified listener from the world
		void removeWorldPostSimulationListener( hkWorldPostSimulationListener* worldListener );

			/// Adds the specified listener to the world
		void addIslandPostDetectionListener( hkIslandPostDetectionListener* islandListener );

			/// Removes the specified listener from the world
		void removeIslandPostDetectionListener( hkIslandPostDetectionListener* islandListener );

			/// Adds the specified listener to the world
		void addIslandPostSimulationListener( hkIslandPostSimulationListener* islandListener );

			/// Removes the specified listener from the world
		void removeIslandPostSimulationListener( hkIslandPostSimulationListener* islandListener );

			/// Adds a listener which listens to ALL collisions.
		void addCollisionListener( hkCollisionListener* worldListener );

			/// Removes a world collision listener. 
		void removeCollisionListener( hkCollisionListener* worldListener);

			/// Add a listener which recieves events whent the world is deleted
		void addWorldDeletionListener( hkWorldDeletionListener* worldListener );

			/// Remove a listener which recieves events whent the world is deleted
		void removeWorldDeletionListener( hkWorldDeletionListener* worldListener );


			//
			// Collision detection
			//

			/// Gets the broad phase owned by the world.
		inline hkBroadPhase* getBroadPhase();

			/// Get the broad phase handler - this is a class which links the broad phase to the simulation
		inline hkTypedBroadPhaseHandler* getBroadPhaseHandler();

			/// Get the collision environment. This is needed to manually query collision agents.
		inline hkCollisionEnvironment* getCollisionEnvironment();

			/// Gets the collision dispatcher owned by the world.
		inline hkCollisionDispatcher* getCollisionDispatcher();

			/// Gets the collision filter.
		hkCollisionFilter* getCollisionFilter();

			/// Set the collision filter. This is used by the world by default when it calls the collision detector.
			/// WARNING: This can only be done before any objects are added to this world.
		void setCollisionFilter( hkCollisionFilter* filter);

			/// This method can be called to update the collision pairs for a given entity. If you chage the collision filter
			/// info for an entity, the current overlapping entities are not automatically updated - this call will do that.
		void updateCollisionFilter( hkEntity* entity );

			/// This updates the collision pairs ( based on the current collision filter ) for a given simulation island
			/// see comments for hkWorld::updateCollisionFilter() for more info.
		void updateCollisionFilterOnIsland( hkSimulationIsland* island );

			/// This updates the collision pairs ( based on the current collision filter ) for all active and inactive simulation islands
			/// WARNING: This is a very expensive function to call!
			/// see comments for hkWorld::updateCollisionFilter() for more info.
		void updateCollisionFilterOnAll();


			/// Casts a ray (specified by fromWorld and toWorld) and returns the first hit object, and fills in the hitNormalWorld and mindist values.
		hkCollidable* castRay(const hkVector4& fromWorld, const hkVector4& toWorld, hkVector4& hitNormalWorld, float& mindist);



			//
			// Deactivation
			//
		/// Gets the high frequency deactivation period.
		inline hkReal getHighFrequencyDeactivationPeriod() const;

		/// Sets the high frequency deactivation period.
		inline void setHighFrequencyDeactivationPeriod( hkReal period );

		/// Gets the low frequency deactivation period.
		inline hkReal getLowFrequencyDeactivationPeriod() const;

		/// Sets the low frequency deactivation period.
		inline void setLowFrequencyDeactivationPeriod( hkReal period );

		/// Deactivates the specified entity.
		void deactivateEntity( hkEntity* entity );


			//
			// Simulation Island access
			//

			/// Get the active simulation islands. You can use this function to access all the active entities, constraints and actions.
			/// NOTE: This should not be used during simulation, as it will generally result in sub-optimal performance.
		inline const hkArray<hkSimulationIsland*>& getActiveSimulationIslands() const;

			/// Get the inactive simulation islands. You can use this function to access all the inactive entities, constraints and actions.
			/// NOTE: This should not be used during simulation, as it will generally result in sub-optimal performance.		
		inline const hkArray<hkSimulationIsland*>& getInactiveSimulationIslands() const;

			/// Get the fixed simulation island. You can use this function to access all the fixed entities.
			/// There is only one fixed island, and it contains no actions or constraints.
			/// NOTE: This should not be used during simulation, as it will generally result in sub-optimal performance.
		inline const hkSimulationIsland* getFixedIsland() const;

			/// Advances the state of the simulation by the amount of time described by the stepInfo instance.
		void simulate( const hkStepInfo& stepInfo );

			/// Performs collision detection on the whole world for a given stepInfo delta time.
			/// For internal use only, use with care.
		void collide( const hkStepInfo& stepInfo );

			/// Integrates the state of the entities in the simulation, advancing them by the amount of time described by the stepInfo instance.
			/// For internal use only, use with care.
		void integrate( const hkStepInfo& stepInfo );
		

		//
		// Backstepping control
		//
			/// Enable pairwise backstepping. This is disabled by default.
		void enableBackstepping();

			/// Disable pairwise backstepping. This is disabled by default
		void disableBackstepping();

			/// Get the pairwise backstepping filter. The default filter enables backstepping between two objects 
			/// where a fixed object is involved in the collision.
		hkWorldPairwiseBackstepFilter* getPairwiseBackstepFilter();

			/// Set the pairwise backstepping filter. The default filter enables backstepping between two objects 
			/// where a fixed object is involved in the collision.  You can change this to disable backstepping for
			/// unimportant objects, for example, or to enable backstepping for other objects.
		void setPairwiseBackstepFilter( hkWorldPairwiseBackstepFilter* filter );


	protected:

		/*
		**	Protected Functions
		*/

		friend class hkSimpleConstraintContactMgr;

		~hkWorld();	// use removeReference() instead


		/*
		**	construction members
		*/


			// sets default: see hkMaterial.h
		void setupContactMgrFactories();

		enum SimulationState
		{
			CAN_REMOVE_ENTITIES_AND_CONSTRAINTS = 0,
			CAN_NOT_REMOVE_ENTITIES_AND_CONSTRAINTS
		};

		enum FilterUpdateState
		{
			CAN_UPDATE_FILTERS = 0,
			CAN_NOT_UPDATE_FILTERS
		};

		// Use this function to modify the simulation state
		// between 	CAN_REMOVE_ENTITIES_AND_CONSTRAINTS and	CAN_NOT_REMOVE_ENTITIES_AND_CONSTRAINTS
		// Returns previous state.
		inline SimulationState setRemovalPermission(SimulationState state);

		// inline wrapper for doRequestedMerges
		// called at the appropriate time in the simulation loop to process delayed merges
		inline void checkForMerges();

		// function to actually traverse the merge island request list
		void doRequestedMerges();

		void updatePairFilter( hkCollidablePair& pair, const hkCollisionEnvironment& env );


			// actually merge two islands (could be in the hkSimulationIsland class)
		hkSimulationIsland* mergeTwoIslands( hkSimulationIsland* islandA, hkSimulationIsland* islandB );


			// split any possible active simulation islands
		void splitSimulationIslands();

		void integrateInternal( const hkStepInfo& stepInfo );


		/*
		**	protected members
		*/

	protected:

		hkArray<hkSimulationIsland*> m_activeSimulationIslands;
		hkArray<hkSimulationIsland*> m_inactiveSimulationIslands;


		struct EntityPair
		{
			EntityPair( hkEntity* entityA, hkEntity* entityB);

			bool operator== (const EntityPair& other) const;

			hkEntity* m_entityA;
			hkEntity* m_entityB;
		};

		hkArray<EntityPair> m_mergeList;

				
		hkSimulationIsland*	m_fixedIsland;

		hkReal m_highFrequencyDeactivationPeriod;
		hkReal m_lowFrequencyDeactivationPeriod;

		// Min and Max extents of broadphase
		// Needed for when construction info is retrieved
		hkVector4 m_broadPhaseExtents[2];

	protected:


		hkRigidBody*	m_fixedRigidBody;

		friend class hkWorldCallbackUtil;
		hkArray<hkActionListener*>		m_actionListeners;
		hkArray<hkEntityListener*>		m_entityListeners;
		hkArray<hkConstraintListener*>	m_constraintListeners;
		hkArray<hkWorldDeletionListener*>	m_worldDeletionListeners;
		hkArray<hkIslandActivationListener*> m_islandActivationListeners;
		hkArray<hkWorldPostSimulationListener*> m_worldPostSimulationListeners;
		hkArray<hkWorldPostDetectionListener*> m_worldPostDetectionListeners;
		hkArray<hkIslandPostSimulationListener*> m_islandPostSimulationListeners;
		hkArray<hkIslandPostDetectionListener*> m_islandPostDetectionListeners;
		hkArray<hkCollisionListener*>		m_collisionListeners;
		
		hkWorldPostSimulationListener* m_endOfFrameListener;

		hkBroadPhase* m_broadPhase;

		hkCollisionDispatcher* m_collisionDispatcher;

		hkCollisionEnvironment* m_collisionEnvironment;

		hkVector4			m_gravity;

		hkArray<hkEntity*> m_entitiesToRemove;
        hkArray<hkConstraint*> m_constraintsToRemove;
		hkArray<hkEntity*> m_entitiesToDeactivate;
		hkArray<hkEntity*> m_entityListenerCleanupList;

		hkArray<hkPhantom*> m_phantoms;
	
		SimulationState m_simulationState:8;
		FilterUpdateState m_filterUpdateState:8;

		hkUint32 m_collidableIdCounter;

		hkTypedBroadPhaseHandler* m_typedBroadPhaseHandler;
		hkPhantomBroadPhaseListener* m_phantomBroadPhaseListener;



	public:
			//
			// These members are for internal use only
			//
		hkDynamicsStepInfo m_dynamicsStepInfo;

			/// Can be uesd if multiple worlds are being simulated
		hkTransform m_origin;

			/// The scratch area used by the solver. By default this variable gets initialized with hkSecondaryScratchPad::getInstance().
			/// To run the physics engine in a multithreaded environment, this variable has to be overridden by a non static version
		hkSecondaryScratchPad* m_secondaryScratchPad;

	public:
			//
			// These functions are for internal use only
			//


		void setEndOfFrameListener( hkWorldPostSimulationListener* worldListener );
		hkWorldPostSimulationListener* getEndOfFrameListener();

			// Ask for two simulation islands to be merged (merge will be delayed if necessary)
		inline void requestMerge(hkEntity* entityA, hkEntity* entityB);

		void activateIsland( hkSimulationIsland* island );

		void deactivateIsland( hkSimulationIsland* island );

		void addToListenerCleanupList( hkEntity* entity );

		void testSplits();

		void validateWorld();

	protected:

		HK_FORCE_INLINE void removeRequestedEntities();
		HK_FORCE_INLINE void removeRequestedConstraints();
		HK_FORCE_INLINE void deactivateRequestedEntities();
		HK_FORCE_INLINE void cleanupEntityListeners();

		// called by the above functions
		void doRemoveRequestedEntities();
		void doRemoveRequestedConstraints();
		void doDeactivateRequestedEntities();

	public:

		bool m_wantSimulationIslands;
		bool m_wantDeactivation;

		// backstepping data
		bool m_wantBackstepping;		
		hkReal m_firstPointThreshold;
		hkReal m_incThreshold;
		hkReal m_minimumDivisibleDeltaTime;
		hkWorldPairwiseBackstepFilter* m_pairwiseBackstepFilter;


};




#include <hkdynamics2/world/hkWorld.inl>

#endif // HK_DYNAMICS2_WORLD_H



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
