/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>


#if HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED

hkMemory* hkMemory::s_instance;

// given an unaligned pointer, round it up to align and
// store the offset just before the returned pointer.
static inline void* HK_CALL hkMemoryRoundUp(void* pvoid, int align=16)
{
	char* p = reinterpret_cast<char*>(pvoid);
	char* aligned = reinterpret_cast<char*>(
				HK_NEXT_MULTIPLE_OF( align,	reinterpret_cast<int>(p+1)) );
	reinterpret_cast<int*>(aligned)[-1] = (int)(aligned - p);
	return aligned;
}

// given a pointer from hkMemoryRoundUp, recover the original pointer.
static inline void* HK_CALL hkMemoryRoundDown(void* p)
{
	int offset = reinterpret_cast<int*>(p)[-1];
	return static_cast<char*>(p) - offset;
}



#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX) 

#	include <malloc.h>

	static void* HK_CALL hkWindowsMalloc(int size, int align=16)
	{
		return _aligned_malloc(size, align);
	}

	static void HK_CALL hkWindowsFree(void* p)
	{
		_aligned_free(p);
	}

	void* (HK_CALL *hkSystemMalloc)(int size, int align) = hkWindowsMalloc;

	void (HK_CALL *hkSystemFree)(void* p) = hkWindowsFree;
	
#elif defined(HK_PLATFORM_UNIX)

#	include <malloc.h>
	static void* HK_CALL hkUnixMalloc(int size, int align=16)
	{
		return memalign(align, size);
	}
	static void HK_CALL hkUnixFree(void* p)
	{
		free(p);
	}

	void* (HK_CALL *hkSystemMalloc)(int size, int align=16) = hkUnixMalloc;

	void (HK_CALL *hkSystemFree)(void* p) = hkUnixFree;

#elif defined(HK_PLATFORM_GC)

#	define HEAP_ID 0
#	include <dolphin.h>

	static void* HK_CALL hkGameCubeMalloc(int size, int align=16)
	{
		HK_ASSERT(sizeof(int)==sizeof(char*));

		void* p = OSAllocFromHeap(HEAP_ID, size + align);
		return hkMemoryRoundUp(p, align);
	}

	static void HK_CALL hkGameCubeFree(void* p)
	{
		void* origp = hkMemoryRoundDown(p);
		OSFreeToHeap(HEAP_ID, origp);	
	}

	void* (HK_CALL *hkSystemMalloc)(int size, int align=16) = hkGameCubeMalloc;

	void (HK_CALL *hkSystemFree)(void* p) = hkGameCubeFree;

#elif defined(HK_PLATFORM_PS2)

#	include <malloc.h>
	static void* HK_CALL hkPS2Malloc(int size, int align=16)
	{
		HK_ASSERT(sizeof(int)==sizeof(char*));

		void* p = malloc(unsigned(size + align));
		return hkMemoryRoundUp(p, align);
	}

	static inline void HK_CALL hkPS2Free(void* p)
	{
		void* origp = hkMemoryRoundDown(p);
		free(origp);	
	}

	void* (HK_CALL *hkSystemMalloc)(int size, int align=16) = hkPS2Malloc;

	void (HK_CALL *hkSystemFree)(void* p) = hkPS2Free;


#else 

#	error no hkSystem{Malloc,Free} defined for this platform.

#endif

/// This function is called from allocateStack when there is not enough
/// room in the current stack to satisfy the request. It should ajust
/// m_stack as necessary and return a pointer to a new block of nbytes.<p>
/// You can disable the frame based allocation completely (for instance if
/// you are running serveral threads) by overriding this method
/// (and onStackUnderflow)to simply return a new block of memory without
/// updating m_stack. This ensures that allocateStack will always forward
/// to onStackOverflow.
void* hkMemory::onStackOverflow(int nbytesin)
{
	HK_ASSERT( (nbytesin & 0xf) == 0);
	// check threading enabled or else stack is not completely empty
	HK_ASSERT2( ( m_stack.m_base == 0 ) || ( m_stack.m_current != HK_NULL ),
				"The stack was empty when onStackOverflow was called.\n" \
				"This can lead to large slowdowns. To fix this either:\n" \
				" * call setStackArea() with a buffer\n" \
				" * override onStackOverflow, onStackUnderflow to hook into\n" \
				"   your memory system.\n" \
				"See the hkbase userguide for more details.");

	const int smallestalloc = 4096;
	const int size = nbytesin > smallestalloc ? nbytesin : smallestalloc;
	
	//
	// NOTE: If you are overriding this function, copy and paste this implementation,
	// and simply replace the NEXT line with your own stack allocation function
	//
	char* newmem = hkAllocateChunk<char>(size+hkSizeOf(Stack), HK_MEMORY_CLASS_ARRAY);

	// save old stack
	*reinterpret_cast<Stack*>(newmem) = m_stack;
	// update current
	m_stack.m_base = newmem + hkSizeOf(Stack);
	m_stack.m_current = m_stack.m_base + nbytesin;
	m_stack.m_freebytes = size - nbytesin;
	m_stack.m_prev = reinterpret_cast<Stack*>(newmem);
	return m_stack.m_base;
}

/// Called when the current stack becomes empty. See also onStackOverflow.
void hkMemory::onStackUnderflow(void* ptr, int nbytes)
{
	char* chunkAddr = m_stack.m_base - hkSizeOf(Stack);
	int chunkSize = m_stack.m_freebytes + nbytes + hkSizeOf(Stack);
	HK_ASSERT( (int(chunkAddr) & 0xf) == 0);
	// restore previous stack
	m_stack = *reinterpret_cast<Stack*>(chunkAddr);
	//
	// NOTE: If you are overriding this function, copy and paste this implementation,
	// and simply replace the NEXT line with your own stack deallocation function.
	//
	hkDeallocateChunk<char>(chunkAddr, chunkSize, HK_MEMORY_CLASS_ARRAY);
}

/// This can only be called if there are no stack allocations currently
/// in use. I.e. outside of hkWorld::stepDeltaTime callbacks.
void hkMemory::setStackArea(void* buf, int nbytes)
{
	HK_ASSERT2( m_stack.m_base != HK_NULL,
				"setStackArea does not work when the stack area is disabled.");
	HK_ASSERT2( (m_stack.m_prev == HK_NULL)	&& (m_stack.m_base == (char*)-1),
				"Cannot call setStackArea when the stack is in use.");
	m_stack.m_current = static_cast<char*>(buf);
	m_stack.m_freebytes = nbytes;
}

#endif //HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED


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
