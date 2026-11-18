/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SPU_UTILS_H
#define HK_SPU_UTILS_H

// Some printf defines used for internal debugging

// Enable printfs from SPU
//#define HK_ENABLED_SPU_DEBUG_PRINTFS

// Enable printfs if simulating SPU on CPU
//#define HK_VIEW_SPU_DEBUG_PRINTFS_ON_CPU

#if defined(HK_PLATFORM_PS3SPU)
	#if defined(HK_ENABLED_SPU_DEBUG_PRINTFS)
		#	include <spu_printf.h>
		#	define HK_SPU_DEBUG_PRINTF spu_printf
	#else
		void hkEmptyPrintfFunc(...);
		#	define HK_SPU_DEBUG_PRINTF if(0) hkEmptyPrintfFunc
	#endif
#else
	#if defined (HK_VIEW_SPU_DEBUG_PRINTFS_ON_CPU)
		#	include <stdio.h>
		#	define HK_SPU_DEBUG_PRINTF printf
	#else
		void hkEmptyPrintfFunc(...);
		#	define HK_SPU_DEBUG_PRINTF if(0) hkEmptyPrintfFunc
	#endif
#endif

#if defined HK_DEBUG
#define HK_CHECK_ALIGN16(A) if ( (((hkUlong)A) & 0xf) != 0 ) { HK_SPU_DEBUG_PRINTF("Alignment error"); }
#else
#define HK_CHECK_ALIGN16(A) {}
#endif


void* hkDmaToSpu( void* localAddr, const HK_CPU_PTR(void*) fromAddr, hkUint32 size);

void hkDmaFromSpu( HK_CPU_PTR(void*) mainAddr, const void* fromAddr, hkUint32 size);

#endif // HK_SPU_CONSTRAINT_H


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
