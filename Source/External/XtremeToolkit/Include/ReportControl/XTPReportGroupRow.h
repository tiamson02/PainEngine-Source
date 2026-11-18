// XTPReportGroupItem.h: interface for the CXTPReportGroupItem class.
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
#if !defined(__XTPREPORTGROUPROW_H__)
#define __XTPREPORTGROUPROW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPReportRow.h"

//===========================================================================
// Summary: This class represents a 'group row' of the report control view - 
//			that means a row which has no associated Record item, but
//			has a special text caption instead.
//
//			It has a slightly different reaction on mouse events and another
//			draw logic.
//			
//			All other behavior of the group row is similar to its parent.
//
// See also: CXTPReportRow overview, CXTPReportControl overview
//===========================================================================
class _XTP_EXT_CLASS CXTPReportGroupRow : public CXTPReportRow
{
public:
	//-----------------------------------------------------------------------
	// Summary: A constructor of the CXTPReportGroupRow object.
	//-----------------------------------------------------------------------
	CXTPReportGroupRow();

	//-----------------------------------------------------------------------
	// Summary: Returns the group text caption.
	// Remarks: Call this member function to retrieve a text caption of this
	//			group row item.
	// Returns: Group text caption.
	// See also: CXTPReportGroupRow::CXTPReportGroupRow
	//-----------------------------------------------------------------------
	virtual CString GetCaption();

	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Input:	pDC		- A pointer to the device context in which the drawing occurs. 
	//			rcRow	- Position of the row in client window coordinates.
	// Summary: Draws the row using the specified device context.
	// See also: CXTPReportRow::Draw
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:	ptClicked - Point coordinates where the message was fired from.
	// Summary: Process mouse single clicks.
	// See also: CXTPReportRow::OnClick
	//-----------------------------------------------------------------------
	void OnClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Input:	ptClicked - Point coordinates where the message was fired from.
	// Summary: Process mouse double clicks.
	// Remarks: Group row adds a collapse/expand functionality if double-
	//			clicked on a row.
	// See also: CXTPReportRow::OnDblClick
	//-----------------------------------------------------------------------
	void OnDblClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Summary: Determines if this is a group row or not.
	// Returns: TRUE because this is a group row.
	// See also: CXTPReportRow::IsGroupRow
	//-----------------------------------------------------------------------
	BOOL IsGroupRow();


protected:
	CString m_strGroupText;		// group text label

	friend class CXTPReportPaintManager;
};

AFX_INLINE BOOL CXTPReportGroupRow::IsGroupRow() {
	return TRUE;
}

#endif