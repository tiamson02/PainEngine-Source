/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkGraphicsDX8.h> // precompiled
#include <hkgraphicsdx8/xbox/hkgWindowXbox.h>
#include <hkgraphicsdx8/xbox/hkgWindowXboxPad.h>
// 
// A lot of this file is taken from 
// the Xbox sample framework XBapp.cpp
// Copyright (c) Microsoft Corporation. All rights reserved.

using namespace hkgWindowXboxPad;

hkgWindow* hkgWindowXbox::createWindowXbox()
{
	return new hkgWindowXbox();
}

hkgWindowXbox::hkgWindowXbox()
: hkgWindowDX8() 
{
	DWORD vf = XGetVideoStandard();
	DWORD pal = vf & XC_VIDEO_STANDARD_PAL_I;

	m_virtualMouseEnabled = true;
	m_drawMousePointer = true;

	m_width = 640;
	m_height = 480;

	m_tvDeadZoneH = 20;
	m_tvDeadZoneV = 20;

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.BackBufferWidth        = m_width;
    m_d3dpp.BackBufferHeight       = m_height;
    m_d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE; // D3DPRESENT_INTERVAL_IMMEDIATE (for no vsync)
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD; 
	m_d3dpp.Flags				   = D3DPRESENTFLAG_10X11PIXELASPECTRATIO; // to be more like the PS2 :-)
}

hkgWindowXbox::~hkgWindowXbox()
{
   if (m_pd3dDevice )
		m_pd3dDevice->Release();

   if (m_pD3D)
		m_pD3D->Release();
}

bool hkgWindowXbox::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* windowHandle, void* parentHandle )
{
    HRESULT hr;

    // Create the Direct3D object
    OutputDebugString( "-- HKG: Creating Direct3D...\n" );
    if( NULL == ( m_pD3D = Direct3DCreate8(D3D_SDK_VERSION) ) )
    {
        OutputDebugString( "-- HKG ERROR: Unable to create Direct3D!\n" );
        return false;
    }
   	
	// Create the device
    OutputDebugString( "-- HKG: Creating D3D device...\n" );
    if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice ) ) )
    {
        OutputDebugString( "-- HKG ERROR: Could not create D3D device!\n" );
        return false;
    }

    XInitDevices( 0, NULL );

	OutputDebugString( "-- HKG: Creating gamepad devices...\n" );
    if( FAILED( hr = CreateGamepads( HK_NULL ) ) )
    {
        OutputDebugString( "-- HKG ERROR: Call to CreateGamepads() failed!\n" );
        return false;
    }

	hkgWindow::initialize(flags, buffers, m_width, m_height, name, windowHandle, parentHandle);

	return true;
}

bool hkgWindowXbox::setAsCurrent()
{
	return true;
}


bool hkgWindowXbox::clearBuffers()
{
	 // Clear the viewport
	DWORD bc = hkgColor4FromVec3( m_clearColor ) | 0xff000000;

	if ( (m_context->getEnabledState() & HKG_ENABLED_ZWRITE) == 0)
		m_context->setDepthWriteState( true );

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         bc, 1.0f, 0L );
	
    // Begin the scene 
    m_pd3dDevice->BeginScene();
  
	return true;

}


bool hkgWindowXbox::swapBuffers()
{
	finishScene();

	m_pd3dDevice->EndScene();
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return true;
}


bool hkgWindowXbox::initializeContext()
{
	return hkgWindow::initializeContext();
}





static unsigned int DigitalButtonMap[8] = 
{
 /*XINPUT_GAMEPAD_DPAD_UP*/		HKG_PAD_DPAD_UP,
 /*XINPUT_GAMEPAD_DPAD_DOWN*/	HKG_PAD_DPAD_DOWN,
 /*XINPUT_GAMEPAD_DPAD_LEFT*/	HKG_PAD_DPAD_LEFT,
 /*XINPUT_GAMEPAD_DPAD_RIGHT*/	HKG_PAD_DPAD_RIGHT,
 /*XINPUT_GAMEPAD_START*/		HKG_PAD_START,
 /*XINPUT_GAMEPAD_BACK*/		HKG_PAD_SELECT,
 /*XINPUT_GAMEPAD_LEFT_THUMB*/	HKG_PAD_BUTTON_LSTICK,
 /*XINPUT_GAMEPAD_RIGHT_THUMB*/	HKG_PAD_BUTTON_RSTICK
};

static unsigned int AnalogButtonMap[8] = 
{
 /*XINPUT_GAMEPAD_A*/				HKG_PAD_BUTTON_0,
 /*XINPUT_GAMEPAD_B*/				HKG_PAD_BUTTON_3,
 /*XINPUT_GAMEPAD_C*/				HKG_PAD_BUTTON_1,
 /*XINPUT_GAMEPAD_D*/				HKG_PAD_BUTTON_2,
 /*XINPUT_GAMEPAD_BLACK*/			HKG_PAD_BUTTON_R2,
 /*XINPUT_GAMEPAD_WHITE*/			HKG_PAD_BUTTON_L2,
 /*XINPUT_GAMEPAD_LEFT_TRIGGER*/	HKG_PAD_BUTTON_L1,
 /*XINPUT_GAMEPAD_RIGHT_TRIGGER*/	HKG_PAD_BUTTON_R1
};

void hkgWindowXbox::updatePads()
{
	for (int pad =0; pad < 2; ++pad)
	{
		hkgXboxGamepad& xp = g_Gamepads[pad];

		hkgPad& hpad = m_pad[pad];
		HKG_PAD_BUTTON b = hpad.getButtonState();

		float px1 = hpad.getStickPosX(0);
		float px2 = hpad.getStickPosX(1);
		float py1 = hpad.getStickPosY(0);
		float py2 = hpad.getStickPosY(1);
 
		bool s1nonzero = m_virtualMouseEnabled && ((hkg_fabs(xp.fX1) > 0.1f) || (hkg_fabs(xp.fY1) > 0.1f));
		bool s2nonzero = m_virtualMouseEnabled && ((hkg_fabs(xp.fX2) > 0.1f) || (hkg_fabs(xp.fY2) > 0.1f));


		if ( s1nonzero || ( hkg_fabs(xp.fX1 - px2) > 0.02f ) || ( hkg_fabs(xp.fY1 - py2) > 0.02f ) )
			processPadStickMove( pad, 1, xp.fX1, -xp.fY1 );

		if ( s2nonzero || ( hkg_fabs(xp.fX2 - px1) > 0.02f ) || ( hkg_fabs(xp.fY2 - py1) > 0.02f ) )
			processPadStickMove( pad, 0, xp.fX2, -xp.fY2 );

		unsigned int i= 0;
		for(i =0; i < 8; ++i)
		{
			if (xp.bPressedAnalogButtons[i])
				processPadButton( pad, AnalogButtonMap[i], true);
			else if ((b & AnalogButtonMap[i]) && (xp.bAnalogButtons[i] <= 10)) // released
				processPadButton( pad, AnalogButtonMap[i], false);
		}

		unsigned int mask = 1;
		for(i =0; i < 8; ++i)
		{
			unsigned int imask = mask << i;
			if (xp.wPressedButtons & imask) // pressed
				processPadButton( pad, DigitalButtonMap[i], true);
			else if ((b & DigitalButtonMap[i]) && ((xp.wButtons & imask) == 0))// released
				processPadButton( pad, DigitalButtonMap[i], false);
		}	
	}
}


HKG_WINDOW_MSG_STATUS hkgWindowXbox::peekMessages(bool inViewportResize)
{
	if (!inViewportResize && (m_viewportResizeArray.getSize() > 0))
		doViewportResizeLoop();

	stepInput(); // step to new state
	GetInput( NULL ); // read all pad input
	
	// update hkgPad 0 and 1 
	updatePads();
	
	return HKG_WINDOW_MSG_CONTINUE; // never quit an xbox game 
}	

void hkgWindowXbox::waitMessages()
{
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
