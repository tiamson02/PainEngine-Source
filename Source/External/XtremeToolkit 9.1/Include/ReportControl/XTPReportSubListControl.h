// XTPReportSubListControl.h: interface for the CXTPReportSubListControl class.
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
#if !defined(__XTPREPORTSUBLISTCONTROL_H__)
#define __XTPREPORTSUBLISTCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportHeaderDragWnd;
class CXTPReportControl;

/////////////////////////////////////////////////////////////////////////////
// CXTPReportSubListControl window

//===========================================================================
// Remarks: Field chooser list box control.
//			This control allows the user to move column names from associated 
//			CXTPReportControl object from and to its header.
//
//			To use a field chooser in an existing dialog box of your application, 
//			add a standard list box to your dialog template using the dialog
//			editor and then initialize your CXTPReportSubListControl object 
//			inside dialog OnInitialUpdate handler. See example below:
//
//			// CXTPReportSubListControl m_wndSubList;
//			wndSubList.SubclassDlgItem(IDC_COLUMNLIST, &pFieldChooserDialog);
//			// CXTPReportControl wndReport;
//			wndReport.GetColumns()->GetReportHeader()->SetSubListCtrl(&wndSubList);
//
// See also: CDragListBox, CXTPReportControl, CXTPReportHeader, 
//			CXTPReportHeaderDragWnd, ReportControl sample application
//===========================================================================
class _XTP_EXT_CLASS CXTPReportSubListControl : public CDragListBox
{
// Construction
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPReportSubListControl object.
	// Remarks: You construct a CXTPReportSubListControl object in two steps. 
	//			First, call the constructor CXTPReportSubListControl and then call 
	//			CListBox::Create, which initializes the Windows list box and 
	//			attaches it to the CXTPReportSubListControl.
	//
	// Example:	// Declare a local CXTPReportSubListControl object.
	//			CXTPReportSubListControl mySubList;
	//
	//			// Declare a dynamic CXTPReportSubListControl object.
	//			CXTPReportSubListControl* pMySubList = new CXTPReportSubListControl;
	//
	// See also: CListBox, CXTPReportSubListControl::SetReportCtrl, 
	//			CXTPReportHeader::SetSubListCtrl
	//-----------------------------------------------------------------------
	CXTPReportSubListControl();

	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportSubListControl object, handles its cleanup.
	//-----------------------------------------------------------------------
	~CXTPReportSubListControl();

public:
	//-----------------------------------------------------------------------
	// Input:	pReportCtrl - Pointer to the associated report control.
	// Summary: Sets associated report control.
	// Remarks: Usually this method should not be called by user, instead
	//			you should call CXTPReportHeader::SetSubListCtrl.
	// Returns: TRUE if set successfully, FALSE otherwise.
	// See also: CXTPReportHeader::SetSubListCtrl, GetReportCtrl
	//-----------------------------------------------------------------------
	BOOL SetReportCtrl(CXTPReportControl* pReportCtrl);
	
	//-----------------------------------------------------------------------
	// Summary: Returns a pointer to the associated report control.
	// Returns: An associated report control.
	// See also: CXTPReportHeader::SetSubListCtrl, SetReportCtrl
	//-----------------------------------------------------------------------
	CXTPReportControl* GetReportCtrl();

// Operations
public:
	//-----------------------------------------------------------------------
	// Summary: Updates columns list data in field chooser.
	// Remarks: Reloads column list from the associated CXTPReportControl object,
	//			deletes all visible column items from the internal list,
	//			and adds the rest of invisible items.
	// Example: wndSubList.UpdateList();
	// Returns: TRUE when updated successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL UpdateList();

// Overrides

	//-----------------------------------------------------------------------
	// Input:	iSubItem - Sub-item number for including.
	// Summary: Not used for the moment.
	// Returns: Non-zero when completed successfully, zero otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Include(int iSubItem);

	//-----------------------------------------------------------------------
	// Input:	iSubItem - Sub-item number for disabling.
	// Summary: Not used for the moment.
	// Returns: Non-zero when completed successfully, zero otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Disable(int iSubItem);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReportSubListControl)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:

	//-----------------------------------------------------------------------
	// Input:	nItemData - Item data for search.
	// Summary: Finds item index by its data.
	// Returns: Found item index, -1 if item was not found.
	//-----------------------------------------------------------------------
	int FindItemData(int nItemData);

	void MeasureItem(LPMEASUREITEMSTRUCT) {
	}


// Implementation
protected:
	CXTPReportControl* m_pReportCtrl;		// Pointer to the parent report control.

	int m_nSubItem;							// Item index of the dragging column.

	CXTPReportHeaderDragWnd* m_pDragWnd;	// Pointer to the associated dragging window.
	CRect m_rcDragWnd;						// Coordinates of the associated dragging window.
	CRect m_rcDropTarget1;					// Contains first drop target - the list window itself.
	CRect m_rcDropTarget2;					// Contains second drop target - report header.

	int m_nDropIndex;						// Drop item index.

protected:

	//-----------------------------------------------------------------------
	// Input:	pt - Coordinates of the starting drag position (typically, the cursor position). 
	// Summary: Called by the framework when a drag operation starts.
	// Remarks: Override this function if you want to control what happens when 
	//			a drag operation begins. This implementation captures the mouse,
	//			creates CXTPReportHeaderDragWnd and stays in drag mode until the 
	//			user clicks the left or right mouse button or presses ESC, 
	//			at which time the drag operation is canceled.
	// Returns: Nonzero if dragging is allowed, otherwise 0.
	// See also: CDragListBox::BeginDrag
	//-----------------------------------------------------------------------
	BOOL BeginDrag(CPoint pt);

	//-----------------------------------------------------------------------
	// Input:	pt - A CPoint object that contains the x and y screen coordinates of the cursor. 
	// Summary: Called by the framework when an item is being dragged within the 
	//			CDragListBox object.
	// Remarks: This implementation allows dropping items only on the control itself
	//			and associated report control header. 
	//			Override this function if you want to provide additional functionality.
	// Returns: The resource ID of the cursor to be displayed.
	//			The following values are possible: 
	//
	//			[ul]
	//			[li]<b>DL_COPYCURSOR</b>	Indicates that the item will be copied. 
	//			[li]<b>DL_MOVECURSOR</b>	Indicates that the item will be moved. 
	//			[li]<b>DL_STOPCURSOR</b>	Indicates that the current drop target is not acceptable. 
	//			[/ul]
	//
	// See also: CDragListBox::Dragging
	//-----------------------------------------------------------------------
	UINT Dragging(CPoint pt);

	//-----------------------------------------------------------------------
	// Input:	pt - A CPoint object that contains the coordinates of the item being dragged. 
	// Summary: Called by the framework when a drag operation has been canceled.
	// Remarks: Destroys dragging CXTPReportHeaderDragWnd window and resets
	//			associated report control mouse mode.
	// See also: CDragListBox::CancelDrag
	//-----------------------------------------------------------------------
	void CancelDrag(CPoint pt);

	//-----------------------------------------------------------------------
	// Input:	iSrcIndex - Specifies the zero-based index of the dropped string. 
	//			pt - A CPoint object that contains the coordinates of the drop site. 
	// Summary: Called by the framework when an item is dropped within a 
	//			CDragListBox object.
	// Remarks: Destroys dragging CXTPReportHeaderDragWnd window and returns the 
	//			control to the caller.
	// See also: CDragListBox::Dropped
	//-----------------------------------------------------------------------
	void Dropped(int iSrcIndex, CPoint pt);

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReportSubListControl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


#endif
