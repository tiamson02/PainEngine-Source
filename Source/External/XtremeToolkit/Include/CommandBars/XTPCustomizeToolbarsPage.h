// XTPCustomizeToolbarsPage.h : interface for the CXTPCustomizeToolbarsPage class.
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
#if !defined(__XTPCUSTOMIZETOOLBARSPAGE_H__)
#define __XTPCUSTOMIZETOOLBARSPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPCustomizeSheet;

//===========================================================================
// Remarks: CXTPCustomizeToolbarsPage is a CPropertyPage derived class. 
//			It represents the Toolbars page of the Customize dialog.
//===========================================================================
class _XTP_EXT_CLASS CXTPCustomizeToolbarsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CXTPCustomizeToolbarsPage)

// Construction
public:

	//-----------------------------------------------------------------------
	// Input:	pSheet - Points to a CXTPCustomizeSheet object that this page 
	//			belongs to.
	// Summary: Constructs a CXTPCustomizeToolbarsPage object
	//-----------------------------------------------------------------------
	CXTPCustomizeToolbarsPage(CXTPCustomizeSheet* pSheet = NULL);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPCustomizeToolbarsPage object, handles cleanup 
	//			and de-allocation.
	//-----------------------------------------------------------------------
	~CXTPCustomizeToolbarsPage();

//:Ignore
	//{{AFX_DATA(CXTPCustomizeToolbarsPage)
	enum { IDD = XTP_IDD_PAGE_TOOLBARS };
	CXTPCustomizeSheet* m_pSheet;		
	CCheckListBox m_checkList;			
	CButton m_btnReset;					
	CButton m_btnRename;				
	CButton m_btnDelete;				
	CButton m_btnNew;					
	//}}AFX_DATA
//:End Ignore


private:
// Overrides
	void RefreshToolbarsList();

	
// Implementation
private:
//:Ignore
	//{{AFX_VIRTUAL(CXTPCustomizeToolbarsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CXTPCustomizeToolbarsPage)
	afx_msg void OnSelectionChanged();
	afx_msg void OnChkChange();
	afx_msg void OnNewToolbar();
	afx_msg void OnRenameToolbar();
	afx_msg void OnDeleteToolbar();
	afx_msg void OnResetToolbar();
	//}}AFX_MSG
//:End Ignore
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(__XTPCUSTOMIZETOOLBARSPAGE_H__)
