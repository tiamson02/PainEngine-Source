/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#include <malloc.h>

#include "sifdev.h"
#include "eeregs.h"

#include "mytexture.h"

#include "ps2s/gsmem.h"

/********************************************
 * CMMTexture methods
 */

CMMTexture::CMMTexture( GS::tContext context )
   : CTexture( context ), pImageMem(NULL), pClutMem(NULL)
{
}

void
CMMTexture::SetImage( tU128* imagePtr, tU32 w, tU32 h, GS::tPSM psm, tU32* clutPtr )
{
   CTexture::SetImage( imagePtr, w, h, psm, clutPtr );

   // create a memarea for the image
   mAssert( pImageMem == NULL );
   // the width of the buffer in gs mem will be different from the image width
   // if the image width is not a power of 2.  This is calculated in CTexEnv::SetDimensions()
   tU32 bufWidth = gsrTex0.TBW * 64;
   pImageMem = new GS::CMemArea( bufWidth, h, psm, GS::kAlignBlock );
	
   // create a memarea for the clut, if any
   if ( clutPtr != NULL ) {
      mAssert( pClutMem == NULL );
      pClutMem = new GS::CMemArea( 16, 16, GS::kPsm32, GS::kAlignBlock );
   }
}

void
CMMTexture::Load( bool waitForEnd )
{
   // first set the addresses and flush the cache
   if ( ! pImageMem->IsAllocated() ) pImageMem->Alloc();
   SetImageGsAddr( pImageMem->GetWordAddr() );
   if ( pClut ) {
      if ( ! pClutMem->IsAllocated() ) pClutMem->Alloc();
      SetClutGsAddr( pClutMem->GetWordAddr() );
   }
   FlushCache(0);

	// send the clut, if any
   if ( pClut ) SendClut( Packet::kDontWait, Packet::kFlushCache );
   // send the image
   SendImage( waitForEnd, Packet::kDontFlushCache );
}

#define mLoad( _pkt )						\
	if ( pClut && ! pClutMem->IsAllocated() ) {		\
		pClutMem->Alloc();				\
		SetClutGsAddr( pClutMem->GetWordAddr() );	\
		SendClut( _pkt );				\
	}							\
	if ( ! pImageMem->IsAllocated() ) {			\
		pImageMem->Alloc();				\
		SetImageGsAddr( pImageMem->GetWordAddr() );	\
		SendImage( _pkt );				\
	}


void
CMMTexture::Load( CSCDmaPacket& packet )
{
   mLoad( packet );
}

void
CMMTexture::Load( CVifSCDmaPacket& packet )
{
   mLoad( packet );
}

void
CMMTexture::Use( bool waitForEnd )
{
   // settings aren't being added...
   Load();
   SendSettings( waitForEnd, Packet::kDontFlushCache );
}

void
CMMTexture::Use( CSCDmaPacket& packet )
{
   Load( packet );
   SendSettings( packet );
}

void
CMMTexture::Use( CVifSCDmaPacket& packet )
{
   Load( packet );
   SendSettings( packet );
}

void
CMMTexture::Free( void )
{
   pImageMem->Free(); 
   if ( pClutMem ) pClutMem->Free();
}


/********************************************
 * CSGITex Methods
 */

CSGITex::CSGITex( GS::tContext context, const char* sgiImageFilename )
   : CMMTexture( context )
{
   LoadSGIFile( sgiImageFilename );
}

inline tU16 fixShort( tU16 sgiShort )
{
   tU16 temp = sgiShort & 0xff;
   sgiShort >>= 8;
   sgiShort |= temp << 8;
   return sgiShort;
}

void
CSGITex::LoadSGIFile( const char* fileName )
{
   // open the file
   int fd = sceOpen( (char*)fileName, SCE_RDONLY ); // sceOpen is not declared with const char*... hmm...
   mAssert( fd != -1 );

   tU8 info;
   tU16 shInfo;

   // Parse the header

	// make sure it's really an sgi-format file (474 is the sgi magic number)
   sceRead( fd, &shInfo, 2 ); shInfo = fixShort( shInfo );
   mAssert( shInfo == 474 );
   // make sure it's not RLE
   sceRead( fd, &info, 1 ); mAssert( info == 0 );
   // make sure it's 1 byte per channel
   sceRead( fd, &info, 1 ); mAssert( info == 1 );
   // let's only handle color images for now..
   sceRead( fd, &shInfo, 2 ); shInfo = fixShort( shInfo );
   mAssert( shInfo == 3 );
   // get the width and height
   sceRead( fd, &shInfo, 2 ); tU32 width = (tU32)fixShort( shInfo );
   sceRead( fd, &shInfo, 2 ); tU32 height = (tU32)fixShort( shInfo );
   // get the number of channels
   sceRead( fd, &shInfo, 2 ); tU32 numChannels = (tU32)fixShort( shInfo );
   mAssert( numChannels >= 3 );
   GS::tPSM psm = ( numChannels == 3 ) ? GS::kPsm24 : GS::kPsm32;
   // ignore the rest for now..
   sceLseek( fd, 512, SCE_SEEK_SET );

   // allocate memory
   tU128* image = AllocMem( width, height, psm );
   SetImage( image, width, height, psm );


   // get the image and rearrange it..
   tU32 pixelSize = ( psm == GS::kPsm24 ) ? 3 : 4;
   tU8* buffer = (tU8*)memalign( 128, uiTexPixelWidth * uiTexPixelHeight * pixelSize ); // memalign just to be on the safe side..
   sceRead( fd, buffer, uiTexPixelWidth * uiTexPixelHeight * pixelSize );
   t32 row, col;
   tU8 *r, *g, *b, *a = NULL;
   tU8* curPixel = (tU8*)pImage;
   for ( row = uiTexPixelHeight - 1; row >= 0; row-- ) {
      r = &buffer[0 * uiTexPixelWidth * uiTexPixelHeight + uiTexPixelWidth * row];
      g = &buffer[1 * uiTexPixelWidth * uiTexPixelHeight + uiTexPixelWidth * row];
      b = &buffer[2 * uiTexPixelWidth * uiTexPixelHeight + uiTexPixelWidth * row];
      if ( numChannels == 4 )
	 a = &buffer[3 * uiTexPixelWidth * uiTexPixelHeight + uiTexPixelWidth * row];

      for ( col = 0; col < (t32)uiTexPixelWidth; col++ ) {
         *curPixel++ = *r++;
         *curPixel++ = *g++;
         *curPixel++ = *b++;
         if ( numChannels == 4 ) *curPixel++ = *a++;
      }
   }

   free( buffer );
   sceClose( fd );
}

