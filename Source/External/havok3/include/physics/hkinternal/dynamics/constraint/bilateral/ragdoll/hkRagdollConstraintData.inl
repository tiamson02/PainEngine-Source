/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Methods to set and get angle limits and friction
//

inline void hkRagdollConstraintData::setTwistMinAngularLimit(hkReal rad)
{
	m_twistMinAngle = rad;
}


inline void hkRagdollConstraintData::setTwistMaxAngularLimit(hkReal rad)
{
	m_twistMaxAngle = rad;
}


inline void hkRagdollConstraintData::setPlaneMaxAngularLimit(hkReal rad)
{
	m_planeMaxAngle = rad;
}


inline void hkRagdollConstraintData::setPlaneMinAngularLimit(hkReal rad)
{
	m_planeMinAngle = rad;
}


inline void hkRagdollConstraintData::setConeAngularLimit(hkReal rad)
{
	m_coneMinAngle = rad;
}


inline hkReal hkRagdollConstraintData::getTwistMinAngularLimit() const
{
	return m_twistMinAngle;
}


inline hkReal hkRagdollConstraintData::getTwistMaxAngularLimit() const
{
	return m_twistMaxAngle;
}


inline hkReal hkRagdollConstraintData::getPlaneMaxAngularLimit() const
{
	return m_planeMaxAngle;
}


inline hkReal hkRagdollConstraintData::getPlaneMinAngularLimit() const
{
	return m_planeMinAngle;
}


inline hkReal hkRagdollConstraintData::getConeAngularLimit() const
{
	return m_coneMinAngle;
}


inline void hkRagdollConstraintData::setAsymmetricConeAngle( hkReal cone_min, hkReal cone_max )
{
	hkReal center_cone = (cone_max + cone_min) * 0.5f;
	hkReal diff_cone   = (cone_max - cone_min) * 0.5f;

	m_basisB.m_twistAxis.setRotatedDir( hkQuaternion( m_basisB.m_planeAxis, -center_cone ), m_basisB.m_twistAxis);

	setConeAngularLimit( diff_cone );
}


inline hkReal hkRagdollConstraintData::getMaxFrictionTorque() const
{
	return m_maxFrictionTorque;
}


inline void hkRagdollConstraintData::setAngularLimitsTauFactor( hkReal mag )
{
	m_angularLimitsTauFactor = mag;

}
	
inline hkReal hkRagdollConstraintData::getAngularLimitsTauFactor(  )
{
	return m_angularLimitsTauFactor;
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
