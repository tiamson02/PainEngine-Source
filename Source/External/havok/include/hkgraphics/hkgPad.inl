/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline hkgPad::hkgPad()
: m_curButtonState(0),
  m_prevButtonState(0),
  m_valid(false)
{
}

inline bool hkgPad::isValid() const
{
	return m_valid;
}

inline void hkgPad::cycleState()
{
	m_prevButtonState = m_curButtonState;
}

inline void hkgPad::setButtonState(HKG_PAD_BUTTON newState)
{
	m_curButtonState = newState;
}

inline HKG_PAD_BUTTON hkgPad::getButtonState() const
{
	return m_curButtonState;
}

inline HKG_PAD_BUTTON hkgPad::getPrevButtonState() const
{
	return m_prevButtonState;
}

inline float hkgPad::getStickPosX(int s) const
{
	return m_stick[s].m_cur[0];
}

inline float hkgPad::getStickPosY(int s) const
{
	return m_stick[s].m_cur[1];
}

inline float hkgPad::getPrevStickPosX(int s) const
{
	return m_stick[s].m_prev[0];
}

inline float hkgPad::getPrevStickPosY(int s) const
{
	return m_stick[s].m_prev[1];
}

inline bool hkgPad::isButtonPressed(HKG_PAD_BUTTON b) const
{
	return (m_curButtonState & b) != 0;
}

inline bool hkgPad::wasButtonPressed(HKG_PAD_BUTTON b) const
{
	return (m_curButtonState & b) && ((m_prevButtonState & b) == 0);
}

inline bool hkgPad::wasButtonReleased(HKG_PAD_BUTTON b) const
{
	return (m_prevButtonState & b) && ((m_curButtonState & b) == 0);
}

inline bool hkgPad::hasStickXChanged(int stick, float tolerance ) const
{
	return hkg_fabs( m_stick[stick].m_cur[0] - m_stick[stick].m_prev[0] ) >= tolerance;
}

inline bool hkgPad::hasStickYChanged(int stick, float tolerance ) const
{
	return hkg_fabs( m_stick[stick].m_cur[1] - m_stick[stick].m_prev[1] ) >= tolerance;
}

inline const hkgPad::Stick& hkgPad::getStickState(int stick) const
{
	return m_stick[stick];
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
