// XTPControlPopup.h : interface for the CXTPControlPopup class.
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
#if !defined(__XTPCONTROLPOPUP_H__)
#define __XTPCONTROLPOPUP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPControl.h"

class CXTPCommandBar;
class CXTPControls;

// Summary: CXTPControlPopup is a CXTPControl derived class. 
//			It represents a popup-able control.
class _XTP_EXT_CLASS CXTPControlPopup : public CXTPControl
{
public:

	// Summary: Constructs a CXTPControlPopup object
	CXTPControlPopup(void);

	// Destroys a CXTPControlPopup object, handles cleanup and de-allocation
	virtual ~CXTPControlPopup(void);

	// Returns: A pointer to a CXTPControlPopup object
	// Summary: Creates a CXTPControlPopup object with the specified type.
	static CXTPControlPopup* AFX_CDECL CreateControlPopup(XTPControlType controlType);
	
public:
	// Input:	pBar - The child command bar.
	// Summary: Call this member to set the child command bar.
	void SetCommandBar(CXTPCommandBar* pBar);
	
	// Input:	pMenu - The menu to be loaded as a child.
	// Summary: Call this member to set the child command bar.
	void SetCommandBar(CMenu* pMenu);

	// Returns: A pointer to the child command bar.
	// Summary: Call this member to get the child command bar.
	CXTPCommandBar* GetCommandBar();

	// Returns: TRUE if the control is a popup.
	// Summary: Call this member to get the control's popup state.
	BOOL GetPopup();

protected:

	// Input:	pControlPopup - Points to a CXTPControlPopup object needed to check.
	// Returns: TRUE if contains; otherwise returns FALSE
	// Summary: This method is called to check if pControlPopup contains in the control recursively.
	BOOL IsContainPopup(CXTPControlPopup* pControlPopup);
	
	// Summary: Call this member to expand the parent popup bar.
	void ExpandCommandBar();
	
	// Input:	bSelected - TRUE if the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called when the control becomes selected.
	BOOL OnSetSelected(int bSelected);
	
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	// Summary: This method is called after the mouse hovers over the control.
	void OnMouseHover();
	
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	
	// Input:	bPopup - TRUE to set popup.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called to popup the control.
	virtual BOOL OnSetPopup(BOOL bPopup);
	
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the user releases the left mouse button.
	virtual void OnLButtonUp(CPoint point);
	
	// Input:	point - Specifies the x- and y-coordinates of the cursor.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This member is called when the user double-clicks the left mouse button.
	virtual BOOL OnLButtonDblClk(CPoint point);
	
	// Input:	pDataObject - Points to a CXTPControl object
	//			point - Mouse position.
	//			dropEffect - DROPEFFECT enumerator.
	// Summary: This method is called when the user moves the mouse over the control.
	void OnCustomizeDragOver(CXTPControl* /*pDataObject*/, CPoint /*point*/, DROPEFFECT& /*dropEffect*/);
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to read or write this object from or to an archive.
	virtual void Serialize(CArchive& archive);

	// Inputs:  pOther - The control needed to compare with.
	// Returns: TRUE if the controls are identical.
	// Summary: Call this member to compare controls.
	virtual BOOL Compare(CXTPControl* pOther);

	// Summary: Call this member to update the shadow of the control.
	virtual void UpdateShadow();

	virtual void AdjustExcludeRect(CRect& rc);

protected:
	BOOL			m_bPopup;			// TRUE if popuped.
	CXTPCommandBar* m_pCommandBar;		// Child popup bar.
	DWORD			m_dwCommandBarID;	// Child popup id.

public:

//:Ignore	

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	
	
	DECLARE_OLETYPELIB_EX(CXTPControlPopup);

	afx_msg LPDISPATCH OleGetCommandBar();
	enum {
		dispidCommandBar = 50L,
		dispidStyle = 51L,
	};
	
#endif

	DECLARE_XTP_CONTROL(CXTPControlPopup)
	friend class CXTPControls;
	friend class CXTPPopupBar;	
	friend class CXTPCommandBar;	
	
//:End Ignore
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE CXTPCommandBar* CXTPControlPopup::GetCommandBar() { 
	return m_pCommandBar;
}
AFX_INLINE BOOL CXTPControlPopup::GetPopup() { 
	return m_bPopup; 
}

#endif //#if !defined(__XTPCONTROLPOPUP_H__)