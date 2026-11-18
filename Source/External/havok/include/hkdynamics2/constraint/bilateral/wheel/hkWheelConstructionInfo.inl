/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

////////////////////////////////////////////////////////////////////

inline hkWheelConstructionInfo::hkWheelConstructionInfo()
{
	m_suspensionMinLimit = -0.5f;
	m_suspensionMaxLimit = 0.5f;
	m_suspensionStrength = 0.0f;
	m_suspensionDamping = 0.0f;
}

////////////////////////////////////////////////////////////////////

inline void hkWheelConstructionInfo::computeSecondaryValues()
{
	m_perpToSteeringAxisInB.setCross( m_referenceAxleInB, m_steeringAxisInB );
	hkVector4Util::calculatePerpendicularVector( m_suspensionAxisInB, m_perpToSuspensionAxisInB1 );
	m_perpToSuspensionAxisInB1.normalize3();
	m_perpToSuspensionAxisInB2.setCross( m_suspensionAxisInB, m_perpToSuspensionAxisInB1 );
}


////////////////////////////////////////////////////////////////////


/// \param bodyA				The attached body, ie the wheel.
/// \param bodyB				The reference body, ie the chassis.
/// \param pivot				The pivot point, specified in world space.
/// \param axle					The axle, specified in world space.
/// \param suspensionAxis		The suspension axis, in world space.
/// \param steeringAxis			The steering axis, in world space.	 
inline void hkWheelConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,const hkVector4& pivot,
		                   const hkVector4& axle, const hkVector4& suspensionAxis,
						   const hkVector4& steeringAxis )
{

	m_bodyA = bodyA;
	m_bodyB = bodyB;

	m_pivotInA.setTransformedInversePos( bodyA->getTransform(), pivot );
	m_pivotInB.setTransformedInversePos( bodyB->getTransform(), pivot );

	m_axleInA.setRotatedInverseDir( bodyA->getRotation(), axle );
	m_referenceAxleInB.setRotatedInverseDir( bodyB->getRotation(), axle );

	m_steeringAxisInB.setRotatedInverseDir( bodyB->getRotation(), steeringAxis );
	m_suspensionAxisInB.setRotatedInverseDir( bodyB->getRotation(), suspensionAxis );

	computeSecondaryValues();
}

////////////////////////////////////////////////////////////////////

	
/// \param bodyA				The attached body, ie the wheel.
/// \param bodyB				The reference body, ie the chassis.
/// \param pivotA				The pivot point, specified in bodyA space.
/// \param pivotB				The pivot point, specified in bodyB space.
/// \param axleA				The axle, specified in bodyA space.
/// \param axleB				The axle, specified in bodyB space.
/// \param suspensionAxisB		The suspension axis, in bodyB space.
/// \param steeringAxisB		The steering axis, in bodyB space.
inline void hkWheelConstructionInfo::setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
												    const hkVector4& pivotA,const hkVector4& pivotB,
						                            const hkVector4& axleA,const hkVector4& axleB,
								                    const hkVector4& suspensionAxisB, const hkVector4& steeringAxisB )
{

	m_bodyA = bodyA;
	m_bodyB = bodyB;

	m_pivotInA = pivotA;
	m_pivotInB = pivotB;

	m_axleInA = axleA;
	m_referenceAxleInB = axleB;

	m_steeringAxisInB = steeringAxisB;
	m_suspensionAxisInB = suspensionAxisB;

	computeSecondaryValues();
}

////////////////////////////////////////////////////////////////////

inline void hkWheelConstructionInfo::setMinSuspensionLimit(hkReal min)
{
	m_suspensionMinLimit = min;
}

////////////////////////////////////////////////////////////////////

inline void hkWheelConstructionInfo::setMaxSuspensionLimit(hkReal max)
{
	m_suspensionMaxLimit = max;
}

////////////////////////////////////////////////////////////////////

inline hkReal hkWheelConstructionInfo::getSuspensionStrength()
{
	return m_suspensionStrength;
}

////////////////////////////////////////////////////////////////////

inline hkReal hkWheelConstructionInfo::getSuspensionDamping()
{
	return m_suspensionDamping;
}

////////////////////////////////////////////////////////////////////

inline void hkWheelConstructionInfo::setSuspensionStrength( hkReal tau )
{
	m_suspensionStrength = tau;
}

////////////////////////////////////////////////////////////////////

inline void hkWheelConstructionInfo::setSuspensionDamping( hkReal dampingPercent )
{
	m_suspensionDamping = dampingPercent;
}


////////////////////////////////////////////////////////////////////

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
