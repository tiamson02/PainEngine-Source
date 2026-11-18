// XTPFrameWnd.h : interface for the CXTPFrameWnd and CXTPMDIFrameWnd classes.
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
#if !defined(__XTPFRAMEWND_H__)
#define __XTPFRAMEWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBars.h"
#include "XTPPaintManager.h"

//////////////////////////////////////////////////////////////////////
// Summary: CXTPFrameWndBase is a stand alone class. It represents the parent
//			class for a CXTPMDIFrameWnd and CXTPFrameWnd classes .
template <class TBase>
class CXTPFrameWndBase : public TBase
{
public:
	// Summary: Constructs a CXTPFrameWndBase object
	CXTPFrameWndBase()
	{
		m_pCommandBars = 0;
	}
	// Summary: Destroys a CXTPFrameWndBase object, handles cleanup and de-allocation
	~CXTPFrameWndBase()
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->InternalRelease();
		}
	}
	// Returns: Nonzero if successful; otherwise 0.
	// Summary: Creates command bars.
	BOOL InitCommandBars(CRuntimeClass* pCommandBarsClass = RUNTIME_CLASS(CXTPCommandBars))
	{
		if (XTPPaintManager() == NULL)
		{		
			CXTPPaintManager::SetTheme(xtpThemeOffice2000);
		}

		ASSERT(pCommandBarsClass->IsDerivedFrom(RUNTIME_CLASS(CXTPCommandBars)));
		m_pCommandBars =  (CXTPCommandBars*) pCommandBarsClass->CreateObject();
		ASSERT(m_pCommandBars);
		m_pCommandBars->SetSite(this);
		m_pCommandBars->EnableDocking();
		return TRUE;
	}

	// Input:	pBarToDock - A CXTPToolBar pointer to the toolbar to be docked.
	//			pBarOnLeft - A CXTPToolBar pointer to the already docked toolbar.
 	// Summary:	This member function will redock a toolbar specified by 'pBarToDock'
	//			to the right of a newly docked toolbar specified by 'pBarOnLeft'. 
   	void DockRightOf(CXTPToolBar* pBarToDock, CXTPToolBar* pBarOnLeft)
	{
		RecalcLayout(TRUE);
		CXTPWindowRect rcBar(pBarOnLeft);
		rcBar.OffsetRect(rcBar.Width(), 0);		
		GetCommandBars()->DockCommandBar(pBarToDock, rcBar, pBarOnLeft->GetDockBar());
	}
	
	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored.
	// Summary: Call this function to save the state information to the registry or .INI file.
	void SaveCommandBars(LPCTSTR lpszProfileName)
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->SaveOptions(lpszProfileName);
			m_pCommandBars->SaveBarState(lpszProfileName);
		}		
	}

	// Input:	lpszProfileName - Points to a null-terminated string that specifies the name of a 
	//			section in the initialization file or a key in the Windows registry where state 
	//			information is stored. 
	// Summary: Call this function to retrieve state information from the registry or .INI file.
	void LoadCommandBars(LPCTSTR lpszProfileName, BOOL bSilent = FALSE)
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->LoadOptions(lpszProfileName);
			m_pCommandBars->LoadBarState(lpszProfileName, bSilent);
		}		
	}
	// Summary: Retrieves Command Bars object.
	CXTPCommandBars* GetCommandBars() { return m_pCommandBars; }

protected:
//:Ignore
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (m_pCommandBars && m_pCommandBars->PreTranslateFrameMessage(pMsg))
			return TRUE;
		
		return TBase::PreTranslateMessage(pMsg);

	}
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		if (m_pCommandBars && m_pCommandBars->OnFrameWndMsg(message, wParam, lParam, pResult))
			return TRUE;

		if (message == WM_XTP_COMMAND)
		{
			LRESULT lResult = FALSE;
			NMHDR* pNMHDR = (NMHDR*)lParam;

			UINT nID = (UINT)pNMHDR->idFrom;
			int nCode = pNMHDR->code;

			AFX_NOTIFY notify;
			notify.pResult = &lResult;
			notify.pNMHDR = pNMHDR;
			if (OnCmdMsg(nID, MAKELONG(nCode, WM_NOTIFY), &notify, NULL))
			{
				*pResult = lResult;
				return TRUE;
			}
			return FALSE;
		}

		return TBase::OnWndMsg(message, wParam, lParam, pResult);
	}
//:End Ignore

private:
	CXTPCommandBars* m_pCommandBars;
};

// Summary: CXTPMDIFrameWnd is a CXTPFrameWndBase derived class.
class CXTPMDIFrameWnd : public CXTPFrameWndBase<CMDIFrameWnd>
{
public:
	// Summary: Constructs a CXTPMDIFrameWnd object
	CXTPMDIFrameWnd()
	{

	}
	
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo)
	{	
		if (nID >= XTP_ID_WORKSAPCE_MOVEPREVIOUS && nID <= XTP_ID_WORKSAPCE_NEWVERTICAL)
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(m_hWndMDIClient);
			if (pWnd && pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
		}
		// then pump through normal frame
		return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}	

};
// Summary: CXTPFrameWnd is a CXTPFrameWndBase derived class.
class CXTPFrameWnd : public CXTPFrameWndBase<CFrameWnd> 
{
public:
	// Summary: Constructs a CXTPFrameWnd object
	CXTPFrameWnd()
	{

	}	
};

#endif // #if !defined(__XTPFRAMEWND_H__)