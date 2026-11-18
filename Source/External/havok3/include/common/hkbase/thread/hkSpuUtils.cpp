/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>

#include <hkbase/thread/hkSpuUtils.h>
#include <hkbase/monitor/hkMonitorStream.h>

#if defined (HK_PLATFORM_PS3SPU)

#include <spu_intrinsics.h>
#include <sys/spu_thread.h>
#include <sys/spu_event.h>
#include <stdint.h>

#include <cell/dma.h>

void waitDmaCompletion(unsigned int tag)
{
	/*Clear MFC tag update */
	spu_writech(MFC_WrTagUpdate, 0x0);
	for (; spu_readchcnt(MFC_WrTagUpdate) != 1;);
	spu_readch(MFC_RdTagStat);

	/* Set tag update and wait for completion */
	spu_writech(MFC_WrTagMask, (1 << tag));
	spu_writech(MFC_WrTagUpdate, 0x2);
	spu_readch(MFC_RdTagStat);
}

void* hkDmaToSpu( void* localAddr, const HK_CPU_PTR(void*) fromAddr, hkUint32 size)
{
	unsigned int tag = 0;
	HK_CHECK_ALIGN16(localAddr);
	HK_CHECK_ALIGN16(fromAddr);
	HK_ON_DEBUG(int ret =) cellDmaLargeGet(localAddr, fromAddr, size, tag, 0, 0);
	HK_ASSERT(0x0, ret == CELL_OK);
	waitDmaCompletion(tag);

	return localAddr;

}

void hkDmaFromSpu( HK_CPU_PTR(void*) mainAddr, const void* fromAddr, hkUint32 size)
{
	unsigned int tag = 0;
	HK_CHECK_ALIGN16(mainAddr);
	HK_CHECK_ALIGN16(fromAddr);
	HK_ON_DEBUG(int ret =) cellDmaLargePut(fromAddr, mainAddr, size, tag, 0, 0);
	HK_ASSERT(0x0, ret == CELL_OK);

	waitDmaCompletion(tag);

}

#else // !HK_PLATFORM_PS3SPU

#define HK_SIMULATE_DMA_ON_CPU
#if defined HK_SIMULATE_DMA_ON_CPU

void* hkDmaToSpu( void* localAddr, const HK_CPU_PTR(void*) fromAddr, hkUint32 size)
{
	HK_CHECK_ALIGN16(localAddr);
	HK_CHECK_ALIGN16(fromAddr);
	hkString::memCpy(localAddr, fromAddr, size );
	return localAddr;
}

void hkDmaFromSpu( HK_CPU_PTR(void*) mainAddr, const void* fromAddr, hkUint32 size)
{
	HK_CHECK_ALIGN16(mainAddr);
	HK_CHECK_ALIGN16(fromAddr);
	hkString::memCpy(mainAddr, fromAddr, size );
}

#else 

void* hkDmaToSpu( void* localAddr, const HK_CPU_PTR(void*) fromAddr, hkUint32 size)
{
	return (void*)fromAddr;
}

void hkDmaFromSpu( HK_CPU_PTR(void*) mainAddr, const void* fromAddr, hkUint32 size)
{
}

#endif // HK_SIMULATE_MEMCPY_ON_CPU

#endif // HK_PLATFORM_PS3SPU

void hkEmptyPrintfFunc(...) {}

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
