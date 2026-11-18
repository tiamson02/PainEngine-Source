// XTMaskEdit.h interface for the CXTMaskEdit class.
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
#if !defined(__XTMASKEDIT_H__)
#define __XTMASKEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "CommonPro.Include\XTMaskEditT.h"
//===========================================================================
// Remarks: CXTMaskEdit is a CXTFlatEdit derived class. It allows text masking to be
//			applied to the control to format it for special editing restrictions.
//===========================================================================
class _XT_EXT_CLASS CXTMaskEdit : public CXTMaskEditT<CXTFlatEdit>
{
	DECLARE_DYNAMIC(CXTMaskEdit)
	DECLARE_MESSAGE_MAP()

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTMaskEdit object
	//-----------------------------------------------------------------------
	CXTMaskEdit();

public:

	//-----------------------------------------------------------------------
	// Input:	lpszMask    - The format for the mask field. For example, if 
	//						  you wanted to set the mask for a phone number, 
	//						  and you only wanted digits to be entered, your 
	//						  mask might look like this; _T("(000) 000-0000").
	//			lpszLiteral - The literal format is entered here. Wherever you 
	//						  place an underscore ('_') is where the user will 
	//						  be allowed to enter data only. Using the phone 
	//						  number example; _T("(___) ___-____").
	//			lpszDefault - Text that is to be displayed when the control 
	//						  is initialized. For example; _T("(800) 555-1212"). 
	//						  If NULL, 'lpszLiteral' is used to initialize the 
	//						  edit text.
	// Summary: This member function will set the mask for the edit control.
	//-----------------------------------------------------------------------
	virtual void SetEditMask(LPCTSTR lpszMask,LPCTSTR lpszLiteral,LPCTSTR lpszDefault=NULL);

};

//===========================================================================
// Remarks: CXTDateEdit is a CXTMaskEdit derived class. It is specifically 
//			geared toward editing date fields.
//===========================================================================
class _XT_EXT_CLASS CXTDateEdit : public CXTMaskEdit
{
	DECLARE_DYNAMIC(CXTDateEdit)

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTDateEdit object
	//-----------------------------------------------------------------------
	CXTDateEdit();

	//-----------------------------------------------------------------------
	// Input:	dt - A reference to a COleDateTime object that represents the 
	//				 date to display.
	// Summary:	This member function will set the time based on the text string 
	//			passed in as 'strDate'.
	//-----------------------------------------------------------------------
	virtual void SetDateTime(COleDateTime& dt);

	//-----------------------------------------------------------------------
	// Input:	strDate - A NULL terminated string that represents the date to display.
	// Summary:	This member function will set the time based on the text string 
	//			passed in as 'strDate'.
	//-----------------------------------------------------------------------
	virtual void SetDateTime(CString strDate);

	//-----------------------------------------------------------------------
	// Summary:	This member function will retrieve a COleDateTime object that 
	//			represents the currently displayed date. 
	// Returns: A COleDateTime object representing the currently displayed date.
	//-----------------------------------------------------------------------
	virtual COleDateTime GetDateTime();

	//-----------------------------------------------------------------------
	// Summary:	This member function will retrieve a CString object that represents 
	//			the currently displayed date. 
	// Returns: A CString object representing the currently displayed date.
	//-----------------------------------------------------------------------
	virtual CString GetWindowDateTime();

	//-----------------------------------------------------------------------
	// Input:	lpszData - A NULL terminated string that represents the date to convert.
	// Summary:	This member function will read the date string passed in as 'lpszData'
	//			and is converted and returned as a COleDateTime object. 
	// Returns: A COleDateTime object that represents the converted date string.
	//-----------------------------------------------------------------------
	virtual COleDateTime ReadOleDateTime(LPCTSTR lpszData);

	//-----------------------------------------------------------------------
	// Input:	nChar   - Contains the character code value of the key.
	//			nEndPos - Index of character in display string.
	// Summary:	This member function is used internally to process the character passed
	//			in by 'nChar' whose index is specified by 'nEndPos'. 
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	virtual bool ProcessMask(UINT& nChar,int nEndPos);

	//-----------------------------------------------------------------------
	// Input:	strData - String reference that is filled with the date.
	//			dt		- COleDateTime object that represents the date to format.
	// Summary:	This member function will read the date passed in as 'dt', and format
	//			the 'strData' string as DD/MM/YY.
	//-----------------------------------------------------------------------
	virtual void FormatOleDateTime(CString& strData,COleDateTime dt);
};

//---------------------------------------------------------------------------
// Input:	pDX         - A pointer to a CDataExchange object. The framework supplies this
//						  object to establish the context of the data exchange, including its
//						  direction.    
//			nIDC		- The resource ID of the date edit control associated with the control
//						  property.
//			rControl	- A reference to a member variable of the dialog box, form view, or
//						  control view object with which data is exchanged.
//			rDateTime	- A reference to a member variable of the dialog box, form view, or
//						  control view object with which data is exchanged.
// Summary:	CXTDateEdit - The DDX_XTOleDateTime function manages the transfer of
//			integer data between a date edit control, in a dialog box, form view, or
//			control view object, and a COleDateTime data member of the dialog box,
//			form view, or control view object.
//
//			When DDX_XTOleDateTime is called, 'value' is set to the current state
//			of the date edit control.
//---------------------------------------------------------------------------
_XT_EXT_CLASS void AFXAPI DDX_XTOleDateTime(CDataExchange* pDX,int nIDC,CXTDateEdit& rControl,COleDateTime& rDateTime);

//===========================================================================
// Remarks: CXTTimeEdit is a CXTDateEdit derived class. It is specifically geared
//			toward editing time fields.
//===========================================================================
class _XT_EXT_CLASS CXTTimeEdit : public CXTDateEdit
{
	DECLARE_DYNAMIC(CXTTimeEdit)

public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTTimeEdit object
	//-----------------------------------------------------------------------
	CXTTimeEdit();

protected:

	int m_iHours;		// Represents the hours to display.
	int	m_iMins;		// Represents the minutes to display.
	bool m_bMilitary;	// true if military time is used.

public:

	//-----------------------------------------------------------------------
	// Input:	nHours - The new hour to be displayed.
	// Summary:	This member function will update the hours displayed.
	//-----------------------------------------------------------------------
	virtual void SetHours(int nHours);

	//-----------------------------------------------------------------------
	// Input:	nMins - The new minutes to be displayed.
	// Summary:	This member function will update the minutes displayed.
	//-----------------------------------------------------------------------
	virtual void SetMins(int nMins);

	//-----------------------------------------------------------------------
	// Input:	nHours - The new hour to be displayed.
	//			nMins  - The new minutes to be displayed.
	// Summary:	This member function will update the hours and minutes that are displayed
	//			for the time edit control.
	//-----------------------------------------------------------------------
	virtual void SetTime(int nHours,int nMins);

	//-----------------------------------------------------------------------
	// Summary:	This member function will retrieve the hours displayed for the time edit
	//			control. 
	// Returns: An integer value that represents the hours displayed.
	//-----------------------------------------------------------------------
	int GetHours() const;

	//-----------------------------------------------------------------------
	// Summary:	This member function will retrieve the minutes displayed for the time
	//			edit control. 
	// Returns: An integer value that represents the minutes displayed.
	//-----------------------------------------------------------------------
	int GetMins() const;

	//-----------------------------------------------------------------------
	// Input:	nChar   - Contains the character code value of the key.
	//			nEndPos - Index of the character in the display string.
	// Summary:	This member function is used internally to process the character
	//			passed in by 'nChar' whose index is specified by 'nEndPos'. 
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	virtual bool ProcessMask(UINT& nChar,int nEndPos);

	//-----------------------------------------------------------------------
	// Input:	strData - String reference that is filled with the time. 
	//			dt      - COleDateTime object that represents the time to format.
	// Summary:	This member function will read the time passed in as 'dt',
	//			and format the 'strData' string as HH:MM.
	//-----------------------------------------------------------------------
	virtual void FormatOleDateTime(CString& strData,COleDateTime dt);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTTimeEdit::GetHours() const {
	return m_iHours;
}
AFX_INLINE int CXTTimeEdit::GetMins() const {
	return m_iMins;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTMASKEDIT_H__)
//:End Ignore