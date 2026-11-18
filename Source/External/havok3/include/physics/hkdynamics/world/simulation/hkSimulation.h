/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SIMULATION_H
#define HK_DYNAMICS2_SIMULATION_H

#include <hkbase/hkBase.h>

#include <hkdynamics/world/hkWorld.h>

	/// Base class for hkDiscreteSimulation and hkAsynchronousSimulation.
	/// Allows a custom stepDeltaTime to be implemented facilitating different stepping strategies.
class hkSimulation: public hkReferencedObject
{
	public:
			/// See hkWorld::stepDeltaTime.
		virtual hkStepResult stepDeltaTime( hkWorld* world, float frameDeltaTime, hkReal physicsDeltaTime ) = 0;

			/// See hkWorld::recoverFromStepFailure.
		virtual hkStepResult recoverFromStepFailure( hkWorld* world, hkStepResult previousResult ) = 0;

			/// If you use a multithreaded physics, you have to call this function single threadedly before
			/// any call to stepDeltaTime();
		virtual void resetStepDeltaTime() {}

		virtual PrepareMultithreadStepResult prepareMultithreadStep() { return ARE_MAIN_THREAD_NOW_CALL_STEP_DELTA_TIME; }

		virtual void childThreadStep( hkWorld* world ) { HK_ASSERT2(0x192fa846, 0, "This function can only be called for a multithreaded simulation"); }
		
		
		virtual hkJobQueue* getJobQueue() { HK_ASSERT2(0x192fa846, 0, "This function can only be called for a multithreaded simulation"); return HK_NULL; }

		virtual void getMultithreadConfig( hkMultithreadConfig& config ) { HK_ASSERT2(0x192fa846, 0, "This function can only be called for a multithreaded simulation"); }

		virtual void setMultithreadConfig( const hkMultithreadConfig& config ) { HK_ASSERT2(0x192fa846, 0, "This function can only be called for a multithreaded simulation"); }


			/// Reintegrates bodies immediately. Only useful in asynchronous simulations.
			/// When physics and framerate run at different rates (especially when the physics
			/// is run at a much lower rate) velocity changes of rigid bodies will only be used
			/// at normal physical integration steps. That means setting a velocity of a body
			/// will not change the visible velocity immediately. This can lead to very small
			/// delays between a player action and the expected reaction. With this function
			/// you can force the reintegration of a set of entities at any time between two
			/// physics integration steps, avoiding any delays in the system at the expense of some
			/// extra CPU.
			/// Tech info:
			///       This does not perform backstepping -- it only integrates the the position from their current
			///       position1.
		virtual void reintegrateAndRecollideEntities( hkEntity** entityBatch, int numEntities, hkWorld* world ) = 0;

			/// Calculates contact information by performing discrete collision detection.
			/// It uses hkTransform from hkMotionState and ignores hkSweptTransform, therefore by default it will 
			/// perform collision detection at m_timeOfNextPsi, unless the user changes hkTransforms of bodies.
			/// Note: in case of hkPredGsk, final position will be used always.
			/// This should be only performed for bodies that have just been added, activated, or moved in the world, 
			/// and have a 'stationary' sweptTransform (with the same start & end positions).
		virtual void collideEntitiesDiscrete( hkEntity** entities, int numEntities, hkWorld* world, const hkStepInfo& stepInfo ) = 0;


		//
		// Internal 
		//

			/// Called after a body's position is changed unphysically.
			/// Invalidates TIM's, clears contact manifolds to prevent usage of 
			/// old/invalid contact information.
		virtual void resetCollisionInformationForEntities( hkEntity** entities, int numEntities, hkWorld* world ) = 0;

			/// Adds deltaTime's value to whatever time-related variables are stored in hkSimulation.
		virtual void warpTime( hkWorld* world, hkReal deltaTime ) = 0;

			/// Allow the simulation to lock world's broadphase for exclusive use.
		virtual void enterBroadphaseCriticalSection() {}

			/// Allow the simulation to unlock world's broadphase for exclusive use.
		virtual void leaveBroadphaseCriticalSection() {}

};

#endif // HK_DYNAMICS2_SIMULATION_H



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
