// XTPControlEdit.h : interface for the CXTPControlEdit class.
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
#if !defined(__XTPCONTOLEDIT_H__)
#define __XTPCONTOLEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPControl.h"
#include "XTPControlComboBox.h"

class CXTPControlEdit;

// Summary: Inplace Edit control of CXTPControlEdit.
class _XTP_EXT_CLASS CXTPControlEditCtrl : public CXTPEdit
{
protected:
	//:Ignore
	//{{AFX_MSG(CXTPControlEditCtrl)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//:End Ignore

public:
	CXTPControlEdit* m_pControl;

private:
	DECLARE_MESSAGE_MAP()
	friend class CXTPControlComboBox;

};

// Summary: CXTPControlEdit is a CXTPControl derived class. 
//			It represents an edit control.
class _XTP_EXT_CLASS CXTPControlEdit : public CXTPControl
{
public:
	// Summary: Constructs a CXTPControlEdit object
	CXTPControlEdit(void);

	// Summary: Destroys a CXTPControlEdit object, handles cleanup and de-allocation
	virtual ~CXTPControlEdit(void);

public:
	// Summary: This method create edit control. Override it to use inherited edit control.
	virtual CXTPControlEditCtrl* CreateEditControl();


public:
	// Input:	nWidth - The width of the control
	// Summary: Call this member to set the width of the control
	void SetWidth(int nWidth);

	// Returns: Width of the control
	// Summary: Call this member to get the width of the control
	int GetWidth();
	
	// Returns: A pointer to the CEdit control.
	// Summary: Call this member to get the edit control.
	CEdit* GetEditCtrl();

	// Returns: The Edit control text.
	// Summary: Call this member to get the edit control text.
	CString GetEditText();

	// Input:   lpszText - New text of the edit control.
	// Summary: Call this member to set the edit control text.
	void SetEditText(LPCTSTR lpszText);

	// Input:	bShow - TRUE to show the label.
	// Summary: Call this member to show the label of the control.
	void ShowLabel(BOOL bShow);

	// Returns: TRUE if the control has a visible label ; otherwise returns FALSE
	// Summary: Call this member to determine if the control has a visible label.
	BOOL IsLabeled();

	void Serialize(CArchive& ar);

protected:
	
	// Summary: Called after the mouse hovers over the control.
	virtual void OnMouseHover();
	
	// Input:	pOldWnd - Points to a CWnd object
	// Summary: This method is called, then the edit control gets the focus.
	virtual void OnSetFocus(CWnd* pOldWnd);
	
	// Summary: This method is called, then the edit control loses the focus.
	virtual void OnKillFocus();
	
	// Input:	pDC - Contains a pointer to the display context for the child window.
	//			nCtlColor - Must be CTLCOLOR_EDIT for edit.
	// Returns: OnCtlColor must return a handle to the brush that is to be used for painting the control 
	//			background.
	// Summary: The framework calls this member function when a child control is about to be drawn.
	virtual HBRUSH OnCtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/) { return 0;}

	// Input:	bSelected - TRUE if the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called when the control becomes selected.
	BOOL OnSetSelected(int bSelected);
	
	// Input:	rcControl - Bounding rectangle of the control.
	// Summary: Call this member to set the bounding rectangle of the control.
	void SetRect(CRect rcControl);	
	
	// Input:	dwFlags - Reasons to hide.
	// Summary:	This method is called to hide the control.
	void SetHideFlags(DWORD dwFlags);
	
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	
	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	This method is called when a non-system key is pressed.
	BOOL OnKeyDown(UINT nChar);
	
	// Summary: This method is called before recalculating the parent command bar size.
	void OnCalcDynamicSize(DWORD dwMode);
	
	// Input:	 bEnabled - TRUE if the control is enabled.  
	// Summary:  Call this member to enable or disable the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetEnabled, SetChecked
	void SetEnabled(BOOL bEnabled);	
	
	// Input:	pParent - Points to a CXTPCommandBar object
	// Summary: This method is called to assign a parent command bar object.
	void SetParent(CXTPCommandBar* pParent);
	
public:
	int m_nLabelWidth;  // Width of the label.

protected:
	
	CXTPControlEditCtrl* m_pEdit;     // Inplace edit control.
	int m_nWidth;		   // Width of the control.
	CString m_strEditText; // Edit text.
	BOOL m_bLabel;		   // TRUE if label is visible.
	
//:Ignore	

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPControlEdit);

	afx_msg BSTR OleGetText();
	afx_msg void OleSetText(LPCTSTR str);

	enum {
		dispidShowLabel = 50L,
		dispidWidth = 54L,
		dispidText = 59L,
	};
	
#endif


public:
	DECLARE_XTP_CONTROL(CXTPControlEdit)
	friend class CXTPControlEditCtrl;
//:End Ignore
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE void CXTPControlEdit::SetWidth(int nWidth) { 
	m_nWidth = nWidth;
}
AFX_INLINE int CXTPControlEdit::GetWidth() { 
	return m_nWidth;
}
AFX_INLINE CEdit* CXTPControlEdit::GetEditCtrl() { 
	return m_pEdit; 
}

AFX_INLINE void CXTPControlEdit::ShowLabel(BOOL bShow) {
	m_bLabel = bShow;
}
AFX_INLINE BOOL CXTPControlEdit::IsLabeled() {
	return m_bLabel;
}


#endif //#if !defined(__XTPCONTOLEDIT_H__)