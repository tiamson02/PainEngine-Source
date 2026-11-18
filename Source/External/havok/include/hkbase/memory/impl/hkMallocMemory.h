/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MALLOC_MEMORY
#define HK_MALLOC_MEMORY

// Simply forward all calls to malloc and free

class hkMallocMemory : public hkMemory
{
	public:

		hkMallocMemory() 
		{
			// set up a small (16k) stack by default. This class has been allocated
			// by global operator new which typically aligns only to 8 bytes.
			// Round up the current pointer to a multiple of 16.
			unsigned alignbuf = HK_NEXT_MULTIPLE_OF(16, reinterpret_cast<unsigned>(m_stackBuffer));
			setStackArea(reinterpret_cast<char*>( alignbuf ), STACK_BUFFER_SIZE);

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

	private:

		enum { STACK_BUFFER_SIZE= 0x4000 };
		char HK_ALIGNED_VARIABLE(m_stackBuffer[STACK_BUFFER_SIZE+8], 16);
};

#endif // HK_MALLOC_MEMORY


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
