// XTTreeBase.h interface for the CXTTreeBase class.
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
#if !defined(__XTTREEBASE_H__)
#define __XTTREEBASE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks: CXTTreeBase is a CTreeView derived class. It extends the CTreeView
//			class to add additional functionality, including tree traversal, searching,
//			color, and settings.
//===========================================================================
class _XT_EXT_CLASS CXTTreeBase
{

protected:

    //-----------------------------------------------------------------------
	// Summary:	Protected constructor used by dynamic creation. Constructs a CXTTreeBase
	//          object
	//-----------------------------------------------------------------------
	CXTTreeBase();

	//-----------------------------------------------------------------------
    // Summary:	Destroys a CXTTreeBase object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTTreeBase();

protected:
//:Ignore - Needed in order to access protected functions.
    class CTreeCtrl_Friendly : public CTreeCtrl { friend class CXTTreeBase; };
//:End Ignore


	BOOL			m_bMultiSelect; // TRUE for a multi-selection tree control.
	BOOL			m_bBandLabel;   // TRUE to include the label when selecting tree items.
	HTREEITEM		m_hSelect;      // For shift selection.
	HTREEITEM       m_htiEdit;      // Tree item that is currently edited.
	HTREEITEM       m_htiLast;      // Tree item that was last had the mouse over.
    CXTColorFontMap m_mapColorFont; // Struct that contains the color and logfont for the tree item.
	CTreeCtrl_Friendly* m_pTreeCtrl;

private:

    bool m_bActionDone;
	bool m_bTreeNotify;
	bool m_bOkToEdit;

public:

	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	hItem -  Handle of a tree item.
	//			nCode - A flag indicating the type of relation to 'hItem'. This flag can be
	//					one of the following values:
    //			[ul]
    //			[li]<b>TVGN_CARET</b> Retrieves the currently selected item.[/li]
    //			[li]<b>TVGN_CHILD</b> Retrieves the first child item. The 'hItem'
	//			parameter <b>must</b> be NULL.[/li]
    //			[li]<b>TVGN_DROPHILITE</b> Retrieves the item that is the target
	//			of a drag-and-drop operation.[/li]
    //			[li]<b>TVGN_FIRSTVISIBLE</b> Retrieves the first visible item.[/li]
    //			[li]<b>TVGN_NEXT</b> Retrieves the next sibling item.[/li]
    //			[li]<b>TVGN_NEXTVISIBLE</b> Retrieves the next visible item
	//			that follows the specified item.[/li]
    //			[li]<b>TVGN_PARENT</b> Retrieves the parent of the specified
	//			item.[/li]
    //			[li]<b>TVGN_PREVIOUS</b> Retrieves the previous sibling item.[/li]
    //			[li]<b>TVGN_PREVIOUSVISIBLE</b> Retrieves the first visible
	//			item that precedes the specified item.[/li]
    //			[li]<b>TVGN_ROOT</b> Retrieves the first child item of the
	//			root item of which the specified item is a part.[/li]
    //			[/ul]
    // Summary:	This member function will retrieve the tree view item that has the
	//			specified relationship, indicated by the 'nCode' parameter, to 'hItem'.
	// Returns: The handle of the next item if successful, otherwise returns
	//			NULL.
	//-----------------------------------------------------------------------
    virtual HTREEITEM GetNextItem(HTREEITEM hItem,UINT nCode) const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function gets the next item as if the outline was completely
	//			expanded. 
	// Returns: The handle of the item immediately below the reference item.
	//-----------------------------------------------------------------------
    virtual HTREEITEM GetNextItem(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function gets the previous item as if the outline was
	//			completely expanded. 
	// Returns: The handle of the item immediately above the reference item.
	//-----------------------------------------------------------------------
    virtual HTREEITEM GetPrevItem(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Node identifying the branch. NULL will return the last item in the
	//					outline.
    // Summary:	This member function retrieves the last item in the branch. 
	// Returns: The handle of the last item.
	//-----------------------------------------------------------------------
    virtual HTREEITEM GetLastItem(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
	// Input:	lpszSearch	   - String to search for.
	//			bCaseSensitive - TRUE if the search should be case sensitive.
	//			bDownDir	   - TRUE for down.
	//			bWholeWord	   - TRUE if search should match whole words.
	//			hItem		   - Handle of the tree item to start searching from, NULL to use
	//			the currently selected tree item.
    // Summary:	This member function searches the entire tree for an item label 
	//			that contains the search string.
	// Returns: The handle to the item, or returns NULL.
	//-----------------------------------------------------------------------
    virtual HTREEITEM FindItem(LPCTSTR lpszSearch,BOOL bCaseSensitive=FALSE,BOOL bDownDir=TRUE,BOOL bWholeWord=FALSE,HTREEITEM hItem=NULL);

	//-----------------------------------------------------------------------
	// Input:	lpszSearch	   - String to search for.
	//			bCaseSensitive - TRUE if the search should be case sensitive.
	//			bWholeWord	   - TRUE if search should match whole words.
	//			hItem		   - Handle of the tree item to start searching from, NULL to use
	//							 the currently selected tree item.
    // Summary:	This member function searches the entire branch specified by htiItem for an item label 
    //          that contains the search string.
	// Returns: The handle to the item, or returns NULL.
	//-----------------------------------------------------------------------
    virtual HTREEITEM FindItemInBranch(LPCTSTR lpszSearch, BOOL bCaseSensitive /*= FALSE*/, BOOL bWholeWord /*= FALSE*/, HTREEITEM htiItem /*= NULL*/);

	//-----------------------------------------------------------------------
	// Input:	hItem   - Handle of the reference item.
	//			logfont - New font for the tree item.
    // Summary:	This member function sets the font for the reference tree item.
	//-----------------------------------------------------------------------
    virtual void SetItemFont(HTREEITEM hItem,LOGFONT& logfont);

	//-----------------------------------------------------------------------
	// Input:	hItem    - Handle of the reference item.
	//			plogfont - Pointer to receive LOGFONT information.
    // Summary:	This member function retrieves the current LOGFONT font used by
	//			the tree item. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL GetItemFont(HTREEITEM hItem,LOGFONT* plogfont);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
	//			bBold - TRUE for bold font.
    // Summary:	This member function sets the reference tree item font to bold.
	//-----------------------------------------------------------------------
    virtual void SetItemBold(HTREEITEM hItem,BOOL bBold=TRUE);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function checks whether or not the tree item has a bold font.
	// Returns: TRUE if the tree item has a bold font, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL GetItemBold(HTREEITEM hItem);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
	//			color - RGB value for the tree item's text.
    // Summary:	This member function sets the tree item text color.
	//-----------------------------------------------------------------------
    virtual void SetItemColor(HTREEITEM hItem,COLORREF color);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function returns the RGB value for the referenced
	//			tree item text or (COLORREF)-1, if color was not set. 
	// Returns: The RGB value for the referenced tree item text, or (COLORREF)-1, if the 
	//			color was not set.
	//-----------------------------------------------------------------------
    virtual COLORREF GetItemColor(HTREEITEM hItem);

	//-----------------------------------------------------------------------
	// Input:	bMultiSelect - TRUE for a multi-selection tree control.
    // Summary:	This member function enables or disables multi-selection for the
	//			tree control. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL EnableMultiSelect(BOOL bMultiSelect=TRUE);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function returns the previously selected tree item in
	//			a multi-selection tree control. 
	// Returns: A handle to the previously selected tree item.
	//-----------------------------------------------------------------------
    virtual HTREEITEM GetPrevSelectedItem(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of the reference item.
    // Summary:	This member function returns the next selected item in a multi-selection
	//			tree control. 
	// Returns: A handle to the next selected tree item.
	//-----------------------------------------------------------------------
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
    // Summary:	This member function returns the first selected item in a multi-selection
	//			tree control. 
	// Returns: A handle to the first selected tree item.
	//-----------------------------------------------------------------------
	HTREEITEM GetFirstSelectedItem() const;

	//-----------------------------------------------------------------------
    // Summary:	This member function returns the number of tree items that are selected.
	// Returns: A UNIT value that represents the number of tree items selected.
	//-----------------------------------------------------------------------
	UINT GetSelectedCount() const;
	
	//-----------------------------------------------------------------------
	// Input:	hItem	   - Handle of the item whose state is to be set.
	//			nState	   - Specifies the new states for the item.
	//			nStateMask - Specifies which states are to be changed.
	// Summary:	This member function is a replacement for the base class function of
	//			the same name, to handle TVIS_FOCUSED in a multi-select tree control.
	//			It sets the state of the item specified by 'hItem'.  
	// Returns: Nonzero if successful, otherwise returns zero.
	//-----------------------------------------------------------------------
	BOOL SetItemState(HTREEITEM hItem,UINT nState,UINT nStateMask);

	//-----------------------------------------------------------------------
	// Input:	hItem	   - Handle of the item whose state is to be retrieved.
	//			nStateMask - Mask indicating which states are to be retrieved. For more information
	//						 on possible values for 'nStateMask', see the discussion of the 'state'
	//						 and 'stateMask' members of the TVITEM structure in the Platform SDK.
	// Summary:	This member function is a replacement for the base class function of
	//			the same name to handle TVIS_FOCUSED in a multi-select tree control.
	// Returns: The state of the item specified by 'hItem'.
	//-----------------------------------------------------------------------
	UINT GetItemState(HTREEITEM hItem,UINT nStateMask) const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of a tree item.
	// Summary:	This member function is a replacement for the base class function of
	//			the same name, to handle TVIS_FOCUSED in a multi-select tree control.
	//			Call this function to select the given tree view item. If 'hItem' is
	//			NULL, then no items are selected.
	// Returns: Nonzero if successful, otherwise returns zero.
	//-----------------------------------------------------------------------
	BOOL SelectItem(HTREEITEM hItem);

	//-----------------------------------------------------------------------
	// Summary:	This member function returns the handle to the tree item that currently
	//			has focus. 
	// Returns: The handle of the item that has focus, otherwise returns NULL.
	//-----------------------------------------------------------------------
	HTREEITEM GetFocusedItem() const;

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of a tree item.
	// Summary:	This member function will set the focus for the tree item specified
	//			by 'hItem'.  
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL FocusItem(HTREEITEM hItem);

	//-----------------------------------------------------------------------
	// Input:	bSelect - TRUE to select all the items, or FALSE to clear the selection.
	//          htItem  - Tree item to start selection from. If NULL selection will begin at the root.
	// Summary:	Call this member function to clear, or select, all of the visible items
	//			in the tree control. This will not effect the focus of the tree items.
	//-----------------------------------------------------------------------
	void SelectAll(BOOL bSelect = TRUE, HTREEITEM htItem=NULL);

	//-----------------------------------------------------------------------
	// Input:	hItemFrom - Handle of the item to start selecting from.
	//			hItemTo   - Handle of the item to end selection at.
	//			bOnly     - TRUE to only select the specified range, or FALSE to keep existing
	//						selections.
    // Summary:	This member function selects items from 'hItemFrom' to 'hItemTo' in
	//			a multi-selection tree control. It does not select a child item if
	//			the parent is collapsed. It will remove the selection from all other
	//			items if 'bOnly' is set to TRUE.
	//-----------------------------------------------------------------------
	void SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo,BOOL bOnly = TRUE);

	//-----------------------------------------------------------------------
	// Input:	hItem - Handle of a tree item.
	// Summary:	This member function checks whether or not the specified item is selected.
	// Returns: TRUE if the specified item is selected, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsSelected(HTREEITEM hItem) const;

	//-----------------------------------------------------------------------
	// Input:	hParent  - Handle of tree item to begin selection from.
	//			bSelect  - TRUE to select only the child items or FALSE to keep existing selections.
	//			bRecurse - TRUE to recurse all siblings or FALSE to select only children of the
	//					   parent item.
	// Summary:	This member function will cause all of the children of the specified
	//			tree item to be selected or deselected. 
	// Returns: TRUE if focus was on a child item, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SelectChildren(HTREEITEM hParent,BOOL bSelect = TRUE,BOOL bRecurse = TRUE);

	//-----------------------------------------------------------------------
	// Input:	list - Reference to a CTypedPtrList<CPtrList, HTREEITEM> object.
	// Summary:	This member function will retrieve a reference to the typed pointer
	//			array that contains the items selected in the tree control.
	//-----------------------------------------------------------------------
	void GetSelectedList(CTypedPtrList<CPtrList, HTREEITEM>& list) const;

	//-----------------------------------------------------------------------
	// Summary:	This member function checks to see if the tree control is a multi-select
	//			tree. 
	// Returns: TRUE if the tree control is a multi-select tree, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsMultiSelect() const;

	//-----------------------------------------------------------------------
	// Input:	bLabel - TRUE to select items only when banding rect passes over the text label,
	//					 or FALSE to select items when banding rect passes over any part of
	//					 the tree item.
	// Summary:	This member function sets the banding mode for a multi-selection tree
	//			control. If 'bLabel' is TRUE, then items are selected only when the
	//			banding rect passes over the tree item label. If FALSE, passing
	//			over any part of the tree item will cause selection to be made when
	//			the banding rect passes over it.  
	// Returns: The previous banding state.
	//-----------------------------------------------------------------------
	BOOL SetBandingHit(BOOL bLabel);

	//-----------------------------------------------------------------------
	// Summary:	Call this member function to determine if the WM_NOTIFY message that was
	//			sent was sent by the tree control or the framework.  
	// Returns: true if the tree control sent the WM_NOTIFY message, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsTreeNotify();

protected:

//:Ignore
	//{{AFX_VIRTUAL(CXTTreeBase)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
//:End Ignore

//:Ignore
	//Internaly used.
	virtual LRESULT SendNotify(LPNMHDR pNMHDR);
	virtual void SelectAllIgnore(BOOL bSelect, HTREEITEM hIgnore);
	virtual BOOL OnButtonDown(BOOL bLeft, UINT nFlags, CPoint point);
	virtual void DoPreSelection(HTREEITEM hItem, BOOL bLeft, UINT nFlags);
	virtual void DoAction(HTREEITEM hItem, BOOL bLeft, UINT nFlags, CPoint point);
	virtual void DoBanding(UINT nFlags, CPoint point);
	virtual void UpdateSelectionForRect(LPCRECT pRect, UINT nFlags, CTypedPtrList<CPtrList, HTREEITEM>& list);
	virtual BOOL IsFindValid(HTREEITEM hti);
	virtual void DoPaint(CDC& dc, BOOL bInternal = TRUE);
	COLORREF GetTreeBackColor() const;
	COLORREF GetTreeTextColor() const;
	COLORREF GetItemBackColor(UINT uState, bool bTreeHasFocus, DWORD dwStyle, COLORREF crBack) const;
	COLORREF GetItemTextColor(UINT uState, bool bTreeHasFocus, DWORD dwStyle, COLORREF crText) const;
	bool HasEditLabels() const;

//:End Ignore

protected:

	//:Ignore
	//{{AFX_MSG(CXTTreeBase)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	//:End Ignore
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE HTREEITEM CXTTreeBase::GetNextItem( HTREEITEM hItem, UINT nCode ) const {
    ASSERT(::IsWindow(m_pTreeCtrl->m_hWnd)); return m_pTreeCtrl->GetNextItem( hItem, nCode );
}
AFX_INLINE HTREEITEM CXTTreeBase::GetFocusedItem() const {
	ASSERT(m_bMultiSelect); return m_pTreeCtrl->GetSelectedItem();
}
AFX_INLINE BOOL CXTTreeBase::IsSelected(HTREEITEM hItem) const {
	return !!(TVIS_SELECTED & m_pTreeCtrl->GetItemState(hItem, TVIS_SELECTED));
}
AFX_INLINE BOOL CXTTreeBase::IsMultiSelect() const {
	return m_bMultiSelect;
};
AFX_INLINE BOOL CXTTreeBase::SetBandingHit(BOOL bLabel) {
	BOOL bReturn = m_bBandLabel; m_bBandLabel = bLabel; return bReturn;
};
AFX_INLINE bool CXTTreeBase::IsTreeNotify() {
	return m_bTreeNotify;
}

#define DECLATE_TREE_BASE(ClassName, Tree, Base)\
class _XT_EXT_CLASS ClassName : public Tree, public Base\
{\
public:\
	UINT GetItemState(HTREEITEM hItem,UINT nStateMask) const  {\
		return Base::GetItemState(hItem, nStateMask);\
	}\
	BOOL SelectItem(HTREEITEM hItem) {\
		return Base::SelectItem(hItem);\
	}\
	BOOL SetItemState(HTREEITEM hItem,UINT nState,UINT nStateMask) {\
		return Base::SetItemState(hItem, nState, nStateMask);\
	}\
	HTREEITEM GetNextItem(HTREEITEM hItem,UINT nCode) const {\
		return Base::GetNextItem(hItem, nCode);\
	}\
protected:\
	void OnLButtonDown(UINT nFlags, CPoint point) {\
		Base::OnLButtonDown(nFlags, point);\
	}\
	void OnRButtonDown(UINT nFlags, CPoint point) {\
		Base::OnRButtonDown(nFlags, point);\
	}\
	void OnSetFocus(CWnd* pOldWnd) {\
		Base::OnSetFocus(pOldWnd);\
	}\
	void OnKillFocus(CWnd* pNewWnd) {\
		Base::OnKillFocus(pNewWnd);	\
	}\
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {\
		Base::OnKeyDown(nChar, nRepCnt, nFlags);\
	}\
	BOOL OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {\
		return Base::OnBeginLabelEdit(pNMHDR, pResult);	\
	}\
	BOOL OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) {\
		return Base::OnEndLabelEdit(pNMHDR, pResult);\
	}\
	BOOL OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) {\
		return Base::OnItemExpanding(pNMHDR, pResult);\
	}\
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) {\
		return Base::OnNotify(wParam, lParam, pResult);\
	}	\
	BOOL PreTranslateMessage(MSG* pMsg) {\
		return Base::PreTranslateMessage(pMsg);\
	}	\
	BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) {\
		return Base::OnDeleteItem(pNMHDR, pResult);\
	}	\
	void OnSize(UINT nType, int cx, int cy) {\
		Base::OnSize(nType, cx, cy);\
	}	\
	void OnMouseMove(UINT nFlags, CPoint point) {	\
		Base::OnMouseMove(nFlags, point);\
	}	\
	void OnTimer(UINT nIDEvent) {		\
		Base::OnTimer(nIDEvent);\
	}	\
	void OnNcMouseMove(UINT nHitTest, CPoint point) {\
		Base::OnNcMouseMove(nHitTest, point);\
	}\
	BOOL OnEraseBkgnd(CDC*) {\
		return TRUE;\
	}\
	void OnPaint() {\
		CPaintDC dc(this);\
		DoPaint(dc); \
	}\
};

#define ON_TREECTRL_REFLECT\
	ON_WM_LBUTTONDOWN()\
	ON_WM_SETFOCUS()\
	ON_WM_KILLFOCUS()\
	ON_WM_RBUTTONDOWN()\
	ON_WM_KEYDOWN()\
	ON_WM_ERASEBKGND()\
	ON_WM_PAINT()\
	ON_WM_SIZE()\
	ON_NOTIFY_REFLECT_EX(TVN_DELETEITEM, OnDeleteItem)\
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemExpanding)\
	ON_NOTIFY_REFLECT_EX(TVN_BEGINLABELEDIT, OnBeginLabelEdit)\
	ON_NOTIFY_REFLECT_EX(TVN_ENDLABELEDIT, OnEndLabelEdit)\
	ON_WM_MOUSEMOVE()\
	ON_WM_TIMER()\
	ON_WM_NCMOUSEMOVE

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTTREEBASE_H__)
//:End Ignore