// XTPTaskPanelPaintManager.h interface for the CXTPTaskPanelPaintManager class.
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
#if !defined(__XTPTASKPANELPAINTMANAGER_H__)
#define __XTPTASKPANELPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPTaskPanel;
class CXTPTaskPanelGroupItem;
class CXTPTaskPanelScrollButton;

#include "CommonPro.include/XTPColorManager.h"

struct XTP_TASKPANEL_GROUPCOLORS
{
	XTP_GRADIENT_COLOR clrHead;
	COLORREF clrClient;
	COLORREF clrClientBorder;
	double dHeadGradientFactor;
	COLORREF clrHeadTextNormal;
	COLORREF clrHeadTextHot;

	COLORREF clrClientText;
	COLORREF clrClientLink;
	COLORREF clrClientLinkHot;
};

enum XTPTaskPanelGripper
{
	xtpTaskPanelGripperNone,
	xtpTaskPanelGripperBitmap,
	xtpTaskPanelGripperPlain,
	xtpTaskPanelGripperTriangle,	
};

// Summary: Standalone class used by CXTPTaskPanel as paint manager.
class _XTP_EXT_CLASS CXTPTaskPanelPaintManager  
{
typedef BOOL (WINAPI  *AlphaBlendProc) (HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);

public:
	static const CRect rectDefault;

public:

	//Summary: Constructs a CXTPTaskPanelPaintManager object
	CXTPTaskPanelPaintManager();
	
	// Summary: Destroys a CXTPTaskPanelPaintManager object, handles cleanup and de-allocation
	virtual ~CXTPTaskPanelPaintManager();

	// Input:	pDC - Pointer to a valid device context
	//			pTaskPanel - Points to a CXTPTaskPanel object
	// Summary: This method is called to fill background of TaskPanel control.
	virtual void FillTaskPanel(CDC* pDC, CXTPTaskPanel* pTaskPanel);
	
	// Input:	pDC - Pointer to a valid device context
	//			pGroup - Points to a CXTPTaskPanelGroup object
	// Summary: This method is called to draw a single group of the control.
	virtual void DrawGroupClient(CDC* pDC, CXTPTaskPanelGroup* pGroup);
	
	// Input:	pDC - Pointer to a valid device context
	//			pGroup - Points to a CXTPTaskPanelGroup object
	//			bDraw - TRUE to draw; FALSE to retrieve height of caption.
	// Returns: Height of the caption.
	// Summary: This method is called to draw the caption of the group.
	virtual int DrawGroupCaption(CDC* pDC, CXTPTaskPanelGroup* pGroup, BOOL bDraw);
	
	// Input:	pDC - Pointer to a valid device context.
	//			pItem - Points to a CXTPTaskPanelGroupItem object.
	//			rc - Rectangle of item to draw.
	//			bDraw - TRUE to draw; FALSE to retrieve height of item
	// Returns: Height of item.
	// Summary: This method is called to draw single item of control.
	virtual CRect DrawGroupItem(CDC* pDC, CXTPTaskPanelGroupItem* pItem, CRect rc, BOOL bDraw);

	// Summary: This method is called to refresh the visual metrics of manager.
	virtual void RefreshMetrics();

	// Summary: Retrieves group colors.
	XTP_TASKPANEL_GROUPCOLORS* GetGroupColors(BOOL bSpecial);

	virtual void DrawGroupItemFrame(CDC* pDC, CXTPTaskPanelGroupItem* pItem, CRect rc);

	virtual CRect GetItemOuterMargins(CXTPTaskPanelGroupItem* pItem);
	virtual CRect GetItemInnerMargins(CXTPTaskPanelGroupItem* pItem);
	virtual CRect GetGroupOuterMargins(CXTPTaskPanelGroup* pGroup);
	virtual CRect GetGroupInnerMargins(CXTPTaskPanelGroup* pGroup);
	virtual CRect GetControlMargins(CXTPTaskPanel* pPanel);
	virtual int GetGroupSpacing(CXTPTaskPanel* pPanel);

	virtual void DrawScrollButton(CDC* pDC, CXTPTaskPanelScrollButton* pScrollButton);

public:	
	void SetItemsFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardItemsFont = FALSE);
	void SetCaptionFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardCaptionFont = FALSE);
	
	CFont* GetItemFont(CXTPTaskPanelItem* pItem);

protected:
	CRect DrawGroupItemImageWithText(CDC* pDC, CXTPTaskPanelGroupItem* pItem, CRect rc, BOOL bDraw);
	CRect DrawGroupItemImageWithTextBelow(CDC* pDC, CXTPTaskPanelGroupItem* pItem, CRect rc, BOOL bDraw);
	CRect DrawGroupItemImage(CDC* pDC, CXTPTaskPanelGroupItem* pItem, CRect rc, BOOL bDraw);

protected:
	
	// Input:	pDC - Pointer to a valid device context
	//			pGroup - Points to a CXTPTaskPanelGroup object
	//			rc - Rectangle of group to be draw.
	// Summary: This method is called to draw the face of the group.
	virtual void DrawGroupClientFace(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
	
	// Input:	pDC - Pointer to a valid device context
	//			pGroup - Points to a CXTPTaskPanelGroup object
	//			rc - Rectangle of group to be draw.
	// Summary: This method is called to draw the items of the group.
	virtual void DrawGroupClientItems(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
	
	// Input:	pDC - Pointer to a valid device context
	//			pGroup - Points to a CXTPTaskPanelGroup object
	//			rc - Rectangle of group to be drawn.
	// Returns: Rectangle of caption after draw.
	// Summary: This method is called to draw gripper of the caption.
	virtual CRect DrawCaptionGripper(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);

	virtual void DrawItemCaption(CDC* pDC, CXTPTaskPanelItem* pItem, CRect rcText, UINT nFormat);
	virtual void DrawItemImage(CDC* pDC, CXTPTaskPanelItem* pItem, CPoint ptIcon, CXTPImageManagerIcon* pImage, CSize szIcon);



	// Helper functions
protected:	
	COLORREF BlendColor(COLORREF clrFrom, COLORREF clrTo, int nAlpha);
	CRect DrawCaptionGripperBitmap(CDC* pDC, BOOL bSpecial, BOOL bExpanded, BOOL bHot, CRect rc);
	void DrawCaptionGripperSigns(CDC* pDC, CPoint pt, COLORREF clr, BOOL bExpanded);
	void CreateGripperBitmaps();
	void CreateGripperIcon(HICON& hIcon, UINT nIDResource, COLORREF clrGripperBack);
	void FreeBitmaps();
	void PremultplyAlpha(HBITMAP& bmp);
	void Rectangle(CDC* pDC, CRect rc, COLORREF clrPen, COLORREF clrBrush);

	virtual CPoint GetHighlightOffset(CXTPTaskPanelGroupItem* pItem);
	virtual void DrawDragArrow(CDC* pDC, CXTPTaskPanelItem* pItem, CRect rc, BOOL bAfter = FALSE);
		
public:
	BOOL m_bEmulateEplorerTheme;		// TRUE to use Explorer theme under WinXP and Office 2000 under older OS.
	XTPTaskPanelGripper m_eGripper;		// Gripper of the manager.

	HICON m_hGripperNormal;				// Icon of normal gripper
	HICON m_hGripperSpecial;			// Icon of special gripper.

	
	HBITMAP m_bmpGrippers[8];			// Bitmaps of alpha grippers.


	BOOL m_bUseBitmapGrippers;			// TRUE to use alpha grippers.
	BOOL m_bBoldCaption;

	CRect m_rcGroupOuterMargins;
	CRect m_rcGroupInnerMargins;
	CRect m_rcItemOuterMargins;
	CRect m_rcItemInnerMargins;
	CRect m_rcControlMargins;
	int m_nGroupSpacing;

	BOOL m_bOfficeHighlight;
	BOOL m_bInvertDragRect;
	BOOL m_bCaptionScrollButton;

protected:

	CFont m_fntCaption;							// Caption font.
	CFont m_fntIcon;							// Normal Items font.
	CFont m_fntIconBold;						// Bold items font.
	CFont m_fntIconHot;							// Hot items font.
	CFont m_fntIconHotBold;						// Bold items font.

	XTP_GRADIENT_COLOR m_clrBackground;			// Background color

	XTP_TASKPANEL_GROUPCOLORS m_groupNormal;	// Normal colors set.
	XTP_TASKPANEL_GROUPCOLORS m_groupSpecial;	// Special colors set.

	COLORREF m_clrHighlightHot;
	COLORREF m_clrHighlightSelected;
	COLORREF m_clrHighlightPressed;
	XTP_GRADIENT_COLOR m_clrHighlightBorder;

	BOOL m_bOfficeBorder;						// TRUE to use office borders.
	BOOL m_bLeftRoundedCorner;					// TRUE to round left corner
	BOOL m_bRightRoundedCorner;					// TRUE to round right corner.

	int m_nCaptionHeight;						// Caption height in pixels.
	AlphaBlendProc m_pAlphaBlend;				// Alpha blend function.
	HMODULE m_hLib;								// HANDLE to msimg32.dll.

	BOOL m_bUseStandardCaptionFont;
	BOOL m_bUseStandardItemsFont;

};

//:Ignore
namespace XTPTaskPanelPaintThemes
{
	class _XTP_EXT_CLASS CXTPTaskPanelOffice2003Theme : public CXTPTaskPanelPaintManager
	{
	public:
		CXTPTaskPanelOffice2003Theme();
		void RefreshMetrics();

	};	


	class _XTP_EXT_CLASS CXTPTaskPanelExplorerTheme : public CXTPTaskPanelPaintManager
	{
	public:
		CXTPTaskPanelExplorerTheme();
		void RefreshMetrics();

		BOOL m_bExplorerTheme;
	};	


	class _XTP_EXT_CLASS CXTPTaskPanelPaintManagerPlain : public CXTPTaskPanelPaintManager
	{
	public:

		CXTPTaskPanelPaintManagerPlain();
		void RefreshMetrics();

		void DrawGroupClientFace(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
		void FillTaskPanel(CDC* pDC, CXTPTaskPanel* pTaskPanel);
		int DrawGroupCaption(CDC* pDC, CXTPTaskPanelGroup* pGroup, BOOL bDraw);
	protected:
		BOOL m_bRoundedFrame;
		BOOL m_bOfficeCaption;
	};

	class _XTP_EXT_CLASS CXTPTaskPanelExplorerThemePlain : public CXTPTaskPanelPaintManagerPlain
	{
	public:
		CXTPTaskPanelExplorerThemePlain();
		void RefreshMetrics();

		BOOL m_bExplorerTheme;

		CRect DrawCaptionGripper(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
	};

	class _XTP_EXT_CLASS CXTPTaskPanelOfficeXPThemePlain : public CXTPTaskPanelPaintManagerPlain
	{
	public:
		CXTPTaskPanelOfficeXPThemePlain();
		void RefreshMetrics();

	};	

	class _XTP_EXT_CLASS CXTPTaskPanelOffice2003ThemePlain : public CXTPTaskPanelOfficeXPThemePlain
	{
	public:
		CXTPTaskPanelOffice2003ThemePlain();
		void RefreshMetrics();
	};	

	class _XTP_EXT_CLASS CXTPTaskPanelToolboxTheme : public CXTPTaskPanelPaintManagerPlain
	{
	public:
		CXTPTaskPanelToolboxTheme();
		void RefreshMetrics();
		int DrawGroupCaption(CDC* pDC, CXTPTaskPanelGroup* pGroup, BOOL bDraw);

	protected:
		virtual CRect DrawGroupCaptionBackground(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);

	};

	class _XTP_EXT_CLASS CXTPTaskPanelToolboxWhidbeyTheme : public CXTPTaskPanelToolboxTheme
	{
	public:
		CXTPTaskPanelToolboxWhidbeyTheme();
		
		CRect DrawGroupCaptionBackground(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
		void DrawGroupClientFace(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
		void FillTaskPanel(CDC* pDC, CXTPTaskPanel* pTaskPanel);
	};

	class _XTP_EXT_CLASS CXTPTaskPanelListViewTheme : public CXTPTaskPanelToolboxTheme
	{
	public:
		CXTPTaskPanelListViewTheme();
		void RefreshMetrics();
	};

	class _XTP_EXT_CLASS CXTPTaskPanelListViewOfficeXPTheme : public CXTPTaskPanelListViewTheme
	{
	public:
		CXTPTaskPanelListViewOfficeXPTheme();
		void RefreshMetrics();
	};

	class _XTP_EXT_CLASS CXTPTaskPanelListViewOffice2003Theme : public CXTPTaskPanelListViewOfficeXPTheme
	{
	public:
		CXTPTaskPanelListViewOffice2003Theme();
		void RefreshMetrics();

		XTP_GRADIENT_COLOR m_grcHot;
		XTP_GRADIENT_COLOR m_grcPushed;
		
		CRect DrawGroupCaptionBackground(CDC* pDC, CXTPTaskPanelGroup* pGroup, CRect rc);
	};

}

using namespace XTPTaskPanelPaintThemes;
//:End Ignore

AFX_INLINE XTP_TASKPANEL_GROUPCOLORS* CXTPTaskPanelPaintManager::GetGroupColors(BOOL bSpecial) {
	return bSpecial? &m_groupSpecial: &m_groupNormal;
}


#endif // !defined(__XTPTASKPANELPAINTMANAGER_H__)
