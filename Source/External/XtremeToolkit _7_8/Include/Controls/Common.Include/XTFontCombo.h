// XTFontCombo.h interface for the CXTFontCombo class.
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

//:Ignore
#if !defined(__XTFONTCOMBO_H__)
#define __XTFONTCOMBO_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTFontCombo is a CXTFlatComboBox derived class. It is used to create
//			a combo box that displays a drop list of available fonts for your system.
//			The fonts are displayed in their various styles.
class _XT_EXT_CLASS CXTFontCombo : public CXTFlatComboBox
{
    DECLARE_DYNAMIC(CXTFontCombo)

public:

    // Summary: Constructs a CXTFontCombo object
    CXTFontCombo();

    // Summary: Destroys a CXTFontCombo object, handles cleanup and de-allocation
    virtual ~CXTFontCombo();
    
protected:

	int			m_cyHScroll;	// Represents system metrics for SM_CYHSCROLL.
	int			m_cyEdge;		// Represents system metrics for SM_CYEDGE.
	eSTYLE		m_eStyle;		// Enumerated style indicating how to display the font list.
	CString		m_strSymbol;	// String displayed for symbol characters.
	CImageList	m_ilFontType;	// true type font image list.

public:

	// Input:	lf - Reference to an XT_LOGFONT structure.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to get the logfont for the currently selected
	//			item. 
    virtual bool GetSelFont(XT_LOGFONT& lf);

	// Input:	strFaceName - A reference to a valid CString object to receive the logfont face
	//			name.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to get the logfont for the currently selected
	//			item. 
	virtual bool GetSelFont(CString& strFaceName);

	// Input:	lf - Reference to an XT_LOGFONT structure.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to select the logfont for the list box.
    virtual bool SetSelFont(XT_LOGFONT& lf);

	// Input:	strFaceName - A NULL terminated string that represents the logfont face name.
	// Returns: true if successful, otherwise returns false.
    // Summary:	Call this member function to select the logfont for the list box.
	virtual bool SetSelFont(CString strFaceName);

	// BULLETED LIST:

	// Input:	eStyle - Specifies the style for the font list box. Styles can be any one of
	//			the following combinations:
	//			[ul]
	//			[li]<b>XT_FLB_NAME_GUI</b> Display font name with GUI font style.[/li]
	//			[li]<b>XT_FLB_NAME_SAMPLE</b> Display font name with its own font
	//			style.[/li]
	//			[li]<b>XT_FLB_BOTH</b> Display font name with GUI font style, then
	//			a sample display to the right.[/li]
	//			[/ul]
	// Summary:	Call this member function to set the font display style for the font
	//			list box. There are three styles to choose from that include displaying
	//			the font in the default GUI font, displaying the font in its own font
	//			style, or displaying both the font name in the default GUI font and
	//			a sample to the right.
	void SetListStyle(eSTYLE eStyle);

	// Input:	lpszFaceName - A NULL terminated string that represents the logfont face name.
	//			nWidth - The minimum allowable width of the list box portion of the combo
	//			box in pixels.
	//			bEnable - TRUE to enable autocompletion, otherwise FALSE.
	// Summary:	Call this member function to initialize the font list box and populate it
	//			with a list of avaliable fonts.
	virtual void InitControl(LPCTSTR lpszFaceName=NULL,UINT nWidth=0,BOOL bEnable=TRUE );

    //:Ignore
	//{{AFX_VIRTUAL(CXTFontCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL
	//:End Ignore
    
    //:Ignore
	//{{AFX_MSG(CXTFontCombo)
	//}}AFX_MSG
	//:End Ignore
	
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFontCombo::SetListStyle(eSTYLE eStyle) {
    m_eStyle = eStyle;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTFONTCOMBO_H__)
//:End Ignore