#ifndef WINE_D3DHALGL_H_INCLUDED
#define WINE_D3DHALGL_H_INCLUDED

#include "windef.h"
#include "ddrawi.h"
#include "d3dtypes.h"

#define COMPUTE_FOG_COORDINATE

#if DIRECT3D_VERSION < 0x0900
/* have to keep FVF parsing d3d9 compatible */
#undef D3DFVF_POSITION_MASK
#define D3DFVF_POSITION_MASK 0x400E
#define D3DFVF_XYZW          0x4002
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000
#endif

#define D3DFVF_GLCOLORORDER 0x2000 /* colours reordered to RGBA */
#define D3DFVF_XYZGLW 0x4004 /* RHW converted to W */

typedef struct
{
    LPVOID lpvData;
    DWORD dwStride;
#ifdef _D3DGL_PRIVATE_H_INCLUDED
    LPD3D_T vbSource;
#else
    LPDDRAWI_DDRAWSURFACE_LCL vbSource;
#endif
} GL_D3D_PTRSTRIDE;

typedef struct
{
    GL_D3D_PTRSTRIDE position;
    GL_D3D_PTRSTRIDE normal;
    GL_D3D_PTRSTRIDE color[2];
    GL_D3D_PTRSTRIDE textureCoords[D3DDP_MAXTEXCOORD];

    GL_D3D_PTRSTRIDE blend_weights;
    GL_D3D_PTRSTRIDE matrix_indices;
    GL_D3D_PTRSTRIDE fog_coordinate;
} GLDRAWPRIMITIVESTRIDEDDATA, *LPGLDRAWPRIMITIVESTRIDEDDATA;

struct GL_D3D_DriverCallbacks
{
    DWORD dwSize;
    DWORD (*D3dVbGetConvertedFVF)(LPDDRAWI_DDRAWSURFACE_LCL vb);
    LPVOID (*D3dVbPreRender)(DWORD ctx, LPDDRAWI_DDRAWSURFACE_LCL vb,
			     LPGLDRAWPRIMITIVESTRIDEDDATA data,
			     BOOL convert,
			     DWORD vstart, DWORD vcount,
			     const WORD* indices, DWORD nindices,
			     DWORD indexbase, DWORD stride, DWORD streamoffset);
    void (*D3dVbPostRender)(LPDDRAWI_DDRAWSURFACE_LCL vb);
    BOOL (*D3dRenderLock)(ULONG_PTR dwhContext, BOOL draw, LPBOOL invert);
    void (*D3dRenderUnlock)(ULONG_PTR dwhContext, BOOL draw);
    void (*D3dRenderArea)(ULONG_PTR dwhContext, LPRECT rect, BOOL clear);
    void (*D3dEnableFVFConversion)(BOOL convert);
    void (*D3dEnableShaderAGP)(BOOL alloc);
    BOOL (*D3dHaveAGP)(DWORD ctx);
    BOOL (*D3dHaveVBO)(DWORD ctx);
    LPVOID (*D3dAllocAGP)(DWORD ctx, DWORD size, DWORD flags);
    void (*D3dFreeAGP)(DWORD ctx, LPVOID data);
    void (*D3dFlushAGP)(DWORD ctx, LPVOID data, DWORD len);
    void (*D3dEnableRenderAGP)(DWORD ctx);
    void (*D3dDisableRenderAGP)(DWORD ctx);
};

DEFINE_GUID( GUID_WineGLD3DCallbacks, 0xC1E3F5C6, 0xEC82, 0x4B53, 0x94, 0xCA,
	     0xAE, 0x4F, 0x08, 0xE0, 0x46, 0x7C );

#endif
