// XTPReportFilterEditControl.h: interface for the CXTPReportFilterEditControl class.
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
#if !defined(__XTPREPORTFILTEREDITCONTROL_H__)
#define __XTPREPORTFILTEREDITCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPReportDefines.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPReportFilterEditControl window

//===========================================================================
// Summary: The CXTPReportFilterEditControl class provides the functionality of the
//			filter string edit control, associated to the main Report control window.
//
//			It has all the functionality of the CEdit control, as well as some
//			specific additions. 
//
//			This control should be instantiated in the user's application.
//			To use a field chooser in an existing dialog box of your application, 
//			add a standard list box to your dialog template using the dialog
//			editor and then initialize your CXTPReportSubListControl object 
//			inside dialog OnInitialUpdate handler. See example below:
//
//			// CXTPReportFilterEditControl wndFilter;
//			wndFilter.SubclassDlgItem(IDC_FILTEREDIT, &pFilterDialog);
//			// CXTPReportControl wndReport;
//			wndReport.GetColumns()->GetReportHeader()->SetFilterEditCtrl(&wndFilter);
//
// See also: CEdit, CXTPReportHeader::SetFilterEditCtrl
//===========================================================================
class _XTP_EXT_CLASS CXTPReportFilterEditControl : public CEdit
{
// Construction
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPReportFilterEditControl object.
	// Remarks: You construct a CXTPReportFilterEditControl object in two steps. 
	//			First, call the constructor CXTPReportFilterEditControl and 
	//			then call Create, which initializes the Windows edit box and 
	//			attaches it to the CXTPReportFilterEditControl.
	//
	// Example:	// Declare a local CXTPReportFilterEditControl object.
	//			CXTPReportFilterEditControl myFilterEdit;
	//
	//			// Declare a dynamic CXTPReportFilterEditControl object.
	//			CXTPReportFilterEditControl* pMyFilterEdit = new CXTPReportFilterEditControl;
	//
	// See also: CEdit, SetReportCtrl, CXTPReportHeader::SetFilterEditCtrl
	//-----------------------------------------------------------------------
	CXTPReportFilterEditControl();

	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportFilterEditControl object, handles its cleanup.
	//-----------------------------------------------------------------------
	~CXTPReportFilterEditControl();

// Attributes
public:
	//-----------------------------------------------------------------------
	// Input:	pReportCtrl - Pointer to the associated report control.
	// Summary: Sets the associated report control.
	// Remarks: Usually this method should not be called by the user, instead
	//			you should call CXTPReportHeader::SetFilterEditCtrl.
	// Returns: TRUE if set successfully, FALSE otherwise.
	// See also: CXTPReportHeader::SetFilterEditCtrl, GetReportCtrl
	//-----------------------------------------------------------------------
	BOOL SetReportCtrl(CXTPReportControl* pReportCtrl);

	//-----------------------------------------------------------------------
	// Summary: Returns a pointer to the associated report control.
	// Returns: An associated report control.
	// See also: CXTPReportHeader::SetFilterEditCtrl, SetReportCtrl
	//-----------------------------------------------------------------------
	CXTPReportControl* GetReportCtrl();

// Operations
public:
	
	//-----------------------------------------------------------------------
	// Summary: Returns the current filter text.
	// Remarks: Call this member function to retrieve a CString containing
	//			the current editing text of the edit box.
	// Returns: A string currently contained in the filter edit window.
	//-----------------------------------------------------------------------
	CString GetFilterText(); 

// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPReportFilterEditControl)
	protected:
	afx_msg BOOL OnChange();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CXTPReportControl* m_pReportCtrl;	// Pointer to the main report control.
	CString m_strFilterText;			// Filter text.

	// Generated message map functions
protected:
	//{{AFX_MSG(CXTPReportFilterEditControl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

AFX_INLINE CString CXTPReportFilterEditControl::GetFilterText() {
	return m_strFilterText;
}

#endif