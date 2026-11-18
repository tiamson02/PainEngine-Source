#include "Engine.h"
#include "Render/HUD.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#include "Render/View.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/DIInputSystem.h"

#include "Engine/PainMenu.h"
#include "Engine/MapSelect.h"
#include "Engine/MagicBoard.h"


InputSystem::InputSystem(HWND hwnd)
:   
#ifndef TRANSGAMING
    MessageHandler(hwnd), 
#endif
    ActionStatus(0), UIActionStatus(0), MouseStatus(0), EventQueueSize(0)
{
	ZeroBindings();

#ifdef TRANSGAMING
    ActiveKeyList.Expand(BUTTON_COUNT);


    for(int i=0; i< BUTTON_COUNT; ++i)
#else
    ActiveKeyList=0;


    for(int i=0; i<256; ++i)
#endif
    {
        KeyTable[i].Key=i;
        KeyTable[i].Status=EKeyNone;
    }

	InvertMouse = false;

	mFireSwitchKey = mFireSwitchToggleKey = mAltFireSwitchKey = mAltFireSwitchToggleKey = 0;
	mPauseKey = mAltPauseKey = 0;
	mScreenshotKey = mAltScreenshotKey = 0;
	mMenuKey = mAltMenuKey = 0;
	mScoreboardKey = mAltScoreboardKey = 0;
	mSayToAllKey = mAltSayToAllKey = mSayToTeamKey = mAltSayToTeamKey = 0;
	mQuickLoadKey = mAltQuickLoadKey = mQuickSaveKey = mAltQuickSaveKey = 0;
	mFlashlightKey = mAltFlashlightKey = 0;
	mZoomKey = mAltZoomKey = 0;

	mIsFireSwitched = false;

#ifdef TRANSGAMING
    mLuaToDoCount = 0;
    mLuaDeleteFirst = false;

    memset(mLuaToDo, 0, sizeof(mLuaToDo));
#endif

/*
    // default bindings: some traditional FPS movement setups
    KeyBindings['W']=EInputMoveForward;
    KeyBindings['S']=EInputMoveBackward;
    KeyBindings['A']=EInputMoveLeft;
    KeyBindings['D']=EInputMoveRight;
    KeyBindings[' ']=EInputJump;

    KeyBindings[VK_UP]      =EInputMoveForward;
    KeyBindings[VK_DOWN]    =EInputMoveBackward;
    KeyBindings[VK_LEFT]    =EInputMoveLeft;
    KeyBindings[VK_RIGHT]   =EInputMoveRight;
	KeyBindings[VK_NUMPAD0] = EInputJump;

    KeyBindings[EMouseKeyLeft]  = EInputFire;
    KeyBindings[EMouseKeyRight] = EInputAltFire;

	KeyBindings[EMouseWheelForward] = EInputNextWeapon;
	KeyBindings[EMouseWheelBack] = EInputPrevWeapon;
*/
	FillKeyNamesTable();

#ifndef TRANSGAMING
	mLuaToDo = "";
#endif
}

void InputSystem::BindKey(int action, int key, const char *cmd)
{
	KeyBindings[key].action = DWORD(action);

#ifndef TRANSGAMING
	KeyBindings[key].command = cmd;

    for( int i=0; i<18; i++ )
	{
		if( MessagesKeys[i] == key )
			MessagesKeys[i] = 0;
	}
#else
	KeyBindings[key].setCommand(cmd);
#endif

	ClearUIBinding( key );
}

#ifndef TRANSGAMING
void InputSystem::BindMessageKey( int msg, int key )
{
	for( int i=0; i<18; i++ )
	{
		if( MessagesKeys[i] == key )
			MessagesKeys[i] = 0;
	}

	MessagesKeys[msg] = key;
}
#endif

BOOL InputSystem::LoadBindings()
{
	ScriptObject cfg = GScript.Globals()["Cfg"];

	//String key = "";

	ZeroBindings();


    TRACE("InputSystem::LoadBindings(): loading primary bindings...\n");

    if( !cfg["KeyPrimaryMoveForward"].IsNil() )             BindKey( EInputMoveForward,         GetKeyByName( cfg["KeyPrimaryMoveForward"].GetString() ) );
    if( !cfg["KeyPrimaryMoveBackward"].IsNil() )            BindKey( EInputMoveBackward,        GetKeyByName( cfg["KeyPrimaryMoveBackward"].GetString() ) );
    if( !cfg["KeyPrimaryStrafeLeft"].IsNil() )              BindKey( EInputMoveLeft,            GetKeyByName( cfg["KeyPrimaryStrafeLeft"].GetString() ) );
    if( !cfg["KeyPrimaryStrafeRight"].IsNil() )             BindKey( EInputMoveRight,           GetKeyByName( cfg["KeyPrimaryStrafeRight"].GetString() ) );
    if( !cfg["KeyPrimaryJump"].IsNil() )                    BindKey( EInputJump,                GetKeyByName( cfg["KeyPrimaryJump"].GetString() ) );
    if( !cfg["KeyPrimaryFire"].IsNil() )                    BindKey( EInputFire,                GetKeyByName( cfg["KeyPrimaryFire"].GetString() ) );
    if( !cfg["KeyPrimaryAlternativeFire"].IsNil() )         BindKey( EInputAltFire,             GetKeyByName( cfg["KeyPrimaryAlternativeFire"].GetString() ) );
    if( !cfg["KeyPrimaryNextWeapon"].IsNil() )              BindKey( EInputNextWeapon,          GetKeyByName( cfg["KeyPrimaryNextWeapon"].GetString() ) );
    if( !cfg["KeyPrimaryPreviousWeapon"].IsNil() )          BindKey( EInputPrevWeapon,          GetKeyByName( cfg["KeyPrimaryPreviousWeapon"].GetString() ) );
#ifndef TRANSGAMING
    if( !cfg["KeyPrimaryWeapon1"].IsNil() )                 BindKey( EInputWeapon1,             GetKeyByName( cfg["KeyPrimaryWeapon1"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon2"].IsNil() )                 BindKey( EInputWeapon2,             GetKeyByName( cfg["KeyPrimaryWeapon2"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon3"].IsNil() )                 BindKey( EInputWeapon3,             GetKeyByName( cfg["KeyPrimaryWeapon3"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon4"].IsNil() )                 BindKey( EInputWeapon4,             GetKeyByName( cfg["KeyPrimaryWeapon4"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon5"].IsNil() )                 BindKey( EInputWeapon5,             GetKeyByName( cfg["KeyPrimaryWeapon5"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon6"].IsNil() )                 BindKey( EInputWeapon6,             GetKeyByName( cfg["KeyPrimaryWeapon6"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon7"].IsNil() )                 BindKey( EInputWeapon7,             GetKeyByName( cfg["KeyPrimaryWeapon7"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon8"].IsNil() )                 BindKey( EInputWeapon8,             GetKeyByName( cfg["KeyPrimaryWeapon8"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon9"].IsNil() )                 BindKey( EInputWeapon9,             GetKeyByName( cfg["KeyPrimaryWeapon9"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon10"].IsNil() )                BindKey( EInputWeapon10,            GetKeyByName( cfg["KeyPrimaryWeapon10"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon11"].IsNil() )                BindKey( EInputWeapon11,            GetKeyByName( cfg["KeyPrimaryWeapon11"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon12"].IsNil() )                BindKey( EInputWeapon12,            GetKeyByName( cfg["KeyPrimaryWeapon12"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon13"].IsNil() )                BindKey( EInputWeapon13,            GetKeyByName( cfg["KeyPrimaryWeapon13"].GetString() ) );
    if( !cfg["KeyPrimaryWeapon14"].IsNil() )                BindKey( EInputWeapon14,            GetKeyByName( cfg["KeyPrimaryWeapon14"].GetString() ) );
#endif
    if( !cfg["KeyPrimaryUseCards"].IsNil() )                BindKey( EInputUseCards,            GetKeyByName( cfg["KeyPrimaryUseCards"].GetString() ) );
#ifndef TRANSGAMING
    if( !cfg["KeyPrimaryRocketJump"].IsNil() )              BindKey( EInputRocketJump,          GetKeyByName( cfg["KeyPrimaryRocketJump"].GetString() ) );
    if( !cfg["KeyPrimaryForwardRocketJump"].IsNil() )       BindKey( EInputForwardRocketJump,   GetKeyByName( cfg["KeyPrimaryForwardRocketJump"].GetString() ) );
    if( !cfg["KeyPrimarySelectBestWeapon1"].IsNil() )       BindKey( EInputSelectBestWeapon1,   GetKeyByName( cfg["KeyPrimarySelectBestWeapon1"].GetString() ) );
    if( !cfg["KeyPrimarySelectBestWeapon2"].IsNil() )       BindKey( EInputSelectBestWeapon2,   GetKeyByName( cfg["KeyPrimarySelectBestWeapon2"].GetString() ) );
    if( !cfg["KeyPrimaryFireBestWeapon1"].IsNil() )         BindKey( EInputFireBestWeapon1,     GetKeyByName( cfg["KeyPrimaryFireBestWeapon1"].GetString() ) );
    if( !cfg["KeyPrimaryFireBestWeapon2"].IsNil() )         BindKey( EInputFireBestWeapon2,     GetKeyByName( cfg["KeyPrimaryFireBestWeapon2"].GetString() ) );
#endif


    TRACE("InputSystem::LoadBindings(): loading alternative bindings...\n");

    if( !cfg["KeyAlternativeMoveForward"].IsNil() )         BindKey( EInputMoveForward,         GetKeyByName( cfg["KeyAlternativeMoveForward"].GetString() ) );
    if( !cfg["KeyAlternativeMoveBackward"].IsNil() )        BindKey( EInputMoveBackward,        GetKeyByName( cfg["KeyAlternativeMoveBackward"].GetString() ) );
    if( !cfg["KeyAlternativeStrafeLeft"].IsNil() )          BindKey( EInputMoveLeft,            GetKeyByName( cfg["KeyAlternativeStrafeLeft"].GetString() ) );
    if( !cfg["KeyAlternativeStrafeRight"].IsNil() )         BindKey( EInputMoveRight,           GetKeyByName( cfg["KeyAlternativeStrafeRight"].GetString() ) );
    if( !cfg["KeyAlternativeJump"].IsNil() )                BindKey( EInputJump,                GetKeyByName( cfg["KeyAlternativeJump"].GetString() ) );
    if( !cfg["KeyAlternativeFire"].IsNil() )                BindKey( EInputFire,                GetKeyByName( cfg["KeyAlternativeFire"].GetString() ) );
    if( !cfg["KeyAlternativeAlternativeFire"].IsNil() )     BindKey( EInputAltFire,             GetKeyByName( cfg["KeyAlternativeAlternativeFire"].GetString() ) );
    if( !cfg["KeyAlternativeNextWeapon"].IsNil() )          BindKey( EInputNextWeapon,          GetKeyByName( cfg["KeyAlternativeNextWeapon"].GetString() ) );
    if( !cfg["KeyAlternativePreviousWeapon"].IsNil() )      BindKey( EInputPrevWeapon,          GetKeyByName( cfg["KeyAlternativePreviousWeapon"].GetString() ) );
#ifndef TRANSGAMING
    if( !cfg["KeyAlternativeWeapon1"].IsNil() )             BindKey( EInputWeapon1,             GetKeyByName( cfg["KeyAlternativeWeapon1"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon2"].IsNil() )             BindKey( EInputWeapon2,             GetKeyByName( cfg["KeyAlternativeWeapon2"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon3"].IsNil() )             BindKey( EInputWeapon3,             GetKeyByName( cfg["KeyAlternativeWeapon3"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon4"].IsNil() )             BindKey( EInputWeapon4,             GetKeyByName( cfg["KeyAlternativeWeapon4"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon5"].IsNil() )             BindKey( EInputWeapon5,             GetKeyByName( cfg["KeyAlternativeWeapon5"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon6"].IsNil() )             BindKey( EInputWeapon6,             GetKeyByName( cfg["KeyAlternativeWeapon6"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon7"].IsNil() )             BindKey( EInputWeapon7,             GetKeyByName( cfg["KeyAlternativeWeapon7"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon8"].IsNil() )             BindKey( EInputWeapon8,             GetKeyByName( cfg["KeyAlternativeWeapon8"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon9"].IsNil() )             BindKey( EInputWeapon9,             GetKeyByName( cfg["KeyAlternativeWeapon9"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon10"].IsNil() )            BindKey( EInputWeapon10,            GetKeyByName( cfg["KeyAlternativeWeapon10"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon11"].IsNil() )            BindKey( EInputWeapon11,            GetKeyByName( cfg["KeyAlternativeWeapon11"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon12"].IsNil() )            BindKey( EInputWeapon12,            GetKeyByName( cfg["KeyAlternativeWeapon12"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon13"].IsNil() )            BindKey( EInputWeapon13,            GetKeyByName( cfg["KeyAlternativeWeapon13"].GetString() ) );
    if( !cfg["KeyAlternativeWeapon14"].IsNil() )            BindKey( EInputWeapon14,            GetKeyByName( cfg["KeyAlternativeWeapon14"].GetString() ) );
#endif
    if( !cfg["KeyAlternativeUseCards"].IsNil() )            BindKey( EInputUseCards,            GetKeyByName( cfg["KeyAlternativeUseCards"].GetString() ) );
#ifndef TRANSGAMING
    if( !cfg["KeyAlternativeRocketJump"].IsNil() )          BindKey( EInputRocketJump,          GetKeyByName( cfg["KeyAlternativeRocketJump"].GetString() ) );
    if( !cfg["KeyAlternativeForwardRocketJump"].IsNil() )   BindKey( EInputForwardRocketJump,   GetKeyByName( cfg["KeyAlternativeForwardRocketJump"].GetString() ) );
    if( !cfg["KeyAlternativeSelectBestWeapon1"].IsNil() )   BindKey( EInputSelectBestWeapon1,   GetKeyByName( cfg["KeyAlternativeSelectBestWeapon1"].GetString() ) );
    if( !cfg["KeyAlternativeSelectBestWeapon2"].IsNil() )   BindKey( EInputSelectBestWeapon2,   GetKeyByName( cfg["KeyAlternativeSelectBestWeapon2"].GetString() ) );
    if( !cfg["KeyAlternativeFireBestWeapon1"].IsNil() )     BindKey( EInputFireBestWeapon1,     GetKeyByName( cfg["KeyAlternativeFireBestWeapon1"].GetString() ) );
    if( !cfg["KeyAlternativeFireBestWeapon2"].IsNil() )     BindKey( EInputFireBestWeapon2,     GetKeyByName( cfg["KeyAlternativeFireBestWeapon2"].GetString() ) );
#endif

    //load UI bindings
    TRACE("InputSystem::LoadBindings(): loading UI bindings...\n");

    if( !cfg["KeyPrimaryFireSwitch"].IsNil() )              mFireSwitchKey =            GetKeyByName( cfg["KeyPrimaryFireSwitch"].GetString() );
    if( !cfg["KeyPrimaryFireSwitchToggle"].IsNil() )        mFireSwitchToggleKey =      GetKeyByName( cfg["KeyPrimaryFireSwitchToggle"].GetString() );
    if( !cfg["KeyAlternativeFireSwitch"].IsNil() )          mAltFireSwitchKey =         GetKeyByName( cfg["KeyAlternativeFireSwitch"].GetString() );
    if( !cfg["KeyAlternativeFireSwitchToggle"].IsNil() )    mAltFireSwitchToggleKey =   GetKeyByName( cfg["KeyAlternativeFireSwitchToggle"].GetString() );

    if( !cfg["KeyPrimaryPause"].IsNil() )                   mPauseKey =                 GetKeyByName( cfg["KeyPrimaryPause"].GetString() );
    if( !cfg["KeyPrimaryScreenshot"].IsNil() )              mScreenshotKey =            GetKeyByName( cfg["KeyPrimaryScreenshot"].GetString() );
    if( !cfg["KeyPrimaryMenu"].IsNil() )                    mMenuKey =                  GetKeyByName( cfg["KeyPrimaryMenu"].GetString() );
    if( !cfg["KeyAlternativePause"].IsNil() )               mAltPauseKey =              GetKeyByName( cfg["KeyAlternativePause"].GetString() );
    if( !cfg["KeyAlternativeScreenshot"].IsNil() )          mAltScreenshotKey =         GetKeyByName( cfg["KeyAlternativeScreenshot"].GetString() );
    if( !cfg["KeyAlternativeMenu"].IsNil() )                mAltMenuKey =               GetKeyByName( cfg["KeyAlternativeMenu"].GetString() );

    if( !cfg["KeyPrimaryScoreboard"].IsNil() )              mScoreboardKey =            GetKeyByName( cfg["KeyPrimaryScoreboard"].GetString() );
    if( !cfg["KeyPrimarySayToAll"].IsNil() )                mSayToAllKey =              GetKeyByName( cfg["KeyPrimarySayToAll"].GetString() );
    if( !cfg["KeyPrimarySayToTeam"].IsNil() )               mSayToTeamKey =             GetKeyByName( cfg["KeyPrimarySayToTeam"].GetString() );
    if( !cfg["KeyAlternativeScoreboard"].IsNil() )          mAltScoreboardKey =         GetKeyByName( cfg["KeyAlternativeScoreboard"].GetString() );
    if( !cfg["KeyAlternativeSayToAll"].IsNil() )            mAltSayToAllKey =           GetKeyByName( cfg["KeyAlternativeSayToAll"].GetString() );
    if( !cfg["KeyAlternativeSayToTeam"].IsNil() )           mAltSayToTeamKey =          GetKeyByName( cfg["KeyAlternativeSayToTeam"].GetString() );

#ifndef TRANSGAMING
    if( !cfg["KeyPrimaryQuickLoad"].IsNil() )               mQuickLoadKey =             GetKeyByName( cfg["KeyPrimaryQuickLoad"].GetString() );
    if( !cfg["KeyPrimaryQuickSave"].IsNil() )               mQuickSaveKey =             GetKeyByName( cfg["KeyPrimaryQuickSave"].GetString() );
    if( !cfg["KeyAlternativeQuickLoad"].IsNil() )           mAltQuickLoadKey =          GetKeyByName( cfg["KeyAlternativeQuickLoad"].GetString() );
    if( !cfg["KeyAlternativeQuickSave"].IsNil() )           mAltQuickSaveKey =          GetKeyByName( cfg["KeyAlternativeQuickSave"].GetString() );
#endif

    if( !cfg["KeyPrimaryFlashlight"].IsNil() )              mFlashlightKey =            GetKeyByName( cfg["KeyPrimaryFlashlight"].GetString() );
    if( !cfg["KeyAlternativeFlashlight"].IsNil() )          mAltFlashlightKey =         GetKeyByName( cfg["KeyAlternativeFlashlight"].GetString() );

    if( !cfg["KeyPrimaryZoom"].IsNil() )                    mZoomKey =                  GetKeyByName( cfg["KeyPrimaryZoom"].GetString() );
    if( !cfg["KeyAlternativeZoom"].IsNil() )                mAltZoomKey =               GetKeyByName( cfg["KeyAlternativeZoom"].GetString() );

    // Invert Mouse
    if( !cfg["InvertMouse"].IsNil() )                       InvertMouse = cfg["InvertMouse"].GetBool();

#ifndef TRANSGAMING
    cfg = GScript.Globals()["Cfg"];

    ScriptObject keys = cfg["MessagesKeys"];
    if( !keys.IsNil() )
    {
        for( int i=1; i<19; i++ )
        {
    	    String keyName = keys[i].GetString();
    	    BindMessageKey( i - 1, GetKeyByName( keyName ) );
        }
    }
#endif

    TRACE("InputSystem::LoadBindings(): loading bound commands...\n");
#ifdef TRANSGAMING
    GScript.Call("Cfg_BindKeyCommands", 0, NULL);
#else
    GScript.DoString( "Cfg_BindKeyCommands()" );
#endif
    TRACE("InputSystem::LoadBindings(): done loading bindings\n");


    return TRUE;
}

BOOL InputSystem::SaveBindings()
{
    return TRUE;
}

void InputSystem::ZeroBindings()
{
#ifdef TRANSGAMING
    for( int i=0; i< BUTTON_COUNT; i++ ){
		KeyBindings[i].action = 0;
		KeyBindings[i].setCommand(NULL);
	}
#else
    for( int i=0; i<256; i++ ){
		KeyBindings[i].action = 0;
		KeyBindings[i].command = "";
	}

	GMemZero(MessagesKeys, sizeof(int)*18);
#endif

	mFireSwitchKey = mFireSwitchToggleKey = mAltFireSwitchKey = mAltFireSwitchToggleKey = 0;
}

#ifndef TRANSGAMING
void InputSystem::SetWindow( HWND hwnd )
{
	hWnd = hwnd;
}
#endif

#ifdef TRANSGAMING
void InputSystem::SendEvent(int eventType, int eventData){
    if (EventQueueSize < MaxEventQueueSize){
        //TRACE("InputSystem::SendEvent(): queueing input event %s with data '%s'\n", eventType == EEventKeyDown ? "{Key Down}" : (eventType == EEventKeyUp ? "{Key Up}" : "{Unknown Event}"), KeyNames[eventData].AsChar());

        EventQueue[EventQueueSize].EventType = eventType;
        EventQueue[EventQueueSize].EventData = eventData;

        EventQueueSize++;
    }
}
#else
void InputSystem::SendEvent(const InputEvent& event)
{
    if(EventQueueSize<MaxEventQueueSize)
    {
        EventQueue[EventQueueSize++]=event;
    }
}
#endif

void InputSystem::ProcessEvents()
{
#ifndef TRANSGAMING
	if( GetForegroundWindow() != GEngine->Video->GetActiveView()->GetTopmostWindow() )
		return;
#endif

    for( int i = 0; i < ActiveKeyList.Size(); ++i )
    {
		KeyInfo& key = KeyTable[ActiveKeyList[i]];

		// automatically free mouse wheel keys after one tick ( script logic will execute once )
#ifndef TRANSGAMING
		if( key.Key == EMouseWheelForward )
		{
			ActionStatus &= ~(1<<KeyBindings[EMouseWheelForward].action);
            key.Status=EKeyNone;
			ActiveKeyList.FastRemove(i);
			i--;
			
#ifndef TRANSGAMING
            DWORD a = KeyBindings[EMouseWheelForward].action;

			if( a >= EInputWeapon6 && a <= EInputWeapon10 )
			{
				ActionStatus &= ~( 1 << ( EInputWeapon1 + a - EInputWeapon6 ) );
			}
#endif
		}
		else if( key.Key == EMouseWheelBack )
		{
	        ActionStatus &= ~(1<<KeyBindings[EMouseWheelBack].action);
            key.Status=EKeyNone;
			ActiveKeyList.FastRemove(i);
			i--;

#ifndef TRANSGAMING
			DWORD a = KeyBindings[EMouseWheelBack].action;
			if( a >= EInputWeapon6 && a <= EInputWeapon10 )
			{
				ActionStatus &= ~( 1 << ( EInputWeapon1 + a - EInputWeapon6 ) );
			}
#endif
		}
		else
#endif
        if(key.Status==EKeyPressed)
            key.Status=EKeyDown;
        else
        if(key.Status==EKeyUp)
        {
            key.Status=EKeyNone;
			ActiveKeyList.FastRemove(i);
			i--;
        }
    }

    if(EventQueueSize>0) 
    {
        for(int i=0; i<EventQueueSize; ++i) 
        {
            const int data=EventQueue[i].EventData;
			bool send = false;

            switch(EventQueue[i].EventType) 
            {
            case EEventKeyDown:
                if(KeyTable[data].Status==EKeyNone)
                {
					// Krystian: link into menu system
					// if menu does not override the key, send it into game. Shift always gets send.
#ifdef TRANSGAMING
                    if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyDown, data ) ) &&
						( !GEngine->Video->MapSel->SendEvent( EMenuKeyDown, data ) ) &&
						( !GEngine->Video->MBoard->SendEvent( EMenuKeyDown, data ) ) ) )
#else
                    if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyDown, data ) ) &&
						( !GEngine->Video->MapSel->SendEvent( EMenuKeyDown, data ) ) &&
						( !GEngine->Video->MBoard->SendEvent( EMenuKeyDown, data ) ) &&
						( !GEngine->Video->Hud->mConsole.SendEvent( EKeyPressed, data ) ) ) )
#endif
					{
						send = true;
					}

					if( ( data == VK_SHIFT ) || ( data == VK_LSHIFT ) || ( data == VK_RSHIFT )
						|| ( data == VK_CONTROL ) || ( data == VK_LCONTROL ) || ( data == VK_RCONTROL ) )
						send = true;

					if( send )//&& !GEngine->Video->Hud->mConsole.Active() )
					{
	                    // key pressed is only set first time you press the key
	                    KeyTable[data].Status=EKeyPressed;
						ActiveKeyList.Add( data );
					}
                }
                else
				{
					// Krystian: link into menu system
					// if menu does not override the key, send it into game. Shift always gets send.
#ifdef TRANSGAMING
                    if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyDownRepeated, data ) ) &&
						( !GEngine->Video->MapSel->SendEvent( EMenuKeyDownRepeated, data ) ) &&
						( !GEngine->Video->MBoard->SendEvent( EMenuKeyDownRepeated, data ) ) ) )
#else
                    if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyDownRepeated, data ) ) &&
						( !GEngine->Video->MapSel->SendEvent( EMenuKeyDownRepeated, data ) ) &&
						( !GEngine->Video->MBoard->SendEvent( EMenuKeyDownRepeated, data ) ) &&
						( !GEngine->Video->Hud->mConsole.SendEvent( EKeyDown, data ) ) ) )
#endif
					{
						send = true;
					}

					if( ( data == VK_SHIFT ) || ( data == VK_LSHIFT ) || ( data == VK_RSHIFT )
						|| ( data == VK_CONTROL ) || ( data == VK_LCONTROL ) || ( data == VK_RCONTROL ) )
						send = true;

					if( send )//&& !GEngine->Video->Hud->mConsole.Active() )
					{
	                    KeyTable[data].Status=EKeyDown;
					}
				}
                
                break;
            case EEventKeyUp:
                if(KeyTable[data].Status==EKeyNone)
					ActiveKeyList.Add( data );

				// Link into menu system        
#ifdef TRANSGAMING
                if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyUp, data ) ) &&
					( !GEngine->Video->MapSel->SendEvent( EMenuKeyUp, data ) ) &&
					( !GEngine->Video->MBoard->SendEvent( EMenuKeyUp, data ) ) ) )
#else
                if( !GEngine->Video || ( ( !GEngine->Video->PMenu->SendEvent( EMenuKeyUp, data ) ) &&
					( !GEngine->Video->MapSel->SendEvent( EMenuKeyUp, data ) ) &&
					( !GEngine->Video->MBoard->SendEvent( EMenuKeyUp, data ) ) &&
					( !GEngine->Video->Hud->mConsole.SendEvent( EKeyUp, data ) ) ) )
#endif
				{
					send = true;
				}

				if( ( data == VK_SHIFT ) || ( data == VK_LSHIFT ) || ( data == VK_RSHIFT )
					|| ( data == VK_CONTROL ) || ( data == VK_LCONTROL ) || ( data == VK_RCONTROL ) )
					send = true;

				if( send )//&& !GEngine->Video->Hud->mConsole.Active() )
				{
	                KeyTable[data].Status=EKeyUp;
				}
				else
				{
					KeyTable[data].Status=EKeyNone;
					for( int i = 0; i < ActiveKeyList.Size(); ++i )
						if( ActiveKeyList[i] == data )
						{
							ActiveKeyList.FastRemove(i);
							break;
						}
				}

                break;
            }
        }

        // clear queue
        EventQueueSize=0;
    }

#ifdef TRANSGAMING
	if (mLuaToDoCount){
        TRACE("InputSystem::ProcessEvents(): executing %d queued lua commands\n", mLuaToDoCount);
        GScript.DoStrings(mLuaToDoCount, mLuaToDo);
        mLuaToDoCount = 0;

        if (mLuaDeleteFirst){
            char *tmp = const_cast<char *>(mLuaToDo[0]);


            delete []tmp;

            mLuaDeleteFirst = false;
        }
    }
#else
    if (!mLuaToDo.IsNull()){
		String temp = mLuaToDo;
		mLuaToDo = "";
		GScript.DoString( temp.AsChar() );
    }
#endif
	
}

void InputSystem::Reset()
{
    for( int i = 0; i < ActiveKeyList.Size(); ++i )
		KeyTable[ActiveKeyList[i]].Status = EKeyNone;
    ActiveKeyList.ClearArray();;

    EventQueueSize=0;

	ActionStatus = 0;
	UIActionStatus = 0;
	MouseStatus = 0;
	mIsFireSwitched = false;
}

#ifdef TRANSGAMING
int InputSystem::GetKeyByName( const char *name ){
	for( int i = 0; i < BUTTON_COUNT; ++i )
		if( stricmp( KeyNamesEng[i].AsChar(), name) == 0 )
			return i;
	return 0;
}

int InputSystem::GetKeyByShortName( const char *name ){
	for( int i = 0; i < BUTTON_COUNT; ++i )
		if( stricmp(KeyShortNames[i].AsChar(), name) == 0 )
			return i;
	return 0;
}

void InputSystem::SetLuaToDo(const char *todo, bool copy){ 
    //make sure there's a string to enqueue
    if (todo == NULL || todo[0] == 0)
        return;


    TRACE("InputSystem::SetLuaToDo(): enqueuing the command '%s'\n", todo);
    mLuaToDoCount = 1;
    char *  tmp;


    if (mLuaDeleteFirst && mLuaToDo[0]){
        tmp = const_cast<char *>(mLuaToDo[0]);

        delete []tmp;
    }


    if (copy){
        size_t  len = strlen(todo);


        tmp = new char[len + 1];

        strncpy(tmp, todo, len + 1);

        mLuaToDo[0] = tmp;
    }

    else
        mLuaToDo[0] = todo;


    mLuaDeleteFirst = copy;
}

void InputSystem::QueueLuaToDo(const char *todo){
    //make sure there's a string to enqueue
    if (todo == NULL || todo[0] == 0)
        return;


    //make sure this string will not be deleted
    if (mLuaToDoCount == 0)
        mLuaDeleteFirst = false;


    mLuaToDo[mLuaToDoCount++] = todo;
}
#else
int InputSystem::GetKeyByName( String name )
{
	for( int i = 0; i < 256; ++i )
		if( stricmp( KeyNamesEng[i].AsChar(), name.AsChar() ) == 0 )
			return i;
	return 0;
}

int InputSystem::GetKeyByShortName( String name )
{
	for( int i = 0; i < 256; ++i )
		if( stricmp( KeyShortNames[i].AsChar(), name.AsChar() ) == 0 )
			return i;
	return 0;
}

#endif

void InputSystem::ClearUIBinding( unsigned int key )
{
	if( key == 0 ) return;

	if( mFireSwitchKey == key ) mFireSwitchKey = 0;
	else if( mFireSwitchToggleKey == key ) mFireSwitchToggleKey = 0;
	else if( mAltFireSwitchKey == key ) mAltFireSwitchKey = 0;
	else if( mAltFireSwitchToggleKey == key ) mAltFireSwitchToggleKey = 0;
	else if( mPauseKey == key ) mPauseKey = 0;
	else if( mScreenshotKey == key ) mScreenshotKey = 0;
	else if( mMenuKey == key ) mMenuKey = 0;
	else if( mAltPauseKey == key ) mAltPauseKey = 0;
	else if( mAltScreenshotKey == key ) mAltScreenshotKey = 0;
	else if( mAltMenuKey == key ) mAltMenuKey = 0;
	else if( mScoreboardKey == key ) mScoreboardKey = 0;
	else if( mAltScoreboardKey == key ) mAltScoreboardKey = 0;
	else if( mSayToAllKey == key ) mSayToAllKey = 0;
	else if( mAltSayToAllKey == key ) mAltSayToAllKey = 0;
	else if( mSayToTeamKey == key ) mSayToTeamKey = 0;
	else if( mAltSayToTeamKey == key ) mAltSayToTeamKey = 0;
	else if( mQuickLoadKey == key ) mQuickLoadKey = 0;
	else if( mAltQuickLoadKey == key ) mAltQuickLoadKey = 0;
	else if( mQuickSaveKey == key ) mQuickSaveKey = 0;
	else if( mAltQuickSaveKey == key ) mAltQuickSaveKey = 0;
	else if( mFlashlightKey == key ) mFlashlightKey = 0;
	else if( mAltFlashlightKey == key ) mAltFlashlightKey = 0;
	else if( mZoomKey == key ) mZoomKey = 0;
	else if( mAltZoomKey == key ) mAltZoomKey = 0;
}

#ifndef TRANSGAMING
void InputSystem::ClearMessageBinding( int key )
{
	if( key <= 0 ) return;
	for( int i=0; i<18; i++ )
	{
		if( MessagesKeys[i] == key )
			MessagesKeys[i] = 0;
	}
}
#endif
