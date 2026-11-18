// DValidateValue.h: interface for the DValidateValue class.
//
// Damir Valiulin, 2002-05-07
//
// Validates a number in a given value range.
//
// Usage examples: 
//  Validate (val, "Unit Weight (0.0,inf)", true); // shows message box to user in case of error
//  Validate (val, "Friction Angle [0,90)", strErrMsg); // fills strErrMsg with error if value is out of range
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVALIDATEVALUE_H__4002AEC1_9A2A_49DD_81B6_85F9172D2DCC__INCLUDED_)
#define AFX_DVALIDATEVALUE_H__4002AEC1_9A2A_49DD_81B6_85F9172D2DCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// DDV_RangeDouble is combined DDX_Text (..., double) and DDV_MinMaxDouble.
//   The validation part is a little better however.
//     1. Validation will not be performed if the control is invisible or disabled
//     2. Instead of numeric values for min and max it accepts 
//        algebraic expression for range (e.g. [-90,90])
//     3. It also provides the facility to output variable name in the error message.
//
// szNameAndRange is in the following format "Name <min,max>", where
//   'Name' is the name of the variable. Leave blank for default "Value" name.
//   '<' and '>' are the range specifiers, use curly () or square [] brackets to specify
//               range that either up to or includes min and max.
//   Write 'inf or '-inf' to specify positive or negative infinity. ('-~' or '~' also works)
//
// Examples: "Cohesion [0,inf)", "Friction Angle [0,90)", "(-inf,0)"
//
void AFXAPI DDV_RangeDouble(CDataExchange* pDX, int nIDC, double& value, LPCTSTR szNameAndRange = NULL);
void AFXAPI DDV_RangeFloat(CDataExchange* pDX, int nIDC, float& value, LPCTSTR szNameAndRange = NULL);




class DValidateValue  
{
public:
	DValidateValue();
	virtual ~DValidateValue();

	static bool Validate (double val, LPCTSTR szRange, bool bMessageBox = true);
	static bool Validate (double val, LPCTSTR szRange, CString &strErrMsg);
	static bool IsNumber (LPCTSTR lpszText, double& d);
	static bool IsNumber (LPCTSTR lpszText, float& d);
};

#endif // !defined(AFX_DVALIDATEVALUE_H__4002AEC1_9A2A_49DD_81B6_85F9172D2DCC__INCLUDED_)
