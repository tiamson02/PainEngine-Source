/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HK_CRITICAL_SECTION_H
#define HKBASE_HK_CRITICAL_SECTION_H

#include <hkbase/config/hkConfigThread.h>

#ifdef HK_PLATFORM_WIN32
#	ifndef _WIN32_WINNT
#		define WIN32_LEAN_AND_MEAN
#		define _WIN32_WINNT 0x0403
#		include <windows.h>
#	endif
#elif defined(HK_PLATFORM_XBOX360)
#	include <xtl.h>
#elif defined(HK_PLATFORM_PS3) || defined(HK_PLATFORM_PS3SPU) 
#	include <cell/sync.h>
#	if defined(HK_PLATFORM_PS3) 
#		include <sys/synchronization.h>	
#	endif
#endif

#include <hkbase/thread/hkThreadLocalData.h>

	/// Set this define if you want a timer begin and timer end call 
	/// if a thread has to wait for a critical section
	/// You also have to call hkCriticalSection::setTimersEnabled()
#define HK_TIME_CRITICAL_SECTION_LOCKS
//#define HK_PS3_CRITICAL_SECTION_SYSTEM_WAIT

	/// Critical section wrapper. This can be used to guard access
	/// to shared data structures between threads.
	/// Note that critical section are fast but have serious drawbacks.
	/// Check windows help for details. 
	/// Note that including this file means including a system file, such as windows.h, 
	/// which makes compile time significantly slower.
class hkCriticalSection
{		
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE_CLASS, hkCriticalSection);

			/// Init a critical section with spin count. 
			/// Read MSDN help of InitializeCriticalSectionAndSpinCount for details.
			/// In short: positive spinCount value results in threads doing busy waiting and is good
			///           when you know that the critical section will only be locked for a short period of time.
			///           zero value causes the thread to immediately go back to the system when waiting.
		inline hkCriticalSection( int spinCount );

			/// Quit a critical section
		inline ~hkCriticalSection();

			/// Lock a critical section
		inline void enter();

			/// Returns whether this process has entered this critical section.
		inline bool haveEntered();

			/// Try to lock a critical section, return !=0 when successful
		inline int tryEnter();

			/// Unlock a critical section
		inline void leave();


			/// Tell the critical section to time blocking locks. HK_TIME_CRITICAL_SECTION_LOCKS must be
			/// defined at compile time for this flag to have any effect
		static inline void HK_CALL setTimersEnabled();

			/// Stop timing blocking locks
		static inline void HK_CALL setTimersDisabled();



	public:

#if HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED 

#	if defined(HK_PLATFORM_PS3) || defined(HK_PLATFORM_PS3SPU) 
		
		// Busy wait mutex used on both PPU and SPU,  a uint32, 
		// not used as this data on SPU, but used thorugh the ptr to the actual one in main mem.
		CellSyncMutex m_mutex; // 32bits
		hkInt32 m_recursiveLockCount;
		HK_CPU_PTR(CellSyncMutex*) m_mutexPtr;  // always 64bit
		hkUint64 m_currentThread; // if this == current, then we have the lock already and just inc recursion count
		
#		if defined(HK_PS3_CRITICAL_SECTION_SYSTEM_WAIT)
#			if defined(HK_PLATFORM_PS3)// PPU  
		// have a kinder system level lock first, so that PPU threads
		// can sleep on locks with each other. ie, if this is locked, then a 
		// ppu thread has the lock, and we can sleep on it
		sys_mutex_t m_ppuMutex;
#			else  // SPU 
		hkUint32   m_ppuMutexPadding;
#			endif
#		endif


#	else // other threaded platforms:

		CRITICAL_SECTION  m_section;
		hkUint64 m_currentThread;

#		ifdef HK_TIME_CRITICAL_SECTION_LOCKS
			static HK_THREAD_LOCAL( int ) m_timeLocks;
#		endif

#	endif

#endif
};

// include the inl before the hkCriticalSectionLock def so 
// that gcc can inline the enter and leave properly
#include <hkbase/thread/hkCriticalSection.inl>

	/// Helper class which locks a critical section as long is this object exists. 
class hkCriticalSectionLock
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE_CLASS, hkCriticalSectionLock);

			/// Create a lock by entering a critical section.
		inline hkCriticalSectionLock( hkCriticalSection* section )
		{
			m_section = section;
			section->enter();
		}

			/// Destructor leaves the critical section
		inline ~hkCriticalSectionLock()
		{
			m_section->leave();
		}

	protected:
		hkCriticalSection* m_section;
};



	/// A base class to ensure locks are not sharing cache lines with other data.
class hkSynchronized
{
	public:
		hkSynchronized( int spinCount ):m_criticalSection(spinCount){}

		void lockMt(){ m_criticalSection.enter(); }
		void unlockMt() { m_criticalSection.leave(); }
	public:
		HK_ALIGN( hkCriticalSection m_criticalSection, 64 );
};

	/// Put this macro as the first line of each member function of 
	//// derived classes of hkSynchronized
#define HK_SYNCRONIZE hkCriticicalSectionLock lock(&m_criticalSection);


#endif // HKBASE_HK_CRITICAL_SECTION_H

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
