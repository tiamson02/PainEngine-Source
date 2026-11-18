/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef cloth_app_h
#define cloth_app_h

#include "ps2app.h"
#include "ps2s/vector.h"

class cloth;
class CSGITex;

class CClothApp : public CPs2App {
   public:
      CClothApp();
      ~CClothApp() {}

      void
      Go();

   private:

      bool		PrintTimes;
      float		time_step;
      bool		SimIsPaused;
      vec_xyzw		wind_dir;
      float		wind_strength;
      bool		UseLibVu0;
      cloth		*streamer1, *streamer2, *streamer3;

      CSGITex		*texture;

      mat_44		PerspectiveXfrm, GSScale;

      void
      DrawStreamer();

      void
      UpdateStreamer();

      void
      DoControls();

      void
      PrintUsage();

      void
      TransferVu1Context( unsigned int primType );

      void
      Perspective( float fov, float aspect, float zNear, float zFar );
};

#endif // cloth_app_h
