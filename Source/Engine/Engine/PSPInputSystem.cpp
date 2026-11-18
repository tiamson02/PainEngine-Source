#include "Engine.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/HUD.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "PSPInputSystem.h"

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"

#include "Net/DemoRecording2.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#ifdef TRANSGAMING_PSP
# include <tg_pc2psp.h>
#elif defined(TRANSGAMING_PS2)
# ifndef TRANSGAMING_USE_PHYSFS
#  include <shellCore.h>
#  include <shellFile.h>
# endif
# include <tg_pc2ps2.h>
# include <libpad.h>
# include <malloc.h>
#endif



#define     DIDELTA_SCALE	    3.2f
#ifdef TRANSGAMING_PSP
# define    GAMEPAD_DEADZONE	0.5f
#else
# define GAMEPAD_DEADZONE	32
#endif
#define     GAMEPAD_SENSITIVITY 40.0f    //currently not used by the tgPad library, but used by us after acquiring the pad state
#define     MENU_MOUSESCALE     0.75f
#define     KEY_REPEAT          0.06f
#define     KEY_REPEAT_DELAY    0.5f


//comment this line out to disable the PS2 input initialization and gathering system
#define USE_PS2_INPUT
//comment this out when the physics system goes in
//#define USE_HACKED_PS2_INPUT

PSPInputSystem::PSPInputSystem( HWND hwnd )
:	InputSystem( hwnd ), mMouseSensitivity( 25.0f ), mMouseLocked( false ), 
		mMouseVisible( true ){

#ifdef TRANSGAMING_PS2
    padType = SHELLPAD_DISCONNECT;
    padDmaBuf = memalign(64, scePadDmaBufferMax * 16);
#endif

	mMousePos.x = 0;
	mMousePos.y = 0;

    mCurrentProfile = PAD_PROFILE_NONE;
    mMovementScale = 0;
    mStrafeScale = 0;

	mMouseDelta = Vector( 0.0f, 0.0f, 0.0f );
	//mLookDelta = Vector( 0.0f, 0.0f, 0.0f );
	//mMoveDelta = Vector( 0.0f, 0.0f, 0.0f );

    mPressedCount = 0;
    mReleasedCount = 0;

    mLastKey = 0;
    mStartTime = 0.0f;
    mLastTime = 0.0f;

	ScriptObject cfg = GScript.Globals()["Cfg"];
	if (!cfg.IsNil())
		mMouseSmooth = cfg["SmoothMouse"].GetBool();

	else
		mMouseSmooth = false;


	FillKeyNamesTable();

    InitInput();
}


PSPInputSystem::~PSPInputSystem(){
	Release();

#ifdef TRANSGAMING_PS2
    if (padDmaBuf)
        free(padDmaBuf);
#endif
}


void PSPInputSystem::SetUseDInput(bool use){}
void PSPInputSystem::UpdateMousePosition(){}

#ifdef TRANSGAMING
void PSPInputSystem::setCurrentControlProfile(ControlProfile cp){
    switch (cp){
        case PAD_PROFILE_1:
        case PAD_PROFILE_2:
# ifdef TRANSGAMING_PSP
        case PAD_PROFILE_3:
        case PAD_PROFILE_4:
#endif
            mCurrentProfile = cp;
            TRACE("PSPInputSystem::setCurrentControlProfile(): setting current controller profile to %d\n", cp);
            break;

        case PAD_PROFILE_NONE:
        default:
            TRACE("PSPInputSystem::setCurrentControlProfile(): ERROR-> invalid control profile: %d\n", cp);
            break;
    }
}

InputSystem::ControlProfile PSPInputSystem::getCurrentControlProfile() const{
    return mCurrentProfile;
}

float PSPInputSystem::getMovementScale() const{
    return mMovementScale;
}

float PSPInputSystem::getStrafeScale() const{
    return mStrafeScale;
}
#endif


void PSPInputSystem::Tick(){
    bool menuShowing = GEngine->Video->PMenu->Active() || GEngine->Video->MBoard->Active() || GEngine->Video->MapSel->Active();


#ifdef TRANSGAMING_PSP
    //read the PSP gamepad state here
    tgPadRead(&pad);
#elif defined(TRANSGAMING_PS2)
# ifdef USE_PS2_INPUT
    //read the PS2 gamepad state here
    padType = shellPadRead(&pad);
# endif

    //make sure there's input to process
    if (padType == SHELLPAD_DISCONNECT)
        return;
#endif


    GamePadTick(menuShowing);
    MouseTick(menuShowing);

    
	// Clear all UI actions but scoreboard
	UIActionStatus &=    ( 1<<EInputUIScoreboard );
	ActionStatus &=     ~( 1<<EInputNextWeapon );
	ActionStatus &=     ~( 1<<EInputPrevWeapon );

    //check on the state of all bindings except the weapon firing ones for all button presses
	for (int i = 0; i < mPressedCount; i++){
        if( mPressed[i] <= 0 ) 
            continue;


        //update the mouse button state
        if (mPressed[i] == EMouseKeyLeft){
            //TRACE("PSPInputSystem::Tick(): pushed left mouse button!\n");
            MouseStatus |= ( 1 << EMouseLeftButton );
        }

        if (mPressed[i] == EMouseKeyRight){
            //TRACE("PSPInputSystem::Tick(): pushed right mouse button!\n");
            MouseStatus |= ( 1 << EMouseRightButton );
        }

        if (mPressed[i] == EMouseKeyMiddle){
            //TRACE("PSPInputSystem::Tick(): pushed middle mouse button!\n");
            MouseStatus |= ( 1 << EMouseMiddleButton );
        }


#ifndef TRANSGAMING
        if (gDemoRec && gDemoRec->IsPlaying()){
            SendEvent(EEventKeyDown, mPressed[i]);

            continue;
        }
#endif


        if (!menuShowing){
            bool isUIKey = BindUIKey(mPressed[i]);
            
            if (!isUIKey){
                if (KeyBindings[mPressed[i]].action != EInputFire && KeyBindings[mPressed[i]].action != EInputAltFire){
                    DWORD a = KeyBindings[mPressed[i]].action;


                    if(a){
                        //TRACE("PSPInputSystem::Tick(): firing binding %d from button '%s'!\n", a, KeyNames[mPressed[i]].AsChar());

                        ActionStatus |= ( 1 << a );
                    }
                    
                    else if (KeyBindings[mPressed[i]].command){
#ifdef DEBUG
                        if (mLuaToDoCount >= LUA_TODO_MAX)
                            TRACE("PSPInputSystem::Tick(): ERROR-> command buffer overflow!  Ignoring the command '%s'\n", KeyBindings[mPressed[i]].command);
                        else
#endif
                        {
                            TRACE("PSPInputSystem::Tick(): enqueuing the command '%s' for action %d (key = %d)\n", KeyBindings[mPressed[i]].action, mPressed[i]);
                            QueueLuaToDo(KeyBindings[mPressed[i]].command);
                        }
                    }
                }
            }
        }
        SendEvent(EEventKeyDown, mPressed[i]);
	}

	mPressedCount = 0;


    //clear firing binding states
    ActionStatus &= ~( 1 << EInputFire );
    ActionStatus &= ~( 1 << EInputAltFire );

    //check on the state of the weapon firing bindings
    if (!menuShowing){
        for (int i = 0; i < BUTTON_COUNT; i++){

            //check the primary fire binding
            if (KeyBindings[i].action == EInputFire){
                if (GetKeyStatus(i) == EKeyPressed || GetKeyStatus(i) == EKeyDown){
                    if (mIsFireSwitched){
                        //TRACE("PSPInputSystem::Tick(): switched fire!  Firing alt-fire binding\n");

	                    ActionStatus |= ( 1 << EInputAltFire );
                    }
                    else{
                        //TRACE("PSPInputSystem::Tick(): unswitched fire!  Firing normal-fire binding\n");

	                    ActionStatus |= ( 1 << EInputFire );
                    }
	            }
	        }

            //check the alternate fire binding
	        else if (KeyBindings[i].action == EInputAltFire){
	            if (GetKeyStatus(i) == EKeyPressed || GetKeyStatus(i) == EKeyDown){
                    if (mIsFireSwitched){
                        //TRACE("PSPInputSystem::Tick(): switched fire!  Firing normal-fire binding\n");

	                    ActionStatus |= ( 1 << EInputFire );
                    }
                    else{
                        //TRACE("PSPInputSystem::Tick(): unswitched fire!  Firing alt-fire binding\n");

	                    ActionStatus |= ( 1 << EInputAltFire );
                    }
	            }
	        }
	    }
	}

	ProcessEvents();
	//UpdateMousePosition();        //<--- this function is dummied out


    //check on all the bindings for button releases
    for (int i = 0; i < mReleasedCount; i++){
        if (mReleased[i] <= 0)
            continue;


        if (mReleased[i] == EMouseKeyLeft ){
            //TRACE("PSPInputSystem::Tick(): released left mouse button!\n");
            MouseStatus &= ~( 1 << EMouseLeftButton );
        }

        if (mReleased[i] == EMouseKeyRight){
            //TRACE("PSPInputSystem::Tick(): released right mouse button!\n");
            MouseStatus &= ~( 1 << EMouseRightButton );
        }

        if (mReleased[i] == EMouseKeyMiddle){
            //TRACE("PSPInputSystem::Tick(): released middle mouse button!\n");
            MouseStatus &= ~( 1 << EMouseMiddleButton );
        }


#ifndef TRANSGAMING
        if(gDemoRec && gDemoRec->IsPlaying()){
            SendEvent(EEventKeyUp, mReleased[i]);

            continue;
        }
#endif

        if (!menuShowing){
            bool isUIKey = UnbindUIKey(mReleased[i]);

            if (!isUIKey){

                //check all but the release of the firing binding buttons
                if (KeyBindings[mReleased[i]].action != EInputFire && KeyBindings[mReleased[i]].action != EInputAltFire){
                    DWORD a = KeyBindings[mReleased[i]].action;

                    //TRACE("PSPInputSystem::Tick(): clearing binding %d from key '%s'\n", a, KeyNames[mReleased[i]].AsChar());
                    ActionStatus &= ~( 1 << a );
                }
            }
        }
        
        SendEvent(EEventKeyUp, mReleased[i]);
    }

    mReleasedCount = 0;
}


void PSPInputSystem::Reset(){
	InputSystem::Reset();

    mMouseDelta = Vector(0.0f, 0.0f, 0.0f);

	mPressedCount = 0;
	mReleasedCount = 0;

    mLastKey = 0;
    mStartTime = 0.0f;
    mLastTime = 0.0f;
}


Vector PSPInputSystem::GetMouseDelta() const{
    return mMouseDelta;
}


void PSPInputSystem::LockMouse(BOOL lock){
    if (lock && (GEngine->Video->MBoard->Active() || GEngine->Video->MapSel->Active()))
		return;

    mMouseLocked = lock;
}


void PSPInputSystem::ShowMouse(BOOL show){
    mMouseVisible = show;
}


/*void PSPInputSystem::SetWindow(HWND hwnd){
	InputSystem::SetWindow( hwnd );
}//*/


void PSPInputSystem::SetMousePosition(int x, int y){
    mMousePos.x = x;
    mMousePos.y = y;
}


void PSPInputSystem::SetMouseSensitivity(float msens){
	mMouseSensitivity = msens;
}

//********************************* Button and Axis Testing Macros *********************************//
#define ADDBUTTONPRESS(button)      { mPressed[mPressedCount++] = button; mLastKey = button; mStartTime = 0.0f; mLastTime = 0.0f; }
#define ADDBUTTONRELEASE(button)    { mReleased[mReleasedCount++] = button; mLastKey = 0; mStartTime = 0.0f; mLastTime = 0.0f; }
#ifdef TRANSGAMING_PSP
# define TESTBUTTON(testButton, setButton) {\
            if (pad.pressedBtn & (testButton)){ \
                ADDBUTTONPRESS(setButton); \
            } \
            else if (pad.releasedBtn & (testButton)){ \
                ADDBUTTONRELEASE(setButton); \
            } \
        }
#elif defined(TRANSGAMING_PS2)
# define TESTBUTTON(testButton, setButton) {\
            if ((pad.button & (testButton)) && !(pad.previousBtn & (testButton))){ \
                ADDBUTTONPRESS(setButton); \
            } \
            else if (!(pad.button & (testButton)) && (pad.previousBtn & (testButton))){ \
                ADDBUTTONRELEASE(setButton); \
            } \
        }
#endif

//macro to grab a joystick axis' current value, compensating for the deadzone setting.
#define GETSTICKVALUE(buf, stick, scale) {\
            if (pad.stick < 0)\
                buf = ((pad.stick + GAMEPAD_DEADZONE) * (scale)) * GAMEPAD_SENSITIVITY;\
            else if (pad.stick == 0)\
                buf = 0;\
            else\
                buf = ((pad.stick - GAMEPAD_DEADZONE) * (scale)) * GAMEPAD_SENSITIVITY;\
        }

//macro to grab the digital state of a button 'axis' as a float scale (0.0 or +-1.0).
//  NOTE: pushing both 'axis' buttons simultaneously sets an idle state.
#define GETBUTTONVALUE(buf, positive, negative) \
        if ((pad.button & ((positive) | (negative)))  == ((positive) | (negative))) \
            buf = 0.0f; \
        else if (pad.button & (positive)) \
            buf = GAMEPAD_SENSITIVITY; \
        else if (pad.button & (negative)) \
            buf = -GAMEPAD_SENSITIVITY; \
        else \
            buf = 0.0f;

#define GETBUTTONSCALE(var, positive, negative) \
        if ((pad.button & ((positive) | (negative))) == ((positive) | (negative))) \
            var = 0.0f; \
        else if (pad.button & ((positive) | (negative))) \
            var = 1.0f; \
        else \
            var = 0.0f;


void PSPInputSystem::testAxis(float current, float previous, const PSButtonNames names[2]){
    if (current > 0){
        if (previous <= 0){
            if (previous != 0){
                ADDBUTTONRELEASE(names[1]);
                //TRACE("PSPInputSystem::testAxis(): releasing negative Y\n");
            }
            ADDBUTTONPRESS(names[0]);
            //TRACE("PSPInputSystem::testAxis(): pressing positive Y\n");
        }
    }
    else if (current < 0){
        if (previous >= 0){
            if (previous != 0){
                ADDBUTTONRELEASE(names[0]);
                //TRACE("PSPInputSystem::testAxis(): releasing positive Y\n");
            }
            ADDBUTTONPRESS(names[1]);
            //TRACE("PSPInputSystem::testAxis(): pressing negative Y\n");
        }
    }
    else if (/* current == 0 && */ previous != 0){
        if (previous > 0){
            ADDBUTTONRELEASE(names[0]);
            //TRACE("PSPInputSystem::testAxis(): releasing positive Y (idling)\n");
        }
        else{
            ADDBUTTONRELEASE(names[1]);
            //TRACE("PSPInputSystem::testAxis(): releasing negative Y (idling)\n");
        }
    }
}
//************************************************************************************************//

void PSPInputSystem::MouseTick(bool menuShowing){
    View *              view = GEngine->Video->GetActiveView();
    float               mdelta[2] = {0.0f, 0.0f};
    float               stickPos[2] = {0.0f, 0.0f};
    //float               buttonPos[2] = {0.0f, 0.0f};
#ifdef TRANSGAMING_PSP
    static const float  scale = 1.0f / (1.0f - GAMEPAD_DEADZONE);
#else
    static const float  scale = 1.0f / (127.0f - GAMEPAD_DEADZONE);
#endif
    //int               sign[2] = {((int)(pad.slideX * 5)) & 0x7fffffff
    const float         aspect = view->LocalViewport.Height / (float)view->LocalViewport.Width;
    const PSButtonNames leftStick[2][2] =  {{BUTTON_POS_LX, BUTTON_NEG_LX}, {BUTTON_POS_LY, BUTTON_NEG_LY}};
#ifdef TRANSGAMING_PS2
    const PSButtonNames rightStick[2][2] = {{BUTTON_POS_RX, BUTTON_NEG_RX}, {BUTTON_POS_RY, BUTTON_NEG_RY}};
#endif


#ifdef TRANSGAMING_PSP
    //!!// handle updating the mouse according to the current controller profile
    if (menuShowing){
        GETSTICKVALUE(mdelta[0], stickX, scale)
        GETSTICKVALUE(mdelta[1], stickY, scale)

        mMovementScale =    0.0f;
        mStrafeScale =      0.0f;
    }

    else{
        switch (mCurrentProfile){
            case PAD_PROFILE_1:
                GETSTICKVALUE(stickPos[0],  stickX,     scale);
                GETSTICKVALUE(stickPos[1],  stickY,     scale);
                GETSTICKVALUE(mdelta[0],    oldStickX,  scale);
                GETSTICKVALUE(mdelta[1],    oldStickY,  scale);
                GETBUTTONSCALE(mStrafeScale, TG_PAD_SQUARE, TG_PAD_CIRCLE);

                testAxis(stickPos[0], mdelta[0], leftStick[0]);
                testAxis(stickPos[1], mdelta[1], leftStick[1]);


                GETBUTTONVALUE(mdelta[0], TG_PAD_CIRCLE,    TG_PAD_SQUARE);
                GETBUTTONVALUE(mdelta[1], TG_PAD_CROSS,     TG_PAD_TRIANGLE);

                mMovementScale = fabsf(stickPos[1] / GAMEPAD_SENSITIVITY);
                mStrafeScale =   fabsf(stickPos[0] / GAMEPAD_SENSITIVITY);
                break;

            case PAD_PROFILE_2:
                GETSTICKVALUE(stickPos[0],  stickX,     scale);
                GETSTICKVALUE(stickPos[1],  oldStickX,  scale);

                testAxis(stickPos[0], stickPos[1], leftStick[0]);


                GETBUTTONVALUE(mdelta[0], TG_PAD_SQUARE, TG_PAD_CIRCLE);
                GETSTICKVALUE(mdelta[1], stickY, scale);

                GETBUTTONSCALE(mMovementScale,  TG_PAD_TRIANGLE,    TG_PAD_CROSS);
                mStrafeScale =   fabsf(stickPos[0] / GAMEPAD_SENSITIVITY);
                break;

            case PAD_PROFILE_3:
                GETSTICKVALUE(mdelta[0], stickX, scale);
                GETSTICKVALUE(mdelta[1], stickY, scale);

                GETBUTTONSCALE(mMovementScale,  TG_PAD_TRIANGLE,    TG_PAD_CROSS);
                GETBUTTONSCALE(mStrafeScale,    TG_PAD_SQUARE,      TG_PAD_CIRCLE);
                //TRACE("PSPInputSystem::MouseTick(): button scales = {strafe = %g, move = %g}\n", mStrafeScale, mMovementScale);
                break;

            case PAD_PROFILE_4:
                GETSTICKVALUE(stickPos[1], stickY, scale);
                GETSTICKVALUE(stickPos[0], oldStickY, scale);

                testAxis(stickPos[1], stickPos[0], leftStick[1]);


                GETSTICKVALUE(mdelta[0], stickX, scale);
                GETBUTTONVALUE(mdelta[1], TG_PAD_CROSS, TG_PAD_TRIANGLE);

                GETBUTTONSCALE(mStrafeScale, TG_PAD_SQUARE, TG_PAD_CIRCLE);
                mMovementScale = fabsf(stickPos[1] / GAMEPAD_SENSITIVITY);

                //TRACE("PSPInputSystem::MouseTick(): button scales = {strafe = %g, move = %g}, rawY = %g, oldY = %g, mdelta = <%g, %g>\n", mStrafeScale, mMovementScale, stickPos[1], stickPos[0], mdelta[0], mdelta[1]);

                break;
        }

    }

#elif defined(TRANSGAMING_PS2)
    // handle reading both PS2 sticks here and updating the mouse according to the current controller profile
# ifndef USE_HACKED_PS2_INPUT
    if (menuShowing){
        GETSTICKVALUE(mdelta[0], leftStickX, scale);
        GETSTICKVALUE(mdelta[1], leftStickY, scale);

        mMovementScale = 0.0f;
        mStrafeScale = 0.0f;
    }

    else{
        switch (mCurrentProfile){
            case PAD_PROFILE_1:
                GETSTICKVALUE(stickPos[0],  leftStickX, scale);     //strafe
                GETSTICKVALUE(stickPos[1],  leftStickY, scale);     //move
                GETSTICKVALUE(mdelta[0],    prevLeftStickX, scale); //old strafe
                GETSTICKVALUE(mdelta[1],    prevLeftStickY, scale); //old move

                testAxis(stickPos[0], mdelta[0], leftStick[0]);
                testAxis(stickPos[1], mdelta[1], leftStick[1]);


                GETSTICKVALUE(mdelta[0], rightStickX, scale);       //look left/right
                GETSTICKVALUE(mdelta[1], rightStickY, scale);       //look up/down

                //keep the absolute values of the movement scales
                mStrafeScale = fabsf(stickPos[0] / GAMEPAD_SENSITIVITY);
                mMovementScale = fabsf(stickPos[1] / GAMEPAD_SENSITIVITY);
                //TRACE("PSPInputSystem::MouseTick(): stick scales = {strafe = %g, move = %g}, raw = <%g, %g>\n", mStrafeScale, mMovementScale, stickPos[0], stickPos[1]);
                break;

            case PAD_PROFILE_2:
                GETSTICKVALUE(stickPos[0],  rightStickX, scale);    //strafe
                GETSTICKVALUE(stickPos[1],  leftStickY, scale);     //move
                GETSTICKVALUE(mdelta[0],    prevRightStickX, scale); //old strafe
                GETSTICKVALUE(mdelta[1],    prevLeftStickY, scale);  //old move

                testAxis(stickPos[0], mdelta[0], rightStick[0]);
                testAxis(stickPos[1], mdelta[1], leftStick[1]);


                GETSTICKVALUE(mdelta[0], leftStickX, scale);        //look left/right
                GETSTICKVALUE(mdelta[1], rightStickY, scale);       //look up/down

                //keep the absolute values of the movement scales
                mStrafeScale = fabsf(stickPos[0] / GAMEPAD_SENSITIVITY);
                mMovementScale = fabsf(stickPos[1] / GAMEPAD_SENSITIVITY);
                //TRACE("PSPInputSystem::MouseTick(): stick scales = {strafe = %g, move = %g}, raw = <%g, %g>\n", mStrafeScale, mMovementScale, stickPos[0], stickPos[1]);
                break;
        }
    }
# endif
#endif

    //check the mouse buttons
    TESTBUTTON(MOUSE_LEFTBUTTON,    EMouseKeyLeft);
    TESTBUTTON(MOUSE_MIDDLEBUTTON,  EMouseKeyMiddle);
    TESTBUTTON(MOUSE_RIGHTBUTTON,   EMouseKeyRight);


    //update the mouse's position (applying smoothing as necessary)
	if (!menuShowing){
		if (mMouseSmooth){
			mdelta[0] = (mdelta[0] + mMouseDelta.X) * 0.5f;
			mdelta[1] = (mdelta[1] + mMouseDelta.Y) * 0.5f;
		}

		mMouseDelta.X = mdelta[0] * mMouseSensitivity * (DIDELTA_SCALE / 400.0f);
		mMouseDelta.Y = mdelta[1] * mMouseSensitivity * (DIDELTA_SCALE / 400.0f);
		mMousePos.x += ( int )mMouseDelta.X;
		mMousePos.y += ( int )mMouseDelta.Y;
	}

    //update the menu mouse's position.  No smoothing is applied, the current delta
    //is just scaled and added to the current position
	else
	{
		mMouseDelta.X = mMouseDelta.Y = 0.0f;
		//mMousePos.x += (long)(mdelta[0] * MENU_MOUSESCALE);
		//mMousePos.y += (long)(mdelta[1] * MENU_MOUSESCALE * aspect);
	}


    //lock the cursor to the center of the screen if the mouse is locked
	if (mMouseLocked){
		mMousePos.x = view->LocalViewport.Width >> 1;
		mMousePos.y = view->LocalViewport.Height >> 1;
	}

    //clamp the mouse to the screen if it's not locked to the center
    else{
	    if (mMousePos.x < 0)
		    mMousePos.x = 0;
	    else if (mMousePos.x >= view->LocalViewport.Width)
		    mMousePos.x = view->LocalViewport.Width - 1;
	    if (mMousePos.y < 0)
		    mMousePos.y = 0;
	    else if (mMousePos.y >= view->LocalViewport.Height)
		    mMousePos.y = view->LocalViewport.Height - 1;
    }




    //assign these action bits depending on the controller scheme (scheme 3 doesn't use them)
    /*ActionStatus &= ~( 1<<EInputMoveForward );
	ActionStatus &= ~( 1<<EInputMoveBackward );
	ActionStatus &= ~( 1<<EInputMoveLeft );
	ActionStatus &= ~( 1<<EInputMoveRight );

	if (mMouseDelta.Y > 0.15f)
		ActionStatus |= ( 1 << EInputMoveBackward );
	else if (mMouseDelta.Y < -0.15f)
		ActionStatus |= ( 1 << EInputMoveForward );
	
	if (mMouseDelta.X > 0.15f)
		ActionStatus |= ( 1 << EInputMoveRight );
	else if (mMouseDelta.X < -0.15f)
		ActionStatus |= ( 1 << EInputMoveLeft );//*/
	
}


void PSPInputSystem::GamePadTick(bool menuShowing)
{

#ifdef TRANSGAMING_PSP
    TESTBUTTON(TG_PAD_TRIANGLE,     BUTTON_TRIANGLE);
    TESTBUTTON(TG_PAD_CROSS,        BUTTON_CROSS);
    TESTBUTTON(TG_PAD_SQUARE,       BUTTON_SQUARE);
    TESTBUTTON(TG_PAD_CIRCLE,       BUTTON_CIRCLE);

    TESTBUTTON(TG_PAD_UP,           BUTTON_UP);
    TESTBUTTON(TG_PAD_DOWN,         BUTTON_DOWN);
    TESTBUTTON(TG_PAD_LEFT,         BUTTON_LEFT);
    TESTBUTTON(TG_PAD_RIGHT,        BUTTON_RIGHT);

    TESTBUTTON(TG_PAD_L1,           BUTTON_L1);
    TESTBUTTON(TG_PAD_R1,           BUTTON_R1);

    TESTBUTTON(TG_PAD_START,        BUTTON_START);
    TESTBUTTON(TG_PAD_SELECT,       BUTTON_SELECT);

    TESTBUTTON(TG_PAD_INTERCEPTED,  BUTTON_HOME);
    TESTBUTTON(TG_PAD_HOLD,         BUTTON_HOLD);

    //try to read the mystery buttons (HOME, Volume +/-, Display, Mute?)
    /*if (pad.pressedBtn & PAD_MYSTERY_1)
        TRACE("PSPInputSystem::GamePadTick(): pushed mystery button 1\n");

    if (pad.pressedBtn & PAD_MYSTERY_2)
        TRACE("PSPInputSystem::GamePadTick(): pushed mystery button 2\n");

    if (pad.pressedBtn & PAD_MYSTERY_3)
        TRACE("PSPInputSystem::GamePadTick(): pushed mystery button 3\n");

    if (pad.pressedBtn & PAD_MYSTERY_4)
        TRACE("PSPInputSystem::GamePadTick(): pushed mystery button 4\n");//*/

#elif defined(TRANSGAMING_PS2)
    //process PS2 button presses here!!
# ifndef USE_HACKED_PS2_INPUT
    TESTBUTTON(SCE_PADRup,      BUTTON_TRIANGLE);
    TESTBUTTON(SCE_PADRdown,    BUTTON_CROSS);
    TESTBUTTON(SCE_PADRleft,    BUTTON_SQUARE);
    TESTBUTTON(SCE_PADRright,   BUTTON_CIRCLE);

    TESTBUTTON(SCE_PADLup,      BUTTON_UP);
    TESTBUTTON(SCE_PADLdown,    BUTTON_DOWN);
    TESTBUTTON(SCE_PADLleft,    BUTTON_LEFT);
    TESTBUTTON(SCE_PADLright,   BUTTON_RIGHT);

    TESTBUTTON(SCE_PADL1,       BUTTON_L1);
    TESTBUTTON(SCE_PADR1,       BUTTON_R1);
    TESTBUTTON(SCE_PADL2,       BUTTON_L2);
    TESTBUTTON(SCE_PADR2,       BUTTON_R2);
    TESTBUTTON(SCE_PADi,        BUTTON_L3);
    TESTBUTTON(SCE_PADj,        BUTTON_R3);

    TESTBUTTON(SCE_PADstart,    BUTTON_START);
    TESTBUTTON(SCE_PADselect,   BUTTON_SELECT);
# else
#  error Using hacked PS2 input!
	float       step = 1.2f;
    static int  updateCamera = 0;
    static int  disableCounter = 0;
    static int  readNumber = 0;
    Vector      lookDelta, moveDelta;


    readNumber++;

    lookDelta.X = 0.0f;
    lookDelta.Y = 0.0f;
    lookDelta.Z = 0.0f;

	moveDelta.X = 0.0f;
	moveDelta.Y = 0.0f;
	moveDelta.Z = 0.0f;

	if ( pad.button & SCE_PADLup ){
		moveDelta.Z = -step;
        updateCamera = readNumber + 1;
	}
	else if ( pad.button & SCE_PADLdown){
		moveDelta.Z = step;
        updateCamera = readNumber + 1;
	}

	if ( pad.button & SCE_PADRleft ){
		moveDelta.Y = step;
        updateCamera = readNumber + 1;
	}
	else if ( pad.button & SCE_PADRdown){
		moveDelta.Y =  -step;
        updateCamera = readNumber + 1;
	}

	if ( pad.button & SCE_PADLleft ){
		moveDelta.X = -step;
        updateCamera = readNumber + 1;
	}	
	else if ( pad.button & SCE_PADLright ){
		moveDelta.X = step;
        updateCamera = readNumber + 1;
	}


	if ( pad.button & SCE_PADRright ){
		lookDelta.Y = 3 * step;
        updateCamera = readNumber + 1;
	}

	if ( pad.button & SCE_PADRup){
		lookDelta.Y = 3 * -step;
        updateCamera = readNumber + 1;
	}

	if ( pad.button & SCE_PADL1 ){
		lookDelta.X = 3 * -step;
        updateCamera = readNumber + 1;
	}

	else if ( pad.button & SCE_PADR1 ){
		lookDelta.X = 3 * step;
        updateCamera = readNumber + 1;
	}


    //update the camera's position manually instead of moving it with the player
    if (readNumber <= updateCamera){
        if (GEngine->Video){
            View *  view = GEngine->Video->GetActiveView();
            Vector  pos = view->mCamera.GetPosition();
            Vector  rot = view->mCamera.GetRotation();
            Matrix  w2c = view->mCamera.WorldToCamera();


            //set the new look angle
            view->mCamera.SetRotation(rot + lookDelta);
	        view->mCamera.InverseTransform();

            //move the camera according to the movement delta
            w2c.SetTranslation(0.0f, 0.0f, 0.0f);
            Vector newpos = w2c.Transform(moveDelta);

            view->mCamera.SetPosition(pos + newpos);
        }
    }
# endif
#endif


    //handle checking for key repeats in the menu
    if (menuShowing){
        float currTime = GEngine->SysDriver.GetCurrentTime();


        if ( mLastTime == 0.0f ){
            mLastTime = currTime;
            mStartTime = currTime;
        }

        if ( ( currTime - mStartTime > KEY_REPEAT_DELAY ) && ( currTime - mLastTime > KEY_REPEAT ) ){
            ActionStatus |= (1 << KeyBindings[mLastKey].action);
            SendEvent(EEventKeyDown, mLastKey);
            mLastTime = currTime;
        }
    }
}


bool PSPInputSystem::InitInput(){
#ifdef TRANSGAMING_PSP
    tgPadInit(&pad, GAMEPAD_DEADZONE, GAMEPAD_SENSITIVITY);
#else
# ifdef USE_PS2_INPUT
#  ifndef TRANSGAMING_USE_PHYSFS
    //initialize the file system so that the input system modules can be found and loaded
    shellFileInit(SHELLFILE_MEDIUM_HOST0, false, "iop", NULL);
#  endif

    shellPadInit(false);
# endif
#endif

    if (!SetupGamePad())
        return false;
	
	return true;
}


void PSPInputSystem::Release(){
#ifndef TRANSGAMING_PSP
# ifdef USE_PS2_INPUT
    shellPadClose(false);
# endif
#endif
}


bool PSPInputSystem::SetupGamePad(){

#ifdef TRANSGAMING_PSP
    // eric-> nothing to initialize here.  The main input init took care of this.  We
    //   just need this function to appease the compiler.
#else
# ifdef USE_PS2_INPUT
    shellPadInitPort(&pad, padDmaBuf, 0, 0, GAMEPAD_DEADZONE);
# endif
#endif
    return true;
}


bool PSPInputSystem::IsKeyPrintable( unsigned char key )
{
    // eric-> no keys on the PSP are printable... except maybe 'X' and 'O', but then 
    //   who wants their name to resemble a tic-tac-toe game?
	return false;
}


unsigned char PSPInputSystem::KeyToPrintable( unsigned char key )
{
    // eric-> no keys on the PSP are printable... except maybe 'X' and 'O', but then 
    //   who wants their name to resemble a tic-tac-toe game?
	return 0;
}


void PSPInputSystem::FillKeyNamesTable( void )
{
	ScriptObject txt = GScript.Globals()["TXT"];
	const char *unknown = txt["None"].GetString();


	for (int i = 0; i < BUTTON_COUNT; ++i)       // Fill whole table with default string
        KeyNames[i] = unknown;

    for (int i = 0; i < BUTTON_COUNT; ++i)       // Fill whole table with default string
        KeyNamesEng[i] = "None";
    
	for (int i = 0; i < BUTTON_COUNT; ++i)       // Fill whole table with default string
        KeyShortNames[i] = "None";


    KeyNames[BUTTON_UP] =               txt["Buttons"]["CursorUp"].GetString();
    KeyNames[BUTTON_DOWN] =             txt["Buttons"]["CursorDown"].GetString();
    KeyNames[BUTTON_LEFT] =             txt["Buttons"]["CursorLeft"].GetString();
    KeyNames[BUTTON_RIGHT] =            txt["Buttons"]["CursorRight"].GetString();
    KeyNames[BUTTON_TRIANGLE] =         txt["Buttons"]["Triangle"].GetString();
    KeyNames[BUTTON_CIRCLE] =           txt["Buttons"]["Circle"].GetString();
    KeyNames[BUTTON_SQUARE] =           txt["Buttons"]["Square"].GetString();
    KeyNames[BUTTON_CROSS] =            txt["Buttons"]["Cross"].GetString();
    KeyNames[BUTTON_START] =            txt["Buttons"]["Start"].GetString();
    KeyNames[BUTTON_SELECT] =           txt["Buttons"]["Select"].GetString();
    KeyNames[BUTTON_HOME] =             txt["Buttons"]["Home"].GetString();
    KeyNames[BUTTON_HOLD] =             txt["Buttons"]["Hold"].GetString();
#ifdef TRANSGAMING_PSP
    KeyNames[BUTTON_L1] =               txt["Buttons"]["LButton"].GetString();
    KeyNames[BUTTON_R1] =               txt["Buttons"]["RButton"].GetString();
#elif defined(TRANSGAMING_PS2)
    KeyNames[BUTTON_L1] =               txt["Buttons"]["L1Button"].GetString();
    KeyNames[BUTTON_R1] =               txt["Buttons"]["R1Button"].GetString();
#endif
    KeyNames[BUTTON_L2] =               txt["Buttons"]["L2Button"].GetString();
    KeyNames[BUTTON_R2] =               txt["Buttons"]["R2Button"].GetString();
    KeyNames[BUTTON_L3] =               txt["Buttons"]["L3Button"].GetString();
    KeyNames[BUTTON_R3] =               txt["Buttons"]["R3Button"].GetString();
    KeyNames[BUTTON_FAKE_LMOUSE] =      txt["Buttons"]["LeftMouse"].GetString();
    KeyNames[BUTTON_FAKE_RMOUSE] =      txt["Buttons"]["RightMouse"].GetString();
    KeyNames[BUTTON_FAKE_MMOUSE] =      txt["Buttons"]["MiddleMouse"].GetString();
    KeyNames[BUTTON_POS_LX] =           txt["Buttons"]["PosLX"].GetString();
    KeyNames[BUTTON_NEG_LX] =           txt["Buttons"]["NegLX"].GetString();
    KeyNames[BUTTON_POS_LY] =           txt["Buttons"]["PosLY"].GetString();
    KeyNames[BUTTON_NEG_LY] =           txt["Buttons"]["NegLY"].GetString();
    KeyNames[BUTTON_POS_RX] =           txt["Buttons"]["PosRX"].GetString();
    KeyNames[BUTTON_NEG_RX] =           txt["Buttons"]["NegRX"].GetString();
    KeyNames[BUTTON_POS_RY] =           txt["Buttons"]["PosRY"].GetString();
    KeyNames[BUTTON_NEG_RY] =           txt["Buttons"]["NegRY"].GetString();


    KeyNamesEng[BUTTON_UP] =            "Up Button";
    KeyNamesEng[BUTTON_DOWN] =          "Down Button";
    KeyNamesEng[BUTTON_LEFT] =          "Left Button";
    KeyNamesEng[BUTTON_RIGHT] =         "Right Button";
    KeyNamesEng[BUTTON_TRIANGLE] =      "Triangle Button";
    KeyNamesEng[BUTTON_CIRCLE] =        "Circle Button";
    KeyNamesEng[BUTTON_SQUARE] =        "Square Button";
    KeyNamesEng[BUTTON_CROSS] =         "Cross Button";
    KeyNamesEng[BUTTON_START] =         "START Button";
    KeyNamesEng[BUTTON_SELECT] =        "SELECT Button";
    KeyNamesEng[BUTTON_HOME] =          "HOME Button";
    KeyNamesEng[BUTTON_HOLD] =          "HOLD Button";
#ifdef TRANSGAMING_PSP
    KeyNamesEng[BUTTON_L1] =            "L Button";
    KeyNamesEng[BUTTON_R1] =            "R Button";
#elif defined(TRANSGAMING_PS2)
    KeyNamesEng[BUTTON_L1] =            "L1 Button";
    KeyNamesEng[BUTTON_R1] =            "R1 Button";
#endif
    KeyNamesEng[BUTTON_L2] =            "L2 Button";
    KeyNamesEng[BUTTON_R2] =            "R2 Button";
    KeyNamesEng[BUTTON_L3] =            "L3 Button";
    KeyNamesEng[BUTTON_R3] =            "R3 Button";
    KeyNamesEng[BUTTON_FAKE_LMOUSE] =   "Left Mouse Button";
    KeyNamesEng[BUTTON_FAKE_RMOUSE] =   "Right Mouse Button";
    KeyNamesEng[BUTTON_FAKE_MMOUSE] =   "Middle Mouse Button";
#ifdef TRANSGAMING_PSP
    KeyNamesEng[BUTTON_POS_LX] =        "Positive X";
    KeyNamesEng[BUTTON_NEG_LX] =        "Negative X";
    KeyNamesEng[BUTTON_POS_LY] =        "Positive Y";
    KeyNamesEng[BUTTON_NEG_LY] =        "Negative Y";
#elif defined(TRANSGAMING_PS2)
    KeyNamesEng[BUTTON_POS_LX] =        "Positive LX";
    KeyNamesEng[BUTTON_NEG_LX] =        "Negative LX";
    KeyNamesEng[BUTTON_POS_LY] =        "Positive LY";
    KeyNamesEng[BUTTON_NEG_LY] =        "Negative LY";
#endif
    KeyNamesEng[BUTTON_POS_RX] =        "Positive RX";
    KeyNamesEng[BUTTON_NEG_RX] =        "Negative RX";
    KeyNamesEng[BUTTON_POS_RY] =        "Positive RY";
    KeyNamesEng[BUTTON_NEG_RY] =        "Negative RY";


    KeyShortNames[BUTTON_UP] =          "Up";
    KeyShortNames[BUTTON_DOWN] =        "Down";
    KeyShortNames[BUTTON_LEFT] =        "Left";
    KeyShortNames[BUTTON_RIGHT] =       "Right";
    KeyShortNames[BUTTON_TRIANGLE] =    "Triangle";
    KeyShortNames[BUTTON_CIRCLE] =      "Circle";
    KeyShortNames[BUTTON_SQUARE] =      "Square";
    KeyShortNames[BUTTON_CROSS] =       "Cross";
    KeyShortNames[BUTTON_START] =       "START";
    KeyShortNames[BUTTON_SELECT] =      "SELECT";
    KeyShortNames[BUTTON_HOME] =        "HOME";
#ifdef TRANSGAMING_PSP
    KeyShortNames[BUTTON_L1] =          "L";
    KeyShortNames[BUTTON_R1] =          "R";
#elif defined(TRANSGAMING_PS2)
    KeyShortNames[BUTTON_L1] =          "L1";
    KeyShortNames[BUTTON_R1] =          "R1";
#endif
    KeyShortNames[BUTTON_L2] =          "L2";
    KeyShortNames[BUTTON_R2] =          "R2";
    KeyShortNames[BUTTON_L3] =          "L3";
    KeyShortNames[BUTTON_R3] =          "R3";
    KeyShortNames[BUTTON_FAKE_LMOUSE] = "LMouse";
    KeyShortNames[BUTTON_FAKE_RMOUSE] = "RMouse";
    KeyShortNames[BUTTON_FAKE_MMOUSE] = "MMouse";
#ifdef TRANSGAMING_PSP
    KeyShortNames[BUTTON_POS_LX] =      "PosX";
    KeyShortNames[BUTTON_NEG_LX] =      "NegX";
    KeyShortNames[BUTTON_POS_LY] =      "PosY";
    KeyShortNames[BUTTON_NEG_LY] =      "NegY";
#elif defined(TRANSGAMING_PS2)
    KeyShortNames[BUTTON_POS_LX] =      "PosLX";
    KeyShortNames[BUTTON_NEG_LX] =      "NegLX";
    KeyShortNames[BUTTON_POS_LY] =      "PosLY";
    KeyShortNames[BUTTON_NEG_LY] =      "NegLY";
#endif
    KeyShortNames[BUTTON_POS_RX] =      "PosRX";
    KeyShortNames[BUTTON_NEG_RX] =      "NegRX";
    KeyShortNames[BUTTON_POS_RY] =      "PosRY";
    KeyShortNames[BUTTON_NEG_RY] =      "NegRY";



	ScriptObject cfg = GScript.Globals()["Cfg"];
	if (!cfg["MouseSensitivity"].IsNil())
		mMouseSensitivity = cfg["MouseSensitivity"].GetFloat();
}


bool PSPInputSystem::BindUIKey(unsigned int keycode){
	if( keycode <= 0 )
		return false;


    if( keycode == mFireSwitchKey || keycode == mAltFireSwitchKey || keycode == mFireSwitchToggleKey || keycode == mAltFireSwitchToggleKey )
		mIsFireSwitched = !mIsFireSwitched;
	else if( keycode == mPauseKey || keycode == mAltPauseKey )
		UIActionStatus |= ( 1<<EInputUIPause );
	else if( keycode == mScreenshotKey || keycode == mAltScreenshotKey )
		UIActionStatus |= ( 1<<EInputUIScreenshot );
	else if( keycode == mMenuKey || keycode == mAltMenuKey )
		UIActionStatus |= ( 1<<EInputUIMenu );
	else if( keycode == mScoreboardKey || keycode == mAltScoreboardKey )
		UIActionStatus |= ( 1<<EInputUIScoreboard );
	else if( keycode == mSayToAllKey || keycode == mAltSayToAllKey )
		UIActionStatus |= ( 1<<EInputUISayToAll );
	else if( keycode == mSayToTeamKey || keycode == mAltSayToTeamKey )
		UIActionStatus |= ( 1<<EInputUISayToTeam );
	else if( keycode == mQuickLoadKey || keycode == mAltQuickLoadKey )
		UIActionStatus |= ( 1<<EInputUIQuickLoad );
	else if( keycode == mQuickSaveKey || keycode == mAltQuickSaveKey )
		UIActionStatus |= ( 1<<EInputUIQuickSave );
	else if( keycode == mFlashlightKey || keycode == mAltFlashlightKey )
		UIActionStatus |= ( 1<<EInputUIFlashlight );
	else if( keycode == mZoomKey || keycode == mAltZoomKey )
		UIActionStatus |= ( 1<<EInputUIZoom );
	else
		return false;

    return true;
}


bool PSPInputSystem::UnbindUIKey(unsigned int keycode){
	if (keycode <= 0)
		return false;


    if( keycode == mPauseKey || keycode == mAltPauseKey )
		UIActionStatus &= ~( 1<<EInputUIPause );
	else if( keycode == mScreenshotKey || keycode == mAltScreenshotKey )
		UIActionStatus &= ~( 1<<EInputUIScreenshot );
	else if( keycode == mMenuKey || keycode == mAltMenuKey )
		UIActionStatus &= ~( 1<<EInputUIMenu );
	else if( keycode == mFireSwitchKey || keycode == mAltFireSwitchKey )
		mIsFireSwitched = !mIsFireSwitched;
	else if( keycode == mScoreboardKey || keycode == mAltScoreboardKey )
		UIActionStatus &= ~( 1<<EInputUIScoreboard );
	else if( keycode == mSayToAllKey || keycode == mAltSayToAllKey )
		UIActionStatus &= ~( 1<<EInputUISayToAll );
	else if( keycode == mSayToTeamKey || keycode == mAltSayToTeamKey )
		UIActionStatus &= ~( 1<<EInputUISayToTeam );
	else if( keycode == mQuickLoadKey || keycode == mAltQuickLoadKey )
		UIActionStatus &= ~( 1<<EInputUIQuickLoad );
	else if( keycode == mQuickSaveKey || keycode == mAltQuickSaveKey )
		UIActionStatus &= ~( 1<<EInputUIQuickSave );
	else if( keycode == mFlashlightKey || keycode == mAltFlashlightKey )
		return true;
	else if( keycode == mZoomKey || keycode == mAltZoomKey )
		return true;
	else
		return false;


	return true;
}
