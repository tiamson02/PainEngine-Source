// XTPMacros.h : common macros
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©1998-2004 Codejock Software, All Rights Reserved.
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

#if !defined(__XTPMACROS_H__)
#define __XTPMACROS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef AFX_INLINE
#define AFX_INLINE inline
#endif

#ifndef CBRS_GRIPPER
#define CBRS_GRIPPER                    0x00400000L
#endif

#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL                 0x00400000L
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//-----------------------------------------------------------------------------
// Summary: This macro will free the dynamically allocated memory specified by 
//			'p' and set its value to NULL. 
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  if(p) { delete p; p = NULL; }

//-----------------------------------------------------------------------------
// Summary: This macro will free the dynamically allocated memory for an array 
//			specified by 'p' and set its value to NULL. 
//-----------------------------------------------------------------------------
#define SAFE_DELETE_AR(p)	if(p) { delete [] p; p = NULL; }

//-----------------------------------------------------------------------------
// Summary: This macro will release the resources for the COM object specified 
//			by 'p' and set its value to NULL. 
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(p)		if(p) { (p)->Release(); (p)=NULL; }

//-----------------------------------------------------------------------------
// Uncomment this definition if your application uses a resource dll, and add 
// #include "XTResource.rc" to your resource dll's .rc2 file.  You will have to 
// call XTAuxData().InitResources(HINSTANCE), once you have a resource handle, to 
// initialize the Xtreme Toolkit resources.  You will need to rebuild the 
// library after you have done this.
//-----------------------------------------------------------------------------
//#define _XT_USES_RESOURCE_DLL

//-----------------------------------------------------------------------------
// Summary: XT_ASSERT_MSG is similar to ASSERT, except that it allows a custom
//			message to be specified.
//-----------------------------------------------------------------------------
#ifdef _DEBUG
#ifndef _AFX_NO_DEBUG_CRT

#define XT_ASSERT_MSG(exp, msg) \
{ \
	if ( !(exp) && (_CrtDbgReport( _CRT_ASSERT, __FILE__, __LINE__, NULL, "\n-----------------------\n" msg "\n-----------------------" ) ) ) \
		AfxDebugBreak(); \
} \

#else
#define XT_ASSERT_MSG(exp, msg) (void)( (exp) || (_assert("\n-----------------------\n" msg "\n-----------------------", __FILE__, __LINE__), 0) )
#endif//_AFX_NO_DEBUG_CRT

#else
#define XT_ASSERT_MSG(exp, msg) ((void)0)
#endif//_DEBUG

//-----------------------------------------------------------------------------

#endif // #if !defined(__XTPMACROS_H__)
