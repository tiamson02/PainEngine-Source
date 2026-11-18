/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline hkRigidMotion::hkRigidMotion(const hkVector4& position, const hkQuaternion& rotation)
{
	m_linearVelocity.setZero4();
	m_angularVelocity.setZero4();
	m_centerOfMassInLocal.setZero4();

	m_deltaAngleMultiplier = 1.0f;
	m_linearDamping = 0.0f;
	m_angularDamping = 0.0f;
	m_centerOfMassInLocal.setZero4();

	m_oldCenterOfMassInWorld = position;
	m_oldCenterOfMassInWorld(3) = 0;
	m_centerOfMassInWorld = position;
	m_localToWorld.setTranslation(position);

	m_rotation = rotation;
	m_localToWorld.setRotation(rotation);
	m_centerOfMassInWorld(3) = 0;

//	HK_RECOVERABLE_ASSERT( (int(this) & 0x3f) == 0, "The memory system did not align the hkRigidMotion on a 64 cache line" );
//	HK_RECOVERABLE_ASSERT_END();
}


	// Get the mass inv of the rigid body.
inline hkReal hkRigidMotion::getMassInv() const
{
	return m_centerOfMassInLocal(3);
}


	// Get the mass of the rigid body.
inline hkReal hkRigidMotion::getMass() const
{
	hkReal massInv = getMassInv();
	if (massInv == 0)
	{
		return 0;
	}
	else
	{
		return 1 / massInv;
	}
}




inline const hkVector4& hkRigidMotion::getCenterOfMassInLocal() const
{
	return m_centerOfMassInLocal;
}


inline const hkVector4& hkRigidMotion::getCenterOfMassInWorld() const
{
	return m_centerOfMassInWorld;
}


	// Return the position (Local Space origin) for this rigid body, in World space.
	// Note: The center of mass is no longer necessarily the local space origin
const hkVector4& hkRigidMotion::getPosition() const
{
	return m_localToWorld.getTranslation();
}

	// Returns the rotation from Local to World space for this rigid body.
inline const hkQuaternion& hkRigidMotion::getRotation() const
{
	return m_rotation;
}

	/// Returns the rigid body (local) to world transformation.
inline const hkTransform& hkRigidMotion::getTransform() const
{
	return m_localToWorld;
}




/*
** VELOCITY ACCESS
*/

	/// Return the linear velocity of the center of mass of the rigid body, in World space.
inline const hkVector4& hkRigidMotion::getLinearVelocity() const
{
	return m_linearVelocity;
}


	/// Returns the angular velocity around the center of mass, in World space.
inline const hkVector4& hkRigidMotion::getAngularVelocity() const
{
	return m_angularVelocity;
}




/*
** DAMPING
*/

inline hkReal hkRigidMotion::getLinearDamping()
{
	return m_linearDamping;
}

inline hkReal hkRigidMotion::getAngularDamping()
{
	return m_angularDamping;
}

inline void hkRigidMotion::setLinearDamping( hkReal d )
{
	m_linearDamping = d;
}


inline void hkRigidMotion::setAngularDamping( hkReal d )
{
	m_angularDamping = d;
}


inline hkReal hkRigidMotion::getStepPosition()
{
	return m_centerOfMassInWorld(3);
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
