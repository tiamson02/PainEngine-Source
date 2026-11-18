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
const hkReal &DefaultBrake::getWheelMaxBrakingTorque(int wheel) const
{
	return m_wheelsMaxBrakingTorque [wheel];
}

const hkReal &DefaultBrake::getWheelMinPedalInputToBlock(int wheel) const
{
	return m_wheelsMinPedalInputToBlock [wheel];
}

const hkReal &DefaultBrake::getWheelMinTimeToBlock() const
{
	return m_wheelsMinTimeToBlock;
}

const bool &DefaultBrake::getWheelIsConnectedToHandbrake(int wheel) const
{
	return m_wheelsIsConnectedToHandbrake [wheel];
}

/*
** SET Methods
*/
void DefaultBrake::setWheelMaxBrakingTorque(int wheel, const hkReal &max_braking_torque)
{
	m_wheelsMaxBrakingTorque [wheel] = max_braking_torque;
}

void DefaultBrake::setWheelMinPedalInputToBlock(int wheel, const hkReal &min_pedal_input_to_block)
{
	m_wheelsMinPedalInputToBlock [wheel] = min_pedal_input_to_block;
}

void DefaultBrake::setWheelMinTimeToBlock(const hkReal &min_time_to_block)
{
	m_wheelsMinTimeToBlock = min_time_to_block;
}

void DefaultBrake::setWheelIsConnectedToHandbrake(int wheel, const bool &is_connected_to_handbrake)
{
	m_wheelsIsConnectedToHandbrake [wheel] = is_connected_to_handbrake;
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
