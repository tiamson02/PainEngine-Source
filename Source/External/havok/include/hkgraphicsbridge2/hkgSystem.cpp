/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>

#include <hkgraphicsbridge2/hkgSystem.h>

extern void hkgSystemGCNInit();
extern void hkgSystemDx8XboxInit();
extern void hkgSystemDx8PcInit();
extern void hkgSystemOglPcInit();
extern void hkgSystemOglPs2Init();
extern void hkgSystemOglSdlInit();
extern void hkgSystemNullInit();

extern void hkgSystemGCNQuit();
extern void hkgSystemDx8XboxQuit();
extern void hkgSystemDx8PcQuit();
extern void hkgSystemOglPcQuit();
extern void hkgSystemOglPs2Quit();
extern void hkgSystemOglSdlQuit();
extern void hkgSystemNullQuit();

#define ALLOW_DX8_ON_PC
static char s_renderer;

void HK_CALL hkgSystem::init(const char* renderer)
{
	hkBaseSystem::init();
	
	if (renderer && renderer[0] == 'n')
	{
		hkgSystemNullInit();
	}
	else
	{
#if defined(HK_PLATFORM_XBOX)
		hkgSystemDx8XboxInit();
#elif defined(HK_PLATFORM_WIN32)
		
		// ogl as default and dx8 optional
#	if defined(ALLOW_DX8_ON_PC)
		if (renderer && renderer[0] == 'd')
		{
			hkgSystemDx8PcInit();
		}
		else
#	endif
		{
			hkgSystemOglPcInit();
		}
		
#elif defined(HK_PLATFORM_UNIX)
		hkgSystemOglSdlInit();
#elif defined(HK_PLATFORM_PS2)		
		hkgSystemOglPs2Init();
#elif defined(HK_PLATFORM_GC)
		hkgSystemGCNInit();
#else
#	error no renderer defined for this platform
#endif
	}

	s_renderer = 0;
	if(renderer)
	{
		s_renderer = renderer[0];
	}
}

void HK_CALL hkgSystem::quit()
{
	if (s_renderer == 'n')
	{
		hkgSystemNullQuit();
	}
	else
	{
#if defined(HK_PLATFORM_XBOX)
		hkgSystemDx8XboxQuit();
#elif defined(HK_PLATFORM_WIN32)
		
		// ogl as default and dx8 optional
#	if defined(ALLOW_DX8_ON_PC)
		if (s_renderer == 'd')
		{
			hkgSystemDx8PcQuit();
		}
		else
#	endif
		{
			hkgSystemOglPcQuit();
		}
		
#elif defined(HK_PLATFORM_UNIX)
		hkgSystemOglSdlInit();
#elif defined(HK_PLATFORM_PS2)		
		hkgSystemOglPs2Quit();
#elif defined(HK_PLATFORM_GC)
		hkgSystemGCNQuit();
#else
#	error no renderer defined for this platform
#endif
	}
	
	hkBaseSystem::quit();
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
