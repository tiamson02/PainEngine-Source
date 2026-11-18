// XTPPropertyGridItemExt.h
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
#if !defined(__XTPPROPERTYGRIDITEMEXT_H__)
#define __XTPPROPERTYGRIDITEMEXT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPPropertyGridItem.h"

class CXTPPropertyGridItemDate;

class CXTPPropertyGridInplaceMonthCal : public CWnd
{
public:
	CXTPPropertyGridInplaceMonthCal(CXTPPropertyGridItemDate* pItem)
	{
		m_pItem = pItem;
	}

	AFX_INLINE BOOL GetMinReqRect(RECT* pRect) const 
	{
		ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM) pRect);
	}

protected:
	void OnAccept();

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSelect(NMHDR* pNMHDR, LRESULT* pResult);


	DECLARE_MESSAGE_MAP();

protected:
	CXTPPropertyGridItemDate* m_pItem;

};

class _XTP_EXT_CLASS CXTPPropertyGridItemDate: public CXTPPropertyGridItem
{
public:
	CXTPPropertyGridItemDate(CString strCaption, COleDateTime& oleDate);
	CXTPPropertyGridItemDate(UINT nID, COleDateTime& oleDate);

public:
	void SetDate(const COleDateTime& oleDate);
	BOOL GetAsSystemTime(SYSTEMTIME& sysTime);
	
	const COleDateTime& GetDate();

	long GetDay();
	void SetDay(long nDay);

	long GetMonth();
	void SetMonth(long nMonth);
	
	long GetYear();
	void SetYear(long nYear);

protected:
	virtual void OnInplaceButtonDown();
	virtual void SetValue(CString strValue);

private:
	void Init(COleDateTime& oleDate);
protected:
	COleDateTime m_oleDate;

#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPropertyGridItemDate)
	
#endif

	friend class CXTPPropertyGridInplaceMonthCal;
};

AFX_INLINE const COleDateTime& CXTPPropertyGridItemDate::GetDate() {
	return m_oleDate;
}

#ifdef __AFXCTL_H__

class _XTP_EXT_CLASS CXTPPropertyGridItemPicture: public CXTPPropertyGridItem
{
public:
	CXTPPropertyGridItemPicture(CString strCaption);

public:
	
	CPictureHolder& GetPicture();

protected:
	virtual void OnInplaceButtonDown();
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);

protected:
	CPictureHolder m_olePicture;

#ifdef _XTP_ACTIVEX
	afx_msg void OleSetValue(const VARIANT* varValue);
	afx_msg const VARIANT OleGetValue();
#endif
};

AFX_INLINE CPictureHolder& CXTPPropertyGridItemPicture::GetPicture() {
	return m_olePicture;
}

#endif

#endif // #if !defined(__XTPPROPERTYGRIDITEMEXT_H__)