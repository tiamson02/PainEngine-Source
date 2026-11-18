/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGID_MOTION_H
#define HK_DYNAMICS2_RIGID_MOTION_H

#include <hkdynamics2/motion/hkMotion.h>

	/// Each hkRigidBody has an hkRigidMotion that stores transform, velocity, and mass information
	/// for the object.
	/// Do not access this class directly - use hkRigidBody functions instead.
class hkRigidMotion : public hkMotion
{
	public:

			/// Default constructor - sets initial velocity to zero.
		inline hkRigidMotion(const hkVector4& position, const hkQuaternion& rotation);

			/// Get the mass of the rigid body (Note: it is better to use getMassInv if possible).
		inline hkReal getMass() const;

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
		inline const hkVector4& getCenterOfMassInLocal() const;

			/// Get the center of mass of the rigid body in world space.
		inline const hkVector4& getCenterOfMassInWorld() const;


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

			/// Sets the rigid body (local) to world transformation.
		virtual void setTransform(const hkTransform& transform);


			//
			// VELOCITY ACCESS
			//

			/// Return the linear velocity of the center of mass of the rigid body, in World space.
		inline const hkVector4& getLinearVelocity() const;

			/// Sets the linear velocity at the center of mass, in World space.
		virtual void setLinearVelocity(const hkVector4& newVel);

			/// Returns the angular velocity around the center of mass, in World space.
		inline const hkVector4& getAngularVelocity() const;

			/// Sets the angular velocity around the center of mass, in World space.
		virtual void setAngularVelocity(const hkVector4& newVel);

			/// Velocity of point p on the rigid body in World space.
		virtual void getPointVelocity(const hkVector4& p, hkVector4& vecOut) const;


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
		virtual void applyForce(const hkVector4& force) = 0;

			/// Applies a force (in world space) to the rigid body at the point p in world space.
		virtual void applyForce(const hkVector4& force, const hkVector4& p) = 0;

			/// Applies the specified torque (in world space) to the rigid body. (Note: the inline
			/// is for internal use only).
		virtual void applyTorque(const hkVector4& torque) = 0;


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

	public:

			//
			// INTERNAL FUNCTIONS
			//

		inline hkReal getStepPosition();

		virtual void setStepPosition( hkReal position, hkReal timestep );


	public:
			// static section
			// stored in m_centerOfMassInLocal.w
		//hkReal			m_massInv;
		hkReal			m_deltaAngleMultiplier;
			// Only used by the OrientedParticleMotion, but put here for cache efficiency
		hkReal			m_particleMinInertiaDiagInv;

		hkReal			m_linearDamping;
		hkReal			m_angularDamping;

			//.w is used as the invertedMass.
		hkVector4		m_centerOfMassInLocal;
		hkQuaternion	m_rotation;

			// ------------------ 2nd CacheLine64 -------------------------
			// velocity data (used by buildAccumulator)
		hkVector4		m_linearVelocity;
		hkVector4		m_angularVelocity;

		hkVector4		m_oldCenterOfMassInWorld;	// .w is the deltaAngle * deltaAngleMultiplier
		hkVector4		m_centerOfMassInWorld; // .w is the step position

			// ------------------ 3rd CacheLine64 -------------------------
		hkTransform		m_localToWorld;
};

#include <hkdynamics2/motion/rigid/hkRigidMotion.inl>

#endif // HK_DYNAMICS2_RIGID_MOTION_H

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
