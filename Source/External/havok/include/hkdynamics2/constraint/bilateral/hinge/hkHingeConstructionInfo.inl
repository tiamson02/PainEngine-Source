/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


////////////////////////////////////////////////////////////////////////////

inline hkHingeConstructionInfo::hkHingeConstructionInfo()
{
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
}

/////////////////////////////////////////////////////////////////////////////

// hkHingeConstructionInfo initialization methods

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivot			The pivot point, specified in world space.
/// \param axis				The hinge axis, specified in world space.
inline void hkHingeConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
												     const hkVector4& pivot,const hkVector4& axis) 
{
	m_bodyA = bodyA;
    m_bodyB = bodyB;

    hkVector4 axisTemp(axis);

    axisTemp.normalize3();
	hkVector4 freeAxisInA;

    freeAxisInA.setRotatedInverseDir(bodyA->getRotation(),axisTemp);
    m_axleInB.setRotatedInverseDir(bodyB->getRotation(),axisTemp);
  
    m_pivotInA.setTransformedInversePos(bodyA->getTransform(),pivot);
    m_pivotInB.setTransformedInversePos(bodyB->getTransform(),pivot);

    hkVector4Util::calculatePerpendicularVector( freeAxisInA, m_perpToAxleInA1);
   
	m_perpToAxleInA2.setCross(freeAxisInA,m_perpToAxleInA1);
}

////////////////////////////////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			The constraint pivot point, specified in bodyA space.
/// \param pivotB			The constraint pivot point, specified in bodyB space.
/// \param axisA			The hinge axis, specified in bodyA space.
/// \param axisB			The hinge axis, specified in bodyB space.
inline void hkHingeConstructionInfo::setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
											        const hkVector4& pivotA,const hkVector4& pivotB,
		                                            const hkVector4& axisA,const hkVector4& axisB)
{
	m_bodyA = bodyA;
    m_bodyB = bodyB;
    m_pivotInA = pivotA;
    m_pivotInB = pivotB;
   
	hkVector4 freeAxisInA;
    freeAxisInA = axisA;
    m_axleInB = axisB;

    freeAxisInA.normalize3();
    m_axleInB.normalize3();
   
    hkVector4Util::calculatePerpendicularVector( freeAxisInA, m_perpToAxleInA1);
    m_perpToAxleInA1.normalize3();
    m_perpToAxleInA2.setCross(freeAxisInA,m_perpToAxleInA1);
}

////////////////////////////////////////////////////////////////////////////////

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
