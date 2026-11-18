/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_THREAD_SAFE_MEMORY
#define HK_THREAD_SAFE_MEMORY

// Simply forward all calls to malloc and free

class hkThreadSafeMemory : public hkMemory
{
	public:

		hkThreadSafeMemory()
		{
			// we need this so that deallocateStack always goes to onStackUnderflow.
			m_stack.m_base = HK_NULL;
		}

		virtual void getStatSynopsis(hkMemoryStatistics* in){}

		virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl)
		{
			return hkSystemMalloc(nbytes, 16);
		}

		virtual void deallocate(void* p)
		{
			if(p)
			{
				hkSystemFree(p);
			}
		}

		virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl)
		{
			return hkSystemMalloc(nbytes, 16);
		}

		virtual void alignedDeallocate(void* p)
		{
			if(p)
			{
				hkSystemFree(p);
			}
		}

		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl)
		{
			return hkSystemMalloc(nbytes, 16);
		}

		virtual void deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS )
		{
			if(p)
			{
				hkSystemFree(p);
			}
		}

		virtual void printStatistics(hkOstream* c)
		{
		}

		virtual void* onStackOverflow(int nbytes)
		{
			// just forward to the heap.
			// or we could do something fancy here with thread local storage.
			return hkSystemMalloc(nbytes, 16);
		}

		virtual void onStackUnderflow(void* p, int nbytes)
		{
			// just forward to the heap.
			hkSystemFree(p);
		}
};

#endif // HK_THREAD_SAFE_MEMORY

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
