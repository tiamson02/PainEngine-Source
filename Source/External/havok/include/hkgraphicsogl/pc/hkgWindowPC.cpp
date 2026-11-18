/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/pc/hkgWindowPC.h>
#include <hkgraphicsogl/pc/hkgWindowManagerPC.h>
#include <hkgraphicsogl/pc/hkgDirectJoystickPC.h>

static const char* hkgWindowClassName = "HKGRAPHICSWINDOWOGL";
static const char* hkgWindowName = "Havok (ogl)";

static GLfloat clearColor[] = { 0.3f, 0.3f, 0.4f, 0.0f };
static GLfloat black[] = { 0, 0, 0, 0 };
static GLfloat white[] = { 1.0f, 1.0f,1.0f,1.0f };

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
	HKG_PAD_BUTTON_R2 
};

hkgWindow* hkgWindowPC::createWindowPC()
{
	return new hkgWindowPC( GetModuleHandle(HK_NULL) );
}

hkgWindowPC::hkgWindowPC( HINSTANCE instance )
: hkgWindow(), m_hinstance( instance ), m_directJoystick(NULL), m_ownHWND(true)
{

}

static bool _setupPixelFormat(HDC& dc, HKG_WINDOW_BUFFERS buffers) {  

	DEVMODE	vmode;
	int currentBPP = 32;

	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &vmode))
	{
		if (vmode.dmBitsPerPel >= 15)
		{
			if (vmode.dmBitsPerPel == 15)
				currentBPP = 16;

			else //   leave at 32 or change to 24/16?
				currentBPP = vmode.dmBitsPerPel;
		}
		else
		{
			MessageBox(NULL, "This renderer requires a color depth of at least 16bpp.\nPlease use a higher desktop color depth and\ntry again or run in fullscreen mode.\n\nThe application will now exit.", "Havok Display", MB_ICONERROR|MB_OK);
			return false;
		}
	}

	PIXELFORMATDESCRIPTOR pfd;
	
	hkString::memSet(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_TYPE_RGBA | PFD_DOUBLEBUFFER;
	pfd.cColorBits   = currentBPP;
	pfd.cDepthBits   = (buffers & HKG_WINDOW_BUF_DEPTH16) ? 16 : 32;
	pfd.iLayerType   = PFD_MAIN_PLANE;

	if (buffers & HKG_WINDOW_BUF_STENCIL)
		pfd.cStencilBits = 16;

	int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(dc, &pfd)) == 0 ) 
	{ 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return false;     
	}  
    if (SetPixelFormat(dc, pixelformat, &pfd) == FALSE)     
	{ 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return false;     
	} 

	return true; 
} 


bool hkgWindowPC::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name, void* platformHandle, void* platformParentHandle)
{
	const char* winName = name? name : hkgWindowName;

	if ( platformHandle==HK_NULL )
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC) hkgWindowProcOGLPC;
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

		if (flags & HKG_WINDOW_FULLSCREEN)
		{
			DEVMODE vidMode;
			vidMode.dmSize = sizeof(DEVMODE);
			vidMode.dmDriverExtra = 0;
			vidMode.dmBitsPerPel = 32;
			vidMode.dmPelsWidth = w;
			vidMode.dmPelsHeight = h;
			vidMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
			
			ChangeDisplaySettings(&vidMode, CDS_FULLSCREEN);			

			platformHandle = CreateWindowEx(WS_EX_TOPMOST, hkgWindowClassName, winName, 
					WS_POPUP | WS_VISIBLE, 
					0, 0, 
					w,
					h,
					(HWND) platformParentHandle, (HMENU) NULL, 
					m_hinstance, (LPVOID) this);

			ShowCursor( FALSE );
			m_drawMousePointer = true;
		}
		else
		{
			platformHandle = CreateWindow(
					hkgWindowClassName, winName, WS_OVERLAPPEDWINDOW, 
					CW_USEDEFAULT, CW_USEDEFAULT,
					w, h, (HWND) platformParentHandle, (HMENU) NULL,(HINSTANCE)m_hinstance, 
					(LPVOID) this);
		
			m_drawMousePointer = false;

		}

		m_ownHWND = true;
	}
	else
		m_ownHWND = false;


	if (platformHandle==HK_NULL)
		return false;

// no need for this as it will be added by the WM_CREATE msg
//	else
//		g_hkgWindowManagerPC.addWindow( (HWND) m_platformHandle, this );


	// the DC:
	m_dc = GetDC( (HWND) platformHandle);

	// the GL DC:
	if (!_setupPixelFormat(m_dc, buffers))
		return false;
	
	m_hglrc = wglCreateContext(m_dc); 
	
	// make current
	if (!wglMakeCurrent(m_dc, m_hglrc))
		return false;

	// make the viewports etc:
	hkgWindow::initialize(flags, buffers, w, h, name, platformHandle, platformParentHandle);

	// finally, show the window:
	UpdateWindow((HWND)m_platformHandle);
	ShowWindow((HWND)m_platformHandle, SW_SHOWNORMAL);	

	//loadStandardExtensions();

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

hkgWindowPC::~hkgWindowPC()
{
	if (m_hglrc)
	{	
		// make the rendering context not current
		ReleaseDC( (HWND) m_platformHandle, m_dc);
		wglMakeCurrent(NULL, NULL) ; 
		wglDeleteContext(m_hglrc);

		// restore vidmode if we have changed it in this class:	
		if (m_fullscreen)
			ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
	}

	if (m_directJoystick)
		delete m_directJoystick;

	if (m_platformHandle)
	{
		hkgWindowManagerPC::getInstance().removeWindow( (HWND) m_platformHandle );

		if (m_ownHWND)
		{
			HRESULT res = DestroyWindow((HWND) m_platformHandle);
			UnregisterClass(hkgWindowClassName,m_hinstance);
		}
	}
}

// will loop on windows messages if any. Will return if idle.
HKG_WINDOW_MSG_STATUS hkgWindowPC::peekMessages(bool inViewportResize)
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

void hkgWindowPC::waitMessages()
{
	WaitMessage();
}

bool hkgWindowPC::clearBuffers()
{
	// we can't clear Z if we don't write to it :-)
	if ( (m_context->getEnabledState() & HKG_ENABLED_ZWRITE) == 0)
		m_context->setDepthWriteState( true );

	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}

bool hkgWindowPC::swapBuffers()
{
	finishScene();

	return (SwapBuffers(m_dc) == TRUE);
}

bool hkgWindowPC::setAsCurrent()
{
	wglMakeCurrent(m_dc, m_hglrc);
	return true;
}


static void _setupDefaultLights()
{
	    // lighting
   GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};

   GLfloat l0_position[] = {0.0f, 1.0f, 1.0f, 0.0f};

#ifdef HK_PLATFORM_PS2
    GLfloat l0_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f };
#else
	GLfloat l0_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f };
#endif

   GLfloat l1_position[] = {0.0f, -20.0f, -80.0f, 1.0f};
   // GLfloat l1_position[] = {0, -1, 1, 0.0};
   GLfloat l1_diffuse[] = {.6f, .6f, .6f, 1.0f };

 
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);

   glLightfv(GL_LIGHT0, GL_AMBIENT, black);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, black);
   glLightfv(GL_LIGHT0, GL_POSITION, l0_position);

   glLightfv(GL_LIGHT1, GL_AMBIENT, black);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, black);
   glLightfv(GL_LIGHT1, GL_POSITION, l1_position);

     
   for (int i =2; i < 8; ++i)
	   glDisable(GL_LIGHT0 + i);

//   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
  // glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
 //  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
}


bool hkgWindowPC::initializeContext()
{
	hkgWindow::initializeContext();
	
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

	glMaterialfv( GL_FRONT, GL_EMISSION, black );
   	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
     
	_setupDefaultLights();

	return true;
}


bool hkgWindowPC::updateJoystick()
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
		return false;
}


void	hkgWindowPC::setMousePosition(int	x,int	y)
{
	int	screenX,screenY;
	screenX = x;
	screenY = y;
	
	POINT pt;
	pt.x = x;
	pt.y = getHeight()-y;

	ClientToScreen((HWND)m_platformHandle,&pt);

	::SetCursorPos(pt.x,pt.y);
}


void	hkgWindowPC::showMouseCursor(bool	visible)
{
	if (!isFullscreen())
	{
		ShowCursor(visible);
	} else
	{
		m_drawMousePointer = visible;
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
