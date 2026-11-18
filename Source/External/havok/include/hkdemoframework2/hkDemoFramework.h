/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEMOFRAMEWORK2_DEMOFRAMEWORK_H
#define HK_DEMOFRAMEWORK2_DEMOFRAMEWORK_H

#include <hkmath2/hkMath.h>
#include <hkdemoframework2/hkGame.h>
#include <hkdemoframework2/hkGameEnvironment.h>

#ifdef HK_PS2
#	define HK_MAX_IRX_MODULES 12

struct PS2SetupOptions
{
	PS2SetupOptions();
	
	const char* m_PS2modulePath;
	const char* m_rebootIOP;
	const char* m_PS2modules[HK_MAX_IRX_MODULES];
	const char* m_PS2moduleOptions[HK_MAX_IRX_MODULES];
	int m_PS2moduleOptionsLength[HK_MAX_IRX_MODULES];
		// Can't use strlen as some of the strings have \0 in them!
};

#endif // HK_PS2

struct hkDemoFrameworkOptions
{
	hkDemoFrameworkOptions();
	~hkDemoFrameworkOptions();
	void parse(int argc, const char** argv);

	bool m_windowed;
	
	bool m_debugger;
	
	bool m_industrial;

	int m_lockFps;

	const char* m_defaultGame;
	const char* m_windowTitle;

	const char* m_masterFile;
	const char* m_renderer;

	const char* m_statsDir;

	bool m_enablePerformanceCounter;

	int m_argc;
	const char** m_argv;

#ifdef HK_PS2
	bool m_rebootIOP;
	PS2SetupOptions m_ps2options;
#endif
};

#endif // HK_DEMOFRAMEWORK2_DEMOFRAMEWORK_H

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
