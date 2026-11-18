/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


/////////////////////////////////////////////////////////////////////////////

inline hkPointToPathConstructionInfo::hkPointToPathConstructionInfo()
{
	m_transform_OS_KS[0].setIdentity();
	m_transform_OS_KS[1].setIdentity();
	m_maxFrictionForce = 0;
	m_angularConstrainedDOF = HK_DO_NOT_CONSTRAIN_ORIENTATION;
	m_path = HK_NULL;
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;	
}

////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB, const hkVector4& pivot, hkParametricCurve *path)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_path = path;
    hkVector4 pa;	
	pa.setTransformedInversePos(bodyA->getTransform(),pivot);
	m_transform_OS_KS[0].setTranslation(pa);
	m_path = path;
}

////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB, const hkVector4& pivotA, const hkVector4& pivotB, 
					hkParametricCurve *path)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_path = path;
	m_transform_OS_KS[0].setTranslation(pivotA);
}

////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setMaxFrictionForce(hkReal fmag)
{
	m_maxFrictionForce = fmag;
}

////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setOrientationType(hkPointToPathConstructionInfo::OrientationConstraintType otype)
{
	m_angularConstrainedDOF = otype;
}

/////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setAttachedBodyPivotTransform(const hkTransform& t)
{
	m_transform_OS_KS[0] = t;
}

/////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setAttachedBodyPivotOrientation(const hkRotation& r)
{
	m_transform_OS_KS[0].setRotation(r);
}

/////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstructionInfo::setReferenceBodyPathTransform(const hkTransform& t)
{
	m_transform_OS_KS[1] = t;
}

/////////////////////////////////////////////////////////////////////////


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
