// XTShellListCtrlView.h : header file
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE 
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN 
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED 
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE GRANTS TO 
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A 
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTSHELLLISTCTRLVIEW_H__)
#define __XTSHELLLISTCTRLVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

DECLATE_SHELLLIST_BASE(CXTShellListViewBase, CListView_VC50, CXTShellListBase)
DECLATE_SHELLLIST_BASE(CXTShellListCtrlBase, CListCtrl_VC50, CXTShellListBase)

//===========================================================================
// Remarks: CXTShellListView is a multiple inheritance class derived from CXTListView
//			and CXTShellPidl. It is used to create a CXTShellListView class object.
//===========================================================================
class _XT_EXT_CLASS CXTShellListView : public CXTShellListViewBase
{
	DECLARE_DYNCREATE(CXTShellListView)

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellListView object
	//-----------------------------------------------------------------------
    CXTShellListView();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellListView object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTShellListView();

protected:

	//-----------------------------------------------------------------------
	// Input:	pNMHDR  - 
	//			pResult - 
	// Summary: 
	//-----------------------------------------------------------------------
	void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	
    //:Ignore
	//{{AFX_VIRTUAL(CXTShellListView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
	//:End Ignore

protected:

    afx_msg LRESULT OnUpdateShell(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//===========================================================================
// Summary: CXTShellListCtrl is a multiple inheritance class derived from CXTListCtrl
//			and CXTShellPidl. It is used to create a CXTShellListCtrl window.
//===========================================================================
class _XT_EXT_CLASS CXTShellListCtrl : public CXTShellListCtrlBase
{

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellListCtrl object
	//-----------------------------------------------------------------------
    CXTShellListCtrl();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellListCtrl object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTShellListCtrl();

protected:
	CWnd*				m_pTreeCtrl;	// Window that receives update notification, usually a CXTShellTreeCtrl.
	bool				m_bInitControl;

public:
	//-----------------------------------------------------------------------
	// Input:	pWnd - Points to the tree control that is associated with the list.
    // Summary:	Call this member function to associate the tree control with the list.
	//-----------------------------------------------------------------------
    virtual void AssociateTree(CWnd* pWnd);

protected:

    //:Ignore
	//{{AFX_MSG(CXTShellListCtrl)
    afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnUpdateShell(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	//:End Ignore

    DECLARE_MESSAGE_MAP()
};

#define CXTShellList CXTShellListCtrl

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTShellListCtrl::AssociateTree(CWnd* pWnd) {
    ASSERT_VALID(pWnd); m_pTreeCtrl = pWnd;
}

//===========================================================================
// Remarks: CXTShellListCtrlEx is a CXTShellListCtrl derived class. It is used to create
//			a stand-alone shell list control that is not dependant on a CXTShellTreeCtrl
//			for initialization. It is used to create a CXTShellListCtrlEx window for 
//			displaying the contents of file folders.
//===========================================================================
class _XT_EXT_CLASS CXTShellListCtrlEx : public CXTShellListCtrl
{

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellListCtrlEx object
	//-----------------------------------------------------------------------
	CXTShellListCtrlEx();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellListCtrlEx object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTShellListCtrlEx();

protected:

	CWnd*   m_pSyncWnd;    // Points to the window to synchronize with the shell list control.
	CString m_strItemPath; // A NULL terminated string that represents the currently selected folder.

public:

	//-----------------------------------------------------------------------
	// Input:	lpszPath - A NULL terminated string that represents the folder to select.
	// Summary:	This member function is called to browse to a particular file folder.  
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	bool BrowseToFolder(LPCTSTR lpszPath);
	
	//-----------------------------------------------------------------------
	// Input:	pSyncWnd - Points to a valid CWnd object.
	// Summary:	This member function will associate a CWnd object with the shell list control. This
	//			window is usually a CEdit control, but can be any CWnd object. This window will have
	//			its window text updated whenever the selected folder is changed.
	//-----------------------------------------------------------------------
	void SetSyncWnd(CWnd* pSyncWnd);

	//{{AFX_VIRTUAL(CXTShellListCtrlEx)
	//}}AFX_VIRTUAL

protected:

	//:Ignore
	//{{AFX_MSG(CXTShellListCtrlEx)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTShellListCtrlEx::SetSyncWnd(CWnd* pSyncWnd) {
	m_pSyncWnd = pSyncWnd;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTSHELLLISTCTRLVIEW_H__)
//:End Ignore