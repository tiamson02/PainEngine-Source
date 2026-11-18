// XTPTaskPanelGroupItem.h interface for the ÑXTPTaskPanelGroup class.
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
#if !defined(__XTPTASKPANELGROUPITEM_H__)
#define __XTPTASKPANELGROUPITEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPTaskPanelItem.h"

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelGroupItem is a CXTPTaskPanelItem derived class. It is used in TaskPanel control
//			as single item of the group.
class _XTP_EXT_CLASS CXTPTaskPanelGroupItem : public CXTPTaskPanelItem
{
	DECLARE_SERIAL(CXTPTaskPanelGroupItem)

	// protected constructor used by dynamic creation
	CXTPTaskPanelGroupItem();           

// Operations
public:

	// Input:	rc - New rectangle of the item.
	// Summary: This method is called when item must be repositioned.
	virtual CRect OnReposition(CRect rc);
	
	// Input:	pDC - Pointer to a valid device context
	//			rc - Rectangle of item to draw.
	// Summary: This method is called to draw item in specified rectangle.
	virtual void OnDrawItem(CDC* pDC, CRect rc);

	// Summary: Call this method to retrieve the margins of the item.
	CRect& GetMargins();

	// Summary: Retrieves rectangle of the item.
	CRect& GetItemRect();

	// Returns: Parent group of the item.
	// Summary: Retrieves the parent group of the item.
	CXTPTaskPanelGroup* GetItemGroup();

	// Input:   TRUE to use bold font.
	// Summary: Call this method to set bold font for the item to be used.
	void SetBold(BOOL bBold = TRUE);
	
	// Summary: Call this member to retrieve if the font is bold.
	BOOL IsBold();
	
	// Input:	hWnd - Window handle to attach.
	// Summary: This method attaches control handle to item.
	void SetControlHandle(HWND hWnd);

	// Input:	nStep - Step of animation.
	// Summary: This method is called when an animation step is executed.
	void OnAnimate(int nStep);

	// Inputs:  szItem - Item size.
	// Summary: Call this method to set the size of the item.
	void SetSize(CSize szItem);

	COLORREF GetBackColor();

	BOOL IsItemSelected();
	void SetItemSelected(BOOL bSelected);

public:
	void Serialize(CArchive& ar);

private:
	void GetPreviewBitmap(CWnd* pWnd, CBitmap& bmp);

	
// Attributes
protected:

	CRect m_rcMargins;	// Item's margins.
	CRect m_rcItem;		// Rectangle of the item.

	BOOL m_bBold;		// TRUE to use bold font.
	BOOL m_bSelected;
	
	HWND m_hWnd;
	CSize m_szItem;
	CBitmap m_bmpPreview;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPTaskPanelGroupItem)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Summary: Destroys a CXTPTaskPanelGroupItem object, handles cleanup and de-allocation.
	virtual ~CXTPTaskPanelGroupItem();

	// Generated message map functions
	//{{AFX_MSG(CXTPTaskPanelGroupItem)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG


//:Ignore
		
#ifdef _XTP_ACTIVEX

	afx_msg void OleSetMargins(long nLeft, long nTop, long nRight, long nBottom);
	afx_msg void OleSetControl(LPDISPATCH lpDispatch);
	
	afx_msg LPDISPATCH OleGetControl() {return NULL;}
	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


	DECLARE_OLETYPELIB_EX(CXTPTaskPanelGroupItem)

	enum {
		dispidBold = 10L,
		dispidSetMargins = 11L,
		dispidControl = 12L,
	};

#endif
//:End Ignore

};

AFX_INLINE CRect& CXTPTaskPanelGroupItem::GetMargins()
{
	return m_rcMargins;
}
AFX_INLINE CRect& CXTPTaskPanelGroupItem::GetItemRect() {
	return m_rcItem;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTPTASKPANELGROUPITEM_H__99E610A7_0108_4733_BFA7_D2D0CB5CF1BA__INCLUDED_)
//:End Ignore