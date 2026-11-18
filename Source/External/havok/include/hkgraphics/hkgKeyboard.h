/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_KEYBOARD
#define HK_GRAPHICS_KEYBOARD

typedef unsigned char HKG_KEYBOARD_VKEY;

#define HKG_VKEY_BACK           0x08
#define HKG_VKEY_TAB            0x09

#define HKG_VKEY_RETURN         0x0D
#define HKG_VKEY_SHIFT          0x10
#define HKG_VKEY_CONTROL        0x11

#define HKG_VKEY_CAPITAL        0x14

#define HKG_VKEY_ESCAPE         0x1B
#define HKG_VKEY_SPACE          0x20
#define HKG_VKEY_END            0x23
#define HKG_VKEY_HOME           0x24

#define HKG_VKEY_LEFT           0x25
#define HKG_VKEY_UP             0x26
#define HKG_VKEY_RIGHT          0x27
#define HKG_VKEY_DOWN           0x28

#define HKG_VKEY_INSERT         0x2D
#define HKG_VKEY_DELETE         0x2E

/* HKG_VKEY_0 thru HKG_VKEY_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* HKG_VKEY_A thru HKG_VKEY_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define HKG_VKEY_LWIN           0x5B
#define HKG_VKEY_RWIN           0x5C
#define HKG_VKEY_APPS           0x5D
#define HKG_VKEY_NUMPAD0        0x60
#define HKG_VKEY_NUMPAD1        0x61
#define HKG_VKEY_NUMPAD2        0x62
#define HKG_VKEY_NUMPAD3        0x63
#define HKG_VKEY_NUMPAD4        0x64
#define HKG_VKEY_NUMPAD5        0x65
#define HKG_VKEY_NUMPAD6        0x66
#define HKG_VKEY_NUMPAD7        0x67
#define HKG_VKEY_NUMPAD8        0x68
#define HKG_VKEY_NUMPAD9        0x69

#define HKG_VKEY_MULTIPLY       0x6A
#define HKG_VKEY_ADD            0x6B
#define HKG_VKEY_SEPARATOR      0x6C
#define HKG_VKEY_SUBTRACT       0x6D
#define HKG_VKEY_DECIMAL        0x6E
#define HKG_VKEY_DIVIDE         0x6F

#define HKG_VKEY_F1             0x70
#define HKG_VKEY_F2             0x71
#define HKG_VKEY_F3             0x72
#define HKG_VKEY_F4             0x73
#define HKG_VKEY_F5             0x74
#define HKG_VKEY_F6             0x75
#define HKG_VKEY_F7             0x76
#define HKG_VKEY_F8             0x77
#define HKG_VKEY_F9             0x78
#define HKG_VKEY_F10            0x79
#define HKG_VKEY_F11            0x7A
#define HKG_VKEY_F12            0x7B
#define HKG_VKEY_F13            0x7C
#define HKG_VKEY_F14            0x7D
#define HKG_VKEY_F15            0x7E
#define HKG_VKEY_F16            0x7F
#define HKG_VKEY_F17            0x80
#define HKG_VKEY_F18            0x81
#define HKG_VKEY_F19            0x82
#define HKG_VKEY_F20            0x83
#define HKG_VKEY_F21            0x84
#define HKG_VKEY_F22            0x85
#define HKG_VKEY_F23            0x86
#define HKG_VKEY_F24            0x87

#define HKG_VKEY_NUMLOCK        0x90
#define HKG_VKEY_SCROLL         0x91
#define HKG_VKEY_LSHIFT         0xA0
#define HKG_VKEY_RSHIFT         0xA1
#define HKG_VKEY_LCONTROL       0xA2
#define HKG_VKEY_RCONTROL       0xA3
#define HKG_VKEY_LMENU          0xA4
#define HKG_VKEY_RMENU          0xA5


#define HKG_KEYBOARD_NUM_VKEYS  0xFF // 255 possible virtual key values

/// A simple keyboard state wrapper. This is not indended to do more
/// than that, so is not meant to be a base class for the actual keyboard 
/// for instance. The #defines used are just a copy of the winuser.h VK_ 
/// codes in Windows but included here so that we can fake them if wanted 
/// on a console etc which may have a usb keyborad etc or just from a pad.
class hkgKeyboard
{
		friend class hkgWindow;

	public:

			/// Is this keyboard valid? In other words does it represent a proper keyboard.
		inline bool isValid() const;

			/// Get the current key state in an unfiltered form as a pure virtual key. Virtual keys
			/// a the un translated, raw key values from the keyboard and so have no case
			/// and no special language characteristics. A virtual key for funtion keys 
			/// and other OEM keys are present, along with the captial version of 'A'-'Z'
			/// and '0'-'9'. But such letters as small 'c' is actually 'C' and 'SHIFT' perhaps.
		inline bool getKeyState( HKG_KEYBOARD_VKEY key ) const; 

			/// Get the previous value of the given HKG_VKEY. See the getVirtualKeyState() method
			/// description for more info.
		inline bool getPrevKeyState( HKG_KEYBOARD_VKEY key ) const; // virtual key state as per above

			/// Check the transition state of the given HKG_VKEY. This method will return true
			/// if the key has gone from an unpressed to a pressed state in this state frame.
		inline bool wasKeyPressed(HKG_KEYBOARD_VKEY key ) const;
	
			/// Check the transition state of the given HKG_VKEY. This method will return true
			/// if the key has gone from a pressed to an unpressed state in this state frame.
		inline bool wasKeyReleased(HKG_KEYBOARD_VKEY key ) const;

	protected:

		inline void cycleState();

		/// Only a window can have a keyboard state so this is a private constructor.
		inline hkgKeyboard(); 

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgKeyboard );
		
		bool m_curState[HKG_KEYBOARD_NUM_VKEYS];
		bool m_prevState[HKG_KEYBOARD_NUM_VKEYS];

		bool m_valid;
};

#include <hkgraphics/hkgKeyboard.inl>

#endif //HK_GRAPHICS_KEYBOARD
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
