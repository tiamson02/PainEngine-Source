/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */


//:HAVOK:
//
// This file is based on the Pads.h file in the PS2GL Glut distrib
// but tidied up to fit into the HKG lib better
//

#ifndef HK_GRAPHICS_WINDOW_PS2_PADS_H
#define HK_GRAPHICS_WINDOW_PS2_PADS_H

#include <eetypes.h> // needed for libpad.h
#include <libpad.h>

typedef unsigned int HKG_PS2_PAD_BUTTON;

#define HKG_PS2_PAD_SELECT      0
#define HKG_PS2_PAD_LEFT_STICK  1
#define HKG_PS2_PAD_RIGHT_STICK 2
#define HKG_PS2_PAD_START       3
#define HKG_PS2_PAD_LEFT_UP     4
#define HKG_PS2_PAD_LEFT_RIGHT  5
#define HKG_PS2_PAD_LEFT_DOWN   6
#define HKG_PS2_PAD_LEFT_LEFT   7
#define HKG_PS2_PAD_L2          8
#define HKG_PS2_PAD_R2          9
#define HKG_PS2_PAD_L1          10
#define HKG_PS2_PAD_R1          11
#define HKG_PS2_PAD_RIGHT_UP    12
#define HKG_PS2_PAD_RIGHT_RIGHT 13
#define HKG_PS2_PAD_RIGHT_DOWN  14
#define HKG_PS2_PAD_RIGHT_LEFT  15


class hkgWindowPS2Pad
{
	friend class hkgWindowPS2;
	
public:

	hkgWindowPS2Pad( unsigned int port );

	bool open();
	void read();
	bool isValid();
	
	inline bool  isDown( HKG_PS2_PAD_BUTTON button ) const;
	inline bool  isUp( HKG_PS2_PAD_BUTTON button ) const;
	inline bool  wasPushed( HKG_PS2_PAD_BUTTON button ) const;
	inline bool  wasReleased( HKG_PS2_PAD_BUTTON button ) const;
	inline float rightStickX( void ) const;
	inline float rightStickY( void ) const;
	inline float leftStickX( void )  const;
	inline float leftStickY( void )  const;

	static unsigned int initAll();
	static void readAll( void );
	
protected:

	struct tButtonsPressed
	{
		hkUint16 select : 1;
		hkUint16 i : 1;
		hkUint16 j : 1;
		hkUint16 start : 1;

		hkUint16 leftUp : 1;
		hkUint16 leftRight : 1;
		hkUint16 leftDown : 1;
		hkUint16 leftLeft : 1;

		hkUint16 l2 : 1;
		hkUint16 r2 : 1;
		hkUint16 l1 : 1;
		hkUint16 r1 : 1;

		hkUint16 rightUp : 1;
		hkUint16 rightRight : 1;
		hkUint16 rightDown : 1;
		hkUint16 rightLeft : 1;
	};

	struct tStickData
	{
		hkUint8 xVal, yVal;
		hkUint8 xCenter, yCenter;
		float xPos, yPos;
		bool isCentered;
	};

	struct tPadStatus
	{
		hkUint8 success;
		hkUint8 statLen;
		hkUint16 buttons; /* 16 buttons */
		hkUint8 r3h;
		hkUint8 r3v;
		hkUint8 l3h;
		hkUint8 l3v;
		hkUint8 kanAtsu[12]; // deal with this later
		hkUint8 whoKnows[12]; // make the structure 32 bytes long
		tStickData rightStick, leftStick;
	};

	bool UpdateStick( tStickData* stickCur, tStickData* stickLast );

	hkUint128 		DmaBuffer[16]	__attribute__ ((aligned(64)));
	tPadStatus		CurStatus						__attribute__ ((aligned(16)));
	tPadStatus		LastStatus						__attribute__ ((aligned(16)));
	unsigned int	uiPort;
	bool			bPadModeSet;
};

extern hkgWindowPS2Pad g_Pad0;
extern hkgWindowPS2Pad g_Pad1;

#include <hkgraphicsogl/ps2/hkgWindowPS2Pad.inl>

#endif // HK_GRAPHICS_WINDOW_PS2_PADS_H

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
