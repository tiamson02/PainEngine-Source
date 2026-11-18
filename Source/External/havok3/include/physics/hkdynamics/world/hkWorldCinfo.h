/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKDYNAMICS_WORLD_HKWORLDCINFO_XML_H
#define HKDYNAMICS_WORLD_HKWORLDCINFO_XML_H

#include <hkmath/basetypes/hkAabb.h>

/// hkWorldCinfo meta information
extern const class hkClass hkWorldCinfoClass;

/// This structure contains all the information needed to construct a hkWorld
/// object.
class hkWorldCinfo : public hkReferencedObject
{
	public:

		HK_DECLARE_REFLECTION();

			/// Used to specify the speed/stiffness balance of the core solver.
		enum SolverType
		{
				/// 
			SOLVER_TYPE_INVALID,
				/// 'Softest' solver type.
			SOLVER_TYPE_2ITERS_SOFT,
				/// .
			SOLVER_TYPE_2ITERS_MEDIUM,
				/// .
			SOLVER_TYPE_2ITERS_HARD,
				/// .
			SOLVER_TYPE_4ITERS_SOFT,
				/// .
			SOLVER_TYPE_4ITERS_MEDIUM,
				/// .
			SOLVER_TYPE_4ITERS_HARD,
				/// .
			SOLVER_TYPE_8ITERS_SOFT,
				/// .
			SOLVER_TYPE_8ITERS_MEDIUM,
				/// 'Hardest' solver type.
			SOLVER_TYPE_8ITERS_HARD,
				/// 
			SOLVER_TYPE_MAX_ID
		};
		
			/// Specifies simulation with discrete or continuous collision detection.
			// Note: all simulation types with enumeration values >= SIMULATION_TYPE_CONTINUOUS set m_wantContinuousAgents = true in hkWorld's constructor.
		enum SimulationType
		{
				/// 
			SIMULATION_TYPE_INVALID,
				/// Old Havok2 style simulation
			SIMULATION_TYPE_DISCRETE,

				/// Specialization of discrete simulation, where the framerate might be independent of the
				/// physics timestep. This simulation is useful when your framerate varies
				/// a lot or if you have very low frame rate occasionally.
				/// However, using this type of simulation makes the integration of the
				/// physics into gameplay tricky.
			SIMULATION_TYPE_ASYNCHRONOUS,

				/// Specialization of asynchronous simulation. Two main simulation parts are split in-between two
				/// consecutive 'simulation step' calls. 
				/// First step: constraint solving + integration is performed for the two coming frames.
				/// Second step: collision detection for the next frame is performed.
				/// (Third step: looped.)
				/// This simulation type effectively requires half the CPU time and lowers the quality accordingly 
				/// (as compared to base asynchronous simulation).
			SIMULATION_TYPE_HALFSTEP,

				/// The new continuous simulation introduced in Havok 3.0.
				/// Depending of the hkEntity->getCollidable()->getQualityType(), collision
				/// are not only performed at 'normal' physical timesteps (called PSI), but
				/// at any time when two objects collide called (TOI)
			SIMULATION_TYPE_CONTINUOUS,

				/// The simulation computations may be split into
				///  - two frames: integration + collision detection, TOI handling        (_HALFSTEP)
				///  - three frames: integration + collision detection + TOI handling     (_ONE_THIRD_STEP)
				///  - or more frames: integration + collision detection + TOI handling + TOI handling + ...
				/// Note: 
				///  Until the TOI handling sub-step is performed, the simulation's state is inconsistent what
				///  may result in noticable visual artifacts.
			SIMULATION_TYPE_CONTINUOUS_HALFSTEP,

				/// See comment for SIMULATION_TYPE_CONTINUOUS_HALFSTEP.
			SIMULATION_TYPE_CONTINUOUS_ONE_THIRD_STEP,

				/// Back-step simulation is derived form hkAsynchronousSimulation. It performs continuous collision
				/// detection and back-steps pairs of bodies that ended up in a penetrating configuration.
			SIMULATION_TYPE_BACKSTEP_SIMPLE,
				
				/// Performs additional continuous collision detection for every backstepped body. Guarantees non penetraiton.
			SIMULATION_TYPE_BACKSTEP_NON_PENETRATING,

				/// Multithreaded simulation.
				/// You must have read the multi threading user guide.<br>
                /// To use this you have to call hkWorld::stepDeltaTime() several times
				/// (once for each thread).
				/// Each thread entering the physics using this hkWorld::stepDeltaTime() call
				/// will immediately be used by the engine. There is not need for
				/// all threads to enter the hkWorld at the same time, actually the
				/// first thread entering will do some special single threaded maintance
				/// jobs. After all threads have been finished you have to call
				/// hkWorld::m_simulation->resetStepDeltaTime() from a single thread.<br>
				/// Notes:
				///   - The internal overhead for multi threaded is small and you can expect
				///     good speedups, except:
				///   - Toi solving is not multithreaded 
				///   - Solving a single huge island is not multithreaded.
				///   So multithreading works best if you have a distributed world.
			SIMULATION_TYPE_MULTITHREADED,
		};
		
			/// Specifies how aggressively the engine accepts new contact points
		enum ContactPointGeneration
		{
				/// Try to gather as many contact points as possible. This
				/// gives you the highest quality at the cost of some (up to 25%)
				/// extra CPU. You should use this setting if you want to stack
				/// very small objects in your game
			CONTACT_POINT_ACCEPT_ALWAYS,

				/// Accept good contact points immediately and try to
				/// to reject the rest. This is a compromise 
			CONTACT_POINT_REJECT_DUBIOUS,
															   
				/// Uses some optimistic algorithms to speed up contact point generation.
				/// This can seriously increase the performance of the engine.
				/// Note: Stacking small objects becomes very difficult with this option enabled
			CONTACT_POINT_REJECT_MANY,
		};

			/// Tell the system what should happen if an objects leaves the
			/// extents of the broadphase. 
			/// If you want to do your own implementation,
			/// just derive from hkBroadPhaseBorder, implement your own
			/// maxPositionExceededCallback and call 
			/// hkWorld::setBroadPhaseBorder( new myBroadPhaseBorder() );
		enum BroadPhaseBorderBehaviour
		{
				/// Cause an assert and set the motion type to be fixed (default).
			BROADPHASE_BORDER_ASSERT,

				/// Set the motion type to be fixed and raise a warning.
			BROADPHASE_BORDER_FIX_ENTITY,

				/// Remove the entity from the hkWorld and raise a warning.
			BROADPHASE_BORDER_REMOVE_ENTITY,

			    /// Do not do anything, just continue to work.
				/// If many (>20) objects leave the broadphase, 
				/// serious memory and CPU can be wasted.
			BROADPHASE_BORDER_DO_NOTHING,
		};
		
			/// Default constructor
		hkWorldCinfo();
		
		//
		// Methods
		//
		
			/// Set the parameters of the world's solver. Each solver type provides a different
			/// speed/stiffness balance for the simulation. The default solver type is
			/// SOLVER_TYPE_4ITERS_MEDIUM, which is fast and moderately soft.
		void setupSolverInfo(enum SolverType st);
		
			/// Sets the broad phase size to be a cube centered on the origin of side
			/// sideLength. See also m_broadPhaseWorldSize.
		void setBroadPhaseWorldSize(hkReal size);
		
		//
		// Members
		//
	public:

		//
		// Basic setup
		//
		
			/// The gravity for the world. The default is (0, -9.8, 0).
		hkVector4 m_gravity;
		
			/// This variable is used to guess at the maximum number of overlaps you will get
			/// when you add or remove an entity or phantom. It determines the size of the local
			/// array used to query the broad phase. The local array uses the hkMemory stack,
			/// The default value is 1024, which requires a local array stack area of 8K.
		hkInt32 m_broadPhaseQuerySize;
		
			/// The velocity which defines when two objects are assumed to be
			/// in resting contact. If the approaching velocity of the two objects
			/// at a point of contact is higher than this value, a special more accurate collision
			/// restitution algorithm is called. By default this value is hkMath::HK_REAL_MAX
			/// which thus disables this algorithm (though more accurate, this algorithm can, as a side-effect,
			/// cause instabilites for long thin bodies). If you want to enable this algorithm 
			/// and are aware of the possible artifacts mentioned above, you can set this value to 0.2f
			/// (the value used in Havok 3.1.0 and 3.1.1).
		hkReal m_contactRestingVelocity;

			/// Tell the system what should happen if an objects leaves the
			/// extents of the broadphase. Read BroadPhaseBorderBehaviour for more details.
			/// This value defaults to BROADPHASE_BORDER_ASSERT
		hkEnum<enum BroadPhaseBorderBehaviour,hkInt8> m_broadPhaseBorderBehaviour;

			/// Defines the area dealt with by the collision detection broadphase. The size
			/// specified is a cuboid in world units - this cuboid defines the broadphase
			/// limits. There is a significant performance cost for objects that leave this
			/// area. The default size is a cube of side 1000, centred at the origin.
		class hkAabb m_broadPhaseWorldAabb;
		
			/// The collision tolerance. This is used to create and keep contact points even for
			/// non penetrating objects. This dramatically increases the stability of the
			/// system. The default collision tolerance is 0.1f.
			/// It is safe to decrease this value slightly when continuous simulation is enabled. This allows
			/// for more precise creation of contact points and better quality at the cost of generating 
			/// more TOI events. 
		hkReal m_collisionTolerance;

			/// The collision filter used by the hkWorld. By default this is HK_NULL.
			/// If left as HK_NULL, a hkNullCollisionFilter is set as the default filter in the
			/// world. This disables no collisions.
		class hkCollisionFilter* m_collisionFilter;

			/// The max linear velocity you expect the continuous physics to work correctly.
			/// This value is used to set internal tolerances. It defaults to 200 [meters per second]
			/// Note: This value does not limit the maximum linear velocities of rigid bodies.
			/// You have to set hkRigidBody::m_maxLinearVelocity for each body yourself.
		hkReal m_expectedMaxLinearVelocity;
			
			/// The min delta time you expect between two PSI. 
			/// This value is only used to set internal tolerances. It defaults to 1/30.0f second
			/// Note: This value does not limit the delta time.
		hkReal m_expectedMinPsiDeltaTime;


			/// The memory "watchdog" used by the world. By default this is HK_NULL, i.e. the memory
			/// for the world is not checked. If this is set, hkWorldMemoryWatchDog::watchMemory will be
			/// called every step, which gives the hkWorldMemoryWatchDog a chance to prevent the memory
			/// exceeding a desired threshold.
		class hkWorldMemoryWatchDog* m_memoryWatchDog;


		//
		// Optimizations
		//

			/// This variable is used to subdivide the x-axis in the 3-axisSweepAndPrune space
			/// into regular intervals. Setting this value slows down the normal operations by a
			/// very small amount, but can speed up asynchronous broadphase queries by up to 10
			/// times. Values have to be of power of 2. A typical good value would be 64.
			/// The default is 0, i.e. markers are not enabled by default.
			/// NOTE: There is currently a known issue HVK-2553 associated with markers which
			/// can cause a possible crash during simulation.  The fix is scheduled for the next
			/// release. You should leave this value at 0 in the mean time.
		hkInt32 m_broadPhaseNumMarkers;
		

			/// Specifies how aggressively the engine accepts new contact points
			/// Defaults to CONTACT_POINT_REJECT_MANY
		hkEnum<enum ContactPointGeneration, hkInt8> m_contactPointGeneration;


		//
		// Solver settings
		//

			/// Parameter of the world's constraint solver. Default sets of values can be
			/// automatically set using the setupSolverInfo() method by passing the required
			/// hkSolverType. See Documentation for details on customising these values.
		hkReal m_solverTau;
		
			/// Parameter of the world's constraint solver. Default sets of values can be
			/// automatically set using the setupSolverInfo() method by passing the required
			/// hkSolverType. See Documentation for details on customising these values.
			/// The default is 0.6f
		hkReal m_solverDamp;
		
			/// Parameter of the world's constraint solver. Default sets of values can be
			/// automatically set using the setupSolverInfo() method by passing the required
			/// hkSolverType. See Documentation for details on customising these values.
			/// The default is 4.
		hkInt32 m_solverIterations;
		
		hkUint32 m_unusedPadding;

		//
		// Internal algorithm settings
		//

	
			/// (Collision config) The early out distance default for Linear casts. Defaults to
			/// 0.01 units, usually about a 10th of collision tolerance so change this if you
			/// change the collision tolerance value.
		hkReal m_iterativeLinearCastEarlyOutDistance;
		
			/// (Collision config) The max number of iterations the Linear casts will make.
			/// Defaults to 20.
		hkInt32 m_iterativeLinearCastMaxIterations;
		
			/// The high deactivation period. By default this is 0.2.
		hkReal m_highFrequencyDeactivationPeriod;
		
			/// The low deactivation period. By default this is 10.
		hkReal m_lowFrequencyDeactivationPeriod;


			/// This says whether bodies are to be activated when they get moved around with any
			/// of the setPositon/setRotation/setTransform calls. By default this is true.
		hkBool m_shouldActivateOnRigidBodyTransformChange;

			/// Advanced Parameter:
			/// Rotates the normal of the contact point for TOI contact point resolution.
			/// The range of this parameter should be between 0 and 1 (0.1f by default).
			/// Setting this parameter bigger than 0 simulates an extra surface roughness,
			/// thereby allowing sliding objects to get an extra separating velocity and
			/// therefore reducing the frequency of subsequent TOI.
			/// In short: Setting this parameter to >0 adds extra bounce and can greatly
			/// reduce CPU.
		hkReal m_toiCollisionResponseRotateNormal;


		//
		// Debugging flags
		//

			/// Set to false if you want deactivation to be disabled. See the frequency settings
			/// for deactivation too.
		hkBool m_enableDeactivation;
				
			/// Simulation type. By default this is SIMULATION_TYPE_CONTINUOUS. See hkWorld::SimulationType for more details.
		hkEnum<enum SimulationType, hkInt8> m_simulationType;

			/// Sets whether the world is automatically segmented into separate islands each step.
			/// This should always be enabled. The default is enabled.
		hkBool m_enableSimulationIslands;

			/// Forces actions to be processed in a single thread. If you have a multi threaded simulation and
			/// you are using actions which influence objects other then the action's entities (listed in hkAction::getEntities(), this flag must
			/// be set to true (e.g. hkVehicleInstance applies forces on objects laying on the ground and uses a phantom).
		hkBool m_processActionsInSingleThread; //+default(true)


			/// Sets the frameTime to the timeOfNextPsi when those times are close together (< 1/100 of physicsStep) and frameDeltaTime is not smaller than a 1/10 of physicsStep.
			///
			/// Synchronization of frameTime and physicsTime is performed by default, and is useful to control numerical drift when e.g.
			/// running simulation at half the frequency of the frame rate. A side effect is such that the total time of simulation may 
			/// slightly vary from the sum of frameDeltaTime parameters passed to world->simulateAsynchronously(frameDeltaTime, physicsStep).
			/// If you use your hkWorld for testing and precise measurements you may prefer to disable synchronization.
		hkBool m_synchronizeFrameAndPhysicsTime; //+default(true)

		virtual ~hkWorldCinfo() {}
		hkWorldCinfo( hkFinishLoadedObjectFlag flag );

};

#endif // HKDYNAMICS_WORLD_HKWORLDCINFO_XML_H

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
