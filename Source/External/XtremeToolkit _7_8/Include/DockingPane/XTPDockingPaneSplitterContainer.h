// XTPDockingPaneSplitterContainer.h : interface for the CXTPDockingPaneSplitterContainer class.
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
#if !defined(__XTPDOCKINGPANESPLITTERCONTAINER_H__)
#define __XTPDOCKINGPANESPLITTERCONTAINER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneDefines.h"

class CXTPDockingPaneSplitterContainer;
class CXTPDockingPaneLayout;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneSplitterWnd is a CWnd derived class. It is  
//			used internally for splitting Panes.
class _XTP_EXT_CLASS CXTPDockingPaneSplitterWnd: public CWnd
{
protected:

	// Input:	pContainer - Points to a parent CXTPDockingPaneSplitterContainer object
	//			pFirst - Points to a first splitting CXTPDockingPaneBase object
	//			pSecond - Points to a second splitting CXTPDockingPaneBase object
	// Summary: Constructs a CXTPDockingPaneSplitterWnd object
	CXTPDockingPaneSplitterWnd(CXTPDockingPaneSplitterContainer* pContainer, CXTPDockingPaneBase* pFirst, CXTPDockingPaneBase* pSecond);
	
	// Summary: Destroys a CXTPDockingPaneSplitterWnd object, handles cleanup and de-allocation
	~CXTPDockingPaneSplitterWnd();

private:
	CXTPDockingPaneManager* GetDockingPaneManager();
	void Reposition(CRect rc, CRect rcAvail);

private:
	CXTPDockingPaneSplitterContainer* m_pContainer;	
	CXTPDockingPaneBase* m_pFirst;
	CXTPDockingPaneBase* m_pSecond;


private:	
	
	//{{AFX_MSG(CXTPDockingPaneSplitterWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

private:
	DECLARE_MESSAGE_MAP();
	friend class CXTPDockingPaneSplitterContainer;	
};


//////////////////////////////////////////////////////////////////////
// Summary: CXTPDockingPaneSplitterContainer is a CXTPDockingPaneBase derived class. 
//			It is used internally as a splitter container for CXTPDockingPaneBase 
//			derived classes.
class _XTP_EXT_CLASS CXTPDockingPaneSplitterContainer : CXTPDockingPaneBase
{

private:
	CXTPDockingPaneSplitterContainer(CXTPDockingPaneBase* pPane, BOOL bHoriz, CFrameWnd* pFrame, CXTPDockingPaneLayout* pLayout);
	CXTPDockingPaneSplitterContainer(CXTPDockingPaneSplitterContainer* pClone, CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap);
	CXTPDockingPaneSplitterContainer(CXTPDockingPaneLayout* pLayout);

protected:

	// Summary: Destroys a CXTPDockingPaneSplitterContainer object, handles cleanup and de-allocation
	~CXTPDockingPaneSplitterContainer();

private:
	BOOL IsHoriz() { return m_bHoriz; }

private:
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual void OnParentConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual POSITION ContaintPain(CXTPDockingPaneBase* pPane);
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList);
	virtual BOOL IsEmpty();
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	void _Replace(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pReplace);
	void _InsertPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour = NULL, BOOL bAfter = TRUE);
	void _UpdateSplitters();
	BOOL _Before(CXTPDockingPaneBase* pPane, POSITION pos);

private:
	CXTPDockingPaneBaseList m_lstPanes;
	CList<CXTPDockingPaneSplitterWnd*, CXTPDockingPaneSplitterWnd*> m_lstSpliters;
	BOOL m_bHoriz;

private:
	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPaneSplitterWnd;
	friend class CXTPDockingPaneMiniWnd;
	friend class CXTPDockingPaneLayout;
};


#endif // #if !defined(__XTPDOCKINGPANESPLITTERCONTAINER_H__)