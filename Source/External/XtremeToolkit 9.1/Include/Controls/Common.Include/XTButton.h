  // XTButton.h : interface for the CXTButton class.
//
// This file is a part of the XTREME TOOLKIT MFC class library.
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
#if !defined(__XTBUTTON_H__)
#define __XTBUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// forwards
class CXTThemeManager;
class CXTButtonTheme;
class CXTPImageManagerIcon;

//////////////////////////////////////////////////////////////////////
// Summary: CXTButton is a CButton derived class. It is used to create flat style
//			and image buttons. This control has several features including flat,
//			semi-flat, and three dimensional borders.
class _XT_EXT_CLASS CXTButton : public CButton
{
	DECLARE_DYNAMIC(CXTButton)

public:
	
    // Summary: Constructs a CXTButton object
	CXTButton();

	// Summary: Destroys a CXTButton object, handles cleanup and de-allocation
    virtual ~CXTButton();

protected:

    int                   m_nBorderGap;    // Gap between the button edge and the image.
    int                   m_nImageGap;     // Gap between the button text and the image.
    BOOL                  m_bHilite;       // TRUE if the button is highlighted.
    BOOL                  m_bUserPosition; // TRUE if the user has defined the XY coordinates for the image and text.
    BOOL                  m_bLBtnDown;     // TRUE if the left mouse button is pressed.
    BOOL                  m_bPainted;      // Used during paint operations.
    BOOL                  m_bChecked;      // TRUE if the button is checked.
    DWORD                 m_dwxStyle;      // The style of the button.  See SetXButtonStyle().
    CSize                 m_sizeImage;     // Initial size of the button.
    CPoint                m_ptImage;       // XY location of the button image.
    CPoint                m_ptText;        // XY location of the button text.
	CXTPImageManagerIcon* m_pIcon;         // Multiple state icon manager converts icon to different states.
	CXTThemeManager*	  m_pThemeManager; // Pointer to the buttons parent theme manager.
    CXTButtonTheme*       m_pButtonTheme;  // Pointer to a user defined theme, can be NULL.

public:
	// Input:	bChecked - Specifies whether the button is to be checked.  TRUE will 
	//			check the button. FALSE will uncheck it.
    // Summary: This member function sets the highlighting state of a button control.
	//			This is the same as its CButton counterpart, except the m_bChecked flag
	//			is set.  This is done because MFC does not recognize the button as being
	//			checked unless it is a radio or check box.
	void SetStateX(BOOL bChecked);

	// Input:	nGap - Amount in pixels of the gap between the button edge and the image.
	// Returns: The previous border gap value.
    // Summary: This member function will set the gap between the button's edge and
	//			the image. 
	int SetBorderGap(int nGap);

	// Input:	nGap - Amount in pixels of the gap between the button text and the image.
	// Returns: The previous border gap value.
    // Summary: This member function will set the gap between the button's text and
	//			the image. 
	int SetImageGap(int nGap);

	// Input:	size - CSize object that represents the size of the icon.
	//			hIcon - Handle to the normal icon.
	//			hIconHot - Handle to the hot icon.
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value
	//			redraws the button. A zero value does not redraw the button. The button is
	//			redrawn by default.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: This member function will set the normal and hot state icons for
	//			the push button. 
	virtual BOOL SetIcon(CSize size, HICON hIcon, HICON hIconHot=NULL, BOOL bRedraw=TRUE);

	// Input:	size - CSize object that represents the size of the icon.
	//			nID - Resource ID for the normal icon.
	//			nHotID - Resource ID for the pressed icon.
	//			bRedraw - Specifies whether the button is to be redrawn.  A nonzero value
	//			redraws the button. A zero value does not redraw the button.  The button is
	//			redrawn by default.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: This member function will set the normal and hot state icons for
	//			the push button.
	virtual BOOL SetIcon(CSize size, UINT nID, UINT nHotID=0, BOOL bRedraw=TRUE);

	// Input:	size - CSize object that represents the size of the icon.
	//			lpszID - Resource string ID for the normal icon.
	//			lpszHotID - Resource string ID for the hot icon.
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value
	//			redraws the button. A zero value does not redraw the button. The button is
	//			redrawn by default.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: This member function will set the normal and hot state icons for
	//			the push button. 
	virtual BOOL SetIcon(CSize size, LPCTSTR lpszID, LPCTSTR lpszHotID=NULL, BOOL bRedraw=TRUE);
	
	// Input:	size - CSize object that represents the size of the bitmap.
	//			nID - Resource ID for the bitmap.
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value redraws
	//			the button. A zero value does not redraw the button. The button is redrawn
	//			by default.
	// Returns:	TRUE if successful, otherwise returns FALSE.
	// Summary:	This member function will set the bitmap associated with the button.
	virtual BOOL SetBitmap(CSize size,UINT nID,BOOL bRedraw=TRUE);

	// Input:	ptImage - XY location of the image displayed on the button.
	//			ptText - XY location of the text displayed on the button.
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value redraws
	//			the button. A zero value does not redraw the button. The button is redrawn
	//			by default.
	// Returns:	TRUE if successful, otherwise returns FALSE.
	// Summary:	This member function will set the XY location of the text and image
	//			that is displayed on the push button.
	virtual BOOL SetTextAndImagePos(CPoint ptImage,CPoint ptText,BOOL bRedraw=TRUE);

	// BULLETED LIST:

	// Input:	dwxStyle - Specifies the button style. It can be one or more of the following:
	//			[ul]
	//			[li]<b>BS_XT_FLAT</b> Draws a flat button.[/li]
	//			[li]<b>BS_XT_SEMIFLAT</b> Draws a semi-flat button.[/li]
	//			[li]<b>BS_XT_TWOROWS</b> Draws images and text that are centered.[/li]
	//			[li]<b>BS_XT_SHOWFOCUS</b> Draws a focus rect when the button has input focus.[/li]
	//			[li]<b>BS_XT_HILITEPRESSED</b> Highlights the button when pressed.[/li]
	//			[li]<b>BS_XT_XPFLAT</b> Draws a flat button ala Office XP.[/li]
	//			[li]<b>BS_XT_WINXP_COMPAT</b> Enables Windows XP themes if available.[/li]
	//			[/ul]
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value redraws
	//			the button. A zero value does not redraw the button. The button is redrawn
	//			by default.
	// Returns: The previous style that was set.
	// Summary: This member function will set the display style for the button.
	virtual DWORD SetXButtonStyle(DWORD dwxStyle,BOOL bRedraw=TRUE);

	// Returns:	The button styles for this CXTButton object.
	// Summary:	This function returns only the BS_XT_ style values, not any of the
	//			other window styles.
	virtual DWORD GetXButtonStyle();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine the gap between the
	//          button image and the button's edge.
	// Returns: The amount in pixels between the button image and border.
	//-----------------------------------------------------------------------
	int GetBorderGap() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine the gap between the 
	//          button text and the image.
	// Returns: The amount in pixels between the button text and image displayed.
	//-----------------------------------------------------------------------
	int GetImageGap() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the button should
	//          display highlights on mouse over.
	// Returns: TRUE if the button display highlights on mouse over, otherwise
	//          returns FALSE.
	//-----------------------------------------------------------------------
	BOOL GetHilite() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the user has defined
	//          specific locations for text and image display.
	// Returns: TRUE if the user has defined text and image display location.
	//-----------------------------------------------------------------------
	BOOL GetUserPosition() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the left mouse button
	//          is currently pressed while hovering the button control.
	// Returns: TRUE if the left mouse button is pressed while over the button.
	//-----------------------------------------------------------------------
	BOOL GetLBtnDown() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the button control needs
	//          repainting.
	// Returns: TRUE if the button needs to be redrawn, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetPainted() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the checked state has
	//          been defined for the button control.
	// Returns: TRUE if the checked state is defined for the button control, 
	//          otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL GetChecked() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the normal icon image
	//          used by the button control.
	// Returns: A handle to a HICON object.
	//-----------------------------------------------------------------------
	HICON GetNormalIcon();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the selected icon image
	//          used by the button control.
	// Returns: A handle to a HICON object.
	//-----------------------------------------------------------------------
	HICON GetSelectedIcon();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the disabled bitmap
	//          used by the button control.
	// Returns: A handle to a HBITMAP object.
	//-----------------------------------------------------------------------
	HBITMAP GetDisabledBitmap() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the size of the image
	//          displayed for the button control.
	// Returns: A CSize object.
	//-----------------------------------------------------------------------
	CSize GetImageSize() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the x-y location of the
	//          top left corner where the button image is displayed.
	// Returns: A CPoint object.
	//-----------------------------------------------------------------------
	CPoint GetImagePoint() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the x-y location of the
	//          top left corner where text is displayed.
	// Returns: A CPoint object.
	//-----------------------------------------------------------------------
	CPoint GetTextPoint() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve a pointer to the buttons
	//          current theme.
	// Returns: A pointer to a CXTButtonTheme object.
	//-----------------------------------------------------------------------
	CXTButtonTheme* GetTheme() const;

	//-----------------------------------------------------------------------
	// Input:   pButtonTheme - Points to a user defined CXTButtonTheme object.
	// Summary: Call this member function to set a user defined theme for the
	//          button control.
	//-----------------------------------------------------------------------
	virtual void SetTheme(CXTButtonTheme* pButtonTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to attach the theme manager for the
	//          buttons parent window.
	//-----------------------------------------------------------------------
	void AttachThemeManager(CXTThemeManager* pThemeManager);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return a pointer to the image
	//          manager for the button control.
	// Returns: A pointer to a CXTPImageManagerIcon object.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon() const;

    //-----------------------------------------------------------------------
    // Input:   clr3DFace    - An RGB value that represents the user defined
    //                         face color for three dimensional display elements.
    //          clr3DHilight - An RGB value that represents the user defined 
    //                         highlight color for three dimensional display 
    //                         elements (edges facing the light source.)
    //          clr3DShadow  - An RGB value that represents the user defined 
    //                         shadow color for three dimensional display 
    //                         elements (edges facing away from the light source).
    //          clrBtnText   - An RGB value that represents the user defined 
    //                         text color on push buttons.
    // Summary: This method will allow the user to define the default colors for the
    //          background shadow and highlight colors for the button.
    //-----------------------------------------------------------------------
	virtual void SetAlternateColors(COLORREF clr3DFace, COLORREF clr3DHilight, COLORREF clr3DShadow, COLORREF clrBtnText);

    //-----------------------------------------------------------------------
    // Input:   clrFace - An RGB value that represents the user defined face color.
    // Summary: This member function sets the default face color for the button.
    //-----------------------------------------------------------------------
	virtual void SetColorFace(COLORREF clrFace);

    //-----------------------------------------------------------------------
    // Returns: An RGB value that represents the current face color.
    // Summary: This member function gets the current face color.
    //-----------------------------------------------------------------------
    virtual COLORREF GetColorFace();

    //-----------------------------------------------------------------------
    // Input:   clrHilite - An RGB value that represents the user defined highlight color.
    // Summary: This member function sets the default highlight color for the button.
    //-----------------------------------------------------------------------
	virtual void SetColorHilite(COLORREF clrHilite);

    //-----------------------------------------------------------------------
    // Input:   clrShadow - An RGB value that represents the user defined shadow color.
    // Summary: This member function sets the default shadow color for the button.
    //-----------------------------------------------------------------------
    virtual void SetColorShadow(COLORREF clrShadow);

    //-----------------------------------------------------------------------
    // Input:   clrText - An RGB value that represents the user defined text color.
    // Summary: This member function sets the default text color for the button.
    //-----------------------------------------------------------------------
	virtual void SetColorText(COLORREF clrText);

    //-----------------------------------------------------------------------
	// Input:	clrBorder    - An RGB value that represents the user defined
	//                         border color for the flat button.
	//			clrHighlight - An RGB value that represents the user defined 
	//                         highlight color for the flat button.
	//			clrPressed   - An RGB value that represents the user defined 
	//                         color for when the flat button is pressed.
	// Summary: This member function sets the colors for the BS_XT_XPFLAT mode.
    //-----------------------------------------------------------------------
    virtual void SetXPFlatColors(COLORREF clrBorder, COLORREF clrHighlight, COLORREF clrPressed);

    //-----------------------------------------------------------------------
	// Summary: This function toggles the image display for the button on or off.
    //-----------------------------------------------------------------------
	virtual void ResetImage();

    //-----------------------------------------------------------------------
	// Input:   pFont - Points to a valid CFont object.
	// Summary: Call this member function to set the font for the button control.
	// Returns: A pointer to the previously set font for the button control.
    //-----------------------------------------------------------------------
	virtual CFont* SetFontEx(CFont* pFont);

protected:

	// Summary: This member function is called to free all associated GDI resources
	//			that have been allocated.
	virtual void CleanUpGDI();

	// Input:	bRemoveAmpersand - TRUE to remove the '&' used for mnemonics.
	// Returns: A CString object that represents the button's text.
    // Summary: Call this member function to return the button's text minus the '&'.
	virtual CString GetButtonText(BOOL bRemoveAmpersand);

	// Input:	bDepressed - true if the button is pressed, otherwise FALSE.
    // Summary: Call this helper function to set the pressed state and redraw the button.
	void NoteButtonDepressed(BOOL bDepressed);

	// Summary: Defers control initialization
	void DeferInitialUpdate();

    //:Ignore
	//{{AFX_VIRTUAL(CXTButton)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	//:End Ignore
    
	//:Ignore
	//{{AFX_MSG(CXTButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTButton::SetImageGap(int nGap) {
	int nOldGap = m_nImageGap; m_nImageGap = nGap; return nOldGap;
}
AFX_INLINE int CXTButton::SetBorderGap(int nGap) {
	int nOldGap = m_nBorderGap; m_nBorderGap = nGap; return nOldGap;
}
AFX_INLINE DWORD CXTButton::GetXButtonStyle() {
	ASSERT(::IsWindow(m_hWnd)); return m_dwxStyle;
}
AFX_INLINE void CXTButton::SetStateX(BOOL bChecked) {
	CButton::SetState(bChecked); m_bChecked = bChecked;
}
AFX_INLINE int CXTButton::GetBorderGap() const {
	return m_nBorderGap;
}
AFX_INLINE int CXTButton::GetImageGap() const {
	return m_nImageGap;
}
AFX_INLINE BOOL CXTButton::GetHilite() const {
	return m_bHilite;
}
AFX_INLINE BOOL CXTButton::GetUserPosition() const {
	return m_bUserPosition;
}
AFX_INLINE BOOL CXTButton::GetLBtnDown() const {
	return m_bLBtnDown;
}
AFX_INLINE BOOL CXTButton::GetPainted() const {
	return m_bPainted;
}
AFX_INLINE BOOL CXTButton::GetChecked() const {
	return m_bChecked;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTBUTTON_H__)
//:End Ignore
