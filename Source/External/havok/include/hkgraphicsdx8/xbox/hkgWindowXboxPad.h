/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_GRAPHICS_WINDOW_XBOX_PAD_H
#define HK_GRAPHICS_WINDOW_XBOX_PAD_H

//-----------------------------------------------------------------------------
// Taken from Xbox framework file: XBInput.h
//-----------------------------------------------------------------------------

namespace hkgWindowXboxPad
{
	struct hkgXboxGamepad : public XINPUT_GAMEPAD
	{
		// The following members are inherited from XINPUT_GAMEPAD:
		//    WORD    wButtons;
		//    BYTE    bAnalogButtons[8];
		//    SHORT   sThumbLX;
		//    SHORT   sThumbLY;
		//    SHORT   sThumbRX;
		//    SHORT   sThumbRY;

		// Thumb stick values converted to range [-1,+1]
		FLOAT      fX1;
		FLOAT      fY1;
		FLOAT      fX2;
		FLOAT      fY2;
    
		// State of buttons tracked since last poll
		WORD       wLastButtons;
		BOOL       bLastAnalogButtons[8];
		WORD       wPressedButtons;
		BOOL       bPressedAnalogButtons[8];

		// Rumble properties
		XINPUT_RUMBLE   Rumble;
		XINPUT_FEEDBACK Feedback;

		// Device properties
		XINPUT_CAPABILITIES caps;
		HANDLE     hDevice;

		// Flags for whether gamepad was just inserted or removed
		BOOL       bInserted;
		BOOL       bRemoved;
	};

	extern	XINPUT_STATE	g_InputStates[4];
	extern	hkgXboxGamepad	g_Gamepads[4];

	HRESULT	CreateGamepads( hkgXboxGamepad** ppGamepads = NULL );
	VOID	GetInput( hkgXboxGamepad* pGamepads = NULL );
}

#endif // HK_GRAPHICS_WINDOW_XBOX_PAD_H

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
