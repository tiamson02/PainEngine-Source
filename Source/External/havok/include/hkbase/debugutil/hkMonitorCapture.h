/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKMONITOR_CAPTURE_H
#define HKBASE_HKMONITOR_CAPTURE_H

struct hkMonitorCaptureCommand
{
	const char* m_commandAndMonitor;
};

struct hkMonitorAddValue : public hkMonitorCaptureCommand
{
	float m_value;
};

struct hkMonitorIdStringMap : public hkMonitorCaptureCommand
{
	const char* m_id;
	int m_sizeOfString;
};

struct hkMonitorStreamParams : public hkMonitorCaptureCommand
{
	const char* m_heading;
	const char* m_nodeIdForFrameOverview;
	hkReal m_timerFactor0;
	hkReal m_timerFactor1;
};

struct hkMonitorTimer : public hkMonitorCaptureCommand
{
	hkUint32	m_time0;
	hkUint32	m_time1;

	inline void setTime()
	{
#if defined(HK_ARCH_IA32)
#	if defined(HK_COMPILER_GCC)
		hkUint32 junk;
		__asm__ __volatile__ (	"rdtsc\n\t"
								: "=a" (m_time0), "=d" (junk));
#	else
		hkUint32 ticks;
		__asm {
			//push ebx
			//cpuid 
			//pop ebx
			rdtsc
			mov dword ptr[ticks], eax
			//mov dword ptr[ticks+4], edx
		}
		m_time0 = ticks;
#	endif 
#elif defined(HK_ARCH_PS2)
		__asm__ __volatile__ ("mfpc %0, 0": "=r" (m_time0));
		__asm__ __volatile__ ("mfpc %0, 1": "=r" (m_time1));
#elif defined(HK_PLATFORM_GC)
#	if defined(HK_COMPILER_MWERKS)
		register hkUint32 ticks;
		asm {
			mftb ticks
		}
		m_time0 = ticks;
#	elif defined(HK_COMPILER_GCC)
		 __asm__ __volatile__ ("mftb %0": "=r"(m_time0));
#	endif
#else
#	error dont know how to get time
#endif
	}
};


#define HK_MONITOR_TYPE_TIMER "t"
#define HK_MONITOR_TYPE_INT "i"
#define HK_MONITOR_TYPE_DIR "d"
#define HK_MONITOR_TYPE_SINGLE "f"

#if HK_CONFIG_MONITORS == HK_CONFIG_MONITORS_ENABLED


#	define HK_MONITOR_ADD_VALUE( NAME, VALUE, MONITOR_TYPE )   \
		if ( hkMonitorStream::memoryAvailable() )							\
		{																	\
			 hkMonitorAddValue* h = reinterpret_cast<hkMonitorAddValue*>(hkMonitorStream::current());	\
			 h->m_commandAndMonitor = "M" MONITOR_TYPE NAME;				\
			 h->m_value = VALUE;											\
			 hkMonitorStream::current() = (char*)(h+1);						\
		}

#	define HK_TIMER_INTERN( NAME, COMMAND, OBJECT )							\
		if ( hkMonitorStream::memoryAvailable() )							\
		{																	\
			 hkMonitorTimer* h = reinterpret_cast<hkMonitorTimer*>(hkMonitorStream::current());	\
			 h->m_commandAndMonitor = COMMAND HK_MONITOR_TYPE_TIMER NAME;							\
			 h->setTime();													\
			 hkMonitorStream::current() = (char*)(h+1);								\
		}

#	define HK_TIMER_BEGIN( NAME, OBJECT )	HK_TIMER_INTERN( NAME, "T", OBJECT	)
#	define HK_TIMER_END(  )	HK_TIMER_INTERN( "", "E", HK_NULL	)

#	define HK_TIMER_BEGIN_LIST( NAME, FIRSTITEM )	HK_TIMER_INTERN( NAME "\0" FIRSTITEM, "L", HK_NULL	)
#	define HK_TIMER_SPLIT_LIST( NEXTITEM )			HK_TIMER_INTERN( NEXTITEM, "S", HK_NULL	)
#	define HK_TIMER_END_LIST( )						HK_TIMER_INTERN( "", "l", HK_NULL	)


#	define HK_MONITOR_PUSH_DIR( PATH )								\
	if ( hkMonitorStream::memoryAvailable() )							\
	{																	\
		 hkMonitorCaptureCommand* h = reinterpret_cast<hkMonitorCaptureCommand*>(hkMonitorStream::expandby(sizeof(hkMonitorCaptureCommand)));	\
		 h->m_commandAndMonitor = "Pd" PATH;							\
	}

#	define HK_MONITOR_POP_DIR(  )								\
	if ( hkMonitorStream::memoryAvailable() )							\
	{																	\
		 hkMonitorCaptureCommand* h = reinterpret_cast<hkMonitorCaptureCommand*>(hkMonitorStream::expandby(sizeof(hkMonitorCaptureCommand)));	\
		 h->m_commandAndMonitor = "pd";							\
	}


#else // HK_CONFIG_MONITORS != HK_CONFIG_MONITORS_ENABLED

#	define HK_MONITOR_PUSH_DIR( PATH ) 
#	define HK_MONITOR_POP_DIR()
#	define HK_MONITOR_ADD_VALUE( NAME, VALUE, MONITOR_TYPE ) 

#	define HK_TIMER_BEGIN( NAME, OBJECT )
#	define HK_TIMER_END()

#	define HK_TIMER_BEGIN_LIST( NAME, OBJECT )
#	define HK_TIMER_SPLIT_LIST( NAME )
#	define HK_TIMER_END_LIST(  )

#endif // HK_CONFIG_MONITORS == HK_CONFIG_MONITORS_ENABLED

#if 0

#define HK_INTERNAL_TIMER_BEGIN(NAME)		HK_TIMER_BEGIN(NAME,HK_NULL)
#define HK_INTERNAL_TIMER_END				HK_TIMER_END
#define HK_INTERNAL_TIMER_BEGIN_LIST(a,b)	HK_TIMER_BEGIN_LIST(a,b)
#define HK_INTERNAL_TIMER_SPLIT_LIST(a)		HK_TIMER_SPLIT_LIST(a)
#define HK_INTERNAL_TIMER_END_LIST			HK_TIMER_END_LIST


#define HK_USER_TIMER_BEGIN(NAME)		{}
#define HK_USER_TIMER_END()				{}
#define HK_USER_TIMER_BEGIN_LIST(a,b)	{}
#define HK_USER_TIMER_SPLIT_LIST(a)		{}
#define HK_USER_TIMER_END_LIST()		{}

#else

#define HK_INTERNAL_TIMER_BEGIN(NAME)		{}
#define HK_INTERNAL_TIMER_END()				{}
#define HK_INTERNAL_TIMER_BEGIN_LIST(a,b)	{}
#define HK_INTERNAL_TIMER_SPLIT_LIST(a)		{}
#define HK_INTERNAL_TIMER_END_LIST()		{}

#define HK_USER_TIMER_BEGIN(NAME)		HK_TIMER_BEGIN(NAME,HK_NULL)
#define HK_USER_TIMER_END				HK_TIMER_END
#define HK_USER_TIMER_BEGIN_LIST(a,b)	HK_TIMER_BEGIN_LIST(a,b)
#define HK_USER_TIMER_SPLIT_LIST(a)		HK_TIMER_SPLIT_LIST(a)
#define HK_USER_TIMER_END_LIST			HK_TIMER_END_LIST

#endif

#endif // HKBASE_HKMONITOR_CAPTURE_H

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
