/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfiltermanager/hkFilterManager.h>
#include <hkfiltermanager/hkFilterMemoryTracker.h>

hkFilterMemoryTracker::hkFilterMemoryTracker() 
: m_chunkAllocs(HK_NULL), m_allocs(HK_NULL), m_alignedAllocs(HK_NULL) 
{ 
}

hkFilterMemoryTracker::~hkFilterMemoryTracker()
{
	deallocateAll();
}
	
inline void hkFilterMemoryTracker::deallocateAll()
{
	MemoryAlloc* na = m_allocs;
	while (na)
	{
		MemoryAlloc* next = na->m_next;
		hkPoolMemory::deallocate(na->m_mem);
		hkPoolMemory::deallocateChunk(na, sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
		na = next;
	}
	m_allocs = HK_NULL;
	
	MemoryAlloc* aa = m_alignedAllocs;
	while (aa)
	{
		MemoryAlloc* next = aa->m_next;
		hkPoolMemory::alignedDeallocate(aa->m_mem);
		hkPoolMemory::deallocateChunk(aa, sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
		aa = next;
	}
	m_alignedAllocs = HK_NULL;

	MemoryChunk* mc = m_chunkAllocs;
	while(mc)
	{
		MemoryChunk* next = mc->m_next;
		hkPoolMemory::deallocateChunk(mc->m_mem, mc->m_memSize, mc->m_memClass);
		hkPoolMemory::deallocateChunk(mc, sizeof(MemoryChunk), HK_MEMORY_CLASS_EXPORT);
		mc = next;
	}
	m_chunkAllocs = HK_NULL;
}

inline void* hkFilterMemoryTracker::allocate(int nbytes, HK_MEMORY_CLASS cl)
{
	unsigned char* val = (unsigned char*)hkPoolMemory::allocate(nbytes, cl);
	MemoryAlloc* ma = (MemoryAlloc*)hkPoolMemory::allocateChunk(sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
	ma->m_mem = val;
	ma->m_next = m_allocs;
	m_allocs = ma;
	return val;
}

inline void* hkFilterMemoryTracker::allocateChunk(int nbytes, HK_MEMORY_CLASS cl)
{
	unsigned char* val = (unsigned char*)hkPoolMemory::allocateChunk(nbytes, cl);
	MemoryChunk* ca = (MemoryChunk*)hkPoolMemory::allocateChunk(sizeof(MemoryChunk), HK_MEMORY_CLASS_EXPORT);
	ca->m_mem = val;
	ca->m_memSize = nbytes;
	ca->m_memClass = cl;
	ca->m_next = m_chunkAllocs;
	m_chunkAllocs = ca;
	return val;
}

inline void* hkFilterMemoryTracker::alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl)
{
	unsigned char* val = (unsigned char*)hkPoolMemory::alignedAllocate(alignment, nbytes, cl);
	MemoryAlloc* ma = (MemoryAlloc*)hkPoolMemory::allocateChunk(sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
	ma->m_mem = val;
	ma->m_next = m_alignedAllocs;
	m_alignedAllocs = ma;
	return val;
}
	
inline void hkFilterMemoryTracker::deallocate(void* p)
{
	// stop tracking it
	MemoryAlloc* prev = HK_NULL; 
	MemoryAlloc* ma = m_allocs;
	while (ma)
	{
		if (ma->m_mem == p)
		{
			hkPoolMemory::deallocate(ma->m_mem);
			if (prev) prev->m_next = ma->m_next;
			if (m_allocs == ma) m_allocs = ma->m_next;
			hkPoolMemory::deallocateChunk(ma, sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
			return;
		}
		prev = ma;
		ma = ma->m_next;
	}
}

inline void hkFilterMemoryTracker::alignedDeallocate(void* p)
{
	// stop tracking it
	MemoryAlloc* prev = HK_NULL; 
	MemoryAlloc* ma = m_alignedAllocs;
	while (ma)
	{
		if (ma->m_mem == p)
		{
			hkPoolMemory::alignedDeallocate(ma->m_mem);
			if (prev) prev->m_next = ma->m_next;
			if (m_alignedAllocs == ma) m_alignedAllocs = ma->m_next;
			hkPoolMemory::deallocateChunk(ma, sizeof(MemoryAlloc), HK_MEMORY_CLASS_EXPORT);
			return;
		}
		prev = ma;
		ma = ma->m_next;
	}
}

inline void hkFilterMemoryTracker::deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS cl)
{
	// stop tracking it
	MemoryChunk* prev = HK_NULL; 
	MemoryChunk* ma = m_chunkAllocs;
	while (ma)
	{
		if (ma->m_mem == p)
		{
			hkPoolMemory::deallocateChunk(ma->m_mem, ma->m_memSize, ma->m_memClass);
			if (prev) prev->m_next = ma->m_next;
			if (m_chunkAllocs == ma) m_chunkAllocs = ma->m_next;
			hkPoolMemory::deallocateChunk(ma, sizeof(MemoryChunk), HK_MEMORY_CLASS_EXPORT);
			return;
		}
		prev = ma;
		ma = ma->m_next;
	}
}

	


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
