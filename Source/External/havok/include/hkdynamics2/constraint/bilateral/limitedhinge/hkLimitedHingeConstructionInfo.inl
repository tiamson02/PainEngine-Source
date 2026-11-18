/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


////////////////////////////////////////////////////////////////////

inline hkLimitedHingeConstructionInfo::hkLimitedHingeConstructionInfo()
{
	m_minAngle = -hkMath::HK_REAL_PI;
	m_maxAngle = hkMath::HK_REAL_PI;
	m_maxFrictionTorque = 0;
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
}


// hkLimitedHingeConstructionInfo initialization methods

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivot			The pivot point, specified in world space.
/// \param axis				The hinge axis, specified in world space.
inline void hkLimitedHingeConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,
															hkRigidBody* bodyB,
															const hkVector4& pivot,
															const hkVector4& axis)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;

	hkVector4 axisTemp(axis);
	axisTemp.normalize3();

	m_axleInA.setRotatedInverseDir(bodyA->getRotation(),axisTemp);
	m_axleInB.setRotatedInverseDir(bodyB->getRotation(),axisTemp);

	// set pivots in body space
	m_pivotInA.setTransformedInversePos(bodyA->getTransform(),pivot);
	m_pivotInB.setTransformedInversePos(bodyB->getTransform(),pivot);

	// create constrained axis
	hkVector4Util::calculatePerpendicularVector(m_axleInA ,m_perpToAxleInA1);
	m_perpToAxleInA1.normalize3();
	m_perpToAxleInA2.setCross(m_axleInA,m_perpToAxleInA1);

	// Get the constrained axis 2 A in world space
	hkVector4 cAxisW2;
	cAxisW2.setRotatedDir(m_bodyA->getRotation(),m_perpToAxleInA2);

	// Get the vector in Body B's space
	m_perp2FreeAxisInB.setRotatedInverseDir(m_bodyB->getRotation(),cAxisW2);
}

/////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			The constraint pivot point, specified in bodyA space.
/// \param pivotB			The constraint pivot point, specified in bodyB space.
/// \param axisA			The hinge axis, specified in bodyA space.
/// \param axisB			The hinge axis, specified in bodyB space.
/// \param axisAPerp		Axis perpendicular to the hinge axis, specified in bodyA space.
/// \param axisBPerp		Axis perpendicular to the hinge axis, specified in bodyB space.
inline void hkLimitedHingeConstructionInfo::setInBodySpace(	hkRigidBody* bodyA,
															hkRigidBody* bodyB,
															const hkVector4& pivotA,
															const hkVector4& pivotB,
															const hkVector4& axisA,
															const hkVector4& axisB,
															const hkVector4& axisAPerp,
															const hkVector4& axisBPerp)
														
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;

	m_pivotInA = pivotA;
	m_pivotInB = pivotB;

	m_axleInA = axisA;
	m_axleInB = axisB;

	m_axleInA.normalize3();
	m_axleInB.normalize3();

	m_perpToAxleInA2 = axisAPerp;
	m_perpToAxleInA2.normalize3();

	m_perp2FreeAxisInB = axisBPerp;
	m_perp2FreeAxisInB.normalize3();
	m_perpToAxleInA1.setCross(m_perpToAxleInA2,m_axleInA);
}


/////////////////////////////////////////////
/// hkLimitedHingeConstructionInfo set methods

inline void hkLimitedHingeConstructionInfo::setMaxFrictionTorque(hkReal tmag)
{
	m_maxFrictionTorque = tmag;
}

/////////////////////////////////////////////

inline void hkLimitedHingeConstructionInfo::setMaxAngularLimit(hkReal rad)
{
	m_maxAngle = rad;
}

//////////////////////////////////////////////////////////////////////////

inline void hkLimitedHingeConstructionInfo::setMinAngularLimit(hkReal rad)
{
	m_minAngle = rad;
}

/////////////////////////////////////////////


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
