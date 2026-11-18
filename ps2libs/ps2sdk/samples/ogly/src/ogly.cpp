

#include <stdlib.h>
#include <stdarg.h>

//#include "ps2gl.h"

#if defined( PS2_LINUX )

#include "ps2gs.h"
#include "ps2dma.h"
#include "ps2vpu.h"
#include "ps2vpufile.h"

#elif defined( __ee__ )
#include "libgraph.h"
#include "libdev.h"

#endif

#include "GL/ps2gl.h"
#include "ps2s/timer.h"
#include "ps2s/gs.h"
#include "ps2s/packet.h"
#include "ps2s/displayenv.h"
#include "ps2gl/debug.h"

#include <ps2s/ps2stuff.h>

#if defined( PS2_LINUX )

#include <linux/ps2/gs.h>
#include "pad.h"

#elif defined( __ee__ )

#include "pads.h"

#endif




#include <math.h>

struct ps2_vpu_struct {
int fd;
int vpu;
unsigned int mapsize;
void *text;
unsigned int text_size;
void *data;
unsigned int data_size;
};

int g_inter;
int g_out_mode;
int g_ff_mode;
int g_resolution;
int g_refresh_rate;
int g_psm;
int g_zpsm;
int g_zbits;

int g_fd_gs;
#ifdef PS2_LINUX
ps2_vpu *g_vpu0, *g_vpu1;
#endif
typedef enum { eNtsc, eVesa0 } screen_mode_t;
screen_mode_t screen_mode = eVesa0;

void test_drawing();

#ifndef PS2_LINUX

static char default_module_path[] = "host0:/usr/local/sce/iop/modules";
static char *module_path = '\0';    


static void
initGsMemory()
{
   // frame and depth buffer
   pgl_slot_handle_t frame_slot_0, frame_slot_1, depth_slot;
   frame_slot_0 = pglAddGsMemSlot( 0, 70, GS::kPsm32 );
   frame_slot_1 = pglAddGsMemSlot( 70, 70, GS::kPsm32 );
   depth_slot = pglAddGsMemSlot( 140, 70, GS::kPsmz24 );
   // lock these slots so that they aren't allocated by the memory manager
   pglLockGsMemSlot( frame_slot_0 );
   pglLockGsMemSlot( frame_slot_1 );
   pglLockGsMemSlot( depth_slot );

   // create gs memory area objects to use for frame and depth buffers
   pgl_area_handle_t frame_area_0, frame_area_1, depth_area;
   frame_area_0 = pglCreateGsMemArea( 640, 224, GS::kPsm24 );
   frame_area_1 = pglCreateGsMemArea( 640, 224, GS::kPsm24 );
   depth_area = pglCreateGsMemArea( 640, 224, GS::kPsmz24 );
   // bind the areas to the slots we created above
   pglBindGsMemAreaToSlot( frame_area_0, frame_slot_0 );
   pglBindGsMemAreaToSlot( frame_area_1, frame_slot_1 );
   pglBindGsMemAreaToSlot( depth_area, depth_slot );

   // draw to the new areas...
   pglSetDrawBuffers( PGL_INTERLACED, frame_area_0, frame_area_1, depth_area );
   // ...and display from them
   pglSetDisplayBuffers( PGL_INTERLACED, PGL_NOFRAMEAA, frame_area_0, frame_area_1 );

   // 32 bit

   // a slot for fonts (probably)
   pglAddGsMemSlot( 210, 2, GS::kPsm8 );

   // 64x32
   pglAddGsMemSlot( 212, 1, GS::kPsm32 );
   pglAddGsMemSlot( 213, 1, GS::kPsm32 );
   // 64x64
   pglAddGsMemSlot( 214, 2, GS::kPsm32 );
   pglAddGsMemSlot( 216, 2, GS::kPsm32 );
   pglAddGsMemSlot( 218, 2, GS::kPsm32 );
   pglAddGsMemSlot( 220, 2, GS::kPsm32 );
   // 128x128
   pglAddGsMemSlot( 222, 8, GS::kPsm32 );
   pglAddGsMemSlot( 230, 8, GS::kPsm32 );
   // 256x256
   pglAddGsMemSlot( 238, 32, GS::kPsm32 );
   pglAddGsMemSlot( 270, 32, GS::kPsm32 );
   // 512x256
   pglAddGsMemSlot( 302, 64, GS::kPsm32 );
   pglAddGsMemSlot( 366, 64, GS::kPsm32 );

   pglPrintGsMemAllocation();
}


void ps2kitInit(/* int *argcp, char **argv*/)
{
//   parse_cl( argcp, argv );

   // get the iop module path and initialize the pads
   // this is before ps2gl initialization because the sifInitRPC in
   // Pads::Init needs to happen before the dtv init
   
   if ( module_path == NULL ) {
      printf("No iop module path specified!  Using %s.\n", default_module_path);
      module_path = default_module_path;
   }
   Pads::Init(module_path);
   
   // does the ps2gl library need to be initialized?
   
   if ( ! pglHasLibraryBeenInitted() ) {
      // reset the machine
      sceDevVif0Reset();
      sceDevVu0Reset();
      sceDmaReset(1);    
      sceGsResetPath();
      
      sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
      
      printf( "ps2gl library has not been initialized by the user; using default values.\n" );
      int immBufferVertexSize = 64 * 1024;
      pglInit( immBufferVertexSize, 1000 );
   }
   
   // does gs memory need to be initialized?
   
   if ( ! pglHasGsMemBeenInitted() ) {
      printf("GS memory has not been allocated by the user; using default values.\n");
      initGsMemory();
   }
}

#else
static void
initGsMemory()
{
   // frame and depth buffer
   pgl_slot_handle_t frame_slot_0, frame_slot_1, depth_slot;
   frame_slot_0 = pglAddGsMemSlot( 0, 150, GS::kPsm32 );
   frame_slot_1 = pglAddGsMemSlot( 150, 150, GS::kPsm32 );
   depth_slot = pglAddGsMemSlot( 300, 150, GS::kPsmz24 );
   // lock these slots so that they aren't allocated by the memory manager
   pglLockGsMemSlot( frame_slot_0 );
   pglLockGsMemSlot( frame_slot_1 );
   pglLockGsMemSlot( depth_slot );

   // ntsc or vesa?

   // default to ntsc
   int fb_height = 480, interlace = PGL_NONINTERLACED;

   if ( screen_mode == eNtsc ) {
      fb_height = 224;
      interlace = PGL_INTERLACED;
   }

   // create gs memory area objects to use for frame and depth buffers
   pgl_area_handle_t frame_area_0, frame_area_1, depth_area;
   frame_area_0 = pglCreateGsMemArea( 640, fb_height, GS::kPsm24 );
   frame_area_1 = pglCreateGsMemArea( 640, fb_height, GS::kPsm24 );
   depth_area = pglCreateGsMemArea( 640, fb_height, GS::kPsmz24 );
   // bind the areas to the slots we created above
   pglBindGsMemAreaToSlot( frame_area_0, frame_slot_0 );
   pglBindGsMemAreaToSlot( frame_area_1, frame_slot_1 );
   pglBindGsMemAreaToSlot( depth_area, depth_slot );

   // draw to the new areas...
   pglSetDrawBuffers( interlace, frame_area_0, frame_area_1, depth_area );
   // ...and display from them
   pglSetDisplayBuffers( interlace, frame_area_0, frame_area_1 );

   // 32 bit

   // 64x32
   pglAddGsMemSlot( 450, 1, GS::kPsm32 );
   pglAddGsMemSlot( 451, 1, GS::kPsm32 );
   // 64x64
   pglAddGsMemSlot( 452, 2, GS::kPsm32 );
   pglAddGsMemSlot( 454, 2, GS::kPsm32 );
   // 128x128
   pglAddGsMemSlot( 456, 8, GS::kPsm32 );
   pglAddGsMemSlot( 464, 8, GS::kPsm32 );
   pglAddGsMemSlot( 472, 8, GS::kPsm32 );
   // 256x256
   pglAddGsMemSlot( 480, 32, GS::kPsm32 );

   pglPrintGsMemAllocation();
}


int release(void)
{
   if (g_vpu1) {
      ps2_vpu_close(g_vpu1);
      g_vpu1 = NULL;
   }
	
   if (g_vpu0) {
      ps2_vpu_close(g_vpu0);
      g_vpu0 = NULL;
   }
	
   if (g_fd_gs >= 0) {
      ps2_gs_close();
      g_fd_gs = -1;
   }
	
   return PS2_GS_VC_REL_SUCCESS;
}

int acquire(void)
{
   g_fd_gs = ps2_gs_open(-1);
   g_vpu0 = ps2_vpu_open(0);
   g_vpu1 = ps2_vpu_open(1);
	
   if (g_fd_gs < 0 || g_vpu0 == NULL || g_vpu1 == NULL) {
      release();
      return PS2_GS_VC_ACQ_FAILURE;
   }
	
   ps2_vpu_reset(g_vpu1);
   ps2_vpu_reset(g_vpu0);
	
   ps2_gs_reset(0, g_inter, g_out_mode, g_ff_mode, g_resolution,
		g_refresh_rate);
	
   return PS2_GS_VC_ACQ_SUCCESS;
}

int g_break = 1;
static bool ps2linuxInit()
{
#if 0
	fprintf(stderr, "TORQUE_ENGINE PID = %d\n", getpid());
	while (1)
	{
		if (g_break == 2)
			break;
	}
#endif
   //x86UNIXState->setScreenNumber( 0 );
   //x86UNIXState->setDesktopSize( 640, 480);
   //x86UNIXState->setDesktopBpp( 32);

  if ( screen_mode == eVesa0 ) {
      printf("vesa0 mode\n");
      g_inter = PS2_GS_NOINTERLACE;
      g_out_mode = PS2_GS_VESA;
      g_ff_mode = PS2_GS_FRAME;
      g_resolution = PS2_GS_640x480;
   }
   else if ( screen_mode == eNtsc ) {
      printf("ntsc1 mode\n");
      g_inter = PS2_GS_NOINTERLACE;
      g_out_mode = PS2_GS_NTSC;
      g_ff_mode = PS2_GS_FRAME;
      g_resolution = PS2_GS_640x480;
   }

   g_refresh_rate = PS2_GS_60Hz;
   g_psm = PS2_GS_PSMCT32;
   g_zpsm = PS2_GS_PSMZ32;
   g_zbits = 32;

//Support Virtual Console
//   ps2_gs_vc_graphicsmode();
	

	//Init the GS engine.  -1 says to the function for it to handle the opening
   g_fd_gs = ps2_gs_open(-1);
   g_vpu0 = ps2_vpu_open(0); // must be opened for vpu1...
   g_vpu1 = ps2_vpu_open(1);

   // these are static, so they should be ok to call before
   // pglInit..
   ps2_gs_reset(0, g_inter, g_out_mode, g_ff_mode, g_resolution,
		g_refresh_rate);
	
   ps2_gs_start_display(1);
	
   ps2_gs_vc_enablevcswitch(acquire, release);

   ps2_gs_sync_v(0);

   // we need to tell ps2stuff the various file descriptors before
   // initializing ps2gl. (temporary)
   ps2sSetGsFd(g_fd_gs);
   ps2sSetVu0Fd(g_vpu0->fd);
   ps2sSetVu1Fd(g_vpu1->fd);

 // does the ps2gl library need to be initialized?

   if ( ! pglHasLibraryBeenInitted() ) {
      mWarn( "ps2gl library has not been initialized by the user; using default values." );
      int immBufferVertexSize = 32 * 1024;
      pglInit( immBufferVertexSize, 1500 );
   }

   pglBeginGeometry();

   // does gs memory need to be initialized?

   if ( ! pglHasGsMemBeenInitted() ) {
      mWarn("GS memory has not been allocated by the user; using default values.");
      initGsMemory();
   }

   //Init joystick
   Pad[PAD1].Initialise(PADDEV1, PAD_INIT_LOCK |PAD_INIT_ANALOGUE | PAD_INIT_PRESSURE);

   // Disable clipping & scissoring by default
   pglDisable(PGL_CLIPPING);
   pglDisable(PGL_SCISSORING);

// Do Spinny cube test
//	test_drawing();
  
   return true;
}
#endif //(+/-)PS2_LINUX

static void InitPS2()
{
#ifdef PS2_LINUX
   ps2linuxInit();
#else
   ps2kitInit();
#endif
}

#if 0 
void* operator new( size_t size ) {
  size = (size == 0) ? 1 : size;
  void *mem = memalign( 16, size );
  return mem;
}
#endif

#include <stdlib.h>
#include <stdio.h>

void pglRegisterClipRenderer();
int  main (void)
{
	
	
	printf("starting test program\n");


	//      set_keypress();	
//	int blah = fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	printf("initing ps2gl\n");
	InitPS2();
	
	printf("entering drawing loop\n");
	test_drawing(); 


}

