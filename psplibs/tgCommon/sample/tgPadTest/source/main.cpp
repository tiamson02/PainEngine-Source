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
//	Function 		:	Sample program showing controller usage on PSP
//						
//	Comments		:	Draws the controller state on screen, including what
//						buttons are pressed, analog stick values and a example
//						of four menus reacting to the different types of button
//						presses that are available
//									
//	Date	 		:	04/04/2005
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

#include "tgPad.h"

// PRX specific information
SCE_MODULE_INFO( tgPadTest, 0, 1, 1 );
SceInt32 sce_newlib_heap_kb_size	= ( 1024 * 20 ); // allocate 20MB to the heap
SceInt32 sce_newlib_stack_kb_size	= ( 256 ); // allocate 256KB to stack

static char		disp_list[0x10000] __attribute__((aligned(64)));
static ScePspFMatrix4	matrix_stack[8+8+32+0];

// triangle point data type, consists of a colour value and a vertex
typedef	struct {
	u_int32		c;
	float		vertex[3];
} PRIM;

// trangle data
static PRIM	model[]={
	{0xff0000ff,	 { -0.5f,	 0.5f,	 0.0f}},
	{0xff00ffff,	 {  0.0f,	-0.5f,	 0.0f}},
	{0xff00ff00,	 {  0.5f,	 0.5f,	 0.0f}},
};

// triangle rotation angle/speed
static float	fAngle = 0.0f;
#define ROTATION_SPEED ( 1.0f )

// Box data for drawing the 2D white box outline
#define BOX_LENGTH ( 100 )

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

// pad variables
TgPadData pad;
#define PAD_DEADZONE (0.5f)
#define PAD_SENSITIVITY (1.0f)

// starting position for the block of text displaying the defines
#define DEFINE_STRING_POSX (290)
#define DEFINE_STRING_POSY (10)

// starting position for the block of text displaying the analog stick values
#define STICK_XPOS (290)
#define STICK_YPOS (150)

// starting position for the block of text displaying the pad values
#define PAD_XPOS (0)
#define PAD_YPOS (160)

// starting position for the menus
#define MENU_XPOS (60)
#define MENU_YPOS (230)

// Data for checking and displaying the current button state
#define NUM_PAD_BUTTONS (14)
char* padBtnString[NUM_PAD_BUTTONS] = {	"SELECT   ", "START    ",
					"UP       ", "RIGHT    ", "DOWN     ", "LEFT     ",
					"L1       ", "R1       ",
					"TRIANGLE ", "CIRCLE   ", "CROSS    ", "SQUARE   ",
					"INTERCEPT", "HOLD     "  };

int padBtnInt[NUM_PAD_BUTTONS] = {	1, 8, 16, 32, 64, 128, 256, 512, 4096, 8192, 16384, 32768, (1<<16), (1<<17) };

// variables to handle menus
int iHighlight[4] = { 0,0,0,0 };
int iFrameCount = 0;

// Function Prototypes
static void	RenderScene(void);
bool		InitSys( void );
void		DrawMenu( int iHighlightPos, int iNumberItems, int iXpos, int iYpos, char *szText );
void		DrawPadDefines( void );
void		ProcessHighlight( int *iHighlight );
void		ProcessMenus( void );

// Description
// This function Renders the output to the screen
//
// Parameters
//
// Returns
//
static void RenderScene(void)
{
	char szString[250];		// string for holding text to be rendered on screen

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
	
	// rotation about Z axis, note unlike OpenGl, the PSP uses radians
	sceGumRotateZ( SCEGU_RAD( fAngle) );

	sceGumDrawArray( SCEGU_PRIM_TRIANGLES,
				SCEGU_COLOR_PF8888|SCEGU_VERTEX_FLOAT,
				3, 0, model);

	sceGumPopMatrix();

	DrawPadDefines();

	// display all buttons, and the current state if pressed or not
	for ( int i = 0 ; i < NUM_PAD_BUTTONS; i++ )
	{
		sprintf( szString, "%s = %d,%d\n", padBtnString[i], ( ( pad.button & padBtnInt[i] ) > 0 ), pad.button & padBtnInt[i] );
		sceGuDebugPrint( 0, 10 + ( 10 * i ), 0x00ffffff, szString );
	}

	// display all raw button data
	sprintf( szString, "PAD VALUES\n==========\nbutton      = %04d\ntoggleBtn   = %04d\npressedBtn  = %04d\nreleasedBtn = %04d\ncombineBtn  = %04d\ncombinePressedBtn = %04d\n", pad.button, pad.toggleBtn,
				pad.pressedBtn, pad.releasedBtn, pad.combineBtn, pad.combinePressedBtn );
	sceGuDebugPrint( PAD_XPOS, PAD_YPOS, 0x00ffffff, szString );

	sprintf( szString, "STICK VALUES\n============\nsensitivity = %02.02f\ndeadzone    = %02.02f", pad.sensitivity, pad.deadZone );
	sceGuDebugPrint( STICK_XPOS, STICK_YPOS, 0x00ffffff, szString );

	sprintf( szString, "stickX    = %02.02f\nstickY    = %02.02f\nstickVelX = %02.02f\nstickVelY = %02.02f", pad.stickX, pad.stickY, pad.stickVelX, pad.stickVelY );
	sceGuDebugPrint( STICK_XPOS, STICK_YPOS + 40, 0x00ffffff, szString );

	ProcessMenus();

	sceGuFinish();
	sceGuSync( SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT );
}

// Description
// Displays text and highlight for an example menu
//
// Parameters
// iHighlightPos - current highlighted element of menu
// iNumberItems - Number of elements of a menu to draw
// iXpos - X start position of menu
// iYpos - Y start position of menu
// szText - pointer of text to display as a menu item
//
// Returns
// true on successful completion
void DrawMenu( int iHighlightPos, int iNumberItems, int iXpos, int iYpos, char *szText )
{
	for( int i = 0; i < iNumberItems; i++ )
	{
		if( i == iHighlightPos )
			sceGuDebugPrint( iXpos, iYpos + ( 10 * i ), 0x00ffffff, szText );
		else
			sceGuDebugPrint( iXpos, iYpos + ( 10 * i ), 0x00ffff00, szText );
	}

	return;
}

// Description
// Initialise graphics and display
//
// Parameters
//
// Returns
// true on successful completion
bool InitSys( void )
{
	tgPadInit( &pad, PAD_DEADZONE, PAD_SENSITIVITY );
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

// Description
// Update angle of rotation for the triangle on screen
//
// Parameters
//
// Returns
//
static void UpdateFrame(void)
{

	tgPadRead( &pad );

	// rotation based on degrees, from 0 to 360
	fAngle += ROTATION_SPEED;
	if ( fAngle >= 360.0f)
		fAngle -= 360.0f;
}

// Description
// Draw the text showing all pad defines and values to the right of the screen
//
// Parameters
//
// Returns
//
void DrawPadDefines( void )
{

	char szDefineString[50];

	sprintf( szDefineString, "%s,  (%d)", "TG_PAD_SELECT" ,TG_PAD_SELECT );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,   (%d)", "TG_PAD_START" ,TG_PAD_START );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 10, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,      (%d)", "TG_PAD_UP" ,TG_PAD_UP );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 20 , 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,   (%d)", "TG_PAD_RIGHT" ,TG_PAD_RIGHT );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 30, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,    (%d)", "TG_PAD_DOWN" ,TG_PAD_DOWN );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 40, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,    (%d)", "TG_PAD_LEFT" ,TG_PAD_LEFT );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 50, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,      (%d)", "TG_PAD_L1" ,TG_PAD_L1 );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 60, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,      (%d)", "TG_PAD_R1" ,TG_PAD_R1 );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 70 , 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,(%d)", "TG_PAD_TRIANGLE" ,TG_PAD_TRIANGLE );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 80, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,  (%d)", "TG_PAD_CIRCLE" ,TG_PAD_CIRCLE );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 90, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,   (%d)", "TG_PAD_CROSS" ,TG_PAD_CROSS );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 100, 0x00ffffff, szDefineString );
	sprintf( szDefineString, "%s,  (%d)", "TG_PAD_SQUARE" ,TG_PAD_SQUARE );
	sceGuDebugPrint( DEFINE_STRING_POSX, DEFINE_STRING_POSY + 110, 0x00ffffff, szDefineString );

	return;
}

// Description
// Ensures that the menu highlight stays within the bounds of the menu elements
//
// Parameters
// iHighlight - pointer to value to check
//
// Returns
//
void ProcessHighlight( int *iHighlight )
{
	if( *iHighlight < 0)
		*iHighlight = 3;
	else if( *iHighlight > 3)
		*iHighlight = 0;

	return;
}

// Description
// Handles all input to the example menus
//
// Parameters
//
// Returns
//
void ProcessMenus( void )
{
	// Only update button press every 5th frame, to allow you to see scrolling
	// better
	if( iFrameCount == 0 )
	{
		iFrameCount = 5;

		if( pad.button & TG_PAD_UP )
		{
			iHighlight[0]--;
			ProcessHighlight( &iHighlight[0] );
		}
		else if( pad.button & TG_PAD_DOWN )
		{
			iHighlight[0]++;
			ProcessHighlight( &iHighlight[0] );
		}

		if( pad.combineBtn & TG_PAD_UP )
		{
			iHighlight[2]--;
			ProcessHighlight( &iHighlight[2] );
		}
		else if( pad.combineBtn & TG_PAD_DOWN )
		{
			iHighlight[2]++;
			ProcessHighlight( &iHighlight[2] );
		}
	}
	else
	{
		iFrameCount--;
	}

	if( pad.pressedBtn & TG_PAD_UP )
	{
		iHighlight[1]--;
		ProcessHighlight( &iHighlight[1] );

	}
	else if( pad.pressedBtn & TG_PAD_DOWN )
	{
		iHighlight[1]++;
		ProcessHighlight( &iHighlight[1] );
	}

	if( pad.combinePressedBtn & TG_PAD_UP )
	{
		iHighlight[3]--;
		ProcessHighlight( &iHighlight[3] );
	}
	else if( pad.combinePressedBtn & TG_PAD_DOWN )
	{
		iHighlight[3]++;
		ProcessHighlight( &iHighlight[3] );
	}

	DrawMenu( iHighlight[0], 4, MENU_XPOS,			MENU_YPOS, "BUTTON" );
	DrawMenu( iHighlight[1], 4, MENU_XPOS + 60,		MENU_YPOS, "PRESSED" );
	DrawMenu( iHighlight[2], 4, MENU_XPOS + 125,	MENU_YPOS, "COMBINE" );
	DrawMenu( iHighlight[3], 4, MENU_XPOS + 190,	MENU_YPOS, "COMBINEPRESS" );
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
