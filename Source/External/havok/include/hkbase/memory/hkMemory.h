/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKMEMORY_H
#define HKBASE_HKMEMORY_H

#include <hkbase/memoryclasses/hkMemoryClassDefinitions.h>

class hkOstream;

#include <hkbase/config/hkConfigMemory.h>

#if HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_ENABLED

/// A
struct hkMemoryStatistics
{
		/// This is the total amount of memory used by the pool (including per-page overhead)
	int m_poolMemoryTotal;
		// This is the total amount currently allocated in block lists
	int m_poolMemoryUsed;

		/// This is the total amount of memory allocated for use by our system OUTSIDE the pool
	int m_osMemoryTotal;
		/// This is the high mark for the memory allocated for use by our system OUTSIDE the pool
	int m_osHighMark;
};

/// All memory allocations in Havok are handled by this class.
/// We distinguish between allocations where the size is explicitly
/// stored or not stored. See the hkBase user guide for more details.
class hkMemory   
{
	public:

			/// Initialise the reference count to 1.
		hkMemory()
			:	m_referenceCount(1)
		{
		}

			/// Allocates nbytes bytes. Memory classes are defined in 'hkbase/memory/hkMemoryClass.h'.
		virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory.
		virtual void  deallocate(void* ptr) = 0;

			/// Allocates nbytes bytes, start of block is aligned. Memory classes are defined in 'hkbase/memory/hkMemoryClass.h'.
		virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory. This must have been allocated by a call to alignedAllocate.
		virtual void  alignedDeallocate(void* ptr) = 0;
		
			/// Allocates a fixed size piece of memory.
			/// Note that the size is not stored and must be supplied when deallocating.
		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl) = 0;

			/// Deallocates a piece of memory that has been allocated by allocateChunk
		virtual void deallocateChunk(void*, int nbytes, HK_MEMORY_CLASS ) = 0;

			/// Allocates a piece of memory from a LIFO pool.
		inline void* allocateStack(int nbytes);

			/// Deallocates a piece of memory that has been allocated by allocateStack
		inline void deallocateStack(void* p, int nbytes);

			/// Make initial stack allocations come from buf.
		void setStackArea(void* buf, int nbytes);

			/// Prints some statistics to the specified console.
		virtual void printStatistics(hkOstream* c) = 0;

			/// Get a simple synopsis of memory usage.
		virtual void getStatSynopsis(hkMemoryStatistics* in) = 0;

		static inline hkMemory& HK_CALL getInstance();

		static inline void HK_CALL replaceInstance(hkMemory* m);

		inline void addReference();

		inline void removeReference();

	protected:

		virtual ~hkMemory() { }

			/// Override this function to hook into the frame based allocation.
		virtual void* onStackOverflow(int nbytes);

			/// Override this function to hook into the frame based allocation.
		virtual void onStackUnderflow(void* p, int nbytes);

		struct Stack
		{
			Stack()
				:	m_current(HK_NULL),
					m_freebytes(0),
					m_prev(HK_NULL),
					m_base((char*)(-1))
					// base is used several ways:
					// usually points to base of current block.
					// if -1 means theres is no current block.
					// if zero means stack allocation is disabled.
			{
			}
			char* HK_ALIGNED_VARIABLE(m_current, 16);
			int m_freebytes;
			Stack* m_prev;
			char* m_base;
		};
		static hkMemory* s_instance;
		int m_referenceCount;
		Stack m_stack;
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

void HK_CALL hkMemory::replaceInstance(hkMemory* m)
{
	if(s_instance)
	{
		s_instance->removeReference();
	}
	s_instance = m;
}

void hkMemory::addReference() 
{ 
	++m_referenceCount;
}

inline void* hkMemory::allocateStack(int nbytesin)
{
	int actualBytes = HK_NEXT_MULTIPLE_OF(16, nbytesin);
	if( actualBytes > m_stack.m_freebytes )
	{
		return onStackOverflow(actualBytes);
	}
	else
	{
		void* p = m_stack.m_current;
		m_stack.m_current += actualBytes;
		m_stack.m_freebytes -= actualBytes;
		return p;
	}
}

inline void hkMemory::deallocateStack(void* p, int nbytesin)
{
	int actualBytes = HK_NEXT_MULTIPLE_OF(16, nbytesin);
	if( actualBytes && (p == m_stack.m_base || m_stack.m_base == HK_NULL) )
	{
		onStackUnderflow(p, actualBytes);
	}
	else
	{
		m_stack.m_current -= actualBytes;
		m_stack.m_freebytes += actualBytes;
	}
}

/// Convenience inline function to allocate memory of the correct type
template <typename TYPE>
inline TYPE* HK_CALL hkAllocate(int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().allocate(n*hkSizeOf(TYPE), cl));
}
/// Convenience inline function to deallocate memory of the correct type
template <typename TYPE>
inline void HK_CALL hkDeallocate(TYPE* ptr)
{
	hkMemory::getInstance().deallocate(static_cast<void *>(ptr));
}

/// Convenience inline function to allocate aligned memory of the correct type
template <typename TYPE>
inline TYPE* HK_CALL hkAlignedAllocate(int alignment,int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().alignedAllocate(alignment,n*hkSizeOf(TYPE), cl));
}
/// Convenience inline function to deallocate memory of the correct type
template <typename TYPE>
inline void HK_CALL hkAlignedDeallocate(TYPE* ptr)
{
	hkMemory::getInstance().alignedDeallocate(static_cast<void *>(ptr));
}


/// Convenience function to allocate memory of the correct type
template <typename TYPE>
inline TYPE* HK_CALL hkAllocateChunk(int n, HK_MEMORY_CLASS cl)
{
	return static_cast<TYPE*>(hkMemory::getInstance().allocateChunk(n*hkSizeOf(TYPE), cl));
}
/// Convenience function to deallocate memory of the correct type
template <typename TYPE>
inline void HK_CALL hkDeallocateChunk(TYPE* ptr, int n, HK_MEMORY_CLASS cl)
{
	hkMemory::getInstance().deallocateChunk(static_cast<void*>(ptr), n*hkSizeOf(TYPE), cl);
}

template <typename TYPE>
inline TYPE* HK_CALL hkAllocateStack(int n)
{
	return static_cast<TYPE*>( hkMemory::getInstance().allocateStack(n*hkSizeOf(TYPE)) );
}

template <typename TYPE>
inline void HK_CALL hkDeallocateStack(TYPE* ptr, int n)
{
	hkMemory::getInstance().deallocateStack( static_cast<void*>(ptr), n*hkSizeOf(TYPE) );
}

#if !defined(HK_COMPILER_MWERKS)

#define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
	inline void* HK_CALL operator new(hk_size_t nbytes)		{ hkBaseObject* b = static_cast<hkBaseObject*>(hkMemory::getInstance().allocateChunk(nbytes,TYPE)); b->m_object_memsize = static_cast<hkUint16>(nbytes); return b; }	\
	inline void  HK_CALL operator delete(void* p)			{ hkBaseObject* b = static_cast<hkBaseObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_object_memsize,TYPE); }	\
	inline void* HK_CALL operator new[](hk_size_t nbytes)	{ hkBaseObject* b = static_cast<hkBaseObject*>(hkMemory::getInstance().allocateChunk(nbytes,TYPE)); b->m_object_memsize = static_cast<hkUint16>(nbytes); return b; }	\
	inline void  HK_CALL operator delete[](void* p)			{ hkBaseObject* b = static_cast<hkBaseObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_object_memsize,TYPE); }	\
	inline void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
	inline void  HK_CALL operator delete(void*, void*)		{ }	\
	inline void* HK_CALL operator new[](hk_size_t, void* p)	{ HK_BREAKPOINT(); return p; }	\
	inline void  HK_CALL operator delete[](void*, void*)	{ HK_BREAKPOINT();	}


#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(TYPE,CLASS_SIZE) \
	inline void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT(nbytes == CLASS_SIZE ); return hkMemory::getInstance().allocateChunk(nbytes,TYPE); }	\
	inline void  HK_CALL operator delete(void* p)			{ hkMemory::getInstance().deallocateChunk(p, CLASS_SIZE, TYPE); }								\
	inline void* HK_CALL operator new[](hk_size_t nbytes)	{ return hkMemory::getInstance().allocate(nbytes,TYPE); }												\
	inline void  HK_CALL operator delete[](void* p)			{ hkMemory::getInstance().deallocate(p); }															\
	inline void* HK_CALL operator new(hk_size_t n, void* p)	{ HK_ASSERT(n == CLASS_SIZE); return p; }										\
	inline void  HK_CALL operator delete(void*, void*)		{ }																						\
	inline void* HK_CALL operator new[](hk_size_t, void* p)	{ return p;	}																			\
	inline void  HK_CALL operator delete[](void*, void*)	{ }

#else // not MWERKS

#define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
	inline void* HK_CALL operator new(hk_size_t nbytes)		{ hkBaseObject* b = static_cast<hkBaseObject*>(hkMemory::getInstance().allocateChunk(int(nbytes),TYPE)); b->m_object_memsize = static_cast<hkUint16>(nbytes); return b; }	\
	inline void  HK_CALL operator delete(void* p)			{ hkBaseObject* b = static_cast<hkBaseObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_object_memsize,TYPE); }	\
	inline void* HK_CALL operator new[](hk_size_t nbytes)	{ hkBaseObject* b = static_cast<hkBaseObject*>(hkMemory::getInstance().allocateChunk(int(nbytes),TYPE)); b->m_object_memsize = static_cast<hkUint16>(nbytes); return b; }	\
	inline void  HK_CALL operator delete[](void* p)			{ hkBaseObject* b = static_cast<hkBaseObject*>(p); hkMemory::getInstance().deallocateChunk(p, b->m_object_memsize,TYPE); }	\
	inline void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
	inline void* HK_CALL operator new[](hk_size_t, void* p)	{ HK_BREAKPOINT(); return p; }

//	inline void  HK_CALL operator delete(void*, void*)		{ }
//	inline void  HK_CALL operator delete[](void*, void*)	{ HK_BREAKPOINT();	}

#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(TYPE,CLASS_SIZE) \
	inline void* HK_CALL operator new(hk_size_t nbytes)		{ HK_ASSERT(nbytes == CLASS_SIZE); return hkMemory::getInstance().allocateChunk(int(nbytes),TYPE); }	\
	inline void  HK_CALL operator delete(void* p)			{ hkMemory::getInstance().deallocateChunk(p, int(CLASS_SIZE), TYPE); }								\
	inline void* HK_CALL operator new[](hk_size_t nbytes)	{ return hkMemory::getInstance().allocate(int(nbytes),TYPE); }												\
	inline void  HK_CALL operator delete[](void* p)			{ hkMemory::getInstance().deallocate(p); }															\
	inline void* HK_CALL operator new(hk_size_t n, void* p)	{ HK_ASSERT(int(n) == int(CLASS_SIZE) ); return p; }										\
	inline void* HK_CALL operator new[](hk_size_t, void* p)	{ return p;	}

//	inline void  HK_CALL operator delete(void*, void*)		{ }
//	inline void  HK_CALL operator delete[](void*, void*)	{ }



#endif // MWERKS

#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(TYPE,THIS_CLASS) \
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(TYPE, sizeof(THIS_CLASS))

#else // HK_CONFIG_MEMORY_CLASS_DISABLED

#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(TYPE,THIS_CLASS) //nothing
#define HK_DECLARE_CLASS_ALLOCATOR(TYPE)  //nothing
#define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(TYPE,CLASS_SIZE) //nothing

class hkMemory
{
	public:

		static hkResult HK_CALL init() { return HK_SUCCESS; }
		static hkResult HK_CALL quit() { return HK_SUCCESS; }
		static void HK_CALL printStatistics(hkOstream* c) { }
		static void HK_CALL replaceInstance(hkMemory* m) { }
};

template <typename TYPE>
inline TYPE* HK_CALL hkAllocate(int n, HK_MEMORY_CLASS cl)
{
	return new TYPE[n];
}

template <typename TYPE>
inline void HK_CALL hkDeallocate(TYPE* ptr)
{
	delete [] ptr;
}

template <typename TYPE>
inline TYPE* HK_CALL hkAllocateChunk(int n, HK_MEMORY_CLASS cl)
{
	return new TYPE[n];
}

template <typename TYPE>
inline void HK_CALL hkDeallocateChunk(TYPE* ptr, hk_size_t n, HK_MEMORY_CLASS cl)
{
	delete [] ptr;
}

template <typename TYPE>
inline TYPE* HK_CALL hkAlignedAllocate(int align,int n, HK_MEMORY_CLASS cl)
{
	void* pvoid = new char[n*sizeof(TYPE) + align];
	char* p = reinterpret_cast<char*>(pvoid);
	char* aligned = reinterpret_cast<char*>(
				HK_NEXT_MULTIPLE_OF( align,	reinterpret_cast<int>(p+1)) );
	reinterpret_cast<int*>(aligned)[-1] = (int)(aligned - p);
	return reinterpret_cast<TYPE *> (aligned);
}

template <typename TYPE>
inline void HK_CALL hkAlignedDeallocate(TYPE* p)
{
	int offset = reinterpret_cast<int*>(p)[-1];
	char* porig = reinterpret_cast<char*>(p) - offset;
	delete [] porig;
}

template <typename TYPE>
inline TYPE* hkAllocateStack(int n)
{
	return new TYPE[n];
}

template <typename TYPE>
inline void hkDeallocateStack(TYPE* ptr, int)
{
	delete [] ptr;
}


#endif // HK_CONFIG_MEMORY_CLASS

#endif // HKBASE_HKMEMORY_H

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
