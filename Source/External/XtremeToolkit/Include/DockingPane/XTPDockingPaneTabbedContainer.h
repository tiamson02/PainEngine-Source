// XTPDockingPaneTabbedContainer.h : interface for the CXTPDockingPaneTabbedContainer class.
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
#if !defined(__XTPDOCKINGPANETABBEDCONTAINER_H__)
#define __XTPDOCKINGPANETABBEDCONTAINER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneBase.h"
#include "XTPDockingPanePaintManager.h"

// forwards

class CXTPDockingPaneAutoHidePanel;
class CXTPDockingPaneCaptionButton;


// Summary: Enumeration of the pin button state
enum XTPDockingPanePinState
{
	xtpPinVisible = 1, // Pin button is visible.
	xtpPinPushed = 2   // Pin button is pushed.
};

//===========================================================================
// Remarks: CXTPDockingPaneTabbedContainer is a multiple inheritance class derived from 
//			CWnd and CXTPDockingPaneBase. It represents a tabbed container for Docking
//			Panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneTabbedContainer : public CWnd, public CXTPDockingPaneBase, public CXTPTabManager
{
	
protected:

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Constructs a CXTPDockingPaneTabbedContainer object
	//			Protected constructor. Internally constructed only.
	//-----------------------------------------------------------------------
	CXTPDockingPaneTabbedContainer(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	pPane   - Child pane.
	//			pFrame  - Parent Frame.
	//			pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Constructs a CXTPDockingPaneTabbedContainer object
	//			Protected constructor. Internally constructed only.
	//-----------------------------------------------------------------------
	void Init(CXTPDockingPane* pPane, CFrameWnd* pFrame);

	//-----------------------------------------------------------------------
	// Input:	pClone  - Points to a CXTPDockingPaneTabbedContainer object
	//			pLayout - Points to a CXTPDockingPaneLayout object
	//			pMap    - Points to a CXTPPaneToPaneMap object
	// Summary: Constructs a CXTPDockingPaneTabbedContainer object
	//			Protected constructor. Internally constructed only.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPDockingPaneTabbedContainer* pClone, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions);

public:
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneTabbedContainer object, handles cleanup and
	//			de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneTabbedContainer();

	//-----------------------------------------------------------------------
	// Input:	pPane     - Pane to be activated.
	//			bSetFocus - TRUE to set focus to child docking pane.
	// Summary: Call this member to activate a child docking pane.
	//-----------------------------------------------------------------------
	virtual void SelectPane(CXTPDockingPane* pPane, BOOL bSetFocus = TRUE);
	
	//-----------------------------------------------------------------------
	// Returns: TRUE if a child pane is active.
	// Summary: Call this member function to get child pane state.
	//-----------------------------------------------------------------------
	BOOL IsActive();

	//-----------------------------------------------------------------------
	// Returns: The selected child pane.
	// Summary:	Call this member function to get selected pane.
	//-----------------------------------------------------------------------
	CXTPDockingPane* GetSelected();

	//-----------------------------------------------------------------------
	// Returns: A reference to the title's close button.
	// Summary:	Call this member to access title's close button.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton* GetCloseButton();

	//-----------------------------------------------------------------------
	// Returns: A reference to the title's pin button.
	// Summary: Call this member to access title's pin button.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton* GetPinButton();
	
	//-----------------------------------------------------------------------
	// Returns: A reference to the child panes collection.
	// Summary: Call this member to get collection of panes.
	//-----------------------------------------------------------------------
	CXTPDockingPaneList& GetPanes();

	//-----------------------------------------------------------------------
	// Returns: TRUE if the title is visible.
	// Summary: Call this member to determine if the title is visible.
	//-----------------------------------------------------------------------
	BOOL IsTitleVisible();

	//-----------------------------------------------------------------------
	// Returns: TRUE if the tab bar is visible.
	// Summary: Call this member to determine if the tab bar is visible.
	//-----------------------------------------------------------------------
	BOOL IsTabsVisible();

	
	// Returns: The state of the pin button.
	// Summary: Call this member to return state of the title's pin button.
	//int GetPinButtonState();

	//-----------------------------------------------------------------------
	// Returns: TRUE if the container contains children.
	// Summary: Call this member to determine if the tabbed container contains 
	//			children panes.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty();

	//-----------------------------------------------------------------------
	// Summary: Retrieves safe window handle.
	//-----------------------------------------------------------------------
	virtual HWND GetSafeHwnd();

	BOOL IsHide();

	virtual void OnCaptionLButtonDown(CPoint point);

	virtual BOOL CanAttach(CRect& rcClient, CPoint pt);
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object.
	// Summary: Call this member to determine if one pane is contained within
	//			the collection of another.
	// Returns: POSITION of the pane if the collection is successful; otherwise
	//			returns NULL.
	//-----------------------------------------------------------------------
	virtual POSITION ContainPane(CXTPDockingPaneBase* pPane);


	void GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo);

protected:
	virtual BOOL IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* pButton);
	virtual void OnCaptionButtonClick(CXTPDockingPaneCaptionButton* pButton);

protected:
	virtual void Reposition();	
	virtual void RedrawControl();
	virtual CXTPTabPaintManager* GetPaintManager();
	virtual void SetPaintManager(CXTPTabPaintManager* pPaintManager);
	virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon);

	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;



private:
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);
	virtual void OnIdleUpdate();
	virtual void SetParentFrame(CFrameWnd* pFrame);
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnParentConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	
private:
	void ShowTitle(BOOL bShow);
	CString GetTitle();
	void Show();

	void _UpdateTabs();
	void _Swap(CXTPDockingPane* p1, CXTPDockingPane* p2);
	void _InsertPane(CXTPDockingPane* pPane);

	int HitTest(CPoint point);
	CXTPDockingPane* GetItemPane(int nIndex);


private:
	BOOL m_bActive;
	BOOL m_bTitleVisible;
	BOOL m_bLockReposition;

	CXTPDockingPane*	  m_pSelectedPane;
	CXTPDockingPane*	  m_pTrackingPane;
	CArray<CRect, CRect&> m_lstRects;

	CXTPDockingPaneList		 m_lstPanes;
	COleDropTarget* m_pDropTarget;

private:

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPDockingPaneTabbedContainer)


private:
	//{{AFX_VIRTUAL(CXTPDockingPaneTabbedContainer)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneTabbedContainer)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	//}}AFX_MSG

#ifdef _XTP_ACTIVEX
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
#endif

protected:
	class CContainerDropTarget;

private:
	friend class CXTPDockingPaneManager;		
	friend class CXTPDockingPane;				
	friend class CXTPDockingPaneContext;		
	friend class CXTPDockingPaneMiniWnd;		
	friend class CXTPDockingPaneLayout;			
	friend class CXTPDockingPaneAutoHidePanel;	
	friend class CXTPDockingPaneAutoHideWnd;	
	friend class CXTPDockingPaneCaptionButton;	
	friend class CContainerDropTarget;
};

AFX_INLINE BOOL CXTPDockingPaneTabbedContainer::IsActive() { 
	return m_bActive;	
}
AFX_INLINE CXTPDockingPane* CXTPDockingPaneTabbedContainer::GetSelected() { 
	return m_pSelectedPane;
}
AFX_INLINE CXTPDockingPaneList& CXTPDockingPaneTabbedContainer::GetPanes() { 
	return m_lstPanes; 
}
AFX_INLINE BOOL CXTPDockingPaneTabbedContainer::IsTitleVisible() { 
	return m_bTitleVisible;
}
AFX_INLINE HWND CXTPDockingPaneTabbedContainer::GetSafeHwnd() { 
	return CWnd::GetSafeHwnd(); 
}

#endif // #if !defined(__XTPDOCKINGPANETABBEDCONTAINER_H__)