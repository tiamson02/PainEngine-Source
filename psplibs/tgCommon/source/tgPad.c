/* SCE CONFIDENTIAL
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
*/
/*
* PlayStation(R) Portable (PSP(TM)) Graphic Library Samples
*
* Copyright (C) 2005 Sony Computer Entertainment Europe
* All Rights Reserved.
*
* tgCommon - tgPad.c
*   Controller functions
*             1. Can remove centre area for analog stick
*             2. Provides toggling buttons
*             3. Provides velocity on analogue stick
*
*   Date            Design      Log
*  ----------------------------------------------------
*   2004-08-23      breugelmans initial implementation
*   2005-03-24      MikeKav added combine button and comments
*   2005-03-31      MikeKav, added HOLD and INTERCEPTED button handling
*/

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "tgPad.h"
#include "tgTypes.h"
#include <math.h>

#ifdef _PSPEMU
u8 _padBuffer[4] __attribute__((aligned(16)));
#else
// This allows us to buffer up to 8 input points, so that we do not lose any input
// however it's not used at this time
SceCtrlData _padBuffer[8] __attribute__((aligned(16))) = {{0}};
#endif
//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// tgPadInit 
// Description: Initialises pad state
//              
// Parameters: pad - data for pad
//         deadzone - size of deadzone (for example 0.5f)
//         sensitivity - curve for response (default should be 1.0f)
//-----------------------------------------------------------------------------
void tgPadInit( TgPadData *pad, float deadzone, float sensitivity ) 
{

	// clear all button values	
	pad->stickX		= 0.0f;
	pad->stickY		= 0.0f;

	pad->stickVelX		= 0.0f;
	pad->stickVelY		= 0.0f;
		
	pad->button		= 0;
	pad->toggleBtn		= 0;

	pad->pressedBtn		= 0;
	pad->releasedBtn	= 0;

	pad->combineBtn		= 0;
	pad->combinePressedBtn	= 0;

	// Ensure deadzone and sensitivity is set to a meaningful value
	if( deadzone > 0.5f )
		deadzone = 0.5f;
	if( sensitivity <= 0.1f )
		sensitivity = 0.1f;

	// Set sensitivity and deadzone
	pad->deadZone		= deadzone;
	pad->sensitivity	= sensitivity;

#ifndef _PSPEMU

	// set sample mode to retrieve analog and digital data, and set the time to collect data
	// from the hardware to once every VSync
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG);
	sceCtrlSetSamplingCycle(0);
#endif
	return;
}


//-----------------------------------------------------------------------------
// tgPadRead
// Description: Reads buttons, pressures and sticks
// Parameters: pad - data for current pad
// Note: Returns 1 on success
//       Stick values are float between +1.0f and -1.0f
//-----------------------------------------------------------------------------
int tgPadRead( TgPadData *pad )
{

//	u32	oldToggleBtn;
	u32	oldBtn;
	float	stickX, stickY;
	//float	oldStickX, oldStickY;

	oldBtn = pad->button;
//	oldToggleBtn = pad->toggleBtn;

#ifdef _PSPEMU
	pad->button = sceEmuAnalogRead(_padBuffer);	
#else
	sceCtrlReadBufferPositive(_padBuffer, 8);
	pad->button = _padBuffer[0].Buttons;
#endif

	pad->pressedBtn = ( pad->button & ~oldBtn );	
	pad->releasedBtn = ( ~pad->button & oldBtn );
	pad->toggleBtn  = ( pad->pressedBtn ^ pad->toggleBtn );	

	if( !( pad->button & (TG_PAD_HOLD | TG_PAD_INTERCEPTED ) ) )
	{
#ifdef _PSPEMU
		stickX  = (float) (((s16)_padBuffer[2]) - 128) / 128.0f;
		stickY  = (float) (((s16)_padBuffer[3]) - 128) / 128.0f;

#else
		stickX  = (float) (((s16)_padBuffer[0].Lx) - 128) / 128.0f;
		stickY  = (float) (((s16)_padBuffer[0].Ly) - 128) / 128.0f;
#endif


		pad->oldStickX  = pad->stickX;
		pad->oldStickY  = pad->stickY;
		
		pad->stickVelX  = stickX  - pad->oldStickX;
		pad->stickVelY  = stickY  - pad->oldStickY;

		if (fabs(stickX)<pad->deadZone)  stickX = 0.0f;
		if (fabs(stickY)<pad->deadZone)  stickY = 0.0f;

/*
	Todo: Create adjustable stick response function

	stickX *= sensitivity;
	stickY *= sensitivity;
*/
		pad->stickX  = stickX;
		pad->stickY  = stickY;

		pad->combineBtn = pad->button;
		if( stickY > 0.0f )
			pad->combineBtn |= TG_PAD_DOWN;
		else if( stickY < 0.0f )
			pad->combineBtn |= TG_PAD_UP;

		if( stickX > 0.0f )
			pad->combineBtn |= TG_PAD_RIGHT;
		else if( stickX < 0.0f )
			pad->combineBtn |= TG_PAD_LEFT;

		pad->combinePressedBtn = ( pad->button & ~oldBtn );
		if( pad->oldStickY == 0.0f )
		{
			if( stickY > 0.0f )
       	        		pad->combinePressedBtn |= TG_PAD_DOWN;
       		 	else if( stickY < 0.0f )
               	 	pad->combinePressedBtn |= TG_PAD_UP;
		}

		if( pad->oldStickX == 0.0f )
        	{
			if( stickX > 0.0f )
				pad->combinePressedBtn |= TG_PAD_RIGHT;
			else if( stickX < 0.0f )
				pad->combinePressedBtn |= TG_PAD_LEFT;
        	}
	}
	else
	{
		pad->combinePressedBtn	= 0;
		pad->combineBtn		= 0;
		pad->stickX		= 0.0f;
		pad->stickY		= 0.0f;
		pad->stickVelX		= 0.0f;
		pad->stickVelY		= 0.0f;
	}

	return TG_SUCCESS;
}

//-----------------------------------------------------------------------------
// tgPadClearToggleBtn
// Description: Toggle buttons stay in same state until cleared by this function
// Parameters: pad - data for current pad
//-----------------------------------------------------------------------------
void tgPadClearToggleBtn(TgPadData *pad)
{
	pad->toggleBtn = 0x0;
}

