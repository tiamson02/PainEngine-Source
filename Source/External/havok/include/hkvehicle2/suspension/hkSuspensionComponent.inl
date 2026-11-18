/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


/* Interface */

inline const hkReal &SuspensionComponent::getWheelSuspensionForce(int wheel) const
{
	return m_wheelsSuspensionForce[wheel];
}

/* SET Methods */

inline void SuspensionComponent::setWheelHardpointCS (int wheel, const hkVector4 &hardpoint_cs)
{
	m_wheelsHardpointCS [wheel] = hardpoint_cs;
}

inline void SuspensionComponent::setWheelDirectionCS (int wheel, const hkVector4 &direction_cs)
{
	m_wheelsDirectionCS [wheel] = direction_cs;
}

inline void SuspensionComponent::setWheelLength (int wheel, const hkReal &length)
{
	m_wheelsLength [wheel] = length;
}

/* GET Methods */

inline const hkVector4 &SuspensionComponent::getWheelHardpointCS(int wheel) const
{
	return m_wheelsHardpointCS[wheel];
}


inline const hkVector4 &SuspensionComponent::getWheelDirectionCS(int wheel) const
{
	return m_wheelsDirectionCS[wheel];
}

inline const hkReal &SuspensionComponent::getWheelLength(int wheel) const
{
	return m_wheelsLength[wheel];
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
