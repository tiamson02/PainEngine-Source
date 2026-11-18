// XTPCustomizeOptionsPage.h : interface for the CXTPCustomizeOptionsPage class.
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
#if !defined(__XTPCUSTOMIZEOPTIONSPAGE_H__)
#define __XTPCUSTOMIZEOPTIONSPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore


class CXTPCustomizeSheet;
class CXTPCommandBars;

// Summary: CXTPCustomizeOptionsPage is a CPropertyPage derived class. 
// It represents the Options page of the Customize dialog.
class _XTP_EXT_CLASS CXTPCustomizeOptionsPage : public CPropertyPage
{  

// Construction
public:

	// Input:	pSheet - Points to a CXTPCustomizeSheet object that this page belongs to.
	// Summary: Constructs a CXTPCustomizeOptionsPage object
	CXTPCustomizeOptionsPage(CXTPCustomizeSheet* pSheet);
	
	// Summary: Destroys a CXTPCustomizeOptionsPage object, handles cleanup and de-allocation.
	~CXTPCustomizeOptionsPage();
	
	// Returns: A pointer to a CXTPCommandBars object
	// Summary: Retrieves parent command bars.
	CXTPCommandBars* GetCommandBars();

//:Ignore
	//{{AFX_DATA(CXTPCustomizeOptionsPage)
	enum { IDD = XTP_IDD_PAGE_OPTIONS };
	BOOL m_bAlwaysShowFullMenus;
	BOOL m_bShowFullAfterDelay;
	BOOL m_bLargeIcons;
	BOOL m_bToolBarScreenTips;
	BOOL m_bToolBarAccelTips;
	CComboBox m_comboAnimationType;
	int m_nAnimationType;
	//}}AFX_DATA
//:End Ignore
	

// Overrides
//:Ignore
	//{{AFX_VIRTUAL(CXTPCustomizeOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
//:End Ignore

private:
	void AddComboString( UINT nIDResource );

// Implementation
private:

	//{{AFX_MSG(CXTPCustomizeOptionsPage)
	afx_msg void OnCheckFullMenus();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAfterdelay();
	afx_msg void OnCheckLargeicons();
	afx_msg void OnCheckScreenttips();
	afx_msg void OnCheckShortcuts();
	afx_msg void OnResetData();
	afx_msg void OnAnimationChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CXTPCustomizeSheet* m_pSheet;				// Parent sheet.
};

#endif // !defined(__XTPCUSTOMIZEOPTIONSPAGE_H__)
