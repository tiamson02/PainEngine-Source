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
 *      7.00 (DSP)      Jun,18,2003      Jason Page 
 */

/**********************************************************************************
MultiStream DSP (V7.0)
(c) 2000-2003 Sony Computer Entertainment Europe
Programmer: Jason Page (Jason_Page@scee.net)
Last update: June 2003

"Laser-Sword"
-------
This demo shows:

1) DSP effects to create a realistic "laser-sword" sound effect
2) Similar to the CarRevs sample. Also using sidechain compression, so that the hit SFX can be heard
over the top of the sword sound..

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
int padstarted=0;
int movement=0;
int movement2=0;
int lastpos=0;
int hitcount=0;
u_int onoff=0;

int XPos=0;
int YPos=0;

int LXPos=0;
int LYPos=0;

int HXPos=96;
int HYPos=96;
int HXSpd=0;
int HYSpd=0;
int HFlag=0;

#define _MOD_ROOT    "host0:/usr/local/sce/iop/modules/"
#define _MOD_ROOT13  "host0:/usr/local/sce/iop/modules13/"
#define _MOD_SIO2MAN "sio2man.irx"
#define _MOD_PADMAN  "padman.irx"
#define _MOD_LIBSD   "libsd.irx"
#define _MOD_STREAM "host0:../iop/stream.irx"
#define _MOD_CDVDMAN "cdvdman.irx"
#define _MOD_CDVDFSV "cdvdfsv.irx"

#define SPU_BASE_ADDR 0x5010		// Start address of SPU RAM

u_long128 pad_dma_buf[scePadDmaBufferMax] __attribute__((aligned (64)));	// JOYPAD data

void MS_BankLoad(void);
void InitBackground(void);

int File_ID=0;

// To test loading from a CD, set CD_LOADING to 1.
// For this code to still work, burn a CD containing the SAMPDATA directory.
#define CD_LOADING 0	// Set this to 1 for testing loading from CD.

#if CD_LOADING
#define MS7_DATA "cdrom0:\\SAMPDATA\\"	// Directory containing sample data
#else
#define MS7_DATA "host0:../sampdata/"	// Directory containing sample data
#endif
#define MainNoise MS7_DATA "LIGHT2.VAG"
#define StartNoise MS7_DATA "LIGHTSTART.VAG"
#define EndNoise MS7_DATA "LIGHTEND.VAG"
#define HitNoise MS7_DATA "LIGHTHIT.VAG"

#define GFX_BODY 1
#define GFX_SWORD 2
#define GFX_TGLOGO 3
#define GFX_MSLOGO 4

#define GROUP_0 0
#define GROUP_1 1
#define GROUP_2 2

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
		printf("  loading %s failed\n",_MOD_LIBSD);
		while(1){};
    }

// LOAD MULTISTREAM MODULE
    if (sceSifLoadModule (_MOD_STREAM, 0, NULL) < 0)
	{
		printf("  loading %s failed\n",_MOD_STREAM);
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
int FilterFreq=0;
int FilterRes=0;
int DistortLevel=0;
int SwordVolume=0;

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

	SOUND_AllChannelOutputsOff(0);				// Turn ALL of core1 output off..
	SOUND_AllChannelEffectsOn(0);				// Turn ALL of core1 effects on..

// Distortion will process PCM data first, followed by filter, and finally, Sidechain compression..

	SOUND_DSPSetEffect(0,0,SOUND_FLAG_DSP_DISTORT);			// Slot 0 - Distortion
	SOUND_DSPSetEffect(1,0,SOUND_FLAG_DSP_FILTER);			// Slot 1 - Filter
//	SOUND_DSPSetEffect(2,0,SOUND_FLAG_DSP_COMPRESS_SIDE); 	// Slot 2 - sidechain compression

	SOUND_SetDSP(0,SOUND_DISTORTION_LEVEL,(u_int)DistortLevel);	// Set up initial value for distortion
	SOUND_SetDSP(1,SOUND_FILTER_TYPE,SOUND_FILTER_LOWPASS);	// Set up initial values for filter
	SOUND_SetDSP(1,SOUND_FILTER_RESONANCE,(u_int)FilterRes);
	SOUND_SetDSP(1,SOUND_FILTER_FREQ,(u_int)FilterFreq);
	SOUND_SetPCMVolume(0x7fff,0x7fff);
//	SOUND_SetDSP(2,SOUND_COMPRESS_THRESHOLD,2000);
//	SOUND_SetDSP(2,SOUND_COMPRESS_RATIO,800);
//	SOUND_SetDSP(2,SOUND_COMPRESS_ATTACK,15);
//	SOUND_SetDSP(2,SOUND_COMPRESS_RELEASE,4);
//	SOUND_SetDSP(2,SOUND_COMPRESS_GAIN,0);

#endif

	SOUND_FlushIOPCommand(0);


	MS_BankLoad();		// Load samples..into SPU RAM..

	SOUND_PlaySFX(0,0,0,0,0,0,0);		// Play "Laser-Sword" sound...

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

	GRAP_SendTIM2(GFX_BODY,MS7_DATA "jed.tm2");
	GRAP_SendTIM2(2,MS7_DATA "sabre2.tm2");
	GRAP_SendTIM2(4,MS7_DATA "mslogo.tm2");
	GRAP_SendTIM2(3,MS7_DATA "tglogo.tm2");

	sceGsSwapDBuff(&db, 0);
	sceGsSwapDBuff(&db, 1);

	frame=0;
	SetAlphaBlend();
    while( sceGsSyncV(0) == 0 ){}

	while( 1 )
	{

		GetUserControl();
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
	printf("MultiStream V7.0 (DSP) - Jason Page @ SCEE\n");
	printf("Code to play both ADPCM streams and resident SFX\n");
	printf("Laser-Sword sample code\n");
	printf("(Uses only one main sample for the sword sound)\n");
	printf("Triangle - Pause/Resume\n");
	printf("Square - Start/stop audio\n");
	printf("X - DSP on/off\n");
	printf("Right analog stick - move sword\n");
	printf("(You MUST enable analog mode for this demo to work)\n");
	printf("************************************************************\n");
	printf("\n");
#endif

	sceSifInitRpc(0);
	load_modules ();	// Load all .IRX files into IOP RAM
    MS7_Init();			// Initialise MultiStream V7
	Gs_Initialize();	// Initialise graphics
	InitBackground();
    main_loop ();		// Goto main (while(1)) loop..
	printf("\nEXIT MULTISTREAM V7\n");
	return 0;
}

#define BGMAX 255
#define XCMax 300
#define YCMax 250

int XSpd=16;
int YSpd=0;
int XAcc=4;
int YAcc=3;
int XT2=0;
int YT2=0;
int XCounter=XCMax/2;
int YCounter=YCMax/2;
int dx1[BGMAX];	// x positions
int dy1[BGMAX];	// y positions

/***********************************************************************************
InitBackground
	Initialises the sinewave background..
***********************************************************************************/
void InitBackground(void)
{
int i;
	for (i=0;i<BGMAX;i++)
	{
		dx1[i]=rand()%640;
		dx1[i]*=256;
		dy1[i]=rand()%224;
		dy1[i]*=256;
	}
}

void HandleStarField(void)
{
int i;

	if (padstarted==1)
	{
		XAcc=XPos-0x40;
		YAcc=YPos-0x40;
		if ((XAcc>-0x20)&&(XAcc<0x20))
		{
			if (XSpd>0)
				XAcc=-32;
			else
				XAcc=32;
		}
		if ((YAcc>-0x20)&&(YAcc<0x20))
		{
			if (YSpd>0)
				YAcc=-32;
			else
				YAcc=32;
		}
		XAcc/=8;
		YAcc/=8;
		XSpd+=XAcc;
		YSpd+=YAcc;
		if (XSpd>512)
			XSpd=512;
		else if (XSpd<-512)
			XSpd=-512;
		if (YSpd>512)
			YSpd=512;
		else if (YSpd<-512)
			YSpd=-512;
	}

		GRAP_RotateY(GROUP_1,YSpd);

// Draw StarField..

	for (i=0;i<BGMAX;i++)
	{
	    SetSprite2(dx1[i]/512,dy1[i]/256,1,1,i,i,i,0,GROUP_0);	// Display stars
		dx1[i]-=i*(XSpd/32);			// (lighter colours move faster...)
		dy1[i]-=i*(YSpd/32);			// (lighter colours move faster...)
		if (dx1[i]<0)
			dx1[i]+=256*640;
		else if (dx1[i]>(256*640))
			dx1[i]-=256*640;
		if (dy1[i]<0)
			dy1[i]+=256*224;
		else if (dy1[i]>(256*224))
			dy1[i]-=256*224;
	}
}

void HandleSwordCollision(void)
{
int i;

	if ((onoff!=0)||(SOUND_GetKeyStatus(24)!=0))
	{

		i=255-(128*psflg);


	    SetSprite2((XPos+64)/2,YPos/2+144,6/2,6,255,255,255,0,GROUP_0);

		SetSprite2((420+-64)/2,140+-74,(128+64)/2,128,255,255,255,GFX_BODY,GROUP_0);	// body
		SetSprite2(-64/2,-64,(128+64)/2,128,255,255,255,GFX_SWORD,GROUP_1);	// sword

		if (SOUND_GetKeyStatus(25)!=0)
		    SetSprite2((HXPos+64)/2,HYPos/2+144,6/2,6,i,i,0,0,GROUP_0);
		else
		    SetSprite2((HXPos+64)/2,HYPos/2+144,6/2,6,0,i,i/2,0,GROUP_0);

	    SetSprite2(64/2,144,(128+6)/2,1,0,255,0,0,GROUP_0);
	    SetSprite2(64/2,144+64+4,(128+6)/2,1,0,255,0,0,GROUP_0);
	    SetSprite2(64/2,144,1,64+4,0,255,0,0,GROUP_0);
	    SetSprite2((64+128+6)/2,144,1,64+4,0,255,0,0,GROUP_0);


		if (SOUND_GetKeyStatus(25)==0)
		{
			if ((XPos>=HXPos-10)&&(XPos<HXPos+10)&&(YPos>=HYPos-10)&&(YPos<HYPos+10)&&(psflg==0))
			{
				i=11000+rand()%1000;
				SOUND_PlaySFX(2,25,0x3fff,0x3fff,(int)SOUND_HzToSPU((u_int)i),0,12+rand()%3);
				SOUND_StopSound(25);	// Stopping now will cause a fadeout, depending on release rate above..
				HXSpd=XPos-LXPos;
				HYSpd=YPos-LYPos;
				HYSpd+=rand()%6;
				HYSpd-=3;
				HXSpd+=rand()%6;
				HXSpd-=3;

				HXSpd*=32;
				HYSpd*=32;
			}
		}
		HXPos+=HXSpd/64;
		HYPos+=HYSpd/64;
		if (HXPos<0)
		{
			HXPos=0;
			HYSpd+=64;
			HXSpd=-HXSpd;
		}
		else if (HXPos>127)
		{
			HXPos=127;
			HYSpd-=64;
			HXSpd=-HXSpd;
		}
		if (HYPos<0)
		{
			HYPos=0;
			HXSpd+=64;
			HYSpd=-HYSpd;
		}
		else if (HYPos>127)
		{
			HYPos=127;
			HXSpd-=64;
			HYSpd=-HYSpd;
		}

		if (HXSpd<0)
			HXSpd++;
		else if (HXSpd>0)
			HXSpd--;
		if (HYSpd<0)
			HYSpd++;
		else if (HYSpd>0)
			HYSpd--;
		LXPos=XPos;
		LYPos=YPos;
	}

}

/***********************************************************************************
DisplayText
	Displays all info on screen.
***********************************************************************************/

#define INFO_YPOS 13
void DisplayText(void)
{
int i;
int j;
static int n=0;

	sceDevConsClear(console);

	GRAP_SetGroupXY(GROUP_0,0,0,0);
	GRAP_SetGroupXY(GROUP_1,438/2,410,-100);	//sword
	GRAP_SetGroupXYOffset(GROUP_1,54,-128);
	GRAP_SetGroupXY(GROUP_2,200,200,0);

	HandleSwordCollision();
	if (padstarted!=0)
		HandleStarField();

	DrawSprites();	// draw sprites

	sceDevConsLocate(console, 0, 0);
	sceDevConsPrintf(console, "MultiStream 7.0 DSP - June 2003 SCEE\n");

	if (padstarted==0)
	{
		sceDevConsPrintf(console, "\n\n");
		sceDevConsPrintf(console, "PLEASE TURN ON ANALOG MODE\n");
		sceDevConsDraw(console);


//		GRAP_RotateY(GROUP_2,n);

		j=n&511;
		if (j<256)
			i=j;
		else if (j<512)
			i=(512-j);
		SetSprite2(96,64,128,128,i,i,i,GFX_TGLOGO+(n/512),GROUP_0);
		n++;
		n&=1023;

#if 0
i=32;
GRAP_CreateCube(0,0,0,i,0,2);
GRAP_CreateCube(i/2,0,0,i,0,2);
GRAP_CreateCube(-i/2,0,0,i,0,2);
GRAP_CreateCube(0,i/2,0,i,0,2);
GRAP_CreateCube(0,-i/2,0,i,0,2);
GRAP_CreateCube(0,0,i/2,i,0,2);
GRAP_CreateCube(0,0,-i/2,i,0,2);
GRAP_RotateY(GROUP_2,n);
GRAP_RotateX(GROUP_2,n);
#endif
		DrawSprites();	// draw sprites

		return;
	}

#if 1
	sceDevConsPrintf(console, "DSP sample code (Filter and Distortion)\n");
	sceDevConsPrintf(console, "(Uses only one main sample for the sword sound)\n\n");

	sceDevConsPrintf(console, "Right analog stick UP - Rev engine\n");
	sceDevConsPrintf(console, "X - DSP processing On/Off (Currently ");
	if (dspproc==1)
		sceDevConsPrintf(console, "OFF)\n");
	else
		sceDevConsPrintf(console, "ON)\n");

	sceDevConsPrintf(console, "SQUARE - Light-Sword On/Off\n");

	sceDevConsPrintf(console, "TRIANGLE - Pause/Resume   ");

	if (psflg==1)
		sceDevConsPrintf(console,"PAUSED - Press Triangle To Resume\n");
	else
		sceDevConsPrintf(console,"\n");

		sceDevConsPrintf(console,"\n\n");

	sceDevConsPrintf(console,"Volume     %x\n",SwordVolume);
	sceDevConsPrintf(console,"Filter     %x\n",FilterFreq);
	sceDevConsPrintf(console,"Resonance  %x\n",FilterRes);
	sceDevConsPrintf(console,"Distortion %x\n",DistortLevel);
#endif

#if 0
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			sceDevConsPrintf(console,"%f,",printMatrix[0][i][j]);
		}
		sceDevConsPrintf(console,"\n");
	}
		sceDevConsPrintf(console,"\n");
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			sceDevConsPrintf(console,"%f,",printMatrix[1][i][j]);
		}
		sceDevConsPrintf(console,"\n");
	}
		sceDevConsPrintf(console,"\n");
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			sceDevConsPrintf(console,"%f,",printMatrix[2][i][j]);
		}
		sceDevConsPrintf(console,"\n");
	}
#endif
//	sceDevConsPrintf(console,"\n");
//	sceDevConsPrintf(console,"Laser Movement 1 %x\n",movement);
//	sceDevConsPrintf(console,"Laser Movement 2 %x\n",movement2);

	i=255-(128*psflg);

	if (SOUND_GetKeyStatus(25)==0)
		SetSprite2(144/2,INFO_YPOS*8,(SwordVolume)/34,4,i,0,i/2,0,GROUP_0);
	else
		SetSprite2(144/2,INFO_YPOS*8,(SwordVolume)/34,4,i,i,0,0,GROUP_0);
	SetSprite2(144/2,(INFO_YPOS+1)*8,FilterFreq/2,4,0,i,i,0,GROUP_0);
	SetSprite2(144/2,(INFO_YPOS+2)*8,FilterRes,4,i,0,i/2,0,GROUP_0);
	SetSprite2(144/2,(INFO_YPOS+3)*8,DistortLevel,4,0,i,i,0,GROUP_0);

	DrawSprites();	// draw sprites
	sceDevConsDraw(console);
}


void ProcessLaserSword(int joystick)
{
int a,r;
int decelerate=0;
int flag=0;
static HCnt=0;
// Note that "revs" is the value read from the joypad. Therefore, due to the analog stick not being very
// good at centering on 0, you need to check at around +/- 28 or so, just to make sure that it doesn't start
// accelerating even when nobody is near the joypad!

	if (psflg==0)						// Not paused?
	{
		a=abs(lastpos-joystick);		// Get difference between last and current joystick position
		lastpos=joystick;
		movement+=a;					// Increase value by this amount..
		movement-=16;					// And take some away..
		if (movement<0)					// Limit it...
			movement=0;
		if (movement>128)
			movement=128;
		movement2+=a;					// Do the same again..
		movement2-=6;					// But don't take so much away (Will take longer to fade out)
		if (movement2<0)
			movement2=0;
		if (movement2>128)
			movement2=128;

// Set Pitch
		a=(int)SOUND_HzToSPU(44100);		// Start with base pitch


		if ((HCnt==0)&&(SOUND_GetKeyStatus(25)==0))	// Not hit object for a while?
		{
			HCnt=10;	// Reset counter
		}

		if ((SOUND_GetKeyStatus(25)!=0)&&(HCnt!=0))	// Struck object?
		{
			a+=128*4;					// Slightly higher pitch than if moving fast..
			HCnt--;
		}
		else if (movement>96)			// Moving stick at a fast speed?
			a+=96*4;					// Use this to modify base pitch

		SOUND_SetChannelPitch(0,a);		// Set pitch

// Set volume

		a=movement;						// Get speed stick is moving at..
		a*=48;							// Scale it..
		a+=0xc00;						// Add base volume..

		if ((onoff==0)&&(SOUND_GetKeyStatus(24)==0))	// Laser-Sword turned off?
			a=0;		// Yes..
		else
		{
			hitcount+=movement;		// Randomly make Laser-Sword hit sound...Just for "realism"...
			if (hitcount>0x1000)
			{
				hitcount=rand()%300;
				r=11000+rand()%1000;
//				SOUND_PlaySFX(2,25,0x3fff,0x3fff,SOUND_HzToSPU(r),0,12+rand()%3);
				SOUND_StopSound(25);	// Stopping now will cause a fadeout, depending on release rate above..
			}
		}
		SwordVolume=a;
		SOUND_SetChannelVolumeSmooth(0,(short)SwordVolume,(short)SwordVolume);
		SOUND_SetChannelVolumeSmooth(1,(short)SwordVolume,(short)SwordVolume);

// Now set filters and distortion..

		a=movement2;		// Use parameter which takes longer to fade out for filter..
		a+=16;				// Add base value.
		if (a>512)			// Limit (Filter Freq = 0-512)
			a=512;

		FilterFreq=a;			// Set filter freq limit
		FilterFreq+=rand()%8;	// Add a bit of randomness, just to make things sound less "loopy.."
		FilterRes=a/2;			// Set resonance (0-256)
		DistortLevel=8+(a/3);	// Set distortion
	
		SOUND_SetDSP(0,SOUND_DISTORTION_LEVEL,(u_int)DistortLevel*4);
		SOUND_SetDSP(1,SOUND_FILTER_FREQ,(u_int)FilterFreq*2);
		SOUND_SetDSP(1,SOUND_FILTER_RESONANCE,(u_int)FilterRes);
	}
}

int LastR[8];
/***********************************************************************************
GetUserControl
	Update user control and call any SOUND_ functions that may be required

	returns:	0	= All ok.
				-1	= START pressed (quit)

***********************************************************************************/
void GetUserControl(void)
{
int pg;
int r;

	pg=0;
    oldpad = paddata;

    if(scePadRead(0, 0, rdata) > 0)
    {
		paddata=rdata[5];						// right joystick Y

		if (paddata!=0)							// Analog enabled..?
		{
			if (padstarted==0)					// Hasn't been enabled yet..
			{
				onoff=1;						// Flag for "laser-sword" start noise..
				padstarted=1;					// set pad started flag..
			}
		}

		if (padstarted==1)
		{
			YPos=paddata;
			XPos=rdata[4];
			paddata+=XPos;


			XPos/=2;
			YPos/=2;
			r=abs(0x80-paddata);
			ProcessLaserSword(r);

r=(XPos*180)/128;

//r=(LastR[0]-(-90+XPos));
//r/=8;
//LastR[0]=LastR[8]-r;
//LastR[8]=-90+XPos;
GRAP_RotateX(GROUP_1,-140+r);	//XPos);
//printf("%d\n",-90+r);
		}
		else
			return;

		if (onoff==1)		// Play laser-sword start sound?
		{
			onoff=2;
			SOUND_PlaySFX(1,24,0x3fff,0x3fff,(int)SOUND_HzToSPU(11025),0,0);
		}

		paddata = 0xffff ^ ((rdata[2] << 8) | rdata[3]);

//		if (paddata & SCE_PADLleft)
//			Rotate(-1);
//		if (paddata & SCE_PADLright)
//			Rotate(1);

		if (paddata & SCE_PADRleft)
		{
			pg=1;
			if ((PadGate!=1)&&(psflg==0))	//not paused..
			{
				if (onoff==2)
				{
					SOUND_PlaySFX(3,24,0x3fff,0x3fff,(int)SOUND_HzToSPU(22050),0,0);	// Turn off "laser-sword"
					onoff=0;
				}
				else if (onoff==0)
				{
					onoff=1;	// Set flag to start "laser-sword"
				}
			}
		}
		else if (paddata & SCE_PADRdown)
		{
			pg=1;
			if (PadGate!=1)		// Mute / un-Mute DSP processing?
			{
				dspproc=1-dspproc;
				SOUND_SetDSP(0,SOUND_DSP_MUTE,(u_int)dspproc);
				SOUND_SetDSP(1,SOUND_DSP_MUTE,(u_int)dspproc);
				SOUND_SetDSP(2,SOUND_DSP_MUTE,(u_int)dspproc);

			}
		}

		else if (paddata & SCE_PADRup)
		{
			pg=1;
			if (PadGate!=1)		// Pause / Resume ?
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

/*******************************************************************************
MS_BankLoad
	Loads an .MSB file to SPU RAM and patches SFX into multistream system using
	data in .MSH file.
*******************************************************************************/

void MS_BankLoad(void)
{

int File_ID;

u_int StrmChan;

	File_ID=0;			// ID of file
	StrmChan=0;			// Stream channel to use for loading .MSB file into SPU RAM from host or cdrom.

	SOUND_AllocateStreamBuffer((int)StrmChan,0,64*1024);	// 128K IOP buffer for streaming resident FX to
	SOUND_SetMaxStreamLimit(1);
	SOUND_AllocateFileMemory(1);
	SOUND_AllocateSpotFXMemory(4);

	printf("loading..SFX\n");
	SOUND_PatchSFX(0,SPU_BASE_ADDR);
	SOUND_CreateFileInfo(File_ID,MainNoise,48);
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,SPU_BASE_ADDR);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..

	SOUND_PatchSFX(1,SOUND_GetSPUAddress());
	SOUND_CreateFileInfo(File_ID,StartNoise,48);
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..

	SOUND_PatchSFX(2,SOUND_GetSPUAddress());
	SOUND_CreateFileInfo(File_ID,HitNoise,48);
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..

	SOUND_PatchSFX(3,SOUND_GetSPUAddress());
	SOUND_CreateFileInfo(File_ID,EndNoise,48);
	SOUND_LoadFile(STREAM_LOAD_SPU,StrmChan,File_ID,0);
	SOUND_WaitForFileToLoad(StrmChan,SOUND_WAIT_COLD);		// Wait for file to load
	SOUND_FreeFileID(File_ID);		// No need to keep this ID around now..
	printf("SFX Loading complete\n");
	SOUND_CloseStreamBuffer((int)StrmChan);	// Free IOP RAM used for ADPCM stream buffer.

	FlushIOPCommand(0);

//	printf("** Loading Samplebank body file (%s) complete **\n\n",BankBody);
}
