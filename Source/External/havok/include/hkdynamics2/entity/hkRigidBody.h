/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGID_BODY_H
#define HK_DYNAMICS2_RIGID_BODY_H

#include <hkdynamics2/entity/hkEntity.h>
#include <hkdynamics2/motion/rigid/hkRigidMotion.h>
#include <hkdynamics2/entity/hkRigidBodyDeactivator.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>
#include <hkdynamics2/entity/hkRigidBodyDeactivatorTypes.h>

	/// This is the basic rigid body class. Rigid bodies are objects whose shape never changes.
	/// They are central to Havok dynamics, as they easily allow for rapid physical simulation
	/// in real time.
	/// An hkRigidBody wraps an hkRigidMotion that stores all the information relating to how
	/// the rigid body moves, including its mass and velocity. It also has an hkCollidable member
	/// that contains the information needed for the body to work with the collision detection
	/// system, such as its shape.
class hkRigidBody : public hkEntity
{
	public:

			/// The constructor takes the information from the passed info parameter.
			/// This contains all the information necessary to create a valid rigid body.
		hkRigidBody( const hkRigidBodyConstructionInfo& info );
	
			/// Default destructor.
		inline ~hkRigidBody();

			/// Get construction info from this entity.
		inline void getConstructionInfo(hkRigidBodyConstructionInfo& info);


			//
			// MASS, INERTIA AND DENSITY PROPERTIES.
			//

			/// Gets the mass of the rigid body.
		inline hkReal getMass() const;

			/// Gets the 1.0/mass of the rigid body.
		inline hkReal getMassInv() const;

			/// Sets the mass of the rigid body.
		inline void setMass(hkReal m);
		
			/// Gets the inertia tensor (around the center of mass) in local space.
		inline void getInertiaLocal(hkMatrix3& inertiaOut) const;

			/// Gets the inertia tensor (around the center of mass) in world space.
		inline void getInertiaWorld(hkMatrix3& inertiaOut) const;

			/// Sets the inertia tensor of the rigid body. Advanced use only.
		inline void setInertiaLocal(const hkMatrix3& inertia);

			/// Sets the inertia tensor of the rigid body by supplying its inverse. Advanced use only.
		inline void setInertiaInvLocal(const hkMatrix3& inertiaInv);

			/// Gets the inverse inertia tensor in local space.
		inline void getInertiaInvLocal(hkMatrix3& inertiaInv) const;

			/// Gets the inverse inertia tensor in world space.
		inline void getInertiaInvWorld(hkMatrix3& inertiaInvOut) const;


			//
			// CENTER OF MASS.
			//

			/// Explicitly sets the center of mass of the rigid body in local space.
			/// Note that this does <b>not</b> change the position of the rigid body.
		inline void setCenterOfMassLocal(const hkVector4& centerOfMass);
		
			/// Gets the center of mass of the rigid body in the rigid body's local space.
		inline const hkVector4& getCenterOfMassLocal();
	
			/// Gets the center of mass of the rigid body in world space.
		inline const hkVector4& getCenterOfMassInWorld();


			//
			// POSITION ACCESS.
			//

			/// Returns the position (the local space origin) for this rigid body, in world space.
			/// Note that the center of mass may no longer be the local space origin.
		inline const hkVector4& getPosition() const;

			/// Sets the position (the local space origin) of this rigid body, in world space.
		inline void setPosition(const hkVector4& position);

			/// Returns the rotation from local to world space for this rigid body.
		inline const hkQuaternion& getRotation() const;

			/// Sets the rotation from local to world Space for this rigid body.
			/// This activates the body and its simulation island if it is inactive.
		inline void setRotation(const hkQuaternion& rotation);

			/// Sets the position and rotation of the rigid body, in world space.
		inline void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation);

			/// Returns the rigid body (local) to world transformation.
		inline const hkTransform& getTransform() const;

			/// Sets the rigid body (local) to world transformation
		inline void setTransform(const hkTransform& transform);


			//
			// VELOCITY ACCESS.
			//

			/// Returns the linear velocity of the center of mass of the rigid body, in world space.
		inline const hkVector4& getLinearVelocity() const;

			/// Sets the linear velocity at the center of mass, in world space.
			/// This activates the body and its simulation island if it is inactive.
		inline void	setLinearVelocity(const hkVector4& newVel);

			/// Returns the angular velocity around the center of mass, in world space.
		inline const hkVector4& getAngularVelocity() const;

			/// Sets the angular velocity around the center of mass, in world space.
			/// This activates the body and its simulation island if it is inactive.
		inline void	setAngularVelocity(const hkVector4& newVel);

			/// Gets the velocity of point p on the rigid body in world space.
		inline void getPointVelocity(const hkVector4& p, hkVector4& vecOut) const;


			//
			// IMPULSE APPLICATION.
			//

			/// Applies an impulse (in world space) to the center of mass.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyLinearImpulse(const hkVector4& imp);

			/// Applies an impulse (in world space) at the point p in world space.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyPointImpulse(const hkVector4& imp, const hkVector4& p);

			/// Applies an instantaneous change in angular velocity (in world space) around
			/// the center of mass.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyAngularImpulse(const hkVector4& imp);


			//
			// FORCE AND TORQUE APPLICATION.
			//

			/// Applies a force (in world space) to the rigid body. The force is applied to the
			/// center of mass.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyForce(const hkVector4& force);

			/// Applies a force (in world space) to the rigid body at the point p in world space.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyForce(const hkVector4& force, const hkVector4& p);

			/// Applies the specified torque (in world space) to the rigid body.
			/// This activates the body and its simulation island if it is inactive.
		inline void applyTorque(const hkVector4& torque);


			//
			// DAMPING.
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
			// DEACTIVATION
			//

			/// Sets the deactivator for this rigid body.
		inline void setDeactivator( hkRigidBodyDeactivator* deactivator );

			/// Sets the deactivator for this rigid body using a hkRigidBodyDeactivatorType.
		void setDeactivator( hkRigidBodyDeactivatorType rigidBodyDeactivatorType );

			/// Gets the deactivator for this rigid body.
		inline hkRigidBodyDeactivator* getDeactivator();
	

			//
			// MOTION
			//

			/// Does this body currently have keyframed motion state?
			/// A body can be permanently of this motion state (HK_KEYFRAMED_MOTION given on construction),
			/// or it can be changed from a dynamic state using setKeyframed(true). To see whether a keyframed body
			/// has been changed for a dynamic state, use the getStoredDynamicMotion() method.
		inline bool isKeyframed() const;

			/// This method lets you turn a moveable rigid body into a keyframed rigid body, and back again. 
			/// A moveable rigid body has a motion type that is not HK_FIXED_MOTION or HK_KEYFRAMED_MOTION.
			/// Note that you can only "unkeyframe" keyframed bodies that were once moveable bodies. 
			/// Calling setKeyframed(false) on a body which has not had setKeyframed(true) called on it will have no effect. 
		void setKeyframed(bool k);

			/// Returns the body's motion type.
		inline hkMotionType getMotionType() const;

			/// If the rigid body has been "set" to be keyframed using setKeyframed(true) this allows you to get access to the 
			/// original dynamic motion (which is stored), and hence get the "unkeyframed" mass and inertia of the rigid body.
			/// If the body has not had setKeyframed(true) called on it (for example if it was constructed with
			/// a HK_KEYFRAMED_MOTION type), this function returns HK_NULL.
		hkRigidMotion* getStoredDynamicMotion();

			//
			// LAZY CONSTRUCTION
			//

			/// Calls hkEntity::setShape(), setMotionDeltaAngleMultiplier() and 
			/// setDeactivatorRadiusSqrd(). 
			/// NB: This function is NOT intended to be called at runtime i.e. after
			/// the hkRigidBody has been added to an hkWorld. It is intended as
			/// a feature for use during setup e.g. if a complex toolchain makes
			/// filling all members of hkRigidBodyConstructionInfo difficult.
		bool setShape(hkShape* shape);

			//
			// TO BE DEPRECATED.
			//

			/// To Be Deprecated. This calls isKeyframed().
		bool getPinned();


			/// To Be Deprecated. This calls setKeyframed().
		void setPinned(bool p);

	public:

			//
			// INTERNAL FUNCTIONS
			// 

		inline hkRigidMotion* getRigidMotion() const;

	protected:

		inline void activateIfNeeded();
		// Note: this class should not contain any data members for cache efficiency

	private:
			// Sets the hkRigidMotion m_deltaAngleMultiplier.
		void setMotionDeltaAngleMultiplier();

			// Set hkEntity::m_deactivator->m_radiusSqrd when setShape is called.
		void setDeactivatorRadiusSqrd();
			
			// Return true if the hkRigidbody is movable
		inline bool isMovable();
};

#include <hkdynamics2/entity/hkRigidBody.inl>

#endif // HK_DYNAMICS2_RIGID_BODY_H

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
