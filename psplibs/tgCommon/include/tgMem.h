/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgMem.h
*   Memory manager
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-23-08      rubens      initial implementation
*/


#ifndef	TGMEM_H
#define	TGMEM_H
//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgTypes.h"


//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------
#define TGMEM_NULL				0

typedef enum
{
	TG_MEM_HEAP_DIR_START =	0,		// heap allocation is from start of heap
	TG_MEM_HEAP_DIR_END =	1		// heap allocation is from end of heap
}	ETgMemHeapDirection;			//heap direction for allocation

typedef u32						TgMemSizeT;		//size of general data used in tgMem
typedef struct _TgMemManager	TgMemManager;	//memory manager structure
typedef struct _TgMemHeapTag	TgMemHeapTag;	//memory address which locates a block within a heap


//-----------------------------------------------------------------------------
// S T R U C T U R E S
//-----------------------------------------------------------------------------
struct _TgMemManager;
struct _TgMemHeapTag;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------
#if		defined(__cplusplus)
extern "C" {
#endif	//--- defined(__cplusplus)
	
	void				tgMemBindManager(			TgMemManager* const			pManager );
	void				tgMemDestroy(				TgMemManager* const			pManager );
	TgMemSizeT			tgMemGetAlignment(			void );
	void				tgMemSetAlignment(			const TgMemSizeT			alignment );

	//--- Heap Manager --------
	TgMemManager*		tgMemHeapCreate(			const TgMemSizeT			size );
	void*				tgMemHeapMalloc(			const TgMemSizeT			size );
	void*				tgMemHeapCalloc(			const TgMemSizeT			num,
													const TgMemSizeT			size );
	void				tgMemHeapFreeAll(			void );
	void				tgMemHeapFreeDir(			const ETgMemHeapDirection	direction );
	void				tgMemHeapFreeFromTag(		TgMemHeapTag* const			pFreeTag );			// Free all allocations after a TAG
	TgMemHeapTag*		tgMemHeapGetTag(			void );											//--- Get a memory TAG
	void				tgMemHeapSetAllocDirection(	const ETgMemHeapDirection	direction );
	ETgMemHeapDirection	tgMemHeapGetAllocDirection(	void );
	void				tgMemHeapChangeDirection(	void );
//---	#ifdef	TGMEM_DEBUG
	void				tgMemHeapFree(				void* const					pMemToFree );		// Free all allocation including pMemToFree area
//---	#endif	// TGMEM_DEBUG
	TgMemSizeT			tgMemHeapGetUsedSize(		void );
	TgMemSizeT			tgMemHeapGetFreeSize(		void );

	//--- Pool Manager --------
	TgMemManager*		tgMemPoolCreate(			const TgMemSizeT			size,
													const TgMemSizeT			num );
	void*				tgMemPoolMalloc(			void );
	void*				tgMemPoolCalloc(			const TgMemSizeT			num );
	void				tgMemPoolFree(				void* const					pMemToFree );
	void				tgMemPoolFreeAll(			void );

	//--- Debug infos ---------
	#ifdef	TGMEM_DEBUG_INFOS
	void				tgMemSetDebugInfos(			void*						pDebugInfos,
													void						(*pDebugCallback)(void) );
	#endif	//--- TGMEM_DEBUG_INFOS
#if		defined(__cplusplus)
}
#endif	// defined(__cplusplus)


//-----------------------------------------------------------------------------
#endif	// TGMEM_H
