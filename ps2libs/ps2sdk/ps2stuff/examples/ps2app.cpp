/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#include "ps2app.h"

/********************************************
 * includes
 */

#include "libpkt.h"
#include "libgraph.h"
#include "libdma.h"
#include "libdev.h"

#include "ps2s/ps2stuff.h"
#include "ps2s/math.h"
#include "ps2s/gs.h"
#include "ps2s/drawenv.h"
#include "ps2s/displayenv.h"
#include "ps2s/gsmem.h"
#include "ps2s/sprite.h"

#include "ps2app.h"
#include "pads.h"

/********************************************
 * code
 */

CPs2App::CPs2App( int fbWidth, int fbHeight,
		  unsigned int clearColor, int dmaPacketQwordSize )
	: CurrentFrameNumber(0), CurrentFieldIsEven(true)
{
   // init ps2stuff

   ps2sInit();

   // reset the machine

   sceDevVif0Reset();
   sceDevVu0Reset();
   sceDmaReset(1);    
   sceGsResetPath();
   sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
   GS::Init();
   Pads::Init();

   // allocate memory for the frame/depth buffers

   int numFrameBufPages = ( fbWidth / 64 + (fbWidth % 64 > 0) )
      * ( fbHeight / 32 + (fbHeight % 32 > 0) );

   using namespace GS;

   // slots in gs mem
   GS::CMemManager &mm = GS::CMemArea::GetMemManager();
   mm.AddSlot( numFrameBufPages * 0, numFrameBufPages, GS::kPsm32 );
   mm.AddSlot( numFrameBufPages * 1, numFrameBufPages, GS::kPsm32 );
   mm.AddSlot( numFrameBufPages * 2, numFrameBufPages, GS::kPsm24 );

   Frame0Mem = new CMemArea( fbWidth, fbHeight, GS::kPsm32, GS::kAlignPage );
   Frame1Mem = new CMemArea( fbWidth, fbHeight, GS::kPsm32, GS::kAlignPage );
   ZBufMem = new CMemArea( fbWidth, fbHeight, GS::kPsm24, GS::kAlignPage );

   Frame0Mem->Alloc();
   Frame0Mem->Lock();
   Frame1Mem->Alloc();
   Frame1Mem->Lock();
   ZBufMem->Alloc();
   ZBufMem->Lock();

   // init vif1 packets

   CurPacket = new CVifSCDmaPacket( dmaPacketQwordSize, DMAC::Channels::vif1,
				    Packet::kXferTags, Core::MemMappings::UncachedAccl );
   LastPacket = new CVifSCDmaPacket( dmaPacketQwordSize, DMAC::Channels::vif1,
				     Packet::kXferTags, Core::MemMappings::UncachedAccl );

   // clearing buffers

   ClearEnv = new CDrawEnv( GS::kContext1 );

   ClearEnv->SetFrameBufferAddr( Frame0Mem->GetWordAddr() );
   ClearEnv->SetFrameBufferDim( fbWidth, fbHeight );
   ClearEnv->SetDepthBufferAddr( ZBufMem->GetWordAddr() );
   ClearEnv->SetDepthBufferPSM( GS::kPsmz24 );
   ClearEnv->SetDepthTestPassMode( GS::ZTest::kAlways );
   ClearEnv->SendSettings( *CurPacket );

   ClearSprite = new CSprite( GS::kContext1, 0, 0, fbWidth, fbHeight );

   ClearSprite->SetColor( clearColor );
   ClearSprite->Draw( *CurPacket );

   // setup display context

   DisplayEnv = new CDisplayEnv;

   DisplayEnv->SetFB2( Frame1Mem->GetWordAddr(), fbWidth );
   DisplayEnv->SetDisplay2( fbWidth, fbHeight * 2, 0, 0, 4 );
   DisplayEnv->SetBGColor( 20, 20, 40 );

   // "current" means the frame being drawn to by the loop of code executing on the core
   // "last" will be the frame being displayed if drawing immediately, or the frame not
   // displayed now if building up a packet to be sent next frame.
   CurFrameMem = Frame0Mem;
   LastFrameMem = Frame1Mem;

   // setup draw context

   DrawEnv = new CDrawEnv( GS::kContext1 );

   DrawEnv->SetFrameBufferAddr( CurFrameMem->GetWordAddr() );
   DrawEnv->SetFrameBufferDim( fbWidth, fbHeight );
   DrawEnv->CalculateClippedFBXYOffsets( GS::kDontAddHalfPixel );
   DrawEnv->SetDepthBufferAddr( ZBufMem->GetWordAddr() );
   DrawEnv->SetDepthBufferPSM( GS::kPsmz24 );
   DrawEnv->SetDepthTestPassMode( GS::ZTest::kGEqual );
   using namespace GS::ABlend;
   DrawEnv->SetAlphaBlendFunc( kSourceRGB, kDestRGB, kSourceAlpha, kDestRGB, 0x80 );
   DrawEnv->SetFogColor( 255, 255, 255 );
   DrawEnv->SendSettings( *CurPacket );
}

void
CPs2App::WaitForVSync()
{
   // wait for beginning of v-sync 
   CurrentFieldIsEven = Math::IsEven( sceGsSyncV(0) );
}

void
CPs2App::SwapBuffers()
{
   // make sure the packet ends
   CurPacket->End();
   CurPacket->Pad128();
   CurPacket->CloseTag();

   /*
     #include "../../dmadebug/dmadebug.h"
   if ( CurrentFrameNumber == 2 ) {
      DMA_init("-B1500");
      VIF_disassembleDmaList( sceDmaGetChan(DMAC::Channels::vif1), CurPacket->GetBase() );
      DEBUG_saveBuffers( "dma.txt", false );
      exit(0);
   }
   */

   // flip vif1 packets
   CVifSCDmaPacket *tempPkt = CurPacket;
   CurPacket = LastPacket;
   LastPacket = tempPkt;
   LastPacket->Send();
   CurPacket->Reset();

   // flip frame buffer ptrs
   GS::CMemArea* temp = CurFrameMem;
   CurFrameMem = LastFrameMem;
   LastFrameMem = temp;

   // new frame

   // clear the new frame
   ClearEnv->SetFrameBufferAddr( CurFrameMem->GetWordAddr() );
   ClearEnv->SendSettings( *CurPacket );
   ClearSprite->Draw( *CurPacket );

   // display the last completed frame (which is frame n-2 because we're not
   // drawing immediately but building up a packet)
   // remember this is immediately sent, not delayed through a packet
   DisplayEnv->SetFB2Addr( CurFrameMem->GetWordAddr() );
   DisplayEnv->SendSettings();

   // add a half-pixel offset if the frame we're going to build on the core will
   // be displayed in an odd field
   DrawEnv->CalculateClippedFBXYOffsets( ! CurrentFieldIsEven );

   // draw to the new frame
   DrawEnv->SetFrameBufferAddr( CurFrameMem->GetWordAddr() );
   DrawEnv->SendSettings( *CurPacket );

   CurrentFrameNumber++;
}
