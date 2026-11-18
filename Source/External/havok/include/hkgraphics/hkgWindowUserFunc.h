/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW_USER_FUNC_H
#define HK_GRAPHICS_WINDOW_USER_FUNC_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgKeyboard.h>
#include <hkgraphics/hkgMouse.h>
#include <hkgraphics/hkgPad.h>

class hkgWindow;

//
// All user functions return whether the user handled it / understood it / etc
// So return false if you want the window to do its default behavior for the 
// key / event.
//

// Keyboard
typedef bool (*HKG_USER_FUNC_KEY)(hkgWindow* w, HKG_KEYBOARD_VKEY key, bool state, void* userContext);

// Mouse
typedef bool (*HKG_USER_FUNC_MOUSE_MOVE)(hkgWindow* w, int x, int y, int dx, int dy, void* userContext);
typedef bool (*HKG_USER_FUNC_MOUSE_WHEEL)(hkgWindow* w, int delta, void* userContext);
typedef bool (*HKG_USER_FUNC_MOUSE_BUTTON)(hkgWindow* w, HKG_MOUSE_BUTTON button, bool state, void* userContext);

// Gamepad
typedef bool (*HKG_USER_FUNC_PAD_MOVE)(hkgWindow* w, unsigned short pad, unsigned short stick, float x, float y, float dx, float dy, void* userContext);
typedef bool (*HKG_USER_FUNC_PAD_BUTTON)(hkgWindow* w, unsigned short pad, HKG_PAD_BUTTON button, bool state, void* userContext);

// Window events
typedef void (*HKG_USER_FUNC_RESIZE)(hkgWindow* w,unsigned int width, unsigned int height, void* userContext);
typedef bool (*HKG_USER_FUNC_COMMAND)(hkgWindow* w, unsigned int code, unsigned int id, void* nativeHandle, void* userContext);



#endif // HK_GRAPHICS_WINDOW_USER_FUNC_H
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
