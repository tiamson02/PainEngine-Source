/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



inline hkRigidMotion* hkRigidBody::getRigidMotion() const
{
	return static_cast<hkRigidMotion*>(m_motion);
}

inline hkRigidBody::~hkRigidBody()
{
	delete getRigidMotion();
}


	// Get the mass of the rigid body.
inline hkReal hkRigidBody::getMass() const
{
	return getRigidMotion()->getMass();
}

inline hkReal hkRigidBody::getMassInv() const
{
	return getRigidMotion()->getMassInv();
}

	// Set the mass of the rigid body.
void hkRigidBody::setMass(hkReal m)
{
	getRigidMotion()->setMass(m);
}


	// Get the inertia tensor in local space
inline void hkRigidBody::getInertiaLocal(hkMatrix3& inertia) const
{
	getRigidMotion()->getInertiaLocal(inertia);
}

	// Get the inertia tensor in world space
inline void hkRigidBody::getInertiaWorld(hkMatrix3& inertia) const
{
	getRigidMotion()->getInertiaWorld(inertia);
}


	// Sets the inertia tensor of the rigid body. Advanced use only.
void hkRigidBody::setInertiaLocal(const hkMatrix3& inertia)
{
	getRigidMotion()->setInertiaLocal(inertia);
}


	// Sets the inertia tensor of the rigid body by supplying its inverse. Advanced use only.
void hkRigidBody::setInertiaInvLocal(const hkMatrix3& inertiaInv)
{
	getRigidMotion()->setInertiaInvLocal(inertiaInv);
}


	// Get the inverse inertia tensor in local space
void hkRigidBody::getInertiaInvLocal(hkMatrix3& inertiaInv) const 
{
	getRigidMotion()->getInertiaInvLocal(inertiaInv);
}

	// Get the inverse inertia tensor in World space
void hkRigidBody::getInertiaInvWorld(hkMatrix3& inertiaInv) const 
{
	getRigidMotion()->getInertiaInvWorld(inertiaInv);
}


	// Explicit center of mass in local space
void hkRigidBody::setCenterOfMassLocal(const hkVector4& centerOfMass)
{
	getRigidMotion()->setCenterOfMassInLocal(centerOfMass);
}

	// Explicit center of mass in local space
const hkVector4& hkRigidBody::getCenterOfMassLocal()
{
	return getRigidMotion()->getCenterOfMassInLocal();
}

		
const hkVector4& hkRigidBody::getCenterOfMassInWorld()
{
	return getRigidMotion()->getCenterOfMassInWorld();
}

/*
** UTILITIY
*/

	// Returns true if a hkRigidBody is movable
	// The conditions for this to be met are either:
	// The hkRigidBody is not Fixed OR
	// The hkRigidBody is Fixed AND added to a hkWorld.
inline bool hkRigidBody::isMovable()
{
	return (!isFixed() || m_owner == HK_NULL);
}

inline void hkRigidBody::activateIfNeeded()
{
	// if currently inactive and added to world
	if ( isActive() == false && m_simulationIsland != HK_NULL )
	{	
		forceActivate();
	}	
}



	// Return the position (Local Space origin) for this rigid body, in World space.
	// Note: The center of mass is no longer necessarily the local space origin
inline const hkVector4& hkRigidBody::getPosition() const
{
	return getRigidMotion()->getPosition();
}


	// Set the position (Local Space origin) of this rigid body, in World space.
inline void hkRigidBody::setPosition(const hkVector4& position)
{

	HK_ASSERT2( isMovable(),"Error: do not call setPosition on a fixed object (hkFixedRigidMotion) that has been added to a hkWorld.");

	activateIfNeeded();

		// If hkRigidBody is movable set its position
	if ( isMovable() )
	{
		getRigidMotion()->setPosition(position);
	}
}


	// Returns the rotation from Local to World space for this rigid body.
inline const hkQuaternion& hkRigidBody::getRotation() const
{
	return getRigidMotion()->getRotation();
}


	/// Set the rotation from Local to World Space for this rigid body.
inline void hkRigidBody::setRotation(const hkQuaternion& rotation)
{
	HK_ASSERT2(rotation.isOk(), "hkQuaternion used in hkRigidBody::setRotation() is not normalized/invalid!");

	HK_ASSERT2( isMovable(), "Error: do not call setRotation on a fixed object (hkFixedRigidMotion) that has been added to a hkWorld.");

	activateIfNeeded();

	// If hkRigidBody is movable set its rotation
	if ( isMovable() )
	{
		getRigidMotion()->setRotation(rotation);
	}	
}


	/// Set the position and rotation of the rigid body, in World space.
inline void hkRigidBody::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation)
{
	HK_ASSERT2(rotation.isOk(), "hkQuaternion used in hkRigidBody::setPositionAndRotation() is not normalized/invalid!");
	
	HK_ASSERT2( isMovable(),"Error: do not call setPositionAndRotation on a fixed object (hkFixedRigidMotion) that has been added to a hkWorld.");

	activateIfNeeded();

	// If hkRigidBody is movable set its position and rotation
	if ( isMovable() )
	{
		getRigidMotion()->setPositionAndRotation(position, rotation);
	}
}


	/// Returns the rigid body (local) to world transformation.
inline const hkTransform& hkRigidBody::getTransform() const
{
	return getRigidMotion()->getTransform();
}


	/// Sets the rigid body (local) to world transformation
inline void hkRigidBody::setTransform(const hkTransform& transform)
{
	HK_ASSERT2( isMovable(),"Error: do not call setTransform on a fixed object (hkFixedRigidMotion) that has been added to a hkWorld.");
	
	activateIfNeeded();

		// If hkRigidBody is movable set its transform
	if ( isMovable() )
	{
		getRigidMotion()->setTransform(transform);
	}
}


/*
** VELOCITY ACCESS
*/

	/// Return the linear velocity of the center of mass of the rigid body, in World space.
inline const hkVector4& hkRigidBody::getLinearVelocity() const
{
	return getRigidMotion()->getLinearVelocity();
}


	/// Sets the linear velocity at the center of mass, in World space.
inline void	hkRigidBody::setLinearVelocity(const hkVector4& newVel)
{
	activateIfNeeded();
	getRigidMotion()->setLinearVelocity(newVel);
}


	/// Returns the angular velocity around the center of mass, in World space.
inline const hkVector4& hkRigidBody::getAngularVelocity() const
{
	return getRigidMotion()->getAngularVelocity();
}


	/// Sets the angular velocity around the center of mass, in World space.
inline void	hkRigidBody::setAngularVelocity(const hkVector4& newVel)
{
	activateIfNeeded();
	getRigidMotion()->setAngularVelocity(newVel);
}


	/// Velocity of point p on the rigid body, in World space.
void hkRigidBody::getPointVelocity(const hkVector4& p, hkVector4& vecOut) const
{
	getRigidMotion()->getPointVelocity(p, vecOut);
}


/*
** IMPULSE APPLICATION
*/

	/// Apply an impulse to the center of mass.
inline void hkRigidBody::applyLinearImpulse(const hkVector4& imp)
{
	activateIfNeeded();
	getRigidMotion()->applyLinearImpulse(imp);
}


	/// Apply an impulse at the point p in world space.
inline void hkRigidBody::applyPointImpulse(const hkVector4& imp, const hkVector4& p)
{
	activateIfNeeded();
	getRigidMotion()->applyPointImpulse(imp, p);
}


	/// Apply an instantaneous change in angular velocity around center of mass.
inline void hkRigidBody::applyAngularImpulse(const hkVector4& imp)
{
	activateIfNeeded();
	getRigidMotion()->applyAngularImpulse(imp);
}


/*
** FORCE AND TORQUE APPLICATION
*/

	/// Applies a force to the rigid body. The force is applied to the center of mass.
inline void hkRigidBody::applyForce(const hkVector4& force)
{
	activateIfNeeded();
	getRigidMotion()->applyForce(force);
}

	/// Applies a force to the rigid body at the point p in world space.
inline void hkRigidBody::applyForce(const hkVector4& force, const hkVector4& p)
{
	activateIfNeeded();
	getRigidMotion()->applyForce(force, p);
}


	/// Applies the specified torque to the rigid body.
inline void hkRigidBody::applyTorque(const hkVector4& torque)
{
	activateIfNeeded();
	getRigidMotion()->applyTorque(torque);
}



/*
** DAMPING
*/

	/// Naive momentum damping
inline hkReal hkRigidBody::getLinearDamping()
{
	return getRigidMotion()->getLinearDamping();
}

inline hkReal hkRigidBody::getAngularDamping()
{
	return getRigidMotion()->getAngularDamping();
}



inline void hkRigidBody::setLinearDamping( hkReal d )
{
	getRigidMotion()->setLinearDamping( d );
}



inline void hkRigidBody::setAngularDamping( hkReal d )
{
	getRigidMotion()->setAngularDamping( d );
}


inline void hkRigidBody::setDeactivator( hkRigidBodyDeactivator* deactivator )
{
	hkEntity::setDeactivator( deactivator );
}

inline hkRigidBodyDeactivator* hkRigidBody::getDeactivator()
{

	return static_cast<hkRigidBodyDeactivator*>( hkEntity::getDeactivator() );
}

inline bool hkRigidBody::isKeyframed() const
{
	if(getMotionType() == HK_KEYFRAMED_MOTION)
	{
		return true;
	}
	else
	{
		return false;
	}
}


inline hkMotionType hkRigidBody::getMotionType() const
{
	return getRigidMotion()->getType();
}

///////////////////////////////////////////////////////////////

inline void hkRigidBody::getConstructionInfo(hkRigidBodyConstructionInfo& info)
{
	info.m_motionType = getMotionType();
	getInertiaLocal( info.m_inertiaTensor );
	info.m_mass = getMass();
	info.m_centerOfMass = getCenterOfMassLocal();
	info.m_friction = getMaterial().getFriction();
	info.m_linearDamping = getLinearDamping();
	info.m_angularDamping = getAngularDamping();
	info.m_shape = getCollidable().getShape();
	info.m_position = getPosition();
	info.m_rotation = getRotation();
	info.m_restitution = getMaterial().getRestitution();
	info.m_collisionFilterInfo = getCollidable().getCollisionFilterInfo();
	info.m_collisionResponse = getMaterial().m_primaryResponseType;
	info.m_processContactCallbackDelay = getProcessContactCallbackDelay();
	info.m_rigidBodyDeactivatorType = getDeactivator()->getRigidBodyDeactivatorType();
}

///////////////////////////////////////////////////////////////

		// To Be Deprecated. This calls isKeyframed()
inline bool hkRigidBody::getPinned()
{
	HK_WARN("This method hkRigidBody::getPinned() will be deprecated in the next release. Please use isKeyframed() instead.");
	return isKeyframed();
}

			// To Be Deprecated. This calls setKeyframed()
inline void hkRigidBody::setPinned(bool p)
{
	HK_WARN("This method hkRigidBody::setPinned() will be deprecated in the next release. Please use setKeyframed() instead.");
	setKeyframed(p);
}


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
