//#include "D3Dev.h"
//#include "D3DEffect.h"
//#include "dxerr9.h"
//#include "Render/Viewport.h"
//#include "Render/View.h"
//#include "Render/Video.h"
//#include "Render/Material.h"
#include "Render/VideoSystem_PSP.h"

#include <psperror.h>
#include <kernelutils.h>
#include <displaysvc.h>
#include <geman.h>
#include <libgu.h>
#ifdef USE_GIM
#include <gim/gim_picture.h>
#include <gim/gim_protected.h>
#endif

#ifdef __vfpu_enabled__
#include <vfpu_intrinsics.h>
#else
#error Please enable VFPU support
#endif
#include <libvfpu.h>

#include <tg_pc2psp.h>
#include <malloc.h>

//#define NVPERFHUD_ENABLE

// this is the largest Z range I found used in the PSP samples;
// it seems to be dangerous to use the whole depth range or something.
#define NEAR_Z 0xe000
#define FAR_Z 0x1000
// I'll use the full range until I know a reason not to, though.
//#define NEAR_Z 0xffff
//#define FAR_Z 0x0000

#define CLIPSTREAM_SIZE 16384   //32768

#define STREAM_UNCACHED

//#define WAIT_VBLANK

#define IGNORE_COLOR
#define VFPU_WRITE

#ifdef IGNORE_COLOR
# define CLIPSTREAM1 RenderVertexN1
# define CLIPSTREAM2 RenderVertexN2
# define MESHVERTEX  MeshVertex
# define IF_COLOR(x)
#else
# define CLIPSTREAM1 RenderVertexNC1
# define CLIPSTREAM2 RenderVertexNC2
# define MESHVERTEX  MeshVertex
# define IF_COLOR(x) x
#endif

#define VFPU_SMAT(x) (*(const vfpu_m4x4*)(const float*)(x))
#define VFPU_DMAT(x) (*(vfpu_m4x4*)((float*)(x)))

#define VFPU_SVEC(x) (*(const vfpu_v4*)(const float*)(x))
#define VFPU_DVEC(x) (*(vfpu_v4*)(float*)(x))

// The VFPU has a way to write directly to memory,
// bypassing the CPU. But for this to happen, the addresses
// must be qword-aligned, which isn't the case.
#if defined(IGNORE_COLOR) && defined(VFPU_WRITE)
// getting some mysterious bus errors if I don't add a vnop here...
// the manual mentions one bug with sv.q,wb which the assembler
// corrects for with vnop, but it seems there might be more
// (for example if the next cpu instruction changes the offset
// register used in the sv.q, which our code tends to do)
#define VFPU_WB_WRITE(x, y) \
  __asm__("sv.q %1, %0, wb;vnop" : "=m" (*(x)) : "j" (y));
#define VFPU_WB_WRITE_X(x, y) (*(x)) = vfpu_extract_float_from_v4x(y);
#define VFPU_WB_WRITE_Y(x, y) (*(x)) = vfpu_extract_float_from_v4y(y);
#define VFPU_WB_WRITE_Z(x, y) (*(x)) = vfpu_extract_float_from_v4z(y);
#define VFPU_WB_WRITE_W(x, y) (*(x)) = vfpu_extract_float_from_v4w(y);
#define VFPU_WB_SYNC() __asm__ __volatile__ ("vflush;vnop");
#else
#define VFPU_WB_WRITE(x, y) VFPU_DVEC(x) = y;
#define VFPU_WB_WRITE_X(x, y) (*(x)) = vfpu_extract_float_from_v4x(y);
#define VFPU_WB_WRITE_Y(x, y) (*(x)) = vfpu_extract_float_from_v4y(y);
#define VFPU_WB_WRITE_Z(x, y) (*(x)) = vfpu_extract_float_from_v4z(y);
#define VFPU_WB_WRITE_W(x, y) (*(x)) = vfpu_extract_float_from_v4w(y);
#define VFPU_WB_SYNC()
#endif

static const int EClipStream = (1<<30);

#ifdef TRANSGAMING_LOGGING
//LogBuffer VideoSystemPSP::PSPLog;
#endif

#ifdef TRANSGAMING_SHOW_FPS
extern int g_triCount;
extern int g_objectCount;
extern int g_vertexCount;
#endif

VideoSystemPSP* VideoSystemPSP::opendev = NULL;

int enable_stage_two = 1;

static ScePspFMatrix4 idmat = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f,
};

static ScePspFMatrix4 idclip[4] = {
  { 1.0f, 0.0f, 0.0f, 1.0f},
  {-1.0f, 0.0f, 0.0f, 1.0f},
  { 0.0f, 1.0f, 0.0f, 1.0f},
  { 0.0f,-1.0f, 0.0f, 1.0f},
};

static inline int calcz(float z)
{
    if (z >= 1.0f) return FAR_Z;
    if (z <= 0.0f) return NEAR_Z;
    return (int)(NEAR_Z + z*(FAR_Z-NEAR_Z));
}

// DLL interface
RenderDevice* CreateDevice()
{
//    VideoSystemPSP::PSPLog.Open("D3Dev.log");
//    VideoSystemPSP::PSPLog.Print("Creating PSP Renderer\n");
    m_setTag("Video");
    return new VideoSystemPSP();
}

void DestroyDevice(RenderDevice* rdevPtr)
{
//    VideoSystemPSP::PSPLog.Print("Destroying PSP Renderer\n");
    if(rdevPtr) delete rdevPtr;
}

// ===========================================================

VideoSystemPSP::VideoSystemPSP() : RenderDevice(), outc(8)
{
/*#ifdef TRANSGAMING_LOGGING
    VideoSystemPSP::PSPLog.Open("D3Dev.log");
    VideoSystemPSP::PSPLog.Print("Creating PSP Renderer\n");
    VideoSystemPSP::PSPLog.Flush();
#endif//*/
    //RenderDevice();
    is_open = 0;
    in_render = 0;
    next_render = 0;
    has_render = 0;
    finish_sema = 0;
    FullScreenWidth = 0;
    FullScreenHeight = 0;
    CurrentStreamSize = 0;
    CurrentStreamIsIndexed = FALSE;
    //CurrentVertexShader = 0;
    //CurrentPixelShader = 0;
    TexBytes = 0;
    FrameTris = 0;
    FrameDIPs = 0;
    FrontBuffer = 0;
    BackBuffer = 0;
    DepthBuffer = 0;
    CurrentBackBuffer = 0;
    CurrentDepthBuffer = 0;
    CurrentWidth = 0;
    CachedBackBufPtr = 0;
    CachedDepthBufPtr = 0;
    ViewportWidth = 0;
    ViewportHeight = 0; 
    StencilSurfaceTouched = FALSE;
    CullFlip = 0;
    waitVBlank = false;

    for (int i = 0; i < MaxShaderCount; i++)
        vshader[i] = NULL;

    clipstream[0] = EInvalidIndex;
    clipstream[1] = EInvalidIndex;
    clippos = 0;

#ifdef VRAM_LIST
    for (unsigned l=0; l<LISTS; l++) {
        dispblk[l] = 0;
        displist[l] = 0;
    }
#endif

    //add a dummy texture so that texture 0 is always occupied
    Textures.Add(PSPTexture());
    GetOriginalGamma();
}

VideoSystemPSP::~VideoSystemPSP()
{
    RestoreOriginalGamma();
    Close();
    Textures.ClearArray();
    FreeTextures.ClearArray();
/*#ifdef TRANSGAMING_LOGGING
    VideoSystemPSP::PSPLog.Print("Destroying PSP Renderer\n");
    VideoSystemPSP::PSPLog.Flush();
#endif//*/
}

static void vram_open(void);
static void vram_close(void);
static VBLK vram_alloc(unsigned size);
static void vram_free(VBLK mh);
static void* vram_lock(VBLK mh);
static void vram_unlock(VBLK mh);
static void* vram_fb_lock(VBLK mh);
static void vram_fb_unlock(VBLK mh);
static void vram_swap(VBLK mh1, VBLK mh2);

// =============================================================
// RenderDevice interface
// =============================================================

BOOL VideoSystemPSP::Open(unsigned int)
{
    Close();

    BOOL success = (sceGuInit() == SCE_OK);

    if (!success) {
        TRACE("VideoSystemPSP::Open(): ERROR-> Could not open PSP Render Device\n");
        return success;
    }

    opendev = this;
    is_open = 1;
    waitVBlank = false;

    HWClass = EHWClassGenericTnL;

    DisplayModes[0].XRes = SCEGU_SCR_WIDTH;
    DisplayModes[0].YRes = SCEGU_SCR_HEIGHT;
    DisplayModes[0].RefreshRates[0] = 60; // ?
    DisplayModes[0].RefreshRateNum = 1;
    DisplayModeNum = 1;

    sceGuSetCallback(SCEGU_INT_SIGNAL, signal_cb);
    sceGuSetCallback(SCEGU_INT_FINISH, finish_cb);

    finish_sema = sceKernelCreateSema("Finish", SCE_KERNEL_SA_THFIFO, 0, LISTS, NULL);

    vram_open();

    AllocList();
    StartList();

#ifdef USE_GIM
    // FIXME: memory manager stuff
# ifdef MEMORY_TEST
    sceGimPictureSetMemoryManager(m_gimAllocator, m_gimDeallocator);
# else
    sceGimPictureSetMemoryManager(malloc, free);
# endif
#endif

#ifdef TRANSGAMING_LOGGING
    TRACE("VideoSystemPSP::Open(): opened PSP Render Device\n");
#endif

    return success;
}

void VideoSystemPSP::Close()
{
    if (!is_open) return;

#ifdef TRANSGAMING_LOGGING
    TRACE("VideoSystemPSP::Close(): closing the video system...\n");
#endif

    ReleaseDevice();

    FinishList(true);
    sceDisplayWaitVblankStart();
    FreeList();

    vram_close();

    sceGuTerm();

    sceKernelDeleteSema(finish_sema);
    finish_sema = 0;

    is_open = 0;
    opendev = 0;
    TRACE("VideoSystem_PSP::Close(): video system close complete\n");
}

BOOL VideoSystemPSP::SetMode(UINT width, UINT height, BOOL fullscreen, int multisampling)
{
    BOOL success=TRUE;

#ifdef TRANSGAMING_LOGGING
    TRACE("VideoSystem_PSP::SetMode(): setting the video mode to %dx%d (fullscreen = %s, multisampling = %d)...\n", width, height, fullscreen ? "true" : "false", multisampling);
#endif

    FinishList(true);
    sceDisplayWaitVblankStart();
    sceGuDisplay(SCEGU_DISPLAY_OFF);

    FreeList();
    if (DepthBuffer) { vram_free(DepthBuffer); }
    if (BackBuffer) { vram_free(BackBuffer); }
    if (FrontBuffer) { vram_fb_unlock(FrontBuffer); vram_free(FrontBuffer); }

    // PSP doesn't support windowed mode...
    FullScreenWidth = width;
    FullScreenHeight = height;

    SetRect(&TG_client_rect, 0, 0, width, height);

#ifdef VRAM_LIST
    // translation is difficult with displists and vbuffers in VRAM,
    // we'll need to find a solution for them before we can enable it.
    sceGeEdramSetAddrTranslation(0);
#else
    sceGeEdramSetAddrTranslation(SCEGU_VRAM_WIDTH * 4); // 32 bpp
#endif

    FrontBuffer = vram_alloc(SCEGU_VRAM_WIDTH * SCEGU_SCR_HEIGHT * 4);
    BackBuffer = vram_alloc(SCEGU_VRAM_WIDTH * SCEGU_SCR_HEIGHT * 4);
    DepthBuffer = vram_alloc(SCEGU_VRAM_WIDTH * SCEGU_SCR_HEIGHT * 2);

    AllocList();
    StartList();

    void* FrontBufPtr = vram_fb_lock(FrontBuffer);
    sceGuDispBuffer(FullScreenWidth, FullScreenHeight, FrontBufPtr, SCEGU_VRAM_WIDTH);
    sceGuOffset(SCEGU_SCR_OFFSETX, SCEGU_SCR_OFFSETY);

#if 0
    void* BackBufPtr = vram_fb_lock(BackBuffer);
    void* DepthBufPtr = vram_fb_lock(DepthBuffer);
    sceGuDrawBuffer(SCEGU_PF8888, BackBufPtr, SCEGU_VRAM_WIDTH);
    CachedBackBufPtr = BackBufPtr;
    sceGuDepthBuffer(DepthBufPtr, SCEGU_VRAM_WIDTH);
    CachedDepthBufPtr = DepthBufPtr;
    vram_fb_unlock(DepthBuffer);
    vram_fb_unlock(BackBuffer);
#endif

    CurrentDepthBuffer = DepthBuffer;
    CurrentWidth = SCEGU_VRAM_WIDTH;
    CurrentBackBuffer = FrontBuffer;
    ClearFB(0);
    CurrentBackBuffer = BackBuffer;

    RestoreDevice();

    // turn display on
    FinishList(true);
    sceDisplayWaitVblankStart();
    sceGuDisplay(SCEGU_DISPLAY_ON);

    // restart display list
    StartList();

#ifdef TRANSGAMING_LOGGING
    TRACE("VideoSystem_PSP::SetMode(): mode set complete (displist = %p)\n", displist);
#endif
    return success;
}

BOOL VideoSystemPSP::SetGamma(float contrast, float gamma, float brightness)
{
    // perhaps gamma could be implemented through the CLUT,
    // but for now, gamma control won't be available
    return TRUE;
}

void VideoSystemPSP::GetOriginalGamma()
{
}

BOOL VideoSystemPSP::RestoreOriginalGamma()
{
    SetGamma(0.5f, 1.0f,0.5f);
    return TRUE;
}

BOOL VideoSystemPSP::ClearZ(float z)
{
    void* BackBufPtr = vram_fb_lock(CurrentBackBuffer);
    void* DepthBufPtr = vram_fb_lock(CurrentDepthBuffer);

    if (BackBufPtr != CachedBackBufPtr) {
        sceGuDrawBuffer(SCEGU_PF8888, BackBufPtr, CurrentWidth);
        CachedBackBufPtr = BackBufPtr;
    }
    if (DepthBufPtr != CachedDepthBufPtr) {
        sceGuDepthBuffer(DepthBufPtr, CurrentWidth);
        CachedDepthBufPtr = DepthBufPtr;
    }

    sceGuClearDepth(calcz(z));
    sceGuClearStencil(0);
    sceGuClear(SCEGU_CLEAR_DEPTH | SCEGU_CLEAR_STENCIL);

    vram_fb_unlock(CurrentDepthBuffer);
    vram_fb_unlock(CurrentBackBuffer);

    return TRUE;
}

BOOL VideoSystemPSP::ClearFB(DWORD abgr)
{
    void* BackBufPtr = vram_fb_lock(CurrentBackBuffer);
    void* DepthBufPtr = vram_fb_lock(CurrentDepthBuffer);

    if (BackBufPtr != CachedBackBufPtr) {
        sceGuDrawBuffer(SCEGU_PF8888, BackBufPtr, CurrentWidth);
        CachedBackBufPtr = BackBufPtr;
    }
    if (DepthBufPtr != CachedDepthBufPtr) {
        sceGuDepthBuffer(DepthBufPtr, CurrentWidth);
        CachedDepthBufPtr = DepthBufPtr;
    }

    sceGuClearColor(abgr);
    sceGuClear(SCEGU_CLEAR_COLOR);

    vram_fb_unlock(CurrentDepthBuffer);
    vram_fb_unlock(CurrentBackBuffer);

    return TRUE;
}

BOOL VideoSystemPSP::SetViewport(int x, int y, int w, int h, float zNear, float zFar)
{
    sceGuViewport(x + 2048, y + 2048, w, h);
    sceGuDepthRange(calcz(zNear), calcz(zFar));
    return TRUE;
}

BOOL VideoSystemPSP::SetScissor(int x1, int y1, int x2, int y2)
{
    sceGuScissor(x1, y1, x2-x1, y2-y1);
    return TRUE;
}

BOOL VideoSystemPSP::Lock(Viewport& viewport)
{
    // restart display list
    FinishList();
    StartList();

    void* BackBufPtr = vram_fb_lock(CurrentBackBuffer);
    void* DepthBufPtr = vram_fb_lock(CurrentDepthBuffer);

    if (BackBufPtr != CachedBackBufPtr) {
        sceGuDrawBuffer(SCEGU_PF8888, BackBufPtr, CurrentWidth);
        CachedBackBufPtr = BackBufPtr;
    }
    if (DepthBufPtr != CachedDepthBufPtr) {
        sceGuDepthBuffer(DepthBufPtr, CurrentWidth);
        CachedDepthBufPtr = DepthBufPtr;
    }

    OriginalViewport = viewport;
    SetViewport(OriginalViewport.XOffset, OriginalViewport.YOffset,
                OriginalViewport.Width, OriginalViewport.Height,
                OriginalViewport.zNear, OriginalViewport.zFar);
    int clearMask = 0;
    if (viewport.ClearFlags == EClearAll)
        clearMask = SCEGU_CLEAR_ALL;
    else {
        if (viewport.ClearFlags & EClearColor)
            clearMask |= SCEGU_CLEAR_COLOR;
        if (viewport.ClearFlags & EClearZ)
            clearMask |= SCEGU_CLEAR_DEPTH;
        if (viewport.ClearFlags & EClearStencil)
            clearMask |= SCEGU_CLEAR_STENCIL;
    }
    if (clearMask) {
        sceGuClearColor(viewport.ClearColor);
        sceGuClearDepth(calcz(1.0f));
        sceGuClearStencil(0);
        sceGuClear(clearMask);
    }
    SetCameraTransform(&viewport.WorldToCamera);
    SetScreenTransform(&viewport.CameraToScreen);
    StencilSurfaceTouched = FALSE;
    return TRUE;
}

void VideoSystemPSP::Unlock()
{
    vram_fb_unlock(CurrentDepthBuffer);
    vram_fb_unlock(CurrentBackBuffer);

}

void VideoSystemPSP::Flip(HWND hwnd)
{
    VBLK vtmp;
    void* ptmp;

    vram_fb_unlock(FrontBuffer);

    FinishList(true);

    vram_swap(FrontBuffer, BackBuffer);

    void* FrontBufPtr = vram_fb_lock(FrontBuffer);

    // Unlike sceGuSwapBuffers, sceGuDispBuffer updates the frame buffer address
    // at next vsync, not immediately, so we'll call it before waiting for vsync.
    // As an alternative, we could call sceDisplaySetFrameBuf directly ourselves,
    // which may be preferable in case we don't actually wait for vsync here.
    sceGuDispBuffer(FullScreenWidth, FullScreenHeight, FrontBufPtr, SCEGU_VRAM_WIDTH);

    // This seems to do the same as sceGuDispBuffer... not what we really want.
    //void* FBPtr = vram_lock(FrontBuffer);
    //sceDisplaySetFrameBuf(FBPtr, SCEGU_VRAM_WIDTH, SCEGU_PF8888, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);
    //vram_unlock(FrontBuffer);

#ifdef WAIT_VBLANK
    sceDisplayWaitVblankStart();
#else
    if (waitVBlank)
        sceDisplayWaitVblankStart();
#endif

    // But for some reason, NEXTHSYNC causes a garbled display at the moment.
    //void* FBPtr = vram_lock(FrontBuffer);
    //sceDisplaySetFrameBuf(FBPtr, SCEGU_VRAM_WIDTH, SCEGU_PF8888, SCE_DISPLAY_UPDATETIMING_NEXTHSYNC);
    //vram_unlock(FrontBuffer);

    StartList();

#if 0
    void* BackBufPtr = vram_fb_lock(BackBuffer);
    sceGuDrawBuffer(SCEGU_PF8888, BackBufPtr, SCEGU_VRAM_WIDTH);
    CachedBackBufPtr = BackBufPtr;
    vram_fb_unlock(BackBuffer);
#endif

}

void VideoSystemPSP::TakeScreenshot(const char* filename)
{
}

void VideoSystemPSP::ReleaseShaders()
{
}

void VideoSystemPSP::RestoreShaders()
{
}

void VideoSystemPSP::Flush()
{
}


// RenderDevice interface
void VideoSystemPSP::CreateTexture(Texture* texture)
{
    Assert(texture);

    // init with invalid values
    texture->DeviceId       = EInvalidIndex;
    texture->OriginalWidth  = -1;
    texture->OriginalHeight = -1;
    //texture->Width          = -1;
    //texture->Height         = -1;
    texture->PixelBytes     = -1;

    // is it from raw bytes? otherwise from file
    int rawBytes = texture->Flags & (Texture::ETexRaw32|Texture::ETexRaw8 | Texture::ETexRaw32Alpha);
    int special = (texture->Type == Texture::ETexRenderTarget)
                | (texture->Type == Texture::ETexRenderTargetCubeMap)
                | (texture->Type == Texture::ETexDepthTarget)
                | (texture->Type == Texture::ETexNormCubeMap);

    // need data/size for non-special textures
    if (!special && (texture->DataFile == NULL || texture->Size <= 0))
        return;

    // dispatch
    switch (texture->Type)
    {
    case Texture::ETexRegular:
        if (rawBytes)
        {
            if( texture->Flags & Texture::ETexDynamic )
#if 0
                CreateDynamicTexture(texture);
#else
                ;
#endif
            else
                CreateRegularTexture(texture);
        }
        else if (texture->Flags & Texture::ETexDuDv){
#if 0
            CreateDuDvTextureFromFile(texture);
#else
            Assert(!"dUdV texture!");
#endif
        }
        else
            CreateRegularTextureFromFile(texture);
        break;

#if 0
    case Texture::ETexRenderTargetCubeMap:
        CreateRenderTargetCubeMapTextureFromFile(texture);
        break;

    case Texture::ETexCubeMap:
        if (rawBytes)
            CreateCubeMapTexture(texture);
        else
            CreateCubeMapTextureFromFile(texture);
        break;

    case Texture::ETexVolume:
        if (rawBytes || (texture->Flags & Texture::ETexAnim3D))
            CreateVolumeTexture(texture);
        else
            CreateVolumeTextureFromFile(texture);
        break;
#endif

    case Texture::ETexRenderTarget:
        CreateRenderTarget(texture);
        break;

    case Texture::ETexDepthTarget:
        CreateDepthSurface(texture);
        break;

#if 0
    case Texture::ETexNormCubeMap:
        if (texture->Width)
            CreateNormCubeMap(texture);
        break;
#endif
    }
    if (texture->DeviceId == EInvalidIndex) {
        texture->ReleaseData();
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CreateTexture(): ERROR-> could not create texture type %d {flags = 0x%08X}\n", texture->Type, texture->Flags);
#endif
        return;
    }
}

void VideoSystemPSP::SetFBRenderTarget()
{
    CurrentBackBuffer = BackBuffer;
    CurrentDepthBuffer = DepthBuffer;
    CurrentWidth = SCEGU_VRAM_WIDTH;
}

void VideoSystemPSP::SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace)
{
    SetRenderTarget(0, colorBuf, cubeFace);
    SetDepthTarget(depthBuf);
}

BOOL VideoSystemPSP::SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace)
{
#ifdef TRANSGAMING_LOGGING
    //TRACE("VideoSystemPSP::SetRenderTarget(): WARNING-> setting render target!\n");
#endif
    if (Index) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::SetRenderTarget(): ERROR-> attempted to set a nonzero indexed render target!\n");
#endif
        return FALSE;
    }

    if (ColorBuf->DeviceId == EInvalidIndex)
        return FALSE;

    PSPTexture& Tex = Textures[ColorBuf->DeviceId];
    if (!Tex.Target)
        return FALSE;

    CurrentBackBuffer = Tex.Target;
#ifdef USE_GIM
    SceGimImageHeader* img = sceGimPictureGetImage(Tex.Pic);
    int width = sceGimImageGetWidth(img, 0);
    CurrentWidth = width;
#endif
}

BOOL VideoSystemPSP::SetDepthTarget(Texture* DepthBuf)
{
#ifdef TRANSGAMING_LOGGING
    //TRACE("VideoSystemPSP::SetDepthTarget(): WARNING-> setting a depth target!\n");
#endif

    if (!DepthBuf) {
        CurrentDepthBuffer = DepthBuffer;
        return TRUE;
    }

    if (DepthBuf->DeviceId == EInvalidIndex)
        return FALSE;

    PSPTexture& Tex = Textures[DepthBuf->DeviceId];
    if (!Tex.Target)
        return FALSE;

    CurrentDepthBuffer = Tex.Target;
}

int VideoSystemPSP::CreateStream(void* sourceData, DWORD type, int count, int stride)
{
    int index = EInvalidIndex;
    void *Mem = NULL;
    int Size = 0;
    int format = 0;

    // the PSP supports 8-bit and 16-bit indices, but not 32-bit.
    if (type & RenderDevice::EIndex16) {
        format = SCEGU_INDEX_USHORT;
    }
    else if (type & RenderDevice::EIndex32) {
        format = SCEGU_INDEX_NONE;
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CreateStream(): ERROR-> attempting to create an index stream with 32-bit indices!\n");
#endif
        Assert(0);
    }

    Size = count*stride;

    if (sourceData) {
        // This is a static buffer. We should consider allocating
        // a buffer in VRAM and transfer the data there, but we'd
        // need a VRAM memory manager first.
        // Otherwise, we can just stream straight from sourceData
        // to save system RAM.
        Mem = NULL;
    } else {
        // This is a dynamic buffer.
        m_setTag("Video");
        Mem = new char[Size];

        if (!Mem) {
#ifdef TRANSGAMING_LOGGING
            TRACE("VideoSystemPSP::CreateStream(): ERROR-> could not allocate memory to store the stream data\n");
#endif
            Assert(Mem);
            return EInvalidIndex;
        }
    }

    // find unused slot
    PSPBuffer* buf=0;
    for (int i=0; i<Buffers.Size(); ++i)
    {
        if(Buffers[i].Type==PSPBuffer::EUnusedStream)
        {
            buf=&Buffers[i];
            index=i;
            break;
        }
    }

    if(!buf)
    {
#ifdef TRANSGAMING
        index = Buffers.GrowArray(1);

        buf = &Buffers[index];
#else
        index=Buffers.Size();
        buf=Buffers.CreateArray();
#endif
        Assert(buf);
    }

    // FIXME: setup DMA transfer if allocated static buffer in VRAM

    if(type & RenderDevice::EIndex16 || type & RenderDevice::EIndex32) 
    {
        buf->Type=PSPBuffer::EIndexStream;

        if (Mem)
            TextureMemory[EMemIBs] += (float)Size/(1024.f*1024.f);
    } 
    else 
    {
        buf->Type=PSPBuffer::EVertexStream;

        if (Mem)
            TextureMemory[EMemVBs] += (float)Size/(1024.f*1024.f);
    }

    buf->Ptr        = Mem;
    buf->Format     = format;
    buf->ElemCount  = count;
    buf->Stride     = stride;
    buf->SourceData = sourceData;

#ifdef TRANSGAMING_LOGGING
    //TRACE("VideoSystemPSP::CreateStream(): new stream ID is %d {ptr = %p, source = %p}\n", index, Mem, sourceData);
#endif
    return index;
}


BYTE* VideoSystemPSP::LockStream(int streamId, int pos, int count)
{
    const PSPBuffer& buf = Buffers[streamId];
    // only dynamic buffers can be locked
    if (!buf.SourceData) {
        if (!pos) {
            // We're about to overwrite the vertex buffer,
            // so we need to synchronize with the GE somehow,
            // to ensure we're not overwriting data the GE isn't
            // done with. We could probably use GE signals
            // to track the progress of the GE, so that we
            // can check whether to
            // 1) overwrite existing memory, or
            // 2) allocate more vbuffer memory, or
            // 3) wait for the GE to complete

            // For now, we'll take the easy way out.
            if (in_render || has_render) {
                FinishList(true);
                StartList();
            }
        }
        // Do we want to return an uncached pointer?
        BYTE* Ptr = (BYTE*)buf.Ptr + pos*buf.Stride;
#ifdef STREAM_UNCACHED
        return (BYTE*)sceKernelMakeUncachedAddr(Ptr);
#else
        return Ptr;
#endif
    }

    return 0;
}

void VideoSystemPSP::UnlockStream(int streamId)
{
    const PSPBuffer& buf = Buffers[streamId];
    // only dynamic buffers can be locked
    if (!buf.SourceData) {
#ifndef STREAM_UNCACHED
        // If LockStream returned a cached pointer,
        // we need to flush the data written to memory,
        // so that the GE will see the updated data.
        // FIXME: use WritebackRange instead?
        sceKernelDcacheWritebackAll();
#endif
    }
}


int VideoSystemPSP::CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount)
{
    if (type == EShaderPixel) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CreateShader(): ERROR-> attempted to create a pixel shader\n");
#endif
        return EInvalidIndex;
    }

    if (type == EShaderVertex) {
        if (!source) {
            if (listCount == 1) {
                int         index = EInvalidIndex;
                const DWORD streamType = streamTypeList[0];
                int         format = 0;
                int         stride = 0;
                bool        clip = false;
                bool        useMultitexture = false;


                if (streamType & EVertexPos) {
                    format |= SCEGU_VERTEX_FLOAT;
                    stride += sizeof(float) * 3;
                }
                if (streamType & EVertexDiffuse) {
                    format |= SCEGU_COLOR_PF8888;
                    stride += 4;
                }
                if (streamType & EVertexNormal) {
                    format |= SCEGU_NORMAL_FLOAT;
                    stride += sizeof(float) * 3;
                }
                if (streamType & ETexCountMask) {
                    int texCount = (streamType & ETexCountMask)>>ETexCountShift;
                    int texType =  streamType & ETexTypeMask;

                    
                    //TRACE("VideoSystemPSP::CreateShader(): texType = %d, streamType = 0x%08X, ETexTypeMask = 0x%08X\n", texType, streamType, ETexTypeMask);

                    if (texCount > 1) {
                        //TRACE("VideoSystemPSP::CreateShader(): WARNING-> attempted to create vertex format with %d texture coordinates\n", texCount);
                        // we're going to clip vertices that has multiple texture coordinates;
                        // we'll split the vertex into streams with one texture coordinate each
                        // as part of the clipping process.
                        clip = true;
#ifdef IGNORE_COLOR
                        format &= ~SCEGU_COLOR_PF8888;
                        stride -= 4;
#endif
                        TRACE("*** VideoSystemPSP::CreateShader(): creating a clipping shader\n");
//                        return EInvalidIndex;
                    }

                    switch (texType){
                        case EVertexTUFloat:
                            //TRACE("VideoSystemPSP::CreateShader(): adding floating point tex coords\n");
                            format |= SCEGU_TEXTURE_FLOAT;
                            stride += sizeof(float) * 2;
                            break;

                        case EVertexTUUByte:
                            //TRACE("VideoSystemPSP::CreateShader(): adding unsigned byte tex coords\n");
                            format |= SCEGU_TEXTURE_UBYTE;
                            stride += sizeof(unsigned char) * 2;
                            break;

                        case EVertexTUUShort:
                            //TRACE("VideoSystemPSP::CreateShader(): adding unsigned short tex coords\n");
                            format |= SCEGU_TEXTURE_USHORT;
                            stride += sizeof(unsigned short) * 2;
                            break;

                        //disable the texture coordinates in this shader!
                        case EVertexTUNone:
                            //TRACE("VideoSystemPSP::CreateShader(): not adding any tex coords\n");
                            break;

                        default:
                            Assert(!"Unknown texture type");
                            break;
                    }
                }

                //bone weights are in the vertex stream
                if (streamType & EBoneTypeMask){
                    int     boneCount = (streamType & EBoneCountMask) >> EBoneCountShift;
                    int     boneType = streamType & EBoneTypeMask;


                    //TRACE("VideoSystemPSP::CreateShader(): adding %d bone weights {streamType = 0x%08X, boneType = %d}\n", boneCount + 1, streamType, boneType);

                    //up to 8 bone weights are supported in one vertex
                    if (boneType != EVertexBoneNone && boneCount >= 0 && boneCount < 8){
                        format |= (SCEGU_WEIGHT_1 + boneCount);

                        switch (boneType){
                            case EVertexBoneFloat:
                                //TRACE("VideoSystemPSP::CreateShader(): adding floating point bone weights\n");
                                format |= SCEGU_WEIGHT_FLOAT;
                                stride += (boneCount + 1) * sizeof(float);
                                break;

                            case EVertexBoneUByte:
                                //TRACE("VideoSystemPSP::CreateShader(): adding unsigned byte bone weights\n");
                                format |= SCEGU_WEIGHT_UBYTE;
                                stride += (boneCount + 1) * sizeof(unsigned char);
                                break;

                            case EVertexBoneUShort:
                                //TRACE("VideoSystemPSP::CreateShader(): adding unsigned short bone weights\n");
                                format |= SCEGU_WEIGHT_USHORT;
                                stride += (boneCount + 1) * sizeof(unsigned short);
                                break;

                            default:
                                Assert(!"Unknown Bone Type");
                                break;
                        }
                    }
                }

                if (streamType & EVertexMultitexture)
                    useMultitexture = true;

                if (streamType & EVertexForceClip)
                    clip = true;


                if (format) {
#ifdef TRANSGAMING
                    index = Shaders.GrowArray(1);

                    PSPShader* shader = &Shaders[index];
#else
                    index = Shaders.Size();
                    PSPShader* shader = Shaders.CreateArray();
#endif
                    shader->Type =      EShaderVertex;
                    shader->Format =    format;
                    shader->Stride =    stride;
                    shader->Clip =      clip;
                    shader->MT =        useMultitexture;
                    TRACE("VideoSystemPSP::CreateShader(): created a shader with a stride of %d bytes and a format of 0x%08X {clip = %d, multitexture = %d}\n", stride, format, clip, useMultitexture);
                }
                return index;
            } else {
#ifdef TRANSGAMING_LOGGING
                TRACE("VideoSystemPSP::CreateShader(): ERROR-> attempted to create multistream vertex shader!\n");
#endif
                return EInvalidIndex;
            }
        }
        else {
#ifdef TRANSGAMING_LOGGING
            TRACE("VideoSystemPSP::CreateShader(): ERROR-> attempted to create a programmable vertex shader\n");
#endif
            return EInvalidIndex;
        }
    }

    return EInvalidIndex;
}

void VideoSystemPSP::SetState(const RenderState& state)
{
    static const int CompareFuncMap[] = 
    {
        SCEGU_NEVER,           // EFuncNever
        SCEGU_LESS,            // EFuncLess
        SCEGU_EQUAL,           // EFuncEqual
        SCEGU_LEQUAL,          // EFuncLessEqual
        SCEGU_GREATER,         // EFuncGreater
        SCEGU_NOTEQUAL,        // EFuncNotEqual
        SCEGU_GEQUAL,          // EFuncGreaterEqual
        SCEGU_ALWAYS           // EFuncAlways
    };

// reverse mapping, since we use a reversed depth range
    static const int DepthFuncMap[] = 
    {
        SCEGU_NEVER,           // EFuncNever
        SCEGU_GREATER,         // EFuncLess
        SCEGU_EQUAL,           // EFuncEqual
        SCEGU_GEQUAL,          // EFuncLessEqual
        SCEGU_LESS,            // EFuncGreater
        SCEGU_NOTEQUAL,        // EFuncNotEqual
        SCEGU_LEQUAL,          // EFuncGreaterEqual
        SCEGU_ALWAYS           // EFuncAlways
    };

// FIXME: I'm unsure whether SCEGU_INCR/DECR is Sat or Wrap.
// But since there's only one type available, I probably have
// to use it for both types anyway, though.
    static const int StencilOpMap[] =
    {
        SCEGU_KEEP,            // EStencilOpKeep      = 0,
        SCEGU_ZERO,            // EStencilOpZero      = 1,
        SCEGU_REPLACE,         // EStencilOpReplace   = 2,
        SCEGU_INCR,            // EStencilOpIncrSat   = 3,
        SCEGU_DECR,            // EStencilOpDecrSat   = 4,
        SCEGU_INVERT,          // EStencilOpInvert    = 5,
        SCEGU_INCR,            // EStencilOpIncr      = 6,
        SCEGU_DECR             // EStencilOpDecr      = 7,
    };



    if (CachedState.DepthFunc!=state.DepthFunc)
    {
        sceGuDepthFunc(DepthFuncMap[state.DepthFunc]);
        CachedState.DepthFunc=state.DepthFunc;
    }

    if(CachedState.DepthTest!=state.DepthTest)
    {
        // will be set in ActivateStage
        CachedState.DepthTest=state.DepthTest;
    }

    if(CachedState.DepthWrite!=state.DepthWrite)
    {
        // will be set in ActivateStage
        CachedState.DepthWrite=state.DepthWrite;
    }

    if(CachedState.ColorWrite!=state.ColorWrite || CachedState.AlphaWrite!=state.AlphaWrite)
    {
        const UINT mskColor=0x00ffffff;
        const UINT mskAlpha=0xff000000;
        sceGuPixelMask((state.ColorWrite ? 0 : mskColor) | (state.AlphaWrite ? 0 : mskAlpha));
        CachedState.ColorWrite=state.ColorWrite;
        CachedState.AlphaWrite=state.AlphaWrite;
    }

    // stencil buffer/function related
    if(CachedState.StencilTest!=state.StencilTest)
    {
        (state.StencilTest ? sceGuEnable : sceGuDisable)(SCEGU_STENCIL_TEST);
        if (state.StencilTest)
            StencilSurfaceTouched = TRUE;
        CachedState.StencilTest=state.StencilTest;
    }

    if(CachedState.StencilFunc!=state.StencilFunc || CachedState.StencilRef!=state.StencilRef ||
       CachedState.StencilMask!=state.StencilMask)
    {
// FIXME: we should beware conflicts with alpha, since the framebuffer's alpha
// channel is apparently used for saving the stencil value
        sceGuStencilFunc(CompareFuncMap[state.StencilFunc], state.StencilRef, state.StencilMask);
        CachedState.StencilFunc=state.StencilFunc;
        CachedState.StencilRef=state.StencilRef;
        CachedState.StencilMask=state.StencilMask;
    }

    if(CachedState.StencilFailOp!=state.StencilFailOp || CachedState.StencilZFailOp!=state.StencilZFailOp ||
       CachedState.StencilPassOp!=state.StencilPassOp)
    {
        sceGuStencilOp(StencilOpMap[state.StencilFailOp], StencilOpMap[state.StencilZFailOp],
                       StencilOpMap[state.StencilPassOp]);
        CachedState.StencilFailOp=state.StencilFailOp;
        CachedState.StencilZFailOp=state.StencilZFailOp;
        CachedState.StencilPassOp=state.StencilPassOp;
    }

    // alpha test
    if(CachedState.AlphaTest!=state.AlphaTest)
    {
        (state.AlphaTest ? sceGuEnable : sceGuDisable)(SCEGU_ALPHA_TEST);
        CachedState.AlphaTest=state.AlphaTest;
    }

    if(CachedState.AlphaFunc!=state.AlphaFunc || CachedState.AlphaRef!=state.AlphaRef)
    {
        sceGuAlphaFunc(CompareFuncMap[state.AlphaFunc], state.AlphaRef, 0xff);
        CachedState.AlphaFunc=state.AlphaFunc;
        CachedState.AlphaRef=state.AlphaRef;
    }

    // scissor test
    if (CachedState.ScissorTest!=state.ScissorTest)
    {
        (state.ScissorTest ? sceGuEnable : sceGuDisable)(SCEGU_SCISSOR_TEST);
        CachedState.ScissorTest = state.ScissorTest;
    }

    // blend modes (inflict fogColor!)
    DWORD fogColor = CachedState.FogColor;

    switch(state.BlendMode)
    {
    case RenderState::EBlendNone:              fogColor = SavedFogColor;   break;
    case RenderState::EBlendAlpha:             fogColor = 0;               break;
    case RenderState::EBlendAdd:               fogColor = 0;               break;
    case RenderState::EBlendTranslucent:       fogColor = SavedFogColor;   break;
    case RenderState::EBlendModulate:          fogColor = (DWORD) -1;      break;
    case RenderState::EBlendModulate2x:        fogColor = (DWORD) -1;      break;
    case RenderState::EBlendFilter:            fogColor = (DWORD) -1;      break;
    case RenderState::EBlendInvModulate:       fogColor = 0;               break;
    case RenderState::EBlendSubtract:          fogColor = 0;               break;
    case RenderState::EBlendRevSubtract:       fogColor = 0;               break;
    case RenderState::EBlendDestTranslucent:   fogColor = SavedFogColor;   break;
    case RenderState::EBlendDestAlpha:         fogColor = 0;               break;    
    }

    if(CachedState.FogColor!=fogColor)
    {
        sceGuFog(FogStart, FogEnd, fogColor);
        CachedState.FogColor=fogColor;
    }

    if(CachedState.BlendMode!=state.BlendMode) 
    {
        // will be set in ActivateStage
        CachedState.BlendMode=state.BlendMode;
    }

    // ignore state.Lighting, it's always false
    // ignore state.AmbientColor, it's always zero

    if(CachedState.NormalizeNormals!=state.NormalizeNormals)
    {
        // not possible
        CachedState.NormalizeNormals=state.NormalizeNormals;
    }

    if(CachedState.SpecularEnable!=state.SpecularEnable)
    {
        // we could change the mode of every loaded light,
        // but I'm not doing so right now, since I'm not
        // sure if anything actually needs this
        CachedState.SpecularEnable=state.SpecularEnable;
    }

    if(CachedState.FillMode!=state.FillMode)
    {
        // not handled
        CachedState.FillMode=state.FillMode;
    }

    if (CachedState.CullMode!=state.CullMode)
    {
        int CullMode = state.CullMode;
        if (CullFlip && CullMode != RenderState::ECullNone)        
            CullMode = (CullMode == RenderState::ECullCCW) ? RenderState::ECullCW : RenderState::ECullCCW;

        switch(CullMode)
        {
        case RenderState::ECullNone:
            sceGuDisable(SCEGU_CULL_FACE);
            break;

        case RenderState::ECullCW:
#if defined(TRANSGAMING_MASTERING) || !defined(DEBUG)
            //EVIL HACK!! disable backface culling since we have some objects with incorrect normals at the moment
            sceGuDisable(SCEGU_CULL_FACE);
#else
            sceGuEnable(SCEGU_CULL_FACE);
#endif
            sceGuFrontFace(SCEGU_CCW);
            break;

        case RenderState::ECullCCW:
#if defined(TRANSGAMING_MASTERING) || !defined(DEBUG)
            //EVIL HACK!! disable backface culling since we have some objects with incorrect normals at the moment
            sceGuDisable(SCEGU_CULL_FACE);
#else
            sceGuEnable(SCEGU_CULL_FACE);
#endif
            sceGuFrontFace(SCEGU_CW);
            break;
        }

        CachedState.CullMode = state.CullMode;
    }

    if(CachedState.ConstColor!=state.ConstColor)
    {
        // The tex env color is not very useful for us
        CachedState.ConstColor=state.ConstColor;
    }


}

void VideoSystemPSP::SetObjectTransform(const Matrix* matPtr)
{
    memcpy(&mat_obj, _PSPMAT(*matPtr), sizeof(mat_obj));
    mat_dirty = true;
    sceGuSetMatrix(SCEGU_MATRIX_WORLD, &mat_obj);
}

void VideoSystemPSP::SetCameraTransform(const Matrix* matPtr)
{
    memcpy(&mat_cam, _PSPMAT(*matPtr), sizeof(mat_cam));
    mat_dirty = true;
    sceGuSetMatrix(SCEGU_MATRIX_VIEW, &mat_cam);
}

void VideoSystemPSP::SetScreenTransform(const Matrix* matPtr)
{
    memcpy(&mat_proj, _PSPMAT(*matPtr), sizeof(mat_proj));
    // sceGuDepthRange sets up a transform that expects the clipspace
    // Z to be in the -1 to 1 range, but D3D projection gives a 0 to 1 range.
    // It would seem like we should change the matrix slightly, so that
    // we can use the full 16-bit depth range, but I have not had much
    // success, for some reason; any such attempt cause missing geometry.
#if 0
    mat_proj.x.z = mat_proj.x.z * 1.0f;
    mat_proj.y.z = mat_proj.y.z * 1.0f;
    mat_proj.z.z = mat_proj.z.z * 1.0f;
    mat_proj.w.z = mat_proj.w.z * 1.0f;
#endif
    mat_dirty = true;

    sceGuSetMatrix(SCEGU_MATRIX_PROJECTION, &mat_proj);
    // We could check whether to use W fog or Z fog here,
    // but we're going to assume W fog is in use, which is
    // similar to the type of vertex fog that the PSP presumably use.
    // (Unfortunately, I haven't quite figured it out yet.)
    // W fog values can be converted into Z fog by using
    // A22 and A32 of the projection matrix:
    // result = (val * A22 + A32) / val
}

void VideoSystemPSP::SetBoneMatrix(int index, const Matrix *matrix){
    if (matrix == NULL)
        sceGuBoneMatrix(index, &idmat);

    else{
        //NOTE: this cast is awful to do because the sony docs don't
        //  tell us whether the matricies are expected in row or column
        //  major format.  Column major is the usual format, and the
        //  Matrix class is column major, so for now we're just going
        //  to cast between classes directly.
        sceGuBoneMatrix(index, reinterpret_cast<const ScePspFMatrix4 *>(matrix));
    }
}


void VideoSystemPSP::SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset)
{
    //the streamOffset parameter seems to be obsolete now.  Its source never seems to be assigned
    //a value in used code any more.
    Assert(streamOffset == 0);


    int index;
    for (index=0; index<streamCount; ++index)
    {
/*#if defined(TRANSGAMING) && defined(DEBUG)
        if (streams[index] < 0){
            //TRACE("VideoSystemPSP::SetStreams(): stream index is negative! (index = %d, streams[index] = %d, streamCount = %d, indexStream = %d, streamOffset = %d)\n", index, streams[index], streamCount, indexStream, streamOffset);
            //HALT();
        }

        if (streams[(index + 1) % streamCount] < 0){
            //TRACE("VideoSystemPSP::SetStreams(): next stream index is negative! (index = %d (%d), streams[index + 1] = %d, streamCount = %d, indexStream = %d, streamOffset = %d)\n", (index + 1) % streamCount, index, streams[(index + 1) % streamCount], streamCount, indexStream, streamOffset);
            //HALT();
        }
#endif//*/

        const PSPBuffer& buf=Buffers[streams[index]];
//#ifndef TRANSGAMING
        Assert(buf.ElemCount==Buffers[streams[(index+1)%streamCount]].ElemCount);
//#endif

        this->streams[index].Buf = &buf;
        this->streams[index].Offset=streamOffset;
    }
#if 0
    for (; index<16; ++index)
    {
        this->streams[index].Buf = NULL;
        this->streams[index].Offset=0;
    }
#endif

    if (indexStream!=EInvalidIndex)
        ibuffer=&Buffers[indexStream];

    else
        ibuffer=NULL;
}

void VideoSystemPSP::SetShaderMemory(DWORD addr, const void* data, int count)
{
}

void VideoSystemPSP::SetPixelShaderMemory(DWORD addr, const void* data, int count)
{
}

void VideoSystemPSP::SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count)
{
}

void VideoSystemPSP::SetShader(int shaderIndex, int shaderType)
{
    switch (shaderType){

        case EShaderVertex:
        //case EShaderVertex0:
            //clear out all the other shaders before setting this one
            vshader[1] = vshader[2] = vshader[3] = vshader[4] = vshader[5] = vshader[6] = vshader[7] = NULL;

        case EShaderVertex1:
        case EShaderVertex2:
        case EShaderVertex3:
        case EShaderVertex4:
        case EShaderVertex5:
        case EShaderVertex6:
        case EShaderVertex7:
            if (shaderIndex >= 0 && shaderIndex < Shaders.Size()){
                vshader[shaderType - EShaderVertexBase] = &Shaders[shaderIndex];

                //make sure it's a vertex shader and that the vertex stride is the same for all 
                //texture stages (it will be operating on the same vertex data
                Assert(Shaders[shaderIndex].Type == EShaderVertex);
                Assert(vshader[shaderType - EShaderVertexBase]->Stride == vshader[0]->Stride);
            }
            else
                vshader[shaderType - EShaderVertexBase] = NULL;

            break;

        case EShaderPixel:
        default:
#ifdef TRANSGAMING_LOGGING
            if (shaderIndex != EInvalidIndex)
                TRACE("VideoSystemPSP::SetShader(): ERROR-> set unsupported shader type (index=%d, type=%d)\n", shaderIndex, shaderType);
#endif
            break;
    }
}

void VideoSystemPSP::SetTextureState(const TextureState* stages, int stageCount)
{
    if (!stages)
        return;


    for (int i=0; i<stageCount; i++) {
        const TextureState& t = stages[i];

        if (t.Stage > 0 && t.Op && t.Op != TextureState::ETexOpDisable) {
        }

        if (t.TexId && CachedTextureStates[t.Stage].TexId!=t.TexId) {
            // set in ActivateStage
            CachedTextureStates[t.Stage].TexId=t.TexId;
        }

        if (t.QVal && t.QVal != CachedTextureStates[t.Stage].QVal) {
            // compare first part of tex state settings
            if (t.V1 && t.V1 != CachedTextureStates[t.Stage].V1) {
                if(t.Op && t.Op!=CachedTextureStates[t.Stage].Op) {
                    CachedTextureStates[t.Stage].Op=t.Op;
                }

                // tex arg
                if(t.Arg && t.Arg!=CachedTextureStates[t.Stage].Arg) {
                    CachedTextureStates[t.Stage].Arg=t.Arg;
                }

                // alpha op
                if(t.AlphaOp && t.AlphaOp!=CachedTextureStates[t.Stage].AlphaOp) {
                    CachedTextureStates[t.Stage].AlphaOp=t.AlphaOp;
                }

                // alpha arg
                if(t.AlphaArg && t.AlphaArg!=CachedTextureStates[t.Stage].AlphaArg) {
                    CachedTextureStates[t.Stage].AlphaArg=t.AlphaArg;
                }
            }

            // compare second part of tex state settings
            if (t.V2 && t.V2 != CachedTextureStates[t.Stage].V2) {
                // filter state
                if(t.Filter && t.Filter!=CachedTextureStates[t.Stage].Filter) {
                    CachedTextureStates[t.Stage].Filter=t.Filter;
                }

                // addr mode
                if (t.AddrMode && t.AddrMode!=CachedTextureStates[t.Stage].AddrMode) {
                    CachedTextureStates[t.Stage].AddrMode=t.AddrMode;
                }


                if (t.Transform!=CachedTextureStates[t.Stage].Transform) {
                    CachedTextureStates[t.Stage].Transform = t.Transform;
                }

                if (t.CoordIndex && t.CoordIndex!=CachedTextureStates[t.Stage].CoordIndex) {
                    CachedTextureStates[t.Stage].CoordIndex = t.CoordIndex;
                }
            }
        }

        if ((t.Transform & 0xf) >= TextureState::ETexTransformNone)
        {
            CachedTextureStates[t.Stage].MatStore = t.MatStore;
            CachedTextureStates[t.Stage].Xform = t.Xform;
        }

#if 0
            if(t.BumpPtr) {
                d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT00, F2DW(t.BumpPtr->m4x4[0][0]));
                d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT01, F2DW(t.BumpPtr->m4x4[0][1]));
                d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT10, F2DW(t.BumpPtr->m4x4[1][0]));
                d3dev->SetTextureStageState(t.Stage, D3DTSS_BUMPENVMAT11, F2DW(t.BumpPtr->m4x4[1][1]));
            }
#endif

    }
}

void VideoSystemPSP::ResetTextureStages()
{
}

void VideoSystemPSP::SetFog(bool isFog)
{
    FogEnable = isFog;
    if (FogMode) {
        (FogEnable ? sceGuEnable : sceGuDisable)(SCEGU_FOG);
    }
}

void VideoSystemPSP::SetFogState(const FogState& state)
{
    // only linear fog is supported, but that's what they use
    FogMode = state.Mode;
    FogStart = state.Start;
    FogEnd = state.End;
    CachedState.FogColor = state.Color;
    RecalcFog();
    if (FogEnable) {
        (FogMode ? sceGuEnable : sceGuDisable)(SCEGU_FOG);
    }
    SavedFogColor = state.Color;
}

void VideoSystemPSP::RecalcFog()
{
    sceGuFog(FogStart, FogEnd, CachedState.FogColor);
}

void VideoSystemPSP::SetLight(int number, const LightState& state)
{
    if (number >= 4) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::SetLight(): ERROR-> setting more than 4 lights is not supported!\n");
#endif
        return;
    }

    if (state.Flags==LightState::ELightOff) {
        if (LightEnable[number]) {
            sceGuDisable(SCEGU_LIGHT0+number);
            LightEnable[number] = false;
        }
        return;
    }

    sceGuLightColor(number, SCEGU_DIFFUSE, RenderColor(&state.Diffuse.X));
    sceGuLightColor(number, SCEGU_SPECULAR, RenderColor(&state.Specular.X));
    sceGuLightColor(number, SCEGU_AMBIENT, RenderColor(&state.Ambient.X));

    const float r   = state.Radius;
    const float att = 16.0f/256.0f;
    const float d   = r*0.2f;

    const float A2 = (state.Flags == LightState::ELightDirectional) ? 0.0f : ((1/(att*r) - 1/d) / (r-d));
    const float A1 = (state.Flags == LightState::ELightDirectional) ? 0.0f : (1/d - A2*d);

    sceGuLightAtt(number, 0.0f, A1, A2);

    // perhaps CachedState.SpecularEnable could be checked,
    // to choose between SCEGU_DIFFUSE and SCEGU_DIFFUSE_AND_SPECULAR

    switch (state.Flags) 
    {
    case LightState::ELightPoint:
        // we don't have a way to provide state.Radius (range),
        // we have to rely solely on the attenuation
        sceGuLight(number, SCEGU_LIGHT_POINT,
                   SCEGU_DIFFUSE_AND_SPECULAR,
                   (const ScePspFVector3*)&state.Pos.X);
        break;
            
    case LightState::ELightDirectional:
        sceGuLight(number, SCEGU_LIGHT_DIRECTION,
                   SCEGU_DIFFUSE_AND_SPECULAR,
                   (const ScePspFVector3*)&state.Dir.X);
        break;

    default:
        return;
    }

    if (!LightEnable[number]) {
        sceGuEnable(SCEGU_LIGHT0+number);
        LightEnable[number] = true;
    }
}

void VideoSystemPSP::SetSpecular(const Vector& color, const float power)
{
    RenderColor matcol(color.X, color.Y, color.Z);
    if (CachedSpecularMat!=matcol)
    {
        sceGuMaterial(SCEGU_SPECULAR, matcol);
        CachedSpecularMat = matcol;
    }
    if (CachedSpecularPow!=power)
    {
        sceGuSpecular(power);
        CachedSpecularPow = power;
    }
}

void VideoSystemPSP::SetLighting(bool isLighting)
{
    // set in ActivateStage
    Lighting = isLighting;
}

void VideoSystemPSP::SetAmbientLight(const Vector& color)
{
    RenderColor AmbientColor(color.X, color.Y, color.Z);
    if(CachedState.AmbientColor!=AmbientColor)
    {
        sceGuAmbient(AmbientColor);
        CachedState.AmbientColor=AmbientColor;
    }
}

void VideoSystemPSP::SetNoLight(int number)
{
    if (number >= 4) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::SetNoLight(): ERROR-> setting more than 4 lights is not supported!\n");
#endif
        return;
    }

    if (LightEnable[number]) {
        sceGuDisable(SCEGU_LIGHT0+number);
        LightEnable[number] = false;
    }
}

void VideoSystemPSP::SetDirLight(int number, const Plane& dir, const Vector& color)
{
    if (number >= 4) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::SetDirLight(): ERROR-> setting more than 4 lights is not supported\n");
#endif
        return;
    }

    RenderColor col(color.X, color.Y, color.Z);

    if (!col) {
        if (LightEnable[number]) {
            sceGuDisable(SCEGU_LIGHT0+number);
            LightEnable[number] = false;
        }
        return;
    }

    sceGuLightColor(number, SCEGU_DIFFUSE, col);
    sceGuLightColor(number, SCEGU_SPECULAR, col);
    //sceGuLightColor(number, SCEGU_AMBIENT, 0);

    sceGuLight(number, SCEGU_LIGHT_DIRECTION,
               SCEGU_DIFFUSE_AND_SPECULAR,
               (const ScePspFVector3*)&dir.X);

    if (!LightEnable[number]) {
        sceGuEnable(SCEGU_LIGHT0+number);
        LightEnable[number] = true;
    }
}

void VideoSystemPSP::RecalcMat()
{
    if (!mat_dirty) return;
    // I'm too lazy to use inline asm here, when I can use the vfpu intrinsics
    // (real inefficient when compiled in debug mode though)
    const vfpu_m4x4 m0 = VFPU_SMAT(&mat_obj);
    // but it did take a while to figure out how to get the matrix multiplication right...
    const vfpu_m4x4 m1 = vfpu_vcmmul_q(VFPU_SMAT(&mat_cam), m0);
    const vfpu_m4x4 m2 = vfpu_vcmmul_q(VFPU_SMAT(&mat_proj), m1);
    VFPU_DMAT(&mat) = m2;

    mat_dirty = false;

#if 0
    // non-vfpu matrix code for comparison
    Matrix n0 = Matrix((const float*)&mat_obj);
    // I *think* this should be the right way to multiply these
    Matrix n1 = n0 * Matrix((const float*)&mat_cam);
    Matrix n2 = n1 * Matrix((const float*)&mat_proj);
    // compare!

    Assert(!memcmp(n2, mat, sizeof(n2));
#endif

    clip_dirty = true;
}

void VideoSystemPSP::RecalcClip()
{
    RecalcMat();
    if (!clip_dirty) return;

    // it's probably possible to do these using the VFPU,
    // but I can look into that later

    clip_plane[0].x = mat.x.w + mat.x.x; // +1
    clip_plane[0].y = mat.y.w + mat.y.x;
    clip_plane[0].z = mat.z.w + mat.z.x;
    clip_plane[0].w = mat.w.w + mat.w.x; // +1

    clip_plane[1].x = mat.x.w - mat.x.x; // -1
    clip_plane[1].y = mat.y.w - mat.y.x;
    clip_plane[1].z = mat.z.w - mat.z.x;
    clip_plane[1].w = mat.w.w - mat.w.x; // +1

    clip_plane[2].x = mat.x.w + mat.x.y;
    clip_plane[2].y = mat.y.w + mat.y.y; // +1
    clip_plane[2].z = mat.z.w + mat.z.y;
    clip_plane[2].w = mat.w.w + mat.w.y; // +1

    clip_plane[3].x = mat.x.w - mat.x.y;
    clip_plane[3].y = mat.y.w - mat.y.y; // -1
    clip_plane[3].z = mat.z.w - mat.z.y;
    clip_plane[3].w = mat.w.w - mat.w.y; // +1

    // the near plane is clipped in hardware, so we won't bother with it;
    // as for the far plane, we'll just cull anything there.

    clip_dirty = false;
}

void VideoSystemPSP::AllocList(void)
{
    unsigned l;
    // note, displists must be 64-bit-aligned.
#ifdef VRAM_LIST
    for (l=0; l<LISTS; l++)
        dispblk[l] = vram_alloc(LISTSIZE);
    for (l=0; l<LISTS; l++)
        displist[l] = (char *)vram_lock(dispblk[l]);
#endif
    Assert(clipstream[0] == EInvalidIndex);
    Assert(clipstream[1] == EInvalidIndex);
    clipstream[0] = CreateStream(NULL, EClipStream, CLIPSTREAM_SIZE, sizeof(CLIPSTREAM1));
    TRACE("VideoSystemPSP::AllocList(%d): creating a clip stream for %d verticies (size = %d, stream = %d)\n", __LINE__, CLIPSTREAM_SIZE, sizeof(CLIPSTREAM1), clipstream[0]);
    clipstream[1] = enable_stage_two ? CreateStream(NULL, EClipStream, CLIPSTREAM_SIZE, sizeof(CLIPSTREAM2)) : EInvalidIndex;
    TRACE("VideoSystemPSP::AllocList(%d): creating a clip stream for %d verticies (size = %d, stream = %d)\n", __LINE__, CLIPSTREAM_SIZE, sizeof(CLIPSTREAM2), clipstream[1]);
    clippos = 0;
}

void VideoSystemPSP::FreeList(void){

    //make sure there's a stream to free
    if (clipstream[0] == EInvalidIndex && clipstream[1] == EInvalidIndex)
        return;

    TRACE("VideoSystemPSP::FreeList(): freeing the vertex streams {%d, %d}\n", clipstream[0], clipstream[1]);
    ReleaseResource(EResBuffer, clipstream, 2);
    clipstream[0] = EInvalidIndex;
    clipstream[1] = EInvalidIndex;

#ifdef VRAM_LIST
    unsigned l;

    for (l=0; l<LISTS; l++) {
        vram_unlock(dispblk[l]);
        displist[l] = 0;
    }
    for (l=0; l<LISTS; l++) {
        vram_free(dispblk[l]);
        dispblk[l] = 0;
    }
#endif
}

void VideoSystemPSP::StartList(void)
{
#if 0 // too slow currently
    // if all lists are busy, wait for one to finish
    if (in_render >= LISTS) {
        sceKernelWaitSema(finish_sema, 1, NULL);
        in_render--;
    }
    has_render = 0;
    sceGuStart(SCEGU_LIST_CACHED,
               displist[next_render],
               LISTSIZE);
#else
    SyncList();
    sceGuStart(SCEGU_IMMEDIATE,
               displist[next_render],
               LISTSIZE);
#endif
}

void VideoSystemPSP::FinishList(bool sync)
{
    //printf("finish\n");
    int dispsize = sceGuFinish();
    if (dispsize >= LISTSIZE) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::FinishList(): ERROR-> display list overflowed {size = %d, maxSize = %d}\n", dispsize, LISTSIZE);
#endif
        Assert(0);
        return;
    }
    // FIXME: check if the display list is empty,
    // and skip this stuff if so. I just don't know
    // what the size of an empty list is yet...
    if (dispsize) {
#if 0
        sceKernelDcacheWritebackRange(displist[next_render],
                                      dispsize);
        sceGuSendList(SCEGU_QUEUE_TAIL, displist[next_render],
                      NULL, 0, NULL);
#endif
        in_render++;
        next_render = (next_render+1)%LISTS;
    }
    if (sync) SyncList();
}

void VideoSystemPSP::SyncList(void)
{
    while (in_render) {
        sceKernelWaitSema(finish_sema, 1, NULL);
        in_render--;
    }
}

void VideoSystemPSP::CheckList(void)
{
    int dispsize = sceGuCheckList();
    if (dispsize >= LISTSIZE) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CheckList(): ERROR-> display list overflowed {size = %d, maxSize = %d}\n", dispsize, LISTSIZE);
#endif
        Assert(0);
        return;
    }
    // if the list is almost full, switch to other one
    if ((dispsize+256) >= LISTSIZE) {
        FinishList();
        StartList();
    }
}

void VideoSystemPSP::SignaledList(int id)
{
}

void VideoSystemPSP::FinishedList(int id)
{
    sceKernelSignalSema(finish_sema, 1);
}

void VideoSystemPSP::signal_cb(int id)
{
    VideoSystemPSP::opendev->SignaledList(id);
}

void VideoSystemPSP::finish_cb(int id)
{
    VideoSystemPSP::opendev->FinishedList(id);
}

bool VideoSystemPSP::ActivateStage(int stage)
{
    TextureState &t = CachedTextureStates[stage];

    bool ret;

    if (stage >= 7)
        ret = false; // no more stages
    else
        ret = (CachedTextureStates[stage+1].Op != TextureState::ETexOpDisable);

    BYTE Op = t.Op;
    BYTE Arg = t.Arg;
    BYTE AlphaOp = t.AlphaOp;
    BYTE AlphaArg = t.AlphaArg;

    if (stage == 0) {
        if (Op == TextureState::ETexOpDisable)
            ret = false;
        if (Arg == TextureState::ETexArgPrevious)
            Arg = TextureState::ETexArgDiffuse;
        if (AlphaArg == TextureState::ETexArgPrevious)
            AlphaArg = TextureState::ETexArgDiffuse;
    }


    PSPTexture *tex = ((t.TexId>0 && t.TexId<Textures.Size()) ? &Textures[t.TexId] : NULL);
    bool enable = tex != 0;
    bool cdbl = false;
    bool light = false;
    bool ztest = false;
    bool zwrite = false;
    int func = -1;
    int comp = tex ? tex->Comp : SCEGU_RGB;
    DWORD color = 0xffffffff;
    int blend = RenderState::EBlendNone;

    if (t.TexId != CachedTexId) {
        if (tex) {
#ifdef USE_GIM
            //TRACE("VideoSystemPSP::ActivateStage(): t.TexId = %d, CachedTexId = %d, pic = %p\n", t.TexId, CachedTexId, tex->Pic);
            sceGimPictureActivate(tex->Pic);
#endif
        }
        /*else
            TRACE("VideoSystemPSP::ActivateStage(): ERROR-> no texture specified! {expected %d}\n", t.TexId);//*/

        CachedTexId = t.TexId;
        CachedTexFunc = -1; // force reload of comp
    }

    switch(Op) {
    case TextureState::ETexOpTexture:
        func = SCEGU_TEX_REPLACE;
        break;
    default:
#ifdef TRANSGAMING_LOGGING
        //PSPLog.Print("SetTextureState using unsupported op %d\n", t.Op);
#endif
    case TextureState::ETexOpDisable:
    case TextureState::ETexOpDiffuse:
        enable = false;
        light = Lighting;
        break;
    case TextureState::ETexOpColor:
        color = CachedState.ConstColor;
        enable = false;
        break;
    case TextureState::ETexOpModulate2x:
        cdbl = true;
        // fall through
    case TextureState::ETexOpModulate:
        func = SCEGU_TEX_MODULATE;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = SCEGU_TEX_REPLACE;
            blend = RenderState::EBlendModulate;
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Modulate with previous\n");
#endif
            break;
        case TextureState::ETexArgDiffuse:
            light = Lighting;
            break;
        case TextureState::ETexArgColor:
            color = CachedState.ConstColor;
            break;
        default:
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Modulate with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    case TextureState::ETexOpAdd:
        func = SCEGU_TEX_ADD;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = SCEGU_TEX_REPLACE;
            blend = RenderState::EBlendAdd;
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Add with previous\n");
#endif
            break;
        case TextureState::ETexArgDiffuse:
            light = Lighting;
            break;
        case TextureState::ETexArgColor:
            color = CachedState.ConstColor;
            break;
        default:
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Add with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    case TextureState::ETexOpBlendTexture:
        func = SCEGU_TEX_DECAL;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = SCEGU_TEX_REPLACE;
            blend = RenderState::EBlendTranslucent;
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Blend with previous\n");
#endif
            break;
        case TextureState::ETexArgDiffuse:
            light = Lighting;
            break;
        case TextureState::ETexArgColor:
            color = CachedState.ConstColor;
            break;
        default:
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Blend with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    case TextureState::ETexOpPreviousModulate:
        // FIXME (blend using composite buffer)
#ifdef TRANSGAMING_LOGGING
        //PSPLog.Print("Previous Modulate\n");
#endif
        enable = false;
        blend = RenderState::EBlendModulate;
        switch (Arg) {
        case TextureState::ETexArgTexture:
            enable = true;
            func = SCEGU_TEX_REPLACE;
            blend = RenderState::EBlendNone;
            break;
        case TextureState::ETexArgDiffuse:
            light = Lighting;
            break;
        case TextureState::ETexArgColor:
            color = CachedState.ConstColor;
            break;
        default:
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("Previous Modulate with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    }

    if (!enable_stage_two) {
        ret = false;
    }

    // FIXME: munge blend modes a little more
    ztest = CachedState.DepthTest;
    if (!ret) {
        // last stage
        if (blend != RenderState::EBlendNone) {
#ifdef TRANSGAMING_LOGGING
            /*if (CachedState.BlendMode != RenderState::EBlendNone)
                PSPLog.Print("Blend mode problem\n");//*/
#endif
        } else {
            blend = CachedState.BlendMode;
        }
        zwrite = CachedState.DepthWrite;
    }

    if (enable) {
        if (!CachedTexEnable) {
            sceGuEnable(SCEGU_TEXTURE);
            CachedTexEnable = true;
        }
        if (func != -1 && CachedTexFunc != func) {
            sceGuTexFunc(func, comp);
            CachedTexFunc = func;
        }

        if ((t.Transform & 0xf) >= TextureState::ETexTransformNone ||
            t.CoordIndex >= 0x10)
        {
            int xfrm = SCEGU_PROJECTION_MAP;
            int proj = SCEGU_PROJ_UV;

            if (t.CoordIndex >= 0x10) {
                switch (t.CoordIndex & 0xe0) {
                case TextureState::ETexCoordCameraSpaceNormal: 
                    proj = SCEGU_PROJ_NORMAL;
                    break;

                case TextureState::ETexCoordCameraSpacePos:
                    proj = SCEGU_PROJ_XYZ;
                    break;

                case TextureState::ETexCoordCameraSpaceReflect:
                    // not supported at this time...
                    // perhaps it would be possible to do some spheremap mojo with SCEGU_SHADE_MAP,
                    // but this texgen mode is primarily for cubemaps, not spheremaps.
#ifdef TRANSGAMING_LOGGING
                    //PSPLog.Print("Requested texgen mode (reflection vector) unsupported\n");
#endif
                    proj = SCEGU_PROJ_XYZ;
                    break;
                }
            }

            if ((t.Transform & 0xf) != TextureState::ETexTransformNone) {
                Matrix m = t.MatStore.GetTranspose();

#if 0
                // compensation for unbeliveble behavior of D3D (http://www.flipcode.org/cgi-bin/fcmsg.cgi?thread_show=10662)
                // we _always_ use 2 component tex coords, unless we use texgen
                switch (t.Transform & 0xf) 
                {
                case TextureState::ETexTransform1:
                    m.Y = m.W;
                    break;
                case TextureState::ETexTransform2:
                    m.Z = m.W;
                    break;
                }
#endif


#if 0
                if (t.CoordIndex >= 0x08)
                {
                    Matrix WorldToCamera = GEngine->Video->GetActiveView()->LocalViewport.WorldToCamera;

                    switch (t.CoordIndex&0x18) 
                    {
                    case TextureState::ETexCoordWorldSpace:
                        m = WorldToCamera.Inverse() * m;
                        break;
                    case TextureState::ETexCoordWorldOrient:
                        m = m * WorldToCamera.GetRotation().GetTranspose();
                        break;
                    }
                }
#endif

                ScePspFMatrix4 mat_tex;
                memcpy(&mat_tex, _PSPMAT(m), sizeof(mat_tex));

                // on the PSP, Q is computed from the 3rd column,
                // the fourth column is ignored...
                if (t.Transform & TextureState::ETexTransformProj) {
                    mat_tex.x.z = mat_tex.x.w;
                    mat_tex.y.z = mat_tex.y.w;
                    mat_tex.z.z = mat_tex.z.w;
                    mat_tex.w.z = mat_tex.w.w;
                } else {
                    // force identity Q
                    mat_tex.x.z = 0.0f;
                    mat_tex.y.z = 0.0f;
                    mat_tex.z.z = 0.0f;
                    mat_tex.w.z = 1.0f;
                }

                sceGuSetMatrix(SCEGU_MATRIX_TEXTURE, &mat_tex);
            } else {
                sceGuSetMatrix(SCEGU_MATRIX_TEXTURE, &idmat);
            }

            if (CachedTexXfrm != xfrm) {
                sceGuTexMapMode(xfrm, 0, 0);
                CachedTexXfrm = xfrm;
            }
            if (CachedTexProj != proj) {
                sceGuTexProjMapMode(proj);
                CachedTexProj = proj;
            }
        }
        else if (CachedTexXfrm != SCEGU_UV_MAP) {
            sceGuTexMapMode(SCEGU_UV_MAP, 0, 0);
            CachedTexXfrm = SCEGU_UV_MAP;
        }
    }
    else if (CachedTexEnable) {
        sceGuDisable(SCEGU_TEXTURE);
        CachedTexEnable = false;
    }

    if (CachedCDbl != cdbl) {
        (cdbl ? sceGuEnable : sceGuDisable)(SCEGU_COLOR_DOUBLE);
        CachedCDbl = cdbl;
    }

    if (t.Filter != CachedFilter) {
        switch(t.Filter) {
        case TextureState::ETexFilterPoint:
            sceGuTexFilter(SCEGU_NEAREST, SCEGU_NEAREST);
            break;

        case TextureState::ETexFilterBilinear:
            sceGuTexFilter(SCEGU_LINEAR_MIPMAP_NEAREST, SCEGU_LINEAR);
            break;

        case TextureState::ETexFilterBilinearNoMips:
            sceGuTexFilter(SCEGU_LINEAR, SCEGU_LINEAR);
            break;

        case TextureState::ETexFilterTrilinear:
            sceGuTexFilter(SCEGU_LINEAR_MIPMAP_LINEAR, SCEGU_LINEAR);
            break;

        case TextureState::ETexFilterAnisotropic:
            // doubt anisotropic is really supported
            sceGuTexFilter(SCEGU_LINEAR_MIPMAP_LINEAR, SCEGU_LINEAR);
            break;
        }

        CachedFilter = t.Filter;
    }

    if (t.AddrMode != CachedAddrMode) {
        switch (t.AddrMode) {
        case TextureState::ETexAddrWrap:
            sceGuTexWrap(SCEGU_REPEAT, SCEGU_REPEAT);
            break;

        case TextureState::ETexAddrClamp:
            sceGuTexWrap(SCEGU_REPEAT, SCEGU_CLAMP);
            break;

        case TextureState::ETexAddrMirror:
#ifdef TRANSGAMING_LOGGING
            //PSPLog.Print("texture mirror mode is unsupported\n");
#endif
            sceGuTexWrap(SCEGU_REPEAT, SCEGU_REPEAT);
            break;
        }

        CachedAddrMode = t.AddrMode;
    }

    if (CachedColor != color) {
        sceGuColor(color);
        CachedColor = color;
    }

    if (CachedState.Lighting != light) {
        (light ? sceGuEnable : sceGuDisable)(SCEGU_LIGHTING);
        CachedState.Lighting = light;
    }
#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("VideoSystemPSP::ActivateStage(): stage %d: lighting is %s\n", stage, light ? "ON" : "OFF");
#endif

    if (CachedDepthTest != ztest) {
        (ztest ? sceGuEnable : sceGuDisable)(SCEGU_DEPTH_TEST);
        CachedDepthTest = ztest;
    }

    if (CachedDepthWrite != zwrite) {
        sceGuDepthMask(zwrite ? 0 : 1);
        CachedDepthWrite = zwrite;
    }

    if (CachedBlendMode != blend) {
        switch(blend)
        {
        case RenderState::EBlendNone:
            sceGuDisable(SCEGU_BLEND);
            break;

        case RenderState::EBlendAlpha:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_SRC_ALPHA, SCEGU_FIX_VALUE,
                           0x00000000, 0xffffffff);
            break;

        case RenderState::EBlendAdd:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_FIX_VALUE, SCEGU_FIX_VALUE,
                           0xffffffff, 0xffffffff);
            break;

        case RenderState::EBlendTranslucent:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_SRC_ALPHA, SCEGU_ONE_MINUS_SRC_ALPHA,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendModulate:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_DST_COLOR, SCEGU_FIX_VALUE,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendModulate2x:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_DST_COLOR, SCEGU_SRC_COLOR,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendFilter:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_FIX_VALUE, SCEGU_SRC_COLOR,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendInvModulate:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_FIX_VALUE, SCEGU_ONE_MINUS_SRC_COLOR,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendSubtract:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_SUBTRACT, SCEGU_FIX_VALUE, SCEGU_FIX_VALUE,
                           0xffffffff, 0xffffffff);
            break;

        case RenderState::EBlendRevSubtract:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_REVERSE_SUBTRACT, SCEGU_FIX_VALUE, SCEGU_FIX_VALUE,
                           0xffffffff, 0xffffffff);
            break;

// FIXME: we should beware conflicts with stencil, since the framebuffer's alpha
// channel is apparently used for saving the stencil value
        case RenderState::EBlendDestTranslucent:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_DST_ALPHA, SCEGU_ONE_MINUS_DST_ALPHA,
                           0x00000000, 0x00000000);
            break;

        case RenderState::EBlendDestAlpha:
            sceGuEnable(SCEGU_BLEND);
            sceGuBlendFunc(SCEGU_ADD, SCEGU_DST_ALPHA, SCEGU_FIX_VALUE,
                           0x00000000, 0xffffffff);
            break;
        }

        CachedBlendMode = blend;
    }

    return ret;
}

int VideoSystemPSP::GetStream(int cidx, int base, VertexStreamState* streams, BYTE*& ptr)
{
    const PSPBuffer *Buf = streams[cidx].Buf;
    int Offset = streams[cidx].Offset;
    int stride = Buf->Stride;
    ptr = (BYTE*)(Buf->Ptr ? Buf->Ptr : Buf->SourceData);
    ptr += Offset + base*stride;
    return stride;
}

//void VideoSystemPSP::DrawArray(int prim, int fmt, int base, int count, void* idx, int streamc, VertexStreamState* streams)
void VideoSystemPSP::DrawArray(int prim, int base, int count, void* idx, int streamc, VertexStreamState* streams)
{
    int     stage = 0;
    bool    next;


    Assert(vshader[0]);

    do {
        PSPShader * currShader = vshader[stage] ? vshader[stage] : vshader[0];
        int         cidx = ((CachedTextureStates[stage].CoordIndex&7)-1) & (streamc-1);
        BYTE *      Ptr;
        int         Stride = GetStream(cidx, base, streams, Ptr);
        int         fmt = currShader->Format | (idx ? ibuffer->Format : 0);


#ifdef DEBUG
        if (Stride != currShader->Stride) {
            TRACE("VideoSystemPSP::DrawArray(): ERROR-> stride mismatch during rendering of stage %d! {stride = %d, expected %d, fmt = 0x%08X}\n", stage, Stride, currShader->Stride, fmt);
            return;
        }
#endif

        next = ActivateStage(stage);
        sceGuDrawArray(prim, fmt, count, idx, Ptr);
        stage++;
    } while (next);

#ifdef TRANSGAMING_MESH_WARNINGS
    //TRACE("VideoSystemPSP::DrawArray(): rendered %d texture stages\n", stage);
#endif
}

static inline void calc_clip_flags(const vfpu_v4 comp, int& status) __attribute__((always_inline));
static inline void calc_clip_flags(const vfpu_v4 comp, int& status)
{
    // convert vector to integer
    const vfpu_v4 cint = vfpu_vf2in_q(comp, 24, 0);
    // pack into a single register
    const vfpu_v4 cpack = vfpu_vi2c_q(cint, 0);
    // and grab it (strict-aliasing-safe, I hope)
    const union { float f; int i; } u_status
      = {vfpu_extract_float_from_v4x(cpack)};
    status = u_status.i;
}

static inline void calc_clip_flags_dist(const vfpu_v4 dist, int& status) __attribute__((always_inline));
static inline void calc_clip_flags_dist(const vfpu_v4 dist, int& status)
{
    // comp will contain 1.0 for each clip plane we're on the wrong side of
    const vfpu_v4 comp = vfpu_vslt_q(dist, dist, VFPU_PFX_VT_Q(VFPU_PFX_0, VFPU_PFX_0, VFPU_PFX_0, VFPU_PFX_0));
    // convert that to bits
    calc_clip_flags(comp, status);
}

static inline const vfpu_v4 calc_clip_dist(const vfpu_m4x4 cp, const Vector *ppos, int& status) __attribute__((always_inline));
static inline const vfpu_v4 calc_clip_dist(const vfpu_m4x4 cp, const Vector *ppos, int& status)
{
    // W will not contain anything useful...
    const vfpu_v4 pos = VFPU_SVEC(ppos);
    // fortunately, vhtfm doesn't use it...
    const vfpu_v4 dist = vfpu_vrhtfm4_q(pos, cp);
    // calculate flags
    calc_clip_flags_dist(dist, status);
    // return dist
    return dist;
}

static inline const vfpu_v4 calc_clip_par(const vfpu_v4 pdst, const vfpu_v4 dst) __attribute__((always_inline));
static inline const vfpu_v4 calc_clip_par(const vfpu_v4 pdst, const vfpu_v4 dst)
{
    // the clip plane intersection is where the distance to it is zero,
    // so presumably, we can find the intersection point by interpolating
    // the distances we computed earlier:
    // P = (0.0 - pdist) / (dist - pdist)
    //   = pdist / (pdist - dist)
    const vfpu_v4 ddst = vfpu_vsub_q(pdst, dst, 0);
    // note, this does 4 divisions...
    const vfpu_v4 pval = vfpu_vdiv_q(pdst, ddst);
    // return it
    return pval;
}

static inline const vfpu_v4 calc_clip_pos(const vfpu_v4 pdst, const vfpu_v4 dst, const vfpu_v4 par, int& status) __attribute__((always_inline));
static inline const vfpu_v4 calc_clip_pos(const vfpu_v4 pdst, const vfpu_v4 dst, const vfpu_v4 par, int& status)
{
    // this recomputes "comp" from calc_clip
    // (1.0 for each clip plane we're outside),
    // cheap enough to re-compute
    const vfpu_v4 vc0 = vfpu_vslt_q(pdst, pdst, VFPU_PFX_VT_Q(VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0));
    const vfpu_v4 vc1t = vfpu_vslt_q(dst, dst, VFPU_PFX_VT_Q(VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0));
    const vfpu_v4 vc1 = vfpu_vocp_q(vc1t, 0);
    // vc0 is now 0.0 if ppos is inside, 1.0 if ppos is outside
    // vc1 is now 1.0 if pos is inside, 0.0 if pos is outside

    // combine those with the intersection points
    const vfpu_v4 vp0 = vfpu_vmul_q(vc0, par, 0);
    const vfpu_v4 vp1t = vfpu_vmul_q(vc1t, par, 0);
    const vfpu_v4 vp1 = vfpu_vadd_q(vp1t, vc1, 0);

    // compute the min and max intersection points
    vfpu_v4 vpmt = vfpu_vmax_p(vp0, vp0, VFPU_VS_XY | VFPU_VT_ZW | VFPU_VD_XY);
    vpmt = vfpu_vmin_p_partial(vpmt, vp1, vp1, VFPU_VS_XY | VFPU_VT_ZW | VFPU_VD_ZW);
    vfpu_v4 vpm = vfpu_vmax_s(vpmt, vpmt, VFPU_VS_X | VFPU_VT_Y | VFPU_VD_Z);
    vpm = vfpu_vmin_s_partial(vpm, vpmt, vpmt, VFPU_VS_Z | VFPU_VT_W | VFPU_VD_W);
    vpm = vfpu_vsat0_p_partial(vpm, vpm, VFPU_VS_ZW | VFPU_VD_XY);
    // vpm.X = outside->inside intersection point, or 0.0 if none
    // vpm.Y = inside->outside intersection point, or 1.0 if none

    // find which clip plane we chose for vpm.X
    const vfpu_v4 ccmp0 = vfpu_vscmp_q(par, vpm, VFPU_PFX_VT_Q(VFPU_PFX_Z,VFPU_PFX_Z,VFPU_PFX_Z,VFPU_PFX_Z));
    // comp will contain 1.0 for the clip plane we clipped against
    const vfpu_v4 acmp0 = vfpu_vabs_q(ccmp0, 0);
    const vfpu_v4 ocmp0 = vfpu_vocp_q(acmp0, 0);
    const vfpu_v4 comp0 = vfpu_vmul_q(ocmp0, vc0, 0);
    // convert that to bits
    calc_clip_flags(comp0, status);

    return vpm;
}

static inline const vfpu_v4 calc_clip_pos_ent(const vfpu_v4 pdst, const vfpu_v4 dst, const vfpu_v4 par, int& status) __attribute__((always_inline));
static inline const vfpu_v4 calc_clip_pos_ent(const vfpu_v4 pdst, const vfpu_v4 dst, const vfpu_v4 par, int& status)
{
    // this recomputes "comp" from calc_clip
    // (1.0 for each clip plane we're outside),
    // cheap enough to re-compute
    const vfpu_v4 vc0t = vfpu_vslt_q(pdst, pdst, VFPU_PFX_VT_Q(VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0));
    const vfpu_v4 vc1t = vfpu_vslt_q(dst, dst, VFPU_PFX_VT_Q(VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0,VFPU_PFX_0));
    const vfpu_v4 vc0 = vfpu_vocp_q(vc0t, 0);
    const vfpu_v4 vc1 = vfpu_vocp_q(vc1t, 0);
    // vc0 is now 1.0 if ppos is inside, 0.0 if ppos is outside
    // vc1 is now 1.0 if pos is inside, 0.0 if pos is outside

    // combine those with the intersection points
    const vfpu_v4 vp0t = vfpu_vmul_q(vc0t, par, 0);
    const vfpu_v4 vp1t = vfpu_vmul_q(vc1t, par, 0);
    const vfpu_v4 vp0 = vfpu_vadd_q(vp0t, vc0, 0);
    const vfpu_v4 vp1 = vfpu_vadd_q(vp1t, vc1, 0);

    // compute the min and max intersection points
    vfpu_v4 vpmt = vfpu_vmin_p(vp0, vp0, VFPU_VS_XY | VFPU_VT_ZW | VFPU_VD_XY);
    vpmt = vfpu_vmin_p_partial(vpmt, vp1, vp1, VFPU_VS_XY | VFPU_VT_ZW | VFPU_VD_ZW);
    vfpu_v4 vpm = vfpu_vmin_s(vpmt, vpmt, VFPU_VS_X | VFPU_VT_Y | VFPU_VD_Z);
    vpm = vfpu_vmin_s_partial(vpm, vpmt, vpmt, VFPU_VS_Z | VFPU_VT_W | VFPU_VD_W);
    vpm = vfpu_vsat0_p_partial(vpm, vpm, VFPU_VS_ZW | VFPU_VD_XY);
    // vpm.X = outside->inside intersection point, or 1.0 if none
    // vpm.Y = inside->outside intersection point, or 1.0 if none

    // find which clip plane we chose for outside->inside intersection
    const vfpu_v4 ccmp0 = vfpu_vscmp_q(par, vpm, VFPU_PFX_VT_Q(VFPU_PFX_Z,VFPU_PFX_Z,VFPU_PFX_Z,VFPU_PFX_Z));
    // comp will contain 1.0 for the clip plane we clipped against
    const vfpu_v4 acmp0 = vfpu_vabs_q(ccmp0, 0);
    const vfpu_v4 ocmp0 = vfpu_vocp_q(acmp0, 0);
    const vfpu_v4 comp0 = vfpu_vmul_q(ocmp0, vc0t, 0);
    // convert that to bits
    calc_clip_flags(comp0, status);

    return vpm;
}

static inline const vfpu_v4 clear_clip_dist(const vfpu_v4 dst, const int status, const float v) __attribute__((always_inline));
static inline const vfpu_v4 clear_clip_dist(const vfpu_v4 dst, const int status, const float v)
{
    vfpu_v4 ret = dst;
    if (status & 0x000000ff)
        ret = vfpu_insert_float_into_v4x(ret, v);
    if (status & 0x0000ff00)
        ret = vfpu_insert_float_into_v4y(ret, v);
    if (status & 0x00ff0000)
        ret = vfpu_insert_float_into_v4z(ret, v);
    if (status & 0xff000000)
        ret = vfpu_insert_float_into_v4w(ret, v);
    return ret;
}

static inline const vfpu_v4 mask_clip_dist(const vfpu_v4 dst, const int status, const float v) __attribute__((always_inline));
static inline const vfpu_v4 mask_clip_dist(const vfpu_v4 dst, const int status, const float v)
{
    vfpu_v4 ret = dst;
    if (!(status & 0x000000ff))
        ret = vfpu_insert_float_into_v4x(ret, v);
    if (!(status & 0x0000ff00))
        ret = vfpu_insert_float_into_v4y(ret, v);
    if (!(status & 0x00ff0000))
        ret = vfpu_insert_float_into_v4z(ret, v);
    if (!(status & 0xff000000))
        ret = vfpu_insert_float_into_v4w(ret, v);
    return ret;
}

static inline const vfpu_v4 grab_clip_pos(const vfpu_v4 ppar, const vfpu_v4 npar, const int status) __attribute__((always_inline));
static inline const vfpu_v4 grab_clip_pos(const vfpu_v4 ppar, const vfpu_v4 npar, const int status)
{
    vfpu_v4 tpos;
    if (status & 0x000000ff) {
        tpos = vfpu_vmov_s_partial(tpos, ppar, VFPU_VS_X | VFPU_VD_X);
        tpos = vfpu_vmov_s_partial(tpos, npar, VFPU_VS_X | VFPU_VD_Y);
    }
    else if (status & 0x0000ff00) {
        tpos = vfpu_vmov_s_partial(tpos, ppar, VFPU_VS_Y | VFPU_VD_X);
        tpos = vfpu_vmov_s_partial(tpos, npar, VFPU_VS_Y | VFPU_VD_Y);
    }
    else if (status & 0x00ff0000) {
        tpos = vfpu_vmov_s_partial(tpos, ppar, VFPU_VS_Z | VFPU_VD_X);
        tpos = vfpu_vmov_s_partial(tpos, npar, VFPU_VS_Z | VFPU_VD_Y);
    }
    else if (status & 0xff000000) {
        tpos = vfpu_vmov_s_partial(tpos, ppar, VFPU_VS_W | VFPU_VD_X);
        tpos = vfpu_vmov_s_partial(tpos, npar, VFPU_VS_W | VFPU_VD_Y);
    }
    tpos = vfpu_vsat0_p_partial(tpos, tpos, VFPU_VS_XY | VFPU_VD_ZW);
    return tpos;
}

static inline const vfpu_v4 calc_corner_pos(const vfpu_v4 idst, const vfpu_v4 drdst, const int status) __attribute__((always_inline));
static inline const vfpu_v4 calc_corner_pos(const vfpu_v4 idst, const vfpu_v4 drdst, const int status)
{
    // divide distance from clip planes with the distance to the other side of the triangle
    // idst is negative on input, and we want a positive result,
    // including positive infinity if drdst is zero... so negate idst.
    const vfpu_v4 ndst = vfpu_vneg_q(idst, 0);
    // note, this does 4 divisions...
    const vfpu_v4 vfact = vfpu_vdiv_q(ndst, drdst);
    // the division can produce NaNs, so mask after it, not before
    const vfpu_v4 mfact = mask_clip_dist(vfact, status, -infinityf());
    // get the highest factor, if we're outside more than one clip plane
    const vfpu_v4 factmt = vfpu_vmax_p(mfact, mfact, VFPU_VS_XY | VFPU_VT_ZW | VFPU_VD_XY);
    vfpu_v4 factor = vfpu_vmax_s(factmt, factmt, VFPU_VS_X | VFPU_VT_Y | VFPU_VD_X);
    // clamp it
    factor = vfpu_vsat0_s_partial(factor, factor, VFPU_VS_X | VFPU_VD_Y);
    return factor;
}

#if defined(CACHED_WRITE) && !(defined(IGNORE_COLOR) && defined(VFPU_WRITE))
template <class T>
static inline T* clipstream_ptr(T *out)
{
    return (T*)sceKernelMakeCachedAddr(out);
}

template <class T>
static inline void clipstream_begin(T* out)
{
    unsigned newc = (unsigned)(out) & ~63;
    /*sceKernelDcacheCreateDirtyExclusiveRange(out, (newc+2*64)-(unsigned)(out));*/
    if (newc < (unsigned)(out)) {
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_FILL_D, (void*)newc);
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_CREATE_DIRTY_EXCLUSIVE_D, (void*)(newc+64));
    } else {
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_CREATE_DIRTY_EXCLUSIVE_D, (void*)newc);
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_CREATE_DIRTY_EXCLUSIVE_D, (void*)(newc+64));
    }
}

template <class T>
static inline void clipstream_advance(T*& out)
{
    unsigned oldc = (unsigned)(out) & ~63;
    out++;
    unsigned newc = (unsigned)(out) & ~63;
    if (newc > oldc) {
        /*sceKernelDcacheWritebackInvalidateRange((void*)oldc, newc - oldc);*/
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_HIT_WRITEBACK_INVALIDATE_D, (void*)oldc);
        /*sceKernelDcacheCreateDirtyExclusiveRange((void*)(newc+64), 64);*/
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_CREATE_DIRTY_EXCLUSIVE_D, (void*)(newc+64));
    }
}

template <class T>
static inline void clipstream_end(T* out)
{
    unsigned oldc = (unsigned)(out) & ~63;
    if ((unsigned)(out) & 63) {
        /*sceKernelDcacheWritebackInvalidateRange((void*)oldc, 64);*/
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_HIT_WRITEBACK_INVALIDATE_D, (void*)oldc);
        /*sceKernelDcacheInvalidateRange((void*)(oldc+64), 64);*/
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_HIT_INVALIDATE_D, (void*)(oldc+64));
    } else {
        /*sceKernelDcacheInvalidateRange((void*)oldc, 2*64);*/
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_HIT_INVALIDATE_D, (void*)oldc);
        __builtin_allegrex_cache(SCE_ALLEGREX_CACHEOP_HIT_INVALIDATE_D, (void*)(oldc+64));
    }
}
#else
template <class T>
static inline T* clipstream_ptr(T *out)
{
    return out;
}

template <class T>
static inline void clipstream_begin(T *out)
{
}

template <class T>
static inline void clipstream_advance(T*& out)
{
    out++;
}

template <class T>
static inline void clipstream_end(T* out)
{
}
#endif

static inline void clipstream_lock(VideoSystemPSP *dev, const int clipstream[2],
                                   CLIPSTREAM1*& out0, CLIPSTREAM2*& out1,
                                   int startpos)
{
    out0 = clipstream_ptr((CLIPSTREAM1*)dev->LockStream(clipstream[0], startpos, 0));
    out1 = enable_stage_two ? clipstream_ptr((CLIPSTREAM2*)dev->LockStream(clipstream[1], startpos, 0)) : 0;
    clipstream_begin(out0);
    if (enable_stage_two) clipstream_begin(out1);
}

static inline void clipstream_unlock(VideoSystemPSP *dev, const int clipstream[2],
                                     CLIPSTREAM1*& out0, CLIPSTREAM2*& out1)
{
    VFPU_WB_SYNC()
    clipstream_end(out0);
    if (enable_stage_two) clipstream_end(out1);
    dev->UnlockStream(clipstream[0]);
    if (enable_stage_two) dev->UnlockStream(clipstream[1]);
}

#if 0
static inline void convert_vertex(int &nextpos, CLIPSTREAM1*& out0, CLIPSTREAM2*& out1,
                                  const MESHVERTEX* vtx, const Vector* nrm)
{
#ifdef IGNORE_COLOR
    const vfpu_v4 sv_pos = VFPU_SVEC(&vtx->Pos);
    const vfpu_v4 sv_tu = VFPU_SVEC(&vtx->TU1);
    const vfpu_v4 sv_nrm = vfpu_assign_float_to_v4xyzw(nrm->X, nrm->Y, nrm->Z, 1.0f);
    const vfpu_v4 dv_tu0p = vfpu_vmov_p(sv_tu, VFPU_VS_XY | VFPU_VD_XY);
    const vfpu_v4 dv_tu0 = vfpu_vmov_p_partial(dv_tu0p, sv_nrm, VFPU_VS_XY | VFPU_VD_ZW);
    const vfpu_v4 dv_tu1p = vfpu_vmov_p(sv_tu, VFPU_VS_ZW | VFPU_VD_XY);
    const vfpu_v4 dv_tu1 = vfpu_vmov_p_partial(dv_tu1p, sv_nrm, VFPU_VS_XY | VFPU_VD_ZW);
    const vfpu_v4 dv_posp = vfpu_vmov_s(sv_nrm, VFPU_VS_Z | VFPU_VD_X);
    const vfpu_v4 dv_pos = vfpu_vmov_t_partial(dv_posp, sv_pos, VFPU_VS_XYZ | VFPU_VD_YZW);
    VFPU_WB_WRITE(((char*)out0), dv_tu0);
    VFPU_WB_WRITE(((char*)out0)+16, dv_pos);
    clipstream_advance(out0);
    if (enable_stage_two) { \
        VFPU_WB_WRITE(((char*)out1), dv_tu1);
        VFPU_WB_WRITE(((char*)out1)+16, dv_pos);
        clipstream_advance(out1);
    }
#else
    // FIXME: we should evict source vertices from dcache when
    // we're done with them (though it's not necessarily easy to know
    // when this would be, given that we're using indexed vertices)
    out0->TU.X = vtx->TU1.X;
    out0->TU.Y = vtx->TU1.Y;
    out0->Color = vtx->Color;
    out0->Normal = *nrm;
    out0->Pos = vtx->Pos;
    clipstream_advance(out0);
    if (enable_stage_two) {
        out1->TU.X = vtx->TU2.X;
        out1->TU.Y = vtx->TU2.Y;
        out1->Color = vtx->Color;
        out1->Normal = *nrm;
        out1->Pos = vtx->Pos;
        clipstream_advance(out1);
    }
#endif
    nextpos++;
}
#endif

static inline void convert_vertex(int &nextpos, CLIPSTREAM1*& out0, CLIPSTREAM2*& out1,
                                  const ClipVertex& vtx)
{
#ifdef IGNORE_COLOR
    const vfpu_v4 sv_pos = VFPU_SVEC(&vtx.Pos);
    const vfpu_v4 sv_tu = VFPU_SVEC(&vtx.TU1);
    const vfpu_v4 sv_nrm = VFPU_SVEC(&vtx.Normal);
    const vfpu_v4 dv_tu0p = vfpu_vmov_p(sv_tu, VFPU_VS_XY | VFPU_VD_XY);
    const vfpu_v4 dv_tu0 = vfpu_vmov_p_partial(dv_tu0p, sv_nrm, VFPU_VS_XY | VFPU_VD_ZW);
    const vfpu_v4 dv_tu1p = vfpu_vmov_p(sv_tu, VFPU_VS_ZW | VFPU_VD_XY);
    const vfpu_v4 dv_tu1 = vfpu_vmov_p_partial(dv_tu1p, sv_nrm, VFPU_VS_XY | VFPU_VD_ZW);
    const vfpu_v4 dv_posp = vfpu_vmov_s(sv_nrm, VFPU_VS_Z | VFPU_VD_X);
    const vfpu_v4 dv_pos = vfpu_vmov_t_partial(dv_posp, sv_pos, VFPU_VS_XYZ | VFPU_VD_YZW);
    VFPU_WB_WRITE(((char*)out0), dv_tu0);
    VFPU_WB_WRITE(((char*)out0)+16, dv_pos);
    clipstream_advance(out0);
    if (enable_stage_two) { \
        VFPU_WB_WRITE(((char*)out1), dv_tu1);
        VFPU_WB_WRITE(((char*)out1)+16, dv_pos);
        clipstream_advance(out1);
    }
#else
    out0->TU.X = vtx.TU1.X;
    out0->TU.Y = vtx.TU1.Y;
    out0->Color = vtx.Color;
    out0->Normal = vtx.Normal;
    out0->Pos = vtx.Pos;
    clipstream_advance(out0);
    if (enable_stage_two) {
        out1->TU.X = vtx.TU2.X;
        out1->TU.Y = vtx.TU2.Y;
        out1->Color = vtx.Color;
        out1->Normal = vtx.Normal;
        out1->Pos = vtx.Pos;
        clipstream_advance(out1);
    }
#endif
    nextpos++;
}

static inline void restore_vertex(int &nextpos, CLIPSTREAM1*& out0, CLIPSTREAM2*& out1,
                                  const CLIPSTREAM1* save0, const CLIPSTREAM2* save1)
{
    memcpy(out0, save0, sizeof(*out0));
    clipstream_advance(out0);
    if (enable_stage_two) {
        memcpy(out1, save1, sizeof(*out1));
        clipstream_advance(out1);
    }
    nextpos++;
}

static void clip_triangle(const ClipVertex (&vtx)[3],
                          const vfpu_v4 dist[3], const vfpu_v4 parm[3],
                          const int status[3], ClipVertexArray& out)
{
#ifdef DEBUG
    int process[3];
#define CP_ALL_INSIDE   0x01
#define CP_ALL_OUTSIDE  0x02
#define CP_EXIT_INSIDE  0x04
#define CP_EXIT_OUTSIDE 0x08
#define CP_ENTER_OUTSIDE 0x10
#define CP_ENTER_INSIDE  0x20
#define CP_ENTER_CORNER  0x30
#define CHECKPOINT(x) process[c] |= (x);
#else
#define CHECKPOINT(x)
#endif

// we're not interpolating Color properly yet
// (but it may not matter if the color isn't used)
// also, we're not renormalizing the normal yet

#define DELTA_VERTEX() \
    const vfpu_v4 ddst = vfpu_vsub_q(dst, pdst, 0); \
    const vfpu_v4 ppos = VFPU_SVEC(&pmv->Pos); \
    const vfpu_v4 pos = VFPU_SVEC(&mv->Pos); \
    const vfpu_v4 dpos = vfpu_vsub_q(pos, ppos, 0); \
    const vfpu_v4 pnorm = VFPU_SVEC(&pmv->Normal); \
    const vfpu_v4 norm = VFPU_SVEC(&mv->Normal); \
    const vfpu_v4 dnorm = vfpu_vsub_q(norm, pnorm, 0); \
    const vfpu_v4 ptex = VFPU_SVEC(&pmv->TU1); \
    const vfpu_v4 tex = VFPU_SVEC(&mv->TU1); \
    const vfpu_v4 dtex = vfpu_vsub_q(tex, ptex, 0);

#define OTHER_VERTEX() \
    vfpu_v4 bdst, bpos, bnorm, btex; \
    vfpu_v4 odst, opos, onorm, otex;

#define DELTA_REVERSE_VERTEX() \
    const vfpu_v4 rdst = vfpu_vsub_q(pdst, ndst, 0); \
    const vfpu_v4 npos = VFPU_SVEC(&nmv->Pos); \
    const vfpu_v4 rpos = vfpu_vsub_q(ppos, npos, 0); \
    const vfpu_v4 nnorm = VFPU_SVEC(&nmv->Normal); \
    const vfpu_v4 rnorm = vfpu_vsub_q(pnorm, nnorm, 0); \
    const vfpu_v4 ntex = VFPU_SVEC(&nmv->TU1); \
    const vfpu_v4 rtex = vfpu_vsub_q(ptex, ntex, 0); \
    bdst = ndst; bpos = npos; bnorm = nnorm; btex = ntex; \
    odst = rdst; opos = rpos; onorm = rnorm; otex = rtex;

#define DELTA_FORWARD_VERTEX() \
    const vfpu_v4 fdst = vfpu_vsub_q(ndst, dst, 0); \
    const vfpu_v4 npos = VFPU_SVEC(&nmv->Pos); \
    const vfpu_v4 fpos = vfpu_vsub_q(npos, pos, 0); \
    const vfpu_v4 nnorm = VFPU_SVEC(&nmv->Normal); \
    const vfpu_v4 fnorm = vfpu_vsub_q(nnorm, norm, 0); \
    const vfpu_v4 ntex = VFPU_SVEC(&nmv->TU1); \
    const vfpu_v4 ftex = vfpu_vsub_q(ntex, tex, 0); \
    bdst = dst; bpos = pos; bnorm = norm; btex = tex; \
    odst = fdst; opos = fpos; onorm = fnorm; otex = ftex;

#define INTER_VERTEX(vpm, reg) \
    const vfpu_v4 vdst = vfpu_vscl_q(ddst, vpm, reg); \
    const vfpu_v4 idst = vfpu_vadd_q(pdst, vdst, 0); \
    const vfpu_v4 vtex = vfpu_vscl_q(dtex, vpm, reg); \
    const vfpu_v4 itex = vfpu_vadd_q(ptex, vtex, 0); \
    const vfpu_v4 vnorm = vfpu_vscl_q(dnorm, vpm, reg); \
    const vfpu_v4 inorm = vfpu_vadd_q(pnorm, vnorm, 0); \
    const vfpu_v4 vpos = vfpu_vscl_q(dpos, vpm, reg); \
    const vfpu_v4 ipos = vfpu_vadd_q(ppos, vpos, 0);

#define INTER_SECOND_VERTEX(vpm, reg) \
    const vfpu_v4 vdst0 = vfpu_vscl_q(odst, vpm, reg); \
    const vfpu_v4 idst0 = vfpu_vadd_q(bdst, vdst0, 0); \
    const vfpu_v4 vtex0 = vfpu_vscl_q(otex, vpm, reg); \
    const vfpu_v4 itex0 = vfpu_vadd_q(btex, vtex0, 0); \
    const vfpu_v4 vnorm0 = vfpu_vscl_q(onorm, vpm, reg); \
    const vfpu_v4 inorm0 = vfpu_vadd_q(bnorm, vnorm0, 0); \
    const vfpu_v4 vpos0 = vfpu_vscl_q(opos, vpm, reg); \
    const vfpu_v4 ipos0 = vfpu_vadd_q(bpos, vpos0, 0);

#define DOUBLE_DELTA_VERTEX() \
    const vfpu_v4 ppos0 = ipos; \
    const vfpu_v4 dpos0 = vfpu_vsub_q(ipos0, ppos0, 0); \
    const vfpu_v4 pnorm0 = inorm; \
    const vfpu_v4 dnorm0 = vfpu_vsub_q(inorm0, pnorm0, 0); \
    const vfpu_v4 ptex0 = itex; \
    const vfpu_v4 dtex0 = vfpu_vsub_q(itex0, ptex0, 0);

#define DOUBLE_INTER_VERTEX(vpm, reg) \
    const vfpu_v4 vtex1 = vfpu_vscl_q(dtex0, vpm, reg); \
    const vfpu_v4 itex = vfpu_vadd_q(ptex0, vtex1, 0); \
    const vfpu_v4 vnorm1 = vfpu_vscl_q(dnorm0, vpm, reg); \
    const vfpu_v4 inorm = vfpu_vadd_q(pnorm0, vnorm1, 0); \
    const vfpu_v4 vpos1 = vfpu_vscl_q(dpos0, vpm, reg); \
    const vfpu_v4 ipos = vfpu_vadd_q(ppos0, vpos1, 0);

#define INSERT_VERTEX() \
    ClipVertex outv; \
    VFPU_DVEC(&outv.TU1) = itex; \
    VFPU_DVEC(&outv.Normal) = inorm; \
    VFPU_DVEC(&outv.Pos) = ipos; \
    out.Add(outv);

#define CONVERT_VERTEX() \
    out.Add(*mv);

    for (int c=0; c<3; c++) {
        const int pc = (c==0)?2:(c-1);
        const int nc = (c==2)?0:(c+1);
        const ClipVertex *pmv = &vtx[pc];
        const ClipVertex *mv = &vtx[c];
        const ClipVertex *nmv = &vtx[nc];
        const int pstat = status[pc];
        const int stat = status[c];
        const int nstat = status[nc];

#ifdef DEBUG
        process[c] = 0;
#endif

        if (!(stat | pstat)) {
            // both previous and current vertex inside clip planes
            CONVERT_VERTEX()
            CHECKPOINT(CP_ALL_INSIDE)
            continue;
        }

        if (pstat && ((pstat & stat) == pstat)) {
            // both previous and current vertex outside clip planes
            // (and not moving inside any of them)
            CHECKPOINT(CP_ALL_OUTSIDE)
            continue;
        }

        const vfpu_v4 pdst = dist[pc];
        const vfpu_v4 dst = dist[c];
        const vfpu_v4 ndst = dist[nc];
        const vfpu_v4 ppar = parm[pc];
        const vfpu_v4 par = parm[c];
        const vfpu_v4 npar = parm[nc];

        DELTA_VERTEX()

        // check which clip planes we're staying outside
        int outside = pstat & stat;
        // check which clip planes we're moving inside
        int cross = pstat & ~stat;
        int partial = 0;
        vfpu_v4 cdst = pdst;
        vfpu_v4 vpm = vfpu_v4(0.0f);
        if (cross) {
            // suppress clip planes we're not crossing
            cdst = clear_clip_dist(cdst, outside, 0.0f);
            int cp_shift = 0;

            while (cross) {
                int ent_cstat, ent_dstat;

                // calculate interpolation factor
                vpm = calc_clip_pos_ent(cdst, dst, par, ent_cstat);

                INTER_VERTEX(vpm, VFPU_VT_X)

                calc_clip_flags_dist(idst, ent_dstat);

                // what's in ent_cstat should never be in ent_dstat,
                // but sometimes is due to precision issues.
                ent_dstat &= ~ent_cstat;

#if 0
                // swap "complementary" bit plane bits (left<->right, etc)
                int ent_dstat_l = (ent_dstat & 0xff00ff00) >> 8;
                int ent_dstat_r = (ent_dstat & 0x00ff00ff) << 8;
                int ent_dstat_swp = ent_dstat_l | ent_dstat_r;

                if (ent_cstat & ent_dstat_swp) {
                    // if we have a clip plane conflict,
                    // the current position is most likely behind the camera,
                    // so we don't want to do anything
                }
                else
#endif
                if (ent_dstat) {
                    // calculated vertex is outside clip planes;
                    // this'll become a "corner" vertex,
                    // which we need two vectors to interpolate

                    // find out which of the two other triangle sides crosses
                    const vfpu_v4 tpos = grab_clip_pos(ppar, npar, ent_cstat);

                    // calculate crossing position
                    OTHER_VERTEX()
                    if (vfpu_vcmp_eq_s(tpos, tpos, VFPU_VS_X | VFPU_VT_Z)) {
                        DELTA_REVERSE_VERTEX()
                        vpm = vfpu_vmov_s_partial(vpm, tpos, VFPU_VS_X | VFPU_VD_Z);
                    } else {
                        DELTA_FORWARD_VERTEX()
                        vpm = vfpu_vmov_s_partial(vpm, tpos, VFPU_VS_Y | VFPU_VD_Z);
                    }

                    INTER_SECOND_VERTEX(vpm, VFPU_VT_Z)

                    // delta
                    const vfpu_v4 drdst = vfpu_vsub_q(idst0, idst, 0);

                    // calculate and clamp second interpolation factor
                    const vfpu_v4 factor = calc_corner_pos(idst, drdst, ent_dstat);

                    if (vfpu_vcmp_eq_s(factor, factor, VFPU_VS_X | VFPU_VT_Y)) {
                        DOUBLE_DELTA_VERTEX()
                        DOUBLE_INTER_VERTEX(factor, VFPU_VT_X)

                        // finally, we're done
                        INSERT_VERTEX()
                        CHECKPOINT(CP_ENTER_CORNER << cp_shift)
                    } else {
                        CHECKPOINT(CP_ENTER_OUTSIDE << cp_shift)
                    }
                } else {
                    // on the edge of visibility!
                    INSERT_VERTEX()
                    CHECKPOINT(CP_ENTER_INSIDE << cp_shift)
                }

                if (!outside)
                    partial = vfpu_vcmp_lt_s(vpm, vpm, VFPU_VS_X | VFPU_VT_Y);

                // suppress completed clip planes
                cdst = clear_clip_dist(cdst, ent_cstat, 0.0f);
                cross &= ~ent_cstat;
                cp_shift += 2;
            }
        }
        else if (!outside) {
            int ent_stat;
            vpm = calc_clip_pos_ent(cdst, dst, par, ent_stat);
            partial = (!ent_stat) || vfpu_vcmp_lt_s(vpm, vpm, VFPU_VS_X | VFPU_VT_Y);
        }

        // check whether we might be partially inside
        if (partial) {
            // looks like we were inside, calculate
            if (stat) {
                // we're moving outside again
                INTER_VERTEX(vpm, VFPU_VT_Y)
                INSERT_VERTEX()
                CHECKPOINT(CP_EXIT_OUTSIDE)
            } else {
                // we're inside
                CONVERT_VERTEX()
                CHECKPOINT(CP_EXIT_INSIDE)
            }
        }
    }

#ifdef DEBUG
    if (out.Size() && out.Size() < 3) {
        printf("Clip bug: %d vertices emitted\n", out.Size());
#if 0
        for (int c=0; c<3; c++) {
            const vfpu_v4 cdst = dist[c];
            const vfpu_v4 cpar = parm[c];
            printf("clip%d: {%ff,%ff,%ff,%ff} {%ff,%ff,%ff,%ff} %08x %08x\n", c,
                   vfpu_extract_float_from_v4x(cdst),
                   vfpu_extract_float_from_v4y(cdst),
                   vfpu_extract_float_from_v4z(cdst),
                   vfpu_extract_float_from_v4w(cdst),
                   vfpu_extract_float_from_v4x(cpar),
                   vfpu_extract_float_from_v4y(cpar),
                   vfpu_extract_float_from_v4z(cpar),
                   vfpu_extract_float_from_v4w(cpar),
                   status[c],
                   process[c]);
        }
#endif
    }
#endif
}

const static Vector zeronorm(0.0f, 0.0f, 0.0f);

#define CACHED_WRITE


//void VideoSystemPSP::DrawArrayClipNI(int prim, int fmt, int base, int count, int streamc, VertexStreamState* streams)
void VideoSystemPSP::DrawArrayClipNI(int prim, int base, int count, int streamc, VertexStreamState* streams)
{
    BYTE* VPtr;
    BYTE* NPtr;
    int VStride = GetStream(0, base, streams, VPtr);
    int NStride = GetStream(1, base, streams, NPtr);

    RecalcClip();

    if (!NPtr) {
        // not sure what's up with this... it could be a bug elsewhere,
        // so this workaround may not always work.
        NPtr = (BYTE*)&zeronorm;
        NStride = 0;
    }

    const vfpu_m4x4 m = VFPU_SMAT(&mat);
    // we conveniently have 4 clip planes...
    const vfpu_m4x4 cp = VFPU_SMAT(&clip_plane);

    const vfpu_v4 vz = vfpu_v4(0.0f);

    int id = 0;
    CLIPSTREAM1* out0;
    CLIPSTREAM2* out1;

    VertexStreamState clipstreams[2];
    clipstreams[0].Buf = &Buffers[clipstream[0]];
    clipstreams[1].Buf = enable_stage_two ? &Buffers[clipstream[1]] : 0;

    // We'll be assuming indexed vertices here, that's all we'll get;
    // our output will not be indexed, though.
    //fmt &= ~(SCEGU_INDEX_UBYTE|SCEGU_INDEX_USHORT);

    int startpos = clippos;
    int nextpos = startpos;

#define PREFETCH(vtx) \
    /*sceKernelDcacheFillRange(vtx, sizeof(*(vtx)));*/

#define CHECK_BUFFER(req) \
    if ((CLIPSTREAM_SIZE - nextpos) < (req)) { \
        clipstream_unlock(this, clipstream, out0, out1); \
        DrawArray(prim, startpos, nextpos - startpos, NULL, 2, clipstreams); \
        startpos = 0; \
        nextpos = 0; \
        clipstream_lock(this, clipstream, out0, out1, startpos); \
        REFILL_BUFFER \
    }

    clipstream_lock(this, clipstream, out0, out1, startpos);

    switch (prim) {
    case SCEGU_PRIM_POINTS:
        while (count > 0) {
            const MESHVERTEX *vtx = (const MESHVERTEX*)(VPtr + id * VStride);
            PREFETCH(vtx)
            const Vector *nrm = (const Vector*)(NPtr + id * NStride);
            PREFETCH(nrm)
            ClipVertex cvtx(*vtx, *nrm);
            int stat;
            calc_clip_dist(cp, &vtx->Pos, stat);
#define REFILL_BUFFER
            if (!stat) {
                // vertex not clipped, copy it
                CHECK_BUFFER(1)
                convert_vertex(nextpos, out0, out1, cvtx);
            }
#undef REFILL_BUFFER

            id++;
            count--;
        }
        break;
    case SCEGU_PRIM_TRIANGLES:
        while (count >= 3) {
            const MESHVERTEX *vtx[3];
            const Vector *nrm[3];
            ClipVertex cvtx[3];
            int status[3];
            vfpu_v4 dist[3];
            int all, any;
            vtx[0] = (const MESHVERTEX*)(VPtr + id * VStride);
            PREFETCH(vtx[0])
            vtx[1] = (const MESHVERTEX*)(VPtr + (id + 1) * VStride);
            PREFETCH(vtx[1])
            vtx[2] = (const MESHVERTEX*)(VPtr + (id + 2) * VStride);
            PREFETCH(vtx[2])
            nrm[0] = (const Vector*)(NPtr + id * NStride);
            PREFETCH(nrm[0])
            nrm[1] = (const Vector*)(NPtr + (id + 1) * NStride);
            PREFETCH(nrm[1])
            nrm[2] = (const Vector*)(NPtr + (id + 2) * NStride);
            PREFETCH(nrm[2])

            cvtx[0] = ClipVertex(*vtx[0], *nrm[0]);
            cvtx[1] = ClipVertex(*vtx[1], *nrm[1]);
            cvtx[2] = ClipVertex(*vtx[2], *nrm[2]);

#if 1
            dist[0] = calc_clip_dist(cp, &cvtx[0].Pos, status[0]);
            dist[1] = calc_clip_dist(cp, &cvtx[1].Pos, status[1]);
            dist[2] = calc_clip_dist(cp, &cvtx[2].Pos, status[2]);
            any = status[0] | status[1] | status[2];
            all = status[0] & status[1] & status[2];
#else
            any = 0; // disable clipping
            all = 0;
#endif
#define REFILL_BUFFER
            if (!any) {
                // vertices not clipped, copy them
                CHECK_BUFFER(3)
                convert_vertex(nextpos, out0, out1, cvtx[0]);
                convert_vertex(nextpos, out0, out1, cvtx[1]);
                convert_vertex(nextpos, out0, out1, cvtx[2]);
            }
            else if (!all) {
                // some vertices clipped
                vfpu_v4 parm[3];
                parm[0] = calc_clip_par(dist[2], dist[0]);
                parm[1] = calc_clip_par(dist[0], dist[1]);
                parm[2] = calc_clip_par(dist[1], dist[2]);

                outc.FastClear();
                clip_triangle(cvtx, dist, parm, status, outc);

                if (outc.Size() >= 3) {
                    CHECK_BUFFER(3)
                    convert_vertex(nextpos, out0, out1, outc[0]);
                    convert_vertex(nextpos, out0, out1, outc[1]);
                    convert_vertex(nextpos, out0, out1, outc[2]);
                    // if we get more than 3 clipped vertices, we will arrange for a triangle fan configuration.
                    for (int c=3; c<outc.Size(); c++) {
                        CHECK_BUFFER(3)
                        convert_vertex(nextpos, out0, out1, outc[0]);
                        convert_vertex(nextpos, out0, out1, outc[c-1]);
                        convert_vertex(nextpos, out0, out1, outc[c]);
                    }
                }
            }
#undef REFILL_BUFFER

            id+=3;
            count-=3;
        }
        break;
    // tri-strips can be clipped similarly by inserting degenerate triangles where necessary
    // tri-fans too, but if the first vertex is clipped there, it'd need to be converted into a tri-strip
    case SCEGU_PRIM_TRIANGLE_STRIP: {
        if (count < 3) break;
        unsigned nvo = 0;
        const MESHVERTEX *vtx[3];
        const Vector *nrm[3];
        ClipVertex cvtx[3];
        int status[3];
        vfpu_v4 dist[3];
        CLIPSTREAM1* save0 = 0;
        CLIPSTREAM2* save1 = 0;
        int stitch_c = 1, stitch_vo = -1;

        vtx[0] = (const MESHVERTEX*)(VPtr + id * VStride);
        PREFETCH(vtx[0])
        vtx[1] = (const MESHVERTEX*)(VPtr + (id + 1) * VStride);
        PREFETCH(vtx[1])
        nrm[0] = (const Vector*)(NPtr + id * NStride);
        PREFETCH(nrm[0])
        nrm[1] = (const Vector*)(NPtr + (id + 1) * NStride);
        PREFETCH(nrm[1])

        cvtx[0] = ClipVertex(*vtx[0], *nrm[0]);
        cvtx[1] = ClipVertex(*vtx[1], *nrm[1]);

#if 1
        dist[0] = calc_clip_dist(cp, &cvtx[0].Pos, status[0]);
        dist[1] = calc_clip_dist(cp, &cvtx[1].Pos, status[1]);
#endif

        id+=2;
        count-=2;

        while (count) {
            int all, any;
            vtx[2] = (const MESHVERTEX*)(VPtr + (id) * VStride);
            PREFETCH(vtx[2])
            nrm[2] = (const Vector*)(NPtr + (id) * NStride);
            PREFETCH(nrm[2])

            cvtx[2] = ClipVertex(*vtx[2], *nrm[2]);

#if 1
            dist[2] = calc_clip_dist(cp, &cvtx[2].Pos, status[2]);
            any = status[0] | status[1] | status[2];
            all = status[0] & status[1] & status[2];
#else
            any = 0; // disable clipping
            all = 0;
#endif

            // could check indices instead of the vertex data itself,
            // but we'll probably want it to work with non-indexed data too
            if (cvtx[0] == cvtx[1] ||
                cvtx[0] == cvtx[2] ||
                cvtx[1] == cvtx[2]) {
                // degenerate triangle, do not render it
                any = 0xffffffff;
                all = 0xffffffff;
            }

            if (!any) {
                // vertices not clipped, copy them
                if (stitch_c && stitch_vo != -1) {
#define REFILL_BUFFER stitch_vo = -1;
                    if (nvo == stitch_vo) {
                        CHECK_BUFFER(6)
                        if (stitch_vo == -1) goto wrap_stitch_noclip;
                        restore_vertex(nextpos, out0, out1, save0, save1);
                        restore_vertex(nextpos, out0, out1, save0, save1);
                    } else {
                        CHECK_BUFFER(5)
                        if (stitch_vo == -1) goto wrap_stitch_noclip;
                        restore_vertex(nextpos, out0, out1, save0, save1);
                    }
                    if (nvo) {
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                    } else {
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                    }
                    stitch_c = 0;
#undef REFILL_BUFFER
                }
                else if (stitch_c) {
wrap_stitch_noclip:
#define REFILL_BUFFER
                    if (nvo) {
                        CHECK_BUFFER(4)
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                    } else {
                        CHECK_BUFFER(3)
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                    }
                    stitch_c = 0;
                }
#undef REFILL_BUFFER
                else {
#define REFILL_BUFFER \
        if (nvo) { \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
            convert_vertex(nextpos, out0, out1, cvtx[0]); \
        } else { \
            convert_vertex(nextpos, out0, out1, cvtx[0]); \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
        }
                    CHECK_BUFFER(1)
#undef REFILL_BUFFER
                }
                save0 = out0; save1 = out1;
                convert_vertex(nextpos, out0, out1, cvtx[2]);
                stitch_vo = nvo;
            }
            else if (!all) {
                // some vertices clipped
                vfpu_v4 parm[3];
                parm[0] = calc_clip_par(dist[2], dist[0]);
                parm[1] = calc_clip_par(dist[0], dist[1]);
                parm[2] = calc_clip_par(dist[1], dist[2]);

                outc.FastClear();
                clip_triangle(cvtx, dist, parm, status, outc);

                if (outc.Size() >= 3) {
                    if (stitch_vo != -1) {
#define REFILL_BUFFER stitch_vo = -1;
                        if (!stitch_vo) {
                            CHECK_BUFFER(6)
                            if (stitch_vo == -1) goto wrap_stitch_clip;
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            restore_vertex(nextpos, out0, out1, save0, save1);
                        } else {
                            CHECK_BUFFER(5)
                            if (stitch_vo == -1) goto wrap_stitch_clip;
                            restore_vertex(nextpos, out0, out1, save0, save1);
                        }
                        convert_vertex(nextpos, out0, out1, outc[0]);
#undef REFILL_BUFFER
                    }
                    else {
wrap_stitch_clip:
#define REFILL_BUFFER
                        CHECK_BUFFER(3)
#undef REFILL_BUFFER
                    }

#define REFILL_BUFFER stitch_vo = -1;
                    convert_vertex(nextpos, out0, out1, outc[0]);
                    convert_vertex(nextpos, out0, out1, outc[1]);
                    save0 = out0; save1 = out1;
                    convert_vertex(nextpos, out0, out1, outc[2]);
                    stitch_vo = 0;

                    // if we get more than 3 clipped vertices, we will arrange for a triangle fan configuration.
                    // FIXME: probably possible to turn a fan into a strip with fewer stitches than this
                    for (int c=3; c<outc.Size(); c++) {
                        CHECK_BUFFER(6)
                        if (stitch_vo != -1) {
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            convert_vertex(nextpos, out0, out1, outc[0]);
                        }

                        convert_vertex(nextpos, out0, out1, outc[0]);
                        convert_vertex(nextpos, out0, out1, outc[c-1]);
                        save0 = out0; save1 = out1;
                        convert_vertex(nextpos, out0, out1, outc[c]);
                        stitch_vo = 0;
                    }
                }
#undef REFILL_BUFFER
                stitch_c = 1;
            }
            else {
                stitch_c = 1;
            }

            if (nvo) {
                vtx[1] = vtx[2];
                nrm[1] = nrm[2];
                cvtx[1] = cvtx[2];
                status[1] = status[2];
                dist[1] = dist[2];
                nvo = 0;
            } else {
                vtx[0] = vtx[2];
                nrm[0] = nrm[2];
                cvtx[0] = cvtx[2];
                status[0] = status[2];
                dist[0] = dist[2];
                nvo = 1;
            }
            id++;
            count--;
        }
        break;
    }
    default:
        TRACE("Unhandled primitive type for clipping %d\n", prim);
        Assert(0);
        break;
    }

    clipstream_unlock(this, clipstream, out0, out1);

    if (nextpos > CLIPSTREAM_SIZE) {
        TRACE("Overflowed clip buffer!\n");
        Assert(0);
    }

    DrawArray(prim, startpos, nextpos - startpos, NULL, 2, clipstreams);
    clippos = nextpos;
}


//void VideoSystemPSP::DrawArrayClipI(int prim, int fmt, int base, int count, void* idx, int streamc, VertexStreamState* streams)
void VideoSystemPSP::DrawArrayClipI(int prim, int base, int count, void* idx, int streamc, VertexStreamState* streams)
{
    BYTE* VPtr;
    BYTE* NPtr;
    int VStride = GetStream(0, base, streams, VPtr);
    int NStride = GetStream(1, base, streams, NPtr);

    RecalcClip();

    if (!NPtr) {
        // not sure what's up with this... it could be a bug elsewhere,
        // so this workaround may not always work.
        NPtr = (BYTE*)&zeronorm;
        NStride = 0;
    }

    const vfpu_m4x4 m = VFPU_SMAT(&mat);
    // we conveniently have 4 clip planes...
    const vfpu_m4x4 cp = VFPU_SMAT(&clip_plane);

    const vfpu_v4 vz = vfpu_v4(0.0f);

    WORD* id = (WORD*)idx;
    CLIPSTREAM1* out0;
    CLIPSTREAM2* out1;

    VertexStreamState clipstreams[2];
    clipstreams[0].Buf = &Buffers[clipstream[0]];
    clipstreams[1].Buf = enable_stage_two ? &Buffers[clipstream[1]] : 0;

    // We'll be assuming indexed vertices here, that's all we'll get;
    // our output will not be indexed, though.
    //fmt &= ~(SCEGU_INDEX_UBYTE|SCEGU_INDEX_USHORT);

    int startpos = clippos;
    int nextpos = startpos;

#define PREFETCH(vtx) \
    /*sceKernelDcacheFillRange(vtx, sizeof(*(vtx)));*/

#define CHECK_BUFFER(req) \
    if ((CLIPSTREAM_SIZE - nextpos) < (req)) { \
        clipstream_unlock(this, clipstream, out0, out1); \
        DrawArray(prim, startpos, nextpos - startpos, NULL, 2, clipstreams); \
        startpos = 0; \
        nextpos = 0; \
        clipstream_lock(this, clipstream, out0, out1, startpos); \
        REFILL_BUFFER \
    }

    clipstream_lock(this, clipstream, out0, out1, startpos);

    switch (prim) {
    case SCEGU_PRIM_POINTS:
        while (count > 0) {
            const MESHVERTEX *vtx = (const MESHVERTEX*)(VPtr + *id * VStride);
            PREFETCH(vtx)
            const Vector *nrm = (const Vector*)(NPtr + *id * NStride);
            PREFETCH(nrm)
            ClipVertex cvtx(*vtx, *nrm);
            int stat;
            calc_clip_dist(cp, &vtx->Pos, stat);
#define REFILL_BUFFER
            if (!stat) {
                // vertex not clipped, copy it
                CHECK_BUFFER(1)
                convert_vertex(nextpos, out0, out1, cvtx);
            }
#undef REFILL_BUFFER

            id++;
            count--;
        }
        break;
    case SCEGU_PRIM_TRIANGLES:
        while (count >= 3) {
            const MESHVERTEX *vtx[3];
            const Vector *nrm[3];
            ClipVertex cvtx[3];
            int status[3];
            vfpu_v4 dist[3];
            int all, any;
            vtx[0] = (const MESHVERTEX*)(VPtr + id[0] * VStride);
            PREFETCH(vtx[0])
            vtx[1] = (const MESHVERTEX*)(VPtr + id[1] * VStride);
            PREFETCH(vtx[1])
            vtx[2] = (const MESHVERTEX*)(VPtr + id[2] * VStride);
            PREFETCH(vtx[2])
            nrm[0] = (const Vector*)(NPtr + id[0] * NStride);
            PREFETCH(nrm[0])
            nrm[1] = (const Vector*)(NPtr + id[1] * NStride);
            PREFETCH(nrm[1])
            nrm[2] = (const Vector*)(NPtr + id[2] * NStride);
            PREFETCH(nrm[2])

            cvtx[0] = ClipVertex(*vtx[0], *nrm[0]);
            cvtx[1] = ClipVertex(*vtx[1], *nrm[1]);
            cvtx[2] = ClipVertex(*vtx[2], *nrm[2]);

#if 1
            dist[0] = calc_clip_dist(cp, &cvtx[0].Pos, status[0]);
            dist[1] = calc_clip_dist(cp, &cvtx[1].Pos, status[1]);
            dist[2] = calc_clip_dist(cp, &cvtx[2].Pos, status[2]);
            any = status[0] | status[1] | status[2];
            all = status[0] & status[1] & status[2];
#else
            any = 0; // disable clipping
            all = 0;
#endif
#define REFILL_BUFFER
            if (!any) {
                // vertices not clipped, copy them
                CHECK_BUFFER(3)
                convert_vertex(nextpos, out0, out1, cvtx[0]);
                convert_vertex(nextpos, out0, out1, cvtx[1]);
                convert_vertex(nextpos, out0, out1, cvtx[2]);
            }
            else if (!all) {
                // some vertices clipped
                vfpu_v4 parm[3];
                parm[0] = calc_clip_par(dist[2], dist[0]);
                parm[1] = calc_clip_par(dist[0], dist[1]);
                parm[2] = calc_clip_par(dist[1], dist[2]);

                outc.FastClear();
                clip_triangle(cvtx, dist, parm, status, outc);

                if (outc.Size() >= 3) {
                    CHECK_BUFFER(3)
                    convert_vertex(nextpos, out0, out1, outc[0]);
                    convert_vertex(nextpos, out0, out1, outc[1]);
                    convert_vertex(nextpos, out0, out1, outc[2]);
                    // if we get more than 3 clipped vertices, we will arrange for a triangle fan configuration.
                    for (int c=3; c<outc.Size(); c++) {
                        CHECK_BUFFER(3)
                        convert_vertex(nextpos, out0, out1, outc[0]);
                        convert_vertex(nextpos, out0, out1, outc[c-1]);
                        convert_vertex(nextpos, out0, out1, outc[c]);
                    }
                }
            }
#undef REFILL_BUFFER

            id+=3;
            count-=3;
        }
        break;
    // tri-strips can be clipped similarly by inserting degenerate triangles where necessary
    // tri-fans too, but if the first vertex is clipped there, it'd need to be converted into a tri-strip
    case SCEGU_PRIM_TRIANGLE_STRIP: {
        if (count < 3) break;
        unsigned nvo = 0;
        const MESHVERTEX *vtx[3];
        const Vector *nrm[3];
        ClipVertex cvtx[3];
        int status[3];
        vfpu_v4 dist[3];
        CLIPSTREAM1* save0 = 0;
        CLIPSTREAM2* save1 = 0;
        int stitch_c = 1, stitch_vo = -1;

        vtx[0] = (const MESHVERTEX*)(VPtr + id[0] * VStride);
        PREFETCH(vtx[0])
        vtx[1] = (const MESHVERTEX*)(VPtr + id[1] * VStride);
        PREFETCH(vtx[1])
        nrm[0] = (const Vector*)(NPtr + id[0] * NStride);
        PREFETCH(nrm[0])
        nrm[1] = (const Vector*)(NPtr + id[1] * NStride);
        PREFETCH(nrm[1])

        cvtx[0] = ClipVertex(*vtx[0], *nrm[0]);
        cvtx[1] = ClipVertex(*vtx[1], *nrm[1]);

#if 1
        dist[0] = calc_clip_dist(cp, &cvtx[0].Pos, status[0]);
        dist[1] = calc_clip_dist(cp, &cvtx[1].Pos, status[1]);
#endif

        id+=2;
        count-=2;

        while (count) {
            int all, any;
            vtx[2] = (const MESHVERTEX*)(VPtr + (*id) * VStride);
            PREFETCH(vtx[2])
            nrm[2] = (const Vector*)(NPtr + (*id) * NStride);
            PREFETCH(nrm[2])

            cvtx[2] = ClipVertex(*vtx[2], *nrm[2]);

#if 1
            dist[2] = calc_clip_dist(cp, &cvtx[2].Pos, status[2]);
            any = status[0] | status[1] | status[2];
            all = status[0] & status[1] & status[2];
#else
            any = 0; // disable clipping
            all = 0;
#endif

            // could check indices instead of the vertex data itself,
            // but we'll probably want it to work with non-indexed data too
            if (cvtx[0] == cvtx[1] ||
                cvtx[0] == cvtx[2] ||
                cvtx[1] == cvtx[2]) {
                // degenerate triangle, do not render it
                any = 0xffffffff;
                all = 0xffffffff;
            }

            if (!any) {
                // vertices not clipped, copy them
                if (stitch_c && stitch_vo != -1) {
#define REFILL_BUFFER stitch_vo = -1;
                    if (nvo == stitch_vo) {
                        CHECK_BUFFER(6)
                        if (stitch_vo == -1) goto wrap_stitch_noclip;
                        restore_vertex(nextpos, out0, out1, save0, save1);
                        restore_vertex(nextpos, out0, out1, save0, save1);
                    } else {
                        CHECK_BUFFER(5)
                        if (stitch_vo == -1) goto wrap_stitch_noclip;
                        restore_vertex(nextpos, out0, out1, save0, save1);
                    }
                    if (nvo) {
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                    } else {
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                    }
                    stitch_c = 0;
#undef REFILL_BUFFER
                }
                else if (stitch_c) {
wrap_stitch_noclip:
#define REFILL_BUFFER
                    if (nvo) {
                        CHECK_BUFFER(4)
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                    } else {
                        CHECK_BUFFER(3)
                        convert_vertex(nextpos, out0, out1, cvtx[0]);
                        convert_vertex(nextpos, out0, out1, cvtx[1]);
                    }
                    stitch_c = 0;
                }
#undef REFILL_BUFFER
                else {
#define REFILL_BUFFER \
        if (nvo) { \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
            convert_vertex(nextpos, out0, out1, cvtx[0]); \
        } else { \
            convert_vertex(nextpos, out0, out1, cvtx[0]); \
            convert_vertex(nextpos, out0, out1, cvtx[1]); \
        }
                    CHECK_BUFFER(1)
#undef REFILL_BUFFER
                }
                save0 = out0; save1 = out1;
                convert_vertex(nextpos, out0, out1, cvtx[2]);
                stitch_vo = nvo;
            }
            else if (!all) {
                // some vertices clipped
                vfpu_v4 parm[3];
                parm[0] = calc_clip_par(dist[2], dist[0]);
                parm[1] = calc_clip_par(dist[0], dist[1]);
                parm[2] = calc_clip_par(dist[1], dist[2]);

                outc.FastClear();
                clip_triangle(cvtx, dist, parm, status, outc);

                if (outc.Size() >= 3) {
                    if (stitch_vo != -1) {
#define REFILL_BUFFER stitch_vo = -1;
                        if (!stitch_vo) {
                            CHECK_BUFFER(6)
                            if (stitch_vo == -1) goto wrap_stitch_clip;
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            restore_vertex(nextpos, out0, out1, save0, save1);
                        } else {
                            CHECK_BUFFER(5)
                            if (stitch_vo == -1) goto wrap_stitch_clip;
                            restore_vertex(nextpos, out0, out1, save0, save1);
                        }
                        convert_vertex(nextpos, out0, out1, outc[0]);
#undef REFILL_BUFFER
                    }
                    else {
wrap_stitch_clip:
#define REFILL_BUFFER
                        CHECK_BUFFER(3)
#undef REFILL_BUFFER
                    }

#define REFILL_BUFFER stitch_vo = -1;
                    convert_vertex(nextpos, out0, out1, outc[0]);
                    convert_vertex(nextpos, out0, out1, outc[1]);
                    save0 = out0; save1 = out1;
                    convert_vertex(nextpos, out0, out1, outc[2]);
                    stitch_vo = 0;

                    // if we get more than 3 clipped vertices, we will arrange for a triangle fan configuration.
                    // FIXME: probably possible to turn a fan into a strip with fewer stitches than this
                    for (int c=3; c<outc.Size(); c++) {
                        CHECK_BUFFER(6)
                        if (stitch_vo != -1) {
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            restore_vertex(nextpos, out0, out1, save0, save1);
                            convert_vertex(nextpos, out0, out1, outc[0]);
                        }

                        convert_vertex(nextpos, out0, out1, outc[0]);
                        convert_vertex(nextpos, out0, out1, outc[c-1]);
                        save0 = out0; save1 = out1;
                        convert_vertex(nextpos, out0, out1, outc[c]);
                        stitch_vo = 0;
                    }
                }
#undef REFILL_BUFFER
                stitch_c = 1;
            }
            else {
                stitch_c = 1;
            }

            if (nvo) {
                vtx[1] = vtx[2];
                nrm[1] = nrm[2];
                cvtx[1] = cvtx[2];
                status[1] = status[2];
                dist[1] = dist[2];
                nvo = 0;
            } else {
                vtx[0] = vtx[2];
                nrm[0] = nrm[2];
                cvtx[0] = cvtx[2];
                status[0] = status[2];
                dist[0] = dist[2];
                nvo = 1;
            }
            id++;
            count--;
        }
        break;
    }
    default:
        TRACE("Unhandled primitive type for clipping %d\n", prim);
        Assert(0);
        break;
    }

    clipstream_unlock(this, clipstream, out0, out1);

    if (nextpos > CLIPSTREAM_SIZE) {
        TRACE("Overflowed clip buffer!\n");
        Assert(0);
    }

    DrawArray(prim, startpos, nextpos - startpos, NULL, 2, clipstreams);
    clippos = nextpos;
}

void VideoSystemPSP::Draw(const int primType, int base, int count, int vbase, int vcount, int basevindex, bool swapPath)
{
    int     prim;
    int     icount;
    int     lighting;
    DWORD   color;

    if (!vshader[0]) {
        /* this should be an assertion */
        Assert(!"ERROR-> No shader set!!");
        return;
    }

    switch (primType){
        case RenderDevice::EPointList:     prim = SCEGU_PRIM_POINTS;         icount = count;   break;
        case RenderDevice::EPrimTriList:   prim = SCEGU_PRIM_TRIANGLES;      icount = count*3; break;
        case RenderDevice::EPrimTriStrip:  prim = SCEGU_PRIM_TRIANGLE_STRIP; icount = count+2; break;
        case RenderDevice::EPrimTriFan:    prim = SCEGU_PRIM_TRIANGLE_FAN;   icount = count+2; break;
        case RenderDevice::EPrimLineList:  prim = SCEGU_PRIM_LINES;          icount = count*2; break;
        case RenderDevice::EPrimLineStrip: prim = SCEGU_PRIM_LINE_STRIP;     icount = count+1; break;
        default: return;
    }

    BYTE* IPtr = 0;

    //int Format = vshader->Format;

    if (ibuffer) {
        //Format |= ibuffer->Format;
        IPtr = (BYTE*)(ibuffer->Ptr ? ibuffer->Ptr : ibuffer->SourceData);
        IPtr += base*ibuffer->Stride;
        //TRACE("VideoSystemPSP::Draw(): setting the index buffer to %08p {base = %d, format = %d}\n", IPtr, base, ibuffer->Format);
        // Although the PSP can do indexed rendering in hardware,
        // the docs say that it's slower than non-indexed rendering
        // because vertices are not cached and the index fetches
        // generate additional memory I/O. Perhaps we'll want to
        // change the engine to avoid indexed geometry, if so we'll
        // want to emit warnings here. However, since available RAM
        // is quite limited, we probably don't want to.
    }

#if 0
    if (ibuffer) // loading screen don't use ibuffer, meshes do
    {
#ifdef TRANSGAMING_LOGGING
        PSPLog.Print("Rendering (%d, %d%s)! (vptr %p) l=%d a=%d op=%d t=%d s=%d z=%d b=%d\n",
            primType, icount, ibuffer ? ", indexed" : "", Ptr,
            Lighting,
            CachedState.AlphaTest,
            CachedTextureStates[0].Op,
            CachedTextureStates[0].TexId,
            CachedState.StencilTest,
            CachedState.DepthTest,
            CachedState.BlendMode);
        //PSPLog.Flush();
#endif // TRANSGAMING_LOGGING
    }
#endif

#ifdef VDUMP
    RecalcMat();
    for (int iv=0; iv<icount; iv++) {
        if (iv >= 9) {
#ifdef TRANSGAMING_LOGGING
            PSPLog.Print(" ...\n");
#endif
            break;
        }
        int id = ibuffer ? (((WORD*)IPtr)[iv]) : iv;
        Vector* m = (Vector*)(Ptr + Stride*(id+1) - sizeof(Vector));
        Vector p = Matrix((float*)&mat).Transform(*m);
        int x = (int)(p.X * 240) + 240;
        int y = (int)(-p.Y * 136) + 136;
        int z = calcz(p.Z);
#ifdef TRANSGAMING_LOGGING
        PSPLog.Print("[%04d] %8.02f %8.02f %8.02f => %5d %5d %5d\n", id, m->X, m->Y, m->Z, x, y, z);
#endif
    }
#endif

    //this object is supposed to be treated as being multitextured => set bone 0's matrix to the identity
    if (vshader[0]->MT){
        //the first bone matrix must be set to the identity in the case of multitextured surfaces.  This
        //is because the first and second sets of texture coordinates are hidden under bone weights and
        //diffuse colors in the vertex format.  There is a single padding float at the start of each vertex
        //that is always treated as bone weight 0 and is always set to 1.0f.  This is because the first
        //texture U coordinate would be treated as a bone weight when rendering the second pass, and since
        //it cannot be guaranteed to be 1.0f, that weight would change the position of the vertex when
        //rendered.  Setting the first value to 1.0f and setting the bone 0 matrix to the identity guarantees
        //that the vertices will remain unmoved when each pass is rendered.
        sceGuBoneMatrix(0, &idmat);
    }
    

    if (vshader[0]->Clip) {
        if (IPtr)
            DrawArrayClipI(prim, ibuffer ? basevindex : base, icount, IPtr, 1, streams);

        else
            DrawArrayClipNI(prim, ibuffer ? basevindex : base, icount, 1, streams);
    }
    
    else {
        DrawArray(prim, ibuffer ? basevindex : base, icount, IPtr, 1, streams);
    }

#ifdef TRANSGAMING_SHOW_FPS
    if (IPtr){
        if (vcount != EInvalidIndex){
            g_vertexCount += vcount;
            g_triCount += count;
        }

        else{
            g_triCount += count;
            g_vertexCount += icount;
        }
    }

    else{
        g_triCount += count;
        g_vertexCount += icount;
    }

    g_objectCount++;
#endif
    has_render++;
    CheckList();
}

// ======================================================
// internal
// ======================================================

#define PK_MIPMAPLODBIAS    (-0.4F)
//#define PK_MIPMAPLODBIAS    (-0.0F)

void VideoSystemPSP::RestoreDevice()
{
    sceGuShadeModel(SCEGU_SMOOTH);
    sceGuLightMode(SCEGU_SEPARATE_SPECULAR_COLOR);
    sceGuEnable(SCEGU_CLIP_PLANE);
    sceGuDisable(SCEGU_DITHER);
    sceGuDisable(SCEGU_COLOR_TEST);
    sceGuDisable(SCEGU_LOGICAL_OPERATION);
    sceGuDisable(SCEGU_COLOR_DOUBLE);
    sceGuTexLevelMode(SCEGU_LOD_AUTO, PK_MIPMAPLODBIAS);
    sceGuDisable(SCEGU_TEXTURE);
    sceGuTexEnvColor(0xffffffff);
    sceGuPixelMask(0x00000000);
    sceGuDisable(SCEGU_LIGHTING);
    sceGuDisable(SCEGU_FOG);
    sceGuFog(0.0f, 1.0f, 0x00000000);
    sceGuFrontFace(SCEGU_CW);
    sceGuEnable(SCEGU_CULL_FACE);
    sceGuEnable(SCEGU_DEPTH_TEST);
    sceGuDepthFunc(SCEGU_GEQUAL); // reversed
    sceGuDepthMask(0);
    sceGuDisable(SCEGU_BLEND);
    sceGuDisable(SCEGU_STENCIL_TEST);
    sceGuStencilFunc(SCEGU_ALWAYS, 0x0, 0xff);
    sceGuStencilOp(SCEGU_KEEP, SCEGU_KEEP, SCEGU_KEEP);
    sceGuDisable(SCEGU_ALPHA_TEST);
    sceGuAlphaFunc(SCEGU_ALWAYS, 0x0, 0xff);
    sceGuScissor(0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);
    sceGuDisable(SCEGU_SCISSOR_TEST);

    // default material
    sceGuModelColor(0x00000000, 0xffffffff, 0xffffff, 0xffffff);
    sceGuSpecular(0.0f);
    sceGuColor(0xffffffff);

    // default global ambient
    sceGuAmbient(0x000000);

    // NOTE: the T&L engine can't normalize normals,
    // hope that won't cause too many problems

    for (int number=0; number<4; number++) {
        sceGuLightColor(number, SCEGU_DIFFUSE, 0);
        sceGuLightColor(number, SCEGU_SPECULAR, 0);
        sceGuLightColor(number, SCEGU_AMBIENT, 0);
        sceGuLightAtt(number, 0.0f, 0.0f, 0.0f);
        sceGuDisable(SCEGU_LIGHT0+number);
    }


    memcpy(&mat_obj, &idmat, sizeof(mat_obj));
    sceGuSetMatrix(SCEGU_MATRIX_WORLD, &mat_obj);
    memcpy(&mat_cam, &idmat, sizeof(mat_cam));
    sceGuSetMatrix(SCEGU_MATRIX_VIEW, &mat_cam);
    memcpy(&mat_proj, &idmat, sizeof(mat_proj));
    sceGuSetMatrix(SCEGU_MATRIX_PROJECTION, &mat_proj);
    memcpy(&mat, &idmat, sizeof(mat));
    mat_dirty = false;
    memcpy(&clip_plane, &idclip, sizeof(clip_plane));
    clip_dirty = false;

    // NOTE: local viewer lighting mode not supported

    // setup cached state
    CachedState.DepthTest=1;
    CachedState.DepthWrite=1;
    CachedState.ColorWrite=1;
    CachedState.AlphaWrite=1;
    CachedState.StencilTest=0;
    CachedState.StencilFunc=RenderState::EFuncAlways;
    CachedState.StencilRef=0x0;
    CachedState.StencilMask=0xFF;
    CachedState.StencilFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilZFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilPassOp=RenderState::EStencilOpKeep;
    CachedState.AlphaTest=0;
    CachedState.Lighting=0;
    CachedState.NormalizeNormals=1;
    CachedState.AmbientColor=0;
    CachedState.SpecularEnable=0;

    CachedState.DepthFunc=RenderState::EFuncLessEqual;
    CachedState.AlphaFunc=RenderState::EFuncAlways;
    CachedState.AlphaRef=0;
    CachedState.StencilFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilZFailOp=RenderState::EStencilOpKeep;
    CachedState.StencilPassOp=RenderState::EStencilOpKeep;

    CachedState.FillMode = RenderState::EFillSolid;
    CachedState.BlendMode= RenderState::EBlendNone;
    CachedState.CullMode = RenderState::ECullCCW;
    CachedState.ConstColor=(DWORD)-1;

    memset(&CachedTextureStates[0],0,sizeof(CachedTextureStates));
    for (int i=0; i<8; i++) {
        CachedTextureStates[i].Op = TextureState::ETexOpDisable;
        CachedTextureStates[i].CoordIndex = i+1;
    }

    FogEnable = 0;
    FogMode = FogState::EFogNone;
    FogStart = 0.0f;
    FogEnd = 1.0f;

    Lighting = false;
    memset(&LightEnable, 0, sizeof(LightEnable));

    SavedFogColor = 0;
    CachedColor = 0xffffffff;
    CachedSpecularMat = 0xffffff;
    CachedSpecularPow = 0.0f;
    CachedTexId = -1;
    CachedTexFunc = 0;
    CachedTexXfrm = 0;
    CachedTexProj = 0;
    CachedBlendMode = RenderState::EBlendNone;
    CachedFilter = 0;
    CachedAddrMode = 0;
    CachedTexEnable = false;
    CachedCDbl = false;
    CachedDepthTest = true;
    CachedDepthWrite = true;
}

void VideoSystemPSP::ReleaseDevice()
{
}

#ifdef DEBUG
void VideoSystemPSP::listTextures() const{
    TRACE("VideoSystemPSP::listTextures(): %d textures slots currently:\n", Textures.Size());

    for (int i = 0; i < Textures.Size(); i++)
        TRACE("  %2d) {Pic = 0x%08X, Target = 0x%08X, Comp = %d, Size = %d}\n", i, Textures[i].Pic, Textures[i].Target, Textures[i].Comp, Textures[i].Size);


    TRACE("VideoSystemPSP::listTextures(): %d free textures slots currently:\n", FreeTextures.Size());

    for (int i = 0; i < FreeTextures.Size(); i++)
        TRACE("  %2d) %d\n", i, FreeTextures[i]);
}
#endif

void VideoSystemPSP::ReleaseResource(int resourceType, const int* ids, int count)
{
    int topTex;

    if (ids == NULL || count == 0)
        return;


    switch (resourceType) {
    case EResTexture:
        for (int i=0; i<count; ++i)
        {
            const int index = ids[i];

            if (index<0 || index>=Textures.Size())
                continue;

            PSPTexture &tex = Textures[index];
#ifdef USE_GIM
            if (!tex.Pic)
                continue;

            sceGimPictureDelete(tex.Pic);
            tex.Pic = NULL;
#endif

            if (tex.Target) {
                vram_free(tex.Target);
                tex.Target = 0;
            }

            //clear out the rest of the texture's members.  The <Size> member will
            //be used to detect whether the texture is currently in use or not.
            //The textures array can unfortunately never be reordered since the 
            //indices themselves are the texture IDs.
            tex.Size = 0;
            tex.Comp = SCEGU_RGB;

            //add the index to the free list
            FreeTextures.Add(index);
        }


        //go through the current list of textures and attempt to chop of all unused
        //textures at the end of the table.
        topTex = Textures.Size();

        for (int i = Textures.Size() - 1; i >= 0 ; i--){
            if (Textures[i].Pic || Textures[i].Size > 0){
                topTex = i + 1;

                break;
            }
        }

        if (topTex < Textures.Size()){
            TRACE("VideoSystemPSP::ReleaseResource(): shrinking the main texture array from %d to %d\n", Textures.Size(), topTex);
            Textures.Shrink(topTex);
        }

#ifdef DEBUG
        //listTextures();
#endif
        

        //update the free list to make sure it doesn't index non-existent textures
        topTex = Textures.Size();

        for (int i = 0; i < FreeTextures.Size(); i++){
            if (FreeTextures[i] >= topTex){
                FreeTextures.FastRemove(i);
                i--;

                continue;
            }

#ifdef DEBUG
            //test each member of the free list to make sure the slot is actually free
            Assert(Textures[FreeTextures[i]].Size == 0 && Textures[FreeTextures[i]].Pic == NULL);
#endif
        }

#ifdef DEBUG
        //listTextures();
#endif

        break;

    case EResBuffer:
        for (int i=0; i<count; ++i)
        {
            if (ids[i] == EInvalidIndex)
                continue;

            PSPBuffer& buf=Buffers[ids[i]];
            if (buf.Type == PSPBuffer::EUnusedStream)
                continue;

            int Size = buf.ElemCount * buf.Stride;

            if (buf.Ptr) {
#ifdef STREAM_UNCACHED
                BYTE* Mem = (BYTE*)sceKernelMakeCachedAddr(buf.Ptr);
#else
                BYTE* Mem = (BYTE*)buf.Ptr;
#endif
                if (buf.Type & PSPBuffer::EIndexStream)   
                { 
                    TextureMemory[EMemIBs] -= (float)Size/(1024.f*1024.f);
                } 
                else if (buf.Type & PSPBuffer::EVertexStream)  
                { 
                    TextureMemory[EMemVBs] -= (float)Size/(1024.f*1024.f);
                }
                delete[] Mem;
                buf.Ptr = NULL;
            }
            buf.Type=PSPBuffer::EUnusedStream;
            buf.SourceData=0;
        }
        break;

    default:
        break;
    }
}

RenderEffect* VideoSystemPSP::CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size)
{
    return NULL;
}

BOOL VideoSystemPSP::GrabBackBuffer(Texture* Tex, HWND hwnd)
{
    return TRUE;
}

BOOL VideoSystemPSP::GrabZStencilBuffer(Texture* Tex)
{
    return TRUE;
}

void VideoSystemPSP::GetBackBufferSize(UINT& SizeX, UINT& SizeY)
{
    SizeX = FullScreenWidth;
    SizeY = FullScreenHeight;
}

// ==========================================================================================================

/*static BYTE GetD3DType(BYTE Type)
{
    return 0;
}//*/

/*int VideoSystemPSP::CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList)
{
#ifdef TRANSGAMING_LOGGING
    PSPLog.Print("CreateVertexShader\n");
#endif
    return 0;
}//*/

// ==========================================================================================================

int VideoSystemPSP::CreateQuery(int Type)
{
    // as far as I know, the hardware can't do occlusion queries
    return -1;
}

BOOL VideoSystemPSP::BeginQuery(int Query)
{
    return -1;
}

void VideoSystemPSP::EndQuery(int Query)
{
}

BOOL VideoSystemPSP::GetQueryData(int Query, void* OutData, int Size)
{
    return 0;
}

void VideoSystemPSP::SetCullFlip(BOOL Flip)
{
    CullFlip = Flip;
    CachedState.CullMode = 0xFF;
}

void VideoSystemPSP::SetVBlankState(bool state){
    waitVBlank = state;
}

bool VideoSystemPSP::GetVBlankState() const{
    return waitVBlank;
}


void VideoSystemPSP::SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite)
{
    if(CachedState.ColorWrite!=ColorWrite || CachedState.AlphaWrite!=AlphaWrite)
    {
        const UINT mskColor=0x00ffffff;
        const UINT mskAlpha=0xff000000;
        sceGuPixelMask((ColorWrite ? 0 : mskColor) | (AlphaWrite ? 0 : mskAlpha));
        CachedState.ColorWrite=ColorWrite;
        CachedState.AlphaWrite=AlphaWrite;
    }
}

// ============================================================================================

/*static inline DWORD TMapOp(const TextureStageState::ETexOp op)
{
    return 0;
}

static inline DWORD TMapArg(const TextureStageState::ETexArg arg)
{
    return 0;
}

static inline DWORD TMapFilter(const TextureStageState::ETexFilter filter)
{
    return 0;
}

static inline DWORD TMapAddr(const TextureStageState::ETexAddr addr)
{
    return 0;
}

static inline DWORD TMapCoord(const TextureStageState::ETexCoord coord)
{
    return 0;
}

static inline DWORD TMapTransform(const TextureStageState::ETexTransform xform)
{
    return 0;
}//*/

// ========================================================================================================
void PSPTexture::SetParams(Texture *texDef, int format, int comp, int size, int locksize){
    Comp = comp;
    //Format = format;
    //LockSize = locksize;
    //TexDef = texDef;
    Size = size;
    Target = 0;
}

int VideoSystemPSP::findFreeTexture(Texture *tex, int format, int comp, int size, int locksize){
    int index = 0;


    //free list is empty => allocate a new texture and add it to the table
    if (FreeTextures.Size() == 0){
        index = Textures.Add(PSPTexture());

        Textures[index].SetParams(tex, format, comp, size, locksize);
        Textures[index].Pic = NULL;
    }

    else{
        index = FreeTextures[0];
        FreeTextures.FastRemove(0);

        Textures[index].SetParams(tex, format, comp, size, locksize);
        Textures[index].Pic = NULL;
    }


    return index;
}


void VideoSystemPSP::CreateRegularTextureFromFile(Texture *texture)
{
#ifdef USE_GIM
    int count = sceGimPictureGetChunkCount(texture->DataFile, texture->Size);
    if (!count) return;
    SceGimChunk* chk = sceGimPictureGetChunk(texture->DataFile, texture->Size, 0);
    SceGimPicture* pic = sceGimPictureCreate(chk);
    SceGimImageHeader* img = sceGimPictureGetImage(pic);
    int format = sceGimImageGetFormat(img);
    int bpp = sceGimImageGetBPP(img);
    int width = sceGimImageGetWidth(img, 0);
    int height = sceGimImageGetHeight(img, 0);
    int size = sceGimImageGetPitch(img, width) * height;
    int comp = sceGimPictureGetFunc(pic)[1];
    sceGimPictureSetEnableBits(pic,
                               SCEGIM_ENABLE_CROP,
                               SCEGIM_ENABLE_CROP);
    if (!format) {
        sceGimPictureDelete(pic);
        return;
    }
#endif
    int index = findFreeTexture(texture,format,comp,size,0);
#ifdef USE_GIM
    //TRACE("VideoSystemPSP::CreateRegularTextureFromFile(): found a free texture at index %d.  Saving pic %p {Textures.Size = %d}\n", index, pic, Textures.Size());
    Textures[index].Pic = pic;
#endif
    // original image size from the artist (needed by HUD code)
    // FIXME: do we have this info in the GIM file?
    texture->OriginalWidth = width * 2;
    texture->OriginalHeight = height * 2;
    // image size after device-specific scaling etc
    texture->Width = width;
    texture->Height = height;

    texture->PixelBytes = size;
    texture->DeviceId = index;

    texture->FreeData();
}

#ifdef USE_GIM
static SceGimPicture* SetupTexture(Texture *texture, int w, int h, int format, int comp, void* buf, int* size)
{
    static const float std_crop[] = { 0.0f, 0.0f, 1.0f, 1.0f };

    SceGimPicture* pic = sceGimPictureCreate(0);
    if (!sceGimPictureReallocImage(pic, format, w, h,
                                   1, 1, NULL)) {
        sceGimPictureDelete(pic);
        return 0;
    }
    sceGimPictureSetFunc(pic, SCEGIM_FUNC_MODULATE, comp);
    sceGimPictureSetImageCrop(pic, std_crop);

    SceGimImageHeader* img = sceGimPictureGetImage(pic);
    int bpp = sceGimImageGetBPP(img);
    int width = sceGimImageGetWidth(img, 0);
    int height = sceGimImageGetHeight(img, 0);
    *size = sceGimImageGetPitch(img, width) * height;
    sceGimPictureSetEnableBits(pic,
                               SCEGIM_ENABLE_CROP,
                               SCEGIM_ENABLE_CROP);
    format = sceGimImageGetFormat(img); // validity check
    if (!format) {
        sceGimPictureDelete(pic);
        return 0;
    }
    return pic;
}
#endif

void VideoSystemPSP::CreateRegularTexture(Texture *texture)
{
#ifdef USE_GIM
    int size = 0;
    int format = SCEGIM_FORMAT_RGBA8888;
    int comp = SCEGIM_FUNC_RGB;
    if (texture->Flags & Texture::ETexRaw8) {
        format = SCEGIM_FORMAT_INDEX8;
        comp = SCEGIM_FUNC_RGB;
    }
    else if (texture->Flags & Texture::ETexRaw32Alpha) {
        format = SCEGIM_FORMAT_RGBA8888;
        comp = SCEGIM_FUNC_RGBA;
    }


    //make sure this was a raw created texture.  If not the file header information will still
    //be in the data block that will be treated as the texture.
    Assert(texture->Flags & (Texture::ETexRaw32 | Texture::ETexRaw8 | Texture::ETexRaw32Alpha));


    SceGimPicture* pic = SetupTexture(texture, texture->Width, texture->Height, format, comp, NULL, &size);
    if (!pic) return;
#endif
    int index = findFreeTexture(texture,format,comp,size,0);
#ifdef USE_GIM
    //TRACE("VideoSystemPSP::CreateRegularTexture(): found a free texture at index %d.  Saving pic %p {Textures.Size = %d}\n", index, pic, Textures.Size());
    Textures[index].Pic = pic;
#endif
    texture->DeviceId = index;

    //the data should already be NULL for this texture, but it doesn't hurt to make sure
    texture->FreeData();
}

void VideoSystemPSP::CreateRenderTarget(Texture *texture)
{
    if (texture->Format && texture->Format != Texture::ETexFmt_A8R8G8B8) {
        return;
    }

    // render target pitch must be multiple of 64 pixels
    int width = (texture->Width+63)&~63;
    int height = texture->Height;
    VBLK blk = vram_alloc(width * height * 4);
    if (!blk) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CreateRenderTarget(): ERROR-> out of VRAM!\n");
#endif
        return;
    }

#ifdef USE_GIM
    int size = 0;
    int format = SCEGIM_FORMAT_RGBA8888;
    int comp = SCEGIM_FUNC_RGBA;

#ifdef TRANSGAMING_LOGGING
    //PSPLog.Print("Creating %dx%d (target: %s)\n", texture->Width, texture->Height, texture->FileName.AsChar());
#endif

    void* ptr = vram_lock(blk);
    SceGimPicture* pic = SetupTexture(texture, width, height, format, comp, ptr, &size);
    vram_unlock(blk);
    if (!pic) {
        vram_free(blk);
        return;
    }
#endif
    int index = findFreeTexture(texture,format,comp,size,0);
#ifdef USE_GIM
    //TRACE("VideoSystemPSP::CreateRenderTarget(): found a free texture at index %d.  Saving pic %p {Textures.Size = %d}\n", index, pic, Textures.Size());
    Textures[index].Pic = pic;
#endif
    Textures[index].Target = blk;
    texture->OriginalWidth = texture->Width;
    texture->OriginalHeight = texture->Height;
    texture->PixelBytes = size;
    texture->DeviceId = index;

    //the data should already be NULL for this texture, but it doesn't hurt to make sure
    texture->FreeData();
}

void VideoSystemPSP::CreateDepthSurface(Texture *texture)
{
    // render target pitch must be multiple of 64 pixels
    int width = (texture->Width+63)&~63;
    int height = texture->Height;
    VBLK blk = vram_alloc(width * height * 2);
    if (!blk) {
#ifdef TRANSGAMING_LOGGING
        TRACE("VideoSystemPSP::CreateDepthSurface(): ERROR-> out of VRAM!\n");
#endif
        return;
    }

#ifdef USE_GIM
    int size = 0;
    int format = SCEGIM_FORMAT_INDEX16;
    int comp = SCEGIM_FUNC_RGBA;

#ifdef TRANSGAMING_LOGGING
    //PSPLog.Print("Creating %dx%d (depth: %s)\n", texture->Width, texture->Height, texture->FileName.AsChar());
#endif

    void* ptr = vram_lock(blk);
    SceGimPicture* pic = SetupTexture(texture, width, height, format, comp, ptr, &size);
    vram_unlock(blk);
    if (!pic) {
        vram_free(blk);
        return;
    }
#endif
    int index = findFreeTexture(texture,format,comp,size,0);
#ifdef USE_GIM
    //TRACE("VideoSystemPSP::CreateDepthSurface(): found a free texture at index %d.  Saving pic %p {Textures.Size = %d}\n", index, pic, Textures.Size());
    Textures[index].Pic = pic;
#endif
    Textures[index].Target = blk;
    texture->OriginalWidth = texture->Width;
    texture->OriginalHeight = texture->Height;
    texture->PixelBytes = size;
    texture->DeviceId = index;

    //the data should already be NULL for this texture, but it doesn't hurt to make sure
    texture->FreeData();
}


// LOCK TEXTURE

BYTE *VideoSystemPSP::LockTexture( Texture *tex, int &pitch )
{
    if (tex->DeviceId == EInvalidIndex)
        return NULL;

#ifdef TRANSGAMING_LOGGING
    //PSPLog.Print("LockTexture %d\n", tex->DeviceId);
#endif
    PSPTexture& Tex = Textures[tex->DeviceId];


#ifdef USE_GIM
    SceGimImageHeader* img = sceGimPictureGetImage(Tex.Pic);
    int width = sceGimImageGetWidth(img, 0);
    pitch = sceGimImageGetPitch(img, width);
    BYTE* ptr;
    if (Tex.Target) {
        void *pixels = vram_lock(Tex.Target);
//        sceGimPictureSetImagePixels(Tex.Pic, 0, 0, pixels);
        ptr = (BYTE*)pixels;
    } else {
        ptr = (BYTE*)sceGimPictureGetImagePixels(Tex.Pic, 0, 0);
    }
#ifdef TRANSGAMING_LOGGING
    //PSPLog.Print(" returning %p, %d\n", ptr, pitch);
#endif
    return ptr;
#endif
    return NULL;
}


void VideoSystemPSP::UnlockTexture( Texture *tex )
{
    if (tex->DeviceId == EInvalidIndex)
        return;

#ifdef TRANSGAMING_LOGGING
    //PSPLog.Print("UnlockTexture %d\n", tex->DeviceId);
#endif

    PSPTexture& Tex = Textures[tex->DeviceId];

    // just in case (perhaps use WritebackRange instead?)
    sceKernelDcacheWritebackAll();

    if (Tex.Target) {
        vram_unlock(Tex.Target);
    }

    // just in case
    if (tex->DeviceId == CachedTextureStates[0].TexId) {
        sceGuTexFlush();
    }
}

void VideoSystemPSP::SetTexturePriority(Texture *tex, DWORD priority)
{
}

void VideoSystemPSP::PreLoadTexture(Texture *tex)
{
}


// ============================= PSPEffect (RenderEffect) ========================================

PSPEffect::PSPEffect()
{
    Effect = 0;
    //Assert(Effect);
}

PSPEffect::~PSPEffect()
{
}

BOOL PSPEffect::EnumTechniques(DynamicArray<RenderTechnique>& Out)
{
    return 0;
}

DWORD PSPEffect::GetParamIndex(const char* Name)
{
    return 0;
}

DWORD PSPEffect::GetTextureIndex(const char* Name)
{
    return 0;
}

BOOL PSPEffect::GetTechniqueString(DWORD Technique, const char* Name, String& Value)
{
    return 0;
}

BOOL PSPEffect::SetParam(const char* Name, const void* Data, DWORD Size)
{
    return 0;
}

BOOL PSPEffect::SetParam(DWORD Index, const void* Data, DWORD Size)
{
    return 0;
}

BOOL PSPEffect::SetTexture(const char* Name, Texture* Tex)
{
    return 0;
}

BOOL PSPEffect::SetTexture(DWORD Index, Texture* Tex)
{
    return 0;
}

BOOL PSPEffect::SetTechnique(DWORD Index)
{
    return 0;
}

UINT PSPEffect::Begin()
{
    return 0;
}

BOOL PSPEffect::BeginPass(UINT Num)
{
    return 0;
}

BOOL PSPEffect::EndPass()
{
    return 0;
}

BOOL PSPEffect::End()
{
    return 0;
}

// === VRAM MEMORY MANAGER

static BYTE* vram_base = NULL;
static int vram_size = 0;

struct MemBlock {
    unsigned Ofs;
    unsigned Size;
    unsigned Flags;
    MemBlock* prev;
    MemBlock* next;
};

static MemBlock* vheap_free = NULL;
static MemBlock* vheap_used = NULL;

static void vram_open(void)
{
    vram_base = (BYTE*)sceGeEdramGetAddr();
    vram_size = sceGeEdramGetSize();
#ifdef TRANSGAMING_LOGGING
    //VideoSystemPSP::PSPLog.Print("VRAM: open: base %p, size %d\n", vram_base, vram_size);
#endif
    m_setTag("Video");
    vheap_free = new MemBlock;
    vheap_free->Ofs = 0;
    vheap_free->Size = vram_size;
    vheap_free->Flags = 0;
    vheap_free->prev = NULL;
    vheap_free->next = NULL;
}

static void vram_close(void)
{
    MemBlock* cur = vheap_free;
    vheap_free = 0;
    while (cur) {
        MemBlock* next = vheap_free->next;
        delete cur;
        cur = next;
    }
#ifdef TRANSGAMING_LOGGING
    //VideoSystemPSP::PSPLog.Print("VRAM: close\n");
#endif
}

static VBLK vram_alloc(unsigned size)
{
    MemBlock* cur;

    // align to 16 bytes
    size = (size+15)&~15;

    cur = vheap_free;
    while (cur) {
        if (cur->Size >= size) {
            MemBlock* mem;

            if (cur->Size == size) {
                if (cur->prev) cur->prev->next = cur->next;
                else vheap_free = cur->next;
                if (cur->next) cur->next->prev = cur->prev;
                mem = cur;
            }
            else {
                m_setTag("Video");
                mem = new MemBlock;
                mem->Ofs = cur->Ofs;
                mem->Size = size;

                cur->Ofs += size;
                cur->Size -= size;
            }
            mem->Flags = 1;
            mem->prev = NULL;
            mem->next = vheap_used;
            vheap_used = mem;

#ifdef TRANSGAMING_LOGGING
            //VideoSystemPSP::PSPLog.Print("VRAM: alloc %d success, got ofs 0x%x\n", size, mem->Ofs);
#endif
            return (VBLK)mem;
        }
        cur = cur->next;
    }
    // allocation failed!
#ifdef TRANSGAMING_LOGGING
    //VideoSystemPSP::PSPLog.Print("VRAM: alloc %d failed\n", size);
#endif
    return NULL;
}

static void vram_free(VBLK mh)
{
    MemBlock* mem = (MemBlock*)mh;
    MemBlock* cur;

    if (!mem->Flags) {
#ifdef TRANSGAMING_LOGGING
        //VideoSystemPSP::PSPLog.Print("VRAM: double free ofs 0x%x size %d\n", mem->Ofs, mem->Size);
#endif
        Assert(mem->Flags);
        return;
    }

#ifdef TRANSGAMING_LOGGING
    //VideoSystemPSP::PSPLog.Print("VRAM: free ofs 0x%x size %d\n", mem->Ofs, mem->Size);
#endif

    if (mem->prev) mem->prev->next = mem->next;
    else vheap_used = mem->next;
    if (mem->next) mem->next->prev = mem->prev;
    mem->Flags = 0;
    mem->prev = NULL;
    mem->next = NULL;

    cur = vheap_free;

    if (!cur) {
        vheap_free = mem;
        mem = NULL;
    }
    else while (TRUE) {
        if ((cur->Ofs <= (mem->Ofs + mem->Size)) && (cur->Ofs >= mem->Ofs)) {
            // add to beginning of current block
#ifdef TRANSGAMING_LOGGING
            //VideoSystemPSP::PSPLog.Print("VRAM: free merge front 0x%x\n", cur->Ofs);
#endif
            cur->Size = cur->Ofs + cur->Size - mem->Ofs;
            cur->Ofs = mem->Ofs;
            break;
        }
        if ((cur->Ofs <= mem->Ofs) && ((cur->Ofs + cur->Size) >= mem->Ofs)) {
            // add to end of current block
#ifdef TRANSGAMING_LOGGING
            //VideoSystemPSP::PSPLog.Print("VRAM: free merge back 0x%x\n", cur->Ofs);
#endif
            cur->Size = mem->Ofs + mem->Size - cur->Ofs;
            if (cur->next && (cur->next->Ofs <= (cur->Ofs + cur->Size))) {
                // merge with next
                MemBlock *next = cur->next;
#ifdef TRANSGAMING_LOGGING
                //VideoSystemPSP::PSPLog.Print("VRAM: free merge next 0x%x\n", cur->next->Ofs);
#endif
                cur->Size = next->Ofs + next->Size - cur->Ofs;
                cur->next = next->next;
                if (cur->next) cur->next->prev = cur;
                delete next;
            }
            break;
        }
        if (cur->Ofs > (mem->Ofs + mem->Size)) {
            // insert before
#ifdef TRANSGAMING_LOGGING
            //VideoSystemPSP::PSPLog.Print("VRAM: free insert before 0x%x\n", cur->Ofs);
#endif
            mem->prev = cur->prev;
            mem->next = cur;
            if (mem->prev) mem->prev->next = mem;
            else vheap_free = mem;
            if (mem->next) mem->next->prev = mem;
            mem = NULL;
            break;
        }
        if (!cur->next) {
            // insert after
#ifdef TRANSGAMING_LOGGING
            //VideoSystemPSP::PSPLog.Print("VRAM: free insert after 0x%x\n", cur->Ofs);
#endif
            mem->prev = cur;
            mem->prev->next = mem;
            mem = NULL;
            break;
        }
        cur = cur->next;
    }

    if (mem) delete mem;
}

static void* vram_lock(VBLK mh)
{
    MemBlock* mem = (MemBlock*)mh;
    return vram_base + mem->Ofs;
}

static void vram_unlock(VBLK mh)
{
}

static void* vram_fb_lock(VBLK mh)
{
    MemBlock* mem = (MemBlock*)mh;
    return (void*)(SCEGU_VRAM_TOP + mem->Ofs);
}

static void vram_fb_unlock(VBLK mh)
{
}

static void vram_swap(VBLK mh1, VBLK mh2)
{
    MemBlock* mem1 = (MemBlock*)mh1;
    MemBlock* mem2 = (MemBlock*)mh2;
    unsigned tmp;
    tmp = mem1->Ofs;
    mem1->Ofs = mem2->Ofs;
    mem2->Ofs = tmp;
}


//------------------------------------------------------------------------------------------------------------------------------------

void SaveScreenBuffer(const char *Filename)
{
#ifndef TRANSGAMING_MASTERING

	#define WIDTH 480
	#define HEIGHT 272

	typedef struct { unsigned char r, g, b ; } RGB888;

	int					fd, i,j;
	unsigned char		TGAHeader[] = { 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0,0x01,0x10,0x01,0x18,0x20};
	RGB888				*dst;
	unsigned char		*FrameBuffer;

	// to create image file. 
	fd = sceIoOpen(Filename, SCE_O_RDWR | SCE_O_TRUNC | SCE_O_CREAT, 0777);
	if(fd < 0)
	{
		printf("Open failed!\n");
		return;
	}

	sceIoWrite(fd, TGAHeader, sizeof(TGAHeader));

	dst			= (RGB888*)malloc(WIDTH*HEIGHT*sizeof(RGB888));
	FrameBuffer	= (unsigned char*)sceGeEdramGetAddr();

	for (i=0; i<HEIGHT; i++)
	{
		for (j=0; j<WIDTH; j++)
		{
			// take into account the width of eDRAM which is 512 while fb size is 480x272
			dst[(i*WIDTH)+j].b = FrameBuffer[4*((i*512)+j)+0];
			dst[(i*WIDTH)+j].g = FrameBuffer[4*((i*512)+j)+1];
			dst[(i*WIDTH)+j].r = FrameBuffer[4*((i*512)+j)+2];
		}
	}

	sceIoWrite(fd, dst, WIDTH*HEIGHT*sizeof(RGB888));

	free(dst);
	sceIoClose(fd);

#endif
}

//------------------------------------------------------------------------------------------------------------------------------------

