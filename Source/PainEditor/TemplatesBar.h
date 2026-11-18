#pragma once

#ifdef EDITOR

/////////////////////////////////////////////////////////////////////////////
// CTemplatesBar window

class CTemplatesBar : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CTemplatesBar)

// Construction
	CTemplatesBar();

// Attributes
protected:
    CString CopiedObject;
	CImageList	m_imageList;
    HTREEITEM FindItemOK(CString name);

// Operations
public:

    static CTemplatesBar& Get();
    void UpdateView(CString path);
    void UpdateLevelTemplates();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplatesBar)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTemplatesBar();
	virtual BOOL Create(CWnd* pParentWnd);

	// Generated message map functions
protected:
    void FillNode(CString path,HTREEITEM hti);
    void SortByBaseObj(HTREEITEM hti);
	//{{AFX_MSG(CTemplatesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


