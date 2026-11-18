/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.8
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                     - Multistream / sample2 - 
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Europe 
 *                       All Rights Reserved.
 *
 *                            main.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      5.00            Oct,31,2001      Jason Page 
 *      6.00            Nov,28,2002      Jason Page 
 *      6.10            Jan,31,2003      Jason Page 
 *      6.20            Feb,25,2003      Jason Page 
 *		7.00 (DSP)		Jun,25,2003		 Jason Page
 */

/**********************************************************************************
MultiStream DSP (V7.0)
(c) 2000-2003 Sony Computer Entertainment Europe
Programmer: Jason Page (Jason_Page@scee.net)
Last update: June 2003

CarRevs
-------
This demo shows:

1) DSP effects to create a realistic car engine.
2) Very similar to the CarRevs sample code (released 1999), but using VAG data instead of PCM.

***********************************************************************************/

#include <eekernel.h>
#include <stdio.h>
#include <libsdr.h>
#include <sdrcmd.h>
#include <libdev.h>
#include <libpad.h>
#include <libgraph.h>
#include <libpkt.h>
#include <libcdvd.h>
#include <sifdev.h>
#include <sif.h>
#include <stdlib.h>
#include "graphics.h"

#include "../ee/sound.h"

void DisplayText(void);
void GetUserControl(void);

extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

int paddata, oldpad;
u_char rdata[32];

extern u_int sbuff2[16]__attribute__((aligned (64)));

int frame=0;	// Frame counter
int PadGate=0;	// Pad button gate flag
int psflg=0;	// Pause flag
int dspproc=0;	// DSP processing on/off (0=process, 1=no process)

#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_ROOT13  "host0:/usr/local/sce/iop/modules13/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "libsd.irx"
#define _MOD_STREAM "host0:../iop/stream.irx"
#define _MOD_CDVDMAN "cdvdman.irx"
#define _MOD_CDVDFSV "cdvdfsv.irx"

#define SPU_BASE_ADDR 0x5010		// Start address of SPU RAM

int rasterh2;	// EE processing time.

u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));	// JOYPAD data

void Display_MIH_HeaderInfo(u_int HeaderInfo);
void MS_BankLoad(void);
void PlayStream(void);
void InitBackground(void);
void UpdateBackground(void);
void MS_EELoad(void);		// Test for loading data into EE RAM..
void Patch_MSH_HeaderInfo(u_int SPUAddr);
void Display_MSH_HeaderInfo(u_int HeaderInfo);

int File_ID=0;
u_int StrmChan=0;
u_short Volume=0x3fff;
short Pitch=0x1000;
u_short SPUChannel=0;
u_short SampleID=0;

u_int NumberOfSamples;

// To test loading from a CD, set CD_LOADING to 1.
// For this code to still work, burn a CD containing the SAMPDATA directory.
#define CD_LOADING 0	// Set this to 1 for testing loading from CD.

#if CD_LOADING
#define MS7_DATA "cdrom0:\\SAMPDATA\\"	// Directory containing sample data
#define BankBody MS7_DATA "CARREV;1"
#else
#define MS7_DATA "host0:../sampdata/"	// Directory containing sample data
#define BankBody MS7_DATA "CARREV.VAG"
#endif

/***********************************************************************************
load_modules
	Loads IRX modules to IOP
***********************************************************************************/
void load_modules (void)
{

    printf ("loading modules ....");

    if (sceSifLoadModule (_MOD_ROOT _MOD_SIO2MAN, 0, NULL) < 0)
    {
		printf ("  loading %s failed\n", _MOD_SIO2MAN);
		while(1){};
    }
    if (sceSifLoadModule (_MOD_ROOT _MOD_PADMAN, 0, NULL) < 0)
    {
		printf ("  loading %s failed\n", _MOD_PADMAN);
		while(1){};
    }

    if (sceSifLoadModule( _MOD_ROOT _MOD_LIBSD, 0, NULL) < 0)
    {
		printf("  loading libsd.irx failed\n");
		while(1){};
    }

	// LOAD STREAMING MODULE
    if (sceSifLoadModule (_MOD_STREAM, 0, NULL) < 0)
	{
		printf("  loading stream.irx failed\n");
		while(1){};
    }

    printf ("Complete.\n");
    return;
}


/***********************************************************************************
init_adpcm
	Init the Stream IOP functions. Init SPU hardware and stream data.
	Allocates 4 buffers on IOP to be used for streaming.
	Sets the max limit which the STREAM_FIND_FREE param uses to know how many streams
	are allocated on the IOP.
***********************************************************************************/
int FilterFreq=512;
int FilterRes=0;
int DistortLevel=0;
#define CarRevMin 0x500
#define CarRevMax 0x2000
int CarDecelerate=48;
int CarRevs=CarRevMin;
int CarVolume=0;
int CarRevVol=0;
int CarGear=0;

int MS7_Init(void)
{
    if (SOUND_InitIOP () < 0)
		return (-1);

	SOUND_InitSPU();	// Initialise SPU
	SOUND_InitCD(SOUND_CD);		// Only required if using MultiStream cd loading routines (SOUND_CD or SOUND_DVD)
	SOUND_InitStreamData(SND_LOAD_INTERNAL,0,0);
	FlushIOPCommand(0);	// Send the three SOUND_ commands above to the IOP and wait for completion

#if 1
	SOUND_SetDSP(0,SOUND_INIT_DSP,0);			// Initialise DSP using SPU channel0 (first parameter ignored!)
	SOUND_DSPOnOff(0,SOUND_DSP_ON);				// Turn DSP processing ON for core0
	SOUND_SetParam(0|SD_P_MMIX,0xfc0);			// Turn PCM reverb OFF for core0
	SOUND_EnableEffects(0,0,0x3fff,0x3fff,0,0);	// Enable hardware effects, using NO revery type (hall/studio A..etc..)

// Audio will play from both DRY and WET areas from core0
	SOUND_AllChannelOutputsOff(0);				// Turn ALL of core1 output off..
	SOUND_AllChannelEffectsOn(0);				// Turn ALL of core1 effects on..

// Filter will process PCM data first, followed by distortion..

	SOUND_DSPSetEffect(0,0,SOUND_FLAG_DSP_FILTER);			// Slot 0 - Filter
	SOUND_DSPSetEffect(1,0,SOUND_FLAG_DSP_DISTORT);			// Slot 1 - Distortion

	SOUND_SetDSP(0,SOUND_FILTER_TYPE,SOUND_FILTER_LOWPASS);	// Set up initial values for filter
	SOUND_SetDSP(0,SOUND_FILTER_FREQ,(u_int)FilterFreq);
	SOUND_SetDSP(0,SOUND_FILTER_RESONANCE,(u_int)FilterRes);
	SOUND_SetDSP(1,SOUND_DISTORTION_LEVEL,(u_int)DistortLevel);	// Set up initial value for distortion
	SOUND_SetPCMVolume(0x7fff,0x7fff);
#endif

	SOUND_FlushIOPCommand(0);

	MS_BankLoad();		// Samplebank loading testbed

	SOUND_PlaySFX(SampleID,SPUChannel,0,0,Pitch,0,0);
//	SOUND_PlaySFX(SampleID,SPUChannel+1,0,0,Pitch+1000,0,0);

    return (0);
}

/***********************************************************************************
main_loop
	Init graphics/pad.
	Handles user control
	Update screen
	Send data packet to IOP
***********************************************************************************/
int main_loop(void)
{
int LastCheck=0;

	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	frame=0;
	GRAP_SendTIM2(1,MS7_DATA "tglogo.tm2");
	SetAlphaBlend();
    while( sceGsSyncV(0) == 0 ){}

	while( 1 )
	{
		GetUserControl();

		FlushCache(0);
		DisplayText();

//		CheckForExternalLoading();
		FlushIOPCommand(SIF_RPCM_NOWAIT);			// Transfer EE command list to IOP buffer

		if(frame&0x01)
			sceGsSetHalfOffset(&db.draw1,2048,2048, (short)(sceGsSyncV(0)^0x01));
		else
			sceGsSetHalfOffset(&db.draw0,2048,2048, (short)(sceGsSyncV(0)^0x01));

		sceGsSwapDBuff(&db, frame);
		frame++;
		FlushCache(0);
		sceGsSyncPath(0,0);
	}
}

/***********************************************************************************
main
	Nothing interesting..
***********************************************************************************/
int main( int argc, char *argv[]  )
{
(void)argc;
(void)argv;
#if 1
	printf("************************************************************\n");
	printf("MultiStream DSP V7.0 - Jason Page @ SCEE\n");
	printf("sample code to show simple DSP effects for creating car engines\n");
	printf("Triangle - Pause/Resume\n");
	printf("X - DSP On/Off\n");
	printf("Right Analog UP - Rev engine\n");
	printf("L2/R2 - Gear change up/down\n");
	printf("Start - Quit\n");
	printf("************************************************************\n");
	printf("\n");
#endif

	sceSifInitRpc(0);
	load_modules ();	// Load all .IRX files into IOP RAM
    MS7_Init();			// Initialise MultiStream V6
	Gs_Initialize();	// Initialise graphics
	InitBackground();
    main_loop ();		// Goto main (while(1)) loop..
	printf("\nEXIT MULTISTREAM V7\n");
	return 0;
}


//Pretty sine way background data..
#define BGMAX 200-24
int dx1[BGMAX];	// x positions
int dx2[BGMAX];	// widths
int da[2];		// accelerations
int ds[2];		// speeds
int dt[2];		// timers
int dx[2];		// current x positions
int dm[2];		// timer limits
/***********************************************************************************
InitBackground
	Initialises the sinewave background..
***********************************************************************************/
void InitBackground(void)
{
int i;
	da[0]=8;
	da[1]=-17;	//13
	dm[0]=200;
	dm[1]=130;	//160
	for (i=0;i<2;i++)
	{
		ds[i]=0;
		dt[i]=dm[i]/2;
		dx[i]=0;
	}
	for (i=0;i<BGMAX;i++)
	{
		UpdateBackground();
	}
}

/***********************************************************************************
UpdateBackground
	Update sinewave background..
***********************************************************************************/
void UpdateBackground(void)
{
int i;
	for (i=0;i<2;i++)
	{
		ds[i]+=da[i];		// add acceleration to speed
		dx[i]+=ds[i];		// add speed to current position
		dt[i]++;			// inc timer
		if (dt[i]>=dm[i])	// reached limit?
		{
			dt[i]=0;		// reset timer
			da[i]=-da[i];	// reverse acceleration
		}
	}
	for (i=0;i<BGMAX;i++)
	{
		dx1[(BGMAX-1)-i]=dx1[(BGMAX-2)-i]+1;		// "scroll" positions down, and modify coord by 1 as well
		dx2[(BGMAX-1)-i]=dx2[(BGMAX-2)-i]-1;		// "scroll" width position down, and modify coord too
	}
	dx1[0]=(dx[0]/400)+250;				// set new coord
	dx2[0]=((dx[1]/550)+300)-((dx[0]/400)+250);		// set new width
}

/***********************************************************************************
DisplayText
	Displays all info on screen.
***********************************************************************************/
int padstarted=0;
int padflash=0;

void DisplayText(void)
{
int i;

	sceDevConsClear(console);

	SetSprite2(260,8,64,64,128,128,128,1,0);
	for (i=128;i<BGMAX;i++)
	{
	    SetSprite(0,24+i,640,1,0,0,(i-128)/2);	// display blue fade
	}
	DrawSprites();	// draw sprites

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream DSP - June 2003 SCEE\n");

	sceDevConsPrintf(console, "DSP sample code (Filter and Distortion)\n\n");
	sceDevConsPrintf(console, "Right analog stick UP - Rev engine\n");
	sceDevConsPrintf(console, "X - DSP processing On/Off (Currently ");
	if (dspproc==1)
		sceDevConsPrintf(console, "OFF)\n");
	else
		sceDevConsPrintf(console, "ON)\n");

	sceDevConsPrintf(console, "TRIANGLE - Pause/Resume   ");

	if (psflg==1)
		sceDevConsPrintf(console,"PAUSED - Press Triangle To Resume\n");
	else
		sceDevConsPrintf(console,"\n");

	sceDevConsPrintf(console,"\n");

	sceDevConsPrintf(console,"Revs       %x\n",CarRevs);
	sceDevConsPrintf(console,"Volume     %x\n",CarVolume);
	sceDevConsPrintf(console,"Filter     %x\n",FilterFreq);
	sceDevConsPrintf(console,"Resonance  %x\n",FilterRes);
	sceDevConsPrintf(console,"Distortion %x\n",DistortLevel);
	sceDevConsPrintf(console,"\n");
	sceDevConsPrintf(console,"Gear       %x\n",CarGear);
	sceDevConsPrintf(console,"L1/ R1 - Shift gears up/down\n");

	if (padstarted==0)
	{
		padflash++;
		if (padflash>32)
			sceDevConsPrintf(console,"\n\n*********** PLEASE TURN ANALOG MODE ON **********\n");
		else
			sceDevConsPrintf(console,"\n");
		padflash&=63;
	}
		

	i=255-(128*psflg);


	SetSprite(144/2,10*8,(CarRevs-0x500)/32,4,i,i,i);
	SetSprite(144/2,11*8,(CarVolume)/68,4,i,i,i);
	SetSprite(144/2,12*8,FilterFreq/4,4,i,i,i);
	SetSprite(144/2,13*8,FilterRes/2,4,i,i,i);
	SetSprite(144/2,14*8,DistortLevel/2,4,i,i,i);
	DrawSprites();	// draw sprites


	sceDevConsDraw(console);
}

void ProcessCarEngine(int revs)
{
int a,t;
int decelerate=0;
int flag=0;

// Note that "revs" is the value read from the joypad. Therefore, due to the analog stick not being very
// good at centering on 0, you need to check at around +/- 28 or so, just to make sure that it doesn't start
// accelerating even when nobody is near the joypad!

	if (revs<28)					// Not accelerating?
	{
		CarRevVol-=CarDecelerate;	// Decrease volume
		if (CarRevVol<0)			// Limit volume
			CarRevVol=0;
		CarRevs-=CarDecelerate;		// Decelerate revs
		revs=0;						// Clamp user rev range to 0
		if (CarRevs<CarRevMin)		// Revs below minimum value?
			CarRevs=CarRevMin;		// Yes..
		else
			decelerate=1;			// Otherwise, set Decelerate flag
			
		flag=1;						// Flag to say that we weren't accelerating
	}
	CarRevs+=revs/(2+((CarGear)*4));					// Add revs to current car revs

	if (CarRevs>CarRevMax)			// Limit car revs
		CarRevs=CarRevMax;
	CarRevVol+=revs;				// Add revs to current car volume
	if (CarRevVol>0x400)			// Limit car volume
		CarRevVol=0x400;

	a=CarRevs-CarRevMin;			// a = 0->n rather than CarRevMin->CarRevMin+n

	DistortLevel=a/64-(decelerate*16);	// Set distortion level
	DistortLevel+=revs/8;				// Increase distortion more if accelerating faster
	if (DistortLevel<0)					// Clamp distortion
		DistortLevel=0;

	CarVolume=0x1800+(CarRevVol*16);		// Set final car volume
	if (CarVolume>0x3fff)				// Clamp volume
		CarVolume=0x3fff;

	FilterFreq=48+(a/16);			// Set filter depending on car rev
	FilterFreq+=revs;
	FilterFreq+=rand()&15;			// also add a bit of randomness
	if (FilterFreq>512)
		FilterFreq=512-(rand()%32);	// Limit filter

	t=CarRevs-0x800;				// Do resonance fade in..
	if (t>0x800)					// above 0x400... fade out..
		t=0x1000-t;
	t/=16;							// scale
	if (t<0)						// limit
		t=0;
	FilterRes=t;					// Will rise up until 0x400, when it will then fade out again...

	if (psflg==0)					// Not paused?
	{
		SOUND_SetChannelPitch(0,CarRevs);					// Give MultiStream the info..
		SOUND_SetChannelVolume(0,CarVolume,CarVolume);
		SOUND_SetDSP(0,SOUND_FILTER_FREQ,(u_int)FilterFreq);
		SOUND_SetDSP(0,SOUND_FILTER_RESONANCE,(u_int)FilterRes);
		SOUND_SetDSP(1,SOUND_DISTORTION_LEVEL,8+((u_int)DistortLevel));
	}
}

/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

	returns:	0	= All ok.
				-1	= START pressed (quit)

***********************************************************************************/

int CarNoRev=0;
void GetUserControl(void)
{
int pg;
int r,t;


	pg=0;
    oldpad = paddata;
    if(scePadRead(0, 0, rdata) > 0)
    {

		r=0;
		t=0;
		paddata=rdata[5];						// right joystick Y

		if (paddata!=0)
			padstarted=1;

		if (padstarted==1)
		{
			r=0x80-paddata;
			if (r<28)
				CarNoRev=0;
			if (CarNoRev!=0)
			{
				CarNoRev--;
				r=0;
			}
			ProcessCarEngine(r);
		}


		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

		if (paddata & SCE_PADRdown)
		{
			pg=1;
			if (PadGate!=1)
			{
				dspproc=1-dspproc;
				SOUND_SetDSP(0,SOUND_DSP_MUTE,(u_int)dspproc);
				SOUND_SetDSP(1,SOUND_DSP_MUTE,(u_int)dspproc);

			}
		}
		else if (paddata & SCE_PADRup)
		{
			pg=1;
			if (PadGate!=1)
			{
				if (psflg==0)
				{

					SOUND_Pause();
					psflg=1;
				}
				else
				{
					SOUND_Resume();
					psflg=0;
				}
			}
		}

		if (paddata & SCE_PADR2)
		{
			pg=1;
			if ((PadGate!=1)&&(CarGear<4))
			{
				CarNoRev=20+(rand()%10);
				CarGear++;
			}
		}
		if (paddata & SCE_PADL2)
		{
			pg=1;
			if ((PadGate!=1)&&(CarGear>0))
			{
				CarNoRev=0;
				CarGear--;
			}
		}
		PadGate=pg;
	}
}


/*****************************************************************************
MS_BankLoad
	Loads the original car engine VAG sample into SPU RAM.
*****************************************************************************/
void MS_BankLoad(void)
{

int File_ID;

	File_ID=0;			// ID of file
	StrmChan=0;			// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	SOUND_AllocateStreamBuffer((int)StrmChan,0,64*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_SetMaxStreamLimit(1);
	SOUND_AllocateFileMemory(1);
	SOUND_AllocateSpotFXMemory(1);

	SOUND_CreateFileInfo(File_ID,BankBody,48);
	printf("loading..\n");
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,SPU_BASE_ADDR);
	FlushIOPCommand(0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..
	SOUND_CloseStreamBuffer((int)StrmChan);	// Free IOP RAM used for ADPCM stream buffer.

	SOUND_PatchSFX(0,SPU_BASE_ADDR);
	FlushIOPCommand(0);

	printf("** Loading Samplebank body file (%s) complete **\n\n",BankBody);
}


