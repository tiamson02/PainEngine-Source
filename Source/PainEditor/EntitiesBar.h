#pragma once


#ifdef EDITOR

/////////////////////////////////////////////////////////////////////////////
// CEntitiesBar window

class CEntitiesBar : public CXTTreeCtrl
{
	DECLARE_DYNAMIC(CEntitiesBar)

// Construction
	CEntitiesBar();

// Attributes
protected:
	CImageList	m_imageList;

// Operations
public:

    static CEntitiesBar& Get();
    void UpdateView(CString objname,bool refresh = false);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntitiesBar)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEntitiesBar();
	virtual BOOL Create(CWnd* pParentWnd);

	// Generated message map functions
protected:
	//{{AFX_MSG(CEntitiesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam); 
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLclickObjtree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif