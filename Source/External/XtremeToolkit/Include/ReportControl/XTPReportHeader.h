// XTPReportHeader.h: interface for the CXTPReportHeader class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPREPORTHEADER_H__)
#define __XTPREPORTHEADER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportColumn;
class CXTPReportColumns;
class CXTPReportControl;
class CXTPReportSubListControl;
class CXTPReportFilterEditControl;
class CXTPReportHeaderDropWnd;
class CXTPReportHeaderDragWnd;

//===========================================================================
// Remarks: A report header is a part of the report window positioned above 
//			columns of text or numbers. It contains a title for each column, 
//			and it can be divided into parts. The user can drag the dividers 
//			that separate the parts to set the width of each column. 
//
//			This class is responsible for header drawing on a report window, 
//			processing user events, and changing header properties.
//
//			Functionality of the header includes the following:
//
//			[ul]
//			[li]Header self-drawing.[/li] 
//			[li]Header item custom ordering. [/li]
//			[li]Header item drag and drop, for the reordering of header items.[/li]
//			[li]Sorting report rows with mouse click on column header.[/li]
//			[li]Filtering of the report row items using associated CXTPReportFilterEditControl.[/li]
//			[li]Header items choosing using associated filed chooser control: CXTPReportSubListControl.[/li]
//			[li]Group By area support containing the order of grouped colums.[/li]
//			[/ul]
//
//
// See also: CXTPReportControl, CXTPReportColumn, CXTPReportSubListControl
//===========================================================================
class _XTP_EXT_CLASS CXTPReportHeader : public CCmdTarget
{
	friend class CXTPReportSubListControl;
	friend class CXTPReportControl;
	friend class CXTPReportColumn;
public:
	//-----------------------------------------------------------------------
	// Input:	pColumns - Pointer to associated columns array.
	//			pControl - Pointer to the parent report control.
	// Summary: Constructs a CXTPReportHeader object.
	// Example: // Declare a local CXTPReportHeader object.
	//			CXTPReportHeader  myHeader;
	//
	//			// Declare a dynamic CXTPReportHeader object.
	//			CXTPReportHeader* pmyHeader = new CXTPReportHeader;
	// See also: ~CXTPReportHeader
	//-----------------------------------------------------------------------
	CXTPReportHeader(CXTPReportControl* pControl, CXTPReportColumns* pColumns);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportHeader object. Performs cleanup operations.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHeader(void);

protected:
	//-----------------------------------------------------------------------
    // Input:	pDC      - Pointer to the destination device context.
	//			rcHeader - Reference to the rectangle in which the header is drawn.
	// Summary: Draws columns header on the provided DC.
	// Remarks: Call this function to draw the header on the provided 
	//			device context. Note that it does not contain Group By area.
	// See also: DrawGroupByControl
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CRect& rcHeader);

	//-----------------------------------------------------------------------
    // Input:	pDC       - Pointer to the destination device context.
	//			rcGroupBy - Reference to the rectangle of the Group By area.
	// Summary: Draws Group By area on the provided DC.
	// Remarks: Call this function to draw the header on the provided 
	//			device context. Note that it does not contain Group By area.
	// See also: Draw
	//-----------------------------------------------------------------------
	void DrawGroupByControl(CDC* pDC, CRect& rcGroupBy);

	//-----------------------------------------------------------------------
	// Input:	ptClick - Point coordinates where the message was fired from.
	// Summary: Processes mouse left button down clicks.
	// Remarks: Call this member function if you want Report Header to process
	//			a mouse left button down message. You should provide the method
	//			with the coordinates of the mouse message to proceed.
	// See also: OnLButtonUp, OnContextMenu, OnMouseMove
	//-----------------------------------------------------------------------
	void OnLButtonDown(CPoint ptClick);

	//-----------------------------------------------------------------------
	// Input:	nFlags  - Indicates whether various virtual keys are down.
	//			ptClick - Point coordinates where the message was fired from.
	// Summary: Process mouse left button up clicks.
	// Remarks: Call this member function if you want Report Header to process
	//			a mouse left button up message. You should provide the method
	//			with the coordinates of the mouse message to proceed and a flag
	//			showing which of virtual keys were down.
	// See also: OnLButtonDown, OnContextMenu, OnMouseMove
	//-----------------------------------------------------------------------
	void OnLButtonUp(UINT nFlags, CPoint ptClick);

	//-----------------------------------------------------------------------
	// Input:	ptClick - Position of the cursor, in screen coordinates, 
	//					  at the time of the mouse click. 
	// Summary: Process context menu. Called by the framework when the user 
	//			has clicked the right mouse button (right clicked) in the window.
	// Remarks: Call this member function if you want Report Header to process
	//			a mouse right button down (context menu call) message. 
	//			You should provide the method with the coordinates of the 
	//			mouse message to proceed.
	// See also: OnLButtonDown, OnLButtonUp, OnMouseMove
	//-----------------------------------------------------------------------
	void OnContextMenu(CPoint ptClick);

	//-----------------------------------------------------------------------
	// Input:	nFlags - Indicates whether various virtual keys are down.
	//			point  - Point coordinates where the message was fired from.
	// Summary: Process mouse moving.
	// Remarks: Call this member function if you want Report Header to process
	//			a mouse move message. You should provide the method with the 
	//			coordinates of the mouse message to proceed and a flag
	//			showing which of virtual keys were down.
	// See also: OnLButtonDown, OnLButtonUp, OnContextMenu
	//-----------------------------------------------------------------------
    void OnMouseMove(UINT nFlags, CPoint point);

public:


	//-----------------------------------------------------------------------
	// Summary: Determines if dragging the header now.
	// Remarks: When user starts dragging a row, Report Header enters a dragging
	//			loop and processes each message in the specific way. 
	//			Using this function you can determine if we are in such a dragging
	//			loop or not.
	// Returns: TRUE when dragging, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsDragHeader();

	//-----------------------------------------------------------------------
	// Input:	bAllow - TRUE for allowing column removing, FALSE for disallowing.
	// Summary: Allows/disallows column removing.
	// Remarks: Usually user is allowed to remove a column using mouse drag and drop.
	//			Using this member function, you can change the user's permissions
	//			and force allowing or disallowing of column removing.
	// See also: IsAllowColumnRemove
	//-----------------------------------------------------------------------
	void AllowColumnRemove(BOOL bAllow);

	//-----------------------------------------------------------------------
	// Summary: Returns the current column removing allowance.
	// Remarks: Call this member function to determine wheither the user is allowed
	//			to remove the column using drag and drop or not. 
	// Returns: TRUE when column removing is allowed, FALSE otherwise.
	// See also: AllowColumnRemove
	//-----------------------------------------------------------------------
	BOOL IsAllowColumnRemove();

	//-----------------------------------------------------------------------
	// Input:	bAllow - TRUE for allowing column reordering, FALSE for disallowing.
	// Summary: Allows/disallows column reordering.
	// Remarks: Usually user is allowed to reorder a column using mouse drag and drop.
	//			Using this member function, you can change the user's permissions
	//			and force allowing or disallowing of column reordering.
	// See also: IsAllowColumnReorder
	//-----------------------------------------------------------------------
	void AllowColumnReorder(BOOL bAllow);

	//-----------------------------------------------------------------------
	// Summary: Returns the current column reordering allowance.
	// Remarks: Call this member function to determine whether the user is allowed
	//			to reorder the column using drag and drop or not. 
	// Returns: TRUE when column reordering is allowed, FALSE otherwise.
	// See also: AllowColumnReorder
	//-----------------------------------------------------------------------
	BOOL IsAllowColumnReorder();
	
	//-----------------------------------------------------------------------
	// Input:	bAllow - TRUE for allowing column resizing, FALSE for disallowing.
	// Summary: Allows/disallows column resizing.
	// Remarks: Usually the user is allowed to resize column using mouse drag and drop.
	//			Using this member function you can change user's permissions
	//			and force allowing or disallowing of column resizing.
	// See also: IsAllowColumnResize
	//-----------------------------------------------------------------------
	void AllowColumnResize(BOOL bAllow);

	//-----------------------------------------------------------------------
	// Summary: Returns current column resizing allowance.
	// Remarks: Call this member function to determine wheither the user is allowed
	//			to resize a column using drag and drop or not. 
	// Returns: TRUE when column resizing is allowed, FALSE otherwise.
	// See also: AllowColumnResize
	//-----------------------------------------------------------------------
	BOOL IsAllowColumnResize();

	//-----------------------------------------------------------------------
	// Input:	bShowInGroups - TRUE for show items in groups, FALSE otherwise.
	// Summary: Sets show items in groups mode.
	// Remarks: Using this member function you can change Report Header
	//			showing items in groups mode.
	// See also: IsShowItemsInGroups
	//-----------------------------------------------------------------------
	void ShowItemsInGroups(BOOL bShowInGroups);

	//-----------------------------------------------------------------------
	// Summary: Returns current show items in groups mode.
	// Remarks: Using this member function you can retrieve current
	//			showing items in groups mode.
	// Returns: TRUE when items are shown in groups, FALSE otherwise.
	// See also: ShowItemsInGroups
	//-----------------------------------------------------------------------
	BOOL IsShowItemsInGroups();

	
	//-----------------------------------------------------------------------
	// Input:	bAllow - TRUE for allowing, FALSE for disallowing.
	// Summary: Sets allowing column sorting mode.
	// Remarks: Using this member function you can allow or disallow sorting
	//			columns by mouse.
	// See also: IsAllowColumnSort
	//-----------------------------------------------------------------------
	void AllowColumnSort(BOOL bAllow);
	
	//-----------------------------------------------------------------------
	// Summary: Returns current allowing column sorting mode.
	// Remarks: Using this member function you can retrieve the current
	//			allowing column sorting mode.
	// Returns: TRUE when column sorting is allowed, FALSE otherwise.
	// See also: AllowColumnSort
	//-----------------------------------------------------------------------
	BOOL IsAllowColumnSort();

	CXTPReportColumn* GetNextVisibleColumn(int nIndex, int nDirection);

	//-----------------------------------------------------------------------
	// Input:	ptPoint - A point to test in report control client coordinates.
	// Summary: Finds a column from point.
	// Remarks: Call this member function if you want to retrieve a pointer to a
	//			column in the specified position.
	// Returns: A pointer to the column placed at the specified position, if any, 
	//			or NULL otherwise.
	// See also: CXTPReportControl::HitTest, CXTPReportRow::HitTest
	//-----------------------------------------------------------------------
	CXTPReportColumn* HitTest(CPoint ptPoint);
	

	//-----------------------------------------------------------------------
	// Input:	ptPoint - Point coordinates to test
	// Summary: Calculates index of where to insert a column nearest to the provided point.
	// Returns: Header column index.
	//-----------------------------------------------------------------------
	int FindHeaderColumn(CPoint ptPoint);

	//-----------------------------------------------------------------------
	// Input:	ptPoint		 - Point coordinates to test.
	//			bExactSearch - TRUE when search by exact position, 
	//						   FALSE when searching average value.
	// Summary: Calculates index of where to insert a column nearest to the provided point.
	// Returns: GroupBy column position.
	//-----------------------------------------------------------------------
	int FindGroupByColumn(CPoint ptPoint, BOOL bExactSearch = FALSE);

private:
	//-----------------------------------------------------------------------
	// Input:	ptPoint - A point to test in report control window client coordinates.
	// Summary: Finds header column index by provided point.
	// Returns: An index of the found column in the header column array, if found, 
	//			or -1 if column was not found.
	//-----------------------------------------------------------------------
	int HitTestHeaderColumnIndex(CPoint ptPoint);

	//-----------------------------------------------------------------------
	// Input:	ptPoint - Point coordinates to test
	// Summary: Checks if the provided point is over column resize area.
	// Returns:	TRUE if mouse is under column resize area, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL MouseOverColumnResizeArea(CPoint ptPoint);


protected:


	//-----------------------------------------------------------------------
	// Input:	nTotalWidth - Total width of the header area in pixels.
	//			nFirstIndex - Index of a column to start adjusting from.
	// Summary: Adjusts width of all columns starting from the nFirstIndex,
	//			for a new total width of nTotalWidth pixels.
	// Remarks: Call this member function when you want to adjust the width of
	//			columns in reaction to any specific event.
	//			For example, this member function is called by the framework
	//			when changing the report control size (WM_SIZE), and when
	//			changing a size of any column by mouse dragging.
	// See also: ResizeColumn, GetMaxAvailWidth
	//-----------------------------------------------------------------------
	void AdjustColumnsWidth(int nTotalWidth, int nFirstIndex = 0);

	//-----------------------------------------------------------------------
	// Input:	pColumn - Pointer to the column, which is entering in dragging mode.
	//			bHeader - TRUE when starting dragging for column header items, 
	//					  FALSE when starting dragging for Group By column items.
	// Summary: Starts column header dragging mode.
	// Remarks: Usually this method is called internally in a reaction to any
	//			specific user actions. For example when a user starts dragging
	//			column header by mouse.
	// See also: GetDraggingColumn, CancelMouseMode
	//-----------------------------------------------------------------------
	void StartDragging(CXTPReportColumn* pColumn, BOOL bHeader = FALSE);

	//-----------------------------------------------------------------------
	// Input:	pColumn - Pointer to a column to resize.
	//			nWidth  - New column width in pixels.
	// Summary: Resizes a provided column to a specific new width. 
	// Remarks: This method resizes a provided column to a specific new width, 
	//			and adjusts the width of other columns in correspondence with
	//			the new one.
	// See also: AdjustColumnsWidth, GetMaxAvailWidth
	//-----------------------------------------------------------------------
	void ResizeColumn(CXTPReportColumn* pColumn, int nWidth);

	//-----------------------------------------------------------------------
	// Summary: Resets any mouse mode to xtpMouseNothing.
	// Remarks: Usually this member function is called when finishing
	//			dragging or resizing columns. It performs all
	//			necessary cleanup and resets mouse mode to xtpMouseNothing.
	// See also: StartDragging
	//-----------------------------------------------------------------------
	void CancelMouseMode();

	//-----------------------------------------------------------------------
	// Input:	pColumn - Pointer to the column to check.
	// Summary: Returns maximum available width for the column.
	// Remarks: Calculates maximum possible width for the column depending
	//			on the width and modes for other columns.
	// Returns: Maximum available width for the column in pixels.
	// See also: AdjustColumnsWidth, ResizeColumn
	//-----------------------------------------------------------------------
	int GetMaxAvailWidth(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	nIndex  - An index of the column to set hot divider before.
	//			bHeader - TRUE if you want to set hot divider in columns header,
	//					  FALSE to set it between on Group By area items.
	// Summary: Sets a divider between columns.
	// Remarks: When dragging columns, call this member function to show a
	//			hot divider (red arrows by default) between columns as a target
	//			for dropping a currently dragging one.
	// Returns: An index of the column before which a hot divider was shown.
	//-----------------------------------------------------------------------
	int SetHotDivider(int nIndex, BOOL bHeader = TRUE);


public:
	//-----------------------------------------------------------------------
	// Summary: Returns a column, which is dragging at the moment.
	// Remarks: Call this member function if you want to retrieve a pointer
	//			to the currently dragging column.
	// Returns: A pointer to the dragging column if the control is in
	//			dragging mode, or NULL otherwise.
	// Example: CXTPReportColumn* pColumn = GetDraggingColumn();
	// See also: StartDragging, GetResizingColumn, 
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetDraggingColumn();

	//-----------------------------------------------------------------------
	// Input:	pSubList - Pointer to the field chooser list box control.
	// Summary: Sets associated Field chooser list control.
	// Remarks: You should call this member function to assign the user's custom
	//			List control as report control Field chooser control.
	//			After assigning, the framework will automatically be processing
	//			all windows messages from the control.
	// Returns: TRUE if successful; otherwise FALSE.
	// Example: // CXTPReportControl wndReport;
	//			// CXTPReportSubListControl wndSubList;
	//			wndReport.GetColumns()->GetReportHeader()->SetSubListCtrl(&wndSubList);
	// See also: CXTPReportSubListControl overview, SetFilterEditCtrl
	//-----------------------------------------------------------------------
	BOOL SetSubListCtrl(CXTPReportSubListControl* pSubList);

	CXTPReportSubListControl* GetSubListCtrl();

	//-----------------------------------------------------------------------
	// Input:	pFilterEdit - Pointer to the user's filter edit control.
	// Summary: Sets associated filter edit control.
	// Remarks: You should call this member function to assign the user's custom
	//			editbox control as report control filter edit control.
	//			After assigning, the framework will automatically be processing
	//			all windows messages for the control.
	// Returns: TRUE if successful; otherwise FALSE.
	// Example: // CXTPReportControl wndReport;
	//			// CXTPReportFilterEditControl wndFilterEdit;
	//			wndReport.GetColumns()->GetReportHeader()->SetFilterEditCtrl(&wndFilterEdit);
	// See also: CXTPReportFilterEditControl overview, GetFilterEditCtrl, SetSubListCtrl
	//-----------------------------------------------------------------------
	BOOL SetFilterEditCtrl(CXTPReportFilterEditControl* pFilterEdit);

	//-----------------------------------------------------------------------
	// Summary: Returns the associated Filter edit control.
	// Remarks: Call this member function to retieve a pointer to the 
	//			associated filter edit control.
	// Returns: A pointer to the associated filter edit control.
	// See also: CXTPReportFilterEditControl overview, SetFilterEditCtrl
	//-----------------------------------------------------------------------
	CXTPReportFilterEditControl* GetFilterEditCtrl();

	//-----------------------------------------------------------------------
	// Summary: Returns the Group By area height.
	// Remarks: Calculates Group By area height depending on the number of 
	//			items contained inside.
	// Returns: A Group By area height in pixels.
	//-----------------------------------------------------------------------
	int GetGroupByHeight();

	//-----------------------------------------------------------------------
	// Input:	pColumn - A column to fit width.
	// Summary: Fits the provided column width to the longest column item width.
	// Remarks: Call this method to adjust column width to the width of the
	//			widest column item.
	//-----------------------------------------------------------------------
	void BestFit(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - A pointer to the column to check.
	// Summary: Checks a column for its position in report header.
	// Returns: TRUE if a provided column is the last visible column in a report
	//			header, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsLastVisibleColumn(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - A pointer to the column to check.
	// Summary: Checks a column for its position in report header.
	// Returns: TRUE if a provided column is the last resizable column in a report
	//			header, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsLastResizebleColumn(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary: Returns an indent for report rows by 1 tree level.
	// Remarks: Call this member function to retrieve an indent for 1 level
	//			of tree depth for the report control in tree mode.
	// Returns: An indent by one level in pixels.
	//-----------------------------------------------------------------------
	int GetIndentLevel();

	//-----------------------------------------------------------------------
	// Summary: Updates header settings after any changes.
	// Remarks: Call this member function if you want to automatically
	//			adjust columns widths and redraw report control depending
	//			on current columns settings.
	//-----------------------------------------------------------------------
	void OnColumnsChanged();

	CXTPReportControl* GetControl();

	void SetAutoColumnSizing(BOOL bAutoColumnSizing);
	BOOL IsAutoColumnSizing();

	int GetWidth();


private:
	//=======================================================================
	// Summary: Enumeration of column dragging modes 
	//=======================================================================
	enum XTPDraggingMode 
	{ 
		xtpDragNothing,				// User is dragging nothing.
		xtpDragOutTarget,			// User is dragging an item outside the drop target.
		xtpDragInTarget,			// User is dragging an item inside the drop target.
		xtpDragFieldChooser			// User is dragging an item inside field chooser.
	} m_dragMode;					// Column dragging mode.

	void TrackColumn(CXTPReportColumn* pColumn, CPoint pt);
	

protected:
	CXTPReportColumns* m_pColumns;	// Contains associated columns list.
	CXTPReportControl* m_pControl;  // Pointer to the parent Report control.

	int m_nDropIndex;							// Item index of the column to drop to.
	CXTPReportHeaderDropWnd* m_pDropWnd;		// Column drop window.
	CXTPReportHeaderDragWnd* m_pDragWnd;		// Column drag window.

	HCURSOR m_hDontDropCursor;					// Handler of user's Don't Drop cursor.

	CXTPReportColumn* m_pDragColumn;			// Dragging column.
	BOOL m_bDragHeader;							// Determines wheither control is in header dragging mode or group by dragging mode.
	BOOL m_bDragGroupBox;						

	CXTPReportSubListControl* m_pSubList;		// Field chooser list control.
	CXTPReportFilterEditControl* m_pFilterEdit;	// Filter edit control

	CRect m_rcGroupBy;		// Group By area coordinates.
	CRect m_rcHeader;		// Header coordinates.
	CPoint m_ptMouse;		// Stores mouse coordinates for processing when required (resizing, etc).

	HCURSOR m_hResizeCursor;	// Handler of user's Resizing cursor.

	BOOL m_bAllowColumnRemove;	// FALSE if columns removing is not allowed.
	BOOL m_bAllowColumnResize;	// FALSE if columns resizing is not allowed.
	BOOL m_bAllowColumnReorder;	// FALSE if columns reordering is not allowed.
	BOOL m_bAllowColumnSort;	// FALSE if columns sorting is not allowed.

	BOOL m_bAutoColumnSizing;
    int m_nResizeCaptureRange;	// Distance around column divider where show column resizer.
	                            //+ Always Draw sets it to FALSE and  AdjustColumnsSize to TRUE
	int m_nIndentLevel;				// Width indent at the left of the column.
	BOOL m_bShowItemsInGroups;	// TRUE when showing items in groups, FALSE when grouping is not used.
	int m_nHeaderWidth;
};


AFX_INLINE CXTPReportFilterEditControl* CXTPReportHeader::GetFilterEditCtrl() {
	return m_pFilterEdit;
}

AFX_INLINE BOOL CXTPReportHeader::IsDragHeader() {
	return m_bDragHeader;
}
AFX_INLINE int CXTPReportHeader::GetIndentLevel() {
	return m_nIndentLevel;
}
AFX_INLINE void CXTPReportHeader::AllowColumnRemove(BOOL bAllow) {
	m_bAllowColumnRemove = bAllow;
}
AFX_INLINE void CXTPReportHeader::AllowColumnResize(BOOL bAllow) {
	m_bAllowColumnResize = bAllow;
}
AFX_INLINE void CXTPReportHeader::AllowColumnReorder(BOOL bAllow) {
	m_bAllowColumnReorder = bAllow;
}
AFX_INLINE BOOL CXTPReportHeader::IsAllowColumnRemove() {
	return m_bAllowColumnRemove;
}
AFX_INLINE BOOL CXTPReportHeader::IsAllowColumnReorder() {
	return m_bAllowColumnReorder;
}
AFX_INLINE BOOL CXTPReportHeader::IsAllowColumnResize() {
	return m_bAllowColumnResize;
}
AFX_INLINE void CXTPReportHeader::AllowColumnSort(BOOL bAllow) {
	m_bAllowColumnSort = bAllow;
}
AFX_INLINE BOOL CXTPReportHeader::IsAllowColumnSort() {
	return m_bAllowColumnSort;
}

AFX_INLINE BOOL CXTPReportHeader::IsShowItemsInGroups() {
	return m_bShowItemsInGroups;
}
AFX_INLINE CXTPReportControl* CXTPReportHeader::GetControl() {
	return m_pControl;
}
AFX_INLINE CXTPReportSubListControl* CXTPReportHeader::GetSubListCtrl() {
	return m_pSubList;
}
AFX_INLINE int CXTPReportHeader::GetWidth() {
	return m_nHeaderWidth;
}
AFX_INLINE BOOL CXTPReportHeader::IsAutoColumnSizing() {
	return m_bAutoColumnSizing;
}
#endif