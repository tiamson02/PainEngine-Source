// XTPControlPopupColor.h : interface for the CXTPControlPopupColor class.
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
#if !defined(__XTPCONTROLPOPUPCOLOR_H__)
#define __XTPCONTROLPOPUPCOLOR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


class CXTPImageManagerIcon;
#define XTP_SELECTOR_SIZE 18
#define XTP_SELECTOR_MARGIN 2

#define XTP_SELECTOR_WIDTH (XTP_SELECTOR_SIZE * 8 + XTP_SELECTOR_MARGIN * 2)
#define XTP_SELECTOR_HEIGHT (XTP_SELECTOR_SIZE * 5 + XTP_SELECTOR_MARGIN * 2)

// Summary: CXTPControlPopupColor is a CXTPControlPopup derived class. 
//			It represents a color picker control.
class _XTP_EXT_CLASS CXTPControlPopupColor : public CXTPControlPopup
{
	DECLARE_XTP_CONTROL(CXTPControlPopupColor)

public:

	// Summary: Constructs a CXTPControlPopupColor object
	CXTPControlPopupColor(void);
	
	// Summary: Destroys a CXTPControlPopupColor object, handles cleanup and de-allocation
	~CXTPControlPopupColor(void);

	// Input:   clr - Color to set.
	// Summary: Call this member to set the picker color.
	void SetColor(COLORREF clr);

	// Returns: Color of the color picker.
	// Summary: Call this member to retrieve the control color.
	COLORREF GetColor();

protected:

	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

private:
	
	HICON RerdawIcon(CXTPImageManagerIcon* pImage);

private:

	COLORREF m_clr;		
};

// Summary: CXTPControlButtonColor is a CXTPControlButton derived class. 
//			It represents a text button in the color picker popup.
class _XTP_EXT_CLASS CXTPControlButtonColor: public CXTPControlButton
{
	DECLARE_XTP_CONTROL(CXTPControlButtonColor)
public:

	// Summary: Constructs a CXTPControlButtonColor object
	CXTPControlButtonColor()
	{
		SetStyle(xtpButtonCaption);
	}
protected:
	
	// Input:	pDC - Pointer to a valid device context
	// Returns: Size of the control.
	// Summary: This method is called to determine the size of the control.
	CSize GetSize(CDC* pDC);
	
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	void Draw(CDC* pDC);
};

// Summary: CXTPControlColorSelector is a CXTPControl derived class. 
//			It represents a color selector control.
class _XTP_EXT_CLASS CXTPControlColorSelector: public CXTPControl
{
	DECLARE_XTP_CONTROL(CXTPControlColorSelector)
public:	

	// Summary: Constructs a CXTPControlColorSelector object
	CXTPControlColorSelector();

	// Summary: Internal helper class.
	struct CColorInfo;			

	// Returns: Color of the selector.
	// Summary: Call this member to retrieve the color of the selector control.
	COLORREF GetColor();

	// Input:	clr - Color to set.
	// Summary: Call this method to set the color of the control.
	void SetColor(COLORREF clr);

protected:

	// Input:	pDC - Pointer to a valid device context
	// Returns: Size of the control.
	// Summary: This method is called to determine the size of the control.
	CSize GetSize(CDC* pDC);
	
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	void Draw(CDC* pDC);
	
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the mouse cursor moves.
	void OnMouseMove(CPoint point);
	
	// Input:	bSelected - TRUE if the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called when the control becomes selected.
	BOOL OnSetSelected(int bSelected);
	
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	// Returns: Tooltip of the control.
	// Summary: Call this member to get the control's tooltip.
	CString  GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, int* nHit = 0);
	
	// Summary: This method is called when the control is executed.
	void OnExecute();

private:

	CRect GetRect(int nIndex);	
	int HitTest(CPoint point);

private:
	
	int m_nSelected;	
	int m_nPressed;		
	COLORREF m_clr;		
};


//////////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTPControlPopupColor::GetColor() {
	return m_clr;
}

#endif // #if !defined(__XTPCONTROLPOPUPCOLOR_H__)
