// XTPReportTip.h: interface for the CXTPReportTip class.
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

//:Ignore
#if !defined(__XTPREPORTTIP_H__)
#define __XTPREPORTTIP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


//:Ignore

// Internal class

/////////////////////////////////////////////////////////////////////////////
// CXTPReportTip window

//===========================================================================
// Remarks: The CXTPReportTip class encapsulates the functionality of 
//			a "tip control," a small pop-up window that displays a 
//			single line of text describing the hidden text when hovering an 
//			item on the report area.
//
//			CXTPReportTip provides the functionality to control the 
//			the tip text, the size of the tip window itself, and the text 
//			font of the tip. 
//
//			This class is used by the report control internally.
//
// See also: CXTPReportControl, CXTPReportRow::ShowToolTip
//===========================================================================
class _XTP_EXT_CLASS CXTPReportTip : public CWnd
{
	friend class CXTPReportRow;
// Construction
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPReportTip object.
	// Remarks: You construct a object object in two steps. 
	//			First, call the constructor object and then call 
	//			Create, which initializes the window and 
	//			attaches it to the parent window.
	//
	// Example:	// Declare a local CXTPReportTip object.
	//			CXTPReportTip wndTip;
	//
	//			// Declare a dynamic CXTPReportTip object.
	//			CXTPReportTip* pTipWnd = new CXTPReportTip;
	//
	// See also: CWnd, Create, Activate, CXTPReportRow::ShowToolTip
	//-----------------------------------------------------------------------
	CXTPReportTip();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPReportTip object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPReportTip();

	//-----------------------------------------------------------------------
	// Input:	pParentWnd - Parent report control window.
	// Summary: Creates report tip window.
	// Remarks: You construct an object in two steps. 
	//			First, call the constructor object and then call 
	//			Create, which initializes the window and 
	//			attaches it to the parent window.
	//
	// Example: void MyDialog::ShowToolTip(CRect rcTip, CXTPReportTip* pTipWnd)
	//			{
	//				if (!pTipWnd->GetSafeHwnd())
	//				{
	//					pTipWnd->Create(this);
	//				}
	//				pTipWnd->SetHoverRect(rcTip);
	//				pTipWnd->Activate(TRUE);
	//			}
	//				
	// Returns: TRUE if the tip window was created successfully, FALSE otherwise.
	// See also: CXTPReportTip::CXTPReportTip, Activate
	//-----------------------------------------------------------------------
	virtual BOOL Create(CWnd *pParentWnd);

// Attributes
public:

	//-----------------------------------------------------------------------
	// Summary: Returns tooltip text string.
	// Returns: Current tooltip text.
	// See also: SetTooltipText
	//-----------------------------------------------------------------------
	CString GetTooltipText();

	//-----------------------------------------------------------------------
	// Input:	str - A string with the new tooltip caption.
	// Summary: Sets new tooltip text to show.
	// Remarks: Should be called before activating the tooltip window.
	// See also: GetTooltipText
	//-----------------------------------------------------------------------
	void SetTooltipText(LPCTSTR str);
	
	//-----------------------------------------------------------------------
	// Input:	pFont - Pointer to the new font object.
	// Summary: Sets tooltip text displaying font.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);
	
	//-----------------------------------------------------------------------
	// Input:	rc - New tooltip hover rectangle in parent window coordinates.
	// Summary: Sets coordinates of the tooltip hover rectangle.
	// Remarks: Should be called before activating the tooltip window.
	// See also: GetHoverRect
	//-----------------------------------------------------------------------
	void SetHoverRect(CRect rc);

	//-----------------------------------------------------------------------
	// Summary: Returns the coordinates of the tooltip hover rectangle.
	// Returns: Coordinates of the tooltip hover rectangle in parent window coordiantes.
	// See also: SetHoverRect
	//-----------------------------------------------------------------------
	CRect GetHoverRect();


// Operations
public:
	
	//-----------------------------------------------------------------------
	// Input:	bActive - Specifies whether the tool tip control is to be activated or deactivated. 
	// Summary: Call this function to activate or deactivate a tool tip control.
	// Remarks: If <i>bActivate</i> is <b>TRUE</b>, the control is activated; 
	//			if <b>FALSE</b>, it is deactivated.
	//
	//			When a tool tip control is active, the tool tip information 
	//			appears when the cursor is on a tool that is registered with 
	//			the control; when it is inactive, the tool tip information 
	//			does not appear, even when the cursor is on a tool.
	// Example: See Create method example.
	// See also: Create, CXTPReportTip::CXTPReportTip, SetTooltipText
	//-----------------------------------------------------------------------
	void Activate(BOOL bActive);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReportTip)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CRect  m_rcHover;			// Hover window coordinates.
	CWnd * m_pParentWnd;		// Pointer to the parent report window.
	CString m_strTooltipText;	// Tooltip text to display.
	CXTPReportRecordItem* m_pItem;

	CFont m_fntToolTip;			// Font for displaying tooltip text.

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReportTip)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

AFX_INLINE CString CXTPReportTip::GetTooltipText() {
	return m_strTooltipText;
}
AFX_INLINE void CXTPReportTip::SetTooltipText(LPCTSTR str) {
	m_strTooltipText = str;
}
AFX_INLINE void CXTPReportTip::SetFont(CFont* pFont) {	
	m_fntToolTip.DeleteObject();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_fntToolTip.CreateFontIndirect(&lf);	
}
AFX_INLINE void CXTPReportTip::SetHoverRect(CRect rc) {
	m_rcHover = rc;
	MoveWindow(rc);
}
AFX_INLINE CRect CXTPReportTip::GetHoverRect() {
	return m_rcHover;
}

//:End Ignore

#endif // !defined(__XTPREPORTTIP_H__)
