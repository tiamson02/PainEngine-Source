// XTPPopupBar.h : interface for the CXTPPopupBar class.
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
#if !defined(__XTPPOPUPBAR_H__)
#define __XTPPOPUPBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBar.h"

class CXTPControlPopup;
class CXTPCommandBars;

// Summary: CXTPPopupBar is a CXTPCommandBar derived class.
class _XTP_EXT_CLASS CXTPPopupBar : public CXTPCommandBar
{
	DECLARE_XTP_COMMANDBAR(CXTPPopupBar)

	CXTPPopupBar(void);
	~CXTPPopupBar(void);

	class CXTPControlExpandButton;


	struct SCROLLINFO
	{
		struct BTNSCROLL
		{
			void Init(CWnd* pParent, UINT nID)
			{
				m_pParent = pParent;
				m_nID = nID;
			}

			CRect rc;
			UINT nTimer;
			CWnd* m_pParent;
			UINT m_nID;

			void KillTimer() { m_pParent->KillTimer(nTimer); nTimer = 0;}
			void SetTimer() { if (nTimer == 0) nTimer = (UINT)m_pParent->SetTimer(m_nID, 80, 0); }
			BOOL OnMouseMove(CPoint point)
			{
				if (rc.PtInRect(point))
				{
					SetTimer();
					return TRUE;
				} else if (nTimer) KillTimer();
				return FALSE;
			}			
		};

		BOOL bScroll;
		int nScrollFirst, nScrollLast;
		BTNSCROLL btnUp;
		BTNSCROLL btnDown;
		
		void Init(CWnd* pWnd, UINT nIDUp, UINT nIDDown)			
		{
			btnUp.Init(pWnd, nIDUp);
			btnDown.Init(pWnd, nIDDown);
			bScroll = nScrollFirst = nScrollLast = 0;
		}
	} m_scrollInfo;

public:

	// Input:	pCommandBars - Points to a CXTPCommandBars object
	// Returns: A pointer to a CXTPPopupBar object
	// Summary: Creates a popup bar object.
	static CXTPPopupBar* AFX_CDECL CreatePopupBar(CXTPCommandBars* pCommandBars);

	// Input:	strCaption - Caption of the tear-off toolbar.
	//			nID - Identifier of the toolbar to be created.
	//			nWidth - Width of the toolbar.
	// Summary: This method adds the tear-off state to the popup bar.
	void SetTearOffPopup(CString strCaption, UINT nID, int nWidth = 400);
	
	// Input:	strCaption - Caption of the tear-off toolbar.
	//			nID - Identifier of the toolbar to be created.
	//			nWidth - Width of the toolbar.
	// Returns: TRUE if tear-off; otherwise returns FALSE
	// Summary: Determines if Popupbar has tear-off state.
	BOOL IsTearOffPopup(CString& strCaption, UINT& nID, int& nWidth);

public:	

	// Input:	x - Specifies the logical x-coordinate of the popup bar
	//			y - Specifies the logical y-coordinate of the  position. 
	//			rcExclude - Excluded area.
	// Returns: TRUE if successful; otherwise returns FALSE.
	// Summary: This method opens the popup bar.
	BOOL Popup(int x, int y, LPCRECT rcExclude);
	
	// Returns: A pointer to a CXTPControlPopup object.
	// Summary: Call this member to retrieve the control's popup.
	CXTPControlPopup* GetControlPopup() { return m_pControlPopup; }

	// Summary: This method expands the bar.
	void ExpandBar();
	
	// Returns: TRUE if successful; otherwise returns FALSE.
	// Summary: Call this member to destroy the window.
	BOOL DestroyWindow();

	void SetPopupToolBar(BOOL bToolBarType);

	// Input:	uItem - Identifier or position of the new default menu item or - 1 for no default item. The meaning of this parameter depends on the value of fByPos. 
	//          fByPos - Value specifying the meaning of uItem. If this parameter is FALSE, uItem is a menu item identifier. Otherwise, it is a menu item position. 
	// Summary: Sets the default menu item for the specified popup bar.
	void SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);

	// Input:   gmdiFlags - Reserved. Should be NULL.
	//          fByPos - Value specifying whether to retrieve the menu item's identifier or its position. If this parameter is FALSE, the identifier is returned. Otherwise, the position is returned. 
	// Returns: If the function succeeds, the return value is the identifier or position of the menu item. If the function fails, the return value is - 1.
	// Summary: Determines the default menu item on the specified popup bar.
	UINT GetDefaultItem(UINT gmdiFlags, BOOL fByPos = FALSE);

protected:
	
	// Summary: Redraw the popup bar.
	void DelayRedraw() { if (m_hWnd) Invalidate(FALSE); }
	
	// Input:	pControlPopup - Pointer to a CXTPControlPopup object
	//			bSelectFirst - TRUE to select the first item.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method opens the popup bar.
	BOOL Popup(CXTPControlPopup* pControlPopup, BOOL bSelectFirst = FALSE);
	
	// Returns: Size of the command bar.
	// Summary:	This method is called to get the size of the command bar.
	CSize CalcDynamicLayout(int, DWORD nMode);
	
	// Input:	 bMode - TRUE to set the tracking mode; otherwise FALSE.
	//			 bSelectFirst - TRUE to select the first item.
	// Returns:  TRUE if the method was successful.
	// Summary:  Call this member to change the tracking state.
	// See Also: IsTrackingMode.
	BOOL SetTrackingMode(int bMode, BOOL bSelectFirst = TRUE, BOOL bKeyboard = FALSE);
	
	// Input:	nSelected - An integer index of the item to be selected.
	//			bKeyboard - TRUE if the item was selected using the keyboard.
	// Returns: TRUE if the method was successful; otherwise FALSE.
	// Summary: Call this member to select the specified control.
	BOOL SetSelected(int nSelected, BOOL bKeyboard = FALSE);
	
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Creates the popup bar.
	BOOL Create();
	
	// Input:	sz - Size of the popup bar.
	// Summary: This method sets the PopupBar position.
	void SetPosition(CSize sz);
	
	// Summary: Updates the control's shortcuts.
	virtual void UpdateShortcuts();
	
	// Summary: Update flags.
	void UpdateFlags();
	
	// Summary: Update expanding state.
	void UpdateExpandingState();
	
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to draw the command bar in the given context.
	void DrawCommandBar(CDC* pDC);
	
	// Summary: This method is called when the controls array is changed.
	virtual void OnControlsChanged();
	
	// Summary: This method is called to recalculate the command bar layout.
	void OnRecalcLayout();

	// Returns: A pointer to a CXTPCommandBar object
	// Summary: Call this member to retrieve the parent command bar
	CXTPCommandBar* GetParentCommandBar() const;
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
 	// Input:	pCommandBar - Command bar needed to be copied.
	//			bRecursive - TRUE to copy recursively.
	// Summary: Call this method make a copy of the command bar.
	virtual void Copy(CXTPCommandBar* pCommandBar, BOOL bRecursive = FALSE);

	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	The framework calls this member function when a non-system key is pressed.
	virtual BOOL OnKeyDown(UINT nChar);

	void PumpMessage();
	
private:
	void AdjustScrolling(LPSIZE lpSize, BOOL bDown = FALSE, BOOL bInvalidate = FALSE);
	void TrackTearOff();
	void SwitchTearOffTracking(BOOL bShow, CPoint point);
	
	void Animate();
	XTPAnimationType GetAnimationType();


protected:
	CXTPControlPopup* m_pControlPopup;	// Control that popuped the bar.
	CPoint m_ptPopup;					// Position of the bar.
	CRect m_rcExclude;					// Excluded area.
	int  m_popupFlags;					// Flags of the bar.
	BOOL m_bDynamicLayout;				// TRUE if DynamicLayout routine executed.
	BOOL m_bCollapsed;					// TRUE if popupbar is collapsed.
	BOOL m_bExpanded;					// TRUE if popupbar is expanded.
	BOOL m_bExpanding;					// TRUE if popupbar is expanding.
	
	BOOL m_bTearOffPopup;				// TRUE if popupbar has tear-off option.
	CRect m_rcTearOffGripper;			// Tear-off gripper position.
	BOOL m_bTearOffSelected;			// TRUE if gripper selected.
	UINT m_nTearOffTimer;				// Tear-off timer.
	BOOL m_bTearOffTracking;			// Tear-off tracking mode.
	CSize m_szTearOffBar;				// Size of the bar.
	CXTPToolBar* m_pTearOffBar;			// Tear-off toolbar pointer.
	CString m_strTearOffCaption;		// Caption of the Tear-off toolbar.
	UINT m_nTearOffID;					// ID of the Tear-off toolbar.
	int m_nTearOffWidth;				// Width of the Tear-off toolbar.
	BOOL m_bShowShadow;					// TRUE to show the shadow.
	CRect m_rcBorders;                  // Borders of the bar.
	

private:
	DECLARE_MESSAGE_MAP()														

	//{{AFX_MSG(CXTPPopupBar)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG


	friend class CXTPControlExpandButton;	
	friend class CXTPControlPopup;			
	friend class CXTPCommandBars;	
	friend class CXTPCommandBar;
};

// Summary: CXTPPopupToolBar is a CXTPPopupBar derived class.
class _XTP_EXT_CLASS CXTPPopupToolBar : public CXTPPopupBar
{
	DECLARE_XTP_COMMANDBAR(CXTPPopupToolBar)	

	// Summary: Constructs a CXTPPopupToolBar object
	CXTPPopupToolBar();

public:

	// Input:	pCommandBars - Points to a CXTPCommandBars object
	// Returns: A pointer to a CXTPPopupToolBar object
	// Summary: Creates a popup toolbar object.
	static CXTPPopupToolBar* AFX_CDECL CreatePopupToolBar(CXTPCommandBars* pCommandBars);


	// Input:	rcBorders - Borders of the toolbar.
	// Summary: Sets the toolbars borders.
	void SetBorders(CRect rcBorders);
	
	// Summary: Disables shadow use.
	void DisableShadow();

protected:

	// Input:	ar - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	virtual void Serialize(CArchive& ar);
	
	// Returns: Size of the command bar.
	// Summary:	This method is called to get the size of the command bar.
	CSize CalcDynamicLayout(int, DWORD nMode);		

};

//////////////////////////////////////////////////////////////////////////



AFX_INLINE void CXTPPopupToolBar::DisableShadow() { 
	m_bShowShadow = FALSE;
}
AFX_INLINE void CXTPPopupToolBar::SetBorders(CRect rcBorders) {
	m_rcBorders = rcBorders;
}

#endif // #if !defined(__XTPPOPUPBAR_H__)