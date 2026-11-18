/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// hkPoweredHinge set and get methods.
//

inline void hkPoweredHingeConstraintData::setIgnoreLimits(hkBool state)
{
	m_ignoreLimits = state;
}


inline hkReal hkPoweredHingeConstraintData::Runtime::getCurrentPos() const
{
	return m_lastAngle + 2.0f*hkMath::HK_REAL_PI*m_rotations; 
}


inline hkConstraintMotor* hkPoweredHingeConstraintData::getMotor() const
{
	return m_motor;
}


inline hkBool hkPoweredHingeConstraintData::getMotorActive() const
{
	return m_motorActive;
}


inline void hkPoweredHingeConstraintData::setMotorTargetAngle( hkReal angle )
{
	m_targetAngle = angle;
}

inline hkReal hkPoweredHingeConstraintData::getMotorTargetAngle()
{
	return m_targetAngle;
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
