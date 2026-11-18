// XTPCustomizeSheet.h : interface for the CXTPCustomizeSheet class.
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
#if !defined(__XTPCUSTOMIZESHEET_H__)
#define __XTPCUSTOMIZESHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"
#include "CommonPro.include/XTPManageState.h"

class CXTPCommandBars;
class CXTPCustomizeToolbarsPage;
class CXTPCustomizeCommandsPage;

// Summary: CXTPCustomizeSheet is a CPropertySheet derived class. It represents the Customize dialog.
class _XTP_EXT_CLASS CXTPCustomizeSheet : CXTPDialogState, public CPropertySheet
{
// Construction
public:

	// Input:	pCommandBars - Points to a CXTPCommandBars object
	//			iSelectPage - Page initially selected.
	// Summary: Constructs a CXTPCustomizeSheet object
	CXTPCustomizeSheet(CXTPCommandBars* pCommandBars, UINT iSelectPage = 0);

	// Summary: Destroys a CXTPCustomizeSheet object, handles cleanup and de-allocation.
	virtual ~CXTPCustomizeSheet();

// Attributes
public:
	CImageList m_ilUserIcons;

// Operations
public:

	// Returns: A pointer to a CXTPCustomizeCommandsPage object
	// Summary: Returns the Commands PropertyPage
	CXTPCustomizeCommandsPage* GetCommandsPage();
	
	// Returns: A pointer to a CXTPCommandBars object
	// Summary: Returns the parent Command Bars class
	CXTPCommandBars* GetCommandBars();

	// Summary: Call this method to show customize dialog.
	int DoModal();

	void ContextMenu(CPoint point);

private:

	BOOL FilterMouse(WPARAM wParam, LPARAM lParam);
	BOOL FilterKey(WPARAM wParam, LPARAM lParam);
	
private:

	//{{AFX_VIRTUAL(CXTPCustomizeSheet)
	BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCustomizeSheet)
	afx_msg void OnCustomizeDefaultStyle();								
	afx_msg void OnCustomizeImageAndText();							
	afx_msg void OnCustomizeTextOnly();								
	afx_msg void OnCustomizeBeginGroup();				
	afx_msg void OnCustomizeDelete();							
	afx_msg void OnCustomizeCopyImage();						
	afx_msg void OnCustomizePasteImage();							
	afx_msg void OnCustomizeResetImage();								
	afx_msg void OnCustomizeEditImage();
	afx_msg void OnCustomizeReset();
	afx_msg LRESULT OnCustomizeName(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateCustomizeDefaultStyle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeImageAndText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeTextOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeBeginGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeName(CCmdUI* pCmdUI);
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
	afx_msg void OnUpdateCustomizeCopyImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizePasteImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeResetImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeReset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomizeEditImage(CCmdUI* pCmdUI);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CXTPCustomizeToolbarsPage* m_pPageToolBars;		
	CXTPCustomizeCommandsPage* m_pPageCommands;		
	CWnd* m_pWndParent;								
	CXTPCommandBars* m_pCommandBars;				

private:
	DECLARE_DYNAMIC(CXTPCustomizeSheet)

	class CCustomizeButton;
	class CCustomizeEdit;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPCustomizeCommandsPage* CXTPCustomizeSheet::GetCommandsPage() {
	return m_pPageCommands;
}
AFX_INLINE CXTPCommandBars* CXTPCustomizeSheet::GetCommandBars() {
	return m_pCommandBars;
}


#endif // #if !defined(__XTPCUSTOMIZESHEET_H__)
