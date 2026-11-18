/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HK_THREAD_LOCAL_POINTER_H
#define HKBASE_HK_THREAD_LOCAL_POINTER_H

#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED)
  
// For Win32 style, we can use Tls* calls which allow the 
// libs to be used inside dynamically loaded DLLs 
#	if defined(HK_COMPILER_MSVC)

		// change thos define to alternate between __declspec and explict TLS
#		define HK_CONFIG_THREAD_USE_TLS

#		if defined(HK_CONFIG_THREAD_USE_TLS)
#			define HK_TLS_OUT_OF_INDEXES 0xffffffff
#			define HK_THREAD_LOCAL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_IMPL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_GET(VAR) VAR.getData()
#			define HK_THREAD_LOCAL_SET(VAR, VAL) VAR.setData(VAL)
#		if defined(HK_PLATFORM_WIN32) //WINBASE import define
#			define HK_TLS_IMPORT __declspec(dllimport)
#		else
#			define HK_TLS_IMPORT 
#		endif
			extern "C" {
				HK_TLS_IMPORT unsigned long _stdcall TlsAlloc(void);
				HK_TLS_IMPORT void* _stdcall TlsGetValue( const unsigned long dwTlsIndex );		
				HK_TLS_IMPORT int _stdcall TlsSetValue( const unsigned long dwTlsIndex, void* lpTlsValue);
				HK_TLS_IMPORT int _stdcall TlsFree( const unsigned long dwTlsIndex );
			}

			/// A platform independent wrapper for thread local storage
			/// We assume we always just store a pointer (or data the same size or smaller than a pointer) 
			/// ie,  sizeof(T) <= sizeof(char*)
			template < typename T > 
			class hkThreadLocalData
			{
				public:

					hkThreadLocalData() { m_slotID = (hkUlong)TlsAlloc(); }
					~hkThreadLocalData() { TlsFree((unsigned long)m_slotID); }

					HK_FORCE_INLINE T getData() 
					{
						return (T)(hkUlong) TlsGetValue((unsigned long)m_slotID) ;
					}

					HK_FORCE_INLINE void setData(T p) 
					{
						hkUlong v = (hkUlong)p;
						TlsSetValue((unsigned long)m_slotID, (void*)v );
					}

				protected:

					hkUlong m_slotID; 
			};

#		else
#			define HK_THREAD_LOCAL(TYPE) __declspec(thread) TYPE
#			define HK_THREAD_LOCAL_IMPL(TYPE) TYPE
#		endif
#	elif defined(HK_PLATFORM_PS3) 

#			include <cell/sync.h>			
#			include <sys/ppu_thread.h>	
			
			// On PPU, so really 2 is normal setting, and only searched to end 
			// if not allocated yet, but have a calling thread perhaps, on some 
			// demos allow up to 6 (for Xbox360 etc), and we may have interupt/signal 
			// handling threads anyway. Keep in sync with SPU dummy def below.
#			define HK_TLS_MAX_THREADS 7 
#			define HK_TLS_NO_INDEX  0xffffffffffffffff

			// There is no TLS support in the 0.4.0 tools, so we have to hard code it in a mapping.

			// This code asssumes that the thread ids do NOT CHANGE (can add threads, but threads should
			// not be deleted and new ones created, thus leaving phantom empty entries)
			// Luckly that is how our thread pool works anyway, so ok for internal use.
			template < typename T > 
			class hkThreadLocalData
			{
			public:

				hkThreadLocalData() 
				{ 
					// busy wait low level mutex (only used to get new entry anyway)
					cellSyncMutexInitialize(&m_tidToIndexMutex);
					for(int i=0; i < HK_TLS_MAX_THREADS; ++i)
					{
						m_tidToIndex[i] = HK_TLS_NO_INDEX;
					}
				}

				~hkThreadLocalData() 
				{ 
					// nothing required.
				}

				int getNewEntry(const sys_ppu_thread_t& tid)
				{
					// if we are here we do not have an entry yet
					cellSyncMutexLock(&m_tidToIndexMutex);
					for(int i = 0 ; i < HK_TLS_MAX_THREADS; ++i)
					{
						if (m_tidToIndex[i] == HK_TLS_NO_INDEX)
						{
							m_tidToIndex[i] = tid; // take this entry
							cellSyncMutexUnlock(&m_tidToIndexMutex);
							m_storage[i] = (T)HK_NULL; // null or 0 always to start with
							return i;
						}
					}
					cellSyncMutexUnlock(&m_tidToIndexMutex);
					
					// Too many threads on PPU: breakpoint
					#ifdef HK_DEBUG
						__asm__ volatile ( "tw 31,1,1" );
					#endif

					return 0; // out of entries.. 
				}	

				HK_FORCE_INLINE int findTlsIndex()
				{
					hkUint64 tid; 
					// r13 == thread id in our setup (set in hkBaseSystem::initThread())
					__asm__ volatile ( "ori %0,13,0" : "=r"( tid ) ); 

					int i;
					// don't want to lock around looking for tid, only when aquiring or releasing a slot
					for(i = 0; i < HK_TLS_MAX_THREADS; ++i)
					{
						if (m_tidToIndex[i] == tid) 
							return i;
					}

					return getNewEntry(tid);
				}

				HK_FORCE_INLINE T getData() 
				{
					const int tidIndex = findTlsIndex();
					return m_storage[tidIndex];
				}

				void freeStorage()
				{
					cellSyncMutexLock(&m_tidToIndexMutex);
					hkUint64 tid; 
					// r13 == thread id in our setup (set in hkBaseSystem::initThread())
					__asm__ volatile ( "ori %0,13,0" : "=r"( tid ) ); 

					for(int i = 0; i < HK_TLS_MAX_THREADS; ++i)
					{
						if (m_tidToIndex[i] == tid)
						{
							m_tidToIndex[i] = HK_TLS_NO_INDEX;
							m_storage[i] = (T)HK_NULL;
							break;
						}
					}
					cellSyncMutexUnlock(&m_tidToIndexMutex);
					return;
				}

				HK_FORCE_INLINE void setData(T p) 
				{
					if (p != (T)HK_NULL) 
					{
						const int tidIndex = findTlsIndex();
						m_storage[tidIndex] = p;
					}
					else
					{
						// setting to NULL same as saying you don't need specifc
						// TLS anymore as 0 is default
						freeStorage();
					}
				}
				
			protected:

				// Any more complicate structure requires more processing overhead to access
				//  Make sure that sizeof hkThreadLocalData and alignemnt the same on PPU as SPU
				CellSyncMutex m_tidToIndexMutex;
				hkUint64 m_tidToIndex[HK_TLS_MAX_THREADS]; 
				T m_storage[HK_TLS_MAX_THREADS];

			};
#			define HK_THREAD_LOCAL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_IMPL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_GET(VAR) VAR.getData()
#			define HK_THREAD_LOCAL_SET(VAR, VAL) VAR.setData(VAL)

#	elif defined(HK_PLATFORM_PS3SPU) 

#			include <cell/sync.h>		
			// make sure this matches above def
#			define HK_TLS_MAX_THREADS 7 

			template < typename T > 
			class hkThreadLocalData
			{
			public:

				hkThreadLocalData() { }
				HK_FORCE_INLINE T getData() 
				{
					return m_storage[0];
				}

				HK_FORCE_INLINE void setData(T p) 
				{
					m_storage[0] = p;
				}

			protected:

				CellSyncMutex m_tidToIndexMutex;
				/*sys_ppu_thread_t*/ hkUint64 m_tidToIndex[HK_TLS_MAX_THREADS]; 
				T m_storage[HK_TLS_MAX_THREADS];
			};

#			define HK_THREAD_LOCAL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_IMPL(TYPE) hkThreadLocalData< TYPE >
#			define HK_THREAD_LOCAL_GET(VAR) VAR.getData()
#			define HK_THREAD_LOCAL_SET(VAR, VAL) VAR.setData(VAL)

#	else
#		error fixme, no TLS defined on this platform yet multithreading is enabled
#	endif 
#else
#	define HK_THREAD_LOCAL(TYPE) TYPE
#	define HK_THREAD_LOCAL_IMPL(TYPE) TYPE
#endif

#ifndef HK_THREAD_LOCAL_GET 
#	define HK_THREAD_LOCAL_GET(VAR) (VAR)
#	define HK_THREAD_LOCAL_SET(VAR, VAL) (VAR)=(VAL)
#endif

#endif // HKBASE_HK_THREAD_LOCAL_POINTER_H


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
