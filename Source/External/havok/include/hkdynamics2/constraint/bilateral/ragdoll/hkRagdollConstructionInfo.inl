/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



/////////////////////////////////////////////////////////
// hkRagdollConstructionInfo info initialization functions
/////////////////////////////////////////////////////

inline hkRagdollConstructionInfo::hkRagdollConstructionInfo()
{
	// Default values
	m_coneMinAngle =  hkMath::HK_REAL_PI/6.0f;
	m_planeMinAngle = -hkMath::HK_REAL_PI*0.45f;  
	m_planeMaxAngle =  hkMath::HK_REAL_PI*0.45f;
	m_twistMinAngle = -hkMath::HK_REAL_PI/6.0f;
	m_twistMaxAngle =  hkMath::HK_REAL_PI/6.0f;  
	m_maxFrictionTorque = 0.0f;
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
}

////////////////////////////////////////////////////////

/// \param bodyA			The attached body.
/// \param bodyB			The reference body.
/// \param pivot			The pivot point, specified in world space.
/// \param twistAxisW		The twist axis, specified in world space.
/// \param planeAxisW		The plane axis, specified in world space.
inline void hkRagdollConstructionInfo::setInWorldSpace(hkRigidBody *bodyA, hkRigidBody *bodyB, 
			                                      const hkVector4& pivot, const hkVector4& twistAxisW, 
						                          const hkVector4& planeAxisW)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
    m_pivotInA.setTransformedInversePos( bodyA->getTransform(), pivot );
    m_pivotInB.setTransformedInversePos( bodyB->getTransform(), pivot );
	m_twistAxisInA.setRotatedInverseDir( bodyA->getTransform().getRotation(), twistAxisW ); 
    m_twistAxisInA.normalize3();
	m_twistAxisInB.setRotatedInverseDir( bodyB->getTransform().getRotation(), twistAxisW ); 
	m_twistAxisInB.normalize3();
	m_planeAxisInA.setRotatedInverseDir( bodyA->getTransform().getRotation(), planeAxisW );
	m_planeAxisInA.normalize3();
	m_planeAxisInB.setRotatedInverseDir( bodyB->getTransform().getRotation(), planeAxisW );
    m_planeAxisInB.normalize3();
}
	
////////////////////////////////////////////////////////////////////////////////////////////////

/// \param bodyA			The attached body.
/// \param bodyB			The reference body.
/// \param pivotA			The pivot point, specified in bodyA space.
/// \param pivotB			The pivot point, specified in bodyB space.
/// \param twistAxisA		The twist axis, specified in bodyA space.
/// \param twistAxisB		The twist axis, specified in bodyB space.
/// \param planeAxisA		The plane axis, specified in bodyA space.
/// \param planeAxisB		The plane axis, specified in bodyB space.	
inline void hkRagdollConstructionInfo::setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                         const hkVector4& pivotA,const hkVector4& pivotB,
		                                         const hkVector4& planeAxisA,const hkVector4& planeAxisB,
						                         const hkVector4& twistAxisA, const hkVector4& twistAxisB)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_pivotInA = pivotA;
	m_pivotInB = pivotB;
	m_planeAxisInA = planeAxisA;
	m_planeAxisInA.normalize3();
	m_planeAxisInB = planeAxisB;
	m_planeAxisInB.normalize3();
	m_twistAxisInA = twistAxisA;
	m_twistAxisInA.normalize3();
	m_twistAxisInB = twistAxisB;
	m_twistAxisInB.normalize3();
}

////////////////////////////////////////////////////////////////////////////////////////////////

// hkRagdollConstructionInfo info set functions

inline void hkRagdollConstructionInfo::setTwistMinAngularLimit(hkReal rad)
{
	m_twistMinAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setTwistMaxAngularLimit(hkReal rad)
{
	m_twistMaxAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setPlaneMaxAngularLimit(hkReal rad)
{
	m_planeMaxAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setPlaneMinAngularLimit(hkReal rad)
{
	m_planeMinAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setConeMinAngularLimit(hkReal rad)
{
	m_coneMinAngle = hkMath::cos(rad); 
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setAsymmetricConeAngle( hkReal cone_min, hkReal cone_max )
{
	hkReal center_cone = (cone_max + cone_min) * 0.5f;
	hkReal diff_cone   = (cone_max - cone_min) * 0.5f;

	m_twistAxisInB.setRotatedDir( hkQuaternion( m_planeAxisInB, -center_cone ), m_twistAxisInB );

	setConeMinAngularLimit( diff_cone );
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstructionInfo::setMaxFrictionTorque(hkReal tmag)
{
	m_maxFrictionTorque = tmag;
}

////////////////////////////////////////////////////////////////////////////////////////////////

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
