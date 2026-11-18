/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok Game Implementation
//

#include <hkdemoframework2/hkDemoFramework.h>
#include <hkdemoframework2/hkGameEnvironment.h>
#include <hkdemoframework2/hkTextDisplay.h>
#include <hkdemoframework2/hkPerformanceCounterUtility.h>
#include <hkdemoframework2/hkGameDatabase.h>
#include <hkdemoframework2/hkDefaultGame.h>

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgLightManager.h>

#include <hkdynamics2/world/hkWorld.h>

#include <hkgraphicsbridge2/hkgSystem.h>
#include <hkgraphicsbridge2/hkgDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkDisplayCallbackUtil.h>
#include <hkutilities2/visualdebugger/hkVisualDebugger.h>

#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/stream/hkStringStream.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkdemoframework2/hkPseudoPad.h>
#include <hkdemoframework2/hkPackedStreambufSource.inl>

#ifdef HK_PS2
#	include <libpc.h>
#	include <sifdev.h>
#	if defined(HK_COMPILER_MWERKS)
#		include <mwutils.h>
#	endif
	PS2SetupOptions::PS2SetupOptions()
	{
		m_PS2modulePath = "host0:/usr/local/sce/iop/modules/";
		m_PS2modules[0] = "sio2man.irx";
		m_PS2modules[1] = "padman.irx";
		m_rebootIOP		= IOP_IMAGE_file;
		for (int i=0 ;i < HK_MAX_IRX_MODULES; i++)
		{
			m_PS2modules[i] = HK_NULL;
			m_PS2moduleOptions[i] = HK_NULL;
			m_PS2moduleOptionsLength[i] = 0;
		}
	}
#endif

#include <stdio.h>

class NullStreambuf : public hkStreambuf
{
	public:
	NullStreambuf() { m_flags |= END_OF_FILE; }
	virtual int sysWrite(const char* buf, int nbytes) { return 0;}
	virtual int sysRead(char* buf, int nbytes) { return 0; }
	virtual void sysFlush() {}
};

// Utility class for persistent text display. Substitutes "demo" for "game" in the framework menus
// in the virtual function outputText.
class hkIndustrialTextDisplay : public hkTextDisplay
{
	public:

			// Create a text display attached to window 'window'
		hkIndustrialTextDisplay( hkgWindow* window );

			// Request 'str' to be rendered at absolute screen position x,y.
			// Substitutes "demo" for "game" in str.
		virtual void outputText(const hkString& str, int x, int y, hkUint32 color = 0xffffffffU, int frames = 1 );
};

hkIndustrialTextDisplay::hkIndustrialTextDisplay(hkgWindow* window)
:	hkTextDisplay(window)
{
}

void hkIndustrialTextDisplay::outputText(const hkString& str, int x, int y, hkUint32 color, int frames )
{
	hkTextDisplay::outputText( str.replace("game", "demo"), x, y, color, frames );
}

class hkDemoStreambufFactory : public hkStreambufFactory
{
	public:

		hkDemoStreambufFactory()
		{
			hkStreambufFactory* parent = &hkStreambufFactory::getInstance();
			parent->addReference();
			m_parent = parent;
		}

		hkStreambuf* open(hkStreambuf::StdStream s)
		{
			return m_parent->open(s);
		}

		hkStreambuf* open(const char* name, hkStreambuf::OpenMode mode )
		{
			const int NUM_LOCATIONS = 2;
			hkString locations[NUM_LOCATIONS];
			locations[0] = ""; // try absolute path first, then demo compile location.
			locations[1] = hkGameDatabase::getInstance().getPrefix().cString();

			for(int i = 0; i < NUM_LOCATIONS; ++i)
			{
				hkString p;
				p += locations[i] + name;

#if defined(HK_PLATFORM_PS2)
				// On PS2 add host0: if it isn't prefixed with a valid device
				if ( (p.beginsWith("host0") == false) && (p.beginsWith("cdrom0") == false))
				{
					p = hkString("host0:") + p;
				}
#elif defined(HK_PLATFORM_XBOX)
				p = hkString("d:/") + p;
				p = p.replace("/", "\\");
#endif

				hkStreambuf* sb = m_parent->open(p.cString(), mode);
				if(sb->isOk())
				{
					return sb;
				}
				else
				{
					sb->removeReference();
				}
			}
			return new NullStreambuf();
		}

		hkStreambufFactory* m_parent;
};


hkDemoFrameworkOptions::hkDemoFrameworkOptions()
:	m_windowed(true),
	m_debugger(false),
	m_industrial(false),
#if defined(HK_PLATFORM_PS2) || defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_GC)
	m_lockFps(0), // consoles dont need framelock.
#else
	m_lockFps(60),
#endif
	m_defaultGame(HK_NULL),
	m_windowTitle("Havok"),
	m_masterFile(HK_NULL),
	m_renderer(HK_NULL),
	m_statsDir(HK_NULL),
	m_enablePerformanceCounter( false ),
	m_argc(0),
	m_argv(HK_NULL)
{
#ifdef HK_PS2
	m_rebootIOP	= true;
#endif
}

hkDemoFrameworkOptions::~hkDemoFrameworkOptions()
{
}

void hkDemoFrameworkOptions::parse(int argc, const char** argv)
{
	m_argc = argc;
	m_argv = argv;

	int i = 1;
	while( i < argc )
	{
		if(argv[i][0] == '-')
		{
			switch( hkString::toLower(argv[i][1]))
			{
				case 'b': // dont boot iop
				{
#					if defined(HK_PS2)
						m_rebootIOP = false;
#					endif
					break;
				}
				case 'd': // debugger
				{
					m_debugger = true;
					break;
				}
				case 'f': // fullscreen
				{
					m_windowed = false;
					break;
				}
				case 'g':
				{
					if(++i < argc)
					{
						m_defaultGame = argv[i];
					}
					break;
				}
				case 'i': // Industrial Logo, name and other options
				{
					m_industrial = true;
					m_windowTitle = "Telekinesys";
					break;
				}
				case 'l':
				{
					m_lockFps = -1;
					if( argv[i][2] != HK_NULL) // '-l60'
					{
						m_lockFps = hkString::atoi(argv[i]+2);
					}
					else if( i+1 < argc && argv[i+1][0] != '-') // '-l' '60'
					{
						++i;
						m_lockFps = hkString::atoi(argv[i]);
					}
					if( m_lockFps == -1) // '-l' or parse error
					{
						m_lockFps = 60;
					}
					break;
				}
				case 'm':
				{
					if(++i < argc)
					{
						m_masterFile = argv[i];
					}	
					break;
				}
				case 'p':
				{
					m_enablePerformanceCounter = true;
					break;
				}
				case 'r': // renderer
				{
					if(++i < argc)
					{
						m_renderer = argv[i];
					}
					break;
				}
				case 's': // set the stats output directory
				{
					if(++i < argc)
					{
						m_statsDir = argv[i];
					}	
					break;
				}
				case 't':
				{
					if(++i < argc)
					{
						m_windowTitle = argv[i];
					}	
					break;
				}
				case 'w': // windowed
				{
					m_windowed = true;
					break;
				}
				default:
				{
					printf("option `%s' was not understood", argv[i]);
				}
			}
		}
		else
		{
			printf("option `%s' was not understood", argv[i]);
		}

		++i;
	}
}

static void HK_CALL setupDefaultLights(hkGameEnvironment& environment)
{
	// make some default lights
	hkgLightManager* lm = hkgLightManager::create();
	environment.m_displayWorld->setLightManager( lm );

	{
		float v[4]; v[3] = 256;
		hkgLight* light;

		v[0] = 183;
		v[1] = 194;
		v[2] = 213;
		v[0] /= v[3]; v[1] /= v[3]; v[2] /= v[3];
		environment.m_window->setClearColor( v );

		// the sun
		{
			light = hkgLight::create();
			light->setType( HKG_LIGHT_DIRECTIONAL );
			v[0] = 256;
			v[1] = 256;
			v[2] = 256;
			v[0] /= v[3]; v[1] /= v[3]; v[2] /= v[3];
			light->setDiffuse( v );
			v[0] = -113;
			v[1] = 475;
			v[2] = -200;
			light->setDirection( v );
			light->setPosition( v );
			light->setDesiredEnabledState( true );
			lm->addLight( light );
			light->release();
		}
		// fill 1 - blue
		{
			light = hkgLight::create();
			light->setType( HKG_LIGHT_DIRECTIONAL );
			v[0] = 200;
			v[1] = 200;
			v[2] = 240;
			v[0] /= v[3]; v[1] /= v[3]; v[2] /= v[3];
			light->setDiffuse( v );
			v[0] = -420;
			v[1] = -370;
			v[2] = -95;
			light->setDirection( v );
			light->setPosition( v );
			light->setDesiredEnabledState( true );
			lm->addLight( light );
			light->release();
		}
		// fill 2 - yellow
		{
			light = hkgLight::create();
			light->setType( HKG_LIGHT_DIRECTIONAL );
			v[0] = 240;
			v[1] = 240;
			v[2] = 200;
			v[0] /= v[3]; v[1] /= v[3]; v[2] /= v[3];
			light->setDiffuse( v );
			v[0] = 500;
			v[1] = -50;
			v[2] = 280;
			light->setDirection( v );
			light->setPosition( v );
			light->setDesiredEnabledState( true );
			lm->addLight( light );
			light->release();
		}
		lm->computeActiveSet( HKG_VEC3_ZERO );
	}

	lm->release();
}

static void render(hkgWindow* window, const hkGameEnvironment& env, bool wireframe)
{
	for( int i = 0; i < window->getNumViewports(); i++ )
	{
		hkgViewport* v = window->getViewport(i);
		hkgDisplayWorld* dw = env.m_displayWorld;

		// send the user camera
		{
			hkgCamera* c = v->getCamera();

			float floatFrom[3]; c->getFrom(floatFrom);
			float floatTo[3]; c->getTo(floatTo);
			float floatUp[3]; c->getUp(floatUp);

			hkVector4 from( floatFrom[0], floatFrom[1], floatFrom[2] );
			hkVector4 to( floatTo[0], floatTo[1], floatTo[2] );
			hkVector4 up( floatUp[0], floatUp[1], floatUp[2] );

			HK_UPDATE_CAMERA(from, to, up, c->getNear(), c->getFar(), c->getFOV(), "Demo Framework");
		}

		v->setDesiredState( (unsigned short)( v->getDesiredState() | HKG_ENABLED_LIGHTING & ~HKG_ENABLED_WIREFRAME) );
		v->setAsCurrent( window->getContext() );
		dw->render( window->getContext() );
		env.m_displayHandler->drawImmediate();

#if defined( HK_PLATFORM_PS2 )
		wireframe = false;
#endif
		if( wireframe)
		{
			hkgCamera* camera = v->getCamera();
			float f[3];
			float d[3];
			camera->getDir( d );
			camera->getFrom( f );

			// disable all the lights.
			// we want the lines drawn as black.
			{
				for( int i = dw->getLightManager()->getNumLights() - 1; i >= 0; i-- )
				{
					dw->getLightManager()->disable( i );
				}

				dw->getLightManager()->computeActiveSet( f );
			}

			// move camera slightly forward to ensure
			// lines get drawn
			hkReal n = 0.01f;
			d[0] *= n; d[1] *= n; d[2] *= n;
			f[0] += d[0]; f[1] += d[1]; f[2] += d[2];
			camera->setFrom( f );
			camera->computeProjection();
			camera->computeModelView();

			v->setDesiredState( (unsigned short)( v->getDesiredState() | HKG_ENABLED_LIGHTING | HKG_ENABLED_WIREFRAME ) );
			v->setAsCurrent( window->getContext() );
			dw->render( window->getContext() );

			// move camera back to original position
			f[0] -= d[0]; f[1] -= d[1]; f[2] -= d[2];
			camera->setFrom( f );
			camera->computeProjection();
			camera->computeModelView();

			// enable all the lights...
			{
				for( int i = dw->getLightManager()->getNumLights() - 1; i >= 0; i-- )
				{
					dw->getLightManager()->enable( i );
				}

				dw->getLightManager()->computeActiveSet( f );
			}
		}
	}
	env.m_textDisplay->displayText(window);
	window->swapBuffers();
}


int HK_CALL hkFrameworkMain (const hkDemoFrameworkOptions& options, char* startUpGame);
int HK_CALL hkFrameworkMain (const hkDemoFrameworkOptions& options, char* startUpGame)
{
#if defined(HK_PS2)
#	if defined(HK_COMPILER_MWERKS)
		mwInit();
#	endif
#endif

	hkgSystem::init(options.m_renderer);
	{
		hkDemoStreambufFactory* dsb = new hkDemoStreambufFactory();
		hkStreambufFactory::replaceInstance(dsb);

		if (options.m_masterFile != HK_NULL)
		{
			hkStreambuf* masterFile = hkStreambufFactory::getInstance().open(options.m_masterFile, hkStreambuf::INPUT);
			if (masterFile->isOk())
			{
				// For PS2 cdrom
				bool seekBug = (hkString::strNcasecmp("cdrom" , options.m_masterFile, 5) == 0);
				// Replace with the masterfile
				hkPackedStreambufSource* msb = new hkPackedStreambufSource(masterFile, seekBug);
				masterFile->removeReference();
				hkStreambufFactory::replaceInstance(msb);
			}
		}

		hkgWindow* window = hkgWindow::create();
		window->initialize( options.m_windowed ? HKG_WINDOW_WINDOWED : HKG_WINDOW_FULLSCREEN,
							HKG_WINDOW_BUF_COLOR | HKG_WINDOW_BUF_DEPTH32, 640, 480,
							options.m_windowTitle);
		
		// don't allow viewport resizing
		window->setWantViewportBorders(false);
		window->setWantViewportResizeByMouse(false);

		for(int i = 0; i < 2; ++i)
		{
			window->clearBuffers();
			window->swapBuffers();
		}

		hkPseudoPad* pad = new hkPseudoPad();

		hkGameEnvironment environment;
		environment.m_window = window;
		environment.m_displayWorld = hkgDisplayWorld::create();
		if (options.m_industrial)
		{
			// Use Industrial logo (or none at all for now).
			environment.m_window->setWantDrawHavokLogo(false);
			// Substitute "demo" for "game" in the framework menus.
			environment.m_textDisplay = new hkIndustrialTextDisplay(window);
		}
		else
		{
			environment.m_textDisplay = new hkTextDisplay(window);
		}
		extern hkgPad* createPad();
		environment.m_gamePad = pad;
		environment.m_displayHandler = new hkgDisplayHandler(environment.m_displayWorld, environment.m_window->getContext());
		environment.m_performanceCounter = new hkPerformanceCounterUtility();
		environment.m_options = &options;

		if (options.m_enablePerformanceCounter)
		{
			environment.m_performanceCounter->enable();
		}

		setupDefaultLights(environment);

		HK_ADD_STEP_DISPLAY_LISTENER(environment.m_displayHandler);

		hkGameDatabase& database = hkGameDatabase::getInstance();

		hkGame* game = database.createGame(startUpGame, &environment);

		environment.m_performanceCounter->setGameName( startUpGame );

		const hkUint64 minTicksPerFrame = options.m_lockFps
										? (hkStopwatch::getTicksPerSecond() / options.m_lockFps)
										: 0;
		const int MAX_FRAMES = 10;
		int frameTimes[MAX_FRAMES] = {0};
		int curFrame = 0;
		hkStopwatch ticksSoFar;
		ticksSoFar.start();

		while ( window->peekMessages() == HKG_WINDOW_MSG_CONTINUE )
		{
#ifdef HK_PS2
			// On PS2 we must restart the performance counters every 7 seconds
			// otherwise we will get an exception which cannot be caught by the user!
			// Please don't move it out, unless you are certain everything works ok on ps2
			ticksSoFar.stop();
			scePcStart( SCE_PC_CTE | SCE_PC_U0 | SCE_PC_U1 | SCE_PC0_ICACHE_MISS | SCE_PC1_CPU_CYCLE, 0 ,0 );
			ticksSoFar.start();
#endif

			pad->updatePad(window, 0.01f);
			window->clearBuffers();

			environment.m_performanceCounter->startCounter();
			hkGame::Result step = game->stepGame();
			environment.m_performanceCounter->stopCounter();

			if ( step != hkGame::GAME_STOP && step != hkGame::GAME_RESTART )
			{
				if( step != hkGame::GAME_PAUSED )
				{
					stepVisualDebugger();
				}

				if(options.m_lockFps)
				{
					hkUint64 ticks = ticksSoFar.getElapsedTicks();
					while( ticks < minTicksPerFrame )
					{
						ticks = ticksSoFar.getElapsedTicks();
					}
					frameTimes[curFrame++] = static_cast<int>(ticks);
					curFrame = curFrame % MAX_FRAMES;

					ticksSoFar.reset();
					ticksSoFar.start();

					int minTime = frameTimes[0];
					for(int i = 1; i < MAX_FRAMES; ++i)
					{
						if(frameTimes[i] < minTime)
						{
							minTime = frameTimes[i];
						}
					}

					// Comment this in to display the frame rate in the top left corner
//					int fps = 1 + int(hkStopwatch::getTicksPerSecond()/(minTime+1));
//					hkString str; str.printf("%i", fps);
//					environment.m_textDisplay->outputText(str, 0,0);
				}
				
				render(window, environment, false);
			}
			else if ( step == hkGame::GAME_STOP )
			{
				break;
			}
			else if ( step == hkGame::GAME_RESTART )
			{
				delete game;
				game = database.createGame(options.m_defaultGame, &environment);
			}
		}

		HK_REMOVE_STEP_DISPLAY_LISTENER(environment.m_displayHandler);
		environment.m_performanceCounter->saveFileAndReset();
		delete pad;
		delete game;
		delete environment.m_textDisplay;
		delete environment.m_displayHandler;
		delete environment.m_performanceCounter;
		environment.m_displayWorld->release();
		window->release();

		hkStreambufFactory::replaceInstance(dsb->m_parent);
	}

	hkgSystem::quit();
	return 0;
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
