/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*
** GET Methods
*/

const hkReal &DefaultAnalogDriverInput::getSlopeChangePointX() const
{
	return m_slopeChangePointX;
}

const hkReal &DefaultAnalogDriverInput::getInitialSlope() const
{
	return m_initialSlope;
}

const hkReal &DefaultAnalogDriverInput::getDeadZone() const
{
	return m_deadZone;
}

/*
** SET Methods
*/

void DefaultAnalogDriverInput::setSlopeChangePointX(const hkReal &slope_change_point_x)
{
	m_slopeChangePointX = slope_change_point_x;
}

void DefaultAnalogDriverInput::setInitialSlope(const hkReal &initial_slope)
{
	m_initialSlope = initial_slope;
}

void DefaultAnalogDriverInput::setDeadZone(const hkReal &dead_zone)
{
	m_deadZone = dead_zone;
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
