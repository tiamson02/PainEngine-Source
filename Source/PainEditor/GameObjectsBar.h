#pragma once

#ifdef EDITOR

enum 
{
    PM_LookAt = 1,
    PM_Clone,
    PM_CloneInTheSamePlace,
    PM_Copy,
    PM_Paste,
    PM_Reset,
    PM_ExecStartCommand,
    PM_Save,
    PM_SaveAsTemplate,
    PM_SaveAsLevelTemplate,
    PM_SaveAllClass,
    PM_Delete,
    PM_EditScript,
    PM_Properties,
    PM_Refresh,
    PM_Pause,
};

/////////////////////////////////////////////////////////////////////////////
// CGameObjectsBar window

class CGameObjectsBar : public CWnd
{
	DECLARE_DYNAMIC(CGameObjectsBar)

// Construction
	CGameObjectsBar();

// Attributes
protected:
    CString CopiedObject;
	CImageList	m_imageList;
    HTREEITEM FindItemOK(CString name);

	CXTTreeCtrl m_wndObjectsView;
	CXTPToolBar m_wndToolBar;


// Operations
public:

    static CGameObjectsBar& Get();
    void UpdateView(CString objname,bool refresh = false);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameObjectsBar)
	protected:
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGameObjectsBar();
	virtual BOOL Create(CWnd* pParentWnd);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam); 

	// Generated message map functions
protected:
	//{{AFX_MSG(CGameObjectsBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLclickObjtree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarCommand(UINT nID);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
