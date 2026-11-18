/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MALLOC_MEMORY
#define HK_MALLOC_MEMORY


/// Simple manager which forwards all calls to malloc and free.
class hkMallocMemory : public hkMemory
{
	public:

		virtual void getStatSynopsis(hkMemoryStatistics* reportOut){}

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
};

#endif // HK_MALLOC_MEMORY


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
