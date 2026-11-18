/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled header

//hkg
#include <hkgraphicsgcn/hkgSystemGCN.h>
#include <hkgraphics/hkgDisplayWorld.h> 
#include <hkgraphics/hkgLightManager.h> 
#include <hkgraphics/hkgMaterialManager.h> 
#include <hkgraphics/hkgGeometry.h> 
#include <hkgraphics/hkgWindow.h> 

//gcn
#include <hkgraphicsgcn/hkgCameraGCN.h> 
#include <hkgraphicsgcn/hkgDisplayContextGCN.h> 
#include <hkgraphicsgcn/hkgDisplayObjectGCN.h> 
#include <hkgraphicsgcn/hkgFaceSetGCN.h> 
#include <hkgraphicsgcn/hkgFontGCN.h> 
#include <hkgraphicsgcn/hkgLightGCN.h> 
#include <hkgraphicsgcn/hkgMaterialGCN.h> 
#include <hkgraphicsgcn/hkgTextureGCN.h> 
#include <hkgraphicsgcn/hkgVertexSetGCN.h> 
#include <hkgraphicsgcn/hkgViewportGCN.h> 
#include <hkgraphicsgcn/hkgWindowGCN.h> 


//#include <demo.h>

#if defined(__MWERKS__)
#pragma force_active on
#endif

namespace hkgSystemGCN
{
	Buffers g_state;
}

static bool 	g_bHeapInitialized = false;

using namespace hkgSystemGCN;


#define HEAP_ID  0
#define DEFAULT_FIFO_SIZE (256 * 1024)


static void _InitMem( void )
{
    void*    arenaLo;
    void*    arenaHi;
    u32      fbSize;

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
	
		
	GXRenderModeObj* rmode = &g_state.m_rmodeobj;

    /*----------------------------------------------------------------*
     *  Allocate external framebuffers                                *
     *----------------------------------------------------------------*/

    // In HW1, we should allocate the buffers at the beginning of arena (<16MB)
    // Need to do this before OSInitAlloc is called.
    fbSize = VIPadFrameBufferWidth(rmode->fbWidth) * rmode->xfbHeight * 
        (u32)VI_DISPLAY_PIX_SZ;
    g_state.m_pFrameBuffer1 = (void*)OSRoundUp32B((u32)arenaLo);
    g_state.m_pFrameBuffer2 = (void*)OSRoundUp32B((u32)g_state.m_pFrameBuffer1 + fbSize);
    g_state.m_pCurrentBuffer = g_state.m_pFrameBuffer2;
    
    arenaLo = (void*)OSRoundUp32B((u32)g_state.m_pFrameBuffer2 + fbSize);
    OSSetArenaLo(arenaLo);

    /*----------------------------------------------------------------*
     *  Create a heap                                                 *
     *----------------------------------------------------------------*/

    // OSInitAlloc should only ever be invoked once.
    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1); // 1 heap
    OSSetArenaLo(arenaLo);

    // Ensure boundaries are 32B aligned
    arenaLo = (void*)OSRoundUp32B(arenaLo);
    arenaHi = (void*)OSRoundDown32B(arenaHi);

    // The boundaries given to OSCreateHeap should be 32B aligned
    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    // From here on out, OSAlloc and OSFree behave like malloc and free
    // respectively
    OSSetArenaLo(arenaLo=arenaHi);
}


static void _InitRenderMode()
{
	GXRenderModeObj* rmode;

    switch (VIGetTvFormat())
    {
      case VI_NTSC:
        rmode = &GXNtsc480IntDf;
        break;
      case VI_PAL:
        rmode = &GXPal528IntDf;
        break;
      case VI_EURGB60:
        rmode = &GXEurgb60Hz480IntDf;
        break;
      case VI_MPAL:
        rmode = &GXMpal480IntDf;
        break;
      default:
        OSHalt("DEMOInit: invalid TV format\n");
        break;
    }

    // Trim off from top & bottom 16 scanlines (which will be overscanned).
    // So almost all demos actually render only 448 lines (in NTSC case.)
    // Since this setting is just for SDK demos, you can specify this
    // in order to match your application requirements.
    GXAdjustForOverscan(rmode, &g_state.m_rmodeobj, 0, 16);
}

static void _InitGX()
{
    u16     xfbHeight;
    f32     yScale;

	GXRenderModeObj* rmode = &g_state.m_rmodeobj;

    /*----------------------------------------------------------------*
     *  GX configuration by a render mode obj                         *
     *----------------------------------------------------------------*/

    // These are all necessary function calls to take a render mode
    // object and set up the relevant GX configuration.

    GXSetViewport(0.0F, 0.0F, (f32)rmode->fbWidth, (f32)rmode->efbHeight, 
                  0.0F, 1.0F);
    GXSetScissor(0, 0, (u32)rmode->fbWidth, (u32)rmode->efbHeight);

    yScale = GXGetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    xfbHeight = (u16)GXSetDispCopyYScale(yScale);
    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, xfbHeight);
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);

    if (rmode->aa)
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

   // Clear embedded framebuffer for the first frame
    GXCopyDisp(g_state.m_pCurrentBuffer, GX_TRUE);

    // Verify (warning) messages are turned off by default
#ifdef _DEBUG
    GXSetVerifyLevel(GX_WARN_MEDIUM);
#endif

    // Gamma correction
    //
    // You may want to use 1.7 or 2.2 if all artwork made on PCs
    // become darker. However, recommended setting is 1.0.
    GXSetDispCopyGamma(GX_GM_1_0);
}

static void _InitVI( void )
{
    u32 nin;

	
    // Double-buffering initialization
    VISetNextFrameBuffer(g_state.m_pFrameBuffer1);
    g_state.m_pCurrentBuffer = g_state.m_pFrameBuffer2;

    // Tell VI device driver to write the current VI settings so far
    VIFlush();
    
    // Wait for retrace to start first frame
    VIWaitForRetrace();

    // Because of hardware restriction, we need to wait one more 
    // field to make sure mode is safely changed when we change
    // INT->DS or DS->INT. (VIInit() sets INT mode as a default)
    GXRenderModeObj* rmode = &g_state.m_rmodeobj;
    
	nin = (u32)rmode->viTVmode & 1;
    if (nin)
        VIWaitForRetrace();
}



	
static void _InitSystem()
{
//	DEMOInit(HK_NULL);
//	g_bHeapInitialized = true;
//	return;
	

    OSInit();
    DVDInit();
    VIInit();
	PADInit(); // must be after VI
   
      
  //  _InitPad();
    _InitRenderMode();
    _InitMem();
    
    VIConfigure( &g_state.m_rmodeobj );
    
    g_state.m_Fifo     = OSAlloc(DEFAULT_FIFO_SIZE);
    g_state.m_FifoObj  = GXInit(g_state.m_Fifo, DEFAULT_FIFO_SIZE);
    
    _InitGX();
    _InitVI();
    
      
 	g_bHeapInitialized = true;   
     
	return;	
}

//
// Just in case the demo wants to use new and delete directly
//

void* operator new       ( hk_size_t blocksize )
{
	if (!g_bHeapInitialized)
		_InitSystem();
		
    return OSAllocFromHeap(HEAP_ID, blocksize);
}

void* operator new[]     ( hk_size_t blocksize )
{
	if (!g_bHeapInitialized)
		_InitSystem();
    return OSAllocFromHeap(HEAP_ID, blocksize);
}

void operator delete     ( void* block )
{
	if (!g_bHeapInitialized)
		_InitSystem();
    OSFreeToHeap(HEAP_ID, block);
}

void operator delete[]   ( void* block )
{
	if (!g_bHeapInitialized)
		_InitSystem();
    OSFreeToHeap(HEAP_ID, block);
}
	
namespace hkgSystemGCN
{

void reinit()
{
 // Create a temporary FIFO while the current one is reset
    GXFifoObj   tmpobj;
    void*       tmpFifo = OSAlloc(64*1024);

    // Get data on current Fifo.
    GXFifoObj*  realFifoObj = GXGetCPUFifo();
    void*   realFifoBase = GXGetFifoBase(realFifoObj);
    u32     realFifoSize = GXGetFifoSize(realFifoObj);
    
    // Abort the GP
    GXAbortFrame();
  
    GXInitFifoBase( &tmpobj, tmpFifo, 64*1024);
    
    GXSetCPUFifo(&tmpobj);
    GXSetGPFifo(&tmpobj);    

	// this does noting other than assign the pointers (which is done already anyway as this is a reinit)
   // _InitRenderMode(mode);
   	
    // This will re-initialize the pointers for the original FIFO.
    g_state.m_FifoObj = GXInit(realFifoBase, realFifoSize);

    _InitGX();
    
    // NOTE: the VI settings do not necessarily have to be reset, but
    //       just to be safe, we do so anyway
    VIConfigure(&g_state.m_rmodeobj);
    _InitVI();

    // remove the temporary fifo
    OSFree(tmpFifo);
}

}

extern void hkgSystemGCNInit();
void hkgSystemGCNInit()
{
	if (!g_bHeapInitialized)
		_InitSystem();

	//hkg
	hkgDisplayWorld::create = hkgDisplayWorld::createDisplayWorld;
	hkgGeometry::create = hkgGeometry::createGeometry;
	hkgLightManager::create = hkgLightManager::createLightManager;
	hkgMaterialFaceSet::create = hkgMaterialFaceSet::createMaterialFaceSet;
	hkgMaterialManager::create = hkgMaterialManager::createMaterialManager;
	//gcn
	hkgCamera::create = hkgCameraGCN::createCameraGCN;
	hkgDisplayContext::create = hkgDisplayContextGCN::createDisplayContextGCN;
	hkgDisplayContext::s_maxNumLights = 8;
	hkgDisplayObject::create = hkgDisplayObjectGCN::createDisplayObjectGCN; 
	hkgFaceSet::create = hkgFaceSetGCN::createFaceSetGCN;
	hkgFont::create = hkgFontGCN::createFontGCN;
	hkgLight::create = hkgLightGCN::createLightGCN;
	hkgMaterial::create = hkgMaterialGCN::createMaterialGCN;
	hkgTexture::create = hkgTextureGCN::createTextureGCN;
	hkgVertexSet::create = hkgVertexSetGCN::createVertexSetGCN;
	hkgViewport::create = hkgViewportGCN::createViewportGCN;
	hkgWindow::create = hkgWindowGCN::createWindowGCN;
	hkBaseSystem::init();
	
		
    hkBaseSystem::init();
}

extern void hkgSystemGCNQuit();
void hkgSystemGCNQuit()
{
	hkBaseSystem::quit();
}

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
