// XTEditListBox.h interface for the CXTEditGroup class.
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
#if !defined(__XTEDITLISTBOX_H__)
#define __XTEDITLISTBOX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTListBox is a CListBox derived class. CXTListBox extends the standard
//			list box control to enable flicker free drawing.
class _XT_EXT_CLASS CXTListBox : public CListBox
{
    DECLARE_DYNAMIC(CXTListBox)

public:
  
    // Summary: Constructs a CXTListBox object
    CXTListBox();

    // Summary: Destroys a CXTListBox object, handles cleanup and de-allocation
    virtual ~CXTListBox();

public:
	
	// Input:   bAutoFont - True to enable automatic font initialization.
	// Summary:	Call this member function to initialize the list box.  This method
	//          should be called directly after creating or sub-classing the control.
	virtual void Initialize(bool bAutoFont=true);

    //:Ignore
	//{{AFX_VIRTUAL(CXTListBox)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    protected:
    //}}AFX_VIRTUAL
	//:End Ignore

	// Input:   bEnable - Specifies the state of the redraw flag. If this parameter is true, the
	//                    redraw flag is set; if false, the flag is cleared.
	// Summary: This is a helper function is used by the list box to suppend drawing while the list
	//          is performing a scroll event.
	virtual void SetRedrawEx(bool bEnable);
	
protected:

    //:Ignore
	//{{AFX_MSG(CXTListBox)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	//:End Ignore	

    DECLARE_MESSAGE_MAP()
};

// Summary: List used for enumerating XT_LOGFONT structures
typedef CList<XT_LOGFONT, XT_LOGFONT&> CXTFontList;


//////////////////////////////////////////////////////////////////////
// Summary: CXTFontEnum is a stand alone singleton font enumeration class. It is
//			used to enumerate a list of fonts found installed for the operating
//			system. CXTFontEnum is a singleton class, which means it can only be
//			instantiated a single time. The constructor is private, so the only
//			way to access members of this class is to use the objects Get() method.
//			To retrieve a list of available fonts for your operating system, you
//			would make the following call:
//
// Example:	<pre>CXTFontEnum::Get().GetFontList()</pre>
class _XT_EXT_CLASS CXTFontEnum  
{
    // Summary: Constructs a CXTFontEnum object. CXTFontEnum is a singleton
	//			class, to instantiate an object, use the static method Get().
	CXTFontEnum();

public:

    // Summary: Destroys a CXTFontEnum object, handles cleanup and de-allocation
	virtual ~CXTFontEnum();

protected:

	CXTFontList m_listFonts; // List of fonts found during enumeration

public:

	// Example:	<pre>CXTFontEnum::Get().GetFontList()</pre>
	// Returns: A reference to a CXTFontEnum object.
	// Summary:	This static member function will return a reference to the one
	//			and only CXTFontEnum object. You can use this function to access
	//			data members for the CXTFontEnum class.
	static CXTFontEnum& Get();

	// Input:	strFaceName - Reference to a NULL terminated string that represents the font name.
	// Returns: true if the font exists, otherwise returns false.
	// Summary:	This member function is used to determine the existence of the font
	//			specified by 'strFaceName'.  
	bool DoesFontExist(CString& strFaceName);
	
	// Input:	strFaceName - A NULL terminated string that represents the font name.
	// Returns: A pointer to the XT_LOGFONT structure for the specified item, or NULL if no font 
	//			was found.
	// Summary:	This member function is used to get a pointer to the font specified
	//			by 'strFaceName'. 
	XT_LOGFONT* GetLogFont(CString strFaceName);

	// Returns: An integer value that represents the width for the longest font in the list.
	// Summary:	This member function is used by the callback function to retrieve the
	//			current width for the longest font name in the list. 
	int GetMaxWidth();

	// Returns: A reference to the CXTFontList used by this class.
	// Summary:	This member function is used to get a reference to the font list. 
	CXTFontList& GetFontList();

	// Input:	pDC - Points to a valid device context, if NULL, the screen device context
	//			is used.
	//			nCharSet - Represents the character set to enumerate.
	// Summary:	This member function is called by the CXTFontEnum class to initialize
	//			the font list. You can also call this member function to reinitialize
	//			the font enumeration. For example, if you changed printers and you want
	//			to enumerate printer fonts, or you wanted to use a different character
	//			set.
	void Init(CDC* pDC=NULL,BYTE nCharSet=DEFAULT_CHARSET);

protected:

	// BULLETED LIST:

	// Input:	pelf - Pointer to an ENUMLOGFONTEX structure that contains information 
	//			about the logical attributes of the font.
	//			lpntm - Pointer to a structure that contains information about the physical 
	//			attributes of a font. The function uses the NEWTEXTMETRICEX structure 
	//			for TrueType fonts; and the TEXTMETRIC structure for other fonts.
	//			dwFontType - Specifies the type of the font. This parameter can be a combination
	//			of these values:
	//			[ul]
	//			[li]<b>DEVICE_FONTTYPE</b> The font is a device font.
	//			[li]<b>RASTER_FONTTYPE</b> The font is a raster font.
	//			[li]<b>TRUETYPE_FONTTYPE</b> The font is a TrueType font.
	//			[/ul]
	//			lParam - Specifies the application-defined data passed by the EnumFontFamiliesEx
	//			function.
	// Returns:	The return value must be a nonzero value to continue enumeration.  
	//			To stop enumeration, the return value must be zero.
    // Summary:	The EnumFontFamExProc function is an application defined–callback
	//			function used with the EnumFontFamiliesEx function. It is used to process
	//			the fonts and is called once for each enumerated font. The FONTENUMPROC
	//			type defines a pointer to this callback function. EnumFontFamExProc
	//			is a placeholder for the application defined–function name. 
    static BOOL CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf,NEWTEXTMETRICEX* lpntm,DWORD dwFontType,LPARAM lParam);

	// BULLETED LIST:

	// Input:	pLF - Points to a valid LOGFONT structure.
	//			dwType - Specifies the type of the font. This parameter can be a combination
	//			of these values:
	//			[ul]
	//			[li]<b>DEVICE_FONTTYPE</b> The font is a device font.[/li] 
	//			[li]<b>RASTER_FONTTYPE</b> The font is a raster font.[/li]
	//			[li]<b>TRUETYPE_FONTTYPE</b> The font is a TrueType font.[/li]
	//			[/ul]
	// Returns: true if successful, otherwise returns false.
	// Summary:	This member function is called by the font enumeration callback to
	//			add a font to the font list. 
	bool AddFont(const LOGFONT* pLF,DWORD dwType);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CXTFontList& CXTFontEnum::GetFontList() {
	return m_listFonts;
}

//////////////////////////////////////////////////////////////////////

// Summary:	Enumerated style that is used to define how the font list box will display
//			the font listings.
enum eSTYLE
{
	XT_FLB_NAME_GUI = 0,	// Display font name with GUI font style. 
	XT_FLB_NAME_SAMPLE,		// Display font name with its own font style.
	XT_FLB_BOTH,			// Display font name with GUI font style, then a sample display to the right.
};

// forwards

class CXTFontListBoxWndHook;

//////////////////////////////////////////////////////////////////////
// Summary: CXTFontListBox is a CXTListBox derived class. It is used to create
//			a font selection list box. You can choose to display the font name
//			with the GUI font style, display the font name with its own font style,
//			or display the font name with the default GUI font style and a sample
//			display to the right.
class _XT_EXT_CLASS CXTFontListBox : public CXTListBox
{
public:

    // Summary: Constructs a CXTFontListBox object
	CXTFontListBox();

    // Summary: Destroys a CXTFontListBox object, handles cleanup and de-allocation
	virtual ~CXTFontListBox();

protected:
	
    eSTYLE      m_eStyle;     // Enumerated style indicating how to display the font list.    
    CString     m_strSymbol;  // String displayed for the symbol characters.
    CImageList  m_ilFontType; // True type font image list.

public:

	// Input:	lf - Reference to an XT_LOGFONT structure.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to get the logfont for the currently selected
	//			item. 
    virtual bool GetSelFont(XT_LOGFONT& lf);

	// Input:	strFaceName - A reference to a valid CString object to receive the logfont face
	//			name.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to get the logfont for the currently selected
	//			item. 
	virtual bool GetSelFont(CString& strFaceName);

	// Input:	lf - Reference to an XT_LOGFONT structure.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to select the logfont for the list box.
    virtual bool SetSelFont(XT_LOGFONT& lf);

	// Input:	strFaceName - A NULL terminated string that represents the logfont face name.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to select the logfont for the list box.
	virtual bool SetSelFont(CString strFaceName);

	// Input:	eStyle - Specifies the style for the font list box. Styles can be any one of
	//			the following combinations:
	//			[ul]
	//			[li]<b>XT_FLB_NAME_GUI</b> Display font name in GUI font style.[/li]
	//			[li]<b>XT_FLB_NAME_SAMPLE</b> Display font name with its own font
	//			style.[/li]
	//			[li]<b>XT_FLB_BOTH</b> Display font name in GUI font style, then
	//			a sample display to the right.[/li]
	//			[/ul]
	// Summary:	Call this member function to set the font display style for the font
	//			list box. There are three styles to choose from that include displaying
	//			the font in the default GUI font, displaying the font in its own font
	//			style, or displaying both the font name in the default GUI font and
	//			a sample to the right.
	void SetListStyle(eSTYLE eStyle);

	// Input:   bAutoFont - True to enable automatic font initialization.
	// Summary:	Call this member function to initialize the font list box and populate it
	//			with a list of avaliable fonts.  This method should be called directly 
	//          after creating or sub-classing the control.
	virtual void Initialize(bool bAutoFont=true);

	//:Ignore
	//{{AFX_VIRTUAL(CXTFontListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL
	//:End Ignore

protected:

	//:Ignore
	//{{AFX_MSG(CXTFontListBox)
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFontListBox::SetListStyle(eSTYLE eStyle) {
    m_eStyle = eStyle;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Summary: CXTEditGroup is a CStatic derived class. It used by the CXTEditListBox
//			class to create a toolbar above the edit list box to display icons for
//			editing. It can be used for other classes by setting the notify window
//			in Initialize. This window will receive notification messages whenever
//			the new, delete, up, and down buttons are pressed. You can handle these
//			messages by adding an ON_BN_CLICKED handler for each of the buttons
//			XT_IDC_BTN_NEW, XT_IDC_BTN_DELETE, XT_IDC_BTN_UP and XT_IDC_BTN_DOWN.
class _XT_EXT_CLASS CXTEditGroup : public CStatic
{
    DECLARE_DYNAMIC(CXTEditGroup)

public:

    // Summary: Constructs a CXTEditGroup object
    CXTEditGroup();

    // Summary: Destroys a CXTEditGroup object, handles cleanup and de-allocation
    virtual ~CXTEditGroup();

protected:

	bool          m_bEnableEdit;   // True if editing is enabled.
	CRect         m_arClipRect[4]; // Array of toolbar button sizees.
	CXTButton     m_arButton[4];   // Array of toolbar buttons.
	CXTIconHandle m_arIcon[4];     // Array of toolbar button icons.
	CToolTipCtrl  m_tooltip;       // Tooltip control for edit buttons.

public:
    
	// Input:   bAutoFont - True to enable automatic font initialization.
	// Summary:	Call this member function to initialize the edit group control.  This method
	//          should be called directly after creating or sub-classing the control.
	virtual void Initialize(bool bAutoFont=true);

	// Summary: This member function returns a reference to the new button of the edit group.
	// Returns: A reference to a CXTButton object.
	CXTButton& GetNewButton();

	// Summary: This member function returns a reference to the delete button of the edit group.
	// Returns: A reference to a CXTButton object.
	CXTButton& GetDeleteButton();

	// Summary: This member function returns a reference to the up button of the edit group.
	// Returns: A reference to a CXTButton object.
	CXTButton& GetUpButton();

	// Summary: This member function returns a reference to the down button of the edit group.
	// Returns: A reference to a CXTButton object.
	CXTButton& GetDownButton();

	// Input:   bEnable - True to enable editing.
	// Summary: This member function will enable or disable editing.
	void EnableEdit(bool bEnable);

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
	// Summary: This member function will set the display style for the toolbar buttons.
	virtual void SetXButtonStyle(DWORD dwxStyle,BOOL bRedraw=TRUE);

    //:Ignore
	//{{AFX_VIRTUAL(CXTEditGroup)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL
	virtual void DrawText(CDC* pDC, CRect& rcClient);
	virtual void MoveButtons();
	virtual void SendCommand(UINT nCmdID);
	//:End Ignore

    //:Ignore
	//{{AFX_MSG(CXTEditGroup)
	afx_msg void OnButtonNew();
    afx_msg void OnButtonDelete();
    afx_msg void OnButtonUp();
    afx_msg void OnButtonDown();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	//:End Ignore
    
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CXTButton& CXTEditGroup::GetNewButton() {
	return m_arButton[0];
}
AFX_INLINE CXTButton& CXTEditGroup::GetDeleteButton() {
	return m_arButton[1];
}
AFX_INLINE CXTButton& CXTEditGroup::GetUpButton() {
	return m_arButton[2];
}
AFX_INLINE CXTButton& CXTEditGroup::GetDownButton() {
	return m_arButton[3];
}
AFX_INLINE void CXTEditGroup::EnableEdit(bool bEnable) {
	m_bEnableEdit = bEnable;
}

// forwards

class CXTItemEdit;

//////////////////////////////////////////////////////////////////////
// Summary: CXTEditListBox is a CXTListBox derived class. It is used to create an
//			editable list box. This list box can be configured to display a toolbar
//			for editing. You can define browse styles to search for files or folders.
//			Each entry is made editable with a double mouse click.
class _XT_EXT_CLASS CXTEditListBox : public CXTListBox
{
    DECLARE_DYNAMIC(CXTEditListBox)

public:

    // Summary: Constructs a CXTEditListBox object
    CXTEditListBox();

    // Summary: Destroys a CXTEditListBox object, handles cleanup and de-allocation
    virtual ~CXTEditListBox();

protected:

	int				m_nIndex;		// Current index when edit functions are performed.
	BOOL			m_bNewItem;		// TRUE if a new item is being entered into the list box.
	bool            m_bEnableEdit;  // True if editing is enabled.
	DWORD			m_dwLStyle;		// List edit styles.
	CString			m_strTitle;		// Caption area title.
	CString			m_strFilter;	// Default file filter.
	CString			m_strItemText;	// Current text of a selected item during edit.
	CXTItemEdit*	m_pItemEdit;	// Points to the in-place edit item.
	CXTEditGroup	m_editGroup;	// The edit group (toolbar) that appears above the list box.
									
public:

	// Input:	lpszTitle - NULL terminated string that represents the caption title.
	//			dwLStyle - Style for the list edit control. Pass in LBS_XT_NOTOOLBAR
    //			if you do not wish the caption edit navigation control
    //			bar to be displayed.
    // Summary:	This member function will create the edit group control.
    void SetListEditStyle(LPCTSTR lpszTitle, DWORD dwLStyle=LBS_XT_DEFAULT);

	// Input:	nTitle -  Resource ID of the string to load for the caption title.
	//			dwLStyle - Style for the list edit control. Pass in LBS_XT_NOTOOLBAR
    //			if you do not wish the caption edit navigation control
    //			bar to be displayed.
    // Summary:	This member function will create the edit group control.
    void SetListEditStyle(UINT nTitle, DWORD dwLStyle=LBS_XT_DEFAULT);

	// Returns: An integer value that represents the edit control index.
	// Summary:	Call this member function to get the current index for the edit control.
	//			Similar to GetCurSel; however, the current index is the index of the
	//			last item to be modified or added to the edit list box and not necessarily
	//			the selected item. 
	int GetCurrentIndex();

	// Input:	iItem - Index of the item to edit.
	// Summary:	This member function will enable editing for the list box item.
	void EditItem(int iItem);

	// Returns: A reference to a CXTEditGroup object.
	// Summary:	Call this member function to return a reference to the CXTEditGroup
	//			control that is associated with the edit list box. 
	CXTEditGroup& GetEditGroup();

	// Input:	strFilter - Points to a NULL terminated string that represents the file filter
	//			used by the file open dialog.
	// Summary:	Call this member function to set the default filter for the file dialog.
	virtual void SetDlgFilter(LPCTSTR strFilter=NULL);

	// Returns: true if the toolbar is turned on, otherwise returns false.
	// Summary:	Call this member function to determine if the edit list has a toolbar.
	bool HasToolbar();

	// Input:   bEnable - True to enable editing.
	// Summary: This member function will enable or disable editing.
	void EnableEdit(bool bEnable);

	// Input:   bAutoFont - True to enable automatic font initialization.
	// Summary:	Call this member function to initialize the list box.  This method
	//          should be called directly after creating or sub-classing the control.
	virtual void Initialize(bool bAutoFont=true);

	// Summary: Call this member function to correctly reposition the edit list box
	//          toolbar.  This will readjust the layout to correctly and position the
	//          toolbar in relation to the list.
	virtual void RecalcLayout();

protected:
	
	// Input:   bAutoFont - True to enable automatic font initialization.
    // Summary:	This member function will create the edit group control.
	// Returns: TRUE if successful, otherwise returns FALSE.
    virtual BOOL CreateEditGroup(bool bAutoFont=true);

	// Input:	bNewItem - TRUE to add a new item.
    // Summary:	This member function will enable editing for the currently selected
	//			list box item. If 'bNewItem' is TRUE, a new item is added to the end
	//			of the list box.
    void EditListItem(BOOL bNewItem);

	CWnd* m_pParentWnd;

    //:Ignore
	//{{AFX_VIRTUAL(CXTEditListBox)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL
	//:End Ignore

    //:Ignore
	//{{AFX_MSG(CXTEditListBox)
    afx_msg void OnEndLabelEdit();
    afx_msg void OnNewItem();
    afx_msg void OnDeleteItem();
    afx_msg void OnMoveItemUp();
    afx_msg void OnMoveItemDown();
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	//:End Ignore
	    
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTEditListBox::GetCurrentIndex() {
	return m_nIndex;
}
AFX_INLINE CXTEditGroup& CXTEditListBox::GetEditGroup() {
	return m_editGroup;
}
AFX_INLINE void CXTEditListBox::SetDlgFilter(LPCTSTR strFilter/*=NULL*/) {
	m_strFilter = strFilter;
}
AFX_INLINE bool CXTEditListBox::HasToolbar() {
	return ((m_dwLStyle & LBS_XT_NOTOOLBAR) == 0);
}
AFX_INLINE void CXTEditListBox::EnableEdit(bool bEnable) {
	m_bEnableEdit = bEnable; m_editGroup.EnableEdit(bEnable);
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTEDITLISTBOX_H__)
//:End Ignore
