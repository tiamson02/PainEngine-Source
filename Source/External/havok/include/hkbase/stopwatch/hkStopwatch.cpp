/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/stopwatch/hkSystemClock.h>

hkReal HK_CALL hkStopwatch::divide64(hkUint64 a, hkUint64 b)
{
	return hkReal( double(hkInt64(a)) / double(hkInt64(b)) );
}

hkUint64 HK_CALL hkStopwatch::getTickCounter()
{
	return hkSystemClock::getInstance().getTickCounter();
}

hkUint64 HK_CALL hkStopwatch::getTicksPerSecond()
{
	return hkSystemClock::getInstance().getTicksPerSecond();
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
