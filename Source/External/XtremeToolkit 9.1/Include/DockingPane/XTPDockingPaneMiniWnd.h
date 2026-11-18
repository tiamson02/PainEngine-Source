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

//===========================================================================
// Remarks: CXTPDockingPaneMiniWnd is a multiple inheritance class derived from 
//			CMiniFrameWnd and CXTPDockingPaneBase. It is used internally as a 
//			MiniWnd container for CXTPDockingPaneBase derived classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneMiniWnd : public CMiniFrameWnd, public CXTPDockingPaneBase
{
protected:

	
	CXTPDockingPaneMiniWnd(CXTPDockingPaneLayout* pLayout);

	virtual void Copy(CXTPDockingPaneMiniWnd* pClone, CXTPPaneToPaneMap* pMap);

	void Init(CXTPDockingPaneBase* pPane, CRect rc);

protected:
	// Summary: Destroys a CXTPDockingPaneMiniWnd object, handles cleanup and de-allocation.
	~CXTPDockingPaneMiniWnd();
public:

	BOOL IsActive() {
		return m_bActive;
	}
	void Expand();

	static BOOL m_bShowPinButton;


protected:
	virtual void RemovePane(CXTPDockingPaneBase* pPane);
	virtual void OnChildConainerChanged(CXTPDockingPaneBase* pContainer);
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);
	virtual void SaveState(CXTPDockingPaneLayout* pLayout);
	virtual BOOL LoadState(CXTPDockingPaneLayout* pLayout, CMapWordToPtr* pMap);
	virtual void RecalcLayout(BOOL bNotify);
	virtual BOOL Create(CRect rc);
	virtual void OnCaptionButtonClick(CXTPDockingPaneCaptionButton* pButton);
	virtual BOOL IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* pButton);

	BOOL IsThemed();
	void OnThemedChanged();

	void RedrawPane();	
	void DoSlideStep(BOOL bActivate = FALSE);	
	void Collapse();

	void OnPinButtonClick();

protected:

//:Ignore

	//{{AFX_VIRTUAL(CXTPDockingPaneMiniWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneMiniWnd)
	afx_msg void OnClose();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnDestroy();	
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnIdleUpdateCmdUI();
	
	//}}AFX_MSG


protected:
	CXTPDockingPaneSplitterContainer* m_pTopContainer;
	BOOL m_bCloseEnabled;

	int m_nExpandedHeight;
	
	int m_nStepsCount;
	int m_nSlideStep;	
	int m_nDeactivationCount;

	BOOL m_bCollapsed;
	BOOL m_bSlideOut;

//:End Ignore

private:

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CXTPDockingPaneMiniWnd)

	friend class CXTPDockingPaneManager;	
	friend class CXTPDockingPaneLayout;		
};

#endif // #if !defined(__XTPDOCKINGPANEMINIWND_H__)
