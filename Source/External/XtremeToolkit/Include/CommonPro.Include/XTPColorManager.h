// XTPColorManager.h: interface for the CXTPColorManager class.
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
#if !defined(__XTPCOLORMANAGER_H__)
#define __XTPCOLORMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore


// Summary: Enumeration used to determine the current Windows XP theme in use
enum XTPCurrentSystemTheme
{
	xtpSystemThemeUnknown, // No known theme in use
	xtpSystemThemeBlue,    // Blue theme in use
	xtpSystemThemeOlive,   // Olive theme in use
	xtpSystemThemeSilver,  // Silver theme in use
	xtpSystemThemeAuto	   // Use OS theme.
};

#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION     27
#endif

#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION   28
#endif

// XP base color.
const UINT XPCOLOR_BASE						= 30;
// XP toolbar background color.
const UINT XPCOLOR_TOOLBAR_FACE				= XPCOLOR_BASE;
// XP menu item selected color.
const UINT XPCOLOR_HIGHLIGHT				= 31;
// XP menu item selected border color.
const UINT XPCOLOR_HIGHLIGHT_BORDER			= 32;
// XP menu item pushed color.
const UINT XPCOLOR_HIGHLIGHT_PUSHED			= 33;
// XP menu item checked color.
const UINT XPCOLOR_HIGHLIGHT_CHECKED		= 36;
// XP menu item checked border color.
const UINT XPCOLOR_HIGHLIGHT_CHECKED_BORDER	= 37;
// XP menu item icon shadow.
const UINT XPCOLOR_ICONSHADDOW				= 34;
// XP menu item disabled text color.
const UINT XPCOLOR_GRAYTEXT					= 35;
// XP toolbar gripper color.
const UINT XPCOLOR_TOOLBAR_GRIPPER			= 38;
// XP toolbar separator color.
const UINT XPCOLOR_SEPARATOR				= 39;
// XP menu icon disabled color.
const UINT XPCOLOR_DISABLED					= 40;
// XP menu item text background color.
const UINT XPCOLOR_MENUBAR_FACE				= 41;
// XP hidden menu commands background color.
const UINT XPCOLOR_MENUBAR_EXPANDED			= 42;
// XP menu border color.
const UINT XPCOLOR_MENUBAR_BORDER			= 43;
// XP menu item text color.
const UINT XPCOLOR_MENUBAR_TEXT				= 44;
// XP menu item selected text color.
const UINT XPCOLOR_HIGHLIGHT_TEXT			= 45;
// XP toolbar text color.
const UINT XPCOLOR_TOOLBAR_TEXT				= 46;
// XP toolbar pushed text color.
const UINT XPCOLOR_PUSHED_TEXT				= 47;
// XP inactive tab background color.
const UINT XPCOLOR_TAB_INACTIVE_BACK		= 48;
// XP inactive tab text color.
const UINT XPCOLOR_TAB_INACTIVE_TEXT		= 49;
// XP border color for pushed in 3D elements.
const UINT XPCOLOR_HIGHLIGHT_PUSHED_BORDER	= 50;
// XP color for text displayed in a checked button.
const UINT XPCOLOR_CHECKED_TEXT				= XPCOLOR_HIGHLIGHT_TEXT;
// XP face color for three-dimensional display elements and for dialog box backgrounds.
const UINT XPCOLOR_3DFACE					= 51;
// XP shadow color for three-dimensional display elements (for edges facing away from the light source).
const UINT XPCOLOR_3DSHADOW					= 52;
// XP edit control border color.
const UINT XPCOLOR_EDITCTRLBORDER			= 53;
// XP frame color.
const UINT XPCOLOR_FRAME					= 54;
// XP splitter face color.
const UINT XPCOLOR_SPLITTER_FACE			= 55;
// XP splitter face color.
const UINT XPCOLOR_LABEL					= 56;
// XP last color.
const UINT XPCOLOR_LAST						= 56;

//===========================================================================
// Summary: Helper struct used by CXTPColorManager to display gradient color.
//===========================================================================
struct _XTP_EXT_CLASS XTP_GRADIENT_COLOR
{
	
	COLORREF clrLight;	// Light color.
	COLORREF clrDark;	// Dark color.

	//-----------------------------------------------------------------------
	// Summary: Call this method to set the gradient colors for the
	//          structure.
	// Input:   _clrLight - RGB light color value
	//          _clrDark  - RGB dark color value
	//-----------------------------------------------------------------------
	void SetColors(COLORREF _clrLight, COLORREF _clrDark);

	//-----------------------------------------------------------------------
	// Summary: Overloaded assignment operator sets the value of the dark and
	//          light colors equal to the specified COLORREF value.
	// Input:   clr - RGB color value
	//-----------------------------------------------------------------------
	const XTP_GRADIENT_COLOR& operator= (const COLORREF& clr);
};

//===========================================================================
// Summary: CXTPColorManager is a stand alone class.  It is used to create a 
//			CXTPColorManager object. This object is used for manipulation of 
//          RGB values.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorManager
{
public:

	//-----------------------------------------------------------------------
	// Summary: This function will refresh the display element colors defined
	//          in the CXTPColorManager object.
	// Input:   bInit - TRUE when called for initialization.
	//-----------------------------------------------------------------------
	void RefreshColors(BOOL bInit = FALSE);

	// BULLETED LIST:

	//-----------------------------------------------------------------------
	// Summary: This function retrieves the current color of the specified XP
	//          display element. Display elements are the parts of a window 
	//          and the display that appear on the system display screen.  
	//          The function returns the red, green, blue (RGB) color value 
	//          of the given element.
	// Input:	nIndex - Specifies the display element whose color is to be 
    //                   retrieved.  This parameter can be one of the 
    //                   following values:
    //                   [ul]
    //                   [li]<b>XPCOLOR_BASE</b> - XP base color.[/li]
    //                   [li]<b>XPCOLOR_TOOLBAR_FACE</b> - XP toolbar
    //                       background color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT</b> - XP menu item selected
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_BORDER</b> - XP menu item 
	//                       selected border color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_PUSHED</b> - XP menu item
	//                       pushed color.[/li]
    //                   [li]<b>XPCOLOR_ICONSHADDOW</b> - XP menu item icon
	//                       shaddow.[/li]
    //                   [li]<b>XPCOLOR_GRAYTEXT</b> - XP menu item disabled
	//                       text color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_CHECKED</b> - XP menu item
	//                       checked color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_CHECKED_BORDER</b> - An RGB
	//                       value that represents the XP menu item checked
	//                       border color.[/li]
    //                   [li]<b>XPCOLOR_TOOLBAR_GRIPPER</b> - XP toolbar
    //                       gripper color.[/li]
    //                   [li]<b>XPCOLOR_SEPARATOR</b> - XP toolbar separator
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_DISABLED</b> - XP menu icon disabled
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_MENUBAR_FACE</b> - XP menu item text
	//                       background color.[/li]
    //                   [li]<b>XPCOLOR_MENUBAR_EXPANDED</b> - XP hidden menu
	//                       commands background color.[/li]
    //                   [li]<b>XPCOLOR_MENUBAR_BORDER</b> - XP menu border
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_MENUBAR_TEXT</b> - XP menu item text
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_TEXT</b> - XP menu item
	//                       selected text color.[/li]
    //                   [li]<b>XPCOLOR_TOOLBAR_TEXT</b> - XP toolbar text
	//                       color.[/li]
    //                   [li]<b>XPCOLOR_PUSHED_TEXT</b> - XP toolbar pushed
	//                       text color.[/li]
    //                   [li]<b>XPCOLOR_TAB_INACTIVE_BACK</b> - XP inactive
	//                       tab background color.[/li]
    //                   [li]<b>XPCOLOR_TAB_INACTIVE_TEXT</b> - XP inactive
	//                       tab text color.[/li]
    //                   [li]<b>XPCOLOR_HIGHLIGHT_PUSHED_BORDER</b> - An RGB
	//                       value that represents the XP border color for
	//                       pushed in 3D elements.[/li]
    //                   [li]<b>XPCOLOR_CHECKED_TEXT</b> - XP color for text
	//                       displayed in a checked button.[/li]
    //                   [li]<b>XPCOLOR_3DFACE</b> - XP face color for three-
	//                       dimensional display elements and for dialog box 
	//                       backgrounds.[/li]
    //                   [li]<b>XPCOLOR_3DSHADOW</b> - XP shadow color for 
	//                       three-dimensional display elements (for edges 
	//                       facing away from the light source).[/li]
    //                   [/ul]
	//-----------------------------------------------------------------------
	COLORREF GetColor(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:	Call this member function to blend the light and dark colors
	//          by the amount specified in nDelta.
	// Input:   clrLight - RGB light color
	//          clrDark  - RGB dark color
	//          nDelta   - Amount saturation to blend colors.
	// Returns: An RGB color value representing the two blended colors.
	//-----------------------------------------------------------------------
	COLORREF LightColor(COLORREF clrLight, COLORREF clrDark, int nDelta);
	

	//-----------------------------------------------------------------------
	// Summary: Call this member function to disable the luna (Blue, Olive 
	//          and Green) colors for Office 2003 theme for Windows XP and 
	//          later operating systems.
	// Input:   bDisable - TRUE to disable luna colors, otherwise FALSE.
	//-----------------------------------------------------------------------
	void DisableLunaColors(BOOL bDisable = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Determines if Luna colors for Office 2003 style is disabled.
	// Returns: TRUE if the luna colors are disabled, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsLunaColorsDisabled();

	//////////////////////////////////////////////////////////////////////
	// XP Color Functions
	//////////////////////////////////////////////////////////////////////  

	XTP_GRADIENT_COLOR grcCaption;				// Caption gradient color.
	XTP_GRADIENT_COLOR grcDockBar;				// Dockbar gradient color.
	XTP_GRADIENT_COLOR grcShortcutBarGripper;	// Shortcut bar gripper.

	//-----------------------------------------------------------------------
	// Summary:	Call this member to set the color for the specified index.  
	//          See GetColor for a complete list of index values.
	// Input:   nIndex - Color index.
	//          RGB color value to set.
	//-----------------------------------------------------------------------
	void SetColor(int nIndex, COLORREF clrValue);
	
	//-----------------------------------------------------------------------
	// Summary:	Call this member to set the colors for each element to the
	//          color in the specified color array.
	// Input:	cElements    - Number of elements in array.
	//			lpaElements  - Array of elements.
	//			lpaRgbValues - Array of RGB values.
	//-----------------------------------------------------------------------
	void SetColors(int cElements, CONST INT *lpaElements, CONST COLORREF* lpaRgbValues);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine the current Windows XP
	//          theme in use.
	// Returns: A XTPCurrentSystemTheme enumeration that represents the 
	//          current Windows theme in use, can be one of the following 
	//          values:
	//          xtpSystemThemeUnknown - No known theme in use
	//          xtpSystemThemeBlue    - Blue theme in use
	//          xtpSystemThemeOlive   - Olive theme in use
	//          xtpSystemThemeSilver  - Silver theme in use
	//-----------------------------------------------------------------------
	XTPCurrentSystemTheme GetCurrentSystemTheme();

	// Inputs: systemTheme:
	//          xtpSystemThemeUnknown - To disable luna theme
	//          xtpSystemThemeBlue    - to use Blue theme
	//          xtpSystemThemeOlive   - to use Olive theme
	//          xtpSystemThemeSilver  - to use Silver theme
	// Summary: Call this method to force system theme.
	void SetLunaTheme(XTPCurrentSystemTheme systemTheme);

private:

	BOOL     m_bInit;
	BOOL     m_bDisableLunaColors;
	COLORREF m_arrColor[XPCOLOR_LAST + 1];

	XTPCurrentSystemTheme m_systemTheme;
	
	CXTPColorManager();
	void RefreshSysColors();
	void RefreshXPColors();
	void RefreshGradientColors();
	float ColorWidth(int nLength, int nWidth);
	float ColorDelta(COLORREF clrA, COLORREF clrB);
	float Length(COLORREF clrA, COLORREF clrB);
	BOOL LongColor(COLORREF clrMain, COLORREF clrSub, BOOL bCalcLength, float fDistance);
	COLORREF MixColor(COLORREF clrMain, COLORREF clrSub, float fDistance);
	COLORREF AdjustColor(COLORREF clrMain, COLORREF clrSub, float fDistance);
	double GetRDelta(COLORREF clr); 
	double GetGDelta(COLORREF clr); 
	double GetBDelta(COLORREF clr); 

	// singleton, instantiate on demand.
    static CXTPColorManager& AFX_CDECL Instance();

	friend CXTPColorManager* XTPColorManager();
};

//---------------------------------------------------------------------------
// Summary:	Call this member function to access CXTPColorManager members. 
//          Since this class is designed as a single instance object you can 
//          only access version info thru this method. You <b>cannot</b> 
//          directly instantiate an object of type CXTPColorManager.
// Example: <pre>COLORREF cr = XTPColorManager()->GetColor(nIndex);</pre>
//---------------------------------------------------------------------------
AFX_INLINE CXTPColorManager* XTPColorManager() { 
	return &CXTPColorManager::Instance(); 
}
//---------------------------------------------------------------------------
// Summary:	Call this member as a convenient way to call
//          XTPColorManager()->GetColor, see CXTPColorManager::GetColor for
//          more details.
// Input:	nIndex - Specifies the display element whose color is to be 
//                   retrieved.
//---------------------------------------------------------------------------
AFX_INLINE COLORREF GetXtremeColor(UINT nIndex) {
	return XTPColorManager()->GetColor(nIndex);
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPCOLORMANAGER_H__)
//:End Ignore