// CLevelExplorerDlg : header file
/////////////////////////////////////////////////////////////////////////////

#ifndef __SOLUTIONSEXPLORER_H
#define __SOLUTIONSEXPLORER_H

#include "TabCtrlBar.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLevelExplorerDlg : public CTabCtrlBar
{
	DECLARE_DYNAMIC(CLevelExplorerDlg)

// Construction / destruction
public:

	bool m_IsLocked;
	//CXTPropertyGrid m_wndPropertyGrid;


	// Constructs a CLevelExplorerDlg object.
	CLevelExplorerDlg();

	// Destroys a CLevelExplorerDlg object, handles cleanup and de-allocation.
	virtual ~CLevelExplorerDlg();

// Member variables
protected:
    CImageList        m_ilTabImages;
	CXTToolBar        m_wndToolBar;
    CXTCoolMenu       m_wndCoolMenu;
    //XT_BAR_SORT_TYPES m_eSortTypes;

// Member functions
public:
	void InitializeToolbar();
	void RemoveMenuButtons();
	void UpdateButtonDisplay();

	//{{AFX_VIRTUAL(CLevelExplorerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	virtual void GetInsideRect(CRect &rect);
	virtual void OnTabSelChange(int nIDCtrl, CXTTabCtrl* pTabCtrl);
	virtual void OnTabSelChanging(int nIDCtrl, CXTTabCtrl* pTabCtrl);
	virtual void OnSize(UINT nType, int cx, int cy);
	void OnNewFolder();
	void OnNewGameObject();
	void OnSaveGameObject();
	void OnResetGameObject();
	void OnPauseGameObject();
	void OnStartGameObject();
	void OnRefreshGameObject();
	void OnLock();

	//{{AFX_MSG(CLevelExplorerDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnUpdateLock(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    friend class CMainFrame;
	afx_msg void OnTimer(UINT nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SOLUTIONSEXPLORER_H

