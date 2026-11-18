// XTColorDialog.h : header file
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
#if !defined(__XTCOLORSHEET_H__)
#define __XTCOLORSHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTColorDialog is a multiple inheritance class derived from CXTPDialogState
//			and CPropertySheet. It is an Office&trade; style color dialog and can be used
//			as a drop in replacement for the MFC CColorDialog API.
class _XT_EXT_CLASS CXTColorDialog : CXTPDialogState, public CPropertySheet
{
	DECLARE_DYNAMIC(CXTColorDialog)

public:

	// BULLETED LIST:

	// Input:	clrNew - An RGB value that represents the new color selection.
	//			clrCurrent - An RGB value that represents the default color selection.
	//			dwFlags - Style for color dialog. It can be one of the following:
	//			[ul]
	//			[li]<b>CPS_XT_SHOW3DSELECTION</b> Displays the color selection
	//			box with a 3D raised border.[/li]
	//			[li]<b>CPS_XT_SHOWHEXVALUE</b> Displays the hex equivalent of
	//			the selected color.[/li]
	//			[/ul]
	//			pWndParent - A pointer to the dialog box’s parent or owner window.
    // Summary:	Constructs a CXTColorDialog object
	CXTColorDialog(COLORREF clrNew,COLORREF clrCurrent,DWORD dwFlags = 0L,CWnd* pWndParent = NULL);

	// Summary: Destroys a CXTColorDialog object, handles cleanup and de-allocation
    virtual ~CXTColorDialog();

protected:

    DWORD    m_dwStyle;    // Styles for the dialog.
    CEdit    m_editHex;    // Edit window to display the color hex value.
    CRect    m_rcNew;      // Size of the rectangle that represents the new color.
    CRect    m_rcCurrent;  // Size of the rectangle that represents the current color.
    COLORREF m_clrNew;     // A COLORREF value that contains the RGB information for the new color.
    COLORREF m_clrCurrent; // A COLORREF value that contains the RGB information for the current color.

public:

	// Input:	clr - An RGB value that represents the color.
	//			bNotify - TRUE to notify tab pages of a color change.
	// Summary: Call this member function to set the current color selection to the
	//			color value specified in 'clr'.  The dialog box will automatically update
	//			the user’s selection based on the value of the 'clr' parameter.
	void SetNewColor(COLORREF clr, BOOL bNotify=TRUE);

	// Input:	clr - An RGB value that represents the color.
	// Summary: Call this function to set the new color selection to the color 
	//			value specified in 'clr'.
	void SetCurrentColor(COLORREF clr);

	// Returns: A COLORREF value that contains the RGB information for
	//			the current color specified when the dialog was instantiated.
	// Summary: Call this member function to retrieve the information about the current
	//			color. 
	COLORREF GetCurrentColor();
	
	// Returns: A COLORREF value that contains the RGB information
	//			for the new color selected in the color dialog box.
    // Summary: Call this member function to retrieve the information about the color
	//			that the user selected. 
	COLORREF GetColor();

	// Input:	strText - A NULL terminated string.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: Call this member function to copy the string specified by 'strText' to the
	//			Windows clipboard. 
	BOOL CopyToClipboard(CString strText);

	// Input:	clr - An RGB value that represents the color.
	// Returns: A CString object.
    // Summary: This member function returns a CString object that represents the
	//			HEX conversion for the specified RGB color. 
	static CString RGBtoHex(COLORREF clr);

    //:Ignore
	//{{AFX_VIRTUAL(CXTColorDialog)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
    virtual void CalculateRects();
	//:End Ignore

protected:

    //:Ignore
	//{{AFX_MSG(CXTColorDialog)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTColorDialog::GetCurrentColor() {
	return m_clrCurrent;
}
AFX_INLINE COLORREF CXTColorDialog::GetColor() {
	return m_clrNew;
}

//////////////////////////////////////////////////////////////////////
// Summary: CXTThemePropertyPage is a multiple inheritance class derived from 
//			CXTPDialogState and CPropertyPage.  It is used as a base class
//          for drawing Windows XP themed property pages.
class _XT_EXT_CLASS CXTThemePropertyPage : CXTPDialogState, public CPropertyPage
{
	DECLARE_DYNCREATE(CXTThemePropertyPage)

public:
	// Input:	pParentSheet - Points to the parent property sheet.
    //          nIDTemplate - Dialog resource id.
    //          nIDCaption - Dialog caption string id.
    // Summary: Constructs a CXTThemePropertyPage object
	CXTThemePropertyPage(CXTColorDialog* pParentSheet = 0, UINT nIDTemplate = 0, UINT nIDCaption = 0);

	// Summary: Destroys a CXTThemePropertyPage object, handles cleanup and de-allocation
    virtual ~CXTThemePropertyPage();

protected:
	CPtrArray           m_arTransText;  // Array of transparent text items.
	CXTColorDialog*     m_pParentSheet; // Points to the parent property sheet
    CXTPWinThemeWrapper m_themeWrapper; // Windows XP theme wrapper.

	// Input:   nIDResource - Resource ID of the text item to draw.
	// Summary: Call this member function to draw the specified text transparent.
    virtual void MakeTextTransparent(UINT nIDResource);

	//:Ignore
	//{{AFX_VIRTUAL(CXTColorPageStandard)
	protected:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
    virtual void OnDrawPage(CDC* pDC);
    virtual void DrawThemedBack(CDC* pDC);
    virtual void DrawTransparentText(CDC* pDC);
	//:End Ignore

    //:Ignore
	//{{AFX_MSG(CXTColorPageStandard)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

#define WM_XT_UPDATECOLOR	(WM_USER+1024)

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTCOLORSHEET_H__)
//:End Ignore