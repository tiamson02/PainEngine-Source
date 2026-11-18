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
#include <math.h>

#include "graphics.h"
#include "tim2.h"

sceDmaChan *jdmaGIF=NULL;	// DMA channel for string

#define GRAPH_LOC 32
//--------------------------------------------------------------------------------------------
// D E F I N E S
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// S T R U C T U R E S
//--------------------------------------------------------------------------------------------

#define TEXTURE_MAX 256

typedef struct group_tag {

	char Status;
	int x;
	int y;
	int z;
	int ox;
	int oy;
}GROUP;

typedef struct texture_tag {

	char Status;
	int Width;
	int Height;
	int WidthLog2;
	int HeightLog2;
}TEXTURE;

typedef struct sprite_tag {

	int group;
	int r,g,b,a;
	int x0,y0;
	int x1,y1,z1;
	int x2,y2,z2;
	int x3,y3,z3;
	int x4,y4,z4;
	int x;
	int y;
	int z; 
	int texture;

}SPRITE;

//--------------------------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------------------------

TEXTURE TextureList[TEXTURE_MAX];
SPRITE sprite[NUM_SPRITES];
//u_char r=127, g=0, b=0;       // cycle colors

sceGsDBuff db;          	// double buffer info
//sceDmaChan *dmaGIF=NULL;	// DMA channel for string
int console;


//--------------------------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------------------------


int SpriteCounter;

#define PI (3.141592f)
#define MAX_GROUPS 64
GROUP GroupList[MAX_GROUPS];

sceSamp0FVECTOR camera_p[MAX_GROUPS] = { 0.0f, 0.0f, -25.0f, 0.0f };
sceSamp0FVECTOR camera_zd[MAX_GROUPS] = { 0.0f, 0.0f, 1.0f, 1.0f };
sceSamp0FVECTOR camera_yd[MAX_GROUPS] = { 0.0f, 1.0f, 0.0f, 1.0f };
sceSamp0FVECTOR camera_rot[MAX_GROUPS] = { 0.0f, 0.0f, 0.0f, 0.0f };

sceSamp0FVECTOR light0[MAX_GROUPS] = { 0.0f, 1.5f, 0.5f, 0.0f };
sceSamp0FVECTOR light1[MAX_GROUPS] = { 1.5f, -0.5f, 0.5f, 0.0f };
sceSamp0FVECTOR light2[MAX_GROUPS] = { -1.5f, -0.5f, 0.5f, 0.0f };

sceSamp0FVECTOR color0[MAX_GROUPS]  = { 0.8f, 0.3f, 0.3f, 1.0f };
sceSamp0FVECTOR color1[MAX_GROUPS]  = { 0.3f, 0.8f, 0.3f, 1.0f };
sceSamp0FVECTOR color2[MAX_GROUPS]  = { 0.3f, 0.3f, 0.8f, 1.0f };

sceSamp0FVECTOR ambient[MAX_GROUPS] = { 0.2f, 0.2f, 0.2f, 0.0f };

sceSamp0FVECTOR obj_trans[MAX_GROUPS] = { 0.0f, 0.0f, 0.0f, 0.0f };
sceSamp0FVECTOR obj_rot[MAX_GROUPS] = { 0.0f, 0.0f, 0.0f, 0.0f };

sceSamp0FMATRIX local_world;
sceSamp0FMATRIX world_view;
sceSamp0FMATRIX view_screen;	//[MAX_GROUPS];
sceSamp0FMATRIX local_screen[MAX_GROUPS];

sceSamp0FMATRIX normal_light[MAX_GROUPS];
sceSamp0FMATRIX light_color[MAX_GROUPS];
sceSamp0FMATRIX local_light[MAX_GROUPS];
sceSamp0FMATRIX local_color[MAX_GROUPS];

sceSamp0FMATRIX work;
sceSamp0IVECTOR v01;	//,c01;

float z2=00.0f;
float delta = 1.0f * PI / 180.0f;

float InitCameraP[4]={0.0f,0.0f,-25,0.0f};
float InitCameraZD[4]={0.0f,0.0f,1.0f,1.0f};
float InitCameraYD[4]={0.0f,1.0f,0.0f,1.0f};
float InitLight0[4]={0.0f,1.5f,0.5f,0.0f};
float InitLight1[4]={1.5f,-0.5f,0.5f,0.0f};
float InitLight2[4]={-1.5f,-0.5f,0.5f,0.0f};
float InitColor0[4]={ 0.8f, 0.3f, 0.3f, 1.0f };
float InitColor1[4]={ 0.3f, 0.8f, 0.3f, 1.0f };
float InitColor2[4]={ 0.3f, 0.3f, 0.8f, 1.0f };
float InitAmbient[4]={ 0.2f, 0.2f, 0.2f, 0.0f };

void GRAP_InitGroups(void)
{
int i,j;

	for (i=0;i<MAX_GROUPS;i++)
	{
		for (j=0;j<4;j++)
		{
			camera_p[i][j]=InitCameraP[j];
			camera_zd[i][j]=InitCameraZD[j];
			camera_yd[i][j]=InitCameraYD[j];
			camera_rot[i][j]=0.0f;
			light0[i][j]=InitLight0[j];
			light1[i][j]=InitLight1[j];
			light2[i][j]=InitLight2[j];
			color0[i][j]=InitColor0[j];
			color1[i][j]=InitColor1[j];
			color2[i][j]=InitColor2[j];
			ambient[i][j]=InitAmbient[j];
			obj_trans[i][j]=0.0f;
			obj_rot[i][j]=0.0f;
		}
		GroupList[i].Status=0;
		GroupList[i].x=-320;
		GroupList[i].y=-256;
		GroupList[i].ox=0;
		GroupList[i].oy=0;
	}
}
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

#if 0
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
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(100,100,100, 0x0, 0x3f800000));


 			// set upper-left position of sprite
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(OFFX+(100<<4),OFFY+(100<<4),1));

 			// set lower-right position of sprite
 			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(OFFX+(150<<4),OFFY+(150<<4),1));

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
#endif

#if 1
void SpritePacket(int ls,int count)
{

sceGifPacket gif_pkt;
int nloop = count;
int num_regs = 8+2;
int cnt0;

static char *buffer=NULL;
char *pbuffer;
int block,width,width_log2,height_log2,texflag;
int x1,y1,x2,y2,x3,y3,x4,y4,z1,z2,z3,z4;
int t1,t2,t3,t4;
int s1,s2,s3,s4;
int jc,jf,jc2;
float delta = 1.0f * PI / 180.0f;
float Q1,Q2,Q3,Q4;
u_int n1,n2,n3,n4;
int zc=0;

int TransLevel=0x80;	//0x60;

	if (buffer==NULL)
	{
		buffer=(char*)memalign(64,4*1024*1024);

//sceSamp0FMATRIX view_screen[MAX_GROUPS];
//void sceSamp0ViewScreenMatrix(sceSamp0FMATRIX m, float scrz, float ax, float ay,

    sceSamp0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 0.47f,
			     2048.0f, 2048.0f, 1.0f, 16777215.0f,
			     1.0f,65536.0f);

	}
	pbuffer=(char*)((int)buffer|UNCACHED_MEM);

	// set base address of GIF packet 
	sceGifPkInit(&gif_pkt,(u_long128*)pbuffer);	//SPR_MEM);
	sceGifPkReset(&gif_pkt);


/*
TO MAKE TRANSPARANCY OPTIONAL:

1) TO TURN IT OFF..REMOVE THIS BIT..
    sceGifPkAddGsData(&gif_pkt,SCE_GS_SET_TEXA(0,1,128));	//0x8000));
	sceGifPkAddGsData( &gif_pkt, SCE_GS_TEXA );

2) AND SET THIS TO 2 INSTEAD OF 3..
	sceGifPkAddGsData(&gif_pkt, SCE_GIF_SET_TAG(3,0,NULL,NULL,SCE_GIF_PACKED,1));

ADD NEW FUNCTION TO SET TRANS LEVEL:

IT WILL MODIFY THIS, WHERE 0X20 RANGES FROM 0-0X80 (0 = TOTALLY TRANSPARRRENT):
		sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g, 
		sprite[jc2].b, 0x20, 0x3f800000));
*/

//printf("-----------------\n");
		  
	jc=0;
	for( cnt0=ls; cnt0<(ls+(count*2)); cnt0++)
	{
		jf=(cnt0-ls)&1;	// flag
		jc2=jc/2;

		texflag=0;
		if (sprite[jc2].texture!=0)
		{
			texflag=1;
			block=sprite[jc2].texture-1;
			width=TextureList[block].Width;
			width_log2=TextureList[block].WidthLog2;
			height_log2=TextureList[block].HeightLog2;

			// set GIF tag
			sceGifPkAddGsData(&gif_pkt, SCE_GIF_SET_TAG(3,0,NULL,NULL,SCE_GIF_PACKED,1));
			sceGifPkAddGsData(&gif_pkt, 0xeL);	// e= Address + data..
		    sceGifPkAddGsData(&gif_pkt,SCE_GS_SET_TEX0((IMAGE_ADDR+(block*32))*GRAPH_LOC,
						     width/64, SCE_GS_PSMCT32,width_log2,height_log2,1,0,0,0,0,0,0));
			sceGifPkAddGsData( &gif_pkt, SCE_GS_TEX0_1 );

		    sceGifPkAddGsData(&gif_pkt,SCE_GS_SET_TEST(1,6,0,0,0,0,1,3));	//0x8000));
			sceGifPkAddGsData( &gif_pkt, SCE_GS_TEST_1 );

		    sceGifPkAddGsData(&gif_pkt,SCE_GS_SET_ALPHA(0,1,0,1,0x80));
			sceGifPkAddGsData( &gif_pkt, SCE_GS_ALPHA_1 );

//			if (TextureList[block].Status==0)
//				printf("Texture 0x%x for poly is NOT available!\n",sprite[jc2].texture);
		}
		// set PRIM info

//printf("num_regs = %x\n",num_regs);
		sceGifPkAddGsData(&gif_pkt, SCE_GIF_SET_TAG(1,1,NULL,NULL,SCE_GIF_REGLIST,num_regs));
		sceGifPkAddGsData(&gif_pkt, 0x5125125120L ); 

		sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRI,0,texflag,0,1,0,0,0,0));

		// set RGBA of sprite

		if (jf==0)
		{

//		if (sprite[jc2].group==3)
//			zc=-100;
//		else
//			zc=0;

//			zc=sprite[jc2].z;

			GRAP_CalculateLocalScreen(sprite[jc2].group);
			Q1=GRAP_SetScreenLocation(sprite[jc2].x1,sprite[jc2].y1,sprite[jc2].z1,sprite[jc2].group);
			x1=v01[0];
			y1=v01[1];
			z1=v01[2];
			s1=0;
			t1=0;
			Q2=GRAP_SetScreenLocation(sprite[jc2].x2,sprite[jc2].y2,sprite[jc2].z2,sprite[jc2].group);
			x2=v01[0];
			y2=v01[1];
			z2=v01[2];
			s2=GRAP_GetST(Q2,1.0f);
			t2=0;
			Q3=GRAP_SetScreenLocation(sprite[jc2].x3,sprite[jc2].y3,sprite[jc2].z3,sprite[jc2].group);
			x3=v01[0];
			y3=v01[1];
			z3=v01[2];
			s3=0;
			t3=GRAP_GetST(Q3,1.0f);
			Q4=GRAP_SetScreenLocation(sprite[jc2].x4,sprite[jc2].y4,sprite[jc2].z4,sprite[jc2].group);
			x4=v01[0];
			y4=v01[1];
			z4=v01[2];
			s4=GRAP_GetST(Q4,1.0f);
//if (sprite[jc2].group==3+7)
//printf("Q=%f (%x)\n",Q1,s4);
			t4=s4;

#if 1

//if (sprite[jc2].group==3+7)
//printf("%f \n",s2);

//s1=0;
//t1=0;
//s2=0x3f800000;
//t2=0;
//s3=0;
//t3=0x3f800000;
//s4=0x3f800000;
//t4=0x3f800000;

//Q1*=0x3f800000;
//Q2*=0x3f800000;
//Q3*=0x3f800000;
//Q4*=0x3f800000;

n1=(u_int)GRAP_GetST(Q1,1.0f);
n2=(u_int)GRAP_GetST(Q2,1.0f);
n3=(u_int)GRAP_GetST(Q3,1.0f);
n4=(u_int)GRAP_GetST(Q4,1.0f);
//if (sprite[jc2].group==3+7)
//	printf("%f %d\n",Q1,n1);
#endif

			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s1,t1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x1,y1,z1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s3,t3));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n3));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x3,y3,z3));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s4,t4));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n4));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x4,y4,z4));
		}
		else
		{
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s1,t1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x1,y1,z1));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s2,t2));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n2));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x2,y2,z2));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_ST(s4,t4));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_RGBAQ(sprite[jc2].r, sprite[jc2].g,sprite[jc2].b,
								TransLevel, n4));
			sceGifPkAddGsData(&gif_pkt, SCE_GS_SET_XYZ(x4,y4,z4));
		}
		jc++;
	}

	// set packet size in quad words
	sceGifPkTerminate(&gif_pkt);

//printf("size = %d\n",sceGifPkSize(&gif_pkt));

	// DMA send (Normal Mode) to GS via path 3		
	sceDmaSendN(jdmaGIF,(u_long128 *)((u_int)gif_pkt.pBase&DMA_MEM), (int)sceGifPkSize(&gif_pkt));
	sceGsSyncPath(0, 0);
	GRAP_EndPlot();
}

#endif


void SetSprite2(int x,int y,int w,int h,int r,int g,int b,int texture,int group)
{
	SetSpriteZ(x,y,w,h,0,r,g,b,texture,group);
}

void SetSprite(int x,int y,int w,int h,int r,int g,int b)
{
	SetSpriteZ(x,y,w,h,0,r,g,b,0,0);
}


void SetPoly(int x1,int y1,int z1,int x2,int y2,int z2,int x3,int y3,int z3,int x4,int y4,int z4,
			int r,int g,int b,int texture,int group)
{
float w2,h2;

//x-=320;
//y-=112;

	x1*=2;
	x2*=2;
	x3*=2;
	x4*=2;

	w2=640.0f/640.0f;
	x1*=w2;
	x2*=w2;
	x3*=w2;
	x4*=w2;
	h2=232.0f/100.0f;
	y1*=h2;
	y2*=h2;
	y3*=h2;
	y4*=h2;

	if (texture!=0)
	{
		r/=2;
		g/=2;
		b/=2;
	}

	sprite[SpriteCounter].group=group;
	sprite[SpriteCounter].texture = texture;
	sprite[SpriteCounter].x = x1;
	sprite[SpriteCounter].y = y1;
	sprite[SpriteCounter].z = z1;

	sprite[SpriteCounter].x1 = x1;
	sprite[SpriteCounter].x2 = x2;
	sprite[SpriteCounter].x3 = x3;
	sprite[SpriteCounter].x4 = x4;
	sprite[SpriteCounter].y1 = y1;
	sprite[SpriteCounter].y2 = y2;
	sprite[SpriteCounter].y3 = y3;
	sprite[SpriteCounter].y4 = y4;
	sprite[SpriteCounter].z1 = z1;
	sprite[SpriteCounter].z2 = z2;
	sprite[SpriteCounter].z3 = z3;
	sprite[SpriteCounter].z4 = z4;
	sprite[SpriteCounter].r = r;
	sprite[SpriteCounter].g = g;
	sprite[SpriteCounter].b = b;
	SpriteCounter++;

}


void SetSpriteZ(int x,int y,int w,int h,int z,int r,int g,int b,int texture,int group)
{
	SetPoly(x,y,z,
			x+w,y,z,
			x,y+h,z,
			x+w,y+h,z,
			r,g,b,
			texture,
			group);
}

void GRAP_CreateCube(int x,int y,int z,int size,int texture, int group)
{
int size2,size3;
int r,g,b;

	r=255;
	g=0;
	b=0;

x*=2;
y*=2;
z*=4;
	size3=size*2;
	size2=size/2;
	x-=size2;
	y-=size2;
	z-=size2;

// back face
	SetPoly(x,y,z,
			x+size,y,z,
			x,y+size,z,
			x+size,y+size,z,
			r,g,b,
			texture,
			group);

r/=2;

// front face
	SetPoly(x,y,z+size3,
			x+size,y,z+size3,
			x,y+size,z+size3,
			x+size,y+size,z+size3,
			r,g,b,
			texture,
			group);
r/=2;

// left
	SetPoly(x,y,z,
			x,y,z+size3,
			x,y+size,z,
			x,y+size,z+size3,
			r,g,b,
			texture,
			group);
r/=2;
// right
	SetPoly(x+size,y,z,
			x+size,y,z+size3,
			x+size,y+size,z,
			x+size,y+size,z+size3,
			r,g,b,
			texture,
			group);
r/=2;

// top
	SetPoly(x,y,z,
			x+size,y,z,
			x,y,z+size3,
			x+size,y,z+size3,
			r,g,b,
			texture,
			group);

r/=2;
//bottom
	SetPoly(x,y+size,z,
			x+size,y+size,z,
			x,y+size,z+size3,
			x+size,y+size,z+size3,
			255,255,255,
			texture,
			group);

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
int i;
int sindex;
sceVif1Packet packet[2];
sceDmaEnv env;
sceDmaChan *p1;
u_long giftagAD[2] = { SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1),0x000000000000000eL };

	for (i=0;i<TEXTURE_MAX;i++)
	{
		TextureList[i].Status=0;
	}
	GRAP_InitGroups();

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



#if 0
static sceGifPacket g_PkGif;				// GIF packet handler
static sceDmaChan *g_DmaGif;				// DMA GIF channel

static void FlushTextureCache(void)
{
	const u_long giftagAD[2] = {
		SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1),		// A+D GIF tag

		0x000000000000000EL
	};

	// GIF packet initialization

	sceGifPkReset(&g_PkGif);

	// Texture flush

	sceGifPkEnd(&g_PkGif, 0, 0, 0);
	sceGifPkOpenGifTag(&g_PkGif, *(u_long128*)&giftagAD);	// Set GIF tag

	sceGifPkAddGsData(&g_PkGif, 0x00000000);		// D
	sceGifPkAddGsData(&g_PkGif, SCE_GS_TEXFLUSH);	// A(TEXFLUSH)
	sceGifPkCloseGifTag(&g_PkGif);					// Close GIF tag


	// Close GIF packet

	sceGifPkTerminate(&g_PkGif);


	// Wait for transmission of GS initialization packet and termination

//	BUG_WALKAROUND(&g_PkGif);
	FlushCache(WRITEBACK_DCACHE);				// Flush D cache

	sceDmaSend(g_DmaGif, ADR_DMA_PERIPH(g_PkGif.pBase));
	sceDmaSync(g_DmaGif, 0, 0);
	return;
}
#endif

int GRAP_SendTIM2(int block,char *filename)
{
int h;
TIM2_PICTUREHEADER *ph;
u_int size;
u_int addr;
u_int b;

	if (block==0)
	{
		printf("INVALID BLOCK: block 0 not allowed\n");
		return -1;
	}
	block--;
	b=(u_int)block*32;

	// Read TIM2 file from target

	if (TextureList[block].Status==0)
		printf("CREATING Texture for file: %s\n",filename);
	else
		printf("OVERWRITING Texture for file: %s\n",filename);
	h  = sceOpen(filename, SCE_RDONLY);
	if (h<0)
	{
		printf("file not found: %s\n",filename);
		return(-1);
	}

	size=(u_int)sceLseek(h,0,SCE_SEEK_END);
    addr=(u_int)memalign(64,size);				// Allocate buffer for loaded data
	sceLseek(h,0,SCE_SEEK_SET);
	sceRead(h, (char*)addr, (int)size);
	sceClose(h);

	if(!Tim2CheckFileHeaer((void*)addr))
	{
		printf("tim2 header failed\n");
		free((void*)addr);	// Free EE buffer
		return(-3);
	}
	else
	{
		// Obtain pixel header
		ph = Tim2GetPictureHeader((void*)addr, 0);
		if(!ph)
		{
			printf("picture header failed\n");
			free((void*)addr);	// Free EE buffer
			return(-2);
			// Acquisition of picture header failed
		}
		else
		{
			// Read picture data into VRAM

//printf("image transfer to 0x%x\n",(IMAGE_ADDR+b)*GRAPH_LOC);
			Tim2LoadPicture(ph, (IMAGE_ADDR+b)*GRAPH_LOC,0);
			TextureList[block].Width=ph->ImageWidth;
			TextureList[block].Height=ph->ImageHeight;
			TextureList[block].WidthLog2=GRAP_GetLog2(ph->ImageWidth);
			TextureList[block].HeightLog2=GRAP_GetLog2(ph->ImageHeight);
			TextureList[block].Status=1;
		}
	}
//printf("all loaded OK\n");
	free((void*)addr);	// Free EE buffer

	return(block);
}

static int GRAP_GetLog2(u_int n)
{
int i;
	for(i=31; i>0; i--) {
		if(n & (1<<i)) {
			break;
		}
	}
	if(n>(1<<i)) {
		i++;
	}
	return(i);
}


void GRAP_EndPlot(void)
{
int i;
	for (i=0;i<64;i++)
	{
		GroupList[i].Status=0;
	}

}


void GRAP_CalculateLocalScreen(int group)
{
//sceSamp0FVECTOR temptrans;

	if (GroupList[group].Status==1)
	{
//		printf("done\n");
		return;
	}
	GroupList[group].Status=1;

// Create Matrix
	sceSamp0UnitMatrix(work);

	sceSamp0RotMatrix(local_world, work, obj_rot[group]);
	sceSamp0TransMatrix(local_world, local_world, obj_trans[group]);

// world -> view matrix
	sceSamp0RotCameraMatrix(world_view,camera_p[group], camera_zd[group], camera_yd[group], camera_rot[group]);

// local -> screen matrix
	sceSamp0MulMatrix(work, world_view, local_world);
	sceSamp0MulMatrix(local_screen[group], view_screen, work);
}

void GRAP_SetGroupXY(int group,int x,int y,int z)
{
	GroupList[group].x=(x*2)-320;
	GroupList[group].y=y-256;
	GroupList[group].z=z;
}

void GRAP_SetGroupXYOffset(int group,int x,int y)
{
	GroupList[group].ox=x;
	GroupList[group].oy=y;
}

float GRAP_SetScreenLocation(int x,int y,int z,int group)
{
sceSamp0FVECTOR vertex;
float Q;

	obj_trans[group][0]=GroupList[group].x;
	obj_trans[group][1]=GroupList[group].y;
	obj_trans[group][2]=GroupList[group].z+512;	//512;

	vertex[0]=x+=GroupList[group].ox;
	vertex[1]=y+GroupList[group].oy;
	vertex[2]=z;
	vertex[3]=1;

//float sceSamp0RotTransPers(sceSamp0IVECTOR v0, sceSamp0FMATRIX m0, sceSamp0FVECTOR v1, int mode)

	Q = sceSamp0RotTransPers(v01, local_screen[group], vertex, 0);

	return(Q);
}

int GRAP_GetST(float Q,float v)
{
union {int i;float f;} colin;

	colin.f = v*Q;
	return(colin.i);
}

float printMatrix[32][4][4];
void GRAP_CopyToPrintMatrix(sceSamp0FMATRIX m,int n)
{
int i,j;
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			printMatrix[n][i][j]=m[i][j];
		}
	}
}


void GRAP_RotateX(int g, int v)
{
float delta = 1.0f * PI / 180.0f;
	delta*=v;
	obj_rot[g][2] = delta;
}

void GRAP_RotateY(int g, int v)
{
float delta = 1.0f * PI / 180.0f;
	delta*=v;
	obj_rot[g][1] = delta;
}



#if 0

if (group==1)
printf("obj_trans = %f,%f,%f,%f\n",obj_trans[group][0],obj_trans[group][1],obj_trans[group][2],obj_trans[group][3]);
if (group==1)
{
printf("1-------\n");
for (i=0;i<4;i++)
{
	printf("%f,%f,%f,%f\n",local_world[group][i][0],local_world[group][i][1],local_world[group][i][2],local_world[group][i][3]);
}
}

#endif



/* mathematical functions */
// ===========================================================================
// 行列計算の関数群(libvu0より)
// 
// 行列の配列形式は以下のようになっています. (libvu0とは転置)
// 
//     |v[0][0] v[0][1] v[0][2] v[0][3]|
//  m =|v[1][0] v[1][1] v[1][2] v[1][3]|
//     |v[2][0] v[2][1] v[2][2] v[2][3]|
//     |v[3][0] v[3][1] v[3][2] v[3][3]|
// 


// ==========================================================================
//  sceSamp0ApplyMatrix		ベクトルにマトリックスを乗算する。
//  形式
//  	void sceSamp0ApplyMatrix(sceSamp0FVECTOR v0, sceSamp0FMATRIX m0, sceSamp0FVECTOR v1)
//  引数
//  	v1		入力:ベクトル
//  	m0		入力:マトリックス
//  	v0		出力:ベクトル
// 
//  解説
//  	マトリックスmにベクトルv0を右から乗算して、v1に与える
// 
//  		v0=m0*v1
//  返り値
//  	なし
// --------------------------------------------------------------------------
void sceSamp0ApplyMatrix(sceSamp0FVECTOR v0, sceSamp0FMATRIX m0,
			 sceSamp0FVECTOR v1)
{
  sceSamp0FVECTOR v;
  v[0] = v1[0]*m0[0][0] + v1[1]*m0[0][1] + v1[2]*m0[0][2] + v1[3]*m0[0][3];
  v[1] = v1[0]*m0[1][0] + v1[1]*m0[1][1] + v1[2]*m0[1][2] + v1[3]*m0[1][3];
  v[2] = v1[0]*m0[2][0] + v1[1]*m0[2][1] + v1[2]*m0[2][2] + v1[3]*m0[2][3];
  v[3] = v1[0]*m0[3][0] + v1[1]*m0[3][1] + v1[2]*m0[3][2] + v1[3]*m0[3][3];
  sceSamp0CopyVector(v0, v);
}

// ==========================================================================
// 
//  sceSamp0MulMatrix		２つマトリックスの積を求める
//  形式
//  	void sceSamp0MulMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FMATRIX m2)
//  引数
//  	m1,m2		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	マトリックスm1にマトリックスm2を右から乗算して、m0に与える
// 
//  		m0=m1*m2
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0MulMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1,
		       sceSamp0FMATRIX m2)
{

  int i, j;
  sceSamp0FMATRIX m;
   for(i=0; i<4; i++){
     for(j=0; j<4; j++){
       m[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j]
	 + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
     }
   }
   sceSamp0CopyMatrix(m0, m);

}

// ==========================================================================
// 
//  sceSamp0OuterProduct		２つのベクトルの外積を求める
// 形式
// 	void sceSamp0OuterProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
//  引数
//  	v1,v2		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1,v2の外積を求めてv0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0OuterProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1,
			  sceSamp0FVECTOR v2)
{
  v0[0] = v1[1] * v2[2] - v1[2] * v2[1];
  v0[1] = v1[2] * v2[0] - v1[0] * v2[2];
  v0[2] = v1[0] * v2[1] - v1[1] * v2[0];
  v0[3] = 0;
}

// ==========================================================================
// 
//  sceSamp0InnerProduct		２つのベクトルの内積を求める
//  形式
//  	float sceSamp0InnerProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1)
//  引数
//  	v0,v1		入力:ベクトル
//  解説
//  	ベクトルv0,v1の内積を求める
//  返り値
//  	内積
// 
// --------------------------------------------------------------------------
float sceSamp0InnerProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1)
{
  return (v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2]);
}

// ==========================================================================
//  sceSamp0Normalize		ベクトルの正規化を行う
//  形式
//  	void sceSamp0Normalize(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1)
//  引数
//  	v1		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv0の正規化を行いv1に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0Normalize(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1)
{
  float norm, dnorm;
  norm = sqrtf( v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2] );
  if(norm > 0){
    dnorm = 1.0f/norm;
    v0[0] = v1[0]*dnorm;
    v0[1] = v1[1]*dnorm;
    v0[2] = v1[2]*dnorm;
    v0[3] = v1[3];
  }
}

// ==========================================================================
// 
//  sceSamp0TransposeMatrix	マトリックスの転置行列を求める
//  形式
//  	void sceSamp0TransposeMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1)
//  引数
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	マトリックスm1の転置行列を求めm0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0TransposeMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1)
{
  int i,j;
  sceSamp0FMATRIX t;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      t[i][j] = m1[j][i];
    }
  }
  sceSamp0CopyMatrix(m0, t);
}

// ==========================================================================
// sceSamp0InversMatrix	逆行列 (4x4) を求める。（行列は正則と仮定する）
//  形式
// 	void sceSamp0InversMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1);
//  引数
//  	m1		入力:マトリックス(回転 & 平行移動)
//  	m0		出力:マトリックス
//  解説
//  	マトリックスm1の逆行列を求めて、マトリックスm0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0InversMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1)
{
  int i,j;
  sceSamp0FMATRIX m;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m[i][j] = m1[j][i];
    }
  }
  m[0][3] = -(m1[0][3]*m[0][0] + m1[1][3]*m[0][1] + m1[2][3]*m[0][2]);
  m[1][3] = -(m1[0][3]*m[1][0] + m1[1][3]*m[1][1] + m1[2][3]*m[1][2]);
  m[2][3] = -(m1[0][3]*m[2][0] + m1[1][3]*m[2][1] + m1[2][3]*m[2][2]);
  m[3][0] = m[3][1] = m[3][2] = 0.0f;
  m[3][3] = 1;
  sceSamp0CopyMatrix(m0, m);
}

// ==========================================================================
// sceSamp0AddVector	4並列加算 (ADD/xyzw)
//  形式
// 	void sceSamp0AddVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
//  引数
//  	v1,v2		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1の各要素とベクトルv2の各要素を各々加算する。
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0AddVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
{
  v0[0] = v1[0] + v2[0];
  v0[1] = v1[1] + v2[1];
  v0[2] = v1[2] + v2[2];
  v0[3] = v1[3] + v2[3];
}

// ==========================================================================
// sceSamp0SubVector		4並列減算 (SUB/xyzw)
//  形式
// 	void sceSamp0SubVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
//  引数
//  	v1,v2		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1の各要素とベクトルv2の各要素を各々減算する。
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0SubVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
{
  v0[0] = v1[0] - v2[0];
  v0[1] = v1[1] - v2[1];
  v0[2] = v1[2] - v2[2];
  v0[3] = v1[3] - v2[3];
}

// ==========================================================================
// sceSamp0MulVector	4並列積算（MUL/xyzw: 内積ではないことに注意）
//  形式
// 	void sceSamp0MulVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
//  引数
//  	v1,v2		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1の各要素とベクトルv2の各要素を各々乗算する。
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0MulVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2)
{
  v0[0] = v1[0] * v2[0];
  v0[1] = v1[1] * v2[1];
  v0[2] = v1[2] * v2[2];
  v0[3] = v1[3] * v2[3];
}

// ==========================================================================
// sceSamp0ScaleVector	スカラー値を乗算する (MULx/xyzw)
//  形式
// 	void sceSamp0ScaleVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float t)
//  引数
//  	v1		入力:ベクトル
//  	t		入力:スカラー
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1にスカラーtを乗算してベクトルv0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0ScaleVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float t)
{
  v0[0] = v1[0]*t;
  v0[1] = v1[1]*t;
  v0[2] = v1[2]*t;
  v0[3] = v1[3]*t;
}

// ==========================================================================
// sceSamp0TransMatrix	行列の平行移動
//  形式
// 	void sceSamp0TransMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR tv);
//  引数
//  	tv		入力:移動ベクトル
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	与えられたベクトル分、マトリックスを平行移動する
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0TransMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR tv)
{
int i,j;

  for(i=0; i<4; i++)
  {
    for(j=0; j<4; j++)
    {
      m0[i][j] = m1[i][j];
    }
  }

  m0[0][3] = m1[0][3] + tv[0];
  m0[1][3] = m1[1][3] + tv[1];
  m0[2][3] = m1[2][3] + tv[2];
  m0[3][3] = m1[3][3] + tv[3];
}

// ==========================================================================
// sceSamp0CopyVector	  ベクトルの複写 (lq と sq の組合せ。alignment check 用に)
//  形式
// 	void sceSamp0CopyVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1);
//  引数
//  	v1		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	ベクトルv1をベクトルv0にコピーする。
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0CopyVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1)
{
  v0[0] = v1[0];
  v0[1] = v1[1];
  v0[2] = v1[2];
  v0[3] = v1[3];
}


// ==========================================================================
// sceSamp0CopyMatrix	     行列の複写 (lq と sq の組合せ。alignment check 用に)
//  形式
// 	void sceSamp0CopyMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1);
//  引数
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	マトリックスm1をマトリックスm0にコピーする。
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0CopyMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1)
{
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m0[i][j] = m1[i][j];
    }
  }
}

// ==========================================================================
// sceSamp0FTOI0Vector 	FTOI0
//  形式
// 	void sceSamp0FTOI0Vector(sceSamp0IVECTOR v0, sceSamp0FVECTOR v1);
//  引数
//  	v1		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	与えられたベクトルの要素を整数に変換する
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0FTOI0Vector(sceSamp0IVECTOR v0, sceSamp0FVECTOR v1)
{
  v0[0] = (int)v1[0];
  v0[1] = (int)v1[1];
  v0[2] = (int)v1[2];
  v0[3] = (int)v1[3];
}

// ==========================================================================
// sceSamp0ITOF0Vector 	ITOF0
//  形式
// 	void sceSamp0ITOF0Vector(sceSamp0FVECTOR v0, sceSamp0IVECTOR v1);
//  引数
//  	v1		入力:ベクトル
//  	v0		出力:ベクトル
//  解説
//  	与えられたベクトルの要素を整数に変換する
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0ITOF0Vector(sceSamp0FVECTOR v0, sceSamp0IVECTOR v1)
{
  v0[0] = (float)v1[0];
  v0[1] = (float)v1[1];
  v0[2] = (float)v1[2];
  v0[3] = (float)v1[3];

}

// ==========================================================================
// sceSamp0UnitMatrix	単位行列
//  形式
// 	void sceSamp0UnitMatrix(sceSamp0FMATRIX m0);
//  引数
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	与えられた行列を単位行列に変換する
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0UnitMatrix(sceSamp0FMATRIX m0)
{
  int i,j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      m0[i][j] = (i==j)? 1.0f : 0.0f;
    }
  }
}

// ==========================================================================
// sceSamp0RotMatrixZ	Ｚ軸を中心とした行列の回転
//  形式
// 	void sceSamp0RotMatrixZ(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float rz);
//  引数
//  	rz		入力:回転角(有効範囲:-π ～ π)
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	回転角rtよりＸ軸を中心とした回転マトリックスを求めて、
// 	マトリックスm1に左側から乗算して、その結果をマトリックス
// 	m0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0RotMatrixZ(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float rz)
{
  sceSamp0FMATRIX rot;
  sceSamp0UnitMatrix(rot);
  /*  rz = rz * (3.141592/180.0);*/
  rot[0][0] = cosf(rz); rot[0][1] = -sinf(rz);
  rot[1][0] = sinf(rz); rot[1][1] = cosf(rz);
  sceSamp0MulMatrix(m0, rot, m1);
}

// ==========================================================================
// sceSamp0RotMatrixX	Ｘ軸を中心とした行列の回転
//  形式
// 	void sceSamp0RotMatrixX(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float ry);
//  引数
//  	rx		入力:回転角(有効範囲:-π ～ π)
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	回転角rtよりＸ軸を中心とした回転マトリックスを求めて、
// 	マトリックスm1に左側から乗算して、その結果をマトリックス
// 	m0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0RotMatrixX(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float rx)
{
  sceSamp0FMATRIX rot;
  sceSamp0UnitMatrix(rot);
  /*  rx = rx * (3.141592/180.0);*/
  rot[1][1] = cosf(rx); rot[1][2] = -sinf(rx);
  rot[2][1] = sinf(rx); rot[2][2] = cosf(rx);
  sceSamp0MulMatrix(m0, rot, m1);
}

// ==========================================================================
// sceSamp0RotMatrixY	Ｙ軸を中心とした行列の回転
//  形式
// 	void sceSamp0RotMatrixY(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float ry);
//  引数
//  	ry		入力:回転角(有効範囲:-π ～ π)
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	回転角rtよりＹ軸を中心とした回転マトリックスを求めて、
// 	マトリックスm1に左側から乗算して、その結果をマトリックス
// 	m0に与える
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0RotMatrixY(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float ry)
{
  sceSamp0FMATRIX rot;
  sceSamp0UnitMatrix(rot);
  /*  ry = ry * (3.141592/180.0);*/
  rot[0][0] = cosf(ry); rot[2][0] = sinf(ry);
  rot[0][2] = -sinf(ry); rot[2][2] = cosf(ry);
  sceSamp0MulMatrix(m0, rot, m1);
}

// ==========================================================================
// sceSamp0RotMatrix	X,Y,Z軸を中心とした行列の回転
//  形式
// 	void sceSamp0RotMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR rot);
//  引数
//  	rot		入力:回転角ベクトル(有効範囲:-π ～ π)
//  	m1		入力:マトリックス
//  	m0		出力:マトリックス
//  解説
//  	回転角ベクトルrotよりX,Y,Z軸それぞれを中心とした回転マトリックス
//       を求め、マトリックスm1に左側から乗算して、その結果をマトリックス
// 	m0に与える．Z ,Y, X 軸の順に回転する．
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0RotMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR rot)
{
  sceSamp0RotMatrixZ(m0, m1, rot[2]);
  sceSamp0RotMatrixY(m0, m0, rot[1]);
  sceSamp0RotMatrixX(m0, m0, rot[0]);
}

// ==========================================================================
// sceSamp0ClampVector	ベクトルのクランプ
//  形式
// 	void sceSamp0ClampVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float min, float max)
//  引数
//  	v0		出力:ベクトル
//  	v1		入力:ベクトル
//  	min		入力:浮動小数(切捨て下限)
//  	max		入力:浮動小数(切捨て上限)
//  解説
//  	ベクトルv1の各要素を minを下限 maxを上限としてクランプし
//       v0に与える．
//  返り値
//  	なし
// 
// --------------------------------------------------------------------------
void sceSamp0ClampVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float min, float max)
{
  sceSamp0FVECTOR vect;
  /*
  v0[0] = v1[0]<min ? min : (v1[0]>max ? max : v1[0]);
  v0[1] = v1[1]<min ? min : (v1[1]>max ? max : v1[1]);
  v0[2] = v1[2]<min ? min : (v1[2]>max ? max : v1[2]);
//  v0[2] = v1[2]<min ? min : (v1[2]>max ? max : v1[2]);
  v0[3] = v1[3]<min ? min : (v1[3]>max ? max : v1[3]);
  */
  vect[0] = v1[0]<min ? min : (v1[0]>max ? max : v1[0]);
  vect[1] = v1[1]<min ? min : (v1[1]>max ? max : v1[1]);
  vect[2] = v1[2]<min ? min : (v1[2]>max ? max : v1[2]);
  vect[3] = v1[3]<min ? min : (v1[3]>max ? max : v1[3]);
  sceSamp0CopyVector(v0,vect);
}


// ==========================================================================
// sceSamp0CameraMatrix    ワールド座標系に対するカメラ座標系を定義
// 
// 	p:  camera position (in the world)
// 	zd: eye direction
// 	yd: vertical direction
// 	xd = zd x yd:  wing direction
// 	yd = yd x xd:  real veritical direction
// 
// 	        | xd0 yd0 zd0 p0 |
// 	M = inv | xd1 yd1 zd1 p1 |
// 	        | xd2 yd2 zd2 p2 |
// 	        |   0   0   0  1 |
void sceSamp0CameraMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR p, sceSamp0FVECTOR zd, sceSamp0FVECTOR yd)
{
	sceSamp0FMATRIX	m0;
	sceSamp0FVECTOR	xd;

	sceSamp0UnitMatrix(m0);
	sceSamp0OuterProduct(xd, yd, zd);
	sceSamp0Normalize(m0[0], xd);
	sceSamp0Normalize(m0[2], zd);
	sceSamp0OuterProduct(m0[1], m0[2], m0[0]);
	sceSamp0TransposeMatrix(m0, m0);
	sceSamp0TransMatrix(m0, m0, p);
	sceSamp0InversMatrix(m, m0);
}


// ==========================================================================
// sceSamp0RotCameraMatrix    ワールド座標系に対するカメラ座標系を定義(回転有)
// 
// 	p:  original camera position (in the world)
// 	zd: original eye direction
// 	yd: original vertical direction
//     rot: rotation vector
// 	direction : real eye direction
// 	vertical  : real veritical direction
//     position  : real camera position
// 
void sceSamp0RotCameraMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR p, sceSamp0FVECTOR zd, sceSamp0FVECTOR yd, sceSamp0FVECTOR rot)
{
	sceSamp0FMATRIX	work;
	sceSamp0FVECTOR direction, vertical, position;

	sceSamp0UnitMatrix(work);
	sceSamp0RotMatrixX(work,work,rot[0]);
	sceSamp0RotMatrixY(work,work,rot[1]);
	sceSamp0RotMatrixZ(work,work,rot[2]);
	sceSamp0ApplyMatrix(direction, work, zd);
	sceSamp0ApplyMatrix(vertical, work, yd);
	sceSamp0ApplyMatrix(position, work, p);
	sceSamp0CameraMatrix(m, position, direction, vertical);

}

// ==========================================================================
// sceSamp0NormalLightMatrix    並行光源(３つ)から方向ベクトル行列を生成
// 
//  make normal light matrix
// 	l0-l2:	light direction
// 	    | l0x l0y l0z 0 |
// 	M = | l1x l1y l1z 0 |
// 	    | l2x l1y l2z 0 |
// 	    |   0   0   0 1 |

void sceSamp0NormalLightMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR l0, sceSamp0FVECTOR l1, sceSamp0FVECTOR l2)
{
        sceSamp0FVECTOR  t;
        sceSamp0ScaleVector(t, l0, -1); sceSamp0Normalize(m[0], t);
        sceSamp0ScaleVector(t, l1, -1); sceSamp0Normalize(m[1], t);
        sceSamp0ScaleVector(t, l2, -1); sceSamp0Normalize(m[2], t);

	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

// =========================================================================
// sceSamp0LighrColorMatrix   並行光源(３つ)のカラー行列を生成
// 
// make normal light matrix
// 	c0-c2:	light color
// 	a:	ambient color
// 
// 	    | c0r c1r c2r ar |
// 	M = | c0g c1g c2g ag |
// 	    | c0b c1b c2b ab |
// 	    |   0   0   0  0 |
// 
void sceSamp0LightColorMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR c0, sceSamp0FVECTOR c1, sceSamp0FVECTOR c2, sceSamp0FVECTOR a)
{
	sceSamp0CopyVector(m[0], c0);
	sceSamp0CopyVector(m[1], c1);
	sceSamp0CopyVector(m[2], c2);
	sceSamp0CopyVector(m[3],  a);
	sceSamp0TransposeMatrix(m, m);
}

// =========================================================================
//  sceSamp0NormalColorVector     各頂点のライティング後の色を計算
// 
//     local_light:  light direction(matrix)
//     light_color:  light color(matrix)
//     v0:   vertex normal vector
//     c0:   vertex color
// 
//     vect = local_light * v0;   (inner product)
//     0.0 < vect < 1.0;          (clamp)
//     c0   = local_color * vect; (inner product)
// 
void sceSamp0NormalColorVector(sceSamp0IVECTOR c0,
		sceSamp0FMATRIX local_light, sceSamp0FMATRIX light_color,
			       sceSamp0FVECTOR v0, sceSamp0FVECTOR c1){
  sceSamp0FVECTOR vect;
  sceSamp0ApplyMatrix(vect, local_light, v0);
  sceSamp0ClampVector(vect, vect, 0.0f, 1.0f);
  sceSamp0ApplyMatrix(vect, light_color, vect);
  sceSamp0MulVector(vect, c1, vect);
  sceSamp0ClampVector(vect, vect, 0.0f, 255.0f);
  sceSamp0FTOI0Vector(c0, vect);
}

// ========================================================================
// sceSamp0ViewScreenMatrix      視点座標系からスクリーン座標系への変換行列
// 
// 	scrz:		distance to screen
// 	ax, ay:		aspect ratio
// 	cx, cy:		center
// 	zmin, zmax:	Z-buffer range
// 	nearz, farz:	near, far edge
// 
void sceSamp0ViewScreenMatrix(sceSamp0FMATRIX m, float scrz, float ax, float ay,
	       float cx, float cy, float zmin, float zmax, float nearz, float farz)
{
	float	az, cz;
	sceSamp0FMATRIX	mt;

	cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
	az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

	//     | scrz    0  0 0 |
	// m = |    0 scrz  0 0 |
	//     |    0    0  0 1 |
	//     |    0    0  1 0 |
	sceSamp0UnitMatrix(m);
	m[0][0] = scrz;
	m[1][1] = scrz;
	m[2][2] = 0;
	m[3][3] = 0;
	m[3][2] = 1.0f;
	m[2][3] = 1.0f;

	//     | ax  0  0 cx |
	// m = |  0 ay  0 cy |
	//     |  0  0 az cz |
	//     |  0  0  0  1 |
	sceSamp0UnitMatrix(mt);
	mt[0][0] = ax;	mt[1][1] = ay;	mt[2][2] = az;
	mt[0][3] = cx;	mt[1][3] = cy;	mt[2][3] = cz;
	sceSamp0MulMatrix(m, mt, m);
	return;
}


// ==========================================================================
// sceSamp0RotTransPers	透視変換
//  形式
// 	void sceSamp0RotTransPers(sceSamp0IVECTOR v0, sceSamp0FMATRIX m0, sceSamp0FVECTOR v1);
//  引数
//  	v0		出力:ベクトル(スクリーン座標)
//  	m0		入力:マトリックス
//  	v1		入力:ベクトル
//  	mode		入力:v0[2],v0[3]の値の切り替え
//  			     0:32bit符号なし固定小数点(28.4)
//  			     1:32bit符号なし固定小数点(32.0)
//  解説
//  	与えられた頂点をスクリーン座標に透視変換する。
// 	出力v0の値は、v0[0],v0[1]は、32bit符号付き固定小数点(28.4)、
// 	v0[2],v0[3]はmode=0のときは、32bit符号なし固定小数点(28.4)、
// 	mode=1のときは、32bit符号なし固定小数点(32.0)を返す。
// 	GIFのPACKEDモードでXYZF2,XYZF3を用いる場合には、mode=0にす
// 	ると、PACK時に整数部が切り出されるので有用である。
// 
//  返り値
//  	透視変換後のベクトルの第４要素(W値)の逆数(1/W)を返す．
//     テクスチャ座標 STQのQ値に用いるため．
// 
// --------------------------------------------------------------------------
float sceSamp0RotTransPers(sceSamp0IVECTOR v0, sceSamp0FMATRIX m0, sceSamp0FVECTOR v1, int mode)
{

float divw;
sceSamp0FVECTOR retv;

  sceSamp0ApplyMatrix(retv, m0, v1);
  divw = 1.0f / retv[3];
  retv[0] = retv[0]*divw;
  retv[1] = retv[1]*divw;
  retv[2] = retv[2]*divw;
  /*    retv[3] = retv[3]*divw;   */
  if(mode==0){
    v0[0] = (int)(retv[0]*16.0);
    v0[1] = (int)(retv[1]*16.0);
    v0[2] = (int)(retv[2]*16.0);
    v0[3] = (int)(retv[3]*16.0);
  }else{
    v0[0] = (int)(retv[0]*16.0);
    v0[1] = (int)(retv[1]*16.0);
    v0[2] = (int)(retv[2]);
    v0[3] = (int)(retv[3]);
  }
  return divw;
}


// output vector contents
void sceSamp0DumpVector(char *s, sceSamp0FVECTOR v)
{
	printf(s);
	printf("\t[%8d,%8d,%8d,%8d]\n\r",
	       (int)v[0], (int)v[1], (int)v[2], (int)v[3]);
}

// output matrix contents
void sceSamp0DumpMatrix(char *s, sceSamp0FMATRIX m)
{
	puts(s);
	printf("\t[%8d,%8d,%8d,%8d]\n\r",
	       (int)(m[0][0]*10), (int)(m[0][1]*10),
	       (int)(m[0][2]*10), (int)(m[0][3]*10));
	printf("\t[%8d,%8d,%8d,%8d]\n\r",
	       (int)(m[1][0]*10), (int)(m[1][1]*10),
	       (int)(m[1][2]*10), (int)(m[1][3]*10));
	printf("\t[%8d,%8d,%8d,%8d]\n\r",
	       (int)(m[2][0]*10), (int)(m[2][1]*10),
	       (int)(m[2][2]*10), (int)(m[2][3]*10));
	printf("\t[%8d,%8d,%8d,%8d]\n\r",
	       (int)(m[3][0]*10), (int)(m[3][1]*10),
	       (int)(m[3][2]*10), (int)(m[3][3]*10));
}

