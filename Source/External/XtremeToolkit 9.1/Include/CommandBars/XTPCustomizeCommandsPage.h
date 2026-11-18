// XTPCustomizeCommandsPage.h : interface for the CXTPCustomizeCommandsPage class.
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
#if !defined(__XTPCUSTOMIZECOMMANDSPAGE_H__)
#define __XTPCUSTOMIZECOMMANDSPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPCustomizeSheet;
class CXTPControl;
class CXTPControls;
class CXTPCommandBar;

//:Ignore
struct XTP_COMMANDBARS_CATEGORYINFO
{
	XTP_COMMANDBARS_CATEGORYINFO(CString _strCategory, CXTPCommandBars* pCommandBars);
	~XTP_COMMANDBARS_CATEGORYINFO();

	CString strCategory;
	CXTPControls* pControls;
};

typedef CArray<XTP_COMMANDBARS_CATEGORYINFO*, XTP_COMMANDBARS_CATEGORYINFO*> CCategoryArray;
//:End Ignore

// Summary: CXTPCustomizeCommandsPage is a CPropertyPage derived class. 
// It represents the Commands page of the Customize dialog.
class _XTP_EXT_CLASS CXTPCustomizeCommandsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CXTPCustomizeCommandsPage)

private:
	// Summary: List of the commands
	class CCommandsListBox : public CListBox
	{
	public:	
		virtual void DrawItem(LPDRAWITEMSTRUCT);
		virtual LRESULT WindowProc(UINT message,WPARAM wParam,LPARAM lParam);

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		
		DECLARE_MESSAGE_MAP()
		friend class CXTPCustomizeCommandsPage;
	private:
		CXTPCustomizeCommandsPage* m_pCommandsPage;

	};	
	
public:

	// Input:	pSheet - Points to a CXTPCustomizeSheet object
	// Summary: Constructs a CXTPCustomizeCommandsPage object
	CXTPCustomizeCommandsPage(CXTPCustomizeSheet* pSheet = NULL);
	
	// Summary: Destroys a CXTPCustomizeCommandsPage object, handles cleanup and de-allocation.
	~CXTPCustomizeCommandsPage();



//:Ignore
	//{{AFX_DATA(CXTPCustomizeCommandsPage)
	enum { IDD = XTP_IDD_PAGE_COMMANDS };
	CCommandsListBox	m_lstCommands;
	CListBox	m_lstCategories;
	//}}AFX_DATA
//:End Ignore

public:

	// Input:	nIDResource - Menu resource from where categories will be built.
	//			bListSubItems - TRUE to add sub menus to categories.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds categories brunch from menu resource.
	BOOL AddCategories(UINT nIDResource,  BOOL bListSubItems = FALSE);
	
	// Input:	pControls - Points to a CXTPControls object
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds categories brunch from Controls.
	BOOL AddCategories(CXTPControls* pControls);
	
	// Input:	strCategory - Category to be added.
	//			pMenu - Points to a CMenu object
	//			bListSubItems - TRUE to add sub menus to the category.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds a new category from a CMenu object.
	BOOL AddCategory(CString strCategory, CMenu* pMenu, BOOL bListSubItems = FALSE);	
	
	// Input:	strCategory - Category to be added.
	//			nIDResource - Toolbar resource identifier.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Adds a new category from a toolbar resource.
	BOOL AddToolbarCategory(CString strCategory, UINT nIDResource);	
	
	// Input:	strCategory - Category to be added.
	//			nIndex - Index to insert.
	// Returns: A pointer to a CXTPControls object
	// Summary: This method adds a new empty category in the given index.
	CXTPControls* InsertCategory(CString strCategory, int nIndex = -1);		
	
	// Input:	nIDResource - "New Menu" category string resource.
	//			nIndex - Index to insert
	// Summary: Adds single "New menu" category.
	void InsertNewMenuCategory(UINT nIDResource = XTP_IDS_NEWMENU, int nIndex = -1);
	
	// Input:	nIDResource - "All commands" category string resource.
	//			nIndex - Index to insert
	// Summary: Adds "All Commands" category and adds all controls already created in other categories here.
	void InsertAllCommandsCategory(UINT nIDResource = XTP_IDS_ALLCOMMANDS, int nIndex = -1);
	
	// Input:	nIDResourceMenu - Menu resource identifier.
	//			nIDResourceCategory - "Built-in" string identifier.
	//			nIndex - Index to insert.
	// Returns: TRUE if successful; otherwise returns FALSE.
	// Summary: Adds built-in menus category.
	BOOL InsertBuiltInMenus(UINT nIDResourceMenu, UINT nIDResourceCategory = XTP_IDS_BUILTINMENUS, int nIndex = -1);

	// Input:	strCategory - Category to retrieve.
	// Returns: A pointer to a CXTPControls object.
	// Summary: Retrieves the control list of the given category.
	CXTPControls* GetControls(CString strCategory);

private:
	XTP_COMMANDBARS_CATEGORYINFO* FindCategory(CString strCategory);
	XTP_COMMANDBARS_CATEGORYINFO* GetCategoryInfo(int nIndex);	
	
	BOOL _LoadToolBar(CXTPCommandBar* pCommandBar, UINT nIDResource);

// Overrides
	
//:Ignore
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CXTPCustomizeCommandsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL OnInitDialog();
//:End Ignore

// Implementation
private:
	// Generated message map functions
	//{{AFX_MSG(CXTPCustomizeCommandsPage)
	afx_msg void OnCategoriesSelectionChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CXTPCustomizeSheet* m_pSheet;
	CCategoryArray m_arrCategories;

private:
	friend class CCommandsListBox;		


};

#endif // !defined(__XTPCUSTOMIZECOMMANDSPAGE_H__)
