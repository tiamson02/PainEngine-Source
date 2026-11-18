// XTWindowPos.h interface for the CXTWindowPos class.
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
#if !defined(__XTWINDOWPLACEMENT_H__)
#define __XTWINDOWPLACEMENT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTWindowPos is a WINDOWPLACEMENT structure derived class. It extends
//			the WINDOWPLACEMENT structure, and is used to save and restore window
//			position.
class _XT_EXT_CLASS CXTWindowPos : public WINDOWPLACEMENT
{
public:

	// Summary: Constructs a CXTWindowPos object
	CXTWindowPos();

	// Summary: Destroys a CXTWindowPos object, handles cleanup and de-allocation
	virtual ~CXTWindowPos();

	// Input:	lpszWndPos - If NULL, the default entry name will be used. If using this for MDI
	//			children or other windows, pass in a unique string value here. This
	//			must match for both LoadWindowPos and SaveWindowPos.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary:	This member function will load the stored window position settings
	//			without applying them to any window. 
	BOOL LoadWindowPos(LPCTSTR lpszWndPos=NULL);

	// Input:	pWnd - Points to the CWnd* derived window to be restored to its previous state.
	//			lpszWndPos - If NULL, the default entry name will be used. If using this for MDI
	//			children or other windows, pass in a unique string value here. This
	//			must match for both LoadWindowPos and SaveWindowPos.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary:	This member function will load the window specified by 'pWnd' to its
	//			previous window position. 
	BOOL LoadWindowPos(CWnd* pWnd,LPCTSTR lpszWndPos=NULL);

	// Input:	pWnd - Points to the CWnd* derived windows position to be saved.
	//			lpszWndPos - If NULL, the default entry name will be used. If using this for MDI
	//			children or other windows, pass in a unique string value here. This
	//			must match for both LoadWindowPos and SaveWindowPos.
	// Returns: TRUE if successful, otherwise returns FALSE.
	// Summary:	This member function will save the window specified by 'pWnd' by its
	//			current window position. 
	BOOL SaveWindowPos(CWnd* pWnd,LPCTSTR lpszWndPos=NULL);
};

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTWINDOWPLACEMENT_H__)
//:End Ignore