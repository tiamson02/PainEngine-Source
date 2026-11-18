/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*/

//**************************************************************************
//	FILE			:	main.cpp
//
//	Project  		:	
//
//	By		 		:	Mike Kavallierou
//
//	Function 		:	Basic Template for PSP
//						
//	Comments		:	Draws some debug text, a 2D box and a triangle rotating
//						in the centre of the screen
//									
//	Date	 		:	23/02/2005
//	Log				:	Created
//**************************************************************************

#include <stdio.h>
#include <psptypes.h>
#include <thread.h>
#include <libgu.h>
#include <libgum.h>
#include <displaysvc.h>
#include <ctrlsvc.h>
#include <moduleexport.h>
#include <malloc.h>

// PRX specific information
SCE_MODULE_INFO( tgBasic, 0, 1, 1 );
SceInt32 sce_newlib_heap_kb_size	= ( 1024 * 20 ); // allocate 20MB to the heap
SceInt32 sce_newlib_stack_kb_size	= ( 256 ); // allocate 256KB to stack

static char		disp_list[0x10000] __attribute__((aligned(64)));
static ScePspFMatrix4	matrix_stack[8+8+32+0];

// triangle point data type, consists of a colour value and a vertex
typedef	struct {
	u_int32		c;
	float		vertex[3];
} PRIM;

// triangle data
static PRIM	model[]={
	{0xff0000ff,	 { -0.5f,	 0.5f,	 0.0f}},
	{0xff00ffff,	 {  0.0f,	-0.5f,	 0.0f}},
	{0xff00ff00,	 {  0.5f,	 0.5f,	 0.0f}},
};

// rotation angle/speed
static float	fAngle = 0.0f;
#define ROTATION_SPEED ( 1.0f )

#define BOX_LENGTH ( 100 )

// Description
// Box data for drawing the 2D white box outline
static short	sBoxData[]={
	( SCEGU_SCR_WIDTH / 2 ) - ( BOX_LENGTH / 2 ),	( SCEGU_SCR_HEIGHT / 2 ) - ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) - ( BOX_LENGTH / 2 ),	( SCEGU_SCR_HEIGHT / 2 ) + ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) - ( BOX_LENGTH / 2 ),   ( SCEGU_SCR_HEIGHT / 2 ) + ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) + ( BOX_LENGTH / 2 ),   ( SCEGU_SCR_HEIGHT / 2 ) + ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) + ( BOX_LENGTH / 2 ),   ( SCEGU_SCR_HEIGHT / 2 ) + ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) + ( BOX_LENGTH / 2 ),	( SCEGU_SCR_HEIGHT / 2 ) - ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) + ( BOX_LENGTH / 2 ),	( SCEGU_SCR_HEIGHT / 2 ) - ( BOX_LENGTH / 2),	0,
	( SCEGU_SCR_WIDTH / 2 ) - ( BOX_LENGTH / 2 ),	( SCEGU_SCR_HEIGHT / 2 ) - ( BOX_LENGTH / 2),	0,
};

// Description
//	Testing Testing
static void RenderScene(void)
{
	sceGuStart(SCEGU_IMMEDIATE, disp_list, sizeof(disp_list));

	// clear COLOUR/STENCIL/DEPTH
	sceGuClear(SCEGU_CLEAR_ALL);

	// set colour for box lines
	sceGuColor(0xffffffff);
	
	// draw bounding box in screen coordinates
	sceGumDrawArray( SCEGU_PRIM_LINES,				// primitive type to draw
				SCEGU_THROUGH|SCEGU_VERTEX_SHORT,	// primitive mode, screen coordinates | signed 16 bit value
				8,					// number of vertices
				0,					// index into array of data
				sBoxData);					// pointer to data

	sceGumPushMatrix();
	sceGumLoadIdentity();

	sceGuDebugPrint( 10, 10, 0x00ffffff, "BASIC FRAMEWORK" );

	// rotation about Z axis, note unlike OpenGl, the PSP uses radians
	sceGumRotateZ( SCEGU_RAD( fAngle) );

	sceGumDrawArray( SCEGU_PRIM_TRIANGLES,
				SCEGU_COLOR_PF8888|SCEGU_VERTEX_FLOAT,
				3, 0, model);

	sceGumPopMatrix();

	sceGuFinish();
	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT );
}

bool InitSys(void)
{

	// Initialise graphics library
	sceGuInit();

	sceGuStart( SCEGU_IMMEDIATE, disp_list, sizeof(disp_list) );

	sceGuDrawBuffer( SCEGU_PF5551,		// pixel format
			 SCEGU_VRAM_BP_0,	// base pointer
			 SCEGU_VRAM_WIDTH );	// width of draw buffer in VRAM

	sceGuDispBuffer( SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT, SCEGU_VRAM_BP_1, SCEGU_VRAM_WIDTH );
	sceGuDepthBuffer( SCEGU_VRAM_BP_2, SCEGU_VRAM_WIDTH );

	sceGuOffset( SCEGU_SCR_OFFSETX, SCEGU_SCR_OFFSETY );
	sceGuViewport( 2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT );

	sceGuDepthRange( 50000, 10000 );

	sceGuScissor( 0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT );
	sceGuEnable( SCEGU_SCISSOR_TEST );

	sceGuDepthFunc( SCEGU_GEQUAL );
	sceGuEnable( SCEGU_DEPTH_TEST );

	sceGuFrontFace( SCEGU_CW );

	sceGuShadeModel( SCEGU_SMOOTH );

	sceGuEnable( SCEGU_CLIP_PLANE );

	sceGuDisable( SCEGU_TEXTURE );
	sceGuDisable( SCEGU_FOG );

	sceGuAmbient( 0xffdddddd );

	sceGumSetMatrixStack( &matrix_stack[0], 8, 8, 32, 0 );
	sceGumMatrixMode( SCEGU_MATRIX_PROJECTION );

	sceGumLoadIdentity();
	sceGumPerspective( SCEGU_RAD(45.0f), SCEGU_SCR_ASPECT,
					1.000000f, 100.000000f );

	sceGumMatrixMode(SCEGU_MATRIX_VIEW);
	sceGumLoadIdentity();

	ScePspFVector3	vEye;
	ScePspFVector3	vLookAt;
	ScePspFVector3	vUp;

	// Eye point, where in the world we are, in the case back away from the origin
	vEye.x = 0.0f;
	vEye.y = 0.0f;
	vEye.z = 10.0f;

	// where we are looking to, as the polygon is at 0,0,0
	vLookAt.x = 0.0f;
	vLookAt.y = 0.0f;
	vLookAt.z = 0.0f;

	vUp.x = 0.0f;
	vUp.y = 1.0f;
	vUp.z = 0.0f;

	sceGumLookAt( &vEye, &vLookAt, &vUp );

	sceGumMatrixMode( SCEGU_MATRIX_WORLD );

	// colour to set background to
	sceGuClearColor( 0x00d00020 );

	sceGuClearDepth( 0 );
	sceGuClearStencil( 0 );

	sceGuFinish();

	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT );

	sceDisplayWaitVblankStart();

	sceGuDisplay( SCEGU_DISPLAY_ON );

	return true;
}

static void UpdateFrame(void)
{

	// rotation based on degrees, from 0 to 360
	fAngle += ROTATION_SPEED;
	if ( fAngle >= 360.0f)
		fAngle -= 360.0f;
}

int main(void)
{
	if( ! InitSys() )
	{
		printf( "Failed to Initialise, exiting\n" );
		return 0;
	}

	// setup specific application variables
	fAngle = 0.0f;

	while ( true )
	{

		// update
		UpdateFrame();

		// display the graphics
		RenderScene();

		// wait for v-blank
		sceDisplayWaitVblankStartCB();

		// display back buffer to display buffer
		sceGuSwapBuffers();

	}

	sceGuTerm();

	return 0;
}
