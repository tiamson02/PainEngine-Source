/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void DriverInputComponent::reinitializeInternal( const hkDriverInputComponentBP &bp )
{
}



inline DriverInputComponent::DriverInputComponent(const hkDriverInputComponentBP &bp)
{
	m_acceleratorPedalInput = 0.0f;
	m_brakePedalInput = 0.0f;
	m_steeringWheelInput = 0.0f;
	m_handbrakeOn = false;
	m_tryingToReverse = false;
}
		

inline const hkReal &DriverInputComponent::getAcceleratorPedalInput() const
{
	return m_acceleratorPedalInput;
}

inline const hkReal &DriverInputComponent::getBrakePedalInput() const
{
	return m_brakePedalInput;
}

inline const hkReal &DriverInputComponent::getSteeringWheelInput() const
{
	return m_steeringWheelInput;
}

inline const bool &DriverInputComponent::getHandbrakeOn() const
{
	return m_handbrakeOn;
}

inline const bool &DriverInputComponent::getTryingToReverse() const
{
	return m_tryingToReverse;
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
