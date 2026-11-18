// XTPDockingPaneDefines.h : public defines, structures and enums.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO 
// BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED 
// WRITTEN CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS 
// OUTLINED IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE 
// GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE 
// THIS SOFTWARE ON A SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTPDOCKINGPANEDEFINES_H__)
#define __XTPDOCKINGPANEDEFINES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

// forwards

class CXTPDockingPane;
class CXTPDockingPaneBase;
class CXTPDockingPaneBase;

// Summary: Safe pointer calling
#define SAFE_CALLPTR(p, f) if (p) p->f

// Summary: Docking direction enumeration
enum XTPDockingPaneDirection
{
	dockLeftOf,  // To dock the pane to the left of the pane or frame.
	dockRightOf, // To dock the pane to the right of the pane or frame.
	dockTopOf,   // To dock the pane to the top of the pane or frame.
	dockBottomOf // To dock the pane to the bottom of the pane or frame.
};

// Summary: Docking pane type 
enum XTPDockingPaneType
{
	typeDockingPane,       // Object is the docking pane.
	typeTabbedContainer,   // Object is the tabbed container.
	typeSplitterContainer, // Object is the splitter container.
	typeMiniWnd,           // Object is the mini window container.
	typeClient,            // Object is the pane contained client area.
	typeAutoHidePanel      // Object is the auto hide panel.
};

// Summary: Visual theme enumeration
enum XTPDockingPanePaintTheme
{
	xtpPaneThemeDefault,    // Default theme.
	xtpPaneThemeOffice,	    // Visual Studio .NET like theme.
	xtpPaneThemeGrippered,  // Visual Studio 6 like theme.
	xtpPaneThemeVisio,      // Visio like theme.
	xtpPaneThemeCustom,     // Custom theme.
	xtpPaneThemeOffice2003, // Office 2003 like theme.
	xtpPaneThemeNativeWinXP // XP Theme.
};

// Summary: Docking Pane Options enumeration
enum XTPDockingPaneOptions
{
	xtpPaneNoCloseable = 1,	    // Pane can't be closed.
	xtpPaneNoHideable = 2,	    // Pane can't be hidden.
	xtpPaneNoFloatable = 4		// Pane can't be floated.
};

enum XTPDockingPaneAction
{
	xtpPaneActionFloating,		
	xtpPaneActionFloated,		
	xtpPaneActionClosing,		
	xtpPaneActionClosed,		
	xtpPaneActionDocking,		
	xtpPaneActionDocked,		
	xtpPaneActionAttaching,		
	xtpPaneActionAttached,		
	xtpPaneActionPinning,		
	xtpPaneActionPinned,
	xtpPaneActionCollapsing,		
	xtpPaneActionCollapsed,
	xtpPaneActionExpanding,		
	xtpPaneActionExpanded,
};

struct XTP_DOCKINGPANE_ACTION
{
	XTP_DOCKINGPANE_ACTION(XTPDockingPaneAction _action) 
	{
		pPane = NULL;
		pDockContainer = NULL;
		action = _action;
		bCancel = FALSE;
	}
	XTPDockingPaneAction action;
	BOOL bCancel;
	CXTPDockingPane* pPane;
	
	CXTPDockingPaneBase* pDockContainer;
	XTPDockingPaneDirection dockDirection;
};



//:Ignore	

// Summary: Internal structure used by Layout manager 
struct XTP_DOCKINGPANE_INFO
{
	XTP_DOCKINGPANE_INFO(CXTPDockingPane* p = NULL) {pPane = p; pLastHolder = pDockingHolder = pFloatingHolder = 0;}
	CXTPDockingPane* pPane;
	CXTPDockingPaneBase* pFloatingHolder;
	CXTPDockingPaneBase* pDockingHolder;
	CXTPDockingPaneBase* pLastHolder;

	operator CXTPDockingPane*() { return pPane;}
	CXTPDockingPane* operator ->() { return pPane;}
};

//:End Ignore	

#ifndef DELAYRECALCLAYOUT
#define DELAYRECALCLAYOUT(pFrame) \
	if ((pFrame)) \
	{ \
		(pFrame)->DelayRecalcLayout();\
	}
#endif

typedef CList<CXTPDockingPane*, CXTPDockingPane*> CXTPDockingPaneList;
typedef CList<CXTPDockingPaneBase*, CXTPDockingPaneBase*> CXTPDockingPaneBaseList;
typedef CList<XTP_DOCKINGPANE_INFO, XTP_DOCKINGPANE_INFO&> CXTPDockingPaneInfoList;
typedef CMap<CXTPDockingPaneBase*, CXTPDockingPaneBase*, CXTPDockingPaneBase*, CXTPDockingPaneBase*> CXTPPaneToPaneMap;



//===========================================================================
// Input:	pPane - The value of lParam points to a CXTPDockingPane object that becomes visible.
//			This pointer should <b>never</b> be NULL.
//
// Summary: The XTPWM_DOCKINGPANE_NOTIFY message is sent to the Docking Pane Manager
//			owner window whenever an action occurs within the DockingPanes.
//			<pre>XTPWM_DOCKINGPANE_NOTIFY 
//			CXTPDockingPane* pPane = (CXTPDockingPane*)lParam; // pointer to an CXTPDockingPane object </pre>
//
//			The docking pane is just a virtual container for a user window. The best
//			place to associate them is after the pane becomes visible because, initially,
//			it can be created, closed, hidden, or as a non-active tab. You can create your
//			user window in this handler.<p>
//
//			Process this message to attach an existing window to the Docking Pane container using the 
//			Attach member function.
// Example: Here is an example of how an application would process the XTPWM_DOCKINGPANE_NOTIFY 
//			message. 
//
//			<pre>LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
//			{
//				if (wParam == XTP_DPN_SHOWWINDOW)
//				{
//					CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
//			
//					// check if the Pane hasn't been attached yet.
//					if (!pPane->IsValid())
//					{
//						// check if the user window hasn't been created.
//						if (!m_wndPane.m_hWnd)
//						{
//							// create it.
//							m_wndPane.CreateEx(WS_EX_STATICEDGE, _T("EDIT"), _T(""), WS_CHILD|ES_AUTOVSCROLL|ES_MULTILINE, CRect(0, 0,200, 120), this, 0);
//						}
//						// attach it to the Pane.
//						pPane->Attach(&m_wndPane);
//					}
//					return TRUE;
//				}
//				return FALSE;	
//			
//			}</pre>
// Returns: If the application is to process this message, the return value should be 
//			TRUE, otherwise the return value is FALSE. 
//  
//===========================================================================
const UINT XTPWM_DOCKINGPANE_NOTIFY = (WM_APP + 2534);

// Summary: The pane becomes visible.
const UINT XTP_DPN_SHOWWINDOW = 1;

// Summary: User presses the right mouse button on the docking pane container.
const UINT XTP_DPN_RCLICK = 2;

// Summary: User close the Pane. Returns XTP_ACTION_NOCLOSE to ignore.
const UINT XTP_DPN_CLOSEPANE = 3;
const int XTP_ACTION_NOCLOSE = -1;

const UINT XTP_DPN_ACTION = 4;


// Uncomment it to disable XML.
//#define _XTP_EXCLUDE_XML

// Summary: he WM_XTP_GETTABCOLOR message is sent to the pane's child window to determine tab color.
#ifndef WM_XTP_GETTABCOLOR
#define WM_XTP_GETTABCOLOR (WM_USER + 134)

#define XTP_TABCOLOR_BLUE		0x1000000
#define XTP_TABCOLOR_YELLOW		0x1000001
#define XTP_TABCOLOR_GREEN		0x1000002
#define XTP_TABCOLOR_RED		0x1000003
#define XTP_TABCOLOR_PURPLE		0x1000004
#define XTP_TABCOLOR_CYAN		0x1000005
#define XTP_TABCOLOR_ORANGE		0x1000006
#define XTP_TABCOLOR_MAGENTA	0x1000007
#endif

#endif // #if !defined(__XTPDOCKINGPANEDEFINES_H__)