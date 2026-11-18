// XTResizeGroupBox.h : header file
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

#if !defined(__XTRESIZEGROUPBOX_H__)
#define __XTRESIZEGROUPBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------------------------------------------------------
// CXTResizeGroupBox is a CButton derived class that can be used to display
// a flicker free group box while resizing.  This class is intended to only
// be used as a group box and only used on a CXTResize window.  You can 
// subclass or create this object the way you would any CButton.
//
// Note: Do not use the transparent flag with this control, and make sure you
// call SetFlag(SZ_NOTRANSPARENTGROUP); to disable the resize manager from
// setting this for you.
//---------------------------------------------------------------------------
class _XT_EXT_CLASS CXTResizeGroupBox : public CButton
{
    DECLARE_DYNAMIC(CXTResizeGroupBox)

public:

	//-----------------------------------------------------------------------
    // Summary: Constructs a CXTResizeGroupBox object 
	//-----------------------------------------------------------------------
	CXTResizeGroupBox();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTResizeGroupBox object, handles cleanup and 
	//          de-allocation 
	//-----------------------------------------------------------------------
	virtual ~CXTResizeGroupBox();

protected:

	//-----------------------------------------------------------------------
	// Input:   pDC      - Pointer to device context.
	//          rcClient - Client area of group box.
	// Summary: Called during paint operations to exclude the control windows
	//          that are grouped by this control.
	//-----------------------------------------------------------------------
	virtual void Exclude(CDC* pDC, CRect& rcClient);

	//{{AFX_VIRTUAL(CXTResizeGroupBox)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CXTResizeGroupBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTRESIZEGROUPBOX_H__)
