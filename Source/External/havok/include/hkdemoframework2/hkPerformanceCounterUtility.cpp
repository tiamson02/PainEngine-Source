/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkdemoframework2/hkDemoFramework.h>
#include <hkdemoframework2/hkGameEnvironment.h>
#include <hkdemoframework2/hkPerformanceCounterUtility.h>
#include <hkbase/stream/hkStreambufFactory.h>

#ifdef HK_PS2
#include <libpc.h> //for the SCE_PC_CTE timings
#endif


hkPerformanceCounterUtility::hkPerformanceCounterUtility()
{
	m_counterSelect = DISABLE;
	m_gameName = "unknown";
}

hkPerformanceCounterUtility::~hkPerformanceCounterUtility()
{
	if ( m_counterSelect == DISABLE )
	{
		return;
	}
	delete m_streamUtility;
}

void hkPerformanceCounterUtility::enable( CounterSelect select )
{
#ifdef HK_PS2
	m_streamUtility = new hkMonitorCaptureStreamUtility( 2000000 );
#else
	m_streamUtility = new hkMonitorCaptureStreamUtility( 5000000 );
#endif
	
	m_streamUtility->m_nodeIdForFrameOverview = "Simulate";

	m_counterSelect = select;	
	setGameName( m_gameName.cString() );
}

void hkPerformanceCounterUtility::startCounter()
{

	if ( m_counterSelect == DISABLE )
	{
		return;
	}
	hkMonitorStream::getInstance().resize( 2000000 );	// 2 meg for timer info
	hkMonitorStream::getInstance().reset();

#ifdef HK_PS2
	switch ( m_counterSelect )
	{
	case DISABLE:
		return;
	case ICACHE_MISSES_PENALTY:
	case ICACHE_MISSES:
			scePcStart( SCE_PC_CTE | SCE_PC_U0 | SCE_PC_U1 | SCE_PC0_ICACHE_MISS | SCE_PC1_CPU_CYCLE, 0 ,0 );
			break;
	case DCACHE_MISSES:
	case DCACHE_MISSES_PENALTY:
			scePcStart( SCE_PC_CTE | SCE_PC_U0 | SCE_PC_U1 | SCE_PC1_DCACHE_MISS | SCE_PC0_CPU_CYCLE, 0 ,0 );
			break;
	}
#endif
}

void hkPerformanceCounterUtility::stopCounter()
{
	if ( m_counterSelect == DISABLE )
	{
		return;
	}

	m_streamUtility->captureFrame();
#ifdef HK_PS2
	scePcStop() ;
#endif
}


void hkPerformanceCounterUtility::setGameName( const char* name )
{
	if ( m_counterSelect == DISABLE )
	{
		return;
	}

	m_gameName = name;
	
	hkString type = "pc";
	hkString heading = m_gameName + "   Timers: ";

#ifdef HK_PS2
	m_streamUtility->m_timerFactor0 = 1 / 1e3f;
	m_streamUtility->m_timerFactor1 = 1 / 1e3f;

	switch ( m_counterSelect )
	{
	case DISABLE:
		return;
	case ICACHE_MISSES_PENALTY:
			m_streamUtility->m_timerFactor0 *= 50;
			heading += "IcachePenalty ( kcycles )";
			type = "ps2_icache_pen";
			break;
	case ICACHE_MISSES:
			heading += "IcacheMisses ( kcycles )";
			type = "ps2_icache_miss";
			break;
	case DCACHE_MISSES:
			heading += "Clocks ( DcacheMisses ) [kcycles]";
			type = "ps2_dcache_pen";
			break;
	case DCACHE_MISSES_PENALTY:
			heading += "Clocks ( DcachePenalty ) [kcycles]";
			m_streamUtility->m_timerFactor1 *= 40;
			type = "ps2_dcache_miss";
			break;
	}
#else
	heading += "useconds";
#endif

	m_streamUtility->m_heading = heading;
}

void hkPerformanceCounterUtility::reset()
{
	if ( m_counterSelect == DISABLE )
	{
		return;
	}

	m_streamUtility->reset();
}

void hkPerformanceCounterUtility::saveFileAndReset()
{
	if ( m_counterSelect == DISABLE )
	{
		return;
	}
	
	hkString type = "pc";

#ifdef HK_PS2
	switch ( m_counterSelect )
	{
	case DISABLE:
		return;
	case ICACHE_MISSES_PENALTY:
			type = "ps2_icache_pen";
			break;
	case ICACHE_MISSES:
			type = "ps2_icache_miss";
			break;
	case DCACHE_MISSES:
			type = "ps2_dcache_pen";
			break;
	case DCACHE_MISSES_PENALTY:
			type = "ps2_dcache_miss";
			break;
	}
#endif
	// stop counter (needer for PS2 )
	stopCounter();

	hkString name = hkString("capture") + type + ".txt";
	hkStreambuf* sb = hkStreambufFactory::getInstance().open(name.cString(), hkStreambuf::OUTPUT);
	hkOstream ostr (sb);
	
	m_streamUtility->writeStatistics( ostr );
	m_streamUtility->reset();
	delete sb;
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
