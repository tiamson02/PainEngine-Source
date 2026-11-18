// XTPReBar.h : header file
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
#if !defined(__XTPREBAR_H__)
#define __XTPREBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPToolBar;

#if _MSC_VER < 1200 // MFC 6.0

#define AFX_IDW_REBAR                   0xE804  

class _XTP_EXT_CLASS CXTPReBarBase : public CControlBar
{
	DECLARE_DYNAMIC(CXTPReBarBase)

// Construction
public:
	CXTPReBarBase();
	BOOL Create(CWnd* pParentWnd, DWORD dwCtrlStyle = RBS_BANDBORDERS,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP,
		UINT nID = AFX_IDW_REBAR);

// Attributes
public:
	int m_iComCtlVersion;
	void GetComCtlVersion();

// Operations
public:
	BOOL AddBar(CWnd* pBar, LPCTSTR pszText = NULL, CBitmap* pbmp = NULL,
		DWORD dwStyle = RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP);
	BOOL AddBar(CWnd* pBar, COLORREF clrFore, COLORREF clrBack,
		LPCTSTR pszText = NULL, DWORD dwStyle = RBBS_GRIPPERALWAYS);

// Implementation
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
#ifdef _DEBUG
	void EnableDocking(DWORD dwDockStyle);
#endif

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL _AddBar(CWnd* pBar, REBARBANDINFO* pRBBI);

	//{{AFX_MSG(CReBar)
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT);
	afx_msg void OnHeightChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
	afx_msg LRESULT OnShowBand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRecalcParent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#else
#define CXTPReBarBase CReBar
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPReBar window

class _XTP_EXT_CLASS CXTPReBar : public CXTPReBarBase
{
// Construction
public:
	CXTPReBar();

// Attributes
public:

// Operations
public:
	BOOL AddToolBar(CXTPToolBar* pToolBar, DWORD dwStyle = RBBS_GRIPPERALWAYS);
	void DeleteToolBar(CXTPToolBar* pToolBar);

	int FindBand(CXTPToolBar* pToolBar);
	CXTPToolBar* GetToolBar(int nBand);

	void LoadState( LPCTSTR lpszProfileName );
	void SaveState( LPCTSTR lpszProfileName );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXTPReBar();

protected:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReBar)
	void OnChildSize(NMHDR * pNotifyStruct, LRESULT* result );
	void OnPaint();
	BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPReBar)
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPREBAR_H__)