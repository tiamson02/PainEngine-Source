#include "Engine.h"


#include "Core/SimpleProfiler.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Engine/InputSystem.h"
//#include "Engine/Win32InputSystem.h"

#if defined( TRANSGAMING_PSP )
# include "Game/PainGame_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "Game/PainGame_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
//# include "../PainEditor/Game/EditorGame.h"
# include "Game/PainGame.h"
# include "Engine/DIInputSystem.h"
#else
# include "Engine/DIInputSystem.h"
#endif

#if defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_PS2 )
#include "Engine/PSPInputSystem.h"
#endif
#include "Physics/PhysicsEngine.h"
#include "Net/NetworkDevice2.h"

//#include "Audio/AudioDevice.h"
#ifndef TRANSGAMING_NO_MILES
// Blair - Keeps this off until we have a miles-psp solution
#include "Audio/MilesEngine.h"
#endif

#include "Engine/BaseGame.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Engine/World.h"
#include "FileSystem/GFChunk.h"

#ifdef TRANSGAMING
# include <malloc.h>
#endif

#if defined(DEBUG) || defined(TRANSGAMING_ENABLE_CONSOLE)
# include <Render/HUD.h>
#endif

#if !(defined (TRANSGAMING_PS2) || defined (TRANSGAMING_PSP))
#include <dshow.h>
#include <wmsdk.h>
#endif

#ifndef	TRANSGAMING_NO_GAMESPY
#include "Net/GameSpy2/ghttp/ghttp.h"
#endif

#include "Engine/PainMenu.h"
#include "Net/DemoRecording2.h"

#include "Game/memoryDump.h"


#ifdef PKTV_ENABLED
# include "Net/PKTVManager.h"
#endif

#ifdef TRANSGAMING_PSP
#include <libgu.h>
#endif

extern ENGINE_API EngineGame* (*OurGame)(void);
extern ENGINE_API View* (*OurView)(void);

ENGINE_API	SimpleProfiler		GProfiler;

extern bool GameSpyBackendTest( void );

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

# ifdef TRANSGAMING_SHOW_FPS
float    g_gameStartTime = 0;
bool     g_gameStartTimeSet = false;
#endif

# if defined( TRANSGAMING_PSP ) && defined( TRANSGAMING_SHOW_FPS )
void PCF_RenderStats(float timeStep, float avgFPS,float currentTime)
{
    static float    moduleSize = m_getModuleSize();
    const int       MSGSIZE = 64;
    char            msg[MSGSIZE];
    float           memUsed = (mallinfo().uordblks + moduleSize)/ 1024.0f;
    float           luaMem = GScript.GetMemUsage() / 1024.0f;

    snprintf(msg, MSGSIZE, "IFPS: %06.3f", 1.0f / timeStep);
    sceGuDebugPrint(0, 0, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "AFPS: %06.3f", avgFPS);
    sceGuDebugPrint(0, 10, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "TFPS: %06.3f", totalFrameCount / (currentTime - g_gameStartTime));
    sceGuDebugPrint(0, 20, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "Objs: %d", g_objectCount);
    sceGuDebugPrint(0, 30, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "Tris: %d", g_triCount);
    sceGuDebugPrint(0, 40, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "Vtcs: %d", g_vertexCount);
    sceGuDebugPrint(0, 50, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "Mem:  %.3fKB", memUsed);
    sceGuDebugPrint(0, 60, 0xffffffff, msg);
    snprintf(msg, MSGSIZE, "LuaM: %.3fKB", luaMem);
    sceGuDebugPrint(0, 70, 0xffffffff, msg);
    sceGuDebugFlush();
    g_triCount = 0;
    g_objectCount = 0;
    g_vertexCount = 0;
}
# endif

PCFSystem::PCFSystem()
:   hInstance(0)
,	TimeMultiplier(1)

,	InputSys(0)
,	Game(0)
,	Physics(0)
,	Audio(0)
,	Net(0)
#ifdef PKTV_ENABLED
,	PKTV(0)
#endif
#ifdef	TRANSGAMING
,   pcfWorld(0)
#else
,   World(0)
#endif
,	Video(0)
#ifdef	TRANSGAMING
,	GameState(PCFSystem::ShutDown)
#else
,	GameState(PCFSystemStates::ShutDown)
#endif
,	ExitRequested(0)

,	StartMultiplayerClient( false )
,	StartMultiplayerServer( false )
,	StartPlayingRecording( false )
,	ServerToConnectTo("")
,	PlayerNameToAssume("")
,	PasswordToGive("")
,	SpeedToConnectAt(255)
,	MapToPlay("")
,	PortToPlayAt(DEFAULT_SERVER_PORT)
#ifndef TRANSGAMING
,	ServerIsPrivate(false)
,	InterfaceToUse("")
,	EnforcedHWClass(0)
#endif
,	MinSecondsPerFrame(0)
,   LastDelta(0)


,	UsePhysicsSubsteps(false)
,	MyPublicIPAddress(0)
,	GameSpyOK(false)
,	LastServerUpdateMethod(NONE)
{
    TRACE("PCFSystem::PCFSystem(): creating a new engine object\n");
#ifdef TRANSGAMING
    StartupCommand = "Game:TNGDemoLoadLevel()";
#endif
//	UdpMessage mesg;
//	mesg.Test();
}

PCFSystem::~PCFSystem()
{
#ifndef TRANSGAMING
	if( gDemoRec )
	{
		delete gDemoRec;
		gDemoRec = NULL;
	}
#endif

#ifdef	TRANSGAMING
	SwitchToState( PCFSystem::ShutDown );
#else
	SwitchToState( PCFSystemStates::ShutDown );
#endif
	// Uninitialize the COM library - for DirectShow
	CoUninitialize();

	GLog.Print( "Engine shutdown: %s\n", SysDriver.GetTimeAsString() );
	GLog.Close();
#ifndef	TRANSGAMING_NO_GAMESPY
	ghttpCleanup();
#endif	
}

#ifdef TRANSGAMING
BOOL PCFSystem::Initialize(int argc, char **argv)
#else
BOOL PCFSystem::Initialize(HINSTANCE hinst, const char* cmdLine)
#endif
{

#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(/*_CRTDBG_ALLOC_MEM_DF /*| _CRTDBG_LEAK_CHECK_DF |*/ _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetBreakAlloc(8415);
	//_crtBreakAlloc = 8415;
#endif
    
#ifdef TRANSGAMING
	hInstance = 0x00040000;
#else
	hInstance = hinst;
#endif

#ifndef CONSOLE_ONLY_SERVER
	Assert(hInstance);
#endif


	// Start log file
#ifdef TRANSGAMING
	GLog.Open( "pk.log" );
#else
	GLog.Open( "pk.log" );
#endif

    m_setTag("Pack");
    PackManager = new CPackageManager();


#ifdef TRANSGAMING
	ParseCommandLine( argc, argv );			// Parse command line
#else
	ParseCommandLine( cmdLine );			// Parse command line
#endif

	SysDriver.Initialize( hInstance );		// Init system driver
#if defined( _DEBUG ) && !defined( TRANSGAMING )
	SysDriver.Test();
#endif

#ifndef	TRANSGAMING_NO_GAMESPY
	GameSpyOK = GameSpyBackendTest();
	ghttpStartup();
#endif

	// System check
	GLog.Print(" PK Log started: %s\n", SysDriver.GetTimeAsString() );
	SysDriver.Dump( GLog );

	ZoneAlarmNetworkAccessPopUp();

#ifndef TRANSGAMING
	Registry.ExePathname = SysDriver.GetExePath();
#endif

#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
	if( StartMultiplayerServer )
	{
		// Don't initialize COM library

		// Start multiplayer server game mode
		SwitchToState( PCFSystem::DedicatedServer );
		Net->SetCDKeyAndIsPublic( Registry.CDKey, !ServerIsPrivate );
		if( !Net->NetworkServer( MapToPlay, PasswordToGive, InterfaceToUse, PortToPlayAt ) )
		{
			GLog.Print( "Server initialization failed!" );
		#ifdef	TRANSGAMING
			SwitchToState( PCFSystem::ShutDown );
		#else		
			SwitchToState( PCFSystemStates::ShutDown );
		#endif		
		}
		else
		{
#ifdef TRANSGAMING
			Net->LoadNewMap(MapToPlay.AsChar());
#else
            Net->LoadNewMap(MapToPlay);
#endif
		}
	}
	else
#endif
    if( StartMultiplayerClient )
	{
		// Initialize the COM library - for DirectShow
//		if( FAILED( CoInitialize( NULL ) ) )
		if( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED ) ) )
		{
			GLog.Print( "Initialization of COM library failed.\n" );
			return FALSE;
		}

		// Start multiplayer client game mode
	#ifdef	TRANSGAMING	
		SwitchToState( PCFSystem::MultiplayerClient );
	#else
		SwitchToState( PCFSystemStates::MultiplayerClient );
	#endif	
		Net->SetCDKeyAndIsPublic( Registry.CDKey, Registry.IsPublic );
		if( ServerToConnectTo.IsNull() ) ServerToConnectTo = Registry.LastHostName;
		if( PlayerNameToAssume.IsNull() )
			PlayerNameToAssume = GScript.Globals()["Cfg"]["PlayerName"].IsNil()?Registry.PlayerName:GScript.Globals()["Cfg"]["PlayerName"].GetString();
		else
			GScript.DoString( "Cfg.PlayerName = '%s'", PlayerNameToAssume.AsChar() );
		if( SpeedToConnectAt != 255 )
			GScript.Call( "PainMenu_ConnectionSpeedPreset", 0, "%d", SpeedToConnectAt+1 );
		if( !Net->NetworkClientAndConnect( ServerToConnectTo.AsChar(), PasswordToGive, 0 ) )
		{
			GLog.Print( "Client initialization failed!" );
		#ifdef	TRANSGAMING	
			SwitchToState( PCFSystem::Default );
		#else
			SwitchToState( PCFSystemStates::Default );
		#endif	
		}
	}
	else if( StartPlayingRecording )
	{
/*
		// Initialize the COM library - for DirectShow
		if( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED ) ) )
		{
			GLog.Print( "Initialization of COM library failed.\n" );
			return FALSE;
		}

		// Start multiplayer client game mode
		SwitchToState( PCFSystemStates::MultiplayerClient );
		String errorText;
		if( !Net->PlayRecording( RecordingToPlay.AsChar(), &errorText ) )
		{
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
			SwitchToState( PCFSystemStates::Default );
		}
*/
		// TO DO - Michal
	}
	else // default - start single game
	{
#ifndef TRANSGAMING
		// Initialize the COM library - for DirectShow
//		if( FAILED( CoInitialize( NULL ) ) )
		if( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED ) ) )
		{
			GLog.Print( "Initialization of COM library failed.\n" );
			return FALSE;
		}
#endif

		// Start default game mode
	#ifdef	TRANSGAMING	
		SwitchToState( PCFSystem::Default );
	#else
		SwitchToState( PCFSystemStates::Default );
	#endif	
	}

	return TRUE;
}

static const int    MIN_FRAME_RATE = 15;


void goo(){
    static int x = 4;

    x++;
}

void PCFSystem::RunLoop()
{
#ifdef TRANSGAMING
     int frameCount = 0;


//define this to use the full game loop.  Undefine it to use the hacked game loop
    while (!ExitRequested){
		GProfiler.SetState(MESSAGE_PUMP);
     
        if (DummyMessageHandler::IsMessageWaiting())
            DummyMessageHandler::DispatchHorribleWindowsMessage();


        //check if a state request has been queued
        switch (Memory::safe.checkForStateRequest()){
            case Memory::SafeMemory::STATE_FAILED:
                TRACE("PCFSystem::RunLoop(): ERROR-> could not perform the queued state operation!\n");
                break;

            case Memory::SafeMemory::STATE_SUCCESS:
                //the state load likely took a while to execute, so fudge the last recorded frame time
                //by a little bit so we don't have an enormously long frame step next.
                SysDriver.FudgeCurrentTime(-0.01f);
                break;
        }



     
        /*if (frameCount % 5 == 1 && frameCount < 10){
            char buf[64];

            snprintf(buf, 64, "memory report (pre-run %d).txt", frameCount);
            m_dumpMemoryReport(buf, true, false);
        }//*/
        
        TickEngine(false);

        goo();
    }
#else
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
#endif
}

int TickEngineList = -1;

#ifdef TRANSGAMING_SHOW_FPS
int g_triCount = 0;
int g_objectCount = 0;
int g_vertexCount = 0;
int g_AnimatedMeshes = 0;
int g_AnimatedVerts[4];
#endif

#ifdef TRANSGAMING
int totalFrameCount = 0;
#endif

void PCFSystem::TickEngine(bool minimized)
{
	static bool     wasMinimized = true;
	static bool     wasStopped = true;
#ifdef TRANSGAMING
	static float    timeSum = 0;
#else
	static int      frameCount = 0;
	static double   timeSum = 0;
#endif

	#ifdef ENGINE_LOG
	GLog.Print("^* T1\n");
	#endif

	bool stopped = ( ( GameState == SingleGame ) && minimized );
	bool stopNow = ( stopped && !wasStopped );
	bool startNow = ( !stopped && wasStopped );



#ifdef TRANSGAMING
    totalFrameCount++;

	float           timeStep = SysDriver.GetTimeStep();
    static float    lastFPSUpdate = 0;
    static int      lastFrameCount = 0;
    float           currentTime = SysDriver.GetCurrentTime();


# ifdef TRANSGAMING_SHOW_FPS
    //initialize the static vars
    if (!g_gameStartTimeSet){
        g_gameStartTime = currentTime - 1;
        lastFPSUpdate = currentTime - 1;
        g_gameStartTimeSet = true;
    }


    //TRACE("PCFSystem::TickEngine(): currentTime = %g, lastFPSUpdate = %g, timeStep = %g, g_gameStartTime = %g, totalFrameCount = %\n", currentTime, lastFPSUpdate, timeStep, g_gameStartTime, totalFrameCount);
# endif

    //it's been more than 1 second since the FPS counter was last updated
    if (currentTime - lastFPSUpdate > 1.0f){
        avgFPS = (totalFrameCount - lastFrameCount) / (currentTime - lastFPSUpdate);
        lastFPSUpdate = currentTime;
        lastFrameCount = totalFrameCount;
    }

# if defined(DEBUG) && !defined(TRANSGAMING_SILENT_DEBUG)
    static float    moduleSize = m_getModuleSize();
    float           luaMem = GScript.GetMemUsage() / 1024.0f;
    float           memUsed = (m_getMemUsed() + moduleSize)/ 1024.0f;

    //log the frame
    TRACE("******* GAME LOOP frame %-4d (dt = %.3f, mem = %.3fKB, luaMem = %.3fKB) *******\n", totalFrameCount, timeStep, memUsed, luaMem);

    if (m_getLoggingState())
        m_logMsg("******* GAME LOOP frame %-4d (dt = %.3f, mem = %.3fKB, luaMem = %.3fKB) *******\r\n", totalFrameCount, timeStep, memUsed, luaMem);
# endif

# if defined(DEBUG) || defined(TRANSGAMING_ENABLE_CONSOLE)
    GEngine->Video->Hud->mConsole.psuedoConsoleRead(timeStep);
# endif
#else
    frameCount++;

    double timeStep = SysDriver.GetTimeStep();
#endif


	if( !stopped )
		timeSum += timeStep;

	// Check if we need to sleep
	if( timeSum < MinSecondsPerFrame )
	{
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
	#ifdef	TRANSGAMING	
		if (GameState == PCFSystem::DedicatedServer)
	#else
		if (GameState == PCFSystemStates::DedicatedServer)
	#endif	
			SysDriver.ExactSleep(1);
#endif
		return;	// to start next iteration of TickEngine()
	}

	// For mouse movement?
#ifndef TRANSGAMING
	if( minimized != wasMinimized )
	{
		ResetTimer();
		if( GEngine->InputSys )
			GEngine->InputSys->Reset();
	}
#endif

	#ifdef ENGINE_LOG
	GLog.Print("^* T2\n");
	#endif

#ifndef TRANSGAMING
	if( GEngine->InputSys )
	{
		if( startNow && GEngine->InputSys->IsMouseLocked() )
		{
			GEngine->InputSys->SetUseDInput( true );
		}
	}
#endif

	#ifdef ENGINE_LOG
	GLog.Print("^* T3\n");
	#endif
	// Check if we need to stop/start sounds

#ifndef TRANSGAMING_NO_MILES
	if( GEngine->Audio )
	{
		if( stopNow )
		{
			Assert( TickEngineList == -1 );
			TickEngineList = GEngine->Audio->PauseCurrentlyPlayingSounds();
		}
		else if( startNow && ( TickEngineList != -1 ) )
		{
			GEngine->Audio->ResumeSounds( TickEngineList );
			TickEngineList = -1;
		}
	}
#endif

	#ifdef ENGINE_LOG
	GLog.Print("^* T4\n");
	#endif


#ifndef	TRANSGAMING	
	// Calculate FPS
	static DWORD lastGetCurrentTime = SysDriver.GetCurrentTimeMS();
	DWORD curTime = SysDriver.GetCurrentTimeMS();
	if( curTime-lastGetCurrentTime >= 1000 ) // a second passed
	{
		FPS = frameCount;	// no multiplies/divides anymore here
		frameCount = 0;
		lastGetCurrentTime = curTime;
	}

    
    static lastMS = 0;
#endif	

//	MinSecondsPerFrame = 0.0;

	// Main loop, paused if stopped
	if( !stopped && ( timeSum > MinSecondsPerFrame ) )
	{
#ifndef TRANSGAMING
		if( gDemoRec )
			gDemoRec->Update();
#endif

		// Net pretick
		if( Net ) Net->PreTick(timeSum);
		GProfiler.SetState(OTHER);

#ifdef TRANSGAMING
		float timeDelta;

		if( MinSecondsPerFrame > 0.0f )
#else
		double timeDelta;

		if( MinSecondsPerFrame > 0.0 )
#endif
        {
			timeDelta = floor(timeSum/MinSecondsPerFrame)*MinSecondsPerFrame*TimeMultiplier;
            timeSum = fmod(timeSum, MinSecondsPerFrame);
        }
		else
        {
			timeDelta = timeSum*TimeMultiplier;
    		timeSum = 0;
        }

//	Time measurement
//		static lastMS = 0;
//		DWORD curTime2 = GEngine->SysDriver.GetCurrentTimeMS();
//		GLog.Print( "time diff: %d, timeDelta: %f\n", curTime2-lastMS, timeDelta );
//		lastMS = curTime2;

		timeDelta = min(timeDelta,1.0f);
#ifdef TRANSGAMING
        LastDelta = timeDelta;
#else
        LastDelta = (float)timeDelta;
#endif
		if( timeDelta > 0.0f )
		{
#ifdef	TRANSGAMING	
			if( !UsePhysicsSubsteps && ( timeDelta > 1.0f/MIN_FRAME_RATE ) && ( GameState == PCFSystem::SingleGame ) )
            {
				timeDelta = 1.0f/MIN_FRAME_RATE; // max delta - auto protection for slowdown
				LastDelta = timeDelta;
				//GLog.Print("delta auto protection: %f\n",timeDelta);
            }
#else
			if( !UsePhysicsSubsteps && ( timeDelta > 1.0/29.0 ) && ( GameState == PCFSystemStates::SingleGame ) )
            {
				timeDelta = 1.0/29.0; // max delta - auto protection for slowdown
				LastDelta = (float)timeDelta;
				//GLog.Print("delta auto protection: %f\n",timeDelta);
            }
#endif	

#ifdef ENGINE_LOG
			GLog.Print("^* T5\n");
#endif
            Game->Tick(minimized, (float)timeDelta );
#ifdef ENGINE_LOG
			GLog.Print("^* T6\n");
#endif //ENGINE_LOG
		}

#ifndef TRANSGAMING_NO_MILES
		if( Audio )
		{
	        GProfiler.SetState(AUDIO_TICK_STOP_FINISHED);
			Audio->StopFinishedSounds();
		}
#endif

		#ifdef ENGINE_LOG
		GLog.Print("^* T7\n");
		#endif

        // Net posttick
		if( Net ) Net->PostTick();

		#ifdef ENGINE_LOG
		GLog.Print("^* T8\n");
		#endif

	#ifdef	TRANSGAMING	
        if (pcfWorld) // after network posttick
            pcfWorld->DeleteDelayedEntities();
	#else
        if (World) // after network posttick
            World->DeleteDelayedEntities();
	#endif	

		GProfiler.SetState(OTHER);

        // profiler
        GProfiler.AdvanceFrame();
        if( GProfiler.GiveTimeSinceViewUpdate() >= 1.0f )
            GProfiler.ViewUpdate();
    }

    //idle
    else if (stopped)
        SysDriver.ExactSleep(1);


	GProfiler.SetState(OTHER);    
	//GScript.Call("Game_GC",0,""); // after all (chwilowo OTHER)
    ScriptGameGC();


#ifdef TRANSGAMING_SHOW_FPS
    //print the FPS message
# if !defined(DEBUG) || defined(TRANSGAMING_SILENT_DEBUG)
    static float    moduleSize = m_getModuleSize();
    float           luaMem = GScript.GetMemUsage() / 1024.0f;
    float           memUsed = (m_getMemUsed() + moduleSize) / 1024.0f;
# endif
# ifdef TRANSGAMING_PSP
	PCF_RenderStats(timeStep, avgFPS,currentTime);
# endif
#endif

	wasStopped = stopped;
	wasMinimized = minimized;

	#ifdef ENGINE_LOG
	GLog.Print("^* T9\n");
	#endif
#ifndef	TRANSGAMING_NO_GAMESPY
	ghttpThink();
#endif	

#ifdef TRANSGAMING_PSP
	//sceGuStart( SCEGU_IMMEDIATE, disp_list, sizeof(disp_list) );
	//sceGuClear( SCEGU_CLEAR_ALL );
	//sceGuFinish();
	//sceGuSwapBuffers();
#endif	
}

void PCFSystem::ScriptGameGC(){
    //TRACE("PCFSystem::ScriptGameGC(): lua is currently using %dKB of memory\n", GScript.GetGCCount());
	//GScript.Call("Game_GC",0,""); // after all (chwilowo OTHER)
}


#ifdef TRANSGAMING
void PCFSystem::ParseCommandLine( int argc, char **args ){
	for ( int i = 1; i < argc; ++i ){
        TRACE("PCFSystem::ParseCommandLine(): checking argument %d: '%s' (argc = %d)\n", i, args[i], argc);
		if( ( !strcasecmp(args[i], "+connect") ) && ( i < argc - 1 ) )
		{
			ServerToConnectTo = args[i + 1];
			StartMultiplayerClient = true;
			GScript.DoString( "PainMenu.host = \"%s\"", ServerToConnectTo.AsChar() );
			i++;
			continue;
		}

		/*if( ( !strcasecmp(args[i], "+playrecording") ) && ( i < argc - 1 ) )
		{
			RecordingToPlay = args[i+1];
			StartPlayingRecording = true;
			i++;
			continue;
		}//*/

#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
		if( !strcasecmp(args[i], "+dedicatedserver") )
		{
			StartMultiplayerServer = true;
			continue;
		}
#endif

		/*if( !strcasecmp(args[i], "+private") )
		{
			ServerIsPrivate = true;
			continue;
		}//*/

		if( ( !strcasecmp(args[i], "+name") ) && ( i < argc-1 ) )
		{
			String temp = args[i + 1];
			if( temp.Len() > 2 )
			{
				char buf[256];
				// Strip surroundings
				if( ( ( temp[0] == '"' ) && ( temp[temp.Len()-1] == '"' ) ) ||
					( ( temp[0] == '\'' ) && ( temp[temp.Len()-1] == '\'' ) ) )
				{
					::strcpy( buf, temp.AsChar()+1 );
					buf[::strlen(buf)-1] = 0;
					PlayerNameToAssume = buf;
				}
				else
					PlayerNameToAssume = temp;

				GScript.DoString( "PainMenu.playerName = \"%s\"", PlayerNameToAssume.AsChar() );
			}
			i++;
			continue;
		}

		if( ( !strcasecmp(args[i], "+map") ) && ( i < argc-1 ) )
		{
			MapToPlay = args[i + 1];
			i++;
			continue;
		}

        //set the level that will be loaded on startup instead of the menu
        if (!strcasecmp(args[i], "+level") && i < argc - 1){
            char    buf[_MAX_PATH];
            int     chapter;
            int     level;
            int     levelette;
            char *  tok = args[i + 1];


            //remove leading white space
            while (tok[0] != 0 && (tok[0] == ' ' || tok[0] == '\t'))
                tok++;

            chapter =   tok[0] - '0';
            level =     tok[1] - '0';
            levelette = tok[2] - '0';

            if (chapter < 1) chapter = 1;
            if (chapter > 5) chapter = 6;
            if (level < 1)   level = 1;
            if (level > 6)   level = 6;
            if (levelette < 1) levelette = 1;
            if (levelette > 9) levelette = 9;
            TRACE("PCFSystem::ParseCommandLine(): setting the current level to <%d, %d, %d>\n", chapter, level, levelette);

            
            snprintf(buf, _MAX_PATH, "Game.CurrLevelette = %d; Lev:SetMap(%d, %d, %d); EDITOR.PostMessage(1,0)", levelette, chapter, level, levelette);
            StartupCommand = buf;
            i++;
            continue;
        }

		/*if( ( !strcasecmp(args[i], "+interface") ) && ( i < argc-1 ) )
		{
			InterfaceToUse = args[i + 1];
			i++;
			continue;
		}//*/

		if( ( !strcasecmp(args[i], "+speed") ) && ( i < argc-1 ) )
		{
			SpeedToConnectAt = atoi( args[i + 1] );
			if( SpeedToConnectAt > 3 )
				SpeedToConnectAt = 255;
			GScript.DoString( "PainMenu.speed = %d", SpeedToConnectAt );
			i++;
			continue;
		}

		if( ( !strcasecmp(args[i], "+port") ) && ( i < argc-1 ) )
		{
			PortToPlayAt = (unsigned short)atoi( args[i + 1] );
			GScript.DoString( "PainMenu.port = %d", PortToPlayAt );
			i++;
			continue;
		}

		if( ( !strcasecmp(args[i], "+maxfps") ) && ( i < argc-1 ) )
		{
			int maxfps = atoi( args[i + 1] );
#ifdef TRANSGAMING
			MinSecondsPerFrame = maxfps ? (1.0f / maxfps) : 0.0f;
#else
			MinSecondsPerFrame = maxfps?((double)1.0)/maxfps:0.0;
#endif
			i++;
			continue;
		}

		if( ( !strcasecmp(args[i], "+password") ) && ( i < argc - 1 ) )
		{
			PasswordToGive = args[i + 1];
			GScript.DoString( "PainMenu.passwd = \"%s\"", PasswordToGive.AsChar() );
			i++;
			continue;
		}

        //we only support the TnL renderer
		/*if( ( !strcasecmp(args[i], "+hwclass") ) && ( i < argc-1 ) )
		{
			i++;
			const char* param = args[i];
			if (!stricmp(param,"tnl"))
				EnforcedHWClass = RenderDevice::EHWClassGenericTnL;
			else if (!stricmp(param,"nv20"))
				EnforcedHWClass = RenderDevice::EHWClassNV20;
			else if (!stricmp(param,"nv25"))
				EnforcedHWClass = RenderDevice::EHWClassNV25;
			else if (!stricmp(param,"r200"))
				EnforcedHWClass = RenderDevice::EHWClassR200;
			else if (!stricmp(param,"nv30"))
				EnforcedHWClass = RenderDevice::EHWClassNV30;
			else if (!stricmp(param,"nv40"))
				EnforcedHWClass = RenderDevice::EHWClassNV40;
			continue;
		}//*/
	}
}
#else
void PCFSystem::ParseCommandLine( const char* cmdLine )
{
	DynamicArray<String> args;
	String::TokenizeWithLiteral( cmdLine, " ", args, '"' );

	GLog.Print( "Command line: %s\n", cmdLine );

	for( int i = 0; i < args.Size(); ++i )
	{
		if( ( args[i] == "+connect" ) && ( i < args.Size() - 1 ) )
		{
			ServerToConnectTo = args[i+1];
			StartMultiplayerClient = true;
			GScript.DoString( "PainMenu.host = \"%s\"", ServerToConnectTo.AsChar() );
			i++;
			continue;
		}

		if( ( args[i] == "+playrecording" ) && ( i < args.Size() - 1 ) )
		{
			RecordingToPlay = args[i+1];
			StartPlayingRecording = true;
			i++;
			continue;
		}

#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
		if( args[i] == "+dedicatedserver" )
		{
			StartMultiplayerServer = true;
			continue;
		}
#endif

		if( args[i] == "+private" )
		{
			ServerIsPrivate = true;
			continue;
		}

		if( ( args[i] == "+name" ) && ( i < args.Size()-1 ) )
		{
			String temp = args[i+1];
			if( temp.Len() > 2 )
			{
				char buf[256];
				// Strip surroundings
				if( ( ( temp[0] == '"' ) && ( temp[temp.Len()-1] == '"' ) ) ||
					( ( temp[0] == '\'' ) && ( temp[temp.Len()-1] == '\'' ) ) )
				{
					::strcpy( buf, temp.AsChar()+1 );
					buf[::strlen(buf)-1] = 0;
					PlayerNameToAssume = buf;
				}
				else
					PlayerNameToAssume = temp;

				GScript.DoString( "PainMenu.playerName = \"%s\"", PlayerNameToAssume.AsChar() );
			}
			i++;
			continue;
		}

		if( ( args[i] == "+map" ) && ( i < args.Size()-1 ) )
		{
			MapToPlay = args[i+1];
			i++;
			continue;
		}

		if( ( args[i] == "+interface" ) && ( i < args.Size()-1 ) )
		{
			InterfaceToUse = args[i+1];
			i++;
			continue;
		}

		if( ( args[i] == "+speed" ) && ( i < args.Size()-1 ) )
		{
			SpeedToConnectAt = atoi( args[i+1].AsChar() );
			if( SpeedToConnectAt > 3 )
				SpeedToConnectAt = 255;
			GScript.DoString( "PainMenu.speed = %d", SpeedToConnectAt );
			i++;
			continue;
		}

		if( ( args[i] == "+port" ) && ( i < args.Size()-1 ) )
		{
			PortToPlayAt = (unsigned short)atoi( args[i+1].AsChar() );
			GScript.DoString( "PainMenu.port = %d", PortToPlayAt );
			i++;
			continue;
		}

		if( ( args[i] == "+maxfps" ) && ( i < args.Size()-1 ) )
		{
			int maxfps = atoi( args[i+1].AsChar() );
			MinSecondsPerFrame = maxfps?((double)1.0)/maxfps:0.0;
			i++;
			continue;
		}

		if( ( args[i] == "+password" ) && ( i < args.Size() - 1 ) )
		{
			PasswordToGive = args[i+1];
			GScript.DoString( "PainMenu.passwd = \"%s\"", PasswordToGive.AsChar() );
			i++;
			continue;
		}

		if( ( args[i] == "+hwclass" ) && ( i < args.Size()-1 ) )
		{
			i++;
			const char* param = args[i].AsChar();
			if (!stricmp(param,"tnl"))
				EnforcedHWClass = RenderDevice::EHWClassGenericTnL;
			else if (!stricmp(param,"nv20"))
				EnforcedHWClass = RenderDevice::EHWClassNV20;
			else if (!stricmp(param,"nv25"))
				EnforcedHWClass = RenderDevice::EHWClassNV25;
			else if (!stricmp(param,"r200"))
				EnforcedHWClass = RenderDevice::EHWClassR200;
			else if (!stricmp(param,"nv30"))
				EnforcedHWClass = RenderDevice::EHWClassNV30;
			else if (!stricmp(param,"nv40"))
				EnforcedHWClass = RenderDevice::EHWClassNV40;
			continue;
		}
	}
}
#endif

void PCFSystem::SwitchToState( int newState )
{
    GScript.DoString("Game:OnSwitchToState( %d )", newState );

	if( newState == GameState ) return;

	GScript.DoString("Game:BeforeChangeGameType( %d )", newState );

	MinSecondsPerFrame = 0;

	switch( newState )
	{
#ifdef	TRANSGAMING	
	case PCFSystem::ShutDown:
#else
	case PCFSystemStates::ShutDown:
#endif	
		GScript.DoString( "Cfg:Save()" );
		GLog.Print("Changing system state to: Shutdown\n");
		GLog.Print("Shutting down subsystems...\n");
		UsePhysicsSubsteps = false;

	#ifdef	TRANSGAMING	
        GameState = PCFSystem::ShutDown;
	#else
        GameState = PCFSystemStates::ShutDown;
	#endif	

		if( Game ) { Game->Close(); delete Game; Game = 0; };			// Close game
	#ifdef	TRANSGAMING	
		if( pcfWorld ) { delete pcfWorld; pcfWorld = 0; };						// Close world
	#else
		if( World ) { delete World; World = 0; };						// Close world
	#endif	
		if( InputSys ) { InputSys->Release(); delete InputSys; InputSys = 0; };				// Close input system
#ifdef PKTV_ENABLED
		if( PKTV ) { delete PKTV; PKTV = 0; };							// Close PKTV manager
#endif
		if( Net ) { delete Net; Net = 0; };								// Close network
#ifndef TRANSGAMING_NO_MILES
		if( Audio ) { delete Audio; Audio = 0; };						// Close audio driver
#endif
#ifndef TRANSGAMING_NO_HAVOK
		if( Physics ) {					// Close physics engine
            TRACE("PCFSystem::SwitchToState(): shutdown: destroying physics engine\n");
            delete Physics; 
            Physics = 0;
        };
#else
		if( Physics ) { 
            TRACE("PCFSystem::SwitchToState(): shutdown: invalidating physics engine\n");
            Physics = 0;
        }
#endif
		if( Video ) { delete Video; Video = 0; };						// Close video device
        if( PackManager ) { delete PackManager; PackManager = 0; };
        

		break;
#ifdef	TRANSGAMING	
	case PCFSystem::DemoPlay:
#else
	case PCFSystemStates::DemoPlay:
#endif	
#ifdef PKTV_ENABLED
		if( Net ) { delete Net; Net = 0; };								// Close network

		GLog.Print("Changing system state to: DemoPlay\n");
		GLog.Print("Starting subsystems...\n");

		// Init subsystems
		if (!InitVideo())
			exit(1);

		if( Physics ) GScript.DoString("Game:NewLevel('NoName','','',0.3); WORLD.Release()");	// needed before physics release
	#ifdef	TRANSGAMING	
        if( pcfWorld && pcfWorld->Physics ){
            TRACE("PCFSystem::SwitchToState(): clearing the physics object (%p)\n", pcfWorld->Physics);
            pcfWorld->Physics = 0;		// Remove physics world pointer from world
        }
	#else
        if( World && World->Physics ){
            TRACE("PCFSystem::SwitchToState(): clearing the physics object (%p)\n", pcfWorld->Physics);
            World->Physics = 0;		// Remove physics world pointer from world
        }
	#endif	
#ifndef TRANSGAMING_NO_HAVOK
		if( Physics ) { 
            TRACE("PCFSystem::SwitchToState(): destroying the physics object (%p)\n", pcfWorld->Physics);
            delete Physics; 
            Physics = 0;
        }			// Close physics engine
#else
		if( Physics ) {
            TRACE("PCFSystem::SwitchToState(): destroying the physics object (%p)\n", pcfWorld->Physics);
            Physics = 0;
        }
#endif
#ifdef TRANSGAMING_NO_MILES
			GLog.Print("ERROR: Could not initialize AudioDevice. It is not currently supported on this platform.\n");
#else
		if( !Audio )
		{
            m_setTag("AudioMiles");
			Audio = new MilesEngine;
			if( !Audio->StartedOk() )
			{
				GLog.Print("ERROR: Could not initialize AudioDevice\n");
				delete Audio;
				Audio = 0;
			}
		}

		if( Audio )
		{
			Audio->SetDefaultObstruction( true );
			Audio->SetObstructionProperties( 3.0f, 0.2f, 0.0f, 0.4f, 0.8f, 10.0f, 30.0f );
		}
#endif

        m_setTag("Input");
#if defined( TRANSGAMING )
		if( !InputSys ) InputSys = new PSPInputSystem( 0 );
#else
		if( !InputSys ) InputSys = new DIInputSystem( 0 );
#endif
//#endif

	#ifdef	TRANSGAMING	
		GameState = PCFSystem::DemoPlay;
	#else
		GameState = PCFSystemStates::DemoPlay;
	#endif	

#ifdef PKTV_ENABLED
		// PKTV Start
		if( PKTV )
			delete PKTV;
		{
			float pktvFps = GScript.Globals()["Cfg"]["PKTVFps"].IsNil()?20:GScript.Globals()["Cfg"]["PKTVFps"].GetFloat();
			DWORD pktvDelay = GScript.Globals()["Cfg"]["PKTVDelay"].IsNil()?30000:GScript.Globals()["Cfg"]["PKTVDelay"].GetInt();
			String pktvPassword = GScript.Globals()["Cfg"]["PKTVPassword"].IsNil()?"":GScript.Globals()["Cfg"]["PKTVPassword"].GetString();
			PKTV = new PKTVManager( true, false, pktvFps, pktvDelay );
			PKTV->SetPassword( pktvPassword );
		}
#endif

		// Start game
		if( !Game )
		{
			Game = (*OurGame)();
		#ifdef	TRANSGAMING	
			if( pcfWorld ) delete pcfWorld;
            m_setTag("Object");
			pcfWorld = new class World;
		#else
			if( World ) delete World;
			World = new class World;
		#endif	
			Game->Initialize();
		}

		MinSecondsPerFrame = 0;
		ResetTimer();
#endif
		break;
#ifdef	TRANSGAMING	
	case PCFSystem::SingleGame:
#else
	case PCFSystemStates::SingleGame:
#endif	
#ifndef STAND_ALONE_SERVER
		UsePhysicsSubsteps = false;

		GLog.Print("Changing system state to: SingleGame\n");

		// Init subsystems
        if( Net ) { delete Net; Net = 0; };								// Close network

		if (!InitVideo())
			exit(1);

		if( !Physics )
		{
#ifndef TRANSGAMING_NO_HAVOK
            TRACE("PCFSystem::SwitchToState(): singleGame: creating a new physics engine\n");
            m_setTag("Physics");
			Physics = new PhysicsEngine;
			Physics->Init();
            TRACE("PCFSystem::SwitchToState(): singleGame: created physics engine at %p\n", Physics);
#endif
		}
		else
			GEngine->Physics->PhysicsFPS = DEFAULT_PHYSICS_FPS;

        TRACE("PCFSystem::SwitchToState(): singleGame: physics engine is at %p\n", Physics);

#ifdef TRANSGAMING_NO_MILES
        GLog.Print("ERROR: Could not initialize AudioDevice. It is not currently supported on this platform.\n");
#else
		if( !Audio )
		{
            m_setTag("AudioMiles");
			Audio = new MilesEngine;
			if( !Audio->StartedOk() )
			{
				GLog.Print("ERROR: Could not initialize AudioDevice\n");
				delete Audio;
				Audio = 0;
			}
		}

		if( Audio )
		{
			Audio->SetDefaultObstruction( false );
			Audio->SetObstructionProperties( 4.0f, 0.2f, 0.0f, 1.0f, 1.0f, 1000.0f, 1000.0f );
		}
#endif

        m_setTag("Input");
#if defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_PS2 )
        if( !InputSys ) InputSys = new PSPInputSystem( 0 );
#else
		if( !InputSys ) InputSys = new DIInputSystem( 0 );
#endif

	#ifdef	TRANSGAMING	
        GameState = PCFSystem::SingleGame;
	#else
        GameState = PCFSystemStates::SingleGame;
	#endif	

#ifdef PKTV_ENABLED
		// PKTV Start
		if( PKTV )
			delete PKTV;

		{
			float pktvFps = GScript.Globals()["Cfg"]["PKTVFps"].IsNil()?20:GScript.Globals()["Cfg"]["PKTVFps"].GetFloat();
			DWORD pktvDelay = GScript.Globals()["Cfg"]["PKTVDelay"].IsNil()?30000:GScript.Globals()["Cfg"]["PKTVDelay"].GetInt();
			String pktvPassword = GScript.Globals()["Cfg"]["PKTVPassword"].IsNil()?"":GScript.Globals()["Cfg"]["PKTVPassword"].GetString();
			PKTV = new PKTVManager( true, false, pktvFps, pktvDelay );
			PKTV->SetPassword( pktvPassword );
		}

#endif

        // Start game
        if( !Game )
        {
    		Game = (*OurGame)();

#ifdef	TRANSGAMING	
            m_setTag("Object");
		    pcfWorld = new class World;
            TRACE("*** PCFSystem::SwitchToState(): initializing game engine for single game\n");
#else
		    World = new class World;
#endif	
		    Game->Initialize();
#ifdef TRANSGAMING
            TRACE("*** PCFSystem::SwitchToState(): done game engine initialization for single game\n");
#endif
        }

		ResetTimer();
#endif
		break;

#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
#ifdef	TRANSGAMING	
	case PCFSystem::DedicatedServer:
#else
	case PCFSystemStates::DedicatedServer:
#endif	
		GLog.Print("Changing system state to: DedicatedServer\n");
		GLog.Print("Starting subsystems...\n");
		UsePhysicsSubsteps = true;

		// Init subsystems

		if( Video ) { delete Video; Video = 0; };						// Close video device
#ifndef TRANSGAMING_NO_MILES
		if( Audio ) { delete Audio; Audio = 0; };						// Close audio driver
#endif

		if( !Physics )
		{
#ifndef TRANSGAMING_NO_HAVOK
			Physics = new PhysicsEngine;
			Physics->Init();
            TRACE("PCFSystem::SwitchToState(): dedicatedServer: created a physics engine at %p\n", Physics);
#endif
		}
		else
			GEngine->Physics->PhysicsFPS = DEFAULT_PHYSICS_FPS;

        if( !Net ){
#ifdef TRANSGAMING_PSP
            m_setTag("Net");
#endif
			Net = new NetworkDevice2;
        }

#ifdef PKTV_ENABLED
		// PKTV Start
		if( PKTV )
			delete PKTV;

		{
			bool startPktvSocket = GScript.Globals()["Cfg"]["PKTV"].IsNil()?false:GScript.Globals()["Cfg"]["PKTV"].GetBool();
			float pktvFps = GScript.Globals()["Cfg"]["PKTVFps"].IsNil()?20:GScript.Globals()["Cfg"]["PKTVFps"].GetFloat();
			DWORD pktvDelay = GScript.Globals()["Cfg"]["PKTVDelay"].IsNil()?30000:GScript.Globals()["Cfg"]["PKTVDelay"].GetInt();
			String pktvPassword = GScript.Globals()["Cfg"]["PKTVPassword"].IsNil()?"":GScript.Globals()["Cfg"]["PKTVPassword"].GetString();
			PKTV = new PKTVManager( true, startPktvSocket, pktvFps, pktvDelay );
			PKTV->SetPassword( pktvPassword );
			if( startPktvSocket )
				GLog.Print( "PKTV Listening Socket established\n" );
		}
#endif

		if( InputSys ) { InputSys->Release(); delete InputSys; InputSys = 0; };				// Close input system
        //InputSys = new Win32InputSystem( 0 ); // only WIN32

	#ifdef	TRANSGAMING	
        GameState = PCFSystem::DedicatedServer; // juz przy inicie gry musze wiedziec, ze to dedicated
	#else
        GameState = PCFSystemStates::DedicatedServer; // juz przy inicie gry musze wiedziec, ze to dedicated
	#endif	

		// Start game
		if( !Game )
		{
			Game = (*OurGame)();
#ifdef	TRANSGAMING	
			if( pcfWorld ) delete pcfWorld;

            m_setTag("Game");
			pcfWorld = new class World;
            TRACE("*** PCFSystem::SwitchToState(): initializing game engine for dedicated server\n");
#else
			if( World ) delete World;
			World = new class World;
#endif	
			Game->Initialize();
#ifdef TRANSGAMING
            TRACE("*** PCFSystem::SwitchToState(): done game engine initialization for dedicated server\n");
#endif
		}
		ResetTimer();
		break;
#endif

#ifdef	TRANSGAMING	
	case PCFSystem::MultiplayerServer:
#else
	case PCFSystemStates::MultiplayerServer:
#endif	
#ifndef STAND_ALONE_SERVER
		GLog.Print("Changing system state to: MultiplayerServer\n");
		GLog.Print("Starting subsystems...\n");
		UsePhysicsSubsteps = false;
		// Init subsystems

		if (!InitVideo())
			exit(1);

		if( !Physics )
		{
#ifndef TRANSGAMING_NO_HAVOK
            TRACE("PCFSystem::SwitchToState(): multiplayerServer: creating a new physics engine\n");
            m_setTag("Physics");
			Physics = new PhysicsEngine;
			Physics->Init();
            TRACE("PCFSystem::SwitchToState(): multiplayerServer: created physics engine at %p\n", Physics);
#endif
		}
		else
			GEngine->Physics->PhysicsFPS = DEFAULT_PHYSICS_FPS;

#ifdef TRANSGAMING_NO_MILES
			GLog.Print("ERROR: Could not initialize AudioDevice. It is not currently supported on this platform.\n");
#else
		if( !Audio )
		{
            m_setTag("AudioMiles");
			Audio = new MilesEngine;
			if( !Audio->StartedOk() )
			{
				GLog.Print("ERROR: Could not initialize AudioDevice\n");
				delete Audio;
				Audio = 0;
			}
		}

		if( Audio )
		{
			Audio->SetDefaultObstruction( true );
			Audio->SetObstructionProperties( 3.0f, 0.2f, 0.0f, 0.4f, 0.8f, 10.0f, 30.0f );
		}
#endif

        m_setTag("Input");
#if defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_PS2 )
        if( !InputSys ) InputSys = new PSPInputSystem( 0 );
#else
		if( !InputSys ) InputSys = new DIInputSystem( 0 );
#endif

	#ifdef	TRANSGAMING	
        GameState = PCFSystem::MultiplayerServer;
	#else
        GameState = PCFSystemStates::MultiplayerServer;
	#endif	

		// Start game
		if( !Game )
		{
			Game = (*OurGame)();
#ifdef	TRANSGAMING	
			if( pcfWorld ) delete pcfWorld;
            m_setTag("Object");
			pcfWorld = new class World;
            TRACE("*** PCFSystem::SwitchToState(): initializing game engine for multiplayer server\n");
#else
			if( World ) delete World;
			World = new class World;
#endif	
			Game->Initialize();
#ifdef TRANSGAMING
            TRACE("*** PCFSystem::SwitchToState(): done game engine initialization for multiplayer server\n");
#endif
		}

        if( !Net ){
            m_setTag("Net");
			Net = new NetworkDevice2;
        }

#ifdef PKTV_ENABLED
		// PKTV Start
		if( PKTV )
			delete PKTV;

		{
			bool startPktvSocket = GScript.Globals()["Cfg"]["PKTV"].IsNil()?false:GScript.Globals()["Cfg"]["PKTV"].GetBool();
			float pktvFps = GScript.Globals()["Cfg"]["PKTVFps"].IsNil()?20:GScript.Globals()["Cfg"]["PKTVFps"].GetFloat();
			DWORD pktvDelay = GScript.Globals()["Cfg"]["PKTVDelay"].IsNil()?30000:GScript.Globals()["Cfg"]["PKTVDelay"].GetInt();
			String pktvPassword = GScript.Globals()["Cfg"]["PKTVPassword"].IsNil()?"":GScript.Globals()["Cfg"]["PKTVPassword"].GetString();
			PKTV = new PKTVManager( true, startPktvSocket, pktvFps, pktvDelay );
			PKTV->SetPassword( pktvPassword );
			if( startPktvSocket )
				GLog.Print( "PKTV Listening Socket established\n" );
		}
#endif

		ResetTimer();
#endif
		break;
#ifdef	TRANSGAMING	
	case PCFSystem::MultiplayerClient:
#else
	case PCFSystemStates::MultiplayerClient:
#endif

#ifndef STAND_ALONE_SERVER
		GLog.Print("Changing system state to: MultiplayerClient\n");
		GLog.Print("Starting subsystems...\n");
		UsePhysicsSubsteps = false;

		// Init subsystems

		GLog.Print("Changing system state to: MultiplayerClient\n");

		if (!InitVideo())
			exit(1);

		if( !Physics )
		{
#ifndef TRANSGAMING_NO_HAVOK
            TRACE("PCFSystem::SwitchToState(): multiplayerClient: creating a new physics engine\n");
            m_setTag("Physics");
			Physics = new PhysicsEngine;
			Physics->Init();
            TRACE("PCFSystem::SwitchToState(): multiplayerClient: created physics engine at %p\n", Physics);
#endif
		}
		else
			GEngine->Physics->PhysicsFPS = DEFAULT_PHYSICS_FPS;

#ifdef TRANSGAMING_NO_MILES
			GLog.Print("ERROR: Could not initialize AudioDevice. It is not currently supported on this platform.\n");
#else
		if( !Audio )
		{
            m_setTag("AudioMiles");
			Audio = new MilesEngine;
			if( !Audio->StartedOk() )
			{
				GLog.Print("ERROR: Could not initialize AudioDevice\n");
				delete Audio;
				Audio = 0;
			}
		}

		if( Audio )
		{
			Audio->SetDefaultObstruction( true );
			Audio->SetObstructionProperties( 3.0f, 0.2f, 0.0f, 0.4f, 0.8f, 10.0f, 30.0f );
		}
#endif


        m_setTag("Input");
#if defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_PS2 )
        if( !InputSys ) InputSys = new PSPInputSystem( 0 );
#else
		if( !InputSys ) InputSys = new DIInputSystem( 0 );
#endif


	#ifdef	TRANSGAMING	
        GameState = PCFSystem::MultiplayerClient;
	#else
        GameState = PCFSystemStates::MultiplayerClient;
	#endif	
		// Start game
		if( !Game )
		{
			Game = (*OurGame)();
#ifdef	TRANSGAMING	
			if( pcfWorld ) delete pcfWorld;
            m_setTag("Object");
			pcfWorld = new class World;
            TRACE("*** PCFSystem::SwitchToState(): initializing game engine for multiplayer client\n");
#else
			if( World ) delete World;
			World = new class World;
#endif	
			Game->Initialize();
#ifdef TRANSGAMING
            TRACE("*** PCFSystem::SwitchToState(): done game engine initialization for multiplayer client\n");
#endif
		}

#ifdef PKTV_ENABLED
		if( PKTV ) { delete PKTV; PKTV = 0; };							// Close PKTV manager
#endif

        if( !Net ){
            m_setTag("Net");
			Net = new NetworkDevice2;
        }

		{
			int maxFpsMP = GScript.Globals()["Cfg"]["MaxFpsMP"].IsNil()?0:GScript.Globals()["Cfg"]["MaxFpsMP"].GetInt();
			if( maxFpsMP > 250 )
				maxFpsMP = 250;
			if( maxFpsMP == 0 )
				maxFpsMP = 125;

#ifdef TRANSGAMING
			MinSecondsPerFrame = maxFpsMP?(1.0f / maxFpsMP) : 0.0f;
#else
			MinSecondsPerFrame = maxFpsMP?((double)1.0)/maxFpsMP:0.0;
#endif
		}

		ResetTimer();
#endif
		break;
	default:
        TRACE("*** PCFSystem::SwitchToState(): bad state!\n");
		UsePhysicsSubsteps = false;
		const char* errMsg = "PCF System Error: Tried to switch to a unknown system state!\n";
		GLog.Print( errMsg );
#ifdef TRANSGAMING
        TRACE("[]::PCFSystem::SwitchToState(): ERROR-> tried to switch to an unknown system state!\n");
#else
		MessageBox( 0, errMsg, "ERROR", MB_OK );
#endif
		break;
	}

	GScript.DoString("Game:AfterChangeGameType()" );
}

bool PCFSystem::InitVideo()
{
	if( !Video )
	{
        m_setTag("Video");
		Video = new GraphicsDevice;
		if( !Video->Init() )
		{
#ifdef TRANSGAMING
            TRACE("[]::PCFSystem::InitVideo(): ERROR-> could not initialize the renderer!\n");
#else
			char tmp[256];
			sprintf( tmp, "ERROR: could not Initialize DirectX 9 Renderer\n");
			GLog.Print( tmp );

            MessageBox( 0, tmp, "ERROR", MB_OK );
#endif
			return false;
		}
	}
	return true;
}

void PCFSystem::TakeScreenshot() const
{
#ifndef TRANSGAMING
	String string;
	int i=0;

	FileManagerStd fm;
	fm.CreateDirectory( "../Data/Screenshots" );

	while(true)
	{
		string = String::Sprintf("../Data/Screenshots/Shot%04d.bmp",i);
		if (!fm.FileExist(string.AsChar()))
			break;
		i++;
	}
	GetRenderDevice()->TakeScreenshot(string.AsChar());
#endif
}

RenderDevice* PCFSystem::GetRenderDevice( void ) const
{
    if (!Video) return NULL;
	return Video->RenderDev;
}


#define SAVE_VERSION 3
#define PCFSYSTEM_LOAD_SAVE_TOKEN 0x5e43

bool PCFSystem::LoadGame( FileObject* f )
{
	if( !f->TestToken( PCFSYSTEM_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: PCFSystem::LoadGame()\n" );
		return false;
	}

    TRACE("PCFSystem::LoadGame(): loading a game\n");
    int ver;
    *f >> ver;
    if (SAVE_VERSION != ver){
#ifdef TRANSGAMING
        TRACE("[]::PCFSystem::LoadGame(): save file version is not the same as the current engine version! (saveVer = %d, engineVer = %d)\n", ver, SAVE_VERSION);
#else
		MessageBox(NULL,"Different save version! Crash possible!","Error",MB_TOPMOST);
#endif
    }

#ifdef	TRANSGAMING	
	if( !pcfWorld->LoadGlasses( f ) ) return false;
#else
	if( !World->LoadGlasses( f ) ) return false;
#endif	
    

#ifndef TRANSGAMING_NO_MILES
    GFChunkReader ch(f);

    if (!Audio)
        ch.Next(f);
    else
    {
		if( ch.GetSize() )
		{
			if(!Audio->LoadAudio( f ) ) return false;
		}
		else
			Audio->LoadReset();
    }
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if( !Physics->LoadPhysics( f ) ) return false;
#endif

#ifdef	TRANSGAMING	
	if( !pcfWorld->Paths2.LoadPathfinding( f ) ) return false;
	if( !pcfWorld->LoadEntities( f ) ) return false;
    GScript.DoString("SaveGame:AfterLoadEntities()");
	if( !pcfWorld->LoadPortalState( f ) ) return false;
    if( !pcfWorld->LoadZoneState( f ) ) return false;
#else
	if( !World->Paths2.LoadPathfinding( f ) ) return false;
	if( !World->LoadEntities( f ) ) return false;
    GScript.DoString("SaveGame:AfterLoadEntities()");
	if( !World->LoadPortalState( f ) ) return false;
    if( !World->LoadZoneState( f ) ) return false;
#endif	

	return true;
}

bool PCFSystem::SaveGame( FileObject* f )
{
#ifdef	TRANSGAMING	
	pcfWorld->DeleteDelayedEntities();
#else
    World->DeleteDelayedEntities();
#endif

#ifndef TRANSGAMING_NO_MILES	
	if( Audio )
		Audio->SaveGame_PauseSounds();
#endif
	if( !f->PutToken( PCFSYSTEM_LOAD_SAVE_TOKEN ) ) return false;

    *f << (int) SAVE_VERSION;

#ifdef	TRANSGAMING	
	if( !pcfWorld->SaveGlasses( f ) ) return false;
#else
	if( !World->SaveGlasses( f ) ) return false;
#endif	
    
#ifndef TRANSGAMING_NO_MILES	
    GFChunkWriter ch(f,"AUDIOv01");
	if( Audio && !Audio->SaveAudio( f ) ) return false;
    ch.Complete(f);
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if( !Physics->SavePhysics( f ) ) return false;
#endif

#ifdef	TRANSGAMING	
	if( !pcfWorld->Paths2.SavePathfinding( f ) ) return false;
	if( !pcfWorld->SaveEntities( f ) ) return false;
	if( !pcfWorld->SavePortalState( f ) ) return false;
    if( !pcfWorld->SaveZoneState( f ) ) return false;
#else
	if( !World->Paths2.SavePathfinding( f ) ) return false;
	if( !World->SaveEntities( f ) ) return false;
	if( !World->SavePortalState( f ) ) return false;
    if( !World->SaveZoneState( f ) ) return false;
#endif	

#ifndef TRANSGAMING_NO_MILES	
	if( Audio )
		Audio->SaveGame_ResumeSounds();
#endif
	return true;
}

bool PCFSystem::ZoneAlarmNetworkAccessPopUp( void )
{
	// This code opens up a listening socket and tries to send an udp packet from it to itself
	// This should make ZoneAlarm pop-up before graphic initialization. Unfortunately it only makes one type of
	// popup out of four types.

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1,1);
	if( WSAStartup(wVersionRequested, &wsaData ) ) return false;
	if( ( LOBYTE(wsaData.wVersion) != 1 ) && ( HIBYTE(wsaData.wVersion) != 1 ) ) { WSACleanup(); return false; };

	in_addr	hostAddr;
	hostAddr.S_un.S_addr = INADDR_ANY;
	char hostnam[256];
	if( gethostname( hostnam, 255 ) ) { WSACleanup(); return false; };	// this is enough to popup one-style of ZoneAlarm window

	hostent* host = gethostbyname( hostnam );
	if( !host ) { WSACleanup(); return false; };

	in_addr** ia = (in_addr**)host->h_addr_list;
	while( *ia )
	{
		in_addr* ia2 = *ia;
		String newStr = String::Sprintf( "%d.%d.%d.%d", ia2->S_un.S_un_b.s_b1, ia2->S_un.S_un_b.s_b2, ia2->S_un.S_un_b.s_b3, ia2->S_un.S_un_b.s_b4 );
		MyInterfaces.AddUnique( newStr );
		ia++;
	}

	WSACleanup();
	return true;
}

void PCFSystem::LoadingTick( int progress )
{
/*
	static DWORD lastTickTime = 0;

	DWORD curTime = SysDriver.GetCurrentTimeMS();
	if( lastTickTime )
	{
		GLog.Print( "Between ticks: %u ms\n", curTime-lastTickTime );
		Assert( curTime-lastTickTime < 1000 );
	}
	else
		GLog.Print( "Load start\n" );
	curTime = SysDriver.GetCurrentTimeMS();
	lastTickTime = curTime;
*/
	if( Video && Video->PMenu && Video->PMenu->LoadScreen.Active() )
		Video->PMenu->LoadScreen.Progress( progress );

#ifdef PKTV_ENABLED
	if( PKTV )
	{
		PKTV->MapLoading = true;
		PKTV->Tick();
		PKTV->MapLoading = false;
	}
#endif
}
