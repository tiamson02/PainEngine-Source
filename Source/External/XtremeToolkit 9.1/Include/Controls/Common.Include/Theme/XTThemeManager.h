// XTThemeManager.h: interface for the CXTThemeManager class.
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

#if !defined(__XTTHEMEMANAGER_H__)
#define __XTTHEMEMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//---------------------------------------------------------------------------
// Summary: XTEnumTheme enumeration defines the constants used by the
//          CXTThemeManger to determine the currently selected theme.
//---------------------------------------------------------------------------
enum XTEnumTheme
{
	xtThemeDefault,	    // Default theme.
	xtThemeOfficeXP,	// Office XP theme.
	xtThemeOffice2003,	// Office 2003 theme.
	xtThemeCustom		// User defined theme.
};

//---------------------------------------------------------------------------
// Summary: CXTColor helper class used by themes to allow for user defined
//          color management.  Class is used as a COLORREF definition that
//          typically holds the default system color for a given system color.
//          The user can set this value to a custom color which will ensure
//          the user defined color is not overridden when system colors are
//          updated.
//---------------------------------------------------------------------------
class _XT_EXT_CLASS CXTColor
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTColor object.
	//-----------------------------------------------------------------------
	CXTColor();

	//-----------------------------------------------------------------------
	// Input:   color        - RGB value for new color.
	//          bUserDefined - Set to true if this color should not be overridden
	//                         when using the '=' assignment operator.
	// Summary: This member is called to define a new color value for the
	//          CXTColor object.
	//-----------------------------------------------------------------------
	void SetColor(const COLORREF color, bool bUserDefined=true);

	//-----------------------------------------------------------------------
	// Summary: Cast operator to retrieve the currently defined color.
	// Returns: An RGB value that represents the color set for this object.
	//-----------------------------------------------------------------------
	operator COLORREF() const;

	//-----------------------------------------------------------------------
	// Summary: Initialization operator that sets a new value for the CXTColor
	//          object.  If you have defined your color by calling SetColor(color, false)
	//          using this assignment operator will have no effect on setting the color
	//          value for this object.  You must call SetColor(color, false) to
	//          reset the user defined flag first before using this operator.
	// Returns: An RGB value that represents the color set for this object.
	//-----------------------------------------------------------------------
	const COLORREF operator= (const COLORREF color);

	//-----------------------------------------------------------------------
	// Summary: Used to determine if the CXTColor object is a user defined color.
	// Returns: True if the CXTColor object is a user defined color, otherwise
	//          returns False.
	//-----------------------------------------------------------------------
	bool IsUserDefined() const;

protected:
	bool     m_bUserDefined; // True if the CXTColor object is a user defined color.
	COLORREF m_crColor;      // RGB value for this object.
};

// forwards
class CXTButtonTheme;

//===========================================================================
// Summary: CXTThemeManager is used to manage the visual styles for your
//          application.  Add a member of this type to your frame or
//          dialog class to manage your application themes.  See SetTheme
//          for more information on setting themes.
//===========================================================================
class _XT_EXT_CLASS CXTThemeManager  
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTThemeManager object.
	//-----------------------------------------------------------------------
	CXTThemeManager();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTThemeManager object, handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTThemeManager();

protected:
	XTEnumTheme      m_enumTheme;    // Enumerated value that holds the currently selected theme.
	CXTButtonTheme*  m_pButtonTheme; // Pointer to the default theme for CXTButton.

public:
	//-----------------------------------------------------------------------
	// Input:   enumTheme - Represents the theme to set.  Can be one of the
	//                      following enumerated values:
	//
    //                      xtThemeDefault    - Default theme.
    //                      xtThemeOfficeXP   - Office XP theme.
    //                      xtThemeOffice2003 - Office 2003 theme.
    //                      xtThemeCustom     - User defined theme.
	//
	// Summary: Call this member function to set the theme for the application.
	//-----------------------------------------------------------------------
	void SetTheme(XTEnumTheme enumTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the currently selected theme.
	// Returns: A XTEnumTheme one of the following enumerated values:
	//
    //          xtThemeDefault    - Default theme.
    //          xtThemeOfficeXP   - Office XP theme.
    //          xtThemeOffice2003 - Office 2003 theme.
    //          xtThemeCustom     - User defined theme.
	//-----------------------------------------------------------------------
	XTEnumTheme GetTheme() const;

	//-----------------------------------------------------------------------
	// Input:   pButtonTheme - Pointer to the theme to be used as default for CXTButton.
	// Summary: Call this member function to set the theme for CXTButton class.
	//-----------------------------------------------------------------------
	void SetButtonTheme(CXTButtonTheme* pButtonTheme);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return a pointer to the active
	//          theme for CXTButton.
	// Returns: A CXTButtonTheme pointer to the active CXTButton theme.
	//-----------------------------------------------------------------------
	CXTButtonTheme* GetButtonTheme() const;
	
	//-----------------------------------------------------------------------
	// Input:   enumTheme - Represents the theme to set.  Can be one of the
	//                      following enumerated values:
	//
    //                      xtThemeDefault    - Default theme.
    //                      xtThemeOfficeXP   - Office XP theme.
    //                      xtThemeOffice2003 - Office 2003 theme.
    //                      xtThemeCustom     - User defined theme.
	//
	// Summary: Call this member function to set the theme used by CXTButton.
	//-----------------------------------------------------------------------
	void SetButtonTheme(XTEnumTheme enumTheme);

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the theme manager to refresh
	//          the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	void RefreshMetrics();
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(__XTTHEMEMANAGER_H__)
