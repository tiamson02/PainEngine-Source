// CXTPReportHyperlink.h: interface for the XTPReportHyperlink class.
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
#if !defined(__XTPREPORTRECORDHYPERLINK_H__)
#define __XTPREPORTRECORDHYPERLINK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks:	This class represents hyperlinks. It stores first position and length
//			in the text for hyperlink and rectangle for drawing and 
//			identify clicks on hyperlink. To create an instance of its class, you 
//			simply call constructor with first position and length of the 
//			hyperlink's text
//===========================================================================
class _XTP_EXT_CLASS CXTPReportHyperlink : public CCmdTarget 
{
public:
	//-----------------------------------------------------------------------
	// Input:	nHyperTextBegin - Begin position of the hyperlink item text in the parent item caption.
	//			nHyperTextLen   - Hyperlink item text length.
	// Summary:	Default CXTPReportHyperlink constructor.
	//-----------------------------------------------------------------------
	CXTPReportHyperlink(int nHyperTextBegin, int nHyperTextLen);
	
	//-----------------------------------------------------------------------
	// Summary:	Default CXTPReportHyperlink destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHyperlink();

public:
	CRect		m_rcHyperSpot;		// Hyperlink draw place.
	int			m_nHyperTextBegin;	// Start position of the hyperlink in the item text.
	int			m_nHyperTextLen;	// The lenght of hyperlink text.
};

#endif // !defined(__XTPREPORTRECORDHYPERLINK_H__)
