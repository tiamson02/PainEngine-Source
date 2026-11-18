// XTPDockContext.h : interface for the CXTPDockContext class.
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
#if !defined(__XTPDOCKCONTEXT_H__)
#define __XTPDOCKCONTEXT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"

class CXTPToolBar;

// CXTPDockContext is a stand alone class. It is used internally by CXTPToolBar.

class _XTP_EXT_CLASS CXTPDockContext
{
public:

	// Input:	pBar - Points to a CXTPToolBar object
	// Summary: Constructs a CXTPDockContext object
	CXTPDockContext(CXTPToolBar* pBar);
	
	// Summary: Destroys a CXTPDockContext object, handles cleanup and de-allocation.
	~CXTPDockContext(void);

	// Input:	pt - Initial cursor position.
	// Summary: This method is called when the user starts to move the toolbar.
	void StartDrag(CPoint pt);
	
	// Input:	nHitTest - Specifies the hit-test area code.
	//			pt - Initial cursor position.
	// Summary: This method is called when the user starts to resize the toolbar.
	void StartResize(int nHitTest, CPoint pt);
	
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called to switch the docking position of the toolbar.
	BOOL ToggleDocking();
	

private:
	void Track();
	void Stretch(CPoint pt);
	void Move(CPoint pt);

	static void AdjustRectangle(CRect& rect, CPoint pt);

private:
	CXTPToolBar* m_pBar;
	CFrameWnd* m_pDockSite;
	int m_nHitTest;
	CPoint m_ptLast;
	
	XTPBarPosition m_uMRUDockPosition;
	
	CRect m_rectMRUDockPos;
	CPoint m_ptMRUFloatPos;

	CRect m_rectDragDock;
	CRect m_rectDragFrame;

private:
	friend class CXTPToolBar;	
	friend class CXTPDockBar;	
};

#endif //#if !defined(__XTPDOCKCONTEXT_H__)