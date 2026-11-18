//#include "D3Dev.h"
//#include "D3DEffect.h"
//#include "dxerr9.h"
//#include "Render/Viewport.h"
//#include "Render/View.h"
//#include "Render/Video.h"
//#include "Render/Material.h"
#include "Render/VideoSystem_PS2.h"

#include <stdlib.h>
#include <sifdev.h>
#include <eekernel.h>
#include <libgraph.h>

#include <GL/ps2gl.h>

#include "Core/nommgr.h"

#include <ps2s/core.h>
#include <ps2s/gs.h>

#include "Core/mmgr.h"

#include <tg_pc2ps2.h>
#include <malloc.h>
#include <libdma.h>
#include <libgp.h>

//#define STREAM_UNCACHED

#ifdef TRANSGAMING_LOGGING
LogBuffer VideoSystemPS2::PS2Log;
#endif

// DLL interface
RenderDevice* CreateDevice()
{
//    VideoSystemPS2::PS2Log.Open("D3Dev.log");
//    VideoSystemPS2::PS2Log.Print("Creating PS2 Renderer\n");
    return new VideoSystemPS2();
}

void DestroyDevice(RenderDevice* rdevPtr)
{
//    VideoSystemPS2::PS2Log.Print("Destroying PS2 Renderer\n");
    if(rdevPtr) delete rdevPtr;
}

#ifdef TRANSGAMING_SHOW_FPS
extern int g_triCount;
extern int g_objectCount;
extern int g_vertexCount;
#endif

// ===========================================================
#if defined(TRANSGAMING) && defined(DEBUG)
int PS2Shader::nextID = 1;
#endif

static GLColor black(0x00000000);
static GLColor white(0xffffffff);

static void *BufferAllocate(size_t Size)
{
#ifdef TRANSGAMING_PS2
	void *Mem = memalign(64, sizeof(char)*Size);
#else
	void *Mem = malloc(sizeof(char)*Size);
#endif
	return(Mem);
}

static void *BufferFree(void *buffer)
{
	free(buffer);
}

VideoSystemPS2::VideoSystemPS2() : RenderDevice()
{
#ifdef TRANSGAMING_LOGGING
    VideoSystemPS2::PS2Log.Open("D3Dev.log");
    VideoSystemPS2::PS2Log.Print("Creating PS2 Renderer\n");
    VideoSystemPS2::PS2Log.Flush();
#endif
    //RenderDevice();
    is_open = 0;
    FullScreenWidth = 0;
    FullScreenHeight = 0;
    CurrentStreamSize = 0;
    CurrentStreamIsIndexed = FALSE;
    //CurrentVertexShader = 0;
    //CurrentPixelShader = 0;
    TexBytes = 0;
    FrameTris = 0;
    FrameDIPs = 0;
    waitVBlank = true;
    //OriginalBackBuffer = 0;
    //OriginalDepthBuffer = 0;
    //CurrentBackBuffer = 0;
    //CurrentDepthBuffer = 0;
    ViewportWidth = 0;
    ViewportHeight = 0; 
    StencilSurfaceTouched = FALSE;
    CullFlip = 0;
	Lighting = false;

    //add a dummy texture so that texture ID 0 is always occupied
    Textures.Add(PS2Texture());
    GetOriginalGamma();
}

VideoSystemPS2::~VideoSystemPS2()
{
    RestoreOriginalGamma();
    Close();
    Textures.ClearArray();
    FreeTextures.ClearArray();
#ifdef TRANSGAMING_LOGGING
    VideoSystemPS2::PS2Log.Print("Destroying PS2 Renderer\n");
    VideoSystemPS2::PS2Log.Flush();
#endif
}

// =============================================================
// RenderDevice interface
// =============================================================
BOOL VideoSystemPS2::Open(unsigned int)
{
    Close();

// the hardware reset is already done in our PainMain.

// copied init stuff from ps2gl's glut for now

    BOOL success = pglInit( 256 * 1024, 2000 );

    HWClass = EHWClassGenericTnL;

    DisplayModes[0].XRes = 640;
    DisplayModes[0].YRes = 448;
    DisplayModes[0].RefreshRates[0] = 60;
    DisplayModes[0].RefreshRateNum = 1;
    DisplayModeNum = 1;

    int VRAM = 0;

    // frame and depth buffer
    pgl_slot_handle_t frame_slot_0, frame_slot_1, depth_slot;
    frame_slot_0 = pglAddGsMemSlot( VRAM, 140, GS::kPsm32 );  VRAM+=140;
    frame_slot_1 = pglAddGsMemSlot( VRAM, 140, GS::kPsm32 );  VRAM+=140;
    depth_slot   = pglAddGsMemSlot( VRAM, 140, GS::kPsmz24 ); VRAM+=140;
    // lock these slots so that they aren't allocated by the memory manager
    pglLockGsMemSlot( frame_slot_0 );
    pglLockGsMemSlot( frame_slot_1 );
    pglLockGsMemSlot( depth_slot );

    // create gs memory area objects to use for frame and depth buffers
    pgl_area_handle_t frame_area_0, frame_area_1, depth_area;
    frame_area_0 = pglCreateGsMemArea( 640, 448, GS::kPsm32 );
    frame_area_1 = pglCreateGsMemArea( 640, 448, GS::kPsm32 );
    depth_area = pglCreateGsMemArea( 640, 448, GS::kPsmz24 );
    // bind the areas to the slots we created above
    pglBindGsMemAreaToSlot( frame_area_0, frame_slot_0 );
    pglBindGsMemAreaToSlot( frame_area_1, frame_slot_1 );
    pglBindGsMemAreaToSlot( depth_area, depth_slot );

    // draw to the new areas...
    pglSetDrawBuffers( PGL_NONINTERLACED, frame_area_0, frame_area_1, depth_area );
    // ...and display from them
    pglSetDisplayBuffers( PGL_NONINTERLACED, PGL_NOFRAMEAA, frame_area_0, frame_area_1 );

    // a slot for fonts (probably)
    pglAddGsMemSlot( VRAM, 2, GS::kPsm8 ); VRAM+=2;

    // 64x32
    pglAddGsMemSlot( VRAM, 1, GS::kPsm32 ); VRAM+=1;
    pglAddGsMemSlot( VRAM, 1, GS::kPsm32 ); VRAM+=1;
    // 64x64
    pglAddGsMemSlot( VRAM, 2, GS::kPsm32 ); VRAM+=2;
    pglAddGsMemSlot( VRAM, 2, GS::kPsm32 ); VRAM+=2;
    pglAddGsMemSlot( VRAM, 2, GS::kPsm32 ); VRAM+=2;
    pglAddGsMemSlot( VRAM, 2, GS::kPsm32 ); VRAM+=2;
    // 128x128
    pglAddGsMemSlot( VRAM, 8, GS::kPsm32 ); VRAM+=8;
    pglAddGsMemSlot( VRAM, 8, GS::kPsm32 ); VRAM+=8;
    // 256x256
//  pglAddGsMemSlot( VRAM, 32, GS::kPsm32 ); VRAM+=32;
//  pglAddGsMemSlot( VRAM, 32, GS::kPsm32 ); VRAM+=32;
    // 512x256
    pglAddGsMemSlot( VRAM, 64, GS::kPsm32 ); VRAM+=64;
//  pglAddGsMemSlot( VRAM, 64, GS::kPsm32 ); VRAM+=64;

    printf(">>> Total VRAM Usage %d\n", VRAM);

    assert(VRAM<=512);

    pglPrintGsMemAllocation();

    is_render = 0;

    pglBeginGeometry();

    // try to clear screen
    ClearFB(0);
    Flip(0);

    // ps2gl has a one-frame delay, so do it again
    ClearFB(0);
    Flip(0);

    return success;
}

void VideoSystemPS2::Close()
{
    if (!is_open) return;

#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("Closing PS2 Render Device\n");
    PS2Log.Flush();
#endif

    ReleaseDevice();

    glFlush();
    pglEndGeometry();

    pglFinish();

    is_open = 0;
}

BOOL VideoSystemPS2::SetMode(UINT width, UINT height, BOOL fullscreen, int multisampling)
{
    BOOL success=TRUE;

#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("SetMode in progress (%dx%d)\n", width, height);
    PS2Log.Flush();
#endif

    int already_reset = FullScreenWidth && FullScreenHeight;

    // PS2 doesn't support windowed mode...
    FullScreenWidth = width;
    FullScreenHeight = height;

    SetRect(&TG_client_rect, 0, 0, width, height);

    // no actual mode switch done here

    RestoreDevice();

#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("SetMode complete\n");
    PS2Log.Flush();
#endif

    return success;
}

BOOL VideoSystemPS2::SetGamma(float contrast, float gamma, float brightness)
{
    // for now, gamma control won't be available (go adjust the TV)
    return TRUE;
}

void VideoSystemPS2::GetOriginalGamma()
{
}

BOOL VideoSystemPS2::RestoreOriginalGamma()
{
    SetGamma(0.5f, 1.0f,0.5f);
    return TRUE;
}

BOOL VideoSystemPS2::ClearZ(float z)
{
//    glClearDepth(z); // no good on ps2gl
//    glClearStencil(0); // no stencil on PS2
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    return TRUE;
}

BOOL VideoSystemPS2::ClearFB(DWORD argb)
{
    GLColor col(argb);
    glClearColor(col.R, col.G, col.B, col.A);
    glClear(GL_COLOR_BUFFER_BIT);
    return TRUE;
}

BOOL VideoSystemPS2::SetViewport(int x, int y, int w, int h, float zNear, float zFar)
{
    glViewport(x, y, w, h);
    glDepthRange(zNear, zFar);
    return TRUE;
}

BOOL VideoSystemPS2::SetScissor(int x1, int y1, int x2, int y2)
{
    glScissor(x1, y1, x2-x1, y2-y1);
    return TRUE;
}

BOOL VideoSystemPS2::Lock(Viewport& viewport)
{
    OriginalViewport = viewport;
    SetViewport(OriginalViewport.XOffset, OriginalViewport.YOffset,
                OriginalViewport.Width, OriginalViewport.Height,
                OriginalViewport.zNear, OriginalViewport.zFar);
    GLbitfield clearMask = 0;
    if (viewport.ClearFlags == EClearAll)
        clearMask = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT;
    else {
        if (viewport.ClearFlags & EClearColor)
            clearMask |= GL_COLOR_BUFFER_BIT;
        if (viewport.ClearFlags & EClearZ)
            clearMask |= GL_DEPTH_BUFFER_BIT;
        if (viewport.ClearFlags & EClearStencil)
            clearMask |= GL_STENCIL_BUFFER_BIT;
    }
    if (clearMask) {
        GLColor col(viewport.ClearColor);
        glClearColor(col.R, col.G, col.B, col.A);
//        glClearDepth(1.0f); // no good on ps2gl
//        glClearStencil(0); // no stencil on PS2
        glClear(clearMask);
    }
    SetCameraTransform(&viewport.WorldToCamera);
    SetScreenTransform(&viewport.CameraToScreen);
    StencilSurfaceTouched = FALSE;

    return TRUE;
}

void VideoSystemPS2::Unlock()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void VideoSystemPS2::Flip(HWND hwnd)
{
    glFlush();
    pglEndGeometry();
    if (is_render)
        pglFinishRenderingGeometry( PGL_DONT_FORCE_IMMEDIATE_STOP );

    if (waitVBlank)
        pglWaitForVSync();

    pglSwapBuffers();
    pglRenderGeometry();
    is_render = 1;
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("Flip complete\n");
    PS2Log.Flush();
#endif
    pglBeginGeometry();
}

void VideoSystemPS2::TakeScreenshot(const char* filename)
{
}

void VideoSystemPS2::ReleaseShaders()
{
}

void VideoSystemPS2::RestoreShaders()
{
}

void VideoSystemPS2::Flush()
{
}


// RenderDevice interface
void VideoSystemPS2::CreateTexture(Texture* texture)
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

#if 0
    case Texture::ETexRenderTarget:
        CreateRenderTarget(texture);
        break;

    case Texture::ETexDepthTarget:
        CreateDepthSurface(texture);
        break;
#endif

#if 0
    case Texture::ETexNormCubeMap:
        if (texture->Width)
            CreateNormCubeMap(texture);
        break;
#endif
    }
    if (texture->DeviceId == EInvalidIndex) {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Could not create texture type %d, flags 0x%x\n", texture->Type, texture->Flags);
#endif
        //PS2Log.Flush();
        return;
    }
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("CreateTexture returns %d\n", texture->DeviceId);
#endif
    //PS2Log.Flush();
}

void VideoSystemPS2::SetFBRenderTarget()
{
}

void VideoSystemPS2::SetRenderTarget(Texture* colorBuf, Texture* depthBuf, int cubeFace)
{
}

BOOL VideoSystemPS2::SetRenderTarget(int Index, Texture* ColorBuf, int CubeFace)
{
    return TRUE;
}

BOOL VideoSystemPS2::SetDepthTarget(Texture* DepthBuf)
{
    return TRUE;
}

int VideoSystemPS2::CreateStream(void* sourceData, DWORD type, int count, int stride)
{
    int index = EInvalidIndex;
    void *Mem = NULL;
    int Size = 0;

    // the PS2 doesn't support indexed rendering, but we're
    // going to try to emulate 16-bit indices in software.
    if (type & RenderDevice::EIndex16) {
    }
    else if (type & RenderDevice::EIndex32) {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("trying to use 32-bit indices\n");
#endif
        Assert(0);
    }

    Size = count*stride;

    if (sourceData) {
        // This is a static buffer.
        // We'll try to just stream straight from sourceData
        // to save system RAM.
        Mem = NULL;
    } else {
        // This is a dynamic buffer.
#ifdef TRANSGAMING
        m_setTag("CreateStream");
#endif
        Mem = BufferAllocate(Size);
        if (!Mem) {
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("Stream allocation failure\n");
#endif
            Assert(Mem);
            return EInvalidIndex;
        }
    }

    // find unused slot
    PS2Buffer* buf=0;
    for (int i=0; i<Buffers.Size(); ++i)
    {
        if(Buffers[i].Type==PS2Buffer::EUnusedStream)
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

    if(type & RenderDevice::EIndex16 || type & RenderDevice::EIndex32) 
    {
        buf->Type=PS2Buffer::EIndexStream;

        if (Mem)
            TextureMemory[EMemIBs] += (float)Size/(1024.f*1024.f);
    } 
    else 
    {
        buf->Type=PS2Buffer::EVertexStream;

        if (Mem)
            TextureMemory[EMemVBs] += (float)Size/(1024.f*1024.f);
    }

    buf->Ptr        = Mem;
    buf->ElemCount  = count;
    buf->Stride     = stride;
    buf->SourceData = sourceData;

#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("CreateStream returns %d (ptr %p, source %p)\n", index, Mem, sourceData);
#endif
    //PS2Log.Flush();
    return index;
}


BYTE* VideoSystemPS2::LockStream(int streamId, int pos, int count)
{
    const PS2Buffer& buf = Buffers[streamId];
    // only dynamic buffers can be locked
    if (!buf.SourceData) {
        if (!pos) {
            // We're about to overwrite the vertex buffer,
            // so we need to synchronize with the DMAC somehow,
            // to ensure we're not overwriting data the VU1 isn't
            // done with.

            // For now, we'll take the easy way out.
        }
        //PS2Log.Print("LockStream succeeded on %d (pos=%d)\n", streamId, pos);
        // Do we want to return an uncached pointer?
        BYTE* Ptr = (BYTE*)buf.Ptr + pos*buf.Stride;
#ifdef STREAM_UNCACHED
        return Core::MakePtrUncached(Ptr);
#else
        return Ptr;
#endif
    }
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("LockStream failed on %d\n", streamId);
#endif
    //PS2Log.Flush();
    return 0;
}

void VideoSystemPS2::UnlockStream(int streamId)
{
    const PS2Buffer& buf = Buffers[streamId];
    // only dynamic buffers can be locked
    if (!buf.SourceData) {
#ifndef STREAM_UNCACHED
        // If LockStream returned a cached pointer,
        // we need to flush the data written to memory,
        // so that the DMAC will see the updated data.
        FlushCache(0);
#endif
    }
}

int VideoSystemPS2::CreateShader(int type, const DWORD* source, const DWORD* streamTypeList, int listCount)
{
    if (type==EShaderPixel) {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Attempted to create pixel shader\n");
#endif
        return EInvalidIndex;
    }

    bool clip = false;
    PS2Shader::ShaderInput input[PS2Shader::ECount];
    memset(input, 0, sizeof(input));

    if (EShaderVertex == type)
    {
        if (!source)
        {
            int tex=0;
    
            for (int i=0; i<listCount; ++i) 
            {
                const DWORD streamType = streamTypeList[i];
#ifdef TRANSGAMING_LOGGING
                //PS2Log.Print("adding Type: %08x\n",streamType);
#endif

                int offset = 0;

                if (streamType & EVertexPos) 
                {
                    //TRACE("VideoSystemPS2::CreateShader(): adding vertex position to stream %d at offset %d\n", i, offset);
                    input[PS2Shader::EPosition].Stream = i;
                    input[PS2Shader::EPosition].Offset = offset;
                    input[PS2Shader::EPosition].Size = 3;
                    input[PS2Shader::EPosition].Type = GL_FLOAT;
                    offset += sizeof(float)*3;
                }
                if (streamType & EVertexPos4) 
                {
                    //TRACE("VideoSystemPS2::CreateShader(): adding vertex position to stream %d at offset %d\n", i, offset);
                    input[PS2Shader::EPosition].Stream = i;
                    input[PS2Shader::EPosition].Offset = offset;
                    input[PS2Shader::EPosition].Size = 4;
                    input[PS2Shader::EPosition].Type = GL_FLOAT;
                    offset += sizeof(float)*4;
                }
                if (streamType & EVertexDiffuse)    
                {
                    //TRACE("VideoSystemPS2::CreateShader(): adding vertex color to stream %d at offset %d\n", i, offset);
                    input[PS2Shader::EDiffuse].Stream = i;
                    input[PS2Shader::EDiffuse].Offset = offset;
                    input[PS2Shader::EDiffuse].Size = 4;
                    input[PS2Shader::EDiffuse].Type = GL_UNSIGNED_BYTE;
                    offset += 4;
                }
                if (streamType & EVertexNormal)        
                {
                    //TRACE("VideoSystemPS2::CreateShader(): adding vertex normal to stream %d at offset %d\n", i, offset);
                    input[PS2Shader::ENormal].Stream = i;
                    input[PS2Shader::ENormal].Offset = offset;
                    input[PS2Shader::ENormal].Size = 3;
                    input[PS2Shader::ENormal].Type = GL_FLOAT;
                    offset += sizeof(float)*3;
                }

                int texCount=(streamType & ETexCountMask)>>ETexCountShift;
                for (int j=0; j<texCount; ++j)
                {
                    //TRACE("VideoSystemPS2::CreateShader(): adding vertex texture coord %d to stream %d at offset %d\n", tex, i, offset);
                    input[PS2Shader::ETex0+tex].Stream = i;
                    input[PS2Shader::ETex0+tex].Offset = offset;
                    input[PS2Shader::ETex0+tex].Size = 2;
                    input[PS2Shader::ETex0+tex].Type = GL_FLOAT;
                    offset += sizeof(float)*2;
                    tex++;
                }
            }
            if (tex > 1) {
                // we're going to clip vertices that has multiple texture coordinates;
                clip = true;
            }
        }
        else {
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("CreateShader: Attempting to create programmable vertex shader - not supported yet\n");
#endif
            return EInvalidIndex;
        }
    }

    // check cached shaders
    int index = EInvalidIndex;
    for (int i=0; i<Shaders.Size(); ++i) 
    {
        if (Shaders[i].Type != type)
            continue;

        BOOL same=TRUE;
        // for fixed or vertex shader - compare declarator
        if (type == EShaderVertex)
        {
            if (memcmp(input, Shaders[i].input, sizeof(input)) != 0 ||
                clip != Shaders[i].Clip)
                same = FALSE;
        }

        if (same) 
        {
            if (source)
            {
#if 0
                for (int t=0; D3DVS_END()!=source[t]; ++t)
                    if (source[t]!=Shaders[i].Source[t])
                    {
                        same = FALSE;
                        break;
                    }
#endif
                same = FALSE;
            }

            if (same)
            {
                Assert(Shaders[i].Type==type);
                //D3DLog.Print("CreateShader(): cached (%d): %d\n",type,Shaders[i].Handle);
                index = i;
                break;
            }
        }
    }

    if (EInvalidIndex == index) // need to create a new one
    {
        PS2Shader* shader=NULL;
        index = Shaders.Size();

        if (type == EShaderVertex) 
        {
#ifdef TRANSGAMING
            index = Shaders.GrowArray(1);

            shader = &Shaders[index];
#else
            shader = Shaders.CreateArray();
#endif
            shader->Type    = EShaderVertex;
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("CreateShader: created fixedfunction vertex shader\n");
#endif
        }
        else if (type == EShaderPixel)
        {
            return EInvalidIndex;
        }
        else
        {
            Assert(false);
            return EInvalidIndex;
        }

        // remember decl and source for programmable shaders
        if (type == EShaderVertex)
        {
            memcpy(shader->input, input, sizeof(input));
            shader->Clip = clip;
        }

#if 0
        if (source)
        {
            int i=0;
            do {
                shader->Source.Add(source[i]);
            }
            while (source[i++] != D3DVS_END());
        }
#endif
    }


    /*
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("CreateShader[%d]: %s (%d)\n",
                index, (type==EShaderVertex ? "vertex" : "pixel"),
                Shaders[index].Handle);
#endif
                */

    return index;
}

void VideoSystemPS2::SetState(const RenderState& state)
{
    // local translation RenderDevice->GL tables
    static const GLenum CompareFuncMap[] = 
    {
        GL_NEVER,           // EFuncNever
        GL_LESS,            // EFuncLess
        GL_EQUAL,           // EFuncEqual
        GL_LEQUAL,          // EFuncLessEqual
        GL_GREATER,         // EFuncGreater
        GL_NOTEQUAL,        // EFuncNotEqual
        GL_GEQUAL,          // EFuncGreaterEqual
        GL_ALWAYS           // EFuncAlways
    };

    static const GLenum FillModeMap[] =
    {
        GL_FILL,            // EFillSolid      = 0,
        GL_LINE,            // EFillLines      = 1,
        GL_POINT            // EFillPoints     = 2,
    };

    // Depth buffer/function related
    glDepthFunc(CompareFuncMap[state.DepthFunc]);
    CachedState.DepthFunc=state.DepthFunc;

    // Depth test will be set in ActivateStage
    CachedState.DepthTest=state.DepthTest;

	// Depth write
    glDepthMask(state.DepthWrite);
    CachedState.DepthWrite=state.DepthWrite;

	// Color mask
    glColorMask(state.ColorWrite, state.ColorWrite, state.ColorWrite, state.AlphaWrite);
    CachedState.ColorWrite=state.ColorWrite;
    CachedState.AlphaWrite=state.AlphaWrite;

    // stencil buffer/function related
    // skipped, the GS doesn't support stencil

    // Alpha test
//    (state.AlphaTest ? glEnable : glDisable)(GL_ALPHA_TEST);
    CachedState.AlphaTest=state.AlphaTest;

	// Alpha func and ref
    const float inv = 1/255.f;
    glAlphaFunc(CompareFuncMap[state.AlphaFunc], (state.AlphaRef+0.5f)*inv);
    CachedState.AlphaFunc=state.AlphaFunc;
    CachedState.AlphaRef=state.AlphaRef;

    // Scissor test
	// [alex] PS2GL doesn't support it!
    //(state.ScissorTest ? glEnable : glDisable)(GL_SCISSOR_TEST);
    //CachedState.ScissorTest = state.ScissorTest;

    // Blend modes (inflict fogColor!)
    DWORD fogColor = CachedState.FogColor;

    switch(state.BlendMode)
    {
    case RenderState::EBlendNone:                fogColor = SavedFogColor;   break;
    case RenderState::EBlendAlpha:                fogColor = 0;                break;
    case RenderState::EBlendAdd:                fogColor = 0;                break;
    case RenderState::EBlendTranslucent:        fogColor = SavedFogColor;   break;
    case RenderState::EBlendModulate:            fogColor = (DWORD) -1;      break;
    case RenderState::EBlendModulate2x:            fogColor = (DWORD) -1;      break;
    case RenderState::EBlendFilter:                fogColor = (DWORD) -1;      break;
    case RenderState::EBlendInvModulate:        fogColor = 0;                break;
    case RenderState::EBlendSubtract:            fogColor = 0;                break;
    case RenderState::EBlendRevSubtract:        fogColor = 0;                break;
    case RenderState::EBlendDestTranslucent:    fogColor = SavedFogColor;    break;
    case RenderState::EBlendDestAlpha:            fogColor = 0;                break;    
    }

	// Fog color
	// [alex] PS2GL doesn't support it!
    //glFogfv(GL_FOG_COLOR, GLColor(fogColor));
    CachedState.FogColor=fogColor;


	// Blend mode 
    {
        // will be set in ActivateStage
        CachedState.BlendMode=state.BlendMode;
    }

    // ignore state.Lighting, it's always false
    // ignore state.AmbientColor, it's always zero

	// Normalize normals
    (state.NormalizeNormals ? glEnable : glDisable)(GL_NORMALIZE);
    CachedState.NormalizeNormals=state.NormalizeNormals;

	// Specular state
    // the GS doesn't support specular...
    CachedState.SpecularEnable=state.SpecularEnable;

	// Fill mode
    glPolygonMode(GL_FRONT_AND_BACK, FillModeMap[state.FillMode]);
    CachedState.FillMode=state.FillMode;

	// Cull mode
    int CullMode = state.CullMode;
    if (CullFlip && CullMode != RenderState::ECullNone)        
        CullMode = (CullMode == RenderState::ECullCCW) ? RenderState::ECullCW : RenderState::ECullCCW;
        
	// [alex] PS2GL doesn't support it!
    //switch(CullMode)
    //{
    //case RenderState::ECullNone:
    //    glDisable(GL_CULL_FACE);
    //    break;

    //case RenderState::ECullCW:
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_FRONT);
    //    break;

    //case RenderState::ECullCCW:
    //    glEnable(GL_CULL_FACE);
    //    glCullFace(GL_BACK);
    //    break;
    //}

	// Constant color
    {
        // Without texture_env_combine, the tex env color is not directly useful,
        // and in order to have working GL_BLEND, we don't want to change it.
//        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GLColor(state.ConstColor));
        CachedState.ConstColor=state.ConstColor;
    }
}

void VideoSystemPS2::SetObjectTransform(const Matrix* matPtr)
{
		mat_obj = *matPtr;
        glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPushMatrix();
        // FIXME: load light positions here?
        glMultMatrixf(mat_obj);
}

void VideoSystemPS2::SetCameraTransform(const Matrix* matPtr)
{
	int StackDepth;

    mat_cam = *matPtr;
    glMatrixMode(GL_MODELVIEW);
    // FIXME: load light positions here?

	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &StackDepth);

	// Evil hack !!!! Make sure the matrix stack is cleared.
	while (StackDepth > 1)
	{
		glPopMatrix();
		StackDepth--;
	};

	glLoadMatrixf(mat_cam);
	glPushMatrix();
}

void VideoSystemPS2::SetScreenTransform(const Matrix* matPtr)
{
    mat_proj = *matPtr;
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(mat_proj);
}

void VideoSystemPS2::SetBoneMatrix(int index, const Matrix *matrix){
    //!!// implement me!
}

void VideoSystemPS2::SetStreams(const int* streams, int streamCount, int indexStream, int streamOffset)
{
    //the streamOffset parameter seems to be obsolete now.  Its source never seems to be assigned
    //a value in used code any more.
    Assert(streamOffset == 0);


    int index;
    for (index=0; index<streamCount; ++index)
    {
        const PS2Buffer& buf=Buffers[streams[index]];
        Assert(buf.ElemCount==Buffers[streams[(index+1)%streamCount]].ElemCount);

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

void VideoSystemPS2::SetShaderMemory(DWORD addr, const void* data, int count)
{
}

void VideoSystemPS2::SetPixelShaderMemory(DWORD addr, const void* data, int count)
{
}

void VideoSystemPS2::SetPixelShaderMemoryB(DWORD addr, const BOOL* data, int count)
{
}

void VideoSystemPS2::SetShader(int shaderIndex, int shaderType)
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
            }
            else
                vshader[shaderType - EShaderVertexBase] = NULL;

            break;

        case EShaderPixel:
        default:
#ifdef TRANSGAMING_LOGGING
            if (shaderIndex != EInvalidIndex)
                TRACE("VideoSystemPS2::SetShader(): ERROR-> set unsupported shader type (index=%d, type=%d)\n", shaderIndex, shaderType);
#endif
            break;
    }
}

void VideoSystemPS2::SetTextureState(const TextureState* stages, int stageCount)
{
    if (!stages)
        return;

    //PS2Log.Print("SetTextureState\n");

    for (int i=0; i<stageCount; i++) {
        const TextureState& t = stages[i];

#ifdef TRANSGAMING_LOGGING
        if (t.Stage > 0 && t.Op && t.Op != TextureState::ETexOpDisable) {
//            PS2Log.Print("Multitexturing attempted\n");
        }
#endif
		// [alex] Uncommenting the line below will turn off multiple texture stage states
        //if (t.Stage > 0) continue;

        if (t.TexId) {
            // set in ActivateStage
            CachedTextureStates[t.Stage].TexId=t.TexId;
        }

        if (t.QVal) {
            // compare first part of tex state settings
            if (t.V1 && t.V1 != CachedTextureStates[t.Stage].V1) {
                if(t.Op) {
                    CachedTextureStates[t.Stage].Op=t.Op;
                }

                // tex arg
                if(t.Arg) {
                    CachedTextureStates[t.Stage].Arg=t.Arg;
                }

                // alpha op
                if(t.AlphaOp) {
                    CachedTextureStates[t.Stage].AlphaOp=t.AlphaOp;
                }

                // alpha arg
                if(t.AlphaArg) {
                    CachedTextureStates[t.Stage].AlphaArg=t.AlphaArg;
                }
            }

            // compare second part of tex state settings
            if (t.V2) {
                // filter state
                if(t.Filter) {
                    CachedTextureStates[t.Stage].Filter=t.Filter;
                }

                // addr mode
                if (t.AddrMode) {
                    CachedTextureStates[t.Stage].AddrMode=t.AddrMode;
                }



                if ((t.Transform & 0xf) > 1)
#ifdef TRANSGAMING_LOGGING
                    PS2Log.Print("TexTransform requested\n");
#endif
#if 0
                    int val = D3DTTFF_DISABLE;

                    if (HWClass==EHWClassGenericTnL)
                        switch (t.Transform & 0xf) 
                        {
                            case TextureState::ETexTransformNone:
                                val = D3DTTFF_DISABLE;
                                break;
                            case TextureState::ETexTransform1:
                                val = D3DTTFF_COUNT1;
                                break;
                            case TextureState::ETexTransform2:
                                val = D3DTTFF_COUNT2;
                                break;
                            case TextureState::ETexTransform3:
                                val = D3DTTFF_COUNT3;
                                break;
                            case TextureState::ETexTransform4:
                                val = D3DTTFF_COUNT4;
                                break;
                        }

                    if (t.Transform & TextureState::ETexTransformProj)
                        val |= D3DTTFF_PROJECTED;

                    d3dev->SetTextureStageState(t.Stage, D3DTSS_TEXTURETRANSFORMFLAGS, val);
#endif
                CachedTextureStates[t.Stage].Transform = t.Transform;

                if (t.CoordIndex) {
                    int val = (t.CoordIndex-1) & 0x7;

                    if (t.CoordIndex >= 0x10)
#ifdef TRANSGAMING_LOGGING
                        PS2Log.Print("TexGen requested\n");
#endif
#if 0
                        if (t.CoordIndex >= 0x10) {
                            switch (t.CoordIndex & 0xe0) {
                                case TextureState::ETexCoordCameraSpaceNormal: 
                                    val |= D3DTSS_TCI_CAMERASPACENORMAL;
                                    break;

                                case TextureState::ETexCoordCameraSpacePos:
                                    val |= D3DTSS_TCI_CAMERASPACEPOSITION;
                                    break;

                                case TextureState::ETexCoordCameraSpaceReflect:
                                    val |= D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
                                    break;
                            }
                        }

                        d3dev->SetTextureStageState(t.Stage,D3DTSS_TEXCOORDINDEX,val);
#endif
                    CachedTextureStates[t.Stage].CoordIndex = t.CoordIndex;
                }
            }
        }

#if 0
            if (HWClass==EHWClassGenericTnL)
            {
                if ((t.Transform & 0xf) != TextureState::ETexTransformNone)
                {
                    Matrix m = t.MatStore.GetTranspose();

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

                    d3dev->SetTransform(D3DTRANSFORMSTATETYPE (D3DTS_TEXTURE0+t.Stage), _DXMAT(m));
                    CachedTextureStates[t.Stage].Xform = t.Xform;
                }
                else
                {
                    d3dev->SetTransform(D3DTRANSFORMSTATETYPE (D3DTS_TEXTURE0+t.Stage), _DXMAT(*GIdentityMatrix));
                }
            }
#endif

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

void VideoSystemPS2::ResetTextureStages()
{
}

void VideoSystemPS2::SetFog(bool isFog)
{
}

void VideoSystemPS2::SetFogState(const FogState& state)
{
}

void VideoSystemPS2::SetLight(int number, const LightState& state)
{
}

void VideoSystemPS2::SetSpecular(const Vector& color, const float power)
{
    GLColor matcol(color.X, color.Y, color.Z);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matcol);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, power);
}

void VideoSystemPS2::SetLighting(bool isLighting)
{
    // set in ActivateStage
    Lighting = isLighting;
}

void VideoSystemPS2::SetAmbientLight(const Vector& color)
{
    GLColor AmbientColor(color);

	// [alex] Tweak the ambient to make animated meshes a bit lighter
	AmbientColor.R *=2;
	AmbientColor.G *=2;
	AmbientColor.B *=2;

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientColor);
}

void VideoSystemPS2::SetNoLight(int number)
{
    if (number >= 4) {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Setting more than 4 lights not supported\n");
#endif
        return;
    }

    if (LightEnable[number]) {
        glDisable(GL_LIGHT0+number);
        LightEnable[number] = false;
    }
}

void VideoSystemPS2::SetDirLight(int number, const Plane& dir, const Vector& color)
{
    if (number >= 4) {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Setting more than 4 lights not supported\n");
#endif
        return;
    }

    GLColor col(color);

    if (col == black) {
        if (LightEnable[number]) {
            glDisable(GL_LIGHT0+number);
            LightEnable[number] = false;
        }
        return;
    }

    glLightfv(GL_LIGHT0+number, GL_DIFFUSE, col);
    glLightfv(GL_LIGHT0+number, GL_SPECULAR, col);
    //glLightfv(GL_LIGHT0+number, GL_AMBIENT, black);

	glLightfv(GL_LIGHT0+number, GL_POSITION, 
			 (const GLfloat *)&dir.X);

    if (!LightEnable[number]) {
        glEnable(GL_LIGHT0+number);
        LightEnable[number] = true;
    }
}

bool VideoSystemPS2::ActivateStage(int stage)
{
    TextureState &t = CachedTextureStates[stage];

    bool ret;

    if (stage >= 7)
        ret = false; // no more stages
    else
        ret = (CachedTextureStates[stage+1].Op != TextureState::ETexOpDisable);

	// [alex] This should be forced in VideoSystemPS2::SetTextureState
    //ret = false; // force one stage for the moment

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

    PS2Texture *tex = (t.TexId>0 && t.TexId<Textures.Size() ? &Textures[t.TexId] : NULL);
    bool enable = tex ? (tex->Tex != 0) : false;
//    bool enable = false;
    bool cdbl = false;
    bool light = false;
    bool ztest = false;
    GLenum func = 0;
    GLColor color = white;
    int blend = RenderState::EBlendNone;

    glBindTexture(GL_TEXTURE_2D, tex ? tex->Tex : 0);
    //TRACE("VideoSystemPS2::ActivateStage(): bound texture #%u\n", tex ? tex->Tex : 0);

#ifdef DEBUG
    /*else
        TRACE("VideoSystemPS2::ActivateStage(): texture #%u is already bound\n", tex ? tex->Tex : 0);//*/
#endif

    switch(Op) {
    case TextureState::ETexOpTexture:
        func = GL_REPLACE;
        break;
    default:
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("SetTextureState using unsupported op %d\n", t.Op);
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
        func = GL_MODULATE;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = GL_REPLACE;
            blend = RenderState::EBlendModulate;
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("Modulate with previous\n");
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
            PS2Log.Print("Modulate with unknown arg %d\n", Arg);
#endif      
            break;
        }
        break;
#if 0 /* not available */
    case TextureState::ETexOpAdd:
        func = GL_ADD;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = GL_REPLACE;
            blend = RenderState::EBlendAdd;
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("Add with previous\n");
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
            PS2Log.Print("Add with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
#endif
    case TextureState::ETexOpBlendTexture:
        func = GL_DECAL;
        switch (Arg) {
        case TextureState::ETexArgPrevious:
            // FIXME (blend using composite buffer)
            func = GL_REPLACE;
            blend = RenderState::EBlendTranslucent;
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("Blend with previous\n");
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
            PS2Log.Print("Blend with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    case TextureState::ETexOpPreviousModulate:
        // FIXME (blend using composite buffer)
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Previous Modulate\n");
#endif
        enable = false;
        blend = RenderState::EBlendModulate;
        switch (Arg) {
        case TextureState::ETexArgTexture:
            enable = true;
            func = GL_REPLACE;
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
            PS2Log.Print("Previous Modulate with unknown arg %d\n", Arg);
#endif
            break;
        }
        break;
    }

    // FIXME: munge blend modes a little more
    ztest = CachedState.DepthTest;
    if (!ret) {
        // last stage
        if (blend != RenderState::EBlendNone) {
#ifdef TRANSGAMING_LOGGING
            if (CachedState.BlendMode != RenderState::EBlendNone)
                PS2Log.Print("Blend mode problem\n");
#endif
	    }
	    else {
            blend = CachedState.BlendMode;
        }
    }

    if (enable) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, func);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    // FIXME: filter, addr mode

    glColor4fv(color);
    CachedColor = color; // Used in DrawArray function

    (light ? glEnable : glDisable)(GL_LIGHTING);
    CachedState.Lighting = light;

    (ztest ? glEnable : glDisable)(GL_DEPTH_TEST);

    switch(blend)
    {
    case RenderState::EBlendNone:
        glDisable(GL_BLEND);
        break;

    case RenderState::EBlendAlpha:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;

    case RenderState::EBlendAdd:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        break;

    case RenderState::EBlendTranslucent:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;

    case RenderState::EBlendModulate:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
		// [alex] Temporary hack to blend the lightmap texture
		// It uses a tweaked GL_DST_COLOR, GL_SRC_COLOR
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
        break;

    case RenderState::EBlendModulate2x:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
        break;

    case RenderState::EBlendFilter:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
        break;

    case RenderState::EBlendInvModulate:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        break;

    case RenderState::EBlendSubtract:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_SUBTRACT);
//            glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        break;

    case RenderState::EBlendRevSubtract:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT);
//            glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        break;

    case RenderState::EBlendDestTranslucent:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        break;

    case RenderState::EBlendDestAlpha:
        glEnable(GL_BLEND);
//            glBlendEquationEXT(GL_FUNC_ADD);
        glBlendFunc(GL_DST_ALPHA, GL_ONE);
        break;
    }

    return ret;
}

int VideoSystemPS2::GetStream(int cidx, int base, VertexStreamState* streams, BYTE*& ptr)
{
    const PS2Buffer *Buf = streams[cidx].Buf;
    int Offset = streams[cidx].Offset;
    int stride = Buf->Stride;
    ptr = (BYTE*)(Buf->Ptr ? Buf->Ptr : Buf->SourceData);
    ptr += Offset + base*stride;
    return stride;
}

void VideoSystemPS2::DrawArray(int prim, int base, int count, void* idx, VertexStreamState* streams)
{
    int stage = 0;
    bool next;
    do {
        int cidx = (CachedTextureStates[stage].CoordIndex&7)-1;

        next = ActivateStage(stage);

        int baseidx = idx ? base : 0;

        if (vshader[0]->input[PS2Shader::EPosition].Type) {
            BYTE* Ptr;
            int Stream = vshader[0]->input[PS2Shader::EPosition].Stream;
            int Stride = GetStream(Stream, baseidx, streams, Ptr);
            Ptr += vshader[0]->input[PS2Shader::EPosition].Offset;
            glVertexPointer(vshader[0]->input[PS2Shader::EPosition].Size,
                            vshader[0]->input[PS2Shader::EPosition].Type,
                            Stride, Ptr);
            glEnableClientState(GL_VERTEX_ARRAY);
        }
        else {
            glDisableClientState(GL_VERTEX_ARRAY);
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("no vertex position - shouldn't happen\n");
#endif
        }

        if (vshader[0]->input[PS2Shader::ENormal].Type) {
            BYTE* Ptr;
            int Stream = vshader[0]->input[PS2Shader::ENormal].Stream;
            int Stride = GetStream(Stream, baseidx, streams, Ptr);
            Ptr += vshader[0]->input[PS2Shader::ENormal].Offset;
            glNormalPointer(vshader[0]->input[PS2Shader::ENormal].Type,
                            Stride, Ptr);
            glEnableClientState(GL_NORMAL_ARRAY);
        }
        else {
            glDisableClientState(GL_NORMAL_ARRAY);
            glNormal3f(0.0f, 0.0f, 0.0f);
        }

        if (vshader[0]->input[PS2Shader::EDiffuse].Type && FALSE) {
            BYTE* Ptr;
            int Stream = vshader[0]->input[PS2Shader::EDiffuse].Stream;
            int Stride = GetStream(Stream, baseidx, streams, Ptr);
            Ptr += vshader[0]->input[PS2Shader::EDiffuse].Offset;
            glColorPointer(vshader[0]->input[PS2Shader::EDiffuse].Size,
                           vshader[0]->input[PS2Shader::EDiffuse].Type,
                           Stride, Ptr);
            glEnableClientState(GL_COLOR_ARRAY);
        }
        else {
            glDisableClientState(GL_COLOR_ARRAY);
            glColor4fv(CachedColor);
        }

        if (vshader[0]->input[PS2Shader::ETex0+cidx].Type) {
            BYTE* Ptr;
            int Stream = vshader[0]->input[PS2Shader::ETex0+cidx].Stream;
            int Stride = GetStream(Stream, baseidx, streams, Ptr);
            Ptr += vshader[0]->input[PS2Shader::ETex0+cidx].Offset;
            glTexCoordPointer(vshader[0]->input[PS2Shader::ETex0+cidx].Size,
                              vshader[0]->input[PS2Shader::ETex0+cidx].Type,
                              Stride, Ptr);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        else {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoord2f(0.0f, 0.0f);
        }

        if (idx) {
            // glDrawRangeElements would probably be useful, if it existed
#if 1
            glDrawElements(prim, count, GL_UNSIGNED_SHORT, idx);
#else
            glBegin(prim);
            for (int ct=0; ct<count; ct++) {
                int id = ((WORD*)idx)[ct];
                glArrayElement(id);
#if 0
                if ((prim == GL_TRIANGLES) && ((ct%3) == 2)) {
                    glEnd();
                    glBegin(prim);
                }
#endif
            }
            glEnd();
#endif
        } else {
            glDrawArrays(prim, base, count);
        }
        stage++;
    } while (next);
}

void VideoSystemPS2::Draw(const int primType, int base, int count, int vbase, int vcount, int basevindex, bool swapPath)
{
    int prim;
    int icount;
    int lighting;
    DWORD color;

    switch (primType)
    {
    case RenderDevice::EPointList:     prim = GL_POINTS;         icount = count;   break;
    case RenderDevice::EPrimTriList:   prim = GL_TRIANGLES;      icount = count*3; break;
    case RenderDevice::EPrimTriStrip:  prim = GL_TRIANGLE_STRIP; icount = count+2; break;
    case RenderDevice::EPrimTriFan:    prim = GL_TRIANGLE_FAN;   icount = count+2; break;
    case RenderDevice::EPrimLineList:  prim = GL_LINES;          icount = count*2; break;
    case RenderDevice::EPrimLineStrip: prim = GL_LINE_STRIP;     icount = count+1; break;
    default: return;
    }

    BYTE* IPtr = 0;

    if (ibuffer) {
        IPtr = (BYTE*)(ibuffer->Ptr ? ibuffer->Ptr : ibuffer->SourceData);
        IPtr += base*ibuffer->Stride;
        // The PS2 can't do indexed rendering in hardware. Perhaps we'll
        // want to change the engine to avoid indexed geometry, if so we'll
        // want to emit warnings here. However, since available RAM
        // is quite limited, we probably don't want to.
    }

#if 0
    if (ibuffer) // loading screen don't use ibuffer, meshes do
    {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Rendering (%d, %d%s)! (vptr %p) l=%d a=%d op=%d t=%d s=%d z=%d b=%d\n",
            primType, icount, ibuffer ? ", indexed" : "", Ptr,
            Lighting,
            CachedState.AlphaTest,
            CachedTextureStates[0].Op,
            CachedTextureStates[0].TexId,
            CachedState.StencilTest,
            CachedState.DepthTest,
            CachedState.BlendMode);
        //PS2Log.Flush();
#endif // TRANSGAMING_LOGGING
    }
#endif

    if (mat_dirty) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(mat_cam);
        // FIXME: load light positions here?
        glMultMatrixf(mat_obj);
    }

    pglEnable(PGL_CLIPPING); // ?

	if (vshader[0]->Clip && !Lighting) {
		pglEnable(PGL_SCISSORING);
	} else {
		pglDisable(PGL_SCISSORING);
	}

//#define VDUMP
#ifdef VDUMP
    Matrix mat0 = Matrix((const float*)&mat_obj);
    Matrix mat1 = mat0 * Matrix((const float*)&mat_cam);
    Matrix mat = mat1 * Matrix((const float*)&mat_proj);
    BYTE* Ptr;
    int Stream = vshader[0]->input[PS2Shader::EPosition].Stream;
    int Stride = GetStream(Stream, ibuffer ? basevindex : base, streams, Ptr);
    Ptr += vshader[0]->input[PS2Shader::EPosition].Offset;
    for (int iv=0; iv<icount; iv++) {
        if (iv >= 9) {
#ifdef TRANSGAMING_LOGGING
            PS2Log.Print(" ...\n");
#endif
            break;
        }
        int id = ibuffer ? (((WORD*)IPtr)[iv]) : iv;
        Vector* m = (Vector*)(Ptr + Stride*id);
        Vector p = Matrix((float*)&mat).Transform(*m);
        int x = (int)(p.X * 640) + 320;
        int y = (int)(-p.Y * 480) + 240;
        float z = p.Z;
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("[%04d] %8.02f %8.02f %8.02f => %5d %5d %8.02f\n", id, m->X, m->Y, m->Z, x, y, z);
#endif
    }
#endif
    DrawArray(prim, ibuffer ? basevindex : base, icount, IPtr, streams);

#ifdef TRANSGAMING_SHOW_FPS
    g_triCount += count;
    g_vertexCount += icount;
    g_objectCount++;
#endif
}

// ======================================================
// internal
// ======================================================

#define PK_MIPMAPLODBIAS    (-0.4F)
//#define PK_MIPMAPLODBIAS    (-0.0F)

void VideoSystemPS2::RestoreDevice()
{
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("Restore device\n");
#endif

    glFrontFace(GL_CW);
    glShadeModel(GL_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    pglEnable(PGL_CLIPPING);
    glDisable(GL_TEXTURE_2D);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_LIGHTING);
	// [alex] PS2GL doesn't support it!
    //glDisable(GL_FOG);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
	// [alex] PS2GL doesn't support it!
    //glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(GL_ALWAYS, 0.0f);
    glScissor(0, 0, 640, 480);
	// [alex] PS2GL doesn't support it!
    //glDisable(GL_SCISSOR_TEST);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
    glColor4fv(white);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);

    //glEnable(GL_NORMALIZE);

#if 0
    for (int number=0; number<4; number++) {
        glLightfv(GL_LIGHT0+number, GL_AMBIENT, black);
        glLightfv(GL_LIGHT0+number, GL_DIFFUSE, black);
        glLightfv(GL_LIGHT0+number, GL_SPECULAR, black);
        glLightf(GL_LIGHT0+number, GL_CONSTANT_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0+number, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0+number, GL_QUADRATIC_ATTENUATION, 0.0f);
        glDisable(GL_LIGHT0+number);
    }
#endif


    mat_obj.SetIdentity();  // = GIdentityMatrix;
    mat_cam.SetIdentity();  // = GIdentityMatrix;
    mat_proj.SetIdentity();  // = GIdentityMatrix;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    mat_dirty = false;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // setup cached state
    SavedFogColor = 0;
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

    CachedColor = white;
    CachedAmbientColor = black;
    CachedSpecularMat = white;
    CachedSpecularPow = 0.0f;
    CachedTexId = -1;
    CachedTexFunc = 0;
    CachedBlendMode = RenderState::EBlendNone;
    CachedFilter = 0;
    CachedAddrMode = 0;
    CachedTexEnable = false;
    CachedCDbl = false;
    CachedDepthTest = true;
}

void VideoSystemPS2::ReleaseDevice()
{
#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("Release device\n");
#endif
}

void VideoSystemPS2::SetConstColor(DWORD color)
{
	CachedState.ConstColor = color;
}

void VideoSystemPS2::ReleaseResource(int resourceType, const int* ids, int count)
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

            if (!Textures[index].Tex)
                continue;

            glDeleteTextures(1, &Textures[index].Tex);
            Textures[index].Tex = 0;
            if (Textures[index].Ptr)
			{
                BYTE* Ptr = (BYTE*)Textures[index].Ptr;
                delete[] Ptr;
                Textures[index].Ptr = NULL;
            }

/*#ifdef TRANSGAMING_LOGGING
            PS2Log.Print("released texture %d (%s)\n", index,
                         Textures[index].TexDef->FileName.AsChar());
#endif//*/
            //add the index to the free list
            FreeTextures.Add(index);
        }


        //go through the current list of textures and attempt to chop of all unused
        //textures at the end of the table.
        topTex = Textures.Size();

        for (int i = Textures.Size() - 1; i >= 0 ; i--){
            if (Textures[i].Tex != 0){
                topTex = i + 1;

                break;
            }
        }

        if (topTex < Textures.Size()){
            TRACE("VideoSystemPS2::ReleaseResource(): shrinking the main texture array from %d to %d\n", Textures.Size(), topTex);
            Textures.Shrink(topTex);
        }


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
            Assert(Textures[FreeTextures[i]].Tex == 0);
#endif
        }

        break;

    case EResBuffer:
        for (int i=0; i<count; ++i)
        {
            if (ids[i] == EInvalidIndex)
                continue;

            PS2Buffer& buf=Buffers[ids[i]];
            if (buf.Type == PS2Buffer::EUnusedStream)
                continue;

            int Size = buf.ElemCount * buf.Stride;

            if (buf.Ptr)
			{
                BYTE* Mem = (BYTE*)buf.Ptr;
                if (buf.Type & PS2Buffer::EIndexStream)   
                { 
                    TextureMemory[EMemIBs] -= (float)Size/(1024.f*1024.f);
                } 
                else if (buf.Type & PS2Buffer::EVertexStream)  
                { 
                    TextureMemory[EMemVBs] -= (float)Size/(1024.f*1024.f);
                }

                BufferFree(buf.Ptr);
                buf.Ptr = NULL;
            }
            buf.Type=PS2Buffer::EUnusedStream;
            buf.SourceData=0;
        }
        break;

    default:
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("Releasing unknown type %d\n", resourceType);
#endif
        break;
    }
}

RenderEffect* VideoSystemPS2::CreateEffect(DWORD Flags, const BYTE* EffectData, UINT Size)
{
    return NULL;
}

BOOL VideoSystemPS2::GrabBackBuffer(Texture* Tex, HWND hwnd)
{
    return TRUE;
}

BOOL VideoSystemPS2::GrabZStencilBuffer(Texture* Tex)
{
    return TRUE;
}

void VideoSystemPS2::GetBackBufferSize(UINT& SizeX, UINT& SizeY)
{
}

// ==========================================================================================================

static BYTE GetD3DType(BYTE Type)
{
    return 0;
}


/*int VideoSystemPS2::CreateVertexShader(const DWORD* Source, const VertexComponent* ComponentList)
{
    return 0;
}//*/

// ==========================================================================================================

int VideoSystemPS2::CreateQuery(int Type)
{
    return -1;
}

BOOL VideoSystemPS2::BeginQuery(int Query)
{
    return -1;
}

void VideoSystemPS2::EndQuery(int Query)
{
}

BOOL VideoSystemPS2::GetQueryData(int Query, void* OutData, int Size)
{
    return 0;
}

void VideoSystemPS2::SetCullFlip(BOOL Flip)
{
}

void VideoSystemPS2::SetVBlankState(bool state){
    waitVBlank = state;
}

bool VideoSystemPS2::GetVBlankState() const{
    return waitVBlank;
}


void VideoSystemPS2::SetColorWrite(BOOL ColorWrite, BOOL AlphaWrite)
{
}

// ==========================================================================================================

/*static void DumpCaps()
{
}//*/

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
void PS2Texture::SetParams(Texture* texDef, GLenum target, GLenum format, GLenum type, GLuint tex, int size, int locksize){
    //TexDef = texDef;
    Target = target;
    Format = format;
    Type = type;
    Tex = tex;
    Size = size;
    //LockSize = locksize;
    Ptr = 0;
    Pal = 0;
    LockPtr = 0;
    Locks = 0;
    V2 = 0;
}


int VideoSystemPS2::findFreeTexture(Texture *texDef, GLenum target, GLenum format, GLenum type, GLuint tex, int size, int locksize){
    int index = 0;


    //free list is empty => allocate a new texture and add it to the table
    if (FreeTextures.Size() == 0)
        index = Textures.Add(PS2Texture(texDef, target, format, type, tex, size, locksize));

    else{
        index = FreeTextures[0];
        FreeTextures.FastRemove(0);

        Textures[index].SetParams(texDef, target, format, type, tex, size, locksize);
    }


    return index;
}



/* I'm not aware of a standard PS2 texture format...
 * for now, I'll just load the original texture files */

struct ImageInfo
{
    UINT Width;
    UINT Height;
    UINT MipLevels;
    UINT BPP, A;
    DWORD Size;
    const BYTE* Pal;
    const BYTE* Data;
    void (*Decompress)(ImageInfo *info, BYTE* output);
};

struct TGAHeader
{
    BYTE idlength;
    BYTE colormaptype;
    BYTE imagetype;
    WORD colormapstart;
    WORD colormaplength;
    BYTE colormapbits;
    WORD xorigin;
    WORD yorigin;
    WORD width;
    WORD height;
    BYTE pixelbits;
    BYTE pixelfmt;
} __attribute__((packed));

unsigned get_pow2(unsigned l)
{
  unsigned t = l-1;
  t |= t>>1;
  t |= t>>2;
  t |= t>>4;
  t |= t>>8;
  t |= t>>16;
  return t+1;
}

static void LineColorfix(ImageInfo *info, int bpp, int pitch, BYTE* output, const BYTE* input)
{
    int count = pitch/bpp, extra = pitch - (count*bpp);
    switch (bpp) {
    // FIXME: colorswap for 16bpp
    case 3:
       while (count--) {
           output[2] = input[0];
           output[1] = input[1];
           output[0] = input[2];
           output += 3;
           input += 3;
       }
       break;
    case 4:
       while (count--) {
           output[2] = input[0];
           output[1] = input[1];
           output[0] = input[2];
           output[3] = input[3];
           output += 4;
           input += 4;
       }
       break;
    default:
        extra = pitch;
        break;
    }
    if (extra) {
        memcpy(output, input, extra);
    }
}

static void Colorfix(ImageInfo *info, BYTE* output)
{
    int bpp = (info->BPP+7)/8;
    int pitch = info->Width*bpp;
    const BYTE* ptr = info->Data;
    memcpy(output, ptr, info->Height * pitch);
    int y = info->Height;
    while (y--) {
        LineColorfix(info, bpp, pitch, output, ptr);
        output += pitch;
        ptr += pitch;
    }
}

static void Deinvert(ImageInfo *info, BYTE* output)
{
    int bpp = (info->BPP+7)/8;
    int pitch = info->Width*bpp;
    const BYTE* ptr = info->Data;
    int y = info->Height;
    while (y--) {
        LineColorfix(info, bpp, pitch, output + y*pitch, ptr);
        ptr += pitch;
    }
}

static void DecompressTGARLE(ImageInfo *info, BYTE* output)
{
    int bpp = (info->BPP+7)/8;
    int pitch = info->Width*bpp;
    const BYTE* ptr = info->Data;
    int y = info->Height;
    while (y--) {
        BYTE* pos = output + y*pitch;
        BYTE* end = pos + info->Width*bpp;
        while (pos < end) {
            int count = (*ptr & 0x7f)+1;
            if (*ptr & 0x80) {
                // run length packet
                while (count--) {
                    LineColorfix(info, bpp, bpp, pos, ptr+1);
                    pos += bpp;
                }
                ptr += 1 + bpp;
            } else {
                // raw packet
                LineColorfix(info, bpp, bpp*count, pos, ptr+1);
                pos += bpp*count;
                ptr += 1 + bpp*count;
            }
        }
    }
}

static void GetImageInfo(LPCVOID data, UINT size, ImageInfo *info)
{
    const BYTE* ptr = (const BYTE*)data;
    memset(info, 0, sizeof(ImageInfo));
#if 0
    // check signatures
    if (size>2 && memcmp(ptr, "BM", 2) == 0) {
        struct bhdr {
            // this is BITMAPFILEHEADER, but psp-gcc seems to ignore the pack pragma in wingdi.h,
            // so redefined here.
            WORD    bfType;
            DWORD   bfSize;
            WORD    bfReserved1;
            WORD    bfReserved2;
            DWORD   bfOffBits;
            BITMAPINFOHEADER bi;
        } __attribute__((packed)) *hdr = (struct bhdr*)ptr;
#ifdef TRANSGAMING_LOGGING
        VideoSystemPS2::PS2Log.Print("Load from BMP file\n");
#endif
        const BYTE *data = ptr + hdr->bfOffBits;
        info->Width = hdr->bi.biWidth;
        info->Height = hdr->bi.biHeight;
        info->BPP = hdr->bi.biBitCount;
        info->Decompress = Deinvert;
        info->Data = data;
    }
    else if (size>4 && memcmp(ptr, "DDS ", 4) == 0) {
        DDSURFACEDESC2* desc = (DDSURFACEDESC2*)(ptr+4);
#ifdef TRANSGAMING_LOGGING
        VideoSystemPS2::PS2Log.Print("Load from DDS file\n");
#endif
        const BYTE *data = ptr + 4 + desc->dwSize;
        info->Width = desc->dwWidth;
        info->Height = desc->dwHeight;
        info->MipLevels = 0; // not supported yet
        if (desc->ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
//            info->Comp = desc->ddpfPixelFormat.dwFourCC;
            info->BPP = 0;
            if (desc->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
                info->A = 1;
//            info->Size = desc->dwLinearSize;

//            info->Decompress = DecompressDXT;
              info->Decompress = Colorfix;
        }
        else {
            info->BPP = desc->ddpfPixelFormat.dwRGBBitCount;
            switch (desc->ddpfPixelFormat.dwRGBAlphaBitMask)
            {
            case 0: info->A = 0; break;
            case 0xff000000: info->A = 8; break;
            default:
#ifdef TRANSGAMING_LOGGING
                VideoSystemPS2::PS2Log.Print("Unsupported Alpha: %08x\n", desc->ddpfPixelFormat.dwRGBAlphaBitMask);
#endif
                break;
            }
            info->Decompress = Colorfix;
        }
        info->Data = data;
    }
    else if (size>18 && memcmp(ptr+size-18, "TRUEVISION-XFILE.", 18) == 0) {
        TGAHeader* hdr = (TGAHeader*)ptr;
#ifdef TRANSGAMING_LOGGING
        VideoSystemPS2::PS2Log.Print("Load from TGA file (type %d)\n", hdr->imagetype);
#endif
        if (hdr->colormaptype != 0) {
#ifdef TRANSGAMING_LOGGING
            VideoSystemPS2::PS2Log.Print("color map not supported\n");
#endif
            return;
        }
        const BYTE *data = ptr + 18 + hdr->idlength;
        info->Width = hdr->width;
        info->Height = hdr->height;
        info->BPP = hdr->pixelbits;
        info->A = hdr->pixelfmt & 0xf;
        if ((hdr->imagetype & 7) != 2) {
#ifdef TRANSGAMING_LOGGING
            VideoSystemPS2::PS2Log.Print("type %d not supported\n", hdr->imagetype);
#endif
            return;
        }
        if (hdr->imagetype & 8)
            info->Decompress = DecompressTGARLE;
        else
            info->Decompress = Deinvert;
        info->Data = data;
    }
    else {
#ifdef TRANSGAMING_LOGGING
        VideoSystemPS2::PS2Log.Print("Load from *unknown* file\n");
        VideoSystemPS2::PS2Log.Print("signature bytes: %02x %02x %02x %02x\n", ptr[0], ptr[1], ptr[2], ptr[3]);
#endif
        return;
    }
#else
    // assume .x format
    DWORD* h = (DWORD *)data;
    info->Width = h[0];
    info->Height = h[1];
    info->BPP = h[2];
    info->MipLevels = h[3];
    info->A = 1;
    if (info->BPP <= 8) {
        info->Pal = (BYTE*)(h+4);
        info->Data = (BYTE*)(h+4+(1<<info->BPP));
    } else {
        info->Data = (BYTE*)(h+4);
    }
#endif
#ifdef TRANSGAMING_LOGGING
    VideoSystemPS2::PS2Log.Print("%dx%dx%d (alpha %d)\n", info->Width, info->Height, info->BPP, info->A);
#endif
    int bpp = (info->BPP+7)/8;
    info->Size = info->Width * info->Height * bpp;
}

void* VideoSystemPS2::CreateTextureFromImage(GLenum target, LPCVOID data, UINT size,
                                             int width, int height, int levels,
                                             ImageInfo *info, GLenum& fmt, GLenum& type)
{
    int bpp;
    GLenum ifmt = 0;
    unsigned h;

    ifmt = GL_RGBA;
    fmt = GL_RGBA;
    type = GL_UNSIGNED_BYTE;

    if (info->A) ifmt = GL_RGBA;
    switch (info->BPP)
    {
    case 8: bpp = 1; fmt = GL_COLOR_INDEX; type = GL_UNSIGNED_BYTE; break;
    case 24: bpp = 3; fmt = GL_RGB; type = GL_UNSIGNED_BYTE; break;
    case 32: bpp = 4; fmt = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
        break;
    default:
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("can't handle depth %d\n", info->BPP);
#endif
        return NULL;
    }
    BYTE* tdata;
    if (info->Decompress) {
        tdata = new BYTE[info->Size];
        info->Decompress(info, tdata);
    }
    else tdata = (BYTE*)info->Data;
    // stupid ps2gl requires these before loading the data
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Enable bilinear filtering textures with size bigger than 8x8 (hardware limitation) 
	if ((info->Width > 8) && (info->Height > 8))
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
    // FIXME: load multiple miplevels (if requested)
    glTexImage2D(target, 0, ifmt, get_pow2(info->Width), get_pow2(info->Height), 0, fmt, type, tdata);
    //gluBuild2DMipmaps(target, ifmt, info->Width, info->Height, fmt, type, tdata);
    //glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0); // no mipmaps
    if (info->Pal) {
        glColorTable(GL_COLOR_TABLE, ifmt, 1 << info->BPP, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, info->Pal);
    }

    if (info->Decompress)
        return tdata;
    else
        return NULL;
}

void VideoSystemPS2::CreateRegularTextureFromFile(Texture *texture)
{
    GLuint Tex=0;//, oldtex = 0;
    GLenum format, type;
    ImageInfo info;

#ifdef TRANSGAMING_LOGGING
    PS2Log.Print("Loading %s\n", texture->FileName.AsChar());
#endif

    GetImageInfo(texture->DataFile, texture->Size, &info);

    int w = info.Width;
    int h = info.Height;


    glGenTextures(1, &Tex);
    //glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&oldtex);
    //TRACE("VideoSystemPS2::CreateRegularTextureFromFile(): binding the texture #%u for the file '%s'\n", Tex, texture->FileName.AsChar());
    glBindTexture(GL_TEXTURE_2D, Tex);
    void* Ptr = CreateTextureFromImage(GL_TEXTURE_2D,
        texture->DataFile, texture->Size, w, h,
        (texture->Flags & Texture::ETexNoMipMaps) ? 1 : 0,
        &info, format, type);
    //glBindTexture(GL_TEXTURE_2D, oldtex);

    if (Tex)
    {
        int texBytes=0; // memory size including mipmaps... but who cares

        int index = findFreeTexture(texture,GL_TEXTURE_2D,format,type,Tex,texBytes,info.Size);
        Textures[index].Ptr = Ptr;

#ifdef ANNOYING_GRAPHICS_LOGS
        PS2Log.Print("RegularTex[%d]:\t%.2f MB %s (%dx%d mip %d)->(%dx%d mip %d) tex 0x%04x/%d\n",
                     index,(float)texBytes/(1024.f*1024.f),
                     texture->FileName.AsChar(),
                     info.Width,info.Height,info.MipLevels,
                     w,h,0 /* LevelCount */,
                     texDef.Target, texDef.Tex);
#endif

        texture->OriginalWidth  = info.Width * 2;
        texture->OriginalHeight = info.Height * 2;
        texture->Width            = w;
        texture->Height            = h;
        texture->PixelBytes        = texBytes;
        texture->DeviceId        = index;
    }
}

void VideoSystemPS2::CreateRegularTexture(Texture *texture)
{
    GLuint Tex=0;//, oldtex = 0;

    if (texture->Width<= 0 || texture->Height<= 0)
    {
#ifdef TRANSGAMING_LOGGING
        PS2Log.Print("ERROR: invalid W,H for CreateRegularTexture (raw) (%d,%d)\n",
                     texture->Width,texture->Height);
#endif    
        return;
    }

    //make sure this was a raw created texture.  If not the file header information will still
    //be in the data block that will be treated as the texture.
    Assert(texture->Flags & (Texture::ETexRaw32 | Texture::ETexRaw8 | Texture::ETexRaw32Alpha));


    GLenum iformat = GL_RGB;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    int bpp = 4;
    if (texture->Flags & Texture::ETexRaw8)
    {
        iformat = GL_COLOR_INDEX;
        format = GL_COLOR_INDEX;
        bpp = 1;
    }
    else if( texture->Flags & Texture::ETexRaw32Alpha )
    {
        iformat = GL_RGBA;
    }

    glGenTextures(1, &Tex);
    //glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&oldtex);
    //TRACE("VideoSystemPS2::CreateRegularTexture(): binding the texture #%u from the file '%s' (size = %dx%d)\n", Tex, texture->FileName.AsChar(), texture->Width, texture->Height);
    glBindTexture(GL_TEXTURE_2D, Tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Enable bilinear filtering textures with size bigger than 8x8 (hardware limitation)
	if ((texture->Width > 8) && (texture->Height > 8))
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
    glTexImage2D(GL_TEXTURE_2D, 0, iformat, texture->Width, texture->Height, 0, format, type, texture->DataFile);
#if 0
    // FIXME: generate mipmaps (could use gluBuild2DMipmaps)
    if (!(texture->Flags & Texture::ETexNoMipMaps))
    {
    }
#endif
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // disable mipmapping
    //glBindTexture(GL_TEXTURE_2D, oldtex);

    if (Tex)
    {
        int index = findFreeTexture(texture,GL_TEXTURE_2D,format,type,Tex,0,texture->Width*texture->Height*bpp);
        texture->DeviceId = index;
    }
}

// LOCK TEXTURE

BYTE *VideoSystemPS2::LockTexture( Texture *tex, int &pitch )
{

    // HACK!! this is a horrible way to get the image pitch!  The Texture::Size and
    //   Texture::Height values don't seem to reliably get any particular values set
    //   into them.  This will for sure work when getting the image buffer of a texture
    //   created with Material::CreateEmptyTexture(), but it might not in other cases.
    if (tex->wasCreatedEmpty()){
        pitch = tex->Size / tex->Height;

        return tex->DataFile;
    }

    //look up the actual texture from the device ID and grab its data pointer and pitch
    else{
        pitch = 0;
        return NULL;
    }
}


void VideoSystemPS2::UnlockTexture( Texture *tex )
{
}

void VideoSystemPS2::SetTexturePriority(Texture *tex, DWORD priority)
{
}

void VideoSystemPS2::PreLoadTexture(Texture *tex)
{
}

void VideoSystemPS2::SetPolygonMode(int mode){
    if (mode == RenderState::EFillSolid)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    else if (mode == RenderState::EFillLines)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    else if (mode == RenderState::EFillPoints)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
}

//------------------------------------------------------------------------------------------------------------------------------------

void SaveScreenBuffer(const char *Filename)
{
#ifndef TRANSGAMING_MASTERING

	#define WIDTH 640
	#define HEIGHT 224

	sceGsStoreImage		gs_simage;
	u_long128			*dst;
	unsigned char		*rgb;
	int					fd, i;
	unsigned char		TGAHeader[] = { 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,0x02,0xc0,0x01,0x18,0x20};

	// to create image file. 
	fd = sceOpen(Filename, SCE_WRONLY | SCE_TRUNC | SCE_CREAT);
	if(fd < 0)
	{
		printf("Open failed!\n");
		return;
	}

	sceWrite(fd, TGAHeader, sizeof(TGAHeader));

	dst	= (u_long128*) malloc(WIDTH * HEIGHT * 4);
	rgb	= (unsigned char*)dst;

	sceGsSetDefStoreImage(&gs_simage, 0, WIDTH / 64, SCE_GS_PSMCT24, 0, 0, WIDTH, HEIGHT);
	FlushCache(0);
	sceGsExecStoreImage(&gs_simage, dst);
	sceGsSyncPath(0, 0);

	for (i=0; i<WIDTH*HEIGHT*3; i+=3)
	{
		unsigned temp	= rgb[i+0];
		rgb[i+0]		= rgb[i+2];
		rgb[i+2]		= temp;
	}

	for(i = 0; i < HEIGHT; i++)
	{
		sceWrite(fd, (char*)((int)dst)+i*(WIDTH*3), WIDTH * 3);
	}

	sceGsSetDefStoreImage(&gs_simage, 2240, WIDTH / 64, SCE_GS_PSMCT24, 0, 0, WIDTH, HEIGHT);
	FlushCache(0);
	sceGsExecStoreImage(&gs_simage, dst);
	sceGsSyncPath(0, 0);

	for (i=0; i<WIDTH*HEIGHT*3; i+=3)
	{
		unsigned temp	= rgb[i+0];
		rgb[i+0]		= rgb[i+2];
		rgb[i+2]		= temp;
	}

	for(i = 0; i < HEIGHT; i++)
	{
		sceWrite(fd, (char*)((int)dst)+i*(WIDTH*3), WIDTH * 3);
	}

	free(dst);
	sceClose(fd);

#endif
}

//------------------------------------------------------------------------------------------------------------------------------------

