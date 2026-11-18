
// #pragma once
#ifndef _DIINPUTSYSTEM_
#define _DIINPUTSYSTEM_


#include "Engine/InputSystem.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

class ENGINE_API DIInputSystem : public InputSystem
{
public:

	DIInputSystem( HWND hwnd );
	~DIInputSystem();

	void Release();

	// MessageHandler interface
    LRESULT HandleMessages( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );

	void Tick();
	void Reset();

	void LockMouse( BOOL lock );
	void ShowMouse( BOOL show );
	void CaptureMouse( BOOL capture );

	void SetWindow( HWND hwnd );

	BOOL IsMouseLocked() const { return mMouseLocked; }
	BOOL IsMouseVisible() const { return mMouseVisible; }

#ifdef TRANSGAMING
    void setCurrentControlProfile(ControlProfile cp);
    ControlProfile getCurrentControlProfile() const;

    float getMovementScale() const;
    float getStrafeScale() const;
#endif

	void   SetMouseSensitivity( float msens );
	float  GetMouseSensitivity() const { return mMouseSensitivity; }
	void   SetWheelSensitivity( float msens ) { mWheelSensitivity = msens; }
	float  GetWheelSensitivity() const { return mWheelSensitivity; }
	void   SetMouseSmooth( bool s ) { mMouseSmooth = s; }
	bool   GetMouseSmooth() const { return mMouseSmooth; }
	Vector GetMouseDelta() const;
	POINT  GetMousePosition() const { return mMousePos; }
	void   SetMousePosition( int x,int y );
	void   CenterMouse( void ) { SetMousePosition( mCenterPos.x, mCenterPos.y ); }
	void   UpdateMousePosition();
	void   SetDIDeltaScale( float s ) { mDIDeltaScale = s; }
	float  GetDIDeltaScale() const { return mDIDeltaScale; }

	Vector GetLookDelta() const { return mLookDelta; }
	Vector GetMoveDelta() const { return mMoveDelta; }

	virtual void	SetUseDInput( bool use );
	virtual bool	GetUseDInput() const { return mUseDInput; }

	bool	IsKeyPrintable( unsigned char key );
	unsigned char	KeyToPrintable( unsigned char key );

	bool UsingGamePad() const { return( mGamePad != NULL ); }

	int GetKBType();

private:

	void MouseTick();
	void KeyboardTick();
	void GamePadTick();

	void SwitchToDInput();
	void SwitchToWin32();

	bool SetupDInput();
	bool SetupMouse();
	bool SetupKeyboard();
	bool SetupGamePad();

	unsigned int TranslateKeyCode( unsigned int code );
	unsigned int TranslateKeyCode( WPARAM wp, LPARAM lp );
	virtual void FillKeyNamesTable( void );
	void FillDIKeyTable();

	bool BindUIKey( unsigned int keycode );
	bool UnbindUIKey( unsigned int keycode );

	bool CheckMessagesKeys( int key );

	IDirectInput8		*mDI;

	IDirectInputDevice8	*mMouseDev;
	IDirectInputDevice8	*mKBDev;
	IDirectInputDevice8	*mGamePad;

	HANDLE				mMouseEvent;

	POINT				mMousePos;
	POINT				mCenterPos;
	Vector				mMouseDelta;
	float				mMouseSensitivity;
	float				mWheelSensitivity;
	bool				mMouseSmooth;
	int					mMouseWheelDelta;
	float				mDIDeltaScale;

	Vector				mLookDelta;
	Vector				mMoveDelta;

	BOOL				mMouseLocked;
	BOOL				mMouseVisible;

	BYTE				mDIKeys[256];
	unsigned int		mLastKey;

	bool				mWin32Only;
	bool				mUseDInput;

	DynamicArray<int>	mPressed;
	DynamicArray<int>	mReleased;

#ifdef TRANSGAMING
	ControlProfile      mCurrentProfile;
	float               mMovementScale;
    float               mStrafeScale;
#endif

	bool				mButtonsState[16];
};


#endif //_DIINPUTSYSTEM_

