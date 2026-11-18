#ifndef __ScrolledDialogBar_H__
#define __ScrolledDialogBar_H__


class CScrolledDialogBar : public CWnd
{
	DECLARE_DYNAMIC(CScrolledDialogBar)

// Construction
public:
	CScrolledDialogBar();

	CWnd m_wndPlace;
	CDialog *m_InnerDlg;

	void CreateInnerDialog(CDialog *dlg, int dialog_resource);
	void Refresh();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrolledDialogBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScrolledDialogBar();

	//{{AFX_MSG(CScrolledDialogBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//}}AFX_MSG
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __MODELDLG_H__

