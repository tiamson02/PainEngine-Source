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

//===========================================================================
// Remarks: CXTPDockingPaneSplitterWnd is a CWnd derived class. It is  
//			used internally for splitting Panes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneSplitterWnd: public CWnd
{
protected:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDockingPaneSplitterWnd object
	//-----------------------------------------------------------------------
	CXTPDockingPaneSplitterWnd();

public:	
	
	BOOL IsHorizontal();

	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a parent CXTPDockingPaneSplitterContainer object
	//			pFirst     - Points to a first splitting CXTPDockingPaneBase object
	//			pSecond    - Points to a second splitting CXTPDockingPaneBase object 
	//-----------------------------------------------------------------------
	void Create(CXTPDockingPaneSplitterContainer* pContainer, CXTPDockingPaneBase* pFirst, CXTPDockingPaneBase* pSecond);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneSplitterWnd object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
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


//===========================================================================
// Remarks: CXTPDockingPaneSplitterContainer is a CXTPDockingPaneBase derived class. 
//			It is used internally as a splitter container for CXTPDockingPaneBase 
//			derived classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneSplitterContainer : public CXTPDockingPaneBase
{

protected:
	CXTPDockingPaneSplitterContainer(CXTPDockingPaneLayout* pLayout);

	void Init(CXTPDockingPaneBase* pPane, BOOL bHoriz, CFrameWnd* pFrame);
	void Copy(CXTPDockingPaneSplitterContainer* pClone, CXTPPaneToPaneMap* pMap);

protected:

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneSplitterContainer object, handles 
	//			cleanup and de-allocation
	//-----------------------------------------------------------------------
	~CXTPDockingPaneSplitterContainer();

public:
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if there are child panes.
	// Returns: TRUE if there are no child panes; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty();

	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CFrameWnd object
	//			rect    - Rectangle of the pane to be set
	//			lParam  - Pointer to AFX_SIZEPARENTPARAMS structure
	// Summary: This method is called to set the position of the pane.
	//-----------------------------------------------------------------------
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);


protected:
	virtual CXTPDockingPaneSplitterWnd* OnCreateSplitter();

protected:
	BOOL IsHoriz() { return m_bHoriz; }

protected:
	
	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object.
	// Summary: Call this member to remove a pane fromt the collection.
	//-----------------------------------------------------------------------
	virtual void RemovePane(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object.
	// Summary: This member is called when a child pane collection is changed.
	//-----------------------------------------------------------------------
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object.
	// Summary: This member is called when the parent pane collection is changed.
	//-----------------------------------------------------------------------
	virtual void OnParentConainerChanged(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object.
	// Summary: Call this member to determine if one pane is contained within
	//			the collection of another.
	// Returns: POSITION of the pane if the collection is successful; otherwise
	//			returns NULL.
	//-----------------------------------------------------------------------
	virtual POSITION ContaintPain(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Input:	type  - Type of the panes to find.
	//			pList - Points to a CXTPDockingPaneBaseList object that will
	//					contain found panes.
	// Summary: Call this member to retrieve a collection of the panes with
	//			the specified type.
	//-----------------------------------------------------------------------
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList);
	
	//-----------------------------------------------------------------------
	// Input:	pLayout          - Points to a CXTPDockingPaneLayout object.
	//			pMap             - Points to a CXTPPaneToPaneMap object.
	//			dwIgnoredOptions - TRUE to ignore the options of the pane.
	// Summary: This member is called to clone the pane.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Call this member to store information about the pane.
	//-----------------------------------------------------------------------
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	//			pMap	- Points to a CMapWordToPtr object.
	// Summary: Call this member to store information about the pane.
	//-----------------------------------------------------------------------
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);

	void _Replace(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pReplace);
	void _InsertPane(CXTPDockingPaneBase* pPane, CXTPDockingPaneBase* pNeighbour = NULL, BOOL bAfter = TRUE);
	void _UpdateSplitters();
	BOOL _Before(CXTPDockingPaneBase* pPane, POSITION pos);

protected:
	CXTPDockingPaneBaseList m_lstPanes;
	CList<CXTPDockingPaneSplitterWnd*, CXTPDockingPaneSplitterWnd*> m_lstSpliters;
	BOOL m_bHoriz;
	BOOL m_bRecalcLayout;

private:
	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPaneSplitterWnd;
	friend class CXTPDockingPaneMiniWnd;
	friend class CXTPDockingPaneLayout;
};


#endif // #if !defined(__XTPDOCKINGPANESPLITTERCONTAINER_H__)