// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _VIDEOSYSTEM_PSP_
#define _VIDEOSYSTEM_PSP_


#include "Engine.h"
#include "Render/Viewport.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Render/Material.h"
//#pragma warning(disable: 4201) // non-standard extension used: nameless union/struct

#include <psptypes.h>
#include <kernel.h>

#define USE_GIM

#define _PSPMAT(x)			((const ScePspFMatrix4Unaligned*)(const FLOAT*)(x))

#define LISTS 1
#define LISTSIZE 0x20000

//#define VRAM_LIST

typedef void* VBLK;

struct PSPShader
{
    int	Type;

    // device stuff
    int Format;
    int Stride;
    bool Clip;
    bool MT;

    PSPShader() : Type(EInvalidIndex)
    {
        Format = 0;
        Stride = 0;
        Clip = false;
        MT = false;
    }
};

struct PSPBuffer
{
    enum { 
        EUnusedStream	= 0, 
        EIndexStream	= (1<<0),	// this is an index stream 
        EVertexStream	= (1<<1),	// this is a vertex stream
    };

    DWORD   Type;       // unused,index,vertex
    int     ElemCount;
    int     Stride;
    void*   SourceData; // NULL for dynamic buffers, points to real data for static ones
                        // to allow for automatic restore and copy to AGP/video mem

    // device stuff
    int Format;
    void* Ptr;

    //PSPBuffer() : Type(0), Format(0), Ptr(0) {}
    PSPBuffer() : Type(EUnusedStream) {}
};

#ifdef USE_GIM
struct SceGimPicture;
#endif

struct PSPTexture
{
    //Texture* TexDef;
    char    Comp;
    //int     Format;
    int     Size;
    //int     LockSize;
    VBLK    Target;
#ifdef USE_GIM
    struct  SceGimPicture *Pic;
#endif

    PSPTexture()
    : /*TexDef(0), Format(0), */Comp(0), Size(0),
      /*LockSize(0), */Target(0)
#ifdef USE_GIM
      , Pic(NULL)
#endif
    {}
    PSPTexture(const PSPTexture &tex){
        (*this) = tex;
    }
    PSPTexture(Texture* texDef, int format, int comp, int size, int locksize)
    : /*TexDef(texDef), Format(format), */Comp(comp), Size(size),
      /*LockSize(locksize), */Target(0)
#ifdef USE_GIM
      , Pic(NULL)
#endif
    {}

    void SetParams(Texture *texDef, int format, int comp, int size, int locksize);

    PSPTexture &operator = (const PSPTexture &tex){
        //TexDef = tex.TexDef;
        Comp = tex.Comp;
        //Format = tex.Format;
        Size = tex.Size;
        //LockSize = tex.LockSize;
        Target = tex.Target;
        Pic = tex.Pic;

        return *this;
    }
};

struct VertexStreamState
{
    const PSPBuffer* Buf;
    DWORD Offset;

    VertexStreamState() : Buf(NULL), Offset(0) {}
};

typedef DynamicArray<ClipVertex> ClipVertexArray;

class VideoSystemPSP : public RenderDevice
{
public:
    VideoSystemPSP();
    ~VideoSystemPSP();

    // RenderDevice interface
    BOOL Open(unsigned int);
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
    BOOL ClearFB(DWORD abgr);

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
#ifndef TRANSGAMING
    // this function is obselete even in the windows version
    int		CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList);
#endif
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

    void    SetBoneMatrix(int index, const Matrix *matrix);

    void    SetSpecular(const Vector& color, const float power);
    void    SetLighting(bool isLighting);
    void    SetAmbientLight(const Vector& color);
    void    SetNoLight(int number);
    void    SetDirLight(int number, const Plane& dir, const Vector& color);

    void	SetObjectTransform(const Matrix* matPtr);
    void	SetCameraTransform(const Matrix* matPtr);
    void	SetScreenTransform(const Matrix* matPtr);

    void 	Draw(const int primType, int base, int count, int vbase=EInvalidIndex, int vcount=EInvalidIndex, int basevindex=0, bool swapPath = false);
    void	ReleaseResource(int resourceType, const int* ids, int count);

    RenderEffect* CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size);

    void SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite);
    void SetCullFlip(BOOL Flip);
    void SetVBlankState(bool state);
    bool GetVBlankState() const;

    void SetConstColor(DWORD color) { }

    // INTERNAL
private:
    void RestoreDevice();
    void ReleaseDevice();

    void ReleaseShaders();
    void RestoreShaders();

    void GetOriginalGamma();
    BOOL RestoreOriginalGamma();

    void RecalcFog();
    void RecalcMat();
    void RecalcClip();
    void AllocList();
    void FreeList();
    void StartList();
    void FinishList(bool sync=false);
    void SyncList();
    void CheckList();
    void SignaledList(int id);
    void FinishedList(int id);
    static void signal_cb(int id);
    static void finish_cb(int id);
    bool ActivateStage(int stage);
    int GetStream(int index, int base, VertexStreamState* streams, BYTE*& ptr);
    /*void DrawArray(int prim, int fmt, int base, int count, void* idx, int streamc, VertexStreamState* streams);
    void DrawArrayClipI(int prim, int fmt, int base, int count, void* idx, int streamc, VertexStreamState* streams);
    void DrawArrayClipNI(int prim, int fmt, int base, int count, int streamc, VertexStreamState* streams);//*/
    void DrawArray(int prim, int base, int count, void* idx, int streamc, VertexStreamState* streams);
    void DrawArrayClipI(int prim, int base, int count, void* idx, int streamc, VertexStreamState* streams);
    void DrawArrayClipNI(int prim, int base, int count, int streamc, VertexStreamState* streams);

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

    int findFreeTexture(Texture *tex, int format, int comp, int size, int locksize);
#ifdef DEBUG
    void listTextures() const;
#endif
    
private:

    int is_open, in_render, next_render, has_render;
    SceUID finish_sema;

    ClipVertexArray outc;

#ifdef VRAM_LIST
    VBLK dispblk[LISTS];
    char *displist[LISTS];
#else
    char displist[LISTS][LISTSIZE] __attribute__((aligned(64)));
#endif

    static const int MaxShaderCount = 8;
    PSPShader* vshader[MaxShaderCount];
    PSPBuffer* ibuffer;
    VertexStreamState streams[16];

    // textures
    DynamicArray<PSPTexture>	Textures;
    DynamicArray<int>           FreeTextures;
    int                         TexBytes;
    int							FrameTris;
    int							FrameDIPs;

    // created buffers
    DynamicArray<PSPBuffer>			    Buffers;
    DynamicArray<PSPShader>				Shaders;
    DynamicArray<int>					ResTextures;
    //DynamicArray<IDirect3DQuery9*>		Queries;

    DWORD				SavedFogColor;

    RenderState         CachedState;
    TextureState        CachedTextureStates[8];
    //TextureStageState   CachedStageState;

    int                 FogMode;
    float               FogStart, FogEnd;
    bool                Lighting, FogEnable;
    bool                LightEnable[4];

    DWORD               CachedColor;
    DWORD               CachedSpecularMat;
    float               CachedSpecularPow;
    int                 CachedTexId, CachedTexFunc, CachedTexXfrm, CachedTexProj;
    BYTE                CachedBlendMode, CachedFilter, CachedAddrMode;
    bool                CachedTexEnable, CachedCDbl;
    bool                CachedDepthTest, CachedDepthWrite;

    //Plane				CachedVSConst[96];
    //Plane				CachedPSConst[8];

    int					CurrentStreamSize;
    BOOL				CurrentStreamIsIndexed;
    BOOL				StencilSurfaceTouched;
    BOOL				CullFlip;

    //LPDIRECT3DVERTEXSHADER9			CurrentVertexShader;
    //LPDIRECT3DPIXELSHADER9			CurrentPixelShader;
    //LPDIRECT3DVERTEXDECLARATION9	CurrentVDecl;


    UINT	FullScreenWidth;
    UINT	FullScreenHeight;

    UINT	ViewportWidth;
    UINT	ViewportHeight;

    //D3DGAMMARAMP		OrigianalGammaRamp;

    VBLK FrontBuffer;
    VBLK BackBuffer;
    VBLK DepthBuffer;

    Viewport OriginalViewport;

    VBLK CurrentBackBuffer;
    VBLK CurrentDepthBuffer;
    unsigned CurrentWidth;

    void* CachedBackBufPtr;
    void* CachedDepthBufPtr;

    ScePspFMatrix4 mat_obj;
    ScePspFMatrix4 mat_cam;
    ScePspFMatrix4 mat_proj;
    ScePspFMatrix4 mat;
    ScePspFVector4 clip_plane[4];
    bool mat_dirty, clip_dirty;

    int clipstream[2];
    int clippos;
    bool    waitVBlank;

public:
    static VideoSystemPSP* opendev;
};



// =============================== RENDER EFFECT ======================================

class PSPEffect : public RenderEffect
{
public:
    PSPEffect();
    ~PSPEffect();

    BOOL	EnumTechniques(DynamicArray<RenderTechnique>& Out);
    DWORD	GetParamIndex(const char* Name);
    DWORD	GetTextureIndex(const char* Name);
    BOOL	GetTechniqueString(DWORD Technique, const char* Name, String& Value);

    BOOL	SetParam(const char* Name, const void* Data, DWORD Size);
    BOOL	SetParam(DWORD Index, const void* Data, DWORD Size);
    BOOL	SetTexture(const char* Name, Texture* Tex);
    BOOL	SetTexture(DWORD Index, Texture* Tex);

    BOOL	SetTechnique(DWORD Index);
    UINT	Begin();
    BOOL	BeginPass(UINT Num);
    BOOL	EndPass();
    BOOL	End();

private:
    int	Effect;
};


#endif //_VIDEOSYSTEM_PSP_

