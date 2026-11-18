// XTPReportInplaceControls.h
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

//:Ignore
#if !defined(__XTPREPORTINPLACECONTROLS_H__)
#define __XTPREPORTINPLACECONTROLS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportRecord;
class CXTPReportControl;
class CXTPReportRecordItem;
class CXTPReportRecordItemConstraints;
class CXTPReportRecordItemConstraint;

#include "XTPReportRecordItem.h"


class _XTP_EXT_CLASS CXTPReportInplaceEdit : public CEdit
{
public:
	CXTPReportInplaceEdit();

public:
	void HideWindow();
	CXTPReportRecordItem* GetItem();
	void Create(XTP_REPORTRECORDITEM_ARGS* pItemArgs);

protected:
	void SetFont(CFont* pFont);

protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	
protected:
	afx_msg void OnEnKillfocus();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);
	
protected:

	CString m_strValue;
	CXTPReportRecordItem* m_pItem;
	CXTPReportControl* m_pControl;
	CXTPReportRow* m_pRow;
	CXTPReportColumn* m_pColumn;
	CFont m_fntEdit;
	CXTPReportRecordItemConstraint* m_pSelectedConstraint;
	
	COLORREF m_clrText;

private:
	DECLARE_MESSAGE_MAP()	

	friend class CXTPReportRecordItem;
};

AFX_INLINE CXTPReportRecordItem* CXTPReportInplaceEdit::GetItem() {
	return m_pItem;
}


#define XTP_ID_REPORT_EXPANDBUTTON 100
#define XTP_ID_REPORT_COMBOBUTTON 101

class _XTP_EXT_CLASS CXTPReportInplaceButton : public CStatic
{
public:
	CXTPReportInplaceButton(UINT nID);

public:
	void Create(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CRect& rcButtons);
	int GetID();

	BOOL IsPressed();

protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);

protected:
	int m_nID;
	int m_nWidth;

	XTP_REPORTRECORDITEM_ARGS m_itemArgs;
	
	BOOL m_bPressed;
	BOOL m_bOver;


private:
	DECLARE_MESSAGE_MAP()	
		
	friend class CXTPReportRecordItem;
};

AFX_INLINE int CXTPReportInplaceButton::GetID() {
	return m_nID;
}
AFX_INLINE BOOL CXTPReportInplaceButton::IsPressed() {
	return m_bPressed && m_bOver;
}

class CXTPReportInplaceList : public CListBox
{
public:
	CXTPReportInplaceList();
	
public:
	void Create(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraints* pConstaints);

	void Cancel(void);
	void Apply(void);

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	void OnKillFocus(CWnd* pNewWnd);
	void OnLButtonUp(UINT, CPoint point);

protected:
	XTP_REPORTRECORDITEM_ARGS m_itemArgs;

	DECLARE_MESSAGE_MAP()	
};

#endif