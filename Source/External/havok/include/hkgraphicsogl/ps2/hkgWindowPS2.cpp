/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled

/*           Copyright (C) 2001 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */


//:HAVOK:
//
// This file is based on the init file in the PS2GL Glut distrib
// but tidied up to fit into the HKG lib better
//

#include <hkgraphicsogl/ps2/hkgWindowPS2.h>
#include <hkgraphicsogl/ps2/hkgWindowPS2Pad.h>
#include <hkgraphics/hkgPad.h>

#include <libgraph.h>
#include <libdev.h>
#include <sifdev.h>

#include <GL/ps2gl.h>

static GLfloat clearColor[] = { 0.3f, 0.3f, 0.4f, 0.0f };
static GLfloat black[] = { 0, 0, 0, 0 };
static GLfloat white[] = { 1.0f, 1.0f,1.0f,1.0f };

/*
 HKG_PS2_PAD_SELECT      0
 HKG_PS2_PAD_LEFT_STICK  1
 HKG_PS2_PAD_RIGHT_STICK 2
 HKG_PS2_PAD_START       3
 HKG_PS2_PAD_LEFT_UP     4
 HKG_PS2_PAD_LEFT_RIGHT  5
 HKG_PS2_PAD_LEFT_DOWN   6
 HKG_PS2_PAD_LEFT_LEFT   7
 HKG_PS2_PAD_L2          8
 HKG_PS2_PAD_R2          9
 HKG_PS2_PAD_L1          10
 HKG_PS2_PAD_R1          11
 HKG_PS2_PAD_RIGHT_UP    12
 HKG_PS2_PAD_RIGHT_RIGHT 13
 HKG_PS2_PAD_RIGHT_DOWN  14
 HKG_PS2_PAD_RIGHT_LEFT  15
*/

static unsigned short _PS2ButtonMap[16] = {
	HKG_PAD_SELECT,  //select 
	HKG_PAD_BUTTON_LSTICK, //lstick
	HKG_PAD_BUTTON_RSTICK, //rstick
	HKG_PAD_START,  //start 
	
	HKG_PAD_DPAD_UP,  //d up
	HKG_PAD_DPAD_RIGHT,  //d left
	HKG_PAD_DPAD_DOWN,  //d down
	HKG_PAD_DPAD_LEFT,  //d right
	
	HKG_PAD_BUTTON_L2, //l2
	HKG_PAD_BUTTON_R2, //r2
	HKG_PAD_BUTTON_L1, //l1
	HKG_PAD_BUTTON_R1, //r1
	
	HKG_PAD_BUTTON_2,  //b right circle
	HKG_PAD_BUTTON_3,  //b up triangle
	HKG_PAD_BUTTON_0,  //b down cross
	HKG_PAD_BUTTON_1,  //b left square
};

bool hkgWindowPS2::g_bSystemInit = false;

char *hkgWindowPS2::g_sModulePath = HK_NULL;

#define DEFAULT_SCE_MODULES "host0:/usr/local/sce/iop/modules"

hkgWindowPS2::hkgWindowPS2()
: hkgWindow(), m_bFirstDraw(true), m_ceeTimer(HK_NULL)
{
	m_virtualMouseEnabled = true;
	m_viewportResizeByMouse = false; // don't allow viewport resize (as makes no dif on PS2GL at the moment)
	m_viewportBorders = false; // don't draw borders
	m_drawMousePointer = true;
	m_tvDeadZoneH = 20;
	m_tvDeadZoneV = 20;
}

hkgWindowPS2::~hkgWindowPS2()
{
	if (g_sModulePath)
	{
		hkDeallocate<char>( g_sModulePath );
		g_sModulePath = HK_NULL;
	}

//	if (m_ceeTimer)
//		delete m_ceeTimer;
}

bool hkgWindowPS2::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle, void* platformParentHandle)
{
	// ignore all attempts top run in a window :-)

	if (!g_bSystemInit)
	{
		if (g_sModulePath == HK_NULL)
		{
			g_sModulePath = hkAllocate<char>( hkString::strLen( DEFAULT_SCE_MODULES ) + 1, HK_MEMORY_CLASS_DISPLAY );
			hkString::strCpy( g_sModulePath, DEFAULT_SCE_MODULES );
		}

		initSubsystem();
		
//		m_ceeTimer = new CEETimer( CEETimer::Timer0 );
//		mInitTimers( m_ceeTimer, CEETimer::BusClock_256th );
	}
	
	return hkgWindow::initialize(HKG_WINDOW_FULLSCREEN, buffers, 640, 448, name, platformHandle, platformParentHandle);
}

static void _setupDefaultLights()
{
	    // lighting
   GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};

   GLfloat l0_position[] = {0.0f, 1.0f, 1.0f, 0.0f};

#ifdef HK_PLATFORM_PS2
    GLfloat l0_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f };
#else
	GLfloat l0_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f };
#endif

   GLfloat l1_position[] = {0.0f, -20.0f, -80.0f, 1.0f};
   // GLfloat l1_position[] = {0, -1, 1, 0.0};
   GLfloat l1_diffuse[] = {.6f, .6f, .6f, 1.0f };

 
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

   glLightfv(GL_LIGHT0, GL_AMBIENT, black);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, black);
   glLightfv(GL_LIGHT0, GL_POSITION, l0_position);

   glLightfv(GL_LIGHT1, GL_AMBIENT, black);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, black);
   glLightfv(GL_LIGHT1, GL_POSITION, l1_position);

     
   for (int i =2; i < 8; ++i)
	   glDisable(GL_LIGHT0 + i);

//   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
  // glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
 //  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
}


bool hkgWindowPS2::initializeContext()
{
	hkgWindow::initializeContext();
	
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

	glMaterialfv( GL_FRONT, GL_EMISSION, black );
   	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
     
	_setupDefaultLights();

	return true;
}


	
void hkgWindowPS2::setSCEModulePath(const char* str)
{
	if (g_sModulePath)
	{
		hkDeallocate<char>( g_sModulePath );
		g_sModulePath = HK_NULL;
	}

	if (str)
	{
		g_sModulePath =	hkAllocate<char>( hkString::strLen( str ) + 1, HK_MEMORY_CLASS_DISPLAY );
		hkString::strCpy( g_sModulePath, str );
	}
}

enum _tPSM
{ 
	_kPsm32 =	0,
	_kPsm24 =	1,
	_kPsm16 =	2,
	_kPsm16s =	10,
	_kPsm8 =	19,
	_kPsm8h =	27,
	_kPsm4 =	20,
	_kPsm4hh =	44,
	_kPsm4hl =	36,

	_kPsmz32 =	48,
	_kPsmz24 =	49,
	_kPsmz16 =	50,
	_kPsmz16s =	58,

	_kInvalidPsm = -1
};


static void _initGsMemory()
{
   // frame and depth buffer
   pgl_slot_handle_t frame_slot_0, frame_slot_1, depth_slot;
   frame_slot_0 = pglAddGsMemSlot( 0, 70, _kPsm32 );
   frame_slot_1 = pglAddGsMemSlot( 70, 70, _kPsm32 );
   depth_slot = pglAddGsMemSlot( 140, 70, _kPsmz24 );
   // lock these slots so that they aren't allocated by the memory manager
   pglLockGsMemSlot( frame_slot_0 );
   pglLockGsMemSlot( frame_slot_1 );
   pglLockGsMemSlot( depth_slot );

   // create gs memory area objects to use for frame and depth buffers
   pgl_area_handle_t frame_area_0, frame_area_1, depth_area;
   frame_area_0 = pglCreateGsMemArea( 640, 224, _kPsm24 );
   frame_area_1 = pglCreateGsMemArea( 640, 224, _kPsm24 );
   depth_area = pglCreateGsMemArea( 640, 224, _kPsmz24 );
   // bind the areas to the slots we created above
   pglBindGsMemAreaToSlot( frame_area_0, frame_slot_0 );
   pglBindGsMemAreaToSlot( frame_area_1, frame_slot_1 );
   pglBindGsMemAreaToSlot( depth_area, depth_slot );

   // draw to the new areas...
   pglSetDrawBuffers( PGL_INTERLACED, frame_area_0, frame_area_1, depth_area );
   // ...and display from them
   pglSetDisplayBuffers( PGL_INTERLACED, frame_area_0, frame_area_1 );

   // 32 bit

   // 210 == 3 * 70 as each (front/back/depth) buffer takes 70 pages (7*10) 
   
   //****
   // There are a total of 512 8K pages
   //****
   
   // 64x32 : size 1 page (page == 8K == 64*32*4)
   pglAddGsMemSlot( 210, 1, _kPsm32 );
   pglAddGsMemSlot( 211, 1, _kPsm32 );
   pglAddGsMemSlot( 212, 1, _kPsm32 );
   pglAddGsMemSlot( 213, 1, _kPsm32 ); //x4
   
   // 64x64 : size 2
   pglAddGsMemSlot( 214, 2, _kPsm32 );
   pglAddGsMemSlot( 216, 2, _kPsm32 );
   pglAddGsMemSlot( 218, 2, _kPsm32 );
   pglAddGsMemSlot( 220, 2, _kPsm32 );
   pglAddGsMemSlot( 222, 2, _kPsm32 );
   pglAddGsMemSlot( 224, 2, _kPsm32 );
   pglAddGsMemSlot( 226, 2, _kPsm32 );
   pglAddGsMemSlot( 228, 2, _kPsm32 ); 
   pglAddGsMemSlot( 230, 2, _kPsm32 );
   pglAddGsMemSlot( 232, 2, _kPsm32 );
   pglAddGsMemSlot( 234, 2, _kPsm32 );
   pglAddGsMemSlot( 236, 2, _kPsm32 );  
   pglAddGsMemSlot( 238, 2, _kPsm32 );  //x13

   // 128x128 : size 8
   pglAddGsMemSlot( 240, 8, _kPsm32 );
   pglAddGsMemSlot( 248, 8, _kPsm32 );
   pglAddGsMemSlot( 256, 8, _kPsm32 );
   pglAddGsMemSlot( 264, 8, _kPsm32 ); 
   pglAddGsMemSlot( 272, 8, _kPsm32 );
   pglAddGsMemSlot( 280, 8, _kPsm32 );
   pglAddGsMemSlot( 288, 8, _kPsm32 );
   pglAddGsMemSlot( 296, 8, _kPsm32 ); //x8
   
   // 256x256 : size 32
   pglAddGsMemSlot( 304, 32, _kPsm32 );
   pglAddGsMemSlot( 336, 32, _kPsm32 );
   pglAddGsMemSlot( 368, 32, _kPsm32 );
   pglAddGsMemSlot( 400, 32, _kPsm32 ); //4
   
   // 432 -- 512 ?

   pglPrintGsMemAllocation();
}



void hkgWindowPS2::initSubsystem()
{
	if (g_bSystemInit)
		return;

	if (!hkgWindowPS2Pad::initAll())
		hkprintf("Warning: Pad #0 did not initialize\n");

	if ( !pglHasLibraryBeenInitted() ) 
	{
		sceDevVif0Reset();
		sceDevVu0Reset();
		sceDmaReset(1);    
		sceGsResetPath();

		sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);

		int immBufferVertexSize = 64 * 1024;
		
		pglInit( immBufferVertexSize, 1000 );
	}

	// does gs memory need to be initialized?
	if ( !pglHasGsMemBeenInitted() ) 
		_initGsMemory();

	g_bSystemInit = true;
}





bool hkgWindowPS2::setAsCurrent() // only need to call this if you have > 1 window
{
	return true;
}

bool hkgWindowPS2::clearBuffers()
{
	pglBeginGeometry();

	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}

bool hkgWindowPS2::swapBuffers()
{
	finishScene();

	glFlush();
	pglEndGeometry();

	if (!m_bFirstDraw)
		pglFinishRenderingGeometry( PGL_DONT_FORCE_IMMEDIATE_STOP );
	else
		m_bFirstDraw = false;

	pglWaitForVSync();
	pglSwapBuffers();

	pglRenderGeometry();
	
	return true;
}

void hkgWindowPS2::handlePadInput()
{
	hkgWindowPS2Pad::readAll();

	for (unsigned int pad = 0; pad < 2; ++pad)
	{
		hkgPad& hpad = m_pad[pad];	
		hkgWindowPS2Pad& ps2pad = pad? g_Pad1 : g_Pad0;
		
		if (!ps2pad.isValid()) // get stable state
			continue;
			
		// Button change
		if (ps2pad.CurStatus.buttons != ps2pad.LastStatus.buttons)
		{
			unsigned int mask = 0x01;
			for (unsigned int i=0; i < 16; ++i)
			{
				if ( (ps2pad.CurStatus.buttons & mask) != 
			  		 (ps2pad.LastStatus.buttons & mask) )
			  	{
			  		bool state = (ps2pad.CurStatus.buttons & mask) != 0;
					processPadButton( pad, _PS2ButtonMap[i], state);
				}
		
				mask = mask << 1;		
			}		
		}
		
		float px1 = hpad.getStickPosX(0);
		float px2 = hpad.getStickPosX(1);
		float py1 = hpad.getStickPosY(0);
		float py2 = hpad.getStickPosY(1);

		float lspy = ps2pad.CurStatus.leftStick.yPos;
		float lspx = ps2pad.CurStatus.leftStick.xPos;
		float rspy = ps2pad.CurStatus.rightStick.yPos;
		float rspx = ps2pad.CurStatus.rightStick.xPos;

		//printf( "pad%d: lx %.3f  ly %.3f  rx %.3f  ry %.3f\n", pad, lspx, lspy, rspx, rspy);
		bool lsmoving = m_virtualMouseEnabled && ((hkg_fabs(lspx) > 0.1f) || (hkg_fabs(lspy) > 0.1f));
		bool rsmoving = m_virtualMouseEnabled && ((hkg_fabs(rspx) > 0.1f) || (hkg_fabs(rspy) > 0.1f));
		
		if ( lsmoving || ( hkg_fabs(lspx - px2) > 0.05f ) || ( hkg_fabs(lspy - py2) > 0.05f ) )
			processPadStickMove( pad, 1, lspx, lspy );
		if ( rsmoving || ( hkg_fabs(rspx - px1) > 0.05f ) || ( hkg_fabs(rspy - py1) > 0.05f ) )
			processPadStickMove( pad, 0, rspx, rspy );
	}
}

HKG_WINDOW_MSG_STATUS hkgWindowPS2::peekMessages(bool inViewportResize)
{
	// step the mouse state etc in preperation for new loop state 

	HKG_WINDOW_MSG_STATUS status = HKG_WINDOW_MSG_CONTINUE;

	if (!inViewportResize && (m_viewportResizeArray.getSize() > 0))
		status = doViewportResizeLoop();

	stepInput(); 
	handlePadInput();

	return status;
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
