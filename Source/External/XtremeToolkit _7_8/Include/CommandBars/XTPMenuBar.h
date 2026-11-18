// XTPMenuBar.h : interface for the CXTPMenuBar class.
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
#if !defined(__XTPMENUBAR_H__)
#define __XTPMENUBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


#include "XTPToolBar.h"
#include "XTPCommandBarsDefines.h"

class CXTPCommandBars;

// Summary: CXTPMenuBar is a CXTPToolBar derived class.   
class _XTP_EXT_CLASS CXTPMenuBar : public CXTPToolBar
{
public:
	// Summary: Constructs a CXTPMenuBar object
	CXTPMenuBar(void);
	
	// Summary: Destroys a CXTPMenuBar object, handles cleanup and de-allocation
	~CXTPMenuBar(void);

public:

	// Input:	nIDResource - Menu identifier to load.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Loads the menu from resource.
	BOOL LoadMenuBar(UINT nIDResource);

protected:
	// Input:	hWnd - Window handle message belongs to.
	//			nMessage - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information. 
	//			lParam - Specifies additional message-dependent information.
	//			lResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Summary: This member function is called by WindowProc, or is called during message reflection.
	int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	// Summary: Resets the controls to their original state.
	virtual void Reset();
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
 	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method makes a copy of the command bar.
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

private:
	void SetupHook();
	HWND GetActiveMdiChildWnd(BOOL* bMaximized = NULL);
	void SyncActiveMdiChild();
	void RefreshMenu();
	void RefreshSysButtons();
	void SwitchMDIMenu(HMENU hMenu);
	void FreeMDIMenus();
	void CleanupMDIMenus();

	void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList);
	void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);

private:
	BOOL m_bMdiApp;
	HWND m_hWndActiveChild;
	HMENU m_hMenuDefault;

	UINT m_nIDResource;
	UINT m_nIDResourceCurrent;

	BOOL m_bDirtyMenu;

	CMap<UINT, UINT, CXTPControls*, CXTPControls*> m_mapMDIMenus;

private:
	DECLARE_MESSAGE_MAP()					

	//{{AFX_MSG(CXTPMenuBar)
	afx_msg void OnTimer(UINT nIDEvent);	
	//}}AFX_MSG

//:Ignore
#ifdef _XTP_DOTNET
public:
	void SetupMDIHook(HWND mdiClient);
#endif
//:End Ignore

private:
	DECLARE_XTP_COMMANDBAR(CXTPMenuBar)

	class CXTPControlMDIButton;  
	class CXTPControlMDISysMenuPopup;

	friend class CXTPCommandBars;				
	friend class CXTPControlMDIButton;			
	friend class CCommandBarsSite;

};


#endif //#if !defined(__XTPMENUBAR_H__)