/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// not standalone : included from hkdemoframework.cpp

class hkPseudoPad : public hkgPad
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO, hkPseudoPad)

		hkPseudoPad() {}

		void updatePad(hkgWindow* window, hkReal deltaTime);
};

#if defined(HK_PLATFORM_PS2) || defined(HK_PLATFORM_GC) || defined(HK_PLATFORM_XBOX)

void hkPseudoPad::updatePad(hkgWindow* window, const hkReal currentTime)
{
	const hkgPad& pad = window->getGamePad(0);
	m_valid = pad.isValid();
	if(m_valid)
	{
		// copy button state
		m_curButtonState = pad.getButtonState();
		m_prevButtonState = pad.getPrevButtonState();
		
		m_stick[0] = pad.getStickState(0);
		m_stick[1] = pad.getStickState(1);
	}
}

#else //defined(HK_PLATFORM_WIN32)

void hkPseudoPad::updatePad(hkgWindow* window, hkReal deltaTime)
{
	const hkgKeyboard& keyboard = window->getKeyboard();

	//
	// map key presses to button pad presses
	//
	{
		m_curButtonState = 0;
		m_prevButtonState = 0;

	
		int mapping[][2] = {
			{ HKG_VKEY_SPACE, HKG_PAD_BUTTON_0 },
			{ '1', HKG_PAD_BUTTON_1 },
			{ '2', HKG_PAD_BUTTON_2 },
			{ '3', HKG_PAD_BUTTON_3 },
			{ HKG_VKEY_UP, HKG_PAD_DPAD_UP },
			{ HKG_VKEY_DOWN, HKG_PAD_DPAD_DOWN },
			{ HKG_VKEY_LEFT, HKG_PAD_DPAD_LEFT },
			{ HKG_VKEY_RIGHT, HKG_PAD_DPAD_RIGHT },
			{ HKG_VKEY_SHIFT, HKG_PAD_SELECT },
			{ HKG_VKEY_RETURN, HKG_PAD_START },
			{ HKG_VKEY_DELETE, HKG_PAD_BUTTON_L1 },
			{ HKG_VKEY_END, HKG_PAD_BUTTON_R1 }
		};

		const int nmaps = sizeof(mapping) / (sizeof(int)*2);
		for(int i = 0; i < nmaps; ++i)
		{
			int from = mapping[i][0];
			int to   = mapping[i][1];

			if (keyboard.getKeyState(from))
			{
				m_curButtonState |= to;
			}

			if (keyboard.getPrevKeyState(from))
			{
				m_prevButtonState |= to;
			}
		}
	}


	
	//
	// map analog sticks 
	//
	{
		const hkReal keyResponse = 2.0f * deltaTime;
		const hkReal maxVal = 1.0f;
		const hkReal minVal = -1.0f;

		int directions[2][4]; // [stick][4 directions]
		directions[0][0] = m_curButtonState & HKG_PAD_DPAD_RIGHT;
		directions[0][1] = m_curButtonState & HKG_PAD_DPAD_LEFT;
		directions[0][2] = m_curButtonState & HKG_PAD_DPAD_DOWN;
		directions[0][3] = m_curButtonState & HKG_PAD_DPAD_UP;

		directions[1][0] = keyboard.getKeyState('L');
		directions[1][1] = keyboard.getKeyState('J');
		directions[1][2] = keyboard.getKeyState('K');
		directions[1][3] = keyboard.getKeyState('I');

		for( int s = 0; s < 2; ++s ) // each stick
		{
			Stick& stick = m_stick[s];

			for( int i = 0; i < 2; ++i ) // two axes
			{
				if( directions[s][i*2] )
				{
					stick.m_prev[i] = stick.m_cur[i];	// copy the old values
					stick.m_cur[i] += keyResponse;
					 
					// check boundaries
					if (stick.m_cur[i] > maxVal)
					{
						stick.m_cur[i] = maxVal;
					}
				}
				else if ( directions[s][i*2+1] )
				{
					stick.m_prev[i] = stick.m_cur[i];	// copy the old values
					stick.m_cur[i] -= keyResponse;
					 
					// check boundaries
					if (stick.m_cur[i] < minVal)
					{
						stick.m_cur[i] = minVal;
					}
				}
				else 
				{
					// center
					stick.m_cur[i] = 0.0f;
				
				}
			}
		}
	}
}

#endif

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
