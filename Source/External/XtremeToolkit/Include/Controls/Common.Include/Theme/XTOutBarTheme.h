// XTOutBarTheme.h: interface for the ÑXTOutBarTheme class.
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

#if !defined(__XTOUTBARTHEME_H__)
#define __XTOUTBARTHEME_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonPro.Include\XTPDrawHelpers.h"

class CXTOutBarCtrl;
class CXTBarItem;
class CXTBarFolder;

//===========================================================================
// Summary: CXTButtonTheme is used to draw the CXTButton object.  All themes 
//          used for CXTButton should inherit from this base class.
//===========================================================================
class _XT_EXT_CLASS CXTOutBarTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTOutBarTheme object.
	//-----------------------------------------------------------------------
	CXTOutBarTheme();

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTOutBarTheme object, handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTOutBarTheme();

public:
	virtual void RefreshMetrics();
	void SetFont(CFont* pFont);

public:
	virtual void DrawItemIcon(CDC* pDC, CPoint pt, CXTBarItem* pBarItem, CImageList* pImageList);
	virtual void DrawItemText(CDC* pDC, CRect rc, CXTBarItem* pBarItem, UINT nFormat);
	virtual void FillInsideRect(CDC* pDC, CRect rc);

	virtual void DrawFolder(CDC* pDC, CRect rect, CXTBarFolder* pBarFolder, const int eHilight);
	virtual void DrawIconFrame(CDC* pDC, CRect rcIcon, BOOL bSelected, BOOL bPressed);
	


protected:
	BOOL m_bFlatIcon;
	BOOL m_bPlainText;
	
public:
	CXTPPaintManagerColor m_clrBack;
	CXTPPaintManagerColor m_clrText;
};

class _XT_EXT_CLASS CXTOutBarThemeOfficeXP : public CXTOutBarTheme
{
public:
	CXTOutBarThemeOfficeXP();
	
	void RefreshMetrics();
	
	virtual void DrawIconFrame(CDC* pDC, CRect rcIcon, BOOL bSelected, BOOL bPressed);

};

class _XT_EXT_CLASS CXTOutBarThemeOffice2003 : public CXTOutBarThemeOfficeXP
{
public:
	CXTOutBarThemeOffice2003();
	
	virtual void RefreshMetrics();	
	virtual void DrawFolder(CDC* pDC, CRect rect, CXTBarFolder* pBarFolder, const int eHilight);
	virtual void DrawIconFrame(CDC* pDC, CRect rcIcon, BOOL bSelected, BOOL bPressed);

protected:
	XTP_GRADIENT_COLOR m_grcSelected;
	XTP_GRADIENT_COLOR m_grcHot;
	XTP_GRADIENT_COLOR m_grcPushed;
	BOOL m_bLunaTheme;
}; 


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTOUTBARTHEME_H__)
