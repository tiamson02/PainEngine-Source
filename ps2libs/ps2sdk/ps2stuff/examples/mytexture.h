/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef mytexture_h
#define mytexture_h

#include "ps2s/gs.h"
#include "ps2s/texture.h"

/********************************************
 * CMMTexture (Mem Managed Texture)
 */

namespace GS {
   class CMemArea;
}

class CMMTexture : public GS::CTexture
{
   public:
      // warning! these two will flush the data cache!
      void Load( bool waitForEnd = true );
      void Use( bool waitForEnd = false );

      void Load( CSCDmaPacket& packet );
      void Load( CVifSCDmaPacket& packet );

      void Use( CSCDmaPacket& packet );
      void Use( CVifSCDmaPacket& packet );

      void Free( void );

   protected:
      CMMTexture( GS::tContext context );

      void SetImage( tU128* imagePtr, tU32 w, tU32 h, GS::tPSM psm, tU32* clutPtr = NULL );

   private:
      GS::CMemArea *pImageMem, *pClutMem;
};

/********************************************
 * CSGITex
 */

class CSGITex : public CMMTexture
{
   public:
      CSGITex( GS::tContext context, const char* sgiImageFilename );

      void LoadSGIFile( const char* fileName );
};




#endif // mytexture_h
