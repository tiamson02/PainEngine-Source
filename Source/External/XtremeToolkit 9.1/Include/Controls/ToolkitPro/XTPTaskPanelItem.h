// XTPTaskPanelItem.h interface for the CXTPTaskPanelItem class.
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
#if !defined(__XTPTASKPANELITEM_H__)
#define __XTPTASKPANELITEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPTaskPanel;
class CXTPTaskPanelPaintManager;

// Summary: Task Panel item types.
enum XTPTaskPanelItemType
{
	xtpTaskItemTypeGroup,		// Item is group.
	xtpTaskItemTypeLink,		// Item is link.
	xtpTaskItemTypeText,		// Item is text.
	xtpTaskItemTypeControl		// Item is control.
};

class CXTPTaskPanelItems;
class CXTPTaskPanelGroup;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelItem is a CCmdTarget derived class. It is used in TaskPanel control
//			as parent for CXTPTaskPanelGroupItem and CXTPTaskPanelGroup classes.
class _XTP_EXT_CLASS CXTPTaskPanelItem : public CCmdTarget
{
	DECLARE_DYNCREATE(CXTPTaskPanelItem)

	// Protected constructor used by dynamic creation
	CXTPTaskPanelItem(CXTPTaskPanel* pPanel = NULL);           

// Operations
public:

	// Returns: Parent TaskPanel control.
	// Summary: Call this method to get parent TaskPanel control item belongs to
	CXTPTaskPanel* GetTaskPanel();

	// Input:	nID - The new identifier of the item.
	// Summary: Call this method to set the identifier of the item.
	void SetID(UINT nID);
	
	// Returns: Identifier of the item.
	// Summary: Call this method to get the identifier of the item.
	UINT GetID();
	
	// Input:	lpstrCaption - The new caption of the item.
	// Summary: Call this method to set the caption of the item. 
	void SetCaption(LPCTSTR lpstrCaption);

	// Input:	lpstrTooltip - The ToolTip text to display when the mouse cursor is over the item. 
	// Summary: Associates ToolTip text with the item.
	void SetTooltip(LPCTSTR lpstrTooltip);

	// Returns: Type of the item.
	// Summary: Retrives the type of the item.
	XTPTaskPanelItemType GetType();
	
	// Input:	typeItem - Type of the item
	// Summary: Call this method to set the type of the item.
	void SetType(XTPTaskPanelItemType typeItem);

	// Returns: A CString object containing caption of the item.
	// Summary: Call this member to retrieve the item's caption
	CString GetCaption();

	// Returns: The ToolTip text for the item.
	// Summary: Call this member to retrieve the item's tooltip
	CString GetTooltip();

	// Returns: TRUE if item is enabled; otherwise FALSE.
	// Summary: Retrives the state of the item.
	BOOL GetEnabled();

	// Input:	bEnabled - TRUE to enable item.
	// Summary: Call this method to enable/disable the item.
	void SetEnabled(BOOL bEnabled);

	// Returns: A pointer to a CXTPTaskPanelPaintManager object
	// Summary: Retrieves the paint manager
	CXTPTaskPanelPaintManager* GetPaintManager();

	// Returns: TRUE if item is hot; otherwise FALSE.
	// Summary: Determines if item is hot.
	BOOL IsItemHot();

	// Returns: TRUE if item is focused; otherwise FALSE.
	// Summary: Determines if item is focused.
	BOOL IsItemFocused();

	// Summary: Call this method to redraw the parent TaskPanel control.
	void RedrawPanel();

	// Input:	nIndex - Index of the icon in the image list of parent TaskPanel control.
	// Summary: Call this method to set icon associated with item.
	void SetIconIndex(int nIndex);
	
	// Returns: Icon index of item.
	// Summary: Retrieves icon index.
	int GetIconIndex();

	// Returns: TRUE if item can be focused; otherwise FALSE.
	// Summary: Call this method to determine if item can be focused.
	virtual BOOL IsAcceptFocus();

	

// Attributes
protected:
	CXTPTaskPanel* m_pPanel;			// Parent TaskPanel class
	CString m_strCaption;				// Caption of the item.
	CString m_strTooltip;				// Tooltip of the item.
	UINT m_nID;							// Identifier.

	XTPTaskPanelItemType m_typeItem;	// Type of the item.

	CXTPTaskPanelItems* m_pItems;		// Collection of child items.

	int m_nIconInex;					// Icon index.
	BOOL m_bEnabled;					// TRUE if item is enabled.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPTaskPanelItem)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Summary: Destroys a CXTPTaskPanelItem object, handles cleanup and de-allocation.
	virtual ~CXTPTaskPanelItem();

	// Generated message map functions
	//{{AFX_MSG(CXTPTaskPanelItem)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

//:Ignore
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()

	enum {
		dispidCaption = 1L,
		dispidTooltip = 2L,
		dispidEnabled = 3L,
		dispidType = 4L,
		dispidId = 5L,
		dispidIconIndex = 6L,
	};
	afx_msg BSTR OleGetCaption();
	afx_msg BSTR OleGetTooltip();

#endif

	friend class CXTPTaskPanelItems;	

	DECLARE_MESSAGE_MAP()
//:End Ignore

};

AFX_INLINE CXTPTaskPanel* CXTPTaskPanelItem::GetTaskPanel() {
	ASSERT(m_pPanel != NULL);
	return m_pPanel;
}
AFX_INLINE CString CXTPTaskPanelItem::GetCaption() {
	return m_strCaption;
}
AFX_INLINE CString CXTPTaskPanelItem::GetTooltip() {
	return m_strTooltip;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPTASKPANELITEM_H__)
//:End Ignore