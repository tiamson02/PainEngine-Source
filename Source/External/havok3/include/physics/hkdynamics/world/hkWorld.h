/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_WORLD_H
#define HK_DYNAMICS2_WORLD_H

#include <hkmath/hkMath.h>

#include <hkbase/config/hkConfigVersion.h>
#include <hkmath/basetypes/hkStepInfo.h>
#include <hkconstraintsolver/solve/hkSolverInfo.h>
#include <hkdynamics/world/hkWorldCinfo.h>
#include <hkdynamics/world/hkMultiThreadLock.h>

class hkCollisionFilter;
class hkCollisionDispatcher;
class hkCollidable;
struct hkCollisionInput;
struct hkProcessCollisionInput;
class hkCollisionListener;
class hkTypedBroadPhaseDispatcher;

struct hkWorldRayCastInput;
struct hkWorldRayCastOutput;
struct hkLinearCastInput;
class hkRayHitCollector;
class hkCdPointCollector;
class hkCdBodyPairCollector;

class hkBroadPhase;
class hkAabb;

class hkEntity;
class hkEntityListener;

class hkRigidBody;

class hkPhantom;
class hkPhantomListener;
class hkPhantomBroadPhaseListener;
class hkEntityEntityBroadPhaseListener;
class hkBroadPhaseBorderListener;

class hkAction;
class hkActionListener;

class hkConstraintInstance;
class hkConstraintData;
class hkConstraintListener;

class hkPhysicsSystem;

class hkSimulationIsland;
class hkContactMgrFactory;

class hkWorldPostSimulationListener;
class hkWorldPostIntegrateListener;
class hkWorldPostCollideListener;
class hkWorldDeletionListener;
class hkIslandActivationListener;

class hkIslandPostIntegrateListener;
class hkIslandPostCollideListener;

class hkStepInfo;
class hkWorldPairwiseBackstepFilter;

class hkSimulation;
class hkWorldOperationQueue;
struct hkDebugInfoOnPendingOperationQueues;
class hkWorldMaintenanceMgr;

class hkWorldMemoryWatchDog;



namespace hkWorldOperation { struct BaseOperation; class UserCallback; }

struct hkOperationParameter;

#if defined HK_DEBUG
//# define HK_ENABLE_EXTENSIVE_WORLD_CHECKING
#endif


	///	Allows you optimize the  hkWorld::updateCollisionFilterOnWorld call
enum hkUpdateCollisionFilterOnWorldMode
{
		/// Full filter filter
	HK_UPDATE_FILTER_ON_WORLD_FULL_CHECK,

		/// Recheck filter but only check for disabled entity-entity collisions
		/// which have been enabled before
	HK_UPDATE_FILTER_ON_WORLD_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY,
};

	/// Allows you to optimize you hkWorld::updateCollisionFilterOnEntity call
enum hkUpdateCollisionFilterOnEntityMode	
{
		/// Do a full check
	HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK,

		/// Recheck filter but only check for disabled entity-entity collisions
		/// which have been enabled before
	HK_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY
};

	/// Activation hints when calling hkWorld::addEntity
enum hkEntityActivation
{
		/// Tries to add the body in an inactive state. However if the body overlaps with an
		/// active body, it gets activated
	HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE,

		/// Always activate this body and overlapping bodies
	HK_ENTITY_ACTIVATION_DO_ACTIVATE 
};

	/// The hkUpdateCollectionFilter specifies whether you want to reevaluate 
	/// all the sub-shape collisions with the world's shape collection filter. Typical example of this is
	/// destructible terrain. So if you just disabled the collision of a bullet and a car, you do not
	/// need to recheck existing landscape collisions and can set 
	/// updateShapeCollectionFilter to HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS
enum hkUpdateCollectionFilterMode 
{
		/// Assume that no single shapes in a shape collections changed their filter status
	HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS,

		/// Recheck all subshapes in a shape collection
	HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS
};

	/// An enumeration used for stepping the world in multithreaded mode.
enum PrepareMultithreadStepResult
{
	ARE_MAIN_THREAD_NOW_CALL_STEP_DELTA_TIME,
	ARE_CHILD_THREAD_CALL_CHILD_THREAD_STEP
};


	/// A structure used by the hkWorld that groups together solver and step time information. 
struct hkWorldDynamicsStepInfo
{
	hkStepInfo   m_stepInfo;
	hkSolverInfo m_solverInfo;
};


	/// See hkWorld::stepDeltaTime for details
enum hkStepResult
{
		/// The call to stepDelta time was a processed to the end
	HK_STEP_RESULT_SUCCESS,

		/// The engine predicted that it would run out of memory before doing any work
	HK_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION,	

		// integration useally does not allocate big blocks, so we do not check here
	//HK_STEP_RESULT_MEMORY_FAILURE_AFTER_INTEGRATION,	

		/// The collide call failed as some collision agents were allocated too much memory
	HK_STEP_RESULT_MEMORY_FAILURE_AFTER_COLLIDE,
};

	/// This structure is currently only used for the PS3. It controls how the multithreading is run.
struct hkMultithreadConfig
{
	hkMultithreadConfig();

		/// PS3 only. A flag used to determine if the cpu can take SPU tasks.
	enum CanCpuTakeSpuTasks
	{
			/// If only SPU jobs are available, and no SPU is waiting, the
			/// CPU will take the task. This is the default setting.
		CPU_CAN_TAKE_SPU_TASKS,

			/// The CPU will never take SPU tasks, even if only SPU tasks are
			/// available.
		CPU_CAN_NOT_TAKE_SPU_TASKS
	};

			/// This variable is used to set whether the cpu can take spu tasks. It defaults to
			/// true. WARNING: If you set this to false, and do not have the correct SPU program
			/// running on at least one SPU during the step() call, the step function will stall indefinitely.
	CanCpuTakeSpuTasks m_canCpuTakeSpuTasks;

		/// This defaults to false. It must be set to true for solving to run on SPUs. For
		/// all other situations this should be left at the default value of false.
	hkBool m_splitConstraintSolvingJob;

};

	/// The hkWorld is a container for the simulation's physical objects.  It also steps the simulation forward in time.
	/// You add elements (including rigid bodies, actions, constraints, and listeners) to the simulation by adding them to the hkWorld.
class hkWorld : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_WORLD);

			/// Creates a world with the given construction information
		hkWorld(const hkWorldCinfo& info, unsigned int sdkversion = HAVOK_SDK_VERSION_NUMBER );

			/// Get construction info for this instance.
		void getCinfo(hkWorldCinfo& info) const;


			//
			//	hkWorld Operations:
			//
			//  Adding/Removing Instances/BatchesOfInstances of Entities/Constraints/Actions/Phantoms
			//

			/// Adds the specified entity to the world and returns its pointer
			/// or returns HK_NULL if adding the body is delayed.
			/// Lets you specify the desired activation state. 
			/// Usage info: Note that even if you add a body inactive, it may be immediately automatically activated 
			///             by the engine due to overlaps with other bodies.
			/// Note: When using HK_ENTITY_ACTIVATION_DO_ACTIVATE parameter, the entity is added in the active state,
			///       however no activationCallbacks are called.
		hkEntity* addEntity( hkEntity* entity, hkEntityActivation initialActivationState = HK_ENTITY_ACTIVATION_DO_ACTIVATE);

			/// Removes the specified entity from the world 
			/// During a simulation step the removal of the entity is delayed until it is safe to do so.
			/// returns true if removed immediately false if delayed.
		hkBool removeEntity( hkEntity* entity);


			/// Efficiently add a group of entities at the same time.  Will not be as efficient when
			/// adding a small number of entities, in that case use addEntity.
			/// Lets you specify the desired activation state. 
			/// Warning: When adding entities inactive: all entities are added in the same island, 
			///          therefore any overlap with an active entity (already inserted into the world)
			///          causes activation of the whole island.
			/// Note: When using HK_ENTITY_ACTIVATION_DO_ACTIVATE parameter, the entity is added in the active state,
			///       however no activationCallbacks are called.
		void addEntityBatch( hkEntity*const* entityBatch, int numEntities, hkEntityActivation initialActivationState = HK_ENTITY_ACTIVATION_DO_ACTIVATE);

			/// Efficiently remove a group of entities at the same time.  Will not be as efficient when
			/// removing a small number of entities, in that case use removeEntity.  
		void removeEntityBatch( hkEntity*const* entityBatch, int numEntities );


			//
			//	Constraint addition and removal
			//

			/// Adds the specified constraint to the world
		hkConstraintInstance* addConstraint( hkConstraintInstance* constraint);

			/// Removes the specified constraint from the world
			/// returns true if removed immediately false if delayed
		hkBool removeConstraint( hkConstraintInstance* constraint);

			/// Utility function for creating and adding a specified constraint instance to the world
		hkConstraintInstance* createAndAddConstraintInstance( hkRigidBody* bodyA, hkRigidBody* bodyB, hkConstraintData* constraintData );
		


			//
			// Actions
			//
			
			/// Adds the specified action to the world. A reference to the action is added to every entity on the action's list
			/// of entities. All those entities must already by placed in an hkWorld.
		hkAction* addAction( hkAction* action ); 

			/// Remove the specified action from the world and removes its reference from each entity that it was connected to.
		void removeAction( hkAction* action );	
			
			/// Remove the specified action from the world and removes its reference from each entity that it was connected to.
			/// It bypasses the operation delay framework, there fore you _MUST_BE_SURE_ that _IT_IS_SAFE_ to use it.
			/// Info: use this only in hkAction::entityRemovedCallback
			/// Non-symmetric function. Only needed to be used explicitly when removing an entity causes removal of an action in a callback. 
			/// addActionImmediately not needed.
		void removeActionImmediately( hkAction* action );	


			//
			// Phantoms
			//

			/// Add the phantom to the world
		hkPhantom* addPhantom( hkPhantom* phantom );

			/// Add multiple phantoms to the world
		void addPhantomBatch( hkPhantom*const* phantomBatch, int numPhantoms );


			/// Remove the phantom from the world
		void removePhantom( hkPhantom* phantom );

			/// Remove multiple phantoms from the world
		void removePhantomBatch( hkPhantom*const* phantomBatch, int numPhantoms );

			/// Activates all entities whose aabb overlaps with the specified region
		void activateRegion( const hkAabb& aabb );


			// 
			// Systems (bodies, actions and phantoms all together). For instance 
			// a vehicle or a ragdoll.

			/// Add a hkPhysicsSystem to the world. The world does not
			/// store or reference the system struct itself, just 
			/// uses the data held to batch add to the world.
		void addPhysicsSystem( const hkPhysicsSystem* sys, hkEntityActivation initialActivationState = HK_ENTITY_ACTIVATION_DO_ACTIVATE );

			/// Remove a hkPhysicsSystem from the world. 
			/// The world will just look through the lists in the given system
			/// and remove all found. It does not alter the system struct itself.
		void removePhysicsSystem( const hkPhysicsSystem* sys );

			//
			// Fixed rigid body
			//


			/// Gets the world's fixed rigid body. This is a fixed body that can be used to constrain an object to
			/// a position in world space without needing to create an additional fixed object. The world's fixed
			/// body differs from 'normal' rigid bodies in a number of ways, including but not limited to the
			/// following:<br>
			/// It doesn't have a shape/collision representation and so it never collides with anything.<br>
			/// It doesn't have a broadphase entry and so it cannot go outside the broadphase, resulting in a
			/// potential performance hit as 'normal' rigid bodies can.
		inline hkRigidBody* getFixedRigidBody() const;

			//
			//  Collision filter update methods
			//

			/// This method should be called if you have altered the collision filtering information for this entity.
			/// Note: this does not activate the entity.
			/// The collisionFilterUpdateMode allows you to do an optimization: Only previously enabled entity entity collisions are 
			/// checked. However entity-phantom or new entity-entity collision are ignored.
			/// The updateShapeCollectionFilter specifies whether you want to reevaluate 
			/// all the sub-shape collisions with the world's shape collection filter. Typical example of this is
			/// destructible terrain. So if you just disabled the collision of a bullet and a car, you do not
			/// need to recheck existing landscape collisions and can set 
			/// updateShapeCollectionFilter to HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS
		void updateCollisionFilterOnEntity( hkEntity* entity, 
			                                hkUpdateCollisionFilterOnEntityMode updateMode,
											hkUpdateCollectionFilterMode updateShapeCollectionFilter);

			/// This method should be called if you have altered the collision filtering information for this phantom. Read updateCollisionFilterOnEntity() for details
		void updateCollisionFilterOnPhantom( hkPhantom* phantom, 
											 hkUpdateCollectionFilterMode updateShapeCollectionFilter );	

			/// This method should be called if you have changed the collision filter for the world.
			///
			/// WARNING: This method is very slow. It involves re-evaluating all the broad phase aabbs in the system.
			/// You should only ever call this at startup time.
			/// updateMode allows you to use a shortcut if you only want to disable previously enabled entity entity
			/// collisions.
			/// The updateShapeCollectionFilter specifies whether you want to reevaluate 
			/// all the sub-shape collisions with the world's shape collection filter. Typical example of this is
			/// destructible terrain. So if you just disabled the collision of a bullet and a car, you do not
			/// need to recheck existing landscape collisions and can set 
			/// updateShapeCollectionFilter to HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS
		void updateCollisionFilterOnWorld( hkUpdateCollisionFilterOnWorldMode updateMode,
										   hkUpdateCollectionFilterMode updateShapeCollectionFilter );

			//
			// Phantoms
			//


			/// Get a list of all the phantoms which have been added to the world
		inline const hkArray<hkPhantom*>& getPhantoms() const;


			//
			// Gravity
			//

			/// Sets the gravity value for the world. This does not wake up objects
		void setGravity( const hkVector4& gravity );

			/// Gets the world's gravity value.
		inline const hkVector4& getGravity() const; 


			//
			// Simulation Control
			//

		
			/// Gets the memory usage required to complete the next integration step
			/// This function is called internally to prevent the memory usage breaching the critical memory limit in
			/// hkMemory.
		int getMemUsageForIntegration();

			/// Calculate the memory statistics for the world, and all objects owned by the world
		void calcStatistics( hkStatisticsCollector* collector ) const;


			/// Get the memory "watch dog" used by the world. By default this is HK_NULL, i.e. the memory
			/// for the world is not checked. If this is set, hkWorldMemoryWatchDog::watchMemory will be
			/// called every step, which gives the hkWorldMemoryWatchDog a chance to prevent the memory
			/// exceeding a desired threshold.
		hkWorldMemoryWatchDog* getMemoryWatchDog( ) const;

			/// Set the memory "watch dog" used by the world. By default this is HK_NULL, i.e. the memory
			/// for the world is not checked. If this is set, hkWorldMemoryWatchDog::watchMemory will be
			/// called every step, which gives the hkWorldMemoryWatchDog a chance to prevent the memory
			/// exceeding a desired threshold.
		void setMemoryWatchDog( hkWorldMemoryWatchDog* watchDog );

			//
			// Multithreaded Access 
			//

			/// Locks the world in a multithreaded environment.
			/// This can only be called outside any step function and should
			/// only be used for asynchronous queries like raycast, etc.
			/// Read the multithreading use guide about details
		void lock();

			/// Unlocks the world. Opposite of lock()
		void unlock();


			//
			// Multithreaded Access checks
			//
		
		enum MtAccessChecking
		{
			MT_ACCESS_CHECKING_ENABLED = 0,
			MT_ACCESS_CHECKING_DISABLED
		};

			/// Enable or disable multithreaded access checking
			/// By default this is enabled if the world simulation type is SIMULATION_TYPE_MULTITHREADED
			/// and false is it is anything else.
		inline void setMultithreadedAccessChecking( MtAccessChecking accessCheckState );

			/// Get whether multithreaded access checking is enabled
			/// By default this is enabled if the world simulation type is SIMULATION_TYPE_MULTITHREADED
			/// and false is it is anything else.
		inline MtAccessChecking getMultithreadedAccessChecking() const;
		
			/// Mark this class and all child classes for read only access for this thread
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread marked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		inline void markForRead( ) const;

			/// Mark this class and all child classes for read write access for this thread
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread marked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		inline void markForWrite( );

			/// Undo lockForRead
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		inline void unmarkForRead( ) const;

			/// Unlock For write
			/// Note: This is only for debugging and does not wait to get exclusive access, 
			/// but simply assert if another thread locked the hkWorld. You must read the
			/// user guide about multithreading to use this.
		inline void unmarkForWrite();


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


			/// Adds the specified listener to the world
		void addPhantomListener( hkPhantomListener* worldListener );

			/// Removes the specified listener from the world
		void removePhantomListener( hkPhantomListener* worldListener );

		
			/// Adds a listener which listens to ALL entity activations.
		void addIslandActivationListener( hkIslandActivationListener* worldListener );

			/// Removes the specified listener from the world
		void removeIslandActivationListener( hkIslandActivationListener* worldListener );

			/// Adds the specified listener to the world
		void addWorldPostCollideListener( hkWorldPostCollideListener* worldListener );

			/// Removes the specified listener from the world
		void removeWorldPostCollideListener( hkWorldPostCollideListener* worldListener );

			/// Adds the specified listener to the world
		void addWorldPostSimulationListener( hkWorldPostSimulationListener* worldListener );

			/// Removes the specified listener from the world
		void removeWorldPostSimulationListener( hkWorldPostSimulationListener* worldListener );

			/// Adds the specified listener to the world
		void addWorldPostIntegrateListener( hkWorldPostIntegrateListener* worldListener );

			/// Removes the specified listener from the world
		void removeWorldPostIntegrateListener( hkWorldPostIntegrateListener* worldListener );

			/// Adds the specified listener to the world
		void addIslandPostCollideListener( hkIslandPostCollideListener* islandListener );

			/// Removes the specified listener from the world
		void removeIslandPostCollideListener( hkIslandPostCollideListener* islandListener );

			/// Adds the specified listener to the world
		void addIslandPostIntegrateListener( hkIslandPostIntegrateListener* islandListener );

			/// Removes the specified listener from the world
		void removeIslandPostIntegrateListener( hkIslandPostIntegrateListener* islandListener );

			/// Adds a listener which listens to ALL collisions.
		void addCollisionListener( hkCollisionListener* worldListener );

			/// Removes a world collision listener. 
		void removeCollisionListener( hkCollisionListener* worldListener);

			/// Add a listener which receives events when the world is deleted
		void addWorldDeletionListener( hkWorldDeletionListener* worldListener );

			/// Remove a listener which receives events when the world is deleted
		void removeWorldDeletionListener( hkWorldDeletionListener* worldListener );


			//
			// Collision detection
			//

			/// Gets the broad phase owned by the world.
		inline hkBroadPhase* getBroadPhase() const;

			/// Get the collision input. This is needed to manually query collision agents.
		inline hkProcessCollisionInput* getCollisionInput() const;

			/// Gets the collision dispatcher owned by the world.
		inline hkCollisionDispatcher* getCollisionDispatcher() const;

			/// Gets the collision filter.
		inline const hkCollisionFilter* getCollisionFilter() const;

			/// Set the collision filter. This is used by the world by default when it calls the collision detector.
			/// WARNING: This should only be done before any objects are added to this world.
		void setCollisionFilter( hkCollisionFilter* filter,
								 hkBool             runUpdateCollisionFilterOnWorld = true, 
								 hkUpdateCollisionFilterOnWorldMode checkBroadPhaseMode = HK_UPDATE_FILTER_ON_WORLD_FULL_CHECK,				
								 hkUpdateCollectionFilterMode       updateShapeCollectionFilter = HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS );

			/// Cast a ray into the world and get the closest hit.
			/// Note: it uses the hkWorld::getCollisionFilter() for filtering
			/// This function uses a ray-cast function for traversing the broad phase, so for
			/// long raycasts it is more applicable than hkAabbPhantom::castRay
		void castRay(const hkWorldRayCastInput& input, hkWorldRayCastOutput& output );

			/// Cast a ray into the world and do a callback for every hit.
			/// Note: it uses the hkWorld::getCollisionFilter() for filtering
			/// This function uses a ray-cast function for traversing the broad phase, so for
			/// long raycasts it is more applicable than hkAabbPhantom::castRay
		void castRay(const hkWorldRayCastInput& input, hkRayHitCollector& collector );


			/// Cast a shape within the world.
			/// The castCollector collects all potential hits.<br>
			/// Note that the distance stored within the castCollector is a hitFraction (between 0 and 1.0) and
			/// not a distance.<br>
			/// This function uses an aabb-cast function for traversing the broad phase, so for
			/// long linear casts it this will perform better than hkShapePhantom::linearCast
			/// The [optional] startPointCollector returns all the closest points
			/// at the start point of the linear cast. If you do not want this functionality, pass HK_NULL as the
			/// "startCollector".
		void linearCast( const hkCollidable* collA, const hkLinearCastInput& input, hkCdPointCollector& castCollector, hkCdPointCollector* startCollector = HK_NULL );

			/// Get the closest points to a hkCollidable (= [shape,transform,filterInfo] ).
			/// Note: If you have call this function every step for a given object, use the hkShapePhantom::getClosestPoints()
			/// By default, you should pass a pointer to world->getCollisionInput() via hkCollisionInput* input. However you 
			/// can use this parameter to pass in your own collision input
			/// structure, and change the shape collection collision filter, and the collision tolerance used. 
			/// (Note: If using your own collision input, make sure you start by copying the world collision input, so that
			/// you use the default hkCollisionDispatcher).
		void getClosestPoints( const hkCollidable* collA, const hkCollisionInput& input, hkCdPointCollector& collector);

			/// Get all shapes which are penetrating the collidable.
			/// Note: If you have call this function every step for a given object, use the hkShapePhantom version
			/// By default, you should pass a pointer to world->getCollisionInput() via hkCollisionInput* input. However you 
			/// can use this parameter to pass in your own collision input
			/// structure, and change the shape collection collision filter, and the collision tolerance used. 
			/// (Note: If using your own collision input, make sure you start by copying the world collision input, so that
			/// you use the default hkCollisionDispatcher).
		void getPenetrations( const hkCollidable* collA, const hkCollisionInput& input, hkCdBodyPairCollector& collector);


			/// Gets the current broadphase border, could be null.
		class hkBroadPhaseBorder* getBroadPhaseBorder() const;


			/// Sets a custom broadphase border.
			/// By default the hkWorld constructor checks the
			/// hkWorldCinfo.m_broadPhaseBorderBehaviour field and will create a default
			///	broadphase border if needed. Real hkWorldCinfo.m_broadPhaseBorderBehaviour for details
		void setBroadPhaseBorder( hkBroadPhaseBorder* bpb );


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


			//
			// Stepping the world, single and multithreaded, synchronously and asynchronously
			//


			/// Advances the state of the simulation by the amount of time.
			/// This function may perform an early exit returning HK_STEP_RESULT_MEMORY_FAILURE, in which case memory should
			/// be freed (by removing objects from the world) and recoverFromStepFailure() should be called.
		hkStepResult stepDeltaTime( hkReal deltaTime );

			/// Advances the state of the simulation by 'frameDeltaTime' (seconds).
			/// Allows you to perform simulation steps asynchronously to your engine loop .
			/// 'physicsDeltaTime' tells the system what size of simulation step it should use.
			/// When the ( 'time of current frame' > 'time of the last performed simulation step' + 'physicsDeltaTime' ),
			/// then a simulation step will be performed. 
			/// This function may perform an early exit returning HK_STEP_RESULT_MEMORY_FAILURE, in which case memory should
			/// be freed (by removing objects from the world) and recoverFromStepFailure() should be called.
			/// Example: You want to run the physics at a constant 30Hz und your graphics is synchronized to
			/// 50 Hz. So you call stepAsynchronously( 1.f/50.f, 1.f/30.f );
		hkStepResult stepAsynchronously( hkReal frameDeltaTime, hkReal physicsDeltaTime );

			/// Attempts to advance the state of the simulation until the time previously determined in stepDeltaTime/stepAsynchronously.
			/// This function is only to be used after stepDeltaTime/stepAsynchronously return HK_STEP_RESULT_MEMORY_FAILURE.
			/// This function may return HK_STEP_RESULT_MEMORY_FAILURE again, in that case more memory should be freed.
			/// This function should be called until it returns HK_STEP_RESULT_SUCCESS.
		hkStepResult recoverFromStepFailure(hkStepResult previousResult);

			/// Returns the time of the next frame.
			/// Frame time is updated at the very beginning of hkSimulation::stepDeltaTime(), therefore this returns the time
			/// of the next frame that will be computed (while still in stepDeltaTime()) or the time of the current frame being 
			/// processed by user code (e.g. graphics engine) -- once simulation computations are done.
		inline hkTime getFrameTime() const;

			/// This returns current time of the simulation. Use it, e.g. to calculate current transforms of rigid bodies
			/// in between PSI steps (with hkRigidBody::approxTransformAt). Note that this is not absolute time; its value is reset every 30 seconds or so.
		inline hkTime getCurrentTime() const;



		//
		// Multithreaded simulation stepping controls
		//


			/// If you are using the multithreaded simulation, you have to call this function single threadedly before
			/// any call to stepDeltaTime();
		void resetStepDeltaTime();
		
			/// If you are using the multithreaded simulation, you have to call this function in EVERY thread before
			/// any call to stepDeltaTime(). Depending on the result of this function, your thread should either
			/// call stepDeltaTime() or childThreadStep()
		PrepareMultithreadStepResult prepareMultithreadStep();

			/// This function should be called by a child thread, after stepDeltaTime has been called by the main thread
			/// To ensure this has happens, each thread shoudl call prepareMultithreadStep() and, if successful call stepDeltatime, 
			/// if not successful, call childThreadStep.
		void childThreadStep();


			/// This function is only valid when using multithreaded simulation. If using other
			/// simulations, it returns HK_NULL.
		class hkJobQueue* getJobQueue();


			/// This function is only valid for multithreading simulation.  It allows you to access
			/// some settings used for PS3 multithreading simulation.
		void getMultithreadConfig( hkMultithreadConfig& config );

			/// This function is only valid for multithreading simulation. It allows you to control
			/// some settings used for PS3 multithreading simulation.
		void setMultithreadConfig( const hkMultithreadConfig& config );


		//
		// Serialization Utilities
		//

			/// Get a hkPhysicsSystem that represents the whole system 
			/// contained in this world. This can be used as a serialization utility
			/// for taking a snapshot of the world.
			/// Release the physics system when you are finished using it.
		hkPhysicsSystem* getWorldAsOneSystem() const;


		/*
		**	Protected Functions
		*/

			// you should use removeReference() instead, however we allow you to force destroy the world
		~hkWorld();

		/*
		**	protected members: They are public so that people can get full control over the engine
		**  if they know what they are doing!!!
		*/
	public:

			// A class controlling the simulation
		hkSimulation*   m_simulation;

				// Current time of the simluation. (Not the same as the time of the last simulation step.)
				// Tech info: Is equal to min3(m_timeOfNextFrame, m_timeOfNextPsi, min_time_of_all_TOI_events_to_be_processed )
				// Note: that the simulation's global time is not the absolute time from the beginning of the simulation. The time is
				// reset in the whole simulation every once in a while to avoid floating-point precision problems.
			hkTime m_currentTime;

				// Target time to which, simulation state should be advanced. Simulation steps (PSI's) and TOI-events are
				// performed until m_currentTime reaches m_timeOfNextFrame
			hkTime m_timeOfNextFrame;

				// Time of the last PSI step performed. 
				// Tech info: Corresponds to t0 (or hkSweptTransform::getBaseTime()) of hkRigidBodies (those which have not been re-integrated in TOI events).
			hkTime m_timeOfLastPsi;

				// Time of the next PSI step. Corresponds to t1 of hkSweptTransforms of all bodies,
				// with exception of all bodies, which were activate/deactivated, are inactive, or had their motionType changed.
			hkTime m_timeOfNextPsi;

		hkVector4			m_gravity;

		hkSimulationIsland*	m_fixedIsland;
			hkRigidBody*	m_fixedRigidBody;

		hkArray<hkSimulationIsland*> m_activeSimulationIslands;
		hkArray<hkSimulationIsland*> m_inactiveSimulationIslands;

			// Dirty islands are scheduled for cleanup, performed  by hkWorldOperationUtil::cleanupDirtyIslands( )
		hkArray<hkSimulationIsland*> m_dirtySimulationIslands;

		hkWorldMaintenanceMgr* m_maintenanceMgr;
		hkWorldMemoryWatchDog* m_memoryWatchDog;

		hkBroadPhase* m_broadPhase;

			// Broad phase handler - this is a class which links the broad phase to the simulation
		hkTypedBroadPhaseDispatcher*    m_broadPhaseDispatcher;
		hkPhantomBroadPhaseListener*    m_phantomBroadPhaseListener;
		hkEntityEntityBroadPhaseListener* m_entityEntityBroadPhaseListener;
		hkBroadPhaseBorderListener* m_broadPhaseBorderListener;


		hkProcessCollisionInput* m_collisionInput;
			hkCollisionFilter*       m_collisionFilter;
			hkCollisionDispatcher*   m_collisionDispatcher;

			/// For delayed operations. Operations get delayed if they cannot be performed as the world is hkWorld::locked()
		hkWorldOperationQueue* m_pendingOperations;
			int m_pendingOperationsCount;
			int m_lockCount;
				// It just holds a value offset which should be added to m_lockCount when querying areCriticalOperationsLockedForPhantoms().
			int m_lockCountForPhantoms;
			hkBool m_blockExecutingPendingOperations;
			hkBool m_criticalOperationsAllowed;
				// This points to structures used to debug operation postponing. Not used in release build.
			hkDebugInfoOnPendingOperationQueues* m_pendingOperationQueues;
				// Holds the number of pending queues (equal to the number of recursive calls to executeAllPendingOperations).
			int m_pendingOperationQueueCount;
				
			/// The thread index of the locking thread
		mutable hkMultiThreadLock m_multiThreadLock;

			/// Internal. Forces actions to be processed in a single thread.
		hkBool m_processActionsInSingleThread;

			// set if running a mt simulation
			// whenever a constraint is added, this critical section is used
		class hkCriticalSection* m_modifyConstraintCriticalSection;

		class hkCriticalSection* m_worldLock;


		hkBool m_wantSimulationIslands;

			// Deactivation parameters
		hkBool m_wantDeactivation;
		hkBool m_shouldActivateOnRigidBodyTransformChange;

		hkReal m_highFrequencyDeactivationPeriod;
		hkReal m_lowFrequencyDeactivationPeriod;

		hkReal m_toiCollisionResponseRotateNormal;
		hkWorldCinfo::SimulationType  m_simulationType;

	protected:
			// World-unique ids are assigned to entities. They're used to sort constraints deterministically when usin hkMultithreadedSimulation.
		hkUint32 m_lastEntityUid;

	
	protected:
		friend class hkWorldOperationUtil;
		friend class hkWorldCallbackUtil;
		friend class hkDiscreteSimulation;
		friend class hkHalfstepSimulation;
		friend class hkContinuousSimulation;
		friend class hkMultiThreadedSimulation;
		friend class hkSimpleConstraintContactMgr;

		hkArray<hkPhantom*> m_phantoms;

		hkArray<hkActionListener*>		m_actionListeners;
		hkArray<hkEntityListener*>		m_entityListeners;
		hkArray<hkPhantomListener*>		m_phantomListeners;
		hkArray<hkConstraintListener*>	m_constraintListeners;
		hkArray<hkWorldDeletionListener*>	m_worldDeletionListeners;
		hkArray<hkIslandActivationListener*> m_islandActivationListeners;
		hkArray<hkWorldPostSimulationListener*> m_worldPostSimulationListeners;
		hkArray<hkWorldPostIntegrateListener*> m_worldPostIntegrateListeners;
		hkArray<hkWorldPostCollideListener*> m_worldPostCollideListeners;
		hkArray<hkIslandPostIntegrateListener*> m_islandPostIntegrateListeners;
		hkArray<hkIslandPostCollideListener*> m_islandPostCollideListeners;
		hkArray<hkCollisionListener*>		m_collisionListeners;
		
		class hkBroadPhaseBorder*			m_broadPhaseBorder;
	

	public:
			//
			// These members are for internal use only
			//
		hkWorldDynamicsStepInfo m_dynamicsStepInfo;


		// Min and Max extents of broadphase
		// Needed for when construction info is retrieved
		hkVector4 m_broadPhaseExtents[2];
		int			m_broadPhaseNumMarkers;
		hkInt32		m_broadPhaseQuerySize;
		hkInt32		m_broadPhaseUpdateSize;
		hkEnum<enum hkWorldCinfo::ContactPointGeneration, hkInt8> m_contactPointGeneration;

		//
		// Deactivation 
		//

			// Gets the high frequency deactivation period.
		inline hkReal getHighFrequencyDeactivationPeriod() const;

			// Sets the high frequency deactivation period.
		void setHighFrequencyDeactivationPeriod( hkReal period );

			// Gets the low frequency deactivation period.
		inline hkReal getLowFrequencyDeactivationPeriod() const;

			// Sets the low frequency deactivation period.
		void setLowFrequencyDeactivationPeriod( hkReal period );

		//
		// Extra action management
		//

			// Attaches an action to an entity -- this should be called for every entity being 
			// attached to the action, when the action is already added to the world
			// Note: this does not manage the action's entity list.
		void attachActionToEntity(hkAction* action, hkEntity* entity);

			// Detaches an action from an entity -- this should be called for every entity being 
			// detached from the action, when the action is already added to the world.
			// Note: this does not manage the action's entity list.
		void detachActionFromEntity(hkAction* action, hkEntity* entity);


		//
		// Locking and delaying of operations
		//
	public:
		
			// Locks the world, i.e. prevents all critical operations (like adding/removing entities or constraints) from being
			// executed immediately. The operations are put on an hkWorldOperationsQueue and should be executed at the earliest
			// safe time. See hkWorldOperationQueue for the list of 'critical' operations which may be postponed by the system.
			// Subsequent lockCriticalOperations() calls accumulate.
		HK_FORCE_INLINE void	lockCriticalOperations();

			// Allows critical operations to be executed immediately after unlockCriticalOperations() has been called or every corresponding lockCriticalOperations().
			// It is advised to call attemptToExecutePendingOperations right after unlockCriticalOperations() to keep the order of operations 
			// the same as the order in which they were called.
		HK_FORCE_INLINE void	unlockCriticalOperations();

			// Returns number of locks put on the hkWorld. Critical operation will be postponed if areCriticalOperationsLocked() > 0
			// We assume that this function is called at a beginning of a critical operation.
			// So this function asserts if critical operations are not allowed at all
		HK_FORCE_INLINE int		areCriticalOperationsLocked() const;

			// like areCriticalOperationsLocked without the extra check
		HK_FORCE_INLINE int		areCriticalOperationsLockedUnchecked() const;

			// Concept: Locking of Phantoms is an add-on work-around.
			//          It lets you execute phantom-related critical functions even when hkWorld::areCriticalOperationsLocked() is called.
			//          Note that recursive phantom operations will not be executed, and will be postponed just like other operations are.

			// Decreases lock count for phantom operations. The operations must query hkWorld::areCriticalOperationsLockedForPhantoms()
			// instead of hkWorld::areCriticalOperationsLocked().
		HK_FORCE_INLINE void	unlockCriticalOperationsForPhantoms();

			// Increases lock count for phantom operations..
		HK_FORCE_INLINE void	lockCriticalOperationsForPhantoms();

			// Queries lock count for phantom operations. hkWorld::m_lockCountForPhantoms is an offset value added to the primary hkWorld::m_lockCount.
		HK_FORCE_INLINE int		areCriticalOperationsLockedForPhantoms() const;



			// Concept: explained in hkWorld.cpp. Search for: "concept: blocking of execution of pending operations"
			// Blocks or unblocks attempts of execution of pending operations.
		HK_FORCE_INLINE void	blockExecutingPendingOperations(hkBool block);

			// See: hkWorld::unlock + hkWorld::attemptToExecutePendingOperations
		HK_FORCE_INLINE void	unlockAndAttemptToExecutePendingOperations();

			// If the world is locked or execution of pending operations is blocked, then it does nothing. 
			// Otherwise it processes the list of pending operations.
		HK_FORCE_INLINE void	attemptToExecutePendingOperations();

			// Appends information about an operation request onto the world's hkWorldOperationQueue.
						void	queueOperation(const hkWorldOperation::BaseOperation& operation);

			// Queues an user callback onto operation queue.
			// If the world is not locked, the callback is executed immediately.
						hkWorldOperation::UserCallback* queueCallback(hkWorldOperation::UserCallback* callback, void* userData = HK_NULL);
	private:
			// Passes execution call to hkWorldOperationQueue::attemptToExecuteAllPending()
		                void	internal_executePendingOperations();

	#if defined HK_DEBUG
			// Returns true if both dynamic bodies attached to the constraint can collide with one another, based on
			// the logic in the hkWorld's hkCollisionFilter.
		hkBool constrainedDynamicBodiesCanCollide( const hkConstraintInstance* constraint ) const;

			// Issues warnings that vary depending on whether the two dynamic, constrained bodies will definitely
			// collide or whether the user simply needs to manually call hkWorld::updateCollisionFilter...()
		void warnIfConstrainedDynamicBodiesCanCollide( const hkConstraintInstance* constraint,
															hkBool collidingBeforeConstraintAdded,
															hkBool collidingAfterConstraintAdded ) const;
	#endif	// #if defined HK_DEBUG


	public:
			// Debugging utility: see hkWorld.cpp. Search for: "concept: allowing critical operations"
		HK_FORCE_INLINE void	allowCriticalOperations(hkBool allow);

		// A utility function used internally for testing
		void replaceSimulationWithMultithreadedSimulation(  );
};




#include <hkdynamics/world/hkWorld.inl>

#endif // HK_DYNAMICS2_WORLD_H


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
