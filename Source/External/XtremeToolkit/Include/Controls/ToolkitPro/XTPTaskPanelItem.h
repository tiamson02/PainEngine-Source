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
class CXTPTaskPanelItems;
class CXTPTaskPanelGroup;
class CXTPImageManagerIcon;

#include "XTPTaskPanelDefines.h"

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelItem is a CCmdTarget derived class. It is used in TaskPanel control
//			as parent for CXTPTaskPanelGroupItem and CXTPTaskPanelGroup classes.
class _XTP_EXT_CLASS CXTPTaskPanelItem : public CCmdTarget
{
	DECLARE_DYNCREATE(CXTPTaskPanelItem)

	// Protected constructor used by dynamic creation
	CXTPTaskPanelItem();           

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

	// Returns: TRUE if item is pressed; otherwise FALSE.
	// Summary: Determines if item is pressed.
	BOOL IsItemPressed();

	// Returns: TRUE if item is focused; otherwise FALSE.
	// Summary: Determines if item is focused.
	BOOL IsItemFocused();

	BOOL IsItemDragging();

	BOOL IsItemDragOver();

	BOOL IsGroup();

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

	CXTPImageManagerIcon* GetImage(int nWidth);

	BOOL PrepareDrag (COleDataSource& srcItem);
	static CXTPTaskPanelItem* AFX_CDECL CreateFromOleData(COleDataObject* pDataObject);
	HGLOBAL CacheGlobalData();

	void Remove();
	void RepositionPanel();

	CXTPTaskPanelItems* GetParentItems();
	virtual CXTPTaskPanelGroup* GetItemGroup();

	int GetIndex();

	BOOL IsAllowDrop();
	void AllowDrop(BOOL bAllowDrop);

	void AllowDrag(BOOL bAllowDrag);
	BOOL IsAllowDrag();

	CFont* GetItemFont();

	void SetTextRect(CDC* pDC, CRect rc, UINT nFormat);
	CRect GetTextRect(UINT* pFormat = 0);

public:
	virtual void Serialize(CArchive& ar);
	
	void CopyToClipboard();
	static CXTPTaskPanelItem* AFX_CDECL PasteFromClipboard();

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
	BOOL m_bAllowDrop;
	BOOL m_bAllowDrag;

	CRect m_rcText;
	UINT m_nTextFormat;


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
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPTaskPanelItem)


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
	afx_msg LPDISPATCH OleGetGroup();
	afx_msg LPDISPATCH OleGetParentItems();

public:
	static CXTPTaskPanelItem* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPTaskPanelItem*)((BYTE*)pDisp - offsetof(CXTPTaskPanelItem, m_xDispatch)));
	}


#endif

	friend class CXTPTaskPanelItems;	

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
AFX_INLINE CXTPTaskPanelItems* CXTPTaskPanelItem::GetParentItems() {
	return m_pItems;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPTASKPANELITEM_H__)
//:End Ignore