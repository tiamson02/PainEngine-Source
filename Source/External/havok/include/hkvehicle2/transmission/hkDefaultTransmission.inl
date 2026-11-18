/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// Returns the gear ratio of the current gear
inline hkReal DefaultTransmission::getCurrentRatio() const
{
	const hkReal cur_gear_ratio = m_isReversing ? -m_reverseGearRatio : m_gearsRatio[m_currentGear];

	const hkReal total_ratio = cur_gear_ratio * m_primaryTransmissionRatio;

	return total_ratio;
}

/*
** SET METHODS
*/

inline void DefaultTransmission::setDownshiftRPM(const hkReal &rpm)
{
	m_downshiftRPM = rpm;
}

inline void DefaultTransmission::setUpshiftRPM(const hkReal &rpm)
{
	m_upshiftRPM = rpm;
}

inline void DefaultTransmission::setPrimaryTransmissionRatio (const hkReal &ratio)
{
	m_primaryTransmissionRatio = ratio;
}

inline void DefaultTransmission::setClutchDelayTime(const hkReal &time)
{
	m_clutchDelayTime = time;
}

inline void DefaultTransmission::setReverseGearRatio(const hkReal &ratio)
{
	m_reverseGearRatio = ratio;
}

inline void DefaultTransmission::setGearRatio(int gear, const hkReal &ratio)
{
	m_gearsRatio [gear] = ratio;
}

inline void DefaultTransmission::setWheelTorqueRatio(int wheel, const hkReal &torque_ratio)
{
	m_wheelsTorqueRatio [wheel] = torque_ratio;
}

/*
** GET METHODS
*/

inline const hkReal &DefaultTransmission::getDownshiftRPM() const
{
	return m_downshiftRPM;
}

inline const hkReal &DefaultTransmission::getUpshiftRPM() const
{
	return m_upshiftRPM;
}

inline const hkReal &DefaultTransmission::getPrimaryTransmissionRatio() const
{
	return m_primaryTransmissionRatio;
}

inline const hkReal &DefaultTransmission::getClutchDelayTime() const
{
	return m_clutchDelayTime;
}

inline const hkReal &DefaultTransmission::getReverseGearRatio() const
{
	return m_reverseGearRatio;
}

inline const hkReal &DefaultTransmission::getGearRatio(int gear) const
{
	return m_gearsRatio [gear];
}

inline const hkReal &DefaultTransmission::getWheelTorqueRatio(int wheel) const
{
	return m_wheelsTorqueRatio [wheel];
}


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
