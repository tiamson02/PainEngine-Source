// XTResizeDialog.h: interface for the CXTResizeDialog class.
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
#if !defined(__XTRESIZEDIALOG_H__)
#define __XTRESIZEDIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Summary: CXTResizeDialog is a multiple inheritance class derived from CDialog
//			and CXTResize. CXTResizeDialog is used to create a resizable CDialog
//			type object that allows its dialog items to be resized or moved dynamically.
//===========================================================================
class _XT_EXT_CLASS CXTResizeDialog : public CDialog, public CXTResize
{
	DECLARE_DYNCREATE(CXTResizeDialog);

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTResizeDialog object
	//-----------------------------------------------------------------------
	CXTResizeDialog();

	//-----------------------------------------------------------------------
	// Input:	nID     - Contains the ID number of a dialog box template resource.
	//			pParent - Points to the parent or owner window object of type CWnd, which
	//					  the dialog object belongs to. If it is NULL, the dialog object’s parent
	//					  window is set to the main application window.
	//			nFlags	- Flags that are to be passed to CXTResize that specify the attributes
	//					  of the resizing property page. They can be one or more of the following,
	//					  and can be combined using the or (|) operator:
	//			[ul]
    //			[li]<b>SZ_NOSIZEICON</b> Do not add size icon.[/li]
    //			[li]<b>SZ_NOHORISONTAL</b> No horizontal resizing.[/li]
    //			[li]<b>SZ_NOVERTICAL</b> No vertical resizing.[/li]
    //			[li]<b>SZ_NOMINSIZE</b> Do not require a minimum size.[/li]
    //			[li]<b>SZ_NOCLIPCHILDREN</b> Do not set clip children style.[/li]
    //			[li]<b>SZ_NOTRANSPARENTGROUP</b> Do not set transparent style
	//			for group boxes.[/li]
	//			[/ul]
    // Summary:	Constructs a CXTResizeDialog object
	//-----------------------------------------------------------------------
	CXTResizeDialog(const UINT nID,CWnd* pParent = 0,const UINT nFlags = 0);

protected:

	UINT m_nDialogID; // Contains the ID number of a dialog box template resource

	//:Ignore
	//{{AFX_MSG(CXTResizeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

//:Ignore
//////////////////////////////////////////////////////////////////////

#endif // !defined(__XTRESIZEDIALOG_H__)
//:End Ignore