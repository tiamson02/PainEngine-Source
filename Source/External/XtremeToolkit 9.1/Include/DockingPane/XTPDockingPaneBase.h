// XTPDockingPaneBase.h : interface for the CXTPDockingPaneBase class.
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
#if !defined(__XTPDOCKINGPANEBASE_H__)
#define __XTPDOCKINGPANEBASE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPDockingPaneDefines.h"

class CXTPDockingPaneBase;
class CXTPDockingPane;
class CXTPDockingPaneTabbedContainer;
class CXTPDockingPaneLayout;
class CXTPDockingPaneManager;
class CXTPDockingPanePaintManager;
class CXTPDockingPaneCaptionButton;

class _XTP_EXT_CLASS CXTPDockingPaneCaptionButtons: public CArray <CXTPDockingPaneCaptionButton*, CXTPDockingPaneCaptionButton*>
{
public:
	BOOL CheckForMouseOver(CPoint pt);
};


//===========================================================================
// Remarks: CXTPDockingPaneBase is a stand alone class. It represents the parent
//			class for a Docking Pane and its containers.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneBase
{
protected:

	//-----------------------------------------------------------------------
	// Summary: Protected constructor. You cannot create this class.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout);

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine the type of the pane.
	// BULLETED LIST:

	// Returns: The type of the Pane. It can be one of the following:
	//			[ul]
	//			[li]<b>typeDockingPane</b> - If it is a docking pane.[/li]
	//			[li]<b>typeTabbedContainer</b> - If it is a tabbed container for pane.[/li]
	//			[li]<b>typeSplitterContainer</b> - If it is a splitter container.[/li]
	//			[li]<b>typeMiniWnd</b> - If it is a floating window container.[/li]
	//			[li]<b>typeClient</b> - If it is a container for client area.[/li]
	//			[li]<b>typeAutoHidePanel</b> - If it is an auto-hide panel container.[/li]
	//			[/ul]
	//-----------------------------------------------------------------------
	XTPDockingPaneType GetType();

	//-----------------------------------------------------------------------
	// Summary: The GetWindowRect function retrieves the dimensions of the 
	//			bounding rectangle of the pane.
	// Returns: The visible rectangle of the pane.
	//-----------------------------------------------------------------------
	CRect GetWindowRect();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the parent container of 
	//			the pane.
	// Returns: The parent container.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetContainer();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the pane manager
	// Returns:	A pointer to a CXTPDockingPaneManager object
	//-----------------------------------------------------------------------
	CXTPDockingPaneManager* GetDockingPaneManager();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the paint manager
	// Returns: A pointer to a CXTPDockingPanePaintManager object
	//-----------------------------------------------------------------------
	CXTPDockingPanePaintManager* GetPaintManager();

	//-----------------------------------------------------------------------
	// Input:	nID - Identifier of the button
	// Summary: Call this member to find the specified caption button 
	// Returns: The CXTPDockingPaneCaptionButton object if successful; otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton* FindCaptionButton(int nID);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get a reference to the caption's close/
	//			popup button
	// Returns: A reference to a CXTPDockingPaneCaptionButtons object
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButtons* GetCaptionButtons() {
		return &m_arrCaptionButtons;
	}

	//-----------------------------------------------------------------------
	// Summary: Call this member to redraw the pane.
	//-----------------------------------------------------------------------
	virtual void RedrawPane();

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	// Summary: Call this member to determine if one pane is contained in the 
	//			collection of another.
	// Returns: POSITION of the pane in the collection if successful; otherwise 
	//			returns NULL.
	//-----------------------------------------------------------------------
	virtual POSITION ContaintPain(CXTPDockingPaneBase* pPane); 

	//-----------------------------------------------------------------------
	// Input:	type  - Type of the panes to find.
	//			pList - Points to a CXTPDockingPaneBaseList object that will 
	//					contain found panes.
	// Summary: Call this member to retrieve a collection of the panes with 
	//			specified type.
	//-----------------------------------------------------------------------
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList);
	
protected:

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneBase object, handles cleanup and 
	//			de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneBase();
	
	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CFrameWnd object.
	//			rect - Rectangle of the pane to be set.
	//			lParam - Pointer to AFX_SIZEPARENTPARAMS structure.
	// Summary: This method is called to set position of the pane.
	//-----------------------------------------------------------------------
	virtual void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);
	
	//-----------------------------------------------------------------------
	// Summary: This method is called to update the state of the pane.
	//-----------------------------------------------------------------------
	virtual void OnIdleUpdate();
	
	
	//-----------------------------------------------------------------------
	// Input:	pFrame - Points to a CFrameWnd object
	// Summary: Call this member to set the parent frame of the pane.
	//-----------------------------------------------------------------------
	virtual void SetParentFrame(CFrameWnd* pFrame);
	
	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object
	// Summary: Call this method to set the parent container of the pane.
	//-----------------------------------------------------------------------
	virtual void SetParentContainer(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object that is changed.
	// Summary: This member is called when a child pane collection is changed.
	//-----------------------------------------------------------------------
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	
	//-----------------------------------------------------------------------
	// Input:	pContainer - Points to a CXTPDockingPaneBase object that is changed.
	// Summary: This member is called when the parent pane collection is changed.
	//-----------------------------------------------------------------------
	virtual void OnParentConainerChanged(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	// Summary: Call this member to store information about the pane.
	//-----------------------------------------------------------------------
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	//			pMap - Points to a CMapWordToPtr object.
	// Summary: Call this member function to restore the settings of the pane.
	// Returns: TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Points to a CXTPDockingPaneLayout object.
	//			pMap - Points to a CXTPPaneToPaneMap object.
	//			dwIgnoredOptions - TRUE to ignore options of the pane.
	// Summary: This member is called to clone the pane.
	// Returns: A pointer to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);

	//-----------------------------------------------------------------------
	// Input:	pPane - Points to a CXTPDockingPaneBase object
	// Summary: Call this member to remove a pane from collection.
	//-----------------------------------------------------------------------
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if there are children panes
	// Returns: TRUE if no children panes; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty();

	//-----------------------------------------------------------------------
	// Summary: This member is called when the dimensions of the caption button
	//			are changed.
	//-----------------------------------------------------------------------
	virtual void OnCaptionButtonRectChanged(CXTPDockingPaneCaptionButton* ) {

	}
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if the caption button is visible.
	// Returns: TRUE if the caption button is visible; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* /*pButton*/) {
		return TRUE;
	}
	
	//-----------------------------------------------------------------------
	// Summary: Returns the window handle for a window. 
	// Returns: Returns NULL if the HWND is not attached to a window or if it 
	//			is used with a NULL HWND pointer.
	//-----------------------------------------------------------------------
	virtual HWND GetSafeHwnd() {
		return 0;
	}

protected:
	CSize m_szDocking;							// Size of the pane.
	CXTPDockingPaneLayout* m_pLayout;			// Parent layout.
	
	CXTPDockingPaneBase* m_pParentContainer;	// Parent container.
	CFrameWnd* m_pParentFrame;					// Parent frame.
	XTPDockingPaneType m_type;					// Type of the pane.
	int m_nIndex;								// Index in saved list.
	CRect m_rcWindow;							// Position of the pane.

	CXTPDockingPaneCaptionButtons m_arrCaptionButtons;

private:
	friend class CXTPDockingPaneManager;			
	friend class CXTPDockingPaneSplitterContainer;  
	friend class CXTPDockingPaneTabbedContainer;	
	friend class CXTPDockingPaneMiniWnd;			
	friend class CXTPDockingPaneSplitterWnd;		
	friend class CXTPDockingPaneContext;			
	friend class CXTPDockingPane;					
	friend class CXTPDockingPaneLayout;				
	friend class CXTPDockingPaneAutoHidePanel;		
	friend class CXTPDockingPaneAutoHideWnd;		
	friend class CXTPDockingPaneCaptionButton;

};

AFX_INLINE XTPDockingPaneType CXTPDockingPaneBase::GetType() { 
	return m_type; 
}
AFX_INLINE CXTPDockingPaneBase* CXTPDockingPaneBase::GetContainer() { 
	return m_pParentContainer; 
}
AFX_INLINE void CXTPDockingPaneBase::SetParentFrame(CFrameWnd* pFrame) { 
	m_pParentFrame = pFrame; 
}
AFX_INLINE void CXTPDockingPaneBase::SetParentContainer(CXTPDockingPaneBase* pContainer) { 
	m_pParentContainer = pContainer;	
}

#endif // #if !defined(__XTPDOCKINGPANEBASE_H__)
