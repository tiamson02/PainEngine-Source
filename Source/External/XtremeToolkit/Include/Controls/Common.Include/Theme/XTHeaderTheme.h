// XTHeaderTheme.h: interface for the CXTHeaderTheme class.
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

#if !defined(__XTHEADERTHEME_H__)
#define __XTHEADERTHEME_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTThemeManager.h"

class CXTHeaderCtrl;

//========================================================================
// Summary: CXTHeaderTheme is a theme class used to draw windows classic 
//          style theme for the header control.
//
//          The CXTHeaderTheme class provides theme capability to the 
//          CXTHeaderCtrl class.  You can derive a class from CXTHeaderTheme
//          to custom user defined themes.
//
//			To use this class, first subclass your header control using the
//          Windows API SubclassWindow and a CXTHeaderCtrl base class.  Once
//          you have done this you can call SetTheme and pass in a pointer to
//          the theme you wish to use for the header control.
//========================================================================
class _XT_EXT_CLASS CXTHeaderTheme  
{
public:

    //--------------------------------------------------------------------
    // Summary: Constructs a CXTHeaderTheme object using the specified 
	//          header control.
    //--------------------------------------------------------------------
	CXTHeaderTheme();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTHeaderTheme object, handles cleanup and 
	//          de-allocation.
    //--------------------------------------------------------------------
	virtual ~CXTHeaderTheme();
	
protected:

    int                 m_iArrowPadding; // Spacing between text and sort arrow.
    DWORD               m_dwDrawStyle;   // Style flags used by theme, see CXTHeaderTheme::SetDrawStyle().
    BOOL                m_bUseWinThemes; // TRUE to use Windows XP themes when available.
    CSize               m_sizePadding;   // Leading and trailing padding for text display.
    CSize               m_sizeArrow;     // Width and height of sort arrow.
    COLORREF            m_crText;        // RGB value for text color.
    COLORREF            m_cr3DFace;      // RGB value for header background color.
    COLORREF            m_cr3DShadow;    // RGB value for border shadow color.
    COLORREF            m_cr3DDkShadow;  // RGB value for border dark shadow color.
    COLORREF            m_cr3DHighLight; // RGB value for border highlight color.
	XTEnumTheme         m_enumTheme;     // Enumerated value that holds the currently selected theme.
    CXTPWinThemeWrapper m_themeWrapper;  // Wraps the Windows XP theme API that is used to display Windows XP themes.

public:

    //--------------------------------------------------------------------
    // Summary: Call this member function to return a pointer to the Windows
	//          theme API wrapper for this control.
    // Returns: A CXTPWinThemeWrapper pointer.
    //--------------------------------------------------------------------
	CXTPWinThemeWrapper* GetThemeWrapper();

	//-----------------------------------------------------------------------
	// Input:   enumTheme - Represents the theme to set.  Can be one of the
	//                      following enumerated values:
	//
    //                      xtThemeDefault    - Default theme.
    //                      xtThemeOfficeXP   - Office XP theme.
    //                      xtThemeOffice2003 - Office 2003 theme.
    //                      xtThemeCustom     - User defined theme.
	//
	// Summary: Call this member function to set the theme for CXTHeader.
	//-----------------------------------------------------------------------
	virtual void SetTheme(XTEnumTheme enumTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the currently selected theme.
	// Returns: A XTEnumTheme one of the following enumerated values:
	//
    //          xtThemeDefault    - Default theme.
    //          xtThemeOfficeXP   - Office XP theme.
    //          xtThemeOffice2003 - Office 2003 theme.
    //          xtThemeCustom     - User defined theme.
	//-----------------------------------------------------------------------
	virtual XTEnumTheme GetTheme() const;

    //--------------------------------------------------------------------
    // Input:   pDC - Pointer to a valid device context.
    // Summary: Call this member function from the header control's WM_PAINT
	//          message to perform themed drawing.
    //--------------------------------------------------------------------
	virtual void DrawHeader(CDC* pDC, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Summary: Call this member whenever the system colors need to be updated.
    //--------------------------------------------------------------------
	virtual void RefreshMetrics();

    //--------------------------------------------------------------------
    // Summary: This member function can be called to determine if Windows XP
	//          themes can be displayed for the header control.
    // Returns: TRUE if Windows themes can be displayed, otherwise FALSE.
    //--------------------------------------------------------------------
    virtual BOOL UseWinXPThemes(CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Input:   dwDrawStyle - The desired styles for the header control theme.
	//                        Can be one or more of the following:
	//
	//                        XTTHEME_WINXPTHEMES - Allows the header control
	//                                              to display Windows XP themes
	//                                              when they are available.
	//                        XTTHEME_HOTTRACKING - Allows the header control
	//                                              to display mouse over effects
	//                                              for the header control.
	//                        XTTHEME_SORTARROW   - Allows the header control
	//                                              to display a sort arrow to
	//                                              indicate sort order.
    // Summary: Call this member function to set the desired XTTHEME_ drawing 
	//          styles for the header theme.  Does not affect the WS_ (window
	//          style) settings.
    //--------------------------------------------------------------------
	virtual void SetDrawStyle(DWORD dwDrawStyle, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Summary: Call this member function to return the current drawing style
	//          used by the header theme.
    // Returns: The current XTTHEME_ (xt theme) settings for the header control.
	//          See CXTHeaderTheme::SetDrawStyle for the complete list of available
	//          styles.
    //--------------------------------------------------------------------
	virtual DWORD GetDrawStyle() const;

    //--------------------------------------------------------------------
    // Input:   lpHDL - Pointer to anHDLAYOUT structure, which contains 
	//                  information used to set the size and position of a 
	//                  header control.
    // Summary: Retrieves the size and position of a header control within a 
	//          given rectangle. This function is used to determine the 
	//          appropriate dimensions for a new header control that is to 
	//          occupy the given rectangle.
    // Returns: Nonzero if successful; otherwise 0.
    //--------------------------------------------------------------------
    virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Summary: Cleans up allocated GDI resources used by the theme.
    //--------------------------------------------------------------------
	virtual void CleanUp(CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
	// Input:	iCol      - Zero-based index of the column to set the sort 
    //                      image for.
	//			uBitmapID - Resource ID of the bitmap to use.
	//			bRemove   - TRUE to remove a bitmap, FALSE if adding a bitmap.
    //          crMask    - Color used to generate the mask. Each pixel of 
	//                      this color in the given bitmap is changed to black 
	//                      and the corresponding bit in the mask is set to one.
    // Summary:	Call this member function to set the bitmap image for the 
    //          specified header item.
    // Returns: TRUE if successful; otherwise FALSE.
    //--------------------------------------------------------------------
    virtual BOOL SetBitmap(int iCol, UINT uBitmapID, BOOL bRemove, COLORREF crMask, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
	// Returns: TRUE if the header control displays a sort arrow, otherwise returns FALSE.
	// Summary:	Call this member function to determine if the header control displays
	//			a sort arrow. 
    //--------------------------------------------------------------------
	BOOL HasSortArrow();

protected:

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the header
	//          control's background.
    //--------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the header
	//          control's background using Windows themes.
    // Returns: TRUE if successful; otherwise FALSE.
    //--------------------------------------------------------------------
	virtual BOOL OnDrawBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the background 
	//          for the header item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
	virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the background 
	//          using Windows themes for the header item specified by lpDIS->itemID.
    // Returns: TRUE if successful; otherwise FALSE.
    //--------------------------------------------------------------------
	virtual BOOL OnDrawItemBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL      - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                       structure contains information about the item to be 
	//                       drawn and the type of drawing required.
    //          bAscending - TRUE if the arrow is to be draw ascending.
    // Summary: This member function is called by the theme to draw the sort 
	//          used to display sort order for header item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
	virtual void OnDrawItemSortArrow(LPDRAWITEMSTRUCT lpDIS, BOOL bAscending);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called to draw the text for the header 
	//          item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
	virtual void OnDrawItemText(LPDRAWITEMSTRUCT lpDIS, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Input:   lpHDL  - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                   structure contains information about the item to be 
	//                   drawn and the type of drawing required.
    //          rcText - Size of the area where the header item text is displayed. 
    // Summary: This member function is called to draw a bitmap for the header 
	//          item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
	virtual void OnDrawItemBitmap(LPDRAWITEMSTRUCT lpDIS, CRect& rcText, CXTHeaderCtrl* pHeaderCtrl);

    //--------------------------------------------------------------------
    // Input:   pDC        - Pointer to a valid device context.
    //          ptsArrow   - POINT array used to define the 3 points of the arrow drawn.
    //          bAscending - TRUE if the arrow is to be draw ascending.
    // Summary: This member function is called by CXTHeaderTheme::OnDrawItemSortArrow
	//          to draw the sort arrow.
    //--------------------------------------------------------------------
	virtual void DrawSortArrow(CDC* pDC, LPPOINT ptsArrow, BOOL bAscending);
};

//===========================================================================

AFX_INLINE CXTPWinThemeWrapper* CXTHeaderTheme::GetThemeWrapper() {
	return &m_themeWrapper;
}
AFX_INLINE void CXTHeaderTheme::SetTheme(XTEnumTheme enumTheme) {
	m_enumTheme = enumTheme;
}
AFX_INLINE XTEnumTheme CXTHeaderTheme::GetTheme() const {
	return m_enumTheme;
}

//===========================================================================
// Summary: CXTHeaderThemeOfficeXP is a CXTHeaderTheme derived theme class
//          used to draw an Office XP style theme for the header control.
//
//          The CXTHeaderThemeOfficeXP class provides theme capability to the 
//          CXTHeaderCtrl class.  You can derive a class from CXTHeaderThemeOfficeXP
//          to custom user defined themes.
//
//			To use this class, first subclass your header control using the
//          Windows API SubclassWindow and a CXTHeaderCtrl base class.  Once
//          you have done this you can call SetTheme and pass in a pointer to
//          the theme you wish to use for the header control.
//===========================================================================
class _XT_EXT_CLASS CXTHeaderThemeOfficeXP : public CXTHeaderTheme
{
public:

    //--------------------------------------------------------------------
    // Input:   pHeaderCtrl - Pointer to the header control to be themed.
    // Summary: Constructs a CXTHeaderThemeOfficeXP object using the specified 
	//          header control.
    //--------------------------------------------------------------------
    CXTHeaderThemeOfficeXP();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTHeaderThemeOfficeXP object, handles cleanup and 
	//          de-allocation.
    //--------------------------------------------------------------------
    virtual ~CXTHeaderThemeOfficeXP();

protected:

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the header
	//          control's background.
    //--------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the background 
	//          for the header item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
    virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);
};

//========================================================================
// Summary: CXTHeaderThemeOffice2003 is a CXTHeaderTheme derived theme class
//          used to draw an Office 2003 style theme for the header control.
//
//          The CXTHeaderThemeOffice2003 class provides theme capability to the 
//          CXTHeaderCtrl class.  You can derive a class from CXTHeaderThemeOffice2003
//          to custom user defined themes.
//
//			To use this class, first subclass your header control using the
//          Windows API SubclassWindow and a CXTHeaderCtrl base class.  Once
//          you have done this you can call SetTheme and pass in a pointer to
//          the theme you wish to use for the header control.
//========================================================================
class _XT_EXT_CLASS CXTHeaderThemeOffice2003 : public CXTHeaderTheme
{
public:

    //--------------------------------------------------------------------
    // Input:   pHeaderCtrl - Pointer to the header control to be themed.
    // Summary: Constructs a CXTHeaderThemeOffice2003 object using the specified 
	//          header control.
    //--------------------------------------------------------------------
    CXTHeaderThemeOffice2003();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTHeaderThemeOffice2003 object, handles cleanup and 
	//          de-allocation.
    //--------------------------------------------------------------------
    virtual ~CXTHeaderThemeOffice2003();

    //--------------------------------------------------------------------
    // Input:   lpHDL - Pointer to anHDLAYOUT structure, which contains 
	//                  information used to set the size and position of a 
	//                  header control.
    // Summary: Retrieves the size and position of a header control within a 
	//          given rectangle. This function is used to determine the 
	//          appropriate dimensions for a new header control that is to 
	//          occupy the given rectangle.
    // Returns: Nonzero if successful; otherwise 0.
    //--------------------------------------------------------------------
    virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

protected:

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the header
	//          control's background.
    //--------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the background 
	//          for the header item specified by lpDIS->itemID.
    //--------------------------------------------------------------------
    virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);
};

//========================================================================
// Summary: CXTHeaderThemeExplorer is a CXTHeaderThemeOffice2003 derived theme class
//          used to draw an Office XP style theme for the header control.
//
//          The CXTHeaderThemeExplorer class provides theme capability to the 
//          CXTHeaderCtrl class.  You can derive a class from CXTHeaderThemeExplorer
//          to custom user defined themes.
//
//			To use this class, first subclass your header control using the
//          Windows API SubclassWindow and a CXTHeaderCtrl base class.  Once
//          you have done this you can call SetTheme and pass in a pointer to
//          the theme you wish to use for the header control.
//========================================================================
class _XT_EXT_CLASS CXTHeaderThemeExplorer : public CXTHeaderThemeOffice2003
{
public:

    //--------------------------------------------------------------------
    // Input:   pHeaderCtrl - Pointer to the header control to be themed.
    // Summary: Constructs a CXTHeaderThemeExplorer object using the specified 
	//          header control.
    //--------------------------------------------------------------------
    CXTHeaderThemeExplorer();

    //--------------------------------------------------------------------
    // Summary: Destroys a CXTHeaderThemeExplorer object, handles cleanup and 
	//          de-allocation.
    //--------------------------------------------------------------------
    virtual ~CXTHeaderThemeExplorer();

    //--------------------------------------------------------------------
    // Input:   lpHDL - Pointer to anHDLAYOUT structure, which contains 
	//                  information used to set the size and position of a 
	//                  header control.
    // Summary: Retrieves the size and position of a header control within a 
	//          given rectangle. This function is used to determine the 
	//          appropriate dimensions for a new header control that is to 
	//          occupy the given rectangle.
    // Returns: Nonzero if successful; otherwise 0.
    //--------------------------------------------------------------------
    virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

protected:

    //--------------------------------------------------------------------
    // Input:   lpHDL - A long pointer to a DRAWITEMSTRUCT structure. The 
	//                  structure contains information about the item to be 
	//                  drawn and the type of drawing required.
    // Summary: This member function is called by the theme to draw the background 
	//          using Windows themes for the header item specified by lpDIS->itemID.
    // Returns: TRUE if successful; otherwise FALSE.
    //--------------------------------------------------------------------
	virtual BOOL OnDrawItemBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

    //--------------------------------------------------------------------
    // Input:   pDC        - Pointer to a valid device context.
    //          ptsArrow   - POINT array used to define the 3 points of the arrow drawn.
    //          bAscending - TRUE if the arrow is to be draw ascending.
    // Summary: This member function is called by CXTHeaderTheme::OnDrawItemSortArrow
	//          to draw the sort arrow.
    //--------------------------------------------------------------------
    virtual void DrawSortArrow(CDC* pDC, LPPOINT ptsArrow, BOOL bAscending);
};

//========================================================================

#ifndef ODS_HOTLIGHT
#define ODS_HOTLIGHT        0x0040
#endif//ODS_HOTLIGHT

#define XTTHEME_WINXPTHEMES	0x0001
#define XTTHEME_HOTTRACKING 0x0002
#define XTTHEME_SORTARROW   0x0004

//========================================================================

AFX_INLINE BOOL CXTHeaderTheme::HasSortArrow() {
	return ((m_dwDrawStyle & XTTHEME_SORTARROW) == XTTHEME_SORTARROW);
}

//========================================================================

#endif // !defined(__XTHEADERTHEME_H__)
