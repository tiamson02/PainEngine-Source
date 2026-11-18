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

inline void DefaultSuspension::setWheelStrength(int wheel, const hkReal &strength)
{
	m_wheelsStrength [wheel] = strength;
}

inline void DefaultSuspension::setWheelDampingRelaxation(int wheel, const hkReal &damping)
{
	m_wheelsDampingRelaxation [wheel] = damping;
}

inline void DefaultSuspension::setWheelDampingCompression(int wheel, const hkReal &damping)
{
	m_wheelsDampingCompression [wheel] = damping;
}


/*
** GET Methods
*/

inline const hkReal &DefaultSuspension::getWheelStrength(int wheel) const
{
	return m_wheelsStrength [wheel];
}

inline const hkReal &DefaultSuspension::getWheelDampingRelaxation(int wheel) const
{
	return m_wheelsDampingRelaxation [wheel];
}

inline const hkReal &DefaultSuspension::getWheelDampingCompression(int wheel) const
{
	return m_wheelsDampingCompression [wheel];
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
