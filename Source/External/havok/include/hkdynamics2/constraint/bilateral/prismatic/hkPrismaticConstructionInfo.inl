/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/////////////////////////////////////////////////////////////////////

inline hkPrismaticConstructionInfo::hkPrismaticConstructionInfo()
{
	m_minLimit = -hkMath::HK_REAL_MAX;
	m_maxLimit = hkMath::HK_REAL_MAX;
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;	
	m_maxFrictionForce = 0;
}

/////////////////////////////////////////////////////////////////////

// hkPrismaticConstructionInfo intialization methods

/// \param bodyA			The attached body.
/// \param bodyB			The reference body.
/// \param pivot			The pivot point, specified in world space.
/// \param axis				The axis, specified in world space.
inline void hkPrismaticConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
													const hkVector4& pivot,const hkVector4& axis)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;

	m_pivotInA.setTransformedInversePos( bodyA->getTransform(), pivot );
	m_pivotInB.setTransformedInversePos( bodyB->getTransform(), pivot );

	m_shaftInA.setRotatedInverseDir( bodyA->getTransform().getRotation(), axis );
	m_shaftInB.setRotatedInverseDir( bodyB->getTransform().getRotation(), axis );

	m_shaftInA.normalize3();
	m_shaftInB.normalize3();

	//hkVector4Util::calculatePerpendicularVector( m_shaftInA, m_perpToShaftInA1);
	//m_perpToShaftInA1.normalize3();

	hkVector4Util::calculatePerpendicularVector( m_shaftInB, m_perpToShaftInB1);
	m_perpToShaftInB1.normalize3();

	//m_perpToShaftInA2.setCross(m_shaftInA,m_perpToShaftInA1);
	m_perpToShaftInB2.setCross(m_shaftInB,m_perpToShaftInB1);

	// create the offset rotation between the bodies
	// using the orientations at this calling time
	hkRotation HK_ALIGNED_VARIABLE(aTw,16);
	aTw = m_bodyA->getTransform().getRotation();
	aTw.transpose(); 
	m_BtoAoffsetRotation.setMul( aTw, m_bodyB->getTransform().getRotation() );
}

//////////////////////////////////////////////////////////////////

/// \param bodyA			The attached body.
/// \param bodyB			The reference body.
/// \param pivotA			The pivot point, specified in bodyA space.
/// \param pivotB			The pivot point, specified in bodyB space.
/// \param axisA			The axis, specified in bodyA space.
/// \param axisB			The axis, specified in bodyB space.
inline void hkPrismaticConstructionInfo::setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
												        const hkVector4& pivotA, const hkVector4& pivotB,
		                                                const hkVector4& axisA, const hkVector4& axisB,
												        const hkVector4& axisAPerp, const hkVector4& axisBPerp)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	
	m_pivotInA = pivotA;
	m_pivotInB = pivotB;
  
	m_shaftInA = axisA;
	m_shaftInB = axisB;

	m_shaftInA.normalize3();
	m_shaftInB.normalize3();
 
	
	hkVector4 perpToShaftInA1;
	perpToShaftInA1 = axisAPerp;
	m_perpToShaftInB1 = axisBPerp;

	perpToShaftInA1.normalize3();
	m_perpToShaftInB1.normalize3();

	hkVector4 perpToShaftInA2;
	perpToShaftInA2.setCross(m_shaftInA,perpToShaftInA1);
	m_perpToShaftInB2.setCross(m_shaftInB,m_perpToShaftInB1);

	// create the offset rotation between the bodies
	// by effectively aligning the constraint spaces 
	// so that we get b to a via when the spaces are aligned

	hkRotation HK_ALIGNED_VARIABLE( a_from_cs, 16);
	hkRotation HK_ALIGNED_VARIABLE( cs_from_b, 16);
	a_from_cs.setCols( m_shaftInA, perpToShaftInA1, perpToShaftInA2 );
	cs_from_b.setRows( m_shaftInB, m_perpToShaftInB1, m_perpToShaftInB2 );
	m_BtoAoffsetRotation.setMul( a_from_cs, cs_from_b );
}

//////////////////////////////////////////////////////////////////
// hkPrismaticConstructionInfo set methods


inline void hkPrismaticConstructionInfo::setMaxLinearLimit(hkReal max)
{
	m_maxLimit = max;
}

//////////////////////////////////////////////////////////////////

inline void hkPrismaticConstructionInfo::setMinLinearLimit(hkReal min)
{
	m_minLimit = min;
}

//////////////////////////////////////////////////////////////////

inline void hkPrismaticConstructionInfo::setMaxFrictionForce(hkReal fmag)
{
	m_maxFrictionForce = fmag;
}

//////////////////////////////////////////////////////////////////


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
