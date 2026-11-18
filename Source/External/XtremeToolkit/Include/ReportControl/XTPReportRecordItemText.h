// XTPReportRecordItemText.h: interface for the CXTPReportRecordItemText class.
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
#if !defined(__XTPREPORTRECORDITEMTEXT_H__)
#define __XTPREPORTRECORDITEMTEXT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPReportRecordItem.h"

//===========================================================================
// Remarks: This Class represents a text-only cell and inherits basic functionality 
//			from CXTPReportRecordItem class.
//			You create a text record item simply by calling a constructor with one 
//          parameter - text string 
// See also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemText : public CXTPReportRecordItem
{
	DECLARE_DYNAMIC(CXTPReportRecordItemText)
public:

	//-----------------------------------------------------------------------	
	// Input:	szText - A text value for the item.
	// Summary: Creates a record text item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemText(LPCTSTR szText);	
	
	//-----------------------------------------------------------------------
	// Summary: Returns a text caption of this item.
	// Remarks: Overwrites parents' member function
	// Returns: Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	void SetValue(LPCTSTR szText);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:	Returns the item value in its original format.
	// Returns: Initial item text string.
	//-----------------------------------------------------------------------
	CString GetValue();


protected:
	CString m_strText;	// Item text value.
};

AFX_INLINE CString CXTPReportRecordItemText::GetValue() {
	return m_strText;
}

//===========================================================================
// Remarks: This class represents a numeric cell and inherits basic functionality 
//			from CXTPReportRecordItem class. 
//
//			You create a number record item simply by calling constructor with 
//			the only one parameter - its numeric value. There is an additional 
//			constructor with second parameter - format string. 
//			Format string allows you to determine exactly in which format  
//			the numeric value will be represented on the screen. 
//			It could be useful for displaying money values, etc.
// See also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemNumber : public CXTPReportRecordItem
{
	DECLARE_DYNAMIC(CXTPReportRecordItemNumber)
public:
	
	//-----------------------------------------------------------------------
	// Input:	dValue - Value of this numeric item.
	// Summary: Creates record numeric item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemNumber(double dValue);
	
	//-----------------------------------------------------------------------
	// Input:	dValue    - Numeric value of this item.
	//			strFormat - Format string for creating caption value of the item.
	// Summary: Creates a record numeric item with the specific formatting.
	// Remarks: If the format string is set, the value of the item will be formatted
	//			according to this format string before drawing it convenient 
	//          in many cases (drawing date for example). Format string is C - like
	//			style, see sprintf() C function or CString.Format() member function
	//-----------------------------------------------------------------------
	CXTPReportRecordItemNumber(double dValue, CString strFormat);
	
	//-----------------------------------------------------------------------
	// Summary: Returns a text caption of this item.
	// Remarks: Overwrites parents' member function
	// Returns: Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:	Returns item value in its original format.
	// Returns: Item original numeric value.
	//-----------------------------------------------------------------------
	double GetValue();

	void SetValue(double dValue);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Input:	pItem - An item to compare with the current.
	// Summary: Compares this item with the provided one.
	// Remarks: Overwrites parents' member function
	// Returns: Zero if pItem is equal with current; 
	//          Less than zero if less than pItem; 
	//          Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem);

protected:
	double m_dValue;	// Cell value.
};

// Summary: Represents time cell.

//===========================================================================
// Remarks: Represents a time cell and inherits basic functionality 
//			from CXTPReportRecordItem class. 
//			You create a bitmap record item simply by calling a constructor with one 
//          parameter - datetime 
// See also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemDateTime : public CXTPReportRecordItem
{
	DECLARE_DYNAMIC(CXTPReportRecordItemDateTime)
public:
	
	//-----------------------------------------------------------------------
	// Input:	odtValue - Initial date/time item value.
	// Summary: Creates record date/time item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemDateTime(COleDateTime odtValue);
	
	//-----------------------------------------------------------------------
	// Summary: Returns a text caption of this item.
	// Remarks: Overwrites parents' member function.
	// Returns: Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pItem - The item provided for comparing with current.
	// Summary: Compares this item with the provided one.
	// Remarks: Overwrites parents' member function
	// Returns: Zero if pItem is equal with current; 
	//			Less than zero if less than pItem; 
	//			Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	
	//-----------------------------------------------------------------------
	// Summary:	Returns the item value in its original format.
	// Returns: Item datetime value.
	//-----------------------------------------------------------------------
	COleDateTime GetValue();

protected:
	COleDateTime m_odtValue;	// Cell value.
};

#ifdef _XTP_ACTIVEX

//===========================================================================
// Summary: Represents a variant item and inherits basic functionality 
//			from CXTPReportRecordItem class. 
//			You can create a bitmap record item simply by calling a constructor with 
//			the only one parameter - value of VARIANT type.
// See also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemVariant : public CXTPReportRecordItem
{
	DECLARE_DYNAMIC(CXTPReportRecordItemVariant)
public:
	
	//-----------------------------------------------------------------------
	// Input:	lpValue - Pointer to the initial item value.
	// Summary: Creates a new variant record item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemVariant(const VARIANT& lpValue);
	
	//-----------------------------------------------------------------------
	// Summary: Returns a text caption of this item.
	// Remarks: Overwrites parents' member function
	// Returns: Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pItem - The item provided for comparing with current.
	// Summary: Compares this item with the provided one.
	// Remarks: Overwrites parents' member function
	// Returns: Zero if pItem is equal with current; 
	//			Less than zero if less than pItem; 
	//			Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem);

	void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);

	DWORD GetSelectedConstraintData(XTP_REPORTRECORDITEM_ARGS* pItemArgs);

protected:
	COleVariant m_oleValue;	// Item value.

	DECLARE_DISPATCH_MAP()

};

#endif


//===========================================================================
// Remarks: This represents a preview cell and inherits basic functionality 
//			from CXTPReportRecordItem class. 
//			You create a bitmap record item simply by calling constructor with one 
//          parameter - text string.
// See also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemPreview : public CXTPReportRecordItem
{
public:
	//-----------------------------------------------------------------------
	// Input:	szPreviewText - Pointer to the initial item value.
	// Summary: Creates a new variant record item.
	// Remarks: It is the constructor of the class
	//-----------------------------------------------------------------------
	CXTPReportRecordItemPreview(LPCTSTR szPreviewText);	
    
	//-----------------------------------------------------------------------
	// Input:	strPreviewText - new preview text
    // Summary: Set preview text
	//-----------------------------------------------------------------------
	void SetPreviewText(CString strPreviewText);

	//-----------------------------------------------------------------------
	// Summary: Returns preview text
	// Returns:	Text of preview item
	//-----------------------------------------------------------------------
	CString GetPreviewText();

	int GetPreviewHeight(CDC* pDC, CXTPReportRow* pRow, int nWidth);

	//-----------------------------------------------------------------------
	// Summary: returns preview text
	// Returns:	text of preview item
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:    pDrawArgs - structure which contains drawind arguments:
	//							CDC* pDC       - Pointer to control drawing context.
	//							int nTextAlign - Text alignment mode, can take next values:
	//											  DT_LEFT   - align text left;
	//                                            DT_RIGHT  - align text right;
	//											  DT_CENTER - align text center.
	//							CXTPReportRecordItem* pItem - Record item to draw.

	// Summary: Should be overriden by descendants for drawing itself.
	void OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	//-----------------------------------------------------------------------
	// Input:	pDrawArgs    - Draw arguments for calculating item metrics.
	//			pItemMetrics - Pointer to the metrics item to fill with values.
	// Summary: Fills XTP_REPORTRECORDITEM_METRICS structure
	// Remarks: Calculates preview item metrics based on provided draw arguments.
	//			Overwrites parents' member function
	// See Also: XTP_REPORTRECORDITEM_DRAWARGS, XTP_REPORTRECORDITEM_METRICS	
	//-----------------------------------------------------------------------
	void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	//-----------------------------------------------------------------------
	// Summary: Determines of this item is a preview item.
	// Returns: TRUE for the preview item, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsPreviewItem();

protected:
	CString m_strPreviewText;	// Preview content.
};


AFX_INLINE void CXTPReportRecordItemText::SetValue(LPCTSTR szText){
	m_strText = szText;
}
AFX_INLINE void CXTPReportRecordItemNumber::SetValue(double dValue) {
	m_dValue = dValue;
}
AFX_INLINE double CXTPReportRecordItemNumber::GetValue() {
	return m_dValue;
}

AFX_INLINE COleDateTime CXTPReportRecordItemDateTime::GetValue() {
	return m_odtValue;
}

AFX_INLINE void CXTPReportRecordItemPreview::SetPreviewText(CString strPreviewText) {
	m_strPreviewText = strPreviewText;
}
AFX_INLINE CString CXTPReportRecordItemPreview::GetPreviewText() {
	return m_strPreviewText;
}
AFX_INLINE BOOL CXTPReportRecordItemPreview::IsPreviewItem() {
	return TRUE;
}

#endif