/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/thread/hkThread.h>
#include <hkbase/fwd/hkcstdio.h>
#include <hkbase/config/hkConfigThread.h>

#if (HK_CONFIG_THREAD==HK_CONFIG_MULTI_THREADED)

#include <sys/ppu_thread.h>

#define STACK_SIZE 0x40000		/* 256KB */
#define PRIO 100 // 1001 == default thread priority of the main PPU thread, so want the same as that

hkThread::hkThread()
	: m_thread(HK_NULL)
{
}

hkThread::~hkThread()
{
	if( m_thread )
	{
		uint64_t exit_code;
		/*int ret = */ sys_ppu_thread_join( (sys_ppu_thread_t)m_thread, &exit_code);
	}
}

hkUint64 HK_CALL hkThread::getThreadId()
{
	hkUint64 tid; // a unit64
	__asm__ volatile ( "ori %0,13,0" : "=r"( tid ) ); //r13 == tid in our setup  (sys_ppu_thread_get_id(&tid) 
	return tid;
}

// so that thread funcs are the same on PC and PS3
// we can redirect the thread func through the PS3 style:
struct ThreadFuncRedirect
{
	hkThread::StartFunction realFunc;
	void* arg;
};

static void staticRedirectFunc(uint64_t ptr )
{
	ThreadFuncRedirect* tfr = (ThreadFuncRedirect*)ptr;
	tfr->realFunc(tfr->arg);
	hkDeallocate<ThreadFuncRedirect>( tfr );

	sys_ppu_thread_exit(0);
}

hkResult hkThread::startThread( hkThread::StartFunction func, void* arg )
{
	sys_ppu_thread_t pu_thr;
	ThreadFuncRedirect* tfr= hkAllocate<ThreadFuncRedirect>( 1, HK_MEMORY_CLASS_BASE );
	tfr->arg = arg;
	tfr->realFunc = func;

	int ret = sys_ppu_thread_create(&pu_thr, staticRedirectFunc,
		(uint64_t)tfr, PRIO, STACK_SIZE,
		SYS_PPU_THREAD_CREATE_JOINABLE, "Havok PPU Thread");

	m_thread = (void*)(pu_thr); // ok as void* == uint64 == sys_ppu_thread_t

	return ret == CELL_OK? HK_SUCCESS : HK_FAILURE;
}

#else // Single threaded

hkThread::hkThread()
{
	HK_ASSERT(0xf9178fa1, "hkSemaphone being used in a single threaded PS3 environment. It will have no effect.");
}

hkThread::~hkThread()
{
}

hkResult hkThread::startThread( hkThread::StartFunction func, void* arg )
{
	return HK_SUCCESS;
}

hkUint64 hkThread::getThreadId()
{
	hkUint64 tid; // a unit64
	__asm__ volatile ( "ori %0,13,0" : "=r"( tid ) ); //r13 == tid in our setup  (sys_ppu_thread_get_id(&tid) 
	return tid;
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
