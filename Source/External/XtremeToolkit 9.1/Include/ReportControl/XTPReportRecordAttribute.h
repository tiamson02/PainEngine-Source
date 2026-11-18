// XTPReportRecordAttribute.h: interface for the CXTPReportRecordAttribute class.
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
#if !defined(__XTPREPORTRECORDATTRIBUTE_H__)
#define __XTPREPORTRECORDATTRIBUTE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportRecord;

//===========================================================================
// Summary:  Defines the base class for custom/multiple states, like Read/Unread, 
//           that can be linked to individual rows.
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordAttribute : public CCmdTarget
{
	DECLARE_DYNAMIC(CXTPReportRecordAttribute)
public:

	//-----------------------------------------------------------------------
	// Summary: Creates a basic record attribute object.
	//-----------------------------------------------------------------------
	CXTPReportRecordAttribute(void);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys attribute object, handles its cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRecordAttribute(void);

	//-----------------------------------------------------------------------
	// Input:	pRecord - A record to apply a state on.
	// Summary: This should be overridden by descendants to apply the current attribute 
	//			state to the record properties.
	//-----------------------------------------------------------------------
	virtual void ApplyState(CXTPReportRecord* pRecord) = 0;
};

#endif