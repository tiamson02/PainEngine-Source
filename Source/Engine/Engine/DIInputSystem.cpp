#include "Engine.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/HUD.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "DIInputSystem.h"
#include <zmouse.h>

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"

#include "Net/DemoRecording2.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


#define DIEXCLUSIVE		1
#define KB_BUFFER_SIZE	512
#define DIWHEEL_DELTA	120
#define DIDELTA_SCALE	3.2f
#define KEY_REPEAT		0.05f
#define KEY_REPEAT_DELAY	0.3f
#define GAMEPAD_DEADZONE	100


DIInputSystem::DIInputSystem( HWND hwnd )
:	InputSystem( hwnd ), mDI( NULL ), mMouseDev( NULL ), mKBDev( NULL ), mGamePad( NULL ), mMouseEvent( NULL ),
	mMouseSensitivity( 25.0f ), mMouseLocked( false ), mMouseVisible( true ), mWin32Only( true )
{

	mMousePos.x = 0;
	mMousePos.y = 0;
	mCenterPos.x = 0;
	mCenterPos.y = 0;
	mMouseDelta = Vector( 0.0f, 0.0f, 0.0f );
	mMouseWheelDelta = 0;
	mLastKey = 0;
	mUseDInput = 1;
	mDIDeltaScale = DIDELTA_SCALE;

	mLookDelta = Vector( 0.0f, 0.0f, 0.0f );
	mMoveDelta = Vector( 0.0f, 0.0f, 0.0f );

	ScriptObject cfg = GScript.Globals()["Cfg"];
	if( !cfg.IsNil() )
	{
		mMouseSmooth = cfg["SmoothMouse"].GetBool();
		mWheelSensitivity = 4.0f - cfg["WheelSensitivity"].GetFloat();
	}
	else
	{
		mWheelSensitivity = 1.0f;
		mMouseSmooth = false;
	}

	for( int i = 0; i < 16; i++ )
		mButtonsState[i] = false;

	FillKeyNamesTable();
	FillDIKeyTable();
}


DIInputSystem::~DIInputSystem()
{
	Release();
}


#ifdef TRANSGAMING
void DIInputSystem::setCurrentControlProfile(ControlProfile cp){
    switch (cp){
        case PAD_PROFILE_1:
        case PAD_PROFILE_2:
            mCurrentProfile = cp;
            TRACE("DIInputSystem::setCurrentControlProfile(): setting current controller profile to %d\n", cp);
            break;

        case PAD_PROFILE_NONE:
        default:
            TRACE("DIInputSystem::setCurrentControlProfile(): ERROR-> invalid control profile: %d\n", cp);
            break;
    }
}

InputSystem::ControlProfile DIInputSystem::getCurrentControlProfile() const{
    return mCurrentProfile;
}

float DIInputSystem::getMovementScale() const{
    return mMovementScale;
}

float DIInputSystem::getStrafeScale() const{
    return mStrafeScale;
}
#endif


LRESULT DIInputSystem::HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if( !GEngine->Video || !GEngine->Video->GetActiveView() )
		return FALSE;

	if( !mWin32Only || GetForegroundWindow() != GEngine->Video->GetActiveView()->GetTopmostWindow() )
		return FALSE;

	unsigned int keycode;

	switch( msg )
	{
	case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
		keycode = TranslateKeyCode( wp, lp );
		if( keycode > 0 )
			mPressed.Add( keycode );
        return TRUE;

	case WM_SYSKEYUP:
    case WM_KEYUP:
		keycode = TranslateKeyCode( wp, lp );
		if( keycode > 0 )
			mReleased.Add( keycode );
        return TRUE;

    case WM_LBUTTONDOWN:
		CaptureMouse(true);
		mPressed.Add( EMouseKeyLeft );
        return TRUE;

	case WM_LBUTTONUP:
		CaptureMouse(false);
		mReleased.Add( EMouseKeyLeft );
        return TRUE;

    case WM_RBUTTONDOWN:
		CaptureMouse(true);
		mPressed.Add( EMouseKeyRight );
        return TRUE;

	case WM_RBUTTONUP:
		CaptureMouse(false);
		mReleased.Add( EMouseKeyRight );
        return TRUE;

	case WM_MBUTTONDOWN:
		CaptureMouse(true);
		mPressed.Add( EMouseKeyMiddle );
        return TRUE;

    case WM_MBUTTONUP:
		CaptureMouse(true);
		mReleased.Add( EMouseKeyMiddle );
        return TRUE;

	case WM_MOUSEWHEEL:
		short add = ((short)HIWORD(wp));
		mMouseWheelDelta += add;
		if( mWheelSensitivity <= 0.0f )
			mWheelSensitivity = 0.1f;
		if( mMouseWheelDelta >= DIWHEEL_DELTA * mWheelSensitivity )
		{
			mMouseWheelDelta = 0;
			mPressed.Add( EMouseWheelForward );
		}
		if( mMouseWheelDelta <= -DIWHEEL_DELTA * mWheelSensitivity )
		{
			mMouseWheelDelta = 0;
			mPressed.Add( EMouseWheelBack );
		}
		return TRUE;
	}

    return FALSE; // message unhandled
}


void DIInputSystem::SetUseDInput( bool use )
{
	mUseDInput = use;

	BOOL wasLocked = mMouseLocked;
	if( wasLocked )
		LockMouse( FALSE );

	if( use )
	{
		SwitchToDInput();
	}
	else
	{
		SwitchToWin32();
		if( wasLocked )
			ShowMouse( FALSE );
	}

	if( wasLocked )
	{
		if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
			LockMouse( TRUE );
	}
}


void DIInputSystem::UpdateMousePosition()
{
	if( mWin32Only )
	{
		RECT r;
		GetClientRect( hWnd, &r );
		mCenterPos.x = ( r.right - r.left ) >> 1;
		mCenterPos.y = ( r.bottom - r.top ) >> 1;
		GetCursorPos( &mMousePos );
		ScreenToClient( hWnd, &mMousePos );
	}
}


void DIInputSystem::Tick()
{
	if( GetForegroundWindow() != GEngine->Video->GetActiveView()->GetTopmostWindow() )
		return;

	static HWND lastWnd = ::GetFocus();
	if( lastWnd && lastWnd != ::GetFocus() )
	{
		Reset();
		lastWnd = ::GetFocus();
	}

	if( !mWin32Only && ( !mMouseDev || !mKBDev ) )
		return;

	if( !mWin32Only )
	{
		MouseTick();
		KeyboardTick();
		GamePadTick();
	}

	// Clear all UI actions but scoreboard
	UIActionStatus &= ( 1<<EInputUIScoreboard );
	ActionStatus &= ~( 1<<EInputNextWeapon );
	ActionStatus &= ~( 1<<EInputPrevWeapon );

	for( int i=0; i<mPressed.Size(); i++ )
	{
		if( mPressed[i] <= 0 ) continue;
		if( mWin32Only && i < mPressed.Size() - 1 && mPressed[i] == VK_LCONTROL && mPressed[i + 1] == VK_LMENU )
		{
			mPressed[i] = VK_RMENU;
			mPressed[i + 1] = 0;
		}
		if( mPressed[i] == EMouseKeyLeft ) MouseStatus |= ( 1 << EMouseLeftButton );
		if( mPressed[i] == EMouseKeyRight ) MouseStatus |= ( 1 << EMouseRightButton );
		if( mPressed[i] == EMouseKeyMiddle ) MouseStatus |= ( 1 << EMouseMiddleButton );

		if( gDemoRec && gDemoRec->IsPlaying() )
		{
#ifdef TRANSGAMING
			SendEvent( EEventKeyDown, mPressed[i] );
#else
			SendEvent( InputEvent( EEventKeyDown, mPressed[i] ) );
#endif
			continue;
		}

#ifndef TRANSGAMING
		if( !GEngine->Video->Hud->mConsole.Active() )
#endif
		{
			if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
			{
				bool isUIKey = BindUIKey( mPressed[i] );
				if( !isUIKey )
				{
					if( KeyBindings[mPressed[i]].action != EInputFire && KeyBindings[mPressed[i]].action != EInputAltFire )
					{
						DWORD a = KeyBindings[mPressed[i]].action;
						if( a >= EInputWeapon8 && a <= EInputWeapon14 )
						{
							for( int i=0; i<7; i++ )
							{
								ActionStatus &= ~( 1 << ( EInputWeapon1 + i ) );
								ActionStatus &= ~( 1 << ( EInputWeapon8 + i ) );
								GScript.DoString( "Game.SwitchFire[%d] = false", i + 1 );
							}

							ActionStatus |= ( 1 << ( EInputWeapon1 + a - EInputWeapon8 ) );
							GScript.DoString( "Game.SwitchFire[%d] = true", a - EInputWeapon8 + 1 );
						}
						else if( a >= EInputWeapon1 && a <= EInputWeapon7 )
						{
							for( int i=0; i<7; i++ )
							{
								ActionStatus &= ~( 1 << ( EInputWeapon1 + i ) );
								ActionStatus &= ~( 1 << ( EInputWeapon8 + i ) );
								GScript.DoString( "Game.SwitchFire[%d] = false", i + 1 );
							}

							ActionStatus |= ( 1 << a );
						}
						else if( a )
						{
							ActionStatus |= ( 1 << a );
						}
#ifdef TRANSGAMING
						else if( !KeyBindings[mPressed[i]].command.IsNull() )
#else
						else if( KeyBindings[mPressed[i]].command != "" )
#endif
						{
#ifdef TRANSGAMING
							char buffer[16];
							snprintf( buffer, 16, "%s %s", mLuaToDo, KeyBindings[mPressed[i]].command);
#else
							mLuaToDo = mLuaToDo + " " + KeyBindings[mPressed[i]].command;
#endif
						}
					}
				}
			}
		}
#ifdef TRANSGAMING
		SendEvent( EEventKeyDown, mPressed[i] );
#else
		SendEvent( InputEvent( EEventKeyDown, mPressed[i] ) );
#endif
	}
	mPressed.ClearArray();

	ActionStatus &= ~( 1 << EInputFire );
	ActionStatus &= ~( 1 << EInputAltFire );

#ifdef TRANSGAMING
    if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
#else
    if( !GEngine->Video->Hud->mConsole.Active() && !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
#endif
	{
		for( int i=0; i<256; i++ )
		{
			if( KeyBindings[i].action == EInputFire )
			{
				if( GetKeyStatus( i ) == 1 || GetKeyStatus( i ) == 2 )
				{
					if( mIsFireSwitched )
						ActionStatus |= ( 1 << EInputAltFire );
					else
						ActionStatus |= ( 1 << EInputFire );
				}
			}
			else if( KeyBindings[i].action == EInputAltFire )
			{
				if( GetKeyStatus( i ) == 1 || GetKeyStatus( i ) == 2 )
				{
					if( mIsFireSwitched )
						ActionStatus |= ( 1 << EInputFire );
					else
						ActionStatus |= ( 1 << EInputAltFire );
				}
			}
		}
	}

	ProcessEvents();
	UpdateMousePosition();

	for( int i=0; i<mReleased.Size(); i++ )
	{
		if( mReleased[i] <= 0 ) continue;
		if( mWin32Only && i < mReleased.Size() - 1 && mReleased[i] == VK_LCONTROL && mReleased[i + 1] == VK_LMENU )
		{
			mReleased[i] = VK_RMENU;
			mReleased[i + 1] = 0;
		}
		if( mReleased[i] == EMouseKeyLeft ) MouseStatus &= ~( 1 << EMouseLeftButton );
		if( mReleased[i] == EMouseKeyRight ) MouseStatus &= ~( 1 << EMouseRightButton );
		if( mReleased[i] == EMouseKeyMiddle ) MouseStatus &= ~( 1 << EMouseMiddleButton );

		if( gDemoRec && gDemoRec->IsPlaying() )
		{
#ifdef TRANSGAMING
			SendEvent( EEventKeyUp, mReleased[i] );
#else
			SendEvent( InputEvent( EEventKeyUp, mReleased[i] ) );
#endif
			continue;
		}

#ifndef TRANSGAMING
		if( !GEngine->Video->Hud->mConsole.Active() )
#endif
		{
			if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
			{
				bool isUIKey = UnbindUIKey( mReleased[i] );
				if( !isUIKey )
					isUIKey = CheckMessagesKeys( mReleased[i] );
				if( !isUIKey )
				{
					if( KeyBindings[mReleased[i]].action != EInputFire && KeyBindings[mReleased[i]].action != EInputAltFire )
					{
						ActionStatus &= ~( 1 << KeyBindings[mReleased[i]].action );
						DWORD a = KeyBindings[mReleased[i]].action;
						if( a >= EInputWeapon8 && a <= EInputWeapon14 )
						{
							ActionStatus &= ~( 1 << ( EInputWeapon1 + a - EInputWeapon7 ) );
						}
					}
				}
			}
		}
#ifdef TRANSGAMING
		SendEvent( EEventKeyUp, mReleased[i] );
#else
		SendEvent( InputEvent( EEventKeyUp, mReleased[i] ) );
#endif
	}
	mReleased.ClearArray();

	if( mWin32Only )
	{
		if( mMouseLocked )
		{
			POINT p = mCenterPos;
			ClientToScreen( hWnd, &p );
			SetCursorPos( p.x, p.y );
		}
	}
}


void DIInputSystem::Reset()
{
	InputSystem::Reset();

	mMouseDelta = Vector( 0.0f, 0.0f, 0.0f );
	mMouseWheelDelta = 0;
	mLastKey = 0;

	mPressed.ClearArray();
	mReleased.ClearArray();
}


Vector DIInputSystem::GetMouseDelta() const
{
	if( mWin32Only )
	{
		return Vector( float( mMousePos.x - mCenterPos.x ) * mMouseSensitivity / 400.0f,
					   float( mMousePos.y - mCenterPos.y ) * mMouseSensitivity / 400.0f,
					   0.0f);
	}
	else
	{
		return mMouseDelta;
	}
}


void DIInputSystem::LockMouse( BOOL lock )
{
	if( mMouseLocked == lock )
		return;

	if( mWin32Only && lock && ( /*GEngine->Video->PMenu->Active() || */GEngine->Video->MBoard->Active() || GEngine->Video->MapSel->Active() ) )
		return;

	mMouseLocked = lock;

	if( lock )
		SwitchToDInput();
	else
		SwitchToWin32();

	if( mWin32Only )
	{
		if( mMouseLocked )
		{
			CenterMouse();
			ShowMouse( FALSE );
			CaptureMouse( TRUE );

			POINT p = mCenterPos;
			ClientToScreen( hWnd, &p );
			SetCursorPos( p.x, p.y );
		}
		else
		{
			ShowMouse( TRUE );
			CaptureMouse( FALSE );
			CenterMouse();
		}
	}

	Reset();
}


void DIInputSystem::ShowMouse( BOOL show )
{
	if( mMouseVisible == show ) return;

	if( mWin32Only )
	{
		mMouseVisible = show;
		ShowCursor( show );
	}
}


void DIInputSystem::CaptureMouse( BOOL capture )
{
	if( mWin32Only )
	{
		if( capture )
			SetCapture( hWnd );
		else
			ReleaseCapture();
	}
}


void DIInputSystem::SetWindow( HWND hwnd )
{
	InputSystem::SetWindow( hwnd );

	if( mWin32Only ) return;

	HRESULT hr;

	View *view = GEngine->Video->GetActiveView();
	if( !view )
	{
		SwitchToWin32();
		return;
	}

	hr = mMouseDev->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if( FAILED( hr ) )
	{
		GLog.Print( "Failed to set mouse cooperative level\n" );
		switch( hr )
		{
		case DIERR_INVALIDPARAM:
			GLog.Print( "Invalid param\n" );
			break;

		case DIERR_NOTINITIALIZED:
			GLog.Print( "Not initialized\n" );
			break;

		case E_HANDLE:
			GLog.Print( "Wrong window\n" );
			break;
		}
		SwitchToWin32();
		return;
	}

	hr = mKBDev->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if( FAILED( hr ) )
	{
		GLog.Print( "Failed to set keyboard cooperative level\n" );
		SwitchToWin32();
		return;
	}

	if( mGamePad )
	{
		hr = mGamePad->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to set gamepad cooperative level\n" );
			SwitchToWin32();
			return;
		}
	}

	hr = mMouseDev->Acquire();
	if( FAILED( hr ) )
	{
		while( hr == DIERR_INPUTLOST )
			hr = mMouseDev->Acquire();

		if( FAILED( hr ) )
		{
			if( hr == DIERR_OTHERAPPHASPRIO )
			{
				SetForegroundWindow( view->GetTopmostWindow() );
				hr = mMouseDev->Acquire();
				if( FAILED( hr ) )
				{
					while( hr == DIERR_INPUTLOST )
						hr = mMouseDev->Acquire();

					if( FAILED( hr ) )
					{
						GLog.Print( "SetWindow() - Cannot acquire mouse (0x%x)\n", hr );
						return;
					}
				}
			}
		}
	}

	GLog.Print( "Mouse acquired in DI mode\n" );

	hr = mKBDev->Acquire();
	if( FAILED( hr ) )
	{
		GLog.Print( "Failed to acquire keyboard\n" );
		SwitchToWin32();
		return;
	}

	if( mGamePad )
	{
		hr = mGamePad->Acquire();
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to acquire gamepad\n" );
			SwitchToWin32();
			return;
		}
	}
}


void DIInputSystem::SetMousePosition(int x, int y)
{
	if( mWin32Only )
	{
		mMousePos.x = x;
		mMousePos.y = y;
		ClientToScreen( hWnd, &mMousePos );
		SetCursorPos( mMousePos.x, mMousePos.y );
	}
}


void DIInputSystem::SetMouseSensitivity( float msens )
{
	mMouseSensitivity = msens;
}


void DIInputSystem::MouseTick()
{
	HRESULT hr;

	Vector mdel(0,0,0);

	while( 1 )
	{
		DIDEVICEOBJECTDATA od;
		DWORD elems = 1;

		hr = mMouseDev->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ), &od, &elems, 0 );
		if( hr == DI_BUFFEROVERFLOW )
		{
			GLog.Print( "DI Mouse buffer overflow\n" );
			Reset();
			return;
		}
		else if( FAILED( hr ) )
		{
			hr = mMouseDev->Acquire();
			while( hr == DIERR_INPUTLOST )
				hr = mMouseDev->Acquire();

			Reset();
			return;
		}

		if( elems == 0 )
			break;

		if( elems > 1 )
			GLog.Print( "%d items in mouse buffer\n", elems );

		switch( od.dwOfs )
		{
		case DIMOFS_X:
			mdel.X += ( float )( int )od.dwData;
			break;

		case DIMOFS_Y:
			mdel.Y += ( float )( int )od.dwData;
			break;

		case DIMOFS_Z:
#ifndef TRANSGAMING
			if( !GEngine->Video->Hud->mConsole.Active() )
			{
#endif
				mMouseWheelDelta += od.dwData;
#ifndef TRANSGAMING
			}
			else
			{
				mMouseWheelDelta = 0;
			}
#endif
			break;

		case DIMOFS_BUTTON0:
			if( od.dwData & 0x80 )
				mPressed.Add( EMouseKeyLeft );
			else
				mReleased.Add( EMouseKeyLeft );
			break;

		case DIMOFS_BUTTON1:
			if( od.dwData & 0x80 )
				mPressed.Add( EMouseKeyRight );
			else
				mReleased.Add( EMouseKeyRight );
			break;

		case DIMOFS_BUTTON2:
			if( od.dwData & 0x80 )
				mPressed.Add( EMouseKeyMiddle );
			else
				mReleased.Add( EMouseKeyMiddle );
			break;

		default:
			GLog.Print( "Unsupported mouse action in buffer: 0x%x\n", od.dwOfs );
			break;
		}
	}

	if( !GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active() )
	{
		if( mMouseSmooth )
		{
			mdel.X = ( mdel.X + mMouseDelta.X ) * 0.5f;
			mdel.Y = ( mdel.Y + mMouseDelta.Y ) * 0.5f;
		}

		mMouseDelta.X = mdel.X * mMouseSensitivity * mDIDeltaScale / 400.0f;
		mMouseDelta.Y = mdel.Y * mMouseSensitivity * mDIDeltaScale / 400.0f;
		mMousePos.x += ( int )mMouseDelta.X;
		mMousePos.y += ( int )mMouseDelta.Y;
	}
	else
	{
		mMouseDelta.X = mMouseDelta.Y = 0.0f;
		mMousePos.x += mdel.X * 1.5;
		mMousePos.y += mdel.Y * 1.5;
	}

	if( mMousePos.x < 0 )
		mMousePos.x = 0;
	if( mMousePos.y < 0 )
		mMousePos.y = 0;
	if( mMousePos.x >= GEngine->Video->GetActiveView()->LocalViewport.Width )
		mMousePos.x = GEngine->Video->GetActiveView()->LocalViewport.Width - 1;
	if( mMousePos.y >= GEngine->Video->GetActiveView()->LocalViewport.Height )
		mMousePos.y = GEngine->Video->GetActiveView()->LocalViewport.Height - 1;

	if( mMouseLocked )
	{
		mMousePos.x = GEngine->Video->GetActiveView()->LocalViewport.Width / 2;
		mMousePos.y = GEngine->Video->GetActiveView()->LocalViewport.Height / 2;
	}

	if( mWheelSensitivity <= 0.0f )
		mWheelSensitivity = 0.1f;

	if( mMouseWheelDelta == 0 )
	{
//		GLog.Print( "wheeldelta = 0\n" );
//		mReleased.Add( EMouseWheelForward );
//		mReleased.Add( EMouseWheelBack );
	}

	if( mMouseWheelDelta >= DIWHEEL_DELTA * mWheelSensitivity )
	{
		//GLog.Print( "wheeldelta > 0\n" );
		mPressed.Add( EMouseWheelForward );
		mMouseWheelDelta = 0;
	}

	if( mMouseWheelDelta <= -DIWHEEL_DELTA * mWheelSensitivity )
	{
		//GLog.Print( "wheeldelta < 0\n" );
		mPressed.Add( EMouseWheelBack );
		mMouseWheelDelta = 0;
	}
}


void DIInputSystem::KeyboardTick()
{
	DIDEVICEOBJECTDATA od[KB_BUFFER_SIZE];
	HRESULT hr;
	DWORD elems = KB_BUFFER_SIZE;
	static float lastTime = 0.0f;
	static float startTime = 0.0f;

	hr = mKBDev->GetDeviceData( sizeof( DIDEVICEOBJECTDATA ), od, &elems, 0 );
	if( hr == DI_BUFFEROVERFLOW )
	{
		GLog.Print( "DI Keyboard buffer overflow\n" );
		Reset();
		return;
	}
	else if( FAILED( hr ) )
	{
		while( hr == DIERR_INPUTLOST )
			hr = mKBDev->Acquire();

		Reset();
		return;
	}

	for( DWORD i=0; i<elems; i++ )
	{
		unsigned int keycode = TranslateKeyCode( od[i].dwOfs );
		if( od[i].dwData & 0x80 )
		{
			if( keycode <= 0 )
				return;
			else
				mPressed.Add( keycode );

			lastTime = 0.0f;
			startTime = 0.0f;
			mLastKey = keycode;
		}
		else
		{
			if( keycode <= 0 )
				return;
			else
				mReleased.Add( keycode );

			if( mLastKey == keycode )
			{
				lastTime = 0.0f;
				startTime = 0.0f;
				mLastKey = 0;
			}
		}
	}

	if( mLastKey == 0 ) return;

	float currTime = GEngine->SysDriver.GetCurrentTime();
	if( lastTime == 0.0f )
	{
		lastTime = currTime;
		startTime = currTime;
	}

	if( ( currTime - startTime > KEY_REPEAT_DELAY ) && ( currTime - lastTime > KEY_REPEAT ) )
	{
#ifndef TRANSGAMING
		if( !GEngine->Video->Hud->mConsole.Active() )
#endif
			ActionStatus|=(1<<KeyBindings[mLastKey].action);
#ifdef TRANSGAMING
		SendEvent( EEventKeyDown,mLastKey );
#else
		SendEvent(InputEvent(EEventKeyDown,mLastKey));
#endif
		lastTime = currTime;
	}
}


void DIInputSystem::GamePadTick()
{
	if( !mGamePad )
		return;

	HRESULT hr = mGamePad->Poll();
	if( FAILED( hr ) )
	{
		hr = mGamePad->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = mGamePad->Acquire();
		return;
	}

	DIJOYSTATE2 js;
	hr = mGamePad->GetDeviceState( sizeof( DIJOYSTATE2 ), &js );
	if( FAILED( hr ) )
		return;

	if( js.lZ < GAMEPAD_DEADZONE && js.lZ > -GAMEPAD_DEADZONE ) js.lZ = 0;
	mLookDelta.X = ( float )js.lZ * 0.001f;
	if( js.lRz < GAMEPAD_DEADZONE && js.lRz > -GAMEPAD_DEADZONE ) js.lRz = 0;
	mLookDelta.Y = ( float )js.lRz * 0.001f;
	if( js.lX < GAMEPAD_DEADZONE && js.lX > -GAMEPAD_DEADZONE ) js.lX = 0;
	mMoveDelta.X = ( float )js.lX * 0.001f;
	if( js.lY < GAMEPAD_DEADZONE && js.lY > -GAMEPAD_DEADZONE ) js.lY = 0;
	mMoveDelta.Y = ( float )js.lY * 0.001f;

	mMouseDelta = mLookDelta * 3.0f;

	for( int i = 0; i < 12; i++ )
	{
		if( js.rgbButtons[i] & 0x80 )
		{
			if( !mButtonsState[i] )
			{
				mButtonsState[i] = true;
				mPressed.Add( i );
			}
		}
		else
		{
			if( mButtonsState[i] )
			{
				mButtonsState[i] = false;
				mReleased.Add( i );
			}
		}
	}

	int pov = js.rgdwPOV[0];
	bool down[4] = { false, false, false, false };
	switch( pov )
	{
	case 0:
		down[0] = true;
		break;

	case 4500:
		down[0] = true;
		down[1] = true;
		break;

	case 9000:
		down[1] = true;
		break;

	case 13500:
		down[1] = true;
		down[2] = true;
		break;

	case 18000:
		down[2] = true;
		break;

	case 22500:
		down[2] = true;
		down[3] = true;
		break;

	case 27000:
		down[3] = true;
		break;

	case 31500:
		down[3] = true;
		down[0] = true;
		break;
	}

	if( mMoveDelta.Y > 0.0f )
		down[2] = true;
	else if( mMoveDelta.Y < 0.0f )
		down[0] = true;

	static float lastRepeat = 0.0f;

	for( int i = 0; i < 4; i++ )
	{
		if( down[i] )
		{
			if( !mButtonsState[12 + i] || ( GEngine->SysDriver.GetCurrentTime() - lastRepeat > 0.2f ) )
			{
				mButtonsState[12 + i] = true;
				mPressed.Add( 12 + i );
				lastRepeat = GEngine->SysDriver.GetCurrentTime();
			}
		}
		else
		{
			if( mButtonsState[12 + i] )
			{
				mButtonsState[12 + i] = false;
				mReleased.Add( 12 + i );
			}
		}
	}

	ActionStatus &= ~( 1<<EInputMoveForward );
	ActionStatus &= ~( 1<<EInputMoveBackward );
	ActionStatus &= ~( 1<<EInputMoveLeft );
	ActionStatus &= ~( 1<<EInputMoveRight );

	if( mMoveDelta.Y > 0.15f )
		ActionStatus |= ( 1 << EInputMoveBackward );
	else if( mMoveDelta.Y < -0.15f )
		ActionStatus |= ( 1 << EInputMoveForward );
	
	if( mMoveDelta.X > 0.15f )
		ActionStatus |= ( 1 << EInputMoveRight );
	else if( mMoveDelta.X < -0.15f )
		ActionStatus |= ( 1 << EInputMoveLeft );
}


void DIInputSystem::SwitchToDInput()
{
	if( !mUseDInput )
		return;

	SetForegroundWindow( GEngine->Video->GetActiveView()->GetTopmostWindow() );

	if( !mWin32Only ) return;

	if( SetupDInput() )
		mWin32Only = false;
}


void DIInputSystem::SwitchToWin32()
{
	if( mWin32Only ) return;

	SetForegroundWindow( GEngine->Video->GetActiveView()->GetTopmostWindow() );

	mMouseDev->Unacquire();
	mKBDev->Unacquire();

	if( GEngine->Video->PMenu->Active() || GEngine->Video->MapSel->Active() || GEngine->Video->MBoard->Active() )
	{
		LockMouse( FALSE );
		ShowMouse( FALSE );
	}

	mWin32Only = true;
}


bool DIInputSystem::SetupDInput()
{
	HRESULT hr;
#ifndef TRANSGAMING
	if( !mDI )
	{
		hr = DirectInput8Create( GEngine->hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
								( void **)&mDI, NULL );
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to create DirectInput8 object\n" );
			return false;
		}
	}
#endif

	if( !SetupMouse() )
	{
		return false;
	}

	if( !SetupKeyboard() )
	{
		return false;
	}

	if( GScript.Globals()["Cfg"]["UseGamepad"].GetBool() )
		SetupGamePad();

	return true;
}


void DIInputSystem::Release()
{
	if( mDI )
	{
		if( mMouseDev )
		{
			mMouseDev->Unacquire();
			mMouseDev->Release();
			mMouseDev = NULL;
		}

		if( mKBDev )
		{
			mKBDev->Unacquire();
			mKBDev->Release();
			mKBDev = NULL;
		}

		if( mGamePad )
		{
			mGamePad->Unacquire();
			mGamePad->Release();
			mGamePad = NULL;
		}

		mDI->Release();
		mDI = NULL;
	}

	if( mMouseEvent )
	{
		CloseHandle( mMouseEvent );
		mMouseEvent = NULL;
	}

	if( !mMouseVisible && mWin32Only )
	   ShowCursor( TRUE );
}


bool DIInputSystem::SetupMouse()
{
	HRESULT hr;
	if( !mMouseDev )
	{
#ifndef TRANSGAMING
		hr = mDI->CreateDevice( GUID_SysMouse, &mMouseDev, NULL );
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to create mouse device\n" );
			return false;
		}

		hr = mMouseDev->SetDataFormat( &c_dfDIMouse );
#endif
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to set mouse data format\n" );
			return false;
		}
	}

	View *view = GEngine->Video->GetActiveView();
	if( !view )
		return false;

#if DIEXCLUSIVE
	hr = mMouseDev->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
#else
	hr = mMouseDev->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
#endif
	if( FAILED( hr ) )
	{
		GLog.Print( "Failed to set mouse cooperative level\n" );
		return false;
	}

	if( !mMouseEvent )
	{
		mMouseEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		if( mMouseEvent == NULL )
		{
			GLog.Print( "Failed to create mouse event\n" );
			return false;
		}

		hr = mMouseDev->SetEventNotification( mMouseEvent );
		if( FAILED( hr ) )
		{
			GLog.Print( "Cannot set mouse event notification\n" );
			return false;
		}
	}

	DIPROPDWORD prop;
	prop.diph.dwSize = sizeof( DIPROPDWORD );
	prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = 1024;

	hr = mMouseDev->SetProperty( DIPROP_BUFFERSIZE, &prop.diph );
	if( FAILED( hr ) )
	{
		GLog.Print( "Cannot set buffer for mouse events\n" );
		return false;
	}

	hr = mMouseDev->Acquire();
	if( FAILED( hr ) )
	{
		while( hr == DIERR_INPUTLOST )
			hr = mMouseDev->Acquire();

		if( FAILED( hr ) )
		{
			if( hr == DIERR_OTHERAPPHASPRIO )
			{
				SetForegroundWindow( view->GetTopmostWindow() );
				hr = mMouseDev->Acquire();
				if( FAILED( hr ) )
				{
					while( hr == DIERR_INPUTLOST )
						hr = mMouseDev->Acquire();

					if( FAILED( hr ) )
					{
						GLog.Print( "SetupMouse() - Cannot acquire mouse (0x%x)\n", hr );
						return false;
					}
				}
			}
		}
	}

	GLog.Print( "Mouse acquired in DI mode\n" );

	return true;
}


bool DIInputSystem::SetupKeyboard()
{
	HRESULT hr;

	if( !mKBDev )
	{
#ifndef TRANSGAMING
		hr = mDI->CreateDevice( GUID_SysKeyboard, &mKBDev, NULL );
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to create keyboard device\n" );
			return false;
		}

		hr = mKBDev->SetDataFormat( &c_dfDIKeyboard );
#endif
		if( FAILED( hr ) )
		{
			GLog.Print( "Failed to set keyboard data format\n" );
			return false;
		}
	}

	View *view = GEngine->Video->GetActiveView();
	if( !view )
		return false;

	hr = mKBDev->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if( FAILED( hr ) )
	{
		GLog.Print( "Failed to set keyboard cooperative level\n" );
		return false;
	}

	DIPROPDWORD prop;
	prop.diph.dwSize = sizeof( DIPROPDWORD );
	prop.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = KB_BUFFER_SIZE;

	hr = mKBDev->SetProperty( DIPROP_BUFFERSIZE, &prop.diph );
	if( FAILED( hr ) )
	{
		GLog.Print( "Cannot set buffer for keyboard events\n" );
		return false;
	}

	hr = mKBDev->Acquire();
	if( FAILED( hr ) )
	{
		GLog.Print( "Cannot acquire keyboard\n" );
		return false;
	}

	return true;
}


static IDirectInput8 *gDI = NULL;


BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* inst, void* context )
{
	HRESULT hr;

	IDirectInputDevice8 **pad = ( IDirectInputDevice8 **)context;

	hr = gDI->CreateDevice( inst->guidInstance, pad, NULL );
	if( FAILED( hr ) )
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}


BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, void* context )
{
	IDirectInputDevice8 *pad = ( IDirectInputDevice8 *)context;

	if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize       = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = pdidoi->dwType;
        diprg.lMin              = -1000;
        diprg.lMax              = +1000;

        if( FAILED( pad->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
            return DIENUM_STOP;
    }

	return DIENUM_CONTINUE;
}


bool DIInputSystem::SetupGamePad()
{
	HRESULT hr;

	gDI = mDI;

	hr = mDI->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, &mGamePad, DIEDFL_ATTACHEDONLY );
	if( FAILED( hr ) )
		return false;

	if( !mGamePad )
		return false;

#ifndef TRANSGAMING
	hr = mGamePad->SetDataFormat( &c_dfDIJoystick2 );
#endif
	if( FAILED( hr ) )
		return false;

	View *view = GEngine->Video->GetActiveView();
	if( !view )
		return false;

	hr = mGamePad->SetCooperativeLevel( view->GetTopmostWindow(), DISCL_EXCLUSIVE | DISCL_FOREGROUND );
	if( FAILED( hr ) )
		return false;

	hr = mGamePad->EnumObjects( EnumObjectsCallback, ( void *)mGamePad, DIDFT_ALL );
	if( FAILED( hr ) )
		return false;

	gDI = NULL;

#ifndef TRANSGAMING
	// Default bindings
	BindKey( 0, EPadButton1 );
	BindKey( 0, EPadButton2 );
	BindKey( EInputNextWeapon, EPadButton3 );
	BindKey( EInputPrevWeapon, EPadButton4 );
	BindKey( EInputAltFire, EPadTriggerLeft1 );
//	BindKey( EInputRun, EPadButton1 );
	BindKey( EInputFire, EPadTriggerRight1 );
//	BindKey( EInputAutoAim, EPadButtonRight3 );	
//	BindKey( EInputPunch, EPadButton4 );	
//	BindKey( EInputKick, EPadButton3 );	
//	BindKey( EInputSpecial1, EPadSelect );	
//	BindKey( EInputSpecial2, EPadStart );

	BindKey( EInputNextWeapon, EPadTriggerLeft2 );
	BindKey( EInputJump, EPadTriggerRight2 );
//	BindKey( 0, EPadSelect );
//	BindKey( 0, EPadStart );
//	BindKey( 0, EPadButtonLeft3 );
//	BindKey( 0, EPadButtonRight3 );
/*
	BindKey( EInputMoveLeft, EPadDPadLeft );
	BindKey( EInputMoveRight, EPadDPadRight );
	BindKey( EInputMoveForward, EPadDPadUp );
	BindKey( EInputMoveBackward, EPadDPadDown );
*/
#endif // TRANSGAMING

	return true;
}


unsigned int DIInputSystem::TranslateKeyCode( unsigned int code )
{
	HKL layout = GetKeyboardLayout( 0 );
	return TranslateKeyCode( mDIKeys[code], MapVirtualKeyEx( mDIKeys[code], 0, layout ) );
}


bool DIInputSystem::IsKeyPrintable( unsigned char key )
{
	if( !mWin32Only && !mKBDev )
		return false;

	static char dibuf[256];
	static BYTE vkbuf[256];

	if( mWin32Only )
	{
		GetKeyboardState( vkbuf );
	}
	else
	{
		HRESULT hr = mKBDev->GetDeviceState( sizeof( dibuf ), ( LPVOID )&dibuf );
		if( FAILED( hr ) )
			return false;

		memset( vkbuf, 0, 256 );

		for( int i=0; i<256; i++ )
		{
			vkbuf[mDIKeys[i]] = dibuf[i];
		}
	}

	HKL layout=GetKeyboardLayout( 0 );
	UINT scan = MapVirtualKeyEx( key, 0, layout );

	WORD ret;
	if( ToAsciiEx( key, scan, vkbuf, &ret, 0, layout ) )
		return true;

	return false;
}


unsigned char DIInputSystem::KeyToPrintable( unsigned char key )
{
	if( !mWin32Only && !mKBDev )
		return 0;

	static char dibuf[256];
	static BYTE vkbuf[256];

	if( mWin32Only )
	{
		GetKeyboardState( vkbuf );
	}
	else
	{
		HRESULT hr = mKBDev->GetDeviceState( sizeof( dibuf ), ( LPVOID )&dibuf );
		if( FAILED( hr ) )
			return false;

		memset( vkbuf, 0, 256 );

		for( int i=0; i<256; i++ )
		{
			vkbuf[mDIKeys[i]] = dibuf[i];
		}

		if( dibuf[DIK_LSHIFT] & 0x80 || dibuf[DIK_RSHIFT] & 0x80 )
		{
			vkbuf[VK_SHIFT] = 0x80;
		}

		if( dibuf[DIK_LCONTROL] & 0x80 || dibuf[DIK_RCONTROL] & 0x80 )
		{
			vkbuf[VK_CONTROL] = 0x80;
		}
	}

	HKL layout=GetKeyboardLayout( 0 );
	UINT scan = MapVirtualKeyEx( key, 0, layout );

	WORD ret = 0;
	ToAsciiEx( key, scan, vkbuf, &ret, 0, layout );

	return ret & 0x00FF;
}


void DIInputSystem::FillKeyNamesTable( void )
{
	ScriptObject txt = GScript.Globals()["TXT"];
	String unknown = txt["None"].GetString();

	char temp[10];
	for( int i = 0; i < 256; ++i ) KeyNames[i] = unknown;	// Fill whole table with default string
	for( int i = '0'; i <= '9'; i++ ) { sprintf(temp, "%c", i ); KeyNames[i] = temp; };	// Set numbers
	for( int i = 'A'; i <= 'Z'; i++ ) { sprintf(temp, "%c", i ); KeyNames[i] = temp; };	// Set letters

	KeyNames[VK_UP] = txt["Keys"]["CursorUp"].GetString();
	KeyNames[VK_DOWN] = txt["Keys"]["CursorDown"].GetString();
	KeyNames[VK_LEFT] = txt["Keys"]["CursorLeft"].GetString();
	KeyNames[VK_RIGHT] = txt["Keys"]["CursorRight"].GetString();
	KeyNames[VK_SPACE] = txt["Keys"]["Space"].GetString();
	KeyNames[VK_TAB] = txt["Keys"]["Tab"].GetString();
	KeyNames[VK_BACK] = txt["Keys"]["Backspace"].GetString();
	KeyNames[VK_RETURN] = txt["Keys"]["Return"].GetString();
	KeyNames[VK_PAUSE] = txt["Keys"]["Pause"].GetString();
	KeyNames[VK_SHIFT] = txt["Keys"]["Shift"].GetString();
	KeyNames[VK_CONTROL] = txt["Keys"]["Control"].GetString();
	KeyNames[VK_ESCAPE] = txt["Keys"]["Escape"].GetString();
	KeyNames[VK_PRIOR] = txt["Keys"]["PageUp"].GetString();
	KeyNames[VK_NEXT] = txt["Keys"]["PageDown"].GetString();
	KeyNames[VK_END] = txt["Keys"]["End"].GetString();
	KeyNames[VK_HOME] = txt["Keys"]["Home"].GetString();
	KeyNames[VK_INSERT] = txt["Keys"]["Insert"].GetString();
	KeyNames[VK_DELETE] = txt["Keys"]["Delete"].GetString();

	KeyNames[VK_NUMPAD0] = txt["Keys"]["Numpad0"].GetString();
	KeyNames[VK_NUMPAD1] = txt["Keys"]["Numpad1"].GetString();
	KeyNames[VK_NUMPAD2] = txt["Keys"]["Numpad2"].GetString();
	KeyNames[VK_NUMPAD3] = txt["Keys"]["Numpad3"].GetString();
	KeyNames[VK_NUMPAD4] = txt["Keys"]["Numpad4"].GetString();
	KeyNames[VK_NUMPAD5] = txt["Keys"]["Numpad5"].GetString();
	KeyNames[VK_NUMPAD6] = txt["Keys"]["Numpad6"].GetString();
	KeyNames[VK_NUMPAD7] = txt["Keys"]["Numpad7"].GetString();
	KeyNames[VK_NUMPAD8] = txt["Keys"]["Numpad8"].GetString();
	KeyNames[VK_NUMPAD9] = txt["Keys"]["Numpad9"].GetString();
	KeyNames[VK_MULTIPLY] = txt["Keys"]["NumpadMultiply"].GetString();
	KeyNames[VK_ADD] = txt["Keys"]["NumpadAdd"].GetString();
	KeyNames[VK_SUBTRACT] = txt["Keys"]["NumpadMinus"].GetString();
	KeyNames[VK_DECIMAL] = txt["Keys"]["NumpadDecimal"].GetString();
	KeyNames[VK_DIVIDE] = txt["Keys"]["NumpadDivide"].GetString();

	KeyNames[VK_NUMLOCK] = txt["Keys"]["NumLock"].GetString();
	KeyNames[VK_SCROLL] = txt["Keys"]["ScrollLock"].GetString();
	KeyNames[VK_LSHIFT] = txt["Keys"]["LeftShift"].GetString();
	KeyNames[VK_RSHIFT] = txt["Keys"]["RightShift"].GetString();
	KeyNames[VK_LCONTROL] = txt["Keys"]["LeftCtrl"].GetString();
	KeyNames[VK_RCONTROL] = txt["Keys"]["RightCtrl"].GetString();
    KeyNames[VK_LMENU] = txt["Keys"]["LeftAlt"].GetString();
    KeyNames[VK_RMENU] = txt["Keys"]["RightAlt"].GetString();
//	KeyNames[VK_MENU] = "Left Alt";

	KeyNames[91] = txt["Keys"]["LeftMenu"].GetString();
	KeyNames[92] = txt["Keys"]["RightMenu"].GetString();
	KeyNames[93] = txt["Keys"]["FarRightMenu"].GetString();
	KeyNames[44] = txt["Keys"]["PrintScreen"].GetString();

	KeyNames[219] = "[";
	KeyNames[221] = "]";
	KeyNames[186] = ";";
	KeyNames[222] = "'";
	KeyNames[189] = "-";
	KeyNames[187] = "=";
	KeyNames[220] = "\\";
	KeyNames[VK_GRAVE] = "~";
	KeyNames[20] = txt["Keys"]["CapsLock"].GetString();
	KeyNames[188] = ",";
	KeyNames[190] = ".";
	KeyNames[191] = "/";
	KeyNames[EVirKeyNumlockEnter] = txt["Keys"]["NumpadEnter"].GetString();
	KeyNames[EMouseKeyLeft] = txt["Keys"]["LeftMouseButton"].GetString();
	KeyNames[EMouseKeyRight] = txt["Keys"]["RightMouseButton"].GetString();
	KeyNames[EMouseKeyMiddle] = txt["Keys"]["MiddleMouseButton"].GetString();
	KeyNames[EMouseWheelForward] = txt["Keys"]["MouseWheelForward"].GetString();
	KeyNames[EMouseWheelBack] = txt["Keys"]["MouseWheelBack"].GetString();

	KeyNames[VK_F1] = "F1";
	KeyNames[VK_F2] = "F2";
	KeyNames[VK_F3] = "F3";
	KeyNames[VK_F4] = "F4";
	KeyNames[VK_F5] = "F5";
	KeyNames[VK_F6] = "F6";
	KeyNames[VK_F7] = "F7";
	KeyNames[VK_F8] = "F8";
	KeyNames[VK_F9] = "F9";
	KeyNames[VK_F10] = "F10";
	KeyNames[VK_F11] = "F11";
	KeyNames[VK_F12] = "F12";


	for( int i = 0; i < 256; ++i ) KeyNamesEng[i] = "None";	// Fill whole table with default string
	for( int i = '0'; i <= '9'; i++ ) { sprintf(temp, "%c", i ); KeyNamesEng[i] = temp; };	// Set numbers
	for( int i = 'A'; i <= 'Z'; i++ ) { sprintf(temp, "%c", i ); KeyNamesEng[i] = temp; };	// Set letters
	KeyNamesEng[VK_UP] = "Cursor Up";
	KeyNamesEng[VK_DOWN] = "Cursor Down";
	KeyNamesEng[VK_LEFT] = "Cursor Left";
	KeyNamesEng[VK_RIGHT] = "Cursor Right";
	KeyNamesEng[VK_SPACE] = "Space";
	KeyNamesEng[VK_TAB] = "Tab";
	KeyNamesEng[VK_BACK] = "Backspace";
	KeyNamesEng[VK_RETURN] = "Return";
	KeyNamesEng[VK_PAUSE] = "Pause";
	KeyNamesEng[VK_SHIFT] = "Shift";
	KeyNamesEng[VK_CONTROL] = "Control";
	KeyNamesEng[VK_ESCAPE] = "Escape";
	KeyNamesEng[VK_PRIOR] = "Page Up";
	KeyNamesEng[VK_NEXT] = "Page Down";
	KeyNamesEng[VK_END] = "End";
	KeyNamesEng[VK_HOME] = "Home";
	KeyNamesEng[VK_INSERT] = "Insert";
	KeyNamesEng[VK_DELETE] = "Delete";
	KeyNamesEng[VK_NUMPAD0] = "Numpad 0";
	KeyNamesEng[VK_NUMPAD1] = "Numpad 1";
	KeyNamesEng[VK_NUMPAD2] = "Numpad 2";
	KeyNamesEng[VK_NUMPAD3] = "Numpad 3";
	KeyNamesEng[VK_NUMPAD4] = "Numpad 4";
	KeyNamesEng[VK_NUMPAD5] = "Numpad 5";
	KeyNamesEng[VK_NUMPAD6] = "Numpad 6";
	KeyNamesEng[VK_NUMPAD7] = "Numpad 7";
	KeyNamesEng[VK_NUMPAD8] = "Numpad 8";
	KeyNamesEng[VK_NUMPAD9] = "Numpad 9";
	KeyNamesEng[VK_MULTIPLY] = "Numpad *";
	KeyNamesEng[VK_ADD] = "Numpad +";
	KeyNamesEng[VK_SUBTRACT] = "Numpad -";
	KeyNamesEng[VK_DECIMAL] = "Numpad .";
	KeyNamesEng[VK_DIVIDE] = "Numpad /";
	KeyNamesEng[VK_F1] = "F1";
	KeyNamesEng[VK_F2] = "F2";
	KeyNamesEng[VK_F3] = "F3";
	KeyNamesEng[VK_F4] = "F4";
	KeyNamesEng[VK_F5] = "F5";
	KeyNamesEng[VK_F6] = "F6";
	KeyNamesEng[VK_F7] = "F7";
	KeyNamesEng[VK_F8] = "F8";
	KeyNamesEng[VK_F9] = "F9";
	KeyNamesEng[VK_F10] = "F10";
	KeyNamesEng[VK_F11] = "F11";
	KeyNamesEng[VK_F12] = "F12";
	KeyNamesEng[VK_NUMLOCK] = "NumLock";
	KeyNamesEng[VK_SCROLL] = "ScrollLock";
	KeyNamesEng[VK_LSHIFT] = "Left Shift";
	KeyNamesEng[VK_RSHIFT] = "Right Shift";
	KeyNamesEng[VK_LCONTROL] = "Left Ctrl";
	KeyNamesEng[VK_RCONTROL] = "Right Ctrl";
    KeyNamesEng[VK_LMENU] = "Left Alt";
    KeyNamesEng[VK_RMENU] = "Right Alt";
//	KeyNamesEng[VK_MENU] = "Left Alt";
	KeyNamesEng[91] = "Left Menu";
	KeyNamesEng[92] = "Right Menu";
	KeyNamesEng[93] = "Far Right Menu";
	KeyNamesEng[44] = "PrintScreen";
	KeyNamesEng[219] = "[";
	KeyNamesEng[221] = "]";
	KeyNamesEng[186] = ";";
	KeyNamesEng[222] = "'";
	KeyNamesEng[189] = "-";
	KeyNamesEng[187] = "=";
	KeyNamesEng[220] = "\\";
	KeyNamesEng[VK_GRAVE] = "~";
	KeyNamesEng[20] = "Caps Lock";
	KeyNamesEng[188] = ",";
	KeyNamesEng[190] = ".";
	KeyNamesEng[191] = "/";
	KeyNamesEng[EVirKeyNumlockEnter] = "Numpad Enter";
	KeyNamesEng[EMouseKeyLeft] = "Left Mouse Button";
	KeyNamesEng[EMouseKeyRight] = "Right Mouse Button";
	KeyNamesEng[EMouseKeyMiddle] = "Middle Mouse Button";
	KeyNamesEng[EMouseWheelForward] = "Mouse Wheel Forward";
	KeyNamesEng[EMouseWheelBack] = "Mouse Wheel Back";

	for( int i = 0; i < 256; ++i ) KeyShortNames[i] = "None";	// Fill whole table with default string
	for( int i = '0'; i <= '9'; i++ ) { sprintf(temp, "%c", i ); KeyShortNames[i] = temp; };	// Set numbers
	for( int i = 'A'; i <= 'Z'; i++ ) { sprintf(temp, "%c", i ); KeyShortNames[i] = temp; };	// Set letters
	KeyShortNames[VK_UP] = "Up";
	KeyShortNames[VK_DOWN] = "Down";
	KeyShortNames[VK_LEFT] = "Left";
	KeyShortNames[VK_RIGHT] = "Right";
	KeyShortNames[VK_SPACE] = "Space";
	KeyShortNames[VK_TAB] = "Tab";
	KeyShortNames[VK_BACK] = "Backspace";
	KeyShortNames[VK_RETURN] = "Return";
	KeyShortNames[VK_PAUSE] = "Pause";
	KeyShortNames[VK_SHIFT] = "Shift";
	KeyShortNames[VK_CONTROL] = "Control";
	KeyShortNames[VK_ESCAPE] = "Escape";
	KeyShortNames[VK_PRIOR] = "PgUp";
	KeyShortNames[VK_NEXT] = "PgDown";
	KeyShortNames[VK_END] = "End";
	KeyShortNames[VK_HOME] = "Home";
	KeyShortNames[VK_INSERT] = "Insert";
	KeyShortNames[VK_DELETE] = "Delete";
	KeyShortNames[VK_NUMPAD0] = "Numpad0";
	KeyShortNames[VK_NUMPAD1] = "Numpad1";
	KeyShortNames[VK_NUMPAD2] = "Numpad2";
	KeyShortNames[VK_NUMPAD3] = "Numpad3";
	KeyShortNames[VK_NUMPAD4] = "Numpad4";
	KeyShortNames[VK_NUMPAD5] = "Numpad5";
	KeyShortNames[VK_NUMPAD6] = "Numpad6";
	KeyShortNames[VK_NUMPAD7] = "Numpad7";
	KeyShortNames[VK_NUMPAD8] = "Numpad8";
	KeyShortNames[VK_NUMPAD9] = "Numpad9";
	KeyShortNames[VK_MULTIPLY] = "NumpadMulti";
	KeyShortNames[VK_ADD] = "NumpadAdd";
	KeyShortNames[VK_SUBTRACT] = "NumpadSub";
	KeyShortNames[VK_DECIMAL] = "NumpadDec";
	KeyShortNames[VK_DIVIDE] = "NumpadDiv";
	KeyShortNames[VK_F1] = "F1";
	KeyShortNames[VK_F2] = "F2";
	KeyShortNames[VK_F3] = "F3";
	KeyShortNames[VK_F4] = "F4";
	KeyShortNames[VK_F5] = "F5";
	KeyShortNames[VK_F6] = "F6";
	KeyShortNames[VK_F7] = "F7";
	KeyShortNames[VK_F8] = "F8";
	KeyShortNames[VK_F9] = "F9";
	KeyShortNames[VK_F10] = "F10";
	KeyShortNames[VK_F11] = "F11";
	KeyShortNames[VK_F12] = "F12";
	KeyShortNames[VK_NUMLOCK] = "NumLock";
	KeyShortNames[VK_SCROLL] = "ScrollLock";
	KeyShortNames[VK_LSHIFT] = "LShift";
	KeyShortNames[VK_RSHIFT] = "RShift";
	KeyShortNames[VK_LCONTROL] = "LCtrl";
	KeyShortNames[VK_RCONTROL] = "RCtrl";
    KeyShortNames[VK_LMENU] = "LAlt";
    KeyShortNames[VK_RMENU] = "RAlt";
//	KeyShortNames[VK_MENU] = "Left Alt";
	KeyShortNames[VK_LWIN] = "LMenu";
	KeyShortNames[VK_RWIN] = "RMenu";
	KeyShortNames[VK_APPS] = "FRMenu";
	KeyShortNames[VK_SNAPSHOT] = "PrintScreen";
	KeyShortNames[219] = "LBracket";	// [
	KeyShortNames[221] = "RBracket";	// ]
	KeyShortNames[186] = "Semicolon";	// ;
	KeyShortNames[222] = "Quote";		// '
	KeyShortNames[189] = "Minus";		// -
	KeyShortNames[187] = "Plus";		// =
	KeyShortNames[220] = "BSlash";		// \\ 
	KeyShortNames[VK_GRAVE] = "Tilde";		// ~
	KeyShortNames[20] = "CapsLock";
	KeyShortNames[188] = "Comma";		// ,
	KeyShortNames[190] = "Period";		// .
	KeyShortNames[191] = "Slash";		// /
	KeyShortNames[EVirKeyNumlockEnter] = "NumpadEnter";
	KeyShortNames[EMouseKeyLeft] = "LMB";
	KeyShortNames[EMouseKeyRight] = "RMB";
	KeyShortNames[EMouseKeyMiddle] = "MMB";
	KeyShortNames[EMouseWheelForward] = "WheelFwd";
	KeyShortNames[EMouseWheelBack] = "WheelBack";

	ScriptObject cfg = GScript.Globals()["Cfg"];
	if( !cfg["MouseSensitivity"].IsNil() )
		mMouseSensitivity = cfg["MouseSensitivity"].GetFloat();
}


void DIInputSystem::FillDIKeyTable()
{
	memset( mDIKeys, 0, 256 );

	mDIKeys[DIK_0]			= '0';
	mDIKeys[DIK_1]			= '1';
	mDIKeys[DIK_2]			= '2';
	mDIKeys[DIK_3]			= '3';
	mDIKeys[DIK_4]			= '4';
	mDIKeys[DIK_5]			= '5';
	mDIKeys[DIK_6]			= '6';
	mDIKeys[DIK_7]			= '7';
	mDIKeys[DIK_8]			= '8';
	mDIKeys[DIK_9]			= '9';
	mDIKeys[DIK_A]			= 'A';
	mDIKeys[DIK_B]			= 'B';
	mDIKeys[DIK_C]			= 'C';
	mDIKeys[DIK_D]			= 'D';
	mDIKeys[DIK_E]			= 'E';
	mDIKeys[DIK_F]			= 'F';
	mDIKeys[DIK_G]			= 'G';
	mDIKeys[DIK_H]			= 'H';
	mDIKeys[DIK_I]			= 'I';
	mDIKeys[DIK_J]			= 'J';
	mDIKeys[DIK_K]			= 'K';
	mDIKeys[DIK_L]			= 'L';
	mDIKeys[DIK_M]			= 'M';
	mDIKeys[DIK_N]			= 'N';
	mDIKeys[DIK_O]			= 'O';
	mDIKeys[DIK_P]			= 'P';
	mDIKeys[DIK_Q]			= 'Q';
	mDIKeys[DIK_R]			= 'R';
	mDIKeys[DIK_S]			= 'S';
	mDIKeys[DIK_T]			= 'T';
	mDIKeys[DIK_U]			= 'U';
	mDIKeys[DIK_V]			= 'V';
	mDIKeys[DIK_W]			= 'W';
	mDIKeys[DIK_X]			= 'X';
	mDIKeys[DIK_Y]			= 'Y';
	mDIKeys[DIK_Z]			= 'Z';

	mDIKeys[DIK_UP]			= VK_UP;
	mDIKeys[DIK_DOWN]		= VK_DOWN;
	mDIKeys[DIK_LEFT]		= VK_LEFT;
	mDIKeys[DIK_RIGHT]		= VK_RIGHT;
	mDIKeys[DIK_SPACE]		= VK_SPACE;
	mDIKeys[DIK_TAB]		= VK_TAB;
	mDIKeys[DIK_BACK]		= VK_BACK;
	mDIKeys[DIK_NUMPADENTER]= EVirKeyNumlockEnter;
	mDIKeys[DIK_RETURN]		= VK_RETURN;
	mDIKeys[DIK_PAUSE]		= VK_PAUSE;
	mDIKeys[DIK_ESCAPE]		= VK_ESCAPE;
	mDIKeys[DIK_PRIOR]		= VK_PRIOR;
	mDIKeys[DIK_NEXT]		= VK_NEXT;
	mDIKeys[DIK_END]		= VK_END;
	mDIKeys[DIK_HOME]		= VK_HOME;
	mDIKeys[DIK_INSERT]		= VK_INSERT;
	mDIKeys[DIK_DELETE]		= VK_DELETE;
	mDIKeys[DIK_NUMPAD0]	= VK_NUMPAD0;
	mDIKeys[DIK_NUMPAD1]	= VK_NUMPAD1;
	mDIKeys[DIK_NUMPAD2]	= VK_NUMPAD2;
	mDIKeys[DIK_NUMPAD3]	= VK_NUMPAD3;
	mDIKeys[DIK_NUMPAD4]	= VK_NUMPAD4;
	mDIKeys[DIK_NUMPAD5]	= VK_NUMPAD5;
	mDIKeys[DIK_NUMPAD6]	= VK_NUMPAD6;
	mDIKeys[DIK_NUMPAD7]	= VK_NUMPAD7;
	mDIKeys[DIK_NUMPAD8]	= VK_NUMPAD8;
	mDIKeys[DIK_NUMPAD9]	= VK_NUMPAD9;
	mDIKeys[DIK_MULTIPLY]	= VK_MULTIPLY;
	mDIKeys[DIK_ADD]		= VK_ADD;
	mDIKeys[DIK_SUBTRACT]	= VK_SUBTRACT;
	mDIKeys[DIK_DECIMAL]	= VK_DECIMAL;
	mDIKeys[DIK_DIVIDE]		= VK_DIVIDE;

	mDIKeys[DIK_F1]			= VK_F1;
	mDIKeys[DIK_F2]			= VK_F2;
	mDIKeys[DIK_F3]			= VK_F3;
	mDIKeys[DIK_F4]			= VK_F4;
	mDIKeys[DIK_F5]			= VK_F5;
	mDIKeys[DIK_F6]			= VK_F6;
	mDIKeys[DIK_F7]			= VK_F7;
	mDIKeys[DIK_F8]			= VK_F8;
	mDIKeys[DIK_F9]			= VK_F9;
	mDIKeys[DIK_F10]		= VK_F10;
	mDIKeys[DIK_F11]		= VK_F11;
	mDIKeys[DIK_F12]		= VK_F12;

	mDIKeys[DIK_NUMLOCK]	= VK_NUMLOCK;
	mDIKeys[DIK_SCROLL]		= VK_SCROLL;
	mDIKeys[DIK_LSHIFT]		= VK_LSHIFT;
	mDIKeys[DIK_RSHIFT]		= VK_RSHIFT;
	mDIKeys[DIK_LCONTROL]	= VK_LCONTROL;
	mDIKeys[DIK_RCONTROL]	= VK_RCONTROL;
    mDIKeys[DIK_LMENU]		= VK_LMENU;
    mDIKeys[DIK_RMENU]		= VK_RMENU;
	mDIKeys[DIK_LBRACKET]	= 219;
	mDIKeys[DIK_RBRACKET]	= 221;
	mDIKeys[DIK_SEMICOLON]	= 186;
	mDIKeys[DIK_APOSTROPHE]	= 222;
	mDIKeys[DIK_MINUS]		= 189;
	mDIKeys[DIK_EQUALS]		= 187;
	mDIKeys[DIK_BACKSLASH]	= 220;
	mDIKeys[DIK_GRAVE]		= VK_GRAVE;
	mDIKeys[DIK_CAPITAL]	= 20;
	mDIKeys[DIK_COMMA]		= 188;
	mDIKeys[DIK_PERIOD]		= 190;
	mDIKeys[DIK_SLASH]		= 191;
	mDIKeys[DIK_SYSRQ]		= 44;
}


unsigned int DIInputSystem::TranslateKeyCode( WPARAM wp, LPARAM lp )
{
	int scancode = (((int)lp)>>16)&0x1FF;
	unsigned int result = wp;

	// Ignore troublesome keys
	if( wp == 44 ) 
		return (unsigned int)-1;	// ignore PrintScreen - it doesn't give KEYDOWN
	
	if( wp == VK_GRAVE )
	{
		if( scancode == 41 || !mWin32Only )
			return 0xDF;
		else
			return wp;
	}
	else if( wp == 0xDF && ( scancode == 40 || !mWin32Only ) )
		return VK_GRAVE;

    //if( wp == 18 ) 
    //  return (unsigned int)-1;	// ignore Right Alt - it doesn't give KEYDOWN, but gives 2 KEYUPs

	// Fix shifts
	if( wp == VK_SHIFT )
	{
		if( scancode == 42 )
			result = VK_LSHIFT;
		else
			result = VK_RSHIFT;
	}

	// Fix returns
	if( wp == VK_RETURN )
	{
		if( scancode == 284 )	// Numlock Enter
			result = EVirKeyNumlockEnter;
		else
			result = VK_RETURN;
	}

	// Fix alt
	if( wp == VK_MENU )
	{
		result = VK_LMENU;
	}

	// Fix keypad keys
	if( ( wp == VK_HOME ) && ( scancode == 71 ) )
		result = VK_NUMPAD7;
	else if( ( wp == VK_UP ) && ( scancode == 72 ) )
		result = VK_NUMPAD8;
	else if( ( wp == VK_PRIOR ) && ( scancode == 73 ) )
		result = VK_NUMPAD9;
	else if( ( wp == VK_LEFT ) && ( scancode == 75 ) )
		result = VK_NUMPAD4;
	else if( ( wp == 12 ) && ( scancode == 76 ) )
		result = VK_NUMPAD5;
	else if( ( wp == VK_RIGHT ) && ( scancode == 77 ) )
		result = VK_NUMPAD6;
	else if( ( wp == VK_END ) && ( scancode == 79 ) )
		result = VK_NUMPAD1;
	else if( ( wp == VK_DOWN ) && ( scancode == 80 ) )
		result = VK_NUMPAD2;
	else if( ( wp == VK_NEXT ) && ( scancode == 81 ) )
		result = VK_NUMPAD3;
	else if( ( wp == VK_INSERT ) && ( scancode == 82 ) )
		result = VK_NUMPAD0;
	else if( ( wp == VK_DELETE ) && ( scancode == 83 ) )
		result = VK_DECIMAL;

	// Fix control keys
	if( wp == VK_CONTROL )
	{
		switch( scancode )
		{
		case 29:	result = VK_LCONTROL;	break;	// Left control
		case 285:	result = VK_RCONTROL;	break;	// Right control
		}
	}

    /*
    // Fix alt keys
    if( wp == VK_MENU )
    {
        if (scancode == 312)
            wp = VK_RMENU;
        else
            wp = VK_LMENU;
    }
    */


//	GLog.Print( "Key number: %d, string: %s, scancode: %d\n", result, KeyNames[result].AsChar(), scancode );

	// UWAGI DO EDYTORA
	// Lewy Alt nie dochodzi, Prawy Alt dochodzi, ale DOWN jako Lewy Ctrl, UP jako unknown key
	// PrintScreen dochodzi tylko DOWN albo tylko UP ( chyba tylko UP )
	// Klawisze Menu powoduja dodatkowe efekty
	// Klawisz F1 wlacza help MFC

	return result;
}


bool DIInputSystem::BindUIKey( unsigned int keycode )
{
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


bool DIInputSystem::UnbindUIKey( unsigned int keycode )
{
	if( keycode <= 0 )
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


bool DIInputSystem::CheckMessagesKeys( int key )
{
	if( key <= 0 )
		return false;

	for( int i=0; i<18; i++ )
	{
		if( MessagesKeys[i] == key )
		{
			char todo[255];
			sprintf( todo, "Hud_OnSayTo(%d)", i + 1 );
			GLog.Print( "%s\n", todo );
#ifdef TRANSGAMING
			strcpy( (char*)mLuaToDo,todo);
#else
			mLuaToDo = String( todo );
#endif
			return true;
		}
	}

	return false;
}	


int DIInputSystem::GetKBType()
{
	WORD langID = HIWORD( GetKeyboardLayout( 0 ) );
	char txt[255];
	sprintf( txt, "0x%x - 0x%x", LOWORD( GetKeyboardLayout( 0 ) ), langID );
	MessageBox( NULL, txt, "Keyboard layout", MB_OK );
	return 1;
}
