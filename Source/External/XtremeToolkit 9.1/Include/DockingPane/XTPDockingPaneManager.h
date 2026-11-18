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
#include "CommonPro.include/XTPImageManager.h"

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

//===========================================================================
// Remarks: CXTPDockingPaneManager is a CWnd derived class. It is used to manipulate
//			docking panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneManager: public CWnd
{
public:	

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPaneManager object
	//-----------------------------------------------------------------------
	CXTPDockingPaneManager();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneManager object, handles cleanup and 
	//			de-allocation
	//-----------------------------------------------------------------------
	~CXTPDockingPaneManager();

	//-----------------------------------------------------------------------
	// Input:	pParent - Parent frame of the panes.
	// Summary: Call this member function to install docking panes. 
	// Remarks: You must call it before any other members.
	//-----------------------------------------------------------------------
	void InstallDockingPanes(CFrameWnd* pParent);

	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	nID		  - Unique identifier of the pane.
	//			rc		  - Initial size of the pane.
	//			direction - Docking direction. It can be one of the following:
	//			[ul]
	//			[li]<b>dockTopOf</b>    - To dock pane to the top of the neighbor.[/li]
	//			[li]<b>dockLeftOf</b>   - To dock pane to the left of the neighbor.[/li]
	//			[li]<b>dockRightOf</b>  - To dock pane to the right of the neighbor.[/li]
	//			[li]<b>dockBottomOf</b> - To dock pane to the bottom of the neighbor.[/li]
	//			[/ul]
	//			pNeighbour - Pane's Neighbor. It can be NULL to use the frame as a neighbor.
	// Summary:	Call this member function to create a docking pane.
	// Returns:	The pointer to the created pane.
	//-----------------------------------------------------------------------
	CXTPDockingPane* CreatePane(UINT nID, CRect rc, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	//-----------------------------------------------------------------------
	// Input:	pPane      - Pane to be docked.
	//			direction  - Docking direction.
	//			pNeighbour - Pane's Neighbor.
	// Summary: Call this member to dock an existing pane.
	//-----------------------------------------------------------------------
	void DockPane(CXTPDockingPaneBase* pPane, XTPDockingPaneDirection direction, CXTPDockingPaneBase* pNeighbour = NULL);

	//-----------------------------------------------------------------------
	// Input:	pPane - Pane to be floated.
	//			rc    - Floating rectangle.
	// Summary: Call this member to float an existing pane.
	//-----------------------------------------------------------------------
	void FloatPane(CXTPDockingPaneBase* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Input:	pPane      - Pane to be attached.
	//			pNeighbour - Pane's Neighbor.
	// Summary: Call this member to attach a pane to another one.
	//-----------------------------------------------------------------------
	void AttachPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour);

	//-----------------------------------------------------------------------
	// Input:	pPane - Docking Pane.
	// Summary: Call this member to switch the pane's position (docking/floating).
	//-----------------------------------------------------------------------
	void ToggleDocking(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary:	Call this member to show (activate) a pane by its identifier.
	//-----------------------------------------------------------------------
	void ShowPane(int nID);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary:	Call this member to show (activate) a pane by its identifier.
	//-----------------------------------------------------------------------
	void ShowPane(CXTPDockingPane* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to close a pane by its identifier.
	//-----------------------------------------------------------------------
	void ClosePane(int nID);
	
	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to close a pane.
	//-----------------------------------------------------------------------
	void ClosePane(CXTPDockingPane* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to destroy a pane by its identifier.
	//-----------------------------------------------------------------------
	void DestroyPane(int nID);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to destroy a pane.
	//-----------------------------------------------------------------------
	void DestroyPane(CXTPDockingPane* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to hide a pane.
	//-----------------------------------------------------------------------
	void HidePane(int nID);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to hide a pane.
	//-----------------------------------------------------------------------
	void HidePane(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to find a pane by its identifier.
	// Returns: A pointer to a CXTPDockingPane object.
	//-----------------------------------------------------------------------
	CXTPDockingPane* FindPane(int nID);	

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to check if the pane is closed.
	// Returns: TRUE if the pane is closed.
	//-----------------------------------------------------------------------
	BOOL IsPaneClosed(int nID);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to check if the pane is closed.
	// Returns: TRUE if the pane is closed.
	//-----------------------------------------------------------------------
	BOOL IsPaneClosed(CXTPDockingPane* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to check if the pane is hidden.
	// Returns: TRUE if the pane is hidden.
	//-----------------------------------------------------------------------
	BOOL IsPaneHidden(int nID);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to check if the pane is hidden.
	// Returns: TRUE if the pane is hidden.
	//-----------------------------------------------------------------------
	BOOL IsPaneHidden(CXTPDockingPane* pPane);

	//-----------------------------------------------------------------------
	// Input:	nID - Pane's identifier.
	// Summary: Call this member to check if the pane is selected.
	// Returns: TRUE if the pane is selected.
	//-----------------------------------------------------------------------
	BOOL IsPaneSelected(int nID);	

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPane object.
	// Summary: Call this member to check if the pane is selected.
	// Returns: TRUE if the pane is selected.
	//-----------------------------------------------------------------------
	BOOL IsPaneSelected(CXTPDockingPane* pPane);	

	//-----------------------------------------------------------------------
	// Input:	nIDResource - Resource Identifier.
	//			nIDs        - Pointer to an array of pane Ids.
	//			nCount      - Number of elements in the array pointed to by nIDs.
	//			clrMask     - RGB value of transparent color.
	// Summary: Call this member to set icons to panes.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetIcons(UINT nIDResource, const int* nIDs, int nCount, COLORREF clrMask = 0xC0C0C0);

	//-----------------------------------------------------------------------
	// Input:	nID   - Docking pane's identifier.
	//			hIcon - Icon handle.	
	// Summary: Call this member to set an icon to the pane.
	//-----------------------------------------------------------------------
	void SetIcon(UINT nID, CXTPImageManagerIconHandle hIcon);

	//-----------------------------------------------------------------------
	// Input:	nID - Docking pane's identifier.
	// Summary: Call this member to get an icon of the pane.
	// Returns: The docking pane's icon.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon(UINT nID);

	//-----------------------------------------------------------------------
	// Summary: Call this member to remove all icons.
	//-----------------------------------------------------------------------
	void ClearIconMap();
	
	//-----------------------------------------------------------------------
	// Input:	dAnimationDelay - Animation delay.
	// Summary: Call this member to set the speed of animation.
	//			The default value is 0.01
	//-----------------------------------------------------------------------
	void SetAnimationDelay(double dAnimationDelay = -1, int nAnimationDuration = 128, int nAnimationInterval = 16);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the animation speed.
	// Returns: Animation delay.
	//-----------------------------------------------------------------------
	double GetAnimationDelay(int* pAnimationDuration = NULL, int* pAnimationInterval = NULL);


public:

	//-----------------------------------------------------------------------
	// Summary:	Call this member to get the list of the created panes.
	// Returns: A list of created panes.
	//-----------------------------------------------------------------------
	CXTPDockingPaneInfoList& GetPaneList();

	//-----------------------------------------------------------------------
	// Summary: Call this member to return the docking site of the manager.
	// Returns: The docking site. 
	//-----------------------------------------------------------------------
	CFrameWnd* GetSite();

	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	theme - New visual theme. It can be one of the following:
	//			[ul]
	//			[li]<b>xtpPaneThemeDefault</b> - Default theme.[/li]
	//			[li]<b>xtpPaneThemeOffice</b> - Visual Studio .NET like theme.[/li]
	//			[li]<b>xtpPaneThemeGrippered</b> - Visual Studio 6 like theme.[/li]
	//			[li]<b>xtpPaneThemeVisio</b> - Visio like theme.[/li]
	//			[/ul]
	// Summary: Call this member to switch the visual theme of the panes.
	//-----------------------------------------------------------------------
	void SetTheme(XTPDockingPanePaintTheme theme);
	
	//-----------------------------------------------------------------------
	// Input:	pTheme - New Theme.
	// Summary: Call this member to set a custom theme. See the DockingPane sample
	//			as a sample of creating a new theme.
	//-----------------------------------------------------------------------
	void SetCustomTheme(CXTPDockingPanePaintManager* pTheme);

	//-----------------------------------------------------------------------
	// Returns: The current theme.
	// Summary: Call this member to get the current visual theme.
	//-----------------------------------------------------------------------
	XTPDockingPanePaintTheme GetCurrentTheme();
	
	//-----------------------------------------------------------------------
	// Summary: Returns paint manager.
	//-----------------------------------------------------------------------
	CXTPDockingPanePaintManager* GetPaintManager();

	//-----------------------------------------------------------------------
	// Summary: Call this member to create a new layout.
	// Returns: A pointer to a CXTPDockingPaneLayout object.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneLayout* CreateLayout();
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Pointer to the existing layout.
	// Summary: Call this member to get the current layout.
	//-----------------------------------------------------------------------
	void GetLayout(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Pointer to the existing layout.
	// Summary: Call this member to set the layout.
	//-----------------------------------------------------------------------
	void SetLayout(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary: Call this member to redraw all panes.
	//-----------------------------------------------------------------------
	void RedrawPanes();

	//-----------------------------------------------------------------------
	// Summary: Call this member to retrieve client pane.
	// Returns: A pointer to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetClientPane();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to destroy all panes.
	//-----------------------------------------------------------------------
	void DestroyAll();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to close all panes.
	//-----------------------------------------------------------------------
	void CloseAll();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to retrieve Image Manager of Docking Panes.
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager();

	//-----------------------------------------------------------------------
	// Input:	pImageManager - Points to a CXTPImageManager object.
	// Summary: Call this member to set the specified image manager.
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Input:	bHide - TRUE to hide client.
	// Summary: Call this member to hide client area.
	//-----------------------------------------------------------------------
	void HideClient(BOOL bHide);

	//-----------------------------------------------------------------------
	// Summary: Checks to see if the client is hidden.
	// Returns: TRUE if the client is hidden, FALSE if otherwise
	//-----------------------------------------------------------------------
	BOOL IsClientHidden();

	//-----------------------------------------------------------------------
	// Summary: Call this member to use trackers.
	//-----------------------------------------------------------------------
	void UseSplitterTracker(BOOL bSplitterTracker);

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if the splitter tracker is used.
	// Returns: TRUE if the splitter tracker is used; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsSplitterTrackerUsed();

	//-----------------------------------------------------------------------
	// Summary: Call this method to lock splitters.
	//-----------------------------------------------------------------------
	void LockSplitters(BOOL bLock = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if the splitters are locked.
	// Returns: TRUE if the splitters are locked; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsSplittersLocked();

	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption to be set.
	// Summary: Call this method to set common caption for floating frames.
	//-----------------------------------------------------------------------
	void SetFloatingFrameCaption(CString strCaption);

	//-----------------------------------------------------------------------
	// Summary: Call this method to use themes for floating frames.
	//-----------------------------------------------------------------------
	void SetThemedFloatingFrames(BOOL bThemedFloatingFrames);
	
	//-----------------------------------------------------------------------
	// Summary: Determines if themes are used for the floating frames.
	// Returns: TRUE if they are used; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsThemedFloatingFrames();

	BOOL IsLayoutRTL();

	//-----------------------------------------------------------------------
	// Input:	direction - Docking direction
	//-----------------------------------------------------------------------
	XTPDockingPaneDirection GetRTLDirection(XTPDockingPaneDirection direction);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	//-----------------------------------------------------------------------
	XTPDockingPaneDirection GetPaneDirection(CXTPDockingPaneBase* pPane);

	CXTPDockingPaneBase* GetTopPane();

	//-----------------------------------------------------------------------
	// Input:	pDockingContext - Points to a CXTPDockingPaneContext object
	//-----------------------------------------------------------------------
	void SetDockingContext(CXTPDockingPaneContext* pDockingContext);

	CXTPDockingPaneContext* GetDockingContext();

	BOOL IsAlphaDockingContext();
	void SetAlphaDockingContext(BOOL bAlphaDockingContext);

	BOOL IsShowDockingContextStickers();
	void SetShowDockingContextStickers(BOOL bShowDockingContextStickers);
	
protected:
	
	//-----------------------------------------------------------------------
	// Input:	type    - Type of pane that is created. 
	//			pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: This method is called when a pane is created.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* OnCreatePane(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout);



	
private:
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

public:
	int m_nSplitterGap;
		
protected:
	CFrameWnd* m_pSite;
	CXTPDockingPaneLayout* m_pLayout;
	CXTPDockingPanePaintManager* m_pPaintManager;

	CXTPImageManager* m_pImageManager;

	BOOL m_bHideClient;
	BOOL m_bUseSplitterTracker;

	BOOL m_bAttachingPane;    
	BOOL m_bLockSplitters;
	BOOL m_bAlphaDockingContext;				
	BOOL m_bShowDockingContextStickers;			

	BOOL m_bThemedFloatingFrames;
	CString m_strFloatingFrameCaption;

	CXTPDockingPaneContext* m_pDockingContext;


	
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
AFX_INLINE void CXTPDockingPaneManager::SetFloatingFrameCaption(CString strCaption) {
	m_strFloatingFrameCaption = strCaption;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsThemedFloatingFrames() {
	return m_bThemedFloatingFrames;
}
AFX_INLINE CXTPDockingPaneContext* CXTPDockingPaneManager::GetDockingContext() {
	return m_pDockingContext;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsAlphaDockingContext() {
	return m_bAlphaDockingContext;
}
AFX_INLINE void CXTPDockingPaneManager::SetAlphaDockingContext(BOOL bAlphaDockingContext) {
	m_bAlphaDockingContext = bAlphaDockingContext;
}
AFX_INLINE BOOL CXTPDockingPaneManager::IsShowDockingContextStickers() {
	return m_bShowDockingContextStickers;
}
AFX_INLINE void CXTPDockingPaneManager::SetShowDockingContextStickers(BOOL bShowDockingContextStickers) {
	m_bShowDockingContextStickers = bShowDockingContextStickers;
}


#endif //#if !defined(__XTPDOCKINGPANEMANAGER_H__)