// XTPReportView.h: interface for the CXTPReportView class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPREPORTVIEW_H__)
#define __XTPREPORTVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTPReportView.h : header file
//

#define XTP_ID_REPORT_CONTROL 100

#include "XTPReportControl.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPReportView view

class _XTP_EXT_CLASS CXTPReportView : public CView
{
protected:
	CXTPReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CXTPReportView)

public:
	CXTPReportControl& GetReportCtrl();

	CXTPReportPaintManager* GetPaintManager();


// Overrides
	//{{AFX_VIRTUAL(CXTPReportView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	BOOL OnPreparePrinting(CPrintInfo* pInfo);
	void OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Implementation
protected:
	virtual ~CXTPReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL PaginateTo(CDC* pDC, CPrintInfo* pInfo);
	void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
protected:
	virtual long PrintPage (CDC* pDC, CPrintInfo* pInfo, CRect rcPage, long nIndexStart);
	
	virtual void PrintHeader(CDC* pDC, CRect rcHeader);
	virtual int PrintRows(CDC* pDC, CRect rcRows, long nIndexStart);
	virtual void PrintRow(CDC* pDC, CXTPReportRow* pRow, CRect rcRow);
	virtual void PrintFooter(CDC* pDC, CPrintInfo* pInfo, CRect rcFooter);

	int GetColumnWidth(CXTPReportColumn* pColumn, int nTotalWidth);

protected:
	CXTPReportControl m_wndReport;
	CUIntArray m_aPageStart;


protected:
	//{{AFX_MSG(CXTPReportView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPReportControl& CXTPReportView::GetReportCtrl() {
	return m_wndReport;
}

#endif // !defined(__XTPREPORTVIEW_H__)
