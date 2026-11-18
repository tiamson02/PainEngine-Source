// XTPReportRow.h: interface for the CXTPReportRow class.
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
#if !defined(__XTPREPORTROW_H__)
#define __XTPREPORTROW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportRecord;
class CXTPReportControl;
class CXTPReportTip;
class CXTPReportRecordItem;
class CXTPReportRows;
class CXTPReportColumn;

struct XTP_REPORTRECORDITEM_DRAWARGS;
struct XTP_REPORTRECORDITEM_METRICS;

//===========================================================================
// Summary: CXTPReportRow class represents an item of the "report control"
//			window.
//
//			Each item has corresponding a CXTPReportRecord, and could have a list 
//			of subitems associated with it. By clicking a Row item, the user 
//			can expand and collapse the associated list of subitems.
//
//			CXTPReportRow handles a behavior of report row. It processes
//			user mouse and keyboard events, it also implements external control
//			member functions. It is also handling drawing logic for the row.
//
//			A collection of report row items is combined into CXTPReportRows
//			and could be accessed via CXTPReportControl::GetRows() method.
//
//          
// See also: CXTPReportControl overview, CXTPReportRows, CXTPReportRecord, 
//			 CXTPReportRecordItem, CXTPReportGroupRow
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRow : public CCmdTarget
{
public:
	
	//-----------------------------------------------------------------------
	// Summary: A constructor of the CXTPReportRow object.
	//-----------------------------------------------------------------------
	CXTPReportRow();

	//-----------------------------------------------------------------------
	// Input:	pControl - Pointer to the parent report control.
	//			pRecord - Pointer to the record associated with the row.
	//			Initialize Row.
	//-----------------------------------------------------------------------
	void InitRow(CXTPReportControl* pControl, CXTPReportRecord* pRecord);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportRow object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRow();


	//-----------------------------------------------------------------------
	// Summary: Get associated record.
	// Returns: Associated list record with data.
	// Example: CXTPReportRecord* pRecord = pRow->GetRecord();
	// See also: CXTPReportRecord overview
	//-----------------------------------------------------------------------
	CXTPReportRecord* GetRecord() const;

	//-----------------------------------------------------------------------
	// Input:    pDC	- Pointer to control drawing context.
	//			 rcRow	- Coordinates of the rectangle for drawing this row.
	// Summary: Draws the row on the provided DC.
	// Remarks: Call this member function to draw an item. This function 
	//			only prepares data for drawing and calls needed drawing functions 
	//			from PaintManager.
	//
	//			Thus if you wish to change the look of your report item, you must just 
	//          provide your own implementation of PaintManager 
	// See Also: CXTPPaintManager
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:	pDC - Provided DC.
	// Summary: Calculates height of the row depending on the provided DC.
	// Remarks: Row height depends of many factors: provided device context,
	//          font to pront text, visibility of rows items. GetHeight()
	//          takes into account all of them to get real height of Row.
	// Returns: Returns height of the row in logical units.
	//-----------------------------------------------------------------------
	virtual int GetHeight(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary: Returns current row index in the view.
	// Remarks: ReportControl draws row in some order. It depens of Index value
	//          You use GetIndex() to get current index value for particular row
	// Returns: Current row index in the view.
	//-----------------------------------------------------------------------
	int GetIndex();

	//-----------------------------------------------------------------------
	// Summary: Returns row focused state.
	// Remarks: Call this member function if you want to determine whether
	//			this row is focused in the report control or not.
	// Returns: TRUE if row is focused, FALSE otherwise.
	// See also: IsSelected
	//-----------------------------------------------------------------------
	BOOL IsFocused();
	
	//-----------------------------------------------------------------------
	// Summary: Returns row selected state.
	// Remarks: Call this member function if you want to determine whether
	//			this row is selected in the report control or not.
	// Returns: TRUE if row is selected, FALSE otherwise.
	// See also: IsFocused
	//-----------------------------------------------------------------------
	BOOL IsSelected();
	
	//-----------------------------------------------------------------------
	// Summary: Returns row expanded state.
	// Remarks: Call this member function if you want to determine whether
	//			this row is expanded in the report control or not.
	// Returns: TRUE if row is expanded, FALSE otherwise.
	// See also: SetExpanded
	//-----------------------------------------------------------------------
	BOOL IsExpanded();
	
	//-----------------------------------------------------------------------
	// Input:	bExpanded - New row expanded state.
	// Summary: Sets row expanded state.
	// Remarks: Call this member function if you want to expand or collapse
	//			this row.
	// See also: IsExpanded
	//-----------------------------------------------------------------------
	virtual void SetExpanded(BOOL bExpanded);

	void SetSelected(BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary: Determines if this row is a group row.
	// Remarks: Call this member function if you want to determine whether
	//			this row is a group row or not.
	// Returns: TRUE if it is a group row, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsGroupRow();

	//-----------------------------------------------------------------------
	// Input:	ptPoint		- Point to test.
	//			pRectItem	- Returns item rect.
	//			ppColumn	- Returns a pointer to the column at tested position.
	// Summary: Determines which report item, if any, is at a specified position.
	// Remarks: Call this member function to test a row contents at the 
	//			specified position.
	// Returns: The record item at the specified position, if any, 
	//			or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPReportRecordItem* HitTest(CPoint ptPoint, CRect* pRectItem = NULL, CXTPReportColumn** ppColumn = NULL);

	//-----------------------------------------------------------------------
	// Input:	ptClicked - Position of the cursor, in screen coordinates, 
	//						at the time of the mouse click. 
	// Summary: Process mouse clicks. Called by the framework when the user 
	//			has clicked the mouse button in the row area.
	// Remarks: Call this member function if you want a row to process
	//			a mouse button click message. 
	//			You should provide the method with the coordinates of the 
	//			mouse message to proceed.
	// See also: OnDblClick, OnMouseMove, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnClick(CPoint ptClicked);
	
	//-----------------------------------------------------------------------
	// Input:	ptClicked - Position of the cursor, in screen coordinates, 
	//						at the time of the mouse click. 
	// Summary: Process mouse double clicks. Called by the framework when 
	//			the user has double-clicked the mouse button in the row area.
	// Remarks: Call this member function if you want a row to process
	//			a mouse button double click message. 
	//			You should provide the method with the coordinates of the 
	//			mouse message to proceed.
	// See also: OnClick, OnMouseMove, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnDblClick(CPoint ptClicked);
	
	//-----------------------------------------------------------------------
	// Input:	nFlags - Indicates whether various virtual keys are down.
	//			point  - Point coordinates where the message was fired from.
	// Summary: Process mouse moving.
	// Remarks: Call this member function if you want a row to process
	//			a mouse move message. You should provide the method with the 
	//			coordinates of the mouse message to proceed and a flag
	//			showing which of virtual keys were down.
	// See also: OnClick, OnDblClick, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Input:	ptClick - Position of the cursor, in screen coordinates, 
	//					  at the time of the mouse click. 
	// Summary: Process context menu. Called by the framework when the user 
	//			has clicked the right mouse button (right clicked) in the window.
	// Remarks: Call this member function if you want a row to process
	//			a mouse right button down (context menu call) message. 
	//			You should provide the method with the coordinates of the 
	//			mouse message to proceed.
	// See also: OnClick, OnDblClick, OnMouseMove
	//-----------------------------------------------------------------------
	virtual void OnContextMenu(CPoint ptClick);

	//-----------------------------------------------------------------------
	// Input:	ptTip	- A screen position to show tooltip at.
	//			pTipWnd - Pointer to the specified tooltip window.
	// Summary: Shows a specified tooltip window at the specified position.
	// Remarks: Call this member function if you want to show a report 
	//			tip window for this row.
	// See also: CXTPReportTip overview
	//-----------------------------------------------------------------------
	void ShowToolTip(CPoint ptTip, CXTPReportTip* pTipWnd);

	//-----------------------------------------------------------------------
	// Summary: Returns a pointer to the parent report control.
	// Remarks: Call this member function to retrieve a pointer to the
	//			CXTPReportControl which this item belongs to.
	// Returns: A pointer to an associated report control object.
	// See also: CXTPReportControl overview
	//-----------------------------------------------------------------------
	CXTPReportControl* GetControl() const;
	
	//-----------------------------------------------------------------------
	// Summary:	Returns the parent row of the current row.
	// Remarks: When report rows are arranged as a tree view, this member
	//			function returns a pointer to the parent CXTPReportRow item.
	// Returns: Pointer to the parent row if any, or NULL otherwise.
	// See also: HasParent, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	CXTPReportRow* GetParentRow();
	
	//-----------------------------------------------------------------------
	// Summary: Returns a depth of this row in tree view.
	// Remarks: When report rows are arranged as a tree view, this member
	//			function returns a depth level of the row, which means
	//			the number of parent elements for the row..
	// Returns: A depth of this row in tree view in levels.
	// See also: GetParentRow, HasParent, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	int GetTreeDepth();

	//-----------------------------------------------------------------------
	// Input:	pRow - A row to check.
	// Summary: Determines if a row has a parent in tree view.
	// Remarks: When report rows are arranged as a tree view, this member
	//			function calculates whether this row has a parent row or not.
	// Returns: TRUE if specified row has a parent, FALSE otherwise.
	// See also: HasParent, GetParentRow, GetTreeDepth, GetChilds, AddChild, 
	//			 HasChildren
	//-----------------------------------------------------------------------
	BOOL HasParent(CXTPReportRow* pRow);
	
	//-----------------------------------------------------------------------
	// Summary: Returns a collection of this row children.
	// Returns: CXTPReportRows collection of row children.
	// Remarks: Call this member function to retrieve access to the collection
	//			of row children. 
	// See also: CXTPReportRows overview, GetParentRow, HasParent, 
	//			 GetTreeDepth, AddChild, HasChildren
	//-----------------------------------------------------------------------
	CXTPReportRows* GetChilds();
	
	//-----------------------------------------------------------------------
	// Input	pRow - Pointer to a child CXTPReportRow object.
	// Summary: Adds a child row to the current row.
	// Remarks: Call this member function if you want to add a CXTPReportRow
	//			object as a child of current row in tree view mode.
	// Returns: A pointer to the added CXTPReportRow object.
	// See also: GetParentRow, HasParent, GetTreeDepth, GetChilds
	//-----------------------------------------------------------------------
	CXTPReportRow* AddChild(CXTPReportRow* pRow);
	
	//-----------------------------------------------------------------------
	// Input:	pRow - A row to check.
	// Summary: Determines if a row has children in tree view.
	// Remarks: When report rows are arranged as a tree view, this member
	//			function calculates whether this row has children rows or not.
	// Returns: TRUE if the specified row has children rows, FALSE otherwise.
	// See also: HasParent, GetParentRow, GetTreeDepth, GetChilds, AddChild, 
	//			 HasChildren
	//-----------------------------------------------------------------------
	BOOL HasChildren();

	//-----------------------------------------------------------------------
	// Input:	rcCollapse - The rectangle in report window client coordinates.
	// Summary: Sets coordinates of the collapse/expand button for this row.
	// Remarks: Call this member function to setup the coordinates of the 
	//			collapse/expand button for the row. Note that it could be
	//			different depending on the row tree depth.
	// See also: GetTreeDepth, GetRect
	//-----------------------------------------------------------------------
	void SetCollapseRect(CRect rcCollapse);

	//-----------------------------------------------------------------------
	// Summary: Determines if preview item is visible.
	// Remarks: Call this member function to determine whether preview item
	//			for this row is visible or not.
	// Returns: TRUE if preview item is visible, FALSE otherwise.
	// See also: CXTPReportRecordItemPreview overview, 
	//			 CXTPReportRecord::GetItemPreview
	//-----------------------------------------------------------------------
	BOOL IsPreviewVisible();

	virtual BOOL IsItemsVisible();

	// Summary: Returns coordinates of the row rectangle.
	// Returns: Coordinates of the row rectangle in report window client coordinates.
	// Remarks: Call this member function to determine the coordinates of
	//			the region where this row is drawn on the parent window.
	// See also: SetCollapseRect
	//-----------------------------------------------------------------------
	CRect GetRect();

	//-----------------------------------------------------------------------
	// Input:	pDrawArgs	 - Pointer to the provided draw arguments 
	//						   structure for calculating metrics.
	//			pItemMetrics - Pointer to the metrics structure to fill.
	// Summary: Returns drawing metrics for the row.
	// Remarks: Call this member function when you want to retrieve drawing
	//			metrics.
	// See also: XTP_REPORTRECORDITEM_DRAWARGS overview, 
	//			 XTP_REPORTRECORDITEM_METRICS overview.
	//-----------------------------------------------------------------------
	void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	void FillMetrics(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	int GetGroupLevel();

	CRect GetItemRect(CXTPReportRecordItem* pItem);

	BOOL IsLastTreeRow();

protected:
	//-----------------------------------------------------------------------
	// Input:	nNewIndex - New row numeric index.
	// Summary: Sets new row index in the view.
	// Remarks: Call this member function when you want to change the row 
	//			position in the corresponding row collection.
	//-----------------------------------------------------------------------
	void SetIndex(int nNewIndex);

private:
	void ShiftTreeIndent(CRect& rcItem, CXTPReportColumn* pColumn);

protected:
	CXTPReportRecord* m_pRecord;	// Pointer to the associated CXTPReportRecord object.
	CXTPReportControl* m_pControl;	// Pointer to the parent report control.
	int m_nIndex;					// Item index.
	CRect m_rcRow;					// A rectangle where a row is to be drawn.
	CRect m_rcCollapse;				// A rectangle with coordinates of collapse/expand bitmap.

	CXTPReportRow* m_pParentRow;	// Pointer to the parent row in the tree.
	int m_nRowLevel;				// Row depth level in a tree.
	int m_nGroupLevel;              // Contains row depth level when row is a group row.
	BOOL m_bVisible;				// TRUE if this row should be visible, FALSE when row is hidden.

	CXTPReportRows* m_pChilds;		// A collection of row children.
	BOOL m_bExpanded;				// TRUE when row is expanded in a tree view, FALSE when row is collapsed.
	int m_nPreviewHeight;			// Height of the preview item of this row.

//:Ignore
#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRow);

	afx_msg LPDISPATCH OleGetRecord();
	afx_msg LPDISPATCH OleGetChilds();
	afx_msg LPDISPATCH OleGetParentRow();

public:
	static CXTPReportRow* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPReportRow*)((BYTE*)pDisp - offsetof(CXTPReportRow, m_xDispatch)));
	}

#endif

	friend class CXTPReportControl;	
	friend class CXTPReportSelectedRows;
	friend class CXTPReportRows;
//:End Ignore
};


AFX_INLINE void CXTPReportRow::SetIndex(int nNewIndex) {
	m_nIndex = nNewIndex;
}

AFX_INLINE CXTPReportRecord* CXTPReportRow::GetRecord() const{
	return m_pRecord;
}

AFX_INLINE CXTPReportRow* CXTPReportRow::GetParentRow() {
	return m_pParentRow;
}

AFX_INLINE CXTPReportRows* CXTPReportRow::GetChilds() {
	return m_pChilds;
}

AFX_INLINE CXTPReportControl* CXTPReportRow::GetControl() const {
	return m_pControl;
}

AFX_INLINE void CXTPReportRow::SetCollapseRect(CRect rcCollapse) {
	m_rcCollapse = rcCollapse;
}
AFX_INLINE CRect CXTPReportRow::GetRect() {
	return m_rcRow;
}
AFX_INLINE int CXTPReportRow::GetGroupLevel() {
	return m_nGroupLevel;

}

#endif