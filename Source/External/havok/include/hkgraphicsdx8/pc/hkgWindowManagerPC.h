/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_WINDOW_MANAGER_PC_H
#define HK_GRAPHICS_WINDOW_MANAGER_PC_H

#include <hkgraphicsdx8/pc/hkgWindowDX8PC.h>
#include <hkbase/htl/hkMap.h>

//
// It is not Havok mem managed as we
// may need to have the manager present after 
// hkBaseSystem:: quit just to tell the wndproc 
// that the havok managed window system has shut down (hkgWindow list is empty)
// It should not be a big mem resource anyway and is only on PC.
// Add, find, and remove use a critical section for the map access
//

class hkgWindowManagerDX8PC: public hkSingleton<hkgWindowManagerDX8PC>
{
public:
	
	// There should be no need to create more than one of these,
	// and that one is created globally.

	hkgWindowManagerDX8PC();
	~hkgWindowManagerDX8PC();

	// will not reference actual hkgWindow as it 
	// will assume that windows are only added and 
	// removed under hkgWindow dtor and ctor
	bool addWindow( HWND handle, hkgWindowDX8PC* w );
	bool setHWND( HWND handle, hkgWindowDX8PC* w );

	hkgWindowDX8PC* findWindow( HWND handle ) const;
	
	bool removeWindow( HWND handle );
	
protected:

	hkMap<void*, hkgWindowDX8PC*> m_hwndMap; 
	HANDLE m_hMutex;
};

#endif // HK_GRAPHICS_WINDOW_MANAGER_PC_H
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
