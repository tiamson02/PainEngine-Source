// XTResizePropertyPage.h: interface for the CXTResizePropertyPage class.
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
#if !defined(__XTRESIZEPROPERTYPAGE_H__)
#define __XTRESIZEPROPERTYPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTResizePropertyPage is a multiple inheritance class derived from 
//			CPropertyPage and CXTResize. CXTResizePropertyPage is used to create
//			a resizable CPropertyPage type object that allows its dialog items to
//			be resized or moved dynamically.
class _XT_EXT_CLASS CXTResizePropertyPage : public CPropertyPage, public CXTResize
{
	DECLARE_DYNCREATE(CXTResizePropertyPage)

public:

	// Input:	nTemplate - ID of the template used for this page.
	//			nCaption - ID of the name to be placed in the tab for this page. If 0, the name
	//			will be taken from the dialog template for this page.
	//			nFlags - Flags that are to be passed to CXTResize that specify the attributes
	//			of the resizing property page. They can be one or more of the following,
	//			and can be combined using the or (|) operator:
	//			[ul]
    //			[li]<b>SZ_NOSIZEICON</b> Do not add size icon.[/li]
    //			[li]<b>SZ_NOHORISONTAL</b> No horizontal resizing.[/li]
    //			[li]<b>SZ_NOVERTICAL</b> No vertical resizing.[/li]
    //			[li]<b>SZ_NOMINSIZE</b> Do not require a minimum size.[/li]
    //			[li]<b>SZ_NOCLIPCHILDREN</b> Do not set clip children style.[/li]
    //			[li]<b>SZ_NOTRANSPARENTGROUP</b> Do not set transparent style
	//			for group boxes.[/li]
	//			[/ul]
    // Summary:	Constructs a CXTResizePropertyPage object
	CXTResizePropertyPage(const UINT nTemplate = 0,const UINT nCaption = 0,const UINT nFlags = 0);

	DWORD m_nDialogID; // ID of the template used for this page

protected:

	//:Ignore
	//{{AFX_MSG(CXTResizePropertyPage)
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

#endif // !defined(__XTRESIZEPROPERTYPAGE_H__)
//:End Ignore