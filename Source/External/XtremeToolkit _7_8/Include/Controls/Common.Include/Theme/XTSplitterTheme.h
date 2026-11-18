// XTSplitterTheme.h: interface for the CXTSplitterTheme class.
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

#if !defined(__XTSPLITTERTHEME_H__)
#define __XTSPLITTERTHEME_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTSplitter;

enum XTSplitterGlyph
{
	xtSplitterGlyphNone,
	xtSplitterGlyphXP,
	xtSplitterGlyphDoubleDots
};

//===========================================================================
// Summary: CXTButtonTheme - 
//===========================================================================
class _XT_EXT_CLASS CXTSplitterTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: 
	//-----------------------------------------------------------------------
	CXTSplitterTheme();

	void SetOffice2003Theme(BOOL bTheme);

	virtual void RefreshMetrics();

	void SetGlyph(XTSplitterGlyph gliph);

public:
	COLORREF            m_clrSplitterFace;
	COLORREF            m_clrSplitterBorders;

protected:
	BOOL                m_bOffice2003Theme;     
	XTSplitterGlyph		m_gliph;
};

//===========================================================================

AFX_INLINE void CXTSplitterTheme::SetOffice2003Theme(BOOL bTheme) {
	m_bOffice2003Theme = bTheme;
	RefreshMetrics();
}
AFX_INLINE void CXTSplitterTheme::SetGlyph(XTSplitterGlyph gliph) {
	m_gliph = gliph;
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTSPLITTERTHEME_H__)
