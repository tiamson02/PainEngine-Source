/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgPad.h
*   Controller functions
*             1. Can remove centre area for analog stick
*             2. Provides toggling buttons
*             3. Provides velocity on analogue stick
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-08-23      breugelmans initial implementation
*   2005-03-24      MikeKav, added comments and combine button
*   2005-03-31      MikeKav, added HOLD and INTERCEPTED button handling
*/

#include "tgTypes.h" 

#ifdef _PSPEMU
	#include <libemu.h>
#else
	#include <ctrlsvc.h>
#endif

#ifndef _TG_PAD_H_
#define _TG_PAD_H_

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

#ifdef _PSPEMU

	#define	TG_PAD_LUP	SCEEMU_PAD_UP
	#define	TG_PAD_LDOWN    SCEEMU_PAD_DOWN
	#define	TG_PAD_LLEFT    SCEEMU_PAD_LEFT
	#define	TG_PAD_LRIGHT   SCEEMU_PAD_RIGHT
		
	#define	TG_PAD_RUP      SCEEMU_PAD_C
	#define	TG_PAD_RDOWN    SCEEMU_PAD_B
	#define	TG_PAD_RLEFT    SCEEMU_PAD_D
	#define	TG_PAD_RRIGHT   SCEEMU_PAD_A

	#define	TG_PAD_L1       SCEEMU_PAD_L1
	#define	TG_PAD_R1       SCEEMU_PAD_R1
		
	#define	TG_PAD_START    SCEEMU_PAD_START
	#define	TG_PAD_SELECT   SCEEMU_PAD_SELECT

#else

	#define	TG_PAD_LUP	SCE_CTRL_UP				//left side direction button
	#define	TG_PAD_LDOWN    SCE_CTRL_DOWN		//left side direction button
	#define	TG_PAD_LLEFT    SCE_CTRL_LEFT		//left side direction button
	#define	TG_PAD_LRIGHT   SCE_CTRL_RIGHT		//left side direction button
		
	#define	TG_PAD_RUP      SCE_CTRL_TRIANGLE	//right side button
	#define	TG_PAD_RDOWN    SCE_CTRL_CROSS		//right side button
	#define	TG_PAD_RLEFT    SCE_CTRL_SQUARE		//right side button 
	#define	TG_PAD_RRIGHT   SCE_CTRL_CIRCLE		//right side button


	#define	TG_PAD_L1       SCE_CTRL_L1			//left shoulder button
	#define	TG_PAD_R1       SCE_CTRL_R1			//right shoulder button
		
	#define	TG_PAD_START    SCE_CTRL_START			//start button
	#define	TG_PAD_SELECT   SCE_CTRL_SELECT			//select button

	#define	TG_PAD_INTERCEPTED	SCE_CTRL_INTERCEPTED	//virtual button
	#define	TG_PAD_HOLD			SCE_CTRL_HOLD			//hold button

#endif

#define	TG_PAD_TRIANGLE TG_PAD_RUP		//right side button
#define	TG_PAD_CROSS    TG_PAD_RDOWN	//right side button
#define	TG_PAD_SQUARE   TG_PAD_RLEFT	//right side button
#define	TG_PAD_CIRCLE   TG_PAD_RRIGHT	//right side button

#define TG_PAD_UP 	TG_PAD_LUP			//left side direction button
#define TG_PAD_DOWN 	TG_PAD_LDOWN	//left side direction button
#define TG_PAD_LEFT 	TG_PAD_LLEFT	//left side direction button
#define TG_PAD_RIGHT 	TG_PAD_LRIGHT	//left side direction button

// Pad data structure to store button state and analog stick values
typedef struct _TgPadData
{
	u32 button;			// current state of all buttons
	u32 toggleBtn;			// set on button press, cancelled next time same button is pressed

	u32 pressedBtn;			// when pressed, (not held) state is set for one frame
	u32 releasedBtn;		// when released, state is set for one frame

	u32 combineBtn;			// current state of all buttons, if analog stick left/right/up/down is used, the related button state is set
	u32 combinePressedBtn;		// As pressedBtn, if analog stick left/right/up/down is used, the related button state is set

	float deadZone;			// value from analog stick before stickX and stickY will start being filled with data
	float sensitivity;		// TODO: this value is not used at this time

	float stickX;			// analog stick X value ( +/- 1.00 )
	float stickY;			// analog stick Y value ( +/- 1.00 )

	float stickVelX;		// analog stick X velocity
	float stickVelY;		// analog stick Y velocity

    float oldStickX;        // analog stick X previous value ( +/- 1.00 )
    float oldStickY;        // analog stick Y previous value ( +/- 1.00 )
}
TgPadData;//--- __attribute__((aligned(64)));

//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#if defined (__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void  tgPadInit( TgPadData *pad, float deadZone, float sensitivity );
int   tgPadRead( TgPadData *pad ); //--- returns success(1) or fail(0)
void  tgPadClearToggleBtn( TgPadData *pad );

#if defined (__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif // TG_PAD_H_
