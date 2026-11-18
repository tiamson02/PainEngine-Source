/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/monitor/hkMonitorStream.h>

HK_THREAD_LOCAL_IMPL( char* ) hkMonitorStream::m_streams[5];

bool hkMonitorStream::m_debugTimersEnabled = true;

#if defined(HK_XBOX_USE_PERFLIB) && defined(HK_PLATFORM_XBOX360)
hkUlong g_hkXbox360PerfSampleRegAddr = 0x8FFF1230; // 0x30 = 6*8 == reg 6 == LHS cycles in a PB0T0 setup
#endif 

void hkMonitorStream::init() 
{
	setDynamic(false);
	setStart(HK_NULL);
	setCapacity(HK_NULL);
	setEnd(HK_NULL);
	setCapacityMinus16(HK_NULL);
}

void hkMonitorStream::quit()
{
#if !defined(HK_PLATFORM_PS3SPU)
	if ( getStart() && isDynamic() )
	{
		hkDeallocate(getStart());
	}
#endif

	// set every TLS entry to 0 which on some of 
	// our platforms frees the storage for it
	setDynamic(false);
	setStart(HK_NULL);
	setCapacity(HK_NULL);
	setEnd(HK_NULL);
	setCapacityMinus16(HK_NULL);
}


void HK_CALL hkMonitorStream::resize( int newSize )
{
#if !defined(HK_PLATFORM_PS3SPU)

	if ( newSize == getCapacity() - getStart() )
	{
		return;
	}

	if (newSize > 0)
	{
		if ( getStart() && isDynamic() )
		{
			hkDeallocate(getStart());
		}
		
		setDynamic( true );
		setStart( hkAllocate<char>(newSize, HK_MEMORY_CLASS_MONITOR) );
		setEnd( getStart() );
		setCapacity( getStart() + newSize );
		setCapacityMinus16( getCapacity() - 16 );
	}
	else
	{
		quit();
	}
#else
	HK_ASSERT(0x0, "No dynamic monitor streams allowed on SPUs. You must preallocate the stream and set the pointers explicitly.");
#endif
}

void HK_CALL hkMonitorStream::setStaticBuffer( char* buffer, int bufferSize )
{
#if !defined(HK_PLATFORM_PS3SPU)
	if ( isDynamic() )
	{
		resize(0);
	}
#endif

	setDynamic(false);
	setStart( buffer );
	setEnd( getStart() );
	setCapacity( getStart() + bufferSize );
	setCapacityMinus16( getCapacity() - 16 );
}

void HK_CALL hkMonitorStream::reset()
{
	setEnd( getStart() );
}


#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
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
