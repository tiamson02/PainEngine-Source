// XTPStatusBar.h : interface for the CXTPStatusBar class.
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
#if !defined(__XTPSTATUSBAR_H__)
#define __XTPSTATUSBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTPCommandBars;
class CXTPPaintManager;

//===========================================================================
// Remarks: CXTPStatusBar is a CStatusBar derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPStatusBar : public CStatusBar
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPStatusBar object
	//-----------------------------------------------------------------------
	CXTPStatusBar();

	//-----------------------------------------------------------------------
	// Input:	lpIDArray - Pointer to an array of IDs. 
	//			nIDCount  - Number of elements in the array pointed to by lpIDArray.
	// Summary: Sets each indicator's ID to the value specified by the corresponding 
	//			element of the array lpIDArray, loads the string resource specified 
	//			by each ID, and sets the indicator's text to the string.
	// Returns: TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
    BOOL SetIndicators (const UINT* lpIDArray, int nIDCount);

	//-----------------------------------------------------------------------
	// Input:	pCommandBars - CXTPCommandBars to be set.
	// Summary: Call this member to set the command bar's object.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary: Retrieves the paint manager.
	// Returns: A pointer to a CXTPPaintManager object.
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager();

protected:

	//-----------------------------------------------------------------------
	// Input:	pDC		 - Pointer to a valid device context.
	//			rcClient - Client rectangle.
	// Summary: Draws the stretch gripper of the status bar.
	//-----------------------------------------------------------------------
	void DrawStretch(CDC* pDC, CRect& rcClient);

private:

    DECLARE_MESSAGE_MAP()    

	//{{AFX_MSG(CXTPStatusBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	CXTPCommandBars* m_pCommandBars;
	CFont m_fontStatus;

};

AFX_INLINE void CXTPStatusBar::SetCommandBars(CXTPCommandBars* pCommandBars) {
	m_pCommandBars = pCommandBars;
}

#endif //#if !defined(__XTPSTATUSBAR_H__)