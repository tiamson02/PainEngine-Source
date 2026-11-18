#include "Engine.h"

#include "Core/MemoryManager.h"

/*
#ifndef _CRTDBG_MAP_ALLOC
	#include <malloc.h>
#endif
*/
/*
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

void* MemoryManagerStd::Alloc(const size_t size)
{
    void* ptr = 0;

    if(size>0)
    {
        ptr = ::malloc(size);
        Assert(ptr);
    }

    //StackTracer::Log("%X = Alloc(%d)\n", ptr, size);

    return ptr;
}

void* MemoryManagerStd::ReAlloc(void* ptr, const size_t size)
{
	void* rptr = 0;
	if( ptr )
	{
		if( !size )
		{
			::free(ptr);
			rptr = 0;
		}
		else
		{
			rptr = ::realloc(ptr,size);
			Assert(rptr);
		}
	}
	else if( size )
	{
		rptr = ::malloc(size);
		Assert(rptr);
	}

    //StackTracer::Log("%X = Realloc(%X, %d)\n", rptr, ptr, size);

    return rptr;
}
void MemoryManagerStd::Free(void* ptr)
{
    if(ptr)
        ::free(ptr);
}

void* MemoryManagerStd::AllocAligned(const size_t size, const size_t alignment )
{
//	Assert(!(size&(alignment-1)));
    void* ptr = 0;
    if(size>0)
    {
        ptr = malloc(size);
        //ptr = _aligned_malloc(size,alignment);
        Assert(ptr);
    }
    return ptr;
}

void* MemoryManagerStd::ReAllocAligned(void* ptr, const size_t size, const size_t alignment )
{
//	Assert(!(size&(alignment-1)));
	void* rptr = 0;
	if( ptr )
	{
		if( !size )
		{
			//_aligned_free(ptr);
			free(ptr);
			rptr = 0;
		}
		else
		{
			//rptr = _aligned_realloc(ptr,size,alignment);
			rptr = realloc(ptr,size);
			Assert(rptr);
		}
	}
	else if( size )
	{
		//rptr = _aligned_malloc(size,alignment);
		rptr = malloc(size);
		if( !rptr )
		{
			GLog.Print( "Failed aligned malloc - size:%d\n", size );
			GLog.Flush();
		}
		Assert(rptr);
	}
    return rptr;
}

void  MemoryManagerStd::FreeAligned(void* ptr)
{
    if(ptr)
        //::_aligned_free(ptr);
        ::free(ptr);
}
*/

/*
#include <malloc.h> // _msize

MemoryManager::MemoryManager()
:   TotalAlloc(0), TotalFreed(0), PeakAlloc(0)
,   f(0)
{
    f=fopen("mem.log","w");
}

MemoryManager::~MemoryManager()
{
    fprintf(f,"TotalAlloc: %04d (%.2fM)\n",TotalAlloc,float(TotalAlloc)/(1024.f*1024.f));
    fprintf(f,"TotalFreed: %04d (%.2fM)\n",TotalFreed,float(TotalFreed)/(1024.f*1024.f));
    fclose(f);
}

void* MemoryManager::ReAlloc(void* ptr, const size_t size)
{   
    const int sz= (ptr!=NULL) ? _msize(ptr) : 0;
    fprintf(f,"GMem(%08x,%04d)[%04d]: ",(ulong32)ptr,size,sz);
    if(NULL==ptr && 0==size) {
        fprintf(f,"NULL\n");
        return NULL;
    }

    if(NULL!=ptr && size>0) {
        void* p=realloc(ptr,size);
        fprintf(f,"realloc=%08x\n",(ulong32)p);
        TotalAlloc+=size-sz;
        return p;
    }
    else
    if(NULL==ptr && size>0) {
        void* p=malloc(size);
        memset(p,0,size);
        fprintf(f,"malloc=%08x\n",(ulong32)p);
        TotalAlloc+=size;
        return p;
    }
    else
    if(NULL!=ptr && 0==size) {
        fprintf(f,"freeing...");
        free(ptr);
        fprintf(f,"OK\n");
        TotalFreed+=sz;
    }

    return NULL;
}
*/
