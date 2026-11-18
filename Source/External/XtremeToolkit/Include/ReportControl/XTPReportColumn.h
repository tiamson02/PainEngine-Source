// CXTPReportColumn.h: interface for the CXTPReportColumn class.
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
#if !defined(__XTPREPORTCOLUMN_H__)
#define __XTPREPORTCOLUMN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore


#include "XTPReportDefines.h"
#include "XTPReportHeader.h"

class CXTPReportGroups;
class CXTPReportRecordItemEditOptions;

//===========================================================================
// Remarks: Represents report column item with its properties and operations.
//          You create object of CXTPReportColumn by calling a constructor and 
//          provide all nessessary parameters. Many of the parameters have default
//          values. Column has 3 properties that greatly influence its behavior:
//          
//			bAutoSize - TRUE allows auto resizing of the width of columns when 
//						ReportControl's client area is resized by the user. If 
//						bAutoSize is set to FALSE, the column keeps its initial 
//						size
//          bSortable - Allows column to be sortable if it has value TRUE. If 
//						you set it to FALSE, you won't be able to sort records 
//						by their column
//          bVisible  - Determines if columns will draw by control or not. For 
//						example, if the column is dragged into FieldChooser, 
//						the column still exists in the columns list, but it
//						becomes invisible.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportColumn : public CCmdTarget
{
	friend class CXTPReportControl;
	friend class CXTPReportColumns;
	friend class CXTPReportHeader;
	friend class CXTPReportRows;
	friend class CXTPReportRow;
	friend class CXTPReportColumnOrder;

protected:
	//-----------------------------------------------------------------------
	// Summary: Default constructor, could be accessible only by the Column 
	//          and its friends.
	//-----------------------------------------------------------------------
	CXTPReportColumn();

public:
	//-----------------------------------------------------------------------
	// Input:   strName    - Column name.
	//          nItemIndex - An index of the corresponding record item.
	//			nWidth     - An initial width of the created column.
	//			nMinWidth  - Minimal width of the column.
	//			nIconID    - Number of the corresponding icon in the image list.
	//			bAutoSize  - Is column resizable flag.
	//			bSortable  - Is column sortable flag.
	//			bVisible   - Is column visible flag.
	// Summary: Constructs a CXTPReportColumn object.
	// Remarks: There are 3 parameters that greatly influence a column's behavior:
	//			bAutoSize  - TRUE allows auto resizing of width of columns when 
	//						 the ReportControl's client area is resized by the 
	//						 user. If bAutoSize is set to FALSE, the column 
	//						 keeps its initial size
	//          bSortable  - Allows the column to be sortable if has value TRUE. 
	//						 If you set it to FALSE, you won't be able to sort 
	//						 records by their column
	//          bVisible   - Determines if columns will draw by control or not. 
	//						 For exampe if the column is dragged into FieldChooser, 
	//						 the column still exists in the columns list,
	//						 but it becomes invisible.
	// Example: 
	//			#define COLUMN_ICON 1
	//			#define COLUMN_MAIL_ICON 1
	//
	//			CXTPReportControl wndReport;
	//          wndReport.AddColumn(new CXTPReportColumn(COLUMN_ICON, _T("Message Class"), 18, FALSE, COLUMN_MAIL_ICON));
	//-----------------------------------------------------------------------
	CXTPReportColumn(int nItemIndex, CString strName, int nWidth, BOOL bAutoSize = TRUE, UINT nIconID = XTP_REPORT_NOICON , BOOL bSortable = TRUE, BOOL bVisible = TRUE);


	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPReportColumn object, handles cleanup and 
	//          de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPReportColumn(void);

	//-----------------------------------------------------------------------
	// Summary:  Returns current column header drag mode.
	// Remarks:  ReportColumn may be moved (change order of columns, delete columns from
	//           header, move column to FieldChooser) using drag and drop. When column
	//           is moved, it is in Dragged mode. You use IsDragging() to determine if
	//           columns are in Dragged mode
	// Returns:  TRUE if the column header is in dragging mode, FALSE otherwise.
	// See Also: IsAllowDragging()
	//-----------------------------------------------------------------------
	BOOL IsDragging();


	//-----------------------------------------------------------------------
	// Summary: Returns the last drawn column rect.
	// Remarks: Each time when the column is drawing, its rectangle is recalculated
	//          to fit in size after ReportControl resizing, Column resizing 
	//          or moving, etc. You use GetRect() to get the latest rectangle 
	//			sizes.
	// Returns: The client rectangle where the column was drawn last time.
	//-----------------------------------------------------------------------
	CRect GetRect();

	//-----------------------------------------------------------------------
	// Summary: Returns column width.
	// Remarks: Each time when the column is drawing, its rectangle is recalculated
	//          to fit in size after ReportControl resizing, Column resizing 
	//          or moving, etc. You use GetWidth() to get the latest rectangle 
	//			width.
	// Returns: The current column width in logical units.
	//-----------------------------------------------------------------------
	int GetWidth();

	//-----------------------------------------------------------------------
	// Input:	pDC - Device context to calculate width at.
	// Summary: Calculates caption length.
	// Remarks: This function calculates caption text length in logical units
	//          using the given device context, current caption font, and string 
	//          of text. 
	// Returns: Caption length in pixels.
	//-----------------------------------------------------------------------
	int GetCaptionWidth(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	nNewWidth - New column width in pixels.
	// Summary: Sets column width.
	// Remarks: This member function gives you possibility to change column width
	//          manually. To set initial width or set width for columns which
	//          AutoSize = FALSE.  
	// Returns: Previous column width in pixels.
	//-----------------------------------------------------------------------
	int SetWidth(int nNewWidth);

	//-----------------------------------------------------------------------
	// Summary: Returns column indentation.
	// Returns: Previous column indentation in pixels.
	//-----------------------------------------------------------------------
	int GetIndent();

	//-----------------------------------------------------------------------
	// Summary: Returns the index of the associated record item.
	// Returns:	The index of the associated record item.
	//-----------------------------------------------------------------------
	int GetItemIndex();

	//-----------------------------------------------------------------------
	// Summary: Returns the index of the column.
	// Returns:	The index of the column.
	//-----------------------------------------------------------------------
	int GetIndex();

	//-----------------------------------------------------------------------
	// Summary: Returns the ID of the associated column header icon.
	// Remarks: Column can have an image that will be drawn on it. You use
	//          GetIconID() to get the current image id which passes to the 
	//			column ID of the column. It must match to one of the ids in the 
	//			imagelist
	// Returns:	The ID of the associated column header icon.
	//-----------------------------------------------------------------------
	UINT GetIconID();

	//-----------------------------------------------------------------------
	// Summary: Returns the text caption for the column header.
	// Remarks: You use GetCaption() to get the current column caption. 
	// Returns:	The text caption for the column header.
	//-----------------------------------------------------------------------
	CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:  Returns the current column header visibility mode.
	// Returns:	 TRUE if the column header is visible, FALSE otherwise.
	// See Also: SetVisible(BOOL);
	//-----------------------------------------------------------------------
	BOOL IsVisible();

	//-----------------------------------------------------------------------
	// Input:	 bVisible - New column visible state.
	// Summary:  Sets column visible mode (TRUE - visible, FALSE - invisible).
	// Remarks:  When you move a column to FieldChooser, it becames invisible. 
	//           To change visibility of column you use SetVisible(BOOL)
	// Returns:	 Previous column visible state.
	// See Also: IsVisible()
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible = TRUE);

	//-----------------------------------------------------------------------
	// Summary:  Returns the current column header filtrability mode.
	// Returns:	 TRUE if the column header is filtrable, FALSE otherwise.
	// See Also: SetFiltrable(BOOL)
	//-----------------------------------------------------------------------
	BOOL IsFiltrable();

	//-----------------------------------------------------------------------
	// Input:	 bFiltrable - New column filtrable state.
	// Summary:  Sets column filtrable mode (TRUE - filtrable, FALSE - infiltrable).
	// Returns:	 Previous column filtrable state.
	// See Also: IsFiltrable()
	//-----------------------------------------------------------------------

	BOOL SetFiltrable(BOOL bFiltrable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:  Returns column allow resizing state.
	// Returns:  TRUE if the column allows resizing, FALSE otherwise.
	// See Also: EnableResize(BOOL bIsResizable)
	//-----------------------------------------------------------------------
	BOOL IsResizable();

	//-----------------------------------------------------------------------
	// Input:	 bIsResizable - TRUE if column will be resizable, FALSE otherwise.
	// Summary:  Enable/Disables the columnn's allow resizing mode.
	// See Also: IsResizable()
	//-----------------------------------------------------------------------
	void EnableResize(BOOL bIsResizable);

	//-----------------------------------------------------------------------
	// Input:	 nMinWidth - New column minimal width value.
	// Summary:  Sets minimal width value for column.
	// Remarks:  After you set the minmal width for the column, it cannot be 
	//           resizable to less than minimal width. 
	// Returns:  Previous column minimal width value.
	// See Also: GetMinWidth()
	//-----------------------------------------------------------------------
	int SetMinWidth(int nMinWidth);

	//-----------------------------------------------------------------------
	// Summary:  Returns the current minimal column width.
	// Returns:  Column minimal width value in pixels.
	// See Also: SetMinWidth(int)
	//-----------------------------------------------------------------------
	int GetMinWidth();

	//-----------------------------------------------------------------------
	// Summary:  Returns the column sortable flag.
	// Returns:  TRUE if column is sortable, FALSE otherwise.
	// See Also: IsSorted(), IsSortedIncreasing(), IsSortedDecreasing()
	//-----------------------------------------------------------------------
	BOOL IsSortable();

	//-----------------------------------------------------------------------
	// Summary: Returns the column groupable flag.
	// Remarks: Not all columns allow for grouping. You use IsGroupable()
	//          to determine if the column is groupable.
	// Returns: TRUE if the column is groupable, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsGroupable();

	//-----------------------------------------------------------------------
	// Summary:  Returns the column dragging allowance flag.
	// Returns:  TRUE if the column is draggable, FALSE otherwise.
	// See Also: IsDragging()
	//-----------------------------------------------------------------------
	BOOL IsAllowDragging();
	
	//-----------------------------------------------------------------------
	// Summary:  Determines if the column is sorted in the increasing order.
	// Returns:  TRUE if the column is sorted in increasing order, FALSE otherwise.
	// See Also: IsSorted(), IsSortable(), IsSortedDecreasing()
	//-----------------------------------------------------------------------
	BOOL IsSortedIncreasing();
	
	//-----------------------------------------------------------------------
	// Summary:  Determines if the column is sorted in the decreasing order.
	// Returns:  TRUE if the column is sorted in decreasing order, FALSE otherwise.
	// See Also: IsSorted(), IsSortable(), IsSortedIncreasing()
	//-----------------------------------------------------------------------
	BOOL IsSortedDecreasing();

	void SetSortIncreasing(BOOL bSortIncreasing);

	//-----------------------------------------------------------------------
	// Summary:  Determines if the column is sorted.
	// Returns:  TRUE if the column is sorted, FALSE otherwise.
	// Remarks:  If the records list is sorted by given column IsSorted() returns 
	//           TRUE, otherwise - FALSE 
	// See Also: IsSortable(), IsSortedIncreasing(), IsSortedDecreasing()
	//-----------------------------------------------------------------------
	BOOL IsSorted();

	//-----------------------------------------------------------------------
	// Input:    bIsTreeColumn - TRUE if this column contains tree items 
	//							 (collapse/expand buttons) drawn inside, 
	//							 FALSE otherwise.
	// Summary:	 Makes/unmakes the current column as containing a tree inside.
	// See Also: IsTreeColumn()
	//-----------------------------------------------------------------------
	void SetTreeColumn(BOOL bIsTreeColumn);

	//-----------------------------------------------------------------------
	// Summary:	 Returns TRUE if the current column contains tree.
	// Returns:  TRUE if the current column contains tree items, FALSE otherwise.
	// See Also: SetTreeColumn(BOOL)
	//-----------------------------------------------------------------------
	BOOL IsTreeColumn();

	//-----------------------------------------------------------------------
	// Input:	ar - Archive stream to write the column data to.
	// Summary: Serializes the column data and state to/from the provided archive 
	//			stream.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);

	//-----------------------------------------------------------------------
	// Input:	nAlignment - New alignment value.
	//			Possible alignment values: DT_LEFT, DT_CENTER, DT_RIGHT.
	//			See also DrawText() Format Flags.
	// Summary: Sets new Alignment value for column.
	// Returns: Previous alignment column value.
	//-----------------------------------------------------------------------
	int SetAlignment(int nAlignment);

	//-----------------------------------------------------------------------
	// Summary: Returns the current alignment value for the column.
	// Returns: Current column alignment value.
	//			The result is one of the following: DT_LEFT, DT_CENTER, DT_RIGHT.
	//-----------------------------------------------------------------------
	int GetAlignment();

	//-----------------------------------------------------------------------
	// Input:	bDrawExternal - TRUE when drawing column header outside the 
	//							report header, FALSE when inside.
	// Summary: Checks if the column header should have the sort triangle drawn.
	// Returns: TRUE if the sort triangle should be drawn for this column, FALSE 
	//			otherwise.
	//-----------------------------------------------------------------------
	BOOL HasSortTriangle();

	//-----------------------------------------------------------------------
	// Summary: Call this method to set the column auto sized.
	// Returns: TRUE if the column is automatically sized.
	//-----------------------------------------------------------------------
	BOOL IsAutoSize();

	void SetSortable(BOOL bSortable);
	void SetGroupable(BOOL bGroupable);

	CXTPReportColumns* GetColumns();

	CXTPReportControl* GetControl();

	BOOL IsShowInFieldChooser();
	void SetShowInFieldChooser(BOOL bShow);

	CXTPReportRecordItemEditOptions* GetEditOptions();
	
	BOOL IsEditable();
	void SetEditable(BOOL bEditable = TRUE);



protected:
	CString m_strName;					// Column name.
	int m_nItemIndex;					// Column index.

	BOOL m_bSortIncreasing;				// Stores column sort direction (used when m_bSortable is TRUE).
	BOOL m_bSortable;					// Determines if the column could be sortable by.
	BOOL m_bGroupable;					// Determines if the column could be groupped by.

	BOOL m_bAllowDrag;					// Allow/disallow dragging of the column

	CXTPReportColumns* m_pColumns;		// Stores a pointer to the parent columns collection.

	UINT m_nIconID;						// Associated icon ID. 
	BOOL m_bVisible;					// Visible state.
	BOOL m_bFiltrable;					// Is filtering by this column allowed?.	

	BOOL m_bIsResizable;				// Indicates if the column allows width resizing 	
	CRect m_rcColumn;					// Saves rectangle where column was drawn last time.
	CRect m_rcGroupBy;					// Saves rectangle where column was drawn in group box.
	int m_nMinWidth;					// Minimal column width. It is not allowed to do it less then minimal.
	int m_nMaxItemWidth;				// Contains maximal width of the corresponding item drawn in the column. 
	int m_nIndent;						// Width indent at the left of the column.
	int m_nAlignment;                   // Alignment mode for column: DT_LEFT || DT_RIGHT || DT_CENTER 

	int m_nColumnAutoWidth;				// The column a  automatic width
	int m_nColumnStaticWidth;			// The user defined with.
	BOOL m_bAutoSize;					// TRUE if column autosized.

	BOOL m_bShowInFieldChooser;
	BOOL m_bEditable;

	CXTPReportRecordItemEditOptions* m_pEditOptions;


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportColumn);

	static CXTPReportColumn* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPReportColumn*)((BYTE*)pDisp - offsetof(CXTPReportColumn, m_xDispatch)));
	}
	LPDISPATCH OleGetEditOptions();

#endif
};


AFX_INLINE CString CXTPReportColumn::GetCaption() {
	return m_strName;
}

AFX_INLINE UINT CXTPReportColumn::GetIconID() {
	return m_nIconID;
}


AFX_INLINE int CXTPReportColumn::SetMinWidth(int nMinWidth) {
	int OldMinWidth = m_nMinWidth;
	m_nMinWidth = nMinWidth;
	return OldMinWidth;
}


AFX_INLINE BOOL CXTPReportColumn::IsFiltrable() {
	return m_bFiltrable;
}

AFX_INLINE BOOL CXTPReportColumn::SetFiltrable(BOOL bFiltrable) {
	BOOL bOldFiltrable = m_bFiltrable; m_bFiltrable = bFiltrable; return bOldFiltrable;
}

AFX_INLINE BOOL CXTPReportColumn::IsResizable() {
	return m_bIsResizable;
}

AFX_INLINE void CXTPReportColumn::EnableResize(BOOL bIsResizable) {
	m_bIsResizable = bIsResizable;
}


AFX_INLINE BOOL CXTPReportColumn::IsSortable() {
	return m_bSortable;
}

AFX_INLINE BOOL CXTPReportColumn::IsGroupable() {
	return m_bGroupable;
}

AFX_INLINE BOOL CXTPReportColumn::IsAllowDragging() {
	return m_bAllowDrag;
}

AFX_INLINE int CXTPReportColumn::SetAlignment(int nAlignment) {
	int nOld = m_nAlignment; m_nAlignment = nAlignment; return nOld;
}

AFX_INLINE int CXTPReportColumn::GetAlignment() {
	return m_nAlignment;
}
AFX_INLINE BOOL CXTPReportColumn::IsAutoSize() {
	return m_bAutoSize;
}
AFX_INLINE void CXTPReportColumn::SetSortable(BOOL bSortable) {
	m_bSortable = bSortable;
}
AFX_INLINE void CXTPReportColumn::SetGroupable(BOOL bGroupable) {
	m_bGroupable = bGroupable;
}
AFX_INLINE CXTPReportColumns* CXTPReportColumn::GetColumns() {
	return m_pColumns;
}
AFX_INLINE void CXTPReportColumn::SetSortIncreasing(BOOL bSortIncreasing)  {
	m_bSortIncreasing = bSortIncreasing;
}
AFX_INLINE CXTPReportRecordItemEditOptions* CXTPReportColumn::GetEditOptions(){
	return m_pEditOptions;
}
AFX_INLINE BOOL CXTPReportColumn::IsEditable() {
	return m_bEditable;
}
AFX_INLINE void CXTPReportColumn::SetEditable(BOOL bEditable /*= TRUE*/) {
	m_bEditable = bEditable;
}


#endif