// XTPaintManager.h: interface for the CXTPaintManager class.
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
//////////////////////////////////////////////////////////////////////

#if !defined(__XTPAINTMANAGER_H__)
#define __XTPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Summary: CXTPaintManager is a singleton object that is used to perform
//          standard drawing routines for the currently active themes.
//===========================================================================
class _XT_EXT_CLASS CXTPaintManager  
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPaintManager object.
	//-----------------------------------------------------------------------
	~CXTPaintManager();

	//-----------------------------------------------------------------------
	// Input:   pDC  - Pointer to a valid device context.
	//          rect - Size of the area to fill.
	// Summary: This member function is called to fill a shaded retangle for
	//          the specified device context.
	//-----------------------------------------------------------------------
	void FillShadedRect(CDC *pDC, CRect& rect);

	//-----------------------------------------------------------------------
	// Input:   strIn - Represents a NULL terminated string that contains
	//                  mnemonic characters to be removed.
	// Summary: Call this member function to remove mnemonic characters from
	//          the specified string. Mnemonic characters are used to display
	//          underlines for accelerators in menus and toolbars.
	//-----------------------------------------------------------------------
	void StripMnemonics(CString& strIn);

private:

	CXTPaintManager();

	// singleton
	static CXTPaintManager m_instance;
	friend CXTPaintManager* XTPaintManager();
};

//===========================================================================

AFX_INLINE CXTPaintManager* XTPaintManager() {
	return &CXTPaintManager::m_instance;
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPAINTMANAGER_H__)
