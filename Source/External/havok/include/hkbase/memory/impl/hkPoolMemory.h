/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_POOL_MEMORY
#define HK_POOL_MEMORY

#define HK_PAGE_ALIGN 64

// The number of small chunks
#define HK_MEMORY_MAX_ROW 13

// The largest chunk we allocate from this pool
#define HK_MEMORY_MAX_SIZE_SMALL_BLOCK 512

// How much we allocate when the pool becomes full
#define HK_MEMORY_SYSTEM_PAGE_SIZE 8192

// Debugging
#define HK_MEMORY_MAGIC_NUMBER 0x3425234

#include <hkbase/config/hkConfigMemoryStats.h>

//This table is created iff the statistics are enabled
//it is not accessed otherwise
#include <hkbase/memoryclasses/hkMemoryClassesTable.h>
extern hkMemoryClassInfo hkMemoryClassesTable[];


//
// local structs
//

// a header for the smaller bits which we return
struct hkMemoryElem
{
	// NEVER NEW THIS
	hkMemoryElem *m_next;
	int m_magic;
};

// A header for each system allocation
struct hkMemoryPage
{
	// NEVER NEW THIS
	hkMemoryPage* m_next;
	hkInt32 m_pad[(HK_PAGE_ALIGN - hkSizeOf(hkMemoryPage*))/ hkSizeOf(hkInt32)];
};

class hkMonitor;
struct hkPoolMemoryMonitors
{
	hkMonitor* m_total;
	hkMonitor* m_size;
	hkMonitor* m_blocks;
	hkMonitor* m_nAllocs;
};

struct hkOSMemoryStats
{
	int	m_numPages;
	int m_numSysAllocs;
	int m_sysAllocsSize;
	int m_sysAllocsHighMark;
	int	m_sizeOfPages;
	int m_pageClassOverhead;
};

struct hkMemorySizeStatistics
{
	int m_blocks_in_use;

	hkMemorySizeStatistics()
	{
		m_blocks_in_use = 0;
	}
};

class hkPoolMemory : public hkMemory
{
	public:


		hkPoolMemory();

		~hkPoolMemory();

		virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl);

		virtual void deallocate(void* ptr);

		virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl);

		virtual void alignedDeallocate(void* ptr);

		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl);

		virtual void deallocateChunk(void*, int nbytes, HK_MEMORY_CLASS cl);

		virtual void printStatistics(hkOstream* c);

		virtual void getStatSynopsis(hkMemoryStatistics* in);

	private:

			// list of all pages allocated
		hkMemoryPage* m_allocated_memory_pages;

			// pointers to the start,end and current position in the current page
			// we make them char* instead of void* because ansi c++ does not permit
			// pointer arithmetic on void pointers
		char* m_current_page_start;
		char* m_current_page_end;
		char* m_current_page_space;

			// free list for blocks of each size
		hkMemoryElem* m_free_list[HK_MEMORY_MAX_ROW];

			// a lookup table of size of each block size
		int m_row_to_size_lut[HK_MEMORY_MAX_ROW];

			// a lookup table of sizes to block size
		char m_size_to_row_lut[HK_MEMORY_MAX_SIZE_SMALL_BLOCK+1];

			//statistics for total amount of memory used by havok
		hkOSMemoryStats m_osMemoryStats;

			// statistics for blocks of each size
		hkMemorySizeStatistics m_size_statistics[HK_MEMORY_MAX_ROW];

	private:

		inline int size_to_row( int size );
		inline void* allocate_and_chop_page( int nbytes );
		inline void putOnFreeList(void* p, int nbytes);
		//update the stats when an allocate occurs (will do nothing if stats not enabled)
		inline void updateStatsAllocate(int nbytes, HK_MEMORY_CLASS cl);
		//update the stats when a deallocate occurs (will do nothing if stats not enabled)
		inline void updateStatsDeallocate(int nbytes, HK_MEMORY_CLASS cl);
		//fetch statistics info for a class (will do nothing if stats not enabled)
		hkMemoryClassInfo* getInfo(int classID);
		//when collating statistics, the stats should be propagated from subclasses to superclasses
		void propogateStatsUp();
		//when finished collating, we must undo these additions
		void undoPropogate();
		//print out the memory class hierarchy
		void printMemoryClassTree(hkOstream* c,int id, int level);

		enum { STACK_BUFFER_SIZE= 0x4000 };
		char HK_ALIGNED_VARIABLE(m_stackBuffer[STACK_BUFFER_SIZE+8], 16);
};


inline int hkPoolMemory::size_to_row( int size )
{
	HK_ASSERT (HK_MEMORY_MAX_ROW == 13 );
		 if (size <= 8 )	return 1;
	else if (size <= 16 )	return 2;
	else if (size <= 32 )	return 3;
	else if (size <= 48 )	return 4;
	else if (size <= 64 )	return 5;
	else if (size <= 96 )	return 6;
	else if (size <= 128 )	return 7;
	else if (size <= 160 )	return 8;
	else if (size <= 192 )	return 9;
	else if (size <= 256 )	return 10;
	else if (size <= 320 )	return 11;
	else if (size <= 512 )	return 12;
	else
	{
		HK_BREAKPOINT();
		return -1;
	}
}

inline void hkPoolMemory::putOnFreeList(void* p, int nbytes)
{
	int row = m_size_to_row_lut[nbytes];
	m_size_statistics[row].m_blocks_in_use--;

	hkMemoryElem* me = static_cast<hkMemoryElem *>(p);
	me->m_magic = HK_MEMORY_MAGIC_NUMBER;
	me->m_next = m_free_list[row];
	m_free_list[row] = me;
}


inline void* hkPoolMemory::allocate_and_chop_page( int nbytes )
{
	int row = m_size_to_row_lut[ nbytes ];
	int size = m_row_to_size_lut[ row ];

	int allocated_size = size;

	// need to alloc a new page?
	if ( size + m_current_page_space > m_current_page_end)
	{
		hkMemoryPage* page = static_cast<hkMemoryPage*>(
				hkSystemMalloc( hkSizeOf(hkMemoryPage) + HK_MEMORY_SYSTEM_PAGE_SIZE, 16) );

		page->m_next = m_allocated_memory_pages;
		m_allocated_memory_pages = page;
		m_current_page_start = reinterpret_cast<char*>(page+1);
		m_current_page_space = m_current_page_start;
		m_current_page_end = m_current_page_space + HK_MEMORY_SYSTEM_PAGE_SIZE;
		m_osMemoryStats.m_numPages++;
	}

	void* result = static_cast<void *>(m_current_page_space);
	//
	// Welcome to the innards of the havok memory manager.
	// If you get a null pointer dereference on the next line it
	// is most likely because you did not call hkBaseSystem::init()
	//
	reinterpret_cast<hkMemoryElem*>(m_current_page_space)->m_magic = 0;
	m_current_page_space += size;

	//each block will be put on a free list, but this does not mean, that
	//the number of blocks in use decreases (which is an assumption in the putOnFreeList() call)
	int stats_save = m_size_statistics[row].m_blocks_in_use;
	// allocate rest to get make sure the alignment is ok
	while ( allocated_size < 256 )
	{
		if( size + m_current_page_space < m_current_page_end)
		{
			reinterpret_cast<hkMemoryElem*>(m_current_page_space)->m_magic = 0;
			putOnFreeList( m_current_page_space, size);
			m_current_page_space += size;
		}
		else
		{
			break;
		}
		allocated_size += size;
	}
	m_size_statistics[row].m_blocks_in_use = stats_save;
	return result;
}

hkPoolMemory::hkPoolMemory()
{
	m_current_page_start = HK_NULL;
	m_current_page_end   = HK_NULL;
	m_current_page_space = HK_NULL;
	m_allocated_memory_pages = HK_NULL;

	int i;

	for(i = HK_MEMORY_MAX_ROW-1; i >= 0; --i )
	{
		m_free_list[i] = HK_NULL;
	}

	for(i = 0; i <= HK_MEMORY_MAX_SIZE_SMALL_BLOCK; ++i )
	{
		int row = size_to_row(i);
		m_size_to_row_lut[ i ] = static_cast<char>(row);
		m_row_to_size_lut[row] = i;
	}

	//initialise OS memory stats
	m_osMemoryStats.m_numPages = 0;
	m_osMemoryStats.m_numSysAllocs = 0;
	m_osMemoryStats.m_sysAllocsSize = 0;
	m_osMemoryStats.m_sysAllocsHighMark = 0;

	m_osMemoryStats.m_sizeOfPages = HK_MEMORY_SYSTEM_PAGE_SIZE;
	m_osMemoryStats.m_pageClassOverhead = hkSizeOf(hkMemoryPage);

	// set up a small (16k) stack by default. This class has been allocated
	// by global operator new which typically aligns only to 8 bytes.
	// Round up the current pointer to a multiple of 16.
	unsigned alignbuf = HK_NEXT_MULTIPLE_OF(16, reinterpret_cast<unsigned>(m_stackBuffer));
	setStackArea(reinterpret_cast<char*>( alignbuf ), STACK_BUFFER_SIZE);
}

hkPoolMemory::~hkPoolMemory()
{
	while ( m_allocated_memory_pages != HK_NULL )
	{
		hkMemoryPage* p = m_allocated_memory_pages;
		m_allocated_memory_pages = m_allocated_memory_pages->m_next;
		hkSystemFree( static_cast<void *>(p) );
	}
}



//if statistics are disabled, these functions are empty
#if HK_CONFIG_MEMORY_STATS == HK_CONFIG_MEMORY_STATS_DISABLED
void hkPoolMemory::printStatistics(hkOstream* c)
{
	HK_ASSERT (c!=HK_NULL);
	c->printf("Statistics are disabled, please enable them in hkbase/config/hkConfigMemoryStats.h\n");
}

inline void hkPoolMemory::updateStatsAllocate(int nbytes, HK_MEMORY_CLASS cl)
{
}

inline void hkPoolMemory::updateStatsDeallocate(int nbytes, HK_MEMORY_CLASS cl)
{
}

hkMemoryClassInfo* hkPoolMemory::getInfo(int classID)
{
	return (hkMemoryClassInfo*)HK_NULL;
}

void hkPoolMemory::printMemoryClassTree(hkOstream* c,int id, int level)
{
}

#else

#include <hkbase/string/hkString.h>

//print out the information for a class
inline void HK_CALL printInfo(hkOstream* c,hkMemoryClassInfo* info, int level)
{
	if(	info->m_classStats.m_size_in_use |
		info->m_classStats.m_blocks_in_use |
		info->m_classStats.m_n_allocates |
		info->m_classStats.m_new_size_in_use |
		info->m_classStats.m_n_news |
		info->m_classStats.m_max_size_in_use)
	{
		const int indent = level * 2;
		const int len = hkString::strLen(info->m_name) - 16 + indent;
		char buf[30];

		for(int i = 0; i < level; i++)
		{
			hkString::sprintf(&buf[(i << 1)],"--");
		}

		hkString::sprintf(&buf[indent],&info->m_name[16]);
		c->printf( "%-20s, %10i, %10i, %10i, %10i, %10i, %10i\n",
				buf,
				info->m_classStats.m_size_in_use,
				info->m_classStats.m_blocks_in_use,
				info->m_classStats.m_n_allocates,
				info->m_classStats.m_new_size_in_use,
				info->m_classStats.m_n_news,
				info->m_classStats.m_max_size_in_use);
	}
}

//recursively print the tree from this entry on
//NOTE: any nodes encountered which are not children of this node
//will not be printed
void hkPoolMemory::printMemoryClassTree(hkOstream* c,int id, int level)
{
	hkMemoryClassInfo* info = getInfo(id);
	//print out the root info for this sub-tree
	printInfo(c,info,level);

	//print out the sub-trees of this one
	int i = id + 1;
	hkMemoryClassInfo* subInfo = &info[1];

	//search through the array for
	while(subInfo->m_name != HK_NULL)
	{
		if(subInfo->m_parent == id)
		{
			printMemoryClassTree(c,i,level + 1);
		}
		i++;
		subInfo = &subInfo[1];
	}
}

void hkPoolMemory::printStatistics(hkOstream* c)
{
	int max_mem = 0;
	int current_mem = 0;
	int system_allocs = 0;
	unsigned i;
	hkMemoryPage* page = m_allocated_memory_pages;

	while(page != HK_NULL)
	{
		system_allocs++;
		page = page->m_next;
	}

	HK_ASSERT (c!=HK_NULL);

	c->printf("\nMemory allocated by the Operating System for the Pool\n");
	c->printf(  "*****************************************************\n");

	c->printf("page size, page overhead, no. of pages\n");
	c->printf("%9i,%14i,%13i\n",
				m_osMemoryStats.m_sizeOfPages,
				m_osMemoryStats.m_pageClassOverhead,
				m_osMemoryStats.m_numPages);

	c->printf("\nMemory allocated by the Operating System outside the Pool\n");
	c->printf(  "*********************************************************\n");

	c->printf("num allocs, total size, high mark\n");
	c->printf("%10i,%11i,%10i\n",
				m_osMemoryStats.m_numSysAllocs,
				m_osMemoryStats.m_sysAllocsSize,
				m_osMemoryStats.m_sysAllocsHighMark);

	c->printf("\nDetails by type\n");
	c->printf("***************\n");
	c->printf("\n%-20s, %10s, %10s, %10s, %10s, %10s, %10s\n","type", "block bytes", "blocks", "allocs", "OS bytes", "news", "high mark (block bytes)");

	propogateStatsUp();

	printMemoryClassTree(c,0,0);

	undoPropogate();

	c->printf("\nCurrent Memory details by size\n");
	c->printf(  "******************************\n");

	int size_in_use = 0;

	c->printf( "block size, blocks in use, free blocks, size in use\n");
	for (i = 1; i < HK_MEMORY_MAX_ROW; i++)
	{
		int free_blocks = 0;
		for ( hkMemoryElem* el = m_free_list[i];   el != HK_NULL;   el = el->m_next )
		{
			free_blocks++;
		}
		c->printf( "%10i,%14i,%12i,%12i\n",
				m_row_to_size_lut[i],
				m_size_statistics[i].m_blocks_in_use,
				free_blocks,
				m_size_statistics[i].m_blocks_in_use * m_row_to_size_lut[i]);
		size_in_use += m_row_to_size_lut[i] * m_size_statistics[i].m_blocks_in_use;
	}

	c->printf("\nCurrent Memory Usage Summary\n");
	c->printf(  "****************************\n");

	int size_free = (m_osMemoryStats.m_sizeOfPages * m_osMemoryStats.m_numPages) - size_in_use;

	int totalOSMemory = ((m_osMemoryStats.m_sizeOfPages + m_osMemoryStats.m_pageClassOverhead) * m_osMemoryStats.m_numPages) +
				m_osMemoryStats.m_sysAllocsSize;

	c->printf("Total memory in Pool = %i",size_in_use + size_free + m_osMemoryStats.m_pageClassOverhead * m_osMemoryStats.m_numPages);
	c->printf("\t%i (%2.3f percent) free, ",size_free,(((float)size_free) / (size_in_use + size_free)) * 100);
	c->printf("%i overhead\n",m_osMemoryStats.m_pageClassOverhead * m_osMemoryStats.m_numPages);
	c->printf("Total outside Pool   = %i\n",m_osMemoryStats.m_sysAllocsSize);
	c->printf("**********************\n");
	c->printf("Total memory         = %i\n",totalOSMemory);
}

hkMemoryClassInfo* hkPoolMemory::getInfo(int classID)
{
	//Any zeroes in a level will stop parsing of sub-classes
	// i.e. 01000000 is BASE class (at level 0) 01023300 (is a subclass at level 2)
	// while 01003356 is BASE class also (at level 0)
	hkMemoryClassInfo* info;

	info = &(hkMemoryClassesTable[classID]);

	return info;
}


inline void hkPoolMemory::updateStatsAllocate(int nbytes, HK_MEMORY_CLASS cl)
{
	hkMemoryClassInfo* info = HK_NULL;

	info = getInfo(cl);

	HK_ASSERT2(info!=HK_NULL,"Memory class identifier represents unknown class");

	if (nbytes <= HK_MEMORY_MAX_SIZE_SMALL_BLOCK)
	{
		int row = m_size_to_row_lut[ nbytes ];
		int size = m_row_to_size_lut[ row ];

		info->m_classStats.m_n_allocates++;
		info->m_classStats.m_size_in_use += size;
		info->m_classStats.m_blocks_in_use++;
		if( info->m_classStats.m_size_in_use > info->m_classStats.m_max_size_in_use)
		{
			info->m_classStats.m_max_size_in_use = info->m_classStats.m_size_in_use;
		}
	}
	else
	{
		info->m_classStats.m_n_news += 1;
		info->m_classStats.m_new_size_in_use += nbytes;
		m_osMemoryStats.m_numSysAllocs++;
		m_osMemoryStats.m_sysAllocsSize += nbytes;

		if (m_osMemoryStats.m_sysAllocsSize > m_osMemoryStats.m_sysAllocsHighMark )
		{
			m_osMemoryStats.m_sysAllocsHighMark = m_osMemoryStats.m_sysAllocsSize;
		}
	}
}

inline void hkPoolMemory::updateStatsDeallocate(int nbytes, HK_MEMORY_CLASS cl)
{
	hkMemoryClassInfo* info = HK_NULL;

	//info = getInfo(mask2 & cl);
	info = getInfo(cl);

	HK_ASSERT2(info!=HK_NULL,"Memory class identifier represents unknown class");
	if (nbytes <= HK_MEMORY_MAX_SIZE_SMALL_BLOCK)
	{
		int row = m_size_to_row_lut[ nbytes ];
		int size = m_row_to_size_lut[ row ];

		info->m_classStats.m_size_in_use -= size;
		info->m_classStats.m_blocks_in_use--;
	}
	else
	{
		info->m_classStats.m_new_size_in_use -= nbytes;
		m_osMemoryStats.m_sysAllocsSize -= nbytes;
	}
}


void hkPoolMemory::propogateStatsUp()
{
	//walk the classes, from the bottom up
	for(int i = (HK_MEMORY_CLASS_MAX - 1); i > 0; i--)
	{
		hkMemoryClassInfo* info	= getInfo(i);
		hkMemoryClassInfo* superInfo = getInfo(info->m_parent);

		superInfo->m_classStats += info->m_classStats;
	}
}

void hkPoolMemory::undoPropogate()
{
	//walk the classes, from the bottom up
	for(int i = 1; i <= (HK_MEMORY_CLASS_MAX - 1); i++)
	{
		hkMemoryClassInfo* info	= getInfo(i);
		hkMemoryClassInfo* superInfo = getInfo(info->m_parent);

		superInfo->m_classStats -= info->m_classStats;
	}

}

#endif
//
// allocate and deallocate forward their calls to allocateChunk and deallocateChunk
// storing the extra info before the returned pointer
//

struct hkMemorySizeInfo
{
	hkInt32 m_magic;
	hkInt32 m_size;
	hkInt32 m_class;
	hkInt32 m_alignedOffset; // only used in aligned{Allocate,Deallocate}
};

#define MAGIC_MEMORY_WITH_SIZE 0x2345656

void* hkPoolMemory::allocate(int nbytes, HK_MEMORY_CLASS cl)
{
	hkMemorySizeInfo* x = static_cast<hkMemorySizeInfo*>(
								allocateChunk(nbytes + hkSizeOf(hkMemorySizeInfo), cl) );
	x->m_magic = MAGIC_MEMORY_WITH_SIZE;
	x->m_size = (int)nbytes; /// XXX
	x->m_class = cl;
	return static_cast<void*>(x+1);
}

void  hkPoolMemory::deallocate(void* p)
{
	if(p)
	{
		hkMemorySizeInfo* x = static_cast<hkMemorySizeInfo*>(p) - 1;
		HK_ASSERT( x->m_magic == MAGIC_MEMORY_WITH_SIZE );
		x->m_magic = 0xdeadbeef;
		deallocateChunk( static_cast<void*>(x),
						x->m_size + hkSizeOf( hkMemorySizeInfo ),
						static_cast<HK_MEMORY_CLASS>(x->m_class) );
	}
}

void* hkPoolMemory::alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl)
{
	// allocate enough to hold the nbytes, the size info and the alignment window
	char* unaligned = reinterpret_cast<char*>(
								allocateChunk(alignment + nbytes + hkSizeOf(hkMemorySizeInfo), cl) );
	// the aligned memory is the nearest aligned block, taking into account that the
	// sizeinfo which is placed just before the returned pointer.
	char* aligned = reinterpret_cast<char*>(
			HK_NEXT_MULTIPLE_OF( alignment,	reinterpret_cast<unsigned>(unaligned+hkSizeOf(hkMemorySizeInfo))) );
	// store the sizeinfo just before the returned pointer
	hkMemorySizeInfo* x = reinterpret_cast<hkMemorySizeInfo*>(aligned) - 1;
	x->m_magic = MAGIC_MEMORY_WITH_SIZE;
	x->m_size = nbytes + alignment; /// XXX
	x->m_class = cl;
	x->m_alignedOffset = (int)(aligned - unaligned);

	return static_cast<void*>(aligned);
}

void hkPoolMemory::alignedDeallocate(void* p)
{
	if(p)
	{
		hkMemorySizeInfo* x = static_cast<hkMemorySizeInfo*>(p) - 1;
		HK_ASSERT( x->m_magic == MAGIC_MEMORY_WITH_SIZE );
		x->m_magic = 0xdeadbeef;

		char* unaligned = reinterpret_cast<char*>(p) - x->m_alignedOffset;

		deallocateChunk( static_cast<void*>(unaligned),
						x->m_size + hkSizeOf(hkMemorySizeInfo),
						static_cast<HK_MEMORY_CLASS>(x->m_class) );
	}
}


#if defined(HK_PLATFORM_GC) && defined(__MWERKS__)
#pragma optimization_level 0
#endif

//
// allocateChunk and deallocateChunk
//
void* hkPoolMemory::allocateChunk(int nbytes, HK_MEMORY_CLASS cl)
{
	updateStatsAllocate(nbytes,cl);

	if ( nbytes <= HK_MEMORY_MAX_SIZE_SMALL_BLOCK)
	{
		int row = m_size_to_row_lut[nbytes];
		m_size_statistics[row].m_blocks_in_use++;

		hkMemoryElem* n = m_free_list[row];
		if(n)
		{
			m_free_list[row] = n->m_next;

			n->m_magic = 0;
			return static_cast<void*>(n);
		}
	}
	else
	{
		return hkSystemMalloc( nbytes, 16 );
	}

	// fell through, we ran out of space for that row
	return allocate_and_chop_page( nbytes );
}

#if defined(HK_PLATFORM_GC) && defined(__MWERKS__)
#pragma optimization_level reset
#endif


void  hkPoolMemory::deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS cl)
{
	if(p)
	{
		updateStatsDeallocate(nbytes,cl);

		if ( nbytes <= HK_MEMORY_MAX_SIZE_SMALL_BLOCK )
		{
			putOnFreeList(p,nbytes);
		}
		else
		{
			hkSystemFree(p);
		}
	}
}

void hkPoolMemory::getStatSynopsis(hkMemoryStatistics* in)
{
	in->m_osMemoryTotal = m_osMemoryStats.m_sysAllocsSize;
	in->m_osHighMark = m_osMemoryStats.m_sysAllocsHighMark;

	int size_in_use = 0;

	for (int i = 1; i < HK_MEMORY_MAX_ROW; i++)
	{
		size_in_use += m_row_to_size_lut[i] * m_size_statistics[i].m_blocks_in_use;
	}

	int size_free = (m_osMemoryStats.m_sizeOfPages * m_osMemoryStats.m_numPages) - size_in_use;

	in->m_poolMemoryTotal = ((m_osMemoryStats.m_sizeOfPages + m_osMemoryStats.m_pageClassOverhead) * m_osMemoryStats.m_numPages);

	in->m_poolMemoryUsed = size_in_use;
}

#endif // HK_POOL_MEMORY



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
