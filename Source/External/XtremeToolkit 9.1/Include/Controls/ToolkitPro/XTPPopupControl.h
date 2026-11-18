// XTPPopupControl.h: interface for the CXTPPopupControl class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE 
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN 
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED 
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE GRANTS TO 
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A 
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_XTPPOPUPCONTROL_H__)
#define _XTPPOPUPCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTPPopupControl.h : header file
//
class CXTPPopupItem;
/////////////////////////////////////////////////////////////////////////////
// CXTPPopupControl window

class CXTPPopupPaintManager;

//---------------------------------------------------------------------------
// Summary: Enumeration used to determine the popup window theme in use
//---------------------------------------------------------------------------
enum XTPPopupPaintTheme
{
	xtpPopupThemeOffice2000,	//The theme like to a Office2000
	xtpPopupThemeOfficeXP,		//The theme like to a OfficeXP
	xtpPopupThemeOffice2003,	//The theme like to a Office2003
	xtpPopupThemeMSN,			//The theme like to a MSN Messenger
	xtpPopupThemeCustom			//The custom theme
};

//---------------------------------------------------------------------------
// Summary: Enumeration used to determine the popup window animation in use
//---------------------------------------------------------------------------
enum XTPPopupAnimation
{ 
	xtpPopupAnimationNone,		//Animation is not used
	xtpPopupAnimationFade,		//Fade method animation
	xtpPopupAnimationSlide,		//Slide method animation
	xtpPopupAnimationUnfold		//Unfold method animation
}; 

//---------------------------------------------------------------------------
// Summary: Enumeration used to determine a current popup window state
//---------------------------------------------------------------------------
enum XTPPopupState
{ 
	xtpPopupStateClosed,		//The popup window is closed
	xtpPopupStateExpanding,		//The popup window is expanding
	xtpPopupStateShow,			//The popup window is shown
	xtpPopupStateCollapsing		//The popup window is collapsing
};

//Notify ID to parent window
const UINT XTPWM_POPUPCONTROL_NOTIFY = (WM_APP + 2591);

//notify param for parent window
const UINT XTP_PCN_ITEMCLICK = 1;
const UINT XTP_PCN_STATECHANGED = 2;

//===========================================================================
// Summary: CXTPPopupControl is a class for control of popup window 
//===========================================================================

class _XTP_EXT_CLASS CXTPPopupControl : public CWnd
{
// Construction
public:
	
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPopupControl object.
	//-----------------------------------------------------------------------
	CXTPPopupControl();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPopupControl object, all handles cleanup and 
	//          deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPopupControl();

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the position of a popup 
	//          window.
	// Input:   ptPopup  - a CPoint object containing XY location.
	//-----------------------------------------------------------------------
	void SetPopupPos(CPoint ptPopup);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the position of a  popup
	//          window.
	// Returns: A CPoint object containing XY location.
	//-----------------------------------------------------------------------
	CPoint GetPopupPos();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the size of a popup window.          
	// Input:   szPopup  - A CSize object containing XY size.
	//-----------------------------------------------------------------------
	void SetPopupSize(CSize szPopup);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the size of a popup window.
	// Returns: A CSize object containing XY size.
	//-----------------------------------------------------------------------
	CSize GetPopupSize();

	//-----------------------------------------------------------------------
	// Summary: Call this member for create and starting expanding state 
	//          of a popup window
	// Input:	pParent - points to a CWnd object that represents the parent 
	//          window.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL Show(CWnd* pParent);

	//-----------------------------------------------------------------------
	// Summary: Call this member for starting collapsing state of a popup 
	//          window with the posterior destroy.
	//-----------------------------------------------------------------------
	void Hide();

	//-----------------------------------------------------------------------
	// Summary: Call this member to destroy a popup window.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL Close();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the transparency value of a 
	//          popup window.
	// Input:   bTransp - value of transparency (0...255). 
	//          If bTransp = 0 the popup window is full transparent, otherwise 
	//          if bTransp = 255 the popup window is full opaque.
	//-----------------------------------------------------------------------
	void SetTransparency(BYTE bTransp);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the transparency value a popup
	//          window.
	// Returns: Transparency value of a popup window.
	//-----------------------------------------------------------------------
	BYTE GetTransparency();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the value of show delay a 
	//          popup window.
	// Input:   dwShowDelay - value of show delay in milliseconds.
	//-----------------------------------------------------------------------
	void SetShowDelay(DWORD dwShowDelay);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the value of show delay a 
	//          popup window.
	// Returns: Transparency value of show delay in milliseconds.
	//-----------------------------------------------------------------------
	DWORD GetShowDelay();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the value of animate delay
	//          a popup window.
	// Input:   dwAnimateDelay - value of animate delay in milliseconds .
	//-----------------------------------------------------------------------
	void SetAnimateDelay(DWORD dwAnimateDelay);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the value of animate 
	//          delay a popup window.
	// Returns: Transparency value of animate delay in milliseconds. 
	//-----------------------------------------------------------------------
	DWORD GetAnimateDelay();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the popup animation method.
	// Input:   popupAnimation - method in use, see XTPPopupAnimation enum.
	//-----------------------------------------------------------------------
	void SetPopupAnimation(XTPPopupAnimation popupAnimation);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the popup animation method.
	// Returns: Method in use, see XTPPopupAnimation enum.
	//-----------------------------------------------------------------------
	XTPPopupAnimation GetPopupAnimation();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the current state of a popup  
	//          window.
	// Returns: Current state, see XTPPopupState enum.
	//-----------------------------------------------------------------------
	XTPPopupState GetPopupState();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the paint manager object used
	//          for drawing a popup window.
	// Input:   pPaintManager - point of paint manager object.
	//-----------------------------------------------------------------------
	void SetTheme(CXTPPopupPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the paint manager object used
	//          for drawing a popup window.
	// Returns: Point of paint manager object.
	//-----------------------------------------------------------------------
	CXTPPopupPaintManager* GetPaintManager();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to set the theme of popup window
	//          drawing.
	// Input:   theme - theme of popup window drawing, see XTPPopupPaintTheme
	//          enum.
	//-----------------------------------------------------------------------
	void SetTheme(XTPPopupPaintTheme theme);

	XTPPopupPaintTheme GetTheme();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to redraw all controlled items of a 
	//          popup window.
	//-----------------------------------------------------------------------
	void RedrawControl();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to install new controlled item in
	//          a popup window.
	// Input:   pItem - point to CXTPPopupItem object which will be installed
	//          in a popup window.
	// Returns: Point to the installed CXTPPopupItem object.
	//-----------------------------------------------------------------------
	CXTPPopupItem* AddItem(CXTPPopupItem* pItem);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to remove all controlled items from
	//          a popup window.
	//-----------------------------------------------------------------------
	void RemoveAllItems();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to remove controlled item from
	//          a popup window.
	// Input:   pItem - point to CXTPPopupItem object which will be installed
	//          in a popup window.
	//-----------------------------------------------------------------------
	void RemoveItem(CXTPPopupItem * pItem);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get count of controlled items
	//          a popup window.
	// Returns: Count of controlled items.
	//-----------------------------------------------------------------------
	int GetItemCount();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get a controlled item on an index
	// Input:   nIndex - index of a controlled item.
	// Returns: Point to CXTPPopupItem object.
	//-----------------------------------------------------------------------
	CXTPPopupItem* GetItem(int nIndex);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to testing a point for controlled 
	//          items of a popup window.
	// Input:   pt - a CPoint object containing XY location for testing.
	// Returns: Point to CXTPPopupItem object if test is successful, otherwise 
	//          returns NULL.
	//-----------------------------------------------------------------------
	CXTPPopupItem* HitTest(CPoint pt);

//Protected members for derive class
protected:
	virtual BOOL Create(CWnd* pParentWnd);			//proc of Create a popup window
	virtual void OnClick(CXTPPopupItem* pItem);		//proc for detecting click on controlled item
	virtual void OnExpanding(BOOL bUpdateCurrent);  //expanding proc
	virtual void OnCollapsing();					//collapsing proc
	virtual void OnShow();							//shown proc	

	virtual void Notify(WPARAM wParam, LPARAM lParam);

	void Animate(int nStep);						//animate proc
	void UpdateState(BOOL bInit = FALSE);			//update window proc
	void SetPopupState(XTPPopupState popupState);	//set popup window state proc
	BOOL SetLayeredWindowAttributes(BYTE bAlpha);	//set transparency proc

protected:
	struct POPUPSTATE								//popup state description
	{
		CRect rcPopup;
		int nTransparency;
	};
	CXTPPopupPaintManager* m_pPaintManager;			//point to CXTPPopupPaintManager object
	CArray<CXTPPopupItem*, CXTPPopupItem*> m_arrItems;//array for controlled items
	CXTPPopupItem* m_pSelected;						//point to selected item
	CXTPPopupItem* m_pPressed;						//point to pressed item
	XTPPopupAnimation m_popupAnimation;				//current animation method 
	XTPPopupState m_popupState;						//current popup state
	CSize m_szPopup;								//current XY location of a popup window
	CPoint m_ptPopup;								//current XY size of a popup window
	LONG m_nTransparency;							//current value transparency of a popup window
	UINT m_uShowDelay;								//value of show delay of a popup window
	UINT m_uAnimationDelay;							//value of animation delay of a popup window
	UINT m_nAnimationInterval;						//value of animation interval of a popup window
	int	 m_nStep;									//value of step animation of a popup window
	XTPPopupPaintTheme m_paintTheme;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPPopupControl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CXTPPopupControl)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnMouseLeave();
	afx_msg int OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT /*message*/);

	DECLARE_MESSAGE_MAP()

	friend class CXTPPopupItem;

private:
	PVOID		m_pfnSetLayeredWindowAttributes;		//point to Transparency proc in USER32.dll module
	HMODULE		m_hUser32;								//handle to USER32.dll module
	BOOL		m_bCapture;								//capture flag of mouse event
	POPUPSTATE	m_stateTarget;							//target descript
	POPUPSTATE	m_stateCurrent;							//current descript
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPPopupPaintManager* CXTPPopupControl::GetPaintManager() {
	return m_pPaintManager;
}
AFX_INLINE void CXTPPopupControl::SetPopupSize(CSize szPopup) {
	m_szPopup = szPopup;
}
AFX_INLINE CSize CXTPPopupControl::GetPopupSize() {
	return m_szPopup;
}
AFX_INLINE void CXTPPopupControl::SetPopupPos(CPoint ptPopup) {
	m_ptPopup = ptPopup;
}
AFX_INLINE CPoint CXTPPopupControl::GetPopupPos() {
	return m_ptPopup;
}
AFX_INLINE void CXTPPopupControl::SetShowDelay(DWORD dwShowDelay) {
	m_uShowDelay = dwShowDelay;
}
AFX_INLINE DWORD CXTPPopupControl::GetShowDelay() {
	return m_uShowDelay;
}
AFX_INLINE void CXTPPopupControl::SetAnimateDelay(DWORD dwAnimateDelay) {
	m_uAnimationDelay = dwAnimateDelay;
}
AFX_INLINE DWORD CXTPPopupControl::GetAnimateDelay() {
	return m_uAnimationDelay;
}
AFX_INLINE BYTE CXTPPopupControl::GetTransparency() {
	return (BYTE)m_nTransparency;
}
AFX_INLINE void CXTPPopupControl::SetTransparency(BYTE nTransparency) {
	m_nTransparency = nTransparency;
}
AFX_INLINE XTPPopupAnimation CXTPPopupControl::GetPopupAnimation() {
	return m_popupAnimation;
}
AFX_INLINE void CXTPPopupControl::SetPopupAnimation(XTPPopupAnimation popupAnimation) { 
	m_popupAnimation = popupAnimation;
}
AFX_INLINE XTPPopupPaintTheme CXTPPopupControl::GetTheme() {
	return m_paintTheme;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_XTPPOPUPCONTROL_H__)
