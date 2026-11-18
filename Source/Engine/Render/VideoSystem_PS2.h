// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _VIDEOSYSTEM_PS2_
#define _VIDEOSYSTEM_PS2_


#include "Engine.h"
#include "Render/Viewport.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Render/Material.h"
//#pragma warning(disable: 4201) // non-standard extension used: nameless union/struct

#include <GL/gl.h>

//#define _DXMAT(x)            ((D3DMATRIX*)(const FLOAT*)(x))

union GLColor
{
    GLfloat f[4];

    struct
    {
        GLfloat R,G,B,A;
    };

    GLColor() {}

    GLColor(const DWORD ub4) {
        const float inv = 1/255.f;
        Color_TG col(ub4);
        R = col.R*inv;
        G = col.B*inv;
        B = col.B*inv;
        A = col.A*inv;
    }

    GLColor(const float r, const float g, const float b, const float a=1.0f) : R(r), G(g), B(b), A(a) {}

    GLColor(const Vector& vec) : R(vec.X), G(vec.Y), B(vec.Z), A(0) {}

    operator const GLfloat* () const { return f; }
};

struct PS2Shader
{
    int Type;

    // device stuff
    enum {
        EPosition,
        ENormal,
        EDiffuse,
        EWeight,
        EIndex,
        ETex0,
        ETex1,
        ECount = 16
    };

    struct ShaderInput {
        int Stream, Offset;
        GLint Size;
        GLenum Type;
    } input[ECount];

    bool Clip;
#if defined(TRANSGAMING) && defined(DEBUG)
    int id;
    static int nextID;
#endif

    PS2Shader() : Type(EInvalidIndex)
    {
#if defined(TRANSGAMING) && defined(DEBUG)
        id = nextID++;
#endif
    }
};

struct PS2Buffer
{
    enum { 
        EUnusedStream    = 0, 
        EIndexStream    = (1<<0),    // this is an index stream 
        EVertexStream    = (1<<1),    // this is a vertex stream

        EUsageSWVP        = (1<<31),    // this stream is used for software vertex processing
    };

    DWORD   Type;       // unused,index,vertex
    int     ElemCount;
    int     Stride;
    void*   SourceData; // NULL for dynamic buffers, points to real data for static ones
                        // to allow for automatic restore and copy to AGP/video mem

    // device stuff
    void* Ptr;

    //PS2Buffer() : Type(0), VBuf(0), IBuf(0) {}
    PS2Buffer() : Type(EUnusedStream) {}
};

struct PS2Texture
{
    //Texture* TexDef;

    GLenum Target, Format, Type;
    GLuint Tex;
    int    Size;//, LockSize;
    void*  Ptr;
    void*  Pal;
    BYTE*  LockPtr;
    int    Locks;
    DWORD  V2;

    PS2Texture() : /*TexDef(0), */Target(0), Tex(0), Size(0), /*LockSize(0), */Ptr(0), Pal(0), LockPtr(0), Locks(0), V2(0) {}
    PS2Texture(Texture* texDef, GLenum target, GLenum format, GLenum type, GLuint tex, int size, int locksize)
    :   /*TexDef(texDef), */Target(target), Format(format), Type(type), Tex(tex), Size(size), /*LockSize(locksize),*/
        Ptr(0), Pal(0), LockPtr(0), Locks(0), V2(0) {}
  
    void SetParams(Texture* texDef, GLenum target, GLenum format, GLenum type, GLuint tex, int size, int locksize);
};

struct VertexStreamState
{
    const PS2Buffer* Buf;
    DWORD Offset;

    VertexStreamState() : Buf(NULL), Offset(0) {}
};

class VideoSystemPS2 : public RenderDevice
{
public:
    VideoSystemPS2();
    ~VideoSystemPS2();

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
    BOOL ClearFB(DWORD argb);

    int        CreateQuery(int Type);
    BOOL    BeginQuery(int Query);
    void    EndQuery(int Query);
    BOOL    GetQueryData(int Query, void* OutData, int Size);
    void    CreateTexture(Texture* tex);
    void    SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace);
    BOOL    SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace = 0);
    BOOL    SetDepthTarget(Texture* DepthBuf);
    void    SetFBRenderTarget();
    BOOL    GrabBackBuffer(Texture* Tex, HWND hwnd);
    BOOL    GrabZStencilBuffer(Texture* Tex);
    void    GetBackBufferSize(UINT& SizeX, UINT& SizeY);
    BYTE*    LockTexture(Texture* tex, int &pitch);
    void    UnlockTexture(Texture* tex);
    void    SetTexturePriority(Texture *tex, DWORD priority);
    void    PreLoadTexture(Texture *tex);
    int     CreateStream(void* sourceData, DWORD type, int count, int stride);
    BYTE*     LockStream  (int streamId, int pos, int count);
    void     UnlockStream(int streamId);
#ifndef TRANSGAMING
    // this function is obselete even in the windows version
    int        CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList);
#endif
    int     CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount);
    void    SetShaderMemory(DWORD addr, const void* data, int count);
    void    SetPixelShaderMemory(DWORD addr, const void* data, int count);
    void    SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count);
    void    SetShader(int shaderIndex, int shaderType);
    void    SetState(const RenderState& state);
    void    SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset=0);
    void    SetTextureState(const TextureState* stages, int stageCount);
    void    ResetTextureStages();
    void    SetFogState(const FogState& state);
    void    SetFog(bool isFog);
    void    SetLight(int number, const LightState& state);

    void    SetBoneMatrix(int index, const Matrix *matrix);
    
    void    SetPolygonMode(int mode);

    void    SetSpecular(const Vector& color, const float power);
    void    SetLighting(bool isLighting);
    void    SetAmbientLight(const Vector& color);
    void    SetNoLight(int number);
    void    SetDirLight(int number, const Plane& dir, const Vector& color);

    void    SetObjectTransform(const Matrix* matPtr);
    void    SetCameraTransform(const Matrix* matPtr);
    void    SetScreenTransform(const Matrix* matPtr);

    void     Draw(const int primType, int base, int count, int vbase=EInvalidIndex, int vcount=EInvalidIndex, int basevindex=0, bool swapPath = false);
    void    ReleaseResource(int resourceType, const int* ids, int count);

    RenderEffect* CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size);

    void SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite);
    void SetCullFlip(BOOL Flip);
    void SetVBlankState(bool state);
    bool GetVBlankState() const;

    void SetConstColor(DWORD color);

    // INTERNAL
private:
    void RestoreDevice();
    void ReleaseDevice();

    void ReleaseShaders();
    void RestoreShaders();

    void GetOriginalGamma();
    BOOL RestoreOriginalGamma();

    bool ActivateStage(int stage);
    int GetStream(int index, int base, VertexStreamState* streams, BYTE*& ptr);
    void DrawArray(int prim, int base, int count, void* idx, VertexStreamState* streams);

    // texture creation utils
    void* CreateTextureFromImage(GLenum,LPCVOID,UINT,int,int,int,struct ImageInfo*,GLenum&,GLenum&);
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

    int  findFreeTexture(Texture *texDef, GLenum target, GLenum format, GLenum type, GLuint tex, int size, int locksize);

private:

    int is_open, is_render;

    static const int MaxShaderCount = 8;
    PS2Shader* vshader[MaxShaderCount];
    PS2Buffer* ibuffer;
    VertexStreamState streams[16];

    // textures
    DynamicArray<PS2Texture>    Textures;
    DynamicArray<int>           FreeTextures;
    int                         TexBytes;
    int                         FrameTris;
    int                         FrameDIPs;

    // created buffers
    DynamicArray<PS2Buffer>     Buffers;
    DynamicArray<PS2Shader>     Shaders;
    DynamicArray<int>           ResTextures;
    //DynamicArray<IDirect3DQuery9*>        Queries;

    DWORD               SavedFogColor;

    RenderState         CachedState;
    TextureState        CachedTextureStates[8];
    //TextureStageState   CachedStageState;

    int                 FogMode;
    float               FogStart, FogEnd;
    bool                Lighting, FogEnable;
    bool                LightEnable[4];

    bool                waitVBlank;

    GLColor             CachedColor;
    GLColor             CachedAmbientColor;
    GLColor             CachedSpecularMat;
    float               CachedSpecularPow;
    int                 CachedTexId, CachedTexFunc;
    BYTE                CachedBlendMode, CachedFilter, CachedAddrMode;
    bool                CachedTexEnable, CachedCDbl;
    bool                CachedDepthTest;

    //Plane               CachedVSConst[96];
    //Plane               CachedPSConst[8];

    int                 CurrentStreamSize;
    BOOL                CurrentStreamIsIndexed;
    BOOL                StencilSurfaceTouched;
    BOOL                CullFlip;

    //LPDIRECT3DVERTEXSHADER9            CurrentVertexShader;
    //LPDIRECT3DPIXELSHADER9            CurrentPixelShader;
    //LPDIRECT3DVERTEXDECLARATION9    CurrentVDecl;


    UINT    FullScreenWidth;
    UINT    FullScreenHeight;

    UINT    ViewportWidth;
    UINT    ViewportHeight;

    //D3DGAMMARAMP        OriginalGammaRamp;

    Viewport OriginalViewport;

    Matrix mat_obj, mat_cam, mat_proj;
    bool mat_dirty;

public:
    static LogBuffer PS2Log;
};

#endif //_VIDEOSYSTEM_PS2_

