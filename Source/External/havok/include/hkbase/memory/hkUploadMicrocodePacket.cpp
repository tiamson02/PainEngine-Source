/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/memory/hkUploadMicrocodePacket.h>

#include <eeregs.h>
#include <libdma.h>

void	hkUploadMicrocodePacket (void* microcodePacketStart, void* microcodePacketEnd )

{
	// calculate length of packet in quad words

	*D0_QWC = ( (unsigned int)(microcodePacketEnd) - (unsigned int)(microcodePacketStart) ) >> 4;

	// set D_STAT and D_PCR

	*D_STAT		= 1;
	*D_PCR		= 1;

	// point DMA source memory address to start of packet

	*D0_MADR	= (unsigned int)microcodePacketStart;


	// Ensure that the channel interrupt mask is disabled.
	// The original state is restored upon completion.
		
	unsigned int	d_StatState = *D_STAT;
	bool		setChannelMask = false;
	
	if( d_StatState & 0x00010000 )
	{
		// Disable channel interrupt mask
		*D_STAT = 0x00010000;
		setChannelMask = true;
	}	
	
	// start DMA transfer

	*D0_CHCR = 0x0100;

	// ensure that memory is synchronised

	__asm__ __volatile__ ("sync.l");
	__asm__ __volatile__ ("sync.p");

	// test for completion using D_STAT

	while( !(*D_STAT & 1))
	{
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
		__asm__ __volatile__ ("nop");
	}
		
	// Transfer has completed without interrupt generation
	// We can now safely restore the channel interrupt mask
	
	if ( setChannelMask )
	{
		*D_STAT = 0x00010000;
	} 
		
}

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
