
// #pragma once
#ifndef _PSPINPUTSYSTEM_
#define _PSPINPUTSYSTEM_


#include "Engine/InputSystem.h"

#define DIRECTINPUT_VERSION	0x0800
#ifdef TRANSGAMING_PSP
# include <tgPad.h>
#elif defined(TRANSGAMING_PS2)
# include <eetypes.h>
# include <libpad.h>
# include <shellPad.h>
#elif !defined( TRANSGAMING_WINDOWS )
# error : including the PSP/PS2 input lib without specifying the platform!
#endif
#include <dinput.h>


class ENGINE_API PSPInputSystem : public InputSystem
{

    /*enum{
        PAD_MYSTERY_1 = (1 << 1),
        PAD_MYSTERY_2 = (1 << 2),
        PAD_MYSTERY_3 = (1 << 10),
        PAD_MYSTERY_4 = (1 << 11),
    };//*/

public:

	PSPInputSystem( HWND hwnd );
	~PSPInputSystem();

	void Release();

	void Tick();
	void Reset();

	void LockMouse( BOOL lock );
	void ShowMouse( BOOL show );
	//void CaptureMouse( BOOL capture );

	//void SetWindow( HWND hwnd );

	BOOL IsMouseLocked() const { return mMouseLocked; }
	BOOL IsMouseVisible() const { return mMouseVisible; }

	void   SetMouseSensitivity( float msens );
	float  GetMouseSensitivity() const { return mMouseSensitivity; }
#ifndef TRANSGAMING
    void   SetWheelSensitivity( float msens ){}// { mWheelSensitivity = msens; }
    float  GetWheelSensitivity() const{ return 2.0f; }// { return mWheelSensitivity; }
#endif
	void   SetMouseSmooth( bool s ) { mMouseSmooth = s; }
	bool   GetMouseSmooth() const { return mMouseSmooth; }
	Vector GetMouseDelta() const;
	POINT  GetMousePosition() const { return mMousePos; }
	void   SetMousePosition( int x,int y );
    void   CenterMouse( void ){}// { SetMousePosition( mCenterPos.x, mCenterPos.y ); }
	void   UpdateMousePosition();
    void   SetDIDeltaScale( float s ){}// { mDIDeltaScale = s; }
    float  GetDIDeltaScale() const { return 3.2f; }//{ return mDIDeltaScale; }

#ifndef TRANSGAMING
	Vector GetLookDelta() const { return mLookDelta; }
	Vector GetMoveDelta() const { return mMoveDelta; }
#endif

	virtual void	SetUseDInput( bool use );
    virtual bool	GetUseDInput() const{ return true; }// { return mUseDInput; }

    bool	        IsKeyPrintable( unsigned char key );
	unsigned char	KeyToPrintable( unsigned char key );

	bool UsingGamePad() const { return( true ); }
#ifdef TRANSGAMING
    void setCurrentControlProfile(ControlProfile cp);
    ControlProfile getCurrentControlProfile() const;

    float getMovementScale() const;
    float getStrafeScale() const;
#endif

	//int GetKBType();

private:

    //these names indicate the actual buttons on the PSP/PS2 that the mouse buttons will map to
    enum MouseButtons{
#ifdef TRANSGAMING_PSP          //PSP static mouse bindings
        MOUSE_LEFTBUTTON =      TG_PAD_CROSS,
        MOUSE_MIDDLEBUTTON =    TG_PAD_CIRCLE,
        MOUSE_RIGHTBUTTON =     TG_PAD_TRIANGLE,
#elif defined(TRANSGAMING_PS2)  //PS2 static mouse bindings
        MOUSE_LEFTBUTTON =      SCE_PADRdown,
        MOUSE_MIDDLEBUTTON =    SCE_PADRright,
        MOUSE_RIGHTBUTTON =     SCE_PADRup,
#endif
    };


	void MouseTick(bool menuShowing);
	//void KeyboardTick();
	void GamePadTick(bool menuShowing);
    void testAxis(float current, float previous, const PSButtonNames names[2]);


	//void SwitchToDInput();
	//void SwitchToWin32();

	bool InitInput();
	//bool SetupMouse();
	//bool SetupKeyboard();
	bool SetupGamePad();

	virtual void FillKeyNamesTable( void );
	//void FillDIKeyTable();

	bool BindUIKey( unsigned int keycode );
	bool UnbindUIKey( unsigned int keycode );

	//bool CheckMessagesKeys( int key );

	//IDirectInput8		*mDI;

	//IDirectInputDevice8	*mMouseDev;
	//IDirectInputDevice8	*mKBDev;
	//IDirectInputDevice8	*mGamePad;

	//HANDLE				mMouseEvent;

	POINT				mMousePos;
	//POINT				mCenterPos;
	Vector				mMouseDelta;
	float				mMouseSensitivity;
	//float				mWheelSensitivity;
	bool				mMouseSmooth;
	//int				mMouseWheelDelta;
	//float				mDIDeltaScale;

	//Vector              mLookDelta;
	//Vector              mMoveDelta;

	BOOL				mMouseLocked;
	BOOL				mMouseVisible;

	//BYTE				mDIKeys[256];   // <--used to map DI keys to printable characters
	//unsigned int		mLastKey;       // <--used to simulate repreated key presses while holding the button down

	//bool				mWin32Only;
	//bool				mUseDInput;

	//DynamicArray<int>	mPressed;       // <--used to gather 'just pressed' events for mouse and joystick buttons
	//DynamicArray<int>	mReleased;      // <--used to gather 'just released' events for mouse and joystick buttons
    int                 mPressed[BUTTON_COUNT];
    int                 mReleased[BUTTON_COUNT];
    int                 mPressedCount;
    int                 mReleasedCount;

	//bool                mButtonsState[16];
    ControlProfile      mCurrentProfile;
    float               mMovementScale;
    float               mStrafeScale;

    unsigned int        mLastKey;
    float               mStartTime;
    float               mLastTime;
	
	// From tgCommon
#ifdef TRANSGAMING_PSP
	TgPadData			pad;
#elif defined( TRANSGAMING_PS2 )
    void *              padDmaBuf;
    ShellPadData        pad;
    EShellPadType       padType;
#endif

};


#endif //_PSPINPUTSYSTEM_

