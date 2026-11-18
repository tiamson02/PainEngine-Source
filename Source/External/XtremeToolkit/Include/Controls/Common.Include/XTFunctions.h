// XTFunctions.h : global functions.
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
#if !defined(__XTFUNCTIONS_H__)
#define __XTFUNCTIONS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#if defined(XT_INIT_BY_REGULAR_DLL)

//-----------------------------------------------------------------------------
// Summary: This member function will initialize the resources for the Xtreme 
//          Toolkit.  If using the Xtreme Toolkit as an extension DLL within a 
//          regular DLL, add the following two lines to your stdafx.h file. 
//          This will give you access to the exported function 
//          InitXtremeExtDll():
// 
// Example:	<pre>#define XT_INIT_BY_REGULAR_DLL
//			#include "XTToolkit.h"</pre>
// 
//			<p>You will then need to add XT_INIT_BY_REGULAR_DLL to the Xtreme 
//          Toolkit preprocessor definitions and rebuild it. After you have 
//          done this, locate your CWinApp::InitInstance() method for your 
//          regular DLL and make the following call:</p>
// 
//			<pre>InitXtremeExtDLL();</pre>
// 
//			<p>This will initialize the resources for the Xtreme Toolkit.</p>
//-----------------------------------------------------------------------------
_XT_EXT_CLASS void WINAPI InitXtremeExtDLL();

#endif//#if defined(XT_INIT_BY_REGULAR_DLL)

//-----------------------------------------------------------------------------
// Input:	pWnd  - Pointer to a valid CWnd object.
//			pFont - Pointer to the new font to set for the window.
// Summary:	This member function will set the font for the window specified by
//			'pWnd' and all of the child windows owned by 'pWnd'.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS void AFXAPI _xtAfxChangeWindowFont(CWnd* pWnd,CFont* pFont);

//-----------------------------------------------------------------------------
// Input:	pDC       - Pointer to the current device context.
//			imageList - Address of an image list.
//			nIndex    - Index of the image in the image list.
//			point     - XY location of where to draw the icon.
//			bInColor  - TRUE to draw the item in color, otherwise the icon will
//                      be drawn with the default disabled look.
// Summary:	This member function will draw an embossed icon, from the image
//			list that is passed in, into the specified device context. It is 
//          typically used by toolbars and menus to draw a disabled icon in color.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS void AFXAPI _xtAfxDrawEmbossed(CDC* pDC,CImageList& imageList,int nIndex,CPoint point,BOOL bInColor);

//-----------------------------------------------------------------------------
// Input:	pDC  - Points to the current device context.
//			rect - Size of the area to draw.
// Summary:	This member function will draw a shadow rect into the specified
//			device context.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS void AFXAPI _xtAfxDrawShadedRect(CDC *pDC,CRect& rect);

//-----------------------------------------------------------------------------
// Input:	hInst - Instance handle of the module that has the required resource.
//			hRsrc - Bitmap resource handle.
//			bMono - Tells if monochrome conversion shall be applied.
// Summary:	This member function loads a bitmap from resource and translates the
//			bitmap color such that they suit requirements for a toolbar bitmap.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS HBITMAP AFXAPI _xtAfxLoadSysColorBitmap(HINSTANCE hInst,HRSRC hRsrc,BOOL bMono = FALSE);

//-----------------------------------------------------------------------------
// Input:	hWnd - HWND handle of the parent window to find the child for.
//			pt   - Current cursor position.
// Summary:	This member function will retrieve an HWND handle for the child 
//          window found directly under the cursor position specified by 'pt'.
// Returns: An HWND handle for the child window at 'pt'.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS HWND AFXAPI _xtAfxChildWindowFromPoint(HWND hWnd,POINT pt);

//-----------------------------------------------------------------------------
// Input:	strText - A reference to a CString object.
// Summary:	This member function will search a string, strip off the mnemonic
//			'&', and reformat the string.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS void AFXAPI _xtAfxStripMnemonics(CString& strText);

//-----------------------------------------------------------------------------
// Input:	lpszFileName - Fully qualified path for the item to find.
// Summary:	This member function will test for the existence of a file or folder.
// Returns: TRUE if successful, otherwise returns FALSE.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS BOOL AFXAPI _xtAfxExist(LPCTSTR lpszFileName);

//-----------------------------------------------------------------------------
// Input:	pszPath - Pointer to a NULL-terminated string with the path. Paths 
//                    are delimited by backslashes or by the NULL at the end of
//                    the path. 
// Summary:	This member function parses a path for the next path component, not
//			dependent on shell32.dll, and is a replacement for the windows API 
//			PathFindNextComponent. 
// Returns: A pointer to a NULL-terminated string with the next path component
//			if successful, or NULL otherwise.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS LPTSTR AFXAPI _xtAfxPathFindNextComponent(LPTSTR pszPath);

//-----------------------------------------------------------------------------
// Input:	pParentWnd - Points to the parent of the view to be created. The 
//                       parent must be valid.
//			pViewClass - CView runtime class.
//			pDocument  - CDocument associated with the view. It can be NULL.
//			pContext   - Create context for the view. It can be NULL.
//			dwStyle    - Default style for the view.
//			pOwnerWnd  - Owner of the view. If NULL, 'pParentWnd' is used.
//			nID        - Control ID of the view.
// Summary:	This member function dynamically creates a view based on a 
//          CRuntimeClass object.
// Returns: A CWnd* pointer to the newly created view if successful, 
//			otherwise returns NULL.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS CWnd* AFXAPI _xtAfxCreateView(CWnd* pParentWnd,CRuntimeClass *pViewClass, CDocument *pDocument=NULL,CCreateContext *pContext=NULL,DWORD dwStyle=AFX_WS_DEFAULT_VIEW,CWnd* pOwnerWnd=NULL,UINT nID=AFX_IDW_PANE_FIRST);

//-----------------------------------------------------------------------------
// Input:	lparam1    - Corresponds to the lParam member of the TV_ITEM 
//                       structure for the two items being compared.
//			lparam2    - Corresponds to the lParam member of the TV_ITEM 
//                       structure for the two items being compared.
//			lparamSort - The 'lParamSort' member corresponds to the lParam 
//                       member of TV_SORTCB.
// Summary:	This member function is a callback function used by CXTShellTreeCtrl
//			and CXTShellTreeView, and is called during a sort operation each
//			time the relative order of two list items needs to be compared.
// Returns: A negative value if the first item should precede the second, 
//			a positive value if the first item should follow the second, or zero 
//			if the two items are equivalent.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS int CALLBACK _xtAfxTreeViewCompareProc(LPARAM lparam1,LPARAM lparam2,LPARAM lparamSort);

//-----------------------------------------------------------------------------
// Input:	lparam1    - Corresponds to the lParam member of the LV_ITEM 
//                       structure for the two items being compared.
//			lparam2    - Corresponds to the lParam member of the LV_ITEM 
//                       structure for the two items being compared.
//			lparamSort - The 'lParamSort' member corresponds to the lParam 
//                       member of LV_SORTCB.
// Summary:	This member function is a callback function used by CXTShellListCtrl
//			and CXTShellListView, and is called during a sort operation each 
//          time the relative order of two list items needs to be compared.
// Returns: A negative value if the first item should precede the second, 
//			a positive value if the first item should follow the second, or zero 
//			if the two items are equivalent.
//-----------------------------------------------------------------------------
_XT_EXT_CLASS int CALLBACK _xtAfxListViewCompareProc(LPARAM lparam1,LPARAM lparam2,LPARAM lparamSort);

//:Ignore
//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTFUNCTIONS_H__)
//:End Ignore