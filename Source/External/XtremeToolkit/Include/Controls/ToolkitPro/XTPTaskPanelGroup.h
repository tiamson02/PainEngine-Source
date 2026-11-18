// XTPTaskPanelGroup.h interface for the ÑXTPTaskPanelGroup class.
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
#if !defined(__XTPTASKPANELGROUP_H__)
#define __XTPTASKPANELGROUP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPTaskPanelItem.h"

class CXTPTaskPanel;
class CXTPTaskPanelItems;
class CXTPTaskPanelItem;
class CXTPTaskPanelPaintManager;
class CXTPTaskPanelGroupItem;
class CXTPTaskPanelGroupItems;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelGroup is a CXTPTaskPanelItem derived class. It is used in TaskPanel control
//			as single group.
class _XTP_EXT_CLASS CXTPTaskPanelGroup : public CXTPTaskPanelItem
{
	DECLARE_SERIAL(CXTPTaskPanelGroup)

	// Protected constructor used by dynamic creation.
	CXTPTaskPanelGroup(CXTPTaskPanel* pPanel = NULL);           

// Attributes
public:

// Operations
public:

	// Returns: Parent TaskPanel control.
	// Summary: Call this method to get the parent TaskPanel control that the group belongs to
	CXTPTaskPanel* GetTaskPanel();
	
	// Returns: Collection of items of group
	// Summary: Call this method to get collection of group items.
	CXTPTaskPanelItems* GetItems();
	
	// Input:	nIndex - Specifies the zero-based index of the item to retrieve. 
	// Returns: A pointer to a CXTPTaskPanelGroupItem object.
	// Summary: This method retrieves an item with the specified index. 
	CXTPTaskPanelGroupItem* GetAt(int nIndex);
	
	// Input:	nID - Identifier of the item to be found.
	// Returns: A pointer to item if successful; otherwise returns NULL.
	// Summary: Call this member to find item with specified identifier.
	CXTPTaskPanelGroupItem* FindItem(int nID);

	// Input:	nID - Identifier of item to be added.
	//			nImage - Image index in TaskPanel image list.
	// Returns: A pointer to the created CXTPTaskPanelGroupItem class 
	// Summary: Call this method to add a new link item to group.
	CXTPTaskPanelGroupItem* AddLinkItem(UINT nID, int nImage = -1);
	
	// Input:	strText - Items text
	// Returns: A pointer to created CXTPTaskPanelGroupItem class 
	// Summary: Call this method to add a new text item to group.
	CXTPTaskPanelGroupItem* AddTextItem(CString strText);
	

	// Input:	hWnd - Window Handle
	// Returns: A pointer to created CXTPTaskPanelGroupItem class 
	// Summary: Call this method to add a new control item to group.
	CXTPTaskPanelGroupItem* AddControlItem(HWND hWnd);

	// Input:	pt - Specifies the point to be tested. 
	// Returns: A pointer to a CXTPTaskPanelGroupItem item that occupies the specified point or NULL if 
	//			no item occupies the point.
	// Summary: This method determines where a point lies in a specified item.
	CXTPTaskPanelGroupItem* HitTest(CPoint pt, CRect* lpRect = 0);

	// Input:	rc - new group's position.
	//			bRecalcOnly - TRUE to recalculate only.
	// Summary: This method is called when a group is repositioned.
	CRect OnReposition(CRect rc, BOOL bRecalcOnly = TRUE);
	
	// Input:	nStep - Step of animation.
	// Summary: This method is called when an animation step is executed.
	void OnAnimate(int nStep);
	
	// Input:	pDC - Points to the client device context.
	// Summary: This method is called to draw a group.
	void OnPaint(CDC* pDC);

	// Returns: Caption rectangle.
	// Summary: Call this method to get the rectangle of the caption.
	CRect GetCaptionRect();
	
	// Returns: Client rectangle.
	// Summary: Call this method to get the current rectangle of the client.
	CRect GetClientRect();

	// Returns: Target client rectangle
	// Summary: Call this method to get the target rectangle of the client.
	CRect GetTargetClientRect();

	// Input:	bSpecial - TRUE if group is special.
	// Summary: Call this method to set group special.
	void SetSpecialGroup(BOOL bSpecial = TRUE);
	
	// Summary: Call this function to determine if the group is special.
	BOOL IsSpecialGroup();

	// Input:	bExpandable - TRUE if group is expandable.
	// Summary: Call this method to set expandable state of group.
	void SetExpandable(BOOL bExpandable);
	
	// Summary: Call this method to determine if group is expandable.
	BOOL IsExpandable();

	// Input:	bShow - TRUE to show caption; otherwise FALSE.
	// Summary: Call this method to show or hide group's caption.
	void ShowCaption(BOOL bShow);
	
	// Summary: Retrieves if caption is visible.
	BOOL IsCaptionVisible();

	// Summary: Retrieves if group is expanded.
	BOOL IsExpanded();

	// Summary: Retrieves if group is currently expanding.
	BOOL IsExpanding();

	// Returns: Client height of expanded group.
	// Summary: Call this function to get the height of expanded group.
	int GetExpandedClientHeight();
	
	// Input:	bExpanded - TRUE to expand group; otherwise FALSE.
	// Summary: Call this method to expand or collapse the group.
	void SetExpanded(BOOL bExpanded);

	// Summary: TRUE if current position of group is dirty.
	BOOL IsDirty();

	// Returns: Outer margins
	// Summary: Retrieves outer margins of group
	CRect& GetOuterMargins();

	// Returns: Inner margins
	// Summary: Retrieves inner margins of group
	CRect& GetInnerMargins();
	
	// Summary: Retrieves if group can accept focus.
	BOOL IsAcceptFocus();

	void SetClientBitmap(UINT nID, COLORREF clrTransparent, BOOL bAlpha = FALSE);
	virtual void OnFillClient(CDC* pDC, CRect rc);

	XTPTaskPanelItemLayout GetItemLayout();
	void SetItemLayout(XTPTaskPanelItemLayout itemLayout);

	void SetSelectedItem(CXTPTaskPanelGroupItem* pItem);

	BOOL IsChildItemDragOver();

	int GetItemCount();

	BOOL IsScrollButtonEnabled(BOOL bScrollButtonUp);
	int GetScrollOffsetPos();
	int GetOffsetItem();
	
	void Scroll(int nDelta);
	void EnsureVisible(CXTPTaskPanelGroupItem* pItem);
	int GetItemsInRow();

	void Serialize(CArchive& ar);

protected:
	int CalcInsideHeight();
	int OnCalcClientHeight();
	
protected:

	CRect m_rcGroupTarget;				// Target rectangle of group.
	CRect m_rcGroupCurrent;				// Current rectangle of group.

	CRect m_rcMarginsOuter;				// Outer margins.
	CRect m_rcMarginsInner;				// Inner margins.
	
	int m_nCaptionHeight;				// Height of caption.
	
	BOOL m_bSpecial;					// TRUE if group is special.

	BOOL m_bExpandable;					// TRUE if group is expandable.
	BOOL m_bExpanded;					// TRUE if group is expanded.
	BOOL m_bExpanding;					// TRUE if group is expanding now.
	int m_nExpandedClientHeight;		// Expanded client height.
	BOOL m_bCaptionVisible;				// TRUE if caption is visible.
	
	CImageList m_ilClient;
	CSize m_szClientBitmap;

	int m_nScrollOffset;
	int m_nItemsInRow;

	CXTPTaskPanelGroupItems* m_pItems;	// Collection of items.
	XTPTaskPanelItemLayout m_itemLayout;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPTaskPanelGroup)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Summary: Destroys a CXTPTaskPanelGroup object, handles cleanup and de-allocation.
	virtual ~CXTPTaskPanelGroup();

	// Generated message map functions
	//{{AFX_MSG(CXTPTaskPanelGroup)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

//:Ignore
#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	
	DECLARE_OLETYPELIB_EX(CXTPTaskPanelGroup)

	enum {
		dispidItems = 10L,
		dispidSpecial = 11L,
		dispidCaptionVisible = 12L,
		dispidExpandable = 13L,
		dispidExpanded = 14L,
	};
	
	afx_msg LPDISPATCH OleGetItems();

#endif

	friend class CXTPTaskPanel;
//:End Ignore
};

AFX_INLINE CXTPTaskPanel* CXTPTaskPanelGroup::GetTaskPanel() {
	ASSERT(m_pPanel != NULL);
	return m_pPanel;
}
AFX_INLINE CXTPTaskPanelItems* CXTPTaskPanelGroup::GetItems() {
	ASSERT(m_pItems);
	return (CXTPTaskPanelItems*)m_pItems;
}


AFX_INLINE int CXTPTaskPanelGroup::GetExpandedClientHeight() {
	return m_nExpandedClientHeight;
}
AFX_INLINE CRect& CXTPTaskPanelGroup::GetOuterMargins() {
	return m_rcMarginsOuter;
}
AFX_INLINE CRect& CXTPTaskPanelGroup::GetInnerMargins() {
	return m_rcMarginsInner;
}
AFX_INLINE int CXTPTaskPanelGroup::GetOffsetItem() {
	return m_nScrollOffset;
}
int AFX_INLINE CXTPTaskPanelGroup::GetItemsInRow() {
	return m_nItemsInRow;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPTASKPANELGROUP_H__)
//:End Ignore