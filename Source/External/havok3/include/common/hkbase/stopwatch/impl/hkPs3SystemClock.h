/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#if defined(HK_PLATFORM_PS3)
//#	include <sys/sys_time.h>

	class hkPs3SystemClock : public hkSystemClock
	{
		public:

			hkUint64 getTickCounter()
			{
				hkUint64 time;
				asm volatile ("mftb %0" : "=r"(time));
				return time;
			}

			hkUint64 getTicksPerSecond()
			{
				// The CEB-201x and 203x kit has a time base of 16MHz, older have 24Mhz, 
				return 16000000; //sys_time_get_timebase_frequency(); 
			}
	};

#else
#	include <spu_intrinsics.h>

	hkUint64 hkSystemClock::getTickCounter()
	{
		return (hkUint64) ( -spu_readch(SPU_RdDec) );
	}

	hkUint64 hkSystemClock::getTicksPerSecond()
	{
		return 16000000;
	}

#endif

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
