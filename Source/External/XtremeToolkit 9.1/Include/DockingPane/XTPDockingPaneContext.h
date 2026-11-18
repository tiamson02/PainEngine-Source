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
	virtual void Move(CPoint pt);
	virtual void InitLoop();
	virtual void CancelLoop();
	virtual void DrawFocusRect(BOOL bRemoveRect = FALSE);

protected:
	static void AdjustRectangle(CRect& rect, CPoint pt);
	
	void _CreateRectangleRgn(CRgn& rgnResult, CRect rc);
	void _CreateRgn(CRgn& rgnResult, CRect rc, BOOL bTabbedRgn, BOOL bRemove = FALSE);

	virtual void FindContainer(CPoint pt);
	BOOL CanDock(CRect rc, CPoint pt, CXTPDockingPaneBase* pPane, BOOL bInside = TRUE);


protected:
	void CreateContextWindow(CXTPDockingPaneContextAlphaWnd* pWnd);
	void UpdateDockingStickers();
	void DestroyDockingStickers();

	CXTPDockingPaneContextStickerWnd* CreateNewSticker(CRect rc, XTPDockingStickerType typeSticker);

private:
	void RegionFromBitmap(CRgn* pRgn, CDC* pDC, CRect rc);

protected:
	CXTPDockingPaneContextAlphaWnd m_wndContext;
	CXTPDockingPaneContextAlphaWnd m_wndAttachedTab;

	PVOID m_pfnSetLayeredWindowAttributes;
	HMODULE m_hUser32;

	BOOL m_bUseDockingStickers;
	BOOL m_bUseAlphaContext;

	CRect m_rectStickerPane;
	CXTPDockingPaneBase* m_pStickerPane;

	CList<CXTPDockingPaneContextStickerWnd*, CXTPDockingPaneContextStickerWnd*> m_lstStickers;

protected:
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

	CRgn m_rgnStickers[5];
	
	XTPDockingPaneDirection m_containDirection;
	CXTPDockingPaneManager* m_pManager;

	CDC* m_pDC;

private:
	friend class CXTPDockingPaneContextAlphaWnd;
};

#endif // #if !defined(__XTPDOCKINGPANECONTEXT_H__)