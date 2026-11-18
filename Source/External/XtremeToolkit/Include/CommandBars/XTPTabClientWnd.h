// XTPTabClientWnd.h : interface for the CXTPTabClientWnd class.
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
#if !defined(__TABCLIENTWNDEX_H__)
#define __TABCLIENTWNDEX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#define WM_XTP_GETTABICON (WM_USER + 131)
#define WM_XTP_PRETRANSLATEMOUSEMSG (WM_USER + 132)
#define WM_XTP_GETWINDOWTEXT (WM_USER + 133)

#ifndef WM_XTP_GETTABCOLOR
#define WM_XTP_GETTABCOLOR (WM_USER + 134)
#endif

#define WM_XTP_GETWINDOWTOOLTIP (WM_USER + 135)
#define WM_XTP_NEWTABITEM (WM_USER + 136)



// Workspace buttons enumerator.
enum XTPWorkspaceButtons
{
	xtpWorkspaceHideArrows = 1, // To hide arrow buttons.
	xtpWorkspaceHideClose = 2,  // To hide close button.
	xtpWorkspaceHideAll = 3     // To hide arrow and close buttons.
};

class CXTPTabPaintManager;
class CXTPCommandBars;
class CXTPMDIFrameWnd;


//===========================================================================
// Remarks: CXTPTabClientWnd is a CWnd derived class. It represents a tabbed 
//			workspace.
//===========================================================================
class _XTP_EXT_CLASS CXTPTabClientWnd : public CWnd, CXTPTabManagerAtom
{
protected:
	//-----------------------------------------------------------------------
	// Summary: CWorkspace  is a CXTPTabManager derived class. internal used.
	//-----------------------------------------------------------------------
	class CWorkspace : public CXTPTabManager
	{
		// Construction
	public:

		//-----------------------------------------------------------------------
		// Summary: Constructs a CWorkspace object
		//-----------------------------------------------------------------------
		CWorkspace(); 
		
		//-----------------------------------------------------------------------
		// Summary: Destroys a CWorkspace object, handles cleanup and de-allocation.
		//-----------------------------------------------------------------------
		virtual ~CWorkspace();
		
		// Attributes
	public:
		
	protected:	
		//-----------------------------------------------------------------------
		// Summary: Retrieves the paint manager.
		// Returns: A pointer to a CXTPTabPaintManager object.
		//-----------------------------------------------------------------------
		virtual CXTPTabPaintManager* GetPaintManager();	
		
		//-----------------------------------------------------------------------
		// Input:	nID - Specifies the control's ID.
		// Summary: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual void OnNavigateButtonClick(UINT nID);

		//-----------------------------------------------------------------------
		// Input:	pItem - Points to a CXTPTabManagerItem object
		// Summary: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual void OnItemClick(CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual void Reposition();	

		//-----------------------------------------------------------------------
		// Input:	pDC    - Pointer to a valid device context
		//			pt     - Position to draw
		//			pItem  - Points to a CXTPTabManagerItem object
		//			bDraw  - TRUE to draw; FALSE to retrieve the size of the control
		//			szIcon - Icon size
		// Summary: This method is called to draw the icon.
		// Returns: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon);
		
		//-----------------------------------------------------------------------
		// Summary: Initiates redrawing of the control
		// Remarks: Call this member function if you want to initialize redrawing
		//			of the control. The control will be redrawn taking into account 
		//			its latest state.
		//-----------------------------------------------------------------------
		virtual void RedrawControl();

		//-----------------------------------------------------------------------
		// Input:	pItem - Points to a CXTPTabManagerItem object.
		// Summary: Call this member to select the specified item.
		//-----------------------------------------------------------------------
		virtual void SetSelectedItem(CXTPTabManagerItem* pItem);		

		//-----------------------------------------------------------------------
		// Input:	hWnd  - Handle of the window
		//			pt    - NEEDS COMMENT 
		//			pItem - Points to a CXTPTabManagerItem object
		// Summary: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual void ReOrder(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem);

		//-----------------------------------------------------------------------
		// Summary: Checks to see if the mouse is locked.
		// Returns: TRUE if locked; otherwise returns FALSE.
		//-----------------------------------------------------------------------
		virtual BOOL IsMouseLocked();

		//-----------------------------------------------------------------------
		// Input:	pItem - Points to a CXTPTabManagerItem object.
		// Summary: Returns the tooltip associated with the specified item.
		// Returns: Associated tooltip
		//-----------------------------------------------------------------------
		virtual CString GetItemTooltip(CXTPTabManagerItem* pItem) const;

		//-----------------------------------------------------------------------
		// Summary: Returns whether or not reorder is allowed.
		// Returns: TRUE when reorder is allowed; FALSE otherwise.
		//-----------------------------------------------------------------------
		virtual BOOL IsAllowReorder();

		//-----------------------------------------------------------------------
		// Input:	bAllowReorder - TRUE for allowing reorder; FALSE for disallowing.
		// Summary: Allows/disallows reorder.
		//-----------------------------------------------------------------------
		virtual void SetAllowReorder(BOOL bAllowReorder);	
		
		//-----------------------------------------------------------------------
		// Summary: NEEDS COMMENT
		//-----------------------------------------------------------------------
		virtual BOOL IsDrawStaticFrame();

		
	protected:
		
		//-----------------------------------------------------------------------
		// Summary: Finds the tab item that corresponds to the specified window
		//-----------------------------------------------------------------------
		CXTPTabManagerItem* FindItem(const HWND hWnd);
		
		//-----------------------------------------------------------------------
		// Summary: Adds new tab item for the specified window
		//-----------------------------------------------------------------------
		CXTPTabManagerItem* AddItem(const CWnd* pChildWnd);		

		//-----------------------------------------------------------------------
		// Input:	pItem - Pointer to a CXTPTabManagerItem.
		// Summary: This member function gets the index for the current icon.
		// Returns: An icon index for the current item.
		//-----------------------------------------------------------------------
		HICON GetItemIcon(CXTPTabManagerItem *pItem);

		//-----------------------------------------------------------------------
		// Input:	pItem - Pointer to a CXTPTabManagerItem.
		// Summary: This member function returns the RGB value for the referenced 
		//			item text or (COLORREF)-1, if the color was not set.
		// Returns: The RGB value for the referenced item text, or (COLORREF)-1, 
		//			if the color was not set
		//-----------------------------------------------------------------------
		COLORREF GetItemColor(CXTPTabManagerItem* pItem);
		
	protected:
		CXTPTabClientWnd* m_pTabClientWnd;	
		HWND m_hwndClient;	
		double m_dHeight;
		CRect m_rcSplitter;
		
		friend class CXTPTabClientWnd;		
	};

	class CSingleWorkspace : public CWnd, public CWorkspace
	{
	public:
		~CSingleWorkspace();

		virtual BOOL PreTranslateMessage(MSG* pMsg);
		virtual void RedrawControl();

		int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;


		//{{AFX_MSG(CSingleWorkspace)	
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnMouseLeave();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	};

// Construction
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPTabClientWnd object
	//-----------------------------------------------------------------------
	CXTPTabClientWnd();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPTabClientWnd object, handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTabClientWnd();

// Operations
public:

	//-----------------------------------------------------------------------
	// Input:	pParentFrame - Points to a CMDIFrameWnd object
	//			bEnableGroups - TRUE to enable groups.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Attaches the tabbed workspace.
	//-----------------------------------------------------------------------
	BOOL Attach(CXTPMDIFrameWnd* pParentFrame, 
		BOOL bEnableGroups = FALSE);
	
	//-----------------------------------------------------------------------
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Detaches the tabbed workspace.
	//-----------------------------------------------------------------------
	BOOL Detach();

	//-----------------------------------------------------------------------
	// Returns: TRUE if attached; otherwise returns FALSE
	// Summary: Determines if a tab workspace is attached.
	//-----------------------------------------------------------------------
	BOOL IsAttached() const;

	//-----------------------------------------------------------------------
	// Summary: Retrieves the parent frame of the control.
	//-----------------------------------------------------------------------
	CMDIFrameWnd* GetParentFrame();

	//-----------------------------------------------------------------------
	// Summary: Saves Position state.
	//-----------------------------------------------------------------------
	void SaveState()
	{
		AfxGetApp()->WriteProfileInt(_T("TabDocking"), _T("Position"), (int)m_pPaintManager->GetPosition());
	}
	//-----------------------------------------------------------------------
	// Summary: Loads previous docking state.
	//-----------------------------------------------------------------------
	void LoadState()
	{
		int nValue = AfxGetApp()->GetProfileInt(_T("TabDocking"), _T("Position"), xtpTabPositionTop); 
		m_pPaintManager->SetPosition(XTPTabPosition(nValue));
		m_bForceToRecalc = TRUE;
	}
	
	//-----------------------------------------------------------------------
	// Scan through all MDIChild windows and update corresponding 
	// tab items if any changes happened (e.g. window text or active MDIChild).
	//-----------------------------------------------------------------------
	void UpdateContents();

	//-----------------------------------------------------------------------
	// Summary: Call this member to set flags of the tab workspace. See XTPWorkspaceButtons enumerators.
	// See also: GetFlags
	//-----------------------------------------------------------------------
	void SetFlags(DWORD dwButtons);

	//-----------------------------------------------------------------------
	// Summary: Retrieves flags of the tab workspace.
	//-----------------------------------------------------------------------
	DWORD GetFlags();

	//-----------------------------------------------------------------------
	// Input:	bEnable - TRUE for enabling, FALSE for disabling.
	// Summary: Call this member function to enable or disable tooltips show
	//-----------------------------------------------------------------------
	void EnableToolTips(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Call this method to retrieve the paint manager.
	// Returns: A pointer to a CXTPTabPaintManager object.
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* GetPaintManager();	
	
	//-----------------------------------------------------------------------
	// Input:	pPaintManager - Points to a CXTPTabPaintManager object.
	// Summary:	Call this member to set the specified paint manager.
	//-----------------------------------------------------------------------
	void SetPaintManager(CXTPTabPaintManager* pPaintManager);	

	//-----------------------------------------------------------------------
	// Input:	bAutoTheme - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void SetAutoTheme(BOOL bAutoTheme = FALSE);

	//-----------------------------------------------------------------------
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	BOOL GetAutoTheme();

	//-----------------------------------------------------------------------
	// Input:	hWnd - Handle of the window.
	// Summary: Call this member to find the item that corresponds to the 
	//			specified window.
	// Returns: A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* FindItem(const HWND hWnd);

	//-----------------------------------------------------------------------
	// Input:	pt - Point to be tested.
	// Summary: Call this function to determine which tab, if any, is at the 
	//			specified screen position.
	// Returns: The zero-based index of the tab, or returns -1 if no tab is 
	//			at the specified position.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* HitTest(CPoint pt) const;	

	//-----------------------------------------------------------------------
	// Summary: Call this member to retrieve the current number of workspaces.
	// Returns: An integer value that represents the number of workspaces.
	//-----------------------------------------------------------------------
	int GetWorkspaceCount() const;

	//-----------------------------------------------------------------------
	// Index:	nIndex - An integer index.
	// Summary: Call this member to return the workspace at the specified index.
	// Returns: A pointer to the workspace currently at this index.
	//-----------------------------------------------------------------------
	CWorkspace* GetWorkspace(int nIndex) const;

	//-----------------------------------------------------------------------
	// Input:	pItem			- Pointer to a CXTPTabManagerItem object.
	//			pFocusWorkspace - Pointer to a CWorkspace object.
	//			nAction			- NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void DoWorkspaceCommand(CXTPTabManagerItem* pItem, CWorkspace* pFocusWorksapce, int nAction);

	//-----------------------------------------------------------------------
	// Input:	nID    - Specifies the identifier of the workspace.
	//			ppItem - NEEDS COMMENT
	// Summary: Returns the workspace command mode.
	// Remarks: Call this member function if you want to determine whether or
	//			not workspace command is enabled.
	// Returns: TRUE when workspace command is enabled; FALSE when it is disabled.
	//-----------------------------------------------------------------------
	BOOL IsWorkspaceCommandEnabled(UINT nID, CXTPTabManagerItem** ppItem = NULL);

	//-----------------------------------------------------------------------
	// Input:	nID - Specifies the identifier of the workspace.
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void OnWorkspaceCommand(UINT nID);

	//-----------------------------------------------------------------------
	// Input:	bRecalcLayout - NEEDS COMMENT
	// Summary: Call this method to refresh all the items.
	//-----------------------------------------------------------------------
	void Refresh(BOOL bRecalcLayout = TRUE);

	//-----------------------------------------------------------------------
	// Input:	lpPoint - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void WorkspaceToScreen(LPPOINT lpPoint);

	//-----------------------------------------------------------------------
	// Input:	lpPoint - NEEDS COMMENT
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void ScreenToWorkspace(LPPOINT lpPoint);

	//-----------------------------------------------------------------------
	// Input:	bShow - TRUE to show the workspace, or FALSE to hide it.
	// Summary: Call this member function to show or hide the workspace.
	//-----------------------------------------------------------------------
	void ShowWorkspace(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary: Returns the current reordering allowance.
	// Returns: TRUE when reordering is allowed; FALSE when it is disallowed.
	//-----------------------------------------------------------------------
	BOOL IsAllowReorder();

	//-----------------------------------------------------------------------
	// Input:	bAllowReorder - TRUE for allowing reordering; FALSE for disallowing
	//							reordering.
	// Summary: Call this member function to allow or disallow reordering.
	//-----------------------------------------------------------------------
	void SetAllowReorder(BOOL bAllowReorder);	

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context.
	//			rc  - Rectangle area to be filled.
	// Summary: This member is called when the background is filled.
	//-----------------------------------------------------------------------
	virtual void OnFillBackground(CDC* pDC, CRect rc);

protected:
	
	//-----------------------------------------------------------------------
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	virtual void CheckCommandBarsTheme();

	//-----------------------------------------------------------------------
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	CWnd* MDIGetActive();

	//-----------------------------------------------------------------------
	// Input:	pWnd - Pointer to a valid device context.
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void MDIActivate(CWnd* pWnd);

	//-----------------------------------------------------------------------
	// Input:	pChildWnd - A CWnd pointer that represents the child to be 
	//						displayed when the window is activated.
	// Summary: This member function retrieves the text of a specific tab.
	// Returns: The text of a particular tab, or NULL if an error occurs.
	//-----------------------------------------------------------------------
	virtual CString GetItemText(const CWnd* pChildWnd) const;

	//-----------------------------------------------------------------------
	// Input:	pItem - A pointer to a CXTPTabManagerItem.
	// Summary: This member function returns the RGB value for the referenced 
	//			item text or (COLORREF)-1, if the color was not set.
	// Returns: The RGB value for the referenced item text, or (COLORREF)-1, 
	//			if the color was not set.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemColor(CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Input:	nIndex - Index of the workspace.
	// Summary: Call this member function to add a workspace.
	// Returns: TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual CWorkspace*  AddWorksapce(int nIndex = -1);

	//-----------------------------------------------------------------------
	// Input:	pChildWnd - A CWnd pointer that represents the child to be
	//						displayed when the window is activated.
	// Summary: This member function adds a new tab item for the specified window.
	// Returns: A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	virtual CXTPTabManagerItem* AddItem(CWnd* pChildWnd);	

	//-----------------------------------------------------------------------
	// Input:	pWorkspace - Pointer to a CXTPTabManger object.
	// Summary: Call this member to find the specified index.
	// Returns: The index of the matching item.
	//-----------------------------------------------------------------------
	int FindIndex(CXTPTabManager* pWorkspace);

	//-----------------------------------------------------------------------
	// Input:	pWorkspace - Pointer to a CWorkspace object.
	// Summary: This member functions sets a workspace active. 
	//-----------------------------------------------------------------------
	void SetActiveWorkspace(CWorkspace* pWorkspace);

	//-----------------------------------------------------------------------
	// Input:	pt    - Position of the workspace to reorder.
	//			pItem - Pointer to a CXTPTabManagerItem object.
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void ReorderWorkspace(CPoint pt, CXTPTabManagerItem* pItem);

	//-----------------------------------------------------------------------
	// Input:	pt - Position of the menu to show.
	// Summary: This method shows the context menu for toolbars.
	//-----------------------------------------------------------------------
	void ContextMenu(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to initialize a loop.
	//-----------------------------------------------------------------------
	void InitLoop();

	//-----------------------------------------------------------------------
	// Summary: This member function is called when a loop is cancelled.
	//-----------------------------------------------------------------------
	void CancelLoop();

	//-----------------------------------------------------------------------
	// Input:	bRemoveRect - NEEDS COMMENT
	// Summary: This member function is called to draw the focus rectangle
	//-----------------------------------------------------------------------
	void DrawFocusRect(BOOL bRemoveRect = FALSE);

	//-----------------------------------------------------------------------
	// Input:	pWorkspace - Pointer to a CWorkspace object.
	// Summary: NEEDS COMMENT
	//-----------------------------------------------------------------------
	void UpdateFlags(CWorkspace* pWorkspace);

	//-----------------------------------------------------------------------
	// Summary: Returns the selected item in the control.
	// Returns: A pointer to a CXTPTabManagerItem object.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* GetSelectedItem();

	//-----------------------------------------------------------------------
	// Input:	nIndex - Zero-based index of the item in the collection.
	// Summary: Returns an item by its index.
	// Remarks: You use this member function to get an item from collection by 
	//			item index. If the given index is less than 0 or greater than 
	//			the value returned by GetItemCount(), GetItem() returns NULL.
	// Returns: Pointer to the found item, if any, or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPTabManagerItem* GetItem(int nIndex);

	//-----------------------------------------------------------------------
	// Summary: Returns the number of items.
	// Returns: The number of items.
	//-----------------------------------------------------------------------
	int GetItemCount();

	//-----------------------------------------------------------------------
	// Summary: Call this method to reposition items.
	//-----------------------------------------------------------------------
	virtual void Reposition();	

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the command bar's object.
	// Returns: A CXTPCommandBars pointer (can be NULL).
	//-----------------------------------------------------------------------
	virtual CXTPCommandBars* GetCommandBars();
	
protected:
	CArray<CWorkspace*, CWorkspace*> m_arrWorkspace;
	
	
	CMDIFrameWnd* m_pParentFrame;	// pointer to the corresponding parent MDIFrame window
	BOOL m_bForceToRecalc;
	BOOL m_bLockReposition;

	BOOL m_bLockUpdate;
	BOOL m_bRefreshed;

	CXTPTabPaintManager* m_pPaintManager;
	XTPPaintTheme m_themeCommandBars;
	BOOL m_bAutoTheme;	

	CWorkspace* m_pActiveWorkspace;
	BOOL m_bHorizSplitting;

	HCURSOR m_hCursorHoriz;
	HCURSOR m_hCursorVert;
	HCURSOR m_hCursorNew;
	HCURSOR m_hCursorDelete;	

	BOOL m_bThemedBackColor;

	CRect m_rcGroup;
	CRect m_rectLast;
	CSize m_sizeLast;
	CDC* m_pDC;
	CWorkspace* m_pFocusWorksapce;
	int m_nFocusedAction;
	BOOL m_bEnableGroups;

	DWORD m_dwFlags;
	BOOL m_bAllowReorder;
	BOOL m_bShowWorkspace;

	BOOL m_bEnableTooltips;


//:Ignore

	//{{AFX_VIRTUAL(CXTPTabClientWnd)
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	//{{AFX_MSG(CXTPTabClientWnd)
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg LRESULT OnMDIActivate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDICreate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDIDestroy(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* );
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	
	afx_msg void OnUpdateWorkspaceCommand(CCmdUI* pCmdUI);
	afx_msg BOOL OnNcActivate(BOOL bActive);

	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPTabClientWnd);


#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPTabClientWnd);
	DECLARE_CONNECTION_EX(CXTPTabClientWnd)

	BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
		HRESULT STDMETHODCALLTYPE Next( 
		/* [in] */ ULONG celt,
		/* [length_is][size_is][out] */ VARIANT *rgVar,
		/* [out] */ ULONG *pCeltFetched);

		HRESULT STDMETHODCALLTYPE Skip( 
		/* [in] */ ULONG celt);

		HRESULT STDMETHODCALLTYPE Reset( void);

		HRESULT STDMETHODCALLTYPE Clone( 
			/* [out] */ IEnumVARIANT **ppEnum);
			int m_nCurrent;
	END_INTERFACE_PART(EnumVARIANT)
	afx_msg LPUNKNOWN OleNewEnum();	

	afx_msg LPDISPATCH OleGetItem(int nItem);
	afx_msg LPDISPATCH OleGetSelected();
	afx_msg long OleGetItemCount();
	afx_msg LPDISPATCH OleHitTest(OLE_XPOS_PIXELS x, OLE_YPOS_PIXELS y);
	afx_msg void OleRefresh();
	afx_msg BOOL OleIsWorkspaceCommandEnabled(long nID);

	void FireRClick(LPDISPATCH lpDispatch){ 
		FireEvent(1,EVENT_PARAM(VTS_DISPATCH),lpDispatch); }	
	void FireNewItem(LPDISPATCH lpDispatch){ 
		FireEvent(2,EVENT_PARAM(VTS_DISPATCH),lpDispatch); }


	enum {
		dispidFlags = 1L,	
	};
	
	LPDISPATCH OleGetPaintManager();
	void OleEnableGroups();
	
#endif

	friend class CWorkspace;
	friend class CSingleWorkspace;
//:End Ignore

};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPTabClientWnd::IsAttached() const { 
	return (m_pParentFrame!=NULL ? TRUE : FALSE); 
}

AFX_INLINE CMDIFrameWnd* CXTPTabClientWnd::GetParentFrame() { 
	return m_pParentFrame; 
}
AFX_INLINE DWORD CXTPTabClientWnd::GetFlags() {
	return m_dwFlags;
}


#endif// #if !defined(__XTPTABCLIENTWNDEX_H__)
