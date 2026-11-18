// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#if 0

#include "Core/Math/LeMath.h"
#include "Engine/InputSystem.h"

class ENGINE_API Win32InputSystem : public InputSystem
{
public:
    Win32InputSystem(HWND hwnd);
    ~Win32InputSystem();

    // MessageHandler interface
    LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

    // InputSystem interface
    void Tick();

    void   SetMouseSensitivity(float msens);
	float  GetMouseSensitivity() const { return MouseSensitivity; }
    Vector GetMouseDelta() const
    {
        return Vector(float(CurrentPos.x-CenterPos.x)*MouseSensitivity,
                      float(CurrentPos.y-CenterPos.y)*MouseSensitivity,
                      0.0f);
    }

	void UpdateMousePosition();
    POINT GetMousePosition() const
    {
        return CurrentPos;
    }
	void SetMousePosition(int x, int y);
	void CenterMouse( void ) { SetMousePosition( CenterPos.x, CenterPos.y ); };
    void LockMouse(BOOL lock);
    void ShowMouse(BOOL show);
    void CaptureMouse(BOOL capture);
	BOOL IsMouseLocked() const { return LockedMouse; }
	BOOL IsMouseVisible() const { return ShownMouse; }

	bool	IsKeyPrintable( unsigned char key );
	unsigned char	KeyToPrintable( unsigned char key );

private:
    POINT           CenterPos;
    POINT           CurrentPos;
    float           MouseSensitivity;
    BOOL            LockedMouse;
	BOOL			ShownMouse;
//	double			mLastMouseRepeat;	// timer for mouse buttons repeat
	int				mMouseWheelDelta;
protected:
	virtual void FillKeyNamesTable( void );
	unsigned int TranslateKeyCode( WPARAM wp, LPARAM lp );
};

#endif // 0


