// XTPPropertyGridItemNumber.h interface for the CXTPPropertyGridItemNumber class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO 
// BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED 
// WRITTEN CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS 
// OUTLINED IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE 
// GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE 
// THIS SOFTWARE ON A SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTPPROPERTYGRIDITEMNUMBER_H__)
#define __XTPPROPERTYGRIDITEMNUMBER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks: CXTPPropertyGridItemNumber is a CXTPPropertyGridItem derived class.  
//			It is used to create an integer value item in a Property Grid control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemNumber : public CXTPPropertyGridItem
{
public:

	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption of the item.
	//			nValue - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemNumber object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemNumber(CString strCaption, long nValue = 0, long* pBindNumber = NULL);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemNumber object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemNumber(UINT nID, long nValue = 0, long* pBindNumber = NULL);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridItemNumber object
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridItemNumber(void);

public:

	//-----------------------------------------------------------------------
	// Input:	nValue - The new integer value of the item.
	// Summary: Call this method to change the item's value.
	//-----------------------------------------------------------------------
	void SetNumber(long nValue);

	//-----------------------------------------------------------------------
	// Summary: Call this method to get the integer value of the item.
	// Returns: The integer value of the item.
	//-----------------------------------------------------------------------
	long GetNumber();

	//-----------------------------------------------------------------------
	// Input:	pBindNumber - Points to a long value.
	// Summary: Call this method to bind an item to a long value.
	//-----------------------------------------------------------------------
	void BindToNumber(long* pBindNumber);


protected:
	
	//-----------------------------------------------------------------------
	// Input:	strValue - New value of the item.
	// Summary: Call this method to change an item's value.
	//			Override this method to add new functionality.
	//			You should call the base class version of this function from your
	//			override.
	//-----------------------------------------------------------------------
	virtual void SetValue(CString strValue);

protected:

	long m_nValue;			// Value of the item.
	long* m_pBindNumber;	// Binded object.

private:	

#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
	
	void BindDispatch();
#endif
	DECLARE_DYNAMIC(CXTPPropertyGridItemNumber)
};


//===========================================================================
// Remarks: CXTPPropertyGridItemDouble is a CXTPPropertyGridItem derived class.  
//			It is used to create an double value item in a Property Grid control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemDouble : public CXTPPropertyGridItem
{
public:

	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption of the item.
	//			fValue - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemDouble object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemDouble(CString strCaption, double fValue = 0, LPCTSTR strFormat = _T("%0.2f"), double* pBindNumber = NULL);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemDouble object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemDouble(UINT nID, double fValue = 0, LPCTSTR strFormat = _T("%0.2f"), double* pBindNumber = NULL);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridItemDouble object.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridItemDouble(void);

public:

	//-----------------------------------------------------------------------
	// Input:	fValue - The new double value of the item.
	// Summary: Call this method to change the item's value.
	//-----------------------------------------------------------------------
	void SetDouble(double fValue);

	//-----------------------------------------------------------------------
	// Summary: Call this method to get the double value of the item.
	// Returns: The double value of the item.
	//-----------------------------------------------------------------------
	double GetDouble();

	//-----------------------------------------------------------------------
	// Input:	pBindDouble - Points to a double value.
	// Summary: Call this method to bind the item to a double value.
	//-----------------------------------------------------------------------
	void BindToDouble(double* pBindDouble);


protected:
	
	//-----------------------------------------------------------------------
	// Input:	strValue - New value of the item.
	// Summary: Call this method to change an item's value.
	//			Override this method to add new functionality.
	//			You should call the base class version of this function from your
	//			override.
	//-----------------------------------------------------------------------
	virtual void SetValue(CString strValue);
	

protected:
	
	double m_fValue;		// Double value.
	double* m_pBindDouble;	// Binded value.

private:
	DECLARE_DYNAMIC(CXTPPropertyGridItemDouble)

#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
	
	void BindDispatch();
#endif
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE long CXTPPropertyGridItemNumber::GetNumber() {
	return m_nValue;
}

//////////////////////////////////////////////////////////////////////

AFX_INLINE double CXTPPropertyGridItemDouble::GetDouble() {
	return m_fValue;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTPPROPERTYGRIDITEMNUMBER_H__)
//:End Ignore