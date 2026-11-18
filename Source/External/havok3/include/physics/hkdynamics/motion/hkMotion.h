/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_MOTION_H
#define HK_DYNAMICS2_MOTION_H

#include <hkmath/hkMath.h>

class hkVelocityAccumulator;
class hkStepInfo;

extern const hkClass hkMotionClass;

/// This class is used internally.
/// An object's hkMotion stores all the information relating to
/// how it moves, including its mass, position, and velocity.
/// A hkMotion is owned by exactly one object, usually a hkEntity
/// or another hkMotion.
class hkMotion: public hkReferencedObject
{
	public:

		HK_DECLARE_REFLECTION();

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MOTION);

	public:

		/// A list of all motion types. The motion type of a hkRigidBody determines what
		/// happens when the rigid body is simulated. 	
		enum MotionType
		{
				/// 
			MOTION_INVALID,
				/// A fully-simulated, movable rigid body. At construction time the engine checks
				/// the input inertia and selects MOTION_SPHERE_INERTIA or MOTION_BOX_INERTIA as
				/// appropriate.
			MOTION_DYNAMIC,
				/// Simulation is performed using a sphere inertia tensor. (A multiple of the
				/// Identity matrix). The highest value of the diagonal of the rigid body's
				/// inertia tensor is used as the spherical inertia.
			MOTION_SPHERE_INERTIA,
				/// This is the same as MOTION_SPHERE_INERTIA, except that simulation of the rigid
				/// body is "softened", which produces more stable results in large constrained
				/// systems.
			MOTION_STABILIZED_SPHERE_INERTIA,
				/// Simulation is performed using a box inertia tensor. The non-diagonal elements
				/// of the inertia tensor are set to zero. This is slower than the
				/// MOTION_SPHERE_INERTIA motions, however it can produce more accurate results,
				/// especially for long thin objects.
			MOTION_BOX_INERTIA,
				/// This is the same as MOTION_BOX_INERTIA, except that simulation of 	the rigid
				/// body is "softened", which produces more stable results in large constrained
				/// systems.
			MOTION_STABILIZED_BOX_INERTIA,
				/// Simulation is not performed as a normal rigid body. During a simulation step,
				/// the velocity of the rigid body is used to calculate the new position of the
				/// rigid body, however the velocity is NOT updated. The user can keyframe a rigid
				/// body by setting the velocity of the rigid body to produce the desired keyframe
				/// positions. The hkKeyFrameUtility class can be used to simply apply keyframes
				/// in this way. The velocity of a keyframed rigid body is NOT changed by the
				/// application of impulses or forces. The keyframed rigid body has an infinite
				/// mass when viewed by the rest of the system.
			MOTION_KEYFRAMED,
				/// This motion type is used for the static elements of a game scene, e.g. the
				/// landscape. Fixed rigid bodies are treated in a special way by the system. They
				/// have the same effect as a rigid body with a motion of type MOTION_KEYFRAMED
				/// and velocity 0, however they are much faster to use, incurring no simulation
				/// overhead, except in collision with moving bodies.
			MOTION_FIXED,
				/// A box inertia motion which is optimized for thin boxes and has less stability problems
			MOTION_THIN_BOX_INERTIA,
				/// 
			MOTION_MAX_ID
		};

			/// Get the type of the hkMotion
		virtual MotionType getType() = 0;

	public:

		//
		// Internal functions
		//
		
		inline hkMotion();

		virtual void step( const hkStepInfo& info ) = 0;

		virtual void applyForcesAndStep( const hkStepInfo& info, const hkVector4& deltaVel ) = 0;

		HK_FORCE_INLINE hkVelocityAccumulator* applyForcesAndBuildAccumulator(const hkStepInfo& info, hkVelocityAccumulator* accumulators, int solverData);

		virtual const hkVelocityAccumulator* applyAccumulator(const hkStepInfo& info, const hkVelocityAccumulator* m ) = 0;

		virtual int getAccumulatorSize() = 0;

		virtual hkMotion* clone() const = 0;

	protected:

		virtual hkVelocityAccumulator* applyForcesAndBuildAccumulator(const hkStepInfo& info, hkVelocityAccumulator* accumulators) = 0;

	public:
		friend class hkConstraintSolverSetup;
		int m_solverData;

	public:
		hkMotion( class hkFinishLoadedObjectFlag flag ) {}
};


#include <hkdynamics/motion/hkMotion.inl>


#endif // HK_DYNAMICS2_MOTION_H

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
