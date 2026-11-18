// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _D3DEV_
#define _D3DEV_


#include "Engine.h"
#include "Render/Viewport.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Core/nommgr.h"
#include <d3dx9.h>
#include "Core/mmgr.h"
#pragma warning(disable: 4201) // non-standard extension used: nameless union/struct

#define _DXMAT(x)			((D3DMATRIX*)(const FLOAT*)(x))
#define SAFE_RELEASE(x)		if((x)) { (x)->Release(); (x)=NULL; }
inline DWORD F2DW(FLOAT f)	{ return *((DWORD*)&f); }

static void DumpCaps(D3DCAPS9& Caps);

struct D3DShader
{
	int		Type;
    D3DVERTEXELEMENT9	Decl[MAXD3DDECLLENGTH];
    DynamicArray<DWORD>	Source;

	union
	{
		LPDIRECT3DVERTEXSHADER9	VSh;
		LPDIRECT3DPIXELSHADER9	PSh;
		void* Ptr;
	};
	union 
	{
		LPDIRECT3DVERTEXDECLARATION9	VDecl;
		DWORD							FVF;
	};

	D3DShader() : Type(EInvalidIndex), Ptr(NULL), FVF(0)
	{
	}

	static bool CmpDecls(const D3DVERTEXELEMENT9& VElem1, const D3DVERTEXELEMENT9& VElem2)
	{
		return memcmp( &VElem1, &VElem2, sizeof( VElem1 ) ) == 0;
	}

	static bool DeclEnd(const D3DVERTEXELEMENT9& VElem1)
	{
		static D3DVERTEXELEMENT9 declEnd = D3DDECL_END();
		return CmpDecls(VElem1, declEnd);
	}
};

struct D3DBuffer
{
    enum { 
		EUnusedStream	= 0, 
		EIndexStream	= (1<<0),	// this is an index stream 
		EVertexStream	= (1<<1),	// this is a vertex stream

		EUsageSWVP		= (1<<31),	// this stream is used for software vertex processing
	};

    DWORD   Type;       // unused,index,vertex
    int     ElemCount;
    int     Stride;
    void*   SourceData; // NULL for dynamic buffers, points to real data for static ones
                        // to allow for automatic restore and copy to AGP/video mem

    // device stuff
	DWORD FVF;	// FVF for vertex buffer, unused for index buffer

    union
    {
        LPDIRECT3DVERTEXBUFFER9     VBuf;
        LPDIRECT3DINDEXBUFFER9      IBuf;
		void*						Ptr;
    };

    //D3DBuffer() : Type(0), VBuf(0), IBuf(0) {}
	D3DBuffer() : Type(EUnusedStream), Ptr(0) {}
};

struct D3DTexture
{
	Texture*				TexDef;
	union
	{
		LPDIRECT3DTEXTURE9			pTex;
		LPDIRECT3DCUBETEXTURE9		pCube;
		LPDIRECT3DVOLUMETEXTURE9	pVol;
		LPDIRECT3DSURFACE9			pSurf;
		void*						Ptr;
	};

	D3DTexture() : TexDef(0), Ptr(0) {}
	D3DTexture(Texture* texDef, LPDIRECT3DTEXTURE9 ptex)
	:	TexDef(texDef), pTex(ptex)
	{}
	D3DTexture(Texture* texDef, LPDIRECT3DCUBETEXTURE9 pcube)
	:	TexDef(texDef), pCube(pcube)
	{}
	D3DTexture(Texture* texDef, LPDIRECT3DVOLUMETEXTURE9 pvol)
	:	TexDef(texDef), pVol(pvol)
	{}
	D3DTexture(Texture* texDef, LPDIRECT3DSURFACE9 psurf)
	:	TexDef(texDef), pSurf(psurf)
	{}
};

struct VertexStreamState
{
	LPDIRECT3DVERTEXBUFFER9	VBuf;
	DWORD					Offset;

	VertexStreamState() : VBuf(NULL), Offset(0) {}
};

template <class T> BOOL D3DStaticBufferFill(T* pBuf, const void* src, const int size, const int offset=0)
{
    VOID* ptr=NULL;
    HRESULT hr=pBuf->Lock(0,0,&ptr,D3DLOCK_NOSYSLOCK);
    if(SUCCEEDED(hr) && ptr) {
        ::memcpy(ptr,src,size);
        pBuf->Unlock();
        return TRUE;
    }            

    return FALSE;
}

template <class T> BYTE* D3DDynamicBufferLock(T* pBuf, const int offset, const int size)
{
    VOID* ptr=NULL;
    HRESULT hr=pBuf->Lock
    (
        offset, size, 
        &ptr,
        offset==0   ? D3DLOCK_NOSYSLOCK|D3DLOCK_DISCARD
                    : D3DLOCK_NOSYSLOCK|D3DLOCK_NOOVERWRITE
    );
    return SUCCEEDED(hr) ? (BYTE*)ptr : NULL;
}

class D3DRenderDevice : public RenderDevice
{
public:
    D3DRenderDevice();
    ~D3DRenderDevice();

    // RenderDevice interface
	BOOL Open(HWND fullScreenHWND);
	void Close();
    
    BOOL SetMode(UINT width, UINT height, BOOL fullScreen, int multisampling);
    BOOL SetGamma(float contrast, float gamma, float brightness);
    BOOL Lock(Viewport& viewport);
	void Unlock();
	void Flip(HWND hwnd);
	void Flush();
	void TakeScreenshot(const char* filename);
	BOOL SetViewport(int x, int y, int w, int h, float zNear, float zFar);
	BOOL SetScissor(int x1, int y1, int x2, int y2);

    BOOL ClearZ(float z);
    BOOL ClearFB(DWORD argb);

	int		CreateQuery(int Type);
	BOOL	BeginQuery(int Query);
	void	EndQuery(int Query);
	BOOL	GetQueryData(int Query, void* OutData, int Size);
	void	CreateTexture(Texture* tex);
    void    SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace);
	BOOL	SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace = 0);
	BOOL	SetDepthTarget(Texture* DepthBuf);
    void    SetFBRenderTarget();
	BOOL	GrabBackBuffer(Texture* Tex, HWND hwnd);
	BOOL	GrabZStencilBuffer(Texture* Tex);
	void	GetBackBufferSize(UINT& SizeX, UINT& SizeY);
	BYTE*	LockTexture(Texture* tex, int &pitch);
	void	UnlockTexture(Texture* tex);
	void	SetTexturePriority(Texture *tex, DWORD priority);
	void	PreLoadTexture(Texture *tex);
	int 	CreateStream(void* sourceData, DWORD type, int count, int stride);
	BYTE* 	LockStream  (int streamId, int pos, int count);
	void 	UnlockStream(int streamId);
	int		CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList);
	int     CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount);
	void	SetShaderMemory(DWORD addr, const void* data, int count);
	void	SetPixelShaderMemory(DWORD addr, const void* data, int count);
	void	SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count);
	void	SetShader(int shaderIndex, int shaderType);
    void    SetState(const RenderState& state);
    void    SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset=0);
    void    SetTextureState(const TextureState* stages, int stageCount);
	void	ResetTextureStages();
    void    SetFogState(const FogState& state);
    void    SetFog(bool isFog);
    void    SetLight(int number, const LightState& state);
	void	SetObjectTransform(const Matrix* matPtr);
	void	SetCameraTransform(const Matrix* matPtr);
	void	SetScreenTransform(const Matrix* matPtr);

	void 	Draw(const int primType, int base, int count, int vbase=EInvalidIndex, int vcount=EInvalidIndex, int basevindex=0);
	void	ReleaseResource(int resourceType, const int* ids, int count);

	RenderEffect* CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size);

	void SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite);
	void SetCullFlip(BOOL Flip);

#ifdef TRANSGAMING
	void	SetSpecular(const Vector& color, const float power);
	void	SetLighting(bool isLighting);
	void	SetAmbientLight(const Vector& color);
	void	SetNoLight(int number);
	void	SetDirLight(int number, const Plane& dir, const Vector& color);
#endif



    // INTERNAL
private:
    void RestoreDevice();
    void ReleaseDevice();

    void ReleaseShaders();
    void RestoreShaders();

	void GetOriginalGamma();
	BOOL RestoreOriginalGamma();

	int  SurfFormatToBPP(const D3DFORMAT &Desc) const;
	void SetSWVP(BOOL enable);
	void SetFVF(DWORD FVF);
	void SetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 VDecl);

	// texture creation utils
	void CreateRenderTarget(Texture* texture);
	void CreateDepthSurface(Texture* texture);
	void CreateRegularTexture(Texture* texture);
	void CreateDynamicTexture(Texture* texture);
	void CreateCubeMapTexture(Texture* texture);
	void CreateVolumeTexture(Texture* texture);
	void CreateRegularTextureFromFile(Texture* texture);
	void CreateDuDvTextureFromFile(Texture* texture);
	void CreateCubeMapTextureFromFile(Texture* texture);
	void CreateRenderTargetCubeMapTextureFromFile(Texture* texture);
	void CreateVolumeTextureFromFile(Texture* texture);
	void CreateNormCubeMap(Texture* texture);

private:

    IDirect3D9*             d3d;
    IDirect3DDevice9*       d3dev;
    D3DCAPS9                d3dCaps;
    D3DPRESENT_PARAMETERS   d3dpp;

	UINT		AdapterOrdinal;

    // textures
    DynamicArray<D3DTexture>	Textures;
	int                         TexBytes;
	int							FrameTris;
	int							FrameDIPs;

	// created buffers
	DynamicArray<D3DBuffer>			    Buffers;
	DynamicArray<D3DShader>				Shaders;
    DynamicArray<int>					ResTextures;
	DynamicArray<IDirect3DQuery9*>		Queries;

	DWORD				SavedFogColor;

    RenderState         CachedState;
    TextureState        CachedTextureStates[8];
    TextureStageState   CachedStageState;
	VertexStreamState	CachedStreams[16];

	Plane				CachedVSConst[96];
	Plane				CachedPSConst[8];

	BOOL				CachedSWVP:1;
	DWORD				CachedFVF;
    int     			CurrentStreamSize;
    BOOL    			CurrentStreamIsIndexed;
	BOOL				StencilSurfaceTouched;
	BOOL				CullFlip;

    LPDIRECT3DVERTEXSHADER9			CurrentVertexShader;
    LPDIRECT3DPIXELSHADER9			CurrentPixelShader;
	LPDIRECT3DVERTEXDECLARATION9	CurrentVDecl;


    BOOL    IsFullScreen;
    HWND    FullScreenHWND;
    UINT    FullScreenWidth;
    UINT    FullScreenHeight;

	UINT	ViewportWidth;
	UINT	ViewportHeight;

    D3DDISPLAYMODE  DesktopMode;
    HWND            DesktopHWND;

	D3DGAMMARAMP		OrigianalGammaRamp;

    IDirect3DSurface9*  OriginalBackBuffer;
    IDirect3DSurface9*  OriginalDepthBuffer;
	Viewport			OriginalViewport;

    IDirect3DSurface9*  CurrentBackBuffer;
    IDirect3DSurface9*  CurrentDepthBuffer;

public:
	static LogBuffer D3DLog;
};

#endif //_D3DEV_

