// XTVC50Helpers.h : Visual C++ 5.0 helpers
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©1998-2004 Codejock Software, All Rights Reserved.
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

#if !defined(__XTVC50HELPERS_H__)
#define __XTVC50HELPERS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (_MSC_VER <= 1100) // Using Visual C++ 5.0

/////////////////////////////////////////////////////////////////////////////
// CString_VC50

class _XT_EXT_CLASS CString_VC50 : public CString
{
public:
    CString_VC50();
    CString_VC50(CString strIn);
    int Find(LPCTSTR lpszSub, int nStart) const;
    int Find(TCHAR ch) const;
    int Find(LPCTSTR lpszSub) const;
    int Insert(int nIndex, TCHAR ch);
    int Insert(int nIndex, LPCTSTR pstr);
    const CString& operator =(const CString& stringSrc);
    const CString& operator =(TCHAR ch);
    const CString& operator =(const unsigned char* psz);
    const CString& operator =(LPCWSTR lpsz);
    const CString& operator =(LPCSTR lpsz);
    int Remove(TCHAR chRemove);
	int Replace(TCHAR chOld, TCHAR chNew);
    int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
    int Delete(int nIndex, int nCount=1);
    CString_VC50 Left(int nCount) const;
};

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrl_VC50

class _XT_EXT_CLASS CHeaderCtrl_VC50 : public CHeaderCtrl
{
protected:
    virtual int GetItemCount() const;
    virtual BOOL GetOrderArray(LPINT piArray,int iCount=-1);
    virtual BOOL GetItemRect(int nIndex,LPRECT lpRect) const;
};

/////////////////////////////////////////////////////////////////////////////
// CDockBar_VC50

class _XT_EXT_CLASS CDockBar_VC50 : public CDockBar
{
protected:
	virtual void DrawGripper(CDC* pDC, const CRect& rect);
};

/////////////////////////////////////////////////////////////////////////////
// CListCtrl_VC50

class _XT_EXT_CLASS CListCtrl_VC50 : public CListCtrl
{
public:
	POSITION GetFirstSelectedItemPosition() const;
	int GetNextSelectedItem(POSITION& pos) const;
};

/////////////////////////////////////////////////////////////////////////////
// Summary: CMap definition for mapping XT_CLRFONT structures using Visual C++ 5.0.

typedef CMap<void*, void*, XT_CLRFONT,XT_CLRFONT&> CXTColorFontMap;

/////////////////////////////////////////////////////////////////////////////
// VC5 support definitions:

#ifndef VERSION_WIN4
#define VERSION_WIN4                    MAKELONG(0, 4)
#endif

#ifndef VERSION_IE401
#define VERSION_IE401                   MAKELONG(72,4)
#endif

#ifndef CBRS_GRIPPER
#define CBRS_GRIPPER                    0x00400000L
#endif

#ifndef AFX_IDW_REBAR
#define AFX_IDW_REBAR                   0xE804
#endif

#ifndef ID_VIEW_REBAR
#define ID_VIEW_REBAR                   0xE804
#endif

#ifndef AFX_IDW_DIALOGBAR
#define AFX_IDW_DIALOGBAR               0xE805
#endif


#ifndef SPI_GETFLATMENU
#define SPI_GETFLATMENU                 0x1022
#endif

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE              0x0040
#endif

#ifndef AFX_INLINE
#define AFX_INLINE inline
#endif

#ifndef AFX_STATIC
#define AFX_STATIC static
#endif

#else

typedef CString         CString_VC50;
typedef CHeaderCtrl     CHeaderCtrl_VC50;
typedef CDockBar        CDockBar_VC50;
typedef CListCtrl       CListCtrl_VC50;

/////////////////////////////////////////////////////////////////////////////
// Summary: CMap definition for mapping XT_CLRFONT structures using Visual C++ 6.0 or later.

typedef CMap<HTREEITEM,HTREEITEM,XT_CLRFONT,XT_CLRFONT&> CXTColorFontMap;

#endif//#if (_MSC_VER <= 1100)

/////////////////////////////////////////////////////////////////////////////
// CListView_VC50

class _XT_EXT_CLASS CListView_VC50 : public CListView
{
public:
	POSITION GetFirstSelectedItemPosition() const;
	int GetNextSelectedItem(POSITION& pos) const;
};

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTVC50HELPERS_H__)