// XTPCommandBar.h : interface for the CXTPCommandBar class.
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
#if !defined(__XTPCOMMANDBAR_H__)
#define __XTPCOMMANDBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"

class CXTPDockBar;
class CXTPControls;
class CXTPControl;
class CXTPControlPopup;
class CXTPCommandBars;
class CXTPCommandBar;
class CXTPPaintManager;
class CXTPImageManager;

#define XTP_TID_HOVER    112223		
#define XTP_TID_EXPANDED_POPUP 112224		
#define XTP_BOOL_DEFAULT 2


//////////////////////////////////////////////////////////////////////
// Summary: CXTPCommandBarList is a CList derived class. It is used as a collection 
// of CXTPCommandBar classes.
class _XTP_EXT_CLASS CXTPCommandBarList : public CList<CXTPCommandBar*, CXTPCommandBar*>
{
public:
	
	// Summary: Destroys a CXTPCommandBarList object, handles cleanup and de-allocation
	~CXTPCommandBarList();	
	
	// Input:	nBarID - Identifier CommandBar to find.
	// Returns: A pointer to a CXTPCommandBar object.
	// Summary: Call this member to find the CommandBar with the specified identifier.
	CXTPCommandBar* FindCommandBar(UINT nBarID);  
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPCommandBar is a CWnd derived class. It represents the parent
//			class for command bars.
class _XTP_EXT_CLASS CXTPCommandBar : public CWnd, public IHookAble
{
	
	
protected:

	// Summary: Constructs a CXTPCommandBar object
	CXTPCommandBar();

	// Summary: Destroys a CXTPCommandBar object, handles cleanup and de-allocation
	virtual ~CXTPCommandBar();

public:

	// Returns: TRUE if the command bar is in the tracking mode.
	// Summary: Call this member to get the tracking state.
	int IsTrackingMode() const;

	// Returns: A CXTPControls pointer.
	// Summary: Call this member to get the controls collection.
	CXTPControls* GetControls();

	// Returns: The type of the command bar.
	// Summary: Call this member to get the type of the command bar.
	XTPBarType GetType();

	// Returns: The position of the command bar.
	// Summary: Call this member to get the position of the command bar.
	XTPBarPosition GetPosition() const;

	// Summary: Call this member to recalculate the layout.
	void DelayLayout();

	// Summary: Call this member to redraw the command bar.
	virtual void DelayRedraw();

	// Input:	 bMode - TRUE to set the tracking mode; otherwise FALSE.
	//			 bSelectFirst - TRUE to select the first item.
	// Returns:  TRUE if the method was successful.
	// Summary:  Call this member to change the tracking state.
	// See Also: IsTrackingMode.
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);

	// Input:	pMenu - Menu to be converted.
	//          bRemoveControls - TRUE to remove all controls first.
	// Returns: TRUE if the method was successful; otherwise FALSE.
	// Summary: Call this member to convert menu items to command bar controls.
	BOOL LoadMenu(CMenu* pMenu, BOOL bRemoveControls = TRUE);

	// Input:	 nIndex - An integer index.
	// Returns:  The CXTPControl pointer currently at this index.
	// Summary:  Call this member to return the control at the specified index.
	// See also: GetControls, CXTPControls::GetAt
	CXTPControl* GetControl(int nIndex);

	// Input:	nSelected - An integer index of the item to be selected.
	//			bKeyboard - TRUE if the item was selected using the keyboard.
	// Returns: TRUE if the method was successful; otherwise FALSE.
	// Summary: Call this member to select the specified control.
	virtual BOOL SetSelected(int nSelected, BOOL bKeyboard = FALSE);

	// Input:	nPopuped - An integer index of the item to be popuped.
	//			bKeyboard - TRUE if the item is popuped by the keyboard.
	// Returns: TRUE if the method was successful; otherwise FALSE.
	// Summary: Call this member to popup the specified control.
	BOOL SetPopuped(int nPopuped, BOOL bKeyboard = FALSE);

	// Input:	strTitle - Title to be set.
	// Summary: Call this member to set a title for this command bar.
	void SetTitle(CString strTitle);

	// Returns: The title of the command bar.
	// Summary: Call this member to get a title for this command bar.
	CString GetTitle();
	
	// Returns: A CXTPCommandBars pointer (can be NULL).
	// Summary: Call this member to get the command bar's object.
	CXTPCommandBars* GetCommandBars() const;  
	
	// Input:	pCommandBars - CXTPCommandBars to be set.
	// Summary: Call this member to set the command bar's object.
	void SetCommandBars(CXTPCommandBars* pCommandBars);

	// Returns: Command bar's site (cannot be NULL).
	// Summary: Call this member to get the command bar's site.
	CWnd* GetSite();	

	// Returns: The command bar's site (can be NULL).
	// Summary: Call this member to get the command bar's site as CFrameWnd.
	CFrameWnd* GetFrameSite();
	
	// Input:	pControl - The control to be set.
	// Summary: Call this member to set focused control.
	void SetFocusedControl(CXTPControl* pControl);

	// Input:	dwFlagsAdd - Flags to be added.
	// Input:	dwFlagsRemove - Flags to be removed.
	// Summary: Call this member to set the command bar's flags.
	void SetFlags(DWORD dwFlagsAdd, DWORD dwFlagsRemove = 0);

	// Input:	 pButtons - Pointer to an array of command Ids. It cannot be NULL.
	//			 nCount - Number of elements in the array pointed to by pButtons.
	// Returns:  Nonzero if successful; otherwise 0.
	// Summary:  Call this member to set the controls from the button array.
	// See Also: LoadToolBar, LoadMenu.
	BOOL SetButtons(UINT* pButtons, int nCount);
	
	// Input:	 nIDResource - Resource ID of the toolbar to be loaded.
	// Returns:  Nonzero if successful; otherwise 0.
	// Summary:  Call this member function to load the toolbar specified by nIDResource. 
	// See Also: LoadMenu, SetButtons.
	virtual BOOL LoadToolBar(UINT nIDResource, BOOL bLoadIcons = TRUE);

	// Input:	size - Width and height, in pixels, of the toolbar's icons.
	// Summary: Call this function to set the size of the actual bitmapped images to be added to a toolbar.
	void SetIconSize(CSize size);

	// Summary: Call this member function to get the size of a toolbar icon.
	CSize GetIconSize();

	// Input:   bTextBelow - TRUE to show the caption below the icon.
	// Summary: Call this member to show the caption below control's icon.
	void ShowTextBelowIcons(BOOL bTextBelow = TRUE);
	
	BOOL IsTextBelowIcons();

	// Input:	pControlPopup - Points to a CXTPControlPopup object.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary: This method is called to popup the command bar.
	virtual BOOL Popup(CXTPControlPopup* pControlPopup, BOOL = FALSE);
	
	// Returns: Window handle of focused control.
	// Summary: Call this method to retrieve the current focused control.
	HWND GetTrackFocus();
	
	// Input:	hwnd - Window handle of focused control
	// Summary: Call this method to set focus to window . 
	void SetTrackFocus(HWND hwnd);
	
	// Input:	bVisible - TRUE to set control visible.
	// Summary: Call this method to set visibility of the control.
	virtual void SetVisible(BOOL bVisible);
	
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Returns a Boolean value that indicates whether the command bar is visible or hidden
	virtual BOOL IsVisible();
	
	// Input:	barPosition - Position of the command bar
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Call this member function to set the position of the command bar
	virtual BOOL SetPosition(XTPBarPosition barPosition);
	
	// Input:	chAccel - Character to find.
	// Returns: Index of the control contained  character as accelerator.
	// Summary: Call this method to get the index of control with specified accelerator character.
	int FindAccel(TCHAR chAccel);
	
	// Input:	chAccel - Character to find
	//			bSelectOnly - TRUE to select only.
	// Returns: Index of the control contained  character as accelerator.
	// Summary: Call this method to get the index of the control with specified accelerator character.
	int FindAccelEx(TCHAR chAccel, BOOL& bSelectOnly);
	
	// Returns: TRUE if command bars are in customized mode; otherwise returns FALSE
	// Summary: Call this method to retrieve the customize mode of the command bars.
	BOOL IsCustomizeMode();
	
	// Input:	pDataObject - Points to a CXTPControl object
	//			point -	Point to test.
	// Returns: DROPEFFECT enumerator.
	// Summary: This member is called when a control is entered into the command bar.
	DROPEFFECT OnCustomizeDragEnter(CXTPControl* pDataObject, CPoint point);
	
	// Summary: This method is called when a control leaves the command bar.
	void OnCustomizeDragLeave();
	
	// Input:	pDataObject - Points to a CXTPControl object
	//			point - Point to test			
	// Returns:	DROPEFFECT enumerator.
	// Summary:	This member is called when control is over the command bar.
	DROPEFFECT OnCustomizeDragOver(CXTPControl* pDataObject, CPoint point);
	
	// Input:	pDataObject - Points to a CXTPControl object
	//			dropEffect - DROPEFFECT enumerator.
	//			ptDrop - Point where user drops the control.
	//			ptDrag - Point where user starts drag the control.
	// Summary: This method is called when control is dropped to the command bar.
	void OnCustomizeDrop(CXTPControl* pDataObject, DROPEFFECT& dropEffect, CPoint ptDrop, CPoint ptDrag);
	
	
	// Returns: TRUE if the command bar is customizable; otherwise returns FALSE
	// Summary: Call this method to retrieve the customizable property of the command bar.
	virtual BOOL IsCustomizable();
	
	// Input:	bEnable - TRUE to enable customization.
	// Summary: Call this method to enable/disable customization of the command bar.
	void EnableCustomization(BOOL bEnable = TRUE);

	// Summary: This method is called to recalculate command bar layout.
	virtual void OnRecalcLayout();

	// ITrackAble

	// Summary: Call this method to remove the tracking state of the command bar.
	void OnTrackLost();
	
	// Summary: Call this method to track the command bar.
	void OnTrackEnter();
	
	// Returns: A pointer to a CWnd object
	// Summary: Call this method to retrieve the owner window of the command bar.
	CWnd* GetOwnerSite();	

	// Summary: Call this method to redraw all parent command bars.
	void InvalidateParents();
	
	// Returns: Flags of the command bar.
	// Summary: Call this method to retrieve the flags of the command bar.
	DWORD GetFlags();
	
	// Returns: Identifier of the command bar.
	// Summary: Call this method to retrieve the identifier of the command bar.
	UINT GetBarID();
	
	// Input:	bSelected - TRUE when control becomes selected.
	//			pControl - Points to a CXTPControl object
	// Summary: This method is called when a child control becomes selected.
	void OnControlSelected(BOOL bSelected, CXTPControl* pControl);
	
	// Input:	strDescription - Description string.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called to set the description context.
	BOOL SetStatusText(CString strDescription);	

	// Input:	dwData - Contains the new value to associate with the command bar. 
	// Summary: This method sets the 32-bit value associated with the command bar.
	void SetCommandBarData(DWORD dwData);
	
	// Returns: The 32-bit value associated with the command bar.
	// Summary: This method retrieves the application-supplied 32-bit value associated with the command bar.
	DWORD GetCommandBarData();

	// Returns: A pointer to a CXTPCommandBar object
	// Summary: This method retrieves the parent command bar.
	virtual CXTPCommandBar* GetParentCommandBar() const;
	
	// Returns: A pointer to a CXTPCommandBar object
	// Summary: Call this method to retrieve the top-level parent command bar.
	CXTPCommandBar* GetRootParent();
	
	// Returns: A pointer to a CXTPPaintManager object
	// Summary: Call this method to retrieve the paint manager.
	CXTPPaintManager* GetPaintManager();

	CXTPImageManager* GetImageManager();


	// Input:	nWidth - Width of the floated or popuped toolbar.
	// Summary: Call this member to set the width of the floated or popup toolbar.
	void SetWidth(int nWidth);
	
	// Summary: Call this member to get the width of the floated or popup toolbar.
	int GetWidth();

	void SetPaintManager(CXTPPaintManager* pPaintManager);

	void SetImageManager(CXTPImageManager* pImageManager);


protected:


	// Input:	hWnd - Window handle message belongs to.
	//			nMessage - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information. 
	//			lParam - Specifies additional message-dependent information.
	//			lResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Summary: This member function is called by WindowProc, or is called during message reflection.
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	The framework calls this member function when a non-system key is pressed.
	virtual BOOL OnKeyDown(UINT nChar);

	// Returns: Size of the command bar.
	// Summary:	This method is called to get the size of the command bar.
	virtual CSize CalcDynamicLayout(int, DWORD /*nMode*/);
	
	// Input:	key - Special keys enumerator.
	// Returns: TRUE if key handled, otherwise returns FALSE
	// Summary:	This method is called to process special keys.
	virtual BOOL ProcessSpecialKey(XTPSpecialKey key);
	
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to draw the command bar in the given context.
	virtual void DrawCommandBar(CDC* pDC);
	
	// Summary: This method is called when the controls array is changed.
	virtual void OnControlsChanged();
	
	
 	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method makes a copy of the command bar.
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to either read or write this object from or to an archive.
	void Serialize(CArchive& ar);

protected:
	UINT GetActiveDocTemplate();

protected:
	virtual void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList);
	virtual void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);

private:
	void CustomizeFindDropIndex(CXTPControl* pDataObject, const CPoint& point, CRect& rcMarker, int& nDropIndex, BOOL& bDropAfter);
	void CustomizeDrawMarker(CDC* pDC);
	void UpdateDocTemplateControls();
	void OnUpdateCmdUI(CWnd* pTarget, BOOL bDisableIfNoHndler);	

public:
	int m_nStateFlags;						// State of the command bars.

	static int s_nHoverDelay;
	static int s_nExpandDelay;

protected:
	
	int				 m_nMRUWidth;			// Recent width.
	int				 m_nSelected;			// Selected control index.
	int				 m_nPopuped;			// Popuped control index.
	HWND			 m_hwndFocus;			// Focused window.
	UINT			 m_nBarID;				// Command bar's identifier.
	BOOL			 m_bVisible;			// TRUE if the command bar is visible.
	BOOL			 m_bTracking;			// TRUE if the command bar is in tracking mode.
	CWnd*			 m_pSite;				// Command bar's site.
	DWORD			 m_nIdleFlags;			// Idle flags.
	DWORD			 m_dwFlags;				// Command bar's flags.
	DWORD			 m_dwStyle;				// Command bar's styles.
	CString			 m_strTitle;			// Command bar's caption.
	XTPBarType		 m_barType;				// Type of the command bar.
	CXTPControl*	 m_pFocused;			// A pointer to focused control.
	CXTPControls*	 m_pControls;			// A pointer to controls collection.
	XTPBarPosition   m_barPosition;			// Position of the command bar.
	CXTPCommandBars* m_pCommandBars;		// parent command bars class.
	CWnd*			 m_pOwner;				// Owner window.
	CSize			 m_szIcons;				// Size of the command bar's icons.
	BOOL			 m_bTextBelow;
	BOOL			 m_bExecOnRButton;


	BOOL			 m_bCustomizable;		// TRUE if command bar is customizable.
	CRect			 m_rcMarker;			// Marker position.
	BOOL			 m_bVerticalMarker;		// TRUE to draw marker vertically.
	BOOL			 m_bIgnoreLButtonUp;	// TRUE to ignore next WM_LBUTTONUP message.

	DWORD m_dwData;							// The 32-bit value associated with the command bar.
	UINT* m_pReturnCmd;						// Return command holder.

	BOOL m_bContextMenuPresent;				// TRUE if the command bar is present in context menu.
	BOOL m_bAnimatePopup;					// TRUE to animate popup bar.
	int m_nSchema;							// Serialized schema.

	CXTPPaintManager* m_pPaintManager;

	CXTPImageManager* m_pImageManager;



public:
//:Ignore

	//{{AFX_VIRTUAL(CXTPCommandBar)
	virtual void OnFinalRelease();
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL 
	 
	//{{AFX_MSG(CXTPCommandBar)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM lParam);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
	DECLARE_XTP_COMMANDBAR(CXTPCommandBar)

	class CXTPCommandBarCmdUI;

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPCommandBar);

	afx_msg LPDISPATCH OleControls();	
	afx_msg long OleGetType();			
	afx_msg long OleGetPosition();		
	afx_msg void OleSetPosition(long);	
	afx_msg long OleShowPopup(const VARIANT& Flags, const VARIANT& x, const VARIANT& y);
	afx_msg void OleSetIconSize(long cx, long cy);
	afx_msg void OleEnableDocking(long dwFlags);
	afx_msg void OleSetTearOffPopup(LPCTSTR strCaption, long nID, long nWidth);
	afx_msg LPDISPATCH OleFindControl(const VARIANT& varType, long nId, const VARIANT& bVisible, const VARIANT& bRecursive);
	afx_msg LPDISPATCH OleParent();
	afx_msg BOOL OleBuiltIn();
	afx_msg void OleDelete();
	afx_msg void OleModifyStyle(long dwRemove, long dwAdd);
	afx_msg void OleGetWindowRect(long* pLeft, long* pTop, long* pRight, long* pBottom);
	afx_msg void OleSetPopupToolBar(BOOL bToolBarType);
	afx_msg LPDISPATCH OleGetControlPopup();




	enum {
		dispidTitle = 1L,
		dispidControls = 2L,
		dispidType = 3L,
		dispidVisible = 4L,
		dispidPosition = 5L,
		dispidShowPopup = 6L,
		dispidSetIconSize = 7L,
		dispidEnableDocking = 8L,
		dispidSetTearOffPopup = 9L,
		dispidBarID = 10L,
		dispidFindControl = 11L, 
		dispidParent = 12L,
		dispidBuiltIn = 13L,
		dispidDelete = 14L,
		dispidContextMenuPresent = 15L,
		dispidModifyStyle = 16L,
		dispidGetWindowRect = 17L,
		dispidSetFlags = 18L
	};
public:
	static CXTPCommandBar* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPCommandBar*)((BYTE*)pDisp - offsetof(CXTPCommandBar, m_xDispatch)));
	}
	

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		MSG msg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.message = message;
		msg.hwnd = m_hWnd;
		GetCursorPos(&msg.pt);
		
		// Tooltips notification.
		_FilterToolTipMessage(&msg, this);

		return CWnd::OnWndMsg(message, wParam, lParam, pResult);

	}
	static void PASCAL _FilterToolTipMessage(MSG* pMsg, CWnd* pWnd);
	
#endif

	friend class CXTPCommandBars;
	friend class CXTPDockBar;
	friend class CXTPCommandBarCmdUI;	
	friend class CXTPControl;
	friend class CXTPControls;
	friend class CXTPControlPopup;
	friend class CXTPToolBar;
	friend class CXTPMouseManager; 
	friend class CXTPPopupBar;
	friend class CXTPCustomizeSheet;

//:End Ignore

};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE int CXTPCommandBar::IsTrackingMode() const{	
	return m_bTracking; 
}
AFX_INLINE CXTPControls* CXTPCommandBar::GetControls() { 
	return m_pControls; 
} 
AFX_INLINE XTPBarType CXTPCommandBar::GetType() { 
	return m_barType; 
}
AFX_INLINE XTPBarPosition CXTPCommandBar::GetPosition() const{ 
	return m_barPosition; 
}
AFX_INLINE void CXTPCommandBar::DelayLayout() { 
	m_nIdleFlags |= idleLayout; 
}
AFX_INLINE void CXTPCommandBar::DelayRedraw() { 
#ifndef _XTP_COMPONENT
	m_nIdleFlags |= idleRedraw; 
#else
	if (m_hWnd) Invalidate(FALSE);
#endif
}
AFX_INLINE void CXTPCommandBar::SetTitle(CString strTitle) { 
	m_strTitle = strTitle;
	if (GetSafeHwnd()) Invalidate(FALSE);
}
AFX_INLINE CString CXTPCommandBar::GetTitle() { 
	return m_strTitle;
}
AFX_INLINE void CXTPCommandBar::SetCommandBars(CXTPCommandBars* pCommandBars){
	m_pCommandBars = pCommandBars;
}
AFX_INLINE HWND CXTPCommandBar::GetTrackFocus() { 
	return m_hwndFocus;
}
AFX_INLINE void CXTPCommandBar::SetTrackFocus(HWND hwnd) { 
	m_hwndFocus = hwnd;
}	
AFX_INLINE void CXTPCommandBar::SetVisible(BOOL bVisible) {
	UNREFERENCED_PARAMETER(bVisible);
}
AFX_INLINE BOOL CXTPCommandBar::IsVisible() {
	return FALSE;
}
AFX_INLINE BOOL CXTPCommandBar::SetPosition(XTPBarPosition barPosition) {
	UNREFERENCED_PARAMETER(barPosition); return FALSE;
}
AFX_INLINE CSize CXTPCommandBar::CalcDynamicLayout(int, DWORD /*nMode*/) {
	return CSize(0,0);
}
AFX_INLINE CXTPCommandBar* CXTPCommandBar::GetParentCommandBar() const {
	return NULL;
}
AFX_INLINE void CXTPCommandBar::OnRecalcLayout() {

}
AFX_INLINE BOOL CXTPCommandBar::IsCustomizable() {
	return m_bCustomizable;
}

AFX_INLINE void CXTPCommandBar::EnableCustomization(BOOL bEnable) {
	m_bCustomizable = bEnable;
}

AFX_INLINE void CXTPCommandBar::SetIconSize(CSize size) {
	m_szIcons = size;
}
AFX_INLINE DWORD CXTPCommandBar::GetFlags() {
	return m_dwFlags;
}

AFX_INLINE UINT CXTPCommandBar::GetBarID() {
	return m_nBarID;
}
AFX_INLINE void CXTPCommandBar::SetCommandBarData(DWORD dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD CXTPCommandBar::GetCommandBarData() {
	return m_dwData;
}

#define foreach(variable, visible)\
	if (1) for (CXTPControl* variable = m_pControls->GetFirst(); variable; m_pControls->GetNext(variable)) if (!visible || (variable->IsVisible()))

//:End Ignore

#endif //#if !defined(__XTPCOMMANDBAR_H__)
