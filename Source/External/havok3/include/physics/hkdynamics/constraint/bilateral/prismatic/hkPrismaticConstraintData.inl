/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkPrismaticConstraintData::setMaxLinearLimit(hkReal mmax)
{
	m_maxLimit = mmax;
}

inline void hkPrismaticConstraintData::setMinLinearLimit(hkReal mmin)
{
	m_minLimit = mmin;
}

inline void hkPrismaticConstraintData::setMaxFrictionForce(hkReal fmag)
{
	m_maxFrictionForce = fmag;	
}

inline hkReal hkPrismaticConstraintData::getMaxLinearLimit() const
{
	return m_maxLimit;
}

inline hkReal hkPrismaticConstraintData::getMinLinearLimit() const
{
	return m_minLimit;
}

inline hkReal hkPrismaticConstraintData::getMaxFrictionForce() const
{
	return m_maxFrictionForce;
}

inline hkConstraintMotor* hkPrismaticConstraintData::getMotor() const
{
	return m_motor;
}

inline void hkPrismaticConstraintData::setMotorTargetPosition( hkReal pos )
{
	m_motorTargetPosition = pos;
}

inline hkReal hkPrismaticConstraintData::getMotorTargetPosition()
{
	return m_motorTargetPosition;
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
