// XTPTabPaintManager.h: interface for the CXTPTabPaintManager class.
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
#if !defined(__XTPTABPAINTMANAGER_H__)
#define __XTPTABPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPTabManager;
class CXTPTabManagerAtom;
class CXTPTabManagerItem;
class CXTPTabManagerNavigateButton;

#include "CommonPro.include/XTPColorManager.h"
#include "CommonPro.include/XTPWinThemeWrapper.h"

#define XTP_TABMANAGER_COLOR_NONE (COLORREF)(-1)
#define XTP_TABMANAGER_COLOR_SHADED (COLORREF)(-2)

#ifndef XTP_TABCOLOR_BLUE

#define XTP_TABCOLOR_BLUE		0x1000000
#define XTP_TABCOLOR_YELLOW		0x1000001
#define XTP_TABCOLOR_GREEN		0x1000002
#define XTP_TABCOLOR_RED		0x1000003
#define XTP_TABCOLOR_PURPLE		0x1000004
#define XTP_TABCOLOR_CYAN		0x1000005
#define XTP_TABCOLOR_ORANGE		0x1000006
#define XTP_TABCOLOR_MAGENTA	0x1000007

#endif  

#define XTP_TABMANAGER_BUTTON_LEFT    0
#define XTP_TABMANAGER_BUTTON_RIGHT   1
#define XTP_TABMANAGER_BUTTON_CLOSE   2


#define XTP_WORKSAPCE_PART_BORDER 0
#define XTP_WORKSAPCE_PART_WIDRH 1
#define XTP_WORKSAPCE_PART_VSPLITTER 2
#define XTP_WORKSAPCE_PART_HSPLITTER 3


enum XTPTabAppearanceStyle
{
	xtpTabAppearancePropertyPage,
	xtpTabAppearancePropertyPageSelected,
	xtpTabAppearancePropertyPageFlat,
	xtpTabAppearancePropertyPage2003,
	xtpTabAppearanceStateButtons,
	xtpTabAppearanceVisualStudio,
	xtpTabAppearanceFlat,
	xtpTabAppearanceExcel,
	xtpTabAppearanceVisio,
};

enum XTPTabColorStyle
{
	xtpTabColorDefault = 1,
	xtpTabColorVisualStudio = 2,
	xtpTabColorOffice2003 = 4,
	xtpTabColorWinXP = 8
};

enum XTPTabPosition
{
	xtpTabPositionTop, 
	xtpTabPositionLeft,
	xtpTabPositionBottom, 
	xtpTabPositionRight, 
};

enum XTPTabLayoutStyle
{
	xtpTabLayoutAutoSize,
	xtpTabLayoutSizeToFit,
	xtpTabLayoutFixed,
	xtpTabLayoutCompressed,
};

enum XTPTabClientFrame
{
	xtpTabFrameBorder,
	xtpTabFrameSingleLine,
	xtpTabFrameNone
};

enum XTPTabNavigateButtonFlags
{
	xtpTabNavigateButtonNone,
	xtpTabNavigateButtonAutomatic,
	xtpTabNavigateButtonAlways,
};


class _XTP_EXT_CLASS CXTPTabPaintManager : public CCmdTarget
{
public:
	
	//////////////////////////////////////////////////////////////////////////
	/// CColorSet
	
	class _XTP_EXT_CLASS CColorSet
	{
	public:
		struct COLORSET_TAB_EXCEL
		{
			COLORREF clrLeftInnerBorder;
			COLORREF clrTopInnerBorder;
			COLORREF clrRightInnerBorder;
			COLORREF clrLeftOuterBorder;
			COLORREF clrRightOuterBorder;
			COLORREF clrTopOuterBorder;
		};

		struct COLORSET_TAB_PROPERTYPAGE
		{
			COLORREF clrHighlight;
			COLORREF clrShadow;
			COLORREF clrDarkShadow;
		};

		struct COLORSET_TAB_PROPERTYPAGE2003
		{
			COLORREF clrDarkShadow;
			COLORREF clrFrameBorder;
			COLORREF clrBorderLeftHighlight;
			COLORREF clrBorderTopHighlight;
			COLORREF clrHighlightFace;
		};
		
		struct COLORSET_TAB_STATEBUTTON
		{
			XTP_GRADIENT_COLOR clrFace;
			XTP_GRADIENT_COLOR clrBorder;
		};

	public:
		CColorSet();
		virtual void RefreshMetrics();

	public:
		virtual void SetTextColor(CDC* pDC, CXTPTabManagerItem* pItem);
		
		virtual void FillHeader(CDC* pDC, CRect rc, CXTPTabManager* pTabManager);
		virtual COLORREF FillClient(CDC* pDC, CRect rc, CXTPTabManager* pTabManager);
		virtual void FillStateButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);
		virtual COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);
		virtual void SelectClipRgn(CDC* pDC, CRect rcClient, CXTPTabManager* pTabManager);
		
		virtual void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc) = 0;

		virtual COLORREF GetItemColor(CXTPTabManagerItem* pItem);
		
		void GradientFill(CDC* pDC, CRect rc, COLORREF clrLight, COLORREF clrDark, XTPTabPosition position);
		
		virtual BOOL IsAppThemed();

	protected:
		
		CXTPTabPaintManager* m_pPaintManager;
	public:

		XTP_GRADIENT_COLOR m_clrHeaderFace;
		XTP_GRADIENT_COLOR m_clrAutoHideFace;
		
		COLORREF m_clrFrameBorder;
		COLORREF m_clrControlFace;
		COLORREF m_clrHighlight;

		COLORREF m_clrButtonSelected;
		COLORREF m_clrButtonNormal;

		COLORREF m_clrNormalText;
		COLORREF m_clrInActiveText;
		COLORREF m_clrSelectedText;
		COLORREF m_clrHighlightText;
		COLORREF m_clrDisabledText;

		COLORSET_TAB_STATEBUTTON m_csStateButton;
		COLORSET_TAB_PROPERTYPAGE m_csPropertyPage;
		COLORSET_TAB_PROPERTYPAGE2003 m_csPropertyPage2003;
		COLORSET_TAB_EXCEL m_csExcelSelected;	
		COLORSET_TAB_EXCEL m_csExcelNormal;

		friend class CXTPTabPaintManager;		
	};

	class _XTP_EXT_CLASS CColorSetDefault : public CColorSet
	{
	public:
		virtual void RefreshMetrics();		
		void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);
	};
	
	class _XTP_EXT_CLASS CColorSetVisualStudio : public CColorSetDefault
	{
	public:
		virtual void RefreshMetrics();
		void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);
		
	};

	class _XTP_EXT_CLASS CColorSetOffice2003 : public CColorSetDefault
	{
	public:
		virtual void RefreshMetrics();
		virtual COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);
		virtual void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);

		void Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush);
	
	protected:
		COLORREF m_clrButtonHighlighted;
		COLORREF m_clrNavigateButtonBorder;
		BOOL m_bLunaTheme;
		
	};

	class _XTP_EXT_CLASS CColorSetWinXP : public CColorSetDefault
	{
	public:
		virtual void RefreshMetrics();
		virtual COLORREF FillPropertyButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);
		virtual COLORREF FillClient(CDC* pDC, CRect rc, CXTPTabManager* pTabManager);
		virtual void SelectClipRgn(CDC* pDC, CRect rcClient, CXTPTabManager* pTabManager);
		virtual void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);
		virtual void FillStateButton(CDC* pDC, CRect rcItem, CXTPTabManagerItem* pItem);
		virtual BOOL IsAppThemed();

	protected:
		CXTPWinThemeWrapper m_themeTabButton;	
		CXTPWinThemeWrapper m_themeToolbar;
	};


	//////////////////////////////////////////////////////////////////////////
	/// CAppearanceSet
	
	class _XTP_EXT_CLASS CAppearanceSet
	{		
	public:
		CAppearanceSet();
		virtual void RefreshMetrics();

		CColorSet* GetColorSet() 
		{
			return m_pPaintManager->m_pColorSet;
		}
		virtual CRect GetHeaderRect(CRect rcControl, CXTPTabManager* pTabManager);
		virtual CRect GetClientRect(CRect rcControl, CXTPTabManager* pTabManager);

		virtual CRect GetHeaderMargin();
		virtual CRect GetClientMargin();

		virtual int GetButtonLength(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual int GetButtonHeight(CXTPTabManager* pTabManager);

	public:
		virtual void DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		virtual void RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient);
		virtual void AdjustClientRect(CXTPTabManager* pTabManager, CRect& rcClient);
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		

		virtual void DrawWorkspacePart(CDC* pDC, LPRECT lpRect, int nPart);

		virtual int GetSupportedColorSets() { return xtpTabColorDefault | xtpTabColorVisualStudio | xtpTabColorOffice2003 | xtpTabColorWinXP; }
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorDefault; }

	public:
		COLORREF FillButton(CDC* pDC, CXTPTabManagerItem* pItem, LPPOINT pts, int nCount, XTPTabPosition tabPosition, BOOL bAbsoulute = FALSE);
		void DrawPolyLine(CDC* pDC, COLORREF clr, LPPOINT pts, int nCount);
		void DrawSingleLineBorder(CDC* pDC, CRect rc, XTPTabPosition position, COLORREF clrTopLeft, COLORREF clrBottomRight);
		
		static void InflateRectEx(CRect& rc, CRect rcInflate, XTPTabPosition position);
		static void DeflateRectEx(CRect& rc, CRect rcDeflate, XTPTabPosition position);

	public:
		CRect m_rcHeaderMargin;		

	protected:
		int m_nButtonHeight;
		CXTPTabPaintManager* m_pPaintManager;

		friend class CXTPTabPaintManager;
		
	};

	class _XTP_EXT_CLASS CAppearanceSetPropertyPage : public CAppearanceSet
	{
	public:
		CAppearanceSetPropertyPage()
		{
			m_rcHeaderMargin.SetRect(2, 3, 6, 0);			
		}
		virtual void RefreshMetrics();
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		virtual int GetSupportedColorSets() { return xtpTabColorDefault | xtpTabColorOffice2003 | xtpTabColorWinXP; }
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorDefault; }
		virtual CRect GetClientMargin();	
	};
	
	class _XTP_EXT_CLASS CAppearanceSetPropertyPageSelected : public CAppearanceSetPropertyPage
	{
	public:
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);		
	};
	
	class _XTP_EXT_CLASS CAppearanceSetPropertyPage2003 : public CAppearanceSet
	{
	public:
		CAppearanceSetPropertyPage2003()
		{
			m_rcHeaderMargin.SetRect(2, 3, 6, 0);			
		}
		virtual CRect GetHeaderMargin()
		{
			return CRect(m_rcHeaderMargin.left + m_nButtonHeight, m_rcHeaderMargin.top, m_rcHeaderMargin.right, m_rcHeaderMargin.bottom);
		}

		virtual void RefreshMetrics();
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		virtual int GetSupportedColorSets() { return xtpTabColorDefault | xtpTabColorOffice2003; }
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorOffice2003; }

	public:

	
	};

	class _XTP_EXT_CLASS CAppearanceSetStateButtons : public CAppearanceSet
	{
	public:
		CAppearanceSetStateButtons()
		{
			m_rcHeaderMargin.SetRect(2, 2, 4, 2);
		}
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);

		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorVisualStudio; }
		
	};	
	

	class _XTP_EXT_CLASS CAppearanceSetVisualStudio : public CAppearanceSet
	{
	public:
		CAppearanceSetVisualStudio()
		{
			m_rcHeaderMargin.SetRect(4, 2, 4, 0);
		}
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);		
		
		virtual int GetSupportedColorSets() { return xtpTabColorDefault |xtpTabColorVisualStudio | xtpTabColorOffice2003; }
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorVisualStudio; }
	};

	class _XTP_EXT_CLASS CAppearanceSetFlat : public CAppearanceSet
	{
	public:
		CAppearanceSetFlat()
		{
			m_rcHeaderMargin.SetRect(2, 2, 4, 0);
		}
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		virtual CRect FillTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		
		virtual int GetSupportedColorSets() { return xtpTabColorDefault |xtpTabColorVisualStudio | xtpTabColorOffice2003; }
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorVisualStudio; }		
	};

	class _XTP_EXT_CLASS CAppearanceSetPropertyPageFlat : public CAppearanceSetFlat
	{
	public:
		CAppearanceSetPropertyPageFlat()
		{
			m_bBlurPoints = TRUE;
		}
		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);		
		virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorDefault; }		
	public:
		BOOL m_bBlurPoints;
	};


	class _XTP_EXT_CLASS CAppearanceSetExcel : public CAppearanceSetFlat
	{
	public:
		CAppearanceSetExcel()
		{
			m_rcHeaderMargin.SetRect(2, 3, 2, 0);			
		}
		virtual CRect GetHeaderMargin()
		{
			return CRect(m_rcHeaderMargin.left + m_nButtonHeight / 2, m_rcHeaderMargin.top, m_rcHeaderMargin.right + m_nButtonHeight / 2, m_rcHeaderMargin.bottom);
		}

		virtual void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);		
	};

	class _XTP_EXT_CLASS CAppearanceSetVisio : public CAppearanceSet
	{
	public:

		void AdjustClientRect(CXTPTabManager* pTabManager, CRect& rcClient);
		void RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient);
		CRect GetHeaderRect(CRect rcControl, CXTPTabManager* pTabManager);
		CRect GetClientRect(CRect rcControl, CXTPTabManager* pTabManager);
		void DrawSingleButton(CDC* pDC, CXTPTabManagerItem* pItem);
		void DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
		
		int GetSupportedColorSets() { return xtpTabColorDefault | xtpTabColorOffice2003; }
	};


public:
	CXTPTabPaintManager();
	virtual ~CXTPTabPaintManager();


public:

	CColorSet* SetColor(XTPTabColorStyle tabColor);
	CColorSet* SetColorSet(CColorSet* pColorSet);

	CAppearanceSet* SetAppearance(XTPTabAppearanceStyle tabAppearance);
	CAppearanceSet* SetAppearanceSet(CAppearanceSet* pAppearanceSet);

	CAppearanceSet* GetAppearanceSet();
	CColorSet* GetColorSet();

	XTPTabAppearanceStyle GetAppearance();
	XTPTabColorStyle GetColor();

	BOOL IsLunaColorsDisabled();
	
	void DisableLunaColors(BOOL bDisableLunaColors);
	void SetOneNoteColors(BOOL bOneNoteColors);
	
	void SetPosition(XTPTabPosition tabPosition);
	void SetLayout(XTPTabLayoutStyle tabLayout);

	XTPTabPosition GetPosition();
	XTPTabLayoutStyle GetLayout();



public:
	virtual void DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
	virtual void AdjustClientRect(CXTPTabManager* pTabManager, CRect& rcClient);
	virtual void RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);	
	CRect RepositionNavigateButtons(CXTPTabManager* pTabManager, CRect rcClient); 
	void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);

	int DrawSingleButtonIconAndText(CDC* pDC, CXTPTabManagerItem* pItem, CRect rcItem, BOOL bDraw);
	void EnsureVisible(CXTPTabManager* pTabManager, CXTPTabManagerItem* pItem);

	virtual void RefreshMetrics();

	void AddObserver(CXTPTabManagerAtom* pObserver);


public:
	CFont* GetFont(BOOL bVertical = FALSE);	
	CFont* GetBoldFont(BOOL bVertical = FALSE);

protected:
	class CSizeToFitSorter;
	void SizeToFit(CXTPTabManager* pTabManager, int nWidth);

	void DrawTabControlEx(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl);
	void RepositionTabControlEx(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient);

	virtual void OnPropertyChanged();

private:
	int _GetButtonLength(int nValue, int nMin, int nMax);

public:
	BOOL m_bHotTracking;
	BOOL m_bShowIcons;
	BOOL m_bBoldSelected;

	BOOL m_bDisableLunaColors;
	BOOL m_bOneNoteColors;

	XTPTabClientFrame m_clientFrame;
	BOOL m_bStaticFrame;

	CRect m_rcClientMargin;
	CRect m_rcControlMargin;
	CRect m_rcButtonMargin;

	BOOL m_bInvertGradient;
	BOOL m_bFillBackground;
	
	CSize m_szIcon;


	
protected:
	CAppearanceSet* m_pAppearanceSet;
	CColorSet* m_pColorSet;
	CArray<CXTPTabManagerAtom*, CXTPTabManagerAtom*> m_arrObservers;


	XTPTabAppearanceStyle m_tabAppearance;
	XTPTabColorStyle m_tabColor;
	XTPTabPosition m_tabPosition;
	XTPTabLayoutStyle m_tabLayout;	
	

	CFont m_fntNormal;		
	CFont m_fntBold;		
	CFont m_fntVerticalNormal;		
	CFont m_fntVerticalBold;	

	friend class CAppearanceSet;
	friend class CColorSet;
	friend class CXTPTabManager;

#ifdef _XTP_ACTIVEX

public:	

	afx_msg BOOL OleIsColorSupported(long nColor);
	afx_msg LPDISPATCH OleGetClientMargin();
	afx_msg LPDISPATCH OleGetControlMargin();
	afx_msg LPDISPATCH OleGetHeaderMargin();
	afx_msg LPDISPATCH OleGetButtonMargin();
	
	// Generated message map functions
	//{{AFX_MSG(CXTPTabPaintManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CXTPTabPaintManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTabPaintManager);

	friend class CXTPTabPaintManagerMargin;
#endif
	
};

#ifdef _XTP_ACTIVEX


class CXTPTabPaintManagerMargin : public CCmdTarget
{
public:

	CXTPTabPaintManagerMargin(CRect* pRect = NULL, CXTPTabPaintManager* pPaintManager = NULL);           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void OnPropertyChanged();

// Implementation
protected:
	CXTPTabPaintManager* m_pPaintManager;
	CRect* m_pRect;
	long m_nTop;
	long m_nLeft;
	long m_nRight;
	long m_nBottom;

	afx_msg void OleSetRect(long nLeft, long nTop, long nRight, long nBottom);

	// Generated message map functions
	//{{AFX_MSG(CTabControlItem)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CTabControlItem)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTabPaintManagerMargin);
};

#endif


AFX_INLINE void CXTPTabPaintManager::CColorSet::SelectClipRgn(CDC* /*pDC*/, CRect /*rcClient*/, CXTPTabManager* /*pTabManager*/) {
}
//////////////////////////////////////////////////////////////////////////

AFX_INLINE CRect CXTPTabPaintManager::CAppearanceSet::GetHeaderMargin() {
	return m_rcHeaderMargin;
}
AFX_INLINE CRect CXTPTabPaintManager::CAppearanceSet::FillTabControl(CXTPTabManager* /*pTabManager*/, CDC* /*pDC*/, CRect /*rcControl*/) {
	return 0;
}
AFX_INLINE void CXTPTabPaintManager::CAppearanceSet::DrawSingleButton(CDC* /*pDC*/, CXTPTabManagerItem* /*pItem*/) {

};
AFX_INLINE void CXTPTabPaintManager::CAppearanceSet::DrawTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcControl) {
	m_pPaintManager->DrawTabControlEx(pTabManager, pDC, rcControl);
}
AFX_INLINE void CXTPTabPaintManager::CAppearanceSet::RepositionTabControl(CXTPTabManager* pTabManager, CDC* pDC, CRect rcClient) {
	m_pPaintManager->RepositionTabControlEx(pTabManager, pDC, rcClient);
}

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CFont* CXTPTabPaintManager::GetFont(BOOL bVertical) {
	return bVertical? &m_fntVerticalNormal: &m_fntNormal;
}
AFX_INLINE CFont* CXTPTabPaintManager::GetBoldFont(BOOL bVertical) {
	return bVertical? &m_fntVerticalBold: &m_fntBold;
}
AFX_INLINE CXTPTabPaintManager::CAppearanceSet* CXTPTabPaintManager::GetAppearanceSet() {
	return m_pAppearanceSet;
}
AFX_INLINE CXTPTabPaintManager::CColorSet* CXTPTabPaintManager::GetColorSet() {
	return m_pColorSet;
}


#endif // !defined(__XTPTABPAINTMANAGER_H__)
