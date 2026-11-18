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


//===========================================================================
// Remarks: CXTPDockingPaneAutoHideWnd is a CMiniFrameWnd derived class. It is 
//			used internally by CXTPDockingPaneAutoHidePanel
//			as a slideable window for the hidden pane.
//===========================================================================
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

//===========================================================================
// Remarks: CXTPDockingPaneAutoHidePanel is a multiple inheritance class derived from
//			CWnd and CXTPDockingPaneBase. It is used internally as an autohide container
//			for CXTPDockingPaneBase derived classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneAutoHidePanel :  public CWnd, public CXTPDockingPaneBase
{
private:
	CXTPDockingPaneAutoHidePanel(CXTPDockingPaneLayout* pLayout);
	void Copy(CXTPDockingPaneAutoHidePanel* pClone, CXTPPaneToPaneMap* pMap);
	~CXTPDockingPaneAutoHidePanel();

	class CAutoHidePanelTabManager;
	class CAutoHidePanelTabManagersArray; 

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine the position of the 
	//			auto-hide panel.
	// Returns: The panel's direction.
	//-----------------------------------------------------------------------
	XTPDockingPaneDirection GetDirection();

	//-----------------------------------------------------------------------
	// Summary: Call this member to return collection of the child panes.
	// Returns: A reference to the child panes collection.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBaseList& GetPanes();

	//-----------------------------------------------------------------------
	// Summary: Call this member to return collection of the tabs.
	// Returns: A reference to the tabs collection.
	//-----------------------------------------------------------------------
	CXTPDockingPaneHiddenTabsArray& GetTabs();

	virtual HWND GetSafeHwnd();

protected:

	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CFrameWnd object.
	//			rect	- Rectangle of the pane to be set.
	//			lParam  - Pointer to AFX_SIZEPARENTPARAMS structure.
	// Summary: This method is called to set the position of the pane
	// ----------------------------------------------------------------------
	virtual void OnSizeParent(CWnd* pParent, CRect& rect, LPVOID lParam);

	//-----------------------------------------------------------------------
	// Input:	pLayout			 - Points to a CXTPDockingPaneLayout object.
	//			pMap			 - Points to a CXTPPaneToPaneMap object.
	//			dwIgnoredOptions - TRUE to ignore options of the pane.
	// Summary: This member is called to clone the pane.
	// Returns: A pointer to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	
	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	// Summary: Call this member to remove a pane from collection.
	//-----------------------------------------------------------------------
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	
	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object that is changed.
	// Summary: This member is called when a child pane collection is changed
	//-----------------------------------------------------------------------
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if there are children panes
	// Returns: TRUE if no children panes; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty();

	//-----------------------------------------------------------------------
	// Input:	point - Point to be tested.
	// Summary: This member function determines which header item, if any, is 
	//			at a specified cursor position
	// Returns: The index of the item at the position specified by 'pt', 
	//			otherwise returns -1.
	//-----------------------------------------------------------------------
	CXTPDockingPane* HitTest(CPoint point);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	// Summary: Call this memeber to insert a pane into a collection
	//-----------------------------------------------------------------------
	void _InsertPane(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Call this member to store information about the pane.
	//-----------------------------------------------------------------------
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	//			pMap    - Points to a CMapWordToPtr object.
	// Summary: Call this member function to restore the settings of the pane.
	// Returns: TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);

	//-----------------------------------------------------------------------
	// Input:	bDelayed - FALSE to delay the closing
	// Summary: Call this member to close the active window
	//-----------------------------------------------------------------------
	void CloseActiveWindow(BOOL bDelayed = FALSE);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	//			bSetFocus - TRUE to set focus to child docking pane
	// Summary: Call this member to show (activate) a pane by its identifier.
	//-----------------------------------------------------------------------
	void ShowPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Points to a CDC object
	//			rcClient - Size of the client area
	// Summary: Call this member to refresh the manager array
	//-----------------------------------------------------------------------
	void RefreshManagersArray(CDC* pDC, CRect rcClient);
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves the height of the panel
	// Returns: The height of the panel
	//-----------------------------------------------------------------------
	int GetPanelHeight();
	
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
AFX_INLINE HWND CXTPDockingPaneAutoHidePanel::GetSafeHwnd() { 
	return CWnd::GetSafeHwnd(); 
}


#endif // #if !defined(__XTPDOCKINGPANEAUTOHIDEPANEL_H__)