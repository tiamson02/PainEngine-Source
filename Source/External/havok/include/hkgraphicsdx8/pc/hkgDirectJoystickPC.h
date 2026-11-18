/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DIRECT_JOYSTICK_PC_H
#define HK_GRAPHICS_DIRECT_JOYSTICK_PC_H

#include <hkgraphics/hkgPad.h>

#define STRICT 1
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <basetsd.h>
#include <dinput.h>


typedef unsigned int HKG_DIRECT_JOYSTICK_CAPS;
// can be a combination depending on the actual device caps
#define HKG_DIRECT_JOYSTICK_NONE	0
#define HKG_DIRECT_JOYSTICK_XAXIS	1
#define HKG_DIRECT_JOYSTICK_YAXIS	1<<1
#define HKG_DIRECT_JOYSTICK_ZAXIS	1<<2
#define HKG_DIRECT_JOYSTICK_XROT	1<<3
#define HKG_DIRECT_JOYSTICK_YROT	1<<4
#define HKG_DIRECT_JOYSTICK_ZROT	1<<5
#define HKG_DIRECT_JOYSTICK_SLIDER0	1<<6
#define HKG_DIRECT_JOYSTICK_SLIDER1	1<<7
#define HKG_DIRECT_JOYSTICK_POV0	1<<8
#define HKG_DIRECT_JOYSTICK_POV1	1<<9
#define HKG_DIRECT_JOYSTICK_POV2	1<<10
#define HKG_DIRECT_JOYSTICK_POV3	1<<11


typedef unsigned int HKG_DIRECT_JOYSTICK_TYPE;
// can be a combination depending on the actual device caps
#define HKG_DIRECT_JOYSTICK_TYPE_UNKOWN			0
#define HKG_DIRECT_JOYSTICK_TYPE_NORMAL			1
#define HKG_DIRECT_JOYSTICK_TYPE_WHEEL			2
#define HKG_DIRECT_JOYSTICK_TYPE_GAMEPAD		4
#define HKG_DIRECT_JOYSTICK_TYPE_FLIGHTSTICK	8



// A DirectInput 8 device wrapper. 

class hkgDirectJoystickPC
{
	friend class hkgWindowDX8PC;

public:

	inline bool isValid();

	// call this a regular intervals. State flips at a higher frequencey will not be detected. 30hz recomended.
	bool poll();

	//
	// Cur state
	//

	// range[ 0.0f -> 1.0f ]	
	inline float getXAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getYAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getZAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getXRotation() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getYRotation() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getZRotation() const;

	// 2 sliders, range[ 0.0f -> 1.0f ]
	inline float getSlider(unsigned int s) const;

	// returns degrees from north (away from user), clockwize
	// so 0.0f is straight ahead, 90.0f is right, 270.0f is left etc
	// -1.0f is centered
	inline float getPov(unsigned int s) const;

	// 128 buttons 
	inline bool getButton(unsigned int b) const; 



	//
	// Prev state
	//

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevXAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevYAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevZAxis() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevXRotation() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevYRotation() const;

	// range[ 0.0f -> 1.0f ]	
	inline float getPrevZRotation() const;

	// 2 sliders, range[ 0.0f -> 1.0f ]
	inline float getPrevSlider(unsigned int s) const;

	// returns degrees from north (away from user), clockwize
	// so 0.0f is straight ahead, 90.0f is right, 270.0f is left etc
	// -1.0f is centered
	inline float getPrevPov(unsigned int s) const;

	// 128 buttons 
	inline bool getPrevButton(unsigned int b) const; 



	// Caps:

	inline unsigned int getNumSupportedButtons() const;

	inline HKG_DIRECT_JOYSTICK_CAPS getCaps() const;
	inline HKG_DIRECT_JOYSTICK_TYPE getType() const;

	// 
	// For _internal_ enumeration only.
	//

	inline void addCaps( HKG_DIRECT_JOYSTICK_CAPS caps );
	inline LPDIRECTINPUT8& getDI();
	inline LPDIRECTINPUTDEVICE8& getDevice();

protected:
	
	hkgDirectJoystickPC(HWND owner);
	~hkgDirectJoystickPC();

	bool initDInput(HWND owner);
	void freeDInput();

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgDirectJoystickPC );

	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pJoystick;
	DIJOYSTATE2				m_curJoystickState;
	DIJOYSTATE2				m_prevJoystickState;

	HKG_DIRECT_JOYSTICK_CAPS	m_caps;
	HKG_DIRECT_JOYSTICK_TYPE    m_type;
	unsigned int				m_numButtons;

	bool					m_forceFeedback;
};

#include <hkgraphicsdx8/pc/hkgDirectJoystickPC.inl>

#endif // HK_GRAPHICS_DIRECT_JOYSTICK_PC_H

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
