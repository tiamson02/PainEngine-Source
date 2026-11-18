// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
// $Log:$
//----------------------------------------------------------------------------

#ifndef TRANSGAMING
// #pragma once
#ifndef _WINDOW_
#define _WINDOW_


class ENGINE_API MessageHandler
{
public:
    MessageHandler(HWND hwnd=0) : hWnd(hwnd)	{ MessageHandler::InsertHandler(this); }
    ~MessageHandler()							{ MessageHandler::RemoveHandler(this); }
    
    HWND GetHandle() const { return hWnd; }
    virtual LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) = 0;

protected:
    HWND    hWnd;

protected:
    static MessageHandler*  GHandlerList[4];

    static void InsertHandler(MessageHandler* handler);
    static void RemoveHandler(MessageHandler* handler);

protected:
    static LRESULT CALLBACK GWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};

// ----------------------------------------------------------------

class ENGINE_API Window : public MessageHandler
{
public:

    enum { EStylePopup, EStyleOverlapped };
	enum { ENotActive, EActive, EClosed };

    class Resources
    {
    public:
        Resources()
#ifdef TRANSGAMING
        :   Icon(0)
        ,   SmallIcon(0)
        ,   Brush((HBRUSH)COLOR_BACKGROUND)
        ,   Cursor(0)
        ,   MenuName(NULL)
#else
        :   Icon(NULL)
        ,   SmallIcon(NULL)
        ,   Brush((HBRUSH)COLOR_BACKGROUND)
        ,   Cursor(NULL)
        ,   Cursor(LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW)))
        ,   MenuName(NULL)
#endif
        {}

        HICON   Icon;
        HICON   SmallIcon;
        HBRUSH  Brush;
        HCURSOR Cursor;
        LPCTSTR MenuName;
    };

public:
    Window();
    ~Window();

    BOOL Initialize(const HINSTANCE hInstance, 
                    const Resources& windowRes,
                    const char* className="default");

    BOOL Create(const char* caption="untitled", 
                int xpos  = CW_USEDEFAULT,
                int ypos  = CW_USEDEFAULT,
                int width = CW_USEDEFAULT,
                int height= CW_USEDEFAULT,
                int style = EStyleOverlapped);

    void Show();
    void Hide();
    void SetPosition(int xpos, int ypos);
    void SetSize(int width, int height);
    void Destroy();

    BOOL IsActive() const { return Status==EActive; }

    void GetSizes(int& width, int& height) const
    {
        RECT r;
        GetClientRect(hWnd,&r);
        width  = r.right  - r.left;
        height = r.bottom - r.top;
    }

    void AdjustSize(int width, int height);
    
protected:
    HINSTANCE   hInstance;
    int         Status;
    char        mClassName[64];
};

#endif  //_WINDOW_

#endif
