// XTTipOfTheDay.h interface for the CXTTipOfTheDay class.
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
#if !defined(__XTTIPOFTHEDAY_H__)
#define __XTTIPOFTHEDAY_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTTipOfTheDay is a multiple inheritance class derived from CXTDialogState
//			and CDialog. CXTTipOfTheDay is used to create a Visual Studio&trade; style
//			Tip Of The Day dialog. To use, place a file called "tips.txt" in the
//			same directory as your application exe. Place each tip on its own line.
class _XT_EXT_CLASS CXTTipOfTheDay : CXTDialogState, public CDialog
{
    DECLARE_DYNAMIC(CXTTipOfTheDay)

public:

	// Input:	lpszTipFile - A NULL terminated string that represents the path and file name
	//			of where the tips text file is located. By default, the file name
	//			is set to "tips.txt".
	//			pParent - Points to the parent window for the Tip Of The Day Dialog.
    // Summary:	Constructs a CXTTipOfTheDay object
    CXTTipOfTheDay(LPCTSTR lpszTipFile=NULL,CWnd* pParent = NULL);

    // Summary: Destroys a CXTTipOfTheDay object, handles cleanup and de-allocation
    virtual ~CXTTipOfTheDay();

protected:

	FILE*   m_pStream;		// A pointer to the open file stream.
	CRect   m_rcBorder;		// Size of the total display area.
	CRect   m_rcShadow;		// Size of the shadowed rect displayed to the left of the tip.
	CRect   m_rcHilite;		// Size of the background area the tips are displayed on.
	CRect   m_rcTipText;	// Size of the display area for tip text.
	CFont   m_fontTitle;	// Default font used for "Did you know..." text.
	CFont   m_fontTip;		// Default font used for tips.
	CFont*  m_pFontTitle;	// User defined font for "Did you know..." text.
	CFont*  m_pFontTip;		// User defined font for tips.
	CString m_strTipTitle;	// Represents the "Did you know" text.
	CString m_strTipText;	// Represents the Tip Of The Day text.
	CString m_strTipFile;	// Represents the file name and path for the tips file.

public:

	// Input:	pFontTitle - Points to a CFont object that represents the new
    //			font to be used for the "Did you know..." text.
	//			pFontTip - Points to a CFont object that represents the new
    //			font to be used for the Tip Of The Day text.
    // Summary:	This member function will set the fonts to be used by the "Did you
	//			know..." and the Tip Of The Day text.
    virtual void SetDefaultFonts(CFont* pFontTitle,CFont* pFontTip);

	// Input:	strNext - A CString reference that represents the next
    //			Tip Of The Day text that is to be displayed.
    // Summary:	This member function will retrieve the next string to be displayed
	//			as the Tip Of The Day.
    virtual void GetNextTipString(CString& strNext);

	// Input:	lpszTitle - Represents a NULL terminated string that is
    //			the string to be displayed in place of the
    //			"Did you know..." text.
    // Summary:	This member function will set the text that is to be displayed
    //			in place of the "Did you know..." string.
    virtual void SetDefaultTitle(LPCTSTR lpszTitle);

	// Input:	lpszTipFile - A NULL terminated string that represents the full
    //			path to where the tips text file is located.
    // Summary:	This member function will set the path to where the tips file is
	//			located.
    virtual void SetTipsFilePath(LPCTSTR lpszTipFile);

    //:Ignore
    //{{AFX_DATA(CXTTipOfTheDay)
    enum { IDD = XT_IDD_TIPOFTHEDAY };
    CButton m_ok;
    CButton m_showTips;
    CButton m_btnNextTip;
    CStatic m_staticBorder;
    BOOL    m_bStartup;
    //}}AFX_DATA
    //:End Ignore

    //:Ignore
	//{{AFX_VIRTUAL(CXTTipOfTheDay)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
	//:End Ignore

protected:

    //:Ignore
	//{{AFX_MSG(CXTTipOfTheDay)
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDaytipNext();
    //}}AFX_MSG
	//:End Ignore

    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTTipOfTheDay::SetDefaultTitle(LPCTSTR lpszTitle) {
    m_strTipTitle = lpszTitle;
}

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTTIPOFTHEDAY_H__)
//:End Ignore