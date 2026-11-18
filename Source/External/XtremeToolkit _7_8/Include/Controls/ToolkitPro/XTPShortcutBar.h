// XTPShortcutBar.h interface for the CXTPShortcutBar class.
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
#if !defined(__XTPSHORTCUTBAR_H__)
#define __XTPSHORTCUTBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPShortcutBarPaintManager.h"


const UINT XTP_SBN_SELECTION_CHANGING  = 1;
const UINT XTP_SBN_RCLICK = 2;

// Input:   nAction -  Value of wParam specifies a ShortcutBar value that indicates the user's 
//			request. This parameter can be one of the following values:
//			[ul]
//			[li]<b>XTP_SBN_SELECTION_CHANGING</b> The selection has changed in the shortcut bar.[/li]
//			[li]<b>XTP_SBN_RCLICK</b> User presses the right mouse button on the shortcut bar item.[/li]
//			[/ul]
//			pItem - The value of lParam points to an CXTPShortcutBarItem object that contains information for the 
//			specified item. This pointer should <b>never</b> be NULL.
// Returns: If the application is to process this message, the return value should be TRUE, otherwise the 
//			return value is FALSE. 
//
// Summary: The XTPWM_SHORTCUTBAR_NOTIFY message is sent to the CXTPShortcutBar owner window 
//			whenever an action occurs within the CXTPShortcutBar
//   
//			<pre>XTPWM_SHORTCUTBAR_NOTIFY 
//			nAction = (int) wParam;             // ShortcutBar action 
//			pItem = (CXTPShortcutBarItem*) lParam;  // pointer to an CXTPShortcutBarItem object</pre>
const UINT XTPWM_SHORTCUTBAR_NOTIFY = (WM_APP + 2583);


class CXTPShortcutBar;
class CXTPImageManager;
//////////////////////////////////////////////////////////////////////
// Summary: CXTPShortcutBarItem is the base class representing an item of the 
//			ShortcutBar Control. 
class _XTP_EXT_CLASS CXTPShortcutBarItem : public CCmdTarget
{
protected:

	// Input:	pShortcutBar - Points to the parent ShortcutBar class.
	//			nID - Identifier of the item.
	//			pWnd - Client window of the item
	// Summary: Constructs a CXTPShortcutBarItem object.
	CXTPShortcutBarItem(CXTPShortcutBar* pShortcutBar, UINT nID, CWnd* pWnd);
	
	// Input:	pShortcutBar - Points to the parent ShortcutBar class.
	// Summary: Constructs a CXTPShortcutBarItem object.
	CXTPShortcutBarItem(CXTPShortcutBar* pShortcutBar);
	
public:
	
	// Input:	nID - The new identifier of the item.
	// Summary: Call this method to set the identifier of the item.
	void SetID(int nID);
	
	// Returns: Identifier of the item.
	// Summary: Call this method to get the identifier of the item.
	int GetID();

	// Input:	strCaption - The new caption of the item.
	// Summary: Call this method to set the caption of the item. 
	void SetCaption(LPCTSTR strCaption);
	
	// Returns: A CString object containing the caption of the item.
	// Summary: Call this method to get the caption of the item.
	CString GetCaption();

	// Input:	bVisible - TRUE if the item is visible; otherwise FALSE. 
	// Summary: Call this member function to set the visibility state of the item.
	void SetVisible(BOOL bVisible);
	
	// Returns: TRUE if the item is visible; otherwise FALSE.
	// Summary: Call this member function to determine if the item is visible.
	BOOL IsVisible();

	// Input:	strTooltip - The ToolTip text to display when the mouse cursor is over the item. 
	// Summary: Associates ToolTip text with the item.
	void SetTooltip(LPCTSTR strTooltip);
	
	// Returns: The ToolTip text for the item.
	// Summary: Retrieves the ToolTip text associated with the item.
	CString GetTooltip();

	// Returns: TRUE if the item is expanded; otherwise FALSE.
	// Summary: Call this member function to determine if the item is expanded.
	BOOL IsExpanded();
	
	// Returns: TRUE if the item is expand button; otherwise FALSE.
	// Summary: Call this member function to determine if the item is expand button.
	BOOL IsItemExpandButton();
	
	// Returns: A Pointer to the parent CXTPShortcutBar class.
	// Summary: Call this member function to get the parent ShortcutBar class.
	CXTPShortcutBar* GetShortcutBar();
	
	// Returns: CRect object contains the bounding rectangle of the item.
	// Summary: This method retrieves the dimensions of the rectangle that bounds the item.
	CRect GetItemRect();

	// Input:	dwData - Contains the new value to associate with the item. 
	// Summary: This method sets the 32-bit value associated with the item.
	void SetItemData(DWORD dwData);
	
	// Returns: The 32-bit value associated with the item
	// Summary: This method retrieves the application-supplied 32-bit value associated with the item
	DWORD GetItemData();
	
	CXTPImageManagerIcon* GetImage(int nWidth);

protected:
	CString m_strCaption;				// Caption for the item.
	CString m_strTooltip;				// Tooltip for the item.
	int m_nID;							// Item's identifier.
	CRect m_rcItem;						// Bounding rectangle of the item
	BOOL m_bVisible;					// TRUE if the item is visible
	BOOL m_bExpanded;					// TRUE if the item is expanded
	BOOL m_bHidden;						// TRUE if the item is hidden
	HWND m_hwndChild;					// Child window associated with the item
	BOOL m_bExpandButton;				// True if the item is expand button
	DWORD m_dwData;						// The 32-bit value associated with the item
	CXTPShortcutBar* m_pShortcutBar;	// Parent CXTPShortcutBar class

//:Ignore

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPShortcutBarItem)
public:

	enum {
		dispidCaption = 1L,
		dispidTooltip = 2L,
		dispidId = 3L,
		dispidIconIndex = 4L,
		dispidVisible = 5L,
		dispidIconHandle = 6L,
	};
	afx_msg BSTR OleGetCaption();
	afx_msg BSTR OleGetTooltip();
	afx_msg void OleSetIconHandle(int nIcon);
	afx_msg int OleGetIconHandle();


	static CXTPShortcutBarItem* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPShortcutBarItem*)((BYTE*)pDisp - offsetof(CXTPShortcutBarItem, m_xDispatch)));
	}

#endif

	friend class CXTPShortcutBar;	
//:End Ignore
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPShortcutBar is a CWnd derived class. It is used to implement
//			an Outlook2003 ShortcutBar style control. 
class _XTP_EXT_CLASS CXTPShortcutBar : public CWnd
{
	typedef CArray<CXTPShortcutBarItem*, CXTPShortcutBarItem*> CShortcutArray;
	
// Construction
public:

	// Summary: Constructs a CXTPShortcutBar object.
	CXTPShortcutBar();
	
	// Input:	dwStyle - Style for the ShortcutBar.
	//			rect -  Specifies the size and position of the ShortcutBar control. 
	//			pParentWnd - Specifies the parent window of the ShortcutBar control.
	//			nID - Specifies the ShortcutBar control ID.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary: This method creates the ShortcutBar control 
	BOOL Create(DWORD dwStyle, const RECT&  rect, CWnd* pParentWnd, UINT nID);


public:
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored. 
	// Summary: Call this function to retrieve state information from the registry or .INI file.
	void LoadState(LPCTSTR lpszProfileName);
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored.
	// Summary: Call this function to save the state information to the registry or .INI file.
	void SaveState(LPCTSTR lpszProfileName);

// Operations
public:
	
	// Input:	nID - Identifier of item that is to be added.
	//			pWnd - A Pointer to CWnd class associated with new item.
	// Returns: A pointer to a CXTPShortcutBarItem object.
	// Summary: Call this method to add a new item.
	CXTPShortcutBarItem* AddItem(UINT nID, CWnd* pWnd = NULL);
	
	// Input:	nID - Identifier of the item needed to find.
	// Returns: A pointer to a CXTPShortcutBarItem object.
	// Summary: Call this method to find an item with the specified identifier.
	CXTPShortcutBarItem* FindItem(int nID);
	
	// Input:	pItem - Item to be selected.
	// Summary: Call this method to select the specified item.
	void SelectItem(CXTPShortcutBarItem* pItem);
	
	// Input: nID - Identifier of the item to delete.
	// Summary: Call this method to remove the item.
	void RemoveItem(int nID);

	// Summary: Call this method to remove all items.
	void RemoveAllItems();

	// Input:	nIndex - Specifies the zero-based index of the item to retrieve. 
	// Returns: A pointer to a CXTPShortcutBarItem object.
	// Summary: This method retrieves an item with specified index. 
	CXTPShortcutBarItem* GetItem(int nIndex);

	// Input:	pt - Specifies the point to be tested. 
	// Returns: A pointer to a CXTPShortcutBarItem item that occupies the specified point or NULL 
	//			if no item occupies the point.
	// Summary: This method determines where a point lies in a specified item.
	CXTPShortcutBarItem* HitTest(CPoint pt);

	// Input:	nMinHeight - Specifies the minimum height in pixels of the client. 
	// Summary: This method sets the minimum height of the client area in a ShortcutBar control.
	//			The default value is 200.
	void SetMinimumClientHeight(int nMinHeight);

	// Returns: CRect object contains gripper bounding rectangle.
	// Summary: Call this method to determine gripper rectangle.
	CRect GetGripperRect();
	
	// Returns: CSize object contains size of items
	// Summary: Call this method to get size of items 
	CSize GetItemSize();

	// Returns: Count of visible lines
	// Summary: This method determines the count of visible lines in the ShortcutBar control. 
	int GetExpandedLinesCount();
	
	// Input:	nCount - Number of visible lines.
	// Summary: This method sets the count of lines that are visible in the ShortcutBar control. 
	void SetExpandedLinesCount(int nCount);

	// Returns: Pointer to the hot item.
	// Summary: Returns the hot item in the ShortcutBar control. 
	CXTPShortcutBarItem* GetHotItem();
	
	// Returns: Pointer to the selected item.
	// Summary: Returns the selected item in the ShortcutBar control. 
	CXTPShortcutBarItem* GetSelectedItem();
	
	// Returns: Pointer to the pressed item.
	// Summary: Returns the pressed item in the ShortcutBar control. 
	CXTPShortcutBarItem* GetPressedItem();

	// Returns: TRUE if client pane is visible.
	// Summary: Determines visibility of client pane.
	BOOL IsClientPaneVisible();

	// Inputs:  bVisible - TRUE to show client pane; FALSE to hide.
	// Summary: Call this member to show or hide client pane.
	void SetClientPaneVisible(BOOL bVisible);

	void SetImageManager(CXTPImageManager* pImageManager);

	CXTPImageManager* GetImageManager();

public:

	// Input:	paintTheme - New visual theme. It can be one of the following:
	//			[ul]
	//			[li]<b>xtpShortcutThemeOffice2000</b> - Office 2000 like theme.[/li]
	//			[li]<b>xtpShortcutThemeOfficeXP</b> - Office XP like theme.[/li]
	//			[li]<b>xtpShortcutThemeOffice2003</b> - Office 2003 like theme.[/li]
	//			[/ul]
	// Summary: Call this member to switch the visual theme of the ShortcutBar control.
	void SetTheme(XTPShortcutBarPaintTheme paintTheme);
	
	// Input:	pPaintManager - New paint manager.
	// Summary: Call this member to set a custom theme. 
	void SetCustomTheme(CXTPShortcutBarPaintManager* pPaintManager);

	// Returns: The current paint manager.
	// Summary: Call this member to get the current paint manager.
	CXTPShortcutBarPaintManager* GetPaintManager();	

	// Returns: The current theme.
	// Summary: Call this member to get the current visual theme.
	XTPShortcutBarPaintTheme GetCurrentTheme();

protected:

	// Summary: Call this method to reposition items.
	void Reposition();

	// Summary: This method is called when reposition is done.
	virtual void OnRepositionDone();
	
	// Input:	pExpandButton - A pointer to expand button.
	// Summary: This method is called when expand button is pressed.
	virtual void OnExpandButtonDown(CXTPShortcutBarItem* pExpandButton);

	// Returns: Number of visible items
	// Summary: Call this method to determine the number of visible items
	int GetVisibleItemsCount();

	// Input:	pItem - Item to be hot.
	// Summary: Call this method to set hot item.
	void SetHotItem(CXTPShortcutBarItem* pItem);



protected:
	CXTPShortcutBarPaintManager* m_pPaintManager;	// Current paint manager.
	CXTPImageManager* m_pImageManager;
	int m_nExpandedLines;							// Number of lines currently visible
	int m_nDesiredExpandedLines;					// Number of lines desired to be visible.
	int m_nMinClientHeight;							// The height of client area.	
	XTPShortcutBarPaintTheme m_paintTheme;			// Current theme.
	CShortcutArray m_arrItems;						// Array of items.
	CSize m_szItem;									// Size of items
	CRect m_rcClient;								// Client's rectangle.
	CRect m_rcGripper;								// Gripper's rectangle.
	HWND m_hwndClient;								// Client window handle
	HCURSOR m_hSizeCursor;							// Handle to the cursor displayed for the size icon.
	BOOL m_bTrackingSize;							// TRUE if control in tracking mode.
	CXTPShortcutBarItem* m_pHotItem;				// Pointer to hot item.
	CXTPShortcutBarItem* m_pSelectedItem;			// Pointer to selected item.
	CXTPShortcutBarItem* m_pPressedItem;			// Pointer to pressed item.
	CToolTipCtrl m_wndToolTip;						// Tooltip to be displayed for the control.

	BOOL m_bClientPaneVisible;



// Overrides
	//:Ignore	
	//{{AFX_VIRTUAL(CXTPShortcutBar)
	public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	//:End Ignore

// Implementation
public:
		
	// Summary: Destroys a CXTPShortcutBar object, handles cleanup and de-allocation.
	virtual ~CXTPShortcutBar();

	// Generated message map functions
protected:

//:Ignore
	//{{AFX_MSG(CXTPShortcutBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseLeave();
	afx_msg void OnSysColorChange();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnExpandPopupExecute(WPARAM wparam, LPARAM lParam);
	afx_msg void OnShowMoreButtons();
	afx_msg void OnShowFewerButtons();
	afx_msg void OnUpdateShowMoreButtons(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowFewerButtons(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CXTPShortcutBarItem;	
//:End Ignore
};

//////////////////////////////////////////////////////////////////////


AFX_INLINE CString CXTPShortcutBarItem::GetCaption() {
	return m_strCaption;
}
AFX_INLINE CString CXTPShortcutBarItem::GetTooltip() {
	return m_strTooltip;
}
AFX_INLINE BOOL CXTPShortcutBarItem::IsItemExpandButton() {
	return m_bExpandButton;
}
AFX_INLINE BOOL CXTPShortcutBarItem::IsExpanded() {
	return m_bExpanded;
}
AFX_INLINE CXTPShortcutBar* CXTPShortcutBarItem::GetShortcutBar() {
	return m_pShortcutBar;
}
AFX_INLINE CRect CXTPShortcutBarItem::GetItemRect() {
	return m_rcItem;
}
AFX_INLINE void CXTPShortcutBarItem::SetItemData(DWORD dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD CXTPShortcutBarItem::GetItemData() {
	return m_dwData;
}





AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetHotItem() {
	return m_pHotItem;
}
AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetSelectedItem() {
	return m_pSelectedItem;
}
AFX_INLINE CXTPShortcutBarItem* CXTPShortcutBar::GetPressedItem() {
	return m_pPressedItem;
}
AFX_INLINE CRect CXTPShortcutBar::GetGripperRect() {
	return m_rcGripper;
}
AFX_INLINE CSize CXTPShortcutBar::GetItemSize() {
	return m_szItem;
}
AFX_INLINE int CXTPShortcutBar::GetExpandedLinesCount() {
	return m_nExpandedLines;
}
AFX_INLINE BOOL CXTPShortcutBar::IsClientPaneVisible() {
	return m_bClientPaneVisible;
}
AFX_INLINE void CXTPShortcutBar::SetClientPaneVisible(BOOL bVisible) {
	m_bClientPaneVisible = bVisible;
	Reposition();
}


AFX_INLINE CXTPShortcutBarPaintManager* CXTPShortcutBar::GetPaintManager() {
	return m_pPaintManager;
}
AFX_INLINE void CXTPShortcutBar::SetMinimumClientHeight(int nMinHeight) {
	m_nMinClientHeight = nMinHeight;
}
AFX_INLINE XTPShortcutBarPaintTheme CXTPShortcutBar::GetCurrentTheme() {
	return m_paintTheme;
}


#endif // !defined(__XTPSHORTCUTBAR_H__)
