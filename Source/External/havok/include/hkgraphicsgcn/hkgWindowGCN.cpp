/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgWindowGCN.h>
#include <hkgraphicsgcn/hkgSystemGCN.h>

//#include <demo.h>

#define START_FRAME_TOKEN  	0xFEEB
#define END_FRAME_TOKEN  	0xB00B

static BOOL g_bGPHangWorkaround = FALSE;
static int  g_iFrameMissThreshold = 0;
static int  g_iFrameCount = 0;
static BOOL g_bFirstFrame = TRUE;

static void _EnableGPHangWorkaround ( u32 timeoutFrames );
static void _NoHangRetraceCallback ( u32 count );
static bool _NoHangDoneRender();
static void _SetGPHangMetric( GXBool enable );
static void _DiagnoseHang();
static void _SwapBuffers();

using namespace hkgSystemGCN;

hkgWindowGCN::hkgWindowGCN()
: 	hkgWindow()
{
	m_virtualMouseEnabled = true;
	m_drawMousePointer = true;
	
	m_tvDeadZoneH = 10;
	m_tvDeadZoneV = 10;
}

bool hkgWindowGCN::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* windowHandle, void* parentHandle)
{
	// make the viewports etc:
	
	hkgWindow::initialize(flags, buffers, 640, 448, name, windowHandle, parentHandle);
	
	//_EnableGPHangWorkaround( 5 );
	return true;
}

hkgWindowGCN::~hkgWindowGCN()
{
	
}

static unsigned int g_PADButtons[12] =
{
	PAD_BUTTON_LEFT,
	PAD_BUTTON_RIGHT,
	PAD_BUTTON_DOWN,
	PAD_BUTTON_UP,
	PAD_TRIGGER_Z,
	PAD_TRIGGER_R,
	PAD_TRIGGER_L,
	PAD_BUTTON_A,
	PAD_BUTTON_B,
	PAD_BUTTON_X,      
	PAD_BUTTON_Y,      
	PAD_BUTTON_START   
};

static unsigned int g_HKGButtons[12] =
{
	HKG_PAD_DPAD_LEFT,  
	HKG_PAD_DPAD_RIGHT, 
	HKG_PAD_DPAD_DOWN,
	HKG_PAD_DPAD_UP,  

	HKG_PAD_SELECT,	   
	HKG_PAD_BUTTON_R1,   
	HKG_PAD_BUTTON_L1,
	
	HKG_PAD_BUTTON_0,   
	HKG_PAD_BUTTON_1,   
	HKG_PAD_BUTTON_3,   
	HKG_PAD_BUTTON_2,   
	HKG_PAD_START      
};

// will loop on windows messages if any. Will return if idle.
HKG_WINDOW_MSG_STATUS hkgWindowGCN::peekMessages(bool inViewportResize)
{
	HKG_WINDOW_MSG_STATUS status = HKG_WINDOW_MSG_CONTINUE;

	if (!inViewportResize && (m_viewportResizeArray.getSize() > 0))
		status = doViewportResizeLoop();

	// step the mouse state etc in preperation for new loop state 
	stepInput(); 

	PADRead( m_gcnPads );
	PADClamp( m_gcnPads );

	updatePads();
	
	return status;
}
	
	
void hkgWindowGCN::updatePads()
{
	for (int pad =0; pad < 2; ++pad)
	{
		PADStatus& s = m_gcnPads[pad];
		if (s.err == PAD_ERR_NONE)
		{
			hkgPad& hpad = m_pad[pad];
			HKG_PAD_BUTTON b = hpad.getButtonState();

			float px1 = hpad.getStickPosX(0);
			float px2 = hpad.getStickPosX(1);
			float py1 = hpad.getStickPosY(0);
			float py2 = hpad.getStickPosY(1);
	 
	 		float sx1 = s.stickX / 72.0f;
			float sx2 = s.substickX / 59.0f;
			float sy1 = s.stickY / 72.0f;
			float sy2 = s.substickY / 59.0f;	
	 
			if ( (m_virtualMouseEnabled && ((hkg_fabs(sx1) > 0.1f) || (hkg_fabs(sy1) > 0.1f)))
				|| ( hkg_fabs(sx1 - px1) > 0.02f ) || ( hkg_fabs(sy1 - py1) > 0.02f ) )
				processPadStickMove( pad, 1, sx1, sy1 );
			if ( (m_virtualMouseEnabled && ((hkg_fabs(sx2) > 0.1f) || (hkg_fabs(sy2) > 0.1f)))
				|| ( hkg_fabs(sx2 - px2) > 0.02f ) || ( hkg_fabs(sy2 - py2) > 0.02f ) )
				processPadStickMove( pad, 0, sx2, sy2 );

			HKG_PAD_BUTTON buttonState = hpad.getButtonState();
			
			for(unsigned int i =0; i < 12; ++i)
			{
				bool cs = s.button & g_PADButtons[i];
				bool cs2 = buttonState & g_HKGButtons[i];
				if (cs != cs2)
					processPadButton( pad, g_HKGButtons[i], cs);
			}
		}
	}
}

void hkgWindowGCN::waitMessages()
{
//	WaitMessage();
}

bool hkgWindowGCN::clearBuffers()
{
//	DEMOBeforeRender();
//	return;
	

	GXRenderModeObj* rmode = &g_state.m_rmodeobj;

	// we can't clear Z if we don't write to it :-)
	if ( (m_context->getEnabledState() & HKG_ENABLED_ZWRITE) == 0)
		m_context->setDepthWriteState( true );

    if (g_bGPHangWorkaround)
    {
        // This will overwrite the end frame token from the previous frame.
        GXSetDrawSync( START_FRAME_TOKEN);

        // Reset the counters for post-hang diagnosis.
        GXClearGPMetric();
    }  

    // Set up viewport (This is inappropriate for full-frame AA.)
    if (rmode->field_rendering)
    {
        GXSetViewportJitter(
          0.0F, 0.0F, (float)rmode->fbWidth, (float)rmode->efbHeight, 
          0.0F, 1.0F, VIGetNextField());
    }
    else
    {
        GXSetViewport(
          0.0F, 0.0F, (float)rmode->fbWidth, (float)rmode->efbHeight, 
          0.0F, 1.0F);
    }
    
    // Invalidate vertex cache in GP
    GXInvalidateVtxCache();

    // Invalidate texture cache in GP
    GXInvalidateTexAll();
    
    // No need to actually clear as the swap (copy) bufs from EXB to VI does clear too

	return true;
}

bool hkgWindowGCN::swapBuffers()
{
	finishScene();


//	DEMODoneRender();
//	return;
	

	GXFlush();

 	// GP hang workaround version (Turned off by default)
    if (g_bGPHangWorkaround)
    {
//        ASSERTMSG(!DemoStatEnable,
//                  "DEMOStats and GP hang diagnosis are mutually exclusive");
        return _NoHangDoneRender();
    }

    // Statistics support (Turned off by default)
 //   if (DemoStatEnable)
 //   {
 //     GXDrawDone();             // make sure data through pipe
 //     DEMOUpdateStats(GX_TRUE); // sample counters here
 //     DEMOPrintStats();         // don't include time to print stats
 //     GXDrawDone();             // make sure data through pipe
 //     DEMOUpdateStats(GX_FALSE); // reset counters here, include copy time
 //   }

    // Issue display copy command
    GXColor bg;
    bg.a = 0xff;
    bg.r = (u8)(m_clearColor[0]*255);
    bg.g = (u8)(m_clearColor[1]*255);
    bg.b = (u8)(m_clearColor[2]*255);
    
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);
    
    GXSetCopyClear(bg, GX_MAX_Z24); // color and z
    GXCopyDisp(g_state.m_pCurrentBuffer, GX_TRUE);

    // Wait until everything is drawn and copied into XFB.
    GXDrawDone();        

	//
	// Swap buffers 
	//
	_SwapBuffers();
	
	return true;
}

static void	_SwapBuffers()
{
    // Set the next frame buffer
    // Display the buffer which was just filled by GXCopyDisplay
    VISetNextFrameBuffer( g_state.m_pCurrentBuffer);

    // If this is the first frame, turn off VIBlack
    if(g_bFirstFrame)
    {
        VISetBlack(FALSE);
        g_bFirstFrame = FALSE;
    }
    
    // Tell VI device driver to write the current VI settings so far
    VIFlush();
    
    // Wait for vertical retrace.
    VIWaitForRetrace();

    // Swap buffers
    if(g_state.m_pCurrentBuffer == g_state.m_pFrameBuffer1)
        g_state.m_pCurrentBuffer = g_state.m_pFrameBuffer2;
    else
        g_state.m_pCurrentBuffer = g_state.m_pFrameBuffer1;  

}

bool hkgWindowGCN::setAsCurrent()
{
	return true;
}


bool hkgWindowGCN::initializeContext()
{
	hkgWindow::initializeContext();
	return true;
}



static void _EnableGPHangWorkaround ( u32 timeoutFrames )
{
    if (timeoutFrames)
    {
//        ASSERTMSG(!DemoStatEnable,
//                  "DEMOStats and GP hang diagnosis are mutually exclusive");
        g_bGPHangWorkaround = TRUE;
        g_iFrameMissThreshold = timeoutFrames;
        VISetPreRetraceCallback( _NoHangRetraceCallback );

        // Enable counters for post-hang diagnosis
        _SetGPHangMetric( GX_TRUE );
    }
    else
    {
        g_bGPHangWorkaround = FALSE;
        g_iFrameMissThreshold = 0;
        _SetGPHangMetric( GX_FALSE );
        VISetPreRetraceCallback( NULL );
    }
}

static void _NoHangRetraceCallback ( u32 count )
{
    #pragma unused (count)
    static u32 ovFrameCount = 0;
    static u32 lastOvc = 0;
    u32        ovc;
    GXBool     overhi, junk;

    // Increment the frame counter used by __NoHangDoneRender.

    g_iFrameCount++;

    // Check the high watermark status.
    // If we're still in the same high watermark state as before,
    // increment the counter and check for time-out.

    GXGetGPStatus(&overhi, &junk, &junk, &junk, &junk);
    ovc = GXGetOverflowCount();

    if (overhi && (ovc == lastOvc))
    {
        ovFrameCount++;
        if (ovFrameCount >= g_iFrameMissThreshold) 
        {
            // We timed out.  Report and diagnose the hang.
            OSReport("---------WARNING : HANG AT HIGH WATERMARK----------\n");

            _DiagnoseHang();
        
            // We cannot easily recover from this situation.  Halt program.
            OSHalt("Halting program");
        }
    }
    else
    {
        lastOvc = ovc;
        ovFrameCount = 0;
    }
}

static bool _NoHangDoneRender()
{
    BOOL abort = FALSE;
    GXCopyDisp(g_state.m_pCurrentBuffer, GX_TRUE);
    GXSetDrawSync( END_FRAME_TOKEN );
    
    g_iFrameCount = 0;

    while( (GXReadDrawSync() != END_FRAME_TOKEN ) && !abort )
    {
        if (g_iFrameCount >= g_iFrameMissThreshold)
        {
            OSReport("---------WARNING : ABORTING FRAME----------\n");
            abort = TRUE;
            _DiagnoseHang();
            reinit();
            // re-enable counters for post-hang diagnosis
            _SetGPHangMetric( GX_TRUE );
        }
    }
    
    _SwapBuffers();
    
   return true;
    
}

void _SetGPHangMetric( GXBool enable )
{
    if (enable)
    {
        // Ensure other counters are off
        GXSetGPMetric( GX_PERF0_NONE, GX_PERF1_NONE );

        // Set up RAS Ready counter
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x2402c004; // ... 101 10000 00000 00100

        // Set up SU Ready counter
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x23000020; // ... 100 000
    
        // Set up XF TOP and BOT busy counters
        GXWGFifo.u8  = GX_LOAD_XF_REG;
        GXWGFifo.u16 = 0x0000;
        GXWGFifo.u16 = 0x1006;
        GXWGFifo.u32 = 0x00084400; // 10000 10001 00000 00000
    }
    else
    {
        // Disable RAS counters
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x24000000;

        // Disable SU counters
        GXWGFifo.u8  = GX_LOAD_BP_REG;
        GXWGFifo.u32 = 0x23000000;

        // Disable XF counters
        GXWGFifo.u8  = GX_LOAD_XF_REG;
        GXWGFifo.u16 = 0x0000;
        GXWGFifo.u16 = 0x1006;
        GXWGFifo.u32 = 0x00000000;
    }
}

static void _DiagnoseHang()
{
    u32 xfTop0, xfBot0, suRdy0, r0Rdy0;
    u32 xfTop1, xfBot1, suRdy1, r0Rdy1;
    u32 xfTopD, xfBotD, suRdyD, r0RdyD;
    GXBool readIdle, cmdIdle, junk;

    // Read the counters twice in order to see which are changing.
    // This method of reading the counters works in this particular case.
    // You should not use this method to read GPMetric counters.
    GXReadXfRasMetric( &xfBot0, &xfTop0, &r0Rdy0, &suRdy0 );
    GXReadXfRasMetric( &xfBot1, &xfTop1, &r0Rdy1, &suRdy1 );

    // XF Top & Bot counters indicate busy, others indicate ready.
    // Convert readings into indications of who is ready/idle.
    xfTopD = (xfTop1 - xfTop0) == 0;
    xfBotD = (xfBot1 - xfBot0) == 0;
    suRdyD = (suRdy1 - suRdy0) > 0;
    r0RdyD = (r0Rdy1 - r0Rdy0) > 0;

    // Get CP status
    GXGetGPStatus(&junk, &junk, &readIdle, &cmdIdle, &junk);

    OSReport("GP status %d%d%d%d%d%d --> ",
             readIdle, cmdIdle, xfTopD, xfBotD, suRdyD, r0RdyD);

    // Depending upon which counters are changing, diagnose the hang.
    // This may not be 100% conclusive, but it's what we've observed so far.
    if (!xfBotD && suRdyD)
    {
        OSReport("GP hang due to XF stall bug.\n");
    }
    else if (!xfTopD && xfBotD && suRdyD)
    {
        OSReport("GP hang due to unterminated primitive.\n");
    }
    else if (!cmdIdle && xfTopD && xfBotD && suRdyD)
    {
        OSReport("GP hang due to illegal instruction.\n");
    }
    else if (readIdle && cmdIdle && xfTopD && xfBotD && suRdyD && r0RdyD)
    {
        OSReport("GP appears to be not hung (waiting for input).\n");
    }
    else
    {
        OSReport("GP is in unknown state.\n");
    }
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
