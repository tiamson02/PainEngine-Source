//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellPad - library for digital, dualshock and dualshock2 pads
//
// $Workfile: shellPad.c $
// $Author: Mbreugel $
// $Date: 10/02/04 15:44 $
// $Revision: 4 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
// Description: Simple functions to handle all types of standard pad
//              1. Returns pad type:
//                         0=none connected
//                         1=digital
//                         2=analog (PlayStation1 dualshock)
//                         3=pressure sensitive(PS2 dualshock2)
//              2. Locks analog mode for sticks
//              3. Removes dead zone from sticks
//              4. Provides sticky and toggling buttons
//              5. Provides pressure sensitivity
//              6. Provides velocity on both analogue sticks and buttons
//              7. Enables easy motor setting for vibration
//              8. Works multiple pads & with LIBMTAP if necessary
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"

static int _modules[2]  = { -1, -1 };

//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellPadInit 
// Description: Loads modules necessary for pad functions or multitap functions
// parameters: enableMultitaps (1=enable multitaps)
//-----------------------------------------------------------------------------
void shellPadInit(int enableMultitaps) 
{
	shellFileLoadStartModule("sio2man", 0, "", NULL);

	if (enableMultitaps) 
	{
		_modules[1] = shellFileLoadStartModule("mtapman", 0, "", NULL);
		_modules[0] = shellFileLoadStartModule("padman", 0, "", NULL);
	}
	else
	{
		shellFileLoadStartModule("padman", 0, "", NULL);
	}
	
	// Only for multitap
	if (enableMultitaps)
    {
		sceMtapInit();
		sceMtapPortOpen(0);
		sceMtapPortOpen(1);
    }
}

//-----------------------------------------------------------------------------
// shellPadClose
// Description: Unloads modules used by pad functions or multitap functions
// parameters: disableMultitaps (1=disable multitaps)
//-----------------------------------------------------------------------------
void shellPadClose(int disableMultitaps) 
{
	if (disableMultitaps) 
	{
		sceMtapEnd();
		scePadPortClose(0,0);
		scePadEnd();
		shellFileStopUnloadModule(_modules[1]);
		shellFileStopUnloadModule(_modules[0]);
	}
	else
	{
		scePadPortClose(0,0);
		scePadEnd();
		shellFileStopUnloadModule(_modules[0]);
	}
}



//-----------------------------------------------------------------------------
// shellPadSetMotors
// Description: Sets motors on and off
// Params: padStruct - data for current pad port and slot
//         motor1 - is only on(1) or off(0)
//         motor2 - often requires a high(>0x50) initial value to start it turning
//-----------------------------------------------------------------------------
void shellPadSetMotors(ShellPadData *padStruct, int motor1, int motor2)
{
	if (padStruct->pad_type<SHELLPAD_DUALSHOCK) return;
	
	padStruct->act_direct[0] = (motor1>0);
	padStruct->act_direct[1] = (unsigned char) motor2;
	scePadSetActDirect(padStruct->port, padStruct->slot,
					   padStruct->act_direct);
}

//-----------------------------------------------------------------------------
// shellPadInitPort
// Description: Initialises pad port
// Params: padStruct - data for current pad port and slot
//         port - 0 or 1
//         slot - 0 or 0-3 if using multitap
//         deadzone - size of deadzone 32 is a good one to start with
//-----------------------------------------------------------------------------
void shellPadInitPort(ShellPadData *padStruct, void *dmaBuf, int port, int slot, int deadzone)
{
	int i;
	static int firstPadStarted=0;
	
	//ASSERT(GET_ALIGNMENT(padStruct)>=64 , "padStruct not 64byte aligned");
#ifdef DEBUG
    if (GET_ALIGNMENT(padStruct) < 64){
        printf("shellPadInitPort(): ERROR-> padStruct not 64byte aligned!\n");
        ASSERT(0);
    }
#endif

	for (i=0; i<6; i++)
    {
		padStruct->act_direct[i] = 0;
		padStruct->act_align[i] = 0xff;
    }
	
	for (i=0; i<12; i++)
		padStruct->oldPressure[i] = 0;
	
	
	padStruct->act_align[0] = 0; // Offset 0 for motor0
	padStruct->act_align[1] = 1; // Offset 1 for motor1
	
	padStruct->port = port;
	padStruct->slot = slot;
	padStruct->phase = 0;
	padStruct->rpad = 0;
	padStruct->frameCount =0;
	
	padStruct->term_id =0;
	if (!firstPadStarted)
    {
		scePadInit(0);
		firstPadStarted=1;
    }
	
	// For velocity
	padStruct->oldLeftStickX = 0;
	padStruct->oldLeftStickY = 0;
	padStruct->oldRightStickX = 0;
	padStruct->oldRightStickY = 0;
	
    padStruct->prevLeftStickX = 0;
    padStruct->prevLeftStickY = 0;
    padStruct->prevRightStickX = 0;
    padStruct->prevRightStickY = 0;

	padStruct->leftStickVelX = 0;
	padStruct->leftStickVelY = 0;
	padStruct->rightStickVelX = 0;
	padStruct->rightStickVelY = 0;
	
	
	scePadPortOpen( port, slot, dmaBuf ); 
	padStruct->button = 0;
	padStruct->stickyBtn = 0;
	padStruct->toggleBtn = 0;
	padStruct->previousToggleBtn = 0;
	padStruct->toggledBtn = 0;
	padStruct->previousBtn = 0;
	padStruct->deadZone = deadzone;
	
	PRINT("shellPad: port=%d slot=%d opened", port, slot);
}


//-----------------------------------------------------------------------------
// shellPadRead
// Description: Reads buttons, pressures and sticks for pads
// Params: padStruct - data for current pad port and slot
// Note: Returns padtype:0=none, 1=digital,2=analog,3=pressure
//       Stick values are signed chars where 0 is centre
//-----------------------------------------------------------------------------
EShellPadType shellPadRead(ShellPadData *padStruct)
{
	u_int changedBtn;
	short leftX = 0;
	short leftY = 0;
	short rightX = 0;
	short rightY = 0;
	unsigned char rdata[32];
	
	int iPressureBtn;
	int id;
	int ext_id=0;
	int state;
	int rterm_id = 0;
	int checkError =0;
	unsigned short paddata, tpad;
	
	rterm_id = padStruct->term_id;

	padStruct->frameCount++; // To count how slow it is to init pads
	state = scePadGetState(padStruct->port, padStruct->slot );
	
	if ((state==scePadStateDiscon) || (state==scePadStateError) )  // Added scePadStateError
    {
		padStruct->phase = 0;
		padStruct->pad_type = SHELLPAD_DISCONNECT;

    }    

	switch(padStruct->phase)
    {
    case 0:
		if (state != scePadStateStable && state != scePadStateFindCTP1) 
			break;
		id = scePadInfoMode(padStruct->port, padStruct->slot, InfoModeCurID, 0);
		if (id==0) break; 
		
		ext_id = scePadInfoMode(padStruct->port, padStruct->slot, InfoModeCurExID, 0);
		if (ext_id>0) id = ext_id;
		
		switch(id)
		{
		case 4: // Digital controller
			padStruct->phase = 40; // Try for analog(dualshock)
			break;
		case 7: // Dualshock2 controller
			padStruct->phase = 70;
			break;
		default:
			padStruct->phase = 99;
			break;
		}
		break;

		// Analog Controller (old dualshock)
    case 40: // Analog Contoller check valid (otherwise fail phase)
		if (scePadInfoMode(padStruct->port, padStruct->slot,InfoModeCurExID, 0)==0)
		{
			padStruct->pad_type=SHELLPAD_DIGITAL; // Digital pad
			padStruct->phase=99;
			break;
		}
		padStruct->phase++;
		
    case 41: // Analog controller: Request Lock analog mode (asynchronous)
		if (scePadSetMainMode(padStruct->port, padStruct->slot, 1, 3)==1) padStruct->phase++;
		break;
		
    case 42: // Analog controller: Check state of previous request
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateFaild)
		{
			padStruct->phase--;
		}
		else
		{
		}
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateComplete)
		{ 
			// Lock mode complete
			padStruct->phase=0; // Accept normal dualshock
		}
		break;
		
		// DualShock 2 Controller 
    case 70: // Set motors 
		if (scePadInfoAct(padStruct->port, padStruct->slot, -1, 0)==0)
		{
			padStruct->phase = 99;
			break;
		}
		// Asynchronous function
		if (scePadSetActAlign(padStruct->port, padStruct->slot,
							  padStruct->act_align)==0)
		{
			WARNING(0, "shellPad: Set actAlign failed!!!!!!!!!!!!\n");
			break;
		}
		padStruct->phase++;
		if (scePadGetReqState(padStruct->port, padStruct->slot)!=scePadReqStateBusy)
		{
			WARNING(0, "shellPad: Set actAlign warning!\n");
		}
		break;
		
    case 71: // Check motors have been set
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateFaild) 
			padStruct->phase--;
		
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateComplete) 
		{
			padStruct->phase++;
		}
		break;
		
    case 72: // DualShock 2 check pressure sensitive possible
		if (scePadInfoPressMode(padStruct->port, padStruct->slot)==1)
		{
			padStruct->phase = 76;
			break;
		}
		padStruct->pad_type=SHELLPAD_DUALSHOCK;
		padStruct->phase = 99;
		break;

    case 76: // DualShock2 enable pressure sensitive mode (asynchronous function)
		if (scePadEnterPressMode(padStruct->port, padStruct->slot)==1) padStruct->phase++;
		break;
		
    case 77: // Dualshock2 check status of request pressure sensitive mode
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateFaild) padStruct->phase--;
		if (scePadGetReqState(padStruct->port, padStruct->slot)==scePadReqStateComplete)
		{
			padStruct->phase=99;
			PRINT("shellPad: port=%d slot=%d [dualshock2] :count=%d", 
				   padStruct->port, padStruct->slot, padStruct->frameCount);
			padStruct->pad_type=SHELLPAD_DUALSHOCK2;
		}
		break;
		
    default:
		checkError=1; // type shouldn't have changed
		break;
    }
	
	if (!(state==scePadStateStable || state==scePadStateFindCTP1))
    {
		padStruct->leftStickX = (char) leftX;
		padStruct->leftStickY = (char) leftY;
		padStruct->rightStickX = (char) rightX;
		padStruct->rightStickY = (char) rightY;
		return SHELLPAD_DISCONNECT; // Failed
    }
	
	// Process a valid pad	
	if ((padStruct->pad_type!=SHELLPAD_DISCONNECT) )
    {
		padStruct->previousBtn = padStruct->button; // Copy old button
		
		if (scePadRead( padStruct->port, padStruct->slot, rdata ) == 0)
			return SHELLPAD_DISCONNECT;
		
		paddata = (unsigned short) ( 0xffff ^ ((rdata[2]<<8) | rdata[3]) ); 
		tpad =(unsigned short) ( paddata & ~padStruct->rpad );
		padStruct->rpad = paddata;
		padStruct->button = padStruct->rpad; 
		padStruct->stickyBtn |= padStruct->button;
		
		// ChangeButton = (was not pressed and now pressed) 
		changedBtn = tpad; 
		padStruct->previousToggleBtn = padStruct->toggleBtn;
		padStruct->toggleBtn =  ( changedBtn ^ padStruct->toggleBtn );				
		padStruct->toggledBtn = ( padStruct->toggleBtn ^ padStruct->previousToggleBtn);
    }    

	if (padStruct->pad_type>SHELLPAD_DIGITAL) // analog pad
    {
		leftX = (short)((short)rdata[6] - 128);
		leftY = (short)((short)rdata[7] - 128);
		rightX = (short)((short)rdata[4] - 128);
		rightY = (short)((short)rdata[5] - 128);
		
		// Velocity
		padStruct->leftStickVelX = (short) (leftX - padStruct->oldLeftStickX);
		padStruct->leftStickVelY = (short) (leftY - padStruct->oldLeftStickY);
		padStruct->rightStickVelX = (short) (rightX - padStruct->oldRightStickX);
		padStruct->rightStickVelY = (short) (rightY - padStruct->oldRightStickY);
		padStruct->oldLeftStickX = (char) leftX;
		padStruct->oldLeftStickY = (char) leftY;
		padStruct->oldRightStickX = (char) rightX;
		padStruct->oldRightStickY = (char) rightY;
		
		if (abs(leftX)<padStruct->deadZone) leftX = 0;
		if (abs(leftY)<padStruct->deadZone) leftY = 0;
		if (abs(rightX)<padStruct->deadZone) rightX = 0;
		if (abs(rightY)<padStruct->deadZone) rightY = 0;
    }
	
    padStruct->prevLeftStickX =  padStruct->leftStickX;
    padStruct->prevLeftStickY =  padStruct->leftStickY;
    padStruct->prevRightStickX = padStruct->rightStickX;
    padStruct->prevRightStickY = padStruct->rightStickY;

	padStruct->leftStickX = (char) leftX;
	padStruct->leftStickY = (char) leftY;
	padStruct->rightStickX = (char) rightX;
	padStruct->rightStickY = (char) rightY;
	
	if ((rdata[0] == 0) && (padStruct->pad_type==SHELLPAD_DUALSHOCK2))
    {
		for (iPressureBtn=0; iPressureBtn<12; iPressureBtn++)
		{			
			padStruct->buttonVelocities[iPressureBtn] = (unsigned short) (rdata[iPressureBtn+8] - padStruct->oldPressure[iPressureBtn]);	
			padStruct->pressures[iPressureBtn] = (unsigned char) rdata[iPressureBtn+8];
			padStruct->oldPressure[iPressureBtn] = rdata[iPressureBtn+8]; // Needed for velocity
		}
    }

	padStruct->term_id = (rdata[1]); 
	
	if (checkError)
	{
		if (padStruct->term_id>0 && rterm_id!=0 && padStruct->term_id!=rterm_id)
		{
			padStruct->phase = 0;
			padStruct->term_id = 0;
			padStruct->pad_type = SHELLPAD_DISCONNECT;
		}
	}
	
	return (EShellPadType) padStruct->pad_type;
}

//-----------------------------------------------------------------------------
// shellPadClearToggleBtn
// Description: Toggle buttons stay in same state until cleared by this function
// Params: padStruct - data for current pad port and slot
//-----------------------------------------------------------------------------
void shellPadClearToggleBtn(ShellPadData *padStruct)
{
	padStruct->toggleBtn = 0x0;
	padStruct->previousToggleBtn = 0x0;
	padStruct->toggledBtn = 0x0;
}

