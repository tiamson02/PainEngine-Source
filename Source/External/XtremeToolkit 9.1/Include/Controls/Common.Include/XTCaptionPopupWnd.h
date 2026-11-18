// XTCaptionPopupWnd.h : interface for the CXTCaptionPopupWnd class.
//
// This file is a part of the XTREME TOOLKIT MFC class library.
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
#if !defined(__XTCAPTIONPOPUPWND_H__)
#define __XTCAPTIONPOPUPWND_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTCaptionPopupWnd is a CWnd derived class. It is used by the CXTCaption
//			class to display a popup child window similar to Outlook&trade;.
class _XT_EXT_CLASS CXTCaptionPopupWnd : public CWnd
{
	DECLARE_DYNAMIC(CXTCaptionPopupWnd)

public:

    // Summary: Constructs a CXTCaptionPopupWnd object
	CXTCaptionPopupWnd();

	// Summary: Destroys a CXTCaptionPopupWnd object, handles cleanup and de-allocation
    virtual ~CXTCaptionPopupWnd();

protected:
	
    CWnd*            m_pParentWnd;    // A CWnd pointer that represents the parent window.
    CWnd*            m_pChildWnd;     // A CWnd pointer that represents the child displayed in the popup.
    CWnd*            m_pChildParent;  // A CWnd pointer that represents the parent of the child window.
    CXTCaption       m_Caption;       // Caption that is displayed when the window is active.
    CXTCaptionButton m_CaptionButton; // Close button associated with the caption.
    CXTIconHandle    m_hIconOn;       // Handle to a normal button icon.
    CXTIconHandle    m_hIconOff;      // Handle to a pressed button icon.

public:
	
    // Input:	rect - A CRect reference that represents the size of the popup window.
	//			pParentWnd - A CWnd pointer that represents the popup window.
	//			pChildWnd - A CWnd pointer that represents the child to be displayed when
	//			the window is activated.
	// Returns: TRUE if successful, otherwise returns FALSE.
    // Summary: This member function is called to create and display the popup
    //			window.  
	virtual BOOL Create(const CRect& rect, CWnd* pParentWnd, CWnd* pChildWnd);

	//:Ignore
	//{{AFX_VIRTUAL(CXTCaptionPopupWnd)
	//}}AFX_VIRTUAL
	//:End Ignore

protected:

    //:Ignore
	//{{AFX_MSG(CXTCaptionPopupWnd)
	afx_msg void OnDestroy();
	afx_msg void OnCaptButton();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTCAPTIONPOPUPWND_H__)
//:End Ignore