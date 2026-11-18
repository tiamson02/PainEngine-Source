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

1) MultiStream and SCREAM working together

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

#include "../../scream/ee/include/scream.h"

// these headers where created by the SCREAM tool and 
// we will use them for the UI...
#include "../scrmdata/Jungle.h"
#include "../scrmdata/Jungle_n.h"

void 	DisplayText(void);
void 	GetUserControl(void);
void 	CheckForExternalLoading(void);
void 	CheckForExternalLoadComplete(void);
u_int 	GetFileUsingID(u_int id);
int 	EXTERNAL_LoadData(void);


extern void sceDevConsLocate(int cd, u_int lx, u_int ly);

SoundBankPtr 	g_ScreamBank 		= NULL;
u_int			g_SCREAMSoundHandle	= 0;
int				g_CurrentSoundIndex = 0;

int 			EXTERNAL_NewRequestCheck = 0;		// MUST be set to 0

u_long128 		pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));	// JOYPAD data
int 			paddata;
int				oldpad;
u_char 			rdata[32];

#define INITIAL_VOLUME	0x0fff	   // of MS VAG stream (turned it down low so we could here the SCREAM sounds

int 			frame				=	0;	// Frame counter
int 			PadGate				=	0;	// Pad button gate flag
int 			psflg				=	0;	// Pause flag
int 			scream_psflg  		=	0;	// Pause flag
u_char 			ExternalLoadFlag	=	0;
u_short 		CurrentVolume		=	INITIAL_VOLUME;
int 			CurrentPitch		=	0x1000;
u_int 			SPUBufferSize;

#define _MOD_ROOT    	"host0:/usr/local/sce/iop/modules/"
#define _MOD_ROOT13  	"host0:/usr/local/sce/iop/modules13/"
#define _MOD_SIO2MAN 	"sio2man.irx"
#define _MOD_PADMAN  	"padman.irx"
#define _MOD_LIBSD   	"libsd.irx"
#define _MOD_SCREAM 	"host0:../../scream/iop/module/scream.irx"
#define _MOD_SCREAM_ERR "host0:../../scream/iop/module/scrm_err.irx"
#define _MOD_STREAM 	"host0:../iop/stream.irx"
#define _MOD_CDVDMAN 	"cdvdman.irx"
#define _MOD_CDVDFSV 	"cdvdfsv.irx"

#define SPU_BASE_ADDR 	0x5010		// Start address of SPU RAM


void Display_MIH_HeaderInfo(u_int HeaderInfo);
void MS_PlayInt(void);

void LoadScreamBank(void);
void BankLoadCB(u_int uReturn, u_long uUser);
void UnLoadScreamBank(void);
void PlaySCREAMSound(int index, u_int *handle);
void PlaySoundCB(u_int uReturn, u_long uUser);
void PlayStream(void);

void InitBackground(void);
void UpdateBackground(void);
void MallocTest(void);

int 		File_ID		=	0;
u_int 		StrmChan	=	0;
u_short 	Pitch;		// Filled in by the MS_PlayInt function.
u_short 	SPUChan		=	2;
u_int		TrackSize; // Size of one data track from Body file (containing 1 interleaved data packet)
u_int		EndOffset;	// Offset through last data track to end of actual data (before zero padding)

u_int 		LoadEEAddr;		// Address of EXTERNAL streamed EE load buffer
int 		DataLoaded	=	0;		// Flag for EXTERNAL loading (0= not loading, 1= loading, 2= loaded)

// To test loading from a CD, set CD_LOADING to 1.
// For this code to still work, burn a CD containing the SAMPDATA directory.
#define 	CD_LOADING 		0	// Set this to 1 for testing loading from CD.

#if CD_LOADING
#define MS6_DATA 			"cdrom0:\\SAMPDATA\\"	// Directory containing sample data
#define IntBody MS6_DATA 	"TEST.MIB"	// Body data (interleaved ADPCM file)
#define IntHeader MS6_DATA 	"TEST.MIH"	// Header data
#else
#define MS6_DATA 			"host0:../sampdata/"	// Directory containing sample data
#define IntBody MS6_DATA 	"test23.mib"
#define IntHeader MS6_DATA 	"test23.mih"	// Header data
#endif

/***********************************************************************************
load_modules
	Loads IRX modules to IOP
***********************************************************************************/
void load_modules (void)
{

    sceSifInitRpc (0);


    printf ("loading modules ....");

    if (sceSifLoadModule (_MOD_ROOT _MOD_SIO2MAN, 0, NULL) < 0)
    {
		printf ("\n  loading %s failed\n", _MOD_SIO2MAN);
		while(1){};
    }
    if (sceSifLoadModule (_MOD_ROOT _MOD_PADMAN, 0, NULL) < 0)
    {
		printf ("\n  loading %s failed\n", _MOD_PADMAN);
		while(1){};
    }

    if (sceSifLoadModule( _MOD_ROOT _MOD_LIBSD, 0, NULL) < 0)
    {
		printf("\n  loading libsd.irx failed\n");
		while(1){};
    }

	// LOAD SCREAM MODULE
    if (sceSifLoadModule (_MOD_SCREAM, 0, NULL) < 0)
	{
		printf("\n  loading scream.irx failed\n");
		while(1){};
    }
	
	// LOAD SCREAM ERROR DECODER MODULE
    if (sceSifLoadModule (_MOD_SCREAM_ERR, 0, NULL) < 0)
	{
		printf("\n  loading scrm_err.irx failed\n");
		while(1){};
    }

	// LOAD STREAMING MODULE
    if (sceSifLoadModule (_MOD_STREAM, 0, NULL) < 0)
	{
		printf("\n  loading stream.irx failed\n");
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
int MS6_Init(void)
{

	// Init MS LIBRARY...
    if (SOUND_InitIOP () < 0)
		return (-1);

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

	// Back to MulstiStream...
	// Initialise SPU...
	SOUND_InitSPU();	
	SOUND_InitCD(SOUND_CD);		// Only required if using MultiStream cd loading routines (SOUND_CD or SOUND_DVD)
	SOUND_InitStreamData(SND_LOAD_INTERNAL,0,0);	// Use MultiStream CD/Host loading routines. Allocate 1 file slot
	
	// limit MS to the bottom 8 voices...
	SOUND_SetMultiStreamVoices(0xff, 0);

	SOUND_AllocateFileMemory(1);
	SOUND_FlushIOPCommand(0);	// Send the three SOUND_ commands above to the IOP and wait for completion
	MS_PlayInt();		// Interleaved playback testbed

	// load a scream bank...
	LoadScreamBank();

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
	scePadInit(0);
	scePadPortOpen(0, 0, pad_dma_buf);

	SetAlphaBlend();
    while( sceGsSyncV(0) == 0 ){}

	frame=0;
	while( 1 )
	{
		GetUserControl();
		DisplayText();

		CheckForExternalLoading();
		CheckForExternalLoadComplete();
		
		// MS command flush...
		SOUND_FlushIOPCommand(SIF_RPCM_NOWAIT);			// Transfer EE command list to IOP buffer
		
		// SCREAM command flush...
		snd_FlushSoundCommands();

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
	printf("************************************************************\n");
	printf("\n");
	printf("MultiStream V7.0 - Jason Page @ SCEE\n");
	printf("Modified by Buzz Burrowes to be SCREAM comaptible\n");
	printf("\n");
	printf("MultiStream sound:\n");
	printf("  Triangle - Pause/Resume\n");
	printf("         X - Play Stream\n");
	printf("    Square - Stop Stream\n");
	printf("    Select - Toggle INTERNAL/EXTERNAL loading\n");
	printf("\n");
	printf("SCREAM sound:\n");
	printf("        Up - Pause/Resume\n");
	printf("      Down - Play Sound\n");
	printf("      Left - Stop Sound\n");
	printf("        L1 - Increment Sound Index\n");
	printf("        L2 - Deccrement Sound Index\n");
	printf("\n");
	printf("************************************************************\n");
	printf("\n");

	sceSifInitRpc(0);
	load_modules ();	// Load all .IRX files into IOP RAM
    MS6_Init();			// Initialise MultiStream V6

	Gs_Initialize();	// Initialise graphics
	InitBackground();
    main_loop ();		// Goto main (while(1)) loop..
	printf("\nEXIT MULTISTREAM V6.0\n");
	return 0;
}


//Pretty sine way background data..
#define BGMAX 200-24
int 	dx1[BGMAX];	// x positions
int 	dx2[BGMAX];	// widths
int 	da[2];		// accelerations
int 	ds[2];		// speeds
int 	dt[2];		// timers
int 	dx[2];		// current x positions
int 	dm[2];		// timer limits
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
void DisplayText(void)
{
int r,i,a;
SOUND_STREAM_INFO buffer;
SOUND_STREAM_INFO *StreamInfo;

	sceDevConsClear(console);

// Update sine wave background data
	UpdateBackground();

	for (i=0;i<BGMAX;i++)
	{
	    SetSprite(0,24+i,640,1,0,0,i/2);	// display blue fade

		r=dx2[i]/2;
		SetSprite(dx1[i],24+i,r/2,1,i/4,0,(i/4)+(abs(dx2[i])/8));	// display sine
		SetSprite(dx1[i]+r,24+i,r/2,1,i/4,0,(i/4)+(abs(dx2[i])/4));

	}
	DrawSprites();	// draw sprites

	StreamInfo=&buffer;
	SOUND_GetStreamInfo(0,StreamInfo);

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream V6.2s - July-2002");
	if (StreamInfo->CDStatus==0)
		sceDevConsPrintf(console,"(CD Free)\n");
	else
		sceDevConsPrintf(console,"(CD Busy)\n");

	if (StreamInfo->CDError)	//SOUND_CD_ERROR==1)
		sceDevConsPrintf(console, "CD ERROR\n");

	sceDevConsPrintf(console, "\n");
	
	sceDevConsPrintf(console, " MulstiStream Sound: (Bottom 8 voices)\n");
	sceDevConsPrintf(console, "          X - Play\n");
	sceDevConsPrintf(console, "     SQUARE - Stop\n");
	sceDevConsPrintf(console, "   TRIANGLE - ");
	if (psflg==1)
		sceDevConsPrintf(console, "Resume\n");
	else			
		sceDevConsPrintf(console, "Pause\n");
	sceDevConsPrintf(console, "     Select - Toggle INTERNAL/EXTERNAL loading (Currently ");
	if (ExternalLoadFlag==0)
		sceDevConsPrintf(console, "INTERNAL)\n");
	else
		sceDevConsPrintf(console, "EXTERNAL)\n");

	sceDevConsPrintf(console, "\n");

	for (i=0;i<2;i++)
	{
		SOUND_GetStreamInfo((u_int)i,StreamInfo);
		if (StreamInfo->Status==STREAM_STATUS_ON)
		{
			if (StreamInfo->Active==0)
				sceDevConsPrintf(console,"Stream Preloading data before keyon..\n");
			else
				sceDevConsPrintf(console,"Strm %x: Stat ACTIVE, SPUChan %x, ID 0x0%x, SPU 0x0%x, Half %x\n",
							i,StreamInfo->SPUChannel,StreamInfo->FileID,StreamInfo->SPUAddress,StreamInfo->PlayHalf);
		}
		else
			sceDevConsPrintf(console,"Stream : OFF\n");
	}
	SOUND_GetStreamInfo(0,StreamInfo);
	
	sceDevConsPrintf(console, "\n");

	sceDevConsPrintf(console, " SCREAM Sound: (Top 40 voices)\n");
	sceDevConsPrintf(console, "       Down - Play \"%s\"\n",(char *)&SND_BNK_JUNGLE_names[g_CurrentSoundIndex]);
	sceDevConsPrintf(console, "    L1 / L2 - Select Sound\n");
	sceDevConsPrintf(console, "       Left - Stop\n");
	sceDevConsPrintf(console, "         Up - ");
	if (scream_psflg==1)
		sceDevConsPrintf(console, "Resume\n");
	else
		sceDevConsPrintf(console, "Pause\n");

	sceDevConsPrintf(console, "      Right - ");
	if(g_ScreamBank)
		sceDevConsPrintf(console, "Unload Bank (0x%8.8x)\n",(u_int)g_ScreamBank);
	else
		sceDevConsPrintf(console, "Load Bank\n");

	sceDevConsPrintf(console, "\n");


	sceDevConsPrintf(console,"KeyStatus:");
	
	// we have set aside the first 8 voices to multistream.
	// so... display multistream voice usage...
	for (i = 0; i < 8; i++)
	{
		a=SOUND_GetKeyStatus(i);
		if (a==0)
		{
			sceDevConsPrintf(console,"-");
		}
		else
			sceDevConsPrintf(console,"m");
	}
	// now show SCREAM voice usage for core 0...
	a = snd_GetVoiceAllocation(0);
	for(i = 8; i < 24; i++)
	{
		if(a & (1L<<i))
			sceDevConsPrintf(console,"s");
		else
			sceDevConsPrintf(console,"-");
	}

	sceDevConsPrintf(console,"/");

	// now show SCREAM voice usage...
	a = snd_GetVoiceAllocation(1);
	for (i = 0; i < 24; i++)
	{
		if(a & (1L<<i))
			sceDevConsPrintf(console,"s");
		else
			sceDevConsPrintf(console,"-");
	}
	
	sceDevConsPrintf(console,"\n");

	sceDevConsPrintf(console,"          (m = multistream, s = scream)\n");

	sceDevConsDraw(console);
}

/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

***********************************************************************************/
void GetUserControl(void)
{
int pg=0;

    oldpad = paddata;
    if(scePadRead(0, 0, rdata) > 0)
    {
		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

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
		if (paddata & SCE_PADLup) // pause or continue sound  
		{
			pg=1;
			if (PadGate!=1)
			{
				if(g_SCREAMSoundHandle && g_SCREAMSoundHandle != SND_HANDLE_STARTING)
				{
					if(scream_psflg)
					{
						snd_ContinueSound(g_SCREAMSoundHandle);
						scream_psflg = 0;
					}
					else
					{
						snd_PauseSound(g_SCREAMSoundHandle);
						scream_psflg = 1;
					}
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
		if (paddata & SCE_PADRdown)
		{
			pg=1;
			if (PadGate!=1)
			{
				PlayStream();
			}
		}
	    if (paddata & SCE_PADRleft)
		{
			pg=1;
			if (PadGate!=1)
			{
				psflg=0;
				SOUND_StopStream((int)StrmChan);	// Stop Parent Stream (also stops child stream)
			}
		}
		if (paddata & SCE_PADRup)	// Triangle PAUSE
		{
			pg=1;
			if (PadGate!=1)
			{
				if (psflg==0)
					SOUND_Pause();
				else
					SOUND_Resume();
				psflg++;
				psflg&=1;
			}
		}
		if (paddata & SCE_PADselect )	// Select = Toggle INTERNAL/EXTERNAL loading
		{
			pg=1;
			if (PadGate!=1)
			{
				ExternalLoadFlag++;
				ExternalLoadFlag&=1;
				PlayStream();
			}
		}
		PadGate=pg;
	}
}

/*****************************************************************************
Load_File
	Loads .MIH (MultiStream Interleaved Header) into EE RAM

	Requires:	*filename		pointer to .MIH filename
	Returns:	addr			EE address where file was loaded to
				0				Failed to load file
*****************************************************************************/
u_int Load_File(u_char *filename)
{
int a,b;
u_int addr;
SOUND_FILE_INFO FileInfo;

	SOUND_AllocateStreamBuffer((int)StrmChan,0,0x10000);	// Allocate stream to load .MIH file to EE RAM

	SOUND_CreateFileInfo(File_ID,filename,0);		// Create file info for filename
	a=SOUND_GetFileIDInfo(File_ID);					// Return file info for file to EE
	SOUND_FlushIOPCommand(0);						// Process both commands on IOP and wait for completion
	b=SOUND_ReturnFileInfo(&FileInfo);				// Copy file info

	if (FileInfo.Size==0)
	{
		printf("File '%s' not found\n",filename);
		while(1){};
	}
	if (a!=b)
	{	
		printf("oops..Something went wrong..%x,%x\n",a,b);
		while(1){};
	}

	printf("file size = 0x%x\n",(u_int)FileInfo.Size);
    addr=(u_int)memalign(64,FileInfo.Size);				// Allocate buffer for loaded data

	SOUND_LoadFile(STREAM_LOAD_EE,StrmChan,File_ID,addr);	// Load file into EE RAM
	SOUND_FlushIOPCommand(0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_CloseStreamBuffer((int)StrmChan);	// Close stream
	SOUND_FreeFileID(File_ID);			// Free file ID
	SOUND_FlushIOPCommand(0);
	return(addr);
}

/*****************************************************************************
Multistream Interleaved testbed..

0 - 3  (Int) Total size of header
4 - 7  (Int) Amount of padding at the end of the header (to make a multiple of 64)
8 - 11(Int) Number of files in the body file
12 - 15(Int) Pitch for each of the files (Only one set for all of them)
16 - 19 (Int) Buffer Size
20 - 23 (Int) Number of 'tracks' for each file
*****************************************************************************/
void MS_PlayInt(void)
{
u_char *Body_Filename;
u_char *Header_Filename;
u_int HeaderInfo;

u_int	NumberOfFiles;	// Number of interleaved files in Body.
u_int	BufferSize;

SOUND_MIH_INFO buf;
SOUND_MIH_INFO *buffer;


	SOUND_SetMaxStreamLimit(2);		// Maximum of two streams playing (one parent, one child)

	File_ID=0x12345;		// ID of file
	StrmChan=0;				// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	buffer=&buf;
	Header_Filename=IntHeader;
	HeaderInfo=Load_File(Header_Filename);	// Load .MIH file into EE RAM

	Display_MIH_HeaderInfo(HeaderInfo);		// Not really required, but shows you info about the .MIH file.

	SOUND_GetMIHInfo(HeaderInfo,buffer,0);	// As it says - Get relevent info about the .MIH file.
	TrackSize=buffer->TrackSize;			// Size of a single track (one half of a buffer for one stream)
	SPUBufferSize=TrackSize;
	EndOffset=buffer->EndOffset;
	
	NumberOfFiles=buffer->NumberOfFiles;	// Number of interleaved streams in .MIB file
	Pitch=(u_short)SOUND_HzToSPU(buffer->Pitch);		// Pitch stored as hz. Convert to SPU value.
	BufferSize=NumberOfFiles*TrackSize*2;	// HalfBuffer size *2 = full IOP buffer size required.

	free((void*)HeaderInfo);	// Free EE buffer

	printf("Stream 0..SPU start address 0x%x, Requesting IOP RAM size 0x%x\n",SPU_BASE_ADDR,BufferSize);
	SOUND_AllocateStreamBuffer(0,SPU_BASE_ADDR,BufferSize);	// Sets IOP and SPU buffer size to BufferSize
	SOUND_ResizeSPUBuffer(0,TrackSize);	// Resize SPU buffer to TrackSize

 	Body_Filename=IntBody;

	printf("Creating Body File (%s) with ID 0x%x\n",Body_Filename,File_ID);
	printf("MIH Size %x, Pad %x\n",buffer->MIHSize,buffer->MIHPad);
	SOUND_CreateFileInfo(File_ID,Body_Filename,0);
	SOUND_FlushIOPCommand(0);						// WAIT FOR TRANSFER REQUEST TO FINISH

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

	g_ScreamBank = (SoundBankPtr)0xffffffff;
	snd_BankLoad_CB("host0:../scrmdata/Jungle.bnk", 0, BankLoadCB, 0);
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

/*****************************************************************************
PlayStream
	Starts a stream (which handles the loading of data)
	Makes this stream the "parent". Tells it to play the first blocks of
	interleaved data.
	Makes a "child" stream, and tells it to play the second blocks of interleaved
	data.
	It doesn't matter if SOUND_AllocateStreamBuffer hasn't been called when
	creating a "child" stream, as you are not using any allocated IOP RAM for this
	stream. But, if you have already called SOUND_AllocateStreamBuffer for a
	child stream, just pass 0 as the last parameter for SOUND_SetStreamChild_Int
	and MultiStream will use the SPU address that you passed when allocating.

	MS6: Note that the SPU BufferSize for each stream now has to be the same size as the TrackSize (the value
	you type into FPacker..)
*****************************************************************************/
void PlayStream(void)
{
	SOUND_PlayStream(File_ID,(int)StrmChan,SPUChan,CurrentVolume,0,Pitch,STREAM_LOOPING,0,0);
	SOUND_SetStreamParent_Int((int)StrmChan,0,TrackSize);	// Parent Stream, track to play, track size
	SOUND_SetStreamChild_Int(StrmChan+1,StrmChan,(u_int)(SPUChan+1),1,0,CurrentVolume,SPU_BASE_ADDR+TrackSize);
	SOUND_SetMIBEndOffset(StrmChan,EndOffset);


// Note: Child streams will use the parents LoadSource, so you only need to set up the parent.
	if (ExternalLoadFlag==1)
		SOUND_SetStreamLoadSource((int)StrmChan,SND_LOAD_EXTERNAL);
}

/*****************************************************************************
Display_MIH_HeaderInfo
	Displays information for .MIH (Interleaved Header) file.
*****************************************************************************/
void Display_MIH_HeaderInfo(u_int HeaderInfo)
{
SOUND_MIH_INFO buf;
SOUND_MIH_INFO *buffer;
u_int RequestFileID=0;

	buffer=&buf;
	SOUND_GetMIHInfo(HeaderInfo,buffer,RequestFileID);
	printf("Number of files interleaved in .MIB file: %d\n",buffer->NumberOfFiles);
	printf("Pitch of files: %d (SPU 0x%x)\n",buffer->Pitch,SOUND_HzToSPU(buffer->Pitch));
	printf("Number of tracks per interleaved file: %d\n",buffer->Tracks);
	printf("Size of a track: %d\n",buffer->TrackSize);
	printf("ID of requested file (%d): %d\n",RequestFileID,buffer->ID);
}


/*****************************************************************************
CheckForExternalLoadComplete
	If an EXTERNAL load was started, this code checks to see if it is completed.
	If it is, it sends the data to IOP RAM and tells MultiStream that its there..
*****************************************************************************/
void CheckForExternalLoadComplete(void)
{
SOUND_EXTERNAL_INFO buf;
SOUND_EXTERNAL_INFO *EXT_Info;
int a;

	if (DataLoaded==2)	// Any EXTERNAL loading completed?
	{
		EXT_Info=&buf;
		a=SOUND_LoadComplete(EXT_Info);	// Also performs a SOUND_GetEXTERNALInfo call.
		if (a!=-1)	// Valid data loaded?
		{
// Transfer data to IOP RAM
			SOUND_TransferDataToIOP(LoadEEAddr,EXT_Info->Buffer,EXT_Info->BufferSize,SOUND_DMA_WAIT);
// Tell IOP that data is ready to use
			SOUND_GetStatus((int)EXT_Info->StreamRequest,(int)EXT_Info->RequestCount);
		}
		DataLoaded=0;	// Clear data loading flag.
	}
}

/*****************************************************************************
CheckForExternalLoading
	Handles loading of data. (Both internal and external)
*****************************************************************************/
void CheckForExternalLoading(void)
{
SOUND_EXTERNAL_INFO buf;
SOUND_EXTERNAL_INFO *EXT_Info;
int a;

	if (DataLoaded!=0)	// EXTERNAL data being loaded, or already loaded?
		return;

	EXT_Info=&buf;

	a=SOUND_ReadyToLoad();
	if (a==-1)		// Any load requested?
	{
		if (DataLoaded==0)	// And EE loading?
			SOUND_HandleCDErrors();	// No..Its safe to check for CD errors
		return;		// No..
	}
	else if (a==SND_LOAD_INTERNAL)
	{
		a=SOUND_LoadComplete(EXT_Info);
		if (a!=-1)
			SOUND_GetStatus((int)EXT_Info->StreamRequest,(int)EXT_Info->RequestCount);
		SOUND_HandleCDErrors();
	}
	else	// EXTERNAL loading..
	{
		SOUND_GetEXTERNALInfo(EXT_Info);
		DataLoaded=1;		// Set flag to say that loading has started
 		LoadEEAddr=(u_int)EXTERNAL_LoadData();	// Load data (returns EE address of data)
		DataLoaded=2;		// Set flag to say data has been loaded (normally to be set after async loading)
	}
}

/*****************************************************************************
EXTERNAL_LoadData

	If using EXTERNAL loading - this function handles loading data that was
	requested from the IOP.
	The IOP has given the EE info that is required via SOUND_GetEXTERNALInfo

	Note: If the size is greater than the remaining file size, you will need to
	either (a) Loop around to the start of the data (+ start offset)
	or (b) Chain to another sample. The IOP code knows what data you
	are meant to be loading, and handles the data correctly.

*****************************************************************************/

int EXTERNAL_LoadData(void)
{
u_int addr;
static u_int reqaddr=0;
int sizeread;	// amount of data read from disk
int fh;
u_char *filename;
u_int sizereq;
u_int fsize;
u_int seekstart;
int CurrentFile;
u_int LSN;		// File LSN

SOUND_EXTERNAL_INFO buf;
SOUND_EXTERNAL_INFO *EXT_Info;

	if (reqaddr!=0)
		free((void*)reqaddr);	// Free previously allocated EE buffer

	EXT_Info=&buf;
	SOUND_GetEXTERNALInfo(EXT_Info);

	sizereq=EXT_Info->BufferSize;	// Size of data buffer that needs to be filled with data
	if (EXT_Info->Destination==STREAM_LOAD_IOP)
	{
	    addr=(u_int)memalign(64,sizereq);		// Allocate memory for stream buffer
	}
	else
	{
		addr=EXT_Info->Buffer;		// EE Address for data to be directly loaded into.
	}

	reqaddr=addr;					// Keep copy of this address for EE->IOP transfer
	seekstart=EXT_Info->FirstSeek;
	fsize=EXT_Info->FirstSize;		// First size of data to transfer

	CurrentFile=EXT_Info->ChainPos;	// Current position through chain list

	if (fsize>sizereq)
		fsize=sizereq;

	while(sizereq!=0)
	{
		LSN=EXT_Info->LSN[CurrentFile];			// You'll need this if loading from CD..!
		filename=IntBody;
	    fh = sceOpen (filename, SCE_RDONLY);		// Open required file
		sceLseek(fh,(int)seekstart,SCE_SEEK_SET);
		sizeread=sceRead(fh,(unsigned char *) addr,(int)fsize);
		addr+=(u_int)sizeread;
		sizereq-=(u_int)sizeread;
		sceClose(fh);		// Loading completed. Close file
		CurrentFile++;
		if (CurrentFile>EXT_Info->ChainCount)
			CurrentFile=0;
		seekstart=EXT_Info->FileSeek[CurrentFile];
		if (sizereq>EXT_Info->PlaySize[CurrentFile])
			fsize=EXT_Info->PlaySize[CurrentFile];
		else
			fsize=sizereq;
	}
	return((int)reqaddr);
}




