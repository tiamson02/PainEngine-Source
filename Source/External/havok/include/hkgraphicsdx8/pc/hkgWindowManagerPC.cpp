/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/pc/hkgWindowManagerPC.h>

hkgWindowManagerDX8PC::hkgWindowManagerDX8PC()
{
	// Create a mutex with no initial owner.
	m_hMutex = CreateMutex( 
		NULL,                       // no security attributes
		FALSE,                      // initially not owned
		"HKGraphicsMutexForGlobalWindowMap");  // name of mutex

}

hkgWindowManagerDX8PC::~hkgWindowManagerDX8PC()
{
	CloseHandle( m_hMutex );
}

bool hkgWindowManagerDX8PC::addWindow( HWND handle, hkgWindowDX8PC* w )
{
	DWORD res = WaitForSingleObject( m_hMutex, 5000L); // five-second time-out interval
	if ( res == WAIT_OBJECT_0 )
	{
		void* k = (void*)handle;
		m_hwndMap[k] = w;
		
		ReleaseMutex(m_hMutex);
		return true;
	}
	return false; // either timed out or the mutex was abandoned
}

hkgWindowDX8PC* hkgWindowManagerDX8PC::findWindow( HWND handle ) const
{
	DWORD res = WaitForSingleObject( m_hMutex, 5000L); // five-second time-out interval
	hkgWindowDX8PC* w = HK_NULL;

	if ( res == WAIT_OBJECT_0 )
	{
		typedef hkgWindowDX8PC* hkgWindowDX8PCPointer;
		const hkgWindowDX8PCPointer* pw = m_hwndMap.find( (void*)handle );
		if (pw)
		{
			w = *pw;
		}
		ReleaseMutex(m_hMutex);
	}

	return w; // NULL if not found, timed out or the mutex was abandoned
}

bool hkgWindowManagerDX8PC::removeWindow( HWND handle )
{
	DWORD res = WaitForSingleObject( m_hMutex, 5000L); // five-second time-out interval

	if ( res == WAIT_OBJECT_0 )
	{
		m_hwndMap.erase( (void*)handle );

		ReleaseMutex(m_hMutex);

		return true;
	}
	return false; // either timed out or the mutex was abandoned
}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_IMPLEMENTATION(hkgWindowManagerDX8PC);
	

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
