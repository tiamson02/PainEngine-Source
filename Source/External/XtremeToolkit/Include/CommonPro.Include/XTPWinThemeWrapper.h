// XTPWinThemeWrapper.h: interface for the CXTPWinThemeWrapper class.
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

#if !defined(__XTPWINTHEMEWRAPPER_H__)
#define __XTPWINTHEMEWRAPPER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "uxtheme.h"
#include "schemadef.h"
#include "tmschema.h"


//===========================================================================
// Summary: The CXTPWinThemeWrapper class wraps the UxTheme.dll visual theme 
//          API.  You can use this class for rendering Windows XP themes for 
//          owner drawn controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPWinThemeWrapper
{
public:

	//-----------------------------------------------------------------------
    // Summary: Constructs a CXTPWinThemeWrapper object.
	//-----------------------------------------------------------------------
	CXTPWinThemeWrapper();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTPWinThemeWrapper object, handles cleanup and 
	//          de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPWinThemeWrapper();

	//-----------------------------------------------------------------------
	// Summary: Open the theme data for the specified HWND and semi-colon 
	//          separated list of class names.  OpenThemeData() will try 
	//          each class name, one at a time, and use the first matching 
	//          theme info found.
	// Input:   hWnd         - Window handle of the control/window to be 
	//                         themed.
	//          pszClassList - Class name (or list of names) to match to 
	//                         theme data section.  if the list contains 
	//                         more than one name, the names are tested one 
	//                         at a time for a match.  If a match is found,
	//                         OpenThemeData() returns a theme handle 
	//                         associated with the matching class.  This 
	//                         param is a list (instead of just a single 
	//                         class name) to provide the class an 
	//                         opportunity to get the "best" match between 
	//                         the class and the current theme.  For example,
	//                         a button might pass L"OkButton, Button" if its
	//                         ID=ID_OK.  If the current theme has an entry 
	//                         for OkButton, that will be used.  Otherwise,
	//                         we fall back on the normal Button entry.
	//-----------------------------------------------------------------------
	void OpenThemeData(HWND hWnd, LPCWSTR pszClassList);

	//-----------------------------------------------------------------------
	// Summary: Closes the theme data handle.  CXTPWinThemeWrapper will do 
	//          this automatically when destroyed or when OpenThemeData is 
	//          called.  Your window should handle the WM_THEMECHANGED 
	//          message when called followed by a call to OpenThemeData.
	//-----------------------------------------------------------------------
	void CloseThemeData();

	//-----------------------------------------------------------------------
	// Summary: Draws the theme-specified border and fill for the "iPartId" 
	//          and "iStateId".  This could be based on a bitmap file, a 
	//          border and fill, or other image description.
	// Input:   hDC       - HDC to draw into
	//          iPartID   - Part number to draw
	//          iStateID  - State number (of the part) to draw
	//          pRect     - Defines the size/location of the part
	//          pClipRect - Optional clipping rect (don't draw outside it)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeBackground(HDC hDC, int iPartID, int iStateID, const RECT *pRect, const RECT *pClipRect);

	//-----------------------------------------------------------------------
	// Summary: Draws the text using the theme-specified color and font for 
	//          the "iPartID" and "iStateID".  
	// Input:   hDC          - HDC to draw into
	//          iPartID      - Part number to draw
	//          iStateID     - State number (of the part) to draw
	//          pszText      - Actual text to draw
	//          dwCharCount  - Number of chars to draw (-1 for all)
	//          dwTextFlags  - Same as DrawText() "uFormat" param
	//          dwTextFlags2 - Additional drawing options 
	//          pRect        - Defines the size/location of the part
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeText(HDC hDC, int iPartID, int iStateID, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);

	//-----------------------------------------------------------------------
	// Summary: Gets the size of the content for the theme-defined 
	//          background.  This is usually the area inside the borders or 
	//          Margins.  
	// Input:   hDC           - (optional) device content to be used for 
	//                          drawing
	//          iPartID       - Part number to draw
	//          iStateID      - State number (of the part) to draw
	//          pBoundingRect - The outer RECT of the part being drawn
	//          pContentRect  - RECT to receive the content area
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundContentRect(HDC hDC, int iPartID, int iStateID, const RECT *pBoundingRect, RECT *pContentRect);

	//-----------------------------------------------------------------------
	// Summary: Calculates the size/location of the theme-specified 
	//          background based on the "pContentRect".
	// Input:   hDC           - (optional) device content to be used for 
	//                          drawing
	//          iPartID       - Part number to draw
	//          iStateID      - State number (of the part) to draw
	//          pContentRect  - RECT that defines the content area
	//          pBoundingRect - RECT to receive the overall size/location of 
	//                          part
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundExtent(HDC hDC, int iPartID, int iStateID, const RECT *pContentRect, RECT *pExtentRect);

	//-----------------------------------------------------------------------
	// Summary: Draws the theme-specified border and fill for the "iPartId" 
	//          and "iStateId".  This could be based on a bitmap file, a 
	//          border and fill, or other image description.
	// Input:   hDC      - HDC to draw into
	//          iPartID  - Part number to draw
	//          iStateID - State number (of the part) to draw
	//          pRect    - Defines the size/location of the part
	//          pOptions - Ptr to optional params
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeBackgroundEx(HDC hDC, int iPartID, int iStateID, const RECT *pRect, const DTBGOPTS *pOptions);
	
	//-----------------------------------------------------------------------
	// Summary: Similar to the DrawEdge() API, but uses part colors and is 
	//          high-DPI aware
	// Input:   hDC          - HDC to draw into
	//          iPartID      - Part number to draw.
	//          iStateID     - State number of part
	//          pDestRect    - The RECT used to draw the line(s)
	//          uEdge        - Same as DrawEdge() API
	//          uFlags       - Same as DrawEdge() API
	//          pContentRect - Receives the interior rect if (uFlags & 
	//                         BF_ADJUST)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeEdge(HDC hDC, int iPartID, int iStateID, const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT *pContentRect);
	
	//-----------------------------------------------------------------------
	// Summary: Draws an image within an imagelist based on a (possible) 
	//          theme-defined effect. 
	// Input:   hDC          - HDC to draw into
	//          iPartID      - Part number to draw.
	//          iStateID     - State number of part
	//          pRect        - The RECT to draw the image within
	//          himl         - Handle to IMAGELIST 
	//          iImageIndex  - Index into IMAGELIST (which icon to draw)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeIcon(HDC hDC, int iPartID, int iStateID, const RECT *pRect, HIMAGELIST himl, int iImageIndex);
	
	//-----------------------------------------------------------------------
	// Summary: Used by partially-transparent or alpha-blended child controls
	//          to draw the part of their parent that they appear in front of.
	// Input:   hWnd  - Handle of the child control
	//          hDC   - HDC of the child control
	//          pRect - (optional) rect that defines the area to be 
	//                  Drawn (CHILD coordinates)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeParentBackground(HWND hWnd, HDC hDC, RECT *pRect);
	
	//-----------------------------------------------------------------------
	// Summary: Enables/disables dialog background theme.  This method can be
	//          used to tailor dialog compatibility with child windows and 
	//          controls that may or may not coordinate the rendering of 
	//          their client area backgrounds with that of their parent  
	//          dialog in a manner that supports seamless background 
	//          texturing.
	// Input:   hWnd    - The window handle of the target dialog
	//          dwFlags - ETDT_ENABLE to enable the theme-defined dialog
	//                    background texturing,
	//                    ETDT_DISABLE to disable background texturing,
	//                    ETDT_ENABLETAB to enable the theme-defined 
	//                    background texturing using the Tab texture
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags);
	
	//-----------------------------------------------------------------------
	// Summary: Enables or disables themeing for the current user in the 
	//          current and future sessions.
	// Input:   bEnable - If FALSE, disable theming & turn themes off.
	//                    if TRUE, enable themeing and, if user 
	//                    previously had a theme active, make it active 
	//                    now.
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT EnableTheming(BOOL bEnable);
	
	//-----------------------------------------------------------------------
	// Summary: Get the name of the current theme in-use.  Optionally, return
	//          the ColorScheme name and the Size name of the theme.
	// Input:   pszThemeFileName - Receives the theme path & filename
	//          cchMaxNameChars  - Max chars allowed in pszNameBuff
	//          pszColorBuff     - (optional) receives the canonical color 
	//                             scheme name
	//                             (not the display name)
	//          cchMaxColorChars - Max chars allowed in pszColorBuff
	//          pszSizeBuff      - (optional) receives the canonical size 
	//                             name (not the display name)
	//          cchMaxSizeChars  - Max chars allowed in pszSizeBuff
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetCurrentThemeName(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars);
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves the property flags that control how visual styles 
	//          are applied in the current application.
	// Returns: The following return values are bit flags combined with a 
	//          logical OR operator.
	//          STAP_ALLOW_NONCLIENT  - Specifies that the nonclient areas of 
	//                                  application windows have visual 
	//                                  styles applied. 
	//          STAP_ALLOW_CONTROLS   - Specifies that controls in 
	//                                  application windows have visual 
	//                                  styles applied. 
	//          STAP_ALLOW_WEBCONTENT - Specifies that all web content 
	//                                  displayed in an application is 
	//                                  rendered using visual styles. 
	//-----------------------------------------------------------------------
	DWORD ThemeAppProperties();
	
	//-----------------------------------------------------------------------
	// Summary: Computes the region for a regular or partially transparent 
	//          theme-specified background that is bound by the specified 
	//          "pRect".  If the rectangle is empty, sets the hRgn to NULL 
	//          and return S_FALSE.
	// Input:   hDC      - Optional hDC to draw into (DPI scaling)
	//          iPartID  - Part number to draw.
	//          iStateID - State number (of the part) 
	//          pRect    - The RECT used to draw the part
	//          pRegion  - Receives handle to calculated region
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundRegion(HDC hDC, int iPartID, int iStateID, const RECT *pRect, HRGN *pRegion);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified BOOL property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pfVal    - Receives the value of the property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBool(int iPartID, int iStateID, int iPropID, BOOL *pfVal);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified COLOR property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pColor   - Receives the value of the property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeColor(int iPartID, int iStateID, int iPropID, COLORREF *pColor);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified property name from the 
	//          [documentation] section of the themes.ini file for the 
	//          specified theme.  If the property has been localized in the 
	//          theme files string table, the localized version of the 
	//          property value is returned.
	// Input:   pszThemeFileName - Filename of the theme file to query
	//          pszPropertyName  - Name of the string property to retrieve a 
	//                             value for
	//          pszValueBuff     - Receives the property string value
	//          cchMaxValChars   - Max chars allowed in pszValueBuff
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeDocumentationProperty(LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified ENUM property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          piVal    - Receives the value of the enum (cast to int*)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeEnumValue(int iPartID, int iStateID, int iPropID, int *piVal);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified FILENAME property.
	// Input:   iPartID          - Part number 
	//          iStateID         - State number of part
	//          iPropID          - The property number to search for
	//          pszThemeFileName - Output buffer to receive the filename
	//          cchMaxBuffChars  - The size of the return buffer, in chars
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeFilename(int iPartID, int iStateID, int iPropID, LPWSTR pszThemeFilename, int cchMaxBuffChars);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified font property
	// Input:   hDC      - (optional) hDC to be drawn to (DPI scaling)
	//          iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pFont    - Receives the value of the LOGFONT property
	//                     (scaled for the current logical screen dpi) 
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeFont(HDC hDC, int iPartID, int iStateID, int iPropID, LOGFONT *pFont);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified int property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          piVal    - Receives the value of the property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeInt(int iPartID, int iStateID, int iPropID, int *piVal);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified INTLIST struct
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pIntList - Receives the value of the INTLIST property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeIntList(int iPartID, int iStateID, int iPropID, INTLIST *pIntList);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified MARGINS property
	// Input:   hDC      - (optional) hDC to be used for drawing
	//          iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pRect    - RECT for area to be drawn into
	//          pMargins - Receives the value of the MARGINS property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeMargins(HDC hDC, int iPartID, int iStateID, int iPropID, RECT *pRect, MARGINS *pMargins);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified metric/size property
	// Input:   hDC      - (optional) hDC to be drawn into (DPI scaling)
	//          iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          piVal    - Receives the value of the property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeMetric(HDC hDC, int iPartID, int iStateID, int iPropID, int *piVal);
	
	//-----------------------------------------------------------------------
	// Summary: Returns the specified size of the theme part
	// Input:   hDC      - HDC to select font into & measure against
	//          iPartID  - Part number to retrieve size for
	//          iStateID - State number (of the part) 
	//          pRect    - (optional) rect for part drawing destination
	//          eSize    - The type of size to be retrieved
	//          pSize    - Receives the specified size of the part 
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePartSize(HDC hDC, int iPartID, int iStateID, RECT *pRect, THEMESIZE eSize, SIZE *pSize);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified position property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pPoint   - Receives the value of the position property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePosition(int iPartID, int iStateID, int iPropID, POINT *pPoint);
	
	//-----------------------------------------------------------------------
	// Summary: Searches for the specified theme property and sets "pOrigin" 
	//          to indicate where it was found (or not found)
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to search for
	//          pOrigin  - Receives the value of the property origin
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePropertyOrigin(int iPartID, int iStateID, int iPropID, PROPERTYORIGIN *pOrigin);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified RECT property
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	//          iPropID  - The property number to get the value for
	//          pRect    - Receives the value of the RECT property
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeRect(int iPartID, int iStateID, int iPropID, RECT *pRect);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value for the specified string property
	// Input:   iPartID         - Part number 
	//          iStateID        - State number of part
	//          iPropID         - The property number to get the value for
	//          pszBuff         - Receives the string property value
	//          cchMaxBuffChars - Max. number of chars allowed in pszBuff
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeString(int iPartID, int iStateID, int iPropID, LPWSTR pszBuff, int cchMaxBuffChars);
	
	//-----------------------------------------------------------------------
	// Summary: Get the boolean value of specified System metric. 
	// Input:   iBoolId - The TMT_XXX BOOL number (first BOOL is 
	//                    TMT_FLATMENUS)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	BOOL GetThemeSysBool(int iBoolID);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value of the specified System color. 
	// Input:   iColorId - The system color index defined in winuser.h
	// Returns: The value of the specified system color.
	//-----------------------------------------------------------------------
	COLORREF GetThemeSysColor(int iColorID);
	
	//-----------------------------------------------------------------------
	// Summary: Get the brush for the specified System color. 
	// Input:   iColorId - The system color index defined in winuser.h
	// Returns: Handle to brush data.
	//-----------------------------------------------------------------------
	HBRUSH GetThemeSysColorBrush(int iColorID);
	
	//-----------------------------------------------------------------------
	// Summary: Get the LOGFONT for the specified System font. 
	// Input:   iFontId - The TMT_XXX font number (first font
	//                    is TMT_CAPTIONFONT)
	//          pLF     - Ptr to LOGFONT to receive the font value.
	//                    (scaled for the current logical screen dpi) 
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysFont(int iFontID, LOGFONT *pLF);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value of specified System int.
	// Input:   iIntId  - Must be one of the following values:
	//                    TMT_DPIX
	//                    TMT_DPIY
	//                    TMT_MINCOLORDEPTH
	//          piValue - Ptr to int to receive value
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysInt(int iIntID, int *piValue);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value of the specified System size metric.  (scaled 
	//          for the current logical screen dpi) 
	// Input:   iSizeId - The following values are supported:
	//                    SM_CXBORDER   (border width)
	//                    SM_CXVSCROLL  (scrollbar width)
	//                    SM_CYHSCROLL  (scrollbar height)
	//                    SM_CXSIZE     (caption width)
	//                    SM_CYSIZE     (caption height)
	//                    SM_CXSMSIZE   (small caption width)
	//                    SM_CYSMSIZE   (small caption height)
	//                    SM_CXMENUSIZE (menubar width)
	//                    SM_CYMENUSIZE (menubar height)
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	int GetThemeSysSize(int iSizeID);
	
	//-----------------------------------------------------------------------
	// Summary: Get the value of specified System string metric. 
	// Input:   iStringId         - Must be one of the following values:
	//                              TMT_CSSNAME
	//                              TMT_XMLNAME
	//          pszStringBuff     - The buffer to receive the string value
	//          cchMaxStringChars - Max. number of chars that pszStringBuff 
	//                              can hold
	// Returns: The size in dots per inch (DPI) scaled for the current 
	//          logical screen.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysString(int iStringID, LPWSTR pszStringBuff, int cchMaxStringChars);
	
	//-----------------------------------------------------------------------
	// Summary: Calculates the size/location of the specified text when 
	//          rendered in the Theme Font. 
	// Input:   hDC             - HDC to select font & measure into
	//          iPartID         - Part number to draw.
	//          iStateID        - State number (of the part) 
	//          pszText         - The text to be measured
	//          dwCharCount     - Number of chars to draw (-1 for all)
	//          dwTextFlags     - Same as DrawText() "uFormat" param
	//          pszBoundingRect - Optional: to control layout of text
	//          pszExtentRect   - Receives the RECT for text size/location
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeTextExtent(HDC hDC, int iPartID, int iStateID, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect);
	
	//-----------------------------------------------------------------------
	// Summary: Returns info about the theme-specified font for the part/
	//          state passed in.
	// Input:   hDC      - Optional: hDC for screen context
	//          iPartID  - Part number to draw.
	//          iStateID - State number (of the part) 
	//          ptm      - Receives the font info
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeTextMetrics(HDC hDC, int iPartID, int iStateID, TEXTMETRIC *ptm);
	
	//-----------------------------------------------------------------------
	// Summary: If window is themed, returns its most recent HTHEME from 
	//          OpenThemeData() - Otherwise, returns NULL.
	// Input:   hWnd - The window to get the HTHEME of
	// Returns: The most recent theme handle from OpenThemeData.
	//-----------------------------------------------------------------------
	HTHEME GetWindowTheme(HWND hWnd); 
	
	//-----------------------------------------------------------------------
	// Summary: Returns a HitTestCode (a subset of the values returned by 
	//          WM_NCHITTEST) for the point "ptTest" within the theme-
	//          specified background (bound by pRect).  "pRect" and "ptTest" 
	//          should both be in the same coordinate system (client, screen,
	//          etc).
	// Input:   hDC           - HDC to draw into
	//          iPartID       - Part number to test against
	//          iStateID      - State number (of the part) 
	//          pRect         - The RECT used to draw the part
	//          hRgn          - Optional region to use; must be in same 
	//                          coordinates as
	//                          pRect and pTest.
	//          ptTest        - The hit point to be tested
	//          dwOptions     - HTTB_xxx constants
	//          pwHitTestCode - Receives the returned hit test code - One of:
	//                          HTNOWHERE, HTLEFT, HTTOPLEFT, HTBOTTOMLEFT,
	//                          HTRIGHT, HTTOPRIGHT, HTBOTTOMRIGHT,
	//                          HTTOP, HTBOTTOM, HTCLIENT
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT HitTestThemeBackground(HDC hDC, int iPartID, int iStateID, DWORD dwOptions, const RECT *pRect, HRGN hRgn, POINT ptTest, WORD *pwHitTestCode); 
	
	//-----------------------------------------------------------------------
	// Summary: Reports whether the current application's user interface 
	//          displays using visual styles.
	// Returns: Returns one of the following values.
	//          TRUE  - The application has a visual style applied. 
	//          FALSE - The application does not have a visual style applied. 
	//-----------------------------------------------------------------------
	BOOL IsAppThemed();
	
	//-----------------------------------------------------------------------
	// Summary: Can be used to test if a system theme is active for the 
	//          current user session.  
	//          use the API "IsAppThemed()" to test if a theme is active for 
	//          the calling process.
	// Returns: Returns one of the following values:
	//          TRUE  - A visual style is enabled, and windows with visual 
	//                  styles applied should call OpenThemeData to start 
	//                  using theme drawing services. 
	//          FALSE - A visual style is not enabled, and the window message 
	//                  handler does not need to make another call to 
	//                  IsThemeActive until it receives a WM_THEMECHANGED 
	//                  message.
	//-----------------------------------------------------------------------
	BOOL IsThemeActive();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the application should
	//          display Windows XP themes.  IsAppThemeReady() will first check
	//          to see if the operating system supports Windows XP theme display,
	//          then checks to see if the application has any controls that are
	//          able to display Windows XP themes, then finally checks to see if
	//          the application has included a manifest file.
	// Returns: TRUE if the application can display Windows XP themes, otherwise FALSE. 
	//-----------------------------------------------------------------------
	BOOL IsAppThemeReady();

	//-----------------------------------------------------------------------
	// Summary: Returns TRUE if the theme specified background for the part/
	//          state has transparent pieces or alpha-blended pieces.
	// Input:   iPartID  - Part number 
	//          iStateID - State number of part
	// Returns: Returns one of the following values.
	//          TRUE  - The theme-specified background for a particular 
	//                  iPartId and iStateId has transparent pieces or alpha-
	//                  blended pieces. 
	//          FALSE - The theme-specified background for a particular 
	//                  iPartId and iStateId does not have transparent pieces
	//                  or alpha-blended pieces. 
	//-----------------------------------------------------------------------
	BOOL IsThemeBackgroundPartiallyTransparent(int iPartID, int iStateID);
	
	//-----------------------------------------------------------------------
	// Summary: Reports whether the dialog supports background texturing.
	// Returns: Returns one of the following values.
	//          TRUE  - Background texturing is supported. 
	//          FALSE - Background texturing is not supported. 
	//-----------------------------------------------------------------------
	BOOL IsThemeDialogTextureEnabled();
	
	//-----------------------------------------------------------------------
	// Summary: Returns TRUE if the theme has defined parameters for the 
	//          specified "iPartID" and "iStateID".
	// Input:   iPartID  - Part number to find definition for
	//          iStateID - State number of part
	// Returns: Returns one of the following values.
	//          TRUE  - The theme has defined parameters for the specified 
	//                  iPartId and iStateId 
	//          FALSE - The theme does not have defined parameters for the 
	//                  specified iPartId and iStateId 
	//-----------------------------------------------------------------------
	BOOL IsThemePartDefined(int iPartID, int iStateID);
	
	//-----------------------------------------------------------------------
	// Summary: Sets the flags that control theming within the app
	// Input:   dwFlags - The flag values to be set
	//-----------------------------------------------------------------------
	void SetThemeAppProperties(DWORD dwFlags);
	
	//-----------------------------------------------------------------------
	// Summary: Redirects an existing Window to use a different section of 
	//          the current theme information than its class normally asks 
	//          for.
	// Input:   hWnd          - The handle of the window (cannot be NULL)
	//          pszSubAppName - App (group) name to use in place of the 
	//                          calling app's name.  If NULL, the actual 
	//                          calling app name will be used.
	//          pszSubIdList  - Semicolon separated list of class Id names to 
	//                          use in place of actual list passed by the 
	//                          window's class.  if NULL, the id list from 
	//                          the calling class is used.
	// Remarks: The Theme Manager will remember the "pszSubAppName" and the
	//          "pszSubIdList" associations thru the lifetime of the window 
	//          (even if themes are subsequently changed).  The window is 
	//          sent a "WM_THEMECHANGED" msg at the end of this call, so that
	//          the new theme can be found and applied.
	//          When "pszSubAppName" or "pszSubIdList" are NULL, the Theme 
	//          Manager removes the previously remember association.  To turn
	//          off theme-ing for the specified window, you can pass an empty
	//          string (L"") so it won't match any section entries.
	// Returns: Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT SetWindowTheme(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if OpenThemeData() was
	//          called to open theme information for a particular window.
	// Returns: TRUE if theme information was loaded, otherwise FALSE. 
	//-----------------------------------------------------------------------
	BOOL ThemeDataOpen() const;

	//-----------------------------------------------------------------------
	// Input:   hModule - Handle to the dll to retrieve version information for.
	// Summary:	Use this member function to return the version number of the
	//			dll specified by hModule. The high-order word of the return 
	//          value represents the major version number and the low-order 
	//          word of the returned value represents the minor version number.
	// Returns: A DWORD value if successful, otherwise 0L.
	//-----------------------------------------------------------------------
	DWORD GetModuleVersion(HMODULE hModule);

	//:Ignore
	static BOOL m_bDllDetached;
	//:End Ignore

	//:Ignore - for compatibility with older versions.
	void CloseTheme() {
		CloseThemeData();
	}
	void OpenTheme(HWND hWnd, LPCWSTR pszClassList) {
		OpenThemeData(hWnd, pszClassList);
	}
	void DrawBackground(HDC hdc, int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect) {
		DrawThemeBackground(hdc, iPartId, iStateId, pRect, pClipRect);
	}
	//:End Ignore

private:

	HMODULE m_hThemeDll;  // Handle to the theme dll.
	HMODULE m_hComCtlDll; // Handle to the common control dll.
	HTHEME  m_hTheme;     // Handle to the theme data.

	//-----------------------------------------------------------------------
	// Function pointers:
	//-----------------------------------------------------------------------

	// SetWindowTheme()
	typedef HRESULT (__stdcall *PFNSETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);
	PFNSETWINDOWTHEME m_pfnSetWindowTheme;

	// SetThemeAppProperties()
	typedef void (__stdcall *PFNSETTHEMEAPPPROPERTIES)(DWORD);
	PFNSETTHEMEAPPPROPERTIES m_pfnSetThemeAppProperties;
	
	// OpenThemeData()
	typedef HTHEME (__stdcall *PFNOPENTHEMEDATA)(HWND, LPCWSTR);
	PFNOPENTHEMEDATA m_pfnOpenThemeData;
	
	// IsThemePartDefined()
	typedef BOOL (__stdcall *PFNISTHEMEPARTDEFINED)(HTHEME, int, int);
	PFNISTHEMEPARTDEFINED m_pfnIsThemePartDefined;
	
	// IsThemeDialogTextureEnabled()
	typedef BOOL (__stdcall *PFNISTHEMEDIALOGTEXTUREENABLED)();
	PFNISTHEMEDIALOGTEXTUREENABLED m_pfnIsThemeDialogTextureEnabled;
	
	// IsThemeBackgroundPartiallyTransparent()
	typedef BOOL (__stdcall *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME, int, int);
	PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT m_pfnIsThemeBackgroundPartiallyTransparent;
	
	// IsThemeActive()
	typedef BOOL (__stdcall *PFNISTHEMEACTIVE)();
	PFNISTHEMEACTIVE m_pfnIsThemeActive;
	
	// IsAppThemed()
	typedef BOOL (__stdcall *PFNISAPPTHEMED)();
	PFNISAPPTHEMED m_pfnIsAppThemed;
	
	// HitTestThemeBackground()
	typedef HRESULT (__stdcall *PFNHITTESTTHEMEBACKGROUND)(HTHEME, HDC, int, int, DWORD, const RECT*, HRGN, POINT, WORD*);
	PFNHITTESTTHEMEBACKGROUND m_pfnHitTestThemeBackground;
	
	// GetWindowTheme()
	typedef HTHEME (__stdcall *PFNGETWINDOWTHEME)(HWND);
	PFNGETWINDOWTHEME m_pfnGetWindowTheme;
	
	// GetThemeTextMetrics()
	typedef HRESULT (__stdcall *PFNGETTHEMETEXTMETRICS)(HTHEME, HDC, int, int, TEXTMETRIC*);
	PFNGETTHEMETEXTMETRICS m_pfnGetThemeTextMetrics;
	
	// GetThemeTextExtent()
	typedef HRESULT (__stdcall *PFNGETTHEMETEXTEXTENT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, const RECT*, RECT*);
	PFNGETTHEMETEXTEXTENT m_pfnGetThemeTextExtent;
	
	// GetThemeSysString()
	typedef HRESULT (__stdcall *PFNGETTHEMESYSSTRING)(HTHEME, int, LPWSTR, int);
	PFNGETTHEMESYSSTRING m_pfnGetThemeSysString;
	
	// GetThemeSysSize()
	typedef int (__stdcall *PFNGETTHEMESYSSIZE)(HTHEME, int);
	PFNGETTHEMESYSSIZE m_pfnGetThemeSysSize;
	
	// GetThemeSysInt()
	typedef HRESULT (__stdcall *PFNGETTHEMESYSINT)(HTHEME, int, int*);
	PFNGETTHEMESYSINT m_pfnGetThemeSysInt;
	
	// GetThemeSysFont()
	typedef HRESULT (__stdcall *PFNGETTHEMESYSFONT)(HTHEME, int, LOGFONT*);
	PFNGETTHEMESYSFONT m_pfnGetThemeSysFont;
	
	// GetThemeSysColorBrush()
	typedef HBRUSH (__stdcall *PFNGETTHEMESYSCOLORBRUSH)(HTHEME, int);
	PFNGETTHEMESYSCOLORBRUSH m_pfnGetThemeSysColorBrush;
	
	// GetThemeSysColor()
	typedef COLORREF (__stdcall *PFNGETTHEMESYSCOLOR)(HTHEME, int);
	PFNGETTHEMESYSCOLOR m_pfnGetThemeSysColor;
	
	// GetThemeSysBool()
	typedef BOOL (__stdcall *PFNGETTHEMESYSBOOL)(HTHEME, int);
	PFNGETTHEMESYSBOOL m_pfnGetThemeSysBool;
	
	// GetThemeString()
	typedef HRESULT (__stdcall *PFNGETTHEMESTRING)(HTHEME, int, int, int, LPWSTR, int);
	PFNGETTHEMESTRING m_pfnGetThemeString;
	
	// GetThemeRect()
	typedef HRESULT (__stdcall *PFNGETTHEMERECT)(HTHEME, int, int, int, RECT*);
	PFNGETTHEMERECT m_pfnGetThemeRect;
	
	// GetThemePropertyOrigin()
	typedef HRESULT (__stdcall *PFNGETTHEMEPROPERTYORIGIN)(HTHEME, int, int, int, PROPERTYORIGIN*);
	PFNGETTHEMEPROPERTYORIGIN m_pfnGetThemePropertyOrigin;
	
	// GetThemePosition()
	typedef HRESULT (__stdcall *PFNGETTHEMEPOSITION)(HTHEME, int, int, int, POINT*);
	PFNGETTHEMEPOSITION m_pfnGetThemePosition;
	
	// GetThemePartSize()
	typedef HRESULT (__stdcall *PFNGETTHEMEPARTSIZE)(HTHEME, HDC, int, int, RECT*, THEMESIZE, SIZE*);
	PFNGETTHEMEPARTSIZE m_pfnGetThemePartSize;
	
	// GetThemeMetric()
	typedef HRESULT (__stdcall *PFNGETTHEMEMETRIC)(HTHEME, HDC, int, int, int, int*);
	PFNGETTHEMEMETRIC m_pfnGetThemeMetric;
	
	// GetThemeMargins()
	typedef HRESULT (__stdcall *PFNGETTHEMEMARGINS)(HTHEME, HDC, int, int, int, RECT*, MARGINS*);
	PFNGETTHEMEMARGINS m_pfnGetThemeMargins;
	
	// GetThemeIntList()
	typedef HRESULT (__stdcall *PFNGETTHEMEINTLIST)(HTHEME, int, int, int, INTLIST*);
	PFNGETTHEMEINTLIST m_pfnGetThemeIntList;
	
	// GetThemeInt()
	typedef HRESULT (__stdcall *PFNGETTHEMEINT)(HTHEME, int, int, int, int*);
	PFNGETTHEMEINT m_pfnGetThemeInt;
	
	// GetThemeFont()
	typedef HRESULT (__stdcall *PFNGETTHEMEFONT)(HTHEME, HDC, int, int, int, LOGFONT*);
	PFNGETTHEMEFONT m_pfnGetThemeFont;
	
	// GetThemeFilename()
	typedef HRESULT (__stdcall *PFNGETTHEMEFILENAME)(HTHEME, int, int, int, LPWSTR, int);
	PFNGETTHEMEFILENAME m_pfnGetThemeFilename;
	
	// GetThemeEnumValue()
	typedef HRESULT (__stdcall *PFNGETTHEMEENUMVALUE)(HTHEME, int, int, int, int*);
	PFNGETTHEMEENUMVALUE m_pfnGetThemeEnumValue;
	
	// GetThemeDocumentationProperty()
	typedef HRESULT (__stdcall *PFNGETTHEMEDOCUMENTATIONPROPERTY)(LPCWSTR, LPCWSTR, LPWSTR, int);
	PFNGETTHEMEDOCUMENTATIONPROPERTY m_pfnGetThemeDocumentationProperty;
	
	// GetThemeColor()
	typedef HRESULT (__stdcall *PFNGETTHEMECOLOR)(HTHEME, int, int, int, COLORREF*);
	PFNGETTHEMECOLOR m_pfnGetThemeColor;
	
	// GetThemeBool()
	typedef HRESULT (__stdcall *PFNGETTHEMEBOOL)(HTHEME, int, int, int, BOOL*);
	PFNGETTHEMEBOOL m_pfnGetThemeBool;
	
	// GetThemeBackgroundRegion()
	typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDREGION)(HTHEME, HDC, int, int, const RECT*, HRGN*);
	PFNGETTHEMEBACKGROUNDREGION m_pfnGetThemeBackgroundRegion;
	
	// GetThemeBackgroundExtent()
	typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDEXTENT)(HTHEME, HDC, int, int, const RECT*, RECT*);
	PFNGETTHEMEBACKGROUNDEXTENT m_pfnGetThemeBackgroundExtent;
	
	// GetThemeBackgroundContentRect()
	typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int, const RECT*, RECT*);
	PFNGETTHEMEBACKGROUNDCONTENTRECT m_pfnGetThemeBackgroundContentRect;
	
	// ThemeAppProperties()
	typedef DWORD (__stdcall *PFNTHEMEAPPPROPERTIES)();
	PFNTHEMEAPPPROPERTIES m_pfnThemeAppProperties;
	
	// GetCurrentThemeName()
	typedef HRESULT (__stdcall *PFNGETCURRENTTHEMENAME)(LPWSTR, int, LPWSTR, int, LPWSTR, int);
	PFNGETCURRENTTHEMENAME m_pfnGetCurrentThemeName;
	
	// EnableTheming()
	typedef HRESULT (__stdcall *PFNENABLETHEMING)(BOOL);
	PFNENABLETHEMING m_pfnEnableTheming;
	
	// EnableThemeDialogTexture()
	typedef HRESULT (__stdcall *PFNENABLETHEMEDIALOGTEXTURE)(HWND, DWORD);
	PFNENABLETHEMEDIALOGTEXTURE m_pfnEnableThemeDialogTexture;
	
	// DrawThemeText()
	typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT*);
	PFNDRAWTHEMETEXT m_pfnDrawThemeText;
	
	// DrawThemeParentBackground()
	typedef HRESULT (__stdcall *PFNDRAWTHEMEPARENTBACKGROUND)(HWND, HDC, RECT*);
	PFNDRAWTHEMEPARENTBACKGROUND m_pfnDrawThemeParentBackground;
	
	// DrawThemeIcon()
	typedef HRESULT (__stdcall *PFNDRAWTHEMEICON)(HTHEME, HDC, int, int, const RECT*, HIMAGELIST, int);
	PFNDRAWTHEMEICON m_pfnDrawThemeIcon;
	
	// DrawThemeEdge()
	typedef HRESULT (__stdcall *PFNDRAWTHEMEEDGE)(HTHEME, HDC, int, int, const RECT*, UINT, UINT, RECT*);
	PFNDRAWTHEMEEDGE m_pfnDrawThemeEdge;
	
	// DrawThemeBackgroundEx()
	typedef HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUNDEX)(HTHEME, HDC, int, int, const RECT*, const DTBGOPTS*);
	PFNDRAWTHEMEBACKGROUNDEX m_pfnDrawThemeBackgroundEx;
	
	// DrawThemeBackground()
	typedef HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT*);
	PFNDRAWTHEMEBACKGROUND m_pfnDrawThemeBackground;
	
	// CloseThemeData()
	typedef HRESULT (__stdcall *PFNCLOSETHEMEDATA)(HTHEME);
	PFNCLOSETHEMEDATA m_pfnCloseThemeData;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPWINTHEMEWRAPPER_H__)
