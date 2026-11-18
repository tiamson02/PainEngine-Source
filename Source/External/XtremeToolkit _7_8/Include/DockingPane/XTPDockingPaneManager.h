// XTPDockingPaneManager.h : interface for the CXTPDockingPaneManager class.
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
#if !defined(__XTPDOCKINGPANEMANAGER_H__)
#define __XTPDOCKINGPANEMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneDefines.h"

class CXTPDockingPane;
class CXTPDockingPaneBase;
class CXTPDockingPaneBase;
class CXTPDockingPaneManager;
class CXTPDockingPaneSplitterContainer;
class CXTPDockingPaneTabbedContainer;
class CXTPDockingPaneMiniWnd;
class CXTPDockingPaneContext;
class CXTPDockingPaneLayout;
class CXTPDockingPanePaintManager;
class CXTPImageManagerIcon;
class CXTPImageManager;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneManager is a CWnd derived class. It is used to manipulate
//			docking panes.
class _XTP_EXT_CLASS CXTPDockingPaneManager: public CWnd
{
public:	

	// Summary: Constructs a CXTPDockingPaneManager object
	CXTPDockingPaneManager();


	// Summary: Destroys a CXTPDockingPaneManager object, handles cleanup and de-allocation
	~CXTPDockingPaneManager();

	// Input:	pParent - Parent frame of the panes.
	// Summary: Call this member function to install docking panes. You must call it before any other members.
	void InstallDockingPanes(CFrameWnd* pParent);

	// BULLETED LIST:

	// Input:	nID - Unique identifier of the pane.
	//			rc - Initial size of the pane.
	//			direction - Docking direction. It can be one of the following:
	//			[ul]
	//			[li]<b>dockTopOf</b> - To dock pane to the top of the neighbor.[/li]
	//			[li]<b>dockLeftOf</b> - To dock pane to the left of the neighbor.[/li]
	//			[li]<b>dockRightOf</b> - To dock pane to the right of the neighbor.[/li]
	//			[li]<b>dockBottomOf</b> - To dock pane to the bottom of the neighbor.[/li]
	//			[/ul]
	//			pNeighbour - Pane's Neighbor. It can be NULL to use the frame as a neighbor.
	// Returns:	The pointer to the created pane.
	// Summary:	Call this member function to create a docking pane.
	CXTPDockingPane* CreatePane(UINT nID, CRect rc, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	// Input:	pPane - Pane to be docked.
	//			direction - Docking direction.
	//			pNeighbour - Pane's Neighbor.
	// Summary: Call this member to dock an existing pane.
	void DockPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	// Input:	pPane - Pane to be floated.
	//			rc - Floating rectangle.
	// Summary: Call this member to float an existing pane.
	void FloatPane(CXTPDockingPaneBase* pPane, CRect rc);

	// Input:	pPane - Pane to be attached.
	//			pNeighbour - Pane's Neighbor.
	// Summary: Call this member to attach a pane to another one.
	//
	void AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour);

	// Input:	pPane - Docking Pane.
	// Summary: Call this member to switch the pane's position (docking/floating).
	void ToggleDocking(CXTPDockingPaneBase* pPane);

	// Input:	nID - Pane's identifier.
	// Summary:	Call this member to show (activate) a pane by its identifier.
	void ShowPane(int nID);

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary:	Call this member to show (activate) a pane by its identifier.
	void ShowPane(CXTPDockingPane* pPane);

	// Input:	nID - Pane's identifier.
	// Summary: Call this member to close a pane by its identifier.
	void ClosePane(int nID);
	
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to close a pane.
	void ClosePane(CXTPDockingPane* pPane);

	// Input:	nID - Pane's identifier.
	// Summary: Call this member to destroy a pane by its identifier.
	void DestroyPane(int nID);

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to destroy a pane.
	void DestroyPane(CXTPDockingPane* pPane);

	// Input:	nID - Pane's identifier.
	// Summary: Call this member to hide a pane.
	void HidePane(int nID);

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to hide a pane.
	void HidePane(CXTPDockingPaneBase* pPane);

	// Input:	nID - Pane's identifier.
	// Returns: A pointer to a CXTPDockingPane object.
	// Summary: Call this member to find a pane by its identifier.
	CXTPDockingPane* FindPane(int nID);	

	// Input:	nID - Pane's identifier.
	// Returns: TRUE if the pane is closed.
	// Summary: Call this member to check if the pane is closed.
	BOOL IsPaneClosed(int nID);

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Returns: TRUE if the pane is closed.
	// Summary: Call this member to check if the pane is closed.
	BOOL IsPaneClosed(CXTPDockingPane* pPane);

	// Input:	nID - Pane's identifier.
	// Returns: TRUE if the pane is hidden.
	// Summary: Call this member to check if the pane is hidden.
	BOOL IsPaneHidden(int nID);

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Returns: TRUE if the pane is hidden.
	// Summary: Call this member to check if the pane is hidden.
	BOOL IsPaneHidden(CXTPDockingPane* pPane);

	// Input:	nID - Pane's identifier.
	// Returns: TRUE if the pane is selected.
	// Summary: Call this member to check if the pane is selected.
	BOOL IsPaneSelected(int nID);	

	// Input:	pPane - Points to a CXTPDockingPane object.
	// Returns: TRUE if the pane is selected.
	// Summary: Call this member to check if the pane is selected.
	BOOL IsPaneSelected(CXTPDockingPane* pPane);	

	// Input:	nIDResource - Resource Identifier.
	//			nIDs - Pointer to an array of pane Ids.
	//			nCount - Number of elements in the array pointed to by nIDs.
	//			clrMask - RGB value of transparent color.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary: Call this member to set icons to panes.
	BOOL SetIcons(UINT nIDResource, const int* nIDs, int nCount, COLORREF clrMask = 0xC0C0C0);

	// Input:	nID - Docking pane's identifier.
	//			hIcon - Icon handle.	
	// Summary: Call this member to set an icon to the pane.
	void SetIcon(UINT nID, HICON hIcon, BOOL bAlpha = FALSE);

	// Input:	nID - Docking pane's identifier.
	// Returns: The docking pane's icon.
	// Summary: Call this member to get an icon of the pane.
	CXTPImageManagerIcon* GetIcon(UINT nID);

	// Summary: Call this member to remove all icons.
	void ClearIconMap();
	
	// Input:	dAnimationDelay - Animation delay.
	// Summary: Call this member to set speed of animation.
	//			The default value is 0.01
	void SetAnimationDelay(double dAnimationDelay = -1, int nAnimationDuration = 128, int nAnimationInterval = 16);

	
	// Returns: Animation delay.
	// Summary: Call this member to get animation speed.
	double GetAnimationDelay(int* pAnimationDuration = NULL, int* pAnimationInterval = NULL);


public:


	// Returns: A list of created panes.
	// Summary:	Call this member to get the list of the created panes.
	CXTPDockingPaneInfoList& GetPaneList();

	// Returns: The docking site. 
	// Summary: Call this member to return docking site of the manager.
	CFrameWnd* GetSite();

	// BULLETED LIST:

	// Input:	theme - New visual theme. It can be one of the following:
	//			[ul]
	//			[li]<b>xtpPaneThemeDefault</b> - Default theme.[/li]
	//			[li]<b>xtpPaneThemeOffice</b> - Visual Studio .NET like theme.[/li]
	//			[li]<b>xtpPaneThemeGrippered</b> - Visual Studio 6 like theme.[/li]
	//			[li]<b>xtpPaneThemeVisio</b> - Visio like theme.[/li]
	//			[/ul]
	// Summary: Call this member to switch the visual theme of the panes.
	void SetTheme(XTPDockingPanePaintTheme theme);
	
	// Input:	pTheme - New Theme.
	// Summary: Call this member to set a custom theme. See the DockingPane sample
	//			as a sample of creating a new theme.
	void SetCustomTheme(CXTPDockingPanePaintManager* pTheme);

	// Returns: The current theme.
	// Summary: Call this member to get current visual theme.
	XTPDockingPanePaintTheme GetCurrentTheme();
	
	// Summary: Returns paint manager.
	CXTPDockingPanePaintManager* GetPaintManager();

	// Returns: A pointer to a CXTPDockingPaneLayout object.
	// Summary: Call this member to create a new layout.
	CXTPDockingPaneLayout* CreateLayout();
	
	// Input:	pLayout - Pointer to the existing layout.
	// Summary: Call this member to get the current layout.
	void GetLayout(CXTPDockingPaneLayout* pLayout);

	// Input:	pLayout - Pointer to the existing layout.
	// Summary: Call this member to set the layout.
	void SetLayout(CXTPDockingPaneLayout* pLayout);

	// Summary: Call this member to redraw all panes.
	void RedrawPanes();

	// Returns: A pointer to a CXTPDockingPaneBase object.
	// Summary: Call this member to retrieve client pane.
	CXTPDockingPaneBase* GetClientPane();
	
	// Summary: Call this member to destroy all panes.
	void DestroyAll();
	
	// Summary: Call this member to close all panes.
	void CloseAll();
	
	// Summary: Call this member to retrieve Image Manager of Docking Panes.
	CXTPImageManager* GetImageManager();

	// Input: bHide - TRUE to hide client.
	// Summary: Call this member to hide client area.
	void HideClient(BOOL bHide);
	BOOL IsClientHidden();

	// Summary: Call this member to use trackers.
	void UseSplitterTracker(BOOL bSplitterTracker);
	BOOL IsSplitterTrackerUsed();

	// Summary: Callthis method to lock splitters.
	void LockSplitters(BOOL bLock = TRUE);
	BOOL IsSplittersLocked();




private:
	CXTPDockingPaneBase* GetTopPane();
	CXTPDockingPaneLayout* GetCurrentLayout();

	void _InsertPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour );
	void _RemovePane(CXTPDockingPaneBase* pPane);	
	void _AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour );
	CXTPDockingPaneBase* _GetHolder(CXTPDockingPaneBase* pPane, BOOL bFloating);
	CXTPDockingPaneBase* _Clone(CXTPDockingPaneBase* pPane);

	void _SyncPanesState();
	void _Redraw();
	void _DetachAll();
	BOOL _ToggleDocking(CXTPDockingPane* pPane, CXTPDockingPaneBase* pHolder);
	BOOL _OnAction(XTPDockingPaneAction action, CXTPDockingPane* pPane, CXTPDockingPaneBase* pDockContainer = NULL, XTPDockingPaneDirection dockDirection = dockLeftOf);

	void UpdatePanes();

private:
	CFrameWnd* m_pSite;
	CXTPDockingPaneLayout* m_pLayout;
	CXTPDockingPanePaintManager* m_pPaintManager;

	//CMap<UINT, UINT, HICON, HICON> m_mapIcons;
	CXTPImageManager* m_pImageManager;


	BOOL m_bHideClient;
	BOOL m_bUseSplitterTracker;

	BOOL m_bAttachingPane;    
	BOOL m_bLockSplitters;


	
protected:
//:Ignore
	//{{AFX_MSG(CXTPDockingPaneManager)
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	//}}AFX_MSG
//:End Ignore

private:
	DECLARE_MESSAGE_MAP()
	
	friend class CXTPDockingPaneBase;				
	friend class CXTPDockingPaneSplitterContainer;	
	friend class CXTPDockingPaneTabbedContainer;	
	friend class CXTPDockingPaneMiniWnd;			
	friend class CXTPDockingPaneAutoHideWnd;		
	friend class CXTPDockingPaneContext;			
	friend class CXTPDockingPane;					
	friend class CXTPDockingPaneLayout;	
	friend class CXTPDockingPaneAutoHidePanel;
	friend class CDockingPaneSite;
	friend class CDockingPaneOptions;
	
#ifdef _XTP_COMPONENT
	CXTPDockingPaneManager* GetDockingPaneManager() { return this; }
#endif

};

AFX_INLINE CFrameWnd* CXTPDockingPaneManager::GetSite() {
	return m_pSite;
}
AFX_INLINE CXTPDockingPaneLayout* CXTPDockingPaneManager::GetCurrentLayout() {
	return m_pLayout;
}

AFX_INLINE void CXTPDockingPaneManager::RedrawPanes() {
	_Redraw();
}
AFX_INLINE CXTPDockingPanePaintManager* CXTPDockingPaneManager::GetPaintManager() {
	return m_pPaintManager;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsClientHidden() {
	return m_bHideClient;
}

AFX_INLINE void CXTPDockingPaneManager::UseSplitterTracker(BOOL bSplitterTracker) {
	m_bUseSplitterTracker = bSplitterTracker;
	
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsSplitterTrackerUsed() {
	return m_bUseSplitterTracker;
}
AFX_INLINE CXTPImageManager* CXTPDockingPaneManager::GetImageManager() {
	return m_pImageManager;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsSplittersLocked() {
	return m_bLockSplitters;
}


#endif //#if !defined(__XTPDOCKINGPANEMANAGER_H__)