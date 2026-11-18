// XTPPopupItem.h: interface for the CXTPPopupItem class.
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

#if !defined(_XTPPOPUPITEM_H__)
#define _XTPPOPUPITEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTPPopupItem.h : header file
//

class CXTPPopupControl;


//---------------------------------------------------------------------------
// Summary: Enumeration used to determine the icon item type
//---------------------------------------------------------------------------
enum XTPPopupItemIcon
{ 
	xtpPopupItemIconNormal = 1,		//normal icon handler
	xtpPopupItemIconSelected = 2,	//selected icon handler
	xtpPopupItemIconPressed = 4		//pressed icon handler
}; 


//===========================================================================
// Summary: CXTPPopupItem is a class used to management popup item object 
//===========================================================================

class _XTP_EXT_CLASS  CXTPPopupItem : public CCmdTarget
{
	DECLARE_DYNCREATE(CXTPPopupItem)

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPopupItem object.
	// Input:   rcItem  - rect of an item.
	//          lpszCaption -  NULL terminated string specifying the caption 
	//          text.
	//-----------------------------------------------------------------------
	CXTPPopupItem(LPRECT rcItem = NULL, LPCTSTR lpszCaption = NULL, COLORREF clrBackground = (COLORREF)-1, COLORREF clrBorder = (COLORREF)-1); 

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPopupItem object.
	//-----------------------------------------------------------------------
	~CXTPPopupItem();


	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the item.
	// Input:   pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get rectangular of an item.
	// Returns: CRect object containing rectangular of an item.
	//-----------------------------------------------------------------------
	CRect GetRect();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get button state of an item.
	// Returns: TRUE if button state is enabled, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsButton();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set button state of an item.
	// Input:   bButton - TRUE to enable button state, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetButton(BOOL bButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get selected state of an item.
	// Returns: TRUE if item selected, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsSelected();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get pressed state of an item.
	// Returns: TRUE if item pressed, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsPressed();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set ID number for an item.
	// Input:   nID - value of ID number.
	//-----------------------------------------------------------------------
	void SetID(int nID);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get ID number of an item.
	//          window.
	// Returns: Value of ID number.
	//-----------------------------------------------------------------------
	int GetID();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the text of an item.
	// Input:   str -  a CString object that represents the item text.
	//-----------------------------------------------------------------------
	void SetCaption(CString str);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the text of an item.
	// Returns: A CString object that represents the item text.
	//-----------------------------------------------------------------------
	CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the color of an item text.
	// Input:   clrText -  COLORREF object that represents the color of an item 
	//          text.
	//-----------------------------------------------------------------------
	void SetTextColor(COLORREF clrText);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the color of an item text.
	// Returns: A COLORREF object that represents the color of an item text.
	//-----------------------------------------------------------------------
	COLORREF GetTextColor();

	COLORREF GetBackgroundColor();
	COLORREF GetBorderColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the alignment of an item text.
	// Input:   nAlign -  The alignment style of an item text.
	//-----------------------------------------------------------------------
	void SetTextAlignment(int nAlign);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the alignment of an item text.
	// Returns: The alignment style of an item text.
	//-----------------------------------------------------------------------
	int GetTextAlignment();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the font of an item text.
	// Input:   pFntText -  point to CFont object.
	//-----------------------------------------------------------------------
	void SetTextFont(CFont * pFntText);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the font of an item text.
	// Input:   pLogfText -  point to LOGFONT structure.
	//-----------------------------------------------------------------------
	void SetTextFont(PLOGFONT pLogfText);

	//-----------------------------------------------------------------------
	// Summary: Call this member function togset the font of an item text.
	// Returns:   Point to CFont object.
	//-----------------------------------------------------------------------
	virtual CFont* GetTextFont();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get hyperlink support of a text 
	//          item.
	// Returns: TRUE if hyperlink support, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHyperLink();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set hyperlink support of a text 
	//          item.
	// Input:   bHyperLink - TRUE to set hyperlink support, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetHyperLink(BOOL bHyperLink);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get bold state of a text item.
	// Returns: TRUE if item state is bold, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsBold();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set bold state of a text item.
	// Input:   bHyperLink - TRUE to set bold state, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetBold(BOOL bBold);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to update rectangular height of a text.
	//-----------------------------------------------------------------------
	void CalculateHeight();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to update rectangular width of a text.
	//-----------------------------------------------------------------------
	void CalculateWidth();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the normal, selected ore pressed
	//          icon for icon item.
	// Input:   hIcon - Handle to the icon.
	//          itemIcon - type of icon in use, see XTPPopupItemIcon.
	//-----------------------------------------------------------------------
	void SetIcon(HICON hIcon, XTPPopupItemIcon itemIcon = xtpPopupItemIconNormal);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the normal, selected ore pressed
	//          icon from resource bitmap picture.
	// Input:   nIDBitmap - Resource ID for the bitmap.
	//			clrTransparent - transparent color.
	//          itemIcon - type of icon in use, see XTPPopupItemIcon.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetIcons(UINT nIDBitmap, COLORREF clrTransparent, int itemIcon);

	BOOL SetIcons(HBITMAP hBitmap, COLORREF clrTransparent, int itemIcon);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get a handle the normal, selected
	//          ore pressed icon from icon item.
	// Input:   itemIcon - type of icon, see XTPPopupItemIcon.
	// Returns: A handle to the icon.
	//-----------------------------------------------------------------------
	HICON GetIcon(XTPPopupItemIcon xtpIcon = xtpPopupItemIconNormal);

	void CenterIcon();
	CPoint GetIconOffset();


protected:
	void RedrawControl();			//redraw item proc
	virtual void OnItemInserted();	

// Attributes
protected:
	BOOL m_bButton;					//flag of button state
	int m_nID;						//ID number of an item
	CRect m_rcItem;					//rectangular of  an item
	CXTPPopupControl* m_pControl;	//poin to CXTPPopupControl object containing this item

protected:
	CFont m_fntText;		//font of an item text
	BOOL m_bHyperLink;		//flag of a hyperlink support
	CString m_strCaption;	//caption string
	int m_nAlign;			//alignment style of an item text
	BOOL m_bBold;			//flag of bold state text

	COLORREF m_clrText;		//text color
	COLORREF m_clrBackground;	//background color
	COLORREF m_clrBorder;		//border color

	HICON m_hNormal;	//handle to normal icon
	HICON m_hSelected;	//handle to selected icon
	HICON m_hPressed;	//handle to pressed icon

	CPoint m_ptOffset;

// Operations
public:

// Implementation
protected:


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	
	DECLARE_OLETYPELIB_EX(CXTPPopupItem)

	void OleSetIcons(HBITMAP hBitmap, COLORREF clrTransparent, int itemIcon);
	LPFONTDISP OleGetFont();
	void OleSetFont(LPFONTDISP pFontDisp);

	CFontHolder m_oleFont;

	enum {
		dispidCaption = 1L,
	};
	

#endif

	friend class CXTPPopupControl;
};



AFX_INLINE CRect CXTPPopupItem::GetRect() {
	return m_rcItem;
}
AFX_INLINE BOOL CXTPPopupItem::IsButton() {
	return m_bButton;
}
AFX_INLINE void CXTPPopupItem::SetButton(BOOL bButton) {
	m_bButton = bButton;
}
AFX_INLINE void CXTPPopupItem::SetID(int nID) {
	m_nID = nID;
}
AFX_INLINE int CXTPPopupItem::GetID() {
	return m_nID;
};
AFX_INLINE COLORREF CXTPPopupItem::GetTextColor() {
	return m_clrText;
}
AFX_INLINE int CXTPPopupItem::GetTextAlignment() {
	return m_nAlign;
}
AFX_INLINE void CXTPPopupItem::SetTextAlignment(int nAlign) {
	m_nAlign = nAlign;
	RedrawControl();
}
AFX_INLINE void CXTPPopupItem::SetCaption(CString str) {
	m_strCaption = str;
	RedrawControl();
}
AFX_INLINE CString CXTPPopupItem::GetCaption() {
	return m_strCaption;
}
AFX_INLINE void CXTPPopupItem::SetTextColor(COLORREF clrText) {
	m_clrText = clrText;
	RedrawControl();
}
AFX_INLINE BOOL CXTPPopupItem::IsHyperLink() {
	return m_bHyperLink;
}
AFX_INLINE BOOL CXTPPopupItem::IsBold() {
	return m_bBold;
}
AFX_INLINE void CXTPPopupItem::SetBold(BOOL bBold) {
	m_bBold = bBold;
	RedrawControl();
}
AFX_INLINE void CXTPPopupItem::SetHyperLink(BOOL bHyperLink) {
	m_bHyperLink = bHyperLink;
}
AFX_INLINE COLORREF CXTPPopupItem::GetBackgroundColor() {
	return m_clrBackground;
}
AFX_INLINE COLORREF CXTPPopupItem::GetBorderColor() {
	return m_clrBorder;
}
AFX_INLINE CPoint CXTPPopupItem::GetIconOffset() {
	return m_ptOffset;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_XTPPOPUPITEM_H__)
