/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled

/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

//:HAVOK:
//
// This file is based on the Pads.cpp file in the PS2GL Glut distrib
// but tidied up to fit into the HKG lib better
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <eetypes.h>
#include <sifdev.h>
#include <sifrpc.h>

#include <hkgraphicsogl/ps2/hkgWindowPS2Pad.h>

/********************************************
 * constants
 */

#define kPad0	0
#define kPad1	1

#define kPort0	0
#define kSlot0	0

#define kPadModeStandard 0x4
#define kPadModeAnalog   0x7

#define kPadSetLockModeUnchanged 0
#define kPadSetLockModeLock 3
#define kPadSetLockModeUnlock 1

#define kStickMaxRadius 120
#define kStickDeadRadius 25


hkgWindowPS2Pad g_Pad0( kPad0 );
hkgWindowPS2Pad g_Pad1( kPad1 );

inline bool hkFuzzyEqualsi( int a, int b, int epsilon )
{
	return ( b - epsilon <= a && a <= b + epsilon );
}

unsigned int hkgWindowPS2Pad::initAll( )
{
	unsigned int np = 0; 

	scePadInit(0); // "must be zero"

	if ( !g_Pad0.open() ) 
	{
		printf("Couldn't open Pad0.\n");
	}
	else
		np++;

	if ( !g_Pad0.open() ) 
	{
		printf("Couldn't open Pad1.\n");
	}
	else
		np++;

	return np;
}

void hkgWindowPS2Pad::readAll()
{
   g_Pad0.read();
   g_Pad1.read();
}

/********************************************
 * CPad
 */

hkgWindowPS2Pad::hkgWindowPS2Pad( unsigned int port )
   : uiPort(port), bPadModeSet(false)
{
	memset( &CurStatus, 0, sizeof(tPadStatus) );
	memset( &LastStatus, 0, sizeof(tPadStatus) );
}

bool hkgWindowPS2Pad::open( void )
{
   // slot is only for use with multitap
   return scePadPortOpen(uiPort, kSlot0, DmaBuffer);
}

bool hkgWindowPS2Pad::isValid()
{
	hkInt32 padState = scePadGetState( uiPort, kSlot0 );
	return (padState == scePadStateStable);
}

void hkgWindowPS2Pad::read( void )
{
	if ( !isValid() )
		return;
	
	if ( !bPadModeSet ) 
	{
		// who knows what the 1 parameter is..  a return val of 1 indicates that the request is
		// being processed
		if ( scePadSetMainMode(uiPort, kSlot0, 1, kPadSetLockModeUnlock) == 1 )
			bPadModeSet = true;
	}
	else 
	{
		tPadStatus padStatus;
		scePadRead( uiPort, kSlot0, (hkUint8*)&padStatus );

		if ( padStatus.success == 0 ) 
		{ // 0 indicates success
			LastStatus = CurStatus;
			padStatus.rightStick = CurStatus.rightStick;
			padStatus.leftStick = CurStatus.leftStick;
			CurStatus = padStatus;

			hkInt32 id = scePadInfoMode( uiPort, kSlot0, InfoModeCurID, 0 );
			if ( id == kPadModeStandard || id == kPadModeAnalog ) 
			{
				// flip the sense of the bit field (1 = pressed)
				CurStatus.buttons ^= 0xffff;
			}

			// sticks
			if ( wasPushed( HKG_PS2_PAD_RIGHT_STICK ) ) 
			{
				CurStatus.leftStick.isCentered = false;
				CurStatus.rightStick.isCentered = false;
			}

			CurStatus.leftStick.xVal = CurStatus.l3h;
			CurStatus.leftStick.yVal = CurStatus.l3v;
			CurStatus.rightStick.xVal = CurStatus.r3h;
			CurStatus.rightStick.yVal = CurStatus.r3v;
			UpdateStick( &CurStatus.leftStick, &LastStatus.leftStick );
			UpdateStick( &CurStatus.rightStick, &LastStatus.rightStick );
		}
	}
}

bool hkgWindowPS2Pad::UpdateStick( tStickData* stickCur, tStickData* stickLast )
{
	hkInt8 temp;
	bool isChanged = false;

	if ( ! stickCur->isCentered ) 
	{
		stickCur->xCenter = stickCur->xVal;
		stickCur->yCenter = stickCur->yVal;
		stickCur->xPos = 0.0f;
		stickCur->yPos = 0.0f;
		stickCur->isCentered = true;

		isChanged = true;
	}
	else 
	{
		if ( !hkFuzzyEqualsi(stickCur->xVal, stickCur->xCenter, kStickDeadRadius) ) 
		{
			// stick is not inside the dead zone
			temp = ((stickCur->xVal > stickCur->xCenter) ? -kStickDeadRadius : kStickDeadRadius);
			stickCur->xPos = (float)(stickCur->xVal - stickCur->xCenter + temp) /
								(float)kStickMaxRadius;
			isChanged = TRUE;
		}
		else 
		{
			// stick is inside the dead zone
			stickCur->xPos = 0.0f;
			// if it just entered the dead zone, send out one last event
			if ( !hkFuzzyEqualsi(stickLast->xVal, stickCur->xCenter, kStickDeadRadius) )
				isChanged = true;
		}


		if ( !hkFuzzyEqualsi(stickCur->yVal, stickCur->yCenter, kStickDeadRadius) ) 
		{
			// stick is not inside the dead zone
			temp = (stickCur->yVal > stickCur->yCenter) ? kStickDeadRadius : -kStickDeadRadius;
			stickCur->yPos = (float)(stickCur->yCenter - stickCur->yVal + temp) /
								(float)kStickMaxRadius;
			isChanged = true;
		}
		else 
		{
			// stick is inside the dead zone
			stickCur->yPos = 0.0f;
			// if it just entered the dead zone, send out one last event
			if ( !hkFuzzyEqualsi(stickLast->yVal, stickCur->yCenter, kStickDeadRadius) )
				isChanged = true;
		}

		stickCur->xPos = hkg_clampf( stickCur->xPos, -1.0f, 1.0f );
		stickCur->yPos = hkg_clampf( stickCur->yPos, -1.0f, 1.0f );
	}

	return isChanged;
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
