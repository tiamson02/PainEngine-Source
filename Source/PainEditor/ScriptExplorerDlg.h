#ifndef __DOCKWINEX1_H__
#define __DOCKWINEX1_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDockWinEx1 class

class CSETreeControl: public CXTTreeCtrl
{
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

class CDockWinEx1 : public CXTDockWindow
{
	DECLARE_DYNAMIC(CDockWinEx1)

// Construction
public:
	CDockWinEx1();

// Attributes
public:

// Operations
public:

	CSETreeControl m_treeCtrl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDockWinEx1)
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddTreeItems();
	virtual ~CDockWinEx1();
	void AddDirectory(CString dir, HTREEITEM parent);

// Generated message map functions
protected:
	CImageList	m_imageList;

	//{{AFX_MSG(CDockWinEx1)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __DOCKWINEX1_H__

