#error This file is obselete

#include "Engine.h"

EngineApp::EngineApp()
:   hInstance(0), TimeMultiplier(1)
,   RenderWnd(0), Game(0),	Physics(0), MaterialSys(0), Audio(0)
,   RenderDev(0), RenderDevType(RenderDeviceManager::ENullRenderDevice)
,   IsFullScreen(0), IsDedicated(0), ExitRequested(0), UsingShaders(0)
,	StartMultiplayerClient( false ), ServerToConnectTo(""), PlayerNameToAssume("John Doe")
,	PasswordToGive("")
{}

EngineApp::~EngineApp()
{
    Shutdown();
}

BOOL EngineApp::Initialize(HINSTANCE hinst, const char* cmdLine)
{
    guard(EngineApp::Initialize);

    hInstance=hinst;
    Assert(hInstance);

    // dump log to file
#ifdef TRANSGAMING
    GLog.Open("host0:pk.log");
#else
    GLog.Open("pk.log");
#endif

    // open config file
    const char* configFile="painkiller.ini";
    const char* s=strstr(cmdLine,"-ini");
    if (s) 
	{
        // voodoo: at least 1 letter+.ini ext
        if (strlen(s)>=10) 
		{
            configFile=s+5;
            GLog.Print("Using ini: %s\n",configFile);
        }
    }

    if (!GConfig.Load(configFile)) 
	{
        const char* errMsg="ERROR: config file could not be opened\n";
        GLog.Print(errMsg);
        MessageBox(0,errMsg,"ERROR",MB_OK);
        return FALSE;
    }

    s = GConfig.GetString("General","UsingShaders");
    if (s && !stricmp(s,"true"))
        UsingShaders = 1;

    // parse command line
	// Krystian: added parameters required by GameSpy to start multiplayer game from within GameSpy Arcade
    DynamicArray<String> args;
    String::TokenizeWithLiteral(cmdLine," ",args,'"');

	const char* gameName = 0;
	for (int i=0; i<args.Size(); ++i)
	{
		if( args[i] == "-game" && i<args.Size()-1 )
		{
			//strlwr(&args[i+1][0]);
			gameName = args[i+1].AsChar();
			i++;
			continue;
		}

		if( args[i] == "+connect" && i<args.Size()-1 )
		{
			ServerToConnectTo = args[i+1];
			StartMultiplayerClient = true;
			i++;
			continue;
		}
		if( args[i] == "+name" && i<args.Size()-1 )
		{
			PlayerNameToAssume = args[i+1];
			i++;
			continue;
		}
		if( args[i] == "+password" && i<args.Size()-1 )
		{
			PasswordToGive = args[i+1];
			i++;
			continue;
		}
	}

    // init system driver
    SysDriver.Initialize(hInstance);

    // check args    
    GLog.Print("PK Log started: %s\n",SysDriver.GetTimeAsString());
    SysDriver.Dump(GLog);

    // various systems
    m_setTag("Engine");
	Physics		= new PhysicsEngine();
    m_setTag("Engine");
	Audio		= new AudioDevice();
    m_setTag("Engine");
    MaterialSys = new MaterialSystem();
    m_setTag("Engine");
	Net			= new IPDriver();

	Physics->Init();

    // start game
	if (!gameName)
		gameName=GConfig.GetString("GameInfo","StartGame");

    if (gameName) 
	{
        Game=AbstractFactory<BaseGame>::GetInstance().Create(gameName);
        if (Game) 
		{
#ifdef TRANSGAMING
            TRACE("*** EngineApp::Initialize(): initializing game engine\n");
#endif
            if (Game->Initialize()) 
            {
#ifdef TRANSGAMING
                TRACE("*** EngineApp::Initialize(): game engine successfully initilaized\n");
#endif
				ResetTimer();
                return TRUE;
            }
            else
            {
#ifdef TRANSGAMING
                TRACE("*** EngineApp::Initialize(): game engine initilaization failed\n");
#else
                char tmp[256];
                sprintf(tmp,"ERROR: could not Initialize Game: %s\n",gameName);
                GLog.Print(tmp);
                MessageBox(0,tmp,"ERROR",MB_OK);
#endif

            }
        }
        else 
		{
#ifdef TRANSGAMING
            TRACE("EngineApp::Initialize(): ERROR-> could not create the game '%s'\n", gameName);
#else
            char tmp[256];
            sprintf(tmp,"ERROR: could not create Game: %s\n",gameName);
            GLog.Print(tmp);
            MessageBox(0,tmp,"ERROR",MB_OK);
#endif
        }
    }
    else 
	{
        const char* errMsg="ERROR: no StartGame found in INI\n";
        GLog.Print(errMsg);
        MessageBox(0,errMsg,"ERROR",MB_OK);
    }

    return FALSE;
    unguard;
}

void EngineApp::Shutdown()
{
    guard(EngineApp::Shutdown);
	GLog.Print("Shutting down subsystems...\n");
    Game->Close();
    delete Game;
    CloseRenderDevice();
    delete RenderWnd;
	delete Net;
	delete Audio; Audio = 0;
    delete Physics;
    delete MaterialSys;
	MaterialSys = 0;
    Net = NULL;
    Physics = NULL;
    RenderWnd = NULL;
    Game = NULL;
    GLog.Print("Engine shutdown: %s\n",SysDriver.GetTimeAsString());
    GLog.Close();
    unguard;
}

void EngineApp::RunLoop()
{
    guard(EngineApp::RunLoop);

    MSG msg;
    msg.message=NULL;
    PeekMessage(&msg,NULL,0U,0U,PM_NOREMOVE);

    while (WM_QUIT!=msg.message && !ExitRequested) 
	{
		GProfiler.SetState(MESSAGE_PUMP);
     
        if (PeekMessage(&msg,NULL,0U,0U,PM_REMOVE)) 
		{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        TickEngine(false);
    }


    unguard;
}

void EngineApp::TickEngine(bool minimized)
{        

    guard(EngineApp::Tick);
	static bool wasMinimized = true;

	float timeDelta;
	if (minimized != wasMinimized)
	{
		ResetTimer();

		if (minimized)
		{
			wasMinimized = minimized;
			return;
		}
		else
			timeDelta = 1.0f/30.0f;

	}
	else
		timeDelta = float(SysDriver.GetTimeStep()*TimeMultiplier);	

	if (timeDelta <= 0.0f) return;

	if (timeDelta > 1.0f/30.0f) timeDelta = 1.0f/30.0f; // max delta - auto protection for slowdown

	GProfiler.SetState(NETWORK_ACTIVITY);
	Net->Tick(timeDelta);
	((EngineGame*)Game)->UpdateNetClients();
	GProfiler.SetState(OTHER);
	if (!minimized)
		Game->Tick(timeDelta);
	GProfiler.SetState(OTHER);

	wasMinimized = minimized;
    unguard;
}

BOOL EngineApp::SetRes(int renderDeviceType, int xRes, int yRes, BOOL fullscreen)
{
    BOOL success=FALSE;
   
    if (RenderDev) // device already opened
	{ 
        // if it's the same device that requested
        if (RenderDevType==renderDeviceType)
        {
            if (RenderDev->SetMode(xRes,yRes,fullscreen))
				success=TRUE;
        }
        else // they want different device, free this one       
            CloseRenderDevice();
    }

    if (NULL == RenderDev)
	{
        RenderDev = RenderDeviceManager::CreateRenderDevice(renderDeviceType);
        if (RenderDev) 
		{
            if (RenderDev->Open(RenderWnd->GetHandle())) 
			{
                GLog.Print("Engine: Created rendering device: %d\n",RenderDevType);
			
			    if (!RenderDev->SetMode(xRes,yRes,fullscreen)) 
				{
                    GLog.Print("Engine: Warning - Could not init RenderDevice with: %d,%d %s, trying safe mode\n",
                                xRes,yRes,(fullscreen ? "FullScreen" : "Windowed"));

                    if (!RenderDev->SetMode(640,480,TRUE)) 
					{
                        GLog.Print("Engine: Fatal - Safe mode didn't quite work, bailing out\n");
                        return FALSE;
                    }
                }

                // contrast,gamma,brightness values
                float contrast  =0.5f;
                float gamma     =1.0f;
                float brightness=0.5f;

			    const char* s;
                const char* section="Video";
			    s=GConfig.GetString(section,"Contrast"  ); if(s) contrast  =float(atof(s));
			    s=GConfig.GetString(section,"Gamma"     ); if(s) gamma     =float(atof(s));
			    s=GConfig.GetString(section,"Brightness"); if(s) brightness=float(atof(s));

    			RenderDev->SetGamma(contrast,gamma,brightness);

                // all good
                success=TRUE;
            }
        }
    }

    if (success)
    {
        IsFullScreen=fullscreen;
        RenderDevType=renderDeviceType;
    }

	return success;
}

BOOL EngineApp::Init(int renderDeviceType, int xRes, int yRes, DWORD initOpts)
{
    if (!xRes || !yRes)
    {
        // get screen configuration
	    const char* section="Video";			
	    const char* xResValue=GConfig.GetString(section,"XRes");
	    const char* yResValue=GConfig.GetString(section,"YRes");
	    if (xResValue && yResValue) 
        {
		    xRes=atoi(xResValue);
		    yRes=atoi(yResValue);
	    }
	    else 
        {
		    // kinda safe defaults
		    xRes=640;
		    yRes=480;
	    }
    }

    // TODO: check xRes,yRes if sane

	BOOL fullscreen=FALSE;

    if (initOpts == EInitFullScreen)
        fullscreen = TRUE;
    else
    if (initOpts == EInitWindowed)
        fullscreen = FALSE;
    else
    {   // initOpts == EInitUseConfig
        const char* section = "Video";
	    const char* fullscrnValue=GConfig.GetString(section,"FullScreen");
	    if (fullscrnValue)
	        if (!stricmp(fullscrnValue,"true"))
		        fullscreen=TRUE;
    }

    if (!RenderWnd) 
	{
        // create rendering window
        m_setTag("Engine");
        RenderWnd = new RenderWindow();
        Assert(RenderWnd);
        Window::Resources res;
        RenderWnd->Initialize(hInstance,res,"PKRender3D");
        TRACE("EngineApp::Init(): creating 320x200 window\n");
        RenderWnd->Create("PainKiller",0,0,320,200,Window::EStylePopup);

        if(FALSE==fullscreen)
            RenderWnd->AdjustSize(xRes,yRes);
    }

    if (SetRes(renderDeviceType,xRes,yRes,fullscreen))
	{
        if (!MaterialSys->Init())
        {
			GLog.Print("ERROR: Could not initialize MaterialSystem\n");
            return FALSE;
        }

		if (!Hud.Init()) {
			GLog.Print("ERROR: Could not initialize HUD\n");
			return FALSE;
		}

#ifdef TRANSGAMING_USE_EDITOR
		if (!LineBuf.Init())
		{
			GLog.Print("ERROR: Could not initialize LineBuf\n");
			return FALSE;
		}
#endif

		if (!ParticleSys.Init())
		{
			GLog.Print("ERROR: Could not initialize ParticleSystem\n");
			return FALSE;
		}

		if (!DecalSys.Init())
		{
			GLog.Print("ERROR: could not initialize DecalSystem\n");
			return FALSE;
		}
		
		if( !Audio->Init())
		{
			GLog.Print("ERROR: Could not initialize AudioDevice\n");
            return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

void EngineApp::CloseRenderDevice()
{
    guard(EngineApp::CloseRenderDevice);

    if (RenderDev) 
	{
        RenderDev->Close();
        RenderDeviceManager::DestroyRenderDevice(RenderDev);
        RenderDevType=RenderDeviceManager::ENullRenderDevice;
        RenderDev=NULL;
    }

    unguard;
}

EngineGame *EngineApp::GetGame()
{
	if (!Game) return NULL;
	return (EngineGame*)Game;
}

Client *EngineApp::GetClient()
{
	if (!GetGame()) return NULL;
	return GetGame()->mClient;
}

World *EngineApp::GetWorld()
{
	if (!GetGame()) return NULL;
	return &GetGame()->mWorld;
}
