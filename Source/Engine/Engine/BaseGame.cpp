#include "Engine.h"

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"
//#include "Audio/AudioDevice.h"

#ifndef TRANSGAMING_NO_MILES
// blair - Rework for PSP support later.
#include "Audio/MilesEngine.h"
#endif

#ifdef TRANSGAMING_PSP
# include <tg_pc2psp.h>
#elif defined(TRANSGAMING_PS2)
# include <tg_pc2ps2.h>
#endif

#include "Engine/BaseGame.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Net/NetworkDevice2.h"
#include "Net/DemoRecording2.h"
#ifdef PKTV_ENABLED
# include "Net/PKTVManager.h"
#endif

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

extern ENGINE_API View* (*OurView)(void);

#define SIMPLE_TIMING

#ifdef TRANSGAMING_WINDOWS
#include "Engine/simpleTiming.h"
#else
#include "simpleTiming.h"
#endif

bool g_gatherTiming = false;


EngineGame::EngineGame() : mPaused(false), mUpdateLogic(false)
{
//#ifdef _DEBUG
//	canPreprocessPlayer = true;
//	preprocessPlayerNow = false;
//#endif
}

void EngineGame::Initialize()
{
#ifdef TRANSGAMING
    TRACE("*** EngineGame::Initialize(): beginning engine initialization\n");
#endif
    if (GEngine->Video)
    {
	    View* view = (*OurView)();
	    view->Init();
#ifdef	TRANSGAMING
        TRACE("*** EngineGame::Initialize(): creating world and view\n");
	    view->SetWorld( GEngine->pcfWorld );
	    GEngine->Video->AddView( view);
	    GEngine->Video->SetViewAsActive(view);
        GEngine->pcfWorld->Paths.SetViewport( &GEngine->Video->GetActiveView()->LocalViewport );
#else
	    view->SetWorld( GEngine->World );
	    GEngine->Video->AddView( view);
	    GEngine->Video->SetViewAsActive(view);
        GEngine->World->Paths.SetViewport( &GEngine->Video->GetActiveView()->LocalViewport );
#endif
    }

#ifndef TRANSGAMING_NO_MILES
    if (GEngine->Audio)
    {
		if( !GScript.Globals()["Cfg"]["SoundProvider3D"].IsNil() )
		{
			if( !GEngine->Audio->Set3DSoundProvider( GScript.Globals()["Cfg"]["SoundProvider3D"].GetString() ) )
//				GEngine->Audio->SetFirstSuccesfulProvider();
				GEngine->Audio->SetMostSuitableProvider();
		}

# if !defined(TRANSGAMING_PS2)
		if( GEngine->Audio->GetCurrentSoundProvider() == -1 )
		{
			GLog.Print("ERROR: Could not find any 3D sound provider!!! Audio off.\n");
			delete GEngine->Audio;
			GEngine->Audio = 0;
		}
		else
		{
# endif
		    Vector front(0,0,1);
		    Vector up(0,1,0);
		    GEngine->Audio->SetPlayerOrientation( front, up );
		}
# if !defined(TRANSGAMING_PS2)
    }
# endif
#endif

#ifdef TRANSGAMING
    TRACE("*** EngineGame::Initialize(): calling script init\n");
	GScript.DoString("Game:Init()");
    TRACE("*** EngineGame::Initialize(): done script init\n");
#else
	GScript.DoString("Game:Init()");
#endif

	mUpdateLogic = true;
}

void EngineGame::Close()
{
#ifdef	TRANSGAMING
	GEngine->pcfWorld->Release();
#else
	GEngine->pcfWorld->Release();
#endif
}

#ifdef TRANSGAMING_HACK
void EngineGame::TickObjsAndRender(float timeDelta)
{
	//GScript.Call("Game_Tick",0,"%f",timeDelta);
	GScript.Call("Game_Render",0,"%f",timeDelta);
}
#endif

/*
#define SIMPLE_TIMING
#ifdef TRANSGAMING_WINDOWS
#include "Engine/simpleTiming.h"
#else
#include "simpleTiming.h"
#endif
bool g_gatherTiming = false;
*/

void EngineGame::ScriptGameTick(float td){
    GScript.Call("Game_Tick", 0, "%f", td);
}

void EngineGame::ScriptGameTick2(float td){
    GScript.Call("Game_Tick2", 0, "%f", td);
}

void EngineGame::ScriptGameTick3(float td){
    GScript.Call("Game_Tick3", 0, "%f", td);
}

#ifdef TRANSGAMING_ENABLE_LUA_PROFILER
void EngineGame::StartLuaProfile()
{
    GScript.Call("Game_StartLuaProfile", 0, "");
}

void EngineGame::StopLuaProfile()
{
    GScript.Call("Game_StopLuaProfile", 0, "");
}
#endif

void EngineGame::Tick(bool minimized, float timeDelta)
{
    DBG_TIMER_DEFINE(9)
//	static DWORD lastMS = 0;

	View* view;

//	GLog.Print( "MinSecPerFrame: %f, timeDelta: %f\n", GEngine->MinSecondsPerFrame, timeDelta );

#ifdef TRANSGAMING_ENABLE_LUA_PROFILER
    StartLuaProfile();
#endif

	switch( GEngine->GameState )
	{
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	case PCFSystem::MultiplayerServer:
#else
	case PCFSystem::PCFSystemStates::MultiplayerServer:
#endif

		Assert( GEngine->InputSys );
        Assert( GEngine->Video );

		GProfiler.SetState(INPUTSYS_TICK);
        if (!minimized)
        {
		    GEngine->InputSys->Tick();
		    ProcessInput();
        }

		if( mUpdateLogic )
		{
			GProfiler.SetState(UPDATE_LOGIC);
			//GScript.Call("Game_Tick",0,"%f",timeDelta);
            ScriptGameTick(timeDelta);
		}

		if( !GEngine->Net )
			break;

		{
			GProfiler.SetState(PHYSICS_TICK);
			// Move the local player
			Entity* pe = GEngine->Net->GetLocalPlayer();
			if( pe && pe->PO )
			{
				Assert( GEngine->Video );

				View * view = GEngine->Video->GetActiveView();
				float pitch = -FDeg2Rad(view->mCamera.GetRotation().Y);
				float yaw = -FDeg2Rad(view->mCamera.GetRotation().X);
				yaw = fmod( F_PI + fmod(yaw,2*F_PI), 2*F_PI );
				if( yaw > F_PI ) yaw -= 2*F_PI;
#ifndef TRANSGAMING_NO_HAVOK
				pe->PO->MultiPlayerActionNonPhysic( yaw, pitch, timeDelta );
#endif
			}

#ifndef TRANSGAMING_NO_HAVOK
			GEngine->Physics->Tick(timeDelta);
#endif
		}
#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
		{
			GProfiler.SetState(AUDIO_TICK);
	        GEngine->Audio->Tick(timeDelta);
		}
#endif
		if( mUpdateLogic )
		{
			GProfiler.SetState( UPDATE_LOGIC );
			//GScript.Call("Game_Tick2",0,"%f", timeDelta ); // after physics
            ScriptGameTick2(timeDelta);
		}
		GProfiler.SetState(WORLD_TICK);
#ifdef	TRANSGAMING
		GEngine->pcfWorld->Tick(timeDelta);
#else
		GEngine->World->Tick(timeDelta);
#endif

		if( mUpdateLogic )
		{
			GProfiler.SetState( UPDATE_LOGIC );
			//GScript.Call("Game_Tick3",0,"%f", timeDelta ); // after world tick
            ScriptGameTick3(timeDelta);
		}

        if (!minimized)
        {
            GProfiler.SetState(CLIENT_RENDER);
            GEngine->Video->MaterialSys->Tick(timeDelta);
            view = GEngine->Video->GetActiveView();
            view->Render(timeDelta);
        }
		GProfiler.SetState(OTHER);

#ifndef TRANSGAMING
        GEngine->Video->Hud->mConsole.ExecLastCommand();
#endif

#ifdef PKTV_ENABLED
		if( GEngine->PKTV )
			GEngine->PKTV->Tick();
#endif

		break;
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	case PCFSystem::DedicatedServer:
#else
	case PCFSystem::PCFSystemStates::DedicatedServer:
#endif

		Assert( !GEngine->InputSys );
        Assert( !GEngine->Audio );
        Assert( !GEngine->Video );

		if( mUpdateLogic )
		{
			GProfiler.SetState(UPDATE_LOGIC);
			//GScript.Call("Game_Tick",0,"%f",timeDelta);
            ScriptGameTick(timeDelta);
		}

		{
			GProfiler.SetState(PHYSICS_TICK);
			// Move physical entities
#ifndef TRANSGAMING_NO_HAVOK
			GEngine->Physics->Tick(timeDelta);
#endif
		}

		if( mUpdateLogic )
		{
			GProfiler.SetState( UPDATE_LOGIC );
			//GScript.Call("Game_Tick2",0,"%f", timeDelta ); // after physics
            ScriptGameTick2(timeDelta);
		}

		GProfiler.SetState(WORLD_TICK);
#ifdef	TRANSGAMING
		GEngine->pcfWorld->Tick(timeDelta);
#else
		GEngine->World->Tick(timeDelta);
#endif
		GProfiler.SetState(OTHER);

#ifdef PKTV_ENABLED
		if( GEngine->PKTV )
			GEngine->PKTV->Tick();
#endif

		break;
#endif
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	case PCFSystem::MultiplayerClient:
#else
	case PCFSystem::PCFSystemStates::MultiplayerClient:
#endif

		#ifdef ENGINE_LOG
		GLog.Print("^* GT1\n");
		#endif
		// Input Sys
        if( !minimized )
        {
		    GProfiler.SetState(INPUTSYS_TICK);
		    GEngine->InputSys->Tick();
		    ProcessInput();
        }

		#ifdef ENGINE_LOG
		GLog.Print("^* GT2\n");
		#endif
		if( mUpdateLogic )
		{
			GProfiler.SetState(UPDATE_LOGIC);
			//GScript.Call("Game_Tick",0,"%f",timeDelta);
            ScriptGameTick(timeDelta);
		}
		#ifdef ENGINE_LOG
		GLog.Print("^* GT3\n");
		#endif

//			DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
//			GLog.Print( "time diff: %d, timeDelta: %f\n", curTime-lastMS, timeDelta );
//			lastMS = curTime;

		// Physics
		GProfiler.SetState( PHYSICS_TICK );
		if( GEngine->Net )
		{
			// Move the local player
			Entity* pe = GEngine->Net->GetLocalPlayer();
			if( pe && pe->PO )
			{
				Assert( GEngine->Video );

				View * view = GEngine->Video->GetActiveView();
				float pitch = -FDeg2Rad(view->mCamera.GetRotation().Y);
				float yaw = -FDeg2Rad(view->mCamera.GetRotation().X);
				yaw = fmod( F_PI + fmod(yaw,2*F_PI), 2*F_PI );
				if( yaw > F_PI ) yaw -= 2*F_PI;
#ifndef TRANSGAMING_NO_HAVOK
				pe->PO->MultiPlayerActionNonPhysic( yaw, pitch, timeDelta );
#endif
			}
		}

#ifndef TRANSGAMING_NO_HAVOK
		GEngine->Physics->Tick(timeDelta,true);
#endif

		#ifdef ENGINE_LOG
		GLog.Print("^* GT4\n");
        #endif

#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
		{
			GProfiler.SetState(AUDIO_TICK);
	        GEngine->Audio->Tick(timeDelta);
		}
#endif
		#ifdef ENGINE_LOG
		GLog.Print("^* GT5\n");
		#endif

		if( !minimized )
		{
	        Assert( GEngine->Video );
            GEngine->Video->MaterialSys->Tick(timeDelta);
		}

		#ifdef ENGINE_LOG
		GLog.Print("^* GT6\n");
		#endif

		if( mUpdateLogic )
		{
			GProfiler.SetState( UPDATE_LOGIC );
			//GScript.Call("Game_Tick2",0,"%f", timeDelta ); // after physics
            ScriptGameTick2(timeDelta);
		}
		#ifdef ENGINE_LOG
		GLog.Print("^* GT7\n");
		#endif

		GProfiler.SetState(WORLD_TICK);
#ifdef	TRANSGAMING
		GEngine->pcfWorld->Tick(timeDelta);
#else
		GEngine->World->Tick(timeDelta);
#endif

		#ifdef ENGINE_LOG
		GLog.Print("^* GT8\n");
		#endif

		GProfiler.SetState( PHYSICS_TICK );
#ifdef	TRANSGAMING
#ifndef TRANSGAMING_NO_HAVOK
        if (GEngine->pcfWorld->Physics)
            GEngine->pcfWorld->Physics->UpdateEntities();
#endif // TRANSGAMING_NO_HAVOK
#else
        if (GEngine->World->Physics)
            GEngine->World->Physics->UpdateEntities();
#endif

		if( mUpdateLogic )
		{
			GProfiler.SetState( UPDATE_LOGIC );
			//GScript.Call("Game_Tick3",0,"%f", timeDelta ); // after world tick
            ScriptGameTick3(timeDelta);
		}

		#ifdef ENGINE_LOG
		GLog.Print("^* GT9\n");
		#endif

		if( !minimized )
		{
		    Assert( GEngine->Video );
	        GProfiler.SetState( CLIENT_RENDER );
	        view = GEngine->Video->GetActiveView();
	    	view->Render( timeDelta );
		}

		#ifdef ENGINE_LOG
		GLog.Print("^* GT10\n");
		#endif

		GProfiler.SetState( OTHER );

#ifndef TRANSGAMING
        GEngine->Video->Hud->mConsole.ExecLastCommand();
#endif

#ifdef PKTV_ENABLED
		if( GEngine->PKTV )
			GEngine->PKTV->Tick();
#endif

		break;
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	case PCFSystem::DemoPlay:
#else
	case PCFSystem::PCFSystemStates::DemoPlay:
#endif

		// Input Sys
        if( !minimized )
		{
			GProfiler.SetState(INPUTSYS_TICK);
			GEngine->InputSys->Tick();
			ProcessInput();
		}

#ifdef PKTV_ENABLED
		Assert( GEngine->PKTV );
		GEngine->PKTV->Tick();
#endif

#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
		{
			GProfiler.SetState(AUDIO_TICK);
	        GEngine->Audio->Tick(timeDelta);
		}
#endif
		if( !minimized )
		{
	        GProfiler.SetState( CLIENT_RENDER );
	        Assert( GEngine->Video );
            GEngine->Video->MaterialSys->Tick(timeDelta);
		}

		GProfiler.SetState(WORLD_TICK);

#ifdef PKTV_ENABLED
# ifdef	TRANSGAMING
		GEngine->pcfWorld->PKTVDemoTick(timeDelta);
# else
		GEngine->World->PKTVDemoTick(timeDelta);
# endif
#endif

		if( !minimized )
		{
		    Assert( GEngine->Video );
	        GProfiler.SetState( CLIENT_RENDER );
	        view = GEngine->Video->GetActiveView();
	    	view->Render( timeDelta );
		}

		GProfiler.SetState( OTHER );
#ifndef TRANSGAMING
        GEngine->Video->Hud->mConsole.ExecLastCommand();
#endif

		break;
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	case PCFSystem::SingleGame:
#else
	case PCFSystem::PCFSystemStates::SingleGame:
#endif
	default:

		GProfiler.SetState(INPUTSYS_TICK);

        DBG_TIMER_GRABTIME("Input", 0);

        Assert( GEngine->InputSys );
		GEngine->InputSys->Tick();
		ProcessInput();

	    if( !mPaused )
		{
			if( mUpdateLogic )
			{
				GProfiler.SetState(UPDATE_LOGIC);
                DBG_TIMER_GRABTIME("Logic", 1);
				//GScript.Call("Game_Tick",0,"%f",timeDelta);
                ScriptGameTick(timeDelta);
			}
#ifdef SIMPLE_TIMING
            else
                DBG_TIMER_GRABTIME("Logic", 1);
#endif

#ifdef TRANSGAMING
			if( GEngine->Physics )
#else
			if( !( gDemoRec && gDemoRec->IsPlaying() ) && GEngine->Physics )
#endif
			{
				GProfiler.SetState(PHYSICS_TICK);
                DBG_TIMER_GRABTIME("Physics", 2);
#ifndef TRANSGAMING_NO_HAVOK
				GEngine->Physics->Tick(timeDelta);
#endif
			}
#ifdef SIMPLE_TIMING
            else
                DBG_TIMER_GRABTIME("Physics", 2);
#endif

#ifndef TRANSGAMING_NO_MILES
			if( GEngine->Audio )
			{
				GProfiler.SetState(AUDIO_TICK);
	            GEngine->Audio->Tick(timeDelta);
			}
#endif
            GProfiler.SetState(CLIENT_RENDER);

            DBG_TIMER_GRABTIME("Render", 3);

            Assert( GEngine->Video );
            GEngine->Video->MaterialSys->Tick(timeDelta);

			if( mUpdateLogic )
			{
				GProfiler.SetState( UPDATE_LOGIC );

                DBG_TIMER_GRABTIME("Logic2", 4);

				//GScript.Call("Game_Tick2",0,"%f", timeDelta ); // after physics
                ScriptGameTick2(timeDelta);
			}
#ifdef SIMPLE_TIMING
            else
                DBG_TIMER_GRABTIME("Logic2", 4);
#endif


			GProfiler.SetState(WORLD_TICK);
            DBG_TIMER_GRABTIME("World", 5);

#ifdef	TRANSGAMING
			GEngine->pcfWorld->Tick(timeDelta);
#else
			GEngine->World->Tick(timeDelta);
#endif
		}
#ifdef SIMPLE_TIMING
        else{
            DBG_TIMER_GRABTIME("Logic",     1);
            DBG_TIMER_GRABTIME("Physics",   2);
            DBG_TIMER_GRABTIME("Render",    3);
            DBG_TIMER_GRABTIME("Logic2",    4);
            DBG_TIMER_GRABTIME("World",     5);
        }

#endif


#ifdef _DEBUG

#ifdef TRANSGAMING
		if( GEngine->pcfWorld && GEngine->pcfWorld->Physics )
			GEngine->pcfWorld->Physics->StaticMeshEntityCheck();
#else
		if( GEngine->World && GEngine->World->Physics )
			GEngine->World->Physics->StaticMeshEntityCheck();
#endif // TRANSGAMING

#endif // _DEBUG

	    Assert( GEngine->Video );
        GProfiler.SetState( CLIENT_RENDER );

        DBG_TIMER_GRABTIME("Render2", 6);

        view = GEngine->Video->GetActiveView();
    	view->Render( timeDelta );
		GProfiler.SetState( OTHER );

        DBG_TIMER_GRABTIME("Other", 7);

#ifndef TRANSGAMING
        GEngine->Video->Hud->mConsole.ExecLastCommand();
#endif

#ifdef PKTV_ENABLED
		if( GEngine->PKTV )
			GEngine->PKTV->Tick();
#endif

#ifdef SIMPLE_TIMING
        static int frameCounter = 0;

        frameCounter++;
        if (timeDelta > 0.30f || (timeDelta <= 0.30f && (frameCounter % 10) == 0)){
            DBG_TIMER_DUMP("EngineGame::Tick(): timings:");
        }
#endif
		break;
	}

#ifdef TRANSGAMING_ENABLE_LUA_PROFILER
		StopLuaProfile();
#endif
}

Entity * EngineGame::CreatePlayer(LPCSTR modelName,bool ragdoll)
{
    Entity *player;
#ifdef	TRANSGAMING
    player = GEngine->pcfWorld->CreateEntity( Entity::ETypeModel, modelName, "", 0.155f );
#else
    player = GEngine->World->CreateEntity( Entity::ETypeModel, modelName, "", 0.155f );
#endif

    if (!player)
    {
#ifdef TRANSGAMING
        TRACE("[]::EngineGame::CreatePlayer(): ERROR-> could not create player entity!\n");
#else
        MessageBox(NULL,"Cannot create player entity!!!","Error",MB_TOPMOST);
#endif
        return NULL;
    }

    if( !ragdoll )
    {
#ifndef TRANSGAMING_NO_HAVOK
		Ragdoll* r = ((Model*)player)->RagdollPtr;
	    if( r ) delete r;
        ((Model*)player)->RagdollPtr = NULL;
#endif
    }

	player->SetPositionAndRotation( Vector(0,100,0),Quaternion(0,Vector(0,1,0)) );
#ifdef	TRANSGAMING
	GEngine->pcfWorld->AddEntity( player );
#else
	GEngine->World->AddEntity( player );
#endif
	player->Flags |= Entity::EEntAlwaysVisible|Entity::EEntNoDraw|Entity::EEntDestroyOnReload;		// don't draw player entity

    // Physics Object
    player->CreatePhysicsObject( EPawnBodyPlayer, 1.0f, -1, true );
    //player->PO->SetModelPivotOffset(Vector(0,6,0));
#ifndef TRANSGAMING_NO_HAVOK
    player->PO->ZeroVelocity();
#endif

    if (GEngine->Video)
    {
        View * view = GEngine->Video->GetActiveView();
#ifndef TRANSGAMING_NO_HAVOK
        if (view)
            player->PO->SetPawnHeadPos( view->mCamera.GetPosition() );
#endif
    }

    return player;
}

void EngineGame::SwitchMenu( bool force )
{
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	if( GEngine->Video->MBoard->Active() )
	{
		if( force )
			SwitchMagicBoard( true );
		else
			return;
	}
	else if( GEngine->Video->MapSel->Active() )
	{
		if( force )
			SwitchMapSelect( true );
		else
			return;
	}

	String curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();

	if( GEngine->Video->PMenu->Active() && curLevelName == "NoName" && !force )
		return;

	if( !GEngine->Video->PMenu->Active() )
	{
		ShowMenu();
	}
	else
	{
		HideMenu();
	}
}

void EngineGame::ShowMenu()
{
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	if( GEngine->Video->PMenu->Active() )
		return;

	View * view = GEngine->Video->GetActiveView();
	if( view )
	{
		view->Mode |= View::EModeDoNotRenderWorld;
		mPaused = true;
		GEngine->Video->PMenu->Activate( true );
	}
}

void EngineGame::HideMenu()
{
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	if( !GEngine->Video->PMenu->Active() )
		return;

	View * view = GEngine->Video->GetActiveView();
	if( view )
	{
		view->Mode &= ~View::EModeDoNotRenderWorld;
		mPaused = false;
		GEngine->Video->PMenu->Activate( false );
		GScript.DoString( "Game.Paused = false" );
	}
}

void EngineGame::SwitchMagicBoard( bool force )
{
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	if( GEngine->Video->PMenu->Active() )
	{
		if( force )
			SwitchMenu( true );
		else
			return;
	}
	else if( GEngine->Video->MapSel->Active() )
	{
		if( force )
			SwitchMapSelect( true );
		else
			return;
	}

	String curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();
	if( GEngine->Video->MBoard->Active() && curLevelName == "NoName" && !force )
		return;

	View * view = GEngine->Video->GetActiveView();
	if( view )
	{
		if( !GEngine->Video->MBoard->Active() )
		{
			view->Mode |= View::EModeDoNotRenderWorld;
			mPaused = true;
		}
		GEngine->Video->MBoard->Activate( !GEngine->Video->MBoard->Active() );
		if( !GEngine->Video->MBoard->Active() )
		{
			view->Mode &= ~View::EModeDoNotRenderWorld;
			mPaused = false;
			GScript.DoString( "Game.Paused = false" );
		}
	}
}

void EngineGame::SwitchMapSelect( bool force )
{
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	if( GEngine->Video->PMenu->Active() )
	{
		if( force )
			SwitchMenu( true );
		else
			return;
	}
	else if( GEngine->Video->MBoard->Active() )
	{
		if( force )
			SwitchMagicBoard( true );
		else
			return;
	}

	String curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();
	if( GEngine->Video->MapSel->Active() && curLevelName == "NoName" && !force )
		return;

	View * view = GEngine->Video->GetActiveView();
	if( view )
	{
		if( !GEngine->Video->MapSel->Active() )
		{
			view->Mode |= View::EModeDoNotRenderWorld;
			mPaused = true;
		}
		GEngine->Video->MapSel->Activate( !GEngine->Video->MapSel->Active() );
		if( !GEngine->Video->MapSel->Active() )
		{
			view->Mode &= ~View::EModeDoNotRenderWorld;
			mPaused = false;
			GScript.DoString( "Game.Paused = false" );
		}
	}
}

void EngineGame::SwitchConsole()
{
#ifndef TRANSGAMING
	ScriptObject game = GScript.Globals()["Game"];

	if( game["Paused"].GetBool() )
		return;

	GEngine->Video->Hud->mConsole.Activate( !GEngine->Video->Hud->mConsole.Active() );
#endif
}

void EngineGame::ShowMPStats()
{
	GEngine->Video->Hud->MPStats.Activate( true );
}

void EngineGame::HideMPStats()
{
	GEngine->Video->Hud->MPStats.Activate( false );
}


void EngineGame::RecordingUpdateViewFromPlayer( Entity *player, float yaw, float dir )
{
#ifndef TRANSGAMING
	if( !gDemoRec->IsPlaying() ) return;

	View* view = GEngine->Video->GetActiveView();
	Vector camRot = view->mCamera.GetRawRotation();


	camRot.Z = 0.0f;
#ifndef TRANSGAMING_NO_HAVOK
	if( player->PO )
	{
		if( dir != 1000.0f )
		{
			camRot.X = 180.0f-dir*180.0/F_PI;
			if( camRot.X < 0.0f ) camRot.X += 360.0f;
			if( camRot.X >= 360.0f ) camRot.X -= 360.0f;
		}

		Vector destPos = player->PO->GetPawnHeadPos();

		if( view->mCamera.Interpolation )
		{
			Vector diff = destPos - LastCamPos;
			if( diff.Magnitude() < 3 )				// interpolation
				destPos = LastCamPos + diff * 0.33f;
		}

		view->mCamera.SetPosition( destPos );
		LastCamPos = destPos;
	}
#endif

	// y lock
	camRot.Y = yaw;
	if( camRot.Y > +80.0f ) camRot.Y = +80.0f;
	if( camRot.Y < -80.0f ) camRot.Y = -80.0f;

	view->mCamera.SetRotation(camRot);
	view->mCamera.InverseTransform();
#endif
}
