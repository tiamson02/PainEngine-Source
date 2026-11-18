// XTPDockState.h : interface for the CXTPDockState class.
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
#if !defined(__XTPDOCKSTATE_H__)
#define __XTPDOCKSTATE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPToolbar.h"

//===========================================================================
// Summary: CXTPDockState is a stand alone class. It is used internally by CXTPToolBar.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockState
{
	friend class CXTPCommandBars;				
	typedef CXTPToolBar::CToolBarInfo TOOLBARINFO;  //TOOLBARINFO define.

public:
	
	//-----------------------------------------------------------------------
	// Input:	pt - Specifies the point to be tested
	// Summary: Maps the point from the stored coordinates to the device.
	//-----------------------------------------------------------------------
	void ScalePoint(CPoint& pt);

	//-----------------------------------------------------------------------
	// Input:	rect - A reference to a CRect object
	// Summary: Maps the point from the stored coordinates to the device.
	//-----------------------------------------------------------------------
	void ScaleRectPos(CRect& rect);

private:

	CXTPDockState(void);
	~CXTPDockState(void);

	void SaveState(LPCTSTR lpszProfileName);
	void LoadState(LPCTSTR lpszProfileName);
	
	void Serialize(CArchive& ar);

private:
	void SaveDockBarState(LPCTSTR lpszProfileName);
	CSize GetScreenSize();
	void SetScreenSize(CSize& size);
	void Clear();

private:
	CArray<TOOLBARINFO*, TOOLBARINFO*> m_arrBarInfo;
	BOOL m_bScaling;
	CRect m_rectDevice;
	CSize m_sizeLogical;
	CRect m_rectClip;
	DWORD m_dwVersion;

};

#endif //#if !defined(__XTPDOCKSTATE_H__)