// XTPCustomizeTools.h
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
#if !defined(__XTPCUSTOMIZETOOLS_H__)
#define __XTPCUSTOMIZETOOLS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


class CXTPCommandBar;
class CXTPCommandBars;
class CXTPControl;
class CXTPCustomizeSheet;


#define	DROPEFFECT_CANCEL	( 5 )


//:Ignore

// Summary: Internal helper class for customization
class _XTP_EXT_CLASS CXTPCustomizeDropSource
{

public:

	CXTPCustomizeDropSource(CXTPCommandBars* pCommandBars);
	~CXTPCustomizeDropSource();

public:

	DROPEFFECT DoDragDrop(CXTPControl* pControl, BOOL bCopyOnly = FALSE);
	void Register(CXTPCommandBar* pCommandBar);
	void UnRegister(CXTPCommandBar* pCommandBar);
	CXTPCustomizeSheet* GetSheet();

private:
	
	
	DROPEFFECT _DoDragDrop();
	void PickTarget(CPoint ptCursor);
	void FreshenCursor();
	
private:
	HWND m_hwndCapture;
	CXTPControl* m_pControl;
	BOOL m_bMove;
	BOOL m_bCopyOnly;
	CXTPCommandBar* m_pTarget;
	CXTPCustomizeSheet* m_pSheet;
	CPoint m_ptStart;

	CArray<CXTPCommandBar*, CXTPCommandBar*> m_arrTargets;
	
	CXTPCommandBars* m_pCommandBars;

	HCURSOR	m_hcurDelete;
	HCURSOR	m_hcurMove;
	HCURSOR	m_hcurCopy;

	friend class CXTPCustomizeSheet;
	friend class CXTPCommandBars;
	
};


//:End Ignore

//:Ignore

/////////////////////////////////////////////////////////////////////////////
// CXTPNewToolbarDlg dialog

class _XTP_EXT_CLASS CXTPNewToolbarDlg : public CDialog
{
// Construction
public:
	CXTPNewToolbarDlg(CWnd* pParent, CXTPCommandBars* pCommandBars, CXTPCommandBar* pCommandBar = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXTPNewToolbarDlg)
	enum { IDD = XTP_IDD_NEWTOOLBAR };
	CString	m_strToolbar;
	//}}AFX_DATA
	UINT m_nNewID;

	CXTPCommandBar* m_pCommandBar;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPNewToolbarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void SetSuggestedName();
	void OnOK();

	CXTPCommandBars* m_pCommandBars;

	// Generated message map functions
	//{{AFX_MSG(CXTPNewToolbarDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//:End Ignore

#endif // #define __XTPCUSTOMIZETOOLS_H__
