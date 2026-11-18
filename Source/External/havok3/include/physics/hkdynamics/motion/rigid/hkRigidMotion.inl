/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkMotionState* hkRigidMotion::getMotionState() 
{
	return &m_motionState;
}

const hkMotionState* hkRigidMotion::getMotionState() const
{
	return &m_motionState;
}

	// Get the mass inv of the rigid body.
hkReal hkRigidMotion::getMassInv() const
{
	return m_massInv;
}


const hkVector4& hkRigidMotion::getCenterOfMassLocal() const
{
	return m_motionState.getSweptTransform().m_centerOfMassLocal;
}

const hkVector4& hkRigidMotion::getCenterOfMassInWorld() const
{
	return m_motionState.getSweptTransform().m_centerOfMass1;
}


	// Return the position (Local Space origin) for this rigid body, in World space.
	// Note: The center of mass is no longer necessarily the local space origin
const hkVector4& hkRigidMotion::getPosition() const
{
	return m_motionState.getTransform().getTranslation();
}

	// Returns the rotation from Local to World space for this rigid body.
const hkQuaternion& hkRigidMotion::getRotation() const
{
	return m_motionState.getSweptTransform().m_rotation1;
}

	// Returns the rigid body (local) to world transformation.
const hkTransform& hkRigidMotion::getTransform() const
{
	return m_motionState.getTransform();
}




/*
** VELOCITY ACCESS
*/

const hkVector4& hkRigidMotion::getLinearVelocity() const
{
	return m_linearVelocity;
}

const hkVector4& hkRigidMotion::getAngularVelocity() const
{
	return m_angularVelocity;
}


void hkRigidMotion::getPointVelocity(const hkVector4& p, hkVector4& pointVelOut) const
{
	// CODE IS getLinearVelocity() + getAngularVelocity().cross(p - centerOfMassWorld);
	hkVector4 relPos; relPos.setSub4( p, getCenterOfMassInWorld() );
	pointVelOut.setCross( m_angularVelocity, relPos);
  	pointVelOut.add4( m_linearVelocity );
}



/*
** DAMPING
*/

hkReal hkRigidMotion::getLinearDamping()
{
	return m_linearDamping;
}

hkReal hkRigidMotion::getAngularDamping()
{
	return m_angularDamping;
}

void hkRigidMotion::setLinearDamping( hkReal d )
{
	m_linearDamping = d;
}


void hkRigidMotion::setAngularDamping( hkReal d )
{
	m_angularDamping = d;
}

/*
 *	SOLVER LEVEL DEACTIVATION
 */

int hkRigidMotion::getDeactivationClass()
{
	return m_motionState.m_deactivationClass;
}

int hkRigidMotion::getDeactivationCounter()
{
	return m_motionState.m_deactivationCounter;
}

void hkRigidMotion::setDeactivationCounter( int newCounter )
{
	m_motionState.m_deactivationCounter = hkUint16(newCounter);
}


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
