
#include <stdafx.h>
#include "Mainfrm.h"
#include "Childfrm.h"
#include "Engine.h"
#include "Main.h"
#include "EditorView.h"
#include "Render/Video.h"
#include "Render/RenderWindow.h"


//RenderWindow win;

HWND EditorView::GetActiveWindow()
{
	if( GEngine->Video->IsFullScreen )
		return GEngine->Video->FullScreenHWND();

//#ifdef EDITOR
	CChildFrame * pWnd = (CChildFrame *) ((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
	if( pWnd )
		return pWnd->/*m_wndSplitter.GetPane(0,0)->*/m_hWnd;

	/*
#else
    if (!win.GetHandle())
    {
        Window::Resources res;
        win.Initialize( GEngine->GetInstance(), res, "PKRender3D" );
        win.Create( "PainKiller", 0, 0, 640, 480, Window::EStyleOverlapped );
		win.Show();
    }
    return win.GetHandle();
#endif
    */
	return NULL;
}

HWND EditorView::GetTopmostWindow()
{
	if( GEngine->Video->IsFullScreen )
		return GEngine->Video->FullScreenHWND();

//#ifdef EDITOR
	return AfxGetMainWnd()->m_hWnd;
/*#else
    if (!win.GetHandle())
    {
        Window::Resources res;
        win.Initialize( GEngine->GetInstance(), res, "PKRender3D" );
        win.Create( "PainKiller", 0, 0, 640, 480, Window::EStyleOverlapped);
		win.Show();
    }
    return win.GetHandle();
#endif
	*/
}
