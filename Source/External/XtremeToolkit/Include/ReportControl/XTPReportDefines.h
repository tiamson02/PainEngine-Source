// CXTPReportDefines.h
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
#if !defined(__XTPREPORTDEFINES_H__)
#define __XTPREPORTDEFINES_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// Custom classname for the main control 
#define XTPREPORTCTRL_CLASSNAME		_T("XTPReport")


// Defines constant for non-defined color.
#define XTP_REPORT_COLOR_DEFAULT (COLORREF)-1

// Summary: Defines no icon value.
#define XTP_REPORT_NOICON					(UINT)-1

// Summary: Defines message for handling field chooser showing.
#define XTP_NM_REPORT_SHOWFIELDCHOOSER		(NM_FIRST-50)

// Summary: Defines message for header context menu
#define XTP_NM_REPORT_HEADER_RCLICK			(NM_FIRST-51)

// Summary: Defines message for handling rows selection changed event.
#define XTP_NM_REPORT_SELCHANGED			(NM_FIRST-52)

// Summary:Defines message for handling row checking event.
#define XTP_NM_REPORT_CHECKED				(NM_FIRST-53)

#define XTP_NM_REPORT_HYPERLINK				(NM_FIRST-54)

#define XTP_NM_REPORT_COLUMNORDERCHANGED	(NM_FIRST-55)
#define XTP_NM_REPORT_SORTORDERCHANGED      (NM_FIRST-56)

#define XTP_NM_REPORT_VALUECHANGED			(NM_FIRST-57)

#define XTP_NM_REPORT_INPLACEBUTTONDOWN		(NM_FIRST-58)


#define XTP_TRACE


#endif