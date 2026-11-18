// XTPDockingPaneContext.h : interface for the CXTPDockingPaneContext class.
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
#if !defined(__XTPDOCKINGPANECONTEXT_H__)
#define __XTPDOCKINGPANECONTEXT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneBase.h"

class CXTPDockingPane;
class CXTPDockingPaneManager;

//////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContext is a stand alone class. It is used internally for docking and sizing panes.

class _XTP_EXT_CLASS CXTPDockingPaneContext
{

public:

	// Input:	pManager - Points to a CXTPDockingPaneManager object
	// Returns: A CXTPDockingPaneContext object
	// Summary: Constructs a CXTPDockingPaneContext object
	CXTPDockingPaneContext(CXTPDockingPaneManager* pManager);

public:

	// Input:	pPane - Points to a CXTPDockingPaneBase object
	//			point - Initial mouse position.
	// Summary: Call this member to drag a pane.
	void Drag(CXTPDockingPaneBase* pPane, CPoint point);

private:
	void Track();
	void Move(CPoint pt);
	void InitLoop();
	void CancelLoop();
	
	static void AdjustRectangle(CRect& rect, CPoint pt);
	
	void _CreateRectangleRgn(CRgn& rgnResult, CRect rc);
	void _CreateRgn(CRgn& rgnResult, CRect rc, BOOL bTabbedRgn, BOOL bRemove = FALSE);
	void DrawFocusRect(BOOL bRemoveRect = FALSE);

	void FindContainer(CPoint pt);
	BOOL CanDock(CRect rc, CPoint pt, CXTPDockingPaneBase* pPane, BOOL bInside = TRUE);

private:
	CPoint m_ptLast;
	CPoint m_ptSticky;
	CRect m_rectDragFrame;
	
	CXTPDockingPaneBase* m_pPane;	
	CXTPDockingPaneBase* m_pContainer;
	CRect m_rectContainer;
	BOOL m_bAttachLast;
	BOOL m_bAttach;
	BOOL m_bFloatable;

	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bDitherLast;
	
	XTPDockingPaneDirection m_containDirection;
	CXTPDockingPaneManager* m_pManager;

	CDC* m_pDC;

};

#endif // #if !defined(__XTPDOCKINGPANECONTEXT_H__)