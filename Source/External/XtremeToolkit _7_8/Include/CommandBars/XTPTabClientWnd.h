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


/////////////////////////////////////////////////////////////////////////////
// CXTPTabClientWnd is a CWnd derived class. It represents a tabbed workspace.
class _XTP_EXT_CLASS CXTPTabClientWnd : public CWnd, CXTPTabManagerAtom
{
protected:
	// Summary: CWorkspace  is a CXTPTabManager derived class. internal used.
	class CWorkspace : public CXTPTabManager
	{
		// Construction
	public:
		// Summary: Constructs a CWorkspace object
		CWorkspace(); 
		
		// Summary: Destroys a CWorkspace object, handles cleanup and de-allocation.
		virtual ~CWorkspace();
		
		// Attributes
	public:
		
	protected:	
		virtual CXTPTabPaintManager* GetPaintManager();	
		virtual void OnNavigateButtonClick(UINT nID);
		virtual void OnItemClick(CXTPTabManagerItem* pItem);
		virtual void Reposition();	
		virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon);
		virtual void RedrawControl();
		virtual void SetSelectedItem(CXTPTabManagerItem* pItem);		
		virtual void ReOrder(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem);
		virtual BOOL IsMouseLocked();
		virtual CString GetItemTooltip(CXTPTabManagerItem* pItem) const;

		virtual BOOL IsAllowReorder();
		virtual void SetAllowReorder(BOOL bAllowReorder);	
		
		virtual BOOL IsDrawStaticFrame();

		
	protected:
		
		// Summary: Finds the tab item that corresponds to the specified window
		CXTPTabManagerItem* FindItem(const HWND hWnd);
		
		// Summary: Adds new tab item for the specified window
		CXTPTabManagerItem* AddItem(const CWnd* pChildWnd);		

		HICON GetItemIcon(CXTPTabManagerItem *pItem);
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
	// Summary: Constructs a CXTPTabClientWnd object
	CXTPTabClientWnd();

	// Summary: Destroys a CXTPTabClientWnd object, handles cleanup and de-allocation.
	virtual ~CXTPTabClientWnd();

// Operations
public:

	// Input:	pParentFrame - Points to a CMDIFrameWnd object
	//			bEnableGroups - TRUE to enable groups.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Attaches the tabbed workspace.
	BOOL Attach(CXTPMDIFrameWnd* pParentFrame, 
		BOOL bEnableGroups = FALSE);
	
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Detaches the tabbed workspace.
	BOOL Detach();

	// Returns: TRUE if attached; otherwise returns FALSE
	// Summary: Determines if a tab workspace is attached.
	BOOL IsAttached() const;

	// Summary: Retrieves the parent frame of the control.
	CMDIFrameWnd* GetParentFrame();

	// Summary: Saves Position state.
	void SaveState()
	{
		AfxGetApp()->WriteProfileInt(_T("TabDocking"), _T("Position"), (int)m_pPaintManager->GetPosition());
	}
	// Summary: Loads previous docking state.
	void LoadState()
	{
		int nValue = AfxGetApp()->GetProfileInt(_T("TabDocking"), _T("Position"), xtpTabPositionTop); 
		m_pPaintManager->SetPosition(XTPTabPosition(nValue));
		m_bForceToRecalc = TRUE;
	}
	
	// Scan through all MDIChild windows and update corresponding 
	// tab items if any changes happened (e.g. window text or active MDIChild).
	void UpdateContents();

	// Summary: Call this member to set flags of the tab workspace. See XTPWorkspaceButtons enumerators.
	// See also: GetFlags
	void SetFlags(DWORD dwButtons);

	// Summary: Retrieves flags of the tab workspace.
	DWORD GetFlags();

	void EnableToolTips(BOOL bEnable = TRUE);

	CXTPTabPaintManager* GetPaintManager();	
	void SetPaintManager(CXTPTabPaintManager* pPaintManager);	
	void SetAutoTheme(BOOL bAutoTheme = FALSE);
	BOOL GetAutoTheme();

	CXTPTabManagerItem* FindItem(const HWND hWnd);
	CXTPTabManagerItem* HitTest(CPoint pt) const;	

	int GetWorkspaceCount() const;
	CWorkspace* GetWorkspace(int nIndex) const;

	void DoWorkspaceCommand(CXTPTabManagerItem* pItem, CWorkspace* pFocusWorksapce, int nAction);
	BOOL IsWorkspaceCommandEnabled(UINT nID, CXTPTabManagerItem** ppItem = NULL);

	void OnWorkspaceCommand(UINT nID);

	void Refresh(BOOL bRecalcLayout = TRUE);

	void WorkspaceToScreen(LPPOINT lpPoint);
	void ScreenToWorkspace(LPPOINT lpPoint);

	void ShowWorkspace(BOOL bShow);

	BOOL IsAllowReorder();
	void SetAllowReorder(BOOL bAllowReorder);	


protected:
	virtual void CheckCommandBarsTheme();

	CWnd* MDIGetActive();
	void MDIActivate(CWnd* pWnd);

	virtual CString GetItemText(const CWnd* pChildWnd) const;
	virtual COLORREF GetItemColor(CXTPTabManagerItem* pItem);

	virtual CWorkspace*  AddWorksapce(int nIndex = -1);

	CXTPTabManagerItem* AddItem(CWnd* pChildWnd);	


	int FindIndex(CXTPTabManager* pWorkspace);

	void SetActiveWorkspace(CWorkspace* pWorkspace);
	void ReorderWorkspace(CPoint pt, CXTPTabManagerItem* pItem);
	void ContextMenu(CPoint pt);

	void InitLoop();
	void CancelLoop();
	void DrawFocusRect(BOOL bRemoveRect = FALSE);
	void UpdateFlags(CWorkspace* pWorkspace);


	CXTPTabManagerItem* GetSelectedItem();
	CXTPTabManagerItem* GetItem(int nIndex);
	int GetItemCount();

	virtual void Reposition();	
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
