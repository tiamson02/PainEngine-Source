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

Swimming
-------
This demo shows:

1) Very simple DSP effect to create a feeling of being underwater (low pass filter).

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
#include "graphics.h"

#include "../ee/sound.h"

#include "../../scream3/ee/include/scream.h"

// these headers where created by the SCREAM tool and 
// we will use them for the UI...
#include "../scrmdata/Jungle.h"
#include "../scrmdata/Jungle_n.h"

void DisplayText(void);
void GetUserControl(void);

extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

int paddata, oldpad;
u_char rdata[32];

extern u_int sbuff2[16]__attribute__((aligned (64)));

int frame=0;	// Frame counter
int PadGate=0;	// Pad button gate flag
int psflg=0;	// Pause flag
int scream_psflg=0;	// Pause flag
int dspproc=0;	// DSP processing on/off (0=process, 1=no process)

#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_ROOT13  "host0:/usr/local/sce/iop/modules13/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "libsd.irx"
#define _MOD_SCREAM 	"host0:../../scream3/iop/module/scream.irx"
#define _MOD_SCREAM_ERR "host0:../../scream3/iop/module/scrm_err.irx"
#define _MOD_STREAM "host0:../iop/stream.irx"
#define _MOD_CDVDMAN "cdvdman.irx"
#define _MOD_CDVDFSV "cdvdfsv.irx"

#define SPU_BASE_ADDR 0x5010		// Start address of SPU RAM

u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));	// JOYPAD data

void Display_MIH_HeaderInfo(u_int HeaderInfo);
void MS_BankLoad(void);
void PlayStream(void);
void InitBackground(void);
void UpdateBackground(void);
void MS_EELoad(void);		// Test for loading data into EE RAM..
void Patch_MSH_HeaderInfo(u_int SPUAddr);
void Display_MSH_HeaderInfo(u_int HeaderInfo);

//***** SCREAM related *****
void LoadScreamBank(void);
void BankLoadCB(u_int uReturn, u_long uUser);
void UnLoadScreamBank(void);
void PlaySCREAMSound(int index, u_int *handle);
void PlaySoundCB(u_int uReturn, u_long uUser);

SoundBankPtr 	g_ScreamBank 		= NULL;
u_int			g_SCREAMSoundHandle	= 0;
int				g_CurrentSoundIndex = 0;

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
#define BankBody MS7_DATA "SPLASH1.VAG;1"
#define BankBody2 MS7_DATA "SWIMAMB.VAG;1"
#define LogoTim "TGLOGO.TM2;1"
#else
#define MS7_DATA "host0:../sampdata/"	// Directory containing sample data
#define BankBody MS7_DATA "SPLASH1.VAG"
#define BankBody2 MS7_DATA "SWIMAMB.VAG"
#define LogoTim "tglogo.tm2"
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

	// LOAD SCREAM MODULE
    if (sceSifLoadModule (_MOD_SCREAM, 17, "tick_priority=45\0\0") < 0)
	{
		printf("  loading scream.irx failed\n");
		while(1){};
    }
	
	// LOAD SCREAM ERROR DECODER MODULE
    if (sceSifLoadModule (_MOD_SCREAM_ERR, 0, NULL) < 0)
	{
		printf("  loading scrm_err.irx failed\n");
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

int FilterFreq=512;
int FilterRes=0;

int MS7_Init(void)
{
    
	if (SOUND_InitIOP () < 0)
		return (-1);
#if 1
	// Init SCREAM...
	snd_StartSoundSystem();
	
	// limit SCREAM to the top 40 voices...
	// we do this by locking the lower 8 voices
	snd_HardLockVoices(0xff, 0);

	// limit SCREAM's memory manager to top 1 meg of memory
	// by marking the lower half of memory used...
	snd_SRAMMarkUsed(SPU_BASE_ADDR, 0xfffff - SPU_BASE_ADDR); 

	// flush those commands down to the IOP...
	snd_FlushSoundCommands();

#endif	

	SOUND_InitSPU();	// Initialise SPU
	SOUND_InitCD(SOUND_CD);		// Only required if using MultiStream cd loading routines (SOUND_CD or SOUND_DVD)
	SOUND_InitStreamData(SND_LOAD_INTERNAL,0,0);
	
	// limit MS to the bottom 8 voices...
	SOUND_SetMultiStreamVoices(0xff, 0);
	FlushIOPCommand(0);	// Send the three SOUND_ commands above to the IOP and wait for completion


	SOUND_SetDSP(0,SOUND_INIT_DSP,0);			// Initialise DSP using SPU channel0 (first parameter ignored!)
	SOUND_DSPOnOff(0,SOUND_DSP_ON);				// Turn DSP processing ON for core0
	SOUND_SetParam(0|SD_P_MMIX,0xfc0);			// Turn PCM reverb OFF for core0
	SOUND_EnableEffects(0,0,0x3fff,0x3fff,0,0);	// Enable hardware effects, using NO revery type (hall/studio A..etc..)

	SOUND_AllChannelOutputsOff(0);				// Turn ALL of core1 output off..
	SOUND_AllChannelEffectsOn(0);				// Turn ALL of core1 effects on..

	SOUND_DSPSetEffect(0,0,SOUND_FLAG_DSP_FILTER);			// Slot 1 - Filter

	SOUND_SetDSP(0,SOUND_FILTER_TYPE,SOUND_FILTER_LOWPASS);	// Set up initial values for filter
	SOUND_SetDSP(0,SOUND_FILTER_RESONANCE,(u_int)FilterRes);
	SOUND_SetDSP(0,SOUND_FILTER_FREQ,(u_int)FilterFreq);
	SOUND_SetPCMVolume(0x7fff,0x7fff);

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

	// Load SCREAM test bank...
	LoadScreamBank();	

	SOUND_PlayStream(File_ID,0,0,0x1fff,0x1fff,(int)SOUND_HzToSPU(44100),STREAM_LOOPING,0,0);
	SOUND_PlayStream(File_ID,1,1,0x0fff,0x0fff,(int)SOUND_HzToSPU(38000),STREAM_LOOPING,62,0);

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
	GRAP_SendTIM2(1,MS7_DATA LogoTim);
	SetAlphaBlend();
    while( sceGsSyncV(0) == 0 ){}

	while( 1 )
	{
		GetUserControl();

		FlushCache(0);
		DisplayText();
		FlushIOPCommand(SIF_RPCM_NOWAIT);			// Transfer EE command list to IOP buffer

		// SCREAM command flush...
//		snd_FlushSoundCommands();

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
	printf("MultiStream DSP (V7.0) - Jason Page @ SCEE\n");
	printf("Code to play both ADPCM streams and resident SFX\n");
	printf("DSP test for underwater effect\n");
	printf("Triangle - Pause/Resume\n");
	printf("X - DSP processing on/off\n");
	printf("square + left analog stick = swim\n");
	printf("circle - jump into water\n");
	printf("(You MUST enable analog mode for this demo to work)\n");
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
int bY=0;
int bX=200*512;
int XSpd=0;
int YSpd=0;
int XAcc=0;
int YAcc=0;
int ScreenX=0;
int ScreenY=0;
int spl=0;
int SwimTimer=0;
int SwimTimer2=0;

void DisplayText(void)
{
int i,a;
	sceDevConsClear(console);

    SetSprite((ScreenX-4)/2,ScreenY-4,9/2,9,0,255,0);

	SetSprite2(260,8,64,64,128,128,128,1,0);
	for (i=0;i<150;i++)
	{
	    SetSprite(0,100+i,640,1,0,0,255-i*2);	// display blue fade
	}

	ScreenX=bX/512;
	ScreenY=bY/512;
	if (ScreenY>100)
	{
		FilterFreq=512-((ScreenY-100)*12);
		if (FilterFreq<0)
			FilterFreq=0;

		FilterFreq*=FilterFreq;
		FilterFreq>>=9;	///=512;
	}
	else
		FilterFreq=512;

	if (FilterFreq>512)
		FilterFreq=512;
	else if (FilterFreq<16)
		FilterFreq=16;
		

	if (ScreenY<100)
	{
		YAcc=128;
		if (spl!=0)
			YSpd/=2;
	}
	else if (SwimTimer2==0)
		YAcc-=16;
	YSpd+=YAcc;

	XSpd+=XAcc;
	if (XSpd>0x200)
		XSpd-=128;	//YSpd=0x200;
	else if (XSpd<-0x200)
		XSpd+=128;	//=-0x200;

	if (spl!=0)
	{
		if (YSpd>0x200)
			YSpd-=128;	//YSpd=0x200;
		else if (YSpd<-0x200)
			YSpd+=128;	//=-0x200;
	}

	bX+=XSpd;
	bY+=YSpd;

	if (bX<8*512)
		bX=8*512;
	else if (bX>(620*512))
		bX=620*512;
	if (bY<0)
		bY=0;
	else if (bY>(200*512))
		bY=200*512;

	if ((ScreenY>100)&&(spl==0))
	{
		spl=1;

		if (psflg==0)
		{
			a=34000+(rand()%10000);
// Splash when entering water (this one is filtered depending on depth)
			SOUND_PlaySFX(0,2,0x3fff,0x3fff,(int)SOUND_HzToSPU((u_int)a),0,0);
// Play again by play it DRY..Have a medium release rate, and then key it off straight away.
// Gives a little more of an audiable splash sound when first entering the water..
			SOUND_PlaySFX(0,25,0x3fff,0x3fff,(int)SOUND_HzToSPU((u_int)a),0,13);
			SOUND_StopSound(25);
		}
	}

	DrawSprites();	// draw sprites

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream 7.0 DSP - June 2003 SCEE\n");

	sceDevConsPrintf(console, "DSP sample code (Low pass filter)\n\n");
	sceDevConsPrintf(console, "X - DSP processing On/Off (Currently ");
	if (dspproc==1)
		sceDevConsPrintf(console, "OFF)\n");
	else
		sceDevConsPrintf(console, "ON)\n");

	sceDevConsPrintf(console, "SQUARE + Left analog - Swim around\n");
	sceDevConsPrintf(console, "CIRCLE - Jump again\n");
	sceDevConsPrintf(console, "TRIANGLE - Pause/Resume   ");

	if (psflg==1)
		sceDevConsPrintf(console,"PAUSED - Press Triangle To Resume\n");
	else
		sceDevConsPrintf(console,"\n");

	sceDevConsDraw(console);

}

/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

	returns:	0	= All ok.
				-1	= START pressed (quit)

***********************************************************************************/

int xc=1;
u_int s_sizes[100];
u_int s_offset[100];

int oo=0;
int oot=0;

int padstarted=0;
void GetUserControl(void)
{
int pg;
int a,r;

int vl,vr;

	SOUND_SetDSP(0,SOUND_FILTER_FREQ,(u_int)FilterFreq);
	if (psflg==0)
	{

		if (oot==0)
		{
			r=rand()%200;
			if (r==4)
			{
				vl=rand()&0x1fff;
				vr=rand()&0x1fff;
					SOUND_PlaySFX(0,2+oo,vl,vr,(int)SOUND_HzToSPU((u_int)(36100+(rand()%10000))),0,0);
				oo++;
				oo%=10;
				oot=300;
			}
		}
		else
			oot--;
	}


	pg=0;
    oldpad = paddata;
    if(scePadRead(0, 0, rdata) > 0)
    {
		XAcc=(XAcc*SwimTimer)/32;
		YAcc=(YAcc*SwimTimer)/32;
		if (SwimTimer!=0)
			SwimTimer--;

		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

		// SWIM Noise!!
		if (SwimTimer2!=0)
			SwimTimer2--;
		if (paddata & SCE_PADRleft)
		{
			if (SwimTimer2==0)
			{
				if (psflg==0)
					SOUND_PlaySFX(0,3,0x3fff,0x3fff,(int)SOUND_HzToSPU((u_int)(34000+(rand()%10000))),0x30,0);
				SwimTimer=32;
				SwimTimer2=80;
				a=rdata[6]-0x80;
				if ((a<0x20)&&(a>-0x20))
					a=0;
				XAcc+=a/2;
				a=rdata[7]-0x80;
				if ((a<0x20)&&(a>-0x20))
					a=0;
				YAcc+=a/2;

			}

		}
		else if (paddata & SCE_PADRright)
		{
			pg=1;
			if (PadGate!=1)
			{
				spl=0;
				bY=rand()%90;
				bY*=512;
				bX=200*512;
				XSpd=0;
				YSpd=0;
				XAcc=0;
				YAcc=0;
			}
		}
		else if (paddata & SCE_PADRdown)
		{
			pg=1;
			if (PadGate!=1)
			{
				dspproc=1-dspproc;
				SOUND_SetDSP(0,SOUND_DSP_MUTE,(u_int)dspproc);
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


		if (paddata & SCE_PADL1) // select a different sound...
		{
			pg = 1;
			if(PadGate != 1)
			{
			int good = 0;

				while(!good)
				{	
					g_CurrentSoundIndex++;
					if(g_CurrentSoundIndex > SND_BNK_JUNGLE_MAX_SOUND_ID)
						g_CurrentSoundIndex = 0;
					if( *((char *)&SND_BNK_JUNGLE_names[g_CurrentSoundIndex]) != 0 )
						good = 1; // sound is not hidden!
				}
			}
		}
		if (paddata & SCE_PADL2) // select a different sound...
		{
			pg = 1;
			if(PadGate != 1)
			{
			int good = 0;

				while(!good)
				{	
					g_CurrentSoundIndex--;
					if(g_CurrentSoundIndex < 0)
						g_CurrentSoundIndex = SND_BNK_JUNGLE_MAX_SOUND_ID;
					if( *((char *)&SND_BNK_JUNGLE_names[g_CurrentSoundIndex]) != 0 )
						good = 1; // sound is not hidden!
				}
			}
		}
		if (paddata & SCE_PADLright) // load or unload bank
		{
			pg=1;
			if (PadGate!=1)
			{
				if(g_ScreamBank)
				{
					// do unload...
					UnLoadScreamBank();
				}
				else
				{
					// do load...
					LoadScreamBank();
				}
			}
		}
		if (paddata & SCE_PADLdown) // play sound
		{
			pg=1;
			if (PadGate!=1)
			{	

				// one one sound at a time in this demo.
				// so... first stop sound if one is playing...

				// This is a hack... if it is starting we will just block here
				// until we have a sound handle and then we will stop it...
				while(g_SCREAMSoundHandle == -1)
					snd_FlushSoundCommands();
				// sound isn't in its startup phase, so stop it
				// if it is playing...
				if(g_SCREAMSoundHandle)
				{
					snd_StopSound(g_SCREAMSoundHandle);
					g_SCREAMSoundHandle = 0;
				}
				// now go ahead and play				
				PlaySCREAMSound(g_CurrentSoundIndex, &g_SCREAMSoundHandle);
				scream_psflg = 0;
			}
		}
		if (paddata & SCE_PADLleft)
		{
			pg=1;
			if (PadGate!=1)
			{
				if(g_SCREAMSoundHandle != 0)
				{	// sound is playing or starting...
						// This is a hack... if it is starting we will just block here
						// until we have a sound handle and then we will stop it...
						while(g_SCREAMSoundHandle == -1)
							snd_FlushSoundCommands();
						// sound isn't in its startup phase, so stop it
						// if it is playing...
						if(g_SCREAMSoundHandle)
						{
							snd_StopSound(g_SCREAMSoundHandle);
							g_SCREAMSoundHandle = 0;
						}

				}
				scream_psflg = 0;
			}
		}


		PadGate=pg;
	}
}


/*****************************************************************************
MS_BankLoad
	Loads an .MSB file to SPU RAM and patches SFX into multistream system using
	data in .MSH file.
*****************************************************************************/

void MS_BankLoad(void)
{

int File_ID;

u_int StrmChan;

	File_ID=0;
	StrmChan=0;			// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	SOUND_AllocateStreamBuffer((int)0,0x5010,32*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_AllocateStreamBuffer((int)1,0x5010+(32*1024),32*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_SetMaxStreamLimit(2);
	SOUND_AllocateFileMemory(1); 
	SOUND_AllocateSpotFXMemory(1);

	SOUND_CreateFileInfo(File_ID,BankBody,48);

	printf("%x\n",SPU_BASE_ADDR);
	printf("loading..%x\n",SPU_BASE_ADDR+(64*1024));
	SOUND_PatchSFX(0,SPU_BASE_ADDR+(64*1024));
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,SPU_BASE_ADDR+(64*1024));
	FlushIOPCommand(0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..

	printf("**** Next SPU address = %x\n",SOUND_GetSPUAddress());

	SOUND_CreateFileInfo(File_ID,BankBody2,48);
	FlushIOPCommand(0);

	printf("** Loading Samplebank body file (%s) complete **\n\n",BankBody);
}


/*****************************************************************************
LoadScreamBank

	Loads a SCREAM bank. This is the quick and dirty demo using the blocking
	call. Note that loading a scream bank when MS is also loaded forces 
	SCREAM to use SPU DMA channel 1. So... you can load banks while you are 
	streaming under most circumstances as MS mostly uses DMA channel 0.
	
*****************************************************************************/
void LoadScreamBank(void)
{
	if(g_ScreamBank) return; // bank already loaded

	// NOTE: While the IOP -> SPU DMA is compatible with playing streams
	// there is NO cooperation on use of the CD system. So... this bank
	// load will work off the host even if MS is playing streams.
	// BUT... if the path below was to a file on "cdrom0" or if
	// the bank load call was snd_BankLoadByLoc we would have to assure
	// that MS wasn't using the CD system.

	// The other option would be to load the bank into IOP or EE memory
	// using MS's data streaming capabilities and loading the bank
	// into SCREAM using SCREAM's snd_BankLoadFromEE or snd_BankLoadFromIOP
	// functions.

	g_ScreamBank = snd_BankLoad("host0:../scrmdata/Jungle.bnk", 0);
	snd_ResolveBankXREFS();
	snd_FlushSoundCommands();

	printf("done loading bank\n");
}

void UnLoadScreamBank(void)
{
	if(!g_ScreamBank) return; // already unloaded
	
	// this is a hack un case we are asked to unload while load is still in progress...
	while(g_ScreamBank == (SoundBankPtr)0xffffffff)
		snd_FlushSoundCommands();

	if(g_ScreamBank)
		snd_UnloadBank(g_ScreamBank);
	
	g_ScreamBank = NULL;
}

void BankLoadCB(u_int uReturn, u_long uUser)
{
	g_ScreamBank = (SoundBankPtr)uReturn;
}

/*****************************************************************************
PlaySCREAMSound

	Calls down to scream to play the named sound at normal volume and panned 
	center. Again... THIS IS A HACK! It is using the blocking scream call!	
			   
*****************************************************************************/
void PlaySCREAMSound(int index, u_int *handle)
{

	*handle = SND_HANDLE_STARTING;

	// tell scream to play the sound at default volume and pan...
	snd_PlaySoundByName_CB(	NULL, 		// no bank pointer... we'll let scream find the sound
							0,			// no bank name... we'll let scream find the bank
							SND_BNK_JUNGLE_names[index], 	// sound name as a u_long128
							PlaySoundCB,
							(u_long)(u_int)handle
							);
}

void PlaySoundCB(u_int uReturn, u_long uUser)
{
	u_int *handle = (u_int *)(u_int)uUser;
	*handle = uReturn;
}


