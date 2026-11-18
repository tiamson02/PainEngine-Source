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

Pitch Detection
---------------
This demo shows:

1) Pitch detection using the DSP IOP engine

Note that the pitch detection isn't perfect. It has a good go at getting the current pitch in Hz, but
don't expect it to be 100% accurate (considering it takes next to no time on the IOP to process, you
can't complain though...)
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
#include <math.h>
#include "graphics.h"

#include "../ee/sound.h"

void DisplayText(void);
void GetUserControl(void);
extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

void MS_BankLoad(void);
void PlayStream(void);
int paddata, oldpad;
u_char rdata[32];

extern u_int sbuff2[16]__attribute__((aligned (64)));
u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));	// JOYPAD data

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

// To test loading from a CD, set CD_LOADING to 1.
// For this code to still work, burn a CD containing the SAMPDATA directory.
#define CD_LOADING 0	// Set this to 1 for testing loading from CD.
#if CD_LOADING
#define MS7_DATA "cdrom0:\\SAMPDATA\\"	// Directory containing sample data
#define BankBody MS7_DATA "440SINE.VAG"
#else
#define MS7_DATA "host0:../sampdata/"	// Directory containing sample data
#define BankBody MS7_DATA "440SINE.VAG"
#endif

SOUND_DSP_INFO DSPInfo;
int File_ID;
u_int StrmChan;
u_short Volume=0x3fff;
short Pitch=0x1000;
u_short SPUChannel=0;
u_short SampleID=0;
int notes2[13]={8192,8679,9195,9742,10321,10935,11585,12274,13004,13777,14596,15464,16384};
int notes[13]={440,466,495,522,550,588,622,660,704,739,792,830,880};

int Current_Note;
u_char bkeyoffset[5]={0,8,24,32,40};
u_char whitenotes[7]={3,5,7,8,10,0,2};
u_char blacknotes[5]={4,6,9,11,1};
u_char *dis_notes[] __attribute__((aligned (64)))=
					{"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};


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
int MS7_Init(void)
{
    if (SOUND_InitIOP () < 0)
		return (-1);

	SOUND_InitSPU();	// Initialise SPU
	SOUND_InitCD(SOUND_CD);		// Only required if using MultiStream cd loading routines (SOUND_CD or SOUND_DVD)
	SOUND_InitStreamData(SND_LOAD_INTERNAL,0,0);
	FlushIOPCommand(0);	// Send the three SOUND_ commands above to the IOP and wait for completion

	SOUND_SetDSP(0,SOUND_INIT_DSP,0);			// Initialise DSP using SPU channel0 (first parameter ignored!)
	SOUND_DSPOnOff(0,SOUND_DSP_ON);				// Turn DSP processing ON for core0
	SOUND_SetParam(0|SD_P_MMIX,0xfc0);			// Turn PCM reverb OFF for core0
	SOUND_EnableEffects(0,0,0x3fff,0x3fff,0,0);	// Enable hardware effects, using NO revery type (hall/studio A..etc..)
	SOUND_AllChannelEffectsOn(0);				// Turn ALL of core1 effects on..

	SOUND_DSPSetEffect(0,0,SOUND_FLAG_DSP_PITCH_DETECT);	// Pitch Detection
	SOUND_FlushIOPCommand(0);

	MS_BankLoad();		// Samplebank loading testbed
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
	printf("MultiStream V7.0 (DSP)- Jason Page @ SCEE\n");
	printf("Sample code to show pitch detection\n");
	printf("Triangle - Pause/Resume\n");
	printf("X - DSP processing on/off\n");
	printf("************************************************************\n");
	printf("\n");
#endif

	sceSifInitRpc(0);
	load_modules ();	// Load all .IRX files into IOP RAM
    MS7_Init();			// Initialise MultiStream V6
	Gs_Initialize();	// Initialise graphics
    main_loop ();		// Goto main (while(1)) loop..
	printf("\nEXIT MULTISTREAM V7.0\n");
	return 0;
}

/***********************************************************************************
GetNote
	Convert frequency into a note value (0-C,1=C#...11=B)

	Requires:	frequency	Frequency in Hz
	Returns:	note		Note number
***********************************************************************************/
int GetNote(int frequency)
{
u_int i;

	if (frequency==0)
		return(0);
	while(frequency>879)
	{
		frequency/=2;
	}
	while(frequency<440)
	{
		frequency*=2;
	}

	for (i=1;i<13;i++)
	{
		if (frequency<notes[i])
			return((int)i-1);
	}
	return(0);
}

/***********************************************************************************
DisplayText
	Displays all info on screen.
***********************************************************************************/
void DisplayText(void)
{
int i,j,a,b;
int freq=0;
int KScale=5;
int KX=64;
int KY=96;

static count=0;
static count2=0;
static original_freq=0;

	sceDevConsClear(console);

	SetSprite2(260,8,64,64,128,128,128,1,0);
	for (i=128;i<256;i++)
	{
	    SetSprite(0,24+i,320,1,0,0,(i-128)/2);	// display blue fade
	}
	DrawSprites();	// draw sprites

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream DSP - June 2003 SCEE\n");

	sceDevConsPrintf(console, "DSP sample code (Pitch detection)\n\n");
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


	if (psflg==0)
	{
		if (count==0)
		{
			original_freq=notes2[count2%12];
			original_freq*=2;
			original_freq*=44100;
			original_freq/=48000;
			SOUND_PlaySFX((u_int)SampleID,SPUChannel,0x3fff,0x3fff,(int)SOUND_HzToSPU((u_int)original_freq),40,0);
			count2++;
			count2%=1200;
		}
		count++;
		if (count>=50)
			count=0;


		SOUND_GetDSPInfo(&DSPInfo);
		freq=DSPInfo.EffectInfo[0];

		if (freq!=0)
			Current_Note=GetNote(freq);
	}
	sceDevConsPrintf(console,"Playing Note: %s\n",dis_notes[(count2+2)%12]);
	sceDevConsPrintf(console,"Detected Note: %s\n",dis_notes[Current_Note]);
	sceDevConsPrintf(console,"Detected Freq: %d\n",freq);


	j=0;
	for (i=0;i<5;i++)
	{
		a=0;
		b=blacknotes[i];
		if (Current_Note==b)
			a=255-(128*psflg);
		SetSprite((KX+(j*7*8*KScale)+((4+bkeyoffset[i])*KScale))/2,KY,(6*KScale)/2,7*KScale,0,a,0);
	}

	a=255-(128*psflg);

	for (i=0;i<7;i++)
	{
		b=whitenotes[i%7];
		if (Current_Note==b)	//&&(Current_Octave==i/7)&&(NoteOn==1))
		    SetSprite((KX+((i*8)*KScale))/2,KY,(7*KScale)/2,16*KScale,0,a,0);
		else
		    SetSprite((KX+((i*8)*KScale))/2,KY,(7*KScale)/2,16*KScale,a,a,a);
	}

	SetSprite(200/2,80,freq/2,4,255,255,0);

	DrawSprites();	// draw sprites

	sceDevConsDraw(console);
}


/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

	returns:	0	= All ok.
				-1	= START pressed (quit)

***********************************************************************************/
void GetUserControl(void)
{
int pg;

	pg=0;
    oldpad = paddata;
    if(scePadRead(0, 0, rdata) > 0)
    {
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
		PadGate=pg;
	}
}

/*****************************************************************************
MS_BankLoad
	Loads a VAG into SPU RAM..
*****************************************************************************/
void MS_BankLoad(void)
{
	File_ID=0;			// ID of file
	StrmChan=0;			// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	SOUND_AllocateStreamBuffer((int)StrmChan,0,128*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_SetMaxStreamLimit(1);
	SOUND_AllocateFileMemory(1);
	SOUND_AllocateSpotFXMemory(1);

	SOUND_CreateFileInfo(File_ID,BankBody,48);	// Skip VAG header (48 bytes)
	printf("loading..\n");
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,SPU_BASE_ADDR);	// Load to 0x5010..
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);				// No need to keep this ID around now..
	SOUND_CloseStreamBuffer((int)StrmChan);	// Free IOP RAM used for ADPCM stream buffer.
	SOUND_PatchSFX(0,SPU_BASE_ADDR);		// Patch SFX into MultiStream system.
	FlushIOPCommand(0);
	printf("** Loading Samplebank body file (%s) complete **\n\n",BankBody);
}


