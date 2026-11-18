// XTPShortcutBarPaintManager.h interface for the CXTPShortcutBarPaintManager class.
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
#if !defined(__XTPSHORTCUTBARPAINTMANAGER_H__)
#define __XTPSHORTCUTBARPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "CommonPro.include/XTPColorManager.h"

// Summary: XTPShortcutBarPaintTheme is an enumeration used by CXTPShortcutBar to set the theme of the control.
enum XTPShortcutBarPaintTheme 
{
	xtpShortcutThemeOffice2000,   // Office 2000 like theme.
	xtpShortcutThemeOfficeXP,     // Office XP like theme.
	xtpShortcutThemeOffice2003    // Office 2003 like theme.
};

class CXTPShortcutBar;
class CXTPShortcutBarItem;
class CXTPShortcutBarPane;
class CXTPShortcutBarPaneItem;
class CXTPImageManagerIcon;

// Summary: Standalone class used by CXTPShortcutBar as paint manager.
class _XTP_EXT_CLASS CXTPShortcutBarPaintManager  
{
public:
	//Summary: Constructs a CXTPShortcutBarPaintManager object
	CXTPShortcutBarPaintManager();
	
	// Summary: Destroys a CXTPShortcutBarPaintManager object, handles cleanup and de-allocation
	virtual ~CXTPShortcutBarPaintManager();

	// Summary: This method is called to refresh the visual metrics of the manager.
	virtual void RefreshMetrics();
	
	// Input:	pDC - Points to the client device context.
	//			pItem - Item to be drawn.
	// Summary: This method is called to draw the specified item of the ShortcutBar control
	virtual void DrawShortcutItem(CDC* pDC, CXTPShortcutBarItem* pItem);

	// Input:	pDC - Points to the client device context.
	//			pShortcutBar - Points to the ShortcutBar control
	// Summary: This method is called to fill the client area of the ShortcutBar control
	virtual void FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar) = 0;
	
	// Input:	pDC - Points to the client device context.
	//			pShortcutBar - Points to the ShortcutBar control
	// Summary: This method is called to draw the frame of the ShortcutBar control
	virtual void DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar)  = 0;	
	
	// Input:	pDC - Points to the client device context.
	//			pItem - Item to be drawn.
	// Summary: This method is called to draw the specified item of the ShortcutBar control
	virtual void FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem) = 0;
	
	// Input:	pDC - Points to the client device context.
	//			pShortcutBar - Points to the ShortcutBar control
	//			bDraw - TRUE to draw, FALSE to determine size of the gripper.
	// Returns: Size of the gripper.
	// Summary: This method is called to draw gripper of the ShortcutBar control
	virtual int DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw) = 0;

	// Input:	pDC - Points to the client device context.
	//			pPane - Shortcut Pane to be drawn
	//			bDraw - bDraw - TRUE to draw, FALSE to determine size of the caption
	// Returns: Size of the caption.
	// Summary: This method is called to draw the caption of the specified pane.
	virtual int DrawPaneCaption(CDC* pDC, CXTPShortcutBarPane* pPane, BOOL bDraw);
	
	// Input:	pDC - Points to the client device context.
	//			pPane - Shortcut item pane to be drawn
	//			bDraw - bDraw - TRUE to draw, FALSE to determine size of the caption
	// Returns: Size of the caption.
	// Summary: This method is called to draw the caption of the specified item pane.
	virtual int DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw);	

	// Input:	pDC - Points to the client device context.
	//			rc - Client rectangle of the expand button.
	// Summary: This method is called to draw the expand button.
	void DrawExpandButton(CDC* pDC, CRect rc);

	virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked) = 0;

public:
	CFont m_fntText;			// Text font.
	CFont m_fntPaneCaption;		// Caption font.
	CFont m_fntPaneItemCaption;	// Subcaption font.

	int m_nPaneItemHeight;		// Height of caption.
	HICON m_hExpandIcon;		// Icon of expand button.
	
	int m_nNormalIconWidth;		// Width of the icons.
	int m_nExpandedIconWidth;	// Width of the expanded icons.

	COLORREF m_clrPushedText;

};

//:Ignore
namespace XTPShortcutBarPaintThemes
{
	class _XTP_EXT_CLASS CXTPShortcutBarOffice2003Theme : public CXTPShortcutBarPaintManager
	{
	public:
		CXTPShortcutBarOffice2003Theme();

		void FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		void DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		
		void FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem);
		
		int DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw);
		int DrawPaneCaption(CDC* pDC, CXTPShortcutBarPane* pPane, BOOL bDraw);
		int DrawPaneItemCaption(CDC* pDC, CXTPShortcutBarPaneItem* pItem, BOOL bDraw);	

		void RefreshMetrics();

		void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked);

	protected:
		XTP_GRADIENT_COLOR m_grcSelected;
		XTP_GRADIENT_COLOR m_grcHot;
		XTP_GRADIENT_COLOR m_grcPushed;
		XTP_GRADIENT_COLOR m_grcShortcutItem;
		
	};

	class _XTP_EXT_CLASS CXTPShortcutBarOfficeXPTheme : public CXTPShortcutBarPaintManager
	{
	public:

		void FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		void DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		
		void FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem);
		
		int DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw);

		void Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush);
		void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen);

		void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked);
		
	};

	class _XTP_EXT_CLASS CXTPShortcutBarOffice2000Theme : public CXTPShortcutBarPaintManager
	{
	public:

		void FillShortcutBar(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		void DrawShortcutBarFrame(CDC* pDC, CXTPShortcutBar* pShortcutBar);
		
		void FillShortcutItemEntry(CDC* pDC, CXTPShortcutBarItem* pItem);
		
		int DrawGripper(CDC* pDC, CXTPShortcutBar* pShortcutBar, BOOL bDraw);

		void Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush);
		void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen);

		void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bChecked);
		
	};


}

using namespace XTPShortcutBarPaintThemes;
//:End Ignore

#endif // !defined(__XTPSHORTCUTBARPAINTMANAGER_H__)

