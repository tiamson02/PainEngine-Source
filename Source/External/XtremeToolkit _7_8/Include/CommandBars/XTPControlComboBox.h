// XTPControlComboBox.h : interface for the CXTPControlComboBox class.
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
#if !defined(__XTPCONTOLCOMBOBOX_H__)
#define __XTPCONTOLCOMBOBOX_H__			

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPPopupBar.h"

class CXTPControlComboBox;

// Summary: CXTPEdit is a CEdit derived class. It is for internal usage only.
class _XTP_EXT_CLASS CXTPEdit : public CEdit
{
	DECLARE_DYNCREATE(CXTPEdit);
public:
	

	// Input:	nID - Contains the command ID. 
	//			nCode - Identifies the command notification code. 
	//			pExtra - Used according to the value of nCode. 
	//			pHandlerInfo - If not NULL, OnCmdMsg fills in the pTarget and pmf members of the 
	//			pHandlerInfo structure instead of dispatching the command. Typically, this parameter 
	//			should be NULL. 
	// Returns: Nonzero if the message is handled; otherwise 0.
	// Summary: Called by the framework to route and dispatch command messages and to handle the update 
	//			of command user-interface objects.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
	// Input:	wParam - The low-order word of wParam identifies the command ID of the menu item, control, 
	//			or accelerator. The high-order word of wParam specifies the notification message if the 
	//			message is from a control. If the message is from an accelerator, the high-order word is 1. 
	//			If the message is from a menu, the high-order word is 0. 
	// Returns: An application returns nonzero if it processes this message; otherwise 0.
	// Summary: The framework calls this member function when the user selects an item from a menu, when a 
	//			child control sends a notification message, or when an accelerator keystroke is translated.
	virtual BOOL OnCommand(WPARAM wParam, LPARAM);
};

	
// Summary: CXTPControlButton is a CXTPControl derived class. 
//			It represents a list box of CXTPControlComboBox control.
class _XTP_EXT_CLASS CXTPControlComboBoxList : public CXTPPopupBar
{	
public:
	
	// Summary: Constructs a CXTPControlButton object
	CXTPControlComboBoxList() { m_barPosition = xtpBarListBox; m_nMRUWidth = 0;}; 

protected:	
	
	// Input:	hWnd - Window handle that the message belongs to.
	//			nMessage - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information. 
	//			lParam - Specifies additional message-dependent information.
	//			lResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Summary: This member function is called by WindowProc, or is called during message reflection.
	int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	
	// Input:	pControlPopup - Points to a CXTPControlPopup object
	//			bSelectFirst - TRUE to select the first item.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called, then PopupBar becomes visible.
	virtual BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE);
	
	// Input:	 bMode - TRUE to set the tracking mode; otherwise FALSE.
	//			 bSelectFirst - TRUE to select the first item.
	// Returns:  TRUE if the method was successful.
	// Summary:  Call this member to change the tracking state.
	// See Also: IsTrackingMode.
	virtual BOOL SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard = FALSE);
	
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to draw the command bar in the given context.
	virtual void DrawCommandBar(CDC* pDC);
	
	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	The framework calls this member function when a non-system key is pressed.
	virtual BOOL OnKeyDown(UINT nChar); 
	
	// Returns: TRUE if command bars are in customized mode; otherwise returns FALSE
	// Summary: Call this member to retrieve the customize mode of the command bars.
	virtual BOOL IsCustomizable() { return FALSE; }
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this member to either read or write this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
 	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method makes a copy of the command bar.
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);


	// Private members declaration.

private:
	DECLARE_MESSAGE_MAP()
	DECLARE_XTP_COMMANDBAR(CXTPControlComboBoxList)
		

	//:Ignore
	//{{AFX_MSG(CXTPControlComboBoxList)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//:End Ignore


private:
	friend class CXTPControlComboBox;	
};

//////////////////////////////////////////////////////////////////////////
 
// Summary: Inplace Edit control of the combo.
class _XTP_EXT_CLASS CXTPControlComboBoxEditCtrl : public CXTPEdit
{
protected:
	//:Ignore
	//{{AFX_MSG(CXTPControlComboBoxEditCtrl)
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
	CXTPControlComboBox* m_pControl;		// Parent Combo Box.

private:
	DECLARE_MESSAGE_MAP()
	friend class CXTPControlComboBox;		
};


// Summary: CXTPControlComboBox is a CXTPControl derived class. 
//			It represents a combo box control.
class _XTP_EXT_CLASS CXTPControlComboBox : public CXTPControlPopup
{
public:
	// Summary: Constructs a CXTPControlComboBox object
	CXTPControlComboBox(void);

	// Summary: Destroys a CXTPControlComboBox object, handles cleanup and de-allocation
	virtual ~CXTPControlComboBox(void);

public:
	// Summary: This method create edit control. Override it to use inherited edit control.
	virtual CXTPControlComboBoxEditCtrl* CreateEditControl();

public:
	// Input:	comboStyle - The style to be set.
	// Summary: Call this member to set the style of the control.
	void SetStyle(XTPComboStyle comboStyle);

	// Returns: The style of the control.
	// Summary: Call this member to get the control's style.	
	XTPComboStyle GetStyle();

	// Input:	bSet - TRUE if the combo box has an edit control.
	// Summary: Call this member to change the type of the combo box.
	void SetDropDownListStyle(BOOL bSet = TRUE);

	// Summary: Call this member to retrieve the dropdown style of the combo box.
	BOOL GetDropDownListStyle();

	// Input:	nWidth - The width of the dropdown list.
	// Summary: Call this member to set the width of the dropdown list.
	void SetDropDownWidth(int nWidth);

	// Returns: Width of the dropdown list.
	// Summary: Call this member to retrieve the dropdown list width.
	int GetDropDownWidth();
	
	// Input:	str - The string that is to be added.
	// Returns: The zero-based index of the string in the list box. The return value is LB_ERR
	//			if an error occurs.
	// Summary: Call this member function to add a string to a list box.
	int AddString(CString str);
	
	// Returns: The number of items in the list box, or LB_ERR if an error occurs.
	// Summary: Call this member to retrieve the number of items in a list box.
	int GetCount();

	// Input:	nIndex - Contains the zero-based index of the list-box string to be copied.
	//			str - A reference to a CString.
	// Summary: Call this member to retrieve a string from the list box of a combo box control.
	void GetLBText(int nIndex, CString& str);

	// Summary: Call this member to removes all items from the list box of a combo box control.	
	void ResetContent();
	
	// Input:	nIndexStart - Specifies the zero-based index of the item before the first item 
	//			to be searched.
	//			str - The string to search for.
	// Returns: The zero-based index of the matching item, or CB_ERR if the search was 
	//			unsuccessful.
	// Summary: Call the FindStringExact member function to find the first list-box string 
	//			(in a combo box) that matches the string specified in str.
	int FindStringExact(int nIndexStart, CString str);

	// Input:	nIndex - Contains the zero-based index to the position in the list box that will receive 
	//			the string.
	//			str - The string that is to be inserted.
	// Summary: Call this member function to insert a string into the list box of a combo box control
	int InsertString(int nIndex, CString str);

	// Input:	nWidth - The width of the control
	// Summary: Call this member to set the width of the control
	void SetWidth(int nWidth);

	// Returns: Width of the control
	// Summary: Call this member to get the width of the control
	int GetWidth();
	
	// Returns: The text that is currently selected.
	// Summary: Call this member to get the currently selected item's text
	CString GetText(); 	

	// Returns: A pointer to the CEdit control.
	// Summary: Call this member to get the edit control of the combo box control.
	CXTPControlComboBoxEditCtrl* GetEditCtrl();

	// Input:	nIndex - Specifies the zero-based index of the string to select.
	// Summary: Call this member to select a string in the list box of a combo box.
	void SetCurSel(int nIndex);
	
	// Returns: The zero-based index of the currently selected item in the list box of a combo box, 
	//			or CB_ERR if no item is selected.
	// Summary: Call this member function to determine which item in the combo box is selected.
	int GetCurSel();

	// Returns: Nonzero if the list box is visible; otherwise 0.
	// Summary: Call the GetDroppedState member function to determine whether the list box of a drop-down 
	//			combo box is visible (dropped down).
	BOOL GetDroppedState();

	// Returns: The Edit control text.
	// Summary: Call this member to get the edit text.
	CString GetEditText();
	
	// Input:   lpszText - New text of the edit control.
	// Summary: Call this member to set the edit control text.
	void SetEditText(LPCTSTR lpszText,BOOL bMatchExact = FALSE);

	// Input:   nStartAfter - Contains the zero-based index of the item before the first item to be 
	//			searched. When the search reaches the bottom of the list box, it continues from the 
	//			top of the list box back to the item specified by nStartAfter. If nStartAfter is –1, 
	//			the entire list box is searched from the beginning.
	//          lpszItem - Points to the null-terminated string that contains the prefix to search for. 
	//			The search is case independent, so this string may contain any combination of uppercase 
	//			and lowercase letters.
	// Returns: The zero-based index of the matching item, or LB_ERR if the search was unsuccessful.	
	// Summary: This method finds the first string in a list box that contains the specified prefix, 
	//			without changing the list-box selection		
	int FindString(int nStartAfter, LPCTSTR lpszItem) const;

	// Input:   nIndex - Contains the zero-based index of an item in the combo box’s list box.
	// Returns: The 32-bit value associated with the item, or CB_ERR if an error occurs.
	// Summary: This method retrieves the application-supplied 32-bit value associated with the 
	//			specified combo box item.
	DWORD GetItemData(int nIndex) const;

	// Input:   nIndex - Contains a zero-based index of the item to set.
	//          dwItemData - Contains the new value to associate with the item.
	// Returns: CB_ERR if an error occurs.
	// Summary: This method sets the 32-bit value associated with the specified item in a combo box.
	int SetItemData(int nIndex, DWORD dwItemData);
	
	// Summary: Call this member function to delete a string.
	void DeleteItem(long nIndex);


protected:
	
	// Summary: Called after the mouse hovers over the control.
	void OnMouseHover();

	// Input:	pOldWnd - Points to a CWnd object
	// Summary: This method is called, then edit control gets the focus.
	virtual void OnSetFocus(CWnd* pOldWnd);
	
	// Summary: This method is called, then the edit control loses the focus.
	virtual void OnKillFocus();
	
	// Summary: This method is called, then the selected string is changed.
	virtual void OnSelChanged();
	
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
	
	// Input:	pParent - Points to a CXTPCommandBar object
	// Summary: This method is called to assign a parent command bar object.
	void SetParent(CXTPCommandBar* pParent);
	
	// Summary: This method is called before recalculating the parent command bar size.
	void OnCalcDynamicSize(DWORD dwMode);
	
	// Input:	 bEnabled - TRUE if the control is enabled.  
	// Summary:  Call this member to enable or disable the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetEnabled, SetChecked
	void SetEnabled(BOOL bEnabled);	
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to either read or write this object from or to an archive.
	virtual void Serialize(CArchive& ar);

	// Input:   bPopup - TRUE to set popup.
    // Returns: TRUE if successful; otherwise returns FALSE
    // Summary: This method is called to popup the control.
    virtual BOOL OnSetPopup(BOOL bPopup);

    // Summary: This method is called when the control is executed.
    virtual void OnExecute();  


	
public:
	int m_nLabelWidth;			// Width of the label.
	int m_nThumbWidth;			// Width of the thumb area.

protected:
	
	CXTPControlComboBoxEditCtrl* m_pEdit;		// Child edit control.

	BOOL m_bDropDown;			// TRUE if the combo is dropdown.
	XTPComboStyle m_comboStyle; // Style of the combo box.
	int m_nWidth;				// Width of the combo box.
	CString m_strEditText;		// Edit text.
	int m_nLastSel;				// last user selected index, (used during display of list box)

//:Ignore	
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPControlComboBox);

	afx_msg void OleSetDropDownWidth(long nWidth);
	afx_msg void OleSetDropDownListStyle(BOOL bSet);
	afx_msg void OleAddItem(LPCTSTR strText, const VARIANT* varIndex);
	afx_msg BSTR OleList(long nIndex);
	afx_msg void OleClear();
	afx_msg void OleRemoveItem(long nIndex);
	
	afx_msg BSTR OleGetText();
	afx_msg void OleSetText(LPCTSTR str);
	afx_msg long OleGetListIndex();
	afx_msg void OleSetListIndex(long nIndex);
	afx_msg long OleFindItem(LPCTSTR strText);

	enum {
		dispidStyle = 50L,
		dispidAddItem = 51L,
		dispidDropDownWidth = 52L,
		dispidDropDownListStyle = 53L,
		dispidWidth = 54L,
		dispidList = 55L,
		dispidClear = 56L,
		dispidListCount = 57L,
		dispidRemoveItem = 58L,		
		dispidText = 59L,
		dispidListIndex = 60L,
		dispidFindItem = 61L,
	};
	
#endif


private:
	
	DECLARE_XTP_CONTROL(CXTPControlComboBox)

	friend class CXTPControlComboBoxList;		
	friend class CXTPControlComboBoxEditCtrl;					

//:End Ignore
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE void CXTPControlComboBox::SetStyle(XTPComboStyle comboStyle) { 
	m_comboStyle = comboStyle;
}
AFX_INLINE XTPComboStyle CXTPControlComboBox::GetStyle() { 
	return m_comboStyle;
}

AFX_INLINE void CXTPControlComboBox::SetDropDownWidth(int nWidth) {
	((CXTPControlComboBoxList*)m_pCommandBar)->m_nMRUWidth = nWidth;
}
AFX_INLINE int CXTPControlComboBox::AddString(CString str) {
	return ((CListBox*)m_pCommandBar)->AddString(str);
}
AFX_INLINE int CXTPControlComboBox::GetCount() { 
	return ((CListBox*)m_pCommandBar)->GetCount();
}
AFX_INLINE void CXTPControlComboBox::GetLBText(int nIndex, CString& str) { 
	((CListBox*)m_pCommandBar)->GetText(nIndex, str);
}
AFX_INLINE void CXTPControlComboBox::ResetContent() { 
	((CListBox*)m_pCommandBar)->ResetContent();
}
AFX_INLINE int CXTPControlComboBox::FindStringExact(int nIndexStart, CString str) { 
	return ((CListBox*)m_pCommandBar)->FindStringExact(nIndexStart, str);
}
AFX_INLINE int CXTPControlComboBox::InsertString(int nIndex, CString str) { 
	return ((CListBox*)m_pCommandBar)->InsertString(nIndex, str); 
}
AFX_INLINE int CXTPControlComboBox::GetCurSel() {
	return ((CListBox*)m_pCommandBar)->GetCurSel();
}
AFX_INLINE void CXTPControlComboBox::SetWidth(int nWidth) { 
	m_nWidth = nWidth;
}
AFX_INLINE int CXTPControlComboBox::GetWidth() { 
	return m_nWidth;
}
AFX_INLINE CXTPControlComboBoxEditCtrl* CXTPControlComboBox::GetEditCtrl() { 
	return m_pEdit; 
}
AFX_INLINE int CXTPControlComboBox::FindString(int nStartAfter, LPCTSTR lpszItem) const {
	return ((CListBox*)m_pCommandBar)->FindString(nStartAfter, lpszItem);
}
AFX_INLINE DWORD CXTPControlComboBox::GetItemData(int nIndex) const {
	return (DWORD)((CListBox*)m_pCommandBar)->GetItemData(nIndex);
}
AFX_INLINE int CXTPControlComboBox::SetItemData(int nIndex, DWORD dwItemData) {
	return ((CListBox*)m_pCommandBar)->SetItemData(nIndex, dwItemData);
}
AFX_INLINE void CXTPControlComboBox::DeleteItem(long nIndex) {
	if (nIndex < GetCount()) ((CListBox*)m_pCommandBar)->DeleteString(nIndex);
}


#endif //#if !defined(__XTPCONTOLCOMBOBOX_H__)