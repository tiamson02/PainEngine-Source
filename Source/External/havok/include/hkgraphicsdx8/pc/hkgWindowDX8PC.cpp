/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkGraphicsDX8.h> // precompiled

#include <hkgraphicsdx8/hkgDisplayContextDX8.h>
#include <hkgraphicsdx8/pc/hkgWindowDX8PC.h>
#include <hkgraphicsdx8/pc/hkgWindowManagerPC.h>
#include <hkgraphicsdx8/pc/hkgDirectJoystickPC.h>

#include <hkgraphics/hkgTexture.h>
// 
// A lot of this file is taken from 
// the DirectX 8.1 samples/Multimedia/Common/src/D3DApp.cpp file
// Copyright (c) 1998-2001 Microsoft Corporation. All rights reserved.

#include <basetsd.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>

static const char* hkgWindowClassName = "HKGRAPHICSWINDOWDX8";
static const char* hkgWindowName = "Havok (dx8)";

static unsigned short _PCButtonMap[16] = {
	HKG_PAD_BUTTON_0,
	HKG_PAD_BUTTON_1,
	HKG_PAD_BUTTON_2,
	HKG_PAD_BUTTON_3,

	HKG_PAD_START,  
	HKG_PAD_SELECT, 
	
	HKG_PAD_DPAD_UP,  
	HKG_PAD_DPAD_RIGHT,
	HKG_PAD_DPAD_DOWN, 
	HKG_PAD_DPAD_LEFT, 
	
	HKG_PAD_BUTTON_L1, 
	HKG_PAD_BUTTON_R1, 
	HKG_PAD_BUTTON_L2, 
	HKG_PAD_BUTTON_R2, 
};


hkgWindow* hkgWindowDX8PC::createWindowDX8PC()
{
	return new hkgWindowDX8PC( GetModuleHandle(NULL), 0);
}

hkgWindowDX8PC::hkgWindowDX8PC( HINSTANCE instance, WORD menuID)
: hkgWindowDX8(), m_hinstance( instance ), /*, m_active(true)  m_directJoystick(NULL)*/
	m_dwNumAdapters(0), m_dwAdapter(0L), m_dwMinDepthBits(16), m_dwMinStencilBits(0), m_bUseDepthBuffer(TRUE), m_ownHWND(false),
	m_directJoystick(NULL)
{
}

hkgWindowDX8PC::~hkgWindowDX8PC()
{
	cleanupD3D();
	
	if (m_directJoystick)
	{
		delete m_directJoystick;
	}

	if (m_platformHandle)
	{
		hkgWindowManagerDX8PC::getInstance().removeWindow( (HWND) m_platformHandle );

		if (m_ownHWND)
		{
			HRESULT res = DestroyWindow((HWND) m_platformHandle);
			UnregisterClass(hkgWindowClassName,m_hinstance);
		}
	}
}

bool hkgWindowDX8PC::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle, void* platformParentHandle )
{
    HRESULT hr;

	if ((w == 0) || (h == 0))
	{	
		w = 32;
		h = 32;
	}

    // Create the Direct3D object
    m_pD3D = Direct3DCreate8( D3D_SDK_VERSION );

    if( m_pD3D == NULL )
	{
		displayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
		return false;
	}

    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
    if( FAILED( hr = buildDeviceList() ) )
    {
        SAFE_RELEASE( m_pD3D );
        displayErrorMsg( hr, MSGERR_APPMUSTEXIT );
		return false;
    }

    // Unless a substitute hWnd has been specified, create a window to
    // render into
    if( platformHandle == NULL)
    {
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC) hkgWindowProcDX8PC;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO ); 
		wc.hCursor = LoadCursor(NULL, IDC_ARROW );
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = HK_NULL;
		wc.lpszClassName = hkgWindowClassName;
	
		if(! RegisterClass(&wc) && ERROR_CLASS_ALREADY_EXISTS != GetLastError())
		{
			// failed
		}

        // Set the window's initial style
        m_dwWindowStyle = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|
                          WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_VISIBLE;

        // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, w, h );
        AdjustWindowRect( &rc, m_dwWindowStyle, TRUE );

        // Create the render window
        platformHandle = CreateWindow( hkgWindowClassName, name, m_dwWindowStyle,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               (rc.right-rc.left), (rc.bottom-rc.top), 0L,
                               HK_NULL,
                               m_hinstance, (LPVOID) this );
    
		m_ownHWND = true;
	}
	else
		m_ownHWND = false;

	if (platformHandle==HK_NULL)
		return false;


	// finally, show the window:
	UpdateWindow((HWND)platformHandle);
	ShowWindow((HWND)platformHandle, SW_SHOWNORMAL);	


	// Save window properties
    m_dwWindowStyle = GetWindowLong( ((HWND)platformHandle), GWL_STYLE );
	GetWindowRect( ((HWND)platformHandle), &m_rcWindowBounds );
	GetClientRect( ((HWND)platformHandle), &m_rcWindowClient );

	if (m_rcWindowClient.right - m_rcWindowClient.left <= 32)
	{
		m_rcWindowClient.right = 32;
		m_rcWindowClient.left  = 0;
	}

	if (m_rcWindowClient.bottom - m_rcWindowClient.top <= 32)
	{
		m_rcWindowClient.bottom = 32;
		m_rcWindowClient.top  = 0;
	}

	m_platformHandle = platformHandle;
	m_platformParentHandle = platformParentHandle;

    // Initialize the 3D environment for the app
    if( FAILED( hr = initializeD3D() ) )
    {
        SAFE_RELEASE( m_pD3D );
        displayErrorMsg( hr, MSGERR_APPMUSTEXIT );
		return false;
    }

	hkgWindow::initialize(flags, buffers, m_rcWindowClient.right - m_rcWindowClient.left, m_rcWindowClient.bottom - m_rcWindowClient.top, name, platformHandle, platformParentHandle);

	// try and init a joystick
	m_directJoystick = new hkgDirectJoystickPC((HWND)platformHandle);
	if (!m_directJoystick->isValid())
	{
		delete m_directJoystick;
		m_directJoystick = NULL;
	}
	else
	{
		// register a 30hz timer to poll the joystick
		SetTimer( (HWND)platformHandle, 0, 1000 / 30, NULL );
	}

	return true;
}


HRESULT hkgWindowDX8PC::initializeD3D()
{
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
    D3DDeviceInfo*  pDeviceInfo  = &pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice];
    D3DModeInfo*    pModeInfo    = &pDeviceInfo->modes[pDeviceInfo->dwCurrentMode];

    // Prepare window for possible windowed/fullscreen change
    adjustWindowForChange();

    // Set up the presentation parameters
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.Windowed               = pDeviceInfo->bWindowed;
    m_d3dpp.BackBufferCount        = 1;
    if( pDeviceInfo->bWindowed )
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeWindowed;
    else
        m_d3dpp.MultiSampleType    = pDeviceInfo->MultiSampleTypeFullscreen;
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.EnableAutoDepthStencil = m_bUseDepthBuffer;
    m_d3dpp.AutoDepthStencilFormat = pModeInfo->DepthStencilFormat;
    m_d3dpp.hDeviceWindow          = ((HWND)m_platformHandle);
    if( !m_fullscreen )
    {
        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
    }
    else
    {
        m_d3dpp.BackBufferWidth  = pModeInfo->Width;
        m_d3dpp.BackBufferHeight = pModeInfo->Height;
        m_d3dpp.BackBufferFormat = pModeInfo->Format;
    }

/*
    if( pDeviceInfo->d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE )
    {
        // Warn user about null ref device that can't render anything
        displayErrorMsg( D3DAPPERR_NULLREFDEVICE, 0 );
    }
*/

    // Create the device
    hr = m_pD3D->CreateDevice( m_dwAdapter, pDeviceInfo->DeviceType,
                               (HWND)m_platformHandle, pModeInfo->dwBehavior | D3DCREATE_FPU_PRESERVE, &m_d3dpp,
                               &m_pd3dDevice );
    if( SUCCEEDED(hr) )
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if( !m_fullscreen )
        {
            SetWindowPos( ((HWND)m_platformHandle), HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );
        }

        // Store device Caps
        m_pd3dDevice->GetDeviceCaps( &m_d3dCaps );
        m_dwCreateFlags = pModeInfo->dwBehavior;

        // Store device description
        if( pDeviceInfo->DeviceType == D3DDEVTYPE_REF )
            lstrcpy( m_strDeviceStats, TEXT("REF") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
            lstrcpy( m_strDeviceStats, TEXT("HAL") );
        else if( pDeviceInfo->DeviceType == D3DDEVTYPE_SW )
            lstrcpy( m_strDeviceStats, TEXT("SW") );

        if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            pModeInfo->dwBehavior & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (pure hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated pure hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated hw vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (mixed vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated mixed vp)") );
        }
        else if( pModeInfo->dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            lstrcat( m_strDeviceStats, TEXT(" (sw vp)") );
        }

        if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
        {
            lstrcat( m_strDeviceStats, TEXT(": ") );
            lstrcat( m_strDeviceStats, pAdapterInfo->d3dAdapterIdentifier.Description );
        }

        // Store render target surface desc
        LPDIRECT3DSURFACE8 pBackBuffer;
        m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();

        // Initialize the app's device-dependent objects
        hr = InitDeviceObjects();
        if( SUCCEEDED(hr) )
        {
            hr = RestoreDeviceObjects();
            if( SUCCEEDED(hr) )
            {
                m_active = true;
                return S_OK;
            }
        }

        // Cleanup before we try again
        InvalidateDeviceObjects();
        DeleteDeviceObjects();
        SAFE_RELEASE( m_pd3dDevice );
    }

    // If that failed, fall back to the reference rasterizer
    if( pDeviceInfo->DeviceType == D3DDEVTYPE_HAL )
    {
        // Select the default adapter
        m_dwAdapter = 0L;
        pAdapterInfo = &m_Adapters[m_dwAdapter];

        // Look for a software device
        for( UINT i=0L; i<pAdapterInfo->dwNumDevices; i++ )
        {
            if( pAdapterInfo->devices[i].DeviceType == D3DDEVTYPE_REF )
            {
                pAdapterInfo->dwCurrentDevice = i;
                pDeviceInfo = &pAdapterInfo->devices[i];
                m_fullscreen = !(pDeviceInfo->bWindowed);
                break;
            }
        }

        // Try again, this time with the reference rasterizer
        if( pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice].DeviceType ==
            D3DDEVTYPE_REF )
        {
            // Make sure main window isn't topmost, so error message is visible
            SetWindowPos( ((HWND)m_platformHandle), HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );
            adjustWindowForChange();

            // Let the user know we are switching from HAL to the reference rasterizer
            displayErrorMsg( hr, MSGWARN_SWITCHEDTOREF );

            hr = initializeD3D();
        }
    }

    return hr;
}

bool hkgWindowDX8PC::setAsCurrent()
{
	return true;
}



HRESULT hkgWindowDX8PC::resizeD3D()
{
    HRESULT hr;

    // Release all vidmem objects
    if( FAILED( hr = InvalidateDeviceObjects() ) )
        return hr;

    // Reset the device
    if( FAILED( hr = m_pd3dDevice->Reset( &m_d3dpp ) ) )
        return hr;

	
    // Store render target surface desc
    LPDIRECT3DSURFACE8 pBackBuffer;
    m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();


    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if( FAILED(hr) )
		return hr;

	m_context->setCurrentTexture(NULL);

	// now that we have reset, we must sync state
	static_cast<hkgDisplayContextDX8*>(m_context)->initD3DState(); // set initial state / non mutable state
	m_context->syncState(); // set to match current context state

	if (m_mousePointerTexture)
		m_mousePointerTexture->realize();

	if (m_havokLogoTexture)
		m_havokLogoTexture->realize();

	
    return S_OK;
}


void hkgWindowDX8PC::cleanupD3D()
{
    if( m_pd3dDevice )
    {
        InvalidateDeviceObjects();
        DeleteDeviceObjects();

        m_pd3dDevice->Release();
        m_pD3D->Release();

        m_pd3dDevice = NULL;
        m_pD3D       = NULL;
    }
}



bool hkgWindowDX8PC::clearBuffers()
{
    HRESULT hr;

    // Test the cooperative level to see if it's okay to render
    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
    {
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
            return true;

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
        {
            // If we are windowed, read the desktop mode and use the same format for
            // the back buffer
            if( !m_fullscreen )
            {
                D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
                m_pD3D->GetAdapterDisplayMode( m_dwAdapter, &pAdapterInfo->d3ddmDesktop );
                m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
            }

            //if( FAILED( hr = resizeD3D() ) )
			// return false;

			updateSizeD3D();
               
        }

        return true;
    }

	 // Clear the viewport
	DWORD bc = hkgColor4FromVec3( m_clearColor ) | 0xff000000;

	if ( (m_context->getEnabledState() & HKG_ENABLED_ZWRITE) == 0)
		m_context->setDepthWriteState( true );

    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER/*|D3DCLEAR_STENCIL*/,
                         bc, 1.0f, 0L );
	
    // Begin the scene 
    m_pd3dDevice->BeginScene();
  
	return true;

}


bool hkgWindowDX8PC::swapBuffers()
{
	finishScene();

	m_pd3dDevice->EndScene();
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return true;
}


bool hkgWindowDX8PC::initializeContext()
{
	return hkgWindow::initializeContext();
}

HKG_WINDOW_MSG_STATUS hkgWindowDX8PC::peekMessages(bool inViewportResize)
{
	MSG msg;

	HKG_WINDOW_MSG_STATUS status = HKG_WINDOW_MSG_CONTINUE;

	if (!inViewportResize && (m_viewportResizeArray.getSize() > 0))
		status = doViewportResizeLoop();

	// step the mouse state etc in preperation for new loop state 
	stepInput(); 

	while ( PeekMessage(&msg, HK_NULL, 0U, 0U, PM_REMOVE) )
    {
        if ( WM_QUIT == msg.message )
		{
            status = HKG_WINDOW_MSG_QUIT;
			break;
		}

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	return status;
}

void hkgWindowDX8PC::waitMessages()
{
	WaitMessage();
}

HRESULT hkgWindowDX8PC::displayErrorMsg( HRESULT hr, DWORD dwType )
{
    TCHAR strMsg[512];

    switch( hr )
    {
        case D3DAPPERR_NODIRECT3D:
            _tcscpy( strMsg, _T("Could not initialize Direct3D. You may\n")
                             _T("want to check that the latest version of\n")
                             _T("DirectX is correctly installed on your\n")
                             _T("system.  Also make sure that this program\n")
                             _T("was compiled with header files that match\n")
                             _T("the installed DirectX DLLs.") );
            break;

        case D3DAPPERR_NOCOMPATIBLEDEVICES:
            _tcscpy( strMsg, _T("Could not find any compatible Direct3D\n")
                             _T("devices.") );
            break;

        case D3DAPPERR_NOWINDOWABLEDEVICES:
            _tcscpy( strMsg, _T("This sample cannot run in a desktop\n")
                             _T("window with the current display settings.\n")
                             _T("Please change your desktop settings to a\n")
                             _T("16- or 32-bit display mode and re-run this\n")
                             _T("sample.") );
            break;

        case D3DAPPERR_NOHARDWAREDEVICE:
            _tcscpy( strMsg, _T("No hardware-accelerated Direct3D devices\n")
                             _T("were found.") );
            break;

        case D3DAPPERR_HALNOTCOMPATIBLE:
            _tcscpy( strMsg, _T("This sample requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator.") );
            break;

        case D3DAPPERR_NOWINDOWEDHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case D3DAPPERR_NODESKTOPHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window with the current\n")
                             _T("desktop display settings.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case D3DAPPERR_NOHALTHISMODE:
            _tcscpy( strMsg, _T("This sample requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator with the current desktop display\n")
                             _T("settings.\n")
                             _T("Press F2 while the app is running to see a\n")
                             _T("list of available devices and modes.") );
            break;

        case D3DAPPERR_MEDIANOTFOUND:
            _tcscpy( strMsg, _T("Could not load required media." ) );
            break;

        case D3DAPPERR_RESIZEFAILED:
            _tcscpy( strMsg, _T("Could not reset the Direct3D device." ) );
            break;

        case D3DAPPERR_NONZEROREFCOUNT:
            _tcscpy( strMsg, _T("A D3D object has a non-zero reference\n")
                             _T("count (meaning things were not properly\n")
                             _T("cleaned up).") );
            break;

        case D3DAPPERR_NULLREFDEVICE:
            _tcscpy( strMsg, _T("Warning: Nothing will be rendered.\n")
                             _T("The reference rendering device was selected, but your\n")
                             _T("computer only has a reduced-functionality reference device\n")
                             _T("installed.  Install the DirectX SDK to get the full\n")
                             _T("reference device.\n") );
            break;

        case E_OUTOFMEMORY:
            _tcscpy( strMsg, _T("Not enough memory.") );
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            _tcscpy( strMsg, _T("Not enough video memory.") );
            break;

        default:
            _tcscpy( strMsg, _T("Generic application error. Enable\n")
                             _T("debug output for detailed information.") );
    }

    if( MSGERR_APPMUSTEXIT == dwType )
    {
        _tcscat( strMsg, _T("\n\nThis program will now exit.") );
        MessageBox( NULL, strMsg, _T("Havok Graphics Lib"), MB_ICONERROR|MB_OK );

        // Close the window, which shuts down the app gracefully
        if( m_platformHandle )
		{
			PostQuitMessage(0);
		}
    }
    else
    {
        if( MSGWARN_SWITCHEDTOREF == dwType )
            _tcscat( strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
                             _T("a software device that implements the entire\n")
                             _T("Direct3D feature set, but runs very slowly.") );
        MessageBox( NULL, strMsg, _T("Havok Graphics Lib"), MB_ICONWARNING|MB_OK );
    }

	return hr;
}


bool hkgWindowDX8PC::updateSize( unsigned int w, unsigned int h)
{
	updateSizeD3D();
	return hkgWindow::updateSize( w, h );
}

bool hkgWindowDX8PC::updateSizeD3D()
{
	HRESULT hr;
	RECT rcClientOld;
    rcClientOld = m_rcWindowClient;
	HWND hWnd = (HWND)m_platformHandle;

	if (!hWnd) return true;

    // Update window properties
    GetWindowRect( hWnd, &m_rcWindowBounds );
    GetClientRect( hWnd, &m_rcWindowClient );

    if( rcClientOld.right - rcClientOld.left !=
        m_rcWindowClient.right - m_rcWindowClient.left ||
        rcClientOld.bottom - rcClientOld.top !=
        m_rcWindowClient.bottom - m_rcWindowClient.top)
    {
        // A new window size will require a new backbuffer
        // size, so the 3D structures must be changed accordingly.
     
		m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

        // Resize the 3D environment
        if( FAILED( hr = resizeD3D() ) )
        {
            displayErrorMsg( D3DAPPERR_RESIZEFAILED, MSGERR_APPMUSTEXIT );
            return false;
        }
	}
	
	return true;
}

bool hkgWindowDX8PC::updatePos( unsigned int x, unsigned int y)
{
	return updateSizeD3D() && hkgWindow::updatePos( x, y );
}

bool hkgWindowDX8PC::updateJoystick()
{
	if (m_directJoystick && m_directJoystick->poll())
	{
		float dx = m_directJoystick->getXAxis() - m_pad[0].getStickPosX(0);
		float dy = m_directJoystick->getYAxis() - m_pad[0].getStickPosY(0);

		if ( (hkg_fabs( dx ) > 0.01f) || (hkg_fabs(dy) > 0.01f) )
		{
		//	printf(" X[%.3f] Y[%.3f]\n",m_directJoystick->getXAxis(), m_directJoystick->getYAxis() );
			processPadStickMove( 0, 0, m_directJoystick->getXAxis(), m_directJoystick->getYAxis() );
		}
		
		float pov0 = m_directJoystick->getPov(0);
		float prevPov0 = m_directJoystick->getPrevPov(0);

		if ( pov0 != prevPov0 )
		{
			
			if ( pov0 < 0.0f )
				processPadStickMove( 0, 1, 0, 0);
			else
			{
				float x = 0;
				float y = 0;
			
				float radians = pov0 * HKG_PI / 180.0f;

				x = hkg_sin( radians );
				y = hkg_cos( radians );

			//	printf(" Pov X[%.3f] Y[%.3f]\n", x, y);
				processPadStickMove( 0, 1, x, y);
			}
		}
	

		for (unsigned int i=0; i < 16; ++i)
		{
			bool state = m_directJoystick->getButton(i);
			bool prevState = m_directJoystick->getPrevButton(i);
			
			if ( state != prevState )
			{
			//	printf(" Button [%d --> %d]\n", i, state? 1 : 0);
		  		processPadButton( 0, _PCButtonMap[i], state);
			}
		}		

		return true;
	}
	else
	{
		return false;
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
