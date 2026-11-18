/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline bool hkgDirectJoystickPC::isValid()
{
	return (m_pDI != NULL) && (m_pJoystick != NULL);
}

inline float hkgDirectJoystickPC::getXAxis() const
{
	return (float)m_curJoystickState.lX / 1000.0f;
}

inline float hkgDirectJoystickPC::getYAxis() const
{
	return (float)m_curJoystickState.lY / 1000.0f;
}

inline float hkgDirectJoystickPC::getZAxis() const
{
	return (float)m_curJoystickState.lZ / 1000.0f;
}

inline float hkgDirectJoystickPC::getXRotation() const
{
	return (float)m_curJoystickState.lRx / 1000.0f;
}

inline float hkgDirectJoystickPC::getYRotation() const
{
	return (float)m_curJoystickState.lRy / 1000.0f;
}

inline float hkgDirectJoystickPC::getZRotation() const
{
	return (float)m_curJoystickState.lRz / 1000.0f;
}

inline float hkgDirectJoystickPC::getSlider(unsigned int s) const
{
	if ( s > 1)
		return 0.0f;
	
	return (float)m_curJoystickState.rglSlider[s] / 1000.0f;
}

inline float hkgDirectJoystickPC::getPov(unsigned int s) const
{
	if ( s > 3)
		return 0.0f;
	
	DWORD dwPOV = m_curJoystickState.rgdwPOV[s];
	BOOL centered = (LOWORD(dwPOV) == 0xFFFF);

	if (centered || ((int)dwPOV) < 0)
		return -1.0f;

	return ( (float)dwPOV ) / 100.0f; // returns degrees from north (away from user), clockwize
}

inline bool hkgDirectJoystickPC::getButton(unsigned int b) const
{
	if ( b > 127 ) // 128 buttons
		return 0.0f;

	return (m_curJoystickState.rgbButtons[b] & 0x80) != 0;
}




inline float hkgDirectJoystickPC::getPrevXAxis() const
{
	return (float)m_prevJoystickState.lX / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevYAxis() const
{
	return (float)m_prevJoystickState.lY / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevZAxis() const
{
	return (float)m_prevJoystickState.lZ / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevXRotation() const
{
	return (float)m_prevJoystickState.lRx / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevYRotation() const
{
	return (float)m_prevJoystickState.lRy / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevZRotation() const
{
	return (float)m_prevJoystickState.lRz / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevSlider(unsigned int s) const
{
	if ( s > 1)
		return 0.0f;
	
	return (float)m_prevJoystickState.rglSlider[s] / 1000.0f;
}

inline float hkgDirectJoystickPC::getPrevPov(unsigned int s) const
{
	if ( s > 3)
		return 0.0f;
	
	DWORD dwPOV = m_prevJoystickState.rgdwPOV[s];
	BOOL centered = (LOWORD(dwPOV) == 0xFFFF);

	if (centered || ((int)dwPOV) < 0)
		return -1.0f;

	return ( (float)dwPOV ) / 100.0f; // returns degrees from north (away from user), clockwize
}

inline bool hkgDirectJoystickPC::getPrevButton(unsigned int b) const
{
	if ( b > 127 ) // 128 buttons
		return 0.0f;

	return (m_prevJoystickState.rgbButtons[b] & 0x80) != 0;
}



inline unsigned int hkgDirectJoystickPC::getNumSupportedButtons() const
{
	return m_numButtons;
}

inline HKG_DIRECT_JOYSTICK_CAPS hkgDirectJoystickPC::getCaps() const
{
	return m_caps;
}

inline void hkgDirectJoystickPC::addCaps( HKG_DIRECT_JOYSTICK_CAPS caps )
{
	m_caps |= caps;
}

inline HKG_DIRECT_JOYSTICK_TYPE hkgDirectJoystickPC::getType() const
{
	return m_type;
}

inline LPDIRECTINPUT8& hkgDirectJoystickPC::getDI()
{
	return m_pDI;
}

inline LPDIRECTINPUTDEVICE8& hkgDirectJoystickPC::getDevice()
{
	return m_pJoystick;
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
