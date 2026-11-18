// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//
//----------------------------------------------------------------------------
#ifndef TRANSGAMING
#include "Engine.h"
#include "Engine/InputSystem.h"

// static MessageHandler stuff
// --------------------------------------------------------------

MessageHandler* MessageHandler::GHandlerList[4] = {0};

void MessageHandler::InsertHandler(MessageHandler* handler)
{
    Assert(handler);

	for (int i=0; i<ARRAY_COUNT(GHandlerList); ++i)
		if (0 == GHandlerList[i])
		{
			GHandlerList[i] = handler;
			break;
		}
}

void MessageHandler::RemoveHandler(MessageHandler* handler)
{    
    Assert(handler);

	for (int i=0; i<ARRAY_COUNT(GHandlerList); ++i)
		if (handler == GHandlerList[i])
		{
			GHandlerList[i] = 0;
			break;
		}
}

LRESULT CALLBACK MessageHandler::GWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    TRACE("MessageHandler::GWindowProc(): handling the message %d for the window 0x%08X {wparam = 0x%08X, lparam = 0x%08X}\n", msg, hwnd, wp, lp);
	for (int i=0; i<ARRAY_COUNT(GHandlerList); ++i)
	{
		MessageHandler* handler = GHandlerList[i];
		if (handler && handler->GetHandle() == hwnd)
        {
            LRESULT res = handler->HandleMessages(hwnd,msg,wp,lp);
            if (res)
                return res;
        }
	}

    return DefWindowProc(hwnd,msg,wp,lp);
}

// Window
// ---------------------------------------------------------------------

Window::Window()
:   MessageHandler(), hInstance(0), Status(ENotActive)
{
}

Window::~Window()
{
    Destroy();
    if (hInstance) 
    {
        UnregisterClass(mClassName, hInstance);
        hInstance = 0;
    }
}

BOOL Window::Initialize(const HINSTANCE hinst, 
                        const Resources& windowRes,
                        const char* className)
{
    BOOL success=FALSE;
    if (hinst) 
    {
        hInstance = hinst;
        strncpy(mClassName,className,sizeof(mClassName));

	    WNDCLASSEX wcex;
	    wcex.cbSize = sizeof(WNDCLASSEX); 

	    wcex.style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= MessageHandler::GWindowProc;
	    wcex.cbClsExtra		= 0;
	    wcex.cbWndExtra		= 0;
	    wcex.hInstance		= hInstance;
	    wcex.hIcon			= windowRes.Icon,
	    wcex.hCursor		= windowRes.Cursor,
        wcex.hbrBackground	= (HBRUSH)BLACK_BRUSH;
	    wcex.lpszMenuName	= windowRes.MenuName,
	    wcex.lpszClassName	= mClassName;
	    wcex.hIconSm		= windowRes.SmallIcon;

        if(RegisterClassEx(&wcex))
            success=TRUE;
    }

    return success;
}

BOOL Window::Create(const char* caption,
                    int xpos, int ypos, int width, int height, 
                    int style)
{
    BOOL success=FALSE;

    if (hInstance) 
    {
	    hWnd = CreateWindowEx
	    (
		    style==EStylePopup ? WS_EX_TOPMOST|WS_EX_APPWINDOW : WS_EX_APPWINDOW|WS_EX_WINDOWEDGE,
		    mClassName, caption,
		    style==EStylePopup ? WS_POPUP : WS_OVERLAPPEDWINDOW,
		    xpos, ypos, width, height,
		    NULL, NULL, hInstance, NULL
	    );

        success = (hWnd != NULL);
    }

    return success;
}

void Window::SetPosition(int xpos, int ypos)
{
    if(hWnd)
        SetWindowPos(hWnd,0,xpos,ypos,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

void Window::SetSize(int width, int height)
{
    if(hWnd)
        SetWindowPos(hWnd,0,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
}

void Window::Destroy()
{
    if (hWnd && Status!=EClosed) 
	{
        Hide();

        // nasty race conditions
        // reset hwnd before calling DestroyWindow()
        // to prevent receiving messages
        HWND hwnd = hWnd;
        hWnd = 0;

        DestroyWindow(hwnd);
		Status = EClosed;
    }
}

void Window::Show()
{
    if(hWnd) 
	{
        // show the window and put it in foreground
        Status=EActive;
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
    }
}

void Window::Hide()
{
    if(hWnd) 
	{
        ShowWindow(hWnd,SW_HIDE);
        Status=ENotActive;
    }
}

// calc client size
void Window::AdjustSize(int width, int height)
{    
    RECT r;
    r.left=r.top=0;
    r.right =width;
    r.bottom=height;
                
    if(AdjustWindowRect(&r,GetWindowLong(hWnd,GWL_STYLE),FALSE)) 
	{
        width =r.right -r.left;
        height=r.bottom-r.top;
        SetWindowPos(hWnd,0,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
    }
}
#endif
