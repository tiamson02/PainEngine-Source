// ---------------------------------------------------------------------------------------------------------------------------------
//                                     _     
//                                    | |    
//  _ __ ___  _ __ ___   __ _ _ __    | |__  
// | '_ ` _ \| '_ ` _ \ / _` | '__|   | '_ \ 
// | | | | | | | | | | | (_| | |    _ | | | |
// |_| |_| |_|_| |_| |_|\__, |_|   (_)|_| |_|
//                       __/ |               
//                      |___/                
//
// Memory manager & tracking software
//
// Best viewed with 8-character tabs and (at least) 132 columns
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/22/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#ifndef	_H_MMGR
#define	_H_MMGR

#ifdef __cplusplus
#include "Core/TypeTraits.h"
#endif

// ---------------------------------------------------------------------------------------------------------------------------------
// For systems that don't have the __FUNCTION__ variable, we can just define it here
// ---------------------------------------------------------------------------------------------------------------------------------

/*#ifndef __FUNCTION__
#pragma warning(disable: 4117)
#define	__FUNCTION__ "??"
#pragma warning(default: 4117)
#endif//*/

// ---------------------------------------------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef MEMORY_TEST
typedef enum{
    MEM_SYSTEM,
    MEM_MENU,
    MEM_LEVEL,

    MEM_COUNT,
} MemorySection;

typedef	struct tag_au
{
	size_t		    actualSize;
	size_t		    reportedSize;
	void *          actualAddress;
	void *          reportedAddress;
	/*char		    sourceFile[40];
	char		    sourceFunc[40];//*/
    const char *    sourceFile;
    const char *    sourceFunc;
    const char *    sourceTag;
	unsigned int	sourceLine;
	unsigned int	allocationType;
	/*bool		    breakOnDealloc;
	bool		    breakOnRealloc;//*/
	char            inGame;
	unsigned int	allocationNumber;
	struct tag_au * next;
	struct tag_au * prev;
} sAllocUnit;

typedef	struct
{
	unsigned int	totalReportedMemory;
	unsigned int	totalActualMemory;
	unsigned int	peakReportedMemory;
	unsigned int	peakActualMemory;
	unsigned int	accumulatedReportedMemory;
	unsigned int	accumulatedActualMemory;
	unsigned int	accumulatedAllocUnitCount;
	unsigned int	totalAllocUnitCount;
	unsigned int	peakAllocUnitCount;
} sMStats;
#endif  //MEMORY_TEST



// ---------------------------------------------------------------------------------------------------------------------------------
// External constants
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef MEMORY_TEST
extern  ENGINE_API const	    unsigned int	m_alloc_unknown;
extern	ENGINE_API const	    unsigned int	m_alloc_new;
extern	ENGINE_API const	    unsigned int	m_alloc_new_array;
extern	"C" ENGINE_API const	unsigned int	m_alloc_malloc;
extern	"C" ENGINE_API const	unsigned int	m_alloc_memalign;
extern	"C" ENGINE_API const	unsigned int	m_alloc_calloc;
extern	"C" ENGINE_API const	unsigned int	m_alloc_realloc;
extern	ENGINE_API const	    unsigned int	m_alloc_delete;
extern	ENGINE_API const	    unsigned int	m_alloc_delete_array;
extern	"C" ENGINE_API const	unsigned int	m_alloc_free;
#endif  //MEMORY_TEST
// ---------------------------------------------------------------------------------------------------------------------------------
// Used by the macros
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef MEMORY_TEST
void ENGINE_API m_setOwner(const char *file, const unsigned int line, const char *func);

# if defined(DEBUG) || defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
#  define m_setTag(name) mmgr_setTag(name)
#  define m_SetInGame(ing) mmgr_SetInGame(ing)
# else
#  define m_setTag(name) {}
#  define m_SetInGame(ing) {}
# endif

extern "C" void mmgr_setTag(const char *tag);
extern "C" void mmgr_SetInGame(MemorySection section);

#else  //MEMORY_TEST
# define m_setOwner(file, line, func)
# define m_setTag(name)
# define m_SetInGame(ing)
#endif  //MEMORY_TEST

// ---------------------------------------------------------------------------------------------------------------------------------
// Allocation breakpoints
// ---------------------------------------------------------------------------------------------------------------------------------

/*#ifdef MEMORY_TEST
bool		&m_breakOnRealloc(void *reportedAddress);
bool		&m_breakOnDealloc(void *reportedAddress);
#else  //MEMORY_TEST
# define    m_breakOnRealloc(reportedAddress)
# define    m_breakOnDealloc(reportedAddress)
#endif  //MEMORY_TEST //*/

// ---------------------------------------------------------------------------------------------------------------------------------
// The meat of the memory tracking software
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef MEMORY_TEST
extern "C" void ENGINE_API *    m_allocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                const unsigned int allocationType, const size_t reportedSize, const size_t align = 0);
extern "C" void ENGINE_API *    m_reallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                  const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress);
extern "C" void ENGINE_API      m_deallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                  const unsigned int deallocationType, const void *reportedAddress);

//special allocators
extern "C" void ENGINE_API *    m_scriptReallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc, 
                                                    const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress);
void *m_havokAllocator(size_t reportedSize);
void *m_havokReallocator(void *reportedAddress, size_t reportedSize);

extern "C" void *               m_gimAllocator(size_t reportedSize);
extern "C" void                 m_gimDeallocator(void *reportedAddress);

extern "C" void ENGINE_API *    m_physfsAllocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                      const unsigned int allocationType, const size_t reportedSize);
extern "C" void ENGINE_API *    m_physfsReallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                        const unsigned int reallocationType, const size_t reportedSize, void *reportedAddress);
extern "C" void ENGINE_API      m_physfsDeallocator(const char *sourceFile, const unsigned int sourceLine, const char *sourceFunc,
			                                        const unsigned int deallocationType, const void *reportedAddress);


#else   //MEMORY_TEST
//these should generate compile errors if they are used explicitly outside of this file
# define m_allocator(sourceFile, sourceLine, sourceFunc, allocationType, reportedSize)                              bad_function_m_allocator
# define m_reallocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize, reportedAddress)         bad_function_m_reallocator
# define m_deallocator(sourceFile, sourceLine, sourceFunc, deallocationType, reportedAddress)                       bad_function_m_deallocator
# define m_scriptReallocator(sourceFile, sourceLine, sourceFunc, reallocationType, reportedSize, reportedAddress)   bad_function_m_scriptReallocator
# define m_havokAllocator(reportedSize)                                                                             bad_function_m_havokAllocator
# define m_havokReallocator(reportedAddress, reportedSize)                                                          bad_function_m_havokReallocator
# define m_gimAllocator(size)                                                                                       bad_function_m_gimAllocator
# define m_gimDeallocator(addr)                                                                                     bad_function_m_gimDeallocator
#endif  //MEMORY_TEST

// ---------------------------------------------------------------------------------------------------------------------------------
// Utilitarian functions
// ---------------------------------------------------------------------------------------------------------------------------------
//uncomment this to enable block validation during allocations and deallocations
//#define ENABLE_VALIDATION

#ifdef MEMORY_TEST
# ifdef ENABLE_VALIDATION
bool		m_validateAddress(const void *reportedAddress);
bool		m_validateAllocUnit(const sAllocUnit *allocUnit);
bool		m_validateAllAllocUnits();

bool        m_getValidateState();
void        m_setValidateState(bool state);
bool        m_getWipeState();
void        m_setWipeState(bool state);
# else
#  define   m_validateAddress(reportedAddress)  {}
#  define   m_validateAllocUnit(allocUnit)      {}
#  define   m_validateAllAllocUnits()           {}
#  define   m_getValidateState()                {}
#  define   m_setValidateState(state)           {}
#  define   m_getWipeState()                    {}
#  define   m_setWipeState(state)               {}
# endif

void        m_setLoggingState(bool state);
bool        m_getLoggingState();

void        m_logMsg(const char *fmt, ...);
#else   //MEMORY_TEST
# define    m_validateAddress(reportedAddress)  {}
# define    m_validateAllocUnit(allocUnit)      {}
# define    m_validateAllAllocUnits()           {}
# define    m_getValidateState()                {}
# define    m_setValidateState(state)           {}
# define    m_getWipeState()                    {}
# define    m_setWipeState(state)               {}

# define    m_setLoggingState(state)
# define    m_getLoggingState()         0

#ifdef TRANSGAMING_WINDOWS
# define    m_logMsg __noop
#else
# define    m_logMsg(fmt, args...) {}
#endif // TRANSGAMING_WINDOWS

#endif  //MEMORY_TEST

// ---------------------------------------------------------------------------------------------------------------------------------
// Unused RAM calculations
// ---------------------------------------------------------------------------------------------------------------------------------

/*#ifdef MEMORY_TEST
unsigned int	m_calcUnused(const sAllocUnit *allocUnit);
unsigned int	m_calcAllUnused();
#else   //MEMORY_TEST
# define        m_calcUnused(allocUnit)
# define        m_calcAllUnused()
#endif  //MEMORY_TEST//*/

// ---------------------------------------------------------------------------------------------------------------------------------
// Logging and reporting
// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef MEMORY_TEST
# ifdef ENABLE_VALIDATION
void		m_dumpAllocUnit(const sAllocUnit *allocUnit, const char *prefix = "");
# endif
void		m_dumpMemoryReport(const char *filename, const bool dumpAllocs, const bool humanReadable);
//sMStats		m_getMemoryStatistics();
#else   //MEMORY_TEST
# define    m_dumpAllocUnit(allocUnit, prefix)  {}
# define    m_dumpMemoryReport(filename, dumpAllocs, humanReadable) {}
//# define    m_getMemoryStatistics()
#endif  //MEMORY_TEST

float       m_getModuleSize();
size_t      m_getMemUsed();

// ---------------------------------------------------------------------------------------------------------------------------------
// Variations of global operators new & delete
// ---------------------------------------------------------------------------------------------------------------------------------


#ifdef MEMORY_TEST
	void	*operator new(size_t reportedSize);
	void	*operator new[](size_t reportedSize);
	void	*operator new(size_t reportedSize, const char *sourceFile, int sourceLine);
	void	*operator new[](size_t reportedSize, const char *sourceFile, int sourceLine);
	void	operator delete(void *reportedAddress);
	void	operator delete[](void *reportedAddress);
#endif  //MEMORY_TEST

#endif // _H_MMGR

#ifdef MEMORY_TEST
// ---------------------------------------------------------------------------------------------------------------------------------
// Macros -- "Kids, please don't try this at home. We're trained professionals here." :)
// ---------------------------------------------------------------------------------------------------------------------------------
# define    new		        (m_setOwner  (__FILE__, __LINE__, __FUNCTION__), false) ? NULL : new
# define    delete	        (m_setOwner  (__FILE__, __LINE__, __FUNCTION__), false) ? m_setOwner("", 0, "") : delete
# define    malloc(sz)	    m_allocator  (__FILE__, __LINE__, __FUNCTION__, m_alloc_malloc, sz)
# define    calloc(sz)	    m_allocator  (__FILE__, __LINE__, __FUNCTION__, m_alloc_calloc, sz)
# define    realloc(ptr,sz)	m_reallocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_realloc, sz, ptr)
# define    free(ptr)	    m_deallocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_free, ptr)
# define    memalign(a, sz) m_allocator  (__FILE__, __LINE__, __FUNCTION__, m_alloc_memalign, sz, a)
#else

	// in engine.dll allocators
/*extern void ENGINE_API * m_malloc(size_t size);
extern void ENGINE_API * m_calloc(size_t size);
extern void ENGINE_API * m_realloc(void *ptr, size_t size);
extern void ENGINE_API   m_free(void *ptr);

extern void ENGINE_API * m_malloc_a16(size_t size);
extern void ENGINE_API * m_realloc_a16(void *ptr, size_t size);
extern void ENGINE_API   m_free_a16(void *ptr);//*/

#endif


// ---------------------------------------------------------------------------------------------------------------------------------
// mmgr.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
