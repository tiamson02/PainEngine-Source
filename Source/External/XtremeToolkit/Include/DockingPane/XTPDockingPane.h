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

//===========================================================================
// Remarks: CXTPDockingPane is a CXTPDockingPaneBase derived class. 
//			It is used as a container for a user window.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPane : public CCmdTarget, public CXTPDockingPaneBase
{

protected:

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object
	// Summary: Constructs a CXTPDockingPane object
	//			Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	//-----------------------------------------------------------------------
	CXTPDockingPane(CXTPDockingPaneLayout* pLayout);
	
	//-----------------------------------------------------------------------
	// Input:	pClone  - Points to a CXTPDockingPane object
	//			pLayout - Points to a CXTPDockingPaneLayout object
	//			pMap    - Points to a CXTPPaneToPaneMap object
	// Summary: Constructs a CXTPDockingPane object
	//			Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPDockingPane* pClone);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPane object, handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPane();

public:

	//-----------------------------------------------------------------------
	// Input:	str - Title to be set.
	// Summary: Call this member to set a title for this object.
	//-----------------------------------------------------------------------
	void SetTitle(CString str);

	//-----------------------------------------------------------------------
	// Summary: Call this member to set the focus of the pane.
	//-----------------------------------------------------------------------
	void SetFocus();

	//-----------------------------------------------------------------------
	// Summary: Call this member to get pane's identifier.
	// Returns: The identifier of the pane.
	//-----------------------------------------------------------------------
	int GetID();

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the title of the pane.
	// Returns: The title of the pane.
	//-----------------------------------------------------------------------
	CString GetTitle();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to get the short title of the pane.
	// Returns: The short title of the pane.
	//-----------------------------------------------------------------------
	CString GetShortTitle();

	//-----------------------------------------------------------------------
	// Summary: Call this member to close the pane.
	//-----------------------------------------------------------------------
	void Close();

	//-----------------------------------------------------------------------
	// Summary: Call this member to hide the pane.
	//-----------------------------------------------------------------------
	void Hide();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to check if the pane is associated with a user window.
	// Returns: TRUE if the pane is associated with a user window.
	//-----------------------------------------------------------------------
	BOOL IsValid();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to check if the pane is closed.
	// Returns: TRUE if the pane is closed.
	//-----------------------------------------------------------------------
	BOOL IsClosed();

	//-----------------------------------------------------------------------
	// Summary: Call this member to check if the pane is hidden.
	// Returns: TRUE if the pane is hidden.
	//-----------------------------------------------------------------------
	BOOL IsHidden();

	//-----------------------------------------------------------------------
	// Summary: Call this member to check if the pane is selected.
	// Returns: TRUE if the pane is selected.
	//-----------------------------------------------------------------------
	BOOL IsSelected();

	//-----------------------------------------------------------------------
	// Summary: Call this member to check if the pane is floating.
	// Returns: TRUE if the pane is floating.
	//-----------------------------------------------------------------------
	BOOL IsFloating();

	//-----------------------------------------------------------------------
	// Input:	pWnd - Pointer to a user window.
	// Summary: Call this member to associate the pane with a user window.
	//			NOTE: the best place to create a user window and associate it with a 
	//			pane is after the pane becomes visible. To catch this event use 
	//			the XTPWM_DOCKINGPANE_NOTIFY Handler.
	//-----------------------------------------------------------------------
	void Attach(CWnd* pWnd);

	//-----------------------------------------------------------------------
	// Input:	pParentWnd - Points to the parent of the view to be created. The parent must be
	//						 valid.
	//			pViewClass - CView runtime class.
	//			pDocument  - CDocument associated with the view. It can be NULL.
	//			pContext   - Create context for the view. It can be NULL.
	// Summary:	This member function dynamically creates a view based on a CRuntimeClass
	//			object and attaches it to the pane.
	//			NOTE: the best place to create a user window and associate it with a 
	//			pane is after the pane becomes visible. To catch this event, use 
	//			the XTPWM_DOCKINGPANE_NOTIFY Handler.
	// Returns: A CWnd* pointer to the newly created view if successful, 
	//			otherwise returns NULL.
	//-----------------------------------------------------------------------
	CWnd* AttachView(CWnd* pParentWnd, CRuntimeClass *pViewClass, CDocument *pDocument=NULL, CCreateContext *pContext=NULL);

	//-----------------------------------------------------------------------
	// Summary: Call this member to disassociate the pane from a user window.
	//-----------------------------------------------------------------------
	void Detach();

	//-----------------------------------------------------------------------
	// Summary: Call this member to select the pane.
	//-----------------------------------------------------------------------
	void Select();
	
	//-----------------------------------------------------------------------
	// Summary: Returns identifier of icon if set, otherwise returns pane's identifier
	// Returns: The identifier of the icon if set, otherwise returns the idenifier
	//			of the pane
	//-----------------------------------------------------------------------
	int GetIconID();
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves icon associated with the pane.
	// Returns: Handle of the icon.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon();

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the icon
	// Summary: Call this member to set the identifier of the icon.
	//-----------------------------------------------------------------------
	void SetIconID(UINT nID);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the associated user window.
	//-----------------------------------------------------------------------
	CWnd* GetChild();

	//-----------------------------------------------------------------------
	// Input:   dwOptions - Options to be set. It can be one or more of the following:
	//			[ul]
	//			[li]<b>xtpPaneNoCloseable</b> - Pane can't be closed.[/li]
	//			[li]<b>xtpPaneNoHideable</b> - Pane can't be hidden.[/li]
	//			[/ul]
	// Summary: Call this member to set options of the pane.
	//-----------------------------------------------------------------------
	void SetOptions(DWORD dwOptions);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the options of the pane.
	// Returns: The pane's data
	//-----------------------------------------------------------------------
	DWORD GetOptions();

	//-----------------------------------------------------------------------
	// Input:	dwData - User item data (lParam).
	// Summary: Call this member function to set the data for the pane.
	//-----------------------------------------------------------------------
	void SetPaneData(DWORD dwData);

	//-----------------------------------------------------------------------
	// Summary: Call this member to the data of the pane.
	// Returns: The pane's data.
	//-----------------------------------------------------------------------
	DWORD GetPaneData();

	//-----------------------------------------------------------------------
	// Summary: This member function returns the RGB value for the referenced
	//			item text or (COLORREF)-1, if the color was not set.
	// Returns: The RGB value for the referenced item text, or (COLORREF)-1, 
	//			if the color was not set.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemColor();

	void GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo);

public:

	//-----------------------------------------------------------------------
	// Summary: Retrieves the state of the child window.
	// Returns: TRUE if child window is focused.
	//-----------------------------------------------------------------------
	BOOL IsFocus();

	//-----------------------------------------------------------------------
	// Summary: Call this method to set minimum size of pane.
	//-----------------------------------------------------------------------
	CSize SetMinTrackSize(CSize sz);

	//-----------------------------------------------------------------------
	// Summary: Call this method to set maximum size of pane.
	//-----------------------------------------------------------------------
	CSize SetMaxTrackSize(CSize sz);
		
protected:
	
	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CFrameWnd object.
	//			rect	- Rectangle of the pane to be set.
	//			lParam  - Pointer to AFX_SIZEPARENTPARAMS structure.
	// Summary: This method is called to set the position of the pane
	// ----------------------------------------------------------------------
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);

	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object.
	// Summary: Call this method to set the parent container of the pane.
	//-----------------------------------------------------------------------
	virtual void SetParentContainer(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the parent frame window
	// Remarks: The member function searches up the parent chain until a CFrameWnd 
	//			(or derived class) object is found.
	// Returns: A pointer to a frame window if successful; otherwise NULL
	//-----------------------------------------------------------------------
	CFrameWnd* GetParentFrame();
	
	//-----------------------------------------------------------------------
	// Summary: This method is called to update the state of the pane
	//-----------------------------------------------------------------------
	virtual void OnIdleUpdate();

	//-----------------------------------------------------------------------
	// Input:	pLayout			 - Points to a CXTPDockingPaneLayout object.
	//			pMap			 - Points to a CXTPPaneToPaneMap object.
	//			dwIgnoredOptions - TRUE to ignore options of the pane.
	// Summary: This member is called to clone the pane.
	// Returns: A pointer to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Call this member to store information about the pane.
	//-----------------------------------------------------------------------
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	//			pMap    - Points to a CMapWordToPtr object.
	// Summary: Call this member function to restore the settings of the pane.
	// Returns: TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	
	//-----------------------------------------------------------------------
	// Input:	bShow - TRUE to show the window, or FALSE to hide it.
	// Summary: Sets the visibility state of the window.
	//-----------------------------------------------------------------------
	void ShowWindow(BOOL bShow);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the item.
	// Summary: Call this member to set the identifier of the item.
	//-----------------------------------------------------------------------
	void SetID(long nID);

	//-----------------------------------------------------------------------
	// Input:	rc - Bounding rectangle to draw.
	// Summary: Call this member to set the bounding rectangle
	//-----------------------------------------------------------------------
	void SetWindowRect(CRect rc);



private:
	HWND	m_hwndChild;
	long	m_nID;
	CString m_strTitle;

	int		m_nIconID;

	DWORD   m_dwOptions;
	DWORD   m_dwData;

	CPoint	m_ptMinTrackSize;
	CPoint	m_ptMaxTrackSize;

//:Ignore
	
#ifdef _XTP_COMPONENT

public:
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
	afx_msg LPDISPATCH OleGetMinTrackSize();
	afx_msg LPDISPATCH OleGetMaxTrackSize();

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

#ifdef _XTP_ACTIVEX


class CXTPDockingPaneTrackSize : public CCmdTarget
{
public:

	CXTPDockingPaneTrackSize(CPoint*);           

// Attributes
public:

// Operations
public:
	void OnPropertyChanged();

// Implementation
protected:
	CPoint* m_pPoint;
	long m_nWidth;
	long m_nHeight;

	afx_msg void OleSetSize(long nWidth, long nHeight);

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CTabControlItem)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPDockingPaneTrackSize);
};

#endif


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
AFX_INLINE CSize CXTPDockingPane::SetMinTrackSize(CSize sz) {
	CSize szMinTrackSize(m_ptMinTrackSize);
	m_ptMinTrackSize = sz;
	return szMinTrackSize;
}
AFX_INLINE CSize CXTPDockingPane::SetMaxTrackSize(CSize sz) {
	CSize szMaxTrackSize(m_ptMaxTrackSize);
	m_ptMaxTrackSize = sz;
	return szMaxTrackSize;
}	

#endif // #if !defined(__XTPDOCKINGPANE_H__)