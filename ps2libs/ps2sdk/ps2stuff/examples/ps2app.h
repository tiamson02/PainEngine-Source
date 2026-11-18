/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef ps2app_h
#define ps2app_h

namespace GS {
   class CMemArea;
   class CDrawEnv;
   class CDisplayEnv;
}

class CVifSCDmaPacket;
class CSprite;

class CPs2App {
      // this class should be subclassed, not instantiated
   protected:

      CPs2App( int fbWidth, int fbHeight,
	       unsigned int clearColor, int dmaPacketQwordSize );
      ~CPs2App() {}

      GS::CMemArea	*Frame0Mem, *Frame1Mem, *ZBufMem;
      GS::CMemArea	*CurFrameMem, *LastFrameMem;
      GS::CDrawEnv	*DrawEnv;
      GS::CDisplayEnv	*DisplayEnv;

      GS::CDrawEnv	*ClearEnv;
      CSprite		*ClearSprite;

      CVifSCDmaPacket	*CurPacket, *LastPacket;

      unsigned int	CurrentFrameNumber;
      bool		CurrentFieldIsEven;

      void		WaitForVSync();
      void		SwapBuffers();
};

#endif // ps2app_h
