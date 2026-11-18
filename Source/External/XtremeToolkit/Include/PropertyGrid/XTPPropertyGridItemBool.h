// XTPPropertyGridItemBool.h interface for the CXTPPropertyGridItemBool class.
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
#if !defined(__XTPPROPERTYGRIDITEMBOOL_H__)
#define __XTPPROPERTYGRIDITEMBOOL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks: CXTPPropertyGridItemBool is a CXTPPropertyGridItem derived class.  
//			It is used to create a Boolean value item in a Property Grid control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemBool : public CXTPPropertyGridItem
{
public:

	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption of the item.
	//			bValue - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemBool object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemBool(CString strCaption, BOOL bValue = FALSE, BOOL* pBindBool = NULL);

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemBool object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemBool(UINT nID, BOOL bValue = FALSE, BOOL* pBindBool = NULL);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridItemBool object.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridItemBool(void);

	//-----------------------------------------------------------------------
	// Input:	bValue - The new BOOL value of the item.
	// Summary: Call this method to change the item's value.
	//-----------------------------------------------------------------------
	void SetBool(BOOL bValue);
	
	//-----------------------------------------------------------------------
	// Summary: Call this method to get the Boolean value of the item.
	//-----------------------------------------------------------------------
	BOOL GetBool();

	//-----------------------------------------------------------------------
	// Input:	pBindBool - Binded value.
	// Summary: Call this method to bind the item to a BOOL object.	
	//-----------------------------------------------------------------------
	void BindToBool(BOOL* pBindBool);

	//-----------------------------------------------------------------------
	// Input:   strTrueText  - NULL terminated string that represents the True label.
	//          strFalseText - NULL terminated string that represents the False label.
	// Summary: Call this method to set the label for true and false values in the
	//          drop list.  By default these values are set to True and False.
	// Returns: True if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	BOOL SetTrueFalseText(CString strTrueText, CString strFalseText);

protected:

	//-----------------------------------------------------------------------
	// Input:	strValue - New value of the item.
	// Summary: Call this method to change an item's value.
	//			Override this method to add new functionality.
	//			You should call the base class version of this function from your
	//			override.
	//-----------------------------------------------------------------------
	void SetValue(CString strValue);
	
	//-----------------------------------------------------------------------
	// Input:   strValueText - NULL terminated string that represents the value label.
	//          strNewText   - NULL terminated string that represents the new value label.
	// Summary: Call this method to set the label for true or false values in the
	//          drop list. This method will search for the value and update it if it
	//          exists. If the value is also the current value for the property grid
	//          item, that value is updated as well.
	// Returns: True if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	BOOL SetValueText(CString& strValueText, CString strNewText);
	
private:
	void _Init(BOOL bValue);

public:

protected:

    BOOL    m_bValue;       // Value of the item.
    BOOL*   m_pBindBool;    // Binded value.
    CString m_strTrueText;  // Text of the TRUE label.
    CString m_strFalseText; // Text of the FALSE label.

private:

	DECLARE_DYNAMIC(CXTPPropertyGridItemBool)

//:Ignore
#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
	
	void BindDispatch();
#endif
//:End Ignore
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPPropertyGridItemBool::GetBool() {
	return m_bValue;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTPPROPERTYGRIDITEMBOOL_H__)
//:End Ignore