//--------------------------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------------------------

#include <eekernel.h>
//#include <stdlib.h>
#include <stdio.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <libvu0.h>
#include <sifdev.h>
#include <malloc.h>

#include "graphics.h"

sceDmaChan *jdmaGIF=NULL;	// DMA channel for string

//--------------------------------------------------------------------------------------------
// D E F I N E S
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// S T R U C T U R E S
//--------------------------------------------------------------------------------------------

typedef struct sprite_tag {

	int r,g,b,a;
	int x0,y0;
	int x1,y1;
	int x;
	int y; 

}SPRITE;

//--------------------------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------------------------

SPRITE sprite[NUM_SPRITES];
//u_char r=127, g=0, b=0;       // cycle colors

sceGsDBuff db;          	// double buffer info
//sceDmaChan *dmaGIF=NULL;	// DMA channel for string
int console;


//--------------------------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------------------------


int SpriteCounter;

//--------------------------------------------------------------------------------------------
// Function: InitSystem()
// Description: Reset the entire system
// Paramaters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------------------------

void InitSystem( void )
{

  // resets the entire VIF0 including VIF0-FIF0
  sceDevVif0Reset();

  // resets the entire VIF1 including VIF1-FIF0
  sceDevVif1Reset();
  
  // reset Vu0
  sceDevVu0Reset();

  // reset Vu1
  sceDevVu1Reset();
  
  // resets devices on the data path to GS (VIF1,Vu0,GIF)
  sceGsResetPath();
  
  // resets the DMAC
  sceDmaReset(1);    
  
}// end InitSystem


//--------------------------------------------------------------------------------------------
// Function: InitGraphics()
// Description: Sets up the drawing environment (screen width/height, z-buffer, etc..)
// Paramaters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------------------------

void JInitGraphics( void )
{

  // init GS board
//  sceGsResetGraph( 0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME );
  
  // set the pixel mode, screen size, depth test, zbuffer, draw clear
//  sceGsSetDefDBuff( &db, SCE_GS_PSMCT32, SCRN_W, SCRN_H, 
//		    					  SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR );
  
  SpriteCounter=0;
}// end InitGraphics



//--------------------------------------------------------------------------------------------
// Function: SetAlphaBlend()
// Description:	Create and send GIF packet to GS
//							- sets alpha blending registers on the GS
// Paramaters: none
// Returns: none
// Notes: Alpha blending register needs to be set for display font
//--------------------------------------------------------------------------------------------

void SetAlphaBlend( void )
 {	

		sceGifPacket gif_pkt;
		u_long128 *base_ptr=NULL;


		// allocate memory
		if( (base_ptr = (u_long128*)malloc(sizeof(u_long128)*5)) == NULL )
		  {
				printf("ERROR: Not enough memory! line:(%d), file:(%s)\n",__LINE__,__FILE__);
				return;
			}

		// set base address of GIF packet
		sceGifPkInit(&gif_pkt, base_ptr );
		sceGifPkReset(&gif_pkt);

		// set GIF tag
		sceGifPkAddGsData(&gif_pkt, SCE_GIF_SET_TAG(3,1,NULL,NULL,SCE_GIF_PACKED,1));
		sceGifPkAddGsData(&gif_pkt, 0xEL);

		// alpha blend settings 									   A  B  C  D  FIX
		sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ALPHA(0, 2, 0, 1, 0));
		sceGifPkAddGsData(&gif_pkt, SCE_GS_ALPHA_1);
		
		// alpha blending in units of pixels turned off
		sceGifPkAddGsData(&gif_pkt, 0);
		sceGifPkAddGsData(&gif_pkt, SCE_GS_PABE);
		
		// alpha correction value off
		sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_FBA(0));
		sceGifPkAddGsData(&gif_pkt, SCE_GS_FBA_1);

		// get packet size in quad words
		sceGifPkTerminate(&gif_pkt);

		//printf("size:(%d)\n",sceGifPkSize(&gif_pkt));
		
		// DMA send (Normal Mode) to GS via path 3		
		FlushCache(WRITEBACK_DCACHE);
		sceDmaSendN(jdmaGIF, (u_long128*)((u_int)gif_pkt.pBase&DMA_MEM), (int)sceGifPkSize(&gif_pkt));
		sceGsSyncPath(0, 0);

		free(base_ptr);

}// end SetAlphaBlend


//--------------------------------------------------------------------------------------------
// Function: SpritePacket()
// Description:	Sprites are packed using REGLIST mode on scratch pad memory and sent
//							to GS by DMA transfer.
// Paramaters: none
// Returns: none
// Notes: - number of sprites is limited to 511 since GIF packet is using scratch pad memory.
//				- packed data is 64-bits
//--------------------------------------------------------------------------------------------

void SpritePacket(int ls,int count)
{

sceGifPacket gif_pkt;
int nloop = count;
int num_regs = 4;
int cnt0;


		// set base address of GIF packet 
		sceGifPkInit(&gif_pkt,(u_long128*)SPR_MEM);
		sceGifPkReset(&gif_pkt);

		// set GIF tag
		sceGifPkAddGsData(&gif_pkt, SCE_GIF_SET_TAG(nloop,1,NULL,NULL,SCE_GIF_REGLIST,num_regs) );
		sceGifPkAddGsData(&gif_pkt, 0x5510L ); 

		for( cnt0=ls; cnt0<(ls+count); cnt0++)
 		{

 			// set PRIM info
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE,0,0,0,0,0,0,0,0) );

 			// set RGBA of sprite
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[cnt0].r, sprite[cnt0].g, 
 			sprite[cnt0].b, 0x0, 0x3f800000));


 			// set upper-left position of sprite
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(sprite[cnt0].x0,sprite[cnt0].y0,1) );

 			// set lower-right position of sprite
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(sprite[cnt0].x1,sprite[cnt0].y1,1) );

 		}
		// set packet size in quad words
		sceGifPkTerminate(&gif_pkt);

		//printf("size:(%d)\n",sceGifPkSize(&gif_pkt));

		// DMA send (Normal Mode) to GS via path 3		
		sceDmaSendN(jdmaGIF,(u_long128 *)((u_int)gif_pkt.pBase|DMA_SPR), (int)sceGifPkSize(&gif_pkt));
		sceGsSyncPath(0, 0);

//		cnt0=SpriteCounter;
//		return(cnt0);


}// end SpritePacket


void SetSprite(int x,int y,int w,int h,int r,int g,int b)
{
    x<<=4;
    y<<=4;
    w<<=4;
    h<<=4;
	// set new locations
	sprite[SpriteCounter].x = x;
	sprite[SpriteCounter].y = y;
	sprite[SpriteCounter].x0 = OFFX+sprite[SpriteCounter].x;
	sprite[SpriteCounter].y0 = OFFY+sprite[SpriteCounter].y;
	sprite[SpriteCounter].x1 = sprite[SpriteCounter].x0+w;
	sprite[SpriteCounter].y1 = sprite[SpriteCounter].y0+h;
	sprite[SpriteCounter].r = r;
	sprite[SpriteCounter].g = g;
	sprite[SpriteCounter].b = b;
	SpriteCounter++;
}

int DrawSprites(void)
{
int cnt0,t,ls;
	ls=0;
	if (SpriteCounter==0)
		return(0);
	t=SpriteCounter;
	while(t>0)
	{
		if (t>511)
		{
			SpritePacket(ls,511);
			ls+=511;
			t-=511;
		}
		else
		{
			SpritePacket(ls,t);
			t=0;
		}
	sceGsSyncPath(0, 0);
	}

	cnt0=SpriteCounter;
	SpriteCounter=0;
	return(cnt0);
}

/***********************************************************************************
Gs_Initialize
	Graphics stuff. Don't know. Don't care.
***********************************************************************************/
void Gs_Initialize(void)
{
	int sindex;
	sceVif1Packet packet[2];
	sceDmaEnv env;
	sceDmaChan *p1;
	u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1),
			 0x000000000000000eL };
	sceDevVif0Reset();
	sceDevVu0Reset();
	sceGsResetPath();
	sceDmaReset(1);

	sceVif1PkInit(&packet[0], (u_long128 *)SUPPER);
	sceVif1PkInit(&packet[1], (u_long128 *)SLOWER);

	sceDmaGetEnv(&env);
	env.notify = 1<<SCE_DMA_VIF1; // notify channel 
	sceDmaPutEnv(&env);

	p1 = sceDmaGetChan(SCE_DMA_VIF1);
	p1->chcr.TTE = 1;

	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	sceGsSetDefDBuff(&db, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZGEQUAL, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	db.clear0.rgbaq.R = 0x00;
	db.clear0.rgbaq.G = 0x00;
	db.clear0.rgbaq.B = 0x00;

	db.clear1.rgbaq.R = 0x00;
	db.clear1.rgbaq.G = 0x00;
	db.clear1.rgbaq.B = 0x00;

	FlushCache(0);

	sindex = 0;
	sceVif1PkReset(&packet[sindex]);
	sceVif1PkCnt(&packet[sindex], 0);
	sceVif1PkOpenDirectCode(&packet[sindex], 0);
	sceVif1PkOpenGifTag(&packet[sindex], *(u_long128*)&giftagAD[0]);

	sceVif1PkReserve(&packet[sindex],
		(u_int)sceGsSetDefAlphaEnv((sceGsAlphaEnv *)packet[sindex].pCurrent,0)
		* 4);
	sceVif1PkCloseGifTag(&packet[sindex]);
	sceVif1PkCloseDirectCode(&packet[sindex]);
	sceVif1PkEnd(&packet[sindex], 0);
	sceVif1PkTerminate(&packet[sindex]);

	// kick Gs initialize packet 
	sceDmaSend(p1,(u_int *)
		(((u_int)packet[sindex].pBase&0x0fffffff)|0x80000000));

	// display next in odd field when interlace 
	while(!sceGsSyncV(0)){}

	JInitGraphics();
    jdmaGIF = sceDmaGetChan( SCE_DMA_GIF );
	sceDevConsInit();
	console = sceDevConsOpen(OFFX + (16<<4), OFFY + (24<<4), 75, 30);
	sceDevConsClear(console);
	SetAlphaBlend();
}

void DisplayScreen(int frame)
{
	if(frame&0x01)
		sceGsSetHalfOffset(&db.draw1,2048,2048, (short)(sceGsSyncV(0)^0x01));
	else
		sceGsSetHalfOffset(&db.draw0,2048,2048, (short)(sceGsSyncV(0)^0x01));
	sceGsSwapDBuff(&db, frame);
	FlushCache(0);
	sceGsSyncPath(0,0);
}
