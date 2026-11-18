// XTPTabControl.h: interface for the CXTPTabControl class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO 
// BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED 
// WRITTEN CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS 
// OUTLINED IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE 
// GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE 
// THIS SOFTWARE ON A SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTPTABCONTROL_H__)
#define __XTPTABCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPTabManager.h"


class CXTPTabPaintManager;
class CXTPImageManager;

/////////////////////////////////////////////////////////////////////////////
// CXTPTabControl window

class _XTP_EXT_CLASS CXTPTabControl : public CWnd, public CXTPTabManager
{
protected:
	class CTabControlDropTarget;

public:
	CXTPTabControl();
	virtual ~CXTPTabControl();

public:
	CXTPTabManagerItem* InsertItem(int nItem, LPCTSTR lpszItem, HWND hwndChild = NULL, int nImage = -1);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);


public:
	virtual CXTPTabPaintManager* GetPaintManager();
	virtual void SetPaintManager(CXTPTabPaintManager* pPaintManager);
	virtual void Reposition();	
	virtual void RedrawControl();
	virtual void SetSelectedItem(CXTPTabManagerItem* pItem);
	virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon);

	void SetImageList(CImageList* pImageList);
	CXTPImageManager* GetImageManager();


protected:
	void OnNavigateButtonClick(UINT nID);


protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPTabControl)
	protected:
	virtual void PreSubclassWindow();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL
	
	// Generated message map functions
	//{{AFX_MSG(CXTPTabControl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	//}}AFX_MSG

private:
	HWND m_hwndClient;

private:
	CXTPTabPaintManager* m_pPaintManager;
	CXTPImageManager* m_pImageManager;
	CTabControlDropTarget* m_pDropTarget;

	friend class CTabControlDropTarget;

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPTabPaintManager* CXTPTabControl::GetPaintManager() {
	return m_pPaintManager;
}

AFX_INLINE CXTPImageManager* CXTPTabControl::GetImageManager() {
	return m_pImageManager;

}
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPTABCONTROL_H__)
