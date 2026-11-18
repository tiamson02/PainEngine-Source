// XTMenuListBox.h interface for the CXTContentItems class.
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
#if !defined(__XTMENULISTBOX_H__)
#define __XTMENULISTBOX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

//===========================================================================
// Remarks: XT_CONTENT_ITEM is a stand alone helper structure class. It is used
//			by CXTMenuListBox.
//===========================================================================
struct XT_CONTENT_ITEM
{
	int				m_nIndex;	// Zero-based index for the menu item.
	CString			m_strText;	// Text label for the menu item.
	CXTIconHandle	m_hIcon;	// Icon handle for the menu item.
};

//===========================================================================
// Summary: CList definition for XT_CONTENT_ITEM structure list.
//===========================================================================
typedef	CList<XT_CONTENT_ITEM*,XT_CONTENT_ITEM*> CXTContentItemList;

//===========================================================================
// Remarks: CCXTMenuListBox is a CListBox derived class. It is used to implement
//			an Outlook bar style control. It can only be used with the LBS_OWNERDRAWVARIABLE
//			style bit set. This is a simpler version of CXTOutBarCtrl and does not
//			allow for shortcut folders.
//===========================================================================
class _XT_EXT_CLASS CXTMenuListBox : public CListBox
{
    DECLARE_DYNAMIC(CXTMenuListBox)

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTMenuListBox object
	//-----------------------------------------------------------------------
    CXTMenuListBox();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTMenuListBox object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTMenuListBox();

protected:

	int                 m_cyEdge;           // Height in pixels of the space between the icon an text.
    int                 m_cxIcon;           // Width of the menu icon.
    int                 m_cyIcon;           // Height of the menu icon.
    int                 m_nIndex;           // Currently selected menu index.
	int					m_bHilightItemID;   // Index of the currently highlighted item, set to -1 if no selection.
    bool                m_bHilight;         // true when the menu item is selected.
    bool                m_bUserColors;      // true when the user has defined custom colors for the Outlook bar.
	bool                m_bNoBorder;        // true to disable a border drawn around highlighted items.
	bool                m_bNoUnderline;     // true if the text is not drawn with a underline when no highlight border is displayed.
    bool                m_bTimerActive;     // true if the redraw timer is active.
    CPoint              m_point;            // Holds the cursor position.
    COLORREF            m_clrBack;          // RGB value representing the background color.
    COLORREF            m_clrText;          // RGB value representing the text color.
    CXTContentItemList  m_arContentItems;   // Array of XT_CONTENT_ITEM structs that represent each item in the Outlook bar.
	HCURSOR             m_hCursor;          // Cursor display when item is hovered.

public:

	//-----------------------------------------------------------------------
	// Input:	dwStyle    - Window style.
	//			rect	   - The size and position of the window, in client coordinates of 
	//						 'pParentWnd'.
	//			pParentWnd - The parent window.
	//			nID		   - The ID of the child window.
	// Summary:	This member function creates an Outlook Bar control. 
	// Returns: Nonzero if successful, otherwise returns zero.
	//-----------------------------------------------------------------------
	virtual BOOL Create(DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID);

	//-----------------------------------------------------------------------
	// Input:	clrText - RGB value representing the text color.
	//			clrBack - RGB value representing the background color.
    // Summary:	Call this member function to set the text and background colors for
    //			the Outlook bar.
	//-----------------------------------------------------------------------
    void SetColors(COLORREF clrText,COLORREF clrBack);

	//-----------------------------------------------------------------------
	// Summary:	This member function gets an RGB value that represents the control's
	//			background color. 
	// Returns: An RGB value that represents the background color.
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary:	This member function gets an RGB value that represents the control's
	//			text color. 
	// Returns: An RGB value that represents the text color.
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Input:	iIndex - Specifies the zero-based index of the position to insert the
	//			menu item. If this parameter is -1, the menu item is added to the
	//			end of the list.
	//			nIconID - Resource ID of the icon associated with this menu item.
	//			lpszText - Points to the null-terminated string for the menu item
	// Summary:	This member function inserts a menu item into the Outlook bar. Unlike
	//			the AddMenuItem member function, InsertMenuItem does not cause an Outlook
	//			bar with the LBS_SORT style to be sorted. 
	// Returns: The zero-based index of the position at which the menu item was inserted. 
	//			The return value is LB_ERR if an error occurs. The return value is LB_ERRSPACE
	//			if insufficient space is available to store the new menu item.
	//-----------------------------------------------------------------------

	int InsertMenuItem(int iIndex,UINT nIconID,LPCTSTR lpszText);

	//-----------------------------------------------------------------------
	// Input:	nIconID - Resource ID of the icon associated with this menu item.
	//			lpszText - Points to the null-terminated string for the menu item.
	// Summary:	Call this member function to add a menu item to an Outlook bar. If
	//			the Outlook bar was not created with the LBS_SORT style, the menu item
	//			is added to the end of the Outlook bar. Otherwise, the menu item is
	//			inserted into the Outlook bar, and the Outlook bar is sorted. If the
	//			Outlook bar was created with the LBS_SORT style but not the LBS_HASSTRINGS
	//			style, the framework sorts the Outlook bar by one or more calls to
	//			the CompareItem member function. Use InsertMenuItem to insert a menu
	//			item into a specific location within the Outlook bar. 
	// Returns: The zero-based index to the menu item in the Outlook bar. The return 
	//			value is LB_ERR if an error occurs. The return value is LB_ERRSPACE 
	//			if insufficient space is available to store the new menu item.
	//-----------------------------------------------------------------------
	int AddMenuItem(UINT nIconID,LPCTSTR lpszText);

	//-----------------------------------------------------------------------
	// Input:	iItem - Specifies the zero-based index of the menu item to retrieve.
	// Summary:	Call this member function to return an XT_CONTENT_ITEM object that
	//			represents the menu item specified by 'iItem'. 
	// Returns: An XT_CONTENT_ITEM pointer.
	//-----------------------------------------------------------------------
	XT_CONTENT_ITEM* GetMenuItem(int iItem);

	//-----------------------------------------------------------------------
	// Input:   nIDCursor    - Resource id of the highlight cursor.
	//          bNoBorder    - True if not border is displayed when item is highlighted.
	//          bNoUnderline - True if text is not underlined when item is highlighted.
	// Summary: Call this member function to set the cursor, text and border attributes
	//          for a highlighted item.
	//-----------------------------------------------------------------------
	void SetHilightCursor(UINT nIDCursor, bool bNoBorder=false, bool bNoUnderline=false);

    //:Ignore
	//{{AFX_VIRTUAL(CXTMenuListBox)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    //}}AFX_VIRTUAL
	//:End Ignore

protected:

	//-----------------------------------------------------------------------
	// Input:	strItem - NULL terminated string.
	// Summary:	This member function is called by the outlook bar to determine
	//          the string height for a multi-line text item.
	// Returns: The height in pixels of the specified string.
	//-----------------------------------------------------------------------
	int GetTextHeight(LPCTSTR lspzItem) const;

    //:Ignore
	//{{AFX_MSG(CXTMenuListBox)
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSysColorChange();
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	//:End Ignore

    DECLARE_MESSAGE_MAP()
};

typedef CXTMenuListBox CXTOutlookBar;

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTMenuListBox::SetColors(COLORREF clrText, COLORREF clrBack) {
    m_clrBack = clrBack; m_clrText = clrText; m_bUserColors = true;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTMENULISTBOX_H__)
//:End Ignore