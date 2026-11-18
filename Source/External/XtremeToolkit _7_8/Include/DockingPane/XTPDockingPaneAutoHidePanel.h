// XTPDockingPaneAutoHidePanel.h : interface for the CXTPDockingPaneAutoHidePanel class.
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
#if !defined(__XTPDOCKINGPANEAUTOHIDEPANEL_H__)
#define __XTPDOCKINGPANEAUTOHIDEPANEL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneBase.h"

class CXTPDockingPaneLayout;
class CXTPDockingPaneAutoHidePanel;

//:Ignore	


//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneAutoHideWnd is a CMiniFrameWnd derived class. It is used internally by CXTPDockingPaneAutoHidePanel
//			as a slideable window for the hidden pane.
class _XTP_EXT_CLASS CXTPDockingPaneAutoHideWnd :  public CMiniFrameWnd
{

private:

	CXTPDockingPaneAutoHideWnd(CXTPDockingPaneAutoHidePanel* pPanel, CXTPDockingPaneTabbedContainer* pPane);
	void CloseWindow();
	CXTPDockingPaneManager* GetDockingPaneManager();

	void RecalcLayout(BOOL bNotify);
	void DoSlideStep();
	void Reposition(CRect rc, CRect rcClient);

public:
	// Summary: Animation delay.
	static double m_dAnimationDelay;	
    static int m_nAnimationInterval;              // ms between animation steps 
    static int m_nAnimationDuration;              // duration of sliding animation in ms 


private:
	CXTPDockingPaneTabbedContainer* m_pPane;
	CXTPDockingPaneAutoHidePanel* m_pPanel;
	XTPDockingPaneDirection m_direction;
	
	BOOL m_bHoriz;
	int m_nSlideStep;
	int m_nStepsCount;
	BOOL m_bSlideOut;
	CRect m_rcWindow;
	int m_nDeactivationCount;
	BOOL m_bTracking;


private:
	//{{AFX_VIRTUAL(CXTPDockingPaneAutoHideWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CXTPDockingPaneAutoHideWnd)
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

private:
	DECLARE_DYNAMIC(CXTPDockingPaneAutoHideWnd);	
	DECLARE_MESSAGE_MAP()							
	
	friend class CXTPDockingPaneAutoHidePanel;	
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneAutoHidePanel is a multiple inheritance class derived from
//			CWnd and CXTPDockingPaneBase. It is used internally as an autohide container
//			for CXTPDockingPaneBase derived classes.
class _XTP_EXT_CLASS CXTPDockingPaneAutoHidePanel :  public CWnd, public CXTPDockingPaneBase
{
private:
	CXTPDockingPaneAutoHidePanel(CXTPDockingPaneLayout* pLayout, XTPDockingPaneDirection direction = dockLeftOf);
	CXTPDockingPaneAutoHidePanel(CXTPDockingPaneAutoHidePanel* pClone, CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap);
	~CXTPDockingPaneAutoHidePanel();

	class CAutoHidePanelTabManager;
	class CAutoHidePanelTabManagersArray; 

public:

	// Returns: The panel's direction.
	// Summary: Call this member function to determine the position of the auto-hide panel.
	XTPDockingPaneDirection GetDirection();

	// Returns: A reference to the child panes collection.
	// Summary: Call this member to return collection of the child panes.
	CXTPDockingPaneBaseList& GetPanes();

	// Returns: A reference to the tabs collection.
	// Summary: Call this member to return collection of the tabs.
	CXTPDockingPaneHiddenTabsArray& GetTabs();

private:

	virtual void OnSizeParent(CWnd* pParent, CRect& rect, LPVOID lParam);
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual BOOL IsEmpty();
	CXTPDockingPane* HitTest(CPoint point);
	void _InsertPane(CXTPDockingPaneBase* pPane);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	void CloseActiveWindow(BOOL bDelayed = FALSE);
	void ShowPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE);
	
	void RefreshManagersArray(CDC* pDC, CRect rcClient);
	int GetPanelHeight();
	
	void Init();

private:	
	CXTPDockingPaneAutoHideWnd* m_pActiveWnd;
	XTPDockingPaneDirection m_direction;
	CXTPDockingPaneBaseList m_lstPanes;
	
	CAutoHidePanelTabManagersArray* m_pTabManagers;



private:
	//{{AFX_MSG(CXTPDockingPaneAutoHidePanel)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM, LPARAM lParam);
	afx_msg void OnMouseLeave();
	//}}AFX_MSG

private:
	DECLARE_MESSAGE_MAP()

	friend class CXTPDockingPaneLayout;				
	friend class CXTPDockingPaneAutoHideWnd;		
	friend class CXTPDockingPaneTabbedContainer;	
	friend class CXTPDockingPaneManager;
};

AFX_INLINE XTPDockingPaneDirection CXTPDockingPaneAutoHidePanel::GetDirection() { 
	return m_direction; 
}
AFX_INLINE CXTPDockingPaneBaseList& CXTPDockingPaneAutoHidePanel::GetPanes() { 
	return m_lstPanes; 
}

#endif // #if !defined(__XTPDOCKINGPANEAUTOHIDEPANEL_H__)