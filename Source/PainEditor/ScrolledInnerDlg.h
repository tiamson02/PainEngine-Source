#pragma once
#include "afxwin.h"


// CScrolledInnerDlg dialog

class CScrolledInnerDlg : public CDialog
{
	DECLARE_DYNAMIC(CScrolledInnerDlg)
public:
	CScrolledInnerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScrolledInnerDlg();

protected:
	BOOL		m_bDragging;
	CPoint		m_ptDragFrom;
	int         posY;

	//{{AFX_MSG(CModelDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};
