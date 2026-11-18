/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGID_MOTION_H
#define HK_DYNAMICS2_RIGID_MOTION_H

#include <hkdynamics/motion/hkMotion.h>
#include <hkmath/basetypes/hkMotionState.h>

extern const hkClass hkRigidMotionClass;

	/// Each hkRigidBody has an hkRigidMotion that stores transform, velocity, and mass information
	/// for the object.
	/// Do not access this class directly - use hkRigidBody functions instead.
class hkRigidMotion : public hkMotion
{
	public:

		HK_DECLARE_REFLECTION();

			/// Default constructor - sets initial velocity to zero.
		hkRigidMotion(const hkVector4& position, const hkQuaternion& rotation);

			/// Get the mass of the rigid body (Note: it is better to use getMassInv if possible).
		hkReal getMass() const;

			/// Get the inverse mass.
		inline hkReal getMassInv() const;

			/// Set the mass of the rigid body. (Note: it is better to use setMassInv if possible).
		virtual void setMass(hkReal m);

			/// Set the inverse mass of the rigid body. 
		virtual void setMassInv(hkReal mInv);

			/// Get the inertia tensor of the rigid body in local space.
		virtual void getInertiaLocal(hkMatrix3& inertiaOut) const = 0;

			/// Get the inertia tensor of the rigid body in world space.
		virtual void getInertiaWorld(hkMatrix3& inertiaOut) const = 0;

			/// Sets the inertia tensor of the rigid body in local space. Advanced use only.
		virtual void setInertiaLocal(const hkMatrix3& inertia) = 0;

			/// Sets the inertia tensor of the rigid body by supplying its inverse. Advanced use only.
		virtual void setInertiaInvLocal(const hkMatrix3& inertiaInv) = 0;

			/// Get the inverse inertia tensor in local space.
		virtual void getInertiaInvLocal(hkMatrix3& inertiaInvOut) const = 0;

			/// Get the inverse inertia tensor in local space.
		virtual void getInertiaInvWorld(hkMatrix3& inertiaInvOut) const = 0;

			/// Explicitly set the center of mass of the rigid body in local space.
			/// This does not change the position of the rigid body.
		virtual void setCenterOfMassInLocal(const hkVector4& centerOfMass);
					
			/// Get the center of mass in local space.
		inline const hkVector4& getCenterOfMassLocal() const;

			/// Get the center of mass of the rigid body in world space.
		inline const hkVector4& getCenterOfMassInWorld() const;

		inline hkMotionState* getMotionState();

			/// Get access to it's internal motion state
		inline const hkMotionState* getMotionState() const;

			//
			// POSITION ACCESS
			//

			/// Return the position (Local Space origin) for this rigid body, in World space.
			/// Note: The center of mass is not necessarily the local space origin.
		inline const hkVector4& getPosition() const;

			/// Set the position (Local Space origin) of this rigid body, in World space.
		virtual void setPosition(const hkVector4& position);

			/// Returns the rotation from Local to World space for this rigid body.
		inline const hkQuaternion& getRotation() const;

			/// Set the rotation from Local to World Space for this rigid body.
		virtual void setRotation(const hkQuaternion& rotation);

			/// Set the position and rotation of the rigid body, in World space.
		virtual void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation);

			/// Returns the rigid body (local) to world transformation.
		inline const hkTransform& getTransform() const;

			/// approximate a transform for any given time T<br>
			/// If T is between t0 and t1 of the motion state than the algorithm is doing in interpolation
			/// otherwise the result is an extrapolation.
			/// Note: This function has a lower accuracy than hkSweptTransformUtil::lerp2 as used by Havoks collision detection
			/// and should be used when you care about speed not accuracy (e.g. graphics)
		void approxTransformAt( hkTime time, hkTransform& transformOut );

			/// Sets the rigid body (local) to world transformation.
		virtual void setTransform(const hkTransform& transform);


			//
			// VELOCITY ACCESS
			//

			/// Return the linear velocity of the center of mass of the rigid body, in world space.
		inline const hkVector4& getLinearVelocity() const;

			/// Sets the linear velocity at the center of mass, in World space.
		virtual void setLinearVelocity(const hkVector4& newVel);

			/// Returns the angular velocity around the center of mass, in world space.
		inline const hkVector4& getAngularVelocity() const;

			/// Sets the angular velocity around the center of mass, in world space.
		virtual void setAngularVelocity(const hkVector4& newVel);

			/// Velocity of point p on the rigid body in World space.
		HK_FORCE_INLINE void getPointVelocity(const hkVector4& p, hkVector4& vecOut) const;


			//
			// IMPULSE APPLICATION
			//

			/// Apply an impulse (in world space) to the center of mass.
		virtual void applyLinearImpulse(const hkVector4& imp);

			/// Apply an impulse (in world space) at the point p in world space.
		virtual void applyPointImpulse(const hkVector4& imp, const hkVector4& p) = 0;

			/// Apply an instantaneous change in angular velocity (in world space) around
			/// center of mass.
		virtual void applyAngularImpulse(const hkVector4& imp) = 0;


			//
			// FORCE AND TORQUE APPLICATION
			//

			/// Applies a force (in world space) to the rigid body. The force is applied to the
			/// center of mass.
		virtual void applyForce(const hkReal deltaTime, const hkVector4& force) = 0;

			/// Applies a force (in world space) to the rigid body at the point p in world space.
		virtual void applyForce(const hkReal deltaTime, const hkVector4& force, const hkVector4& p) = 0;

			/// Applies the specified torque (in world space) to the rigid body. (Note: the inline
			/// is for internal use only).
		virtual void applyTorque(const hkReal deltaTime, const hkVector4& torque) = 0;


			//
			// DAMPING
			//
			
			/// Naive momentum damping.
		inline hkReal getLinearDamping();

			/// Naive momentum damping.
		inline void setLinearDamping( hkReal d );

			/// Naive momentum damping.
		inline hkReal getAngularDamping();

			/// Naive momentum damping.
		inline void setAngularDamping( hkReal d );
		
			//
			//	SOLVER DEACTIVATION SETTINGS
			//
			/// get the deactivation class as defined in hkSolverInfo::DeactivationClass
		inline int getDeactivationClass();

			/// Gets the deactivation counter. If the counter is 0, the solver decided that the object is deactivated
		inline int getDeactivationCounter();

			/// set the deactivation class as defined in hkSolverInfo::DeactivationClass
		void setDeactivationClass(int deactivationClass);

			/// sets the deactivation counter
		inline void setDeactivationCounter( int newCounter );

	public:


			//
			// INTERNAL FUNCTIONS
			//

			// This method updates a given motion with the dynamic properties of the current motion, namely
			// m_linearVelocity, m_angularVelocity, m_rotation, m_oldCenterOfMassInWorld, m_centerOfMassInWorld and m_localToWorld.
		virtual void getMotionStateAndVelocities(hkRigidMotion* motionOut);

		virtual void step( const hkStepInfo& info );

	public:
		class hkMotionState m_motionState;

			// static section
		hkReal			m_massInv;

		// Only used by the SphereMotion, but put here for cache efficiency
		hkReal			m_particleMinInertiaDiagInv;

		hkReal			m_linearDamping;
		hkReal			m_angularDamping;

			// ------------------ 2nd CacheLine64 -------------------------
			// velocity data (used by buildAccumulator)
		hkVector4		m_linearVelocity;
		hkVector4		m_angularVelocity;

	public:

		// serialization ctor. Note that the time in the motion state may 
		// be wrong (commonly reset in world addEntity etc)
		hkRigidMotion( class hkFinishLoadedObjectFlag flag ) : hkMotion(flag) { }
};

#include <hkdynamics/motion/rigid/hkRigidMotion.inl>

#endif // HK_DYNAMICS2_RIGID_MOTION_H

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
