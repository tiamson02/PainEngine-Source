// XTShellTreeCtrl.h : header file
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
#if !defined(__XTSHELLTREECTRL_H__)
#define __XTSHELLTREECTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

DECLATE_SHELLTREE_BASE(CXTShellTreeViewBase, CTreeView, CXTShellTreeBase)
DECLATE_SHELLTREE_BASE(CXTShellTreeCtrlBase, CTreeCtrl, CXTShellTreeBase)

//===========================================================================
// Remarks: CXTShellTreeView is a multiple inheritance class derived from CXTTreeView
//			and CXTShellPidl. It is used to create a CXTShellTreeView class object.
//===========================================================================
class _XT_EXT_CLASS CXTShellTreeView : public CXTShellTreeViewBase
{
	DECLARE_DYNCREATE(CXTShellTreeView)

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellTreeView object
	//-----------------------------------------------------------------------
    CXTShellTreeView();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellTreeView object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTShellTreeView();

protected:

//:Ignore
	//{{AFX_VIRTUAL(CXTShellTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL
//:End Ignore

//:Ignore
	virtual void SelectionChanged(HTREEITEM hItem, CString strFolderPath);
//:End Ignore

protected:
	DECLARE_MESSAGE_MAP()
};

//===========================================================================
// Remarks: CXTShellTreeCtrl is a multiple inheritance class derived from CXTTreeCtrl
//			and CXTShellPidl. CXTShellTreeCtrl is used to create a tree control
//			that displays an explorer style tree, and can be associated with a combo box
//			and list control.
//===========================================================================
class _XT_EXT_CLASS CXTShellTreeCtrl : public CXTShellTreeCtrlBase
{

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellTreeCtrl object
	//-----------------------------------------------------------------------
    CXTShellTreeCtrl();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellTreeCtrl object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTShellTreeCtrl();

 protected:
    
	BOOL			 m_bAutoInit;			// TRUE if the tree control is to initialize when created.
	BOOL			 m_bContentInitPending; // TRUE if content initialization is pending.
	BOOL			 m_bTunnelPath;			// TRUE if tunnel initialization is pending.
	CWnd*			 m_pListCtrl;			// Window that receives the update notification, usually a CXTShellListCtrl.
	CString			 m_strTunnelPath;		// Current path to tunnel if tree initialization is still pending. Valid if 'm_bTunnelPath' is TRUE.

public:

	//-----------------------------------------------------------------------
	// Input:	bEnable - TRUE if the tree initializes upon creation.
    // Summary:	Call this member function to enable or disable auto-initialization
	//			of the shell tree control.
	//-----------------------------------------------------------------------
    void EnableAutoInit(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Input:	pWnd - Points to the list control that is associated with the tree.
    // Summary:	Call this member function to associate the list control with the tree.
	//-----------------------------------------------------------------------
    virtual void AssociateList(CWnd* pWnd);


	//-----------------------------------------------------------------------
	// Input:	strFindPath - Path to find.
    // Summary:	This member function will "tunnel" the tree to find the specified
	//			path. This will work only when the PopulateTreeView() method is used
	//			to populate the tree.
	//-----------------------------------------------------------------------
    virtual void TunnelTree(CString strFindPath);

//:Ignore
	//{{AFX_VIRTUAL(CXTShellTreeCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL
//:End Ignore

protected:

//:Ignore
	// Internal used.
	virtual void DelayContentInit();
	virtual void ProcessContentInit();
	virtual void SelectionChanged(HTREEITEM hItem, CString strFolderPath);
//:End Ignore

//:Ignore
	//{{AFX_MSG(CXTShellTreeCtrl)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateShell(WPARAM wParam, LPARAM lParam);
//:End Ignore
	

    DECLARE_MESSAGE_MAP()
};

#define CXTShellTree    CXTShellTreeCtrl

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTShellTreeCtrl::EnableAutoInit(BOOL bEnable) {
    m_bAutoInit = bEnable;
}
AFX_INLINE void CXTShellTreeCtrl::AssociateList(CWnd* pWnd) {
    ASSERT_VALID(pWnd); m_pListCtrl = pWnd;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTSHELLTREECTRL_H__)
//:End Ignore