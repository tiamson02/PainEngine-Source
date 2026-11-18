/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*
** SET Methods
*/

inline void DefaultEngine::setTorque (const hkReal &torque)
{
	m_torque = torque;
}

inline void DefaultEngine::setTorqueFactorAtMinRPM (const hkReal &factor)
{
	m_torqueFactorAtMinRPM = factor;
}

inline void DefaultEngine::setTorqueFactorAtMaxRPM (const hkReal &factor)
{
	m_torqueFactorAtMaxRPM = factor;
}

inline void DefaultEngine::setResistanceFactorAtMinRPM (const hkReal &factor)
{
	m_resistanceFactorAtMinRPM = factor;
}

inline void DefaultEngine::setResistanceFactorAtOptRPM (const hkReal &factor)
{
	m_resistanceFactorAtOptRPM = factor;
}

inline void DefaultEngine::setResistanceFactorAtMaxRPM (const hkReal &factor)
{
	m_resistanceFactorAtMaxRPM = factor;
}

inline void DefaultEngine::setClutchSlipRPM (const hkReal &rpm)
{
	m_clutchSlipRPM = rpm;
}


/*
** GET METHODS
*/
inline const hkReal &DefaultEngine::getTorque() const
{
	return m_torque;
}

inline const hkReal &DefaultEngine::getTorqueFactorAtMinRPM() const
{
	return m_torqueFactorAtMinRPM;
}

inline const hkReal &DefaultEngine::getTorqueFactorAtMaxRPM() const
{
	return m_torqueFactorAtMaxRPM;
}

inline const hkReal &DefaultEngine::getResistanceFactorAtMinRPM() const
{
	return m_resistanceFactorAtMinRPM;
}

inline const hkReal &DefaultEngine::getResistanceFactorAtOptRPM() const
{
	return m_resistanceFactorAtOptRPM;
}

inline const hkReal &DefaultEngine::getResistanceFactorAtMaxRPM() const
{
	return m_resistanceFactorAtMaxRPM;
}

inline const hkReal &DefaultEngine::getClutchSlipRPM() const
{
	return m_clutchSlipRPM;
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
