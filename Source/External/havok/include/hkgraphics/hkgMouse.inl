/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline hkgMouse::hkgMouse()
:m_curButtonState(0), m_prevButtonState(0), m_curX(0), m_curY(0), m_prevX(0), m_prevY(0), m_valid(false)
{
}

inline bool hkgMouse::isValid() const
{
	return m_valid;
}

inline void hkgMouse::cycleState()
{
	m_prevButtonState = m_curButtonState;
	m_prevX = m_curX;
	m_prevY = m_curY;
}

inline HKG_MOUSE_BUTTON hkgMouse::getButtonState() const
{
	return m_curButtonState;
}

inline HKG_MOUSE_BUTTON hkgMouse::getPrevButtonState() const
{
	return m_prevButtonState;
}

inline int hkgMouse::getPosX() const
{
	return m_curX;
}

inline int hkgMouse::getPosY() const
{
	return m_curY;
}

inline int hkgMouse::getPrevPosX() const
{
	return m_prevX;
}

inline int hkgMouse::getPrevPosY() const
{
	return m_prevY;
}

inline bool hkgMouse::wasButtonPressed(HKG_MOUSE_BUTTON b) const
{
	return (m_curButtonState & b) && ((m_prevButtonState & b) == 0);
}

inline bool hkgMouse::wasButtonReleased(HKG_MOUSE_BUTTON b) const
{
	return (m_prevButtonState & b) && ((m_curButtonState & b) == 0);
}

inline bool hkgMouse::hasXChanged(int tolerance) const
{
	return hkg_fabs( m_curX - m_prevX ) >= tolerance;
}

inline bool hkgMouse::hasYChanged(int tolerance) const
{
	return hkg_fabs( m_curY - m_prevY ) >= tolerance;
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
