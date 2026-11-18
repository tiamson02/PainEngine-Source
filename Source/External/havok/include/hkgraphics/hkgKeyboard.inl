/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
inline hkgKeyboard::hkgKeyboard()
: m_valid(false)
{
	hkString::memSet( m_curState, 0, sizeof(m_curState) );
	hkString::memSet( m_prevState, 0, sizeof(m_prevState) );
}

inline bool hkgKeyboard::getKeyState( HKG_KEYBOARD_VKEY key ) const
{
	HK_ASSERT( key < HKG_KEYBOARD_NUM_VKEYS );
	return m_curState[key];
}

inline bool hkgKeyboard::getPrevKeyState( HKG_KEYBOARD_VKEY key ) const
{
	HK_ASSERT( key < HKG_KEYBOARD_NUM_VKEYS );
	return m_prevState[key];
}

inline bool hkgKeyboard::isValid() const
{
	return m_valid;
}

inline bool hkgKeyboard::wasKeyPressed(HKG_KEYBOARD_VKEY key ) const
{
	return m_curState[key] && (!m_prevState[key]);
}	

inline bool hkgKeyboard::wasKeyReleased(HKG_KEYBOARD_VKEY key ) const
{
	return (!m_curState[key]) && m_prevState[key];
}

inline void hkgKeyboard::cycleState()
{
	hkString::memCpy( m_prevState, m_curState, HKG_KEYBOARD_NUM_VKEYS * sizeof(bool));
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
