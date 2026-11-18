#include <stdafx.h>
#include "Mainfrm.h"
#include "Engine.h"
#include "Main.h"
#include "EditorGame.h"
#include "EditorView.h"
#include "Render/HUD.h"
#include "Render/Video.h"
#include "Net/NetworkDevice2.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/InputSystem.h"
//#include "Audio/AudioDevice.h"
#include "Audio/MilesEngine.h"

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Net/DemoRecording2.h"

#ifdef TRANSGAMING_WINDOWS
#include "Physics/PhysicsEngine.h"
#include "Engine/World.h"
#endif

void EditorGame::ProcessInput()
{
	View * view = GEngine->Video->GetActiveView();
	if( !GEngine->InputSys ) return;
	static bool firstTimeMenuInit = true;

	InputSystem::KeyInfo* KeyTable = GEngine->InputSys->GetKeyTable();
	DynamicArray<int>& ActiveKeyList = GEngine->InputSys->GetActiveKeyList();

	for( int i = 0; i < ActiveKeyList.Size(); ++i )
	{
		InputSystem::KeyInfo& key = KeyTable[ActiveKeyList[i]];

		if( key.Status == EKeyPressed )
		{
			//GLog.Print("key_pressed: %d\n",key->Key);
			const float d = 0.05f;

			if( ( GEngine->InputSys->GetPauseKey() && key.Key == GEngine->InputSys->GetPauseKey() ) || ( GEngine->InputSys->GetAltPauseKey() && key.Key == GEngine->InputSys->GetAltPauseKey() ) )
			{
#ifdef TRANSGAMING
				if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MapSel->Active()
					&& !GEngine->Video->MBoard->Active() )
#else
				if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MapSel->Active()
					&& !GEngine->Video->MBoard->Active() && !GEngine->Video->Hud->mConsole.Active() )
#endif
				{
					mPaused = !mPaused;
					if( mPaused )
						GScript.DoString( "Game:Pause(true)" );
					else
						GScript.DoString( "Game:Pause(false)" );
				}
			}

			switch(key.Key)
			{
			/*case 'I': 
				GEngine->InputSys->InvertMouse = !GEngine->InputSys->InvertMouse;
				break;*/

			case 0xDF/*192*/:
				SwitchConsole();
				break;

			case 'H': 
				#ifndef EDITOR
                if (!GEngine->InputSys->GetKeyStatus(VK_RSHIFT) || !GEngine->InputSys->GetKeyStatus(VK_LCONTROL))
					break;
				#endif
				if (! (view->HUDMode & View::EModeHUD_Show))
				{
					view->HUDMode |= View::EModeHUD_Show;
					view->HUDMode |= View::EModeHUD_FPSOnly;
				}
				else
				{
					if (view->HUDMode & View::EModeHUD_FPSOnly)
						view->HUDMode &= ~View::EModeHUD_FPSOnly;
					else
						view->HUDMode &= ~View::EModeHUD_Show;
				}					
				break;

							// Speedying up or slowing down playing of recording
			case VK_ADD:
				if( gDemoRec && gDemoRec->IsPlaying() )
				{
					if( gDemoRec->GetSpeed() < 1.0f )
						gDemoRec->SetSpeed( gDemoRec->GetSpeed() + 0.1f );
					else
						gDemoRec->SetSpeed( gDemoRec->GetSpeed() + 1.0f );
					GScript.DoString( "CONSOLE.AddMessage( 'Play speed is now %d%% of normal' )", ( int )( 100.0f * gDemoRec->GetSpeed() ) );
				}
				break;
			case VK_SUBTRACT:
				if( gDemoRec && gDemoRec->IsPlaying() )
				{
					if( gDemoRec->GetSpeed() <= 1.0f )
						gDemoRec->SetSpeed( gDemoRec->GetSpeed() - 0.1f );
					else
						gDemoRec->SetSpeed( gDemoRec->GetSpeed() - 1.0f );
					GScript.DoString( "CONSOLE.AddMessage( 'Play speed is now %d%% of normal' )", ( int )( 100.0f * gDemoRec->GetSpeed() ) );
				}
				break;


/*            case VK_TAB:
                {
                    bool static bt = false;
                    bt = !bt;
                    float tm = 1.0f;
                    if (bt)
                        tm = 1.0f/8.0f;
                    GEngine->SetTimeMultiplier(tm);
                    GEngine->Audio->SetSpeed(GEngine->TimeMultiplier);
                    GEngine->Audio->SetLowPass(sqrt(GEngine->TimeMultiplier));
                }
                break;*/
		#ifdef EDITOR
                /*
			case 'M':
				if( view )
				{
					if( firstTimeMenuInit )
					{
						firstTimeMenuInit = false;
						view->Mode |= View::EModeDoNotRenderWorld;
					}
					GEngine->Video->Menu.Activate( !GEngine->Video->Menu.Active() );
					if( !GEngine->Video->Menu.Active() )
						view->Mode &= ~View::EModeDoNotRenderWorld;
				}
				break;
                */
            /*case VK_F9:
                if( !GEngine->Video->PMenu->Active() )
                    ((CMainFrame*)AfxGetMainWnd())->PostMessage(WM_APP + 2555,1);
                break;*/

			case 'K':
                SwitchMapSelect( true );
                return;
//#ifdef _DEBUG
//			case 'L':
//				preprocessPlayerNow = true;
//				return;
//#endif
			case 'B':
                SwitchMagicBoard( true );
                return;
			case 'N':
                SwitchMenu( true );
                return;
			case 'O':
				view->Mode ^= View::EModeFrozenOcclude;
				break;

			case VK_F10:
				SwitchToFullScreen( !GEngine->Video->IsFullScreen );
				return;
//			case VK_ESCAPE:
//				if( GEngine->Video->IsFullScreen && !GEngine->Video->PMenu->Active() )
//					SwitchToFullScreen( false );
//				break;
			case VK_F5:
				((CMainFrame*)AfxGetMainWnd())->OnPlay();
                GScript.DoString("Game:SwitchPlayerToPhysics(true)");                
				return;
			case VK_F6:
				((CMainFrame*)AfxGetMainWnd())->OnPause();
				break;

			case 'G':
				view->FakeStuff();
				break;
/*
			case VK_F7:
			case VK_F8:
				{
					float speedTM = (key.Key==VK_F7)?GEngine->TimeMultiplier*0.5f:GEngine->TimeMultiplier*2.0f;
					bool wasNormal = ( GEngine->TimeMultiplier == 1.0f );
					GEngine->SetTimeMultiplier( speedTM );
					if( GEngine->Audio )
					{
						float speedTM2 = (GEngine->TimeMultiplier-1.0f)*0.5f + 1.0f;
						GEngine->Audio->SetSpeed( speedTM2 );
						GEngine->Audio->SetLowPass( sqrt( speedTM2 ) );

						if( ( GEngine->TimeMultiplier == 1.0f ) && !wasNormal )
						{
							GEngine->Audio->Reset();
							GLog.Print( "Sound Reset\n" );
						}
					}
				}
				break;
*/
				// UWAGA: ma byc zrobione Shift+F7 -> TimeMultiplier * 1/8, oraz Shift+F8 -> TimeMultiplier * 8
		#else
			
		#endif
			}
		}
		else if(key.Status==EKeyUp)
		{
//		#ifndef EDITOR
			switch(key.Key)
			{
			case VK_ESCAPE:
				/*#ifndef EDITOR
					if (!GEngine->InputSys->GetKeyStatus(VK_RSHIFT))
						break;
				#endif*/
				///AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
				if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MapSel->Active() && !GEngine->Video->MBoard->Active() )
				{
					CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();
					if (strLevName != "NoName" && GEngine->Video)
					{
						ShowMenu();
						GEngine->Video->PMenu->PauseSounds();
					}
				}
				return;
			}
//		#endif
		}
		else if( key.Status == EKeyDown )
		{
			switch(key.Key)
			{
			// Changing HUD transparency
			case VK_ADD:
				{
					BYTE tr = GEngine->Video->Hud->GetTransparency();
					if( tr < 255 ) tr++;
					GEngine->Video->Hud->SetTransparency( tr );
				}
				break;

			case VK_SUBTRACT:
				{
					BYTE tr = GEngine->Video->Hud->GetTransparency();
					if( tr > 0 ) tr--;
					GEngine->Video->Hud->SetTransparency( tr );
				}
				break;
			}
		}
	}	
}

/*
bool                        DummyMessageHandler::m_bEnabledSaveAll =       true;
bool                        DummyMessageHandler::m_bPausedScripts =        true;
DummyMessageHandler::Msg    DummyMessageHandler::m_msgQueue[QUEUE_SIZE] =  {0};
int                         DummyMessageHandler::m_msgCount =              0;


void DummyMessageHandler::EnableSave(bool enable){
	m_bEnabledSaveAll = enable;
	m_bPausedScripts = enable;
}

void DummyMessageHandler::OnPlay(){
	if (GEngine && GEngine->Game){
#ifndef TRANSGAMING_NO_HAVOK
        TRACE("DummyMessageHandler::OnPlay(): ticking physics\n");
		GEngine->Physics->Tick(0.001f);
#endif

		m_bPausedScripts = false;
        if (m_bEnabledSaveAll)
			GScript.DoString("Game:OnPlay(true)");
		else
			GScript.DoString("Game:OnPlay()");
	}
	EnableSave(false);
}

void DummyMessageHandler::OnReloadLevel()	
{
    //ReleaseParticleEditor();
    const char *curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();


    //TRACE("DummyMessageHandler::OnReloadLevel(): activating load screen\n", curLevelName);
    //GEngine->Video->PMenu->LoadScreen.Activate( true );
    
    TRACE("DummyMessageHandler::OnReloadLevel(): loading level '%s'\n", curLevelName);
    GScript.DoString("Game:LoadLevel('%s')", curLevelName);
    EnableSave(true);
    GEngine->ResetTimer();
}

void DummyMessageHandler::QueueHorribleWindowsMessage(int msg, WPARAM wParam, LPARAM lParam){

    //queue full!
    if (m_msgCount == QUEUE_SIZE)
        return;

    m_msgQueue[m_msgCount].msg = msg;
    m_msgQueue[m_msgCount].wparam = wParam;
    m_msgQueue[m_msgCount].lparam = lParam;
    m_msgCount++;
}

bool DummyMessageHandler::IsMessageWaiting(){
    return m_msgCount > 0;
}

void DummyMessageHandler::unqueueFirstMessage(){
    m_msgCount--;

    memcpy(m_msgQueue, &m_msgQueue[1], sizeof(Msg) * m_msgCount);    
}


void DummyMessageHandler::DispatchHorribleWindowsMessage(){
    WPARAM  w = m_msgQueue[0].wparam;
    LPARAM  l = m_msgQueue[0].lparam;
    int     msg = m_msgQueue[0].msg;


    unqueueFirstMessage();

    OnScriptMessage(msg, w, l);
}

int DummyMessageHandler::OnScriptMessage(int msg, WPARAM wParam, LPARAM lParam){
    switch (msg){
        case PKMSG_LOADLEVEL:
            if (wParam == 1){ // restart level
                TRACE("DummyMessageHandler::OnScriptMessage(): received a level load message!  Changing view mode to not render world.\n");

                GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;
                TRACE("DummyMessageHandler::OnScriptMessage(): changing game state to SingleGame\n");

                GEngine->SwitchToState(PCFSystem::SingleGame);

                TRACE("DummyMessageHandler::OnScriptMessage(): resetting world filename\n");
                GEngine->pcfWorld->FileName = "";

                TRACE("DummyMessageHandler::OnScriptMessage(): reloading level\n");
                OnReloadLevel();

                TRACE("DummyMessageHandler::OnScriptMessage(): starting play\n");
                OnPlay();

                TRACE("DummyMessageHandler::OnScriptMessage(): enabling physics\n");
                GScript.DoString("Game:SwitchPlayerToPhysics(true)");

                TRACE("DummyMessageHandler::OnScriptMessage(): resetting timer\n");
	            GEngine->ResetTimer();

                TRACE("DummyMessageHandler::OnScriptMessage(): done!\n");
            }
            break;

        case PKMSG_SETCONTROLPROFILE:
            TRACE("DummyMessageHandler::OnScriptMessage(): setting the current control profile to %d\n", wParam);
            GEngine->InputSys->setCurrentControlProfile((InputSystem::ControlProfile)wParam);
            GEngine->InputSys->LoadBindings();
            break;

        default:
            TRACE("DummyMessageHandler::OnScriptMessage(): ERROR-> unknown message: 0x%X\n", msg);
            break;
    }

    return 0;
}

*/
