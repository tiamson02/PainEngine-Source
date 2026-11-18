// XTPPropertyGridItemFont.h interface for the CXTPPropertyGridItemFont class.
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
#if !defined(__XTPPROPERTYGRIDITEMFONT_H__)
#define __XTPPROPERTYGRIDITEMFONT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks: CXTPPropertyGridItemFont is a CXTPPropertyGridItem derived class.  
//			It is used to create a Font value item in a Property Grid control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemFont : public CXTPPropertyGridItem
{
public:
	
	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption of the item.
	//			font - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemFont object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemFont(CString strCaption, LOGFONT& font);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemFont object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemFont(UINT nID, LOGFONT& font);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridItemFont object.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridItemFont(void);

public:

	//-----------------------------------------------------------------------
	// Input:	font - The new value of the item.
	// Summary: Call this method to change the item's value.
	//-----------------------------------------------------------------------
	void SetFont(LOGFONT& font);
	
	//-----------------------------------------------------------------------
	// Input:	lf - Pointer to the LOGFONT structure to receive the font information.
	// Summary: Call this method to get the LOGFONT value of the item.
	//-----------------------------------------------------------------------
	void GetFont(LOGFONT* lf);

	void SetColor(COLORREF clr);
	COLORREF GetColor();

protected:
	
	//-----------------------------------------------------------------------
	// Summary: This method is called when the user presses the inplace button.
	//			Override the method to show an item-specific dialog.
	//-----------------------------------------------------------------------
	virtual void OnInplaceButtonDown();
	BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	CRect GetValueRect();

protected:
	
	LOGFONT m_lfValue;		// Value of the item.
	COLORREF m_clrValue;


protected:
	DECLARE_DYNAMIC(CXTPPropertyGridItemFont)

//:Ignore
#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPropertyGridItemFont)
	
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
#endif
//:End Ignore
};

//////////////////////////////////////////////////////////////////////


//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTPPROPERTYGRIDITEMFONT_H__)
//:End Ignore