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

inline bool hkgWindowPS2Pad::isDown( HKG_PS2_PAD_BUTTON button ) const
{
   return CurStatus.buttons & (1 << button);
}

inline bool hkgWindowPS2Pad::isUp( HKG_PS2_PAD_BUTTON button ) const
{
   return ! (CurStatus.buttons & (1 << button));
}

inline bool hkgWindowPS2Pad::wasPushed( HKG_PS2_PAD_BUTTON button ) const
{
   return (CurStatus.buttons & (1 << button)) && !(LastStatus.buttons & (1 << button));
}

inline bool hkgWindowPS2Pad::wasReleased( HKG_PS2_PAD_BUTTON button ) const
{
   return (LastStatus.buttons & (1 << button)) && !(CurStatus.buttons & (1 << button));
}

inline float hkgWindowPS2Pad::rightStickX( ) const
{
	return CurStatus.rightStick.xPos; 
}

inline float hkgWindowPS2Pad::rightStickY( ) const
{ 
	return CurStatus.rightStick.yPos; 
}

inline float hkgWindowPS2Pad::leftStickX( ) const 
{ 
	return CurStatus.leftStick.xPos; 
}

inline float hkgWindowPS2Pad::leftStickY( ) const
{ 
	return CurStatus.leftStick.yPos; 
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
