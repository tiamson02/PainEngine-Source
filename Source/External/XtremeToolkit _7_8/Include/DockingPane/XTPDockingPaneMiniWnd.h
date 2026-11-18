// XTPDockingPaneMiniWnd.h : interface for the CXTPDockingPaneMiniWnd class.
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
#if !defined(__XTPDOCKINGPANEMINIWND_H__)
#define __XTPDOCKINGPANEMINIWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneBase.h"

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneMiniWnd is a multiple inheritance class derived from 
//			CMiniFrameWnd and CXTPDockingPaneBase. It is used internally as a 
//			MiniWnd container for CXTPDockingPaneBase derived classes.
class _XTP_EXT_CLASS CXTPDockingPaneMiniWnd : public CMiniFrameWnd, public CXTPDockingPaneBase
{
private:

	CXTPDockingPaneMiniWnd(CXTPDockingPaneBase* pPane, CRect rc, CXTPDockingPaneLayout* pLayout);
	CXTPDockingPaneMiniWnd(CXTPDockingPaneMiniWnd* pClone, CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap);
	CXTPDockingPaneMiniWnd(CXTPDockingPaneLayout* pLayout);

protected:
	// Summary: Destroys a CXTPDockingPaneMiniWnd object, handles cleanup and de-allocation.
	~CXTPDockingPaneMiniWnd();


private:
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	void RecalcLayout(BOOL bNotify);
	BOOL Create(CRect rc);

private:

	//{{AFX_VIRTUAL(CXTPDockingPaneMiniWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneMiniWnd)
	afx_msg void OnClose();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnDestroy();		
	//}}AFX_MSG


private:
	CXTPDockingPaneSplitterContainer* m_pTopContainer;
	BOOL m_bCloseEnabled;


private:

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPDockingPaneMiniWnd)

	friend class CXTPDockingPaneManager;	
	friend class CXTPDockingPaneLayout;		
};

#endif // #if !defined(__XTPDOCKINGPANEMINIWND_H__)
