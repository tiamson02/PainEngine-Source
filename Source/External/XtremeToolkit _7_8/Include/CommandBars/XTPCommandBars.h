// XTPCommandBars.h : interface for the CXTPCommandBars class.
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
#if !defined(__XTPCOMMANDBARS_H__)
#define __XTPCOMMANDBARS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"

class CXTPDockBar;
class CXTPToolBar;
class CXTPControl;
class CXTPPopupBar;
class CXTPDockState;
class CXTPCommandBar;
class CXTPCustomizeDropSource;
class CXTPCommandBars;
class CXTPCommandBarList;
class CXTPImageManager;
class CXTPPaintManager;
class CXTPControls;
class CXTPMenuBar;

// Summary: Options of the command bars.
struct  XTP_COMMANDBARS_OPTIONS : public CCmdTarget
{
public:
	
	// Summary: Constructs an XTP_COMMANDBARS_OPTIONS object
	XTP_COMMANDBARS_OPTIONS()
	{
		bAlwaysShowFullMenus = TRUE; bShowFullAfterDelay = TRUE;
		bToolBarScreenTips = TRUE; bToolBarAccelTips = FALSE;
		bLargeIcons = FALSE;
		bShowExpandButtonAlways = TRUE;
		szIcons = 0; szLargeIcons = 0;
		
		bSyncFloatingBars = TRUE;
		bDblClickFloat = FALSE;
		bShowTextBelowIcons = FALSE;

		animationType = xtpAnimateWindowsDefault;


		EnableAutomation();

	#ifdef _XTP_ACTIVEX
		nUpdatePeriod = 100;
		EnableTypeLib();
	#endif
	}
	BOOL bAlwaysShowFullMenus;							// TRUE to show full menus always.
	BOOL bShowFullAfterDelay;							// TRUE to show full menus after short delay.
	BOOL bToolBarScreenTips;							// TRUE to show toolbar's tooltips 
	BOOL bToolBarAccelTips;								// TRUE to add accelerators to tooltips.
	BOOL bLargeIcons;									// TRUE to use large toolbars' icons.
	BOOL bSyncFloatingBars;								// TRUE to hide floating bars when site frame deactivated.
	BOOL bShowExpandButtonAlways;						// TRUE to show expand buttons always.
	BOOL bDblClickFloat;								// TRUE to make toolbar floating be double-clicking.
	BOOL bShowTextBelowIcons;							// TRUE to show captions below icons.
	
	XTPAnimationType animationType;						// Animation type.
	
	CSize szIcons;										// Size of the normal icons.
	CSize szLargeIcons;									// Size of the large icons.

	CMap<UINT, UINT, BOOL, BOOL> m_mapHiddenCommands;	// Hidden command array.
	CMap<UINT, UINT, BOOL, BOOL> m_mapUsedCommands;		// Used commands array.

	// Input:	ar - A CArchive object to serialize to or from. 
	// Summary: Reads or writes options from or to an archive.
	void Serialize(CArchive& ar);

//:Ignore
#ifdef _XTP_ACTIVEX

	long nUpdatePeriod;
	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(XTP_COMMANDBARS_OPTIONS);
public:
	afx_msg BOOL OleGetFadedIcons();
	afx_msg void OleSetFadedIcons(BOOL);
	afx_msg BOOL OleGetDisabledIcons();
	afx_msg void OleSetDisabledIcons(BOOL);
	afx_msg BOOL OleGetIconsWithShadow();
	afx_msg void OleSetIconsWithShadow(BOOL);
	afx_msg void OleSetIconSize(BOOL bBigIcons, long cx, long cy);
	afx_msg void OleAddHiddenCommand(long nIDCommand);	
	afx_msg BOOL OleGetLunaColors();
	afx_msg void OleSetLunaColors(BOOL bLunaColors);
	afx_msg BOOL OleGetUseSharedImageList();
	afx_msg void OleSetUseSharedImageList(BOOL bUseSharedImageList);

	afx_msg void OleUpdatePeriodChanged();

#endif
//:End Ignore

private:
	
	CXTPCommandBars* m_pCommandBars;
	friend class CXTPCommandBars;
};	

//////////////////////////////////////////////////////////////////////
// Summary: CXTPCommandBars is a CCmdTarget derived class. 
class _XTP_EXT_CLASS CXTPCommandBars : public CCmdTarget
{

protected:
	// Summary: Constructs a CXTPCommandBars object
	CXTPCommandBars();

	// Summary: Destroys a CXTPCommandBars object, handles cleanup and de-allocation
	virtual ~CXTPCommandBars();
	

public:
	// Input:	strName - Title of menu bar.
	//			nID - menu identifier to be loaded.
	// Returns: A CXTPCommandBar object if successful; otherwise NULL.
	// Summary: Call this member to set the menu bar.
	CXTPMenuBar* SetMenu(CString strName, UINT nID);

	// Input:	strName - Title of toolbar.
	//			xtpPosition - Position of toolbar to be created.
	// Returns: A CXTPToolBar object if successful; otherwise NULL.
	// Summary: Call this member to add a new toolbar.
	CXTPToolBar* Add(CString strName, XTPBarPosition xtpPosition, CRuntimeClass* pToolbarClass = RUNTIME_CLASS(CXTPToolBar));

	// Returns: A CXTPCommandBar pointer associated with the menu bar.
	// Summary: Call this member to get the active menu bar.
	CXTPMenuBar* GetMenuBar();

	// Input:	nID - An integer identifier.
	// Returns: The CXTPToolBar pointer 
	// Summary: Call this member to get the toolbar with the specified identifier.
	CXTPToolBar* GetToolBar(UINT nID);

	// Returns: A CFrameWnd pointer representing the site of the command bars.
	// Summary: Call this member to get the site of the command bars.
	CWnd* GetSite();
	
	// Input:	xtpPosition - Position of the dockbar.
	// Returns: A CXTPDockBar pointer.
	// Summary: Call this member to get the dock bar at the specified position.
	CXTPDockBar* GetDockBar(XTPBarPosition xtpPosition);

	// Summary: Call this member to redraw the command bars.
	void RedrawCommandBars();

	// Summary: Call this member to reclculate site layout.
	void RecalcFrameLayout(BOOL bDelay = FALSE);
	
	// Input:	type - Type of the control to find.
	//			nId - Control's identifier.
	//			bVisible - Control is visible.
	//			bRecursive - To find in the nested command bars.
	// Returns: The CXTPControl object if successful; otherwise NULL.
	// Summary: Call this member to find the specified control.
	CXTPControl* FindControl(XTPControlType type, UINT nId, BOOL bVisible, BOOL bRecursive);

	// Input:	pMenu - Menu to be popped up
	//			nFlags - Reserved, currently not used.
	//			x - Specifies the horizontal position in screen coordinates of the pop-up menu.
	//			y - Specifies the vertical position in screen coordinates of the top of the menu on the screen.
	//			pWnd - Identifies the window that owns the pop-up menu. This window receives all WM_COMMAND messages from the menu.
	//			rcExclude - Reserved, currently not used.
	// Returns: Nonzero if the function is successful; otherwise 0.
	// Summary: Call this member to display a floating pop-up menu at the specified location.
	static BOOL AFX_CDECL TrackPopupMenu(CMenu* pMenu, UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT rcExclude = NULL, CWnd* pWndOwner = NULL, CXTPCommandBars* pCommandBars = NULL);
	
	// Input:	pPopupBar - Points to a CXTPPopupBar object
	//			nFlags - Flags of the popuped bar.
	//			x - Specifies the horizontal position in screen coordinates of the pop-up menu. 
	//			y - Specifies the vertical position in screen coordinates of the top of the menu on the screen. 
	//			pWnd - Specifies the vertical position in screen coordinates of the top of the menu on the screen. 
	//			rcExclude - Reserved, currently not used.
	//			pWndOwner - Points to a CWnd object
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Call this member to display a floating pop-up menu at the specified location.
	static BOOL AFX_CDECL TrackPopupMenu(CXTPPopupBar* pPopupBar, UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT rcExclude = NULL, CWnd* pWndOwner = NULL);

	// Returns: Command Bars options.
	// Summary: Call this member to retrieve the options of the command bars.
	XTP_COMMANDBARS_OPTIONS* GetCommandBarsOptions(); 

	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored.
	// Summary: Call this function to save the state information to the registry or .INI file.
	void SaveBarState(LPCTSTR lpszProfileName);
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored. 
	// Summary: Call this function to retrieve state information from the registry or .INI file.
	void LoadBarState(LPCTSTR lpszProfileName, BOOL bSilent = FALSE);
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored.
	// Summary: Call this function to save the command bars options.
	void SaveOptions(LPCTSTR lpszProfileName);
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored. 
	// Summary: Call this function to restore the options of the command bars.
	void LoadOptions(LPCTSTR lpszProfileName);

	// Input:	ar - A CArchive object to serialize to or from. 
	//			bSerializeControls - TRUE to store controls information.
	// Summary: Call this function to either read or write command bars from or to an archive.
	void SerializeBarState(CArchive& ar, BOOL bSerializeControls = FALSE, BOOL bOnlyCustomized = TRUE, BOOL bSilent = FALSE);
	
	// Input:	ar - A CArchive object to serialize to or from. 
	// Summary: Call this member function to load designer command bars from an archive.
	void LoadDesignerBars(CArchive& ar);


	// Intelligent menus routines

	// Input:	pCommands - An array of command IDs, of menu items, to hide.
	//			nCount - Size of the array passed in.
    // Summary: This member function adds the specified commands to the list of
    //			menu items to hide until activated by clicking on the chevron.
 	void HideCommands(const UINT* pCommands, int nCount);
	
	// Input:	nCommand - The command ID of a menu item to hide.
    // Summary: This member function adds the specified command to the list of
    //			menu items to hide until activated by clicking on the chevron.
	void HideCommand(UINT nCommand);
	
	// Input:	nCommand - Command id to check.
	// Returns:	TRUE if the command item is hidden; otherwise returns FALSE.
	// Summary: Call this member function to retrieve if the command is hidden until activated 
	//			by clicking on the chevron.
	virtual BOOL IsCommandHidden(UINT nCommand);
	
	// Input:	nCommand - Command id to set
	// Summary:	Call this member fundtion to set the command activated by user.
	void SetCommandUsed(UINT nCommand);
	
	// Summary: Call this member function to restore the state of the hidden commands.
	void ResetUsageData();

	virtual UINT GetActiveDocTemplate();

//:Ignore
#ifdef _XTP_DOTNET
	void UnHideCommand(UINT nCommand);
	BOOL IsCommandUsed(UINT nCommand);
	void UnSetCommandUsed(UINT nCommand);
	void SetCustomizeAvail(bool customizeAvail)
	{
		m_bCustomizeAvail=customizeAvail;
	}
#endif
//:End Ignore

public:
	// Input:	pMsg - Points to an MSG object
	// Returns: TRUE if the message was handled; otherwise FALSE.
	// Summary: Call this method to process frame messages.
	//			CXTPFrameWndBase derived classes call this method automatically. 
	BOOL PreTranslateFrameMessage(MSG* pMsg);
	
	// Input:	message - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information.
	//			lParam - Specifies additional message-dependent information. 
	//			pResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Returns: TRUE if the message was handled; otherwise FALSE.
	// Summary: Call this method to process frame messages.
	//			CXTPFrameWndBase derived classes call this method automatically. 
	BOOL OnFrameWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	
	// Input:	point - Position of the menu to show.
	// Summary: This method shows the context menu for toolbars.
	virtual void ContextMenu(CPoint point);

	// Returns: A CXTPPopupBar object.
	// Summary: This method creates a context menu for toolbars.
	virtual CXTPPopupBar* GetToolbarsPopup();
	
	// Input:	pToolBar - Points to a CXTPToolBar object 
	//			pExpandBar - Points to a CXTPCommandBar object
	// Summary: Call this method to create an expand bar.
	virtual void GetHiddenControls(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar);
	
	// Input:	pToolBar - Points to a CXTPToolBar object
	//			pExpandBar - Points to a CXTPCommandBar object
	// Summary: Call this method to create customized controls.
	virtual void GetAddOrRemovePopup(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar);
	

	// Input:	pFrame - Points to a CWnd object
	// Summary: This method is called to set the site window.
	void SetSite(CWnd* pFrame);
	
	// Input:	pt - Point needed to test.
	//			pMainDock - CXTPDockBar class currently docked in.
	// Returns: A pointer to a CXTPDockBar object,
	// Summary: Call this member to retrieve dock bar in specified point.
	CXTPDockBar* CanDock(CPoint pt, CXTPDockBar* pMainDock = NULL);
	
	// Returns: A pointer to a CXTPCommandBars object
	// Summary: This member creates command bars.
	static CXTPCommandBars* AFX_CDECL CreateCommandBars();
	
	// Summary: This member is called to create dock bars.
	void EnableDocking(void);
	
	// Returns: Number of the toolbars.
	// Summary: Call this member to get toolbars count.
	int GetCount();

	// Input:	nIndex - Index of the toolbar.
	// Returns: A pointer to a CXTPToolBar object
	// Summary: Call this member to retrieve a toolbar with the specified index.
	CXTPToolBar* GetAt(int nIndex);
	
	// Returns: TRUE if command bars in customize mode; otherwise returns FALSE
	// Summary: Call this member to retrieve the  customize mode of the command bars.
	BOOL IsCustomizeMode() { return m_bCustomizeMode;}
	
	// Input:	bMode - TRUE to set customize mode.
	// Summary: Call this member to set customize mode of the command bars.
	void SetCustomizeMode(BOOL bMode);

	// Returns: A pointer to a CXTPCustomizeDropSource object
	// Summary: Call this member to retrieve the CXTPCustomizeDropSource helper class for customization.
	CXTPCustomizeDropSource* GetDropSource();
	
	// Input:	pControl - Points to a CXTPControl object
	// Summary: This method is called when customized control changed.
	void SetDragControl(CXTPControl* pControl);
	
	// Returns: A pointer to a CXTPControl object
	// Summary: Call this member to retrieve the current customized control.
	CXTPControl* GetDragControl();
	
	// Returns: TRUE if customize avail; otherwise returns FALSE
	// Summary: Call this member to retrieve if the customize is avail.
	BOOL IsCustomizeAvail();
	
	// Input:	pToolBar - Points to a CXTPToolBar object to remove.
	// Summary: Call this member to remove the toolbar.
	void Remove(CXTPToolBar* pToolBar);
	
	// Input:	pBar - Points to a CXTPToolBar object
	//			lpRect - Position to dock.
	//			pDockBar - Dock bar needed to be docked.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Call this member to dock a toolbar to the specified dockbar.
	BOOL DockCommandBar(CXTPToolBar* pBar, LPRECT lpRect, CXTPDockBar* pDockBar);

	// Input:	pPaintManager - Points to a CXTPPaintManager object
	// Summary: Call this member to set the specified paint manager.
	void SetPaintManager(CXTPPaintManager* pPaintManager);
	
	// Input:	pImageManager - Points to a CXTPPaintManager object
	// Summary: Call this member to set the specified image manager.
	void SetImageManager(CXTPImageManager* pImageManager);
	
	// Returns: A pointer to a CXTPPaintManager object
	// Summary: Call this member to retrieve the paint manager of the command bars.
	CXTPPaintManager* GetPaintManager();
	
	// Returns: A pointer to a CXTPImageManager object
	// Summary: Call this member to retrieve the image manager of the command bars.
	CXTPImageManager* GetImageManager();

	// Inputs:  nIndex - Index of the toolbar ned to show or hide.
	// Summary: Call this member to show/hide the toolbar.
	void ToggleVisible(int nIndex);
private:

	BOOL FloatCommandBar(CXTPToolBar* pBar);
	BOOL DockCommandBar(CXTPToolBar* pBar, XTPBarPosition xtpPosition);
	void GetDockState(CXTPDockState& state);
	void SetDockState(CXTPDockState& state);
	void SaveDockBarsState(LPCTSTR lpszProfileName);
	void LoadDockBarsState(LPCTSTR lpszProfileName);
	void SerializeDockBarsState(CArchive& ar);
	void GenerateCommandBarList(CXTPCommandBarList* pCommandBarList, BOOL bOnlyCustomized = TRUE);
	void SerializeCommandBars(CArchive& ar);
	BOOL SaveCommandBarList(CArchive& ar, CXTPCommandBarList* pCommandBarList);
	BOOL LoadCommandBarList(CArchive& ar, CXTPCommandBarList* pCommandBarList);	
	void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList, BOOL bSilent = FALSE);	
	CXTPToolBar* AddCustomBar(LPCTSTR lpcstrCaption, UINT nID, BOOL bTearOffBar = FALSE);	
	void _GetAddOrRemovePopup(CXTPToolBar* pToolBar, CXTPCommandBar* pExpandBar);	
	void _LoadControlsPart(CFile& file, BOOL bSilent);

	BOOL GetControlsFileName(CString& strFileName, LPCTSTR lpszProfileName);
	CDocTemplate* FindDocTemplate(CMDIChildWnd* pChild);

public:
	CXTPControls* m_pDesignerControls;		// Designer controls if used.
	HACCEL m_hDesignerAccels;				// Designed accelerators.
	BOOL m_bIgnoreShiftMenuKey;				// if set menus will ignore Shift+Alt key combinations

//:Ignore

private:
	CXTPCustomizeDropSource* m_pDropSource;	// Customize helper class.

	CXTPPaintManager* m_pPaintManager;		// Paint manager.
	CXTPImageManager* m_pImageManager;		// Image manager

	CXTPControl* m_pDragSelected;			// Customize control.
	BOOL m_bCustomizeMode;					// Customize mode.
	BOOL m_bCustomizeAvail;					// TRUE if customize available.
	int  m_nDefaultMenuID;					// Default menu resource.
	BOOL m_bRecalcLayout;



	XTP_COMMANDBARS_OPTIONS* m_pOptions;	// Command bars' options

	CList <CXTPToolBar*, CXTPToolBar*> m_lstBars; 
	CXTPDockBar* m_pDocks[4];
	CWnd* m_pFrame;
	CXTPMenuBar* m_pMenuBar;

private:	
	DECLARE_DYNCREATE(CXTPCommandBars)

	friend class CXTPCommandBar;
	friend class CXTPToolBar;
	friend class CXTPDockContext;
	friend class CXTPDockState;
	friend class CXTPControlToolbars;
	friend class CXTPControl;
	friend class CCommandBarsCtrl;
	friend class CXTPCustomizeToolbarsPage;
	friend class CXTPPopupBar;
	friend struct XTP_COMMANDBARS_OPTIONS;


//:End Ignore
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE CXTPMenuBar* CXTPCommandBars::GetMenuBar() { 
	return m_pMenuBar;
}
AFX_INLINE CWnd* CXTPCommandBars::GetSite() { 
	return m_pFrame;
}
AFX_INLINE CXTPDockBar* CXTPCommandBars::GetDockBar(XTPBarPosition xtpPosition) { 
	return m_pDocks[xtpPosition];
}
AFX_INLINE int CXTPCommandBars::GetCount() {
	return (int)m_lstBars.GetCount();
}
AFX_INLINE CXTPCustomizeDropSource* CXTPCommandBars::GetDropSource() {
	return m_pDropSource;
}
AFX_INLINE CXTPControl* CXTPCommandBars::GetDragControl() {
	return  m_pDragSelected;
}
AFX_INLINE BOOL CXTPCommandBars::IsCustomizeAvail() {
	return m_bCustomizeAvail;
}

AFX_INLINE XTP_COMMANDBARS_OPTIONS* CXTPCommandBars::GetCommandBarsOptions() {
	return m_pOptions;
}


#endif //#if !defined(__XTPCOMMANDBARS_H__)
