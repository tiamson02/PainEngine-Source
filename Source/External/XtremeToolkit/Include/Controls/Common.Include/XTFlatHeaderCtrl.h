// XTFlatHeaderCtrl.h interface for the CXTFlatHeaderCtrl class.
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
#if !defined(__XTFLATHEADERCTRL_H__)
#define __XTFLATHEADERCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTHeaderTheme;
class CXTThemeManager;

//========================================================================
// Remarks: CXTHeaderCtrl is a CHeaderCtrl derived class. It is used to create
//			a CXTHeaderCtrl window similar to CHeaderCtrl, but with additional functionality. 
// 
//			<p>A “header control” is a window that is usually positioned above columns of text
//			or numbers. It contains a title for each column, and it can be divided
//			into parts. The user can drag the dividers that separate the parts to
//			set the width of each column.</p>
//
//			<p>Use a header control, represented by class CXTHeaderCtrl, to display
//			column headers for a columnar list. For example, a header control would
//			be useful for implementing column controls in a spreadsheet.</p>
//			
//			<p>The header control is usually divided into parts, called "header items,"
//			each bearing a title for the associated column of text or numbers. Depending
//			on the styles you set, you can provide a number of direct ways for users
//			to manipulate the header items.</p>
//========================================================================
class _XT_EXT_CLASS CXTHeaderCtrl : public CHeaderCtrl_VC50
{
    DECLARE_DYNAMIC(CXTHeaderCtrl)

public:

    //--------------------------------------------------------------------
    // Summary: Constructs a CXTHeaderCtrl object
    //--------------------------------------------------------------------
	CXTHeaderCtrl();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTHeaderCtrl object, handles cleanup and de-allocation
    //--------------------------------------------------------------------
    virtual ~CXTHeaderCtrl();

protected:

    int              m_iMinSize;      // Minimum column size for an auto-size header control.
    int              m_nPos;          // Index of the popup menu contained in the menu.
    int              m_iOverIndex;    // Column mouse is currently hovering.
    int              m_nSortedCol;    // Last column pressed during sort.
    bool             m_bRTL;          // Used internally to determine if text is right-to-left or left-to-right (depends on system locale).
    bool             m_bAutoSize;     // true if the header control columns are auto-sizing.
    BOOL             m_bEnableMenus;  // TRUE to disable the popup menu display.
    BOOL             m_bAscending;    // Used, when column is pressed, to draw a sort arrow.
    BOOL             m_bLBtnDown;     // TRUE if left mouse button is pressed.
    BOOL             m_bPainted;      // Used during paint operations.
    UINT             m_popupMenuID;   // Popup menu resource ID.
    CPoint           m_pt;            // Point where right click took place.
    CXTHeaderTheme*  m_pHeaderTheme;  // Pointer to the current header theme.
	CXTThemeManager* m_pThemeManager; // Pointer to the theme manager used by the header control.
    CList<int,int&>  m_arFrozenCols;  // List of columns that are not sizable.

public:

    //--------------------------------------------------------------------
	// Input:	bEnable - true to enable an auto-sizing header control.
    // Summary:	Call this member function to enable auto-sizing for the header control.
	//			This will cause the columns in the list control to be sized to fit
	//			in the available space when the list control is resized.
    //--------------------------------------------------------------------
	void EnableAutoSize(bool bEnable=true);

    //--------------------------------------------------------------------
	// Input:	iCol - Index of the column to freeze.
    // Summary:	Call this member function to freeze a column in the header control.
	//			Freezing a column will disable sizing for the column.
    //--------------------------------------------------------------------
	void FreezeColumn(int iCol);

    //--------------------------------------------------------------------
	// Input:	iCol - Index of the column to thaw.
	// Summary:	Call this member function to thaw a column in the header control.
	//			Thawing a column will enable sizing for the column if it was previously
	//			frozen.
    //--------------------------------------------------------------------
	void ThawColumn(int iCol);

    //--------------------------------------------------------------------
	// Summary:	Call this member function to thaw all column in the header control.
	//			This will will enable sizing for any column that was previously
	//			frozen.
    //--------------------------------------------------------------------
	void ThawAllColumns();

    //--------------------------------------------------------------------
	// Input:	iCol - Index of the column to check.
    // Summary:	Call this member function to determine if the specified column is
	//			frozen.  
	// Returns: true if the column is frozen, otherwise returns false.
    //--------------------------------------------------------------------
	bool IsColFrozen(int iCol);

    //--------------------------------------------------------------------
	// Input:	iMinSize - Minimum column size.
    // Summary:	Call this member to set the minimum size for auto-sizing columns.
	//			The minimum size represents the smallest size that all columns can
	//			be sized to.
    //--------------------------------------------------------------------
	void SetMinSize(int iMinSize);

    //--------------------------------------------------------------------
	// Input:	iSortCol - Zero-based index of the column to set the sort image for.
	//			bSortAsc - TRUE if ascending, otherwise FALSE.
    // Summary:	This member function will set the sort image for the specified column.
	// Returns: A zero-based index of the previously sorted column.
    //--------------------------------------------------------------------
	int SetSortImage(int iSortCol, BOOL bSortAsc);

    //--------------------------------------------------------------------
	// Input:   pbSortAsc - Set to TRUE if ascending, otherwise FALSE. 
    // Summary:	Call this member function to return the index to the currently sorted
	//			column. 
	// Returns: The index of the currently sorted column.
    //--------------------------------------------------------------------
	int GetSortedCol(BOOL* pbSortAsc=NULL);

    //--------------------------------------------------------------------
	// Input:	iCol      - Zero-based index of the column to set the sort image for.
	//			uBitmapID - Resource ID of the bitmap to use.
	//			bRemove   - TRUE to remove a bitmap, FALSE if adding a bitmap.
    //          crMask    - Color used to generate the mask. Each pixel of 
	//                      this color in the given bitmap is changed to black 
	//                      and the corresponding bit in the mask is set to one.
    // Summary:	Call this member function to set the bitmap image for the specified
    //			header item.
    //--------------------------------------------------------------------
    void SetBitmap(int iCol, UINT uBitmapID, BOOL bRemove, COLORREF crMask);

    //--------------------------------------------------------------------
	// Input:	iCol      - Zero-based index of the column to set the sort image for.
	//			uBitmapID - Resource ID of the bitmap to use.
	//			dwRemove  - Style bits to be removed from the HD_ITEM::fmt variable. For
    //			            a column that does not display text, pass in HDF_STRING.
    // Summary:	Call this member function to set the bitmap image for the specified
    //			header item.
    //--------------------------------------------------------------------
    void SetBitmap(int iCol, UINT uBitmapID, DWORD dwRemove=NULL);

    //--------------------------------------------------------------------
	// Input:   iItem - Index of the header item to check.
    // Summary: Call this member function to determine if the specified header
	//          item is pressed (selected).
	// Returns: TRUE if the specified header item is pressed (selected).
    //--------------------------------------------------------------------
	virtual BOOL ItemPressed(int iItem);

    //--------------------------------------------------------------------
	// Summary: Call this member function to return a pointer to the theme
	//          manager used by the header control.
	// Returns: A CXTThemeManager pointer.
    //--------------------------------------------------------------------
	CXTThemeManager* GetThemeManager() const;

    //--------------------------------------------------------------------
	// Input:   A CXTThemeManager pointer.
	// Summary: Call this member function to set the theme manager used by the
	//          header control.  This provides a way for extending the standard
	//          theme manager for use with the header control.
    //--------------------------------------------------------------------
	void SetThemeManager(CXTThemeManager* pThemeManager);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve a pointer to the header
	//          control theme.
	// Returns: A pointer to a CXTHeaderTheme object.
	//-----------------------------------------------------------------------
	CXTHeaderTheme* GetTheme() const;

	//-----------------------------------------------------------------------
	// Input:   pHeaderTheme - Points to a user defined CXTHeaderTheme object.
	// Summary: Call this member function to set a user defined theme for the
	//          header control.
	//-----------------------------------------------------------------------
	virtual void SetTheme(CXTHeaderTheme* pHeaderTheme);

    //--------------------------------------------------------------------
	// Summary: This member function is called to check if the header control
	//          is using a theme.
	// Returns: TRUE if a theme is used, otherwise FALSE.
    //--------------------------------------------------------------------
	BOOL IsThemeValid() const;

    //--------------------------------------------------------------------
	// Input:   pt     - POINT structure that contains the point to be hit 
	//                   test, in client coordinates.
	//          pFlags - Variable that receives information about the results 
	//                   of a hit test. This member can be one or more of the 
	//                   following values: HHT_ABOVE  The point is above the 
	//                   header control's bounding rectangle. 
	// 
	//                   HHT_BELOW          - The point is below the header 
	//                                        control's bounding rectangle. 
	//                   HHT_NOWHERE        - The point is inside the header 
	//                                        control's bounding rectangle but 
	//                                        is not over a header item. 
	//                   HHT_ONDIVIDER      - The point is on the divider 
	//                                        between two header items. 
	//                   HHT_ONDIVOPEN      - The point is on the divider of 
	//                                        an item that has a width of zero. 
	//                                        Dragging the divider reveals the 
	//                                        item instead of resizing the item 
	//                                        to the left of the divider. 
	//                   HHT_ONHEADER       - The point is inside the header 
	//                                        control's bounding rectangle. 
	//                   HHT_ONFILTER       - Version 5.80 The point is over 
	//                                        the filter area. 
	//                   HHT_ONFILTERBUTTON - Version 5.80 The point is on the 
	//                                        filter button. 
	//                   HHT_TOLEFT         - The point is to the left of the 
	//                                        header control's bounding rectangle. 
	//                   HHT_TORIGHT        - The point is to the right of the 
	//                                        header control's bounding rectangle. 
    // Summary: Tests a point to determine which header item, if any, is at 
	//          the specified point. 
	// Returns: If the hit test is successful, contains the index of the item 
	//          at the hit test point, if any, or – 1 otherwise.
    //--------------------------------------------------------------------
    int HitTest(CPoint pt, UINT* pFlags = NULL) const;

    //--------------------------------------------------------------------
    // Summary: Called by the header control to recalculate the size and layout
	//          for the header control and header items.
	// Returns: TRUE if successful, otherwise FALSE.
    //--------------------------------------------------------------------
    BOOL RecalcLayout();

    //--------------------------------------------------------------------
	// Input:	bBoldFont - TRUE if the header's font should be bold.
    // Summary:	This member function must be called after creation to initialize 
    //			the font that the header will use.
    //--------------------------------------------------------------------
    virtual void InitializeHeader(BOOL bBoldFont);

    //--------------------------------------------------------------------
	// Input:	popupMenuID - Resource ID for the popup menu used with the header.
	//			nPos        - Position of the submenu to be displayed.
    // Summary:	This member function is called to associate a menu and toolbar 
    //			resource with the context menu.
    //--------------------------------------------------------------------
    virtual void SetMenuID(UINT popupMenuID,int nPos=0);

    //--------------------------------------------------------------------
	// Input:	bBoldFont - TRUE if the header's font should be bold.
    // Summary:	This member function can be used to toggle the font from bold to normal.
    //--------------------------------------------------------------------
    virtual void SetFontBold(BOOL bBoldFont = TRUE);

    //--------------------------------------------------------------------
	// Input:	bSortArrow - Set to TRUE to draw the column sort arrow.
    // Summary:	Call this member function to enable or disable the column sort arrow.
    //--------------------------------------------------------------------
    virtual void ShowSortArrow(BOOL bSortArrow);

    //--------------------------------------------------------------------
	// Input:	nFlag - Text alignment, either LVCFMT_CENTER, LVCFMT_LEFT, or LVCFMT_RIGHT.
    // Summary:	Call this member function to set the text justification for the
	//			header control. 
	// Returns: TRUE if successful, otherwise returns FALSE.
    //--------------------------------------------------------------------
    virtual BOOL SetAlingment(int nFlag);
    
    //--------------------------------------------------------------------
    // Summary:	Call this member function to return the current sorting order. 
	// Returns: TRUE if the sorting order is ascending, otherwise returns FALSE.
    //--------------------------------------------------------------------
    BOOL GetAscending() const;

    //--------------------------------------------------------------------
	// Input:	bEnableMenus - TRUE to enable popup menus, and FALSE to disable.
	// Summary:	Call this member function to enable or disable the popup menu display whenever
	//			a user right clicks on the header control.
    //--------------------------------------------------------------------
	void EnablePopupMenus(BOOL bEnableMenus);

    //--------------------------------------------------------------------
	// Summary:	Call this member function to determine if the header control displays
	//			a sort arrow. 
	// Returns: TRUE if the header control displays a sort arrow, otherwise returns FALSE.
    //--------------------------------------------------------------------
	BOOL HasSortArrow();

    //--------------------------------------------------------------------
	// Summary:	Call this member function to resize all header columns equally
	//			to fit the available client area.
    //--------------------------------------------------------------------
	void ResizeColumnsToFit();

protected:

    //--------------------------------------------------------------------
    // Summary:	This member function is used by the header control to determine
	//			the text alignment for the system locale.  
	// Returns: true if the text alignment is right-to-left, and false if the text 
	//			alignment is left-to-right.
    //--------------------------------------------------------------------
	virtual bool DetermineRTL();

    //--------------------------------------------------------------------
	// Input:	iNewWidth - New width to resize all columns to.
    // Summary:	This member function is used by the header control to auto-size the
	//			columns for the list control.
    //--------------------------------------------------------------------
	virtual void ApplyFieldWidths(int iNewWidth);

    //--------------------------------------------------------------------
	// Input:	iNewWidth - New width to resize all columns to.  If iNewWidth
	//                      is '0', the columns are equally sized to fit the
	//                      available client area.
	// Summary:	This member function is used by the header control to determine
	//			the new width for auto-sized columns and resize accordingly.
    //--------------------------------------------------------------------
	virtual void FitFieldWidths(int iNewWidth);

    //--------------------------------------------------------------------
    // Summary:	This member function is used by the header control to determine
	//			the total width of all frozen columns in the header control.  
	// Returns: The combined size in pixels of all frozen columns.
    //--------------------------------------------------------------------
	virtual int GetFrozenColWidth();

    //--------------------------------------------------------------------
	// Input:	iIndex - Index of the column to be sorted.
    // Summary:	Called to send WM_NOTIFY to tell parent's owner that the column needs
	//			to be sorted.
    //--------------------------------------------------------------------
    void SendNotify(int iIndex);
    
	//{{AFX_VIRTUAL(CXTHeaderCtrl)
	//}}AFX_VIRTUAL

protected:

    //:Ignore
	//{{AFX_MSG(CXTHeaderCtrl)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnLayout(WPARAM wparam, LPARAM lparam);
	afx_msg void OnDestroy();
	afx_msg BOOL OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSysColorChange();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSortAsc();
    afx_msg void OnSortDsc();
    afx_msg void OnAlignLeft();
    afx_msg void OnAlignCenter();
    afx_msg void OnAlignRight();
	//}}AFX_MSG
	//:End Ignore
	
	DECLARE_MESSAGE_MAP()

	friend class CXTHeaderTheme;
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTHeaderCtrl::SetFontBold(BOOL bBoldFont) {
    ASSERT(::IsWindow(m_hWnd)); SetFont(bBoldFont?&XTAuxData().fontBold:&XTAuxData().font);
}
AFX_INLINE BOOL CXTHeaderCtrl::GetAscending() const {
    ASSERT(::IsWindow(m_hWnd)); return m_bAscending;
}
AFX_INLINE void CXTHeaderCtrl::EnablePopupMenus(BOOL bEnableMenus) {
	m_bEnableMenus = bEnableMenus;
}

//========================================================================
// Remarks: CXTFlatHeaderCtrl is a CHeaderCtrl derived class. It is used to create
//			list box flat header controls that are similar to list box flat header
//			controls seen in Visual Studio&trade and Outlook&trade;.
//========================================================================
class _XT_EXT_CLASS CXTFlatHeaderCtrl : public CXTHeaderCtrl
{
    DECLARE_DYNAMIC(CXTFlatHeaderCtrl)
public:

    //--------------------------------------------------------------------
    // Summary: Constructs a CXTFlatHeaderCtrl object
    //--------------------------------------------------------------------
    CXTFlatHeaderCtrl();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTFlatHeaderCtrl object, handles cleanup and de-allocation
    //--------------------------------------------------------------------
    virtual ~CXTFlatHeaderCtrl();
};


//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTFLATHEADERCTRL_H__)
//:End Ignore
