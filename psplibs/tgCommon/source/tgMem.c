/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMem.c
*   Memory manager
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      rubens      initial implementation
*/


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------
#include <malloc.h>

#include "tgMem.h"


#if		(defined(TGMEM_ALIGNOF) || defined(TGMEM_ASSERT) || defined(TGMEM_CASSERT) || defined(TGMEM_CASSERT_MSG) || defined(TGMEM_ALIGNOF_CHUNKHEADER) || defined(TGMEM_ALIGNOF_CHUNKPOOL) || defined(TGMEM_ALIGNOF_HEAPMANAGER) || defined(TGMEM_ALIGNOF_POOLMANAGER) || defined(TGMEM_POOL_NB_FREE_LISTS) || defined(TGMEM_INLINE) || defined(TGMEM_FALSE) || defined(TGMEM_TRUE))
	#error Impossible to compile tgMem.c (one of the macro is already defined)
#endif

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S (to be moved to TG?)
//-----------------------------------------------------------------------------

#if		defined(__cplusplus)
#define TGMEM_INLINE							inline
#define TGMEM_FALSE								false
#define TGMEM_TRUE								true
#else	// defined(__cplusplus)
#define TGMEM_INLINE							__inline
#define TGMEM_FALSE								0
#define TGMEM_TRUE								(~TGMEM_FALSE)
#endif	// defined(__cplusplus)

#if		defined(__cplusplus)
typedef bool				tgMemBool;
#else	// defined(__cplusplus)
typedef s32					tgMemBool;
#endif	// defined(__cplusplus)

#ifdef	TGMEM_DEBUG
	#include <libemu.h>
	#include <stdio.h>

//	#define TGMEM_OUTPUT_TEXT					sceEmuPrintf
	#define TGMEM_OUTPUT_TEXT(...)
	#define TGMEM_BREAKPOINT				{	while(0) {};	}
	//#define TGMEM_OUTPUT_TEXT	printf
	#define TGMEM_ASSERT()																		\
	{																							\
		TGMEM_OUTPUT_TEXT( "TGMEM_ASSERT( file=%s, line=%d )\n", __FILE__, __LINE__ );			\
		TGMEM_BREAKPOINT;																		\
	}
	#define TGMEM_CASSERT(c)																	\
	{																							\
		if	(!(c))																				\
		{																						\
			TGMEM_OUTPUT_TEXT( "TGMEM_CASSERT_MSG( file=%s, line=%d )\n", __FILE__, __LINE__ );	\
			TGMEM_OUTPUT_TEXT( "    condition: %s\n", #c );										\
			TGMEM_BREAKPOINT;																	\
		}																						\
	}
	#define TGMEM_CASSERT_MSG( c, msg, args... )												\
	{																							\
		if	(!(c))																				\
		{																						\
			TGMEM_OUTPUT_TEXT( "TGMEM_CASSERT_MSG( file=%s, line=%d )\n", __FILE__, __LINE__ );	\
			TGMEM_OUTPUT_TEXT( "    condition: %s\n", #c );										\
			TGMEM_OUTPUT_TEXT( "    message: " );												\
			TGMEM_OUTPUT_TEXT( msg, ##args );													\
			TGMEM_OUTPUT_TEXT( "\n" );															\
			TGMEM_BREAKPOINT;																	\
		}																						\
	}

#else	// TGMEM_DEBUG
	#define TGMEM_VOID()
	#define TGMEM_OUTPUT_TEXT(args...)			((void) 0)
	#define TGMEM_BREAKPOINT					((void) 0)
	#define TGMEM_ASSERT()						((void) 0)
	#define TGMEM_CASSERT(c)					((void) 0)
	#define TGMEM_CASSERT_MSG( c, msg, args )	((void) 0)

#endif	// TGMEM_DEBUG

#define TGMEM_DEBUG_OUTPUT_TEXT(...)			((void) 0)
//#define TGMEM_DEBUG_OUTPUT_TEXT					TGMEM_OUTPUT_TEXT


//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------
#ifdef __MWERKS__
	#define TGMEM_ALIGNOF						__alignof__
#else
	#define TGMEM_ALIGNOF						__alignof
#endif

/*#ifdef	TG_DEBUG
#define TGMEM_ASSERT()						TG_ASSERT()
#define TGMEM_CASSERT(c)					TG_CASSERT(c)
#define TGMEM_CASSERT_MSG( c, msg, args )	TG_CASSERT_MSG( c, msg, args )
#else	// TG_DEBUG
#define TGMEM_ASSERT()
#define TGMEM_CASSERT(c)
#define TGMEM_CASSERT_MSG( c, msg, args )
#endif	// TG_DEBUG*/

#define TGMEM_ALIGNOF_CHUNKHEADER			TGMEM_ALIGNOF(TgMemChunkHeader)
#define TGMEM_ALIGNOF_CHUNKPOOL				TGMEM_ALIGNOF(TgMemChunkPool)
#define TGMEM_ALIGNOF_HEAPMANAGER			TGMEM_ALIGNOF(TgMemHeapManager)
#define TGMEM_ALIGNOF_POOLMANAGER			TGMEM_ALIGNOF(TgMemPoolManager)
#define TGMEM_POOL_NB_FREE_LISTS			(sizeof(TgMemSizeT)*8)


//-----------------------------------------------------------------------------
// S T R U C T U R E S
//-----------------------------------------------------------------------------
typedef enum
{
	TG_MEM_MANAGER_BASE_TYPE =	0,
	TG_MEM_MANAGER_BLOC_TYPE =	1,
	TG_MEM_MANAGER_HEAP_TYPE =	2,
	TG_MEM_MANAGER_POOL_TYPE =	3
}	ETgMemManagerType;


typedef struct _TgMemManagerVtbl
{
	ETgMemManagerType		m_rtti;
	void*					(*m_pMalloc)(	const TgMemSizeT	size );
	void					(*m_pFree)(		void* const			pMemToFree );
}	TgMemManagerVtbl;

struct _TgMemManager
{
	TgMemSizeT				m_alignment;
	TgMemSizeT				m_memorySize;												// Size of managed memory
	void*					m_pMemory;													// Link to managed memory
	const TgMemManagerVtbl*	m_pVtbl;
	TgMemManager*			m_pFather;													// Links between managers
	TgMemManager*			m_pFirstChild;
	TgMemManager*			m_pPrevSibling;
	TgMemManager*			m_pNextSibling;
};

typedef struct _TgMemChunkHeader
{
	TgMemSizeT				m_sizeOfPrevChunkInMem;
	TgMemSizeT				m_sizeOfCurrChunkInMem;
	#ifdef	TGMEM_DEBUG_INFOS
	void*					m_pDebugInfos;
	void					(*m_pDebugCallback)(void);
	TgMemSizeT				m_justToTest;
	#endif	// TGMEM_DEBUG_INFOS
}	TgMemChunkHeader;

typedef struct _TgMemChunkFree
{
	TgMemChunkHeader		m_chunkHeader;
	TgMemSizeT				m_prevChunkInFreeList;
	TgMemSizeT				m_nextChunkInFreeList;
}	TgMemChunkFree;

typedef struct _TgMemChunkPool
{
	TgMemSizeT				m_sizeOfPrevPoolInMem;
	TgMemSizeT				m_sizeOfCurrPoolInMem;
	#ifdef	TGMEM_DEBUG_INFOS
	void*					m_pDebugInfos;
	void					(*m_pDebugCallback)(void);
	TgMemSizeT				m_justToTest;
	#endif	// TGMEM_DEBUG_INFOS
	struct _TgMemChunkPool*	m_pPrevPoolInFreeList;
	struct _TgMemChunkPool*	m_pNextPoolInFreeList;
	tgMemBool				m_isAFreeChunk;
	TgMemSizeT				m_numChunk;
}	TgMemChunkPool;


// Bloc
typedef struct _TgMemBlocManager
{
	TgMemManager			m_manager;
	TgMemSizeT				m_firstFreeBlocAddr;
}	TgMemBlocManager;

// Heap
typedef struct _TgMemHeapManager
{
	TgMemManager			m_manager;
	ETgMemHeapDirection		m_direction;
	TgMemSizeT				m_memoryStartAddr;
	TgMemSizeT				m_memoryEndAddr;
	TgMemSizeT				m_currStartAddr;
	TgMemSizeT				m_currEndAddr;
	#ifdef	TGMEM_DEBUG
	TgMemSizeT				m_firstStartAddr;
	#endif	// TGMEM_DEBUG
}	TgMemHeapManager;

// Pool
typedef struct _TgMemPoolManager
{
	TgMemManager			m_manager;
	TgMemSizeT				m_size;
	TgMemSizeT				m_num;
	TgMemSizeT				m_realSize;
	TgMemSizeT				m_memoryAddr;
	TgMemChunkPool*			m_pChunkPool;
	TgMemChunkPool*			m_pFirstFree[TGMEM_POOL_NB_FREE_LISTS];
}	TgMemPoolManager;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------
static void*	_TgMemMalloc(		const TgMemSizeT	size );
static void		_TgMemFree(			void* const			pMemToFree );
static void*	_TgMemPoolMalloc(	const TgMemSizeT	size );


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S
//-----------------------------------------------------------------------------
static const TgMemManagerVtbl	_tgMemManagerVirtualTable =
{
	TG_MEM_MANAGER_BASE_TYPE,
	&_TgMemMalloc,
	&_TgMemFree
};

/*static const TgMemManagerVtbl	_tgMemBlocVirtualTable =
{
	TG_MEM_MANAGER_BLOC_TYPE,
	TGMEM_NULL,				//	&tgMemBloc_Malloc,
	TGMEM_NULL				//	&tgMemBloc_Free
};*/

static const TgMemManagerVtbl	_tgMemHeapVirtualTable =
{
	TG_MEM_MANAGER_HEAP_TYPE,
	&tgMemHeapMalloc,
	&tgMemHeapFree
};

static const TgMemManagerVtbl	_tgMemPoolVirtualTable =
{
	TG_MEM_MANAGER_POOL_TYPE,
	&_TgMemPoolMalloc,
	&tgMemPoolFree
};

static TgMemManager				_tgMemDefaultManager =
{
	16,																				// m_alignment
	0,																				// m_memorySize
	TGMEM_NULL,																		// m_pMemory
	&_tgMemManagerVirtualTable,														// m_pVtbl
	TGMEM_NULL,																		// m_pFather
	TGMEM_NULL,																		// m_pFirstChild
	TGMEM_NULL,																		// m_pPrevSibling
	TGMEM_NULL																		// m_pNextSibling;
};

static TgMemManager*			_pBindedManager				=	&_tgMemDefaultManager;
//static void*					_pDebugInfos				=	TGMEM_NULL;
//static void						(*_pDebugCallback)(void)	=	TGMEM_NULL;;


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _TgMemComputeChunkHeaderAddr
// Description: returns the chunk header address (alignment)
// Parameters: 	address of the memory block
// Returns: address of the chunk header
// Note: 
//-----------------------------------------------------------------------------
//static TGMEM_INLINE TgMemSizeT			_TgMemComputeChunkHeaderAddr(	const TgMemSizeT			addr )
//{
//	return	((addr - sizeof(TgMemChunkHeader)) & (~(TGMEM_ALIGNOF_CHUNKHEADER-1)));
//}


//-----------------------------------------------------------------------------
// _TgMemGetPrevChunkHeader
// Description: returns the address of the previous chunk header
// Parameters: 	chunk header address
// Returns: previous chunk header address
// Note: 
//-----------------------------------------------------------------------------
//static TGMEM_INLINE TgMemChunkHeader*	_TgMemGetPrevChunkHeader(		TgMemChunkHeader* const		pChunkHeader )
//{
//	return	(TgMemChunkHeader*) (((TgMemSizeT)pChunkHeader) - (pChunkHeader->m_sizeOfPrevChunkInMem & ~1));
//}


//-----------------------------------------------------------------------------
// _TgMemGetNextChunkHeader
// Description: returns the address of the next chunk header
// Parameters: 	chunk header address
// Returns: next chunk header address
// Note: 
//-----------------------------------------------------------------------------
//static TGMEM_INLINE TgMemChunkHeader*	_TgMemGetNextChunkHeader(		TgMemChunkHeader* const		pChunkHeader )
//{
//	return	(TgMemChunkHeader*) (((TgMemSizeT)pChunkHeader) + (pChunkHeader->m_sizeOfCurrChunkInMem & ~1));
//}


//-----------------------------------------------------------------------------
// _TgMemVerifyChunk
// Description: checks if a memory chunk is valid (only in DEBUG mode)
// Parameters: 	chunk header address
// Returns: true or false
// Note: 
//-----------------------------------------------------------------------------
#ifdef	TGMEM_DEBUG
static TGMEM_INLINE tgMemBool			_TgMemVerifyChunk(				TgMemChunkHeader* const		pChunkHeader )
{
	if	(pChunkHeader->m_sizeOfPrevChunkInMem == 0)
		return	TGMEM_TRUE;
	if	(pChunkHeader->m_sizeOfPrevChunkInMem < sizeof(TgMemChunkHeader))
		return	TGMEM_FALSE;
	return	(pChunkHeader->m_sizeOfPrevChunkInMem == _TgMemGetPrevChunkHeader(pChunkHeader)->m_sizeOfCurrChunkInMem);
}


//-----------------------------------------------------------------------------
// _TgMemIsAManager
// Description: checks if a pointer to a manager is pointing to a memory manager
// Parameters: 	manager address
// Returns: true or false
// Note: 
//-----------------------------------------------------------------------------
static TGMEM_INLINE tgMemBool			_TgMemIsAManager(				const TgMemManager*			pManager )
{
	if	(pManager == TGMEM_NULL)
		return	TGMEM_FALSE;

	if	(	(pManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_BASE_TYPE)
		||	(pManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_BLOC_TYPE)
		||	(pManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE)
		||	(pManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_POOL_TYPE) )
		return	TGMEM_TRUE;

	return	TGMEM_FALSE;
}
#else	// TGMEM_DEBUG
#define _TgMemVerifyChunk(x)
#define _TgMemIsAManager(x)		TGMEM_TRUE
#endif	// TGMEM_DEBUG


//-----------------------------------------------------------------------------
// _TgMemStartAlignPow2
// Description: returns an address aligned to a power of two (floor or start)
// Parameters: 	manager address and alignment
// Returns: aligned address
// Note: 
//-----------------------------------------------------------------------------
static TGMEM_INLINE TgMemSizeT	_TgMemStartAlignPow2(	const TgMemSizeT	addr,
														const TgMemSizeT	alignment )
{
	return	(addr & ~(alignment - 1));
}


//-----------------------------------------------------------------------------
// _TgMemEndAlignPow2
// Description: returns an address aligned to a power of two (ceil or end)
// Parameters: 	manager address and alignment
// Returns: aligned address
// Note: 
//-----------------------------------------------------------------------------
static TGMEM_INLINE TgMemSizeT	_TgMemEndAlignPow2(		const TgMemSizeT	addr,
														const TgMemSizeT	alignment )
{
	return	((addr + alignment -1) & ~(alignment -1));
}


//-----------------------------------------------------------------------------
// _TgMemMalloc
// Description: returns a pointer to a memory block allocated with malloc
// Parameters: 	size of the block to be allocated
// Returns: pointer the the block
// Note: 
//-----------------------------------------------------------------------------
static void*				_TgMemMalloc(			const TgMemSizeT	size )
{
	return	(void*) malloc((size_t)size);
}


//-----------------------------------------------------------------------------
// _TgMemFree
// Description: free a memory block allocated with free
// Parameters: 	pointer the the block
// Returns: nothing
// Note: 
//-----------------------------------------------------------------------------
static void					_TgMemFree(				void* const			pMemToFree )
{
	free((void*)pMemToFree);
}


//-----------------------------------------------------------------------------
// _TgMemCreateManager
// Description: create a memory manager
// Parameters: 	alignment for the memory manager, manager size, memory size the manager can support
// Returns: pointer to the manager
// Note: 
//-----------------------------------------------------------------------------
static TgMemManager*		_TgMemCreateManager(	const TgMemSizeT	managerAlignment,
													const TgMemSizeT	managerSize,
													const TgMemSizeT	memorySize )
{
	register TgMemManager*	pManager;
	register TgMemManager*	pNextSibling;
	register TgMemSizeT		memoryAlignment;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));

	memoryAlignment =	tgMemGetAlignment();

	// Allocate manager
	tgMemSetAlignment(managerAlignment);
	pManager =	(TgMemManager*) _pBindedManager->m_pVtbl->m_pMalloc(managerSize);
//	TgMemManager	space1;
//	pManager =	(TgMemManager*) &space1;
	if	(pManager == TGMEM_NULL)
		return	TGMEM_NULL;

	// Allocate used memory in previous memory manager
	tgMemSetAlignment(memoryAlignment);
	pManager->m_pMemory =	(TgMemManager*) _pBindedManager->m_pVtbl->m_pMalloc(memorySize);
//	u128	space2[1*1024*1024/16];
//	pManager->m_pMemory =	(void*) &space2[0];
	if	(pManager->m_pMemory == TGMEM_NULL)
	{
		_pBindedManager->m_pVtbl->m_pFree(pManager->m_pMemory);
		return	TGMEM_NULL;
	}


	// Link managers together	(_pBindedManager is always != TGMEM_NULL)
	pNextSibling						=	_pBindedManager->m_pFirstChild;
	_pBindedManager->m_pFirstChild		=	pManager;

	pManager->m_alignment				=	memoryAlignment;
	pManager->m_memorySize				=	memorySize;
//	pManager->m_pMemory						is set;
//	pManager->m_pVtbl						will be set;
	pManager->m_pFather					=	_pBindedManager;
	pManager->m_pFirstChild				=	TGMEM_NULL;
	pManager->m_pPrevSibling			=	TGMEM_NULL;
	pManager->m_pNextSibling			=	pNextSibling;
	if	(pNextSibling != TGMEM_NULL)
		pNextSibling->m_pPrevSibling	=	pManager;

	return	pManager;
}


//-----------------------------------------------------------------------------
//G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgMemBindManager
// Description: bind a manager for allocation
// Parameters: 	pManager - pointer to the manager
// Returns: nothing
// Note: the original supported abstract classes for managers (malloc, free virtual
//       fucntions), and has been removed due to the obligation to use strict C.
//       This function made more sense with the C++ library.
//-----------------------------------------------------------------------------
void				tgMemBindManager(			TgMemManager* const 		pManager )
{
	if	(pManager == TGMEM_NULL)
		_pBindedManager	=	&_tgMemDefaultManager;
	else
	{
		TGMEM_CASSERT(_TgMemIsAManager(pManager));
		_pBindedManager	=	pManager;
	}
}


//-----------------------------------------------------------------------------
// tgMemDestroy
// Description: destroy all sub manager of the manager and the memory manager itself
// Parameters: 	pManager - pointer to the manager
// Returns: nothing
// Note: it is possible to have a hierarchy of manager by allocating a manager in another manager
//-----------------------------------------------------------------------------
void				tgMemDestroy(				TgMemManager* const			pManager )
{
	TGMEM_CASSERT(_TgMemIsAManager(pManager));
	TGMEM_CASSERT(pManager != (TgMemManager*)&_tgMemDefaultManager);			// This is not the default manager
	TGMEM_CASSERT(pManager != _pBindedManager);									// This is not the binded manager
	TGMEM_CASSERT(pManager->m_pFather != TGMEM_NULL);							// This is not the default manager
	TGMEM_CASSERT(pManager->m_pFather->m_pVtbl != TGMEM_NULL);
	TGMEM_CASSERT(pManager->m_pFather->m_pVtbl->m_pFree!= TGMEM_NULL);

	// Remove manager from the manager hierarchy
	if	(pManager->m_pFather->m_pFirstChild == pManager)
	{
		pManager->m_pFather->m_pFirstChild					=	pManager->m_pNextSibling;
		if	(pManager->m_pNextSibling != TGMEM_NULL)
			pManager->m_pNextSibling->m_pPrevSibling		=	TGMEM_NULL;
	}
	else
	{
			if	(pManager->m_pPrevSibling != TGMEM_NULL)
				pManager->m_pPrevSibling->m_pNextSibling	=	pManager->m_pNextSibling;
			if	(pManager->m_pNextSibling != TGMEM_NULL)
				pManager->m_pNextSibling->m_pPrevSibling	=	pManager->m_pPrevSibling;
	}

	pManager->m_pFather->m_pVtbl->m_pFree(pManager->m_pMemory);
	pManager->m_pFather->m_pVtbl->m_pFree(pManager);
}


//-----------------------------------------------------------------------------
// tgMemGetAlignment
// Description: gets the alignment for the binded manager
// Parameters: 	nothing
// Returns: alignment
// Note:
//-----------------------------------------------------------------------------
TgMemSizeT			tgMemGetAlignment(			void )
{
	TGMEM_CASSERT(_pBindedManager != TGMEM_NULL);
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));

	return	_pBindedManager->m_alignment;
}


//-----------------------------------------------------------------------------
// tgMemSetAlignment
// Description: sets the alignment for the binded manager
// Parameters: alignment -	alignment
// Returns: nothing
// Note:
//-----------------------------------------------------------------------------
void				tgMemSetAlignment(			const TgMemSizeT			alignment )
{
	register int			size;
	register TgMemSizeT		temp;

	if	(_pBindedManager == &_tgMemDefaultManager)
		return;

	TGMEM_CASSERT(_pBindedManager != TGMEM_NULL);
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(alignment >= 0);

	size =	8*sizeof(TgMemSizeT);
	temp =	alignment;

	// shift until we find a bit
	while	(size > 0)
	{
		if	((temp & 1) != 0)
			break;
		temp >>= 1;
		size--;
	}

	// alignement must be a power of 2 (there is just one bit set in the alignement value)
	TGMEM_CASSERT((temp & ~1) == 0);

	_pBindedManager->m_alignment	=	alignment;
}


//-----------------------------------------------------------------------------
//G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------
//BlocManager

#ifdef	CompileBloc
//-----------------------------------------------------------------------------
// tgMemBloc_Malloc
// Description: NOT USED!
// Parameters: 	NOT USED!
// Returns: NOT USED!
// Note:
//-----------------------------------------------------------------------------
void*				tgMemBloc_Malloc(			const TgMemSizeT			size )
{
	TGMEM_CASSERT(_pBindedManager != NULL);
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_BLOC_TYPE);

	return	tgMemBlocCalloc( 1, size );
}


//-----------------------------------------------------------------------------
// tgMemBloc_Calloc
// Description: NOT USED!
// Parameters: 	NOT USED!
// Returns: NOT USED!
// Note:
//-----------------------------------------------------------------------------
void*				tgMemBloc_Calloc(			const TgMemSizeT			num,
												const TgMemSizeT			size )
{
	register TgMemManager*		pManager;
	register TgMemBlocManager*	pBlocManager;
	register TgMemSizeT			realSize;

	TGMEM_CASSERT(_pBindedManager != TGMEM_NULL);
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pManager		=	(TgMemManager*) _pBindedManager;
	pBlocManager	=	(TgMemBlocManager*) _pBindedManager;
	realSize		=	(num == 1) ? size : num * _TgMemEndAlignPow2( size, pManager->m_alignment );

	return	TGMEM_NULL;
}


//-----------------------------------------------------------------------------
// tgMemBloc_Free
// Description: NOT USED!
// Parameters: 	NOT USED!
// Returns: NOT USED!
// Note:
//-----------------------------------------------------------------------------
void				tgMemBloc_Free(				void* const					pMemToFree )
{
	static void*	s_pMemToFree;

	s_pMemToFree =	pMemToFree;
}


//-----------------------------------------------------------------------------
// tgMemBloc_FreeAll
// Description: NOT USED!
// Parameters: 	NOT USED!
// Returns: NOT USED!
// Note:
//-----------------------------------------------------------------------------
void				tgMemBloc_FreeAll(			void )
{
}
#endif	// CompileBloc


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------
// HeapManager

//-----------------------------------------------------------------------------
// tgMemHeapCreate
// Description: Create a heap manager
// Parameters: 	size - size of allocatable memory
// Returns: address of the manager
// Note:
//-----------------------------------------------------------------------------
TgMemManager*		tgMemHeapCreate(			const TgMemSizeT			size )
{
	register TgMemManager*		pManager;
	register TgMemHeapManager*	pHeapManager;
	register TgMemSizeT			memoryAlignment;
	register TgMemSizeT			memorySize;
	#ifdef	TGMEM_DEBUG
	register TgMemSizeT			chunkAddr;
	register TgMemChunkHeader*	pChunk;
	#endif	// TGMEM_DEBUG


	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));

	memoryAlignment					=	tgMemGetAlignment();											// Save current alignment
	memorySize						=	sizeof(TgMemChunkHeader) + size;								// Add the size of a (free) chunk header
	memorySize						=	_TgMemEndAlignPow2( memorySize, TGMEM_ALIGNOF_CHUNKHEADER );	// Align the size at the end
	tgMemSetAlignment(TGMEM_ALIGNOF_CHUNKHEADER);														// Align the size at the begining
	pManager						=	(TgMemManager*) _TgMemCreateManager(						// Allocate the manager
																TGMEM_ALIGNOF_HEAPMANAGER,
																sizeof(TgMemHeapManager),
																memorySize );
	tgMemSetAlignment(memoryAlignment);																// Restore current alignment
	if	(pManager == TGMEM_NULL)
		return	TGMEM_NULL;

	pManager->m_pVtbl				=	(TgMemManagerVtbl*) &_tgMemHeapVirtualTable;

	pHeapManager					=	(TgMemHeapManager*) pManager;
	pHeapManager->m_direction		=	TG_MEM_HEAP_DIR_START;
	pHeapManager->m_memoryStartAddr =	(TgMemSizeT) pManager->m_pMemory;
	pHeapManager->m_memoryEndAddr	=	((TgMemSizeT)pManager->m_pMemory) + pManager->m_memorySize;
	pHeapManager->m_currStartAddr	=	pHeapManager->m_memoryStartAddr;
	pHeapManager->m_currEndAddr		=	pHeapManager->m_memoryEndAddr;

	#ifdef	TGMEM_DEBUG
	chunkAddr						=	pHeapManager->m_memoryStartAddr;								// Create the very first free chunck
	pHeapManager->m_firstStartAddr	=	chunkAddr;
	pChunk							=	(TgMemChunkHeader*) chunkAddr;
	pChunk->m_sizeOfPrevChunkInMem	=	0;
	pChunk->m_sizeOfCurrChunkInMem	=	(pHeapManager->m_memoryEndAddr - chunkAddr) | 1;				// Indicate a free chunk
	#endif	// TGMEM_DEBUG

	return	pManager;
}


//-----------------------------------------------------------------------------
// tgMemHeapMalloc
// Description: Allocate a block in a heap
// Parameters: 	size - size of the block
// Returns: address of the block
// Note:
//-----------------------------------------------------------------------------
void*				tgMemHeapMalloc(			const TgMemSizeT			size )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);

	return	tgMemHeapCalloc( 1, size );
}


//-----------------------------------------------------------------------------
// tgMemHeapCalloc
// Description: Allocate n block in a heap (array of blocks)
// Parameters: 	num - number of blocks to be allocated
//				size - size of the blocks
// Returns: address of the first block
// Note:
//-----------------------------------------------------------------------------
void*				tgMemHeapCalloc(			const TgMemSizeT			num,
												const TgMemSizeT			size )
{
	register TgMemManager*		pManager;
	register TgMemHeapManager*	pHeapManager;
	register TgMemSizeT			realSize;
	register TgMemSizeT			startAddr;
	register TgMemSizeT			endAddr;
	#ifdef	TGMEM_DEBUG
	register TgMemSizeT			currChunkAddr;
	register TgMemChunkHeader*	pCurrChunk;
	#endif	// TGMEM_DEBUG

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pManager		=	(TgMemManager*) _pBindedManager;
	pHeapManager	=	(TgMemHeapManager*) _pBindedManager;
	realSize		=	(num == 1) ? size : num * _TgMemEndAlignPow2( size, pManager->m_alignment );

	if	(pHeapManager->m_direction == TG_MEM_HEAP_DIR_START)
	{
		#ifdef	TGMEM_DEBUG
		register TgMemChunkHeader*	pNextChunk;
		#endif	// TGMEM_DEBUG

		#ifdef	TGMEM_DEBUG
		currChunkAddr	=	pHeapManager->m_currStartAddr;
		startAddr		=	currChunkAddr + sizeof(TgMemChunkHeader);							// We add the size of ChunkHeader
		startAddr		=	_TgMemEndAlignPow2( startAddr, pManager->m_alignment );				// We align the start on the user boundary
		endAddr			=	startAddr + realSize;												// We add the realSize of the user datas
		endAddr			=	_TgMemEndAlignPow2( endAddr, TGMEM_ALIGNOF_CHUNKHEADER );			// We align the end to a new ChunkHeader
		currChunkAddr	=	startAddr - sizeof(TgMemChunkHeader);								// We recompute address of ChunkHeader after alignement
		currChunkAddr	=	_TgMemStartAlignPow2( currChunkAddr, TGMEM_ALIGNOF_CHUNKHEADER );	// We realign address of ChunkHeader
		#else	// TGMEM_DEBUG
		startAddr		=	pHeapManager->m_currStartAddr;
		startAddr		=	_TgMemEndAlignPow2( startAddr, pManager->m_alignment );				// We align the start on the user boundary
		endAddr			=	startAddr + realSize;												// We add the realSize of the user datas
		#endif	// TGMEM_DEBUG

		if	(endAddr > pHeapManager->m_currEndAddr)
			return	TGMEM_NULL;

		#ifdef	TGMEM_DEBUG
		pCurrChunk	=	(TgMemChunkHeader*) currChunkAddr;
		if	(pHeapManager->m_currStartAddr == pHeapManager->m_memoryStartAddr)
		{	// If the current free chunk is the first in start memory, we create an (allocation) chunk
			pHeapManager->m_firstStartAddr			=	currChunkAddr;
			pCurrChunk->m_sizeOfPrevChunkInMem		=	0;
		}
		else
		{	// If the current chunk is not the first in start memory, we update the previous chunk
			register TgMemSizeT			prevChunkAddr;
			register TgMemChunkHeader*	pPrevChunk;
			// Previous chunk
			prevChunkAddr							=	pHeapManager->m_currStartAddr;
			pPrevChunk								=	(TgMemChunkHeader*) prevChunkAddr;
			TGMEM_CASSERT((pPrevChunk->m_sizeOfPrevChunkInMem & 1) == 0);
			TGMEM_CASSERT(pPrevChunk->m_sizeOfPrevChunkInMem >= sizeof(TgMemChunkHeader));
			prevChunkAddr							=	prevChunkAddr - pPrevChunk->m_sizeOfPrevChunkInMem;
			pPrevChunk								=	(TgMemChunkHeader*) prevChunkAddr;
			pPrevChunk->m_sizeOfCurrChunkInMem		=	currChunkAddr - prevChunkAddr;
			// Current chunk
			pCurrChunk->m_sizeOfPrevChunkInMem		=	currChunkAddr - prevChunkAddr;
		}
		pCurrChunk->m_sizeOfCurrChunkInMem			=	endAddr - currChunkAddr;
		#ifdef	TGMEM_DEBUG_INFOS
		pCurrChunk->m_pDebugInfos					=	_pDebugInfos;
		pCurrChunk->m_pDebugCallback				=	_pDebugCallback;
		#endif	// TGMEM_DEBUG_INFOS

		// Update next chunk
		if	((endAddr + sizeof(TgMemChunkHeader)) >= pHeapManager->m_currEndAddr)
		{	// If we don't have the place for a free chunk, we collapse the free space with current (allocated) chunk
			pCurrChunk->m_sizeOfCurrChunkInMem		=	pHeapManager->m_currEndAddr - currChunkAddr;
			if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
			{
				pNextChunk							=	(TgMemChunkHeader*) pHeapManager->m_currEndAddr;
				pNextChunk->m_sizeOfPrevChunkInMem	=	pHeapManager->m_currEndAddr - currChunkAddr;
			}
			pHeapManager->m_currStartAddr			=	pHeapManager->m_currEndAddr;
		}
		else
		{	// We create a new free chunk
			pNextChunk								=	(TgMemChunkHeader*) endAddr;
			pNextChunk->m_sizeOfPrevChunkInMem		=	endAddr - currChunkAddr;
			pNextChunk->m_sizeOfCurrChunkInMem		=	(pHeapManager->m_currEndAddr - endAddr) | 1;
			if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
			{
				pNextChunk							=	(TgMemChunkHeader*) pHeapManager->m_currEndAddr;
				pNextChunk->m_sizeOfPrevChunkInMem	=	(pHeapManager->m_currEndAddr - endAddr) | 1;
			}
			pHeapManager->m_currStartAddr			=	endAddr;
		}

		#else	// TGMEM_DEBUG
		pHeapManager->m_currStartAddr				=	endAddr;
		#endif	// TGMEM_DEBUG
	}
	else	// (pHeapManager->m_direction == TG_MEM_HEAP_DIR_END)
	{
		#ifdef	TGMEM_DEBUG
		register TgMemSizeT			prevChunkAddr;
		register TgMemChunkHeader*	pPrevChunk;
		#endif	// TGMEM_DEBUG

		endAddr			=	pHeapManager->m_currEndAddr;
		startAddr		=	endAddr - realSize;													// We sub the realSize of the user datas
		startAddr		=	_TgMemStartAlignPow2( startAddr, pManager->m_alignment );			// We align the start on the use boundary
		#ifdef	TGMEM_DEBUG
		currChunkAddr	=	startAddr - sizeof(TgMemChunkHeader);								// We sub the size of ChunkHeader
		currChunkAddr	=	_TgMemStartAlignPow2( currChunkAddr, TGMEM_ALIGNOF_CHUNKHEADER );	// We align the start to a new ChunkHeaderr
		#endif	// TGMEM_DEBUG

		#ifdef	TGMEM_DEBUG
		if	(currChunkAddr < pHeapManager->m_currStartAddr)
			return	TGMEM_NULL;

		pCurrChunk		=	(TgMemChunkHeader*) currChunkAddr;
		if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
		{	// If the current chunk is not the first in end memory, we update the next chunk
			register TgMemChunkHeader*	pNextChunk;

			pNextChunk								=	(TgMemChunkHeader*) endAddr;
			pNextChunk->m_sizeOfPrevChunkInMem		=	endAddr - currChunkAddr;
			TGMEM_CASSERT(pNextChunk->m_sizeOfCurrChunkInMem >= sizeof(TgMemChunkHeader));
		}
		#ifdef	TGMEM_DEBUG_INFOS
		pCurrChunk->m_pDebugInfos					=	_pDebugInfos;
		pCurrChunk->m_pDebugCallback				=	_pDebugCallback;
		#endif	// TGMEM_DEBUG_INFOS

		pHeapManager->m_currEndAddr					=	currChunkAddr;
		if	((currChunkAddr - sizeof(TgMemChunkHeader)) <= pHeapManager->m_currStartAddr)
		{	// If we don't have the place for a free chunk, we collapse the free space with previous (allocated) chunk
			if	(pHeapManager->m_currStartAddr == pHeapManager->m_firstStartAddr)
			{	// If the current free chunk is the first in start memory, we create an (allocation) chunk
				pCurrChunk->m_sizeOfPrevChunkInMem	=	0;
				pCurrChunk->m_sizeOfCurrChunkInMem	=	endAddr - currChunkAddr;
				pHeapManager->m_currStartAddr		=	currChunkAddr;
				pHeapManager->m_firstStartAddr		=	currChunkAddr;
			}
			else
			{
				prevChunkAddr						=	pHeapManager->m_currStartAddr;
				pPrevChunk							=	(TgMemChunkHeader*) prevChunkAddr;
				TGMEM_CASSERT((pPrevChunk->m_sizeOfPrevChunkInMem & 1) == 0);
				TGMEM_CASSERT(pPrevChunk->m_sizeOfPrevChunkInMem >= sizeof(TgMemChunkHeader));
				prevChunkAddr						=	prevChunkAddr - pPrevChunk->m_sizeOfPrevChunkInMem;
				pPrevChunk							=	(TgMemChunkHeader*) prevChunkAddr;
				TGMEM_CASSERT((pPrevChunk->m_sizeOfCurrChunkInMem & 1) == 1);		// It must be a free chunk since we have memory space to alloc
				pCurrChunk->m_sizeOfPrevChunkInMem	=	(currChunkAddr - prevChunkAddr) | 1;
				pCurrChunk->m_sizeOfCurrChunkInMem	=	endAddr - currChunkAddr;
				pPrevChunk->m_sizeOfCurrChunkInMem	=	(currChunkAddr - prevChunkAddr) | 1;
			}
		}
		else
		{	// We create a free chunk
			prevChunkAddr							=	pHeapManager->m_currStartAddr;
			pPrevChunk								=	(TgMemChunkHeader*) pHeapManager->m_currStartAddr;
			TGMEM_CASSERT((pPrevChunk->m_sizeOfCurrChunkInMem & 1) == 1);			// It must be a free chunk
			pCurrChunk->m_sizeOfPrevChunkInMem		=	(currChunkAddr - prevChunkAddr) | 1;
			pCurrChunk->m_sizeOfCurrChunkInMem		=	endAddr - currChunkAddr;
			pPrevChunk->m_sizeOfCurrChunkInMem		=	(currChunkAddr - prevChunkAddr) | 1;
		}

		#else	// TGMEM_DEBUG
		if	(startAddr < pHeapManager->m_currStartAddr)
			return	TGMEM_NULL;
		pHeapManager->m_currStartAddr			=	startAddr;
		#endif	// TGMEM_DEBUG
	}

	return	(void*) startAddr;
}


//-----------------------------------------------------------------------------
// tgMemHeapFreeAll
// Description: Free all blocks in the heap
// Parameters: 	None
// Returns: None
// Note:
//-----------------------------------------------------------------------------
void				tgMemHeapFreeAll(			void )
{
	register TgMemHeapManager*	pHeapManager;
	#ifdef	TGMEM_DEBUG
	register TgMemSizeT			chunkAddr;
	register TgMemChunkHeader*	pChunk;
	#endif	// TGMEM_DEBUG

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pHeapManager					=	(TgMemHeapManager*) _pBindedManager;
	pHeapManager->m_currStartAddr	=	pHeapManager->m_memoryStartAddr;
	pHeapManager->m_currEndAddr		=	pHeapManager->m_memoryEndAddr;

	#ifdef	TGMEM_DEBUG
	chunkAddr						=	pHeapManager->m_memoryStartAddr;
	pHeapManager->m_firstStartAddr	=	chunkAddr;
	pChunk							=	(TgMemChunkHeader*) chunkAddr;
	pChunk->m_sizeOfPrevChunkInMem	=	0;
	pChunk->m_sizeOfCurrChunkInMem	=	(pHeapManager->m_memoryEndAddr - chunkAddr) | 1;
	#endif	// TGMEM_DEBUG
}


//-----------------------------------------------------------------------------
// tgMemHeapFreeDir
// Description: Free one direction in the heap
// Parameters: 	direction - direction to be freed
// Returns: None
// Note:
//-----------------------------------------------------------------------------
void				tgMemHeapFreeDir(			const ETgMemHeapDirection	direction  )
{
	register TgMemHeapManager*	pHeapManager;
	#ifdef	TGMEM_DEBUG
	register TgMemSizeT			chunkAddr;
	register TgMemChunkHeader*	pChunk;
	register TgMemSizeT			sizeOfFreeChunk;
	#endif	// TGMEM_DEBUG

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pHeapManager							=	(TgMemHeapManager*) _pBindedManager;
	if	(direction == TG_MEM_HEAP_DIR_START)
	{
		#ifdef	TGMEM_DEBUG
		chunkAddr							=	pHeapManager->m_memoryStartAddr;
		sizeOfFreeChunk						=	(pHeapManager->m_currEndAddr - chunkAddr) | 1;
		pChunk								=	(TgMemChunkHeader*) chunkAddr;
		pChunk->m_sizeOfPrevChunkInMem		=	0;
		pChunk->m_sizeOfCurrChunkInMem		=	sizeOfFreeChunk;
		// next chunk
		if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
		{
			pChunk							=	(TgMemChunkHeader*) pHeapManager->m_currEndAddr;
			pChunk->m_sizeOfPrevChunkInMem	=	sizeOfFreeChunk;
		}
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currStartAddr		=	pHeapManager->m_memoryStartAddr;
		#ifdef	TGMEM_DEBUG
		pHeapManager->m_firstStartAddr		=	pHeapManager->m_memoryStartAddr;
		#endif	// TGMEM_DEBUG
	}
	else	// if	(m_direction == TG_MEM_HEAP_DIR_END)
	{
		#ifdef	TGMEM_DEBUG
		chunkAddr							=	pHeapManager->m_currStartAddr;
		sizeOfFreeChunk						=	(pHeapManager->m_memoryEndAddr - chunkAddr) | 1;
		pChunk								=	(TgMemChunkHeader*) chunkAddr;
		pChunk->m_sizeOfCurrChunkInMem		=	sizeOfFreeChunk;
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currEndAddr			=	pHeapManager->m_memoryEndAddr;
	}
}


//-----------------------------------------------------------------------------
// tgMemHeapFreeFromTag
// Description: Free blocks from the tag, in the current allocation direction
// Parameters: pFreeTag - tag that indicates the beginning of the blocks to free
// Returns: None
// Note:
//-----------------------------------------------------------------------------
void				tgMemHeapFreeFromTag(		TgMemHeapTag* const			pFreeTag )
{
	register TgMemHeapManager*	pHeapManager;
	register TgMemSizeT			memToFreeAddr;
	#ifdef	TGMEM_DEBUG
	register TgMemChunkHeader*	pChunkToFree;
	register TgMemSizeT			sizeOfFreeChunk;
	#endif	// TGMEM_DEBUG

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= (TgMemSizeT)pFreeTag);
	TGMEM_CASSERT((TgMemSizeT)pFreeTag <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);
	TGMEM_CASSERT(((TgMemSizeT)pFreeTag < ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr) || ((TgMemSizeT)pFreeTag > ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr));
	TGMEM_CASSERT(_TgMemVerifyChunk((TgMemChunkHeader*)_TgMemComputeChunkHeaderAddr((TgMemSizeT)pFreeTag)));

	pHeapManager	=	(TgMemHeapManager*) _pBindedManager;
	memToFreeAddr	=	(TgMemSizeT) pFreeTag;
	#ifdef	TGMEM_DEBUG
	memToFreeAddr	=	_TgMemComputeChunkHeaderAddr(memToFreeAddr);
	pChunkToFree	=	(TgMemChunkHeader*) memToFreeAddr;
	#endif	// TGMEM_DEBUG

	if	(memToFreeAddr <= pHeapManager->m_currStartAddr)
	{	// Free at start
		#ifdef	TGMEM_DEBUG
		TGMEM_CASSERT((memToFreeAddr+sizeof(TgMemChunkHeader)) <= pHeapManager->m_currEndAddr);
		if	(memToFreeAddr == pHeapManager->m_firstStartAddr)
		{
			memToFreeAddr							=	pHeapManager->m_memoryStartAddr;
			sizeOfFreeChunk							=	(pHeapManager->m_currEndAddr - memToFreeAddr) | 1;
			pChunkToFree->m_sizeOfPrevChunkInMem	=	0;
			pChunkToFree->m_sizeOfCurrChunkInMem	=	sizeOfFreeChunk;
		}
		else
		{
			sizeOfFreeChunk							=	(pHeapManager->m_currEndAddr - memToFreeAddr) | 1;
			pChunkToFree->m_sizeOfCurrChunkInMem	=	sizeOfFreeChunk;
		}
		if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
		{
			pChunkToFree							=	(TgMemChunkHeader*) pHeapManager->m_currEndAddr;
			pChunkToFree->m_sizeOfPrevChunkInMem	=	sizeOfFreeChunk;
		}
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currStartAddr				=	memToFreeAddr;
	}
	else	// if	(memToFreeAddr >= pHeapManager->m_currEndAddr)
	{	// Free at end
		TGMEM_CASSERT((memToFreeAddr-sizeof(TgMemChunkHeader)) >= pHeapManager->m_currEndAddr);
		#ifdef	TGMEM_DEBUG
		sizeOfFreeChunk								=	(memToFreeAddr - pHeapManager->m_currStartAddr) | 1;
		pChunkToFree								=	(TgMemChunkHeader*) pHeapManager->m_currStartAddr;
		pChunkToFree->m_sizeOfCurrChunkInMem		=	sizeOfFreeChunk;
		if	(memToFreeAddr != pHeapManager->m_memoryEndAddr)
		{
			pChunkToFree							=	(TgMemChunkHeader*) memToFreeAddr;
			pChunkToFree->m_sizeOfPrevChunkInMem	=	sizeOfFreeChunk;
		}
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currEndAddr					=	memToFreeAddr;
	}
}


//-----------------------------------------------------------------------------
// tgMemHeapGetTag
// Description: Gets a memory tag to free the heap later
// Parameters: 	None
// Returns: TgMemHeapTag, a memory tag to free the heap later
// Note: see tgMemHeapFreeFromTag
//-----------------------------------------------------------------------------
TgMemHeapTag*		tgMemHeapGetTag(			void )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);

	if	(((TgMemHeapManager*)_pBindedManager)->m_direction == TG_MEM_HEAP_DIR_START)
		return	(TgMemHeapTag*) (((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	else	// if	(((TgMemHeapManager*)_pBindedManager)->m_direction == TG_MEM_HEAP_DIR_END)
		return	(TgMemHeapTag*) (((TgMemHeapManager*)_pBindedManager)->m_currEndAddr);
}


//-----------------------------------------------------------------------------
// tgMemHeapSetAllocDirection
// Description: Sets the allocation direction in the heap
// Parameters: direction -	allocation direction
// Returns: mone
// Note:
//-----------------------------------------------------------------------------
void				tgMemHeapSetAllocDirection(	const ETgMemHeapDirection	direction  )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);

	((TgMemHeapManager*)_pBindedManager)->m_direction =	direction;
}


//-----------------------------------------------------------------------------
// tgMemHeapGetAllocDirection
// Description: Sets the allocation direction in the heap
// Parameters: 	none
// Returns: allocation direction
// Note:
//-----------------------------------------------------------------------------
ETgMemHeapDirection	tgMemHeapGetAllocDirection(	void )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);

	return	((TgMemHeapManager*)_pBindedManager)->m_direction;
}


//-----------------------------------------------------------------------------
// tgMemHeapChangeDirection
// Description: change the direction of allocation
// Parameters: 	none
// Returns: none
// Note:  Can be used to switch from the current direction (for temporary alloc)
//-----------------------------------------------------------------------------
void				tgMemHeapChangeDirection(	void )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);

//	if	((TgMemHeapManager*)_pBindedManager)->m_direction == TG_MEM_HEAP_DIR_START)
//		tgMemHeapSetAllocDirection(TG_MEM_HEAP_DIR_END);
//	else	// if	((TgMemHeapManager*)_pBindedManager)->m_direction == TG_MEM_HEAP_DIR_END)
//		tgMemHeapSetAllocDirection(TG_MEM_HEAP_DIR_START);
	tgMemHeapSetAllocDirection( (ETgMemHeapDirection) (1 - ((TgMemHeapManager*)_pBindedManager)->m_direction) );
}


//-----------------------------------------------------------------------------
// tgMemHeapFree
// Description: free the heap from the pointer
// Parameters: pMemToFree -	pointer
// Returns: none
// Note:
//-----------------------------------------------------------------------------
void				tgMemHeapFree(				void* const					pMemToFree )
{
	register TgMemHeapManager*	pHeapManager;
	register TgMemSizeT			memToFreeAddr;
	#ifdef	TGMEM_DEBUG
	register TgMemChunkHeader*	pChunkToFree;
	register TgMemSizeT			sizeOfFreeChunk;
	#endif	// TGMEM_DEBUG

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= (TgMemSizeT)pMemToFree);
	TGMEM_CASSERT((TgMemSizeT)pMemToFree <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);
	TGMEM_CASSERT(((TgMemSizeT)pMemToFree < ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr) || ((TgMemSizeT)pMemToFree > ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr));
	TGMEM_CASSERT(_TgMemVerifyChunk((TgMemChunkHeader*)_TgMemComputeChunkHeaderAddr((TgMemSizeT)pMemToFree)));

	pHeapManager	=	(TgMemHeapManager*) _pBindedManager;
	memToFreeAddr	=	(TgMemSizeT) pMemToFree;
	#ifdef	TGMEM_DEBUG
	memToFreeAddr	=	_TgMemComputeChunkHeaderAddr(memToFreeAddr);
	pChunkToFree	=	(TgMemChunkHeader*) memToFreeAddr;
	#endif	// TGMEM_DEBUG

	if	(memToFreeAddr <= pHeapManager->m_currStartAddr)
	{	// Free at start
		#ifdef	TGMEM_DEBUG
		TGMEM_CASSERT((pChunkToFree->m_sizeOfCurrChunkInMem & 1) == 0);
	//	pChunkToFree								=	(TgMemChunkHeader*) (memToFreeAddr + pChunkToFree->m_sizeOfCurrChunkInMem);
	//	TGMEM_CASSERT((pChunkToFree->m_sizeOfCurrChunkInMem & 1) == 1);		// The next chunk can be used

		if	(memToFreeAddr == pHeapManager->m_firstStartAddr)
		{
			memToFreeAddr							=	pHeapManager->m_memoryStartAddr;
			sizeOfFreeChunk							=	(pHeapManager->m_currEndAddr - memToFreeAddr) | 1;
			pChunkToFree							=	(TgMemChunkHeader*) memToFreeAddr;
			pChunkToFree->m_sizeOfPrevChunkInMem	=	0;
			pChunkToFree->m_sizeOfCurrChunkInMem	=	sizeOfFreeChunk;
		}
		else
		{
			sizeOfFreeChunk							=	(pHeapManager->m_currEndAddr - memToFreeAddr) | 1;
			pChunkToFree							=	(TgMemChunkHeader*) memToFreeAddr;
			pChunkToFree->m_sizeOfCurrChunkInMem	=	sizeOfFreeChunk;
		}
		if	(pHeapManager->m_currEndAddr != pHeapManager->m_memoryEndAddr)
		{
			pChunkToFree							=	(TgMemChunkHeader*) pHeapManager->m_currEndAddr;
			pChunkToFree->m_sizeOfPrevChunkInMem	=	sizeOfFreeChunk;
		}
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currStartAddr				=	memToFreeAddr;
	}
	else	// if	(memToFreeAddr >= pHeapManager->m_currEndAddr)
	{	// Free at end
		#ifdef	TGMEM_DEBUG
		TGMEM_CASSERT((pChunkToFree->m_sizeOfCurrChunkInMem & 1) == 0);
	//	pChunkToFree								=	(TgMemChunkHeader*) (memToFreeAddr - pChunkToFree->m_sizeOfCurrChunkInMem);
	//	TGMEM_CASSERT((pChunkToFree->m_sizeOfCurrChunkInMem & 1) == 1);		// The previous chunk can be used
	//	pChunkToFree								=	(TgMemChunkHeader*) memToFreeAddr;

		memToFreeAddr								=	memToFreeAddr + pChunkToFree->m_sizeOfCurrChunkInMem;
		sizeOfFreeChunk								=	(memToFreeAddr - pHeapManager->m_currStartAddr) | 1;
		pChunkToFree								=	(TgMemChunkHeader*) memToFreeAddr;
		pChunkToFree->m_sizeOfPrevChunkInMem		=	sizeOfFreeChunk;

	//	if	(pHeapManager->m_currStartAddr != pHeapManager->m_firstStartAddr)
		{
			pChunkToFree							=	(TgMemChunkHeader*) pHeapManager->m_currStartAddr;
			pChunkToFree->m_sizeOfCurrChunkInMem	=	sizeOfFreeChunk;
		}
		#endif	// TGMEM_DEBUG

		pHeapManager->m_currEndAddr					=	memToFreeAddr;
	}
}


//-----------------------------------------------------------------------------
// tgMemHeapGetUsedSize
// Description: returns the memory size used by the manager
// Parameters: 	none
// Returns: size used by the manager
// Note:
//-----------------------------------------------------------------------------
TgMemSizeT			tgMemHeapGetUsedSize(		void )
{
	register TgMemHeapManager*	pHeapManager;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pHeapManager	=	(TgMemHeapManager*) _pBindedManager;
	return	((pHeapManager->m_currStartAddr - pHeapManager->m_memoryStartAddr) + (pHeapManager->m_memoryEndAddr - pHeapManager->m_currEndAddr));
}


//-----------------------------------------------------------------------------
// tgMemHeapGetFreeSize
// Description: returns the free memory size used by the manager
// Parameters: 	none
// Returns: free size in the manager
// Note:
//-----------------------------------------------------------------------------
TgMemSizeT			tgMemHeapGetFreeSize(		void )
{
	register TgMemHeapManager*	pHeapManager;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_HEAP_TYPE);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_memoryStartAddr <= ((TgMemHeapManager*)_pBindedManager)->m_currStartAddr);
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currStartAddr   <= ((TgMemHeapManager*)_pBindedManager)->m_currEndAddr  );
	TGMEM_CASSERT(((TgMemHeapManager*)_pBindedManager)->m_currEndAddr     <= ((TgMemHeapManager*)_pBindedManager)->m_memoryEndAddr);

	pHeapManager	=	(TgMemHeapManager*) _pBindedManager;
	return	(pHeapManager->m_currEndAddr - pHeapManager->m_currStartAddr);
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------
// PoolManager

//-----------------------------------------------------------------------------
// _TgMemPoolMalloc
// Description: alloc size bytes in the pool manager
// Parameters: 	size - size in bytes to be allocated
// Returns: pointer to the first allocated element
// Note:  There can be more than one allocated element if size exceed the size
//        of one pool element
//-----------------------------------------------------------------------------
void*				_TgMemPoolMalloc(			const TgMemSizeT			size )
{
	register TgMemSizeT	nbBlocs;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));

	nbBlocs =	((TgMemPoolManager*)_pBindedManager)->m_realSize;
	nbBlocs =	((size + nbBlocs) - 1) / nbBlocs;

	return	tgMemPoolCalloc(nbBlocs);
}


//-----------------------------------------------------------------------------
// tgMemPoolCreate
// Description: Create a pool manager
// Parameters: 	size - size of one element in the pool
//				num - number of element in the pool
// Returns: address of the manager
// Note:
//-----------------------------------------------------------------------------
TgMemManager*		tgMemPoolCreate(			const TgMemSizeT			size,
												const TgMemSizeT			num )
{
	register TgMemManager*		pManager;
	register TgMemPoolManager*	pPoolManager;
	register TgMemSizeT			memoryAlignment;
	register TgMemSizeT			memorySize;
	register TgMemSizeT			realSize;
	register TgMemSizeT			managerSize;
	register TgMemChunkPool*	pCurrChunkPool;
	register TgMemSizeT			currFreeList, sizeFreeList;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT((size & 1) == 0);

	memoryAlignment					=	tgMemGetAlignment();											// Save current alignment
	managerSize						=	_TgMemEndAlignPow2( sizeof(TgMemPoolManager) , TGMEM_ALIGNOF_CHUNKPOOL ) + sizeof(TgMemChunkPool) * num;
	realSize						=	_TgMemEndAlignPow2( size,  memoryAlignment );
	memorySize						=	realSize * num;
	pManager						=	(TgMemManager*) _TgMemCreateManager(
																TGMEM_ALIGNOF_POOLMANAGER,
																managerSize,
																memorySize );
	tgMemSetAlignment(memoryAlignment);																	// Restore current alignment
	if	(pManager == TGMEM_NULL)
		return	TGMEM_NULL;

	pManager->m_pVtbl				=	(TgMemManagerVtbl*) &_tgMemPoolVirtualTable;

	pPoolManager					=	(TgMemPoolManager*) pManager;
	pPoolManager->m_size			=	size;
	pPoolManager->m_num				=	num;
	pPoolManager->m_realSize		=	realSize;
	pPoolManager->m_memoryAddr		=	(TgMemSizeT) pManager->m_pMemory;
	pPoolManager->m_pChunkPool		=	(TgMemChunkPool*) _TgMemEndAlignPow2( sizeof(TgMemPoolManager)+(TgMemSizeT)pPoolManager, TGMEM_ALIGNOF_CHUNKPOOL );

	pCurrChunkPool								=	pPoolManager->m_pChunkPool;
	pCurrChunkPool->m_sizeOfPrevPoolInMem		=	0;
	pCurrChunkPool->m_sizeOfCurrPoolInMem		=	num;
	pCurrChunkPool->m_pPrevPoolInFreeList		=	TGMEM_NULL;
	pCurrChunkPool->m_pNextPoolInFreeList		=	TGMEM_NULL;
	pCurrChunkPool->m_isAFreeChunk				=	TGMEM_TRUE;
	pCurrChunkPool->m_numChunk					=	0;
	for	( currFreeList=1 ; currFreeList<num ; currFreeList++ )
	{
		pCurrChunkPool							=	&pCurrChunkPool[1];
		pCurrChunkPool->m_sizeOfPrevPoolInMem	=	0;
		pCurrChunkPool->m_sizeOfCurrPoolInMem	=	0;
		pCurrChunkPool->m_pPrevPoolInFreeList	=	TGMEM_NULL;
		pCurrChunkPool->m_pNextPoolInFreeList	=	TGMEM_NULL;
		pCurrChunkPool->m_isAFreeChunk			=	TGMEM_FALSE;
		pCurrChunkPool->m_numChunk				=	currFreeList;
	}

	for	( currFreeList=0 ; currFreeList<TGMEM_POOL_NB_FREE_LISTS ; currFreeList++ )
		pPoolManager->m_pFirstFree[currFreeList]=	TGMEM_NULL;

	currFreeList	=	0;
	sizeFreeList	=	num;
	while	(sizeFreeList > 1)
	{
		currFreeList++;
		sizeFreeList >>= 1;
	}
	pPoolManager->m_pFirstFree[currFreeList]	=	pPoolManager->m_pChunkPool;

	return	pManager;
}


//-----------------------------------------------------------------------------
// tgMemPoolMalloc
// Description: alloc one element in the pool
// Parameters: 	none
// Returns: pointer to the allocated element
// Note:
//-----------------------------------------------------------------------------
void*				tgMemPoolMalloc(			void )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_POOL_TYPE);

	return	tgMemPoolCalloc(1);
}


//-----------------------------------------------------------------------------
// tgMemPoolCalloc
// Description: alloc number of elements in the pool
// Parameters: 	num - elements in the pool
// Returns: pointer to the first element
// Note:
//-----------------------------------------------------------------------------
void*				tgMemPoolCalloc(			const TgMemSizeT			num )
{
	register TgMemPoolManager*	pPoolManager;
	register TgMemChunkPool*	pBestFreeChunk;
	register TgMemSizeT			bestRemainingSize;
	register TgMemSizeT			currFreeList;
	register TgMemSizeT			sizeFreeList;
	register void*				pMemory;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_POOL_TYPE);

	if	(num == 0)
		return	TGMEM_NULL;

	// Find the first correct list
	currFreeList		=	0;
	sizeFreeList		=	num;
	while	(sizeFreeList > 1)
	{
		currFreeList++;
		sizeFreeList >>= 1;
	}

	pPoolManager		=	(TgMemPoolManager*) _pBindedManager;
	pBestFreeChunk		=	TGMEM_NULL;
	bestRemainingSize	=	(TgMemSizeT) -1;
	while	(currFreeList < TGMEM_POOL_NB_FREE_LISTS)
	{
		register TgMemChunkPool*	pCurrFreeChunk;

		pCurrFreeChunk	=	pPoolManager->m_pFirstFree[currFreeList];

		while	(pCurrFreeChunk != TGMEM_NULL)
		{
			register TgMemSizeT	remainingSize;

			remainingSize =	(pCurrFreeChunk->m_sizeOfCurrPoolInMem & ~1) - num;

			if	(remainingSize == 0)
			{
				pBestFreeChunk	=	pCurrFreeChunk;
				break;
			}

			if	(bestRemainingSize > remainingSize)
			{
				pBestFreeChunk		=	pCurrFreeChunk;
				bestRemainingSize	=	remainingSize;
			}

			pCurrFreeChunk =	pCurrFreeChunk->m_pNextPoolInFreeList;
		}

		if	(pBestFreeChunk != TGMEM_NULL)
			break;

		currFreeList++;
	}

	if	(pBestFreeChunk == TGMEM_NULL)
		return	TGMEM_NULL;

	// Allocated bloc
	if	(pPoolManager->m_pFirstFree[currFreeList] == pBestFreeChunk)
		pPoolManager->m_pFirstFree[currFreeList] =	pBestFreeChunk->m_pNextPoolInFreeList;
//	pBestFreeChunk->m_sizeOfPrevPoolInMem		is unchanged
	pBestFreeChunk->m_sizeOfCurrPoolInMem	=	num;
	pBestFreeChunk->m_pPrevPoolInFreeList	=	TGMEM_NULL;
	pBestFreeChunk->m_pNextPoolInFreeList	=	TGMEM_NULL;
	pBestFreeChunk->m_isAFreeChunk			=	TGMEM_FALSE;
	#ifdef	TGMEM_DEBUG_INFOS
	pBestFreeChunk->m_pDebugInfos			=	_pDebugInfos;
	pBestFreeChunk->m_pDebugCallback		=	_pDebugCallback;
	#endif	// TGMEM_DEBUG_INFOS

	// Compute address of block
	pMemory =	(void*) (pPoolManager->m_memoryAddr + pPoolManager->m_realSize * pBestFreeChunk->m_numChunk);

	// New free bloc (eventualy)
	if	(bestRemainingSize > 0)
	{
		// Find the first correct list
		currFreeList	=	0;
		sizeFreeList	=	bestRemainingSize;
		while	(sizeFreeList > 1)
		{
			currFreeList++;
			sizeFreeList >>= 1;
		}

		pBestFreeChunk								=	&pBestFreeChunk[num];
		pBestFreeChunk->m_sizeOfPrevPoolInMem		=	num;
		pBestFreeChunk->m_sizeOfCurrPoolInMem		=	bestRemainingSize;
		pBestFreeChunk->m_pPrevPoolInFreeList		=	TGMEM_NULL;
		pBestFreeChunk->m_pNextPoolInFreeList		=	pPoolManager->m_pFirstFree[currFreeList];
		pBestFreeChunk->m_isAFreeChunk				=	TGMEM_TRUE;

		pPoolManager->m_pFirstFree[currFreeList]	=	pBestFreeChunk;
	}

	return	pMemory;
}


//-----------------------------------------------------------------------------
// tgMemPoolFree
// Description: free a memory block allocated from the pool
// Parameters: 	pMemToFree - pointer to the first element
// Returns: none
// Note:  can free more than one element in the pool
//-----------------------------------------------------------------------------
void				tgMemPoolFree(				void* const					pMemToFree )
{
	register TgMemPoolManager*	pPoolManager;
	register TgMemChunkPool*	pPoolDatas;
	register TgMemChunkPool*	pPrevPool;
	register TgMemChunkPool*	pCurrPool;
	register TgMemChunkPool*	pNextPool;
	register TgMemSizeT			memToFreeAddr;
	register TgMemSizeT			numPool;
	register TgMemSizeT			currFreeList;
	register TgMemSizeT			sizeFreeList;

	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_POOL_TYPE);
	TGMEM_CASSERT(((((TgMemSizeT)pMemToFree) - ((TgMemPoolManager*)_pBindedManager)->m_memoryAddr) % ((TgMemPoolManager*)_pBindedManager)->m_realSize) == 0);

	pPoolManager		=	(TgMemPoolManager*) _pBindedManager;
	pPoolDatas			=	(TgMemChunkPool*) pPoolManager->m_pChunkPool;
	memToFreeAddr		=	(TgMemSizeT) pMemToFree;
	numPool				=	(memToFreeAddr - pPoolManager->m_memoryAddr) / pPoolManager->m_realSize;
	
	TGMEM_CASSERT(pPoolDatas[numPool].m_sizeOfCurrPoolInMem != 0);

	pCurrPool			=	&pPoolDatas[numPool];
	pPrevPool			=	&pPoolDatas[numPool - pCurrPool->m_sizeOfPrevPoolInMem];
	pNextPool			=	&pPoolDatas[numPool + pCurrPool->m_sizeOfCurrPoolInMem];

	TGMEM_CASSERT(pPrevPool->m_sizeOfCurrPoolInMem == pCurrPool->m_sizeOfPrevPoolInMem);
	TGMEM_CASSERT(pCurrPool->m_sizeOfCurrPoolInMem == pNextPool->m_sizeOfPrevPoolInMem);
//	TGMEM_CASSERT_MSG( pPrevPool->m_sizeOfCurrPoolInMem == pCurrPool->m_sizeOfPrevPoolInMem, "tgMemPoolFree(pMemToFree=0x%x)", pMemToFree );
//	TGMEM_CASSERT_MSG( pCurrPool->m_sizeOfCurrPoolInMem == pNextPool->m_sizeOfPrevPoolInMem, "tgMemPoolFree(pMemToFree=0x%x)", pMemToFree );

	if	(pPrevPool->m_isAFreeChunk == TGMEM_TRUE)
	{
	//	pPrevPool->m_sizeOfPrevPoolInMem		is unchanged
		pPrevPool->m_sizeOfCurrPoolInMem	=	((pPrevPool->m_sizeOfCurrPoolInMem & ~1) + (pCurrPool->m_sizeOfCurrPoolInMem & ~1)) | 1;
	//	pPrevPool->m_pPrevPoolInFreeList		is unchanged
		pPrevPool->m_pNextPoolInFreeList	=	pNextPool;
		pPrevPool->m_isAFreeChunk			=	TGMEM_TRUE;

		pCurrPool->m_sizeOfPrevPoolInMem	=	0;
		pCurrPool->m_sizeOfCurrPoolInMem	=	0;
		pCurrPool->m_pPrevPoolInFreeList	=	TGMEM_NULL;
		pCurrPool->m_pNextPoolInFreeList	=	TGMEM_NULL;
		pCurrPool->m_isAFreeChunk			=	TGMEM_FALSE;

		pNextPool->m_sizeOfPrevPoolInMem	=	pPrevPool->m_sizeOfCurrPoolInMem;

		pCurrPool							=	pPrevPool;
	}
	else
	{
	//	pCurrPool->m_sizeOfPrevPoolInMem		is unchanged
		pCurrPool->m_sizeOfCurrPoolInMem	=	(pPrevPool->m_sizeOfCurrPoolInMem & ~1) | 1;
	//	pCurrPool->m_pPrevPoolInFreeList		is unchanged
		pCurrPool->m_pNextPoolInFreeList	=	pNextPool;
		pCurrPool->m_isAFreeChunk			=	TGMEM_TRUE;

		pNextPool->m_sizeOfPrevPoolInMem	=	pCurrPool->m_sizeOfCurrPoolInMem;
	}

	// New free bloc to insert
	currFreeList	=	0;
	sizeFreeList	=	pCurrPool->m_sizeOfCurrPoolInMem;
	while	(sizeFreeList > 1)
	{
		currFreeList++;
		sizeFreeList >>= 1;
	}
	pCurrPool->m_pNextPoolInFreeList			=	pPoolManager->m_pFirstFree[currFreeList];
	pPoolManager->m_pFirstFree[currFreeList]	=	pCurrPool;
}


//-----------------------------------------------------------------------------
// tgMemPoolFreeAll
// Description: free all the blocks in the pool
// Parameters: 	none
// Returns: none
// Note:
//-----------------------------------------------------------------------------
void				tgMemPoolFreeAll(			void )
{
	TGMEM_CASSERT(_TgMemIsAManager(_pBindedManager));
	TGMEM_CASSERT(_pBindedManager->m_pVtbl->m_rtti == TG_MEM_MANAGER_POOL_TYPE);
}


//-----------------------------------------------------------------------------
//G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgMemSetDebugInfos
// Description: sets callback and pointer to debug memory allocation
// Parameters: 	pDebugInfos - pointer to debug infos
//				pDebugCallback - pointer to callback when allocation error
// Returns: none
// Note:  Never completly implemented
//-----------------------------------------------------------------------------
//void				tgMemSetDebugInfos(			void*						pDebugInfos,
//												void						(*pDebugCallback)(void) )
//{
//	_pDebugInfos	=	pDebugInfos;
//	_pDebugCallback	=	pDebugCallback;
//}


//-----------------------------------------------------------------------------
// U N D E F
//-----------------------------------------------------------------------------
#undef TGMEM_NULL
#undef TGMEM_ALIGNOF

#undef TGMEM_ASSERT
#undef TGMEM_CASSERT
#undef TGMEM_CASSERT_MSG

#undef TGMEM_ALIGNOF_CHUNKHEADER
#undef TGMEM_ALIGNOF_CHUNKPOOL
#undef TGMEM_ALIGNOF_HEAPMANAGER
#undef TGMEM_ALIGNOF_POOLMANAGER
#undef TGMEM_POOL_NB_FREE_LISTS

#undef TGMEM_INLINE
#undef TGMEM_FALSE
#undef TGMEM_TRUE


//-----------------------------------------------------------------------------
