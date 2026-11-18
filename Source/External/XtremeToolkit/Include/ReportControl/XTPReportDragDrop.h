// XTPReportDragDrop.h: interface for the CXTPReportHeaderDragWnd and 
//						CXTPReportHeaderDropWnd classes.
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
#if !defined(__XTPREPORTDRAGDROP_H__)
#define __XTPREPORTDRAGDROP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//:Ignore

// Internal classes

class CXTPReportHeader;
class CXTPReportPaintManager;
class CXTPReportColumn;

/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDragWnd window

class _XTP_EXT_CLASS CXTPReportHeaderDragWnd : public CWnd
{
// Construction
public:

	//-----------------------------------------------------------------------
	// Summary: Creates CXTPReportHeaderDragWnd object.
	//-----------------------------------------------------------------------
	CXTPReportHeaderDragWnd();

// Attributes
public:

// Operations
public:

// Overrrides
protected:
    // Drawing

	//-----------------------------------------------------------------------
	// Input:	pDC - A pointer to the device context in which the drawing occurs. 
	//			rcClient - Coordinates of the client window position.
	// Summary: Called by the framework to draw the contents of the window using the specified device context.
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, CRect rcClient);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReportHeaderDragWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	
	//-----------------------------------------------------------------------
	// Summary: Default CXTPReportHeaderDragWnd destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHeaderDragWnd();
	
	//-----------------------------------------------------------------------
	// Input:	rect - Drawing rectangle position.
	//			pHeader - Pointer to parent report header object.
	//			pPaintManager - Paint manager for performing draw operations.
	//			pColumn - Pointer to the associated column.
	// Summary: Creates dragging window with the specified paramters.
	// Returns: TRUE if created successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Create(CRect rect, CXTPReportHeader* pHeader, CXTPReportPaintManager* pPaintManager, CXTPReportColumn* pColumn);

protected:
	
	CXTPReportHeader* m_pHeader;			 //	Pointer to the associated report header object.
	CXTPReportPaintManager* m_pPaintManager; //	Pointer to the associated paint manager object.
	CXTPReportColumn* m_pColumn;			 // Pointer to the associated report column object.

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReportHeaderDragWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXTPReportHeaderDropWnd window

//===========================================================================
// Remarks: Drop target as 2 arrows window.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportHeaderDropWnd : public CWnd
{
// Construction
public:
	//-----------------------------------------------------------------------
	// Input:	crColor - Drop window color.
	// Summary: Creates CXTPReportHeaderDropWnd object.
	//-----------------------------------------------------------------------
	CXTPReportHeaderDropWnd(COLORREF crColor);

// Attributes
public:

// Operations
public:
	//-----------------------------------------------------------------------
	// Input:	x - New horizontal position.
	//			y - New vertical position.
	// Summary: Changes window position.
	//-----------------------------------------------------------------------
	void SetWindowPos(int x, int y);

// Overrrides
protected:
    // Drawing

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReportHeaderDropWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	
	//-----------------------------------------------------------------------
	// Summary: Default destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHeaderDropWnd();
	
	//-----------------------------------------------------------------------
	// Input:	nHeight - Height between 2 arrows in pixels.
	// Summary: Creates a drop window with the specified gap inside.
	// Returns: TRUE if created successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Create(int nHeight);

protected:
	
	CBrush m_brush; // Drawing brush.
	CRgn m_rgn;		// Arrows region.
	int m_nHeight;  // Height of the gap between 2 arrows.

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReportHeaderDropWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//:End Ignore

#endif