/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.6
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

Ducking
-------
This demo shows:

1) Side-chain compression (ducking).
2) Notice how the background ambience automatically fades up/down depending on vocal volume

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
#include <malloc.h>
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
int CompType=0;
int CompMod=0;

// To test loading from a CD, set CD_LOADING to 1.
// For this code to still work, burn a CD containing the SAMPDATA directory.
#define CD_LOADING 0	// Set this to 1 for testing loading from CD.

#if CD_LOADING
#define MS7_DATA "cdrom0:\\SAMPDATA\\"	// Directory containing sample data
#define BankBody MS7_DATA "VOCAL1.VAG"
#define BankBody2 MS7_DATA "TRAFNORM.VAG"
#else
#define MS7_DATA "host0:../sampdata/"	// Directory containing sample data
#define BankBody MS7_DATA "VOCAL1.VAG"
#define BankBody2 MS7_DATA "trafnorm.vag"
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
u_int StackBuffer=0;

int CompressThreshold=4000;
int CompressRatio=800;	//2024;
int CompressGain=1000;
int CompressAttack=15;
int CompressRelease=2;

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
	SOUND_DSPOnOff(1,SOUND_DSP_ON);				// Turn DSP processing ON for core1
	SOUND_SetParam(0|SD_P_MMIX,0xfc0);			// Turn PCM reverb OFF for core0
	SOUND_EnableEffects(0,0,0x3fff,0x3fff,0,0);	// Enable hardware effects, using NO revery type (hall/studio A..etc..)
	SOUND_EnableEffects(1,0,0x3fff,0x3fff,0,0);	// Enable hardware effects, using NO revery type (hall/studio A..etc..)

	SOUND_AllChannelOutputsOff(0);				// Turn ALL of core1 output off..
	SOUND_AllChannelEffectsOn(0);				// Turn ALL of core1 effects on..
	SOUND_AllChannelEffectsOn(1);				// Turn ALL of core1 effects on..

	SOUND_DSPSetEffect(0,0,SOUND_FLAG_DSP_COMPRESS_SIDE); // Slot j - sidechain compression
	SOUND_SetDSP(0,SOUND_COMPRESS_THRESHOLD,(u_int)CompressThreshold);
	SOUND_SetDSP(0,SOUND_COMPRESS_RATIO,(u_int)CompressRatio);
	SOUND_SetDSP(0,SOUND_COMPRESS_ATTACK,(u_int)CompressAttack);
	SOUND_SetDSP(0,SOUND_COMPRESS_RELEASE,(u_int)CompressRelease);
	SOUND_SetDSP(0,SOUND_COMPRESS_GAIN,(u_int)CompressGain);
	SOUND_SetPCMVolume(0x7fff,0x7fff);
#endif

    StackBuffer=(u_int)memalign(64,32*1024);	// Allocate memory to load data to
	ChangeThreadPriority(GetThreadId(),10);
	SOUND_InitFastLoad(9,(int)StackBuffer,32*1024);
// You no longer need to do SOUND_GetStatus(streamnumber,0) if you call this..
	SOUND_SetFastLoadMode(SOUND_FASTLOAD_ON);
// If EE loading, no waiting for user to allow load after each data packet sent..
//	SOUND_SetFastLoadMode(SOUND_FASTLOAD_STOP);
	SOUND_FlushIOPCommand(0);

	SOUND_FlushIOPCommand(0);

	MS_BankLoad();		// Samplebank loading testbed

	SOUND_PlayStream(File_ID,0,0,0x3fff,0x3fff,(int)SOUND_HzToSPU(22050),STREAM_LOOPING,0,0);

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
	printf("MultiStream V7.0 - Jason Page @ SCEE\n");
	printf("Code to play both ADPCM streams and resident SFX\n");
	printf("DSP test for sidechain compression\n");
	printf("Triangle - Pause/Resume\n");
	printf("X - DSP processing on/off\n");
	printf("square - Play vocal sample\n");
	printf("up/down - select\n");
	printf("left/right - modify values\n");
	printf("************************************************************\n");
	printf("\n");
#endif

	sceSifInitRpc(0);
	load_modules ();	// Load all .IRX files into IOP RAM
    MS7_Init();			// Initialise MultiStream V7
	Gs_Initialize();	// Initialise graphics
    main_loop ();		// Goto main (while(1)) loop..
	printf("\nEXIT MULTISTREAM V7\n");
	return 0;
}

/***********************************************************************************
DisplayText
	Displays all info on screen.
***********************************************************************************/
void DisplayText(void)
{
int i;
float a;
SOUND_DSP_INFO DSPInfo;

	SOUND_GetDSPInfo(&DSPInfo);
	sceDevConsClear(console);


	SetSprite2(260,8,64,64,128,128,128,1,0);
	for (i=128;i<256;i++)
	{
	    SetSprite(0,24+i,640,1,0,0,(i-128)/2);	// display blue fade
	}

    SetSprite(40,112,DSPInfo.EffectInfo[0]/8,4,255,255,255);


    SetSprite(0,132+(CompType*8),640,7,64+(frame&63),0,0);

	DrawSprites();	// draw sprites

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream 7.0 DSP - June 2003 SCEE\n");

	sceDevConsPrintf(console, "DSP sample code (Sidechain compression)\n\n");
	sceDevConsPrintf(console, "X - DSP processing On/Off (Currently ");
	if (dspproc==1)
		sceDevConsPrintf(console, "OFF)\n");
	else
		sceDevConsPrintf(console, "ON)\n");

	sceDevConsPrintf(console, "SQUARE - Play vocal\n");
	sceDevConsPrintf(console, "TRIANGLE - Pause/Resume   ");

	if (psflg==1)
		sceDevConsPrintf(console,"PAUSED - Press Triangle To Resume\n");
	else
		sceDevConsPrintf(console,"\n");

	sceDevConsPrintf(console,"\n");
	sceDevConsPrintf(console,"(notice how the background noise automatically fades up and down..\n");
	sceDevConsPrintf(console,"depending on the vocal volume...)\n");

	sceDevConsPrintf(console,"\n\n");
	sceDevConsPrintf(console,"Level:\n");
	sceDevConsPrintf(console,"\n\n");

	i=CompressRatio;
	if (i==0)
		i++;
	a=i;
	a=2048/a;

	sceDevConsPrintf(console,"Compressor Threshold (0-32767) = %d\n",CompressThreshold);
	sceDevConsPrintf(console,"Compressor Ratio (0-2048) = %d (RATIO = %4.2f:1)\n",CompressRatio,a);

	if (CompType==0)
	{
		CompressThreshold+=CompMod;
		if (CompressThreshold<0)
			CompressThreshold=0;
		else if (CompressThreshold>32767)
			CompressThreshold=32767;
	}
	else if (CompType==1)
	{
		CompressRatio+=CompMod;
		if (CompressRatio<0)
			CompressRatio=0;
		else if (CompressRatio>2048)
			CompressRatio=2048;
	}

	SOUND_SetDSP(0,SOUND_COMPRESS_THRESHOLD,(u_int)CompressThreshold);
	SOUND_SetDSP(0,SOUND_COMPRESS_RATIO,(u_int)CompressRatio);

	sceDevConsDraw(console);
}

/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

***********************************************************************************/
void GetUserControl(void)
{
int pg;

	pg=0;
    oldpad = paddata;
    if(scePadRead(0, 0, rdata) > 0)
    {
		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

		if (paddata & SCE_PADRleft)
		{
			pg=1;
			if (PadGate!=1)
				SOUND_PlaySFX(SampleID,24,0x3fff,0x3fff,(int)SOUND_HzToSPU(44100),0,0);
		}
		else if (paddata & SCE_PADRdown)
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

		if (paddata & SCE_PADLup)
		{
			if (CompType!=0)
				CompType--;
			pg=1;
		}
		if (paddata & SCE_PADLdown)
		{
			if (CompType!=1)
				CompType++;
			pg=1;
		}

		CompMod=0;
		if (paddata & SCE_PADLleft)
		{
			CompMod=-64;
		}
		if (paddata & SCE_PADLright)
		{
			CompMod=+64;
		}
		PadGate=pg;

	}
}

/*****************************************************************************
MS_BankLoad
	Loads an vocal VAG into SPU RAM to be played as a SFX.
	Creates a FileID to be used as backgroud audio stream.

	Requires:	Nothing
	Returns:	Nothing
*****************************************************************************/
void MS_BankLoad(void)
{
int File_ID;
u_int StrmChan;

	File_ID=0;			// ID of file
	StrmChan=0;			// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	SOUND_AllocateStreamBuffer((int)StrmChan,0x5010,32*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_SetMaxStreamLimit(1);
	SOUND_AllocateFileMemory(1);
	SOUND_AllocateSpotFXMemory(1);

	SOUND_CreateFileInfo(File_ID,BankBody,48);

	printf("loading..%x\n",SPU_BASE_ADDR+(32*1024));

	SOUND_PatchSFX(0,SPU_BASE_ADDR+(32*1024));
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,SPU_BASE_ADDR+(32*1024));
	FlushIOPCommand(0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..

	printf("**** Next SPU address = %x\n",SOUND_GetSPUAddress());

	SOUND_CreateFileInfo(File_ID,BankBody2,48);
	FlushIOPCommand(0);

	printf("** Loading Samplebank body file (%s) complete **\n\n",BankBody);
}


