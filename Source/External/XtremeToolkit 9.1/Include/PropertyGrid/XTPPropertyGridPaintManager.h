// XTPPropertyGridPaintManager.h interface for the CXTPPropertyGridPaintManager class.
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
#if !defined(_XTPPROPERTYGRIDPAINTMANAGER_H__)
#define _XTPPROPERTYGRIDPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "CommonPro.include/XTPWinThemeWrapper.h"

class CXTPPropertyGrid;
class CXTPPropertyGridInplaceButton;
class CXTPPropertyGridItem;

// Summary: Visual theme enumeration
enum XTPPropertyGridPaintTheme
{
	xtpGridThemeDefault,     // Default theme.
	xtpGridThemeNativeWinXP, // WinXP like theme. 
	xtpGridThemeOffice2003,  // Office 2003 like theme.
	xtpGridThemeCool,        // Cool theme.
	xtpGridThemeSimple,		 // Visual Basic like theme.
	xtpGridThemeDelphi		 // Delphi like theme.
};


//===========================================================================
// Remarks: Standalone class used by CXTPPropertyGrid as the paint manager.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridPaintManager  
{
public:

	//-----------------------------------------------------------------------
	// Input:	pGrid - Points to a CXTPPropertyGrid object
	// Summary: Constructs a CXTPPropertyGridPaintManager object
	//-----------------------------------------------------------------------
	CXTPPropertyGridPaintManager(CXTPPropertyGrid* pGrid);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridPaintManager object, handles cleanup
	//			and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridPaintManager();

public:

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	// Summary: This methos is called to draw property grid background.
	//-----------------------------------------------------------------------
	virtual void FillPropertyGrid(CDC* pDC);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			bClient - TRUE to draw client area; FALSE to draw frame.
	// Summary: This methos is called to draw view background.
	//-----------------------------------------------------------------------
	virtual void FillPropertyGridView(CDC* pDC, BOOL bClient);
	
	//-----------------------------------------------------------------------
	// Input:	lpDrawItemStruct - A long pointer to a DRAWITEMSTRUCT structure 
	//			that contains information about the type of drawing required. 
	// Summary: This methos is called to draw single item of the grod.
	//-----------------------------------------------------------------------
	virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);
	
	//-----------------------------------------------------------------------
	// Input:	lpMeasureItemStruct - Specifies a long pointer to a MEASUREITEMSTRUCT 
	//			structure. 
	// Summary: Override this method and fill in the MEASUREITEMSTRUCT structure 
	//			to inform Windows of the list-box dimensions.
	//-----------------------------------------------------------------------
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPPropertyGridInplaceButton object
	// Summary: This method is called to draw inplace button of the grid.
	//-----------------------------------------------------------------------
	virtual void FillInplaceButton(CDC* pDC, CXTPPropertyGridInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

protected:

	//-----------------------------------------------------------------------
	// Input:	dc - Reference to a valid device context
	//			pItem - Points to a CXTPPropertyGridItem object
	//			rcCaption - Caption rectangle.
	// Summary: This method is called to draw expand buttons of the grid.
	//-----------------------------------------------------------------------
	void DrawExpandButton(CDC& dc, CXTPPropertyGridItem* pItem, CRect rcCaption);

protected:
	CXTPPropertyGrid* m_pGrid;						// Parent grid class.
	BOOL m_bThemed;									// TRUE to use WinXP themes.

	CXTPWinThemeWrapper m_themeTree;		        // Themes for Tree controls.
	CXTPWinThemeWrapper	m_themeButton;				// Themes for button controls

	COLORREF m_clrFace;								// Face color.
	COLORREF m_clrShadow;							// Shadow color.
};

//:Ignore
namespace XTPPropertyGridPaintThemes
{
	class _XTP_EXT_CLASS CXTPPropertyGridOffice2003Theme : public CXTPPropertyGridPaintManager
	{
	public:
		CXTPPropertyGridOffice2003Theme(CXTPPropertyGrid* pGrid);
		virtual void RefreshMetrics();
	

	protected:
		BOOL m_bLunaTheme;
	};	

	class _XTP_EXT_CLASS CXTPPropertyGridNativeXPTheme : public CXTPPropertyGridPaintManager
	{
	public:
		CXTPPropertyGridNativeXPTheme(CXTPPropertyGrid* pGrid);
		virtual void RefreshMetrics();
	};	

	class _XTP_EXT_CLASS CXTPPropertyGridCoolTheme : public CXTPPropertyGridPaintManager
	{
	public:
		CXTPPropertyGridCoolTheme(CXTPPropertyGrid* pGrid);
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);
	};	

	class _XTP_EXT_CLASS CXTPPropertyGridSimpleTheme : public CXTPPropertyGridPaintManager
	{
	public:
		CXTPPropertyGridSimpleTheme(CXTPPropertyGrid* pGrid);
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);
	};	

	class _XTP_EXT_CLASS CXTPPropertyGridDelphiTheme : public CXTPPropertyGridPaintManager
	{
	public:
		CXTPPropertyGridDelphiTheme(CXTPPropertyGrid* pGrid);
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);
		virtual void RefreshMetrics();

	};	

}
using namespace XTPPropertyGridPaintThemes;
//:End Ignore


#endif // !defined(_XTPPROPERTYGRIDPAINTMANAGER_H__)
