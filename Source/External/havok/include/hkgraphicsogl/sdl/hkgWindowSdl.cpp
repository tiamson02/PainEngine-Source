/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/sdl/hkgWindowSdl.h>
#include <stdlib.h>
#include <ctype.h>

#include <SDL.h>
#include <GL/gl.h>

static GLfloat black[] = { 0, 0, 0, 0 };
static GLfloat white[] = { 1.0f, 1.0f,1.0f,1.0f };

#define ERROR_IF(cond,where,whattoreturn) \
	if(cond) \
	{ \
		hkcout << "SDL error in " << where << "("<< SDL_GetError() << ")" << hkendl; \
		return whattoreturn; \
	} else

hkgWindowSdl::hkgWindowSdl()
	: hkgWindow()
{
	int init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
	ERROR_IF( init < 0, "SDL_Init", /*nothing*/ );
	atexit(SDL_Quit);

	setWantDrawMousePointer(false);
}

bool hkgWindowSdl::initialize( HKG_WINDOW_CREATE_FLAG flags, HKG_WINDOW_BUFFERS buffers, int w, int h, const char* name,void* platformHandle, void* platformParentHandle)
{
	w = 800;
	h = 600;
	const SDL_VideoInfo* videoinfo = SDL_GetVideoInfo();
	ERROR_IF( videoinfo == HK_NULL, "SDL_GetVideoInfo", false);

	if (videoinfo->vfmt->BitsPerPixel <= 16)
	{
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	}
	else
	{
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	}

	m_surface = SDL_SetVideoMode( w, h, videoinfo->vfmt->BitsPerPixel, SDL_OPENGL | SDL_RESIZABLE);
	ERROR_IF(m_surface == HK_NULL, "SDL_SetVideoMode", false);

	SDL_WM_SetCaption(name, HK_NULL);

	hkgWindow::initialize(flags, buffers, w,h,name, platformHandle, platformParentHandle);
	return true;
}

static void mousebutton(bool isdown, hkgWindow* window, const SDL_Event& event)
{
	int x = event.button.x;
	int y = window->getHeight() - event.button.y;
	switch(int(event.button.button))
	{
		case SDL_BUTTON_LEFT:
			window->processMouseButton( HKG_MOUSE_LEFT_BUTTON, isdown, x, y );
			break;
		case SDL_BUTTON_MIDDLE:
			window->processMouseButton( HKG_MOUSE_MIDDLE_BUTTON, isdown, x, y );
			break;
		case SDL_BUTTON_RIGHT:
			window->processMouseButton( HKG_MOUSE_RIGHT_BUTTON, isdown, x, y );
			break;
		case 4:
			window->processMouseWheel( 1, x, y );
			break;
		case 5:
			window->processMouseWheel( -1, x, y );
			break;
	}
}

static int translate(int k)
{
	switch(k) // order is the same as hkgraphics/hkgKeyboard.h
	{
		case SDLK_RETURN:
			return HKG_VKEY_RETURN;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return HKG_VKEY_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return HKG_VKEY_CONTROL;

		case SDLK_SPACE:
			return HKG_VKEY_SPACE;
		case SDLK_END:
			return HKG_VKEY_END;
		case SDLK_HOME:
			return HKG_VKEY_HOME;

		case SDLK_LEFT:
			return HKG_VKEY_LEFT;
		case SDLK_RIGHT:
			return HKG_VKEY_RIGHT;
		case SDLK_UP:
			return HKG_VKEY_UP;
		case SDLK_DOWN:
			return HKG_VKEY_DOWN;

		case SDLK_INSERT:
			return HKG_VKEY_INSERT;
		case SDLK_DELETE:
			return HKG_VKEY_DELETE;
	}
	if( k >= SDLK_F1 && k <= SDLK_F15)
	{
		return k - SDLK_F1 + HKG_VKEY_F1;
	}
	return k; // ascii '0'-'9' and 'a'-'z' map identically.
}

HKG_WINDOW_MSG_STATUS hkgWindowSdl::peekMessages(bool inViewportResize)
{
	SDL_Event event;
	HKG_WINDOW_MSG_STATUS status = HKG_WINDOW_MSG_CONTINUE;

	if ((!inViewportResize) && (m_viewportResizeArray.getSize() > 0))
	{
		status = doViewportResizeLoop();	
	}

	stepInput(); 

	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case SDL_VIDEORESIZE:
			{
				int w = event.resize.w;
				int h = event.resize.h;
				m_surface = SDL_SetVideoMode(w,h, 0, SDL_OPENGL | SDL_RESIZABLE);
				updateSize( w, h);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				mousebutton(true, this, event);
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				mousebutton(false, this, event);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				processMouseMove( event.motion.x, getHeight()-event.motion.y );
				break;
			}

			case SDL_KEYDOWN:
			{
				if( event.key.keysym.sym == SDLK_ESCAPE )
				{
					SDL_Event e;
					e.type = SDL_QUIT;
					SDL_PushEvent(&e);
				}
				processKey( translate(event.key.keysym.sym), true);

				break;
			}
			case SDL_KEYUP:
			{
				processKey( translate(event.key.keysym.sym), false);
				break;
			}
			case SDL_QUIT:
			{
				return HKG_WINDOW_MSG_QUIT;
				break;
			}
			case SDL_ACTIVEEVENT:
			{
				if( 0 ) // pause on focus lost?
				{
					// if lost focus
					if( (event.active.gain == 0 ) &&
						(event.active.state == SDL_APPINPUTFOCUS || event.active.state == SDL_APPACTIVE))
					{
						while(1) // wait for focus to come back
						{
							SDL_Event e;
							SDL_WaitEvent(&e); // block here
							if( (e.type == SDL_ACTIVEEVENT) && 
								(e.active.gain == 1) &&
								(e.active.state == SDL_APPINPUTFOCUS || e.active.state == SDL_APPACTIVE))
							{
								break;
							}
						} 
					}
				}
				break;
			}
			default:
			{
				//hkcout << "Unhandled event type " << int(event.type) << hkendl;
			}
		}
	}
	return status;
}


bool hkgWindowSdl::swapBuffers()
{
	finishScene();
	glFlush();
	SDL_GL_SwapBuffers();
	return true;
}
bool hkgWindowSdl::clearBuffers()
{
	if ( (m_context->getEnabledState() & HKG_ENABLED_ZWRITE) == 0)
	{
		m_context->setDepthWriteState( true );
	}

	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	return true;
}


hkgWindowSdl::~hkgWindowSdl()
{
	SDL_Quit();
}


static void setupDefaultLights()
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
   {
	   glDisable((GLenum)(GL_LIGHT0 + i));
   }

//	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
//	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
//	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
}

bool hkgWindowSdl::initializeContext()
{
	hkgWindow::initializeContext();
	
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

	glMaterialfv( GL_FRONT, GL_EMISSION, black );
   	glMaterialfv( GL_FRONT, GL_SPECULAR, black);
     
	setupDefaultLights();

	return true;
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
