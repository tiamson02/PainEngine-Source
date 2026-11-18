// XTPDockBar.h : interface for the CXTPDockBar class.
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
#if !defined(__XTPDOCKBAR_H__)
#define __XTPDOCKBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"


class CXTPToolBar;
class CXTPCommandBars;

struct AFX_SIZEPARENTPARAMS;

typedef CArray<CXTPToolBar*, CXTPToolBar*> CXTPToolBarArray;

// Summary: CXTPDockBar is a CWnd derived class. It is used internally by CXTPCommandBars.
class _XTP_EXT_CLASS CXTPDockBar : public CWnd
{
	class _XTP_EXT_CLASS CXTPDockInfoArray;
	struct XTP_DOCK_INFO;

public:

	// Input:	pCommandBars - Points to a CXTPCommandBars object
	// Summary: Constructs a CXTPDockBar object
	CXTPDockBar(CXTPCommandBars* pCommandBars);
	
	// Summary: Destroys a CXTPDockBar object, handles cleanup and de-allocation.
	virtual ~CXTPDockBar();
	
	
	// Returns: Position of the dock bar.
	// Summary: Retrieves the dockbar position.
	XTPBarPosition GetPosition();
	
	// Returns: TRUE if docked vertically; otherwise returns FALSE
	// Summary: Checks if dockbar is vertical.
	BOOL IsVerticalPosition();

private:
	BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL DockCommandBar(CXTPToolBar* bBar);	
	void DockCommandBar(CXTPToolBar* pBar, LPCRECT lpRect);
	BOOL RemoveCommandBar(CXTPToolBar* pBar, int nPosExclude = -1);

	CXTPToolBar* GetDockedCommandBar(int nPos) const;
	int GetDockedCount() const;
	int GetDockedVisibleCount() const;
	int FindBar(CXTPToolBar* pBar, int nPosExclude = -1);
	int Insert(CXTPToolBar* pBarIns, CRect rect, CPoint ptMid);
	
	void GetVisibleToolbars(int nPos, CXTPToolBarArray& arrBars);
	void AdjustStretchBars();
	
	CFrameWnd* GetDockingFrame()
	{
		return (CFrameWnd*)GetParent();
	}
	int AdjustRow(CXTPToolBarArray&, CPoint pt, int nLength, BOOL bHorz, AFX_SIZEPARENTPARAMS* lpLayout);
	int _AdjustRow(CXTPToolBarArray&, CPoint pt, int nLength, BOOL bHorz, AFX_SIZEPARENTPARAMS* lpLayout, int& nRemove);
	int _GetMode(BOOL bHorz, CXTPToolBar* pBar);
	
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz, AFX_SIZEPARENTPARAMS* lpLayout);
	CSize CalcDynamicLayout(int, DWORD nMode, AFX_SIZEPARENTPARAMS* lpLayout);

private:
	DWORD m_dwStyle;
	CXTPToolBarArray m_arrBars;
	CXTPCommandBars* m_pCommandBars;

private:
	DECLARE_MESSAGE_MAP()
	
	//{{AFX_MSG(CXTPDockBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_DYNAMIC(CXTPDockBar)
	friend class CXTPToolBar;		
	friend class CXTPCommandBars;	

};

#endif // #if !defined(__XTPDOCKBAR_H__)