// XTPPaintManager.h : interface for the CXTPPaintManager class.
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
#if !defined(__XTPPAINTMANAGER_H__)
#define __XTPPAINTMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"
#include "CommonPro.include/XTPWinThemeWrapper.h" 
#include "CommonPro.include/XTPColorManager.h" 

class CXTPControlButton;
class CXTPControlPopup;
class CXTPControl;
class CXTPToolBar;
class CXTPCommandBar;
class CXTPPopupBar;
class CXTPControlComboBox;
class CXTPTabControl;
class CXTPDockBar;
class CXTPStatusBar;
class CXTPControlEdit;
class CXTPImageManagerIcon;

class CXTPBufferDC;


enum XTPSpecialControl {xtpButtonExpandToolbar, xtpButtonExpandMenu, xtpButtonHideFloating, xtpButtonExpandFloating, xtpButtonCustomize};

struct XTP_COMMANDBARS_ICONSINFO
{
	BOOL bUseFadedIcons;
	BOOL bUseDisabledIcons;
	BOOL bIconsWithShadow;
};

// Summary: Paint manager of the command bars.
class _XTP_EXT_CLASS CXTPPaintManager : public CCmdTarget
{
	friend class CXTPPaintManager* XTPPaintManager();

	class CXTPPaintManagerDestructor;

public:

	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPControl object
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control.
	CSize DrawControl(CDC* pDC, CXTPControl* pButton, BOOL bDraw = TRUE);

	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPControl object
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control placed on the PopupBar.
	virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPControl object
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control placed on the ToolBar.
	virtual CSize DrawControlToolBarParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);
	
	virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rc);
	
	// Input:	pDC - Pointer to a valid device context
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//          pCombo - ComboBox to draw.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control.
	virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pCombo, BOOL bDraw) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//          pEdit - Edit control to draw.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control.
	virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pEdit, BOOL bDraw) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPControl object
	//			rc - Bounding rectangle to draw.
	//			bSelected - TRUE if the control is selected.
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns: Size of the control.
	// Summary: This method is called to draw the control placed on the ListBox.
	virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw) = 0;

	// Input:	pDC - Pointer to a valid device context
	//			pBar - Points to a CXTPCommandBar object to draw.
	// Summary: This method is called to fill the command bar's face.
	virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			pButton - Points to a CXTPControl object to draw.
	// Summary: This method is called to fill the control's face
	virtual void DrawControlEntry(CDC* pDC, CXTPControl* pButton);
	
	// Input:	pDC - Pointer to a valid device context
	//			rc - Rectangle to draw.
	//			bEnabled - TRUE if the control is enabled.
	//			bSelected - TRUE if the control is selected.
	//			bPressed - TRUE if the control is pushed.
	//			bChecked - TRUE if the control is checked.
	//			bPopuped - TRUE if the control is popuped.
	//			barPosition - Parent's bar position.
	// Summary: This method is called to fill the control's face
	virtual void DrawControlEntry(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bPopuped, XTPBarPosition barPosition) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			rc - Rectangle to draw.
	//			bSelected - TRUE if the control is selected.
	//			bPressed - TRUE if the control is pushed.
	//			bEnabled - TRUE if the control is enabled.
	//			bChecked - TRUE if the control is checked.
	//			bPopuped - TRUE if the control is popuped.
	//			bToolBar - TRUE if the parent bar is a toolbar.
	//			barPosition - Parent's bar position.
	// Summary: This method is called to fill the control's face
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBar = TRUE, XTPBarPosition barPosition = xtpBarPopup) = 0;

	// Input:	pDC - Pointer to a valid device context
	//			pBar - Points to a CXTPCommandBar object
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
	// Returns: Size of the gripper.
	// Summary: This method is called to draw the command bar's gripper.
	virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			pBar - Points to a CXTPCommandBar object
	//			pControl - Points to a CXTPControl object
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the separator.
	// Returns: This method is called to draw a command bar's separator.
	// Summary: This method is called to draw a command bar's separator.
	virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE) = 0;
	
	// Input:	rc - Control's bounding rectangle.
	//			pControl - Points to a CXTPControl object
	// Summary: This method is called to determine offset of popuped bar.
	virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			rc - Bounding rectangle of the pane.
	//			dwStyle - Style of the pane.
	//			str - Text of the pane.
	// Summary: This method is called to draw a single pane of the status bar.
	virtual void DrawStatusBarPane(CDC* pDC, CRect& rc, DWORD dwStyle, CString str) = 0;
	
	// Input:	pDC - Pointer to a valid device context
	//			rcClient - Client rectangle of the status bar.
	// Summary: This method is called to draw the status bar's gripper.
	virtual void DrawStatusBarGripper(CDC* pDC, CRect rcClient);

	// Input:	pDC - Pointer to a valid device context
	//			pBar - Points to a CXTPDockBar object
	// Summary: This method is called to fill a dockbar.
	virtual void FillDockBar(CDC* pDC, CXTPDockBar* pBar);
	
	// Input:	pDc - Pointer to a valid device context
	//			pBar - Points to a CXTPStatusBar object
	// Summary: This method is called to fill a status bar.
	virtual void FillStatusBar(CDC* pDC, CXTPStatusBar* pBar);

	// Summary: This method is called to refresh the visual metrics of the manager.
	virtual void RefreshMetrics();
	
	// Returns: TRUE to use alpha and solid shadow; otherwise returns FALSE
	// Summary: Determines the kind of shadow to use.
	virtual BOOL IsOfficeLikeShadow() { return FALSE; };

	// Input:	pDC - Pointer to a valid device context
	//			controlType - Special control enumerator.
	//			pButton - Points to a CXTPControl object to draw.
	//			pBar - Parent CXTPCommandBar object.
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//			lpParam - Specified parameter.
	// Returns: Size of the control.
	// Summary: This method is called to draw the specified controls.
	virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);
	
	// Input:	pBar - Points to a CXTPCommandBar object that the borders need to get.
	// Returns: Borders of the command bar.
	// Summary: This method is called to retrieve the command bar's borders.
	virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);	
	
	// Input:	pDC - Pointer to a valid device context
	//			rc - Bounding rectangle of the gripper.
	//			bSelected - True if the gripper is selected.
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
	// Returns: Size of the gripper.
	// Summary: This method is called to draw a tear-off gripper of Popup Bar.
	virtual CSize DrawTearOffGripper(CDC* pDC, CRect rc, BOOL bSelected, BOOL bDraw) = 0;

	// Input:	pDC - Pointer to a valid device context
	//			pt - Position to draw.
	//			pImage - Points to a CXTPImageManagerIcon object
	//			bSelected - TRUE if the control is selected.
	//			bPressed - TRUE if the control is pushed.
	//			bEnabled - TRUE if the control is enabled.
	//			bChecked - TRUE if the control is checked.
	//			bPopuped - TRUE if the control is popuped.
	//			bToolBarImage - TRUE if it is a toolbar image.
	// Summary: This method is called to draw the image of the control.
	virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE) = 0;

	

public:

	static CXTPPaintManager* AFX_CDECL CreateTheme(XTPPaintTheme paintTheme);

	// Summary: Call this member to switch the visual theme of the Command Bars..
	static void AFX_CDECL SetTheme(XTPPaintTheme paintTheme);
	
	// Summary: Call this member to set a custom theme. 
	static void AFX_CDECL SetCustomTheme(CXTPPaintManager* pTheme);

	// Returns: The current theme.
	// Summary: Call this member to get the current visual theme.
	XTPPaintTheme GetCurrentTheme() { return m_themeCurrent;}
	
	// Summary: Call this method to destroy a paint manager object.
	static void AFX_CDECL Done();
	
	// Returns: Returns the base theme.
	// Summary: Retrieves the base theme of the manager.
	virtual XTPPaintTheme BaseTheme() = 0;

public:
	
	// Input:	pBar - Points to a CXTPCommandBar object
	//          bBold - TRUE to retrieve BOLD font
	// Returns: A pointer to a CFont object
	// Summary: Call this member to retrieve the font of the command bar
	virtual CFont* GetCommandBarFont(CXTPCommandBar* pBar, BOOL bBold = FALSE);
	
	// Input:	pDC - Pointer to a valid device context
	//			pt0 - Specifies the logical coordinates of the first point. 
	//			pt1 - Specifies the logical coordinates of the second point. 
	//			pt2 - Specifies the logical coordinates of the third point. 
	//			clr - Color to fill.
	// Summary: Draws a triangle.
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr);
	
	// Summary: Refreshes the fonts to use.
	static void AFX_CDECL UpdateFonts();
	
	// Returns: A Pointer to a CFont object
	// Summary: Returns the status bar's font.
	static CFont* AFX_CDECL GetStatusFont() { return &m_fontStatus; }
	
	// Returns: A Pointer to a CFont object
	// Summary: Returns the general font.
	static CFont* AFX_CDECL GetRegularFont(){ return &m_fontRegular; }
	
	// Returns: A pointer to a CFont object
	// Summary: Returns the icon font.
	static CFont* AFX_CDECL GetIconFont() { return &m_fontIcon;}
	
	// Returns: A pointer to a CFont object
	// Summary: Returns the icon's bold font.
	static CFont* AFX_CDECL GetIconBoldFont() { return &m_fontIconBold;}
    
	// Input:	bUseOfficeFont - TRUE to use office font.
	// Summary: Call this method to use Tahoma font despite system metrics.
	void UseOfficeFont(bool bUseOfficeFont);
    
	// Summary: De-allocates all fonts.
	static void FreeSysFonts();
    
	// Returns: Controls height.
	// Summary: This method returns the common controls height.
	int GetControlHeight();

	// Input:	pDC - Pointer to a valid device context
	//			rc - Specifies the rectangle in logical units.
	//			nPen - Specifies the color used to paint the rectangle.
	//			nBrush - Specifies the color used to fill the rectangle.
	// Summary: Draws a rectangle.
	void Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush);

	// Input:	pDC - Pointer to a valid device context
	//			rc - Specifies the rectangle in logical units.
	//			nTopLeft - Specifies the color of the top and left sides of the three-dimensional rectangle. 
	//			nBottomRight - Specifies the color of the bottom and right sides of the three-dimensional rectangle. 
	// Summary: Call this member function to draw a three-dimensional rectangle.
	void Draw3dRect(CDC* pDC, CRect rc, int nTopLeft, int nBottomRight);
	
	// Input:	pDC - Pointer to a valid device context
	//			rcCheck - Specifies the rectangle in logical units.
	//			clr - Color to fill.
	// Summary: Call this member function to draw a check mark.
	void DrawCheckMark(CDC* pDC, CRect rcCheck, COLORREF clr = 0);

	// Input:	pDC - Pointer to a valid device context
	//			p0 - Specifies the logical coordinates of the start position. 
	//			p1 - Specifies the logical coordinates of the endpoint for the line. 
	// Summary: Draws a line.
	void Line(CDC* pDC, CPoint p0, CPoint p1);
	
	// Input:	pDC - Pointer to a valid device context
	//			x0 - Specifies the logical x-coordinate of the start position. 
	//			y0 - Specifies the logical y-coordinate of the start position. 
	//			x1 - Specifies the logical x-coordinate of the endpoint for the line. 
	//			y1 - Specifies the logical y-coordinate of the endpoint for the line. 
	//			nPen - Specifies the color used to paint the line.
	// Summary: Draws a line.
	void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen);
	
	// Input:	pDC - Pointer to a valid device context
	//			x - Specifies the logical x-coordinate of the point to be set. 
	//			y - Specifies the logical y-coordinate of the point to be set. 
	//			nPen -  Specifies the color used to paint the point
	// Summary: Sets the pixel at the point.
	void Pixel(CDC* pDC, int x, int y, int nPen);
	
	// Input:	pDC - Pointer to a valid device context
	//			lpRect - Specifies the rectangle in logical units.
	//			crFrom - Start color.
	//			crTo - Endpoint color.
	//			bHorz - TRUE to fill horizontally.
	// Summary: Gradient fills the rectangle.
	void GradientFill(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	
	// Summary: Retrieves specific icon's options.
	XTP_COMMANDBARS_ICONSINFO* GetIconsInfo();


	//////////////////////////////////////////////////////////////////////////
	/// Animation Routines
	
	// Summary: This method is called to animate the PopupBar.
	virtual void Animate(CDC* pDestDC, CDC* pSrcDC, CRect rc, XTPAnimationType animationType);
	
	// Summary: This method is called to animate the expanding process.
	virtual void AnimateExpanding(CXTPCommandBar* pCommandBar, CDC* pDestDC, CDC* pSrcDC);

	// Summary: Call this method to set animation delay.
	void SetAnimationDelay(int nAnimationSteps, int nAnimationTime);

	COLORREF GetXtremeColor(UINT nIndex);

	//-----------------------------------------------------------------------
	// Summary:	Call this member to set the colors for each element to the
	//          color in the specified color array.
	// Input:	cElements    - Number of elements in array.
	//			lpaElements  - Array of elements.
	//			lpaRgbValues - Array of RGB values.
	//-----------------------------------------------------------------------
	void SetColors(int cElements, CONST INT *lpaElements, CONST COLORREF* lpaRgbValues);

public:

	// Input:	strFaceName - Font needed to test.
	// Returns: TRUE if font exists; otherwise returns FALSE
	// Summary: Determines if the font exists in the system.
    static bool AFX_CDECL FontExists(CString& strFaceName);
	
	// Input:	pDC - Pointer to a valid device context
	//			str - Text to draw.
	//			rcText - Bounding rectangle.
	//			bDraw - TRUE to draw; FALSE to retrieve the size of the text.
	//			bVert - Draws vertically.
	//			bCentered - Draws centered aligned.
	//			bTriangled - Draws with triangle at the end.
	//			bEmbossed - Draws text embossed.
	// Returns: Size of the text to be drawn.
	// Summary: Draws the text
	CSize DrawTextEx(CDC* pDC, CString str, CRect rcText, BOOL bDraw, BOOL bVert, BOOL bCentered, BOOL bTriangled, BOOL bEmbosssed = FALSE);
	
	// Input:	strClear - Text needed to strip.
	// Summary: This member function will search a string, strip off the mnemonic
	//			'&', and reformat the string.
	static void AFX_CDECL StripMnemonics(CString& strClear);
protected:
	
	CSize _DrawButtonText(CDC* pDC, CString strText, CRect rcText, BOOL bVert, BOOL bDraw, BOOL bEmbossed, BOOL bTriangled, CSize szButton, BOOL bDrawImage);


protected:
	static CFont m_fontRegular;				// Regular font.
	static CFont m_fontRegularBold;			// Regular bold font.
	static CFont m_fontVert;				// Vertical font.
	static CFont m_fontVertBold;				// Vertical font.	
	static CFont m_fontSmCaption;			// Small caption font.
	static CFont m_fontIcon;				// Icon font.
	static CFont m_fontStatus;				// Status bar's font.
	static CFont m_fontMarlettIcon;			// Marlett font.
	static CFont m_fontIconBold;			// Bold font.
	static int   m_nTextHeight;				// Text height of the regular font.
    static bool  m_bUseOfficeFont;			// TRUE to use Tahoma font.

	XTPPaintTheme m_themeCurrent;			// Current theme.
	static CXTPPaintManager* s_pInstance;	// Instance of the manager.

	XTP_COMMANDBARS_ICONSINFO m_iconsInfo;	// Specific icons options.


	int m_nAnimationSteps;                  // Steps of the animation.
	int m_nAnimationTime;                   // Time of the animation.

	COLORREF m_arrColor[XPCOLOR_LAST + 1];
public:
	COLORREF m_clrShadowFactor;


protected:
	
	// Summary: Constructs a CXTPPaintManager object
	CXTPPaintManager(void);
	
	// Summary: Destroys a CXTPPaintManager object, handles cleanup and de-allocation.
	virtual ~CXTPPaintManager(void);

    // Logfont helper.
	struct CXTPLogFont : public LOGFONT {
		// Summary: Constructs a CXTPLogFont object
        CXTPLogFont(bool bIconTitle=false);
    };

    // ClientMetrics helper.
	struct CXTPNonClientMetrics : public NONCLIENTMETRICS {
		// Summary: Constructs a CXTPNonClientMetrics object
        CXTPNonClientMetrics();
    };
    
};

inline CXTPPaintManager* XTPPaintManager() 
	{ return CXTPPaintManager::s_pInstance; }

//:Ignore
namespace XTPPaintThemes
{
	class _XTP_EXT_CLASS CXTPOfficeTheme : public CXTPPaintManager
	{
	public:
		CXTPOfficeTheme();

		virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);
		virtual void DrawControlEntry(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bPopuped, XTPBarPosition barPosition);
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);
		virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw);

		virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl);
		virtual void DrawStatusBarPane(CDC* pDC, CRect& rc, DWORD dwStyle, CString str);
		virtual BOOL IsOfficeLikeShadow() { return TRUE; };
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rc, BOOL bSelected, BOOL bDraw);
		virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);	

	protected:
		virtual void DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded = FALSE);
		virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);
		virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBar = TRUE, XTPBarPosition barPosition = xtpBarPopup);
		virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE);
		virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox*, BOOL bDraw);
		virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit*, BOOL bDraw);
		void FillIntersectRect(CDC* pDC, CXTPPopupBar* pPopupBar, COLORREF clr);

		XTPPaintTheme BaseTheme() { return xtpThemeOfficeXP; }
	};

	class _XTP_EXT_CLASS CXTPDefaultTheme : public CXTPPaintManager
	{
	public:
		CXTPDefaultTheme();
		
		virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);
		virtual void DrawControlEntry(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bPopuped, XTPBarPosition barPosition);
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);
		virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl);
		virtual void DrawStatusBarPane(CDC* pDC, CRect& rc, DWORD dwStyle, CString str);
		virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);
		virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw);
		virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);	
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw);
		
	protected:
		virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);
		virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton);
		virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBar = TRUE, XTPBarPosition barPosition = xtpBarPopup);
		virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE);
		virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox*, BOOL bDraw);
		virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit*, BOOL bDraw);

		void DrawShadedRect(CDC *pDC, CRect& rect);
		virtual void DrawText(CDC* pDC, CString strText, CRect& rcText, UINT nFlags, BOOL bDraw, BOOL bSelected, BOOL bEnabled);

		XTPPaintTheme BaseTheme() { return xtpThemeOffice2000; }
	};

	class _XTP_EXT_CLASS CXTPOffice2003Theme : public CXTPOfficeTheme  
	{
	public:
		CXTPOffice2003Theme();
		virtual ~CXTPOffice2003Theme();

		static HBITMAP AFX_CDECL GetModernBitmap(UINT nIDResource);
		static BOOL AFX_CDECL LoadModernToolbarIcons(UINT nIDResource);
		static BOOL AFX_CDECL LoadModernToolbarIcons(UINT nIDResource, UINT* pCommands, int nCount, CSize szIcon = CSize(16, 15));
	 
	protected:
		virtual void RefreshMetrics();
		virtual void DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded = FALSE);
		virtual void FillDockBar(CDC* pDC, CXTPDockBar* pBar);
		virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);
		virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBar = TRUE, XTPBarPosition barPosition = xtpBarPopup);
		virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);
		virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw);
		virtual void DrawStatusBarGripper(CDC* pDC, CRect rcClient);
		virtual void DrawStatusBarPane(CDC* pDC, CRect& rc, DWORD dwStyle, CString str);

		XTPPaintTheme BaseTheme() { return xtpThemeOffice2003; }

		void CreateGradientCircle();

	public:
		
		HRESULT GetCurrentThemeName(
			LPWSTR pszThemeFileName, int cchMaxNameChars, 
			LPWSTR pszColorBuff, int cchMaxColorChars,
			LPWSTR pszSizeBuff, int cchMaxSizeChars);

		void ExcludeCorners(CDC* pDC, CRect rc);
		void DrawExpandSymbols(CDC* pDC, BOOL bVertical, CRect rc, BOOL bHiddenExists, COLORREF clr);

		void RectangleEx(CDC* pDC, CRect rc, int nColorBorder, BOOL bHoriz, COLORREF clrLight, COLORREF clrDark);

	public:
		COLORREF m_clrDockBarDark;
		COLORREF m_clrDockBarLight;

		COLORREF m_clrCommandBarDark;
		COLORREF m_clrCommandBarLight;

		COLORREF m_clrPopupDark;
		COLORREF m_clrPopupLight;

		COLORREF m_crlToolbarShadow;
		COLORREF m_crlFloatingGripper;

		COLORREF m_crlExpandNormalDark;
		COLORREF m_crlExpandNormalLight;

		COLORREF m_clrMenuExpandedDark;
		COLORREF m_clrMenuExpandedLight;

		COLORREF m_clrCircleDark;
		COLORREF m_clrCircleLight;

		COLORREF m_clrStatusPane;

		BOOL m_bLunaTheme;
		CXTPWinThemeWrapper m_themeStatusBar;

		CImageList m_ilGradientCircle;

	};

	class _XTP_EXT_CLASS CXTPNativeXPTheme : public CXTPDefaultTheme
	{
	public:
		CXTPNativeXPTheme();
		~CXTPNativeXPTheme();

	protected: 
		virtual void RefreshMetrics();

		virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);
		void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBar = TRUE, XTPBarPosition barPosition = xtpBarPopup);
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);
		void FillDockBar(CDC* pDC, CXTPDockBar* pBar);
		void FillStatusBar(CDC* pDC, CXTPStatusBar* pBar);		
		void _FillRect(CDC* pDC, CWnd* pWnd, CFrameWnd* pFrame);
		void AdjustExcludeRect(CRect& rc, CXTPControl* pControl);
		void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton);

		XTPPaintTheme BaseTheme() { return xtpThemeNativeWinXP; }

	protected:
		CXTPWinThemeWrapper m_themeRebar;
		CXTPWinThemeWrapper m_themeToolbar;
	};

};
using namespace XTPPaintThemes;
//:End Ignore

//////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTPPaintManager::SetAnimationDelay(int nAnimationSteps, int nAnimationTime) {
	m_nAnimationSteps = nAnimationSteps;
	m_nAnimationTime = nAnimationTime;
}


#endif // #if !defined(__XTPPAINTMANAGER_H__)