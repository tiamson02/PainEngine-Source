#ifndef gfx
#define gfx


#define SCRN_W  (640)
#define SCRN_H  (224)
#define SCRN_CENTER_X (2048)
#define SCRN_CENTER_Y (2048)
#define OFFX (((4096-SCRN_W)/2)<<4)
#define OFFY (((4096-SCRN_H)/2)<<4)
#define SPR_MEM (0x70000000)
#define UNCACHED_MEM (0x20000000)
#define DMA_SPR (0x80000000)
#define DMA_MEM (0x0FFFFFFF)
#define NUM_SPRITES (4095)


#define SUNIT		0x01
#define PACKETSIZE	(0x100*SUNIT)
#define WORKSIZE	(0x80)
#define WORKBASE	0x70000000
#define SUPPER		(WORKBASE+WORKSIZE)
#define SLOWER		(WORKBASE+WORKSIZE+PACKETSIZE)
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	224
#define CNTL_RUN  0
#define CNTL_STOP 1

extern sceGsDBuff db;          	// double buffer info
extern sceDmaChan *jdmaGIF;	// DMA channel for string
extern int console;

void Gs_Initialize(void);
void InitSystem( void );
void JInitGraphics( void );
void SetAlphaBlend( void );
void SetSprite(int x,int y,int w,int h,int r,int g,int b);
void SpritePacket(int ls,int count);
int DrawSprites(void);
void DisplayScreen(int frame);

//static sceGsDBuff db;


#endif
