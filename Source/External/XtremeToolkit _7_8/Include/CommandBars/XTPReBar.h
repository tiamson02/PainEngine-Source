// XTPReBar.h : header file
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
#if !defined(__XTPREBAR_H__)
#define __XTPREBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#if _MSC_VER >= 1200 // MFC 6.0 or greater

//////////////////////////////////////////////////////////////////////
// Summary: CXTPReBar is a CReBar derived class. It is used to create a CXTPReBar
//			object. A CXTPReBar object is a control bar that provides layout, persistence,
//			and state information for rebar controls. 
//
//			A rebar object can contain a variety of child windows, usually other
//			controls, including edit boxes, toolbars, and list boxes. A rebar object
//			can display its child windows over a specified bitmap. Your application
//			can automatically resize the rebar, or the user can manually resize
//			the rebar by clicking or dragging its gripper bar. 
//
//			<b>Rebar Control</b>
//
//			A rebar object behaves similarly to a toolbar object. A rebar uses
//			the click-and-drag mechanism to resize its bands. A rebar control can
//			contain one or more bands, with each band having any combination of
//			a gripper bar, a bitmap, a text label, and a child window. However,
//			bands cannot contain more than one child window.
//
//			CXTPReBar uses the CReBarCtrl class to provide its implementation.
//			You can access the rebar control through GetReBarCtrl to take advantage
//			of the control's customization options. For more information about
//			rebar controls, see CReBarCtrl. For more information about using
//			rebar controls, see Using CReBarCtrl in the XTreme Toolkit online
//			help.
//
//			<b>Warning</b>    Rebar and rebar control objects do not support MFC
//			control bar docking. If CRebar::EnableDocking is called, your application
//			will assert.
class _XTP_EXT_CLASS CXTPReBar : public CReBar
{
	DECLARE_DYNAMIC(CXTPReBar)

public:

    // Summary: Constructs a CXTPReBar object 
	CXTPReBar();

    // Summary: Destroys a CXTPReBar object, handles cleanup and de-allocation 
	virtual ~CXTPReBar();

	// Input:	pBar - A pointer to a CWnd object that is the child window to be inserted
	//			into the rebar. The referenced object must have a WS_CHILD.
	//			pszText - A pointer to a string containing the text to appear on the rebar.
	//			NULL by default. The text contained in 'pszText' is not part of the
	//			child window. It is on the rebar itself.
	//			pbmp - A pointer to a CBitmap object to be displayed on the rebar background.
	//			NULL by default.
	//			dwStyle - A DWORD containing the style to apply to the rebar. See the 'fStyle'
	//			function description in the Win32 structure REBARBANDINFO for a complete
	//			list of band styles.
	// Example: <pre>
	//			// Define a pointer to a CRebar in your class definition,
	//			// such as: CReBar* m_pReBar; More often, however, you
	//			// would probably specify an instance in your class
	//			// definition, such as: CReBar m_ReBar;
	//			m_pReBar = new CReBar();
	//			m_pReBar->Create(this);
	//			m_wndDlgBar.Create(this, IDR_MAINFRAME, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR);
	//			m_pReBar->AddBar(&m_wndDlgBar);</pre>
	// Returns: Nonzero if successful, otherwise returns zero.
	// Summary:	Call this member function to add a band to the rebar.
	virtual BOOL AddBar(CWnd* pBar,LPCTSTR pszText = NULL,CBitmap* pbmp = NULL,DWORD dwStyle = RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP);

	// Input:	pBar - A pointer to a CWnd object that is the child window to be inserted
	//			into the rebar. The referenced object must have a WS_CHILD.
	//			clrFore - An RGB value that represents the foreground color of the rebar.
	//			clrBack - An RGB value that represents the background color of the rebar.
	//			pszText - A pointer to a string containing the text to appear on the rebar.
	//			NULL by default. The text contained in 'pszText' is not part of the
	//			child window. It is on the rebar itself.
	//			dwStyle - A DWORD containing the style to apply to the rebar. See the 'fStyle'
	//			function description in the Win32 structure REBARBANDINFO for a complete
	//			list of band styles.
	// Example:	<pre>
	//			// Define a pointer to a CRebar in your class definition,
	//			// such as: CReBar* m_pReBar; More often, however, you
	//			// would probably specify an instance in your class
	//			// definition, such as: CReBar m_ReBar;
	//			m_pReBar = new CReBar();
	//			m_pReBar->Create(this);
	//			m_wndDlgBar.Create(this, IDR_MAINFRAME, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR);
	//			m_pReBar->AddBar(&m_wndDlgBar);</pre>
	// Returns: Nonzero if successful, otherwise returns zero.
	// Summary:	Call this member function to add a band to the rebar.
	virtual BOOL AddBar(CWnd* pBar,COLORREF clrFore,COLORREF clrBack,LPCTSTR pszText = NULL,DWORD dwStyle = RBBS_GRIPPERALWAYS);

	// Input:	pBar - A pointer to a CWnd object that is the child window to be inserted
	//			into the rebar. The referenced object must have a WS_CHILD.
	//			pRBBI - Points to a REBARBANDINFO struct that contains information about
	//			the band added to a rebar control.
	// Example: <pre>
	//			// Define a pointer to a CRebar in your class definition,
	//			// such as: CReBar* m_pReBar; More often, however, you
	//			// would probably specify an instance in your class
	//			// definition, such as: CReBar m_ReBar;
	//			m_pReBar = new CReBar();
	//			m_pReBar->Create(this);
	//			m_wndDlgBar.Create(this, IDR_MAINFRAME, CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR);
	//			m_pReBar->AddBar(&m_wndDlgBar);</pre>
	// Returns: Nonzero if successful, otherwise returns zero.
	// Summary:	Call this member function to add a band to the rebar.
	virtual BOOL AddBar(CWnd* pBar,REBARBANDINFO* pRBBI);

	// Input:	lpszSection - Name of a section in the initialization file or a key in the Windows 
	//			registry where state information is stored.
	// Summary:	Call this function to store information about the rebar owned by the
	//			frame window. This information can be read from the initialization
	//			file using LoadState. Information stored includes visibility, 
	//			horizontal and vertical orientation, band size, and control bar position.
	virtual void SaveState(LPCTSTR lpszSection);

	// Input:	lpszSection - Name of a section in the initialization file or a key in the Windows 
	//			registry where state information is stored.
	// Summary:	Call this function to restore the settings of the rebar owned by the
	//			frame window. This information is written to the initialization file
	//			using SaveState. Information restored includes visibility, 
	//			horizontal and vertical orientation, band size, and control bar position.
	virtual void LoadState(LPCTSTR lpszSection);

protected:

	// Returns: A DWORD value.
    // Summary:	This member function is called to retrieve the version information
	//			for the common control dll (comctl32.dll). 
    DWORD GetComCtlVersion();		

protected:
    int   m_iComCtlVersion;			// Common control dll version.

    //{{AFX_VIRTUAL(CXTPReBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPReBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//:Ignore
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif//_MSC_VER >= 1200

#endif // !defined(__XTPREBAR_H__)
//:End Ignore