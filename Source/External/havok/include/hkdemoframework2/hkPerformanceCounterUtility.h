/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GAME__PERFORMANCE_COUNTER_UTILITY
#define HK_GAME__PERFORMANCE_COUNTER_UTILITY

#include <hkbase/debugutil/hkMonitorCaptureStreamUtility.h>

class hkPerformanceCounterUtility
{
	public:
		enum CounterSelect
		{
			DISABLE,
			ICACHE_MISSES_PENALTY,
			ICACHE_MISSES,
			DCACHE_MISSES,
			DCACHE_MISSES_PENALTY
		};

		hkPerformanceCounterUtility();
		~hkPerformanceCounterUtility();

		void enable( CounterSelect select = ICACHE_MISSES_PENALTY );

		void startCounter();
		void stopCounter();

		void setGameName( const char* name );

		void saveFileAndReset();

		void reset();

	protected:
		CounterSelect m_counterSelect;
		hkString m_gameName;
		hkMonitorCaptureStreamUtility*	m_streamUtility;
};

#endif // HK_GAME__PERFORMANCE_COUNTER_UTILITY

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
