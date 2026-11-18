// DValidateValue.cpp: implementation of the DValidateValue class.
//
// Damir Valiulin, 2002-05-07
//
// See .h header file.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DValidateValue.h"
#include "afxpriv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define _countof(array) (sizeof(array)/sizeof(array[0]))

void AFXAPI DDV_RangeDouble(CDataExchange* pDX, int nIDC, double& value, LPCTSTR szNameAndRange /*= NULL*/)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];

	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d=0.0;
		if (!DValidateValue::IsNumber(szBuffer, d))
		{
			if (::IsWindowVisible(hWndCtrl) && ::IsWindowEnabled(hWndCtrl)){
				AfxMessageBox(AFX_IDP_PARSE_REAL);
				pDX->Fail();            // throws exception
			}
		}
		value = d;
	}
	else
	{
		_stprintf(szBuffer, _T("%.*g"), DBL_DIG, value);
		AfxSetWindowText(hWndCtrl, szBuffer);
	}

	if (szNameAndRange != NULL)
	{
		CString prompt;
		if (!DValidateValue::Validate(value, szNameAndRange, prompt)){
			if (!pDX->m_bSaveAndValidate){
				TRACE("Warning: initial dialog data is out of range. Value: %g,  Name and Range \"%s\"\n", value, szNameAndRange);
				return;         // don't stop now
			}
			else if (::IsWindowVisible(hWndCtrl) && ::IsWindowEnabled(hWndCtrl)){
				AfxMessageBox(prompt, MB_ICONEXCLAMATION);
				prompt.Empty(); // exception prep
				pDX->Fail();
			}
		}
	}
}

void AFXAPI DDV_RangeFloat(CDataExchange* pDX, int nIDC, float& value, LPCTSTR szNameAndRange /*= NULL*/)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];

	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, _countof(szBuffer));
		double d=0.0;
		if (!DValidateValue::IsNumber(szBuffer, d))
		{
//			if (::IsWindowVisible(hWndCtrl) && ::IsWindowEnabled(hWndCtrl)){
//				AfxMessageBox(AFX_IDP_PARSE_REAL);
//				pDX->Fail();            // throws exception
//			}
		}
		value = d;
	}
	else
	{
		sprintf(szBuffer, "%.6g", value);
		AfxSetWindowText(hWndCtrl, szBuffer);
	}

	if (szNameAndRange != NULL)
	{
		CString prompt;
		if (!DValidateValue::Validate(value, szNameAndRange, prompt)){
			if (!pDX->m_bSaveAndValidate){
				TRACE("Warning: initial dialog data is out of range. Value: %g,  Name and Range \"%s\"\n", value, szNameAndRange);
				return;         // don't stop now
			}
			else if (::IsWindowVisible(hWndCtrl) && ::IsWindowEnabled(hWndCtrl)){
				AfxMessageBox(prompt, MB_ICONEXCLAMATION);
				prompt.Empty(); // exception prep
				pDX->Fail();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DValidateValue::DValidateValue()
{

}

DValidateValue::~DValidateValue()
{

}

bool DValidateValue::Validate (double val, LPCTSTR szRange, bool bMessageBox /*= true*/)
{
	CString strErrMsg;
	if (!Validate(val, szRange, strErrMsg)){
		if (bMessageBox){
			AfxMessageBox (strErrMsg);
		}
		return false;
	}
	else{
		return true;
	}
}

bool DValidateValue::Validate (double val, LPCTSTR szRange, CString &strErrMsg)
{
	strErrMsg.Empty ();
	CString strRange (szRange);

	/// Find positions to braces and separator
	int pos_lb = -1; bool bLsqbr = false;
	{
		int curlyb = strRange.ReverseFind ('(');
		int squareb = strRange.ReverseFind ('[');
		if (curlyb != -1 && squareb != -1){
			pos_lb = max (curlyb, squareb);
			bLsqbr = squareb > curlyb;
		}
		else if (curlyb != -1){
			pos_lb = curlyb;
			bLsqbr = false;
		}
		else if (squareb != -1){
			pos_lb = squareb;
			bLsqbr = true;
		}
		else{
			ASSERT (FALSE); return true;
		}
	}

	int pos_sp = strRange.ReverseFind (',');
	if (pos_sp == -1 || pos_sp < pos_lb){
		ASSERT(FALSE); return true;
	}

	int pos_rb = -1; bool bRsqbr = false;
	{
		int curlyb = strRange.ReverseFind (')');
		int squareb = strRange.ReverseFind (']');
		if (curlyb != -1 && squareb != -1){
			pos_rb = max (curlyb, squareb);
			bRsqbr = squareb > curlyb;
		}
		else if (curlyb != -1){
			pos_rb = curlyb;
			bRsqbr = false;
		}
		else if (squareb != -1){
			pos_rb = squareb;
			bRsqbr = true;
		}
		else{
			ASSERT (FALSE); return true;
		}
	}
	if (pos_rb < pos_sp || pos_rb < pos_lb){
		ASSERT (FALSE); return true;
	}

	/// Extract first and second numbers,
	/// check what type of brackets is used,
	/// check if range numbers are infinities
	CString strLNum = strRange.Mid (pos_lb+1, pos_sp-pos_lb-1);
	CString strRNum = strRange.Mid (pos_sp+1, pos_rb-pos_sp-1);
	strLNum.TrimLeft (); strLNum.TrimRight ();
	strRNum.TrimLeft (); strRNum.TrimRight ();
	bool bLNegInf = (strLNum == "-~" || 0==strLNum.CompareNoCase ("-inf"));
	bool bRPosInf = (strRNum == "~" || 0==strRNum.CompareNoCase ("inf"));

	/// Extract value name
	CString strValName = strRange.Left (pos_lb); 
	if (strValName.GetLength () == 0){
		strValName = "Value ";
	}

	/// Convert numbers if neccessary
	double lval = -DBL_MAX;
	double rval = DBL_MAX;
	if (!bLNegInf){ lval = strtod (strLNum, NULL); }
	if (!bRPosInf){ rval = strtod (strRNum, NULL); }

	/// Do the range validation
	if (bLNegInf && bRPosInf){
		return true;
	}
	else if (bLNegInf){
		if (val < rval) return true;
		else if (bRsqbr && (val == rval)) return true;

		strErrMsg = strValName + "is out of range. The value should be less than ";
		if (bRsqbr) strErrMsg += "or equal to ";
		strErrMsg += strRNum;
		return false;
	}
	else if (bRPosInf){
		if (val > lval) return true;
		else if (bLsqbr && (val == lval)) return true;

		strErrMsg = strValName + "is out of range. The value should be greater than ";
		if (bLsqbr) strErrMsg += "or equal to ";
		strErrMsg += strLNum;
		return false;
	}
	else{ //  !bLNegInf && !bRPosInf
		if (val > lval && val < rval) return true;
		else{
			if ((bLsqbr && (val == lval)) || (bRsqbr && (val == rval))) return true;
		}

		strErrMsg = strValName + "is out of range. The valid value range is ";
		strErrMsg += strRange.Mid (pos_lb, pos_rb-pos_lb+1) + ".";
		return false;
	}
}

bool DValidateValue::IsNumber (LPCTSTR lpszText, double& d)
{
	/// From DLGFLOAT.CPP, function _AfxSimpleFloatParse
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	
	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return false;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	
	if (*lpszText != '\0')
		return false;   // not terminated properly
	
	return true;
}
