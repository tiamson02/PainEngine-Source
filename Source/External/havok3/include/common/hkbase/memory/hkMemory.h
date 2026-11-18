/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKMEMORY_H
#define HKBASE_HKMEMORY_H

#include <hkbase/memoryclasses/hkMemoryClassDefinitions.h>

class hkOstream;

#include <hkbase/config/hkConfigMemory.h>

extern void*	(HK_CALL *hkSystemMalloc)(int size, int align);
extern void		(HK_CALL *hkSystemFree) (void* p);

	/// A struct holding memory allocation information
struct hkMemoryStatistics
{
		/// The total number of memory allocations bypassing the pool memory and going 
		/// directly to the system memory. If you use the hkPoolMemory, then very big
		/// allocations (greater then 8K) can't be handled by the pool and have to go 
		/// to the system memory
	int m_numSysAllocs;

		/// The total number of bytes allocated by the system memory
	int m_sysAllocsSize;

		/// The current maximum of the total number of bytes allocated by the system memory
	int m_sysAllocsHighMark;

		/// The number of pages allocated by the pool memory
	int	m_numPages;
		/// Size of one page without overhead
	int	m_sizeOfPage;
		/// Size of the overhead
	int m_pageOverhead;
		/// Bytes allocated within the pool memory
	int m_pageMemoryUsed;

		/// Returns the size which is allocated by the memory manager.
	inline int getSizeAllocatedByMemoryManager()
	{
		return (m_sizeOfPage + m_pageOverhead) * m_numPages + m_sysAllocsSize;
	}

		/// Returns the size which is allocated within the memory manager.
		/// Note: If you allocate a block of size 3, it will be padded to 4 bytes
		/// and 4 will be added to m_pageMemoryUsed
	inline int getMemoryUsed()
	{
		return m_pageMemoryUsed + m_sysAllocsSize;
	}
};



	/// All memory allocations in Havok are handled through an instance of this class.
	/// Note that all allocations greater than 8 bytes should be 16 byte aligned.
	/// Allocations of 8 bytes or less should be aligned to 8 bytes.
	/// We distinguish between allocations where the size is explicitly
	/// stored or not stored. See the hkBase user guide for more details.
class hkMemory
{
	public:

			/// Initialise the reference count to 1.
		hkMemory();

			/// Allocate nbytes bytes, aligned to 16 bytes.
			/// Allocations of 8 bytes or less are aligned to 8 bytes.
			/// Memory classes are defined in 'hkbase/memory/hkMemoryClass.h'.
		virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory.
		virtual void  deallocate(void* ptr) = 0;

			/// Allocates nbytes bytes, aligned to 'alignment' which is a power of 2.
			/// Note that the default allocate is already 16 byte aligned.
			/// Memory classes are defined in 'hkbase/memoryclasses/hkMemoryClasses.h'.
		virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory. This must have been allocated by a call to alignedAllocate.
		virtual void  alignedDeallocate(void* ptr) = 0;
		
			/// Allocates a fixed size piece of memory, aligned to 16 bytes.
			/// Allocations of 8 bytes or less are aligned to 8 bytes.
			/// Note that the size is not stored and must be supplied when deallocating.
		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory that has been allocated by allocateChunk
		virtual void deallocateChunk(void*, int nbytes, HK_MEMORY_CLASS ) = 0;

			/// An optimized interface used by the hkThreadMemory
		virtual void* allocateChunkByRow(int rowId, HK_MEMORY_CLASS cl);

			/// Deallocates a piece of memory that has been allocated by allocateChunk
		virtual void deallocateChunkByRow(void*, int rowId, HK_MEMORY_CLASS );

			/// returns true if the above functions are properly implemented
		virtual bool isAllocateChunkByRowSupported();

			/// Prints some statistics to the specified console.
		virtual void printStatistics(hkOstream* c) = 0;

			/// Find the memory rounded up by the memory allocator for a requested memory allocation
		virtual int getAllocatedSize( int size );

			/// Get a simple synopsis of memory usage.
		virtual void getStatSynopsis(hkMemoryStatistics* reportOut) = 0;

		inline hkMemoryStatistics& getMemoryStatistics()
		{
			return m_memoryStatistics;
		}

		HK_FORCE_INLINE int getAvailableMemory( );

			/// Some impls (the DebugMemory for instance) can do checks on the current
			/// allocated pool. If the impl does not support the check it will just return true.
		virtual hkBool isOk() const;

		enum MemoryState
		{
			MEMORY_STATE_OK,
			MEMORY_STATE_OUT_OF_MEMORY
		};

		MemoryState m_memoryState;
		int m_criticalMemoryLimit;

		static inline hkMemory& HK_CALL getInstance();

		static void HK_CALL replaceInstance(hkMemory* m);

		HK_FORCE_INLINE void addReference();

		HK_FORCE_INLINE void removeReference();

			/// Constant values used in debug modes.
		enum MemoryFill
		{
				/// For guard bytes before and after allocations.
			MEMORY_FILL_GUARD_AREA = 0xFD,
				/// Unused, free memory is filled with this value.
			MEMORY_FILL_FREE_SPACE = 0xDD,
				/// Newly allocated memory.
			MEMORY_FILL_UNINITIALIZED = 0xCD
		};

	protected:

		virtual ~hkMemory() { }

	protected:

		static hkMemory* s_instance;
		int m_referenceCount;

			//statistics for total amount of memory used by havok
		hkMemoryStatistics m_memoryStatistics;
};




hkMemory& HK_CALL hkMemory::getInstance()
{
	return *s_instance;
}

void hkMemory::removeReference()
{
	--m_referenceCount;
	if(m_referenceCount==0)
	{
		delete this;
	}
}

void hkMemory::addReference()
{
	++m_referenceCount;
}



/// Convenience inline function to allocate memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE TYPE* HK_CALL hkAllocate(int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().allocate(n*hkSizeOf(TYPE), cl));
}


/// Convenience inline function to deallocate memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE void HK_CALL hkDeallocate(TYPE* ptr)
{
	hkMemory::getInstance().deallocate(static_cast<void *>(ptr));
}

/// Convenience inline function to allocate aligned memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE TYPE* HK_CALL hkAlignedAllocate(int alignment,int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().alignedAllocate(alignment,n*hkSizeOf(TYPE), cl));
}

/// Convenience inline function to deallocate memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE void HK_CALL hkAlignedDeallocate(TYPE* ptr)
{
	hkMemory::getInstance().alignedDeallocate(static_cast<void *>(ptr));
}


/// Convenience function to allocate memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE TYPE* HK_CALL hkAllocateChunk(int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().allocateChunk(n*hkSizeOf(TYPE), cl));
}


/// Convenience function to deallocate memory of the correct type
template <typename TYPE>
HK_FORCE_INLINE void HK_CALL hkDeallocateChunk(TYPE* ptr, int n, HK_MEMORY_CLASS cl)
{
	hkMemory& mem = hkMemory::getInstance();
	mem.deallocateChunk(static_cast<void*>(ptr), n*hkSizeOf(TYPE), cl);
}


HK_FORCE_INLINE int hkMemory::getAvailableMemory( )
{
	hkMemory& mem = hkMemory::getInstance();
	int memoryUsed = mem.getMemoryStatistics().getMemoryUsed();
	if ( mem.m_criticalMemoryLimit <= memoryUsed)
	{
		return 0;
	}
	return mem.m_criticalMemoryLimit - memoryUsed;
}


#if defined(HK_DEBUG)
	extern void HK_CALL HK_ASSERT_OBJECT_SIZE_OK_FUNC(hk_size_t nbytes);
#	define HK_ASSERT_OBJECT_SIZE_OK(A) HK_ASSERT_OBJECT_SIZE_OK_FUNC(A)
#else
#	define HK_ASSERT_OBJECT_SIZE_OK(A)
#endif

#if !defined(HK_COMPILER_MWERKS)

#define HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE) \
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT_OBJECT_SIZE_OK(nbytes); hkReferencedObject* b = static_cast<hkReferencedObject*>(hkMemory::getInstance().allocateChunk(static_cast<int>(nbytes),TYPE)); b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes); return b; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete(void* p)			{ hkReferencedObject* b = static_cast<hkReferencedObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_memSizeAndFlags,TYPE); }	\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t nbytes)	{ hkReferencedObject* b = static_cast<hkReferencedObject*>(hkMemory::getInstance().allocateChunk(static_cast<int>(nbytes),TYPE)); b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes); return b; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void* p)			{ hkReferencedObject* b = static_cast<hkReferencedObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_memSizeAndFlags,TYPE); }	\
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete(void*, void*)		{ }	\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p)	{ HK_BREAKPOINT(); return p; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void*, void*)	{ HK_BREAKPOINT();	} \
	class MustEndWithSemiColon


#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(TYPE,CLASS_SIZE) \
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT(0x6c787b7f, nbytes == CLASS_SIZE ); return hkMemory::getInstance().allocateChunk(static_cast<int>(nbytes),TYPE); }	\
	HK_FORCE_INLINE void  HK_CALL operator delete(void* p)			{ hkMemory::getInstance().deallocateChunk(p, CLASS_SIZE, TYPE); }								\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t nbytes)	{ return hkMemory::getInstance().allocate(static_cast<int>(nbytes),TYPE); }												\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void* p)			{ hkMemory::getInstance().deallocate(p); }															\
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t n, void* p)	{ HK_ASSERT(0x77bb90a1, n == CLASS_SIZE); return p; }										\
	HK_FORCE_INLINE void  HK_CALL operator delete(void*, void*)		{ }																						\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p)	{ return p;	}																			\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void*, void*)	{ } \
	class MustEndWithSemiColon

#else // not MWERKS

#define HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE) \
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT_OBJECT_SIZE_OK(nbytes); hkReferencedObject* b = static_cast<hkReferencedObject*>(hkMemory::getInstance().allocateChunk(int(nbytes),TYPE)); b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes); return b; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete(void* p)			{ hkReferencedObject* b = static_cast<hkReferencedObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_memSizeAndFlags,TYPE); }	\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t nbytes)	{ hkReferencedObject* b = static_cast<hkReferencedObject*>(hkMemory::getInstance().allocateChunk(int(nbytes),TYPE)); b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes); return b; }	\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void* p)			{ hkReferencedObject* b = static_cast<hkReferencedObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_memSizeAndFlags,TYPE); }	\
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p)	{ HK_BREAKPOINT(); return p; } \
	class MustEndWithSemiColon

//	HK_FORCE_INLINE void  HK_CALL operator delete(void*, void*)		{ }
//	HK_FORCE_INLINE void  HK_CALL operator delete[](void*, void*)	{ HK_BREAKPOINT();	}

#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(TYPE,CLASS_SIZE) \
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT(0x2210c797, nbytes == CLASS_SIZE); return hkMemory::getInstance().allocateChunk(int(nbytes),TYPE); }	\
	HK_FORCE_INLINE void  HK_CALL operator delete(void* p)			{ hkMemory::getInstance().deallocateChunk(p, int(CLASS_SIZE), TYPE); }								\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t nbytes)	{ return hkMemory::getInstance().allocate(int(nbytes),TYPE); }												\
	HK_FORCE_INLINE void  HK_CALL operator delete[](void* p)			{ hkMemory::getInstance().deallocate(p); }															\
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t n, void* p)	{ HK_ASSERT(0x3506e23f, int(n) == int(CLASS_SIZE) ); return p; }										\
	HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p)	{ return p;	} \
	class MustEndWithSemiColon

//	inline void  HK_CALL operator delete(void*, void*)		{ }
//	inline void  HK_CALL operator delete[](void*, void*)	{ }



#endif // MWERKS

//
// In debug, use some compile time trickery to ensure correct allocator is used.
// Use -Wshadow to catch nonvirtual allocators in virtual classes.
//
#if defined(HK_DEBUG)
#   define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
        HK_FORCE_INLINE const hkReferencedObject* iMustBeDerivedFromReferencedObject() const { return static_cast<const hkReferencedObject*>(this); } \
        HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE)
#   define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(MEMORY_CLASS,THIS_CLASS) \
        HK_FORCE_INLINE void iShouldNotHaveVtable() const { int m_memSizeAndFlags = 0; m_memSizeAndFlags--; } \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, sizeof(THIS_CLASS))
#else
#   define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
        HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE)
#   define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(TYPE,THIS_CLASS) \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(TYPE, sizeof(THIS_CLASS))
#endif


#endif // HKBASE_HKMEMORY_H

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
