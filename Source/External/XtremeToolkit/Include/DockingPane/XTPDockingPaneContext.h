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
class CXTPDockingPaneContext;


enum XTPDockingStickerType
{
	xtpStickerNone = 0,

	xtpStickerLeft = 1,
	xtpStickerRight = 2,
	xtpStickerTop = 4,
	xtpStickerBottom = 8,
	xtpStickerCenter = 16,
	
	xtpStickerClient = xtpStickerLeft + xtpStickerRight + xtpStickerTop + xtpStickerBottom,
	xtpStickerPane = xtpStickerClient + xtpStickerCenter
};

/////////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneContextAlphaWnd window

class CXTPDockingPaneContextAlphaWnd : public CWnd
{
	friend class CXTPDockingPaneContext;
// Construction
public:
	CXTPDockingPaneContextAlphaWnd();

// Attributes
public:
	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPDockingPaneContextAlphaWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXTPDockingPaneContextAlphaWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPDockingPaneContextAlphaWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CXTPDockingPaneContextStickerWnd : public CWnd
{
	friend class CXTPDockingPaneContext;

public:	
	struct SPRITEINFO;
// Construction
public:
	CXTPDockingPaneContextStickerWnd();

// Attributes
public:
	XTPDockingStickerType m_typeSticker;
	XTPDockingStickerType m_selectedSticker;

	XTPDockingStickerType HitTest(CPoint pt);

	void OnDraw(CDC* pDC);

protected:
	void DrawSprite(CDC* pDC, UINT nID, SPRITEINFO* pSpriteInfo, BOOL bClientBitmap = TRUE);
	void DrawTransparent(CDC* pDC , const CPoint& ptDest, const CSize& sz, CBitmap* pBitmap);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPDockingPaneContextStickerWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXTPDockingPaneContextStickerWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPDockingPaneContextStickerWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//===========================================================================
// Remarks: CXTPDockingPaneContext is a stand alone class. It is used internally 
//			for docking and sizing panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneContext
{
	friend class CXTPDockingPaneManager;

public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPaneContext object
	// Returns: A CXTPDockingPaneContext object
	//-----------------------------------------------------------------------
	CXTPDockingPaneContext();

	virtual ~CXTPDockingPaneContext();

public:

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	//			point - Initial mouse position.
	// Summary: Call this member to drag a pane.
	//-----------------------------------------------------------------------
	virtual void Drag(CXTPDockingPaneBase* pPane, CPoint point);

protected:

	//-----------------------------------------------------------------------
	// Summary: This method is called to track the size of the pane.
	//-----------------------------------------------------------------------
	virtual void Track();

	//-----------------------------------------------------------------------
	// Input:	pt - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	virtual void Move(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to initialize a loop.
	//-----------------------------------------------------------------------
	virtual void InitLoop();

	//-----------------------------------------------------------------------
	// Summary: This member function is called when a loop is cancelled.
	//-----------------------------------------------------------------------
	virtual void CancelLoop();

	//-----------------------------------------------------------------------
	// Input:	bRemoveRect - NEEDS COMMENT
	// Summary: This member function is called to draw the focus rectangle.
	//-----------------------------------------------------------------------
	virtual void DrawFocusRect(BOOL bRemoveRect = FALSE);

protected:
	
	//-----------------------------------------------------------------------
	// Input:	rect - Rectangle to be changed.
	//			pt   - Position of the rectangle to be changed.
	// Summary: Call this member to adjust the rectangle.
	//-----------------------------------------------------------------------
	static void AdjustRectangle(CRect& rect, CPoint pt);
	
	//-----------------------------------------------------------------------
	// Input:	rgnResult - NEEDS COMMENT
	//			rc        - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void _CreateRectangleRgn(CRgn& rgnResult, CRect rc);

	//-----------------------------------------------------------------------
	// Input:	rgnResult  - NEEDS COMMENT
	//			bTabbedRgn - NEEDS COMMENT
	//			bRemove    - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void _CreateRgn(CRgn& rgnResult, CRect rc, BOOL bTabbedRgn, BOOL bRemove = FALSE);

	//-----------------------------------------------------------------------
	// Input:	pt - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	virtual void FindContainer(CPoint pt);

	//-----------------------------------------------------------------------
	// Input:	rc		- NEEDS COMMENT
	//			pt		- Position of the dock bar.
	//			pPane   - Pointer to a CXTPDockingPaneBase object.
	//			bInside - NEEDS COMMENT
	// Summary: Call this member to retrieve the dock bar at the specified point.
	// Returns: NEEDS COMMENT
	//-----------------------------------------------------------------------
	BOOL CanDock(CRect rc, CPoint pt, CXTPDockingPaneBase* pPane, BOOL bInside = TRUE);


protected:
	//-----------------------------------------------------------------------
	// Input:	pWnd - A pointer to a CXTPDockingPaneContextAlphaWnd object.
	// Summary: Call this member function to create a context menu.
	//-----------------------------------------------------------------------
	void CreateContextWindow(CXTPDockingPaneContextAlphaWnd* pWnd);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to refresh the docking stickers.
	// Returns: TRUE if updated successfully; FALSE otherwise.
	//-----------------------------------------------------------------------
	void UpdateDockingStickers();
	
	//-----------------------------------------------------------------------
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void DestroyDockingStickers();

	//-----------------------------------------------------------------------
	// Index:	rc          - NEEDS COMMENT
	//			typeSticker - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	// Returns: NEEDS COMMENT
	//-----------------------------------------------------------------------
	CXTPDockingPaneContextStickerWnd* CreateNewSticker(CRect rc, XTPDockingStickerType typeSticker);

private:
	
	void RegionFromBitmap(CRgn* pRgn, CDC* pDC, CRect rc);
	BOOL IsAllowDockingTo(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction);
	BOOL IsAllowAttachTo(CXTPDockingPaneBase* pPane);

protected:
	
	CXTPDockingPaneContextAlphaWnd m_wndContext;		// Alpha context control.
	CXTPDockingPaneContextAlphaWnd m_wndAttachedTab;	// Attached tab control.

	PVOID m_pfnSetLayeredWindowAttributes;				// Pointer to SetLayeredWindowAttributes method.

	BOOL m_bUseDockingStickers;							// TRUE to use docking stickers.
	BOOL m_bUseAlphaContext;							// TRUE to use alpha context.

	CRect m_rectStickerPane;							// Area occupied by the sticker pane.
	CXTPDockingPaneBase* m_pStickerPane;				// Sticker Pane.

	CList<CXTPDockingPaneContextStickerWnd*, CXTPDockingPaneContextStickerWnd*> m_lstStickers;  // 

protected:
	CPoint m_ptLast;									// Holds the last selection point.
	CPoint m_ptSticky;									// Position of the sticky control.
	CRect m_rectDragFrame;								// Area occupied by the drag frame.
	
	CXTPDockingPaneBase* m_pPane;						// Pointer to the associated pane.
	CXTPDockingPaneBase* m_pContainer;
	CRect m_rectContainer;
	BOOL m_bAttachLast;
	BOOL m_bAttach;
	BOOL m_bFloatable;

	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bDitherLast;

	CRgn m_rgnStickers[5];
	
	XTPDockingPaneDirection m_containDirection;
	CXTPDockingPaneManager* m_pManager;

	CDC* m_pDC;

private:
	friend class CXTPDockingPaneContextAlphaWnd;
};

#endif // #if !defined(__XTPDOCKINGPANECONTEXT_H__)