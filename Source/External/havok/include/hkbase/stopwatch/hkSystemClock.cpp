/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkSystemClock.h>

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX)
#	include <hkbase/stopwatch/impl/hkWindowsSystemClock.h>
	typedef hkWindowsSystemClock hkDefaultSystemClock;
#elif defined(HK_ARCH_IA32) && defined(HK_PLATFORM_UNIX)
#	include <hkbase/stopwatch/impl/hkLinuxSystemClock.h>
	typedef hkLinuxSystemClock hkDefaultSystemClock;
#elif defined(HK_PLATFORM_PS2)
#	include <hkbase/stopwatch/impl/hkPs2SystemClock.h>
	typedef hkPs2SystemClock hkDefaultSystemClock;
#elif defined(HK_PLATFORM_MAC)
#	include <hkbase/stopwatch/impl/hkMacSystemClock.h>
	typedef hkMacSystemClock hkDefaultSystemClock;
#elif defined(HK_PLATFORM_GC)
#	include <hkbase/stopwatch/impl/hkNgcSystemClock.h>
	typedef hkNgcSystemClock hkDefaultSystemClock;
#else
#  error ERROR: No SystemClock implementation available!
#endif

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

HK_SINGLETON_CUSTOM_IMPLEMENTATION(hkSystemClock, hkDefaultSystemClock);

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
