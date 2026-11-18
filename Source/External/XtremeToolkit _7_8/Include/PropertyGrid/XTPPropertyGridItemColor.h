// XTPPropertyGridItemColor.h interface for the CXTPPropertyGridItemColor class.
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
#if !defined(__XTPPROPERTYGRIDITEMCOLOR_H__)
#define __XTPPROPERTYGRIDITEMCOLOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTPPropertyGridItemColor is a CXTPPropertyGridItem derived class.  
//			It is used to create an RGB value item in a Property Grid control.
class _XTP_EXT_CLASS CXTPPropertyGridItemColor : public CXTPPropertyGridItem
{
public:

	// Input:	strCaption - Caption of the item.
	//			clr - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemColor object
	CXTPPropertyGridItemColor(CString strCaption, COLORREF clr = 0, COLORREF* pBindColor = NULL);

	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemColor object
	CXTPPropertyGridItemColor(UINT nID, COLORREF clr = 0, COLORREF* pBindColor = NULL);

	// Summary: Destroys a CXTPPropertyGridItemColor object
	virtual ~CXTPPropertyGridItemColor(void);

public:

	// Input:	clr - The new RGB value of the item.
	// Summary: Call this method to change the item's value.
	void SetColor(COLORREF clr);

	// Returns: An RGB value.
	// Summary: Call this method to get the RGB value of the item.
	COLORREF GetColor();

	// Input:	str - String that is converted to a COLORREF.
	// Returns: An RGB value.
	// Summary: The StringToRGB function converts a string to a COLORREF.
	static COLORREF StringToRGB(CString str);

	// Input:	clr - COLORREF that is converted to a string.
	// Summary: The RGBToString function converts a COLORREF to a string.
	static CString RGBToString(COLORREF clr);

	// Input:	pBindColor - Points to a COLORREF object
	// Summary: Call this method to bind an item to a COLORREF object.	
	void BindToColor(COLORREF* pBindColor);

protected:

	// Input:	dc - Reference to the device context to be used for rendering an image
	//			of the item.
	//			rcValue - Bounding rectangle of the item.
	// Returns: TRUE if the item is self-drawn.
	// Summary: This method is called when an item is drawn. Override this function if
	//			needed.
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	
	// Returns: A CRect object that represents the bounding rectangle of the item value's text.
	// Summary: Retrieves the bounding rectangle of the item value's text.
	//			Override this member function to change it.
	virtual CRect GetValueRect();
	
	// Input:	strValue - New value of the item.
	// Summary: Call this method to change an item's value.
	//			Override this method to add new functionality.
	//			You should call the base class version of this function from your
	//			override.
	virtual void SetValue(CString strValue);
	
	// Summary: This method is called when the user presses the inplace button.
	//			Override the method to show an item-specific dialog.
	virtual void OnInplaceButtonDown();
	

protected:

	COLORREF m_clrValue;		// Value of the item.
	COLORREF* m_pBindColor;		// Binded value.

private:
	DECLARE_DYNAMIC(CXTPPropertyGridItemColor)

#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
	
	void BindDispatch();
#endif
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTPPropertyGridItemColor::GetColor() {
	return m_clrValue;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //#if !defined(__XTPPROPERTYGRIDITEMCOLOR_H__)
//:End Ignore