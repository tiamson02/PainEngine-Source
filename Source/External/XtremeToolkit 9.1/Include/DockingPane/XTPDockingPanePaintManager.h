// XTPDockingPanePaintManager.h : interface for the CXTPDockingPanePaintManager class.
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
#if !defined(__XTPDOCKINGPANEPAINTMANAGER_H__)
#define __XTPDOCKINGPANEPAINTMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneDefines.h"
#include "CommonPro.include/XTPWinThemeWrapper.h"
#include "CommonPro.include/XTPColorManager.h" 

//forwards

class CXTPDockingPaneTabsArray;
class CXTPDockingPaneTabbedContainer;
class CXTPDockingPaneAutoHidePanel;
class CXTPDockingPaneHiddenTabsArray;
class CXTPTabPaintManager;
class CXTPDockingPaneMiniWnd;
class CXTPDockingPaneSplitterWnd;

struct XTP_DOCKINGPANE_TAB;

#define XTP_DB_HIDE		0x0002			// Pin button.
#define XTP_DB_CLOSE	0x0004			// Close button.
#define XTP_DB_PRESSED	0x1000			// Button is pressed.
#define XTP_DB_OVER		0x4000			// Cursor is over button

#define XTP_DB_ISPRESSED(Style)     (((Style) & XTP_DB_PRESSED) !=0 )			// Is button pressed.
#define XTP_DB_ISOVER(Style)        (((Style) & XTP_DB_OVER) == XTP_DB_OVER)	// Is cursor over the button.


#define XTP_CAPTIONBUTTON_CLOSE	1
#define XTP_CAPTIONBUTTON_PIN 2  

class CXTPDockingPaneCaptionButtons;
//===========================================================================
// Summary: CXTPDockingPaneCaptionButton is a stand alone internal class.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneCaptionButton
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPaneCaptionButton object
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton (int nID, CXTPDockingPaneBase* pPane) { 
		m_nID = nID; m_pPane = pPane;	
		m_bPressed = m_bSelected = FALSE;
		m_dwState = 0;
	}

	//-----------------------------------------------------------------------
	// Input:	rc - Button's rectangle.
	// Summary: Call this member to set the button's rectangle.
	//-----------------------------------------------------------------------
	void SetRect (CRect rc);

	//-----------------------------------------------------------------------
	// Input:	pDC     - Pointer to the device context in which to draw.
	//			pt      - Center point.
	//			bPinned - TRUE if the button is pinned.
	// Summary: Call this member to draw a pin button.
	//-----------------------------------------------------------------------
	static void DrawPinnButton(CDC* pDC, CPoint pt, BOOL bPinned);

	//-----------------------------------------------------------------------
	// Input:	pWnd - Parent window of button.
	//			pt   - Initial mouse position.
	// Summary: Call this method to click button.
	//-----------------------------------------------------------------------
	DWORD Click (CWnd* pWnd, CPoint pt, BOOL bClient = TRUE);
	
	//-----------------------------------------------------------------------
	// Returns: A CRect object
	// Summary: Call this member to get the bounding rectangle 
	//-----------------------------------------------------------------------
	CRect GetRect() { return m_rcButton;}

	DWORD GetState() {
		return m_dwState;
	}
	void SetState(DWORD dwState) {
		m_dwState = dwState;
	}
	
	int GetID() { 
		return m_nID;
	}	

	CXTPDockingPaneBase* GetPane() {
		return m_pPane;
	}

	//-----------------------------------------------------------------------
	// Input:	pt - Point to test.
	// Returns: TRUE if point belongs to the button; otherwise returns FALSE
	// Summary: Call this member to determine if the point belongs to button.
	//-----------------------------------------------------------------------
	BOOL PtInRect (POINT pt) const {return m_rcButton.PtInRect (pt) != 0; }
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to refresh styles of the button.
	//-----------------------------------------------------------------------
	BOOL CheckForMouseOver (CPoint pt);

	void InvalidateRect();

	BOOL IsPressed() {
		return m_bPressed;
	}

	BOOL IsSelected() {
		return m_bSelected;
	}

	virtual BOOL IsVisible();

protected:
	CRect m_rcButton;
	CXTPDockingPaneBase* m_pPane;
	int m_nID;
	BOOL m_bPressed;
	BOOL m_bSelected;
	DWORD m_dwState;
};

//===========================================================================
// Remarks: CXTPDockingPanePaintManager is a stand alone class. 
//			Override some virtual functions to get a new visual theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPanePaintManager
{

public:

	//-----------------------------------------------------------------------
	// Input:	dc    - Reference to the device context in which to draw.
	//			pPane - Pointer to the tabbed container.
	//			rc    - Client rectangle of the tabbed container.
	// Summary: Override this member function to draw a tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Input:	dc - Reference to the device context in which to draw.
	//			pPane - Pointer to the tabbed container.
	//			rc - Client rectangle of the tabbed container.
	// Summary: Override this member function to draw a caption.
	//-----------------------------------------------------------------------
	virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc) = 0;

	//-----------------------------------------------------------------------
	// Summary: This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Input:	pPane - Pointer to the tabbed container. 
	//			rect  - Client rectangle to be changed.
	// Summary: Override this member to change the client rectangle of the child 
	//			docking pane.
	//-----------------------------------------------------------------------
	virtual void AdjustClientRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rect);
	
	//-----------------------------------------------------------------------
	// Input:	pPane - Pointer to a CXTPDockingPaneTabbedContainer object.
	//			rc    - Caption rectangle to be changed.
	// Summary: Override this member to change the caption rectangle of the child
	//			docking pane.
	//-----------------------------------------------------------------------
	virtual void AdjustCaptionRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rc);


public:

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			x0 - Specifies the logical x-coordinate of the start position. 
	//			y0 - Specifies the logical y-coordinate of the start position. 
	//			x1 - Specifies the logical x-coordinate of the endpoint for the line. 
	//			y1 - Specifies the logical y-coordinate of the endpoint for the line. 
	//			nPen - Specifies the color used to paint the line.
	// Summary: Draws a line.
	//-----------------------------------------------------------------------
	void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			x - Specifies the logical x-coordinate of the point to be set. 
	//			y - Specifies the logical y-coordinate of the point to be set. 
	//			nPen -  Specifies the color used to paint the point
	// Summary: Sets the pixel at the point.
	//-----------------------------------------------------------------------
	void Pixel(CDC* pDC, int x, int y, int nPen);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			rc - Specifies the rectangle in logical units.
	//			nPenEntry - Specifies the color used to paint the rectangle.
	//			nPenBorder - Specifies the color used to fill the rectangle.
	// Summary: Draws a rectangle.
	//-----------------------------------------------------------------------
	void Rectangle(CDC* pDC, CRect rc, int nPenEntry, int nPenBorder);

	//-----------------------------------------------------------------------
    // Input:	bUseOfficeFont - TRUE to use office font.
	// Summary: Call this member to use office (Tahoma) font.
	//-----------------------------------------------------------------------
	void UseOfficeFont(bool bUseOfficeFont);
    
	//-----------------------------------------------------------------------
	// Summary: This member is called to re-allocate fonts.
	//-----------------------------------------------------------------------
	void FreeSysFont();

	virtual void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	virtual void DrawSplitter(CDC& dc, CXTPDockingPaneSplitterWnd* pSplitter);


	CXTPTabPaintManager* SetTabPaintManager(CXTPTabPaintManager* pManager);
	CXTPTabPaintManager* SetPanelPaintManager(CXTPTabPaintManager* pManager);
	
	CXTPTabPaintManager* GetTabPaintManager();
	CXTPTabPaintManager* GetPanelPaintManager();


	COLORREF GetXtremeColor(UINT nIndex);

	void SetColors(int cElements, CONST INT *lpaElements, CONST COLORREF *lpaRgbValues);

	virtual void DrawCaptionButtons(CDC* pDC, CXTPDockingPaneCaptionButtons* pButtons, CRect& rcCaption, COLORREF clr, int nButtonSize, int nButtonGap);

	virtual void DrawCaptionButton(CDC* /*pDC*/, CXTPDockingPaneCaptionButton* /*pButton*/, COLORREF /*clrButton*/) {}

protected:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPanePaintManager object
	//-----------------------------------------------------------------------
	CXTPDockingPanePaintManager(void);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPanePaintManager object, handles cleanup 
	//			and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPanePaintManager(void);

	//-----------------------------------------------------------------------
    // Summary: Log font helper.
	//-----------------------------------------------------------------------
	struct CXTPLogFont : public LOGFONT {
		// Summary: Constructs a CXTPLogFont object
        CXTPLogFont(bool bIconTitle=false);
    };

	//-----------------------------------------------------------------------
    // Summary: Client Metrics helper.
	//-----------------------------------------------------------------------
	struct CXTPNonClientMetrics : public NONCLIENTMETRICS {
		// Summary: Constructs a CXTPNonClientMetrics object
        CXTPNonClientMetrics();
    };

	//-----------------------------------------------------------------------
	// Input:	strFaceName - Font need to test.
	// Returns: True if font exists; otherwise returns false.
    // Summary: Call this member to check if the font exists in the system.
	//-----------------------------------------------------------------------
	bool FontExists(CString& strFaceName);
	
public:
	int   m_nTitleHeight;    // Title's height.
	int	  m_nTabsHeight;     // Tab bar's height.
	CFont m_fntTitle;        // Title font.
	CFont m_fntIcon;         // Tabs font.
	CFont m_fntIconVertical; // Vertical tabs font.
    bool  m_bUseOfficeFont;  // TRUE to use Tahoma font.	
	BOOL  m_bDrawSingleTab;
	BOOL  m_nSplitterSize;
	BOOL m_bShowCaption;

	CXTPTabPaintManager* m_pTabPaintManager;
	CXTPTabPaintManager* m_pPanelPaintManager;

	COLORREF m_arrColor[XPCOLOR_LAST + 1];

protected:
	XTPDockingPanePaintTheme m_themeCurrent;	// Current theme.
    CString m_strOfficeFont;					// Office font name.
	CRect m_rcCaptionMargin;


private:
	friend CXTPDockingPanePaintManager* XTPDockingPanePaintManager();	
	friend class CXTPDockingPaneManager;								
};


//:Ignore
namespace XTPDockingPanePaintThemes
{
	class _XTP_EXT_CLASS CXTPDockingPaneDefaultTheme : public CXTPDockingPanePaintManager
	{ 
	public:
		CXTPDockingPaneDefaultTheme();

		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);
		
		void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clrButton);
		virtual void DrawCaption(CDC& dc,CXTPDockingPaneTabbedContainer* pPane, CRect rc);
	};

	class _XTP_EXT_CLASS CXTPDockingPaneGripperedTheme : public CXTPDockingPaneDefaultTheme
	{
	public:
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);
		virtual void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clrButton);
		virtual void DrawCaption(CDC& dc,CXTPDockingPaneTabbedContainer* pPane, CRect rc);
	protected:
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, CString strTitle, BOOL bActive);
	};

	class _XTP_EXT_CLASS CXTPDockingPaneOfficeTheme : public CXTPDockingPaneGripperedTheme
	{
	public:
		CXTPDockingPaneOfficeTheme();

		//void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);
		//virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);
	protected:
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, CString strTitle, BOOL bActive);

	};

	class _XTP_EXT_CLASS CXTPDockingPaneNativeXPTheme : public CXTPDockingPaneOfficeTheme
	{
	public:
		CXTPDockingPaneNativeXPTheme();
		virtual void RefreshMetrics();
		
		void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clr);
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	protected:
		CXTPWinThemeWrapper m_themeWindow;
	};	

	class _XTP_EXT_CLASS CXTPDockingPaneVisioTheme : public CXTPDockingPaneOfficeTheme
	{
	public:
		CXTPDockingPaneVisioTheme();
		void RefreshMetrics();		
		void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);
		void DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);
		void AdjustCaptionRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rc);	
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	
	public:
		void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF /*clr*/);
		int DrawCaptionButtonFrame(CDC* pDC, CXTPDockingPaneCaptionButton* pButton);

		BOOL m_bDrawTabsIcons;
	protected:
		CBitmap m_bmpGripper;
	};
	
	class _XTP_EXT_CLASS CXTPDockingPaneOffice2003Theme : public CXTPDockingPaneOfficeTheme  
	{
	public:
		CXTPDockingPaneOffice2003Theme();
		virtual ~CXTPDockingPaneOffice2003Theme();

		virtual void RefreshMetrics();
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);		


		virtual void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);		
		
		COLORREF GetColor(XTP_DOCKINGPANE_TAB* pTab);
		COLORREF DrawButton(CDC* pDC, CRect rc, COLORREF clrDark, BOOL bFocused, COLORREF clrBorder, COLORREF clrLeftHighlight, COLORREF clrTopHighlight);

		virtual int DrawCaptionButtonFrame(CDC* pDC, CXTPDockingPaneCaptionButton* pButton);
		void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clr);

	protected:
		COLORREF m_clrCommandBarLight;
		COLORREF m_clrCommandBarDark;

		BOOL m_bLunaTheme;
	};

};

using namespace XTPDockingPanePaintThemes;
//:End Ignore

AFX_INLINE CXTPTabPaintManager* CXTPDockingPanePaintManager::GetTabPaintManager() {
	return m_pTabPaintManager;
}
AFX_INLINE CXTPTabPaintManager* CXTPDockingPanePaintManager::GetPanelPaintManager() {
	return m_pPanelPaintManager;
}
AFX_INLINE void CXTPDockingPanePaintManager::DrawFloatingFrame(CDC& /*dc*/, CXTPDockingPaneMiniWnd* /*pPane*/, CRect /*rc*/) {
}

#endif // #if !defined(__XTPDOCKINGPANEPAINTMANAGER_H__)