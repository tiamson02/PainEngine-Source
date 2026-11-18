// XTPPropertyGrid.h interface for the CXTPPropertyGrid class.
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
#if !defined(__XTPPROPERTYGRID_H__)
#define __XTPPROPERTYGRID_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPPropertyGridPaintManager.h"
#include "CommonPro.Include/XTPDrawHelpers.h"

//===========================================================================
// Remarks: XTP_PROPERTYSORT is an enumeration used by CXTPPropertyGrid to 
//			determine the grouping style.
//===========================================================================
enum XTP_PROPERTYSORT 
{
	Categorized,  // Group items by category.
	Alphabetical, // Sort items alphabetically.
	NoSort        // Disable sorting.
};

class CXTPPropertyGridView;
class CXTPPropertyGridPaintManager;
class CXTPPropertyGrid;

//===========================================================================
// Remarks: CXTPPropertyGridToolTip is a CWnd derived class.
//			It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridToolTip : public CWnd
{
public:
	//-----------------------------------------------------------------------
	// Input:	bActive - TRUE to activate.
	// Summary: Activate Tooltip control.
	//-----------------------------------------------------------------------
	void Activate(BOOL bActive);
	
	//-----------------------------------------------------------------------
	// Input:	pParentWnd - Points to a CXTPPropertyGridView object.
	// Summary: Creates Tooltip control.
	//-----------------------------------------------------------------------
	void Create(CXTPPropertyGridView* pParentWnd);
	
protected:	
	CXTPPropertyGridView* m_pGrid;		// Parent window.

protected:

//:Ignore
    //{{AFX_MSG(CXTPPropertyGridView)
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnPaint();
	afx_msg UINT OnNcHitTest(CPoint point);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()	
//:End Ignore
};

//===========================================================================
// Remarks: CXTPPropertyGridView is a CListBox derived class.
//			It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridView : public CListBox
{
	DECLARE_DYNAMIC(CXTPPropertyGridView)

public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPropertyGridView object
	//-----------------------------------------------------------------------
	CXTPPropertyGridView();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridView object, handles cleanup and 
	//			de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridView();

	//-----------------------------------------------------------------------
	// Summary: Call this method to get the position of the divider.
	// Returns: Position of the divider
	//-----------------------------------------------------------------------
	int GetDividerPos();
	
	//-----------------------------------------------------------------------
	// Input:	point - Specifies the point to be tested. 
	// Summary: This method determines where a point lies in a specified item.
	// Returns: A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* ItemFromPoint(CPoint point);

	//-----------------------------------------------------------------------
	// Summary: Call this method to refresh items in the list.
	//-----------------------------------------------------------------------
	void Refresh();
	
	//-----------------------------------------------------------------------
	// Summary: Call this method to remove all items in the list.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Input:	nIndex - Specifies the zero-based index of the item to retrieve. 
	// Summary: This method retrieves an item with the specified index. 
	// Returns: A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetItem(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	strCaption - Name of the category to add.
	//			pCategory  - Points to a CXTPPropertyGridItem object.
	// Summary: Call this member function to add a new category to a Property
	//			Grid Control.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddCategory(CString strCaption, CXTPPropertyGridItem* pCategory = NULL);

private:

	CXTPPropertyGridItem* GetSelectedItem();	
	void SetPropertySort(XTP_PROPERTYSORT sort, BOOL bRrefresh = FALSE);
	int	InsertItem(CXTPPropertyGridItem* pItem, int nIndex);
	int _DoExpand(CXTPPropertyGridItem* pItem, int nIndex);
	void _DoCollapse(CXTPPropertyGridItem* pItem);
	void _RefreshIndexes();
	void SwitchExpandState(int nItem);
	int HitTest(CPoint point);

	void ShowToolTip(CPoint pt);
	void _ShowToolTip(CRect rcBound, CRect rcText);
	CSize _GetTextExtent(CString str);
	void RelayToolTipEvent(UINT message);


public:
    CFont						  m_fontNormal;			// Normal font.
    CFont						  m_fontBold;			// Bold font.

//:Ignore
#ifdef _XTP_DOTNET
	// Indicates if the selected item is drawn with a black border ( at design-time only )
	BOOL m_highLight;
#endif
//:End Ignore

    CXTPPaintManagerColor					  m_clrLine;			// Line color.
    CXTPPaintManagerColor					  m_clrFore;            // Color of the item's text.
	CXTPPaintManagerColor					  m_clrCategoryFore;    // Color of the category text.
    CXTPPaintManagerColor					  m_clrBack;            // Background color.
	CXTPPaintManagerColor					  m_clrReadOnlyFore;            
	CXTPPropertyGrid*			  m_pGrid;              // Parent CXTPPropertyGrid class.

protected:

    BOOL						  m_bTracking;			// TRUE if grid in tracking mode.
	CRect						  m_rcToolTip;			// Position of the Tooltip.
    double						  m_dDivider;			// Divider position.
	HCURSOR						  m_hCursor;			// Handle of the cursor.
	CString						  m_strTipText;			// Tooltip text.
    XTP_PROPERTYSORT			  m_properetySort;		// Current sort order.
    CXTPPropertyGridItem*		  m_pSelected;			// Current selected item.
    CXTPPropertyGridItems*        m_pCategories;		// Collection of the categories.
	CXTPPropertyGridToolTip		  m_wndTip;				// Tooltip control.
	CXTPPropertyGridInplaceEdit	  m_wndEdit;			// Inplace edit control.
	CXTPPropertyGridInplaceButton m_wndButton;			// Inplace button control.
	CXTPPropertyGridInplaceList	  m_wndListBox;			// Inplace list control.
	
	int						  m_nLockUpdate;		// TRUE to lock update.
	
	

protected:
//:Ignore
    //{{AFX_VIRTUAL(CXTPPropertyGridView)
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	
	virtual int CompareItem(LPCOMPAREITEMSTRUCT) { return 0;}
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CXTPPropertyGridView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelectionChanged();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCntr, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus (CWnd* pNewWnd);	
    //}}AFX_MSG
//:End Ignore

private:
	DECLARE_MESSAGE_MAP()

	friend class CXTPPropertyGridItem;			
	friend class CXTPPropertyGridItems;			
	friend class CXTPPropertyGrid;				
	friend class CXTPPropertyGridInplaceEdit;	
	friend class CPropertyGridCtrl;				
	friend class CXTPPropertyGridToolTip;		
};

//===========================================================================
// Remarks: CXTPPropertyGridToolBar is a CToolBar derived class.
//			It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridToolBar : public CToolBar
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPropertyGridToolBar object
	//-----------------------------------------------------------------------
	CXTPPropertyGridToolBar();

protected:

//:Ignore
    //{{AFX_VIRTUAL(CXTPPropertyGridToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

	
    //{{AFX_MSG(CXTPPropertyGridToolBar)
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnPaint();
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()
//:End Ignore
		
};

//===========================================================================
// Summary: CXTPPropertyGrid is a CWnd derived class. It is used to create a property grid
//			control similar to the property window in VS.NET.<p>
//			See the "PropertyGrid" demo for an example of its usage.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGrid : public CWnd
{
	DECLARE_DYNAMIC(CXTPPropertyGrid)

public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPropertyGrid object
	//-----------------------------------------------------------------------
	CXTPPropertyGrid();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGrid object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGrid();

	//-----------------------------------------------------------------------
	// Input:	rect       - Size of the control.
	//       	pParentWnd - Parent of the control.
	//       	nID        - Identifier of the Property Grid control.
	// Summary: Call this member function to create the Property Grid control.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwListStyle = LBS_OWNERDRAWFIXED| LBS_NOINTEGRALHEIGHT);

public:

	//-----------------------------------------------------------------------
	// Input:	strCaption - Name of the category to add.
	// Summary: Call this member function to add a new category to the Property Grid control.
	// Returns: The pointer to the item object of the newly inserted category.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddCategory(CString strCaption);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of new category.
	// Summary: Call this member function to add a new category to the Property 
	//			Grid control.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddCategory(int nID);
	
	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	sort - style of grouping. It can be one of the following:<p>
	//			[ul]
	//			[li]<b>Categorized</b> Group items by category.[/li]
	//			[li]<b>Alphabetical</b> Sort items alphabetically.[/li]
	//			[li]<b>NoSort</b> Disable sorting.[/li]
	//			[/ul]
	// Summary:	Call this member function to modify the way items are grouped.
	//-----------------------------------------------------------------------
	void SetPropertySort (XTP_PROPERTYSORT sort);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the current grouping style for 
	//			the Property Grid control.
	// Returns: An XTP_PROPERTYSORT value representing the current grouping style.
	//-----------------------------------------------------------------------
	XTP_PROPERTYSORT GetPropertySort();

	//-----------------------------------------------------------------------
	// Input:	bShow - TRUE to show the toolbar control, or FALSE to hide it.		
	// Summary: Call this member function to show or hide the toolbar control
	//			in the Property Grid window.
	//-----------------------------------------------------------------------
	void ShowToolBar(BOOL bShow = TRUE);
	
	//-----------------------------------------------------------------------
	// Input:	bShow - TRUE to show the description pane, or FALSE to hide it.
	// Summary: Call this member function to show or hide the description pane 
	//			in the Property Grid window.
	//-----------------------------------------------------------------------
	void ShowHelp(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Input:	clrHelpBack - An RGB value that represents the background color 
	//						  the of description pane.
	//       	clrHelpFore - An RGB value that represents the text color of 
	//						  the description pane.
	//       	clrViewLine - An RGB value that represents the color of seperating 
	//						  lines in control.
	//       	clrViewBack - An RGB value that represents the background color 
	//						  of the Property Grid control.
	//       	clrViewFore - An RGB value that represents the text color of 
	//						  the Property Grid control.
	// Summary: Call this member function to set the custom colors of the Property 
	//			Grid control.
	//-----------------------------------------------------------------------
	void SetCustomColors(COLORREF clrHelpBack, COLORREF clrHelpFore, COLORREF clrViewLine, COLORREF clrViewBack, COLORREF clrViewFore, COLORREF clrCategoryFore = ::GetSysColor(COLOR_GRAYTEXT) );

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the default colors of the Property 
	//			Grid control.
	//-----------------------------------------------------------------------
	void SetStandardColors();

	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption for the item to find.
	// Summary: Call this member function to find an item.
	// Returns:	The pointer to the Item object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* FindItem(CString strCaption);

	//-----------------------------------------------------------------------
	// Input:	 nID - Identifier for the item to find.
	// Summary: Call this member function to find an item.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* FindItem(UINT nID);

	//-----------------------------------------------------------------------
	// Summary: Removes all items from the list box of a property grid control.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the help pane is visible.
	// Returns:	TRUE if the help pane is visible, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHelpVisible() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the toolbar is visible.
	// Returns:	TRUE if the toolbar is visible, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsBarVisible() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the height of the help pane 
	//			when it is visible.
	// Returns:	An integer value that represents the height in pixels.
	//-----------------------------------------------------------------------
	int GetHelpHeight() const;

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the background color for 
	//			the property grid.
	// Returns:	An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetHelpBackColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the foreground color for 
	//			the property grid.
	// Returns:	An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetHelpForeColor();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the categories of the property 
	//			grid.
	// Returns:	The pointer to the CXTPPropertyGridItems object that represents 
	//			collection of the categories.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItems* GetCategories();

	//-----------------------------------------------------------------------
	// Summary: Call this method to retrieve reference to the view object.
	// Returns: A CXTPPropertyGridView object
	//-----------------------------------------------------------------------
	CXTPPropertyGridView& GetGridView();
	
	//-----------------------------------------------------------------------
	// Summary: Call this method to retrieve reference to the toolbar for the 
	//			property grid.
	// Returns: A reference to a CToolBar object
	//-----------------------------------------------------------------------
	CToolBar& GetToolBar();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get selected item.
	// Returns:	The pointer to the CXTPPropertyGridItems object currently selected.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetSelectedItem();

	//-----------------------------------------------------------------------
	// Input:	nHeight - The integer value represents the height in pixels.
	// Summary: Call this member function to set the height of the help pane 
	//			when visible.
	//-----------------------------------------------------------------------
	void SetHelpHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Input:   dDivider - A value between 0 and 1 that represents the location of the divider as
	//                     a percentage from the left side of the view.  For example, if you wanted
	//                     the divider to divide at 25% you would pass in .25.
	// Summary: Call this member function to set the divider position of the grid view.
	//-----------------------------------------------------------------------
	void SetViewDivider(double dDivider);
	
	//-----------------------------------------------------------------------
	// Input:   pFont - Pointer to a CFont object.
	// Summary: This method sets the grid’s current font to the specified font.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary: Call this member to set the grid theme.
	//-----------------------------------------------------------------------
	void SetTheme(XTPPropertyGridPaintTheme paintTheme);
	
	//-----------------------------------------------------------------------
	// Input:	pPaintManager - Points to a CXTPPropertyGridPaintManager object
	// Summary: Call this method to set custom theme of the grid.
	//-----------------------------------------------------------------------
	void SetCustomTheme(CXTPPropertyGridPaintManager* pPaintManager);
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves current theme of the grid.
	//-----------------------------------------------------------------------
	XTPPropertyGridPaintTheme GetCurrentTheme();

	//-----------------------------------------------------------------------
	// Summary: Call this method to refresh items in the list.
	//-----------------------------------------------------------------------
	void Refresh();
	
	CXTPPropertyGridPaintManager* m_pPaintManager;	// Current paint manager.

private:

	void OnInvertTracker(CRect rc);
	int HitTest(CPoint pt);
	void OnSelectionChanged(CXTPPropertyGridItem* pItem);
	void OnSortChanged();
	void Reposition(int cx, int cy);
	void CreateToolbar();
	void RefreshToolBarButtons();

public:
	int					m_nHelpHeight;				// Height of the description area.
	CXTPPaintManagerColor		m_clrHelpBack;				// Color of the description background.
	CXTPPaintManagerColor		m_clrHelpFore;				// Color of the description text.
	
private:
	CXTPPropertyGridView m_wndView;					
	BOOL				m_bHelpVisible;				
	BOOL				m_bToolBarVisible;			
	CRect				m_rectTracker;				
	CXTPPropertyGridToolBar			m_wndToolbar;
	HCURSOR              m_hCursor;					

	XTPPropertyGridPaintTheme m_themeCurrent;	

protected:

//:Ignore
    //{{AFX_MSG(CXTPPropertyGrid)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSortAlphabetic();
	afx_msg void OnSortCategorized();
	afx_msg void OnSysColorChange();
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CXTPPropertyGridView;		
	friend class CPropertyGridCtrl;	
//:End Ignore	
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::FindItem(CString strCaption) {
	return m_wndView.m_pCategories->FindItem(strCaption);
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::FindItem(UINT nID) {
	return m_wndView.m_pCategories->FindItem(nID);
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::AddCategory(CString strCaption) { 
	return m_wndView.AddCategory(strCaption); 
}
AFX_INLINE void CXTPPropertyGrid::SetPropertySort (XTP_PROPERTYSORT sort) { 
	m_wndView.SetPropertySort(sort); 
}
AFX_INLINE XTP_PROPERTYSORT CXTPPropertyGrid::GetPropertySort () { 
	return m_wndView.m_properetySort; 
}
AFX_INLINE BOOL CXTPPropertyGrid::IsHelpVisible() const {
	return m_bHelpVisible;
}
AFX_INLINE BOOL CXTPPropertyGrid::IsBarVisible() const {
	return m_bToolBarVisible;
}
AFX_INLINE int CXTPPropertyGrid::GetHelpHeight() const {
	return m_nHelpHeight;
}
AFX_INLINE COLORREF CXTPPropertyGrid::GetHelpBackColor() {
	return m_clrHelpBack;
}
AFX_INLINE COLORREF CXTPPropertyGrid::GetHelpForeColor() {
	return m_clrHelpFore;
}
AFX_INLINE CXTPPropertyGridView& CXTPPropertyGrid::GetGridView() {
	return m_wndView;
}
AFX_INLINE CToolBar& CXTPPropertyGrid::GetToolBar() {
	return m_wndToolbar;
}
AFX_INLINE XTPPropertyGridPaintTheme CXTPPropertyGrid::GetCurrentTheme() {
	return m_themeCurrent;
}

#ifdef _XTP_DOTNET
AFX_INLINE void CXTPPropertyGrid::SetHelpForeColor(COLORREF c)
{
	m_clrHelpFore=c;
}

AFX_INLINE void CXTPPropertyGrid::SetHelpBackColor(COLORREF c)
{
	m_clrHelpBack=c;
}
AFX_INLINE void CXTPPropertyGrid::SetViewForeColor(COLORREF c)
{
	m_wndView.m_clrFore = c;
}

AFX_INLINE void CXTPPropertyGrid::SetViewBackColor(COLORREF c)
{
	m_wndView.m_clrBack = c;
}
AFX_INLINE void CXTPPropertyGrid::SetLineColor(COLORREF c)
{
	m_wndView.m_clrLine = c;
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::GetItemFromPoint(CPoint pt)
{
	return m_wndView.ItemFromPoint(pt);
}
#endif


//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTPPROPERTYGRID_H__)
