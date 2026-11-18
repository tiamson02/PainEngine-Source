#ifndef TRANSGAMING
#include "Engine.h"
#include "Render/RenderWindow.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

RenderWindow::RenderWindow()
:   Window(), HasFocus(0)
{
}

RenderWindow::~RenderWindow()
{
}

UINT g_uQueryCancelAutoPlay = 0;

LRESULT RenderWindow::HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    // handle window messages
    switch(msg) {
    case WM_ACTIVATEAPP:
        HasFocus = (BOOL) wp;
        break;

    case WM_CONTEXTMENU:
        // no context menus in fullscreen
        return 1;

    case WM_NCHITTEST:
        return HTCLIENT;


    case WM_SYSCOMMAND:
        switch(wp)
        {
            case SC_MOVE:
            case SC_SIZE:
            case SC_MAXIMIZE:
            case SC_KEYMENU:
            case SC_MONITORPOWER:
            case SC_SCREENSAVE:
                return 1;
        }
        break;

    case WM_ENTERSIZEMOVE:
        Status=ENotActive;
        return 1;

    case WM_EXITSIZEMOVE: 
        Status=EActive;
        return 1;

    case WM_SIZE:
        switch(wp) {
        case SIZE_MINIMIZED:
        case SIZE_MAXHIDE:
            Status=ENotActive;
            break;

        case SIZE_MAXIMIZED:
        case SIZE_RESTORED:
            Status=EActive;
            break;
        }
        return 1;    

    case WM_CLOSE:
        GEngine->Close();
        Status=EClosed;
        return 1;

    case WM_DESTROY:
        GEngine->Close();
        Status=EClosed;
        return 1;	

		
	default:
		if (!g_uQueryCancelAutoPlay)
		{ 
			g_uQueryCancelAutoPlay = RegisterWindowMessage(TEXT("QueryCancelAutoPlay"));
		} 
		if (msg && msg == g_uQueryCancelAutoPlay)
		{ 
			return TRUE;       // cancel auto-play
		}
		break;		

    }


    return 0;
}
#endif
