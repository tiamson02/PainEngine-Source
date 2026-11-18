// XTPDockingPane.h : interface for the CXTPDockingPane class.
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
#if !defined(__XTPDOCKINGPANE_H__)
#define __XTPDOCKINGPANE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneBase.h"

class CXTPImageManagerIcon;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPane is a CXTPDockingPaneBase derived class. 
//			It is used as a container for a user window.
class _XTP_EXT_CLASS CXTPDockingPane : public CCmdTarget, public CXTPDockingPaneBase
{

protected:

	// Input:	nID - Identifier of the pane.
	//			rc - Size of the pane.
	//			pLayout - Points to a CXTPDockingPaneLayout object
	// Summary: Constructs a CXTPDockingPane object
	//			Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	CXTPDockingPane(CXTPDockingPaneLayout* pLayout);
	
	// Input:	nID - Identifier of the pane.
	//			rc - Size of the pane.
	//			pLayout - Points to a CXTPDockingPaneLayout object
	// Summary: Constructs a CXTPDockingPane object
	//			Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	CXTPDockingPane(UINT nID, CRect rc, CXTPDockingPaneLayout* pLayout);
	
	// Input:	pClone - Points to a CXTPDockingPane object
	//			pLayout - Points to a CXTPDockingPaneLayout object
	//			pMap - Points to a CXTPPaneToPaneMap object
	// Summary: Constructs a CXTPDockingPane object
	//			Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	CXTPDockingPane(CXTPDockingPane* pClone, CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap);

	// Summary: Destroys a CXTPDockingPane object, handles cleanup and de-allocation.
	virtual ~CXTPDockingPane();

public:


	// Input:	str - Title to be set.
	// Summary: Call this member to set a title for this object.
	void SetTitle(CString str);

	// Summary: Call this member to set the focus of the pane.
	void SetFocus();

	// Returns: The identifier of the pane.
	// Summary: Call this member to get pane's identifier.
	int GetID();

	// Returns: The title of the pane.
	// Summary: Call this member to get the title of the pane.
	CString GetTitle();
	
	// Returns: The short title of the pane.
	// Summary: Call this member to get the short title of the pane.
	CString GetShortTitle();

	// Summary: Call this member to close the pane.
	void Close();

	// Summary: Call this member to hide the pane.
	void Hide();
	
	// Returns: TRUE if the pane is associated with a user window.
	// Summary: Call this member to check if the pane is associated with a user window.
	BOOL IsValid();
	
	// Returns: TRUE if the pane is closed.
	// Summary: Call this member to check if the pane is closed.
	BOOL IsClosed();

	// Returns: TRUE if the pane is hidden.
	// Summary: Call this member to check if the pane is hidden.
	BOOL IsHidden();

	// Returns: TRUE if the pane is selected.
	// Summary: Call this member to check if the pane is selected.
	BOOL IsSelected();

	// Returns: TRUE if the pane is floating.
	// Summary: Call this member to check if the pane is floating.
	BOOL IsFloating();

	// Input:	pWnd - Pointer to a user window.
	// Summary: Call this member to associate the pane with a user window.
	//			NOTE: the best place to create a user window and associate it with a 
	//			pane is after the pane becomes visible. To catch this event use 
	//			the XTPWM_DOCKINGPANE_NOTIFY Handler.
	void Attach(CWnd* pWnd);

	// Input:	pParentWnd - Points to the parent of the view to be created. The parent must be
	//			valid.
	//			pViewClass - CView runtime class.
	//			pDocument - CDocument associated with the view. It can be NULL.
	//			pContext - Create context for the view. It can be NULL.
	//			dwStyle - Default style for the view.
	//			pOwnerWnd - Owner of the view. If NULL, 'pParentWnd' is used.
	//			nID - Control ID of the view.
	// Returns: A CWnd* pointer to the newly created view if successful, 
	//			otherwise returns NULL.
	// Summary:	This member function dynamically creates a view based on a CRuntimeClass
	//			object and attaches it to the pane.
	//			NOTE: the best place to create a user window and associate it with a 
	//			pane is after the pane becomes visible. To catch this event use 
	//			the XTPWM_DOCKINGPANE_NOTIFY Handler.
	CWnd* AttachView(CWnd* pParentWnd, CRuntimeClass *pViewClass, CDocument *pDocument=NULL, CCreateContext *pContext=NULL, DWORD dwStyle=AFX_WS_DEFAULT_VIEW, CWnd* pOwnerWnd=NULL, UINT nID=AFX_IDW_PANE_FIRST);

	// Summary: Call this member to disassociate the pane from a user window.
	void Detach();

	// Summary: Call this member to select the pane.
	void Select();
	
	// Summary: Returns identifier of icon if set, otherwise returns pane's identifier
	int GetIconID();
	
	// Returns: Handle of the icon.
	// Summary: Retrieves icon associated with the pane.
	CXTPImageManagerIcon* GetIcon();

	// Summary: Call this member to set the identifier of the icon.
	void SetIconID(UINT nID);

	// Summary: Call this member to get the associated user window.
	CWnd* GetChild();


	// Input:   dwOptions - Options to be set. It can be one or more of the following:
	//			[ul]
	//			[li]<b>xtpPaneNoCloseable</b> - Pane can't be closed.[/li]
	//			[li]<b>xtpPaneNoHideable</b> - Pane can't be hidden.[/li]
	//			[/ul]
	// Summary: Call this member to set options of the pane.
	void SetOptions(DWORD dwOptions);

	// Summary: Call this member to get options of the pane.
	DWORD GetOptions();

	void SetPaneData(DWORD dwData);
	DWORD GetPaneData();

public:

	// Returns: TRUE if child window is focused.
	// Summary: Retrieves the state of the child window.
	BOOL IsFocus();


	
private:
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);
	virtual void SetParentContainer(CXTPDockingPaneBase* pContainer);
	CFrameWnd* GetParentFrame();
	virtual void OnIdleUpdate();
	CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	void ShowWindow(BOOL bShow);

private:
	HWND	m_hwndChild;
	long	m_nID;
	CString m_strTitle;

	int		m_nIconID;

	DWORD   m_dwOptions;
	DWORD   m_dwData;

//:Ignore
	
#ifdef _XTP_COMPONENT

public:
	void SetID(long id)
	{
		m_nID=id;
	}
	// Added for use through the the .Net wrapper class Pane
	void SetHandle(long hWnd)
	{
		OleSetHandle(hWnd);
	}
	void OleSetHandle(long nHandle);

	// Added for use through the the .Net wrapper class Pane	
	long GetHandle()
	{
		return (long)m_hwndChild;
	}
#endif
#ifdef _XTP_ACTIVEX

private:
	DECLARE_DYNAMIC(CXTPDockingPane)
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPDockingPane);

	afx_msg long OleGetHandle();
	afx_msg void OleAttachTo(LPDISPATCH pNeighbourDisp);
	afx_msg void OleSetHidden(BOOL bHidden);
	afx_msg void OleSetClosed(BOOL bClosed);
	afx_msg void OleSetSelected(BOOL bSelected);

	enum {
	dispidTitle = 1L,
	dispidId = 2L, 
	dispidIconId = 3L, 
	dispidHidden = 4L, 
	dispidClosed = 5L, 
	dispidSelected = 6L, 
	dispidOptions = 7L, 
	dispidFloating = 8L, 
	dispidHide = 10L,
	dispidClose = 11L,
	dispidSelect = 12L,
	dispidAttachTo = 13L,
	dispidTag = 14L,
	};

	static CXTPDockingPane* FromDispatch(LPDISPATCH pDisp)
	{
		if (pDisp == 0) return 0;
		return ((CXTPDockingPane*)((BYTE*)pDisp - offsetof(CXTPDockingPane, m_xDispatch)));
	}
	
#endif
//:End Ignore


private:
	friend class CXTPDockingPaneMiniWnd;			
	friend class CXTPDockingPaneManager;			
	friend class CXTPDockingPaneTabbedContainer;	
	friend class CXTPDockingPaneLayout;				
	friend class CDockingPaneCtrl;	

};

AFX_INLINE int  CXTPDockingPane::GetID() {
	return m_nID;
}
AFX_INLINE int CXTPDockingPane::GetIconID() {
	return m_nIconID == -1? m_nID: m_nIconID;
}
AFX_INLINE void CXTPDockingPane::SetIconID(UINT nID) { 
	m_nIconID = nID;
}
AFX_INLINE CWnd* CXTPDockingPane::GetChild() {
	return m_hwndChild? CWnd::FromHandle(m_hwndChild): NULL;
}
AFX_INLINE void CXTPDockingPane::SetOptions(DWORD dwOptions) {
	m_dwOptions = dwOptions;
}
AFX_INLINE DWORD CXTPDockingPane::GetOptions() {
	return m_dwOptions;
}

AFX_INLINE DWORD CXTPDockingPane::GetPaneData() {
	return m_dwData;
}
AFX_INLINE void CXTPDockingPane::SetPaneData(DWORD dwData) {
	m_dwData = dwData;
}


#endif // #if !defined(__XTPDOCKINGPANE_H__)