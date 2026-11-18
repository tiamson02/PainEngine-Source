// XTPControlCustom.h : interface for the CXTPControlCustom class.
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
#if !defined(__XTPCONTROLCUSTOM_H__)
#define __XTPCONTROLCUSTOM_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPControlButton.h"

class CXTPCommandBars;

// Vertical options enumerator.
enum XTPControlCustomVerticalOptions 
{
	xtpVerticalHide, // To hide control if parent toolbar verticaly docked.
	xtpVerticalShow, // To show control if parent toolbar verticaly docked.
	xtpVerticalButton // To show control as button.
};

// Summary: CXTPControlCustom is a CXTPControlButton derived class. It represents the control
//			with some child window.
class _XTP_EXT_CLASS CXTPControlCustom : public CXTPControlButton, public IHookAble
{
public:
	
	// Summary: Constructs a CXTPControlCustom object
	CXTPControlCustom();
	
	// Summary: Destroys a CXTPControlCustom object, handles cleanup and de-allocation
	~CXTPControlCustom();

	static CXTPControlCustom* AFX_CDECL CreateControlCustom(CWnd* pWndChild);
	
public:
	
	// Input:	pWnd - Points to a child CWnd object
	// Summary: Call this method to attach the window to the control.
	void SetControl(CWnd* pWnd);
	
	// Returns: A pointer to a CWnd object
	// Summary: Call this method to retrieve the child window of the control.
	CWnd* GetControl();
	
	// Input:	szControl - Size of the child window.
	// Summary: This method sets the child window size.
	void SetSize(CSize szControl);
	
	// Input:	cxRight - Right border gap.
	//			cxTop - Top border gap.
	//			cxLeft - Left border gap.
	//			cxBottom - Bottom border gap.
	// Summary: This method sets the margins of the child window.
	void SetBorders(int cxRight, int cxTop, int cxLeft, int cxBottom);	
	
	// Summary: Call this method to set vertical options to the control.
	void SetVerticalPositionOption(XTPControlCustomVerticalOptions options);	
	
protected:
	
	// Input:	rcControl - Bounding rectangle of the control.
	// Summary: Call this member to set the bounding rectangle of the control.
	virtual void SetRect(CRect rcControl);
	
	// Input:	pDC - Pointer to a valid device context
	// Returns: Size of the control.
	// Summary: This method is called to determine the size of the control.
	virtual CSize GetSize(CDC* pDC);
	
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	virtual void Draw(CDC* pDC);
	
	// Input:	 bEnabled - TRUE if the control is enabled.  
	// Summary:  Call this member to enable or disable the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetEnabled, SetChecked
	virtual void SetEnabled(BOOL bEnabled);
	
	// Input:	dwFlags - Reasons to hide.
	// Summary:	This method is called to hide the control.
	virtual void SetHideFlags(DWORD dwFlags);
	
	// Summary: This method is called before recalculating the parent command bar size.
	virtual void OnCalcDynamicSize(DWORD dwMode);
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to read or write this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point - Mouse position.
	//			dropEffect - DROPEFFECT enumerator.
	// Returns: TRUE if successful, otherwise returns FALSE
	// Summary: Call this method to check if the user can drop the control.
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint /*point*/, DROPEFFECT& dropEffect);

	// Input:	hWnd - Window handle that the message belongs to.
	//			nMessage - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information. 
	//			lParam - Specifies additional message-dependent information.
	//			lResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Summary: This member function is called by WindowProc, or is called during message reflection.
	int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);


	CWnd* FindChildWindow(CXTPCommandBars* pCommandBars, UINT nID);

protected:
	CWnd* m_pControlWnd;								// The child window.
	CRect m_rcBorders;									// Borders.
	BOOL m_bControlVisible;								// TRUE if child window is visible.
	CSize m_szControl;									// Size of the control.
	XTPControlCustomVerticalOptions m_verticalOptions;	// Vertical options.
	int m_nControlID;

public:
	
//:Ignore	
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	afx_msg long OleGetHandle();
	afx_msg void OleSetHandle(long nHandle);

	enum {
		dispidHandle = 50L,
	};
	
#endif


	DECLARE_XTP_CONTROL(CXTPControlCustom);
//:End Ignore
};



#endif // #if !defined(__XTPCONTROLCUSTOM_H__)