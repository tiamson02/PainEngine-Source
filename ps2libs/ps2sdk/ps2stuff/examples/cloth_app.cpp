/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

/********************************************
 * includes
 */

#include <iostream>
using namespace std;

#include <math.h>
#include "eetypes.h"
#include "libgraph.h"

#include "ps2s/timer.h"
#include "ps2s/gs.h"
#include "ps2s/packet.h"
#include "ps2s/matrix.h"
#include "ps2s/math.h"
#include "ps2s/gsmem.h"

#include "cloth_app.h"
#include "pads.h"
#include "vu1_renderers.h"
#include "vu1_format.h"
#include "cloth.h"
#include "mytexture.h"

/********************************************
 * globals
 */

CEETimer gTimer0( CEETimer::Timer0 );
CEETimer gTimer1( CEETimer::Timer1 );

CClothApp gApp;

/********************************************
 * main
 */

int
main( void )
{
   gApp.Go();
   return 0;
}

/********************************************
 * app code
 */

static const int kScreenResX = 640;
static const int kScreenResY = 224;

static const int kDmaPacketQwordLength = 80000;

#include "ps2s/drawenv.h"

CClothApp::CClothApp()
   :
   CPs2App( kScreenResX, kScreenResY,
	    GS::PackRGBA(0, 0, 0, 0),
	    kDmaPacketQwordLength ),
   PrintTimes(false),
   time_step(0.08f), SimIsPaused(false),
   wind_dir( 0, 1.0f, 0.1f, 0 ), wind_strength(1.0f),
   UseLibVu0(false)
{
   mInitTimers( &gTimer0, CEETimer::BusClock_256th );

   // transfer microcode

   CurPacket->Call( (void*)vsmSCEI );
   CurPacket->Pad128();
   CurPacket->CloseTag();

   // view xform

   GSScale.set_scale( vec_xyz(320.0f, -1 * 112.0f, -1 * 8388607.5f) );
   Perspective( 40.0f, kScreenResX / (2*kScreenResY), 1, 10000 );

   streamer1 = new cloth( 5, 35, 5.0f, 1.0f, vec_xyzw(-1.5f, -2.5f, -10, 1) );
   streamer2 = new cloth( 5, 35, 5.0f, 1.0f, vec_xyzw(0, -2.5f, -10, 1) );
   streamer3 = new cloth( 5, 35, 5.0f, 1.0f, vec_xyzw(1.5f, -2.5f, -10, 1) );

   // setup texture

   GS::CMemManager &mm = GS::CMemArea::GetMemManager();
   // 210 is where the frame and depth buffers end at 640x224
   // a 256x256 image takes 32 pages
   mm.AddSlot( 210, 32, GS::kPsm32 );

   texture = new CSGITex( GS::kContext1, "host0:tex_cub.sgi" );
   texture->SetMagMode( GS::MagMode::kLinear );
   texture->SetTexMode( GS::TexMode::kModulate );
   texture->SetWrapMode( GS::TexWrapMode::kRepeat, GS::TexWrapMode::kRepeat );

   PrintUsage();

   GS::CDrawEnv de( GS::kContext1, 640, 480, 0, NULL );
   CSCDmaPacket packet( 20, DMAC::Channels::gif, false );
   de.SendSettings( packet );
   packet.HexDump();
}

void
CClothApp::Go()
{
   while(1) {
      mUpdateTimers();

      mStartTimer("frame time");

      DoControls();

      TransferVu1Context( 4 );

      UpdateStreamer();
      DrawStreamer();

      if ( PrintTimes ) {
	 mDisplayTimers();
	 PrintTimes = false;
      }

      vec_xyz a(0, 1, 2);
      vec_y b(2.0f);
      vec_xyz c = a * b;
      c.print();

      mStartTimer("wait for vu1");
      sceGsSyncPath(0, 0);
      mStopTimer("wait for vu1");

      mStopTimer("frame time");

      WaitForVSync();
      SwapBuffers();
   }
}

void
CClothApp::PrintUsage()
{
   cout << endl;
   cout << "left/right - wind speed\n";
   // cout << "up/down    - time step\n";
   cout << endl;
   cout << "x button   - pause/unpause simulation\n";
   cout << "square     - reset simulation\n";
   cout << "circle     - single-step simulation\n";
   cout << endl;
   cout << "start      - print timings\n";
   cout << "select     - use libvu0 for cloth::update\n";
   cout << endl;
}

void
CClothApp::DoControls()
{
   Pads::Read();

   if ( Pad0.WasPushed(Pads::kStart) )
      PrintTimes = true;
   if ( Pad0.WasPushed(Pads::kSelect) ) {
      UseLibVu0 = ! UseLibVu0;
      if ( UseLibVu0 )
	 cout << "Using libvu0 for cloth::update\n";
      else
	 cout << "Using vector classes for cloth::update\n";
   }

   if (Pad0.WasPushed(Pads::kRightDown))
      SimIsPaused = ! SimIsPaused;

   if (Pad0.WasPushed(Pads::kRightLeft)) {
      streamer1->reset();
      streamer2->reset();
      streamer3->reset();
   }
      

   if (Pad0.IsDown(Pads::kLeftRight))
      wind_strength += 0.02f;
   if (Pad0.IsDown(Pads::kLeftLeft))
      wind_strength -= 0.02f;
   wind_strength = Math::Clamp( wind_strength, -3.0f, 3.0f );
}

void
CClothApp::UpdateStreamer()
{
   mStartTimer("update");

   // the simulation is not very stable, so run it many times
   // with smaller timesteps

   if ( ! SimIsPaused 
	|| (SimIsPaused && Pad0.WasPushed(Pads::kRightRight)) )
   {
      unsigned int i, sim_times = 3;

      if ( UseLibVu0 ) {
	 float wind_dir_lv[4];
	 memcpy( (char*)wind_dir_lv, (char*)&wind_dir, 16 );

	 for ( i = 0; i < sim_times; i++ ) {
	    streamer1->update_libvu0( time_step, wind_dir_lv, wind_strength );
	    streamer2->update_libvu0( time_step, wind_dir_lv, wind_strength );
	    streamer3->update_libvu0( time_step, wind_dir_lv, wind_strength );
	 }
      }
      else {
	 for ( i = 0; i < sim_times; i++ ) {
	    streamer1->update( time_step, wind_dir, wind_strength );
	    streamer2->update( time_step, wind_dir, wind_strength );
	    streamer3->update( time_step, wind_dir, wind_strength );
	 }
      }	 
   }

   mStopTimer("update");
}

void
CClothApp::DrawStreamer()
{
   mStartTimer("draw");

   CVifSCDmaPacket &packet = *CurPacket;

   texture->Use( packet );

   packet.Cnt();
   {
      mat_44 tex_scale, tex_shift, tex_xform;
      tex_scale.set_scale( vec_xyzw(1.0f/6.0f, -1.0f/6.0f, 1, 1) );
      tex_shift.set_translate( vec_xyz(2.5f, 2.5f, 0) );
      tex_xform = tex_scale * tex_shift;

      streamer1->draw( packet, tex_xform );
      streamer2->draw( packet, tex_xform );
      streamer3->draw( packet, tex_xform );
   }
   packet.CloseTag();

   mStopTimer("draw");
}

typedef struct _myGifTag {
	unsigned long NLOOP:15;
	unsigned long EOP:1;
	unsigned long pad16:16;
	unsigned long id:14;
	unsigned long PRE:1;
	unsigned long PRIM:11;
	unsigned long FLG:2;
	unsigned long NREG:4;
	unsigned long REGS0:4;
	unsigned long REGS1:4;
	unsigned long REGS2:4;
	unsigned long REGS3:4;
	unsigned long REGS4:4;
	unsigned long REGS5:4;
	unsigned long REGS6:4;
	unsigned long REGS7:4;
	unsigned long REGS8:4;
	unsigned long REGS9:4;
	unsigned long REGS10:4;
	unsigned long REGS11:4;
	unsigned long REGS12:4;
	unsigned long REGS13:4;
	unsigned long REGS14:4;
	unsigned long REGS15:4;
} myGifTag __attribute__((aligned(16)));

void
CClothApp::TransferVu1Context( unsigned int primType )
{
   // The "global" vu1 parameters depend on the following info:
   // which lights are enabled/disabled, each enabled light's settings,
   // global ambient, current material, vertex xform, giftag

   CVifSCDmaPacket &packet = *CurPacket;

   packet.Cnt();
   {
      packet.Stcycl(1,1);
      packet.Flush();
      packet.Pad96();
      packet.OpenUnpack( Vifs::UnpackModes::v4_32, kInitInfoStart, Packet::kSingleBuff );
      {
	 // find light pointers
	 tLightPtrs lightPtrs[8];
	 tLightPtrs *nextDir, *nextPt, *nextSpot;
	 nextDir = nextPt = nextSpot = &lightPtrs[0];

	 int numDirs, numPts, numSpots;
	 numDirs = numPts = numSpots = 0;
	 nextDir->dir = kLight0Base;
	 numDirs++;

	 // num lights
	 packet += numDirs;
	 packet += numPts;
	 packet += numSpots;
	 packet += 0; // placeholder

	 // light pointers
	 packet.Add( &lightPtrs[0], 8 );
	 
	 // when texturing in modulate mode, 100% of a channel occurs at
	 // 0.5 on the gs, so we want a color component value of 1.0 to map to
	 // 128, else 255
	 float maxColorValue = 128.0f;

	 vec_xyz light_amb( 0, 0, 0 ),
	    light_diff( 1, 1, 1 ),
	    light_spec( 1, 0, 0 ),
	    light_pos( 0, 0, 1 ),
	    dummy;

	 light_pos.normalize();

	 // add light info
	 for ( int i = 0; i < 8; i++ ) {
	    packet += light_amb * maxColorValue;
	    packet += light_diff * maxColorValue;
	    packet += light_spec * maxColorValue;
	    packet += light_pos;
	    // no spot dir
	    packet += dummy;

	    // no attentuation
	    packet += dummy;
	 }

	 // global ambient

	 vec_xyz global_amb( 0.4f, 0.4f, 0.4f );
	 packet += global_amb * maxColorValue;

	 // material properties

	 vec_xyzw mat_emm( 0, 0, 0, 0 ),
	    mat_amb( 1, 1, 1, 0 ),
	    mat_diff( 0.6f, 0.6f, 0.6f, 0 ),
	    mat_spec( 1, 1, 1, 0 );

	 packet += mat_emm * maxColorValue;
	 packet += mat_amb;
	 packet += mat_diff;
	 packet += mat_spec;

	 // vertex xform

	 packet += GSScale * PerspectiveXfrm;

	 // transpose of object to world space xfrm (for light directions)

	 mat_44 objToWorldXfrmTrans;
	 objToWorldXfrmTrans.set_identity();
	 // clear any translations.. should be doing a 3x3 transpose..
	 objToWorldXfrmTrans.set_col3( vec_xyzw(0, 0, 0, 1) );
	 objToWorldXfrmTrans = objToWorldXfrmTrans.transpose();

	 // fixed vertToEye vector for non-local specular

	 vec_xyzw vertToEye( 0.0f, 0.0f, 1.0f, 0.0f );
	 packet += objToWorldXfrmTrans * vertToEye;

	 // transpose of object to world space transform

	 packet += objToWorldXfrmTrans;

	 // world to object space xfrm (for light positions)

	 mat_44 worldToObjXfrm;
	 worldToObjXfrm.set_identity();
	 packet += worldToObjXfrm;

	 // giftag - this is down at the bottom to make sure that when switching
	 // primitives the last buffer will have a chance to copy the giftag before
	 // it is overwritten with the new one

	 bool smoothShading = true;
	 bool useTexture = true;
	 bool alpha = false;

	 sceGsPrim prim = { PRIM: primType, IIP: smoothShading, TME: useTexture,
			    FGE: 0, ABE: alpha, AA1: 0, FST: 0, CTXT: 0, FIX: 0 };
	 myGifTag giftag = { NLOOP: 0, EOP: 1, pad16: 0, id: 0, PRE: 1,
			     PRIM: *(tU64*)&prim, FLG: 0, NREG: 3, REGS0: 2, REGS1: 1,
			     REGS2: 4 };

	 packet += giftag;
      }
      packet.CloseUnpack();

      packet.Mscal(0);
      packet.Flushe();

      packet.Base( kDoubleBufBase );
      packet.Offset( kDoubleBufOffset );
   }
   packet.CloseTag();
}

void
CClothApp::Perspective( float fov, float aspect, float zNear, float zFar )
{
   float w, h;
   aspect *= 1.27f; // weird hack
   fov *= 3.141592654f/180.0f;
   h = 2.0f * zNear * (float)tanf( fov/2.0f );
   w = h * aspect;

   float left = -w/2.0f, right = w/2.0f, bottom = -h/2.0f, top = h/2.0f;

   mat_44 xform( vec_xyzw( (2.0f * zNear)
			   / (right - left),
			   0.0f,
			   0.0f,
			   0.0f ),
		 vec_xyzw( 0.0f,
			   (2.0f * zNear)
			   / (top - bottom),
			   0.0f,
			   0.0f ),
		 vec_xyzw( (right + left)
			   / (right - left),
			   (top + bottom)
			   / (top - bottom),
			   -(zFar + zNear)
			   / (zFar - zNear),
			   -1.0f ),
		 vec_xyzw( 0.0f,
			   0.0f,
			   (-2.0f * zFar * zNear)
			   / (zFar - zNear),
			   0.0f )
      );

   PerspectiveXfrm = xform;
}

