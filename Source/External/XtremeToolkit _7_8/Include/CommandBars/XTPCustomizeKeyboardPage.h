// XTPCustomizeKeyboardPage.h : interface for the CXTPCustomizeKeyboardPage class.
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
#if !defined(__XTPCUSTOMIZEKEYBOARDPAGE_H__)
#define __XTPCUSTOMIZEKEYBOARDPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPCustomizeSheet;

// Summary: CXTPCustomizeKeyboardPage is a CPropertyPage derived class. 
// It represents the Keyboard page of the Customize dialog.
class _XTP_EXT_CLASS CXTPCustomizeKeyboardPage : public CPropertyPage
{
// Construction
public:

	// Input:	pSheet - Points to a CXTPCustomizeSheet object that this page belongs to.
	// Summary: Constructs a CXTPCustomizeKeyboardPage object
	CXTPCustomizeKeyboardPage(CXTPCustomizeSheet* pSheet);
	
	// Summary: Destroys a CXTPCustomizeKeyboardPage object, handles cleanup and de-allocation.
	~CXTPCustomizeKeyboardPage();

//:Ignore
	//{{AFX_DATA(CXTPCustomizeKeyboardPage)
	enum { IDD = XTP_IDD_PAGE_KEYBOARD };
	CListBox		m_lboxKeys;			
	CListBox		m_lboxCommands;		
	CComboBox		m_comboCategory;	
	CString			m_strTitle;			
	CString			m_strDesc;			
	int		m_iCategory;				
	CXTPShortcutManager::CKeyAssign	m_editShortcutKey;
	//}}AFX_DATA
//:End Ignore

	// Input:	strCategory - Category needed to find.
	// Summary: Finds the category.
	XTP_COMMANDBARS_CATEGORYINFO* FindCategory(CString strCategory);
	
	// Input:	strCategory - Category to be added.
	//			nIndex - Index to insert.
	// Returns: A pointer to a CXTPControls object
	// Summary: This method adds a new empty category in the given index.
	CXTPControls* InsertCategory(CString strCategory, int nIndex = -1);
	
	// Input:	nIDResource - Menu resource from where caetgories will be built.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds categories brunch from menu resource.
	BOOL AddCategories(UINT nIDResource);
	
	// Input:	strCategory - Category to be added.
	//			pMenu - Points to a CMenu object
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds a new category from a CMenu object.
	BOOL AddCategory(CString strCategory, CMenu* pMenu);
	
	// Input:	pControls - Points to a CXTPControls object
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds categories brunch from Controls.
	BOOL AddCategories(CXTPControls* pControls);

	// Summary: This member function is called by the page to reload the list items.
	void ReloadList();

	// Summary: This member function is called by the page to enable the assignment buttons.
	void EnableControls();

	// Summary: Retrieves frame accelerators.
	HACCEL GetFrameAccelerator();
	
	// Input:	lpAccel - Accelerator table to set.
	//			nSize - Number of items.
	// Summary: Updates frame accelerators.
	void UpdateAcellTable(LPACCEL lpAccel, int nSize);


private:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CXTPCustomizeKeyboardPage)
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Generated message map functions

	//{{AFX_MSG(CXTPCustomizeKeyboardPage)
	afx_msg void OnSelchangeCategory();
	afx_msg void OnSelchangeCommands();
	afx_msg void OnSelchangeCurKeys();
	afx_msg void OnChangeShortcutKey();
	afx_msg void OnAssign();
	afx_msg void OnResetAll();
	afx_msg void OnRemove();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

private:
	CCategoryArray m_arrCategories;		
	CXTPCustomizeSheet* m_pSheet;
};

#endif // !defined(__XTPCUSTOMIZEKEYBOARDPAGE_H__)
