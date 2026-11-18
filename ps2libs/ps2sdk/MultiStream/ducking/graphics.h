#ifndef gfx
#define gfx


typedef int		sceSamp0IVECTOR[4] __attribute__((aligned (16)));
typedef float		sceSamp0FVECTOR[4] __attribute__((aligned (16)));
typedef float		sceSamp0FMATRIX[4][4] __attribute__((aligned (16)));

// prototypes
void  sceSamp0DumpVector(char *s, sceSamp0FVECTOR v);
void  sceSamp0DumpMatrix(char *s, sceSamp0FMATRIX m);
void  sceSamp0CopyVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1);
void  sceSamp0FTOI0Vector(sceSamp0IVECTOR v0, sceSamp0FVECTOR v1);
void  sceSamp0ITOF0Vector(sceSamp0FVECTOR v0, sceSamp0IVECTOR v1);
void  sceSamp0ScaleVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float s);
void  sceSamp0AddVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2);
void  sceSamp0SubVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2);
void  sceSamp0MulVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2);
float sceSamp0InnerProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1);
void  sceSamp0OuterProduct(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, sceSamp0FVECTOR v2);
void sceSamp0Normalize(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1);
void  sceSamp0ApplyMatrix(sceSamp0FVECTOR v0, sceSamp0FMATRIX m, sceSamp0FVECTOR v1);
void  sceSamp0UnitMatrix(sceSamp0FMATRIX m);
void  sceSamp0CopyMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1);
void  sceSamp0TransposeMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1);
void  sceSamp0MulMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FMATRIX m2);
void  sceSamp0InversMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1);
void  sceSamp0RotMatrixX(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float rx);
void  sceSamp0RotMatrixY(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float ry);
void  sceSamp0RotMatrixZ(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, float rz);
void  sceSamp0RotMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR rot);
void  sceSamp0TransMatrix(sceSamp0FMATRIX m0, sceSamp0FMATRIX m1, sceSamp0FVECTOR tv);
void  sceSamp0CameraMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR p, sceSamp0FVECTOR zd, sceSamp0FVECTOR yd);
void  sceSamp0RotCameraMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR p, sceSamp0FVECTOR zd, sceSamp0FVECTOR yd, sceSamp0FVECTOR rot);
void  sceSamp0NormalLightMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR l0, sceSamp0FVECTOR l1, sceSamp0FVECTOR l2);
void sceSamp0NormalColorVector(sceSamp0IVECTOR c0, sceSamp0FMATRIX local_light, sceSamp0FMATRIX light_color,
				sceSamp0FVECTOR v0, sceSamp0FVECTOR c1);
void  sceSamp0LightColorMatrix(sceSamp0FMATRIX m, sceSamp0FVECTOR c0,
						sceSamp0FVECTOR c1, sceSamp0FVECTOR c2, sceSamp0FVECTOR a);
void  sceSamp0ViewScreenMatrix(sceSamp0FMATRIX m, float scrz, float ax, float ay,
	float cx, float cy, float zmin, float zmax, float nearz, float farz);
void sceSamp0ClampVector(sceSamp0FVECTOR v0, sceSamp0FVECTOR v1, float min, float max);
float sceSamp0RotTransPers(sceSamp0IVECTOR v0, sceSamp0FMATRIX m0, sceSamp0FVECTOR v1, int mode);



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

#define IMAGE_ADDR 300 // each page is 32 blocks..512 pages in total...crap, isn't it?!
#define IMAGE_WIDTH 64
#define IMAGE_TW 6		// 64 LOG 2
#define IMAGE_TH 6

extern sceGsDBuff db;          	// double buffer info
extern sceDmaChan *jdmaGIF;	// DMA channel for string
extern int console;

extern float printMatrix[32][4][4];

void Gs_Initialize(void);
void InitSystem( void );
void JInitGraphics( void );
void SetAlphaBlend( void );

void SetSprite2(int x,int y,int w,int h,int r,int g,int b,int texture,int group);
void SetSprite(int x,int y,int w,int h,int r,int g,int b);
void SetSpriteZ(int x,int y,int w,int h,int z,int r,int g,int b,int texture,int group);
void SetBox(int x,int y,int x1,int y1,int r,int g,int b,int texture);
void SpritePacket(int ls,int count);
int DrawSprites(void);
void DisplayScreen(int frame);
int GRAP_SendTIM2(int block,char *filename);
static int GRAP_GetLog2(u_int n);

void GRAP_CalculateLocalScreen(int group);
float GRAP_SetScreenLocation(int x,int y,int z,int group);
void GRAP_EndPlot(void);
void GRAP_RotateX(int g, int v);
void GRAP_RotateY(int g, int v);
void GRAP_CopyToPrintMatrix(sceSamp0FMATRIX m,int n);
void GRAP_SetGroupXY(int group,int x,int y,int z);
void GRAP_SetGroupXYOffset(int group,int x,int y);

int GRAP_GetST(float Q,float v);
void GRAP_CreateCube(int x,int y,int z,int size,int texture, int group);

//static sceGsDBuff db;


#endif







