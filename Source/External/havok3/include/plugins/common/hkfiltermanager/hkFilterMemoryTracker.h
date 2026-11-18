/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_MEMORY_TRACKER__H
#define HAVOK_FILTER_MEMORY_TRACKER__H

extern void*	(HK_CALL *hkSystemMalloc)(int size, int align);
extern void		(HK_CALL *hkSystemFree) (void* p);
#include <hkbase/memory/impl/hkPoolMemory.h>

/// A simple class to track all allocations and 
/// allow them all to be deallocated. All filters 
/// should use this for anything allocated into 
/// the scene.
/// All inlined so that the filters don't need a lib for it.
/// It inherits from hkMemory so that you can pop it in as your mem instance, do your
/// object creation and then pop it out again
class hkFilterMemoryTracker : public hkPoolMemory
{
public:

	inline hkFilterMemoryTracker();
	inline ~hkFilterMemoryTracker();

	inline virtual void deallocate(void* p);
	inline virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl);
	inline virtual void alignedDeallocate(void* p);
	inline virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl);
	inline virtual void deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS cl);
	inline virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl);
	
	inline void deallocateAll();

protected:

	// So that we can track hkAllocateChunk calls too
    struct MemoryChunk
	{
		HK_MEMORY_CLASS m_memClass;
		unsigned int m_memSize;
		unsigned char* m_mem;	
		MemoryChunk* m_next;
	};

	struct MemoryAlloc
	{
		unsigned char* m_mem;	
		MemoryAlloc* m_next;
	};

	MemoryChunk* m_chunkAllocs;
	MemoryAlloc* m_allocs;
	MemoryAlloc* m_alignedAllocs;
};

#include <hkfiltermanager/hkFilterMemoryTracker.inl>

#endif // HAVOK_FILTER_MANAGER_INTERFACE__H

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
