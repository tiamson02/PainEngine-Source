// XTPTaskPanel.h interface for the ÑXTPTaskPanel class.
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
#if !defined(__XTPTASKPANEL_H__)
#define __XTPTASKPANEL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPTaskPanelGroups;
class CXTPTaskPanelGroup;
class CXTPTaskPanelPaintManager;
class CXTPTaskPanelItems;
class CXTPTaskPanelItem;
class CXTPTaskPanelGroupItem;
class CXTPImageManager;

#include "XTPTaskPanelDefines.h"


class CXTPTaskPanelScrollButton
{
public:
	CXTPTaskPanelScrollButton(BOOL bScrollUp);

public:
	BOOL IsVisible();

public:
	BOOL bScrollUp;
	BOOL bEnabled;
	BOOL bPressed;
	CRect rcButton;
	BOOL bHot;

	CXTPTaskPanelGroup* pGroupScroll;
	CXTPTaskPanelGroup* pGroupCaption;
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanel is a CWnd derived class. It is used to implement
//			an Explorer and Office-like Task Panel control. 
class _XTP_EXT_CLASS CXTPTaskPanel : public CWnd
{
	DECLARE_DYNAMIC(CXTPTaskPanel)
// Construction
public:
	// Summary: Constructs a TaskPanel object
	CXTPTaskPanel();

// Operations
public:

	// Input:	dwStyle - Style for the TaskPanel.
	//			rect -  Specifies the size and position of the TaskPanel control. 
	//			pParentWnd - Specifies the parent window of the TaskPanel control.
	//			nID - Specifies the TaskPanel control ID.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary: This method creates the TaskPanel control.
	BOOL Create(DWORD dwStyle, const RECT&  rect, CWnd* pParentWnd, UINT nID);

	// Summary: Call this member to get group collection of TaskPanel control.
	// Returns: A pointer to CXTPTaskPanelItems class that represents the groups of TaskPanel.
	CXTPTaskPanelItems* GetGroups();

	// Input:	nID - Identifier of the group to be added.
	// Returns: A pointer to the created CXTPTaskPanelGroup class 
	// Summary: Call this member to add a new group.
	CXTPTaskPanelGroup* AddGroup(UINT nID, int nImage = -1);

	// Input:	nID - Identifier of group to be found.
	// Returns: A pointer to group if successful; otherwise returns NULL.
	// Summary: Call this member to find a group with specified identifier.
	CXTPTaskPanelGroup* FindGroup(UINT nID);

	// Input:	nIndex - Specifies the zero-based index of the group to retrieve. 
	// Returns: A pointer to a CXTPTaskPanelGroup object.
	// Summary: This method retrieves a group with specified index. 
	CXTPTaskPanelGroup* GetAt(int nIndex);

	//Summary: Call this method to get scroll offset.
	int GetScrollOffset();

	// Summary: Retrieves the current height of a client area.
	int GetClientHeight();

	// Input:	bRecalcOnly - TRUE if needs recalculate only without positioning.
	// Summary: Call this method to reposition groups.
	void Reposition(BOOL bRecalcOnly = TRUE);

	// Input:	pItem - Item to be hot.
	// Summary: Call this method to set hot item of the TaskPanel control.
	void SetHotItem(CXTPTaskPanelItem* pItem);
	
	// Summary: Retrieves the current hot item. 
	CXTPTaskPanelItem* GetHotItem();

	// Input:	pItem - Item to be focused.
	//			bDrawFocusRect - TRUE to draw focused rect.
	// Summary: Call this method to focus the specified item.
	void SetFocusedItem(CXTPTaskPanelItem* pItem, BOOL bDrawFocusRect = FALSE);

	void EnsureVisible(CXTPTaskPanelItem* pItem);
	
	// Returns:	Item currently focused.
	// Summary: Call this method to get focused item.
	CXTPTaskPanelItem* GetFocusedItem();

	BOOL IsDrawFocusRect();
	
	// Input:	pt - Specifies the point to be tested. 
	//          pbCaption - Pointer to BOOL value to return if point lies on group caption.
	// Returns: A pointer to a CXTPTaskPanelGroup item that occupies the specified point or NULL if 
	//			no group occupies the point.
	// Summary: This method determines where a point lies in a specified group.
	CXTPTaskPanelGroup* HitTestGroup(CPoint pt, BOOL* pbCaption = NULL);
	
	// Input:	pt - Specifies the point to be tested. 
	// Returns: A pointer to a CXTPTaskPanelItem item that occupies the specified point or NULL if 
	//			no item occupies the point.
	// Summary: This method determines where a point lies in a specified item.
	CXTPTaskPanelItem* HitTestItem(CPoint pt, BOOL bClientAvail = FALSE);

	// Input:	nLeft - Left margin.
	//			nTop - Top margin,
	//			nRight - Right margin,
	//			nBottom - Bottom margin.
	//			nMiddle - Margin between groups.
	// Summary: Call this method to set group margins.
	void SetMargins(long nLeft, long nTop, long nRight, long nBottom, long nMiddle);

	// Input:	pImageList - Pointer to the image list to be assigned to the TaskPanel control. 
	// Summary: Assigns an image list to a TaskPanel control.
	void SetImageList(CImageList* pImageList, CSize szItemIcon = CSize(16, 16));
	
	// Input:	pImageList - Pointer to the image list to be assigned to the groups of TaskPanel.  
	// Summary: Assigns an image list to a TaskPanel's groups.
	void SetGroupImageList(CImageList* pImageList, CSize szGroupIcon = CSize(32, 32));

	// Summary: Retrieves Image manager associated with Task Panel.
	CXTPImageManager* GetImageManager();

	// Summary: Call this method to assign new image manager.
	void SetImageManager(CXTPImageManager* pImageManager);

	// Input:	bExpandable - TRUE if TaskPanel groups are expandable.
	// Summary: Call this method to set groups to expandable.
	void SetExpandable(BOOL bExpandable);
	
	// Summary: Call this method to determine if groups are expandable
	BOOL IsExpandable();

	// Input:	panelAnimation - Animation type to be set. It can be one of the following:
	//			[ul]
	//			[li]<b>xtpTaskPanelAnimationYes</b> - To set animation. It's default value.[/li]
	//			[li]<b>xtpTaskPanelAnimationNo</b> - To remove animation.[/li]
	//			[li]<b>xtpTaskPanelAnimationSystem</b> - Use system settings to determine animation.[/li]
	//			[/ul]
	// Summary: Call this method to set animation of the TaskPanel control
	void SetAnimation(XTPTaskPanelAnimation panelAnimation);
	
	// Returns: Animation type of TaskPanel control.
	// Summary: Call this method to get animation type.
	XTPTaskPanelAnimation GetAnimation();
	
	// Input:	bLockRedraw - TRUE to lock redraw.
	// Summary: Call this method to lock/unlock redraw of controls.
	void SetLockRedraw(BOOL bLockRedraw);

	// Input:	pItem - Current item.
	//			nDirection - Direction to find (1 to forward, -1 to backward)
	//			bTab - TRUE to use tabulation.
	// Returns: Next item.
	// Summary: Call this method to get next item in tab order.
	CXTPTaskPanelItem* GetNextItem(CXTPTaskPanelItem* pItem, int nDirection, BOOL bTab, BOOL bSkipRowItems = TRUE);

	// Input:	pItem - Pointer to clicked item.
	// Summary: This method is called when user clicks on the specified item.
	virtual void OnClick(CXTPTaskPanelGroupItem* pItem);

	// Summary: TRUE if current position of groups is dirty.
	BOOL IsDirty();	

	void OnSrollChanged(int nCurPos);
	BOOL AnimateGroups(BOOL bCheckDirty = TRUE);

	CSize GetItemIconSize();
	CSize GetGroupIconSize();
	void SetIconSize(CSize szIcon);

public:

	// Input:	paintTheme - New visual theme. It can be one of the following:
	//			[ul]
	//			[li]<b>xtpTaskPanelThemeOffice2000</b> - Office 2000 like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeOffice2003</b> - Office 2003 like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeNativeWinXP</b> - Explorer like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeOffice2000Plain</b> - Plain Office 2000 like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeOfficeXPPlain</b> - Plain Office XP like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeOffice2003Plain</b> - Plain Office 2003 like theme.[/li]
	//			[li]<b>xtpTaskPanelThemeNativeWinXPPlain</b> - Plain  xplorer like theme.[/li]
	//			[/ul]
	// Summary: Call this member to switch the visual theme of the TaskPanel control.
	void SetTheme(XTPTaskPanelPaintTheme paintTheme);
	
	// Input:	pPaintManager - New paint manager.
	// Summary: Call this member to set a custom theme. 
	void SetCustomTheme(CXTPTaskPanelPaintManager* pPaintManager);
	
	// Returns: The current paint manager.
	// Summary: Call this member to get the current paint manager.
	CXTPTaskPanelPaintManager* GetPaintManager();
	
	// Returns: The current theme.
	// Summary: Call this member to get the current visual theme.
	XTPTaskPanelPaintTheme GetCurrentTheme();	

public:
	void ExpandGroup(CXTPTaskPanelGroup* pGroup, BOOL bExpanded);
	BOOL IsGroupExpanded(CXTPTaskPanelGroup* pGroup);

	void SetHotTrackStyle(XTPTaskPanelHotTrackStyle hotTrackStyle);
	XTPTaskPanelHotTrackStyle GetHotTrackStyle();

	XTPTaskPanelItemLayout GetItemLayout();
	void SetItemLayout(XTPTaskPanelItemLayout itemLayout);

	void SetBehaviour(XTPTaskPanelBehaviour panelBehaviour);
	XTPTaskPanelBehaviour GetBehaviour();

	CXTPTaskPanelGroup* GetActiveGroup();

	void SetSelectItemOnFocus(BOOL bSelect);
	void DrawFocusRect(BOOL bAccept);
	CRect GetMargins();

	static CLIPFORMAT AFX_CDECL GetClipboardFormat();

	void AllowDrag(BOOL bAllowDrag);
	void AllowDrop(BOOL bAllowDrop);

	void RenameItem(CXTPTaskPanelItem* pItem);

	CXTPTaskPanelScrollButton* GetScrollButton(BOOL bScrollUp);
	CXTPTaskPanelScrollButton* HitTestScrollButton(CPoint point);

protected:
	BOOL IsExplorerBehaviour();
	
	virtual void OnStartItemDrag(CXTPTaskPanelGroupItem* pItem);
	virtual void OnStartGroupDrag(CXTPTaskPanelItem* pItem);

	virtual void OnEndLabelEdit(CXTPTaskPanelItem* pItem, LPCTSTR str);
	
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	void RepositionScrollButtons();
	void OnClickScrollButton(CXTPTaskPanelScrollButton* pScrollButton);
	void UpdateScrollButtons();
	void CheckScrollButtonMouseOver(CPoint point);

	virtual void OnItemRemoved(CXTPTaskPanelItem* pItem);

private:
	class CPanelDropTarget;

// Attributes
protected:
	CXTPTaskPanelGroups* m_pGroups;					// Collection of groups.

	CXTPTaskPanelPaintManager* m_pPaintManager;		// Current Paint Manager.
	XTPTaskPanelPaintTheme m_paintTheme;			// Current theme.

	BOOL m_bLockRedraw;								// TRUE when control is locked.

	int m_nClientHeight;							// Client height.
	
	CRect m_rcMargins;								// Groups margins.
	int m_nGroupSpacing;

	BOOL m_bScrollVisible;							// TRUE if scroll bar is visible.
	
	CXTPTaskPanelItem* m_pItemHot;					// Currently hot item.
	CXTPTaskPanelItem* m_pItemFocused;				// Currently focused item.
	CXTPTaskPanelItem* m_pItemPressed;				// Currently pressed item.
	
	CXTPTaskPanelItem* m_pItemDragging;
	CXTPTaskPanelItem* m_pItemDragOver;
	
	BOOL m_bAnimation;								// TRUE if animation enabled.
	XTPTaskPanelAnimation m_eAnimation;				// Current animation state.

	HCURSOR m_hHandCursor;							// Hand cursor.
	
	
	UINT m_nAnimateTimer;							// Timer identifier for animation.
	int m_nAnimationStep;							// Current step of animation.

	CXTPImageManager* m_pImageManager;

	BOOL m_bExpandable;								// TRUE if groups are expandable.

	BOOL m_bAcceptFocus;							// TRUE if control accept focus.
	BOOL m_bDrawFocusRect;							// TRUE to draw focused rect.
	CSize m_szItemIcon;
	CSize m_szGroupIcon;
	BOOL m_bSelectItemOnFocus;
	
	CPoint m_ptPressed;

	BOOL m_bAllowDrag;
	BOOL m_bAllowDrop;

	XTPTaskPanelItemLayout m_itemLayout;
	XTPTaskPanelHotTrackStyle m_hotTrackStyle;
	XTPTaskPanelBehaviour m_panelBehaviour;

	CXTPTaskPanelGroup* m_pActiveGroup;
	CPanelDropTarget* m_pDropTarget;
	static CLIPFORMAT m_cfItem;
	
	BOOL m_bTimerGroupHover;

	CXTPTaskPanelScrollButton* m_pScrollButton[2];


// Implementation
public:

	// Summary: Destroys a CXTPTaskPanel object, handles cleanup and de-allocation.
	virtual ~CXTPTaskPanel();

	// Generated message map functions
protected:

//:Ignore
	//{{AFX_VIRTUAL(CXTPTaskPanel)
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTaskPanel)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseLeave();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


#ifdef _XTP_ACTIVEX
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		MSG msg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.message = message;
		msg.hwnd = m_hWnd;
		GetCursorPos(&msg.pt);

		// Tooltips notification.
		AxFilterToolTipMessage(&msg, this);

		return CWnd::OnWndMsg(message, wParam, lParam, pResult);
	}	

#endif



	friend class CXTPTaskPanelGroup;
	friend class CXTPTaskPanelItem;
	friend class CTaskPanelCtrl;
	friend class CXTPTaskPanelPaintManager;
	friend class CPanelDropTarget;
	friend class CXTPTaskPanelGroupItem;
	friend class CXTPTaskPanelEditItem;
	friend class CXTPTaskPanelItems;
//:End Ignore
};

AFX_INLINE CXTPTaskPanelItems* CXTPTaskPanel::GetGroups() {
	return (CXTPTaskPanelItems*)m_pGroups;
}
AFX_INLINE CXTPTaskPanelPaintManager* CXTPTaskPanel::GetPaintManager() {
	return m_pPaintManager;
}
AFX_INLINE CXTPTaskPanelItem* CXTPTaskPanel::GetHotItem() {
	return m_pItemHot;
}
AFX_INLINE XTPTaskPanelPaintTheme CXTPTaskPanel::GetCurrentTheme() {
	return m_paintTheme;
}
AFX_INLINE void CXTPTaskPanel::SetExpandable(BOOL bExpandable) {
	m_bExpandable = bExpandable;
}
AFX_INLINE XTPTaskPanelAnimation CXTPTaskPanel::GetAnimation() {
	return m_eAnimation;
}
AFX_INLINE BOOL CXTPTaskPanel::IsExpandable() {
	return m_bExpandable;
}
AFX_INLINE CSize CXTPTaskPanel::GetItemIconSize() {
	return m_szItemIcon;
}
AFX_INLINE CSize CXTPTaskPanel::GetGroupIconSize() {
	return m_szGroupIcon;
}
AFX_INLINE BOOL CXTPTaskPanel::IsExplorerBehaviour() {
	return m_panelBehaviour == xtpTaskPanelBehaviourExplorer;
}
AFX_INLINE CXTPTaskPanelGroup* CXTPTaskPanel::GetActiveGroup() {
	return m_pActiveGroup;
}
AFX_INLINE void CXTPTaskPanel::SetSelectItemOnFocus(BOOL bSelect) {
	m_bSelectItemOnFocus = bSelect;
}
AFX_INLINE void CXTPTaskPanel::DrawFocusRect(BOOL bAccept) {
	m_bAcceptFocus = bAccept;
}
AFX_INLINE CRect CXTPTaskPanel::GetMargins() {
	return m_rcMargins;
}
AFX_INLINE void CXTPTaskPanel::SetIconSize(CSize szIcon) {
	m_szItemIcon = szIcon;
	Reposition();
}
AFX_INLINE CLIPFORMAT AFX_CDECL CXTPTaskPanel::GetClipboardFormat() {
	return m_cfItem;
}
AFX_INLINE void CXTPTaskPanel::AllowDrag(BOOL bAllowDrag ) {
	m_bAllowDrag = bAllowDrag;
}
AFX_INLINE void CXTPTaskPanel::AllowDrop(BOOL bAllowDrop) {
	m_bAllowDrop = bAllowDrop;
}
AFX_INLINE CXTPTaskPanelScrollButton* CXTPTaskPanel::GetScrollButton(BOOL bScrollUp) {
	return bScrollUp? m_pScrollButton[TRUE]: m_pScrollButton[FALSE];
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////


#endif // !defined(__XTPTASKPANEL_H__)
//:End Ignore