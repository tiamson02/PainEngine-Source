// XTPShortcutBarPaneItem.h interface for the CXTPShortcutBarPaneItem class.
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
#if !defined(__XTPSHORTCUTBARPANE_H__)
#define __XTPSHORTCUTBARPANE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPShortcutBar;



//////////////////////////////////////////////////////////////////////
// Summary: CXTPShortcutBarPaneItem is the base class representing an item of the panes. 
class _XTP_EXT_CLASS CXTPShortcutBarPaneItem
{
protected:

	// Input:	lpszCaption - Caption of the item
	//			pWnd - Child window associated with item
	//			nHeight - Height of the item
	// Summary: Constructs a CXTPShortcutBarPaneItem object.
	CXTPShortcutBarPaneItem(LPCTSTR lpszCaption, CWnd* pWnd, int nHeight);

public:

	// Summary: Returns rectangle of caption.
	CRect GetCaptionRect();
	
	// Summary: Retrieevs the caption of the pane.
	CString GetCaption();
	
	// Input:	nHeight - Height of client area of the item.
	// Summary: Call this method to set height of the item.
	void SetHeight(int nHeight);
	
	// Returns: Height of the item.
	// Summary: Call this method to determine height of the item.
	int GetHeight();

protected:
	CString m_strCaption;	// Caption of the item
	CRect m_rcCaption;		// Rectangle of the caption.
	CRect m_rcClient;		// Bounding rectangle of the client area.
	CWnd* m_pWndClient;		// Associated child window.
	int m_nHeight;			// Height of the item.


	friend class CXTPShortcutBarPane;	
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPShortcutBarPane is a CWnd derived class. It is used as client area
//			of ShortcutBar control. 
class _XTP_EXT_CLASS CXTPShortcutBarPane : public CWnd
{
// Construction
public:

	// Summary: Constructs a CXTPShortcutBarPane object.
	CXTPShortcutBarPane();

// Attributes
public:

	// Input:	lpszCaption - Caption of the pane.
	//			pParent - parent CXTPShortcutBar class.
	// Summary: This method creates the CXTPShortcutBarPane control. 
	BOOL Create(LPCTSTR lpszCaption, CXTPShortcutBar* pParent);
	
	// Summary: Call this method to get the caption of the pane.
	CString GetCaption();

	// Input:	lpszCaption - Caption of the item that is to be added.
	//			pWnd - Associated child window of the item.
	//			nHeight - Height of the item.
	// Summary: Call this method to add sub item of pane.
	CXTPShortcutBarPaneItem* AddItem(LPCTSTR lpszCaption, CWnd* pWnd, int nHeight);

	// Input:	nIndex - Specifies the zero-based index of the item to retrieve. 
	// Returns: A pointer to a CXTPShortcutBarPaneItem object.
	// Summary: This method retrieves item with specified index. 
	CXTPShortcutBarPaneItem* GetItem(int nIndex);

protected:
	
	CXTPShortcutBar* m_pShortcutBar;										// Parent CXTPShortcutBar class.
	CString m_strCaption;													// Caption of the pane.

	CArray<CXTPShortcutBarPaneItem*, CXTPShortcutBarPaneItem*> m_arrItems;  // Array of items.

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPShortcutBarPane)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	// Summary: Destroys a CXTPShortcutBarPane object, handles cleanup and de-allocation.
	virtual ~CXTPShortcutBarPane();

	// Generated message map functions
protected:

//:Ignore
	//{{AFX_MSG(CXTPShortcutBarPane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
//:End Ignore

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


AFX_INLINE CString CXTPShortcutBarPane::GetCaption() {
	return m_strCaption;
}

AFX_INLINE CRect CXTPShortcutBarPaneItem::GetCaptionRect() {
	return m_rcCaption;
}
AFX_INLINE int CXTPShortcutBarPaneItem::GetHeight() {
	return m_nHeight;
}
AFX_INLINE CString CXTPShortcutBarPaneItem::GetCaption() {
	return m_strCaption;
}


#endif // !defined(__XTPSHORTCUTBARPANE_H__)
