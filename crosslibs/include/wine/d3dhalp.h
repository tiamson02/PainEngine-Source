#ifndef WINE_D3DHALP_H_INCLUDED
#define WINE_D3DHALP_H_INCLUDED

#include "windef.h"
#include "ddrawi.h"
#include "d3dtypes.h"

struct _D3DHAL_CALLBACKS;
struct _D3DHAL_CALLBACKS2;
struct _VertexShader;
struct _PixelShader;

typedef struct {
    D3DLIGHT7 l;
    DWORD flags, devnum;
    D3DVALUE devpos[4];
} D3D_Light;

typedef struct
{
    DDRAWI_DDRAWSURFACE_LCL lcl;
    DDRAWI_DDRAWSURFACE_MORE more;
    LPDDRAWI_DDRAWSURFACE_GBL_MORE gmore;
    DDRAWI_DDRAWSURFACE_GBL gbl;
    DDRAWI_DDRAWSURFACE_GBL_MORE gbl_more;
    DWORD tdata[17];
} D3D_T,*LPD3D_T;

typedef struct
{
    LPDDRAWI_DIRECTDRAW_LCL lcl;
    LPDDRAWI_DDRAWSURFACE_LCL surf, zbuf;
    ULONG_PTR dwhContext;
    LPDDHAL_GETDRIVERINFO gdi;
    struct _D3DHAL_CALLBACKS *cbs1;
    struct _D3DHAL_CALLBACKS2 *cbs2;
    HANDLE surface_heap;
    BOOL in_scene;
    DWORD renderstate[256];
    DWORD texstagestate[8][33];
    D3DMATRIX xfrm[8], txfrm[8];
    D3DVIEWPORT7 viewport;
    D3DMATERIAL7 material;
    DWORD numlights;
    D3D_Light *light;
    D3DVALUE clipplane[32][4];
    struct _VertexShader* vertex_shader;
    struct _PixelShader* pixel_shader;
    DWORD vertex_fvf;
    float* vertex_const;
    float* pixel_const;
    struct _PixelShader* pixel_shader_cache;
    /* PLEASE do not add new fields to D3D_P if you can avoid it.
     * I've been working on phasing it out for a while. */
} D3D_P,*LPD3D_P;

/*----------------------------------------------------------------------------------
 * Private VertexShader structure and macros that are shared between D3D8 and D3DGL
 * - can be modified as necessary
 *----------------------------------------------------------------------------------*/

/* to distinguish a handle from a FVF, we'll use D3DFVF_RESERVED0,
 * assuming that HeapAlloc-ed memory is always DWORD-aligned so
 * that the lower bit of the returned pointer is always zero */
#define VS_TO_HANDLE(x) (((DWORD)x) | D3DFVF_RESERVED0)
#define VS_FROM_HANDLE(x) ((VertexShader*)(x & ~D3DFVF_RESERVED0))
#define IS_VS(x) (x & D3DFVF_RESERVED0)
#define IS_FVF(x) (!IS_VS(x))

#define SET_INPUT_REG_TYPE(x) ((x) | (1<<7))
#define GET_INPUT_REG_TYPE(x) ((x) & ~(1<<7))
#define IS_INPUT_REG_ENABLED(x) ((x) & (1<<7))

typedef struct _VertexShaderCacheState {
  DWORD clipspace_fix; 
  DWORD pos_offset;
  DWORD ypos_inverted;
  DWORD colorfix;               /* bitfield for color swizzle enable */
  DWORD tex_scale;              /* bitfield for tex coord scaling enable */
  DWORD tex_offset;             /* bitfield for tex coord offsets enable */
} VertexShaderCacheState;

typedef struct _VertexShaderCache {
  VertexShaderCacheState state;
  char *arb_program;
  DWORD DriverReserved0;      /* used by d3dgl to store driver specific info (name) */
  DWORD DriverReserved1;      /* (context) */
  DWORD pos_offset_size[2];   /* cached viewport size for pos fragment offset */
  DWORD tex_offset_size[8][2];/* cached texture sizes for tex fragment offset */
  DWORD tex_scale_size[8][2]; /* cached texture sizes for fragment_offset */
  struct _VertexShaderCache *next;
} VertexShaderCache;

typedef struct _VertexDeclaration {
  LPVOID pDeclaration;  	/* app given declaration, dependant on dx version */
  DWORD DeclarationLength;	/* length of declaration in bytes */
  DWORD FVF;
  BOOL is_fvf_compatible;	/* are all of the elements properly represented in FVF? */
  BOOL is_direct;
} VertexDeclaration;

typedef struct _VertexShader {
  VertexDeclaration vdecl;   /* dx8 vertex decl */
  CONST DWORD* pFunction;
  DWORD FunctionLength;
  DWORD Usage;
  void *shaderprogram;       /* parsed structure */
  struct _VertexShader *prev;
  struct _VertexShader *next;
  VertexShaderCache *cache;
  VertexShaderCache *current; 	
  DWORD colorfix;             /* bit field indicating which input_regs had the colorfix applied */
  BYTE  has_fog;
  BYTE  has_pointsize;
  BYTE  is_software;
  BYTE  fixed_mode;
  struct {
    BYTE stream;   /* stream source number */
    BYTE offset;   /* offset into stream in BYTES */
    BYTE type;     /* D3DVSDT_* type of data, with high bit used a flag (see macros above) */
    int gltype;    /* gl type that the D3DVSDT_* type maps to */
    int size;	   /* the number of data elements in field of type */
    int normalize; /* should fixed point data be normalized when converting to floating point? */
  } input_reg[17];
  float constant_override[96*4];
  DWORD dirty_vsc[3];
} VertexShader;

typedef struct _PixelShaderCacheState {
  DWORD texture_type[8];
  DWORD texture_projected[8];
  DWORD fog_type;
} PixelShaderCacheState;

typedef struct _PixelShaderCache {
  char *programstring;
  struct _PixelShaderCacheState state;
  DWORD DriverReserved0;      /* used by d3dgl to store driver specific info (name) */
  DWORD DriverReserved1;      /* (context) */
  struct _PixelShaderCache *next;
} PixelShaderCache;

typedef struct _PixelShader {
  CONST DWORD* pFunction;
  DWORD FunctionLength;
  char *programstring;
  void *ExtensionSpecific;
  float constant_override[8*4];    /* constants declared and set in original shader */
  DWORD dirty_psc[1];
  DWORD used_texture[1];     /* bitfield: textures used */
  struct _PixelShader *prev;
  struct _PixelShader *next;
  struct _PixelShaderCache *cache;
  struct _PixelShaderCache *current;
} PixelShader;


#endif
