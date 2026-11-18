#include "Engine.h"
#include "PainGame_PSP.h"
//#include "EditorView.h"
#include "Render/Video.h"
#include "Net/NetworkDevice2.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/InputSystem.h"

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Net/DemoRecording2.h"
#include "../Render/View.h"
#include "../Engine/World.h"
#include "../Physics/PhysicsEngine.h"


void PainGame::ProcessInput()
{
}


//********************************* DummyMessageHandler Implementation ************************************//
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
        TRACE("DummyMessageHandler::OnPlay(): ticking physics (GEngine->Physics = %p)\n", GEngine->Physics);
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
/*#ifdef DEBUG
    const char *curTexName =   GScript.Globals()["Lev"]["TexName"].GetString();
    const char *curLevelDesc = GScript.Globals()["Lev"]["LevelName"].GetString();

    
    TRACE("DummyMessageHandler::OnReloadLevel(): loading level '%s' (textureFolder = '%s', levelName = '%s')\n", curLevelName, curTexName, curLevelDesc);
#endif//*/

    //TRACE("DummyMessageHandler::OnReloadLevel(): activating load screen\n", curLevelName);
    //GEngine->Video->PMenu->LoadScreen.Activate( true );

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

//**************************************** Main Message Handler Function ***********************************************//

int DummyMessageHandler::OnScriptMessage(int msg, WPARAM wParam, LPARAM lParam){
    switch (msg){
        case PKMSG_LOADLEVEL:
            if (wParam == 1)
            { // restart level
                TRACE("DummyMessageHandler::OnScriptMessage(): received a level load message!  Changing view mode to not render world.\n");

                m_SetInGame(MEM_LEVEL);	//+DDP - set mmgr to ingame

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

        case PKMSG_LOADNEXTLEVELETTE:
            TRACE("DummyMessageHandler::OnScriptMessage(): loading the next queued levelette\n");
            GScript.DoString("Game:LoadNextLevelette()");
            break;

        default:
            TRACE("DummyMessageHandler::OnScriptMessage(): ERROR-> unknown message: 0x%X\n", msg);
            break;
    }

    return 0;
}
