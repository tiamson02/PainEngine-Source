// XTPToolBar.h : interface for the CXTPToolBar class.
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
#if !defined(__XTPTOOLBAR_H__)
#define __XTPTOOLBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBar.h"

class CXTPDockBar;
class CXTPCommandBars;
class CXTPDockContext;

// Summary: CXTPToolBar is a CXTPCommandBar derived class.
class _XTP_EXT_CLASS CXTPToolBar : public CXTPCommandBar
{
public:
	// Summary: Constructs a CXTPToolBar object
	CXTPToolBar(void);

	// Summary: Destroys a CXTPToolBar object, handles cleanup and de-allocation
	~CXTPToolBar(void);
public:
//:Ignore
	class CXTPControlExpandButton;		
	class CXTPControlButtonHide;		
	class CXTPControlButtonCustomize;	
	class CToolBarInfo;					
//:End Ignore
public:

	// Input:	bVisible - TRUE to show the toolbar; FALSE to hide.
	// Summary: Call this member function to show or hide the toolbar.
	void SetVisible(BOOL bVisible);

	// Returns: TRUE if the toolbar is visible.
	// Summary: Returns a Boolean value that indicates whether the toolbar is visible or hidden.
	BOOL IsVisible() { return m_bVisible; }

	// Input:	barPosition - New position of toolbar.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary: Call this member to set the position of the toolbar.
	BOOL SetPosition(XTPBarPosition barPosition);

	// Input:	nIDResource - Resource ID of the toolbar to be loaded.
	// Returns: Nonzero if successful; otherwise 0.
	// Summary: Call this member function to load the toolbar specified by nIDResource. 
	BOOL LoadToolBar(UINT nIDResource, BOOL bLoadIcons = TRUE);

public:

	// Input:	dwStyle - Styles for the toolbars
	//			pParentWnd - Specifies the parent window of the TaskPanel control.
	//			nID - Specifies the Toolbar control ID.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method creates the ToolBar control.
	BOOL CreateToolBar(DWORD dwStyle, CWnd* pParentWnd, UINT nID = AFX_IDW_TOOLBAR);
	
	// Input:	pInfo - Points to a CToolBarInfo object
	// Summary: Retrieves bar information for serialization routines.
	void GetBarInfo(CToolBarInfo* pInfo);
	
	// Input:	pInfo - Points to a CToolBarInfo object
	// Summary: Sets bar information.
	void SetBarInfo(CToolBarInfo* pInfo);
	
	// Input:	nLength - Length of the bar.
	//			dwMode - Mode to dock.
	//			nWidth - Width of the bar.
	// Returns: Size of the docked toolbar.
	// Summary: This method calculates the dimensions of a toolbar.
	CSize CalcDockingLayout(int nLength, DWORD dwMode, int nWidth = 0);
	
	// Input:	dwMode - Mode to dock.
	// Returns: Size of the docked toolbar.
	// Summary: This method calculates the dimensions of a toolbar.
	CSize CalcDynamicLayout(int, DWORD dwMode);
	
	// Input:	dwFlags - Flags of docking. It can be one or more of the following:
	//			[ul]
	//			[li]<b>xtpFlagAlignTop</b> - Allows docking at the top of the client area. .[/li]
	//			[li]<b>xtpFlagAlignBottom</b> - Allows docking at the bottom of the client area. [/li]
	//			[li]<b>xtpFlagAlignLeft</b> - Allows docking on the left side of the client area.[/li]
	//			[li]<b>xtpFlagAlignRight</b> - Allows docking on the left side of the client area. [/li]
	//			[li]<b>xtpFlagFloating</b> - Allows floating. [/li]
	//			[li]<b>xtpFlagHideWrap</b> - Allows to hide wrapped controls..[/li]
	//			[li]<b>xtpFlagStretched</b> - Bar is stretched..[/li]
	//			[/ul]
	// Summary: Call this method to set docking flags.
	void EnableDocking(DWORD dwFlags = xtpFlagAlignAny | xtpFlagFloating | xtpFlagHideWrap);
	
	// Returns: TRUE if toolbar is built-in; otherwise returns FALSE
	// Summary: Determines if the toolbar is built-in.
	BOOL IsBuiltIn();
	
	// Input:	nID - Identifier to set.
	// Summary: Call this method to set the identifier.
	void SetBarID(UINT nID);
	
	// Returns: A pointer to a CXTPDockBar object
	// Summary: Retrieves the parent dock bar.
	CXTPDockBar* GetDockBar();

	// Summary: Resets the controls to their original state.
	virtual void Reset();
	
	// Input:	bPreview - Preview mode
	// Summary: Call this method to hide/show toolbars in preview mode.
	void OnSetPreviewMode(BOOL bPreview);
	
	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	The framework calls this member function when a non-system key is pressed.
	BOOL OnKeyDown(UINT nChar);

	// Input:	bPresent - TRUE to show the toolbar in a context menu.
	// Summary: This method allows to hide toolbar in a context menu.
	void SetContextMenuPresent(BOOL bPresent);
	
	// Input:	dwRemove - Styles to remove.
	//			dwAdd - Styles to add.
	// Summary: Modifies the styles of the toolbar.
	void ModifyBarStyle(DWORD dwRemove, DWORD dwAdd);
	
	

protected:

	// Input:	pParentWnd - Points to a CWnd object
	//			bFloat - TRUE to create a floating bar.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Creates a toolbar
	virtual BOOL Create(CWnd* pParentWnd, BOOL bFloat);
	
	// Summary: This method recalculates the position of the toolbar.
	void OnRecalcLayout();

	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
 	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method makes a copy of the command bar.
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

private:
	CSize _CalcDynamicLayout(int nLength, DWORD dwMode);
	
	void BuildCustomizePopup(CXTPCommandBar* pExpandBar);
	int _FindNearest(CXTPControls* pControls, CXTPControl* pFind, int nPos, BOOL bVisible = FALSE);
	void _RestoreDeletedControls();

protected:

	CXTPDockBar* m_pDockBar;			// Parent dock bar.
	CXTPDockContext* m_pDockContext;	// Docking context.
	BOOL m_bBuiltIn;					// TRUE if toolbar is built-in.
	BOOL m_bTearOff;					// TRUE if toolbar is tear-offed.
	BOOL m_bPreviewVisible;				// TRUE if Toolbar was visible in preview mode.

protected: 
//:Ignore
	
	DECLARE_MESSAGE_MAP()
	
	//{{AFX_MSG(CXTPToolBar)	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg LRESULT OnFloatStatus(WPARAM wParam, LPARAM);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();  
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint pt);
	//}}AFX_MSG


//:End Ignore

private:
	DECLARE_XTP_COMMANDBAR(CXTPToolBar)

private:
	friend class CXTPCommandBars;			
	friend class CXTPDockContext;			
	friend class CXTPDockBar;				
	friend class CXTPCustomizeToolbarsPage;	
	friend class CXTPPopupBar;				
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE BOOL CXTPToolBar::IsBuiltIn() {
	return m_bBuiltIn;
}

AFX_INLINE void CXTPToolBar::SetBarID(UINT nID) {
	m_nBarID = nID;
}
AFX_INLINE CXTPDockBar* CXTPToolBar::GetDockBar()
{
	return m_pDockBar;
}
AFX_INLINE void CXTPToolBar::SetContextMenuPresent(BOOL bPresent) {
	m_bContextMenuPresent = bPresent;
}
AFX_INLINE void CXTPToolBar::ModifyBarStyle(DWORD dwRemove, DWORD dwAdd) {
	m_dwStyle |= dwAdd;
	m_dwStyle &= ~dwRemove;
}
 



#endif //#if !defined(__XTPTOOLBAR_H__)