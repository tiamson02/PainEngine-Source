// XTCaption.h : interface for the CXTCaption class.
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
#if !defined(__XTCAPTION_H__)
#define __XTCAPTION_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTCaption;
class CXTCaptionPopupWnd;

//////////////////////////////////////////////////////////////////////
// Summary: CXTCaptionButton is a CXTButton derived class. It is used by
//			the CXTCaption class to activate a CXTCaptionPopupWnd window.
class _XT_EXT_CLASS CXTCaptionButton : public CXTButton
{
public:
	// Input:	dwxStyle - Specifies the button style. It can be one or more of the following:
	//			[ul]
	//			[li]<b>BS_XT_FLAT</b> Draws a flat button.[/li]
	//			[li]<b>BS_XT_SEMIFLAT</b> Draws a semi-flat button.[/li]
	//			[li]<b>BS_XT_TWOROWS</b> Draws images and text that are centered.[/li]
	//			[li]<b>BS_XT_SHOWFOCUS</b> Draws a focus rect when the button
	//			has input focus.[/li]
	//			[li]<b>BS_XT_HILITEPRESSED</b> Highlights the button when pressed.[/li]
	//			[li]<b>BS_XT_XPFLAT</b> Draws a flat button ala Office XP.[/li]
	//			[/ul]
	//			bRedraw - Specifies whether the button is to be redrawn. A nonzero value redraws
	//			the button. A zero value does not redraw the button. The button is redrawn
	//			by default.
	// Returns: The previous style that was set.
	// Summary: This member function will set the display style for the button.
	virtual DWORD SetXButtonStyle(DWORD dwxStyle,BOOL bRedraw=TRUE);
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTCaption is a CStatic derived class. It is used to create caption
//			or info bars similar to those in Microsoft&reg Outlook&trade;.  
class _XT_EXT_CLASS CXTCaption : public CStatic
{
	DECLARE_DYNAMIC(CXTCaption)

public:
	
    // Summary: Constructs a CXTCaption object
    CXTCaption();

	// Summary: Destroys a CXTCaption object, handles cleanup and de-allocation
    virtual ~CXTCaption();

protected:

    int                 m_nOffset;      // Size in pixels that the child window should leave for its caption area.
    int                 m_nBorder;      // Size in pixels for the caption border.
    bool                m_bUserColors;  // true if the user has specified caption colors other than the default.
    CWnd*               m_pChildWnd;    // A CWnd pointer that represents the child window displayed in the popup window.
    CWnd*               m_pParentView;  // A CWnd pointer that represents the child window's parent view.
    CWnd*               m_pSplitterWnd; // A CWnd pointer that represents the splitter window. It is used to track size changes.
    CSize               m_sizeIcon;     // Width and height of the caption icon area.
    CRect               m_rcChild;      // Size of the child window displayed in the popup.
    CRect               m_rcParent;     // Size of the child's parent view.
    CRect               m_rcSplitter;   // Size of the splitter window.
    DWORD               m_dwExStyle;    // Border style bits, either CPWS_EX_GROOVE_EDGE | CPWS_EX_RAISED_EDGE.
    DWORD               m_dwTextStyle;  // Text style, one of DT_ styles.  See CDC::DrawText(...) for more details.
    HICON               m_hIcon;        // User defined icon handle. The default value is NULL.
    CString             m_strCaption;   // Text that will be displayed in the caption.
    COLORREF            m_clrBorder;    // An RGB value that represents the user defined border color.
    COLORREF            m_clrFace;      // An RGB value that represents the user defined background color.
    COLORREF            m_clrText;      // An RGB value that represents the user defined font color.
    CImageList          m_ilButton;     // Image list used to create the close button icon.
    CXTCaptionButton    m_btnCaption;   // Button used to 'tack' the popup window back in place.
    CXTCaptionPopupWnd* m_pPopupWnd;    // Points to the popup window.
	bool                m_bOffice2003Colors;

public:

	// Returns: A reference to a CXTCaptionButton object.
	// Summary: Call this member function to get a reference to the caption's close
	//			/ popup button. 
	CXTCaptionButton& GetCaptionButton();

	// Input:	clrBorder - An RGB value that represents the new border color.
	//			clrFace - An RGB value that represents the new background color.
	//			clrText - An RGB value that represents the new font color.
    // Summary: This member function will set the caption bar border, background,
	//			and font colors.
    virtual void SetCaptionColors(COLORREF clrBorder, COLORREF clrFace, COLORREF clrText);

	void SetOffice2003Colors();

	// Input:	nBorderSize - Specifies size in pixels of the banner border.
	//			pFont - Specifies the new caption font.
	//			lpszWindText - NULL terminated string specifying the new caption text.
	//			hIcon - Handle of the icon to be drawn in the caption.
    // Summary: This function will modify the caption style.  You can use this 
    //			member function to set the border size that is drawn around the 
    //			caption banner, the font that the caption will use, and the caption text 
    //			and icon to be displayed.
    virtual void ModifyCaptionStyle(int nBorderSize, CFont* pFont=NULL, LPCTSTR lpszWindText=NULL, HICON hIcon=NULL);

    // Input:	pChild - A CWnd pointer that represents the child window to be displayed in
	//			the popup window.
	//			pNotifyWnd - A CWnd pointer that represents the window to receive notification
	//			messages.
	// Summary: Call this member function to set the child and notification windows
	//			for the caption bar.
    virtual void SetChildWindow(CWnd* pChild, CWnd* pNotifyWnd);

    // Summary: This member function is called to destroy the popup window and associated
	//			children.
    virtual void KillChildWindow();

	// Input:	lpszWindowText - NULL terminated string to display in the caption bar.
	//			hIcon - Handle of the icon to display in the caption bar. It can be NULL.
    // Summary: This member function is called to update the text and icon for the
	//			caption bar.
    virtual void UpdateCaption(LPCTSTR lpszWindowText, HICON hIcon);

	// Input:	dwStyle - Specifies the method of formatting the text.  See CDC::DrawText
	//			for	a detailed listing of available styles.
    // Summary: This member function is called to set the formatting style of the
	//			text that is displayed in the caption bar.
    virtual void SetTextStyle(DWORD dwStyle); 

	// Returns: A DWORD value that represents the formatting style of the current caption text.
	// Summary: This member function is called to retrieve the formatting style of
	//			the text that is displayed in the caption bar.  
	virtual DWORD GetTextStyle();

	// Input:	pParentWnd - Specifies the parent window.
	//			lpszWindowName - Points to a null-terminated character string that contains
	//			the window name. This will be displayed in the caption area.
	//			dwExStyle - Specifies caption bar style.  Can be either CPWS_EX_RAISED_EDGE
	//			which will draw a 3D edge around the caption bar, or CPWS_EX_GROOVE_EDGE
	//			which will draw a sunken edge.
	//			dwStyle - Specifies the control window style.  Apply any combination of 
	//			caption bar styles to the control.
	//			rect - Specifies the position and size of the caption bar.  It can be 
	//			either a RECT structure or a CRect object. 
	//			nID - Specifies the caption bar control ID.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: This method creates a caption bar window and attaches it to the
	//			CXTCaption object. You construct a caption bar in two steps.  First,
	//			call the constructor, which constructs the CXTCaption object.  Then
	//			call Create, which creates the Window's child window and attaches it
	//			to CXTCaption.  Create initializes the window class name and window
	//			name and registers values for its style, parent, and ID.  
    virtual BOOL Create(CWnd* pParentWnd, LPCTSTR lpszWindowName, DWORD dwExStyle=CPWS_EX_RAISED_EDGE, DWORD dwStyle=WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE, const CRect& rect=CRect(0,0,0,0), UINT nID = 0xffff);

	// Summary: This member function is called to refresh the caption button style.
	void RefreshButton();

    //:Ignore
	//{{AFX_VIRTUAL(CXTCaption)
	//}}AFX_VIRTUAL
	//:End Ignore

protected:

	// Input:	pDC - A CDC pointer that represents the current device context.
	//			rcItem - A CRect reference that represents the size of the area to paint.
    // Summary: This member function is called to draw the caption background.
    virtual void DrawCaptionBack(CDC* pDC, CRect& rcItem);

	// Input:	pDC - A CDC pointer that represents the current device context.
	//			rcItem - A CRect reference that represents the size of the area to paint.
    // Summary: This member function is called to paint the caption text.
    virtual void DrawCaptionText(CDC* pDC, CRect& rcItem);

	// Input:	pDC - A CDC pointer that represents the current device context.
	//			rcItem - A CRect reference that represents the size of the area to paint.
    // Summary: This member function is called to paint the caption icon.
    virtual void DrawCaptionIcon(CDC* pDC, CRect& rcItem);

	// Input:	pDC - A CDC pointer that represents the current device context.
	//			rcItem - A CRect reference that represents the size of the area to paint.
    // Summary: This member function is called to update the caption text.
    virtual void UpdateCaptionText(CDC* pDC, CRect& rcItem);

	// Summary: This member function is called to determine the size and location of the
	//          button associated with the caption.
	// Returns: The size and location of the caption button or an empty rect if no button
	//          is defined.
	virtual CRect GetButtonRect() const;

    //:Ignore
	//{{AFX_MSG(CXTCaption)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnSysColorChange();
	//}}AFX_MSG	
	afx_msg void OnCaptButton();
	afx_msg void OnPushPinButton();
	afx_msg void OnPushPinCancel();
	//:End Ignore

	DECLARE_MESSAGE_MAP()

	friend class CXTCaptionButton;
	friend class CXTCaptionButtonTheme;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTCaption::SetTextStyle(DWORD dwStyle) {
	m_dwTextStyle = dwStyle;
}
AFX_INLINE DWORD CXTCaption::GetTextStyle() {
	return m_dwTextStyle;
}
AFX_INLINE CXTCaptionButton& CXTCaption::GetCaptionButton() {
	ASSERT(::IsWindow(m_btnCaption.m_hWnd)); return m_btnCaption;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTCAPTION_H__)
//:End Ignore