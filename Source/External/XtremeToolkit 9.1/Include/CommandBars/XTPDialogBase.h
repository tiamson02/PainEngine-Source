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
#if !defined(__XTPDIALOGBASE_H__)
#define __XTPDIALOGBASE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBars.h"
#include "XTPPaintManager.h"

//////////////////////////////////////////////////////////////////////
// Summary: CXTPDialogBase is a stand alone class. It represents the parent
//			class for a CXTPDialog class .
template <class TBase>
class CXTPDialogBase : public TBase
{
public:
	
	CXTPDialogBase(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		: TBase(nIDTemplate, pParentWnd)
	{
		m_pMenuBar = 0;
	}	
	CXTPDialogBase(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		: TBase(lpszTemplateName, pParentWnd)
	{
		m_pMenuBar = 0;
	}

	// Summary: Constructs a CXTPDialogBase object
	CXTPDialogBase()
	{
		m_pMenuBar = 0;
	}

	// Summary: Destroys a CXTPDialogBase object, handles cleanup and de-allocation
	~CXTPDialogBase()
	{
	}

	// Summary: Assigns menu bar of dialog.
	void SetMenuBar(CXTPCommandBar* pMenuBar)
	{
		m_pMenuBar = pMenuBar;
	}
	

protected:
//:Ignore
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (!XTPMouseManager()->IsMouseLocked() && !XTPMouseManager()->IsTrackedLock(0))
		{	
			if (m_pMenuBar && pMsg->message == WM_SYSKEYDOWN && (HIWORD(pMsg->lParam) & KF_ALTDOWN))
			{
				
				TCHAR chAccel = (TCHAR)MapVirtualKey((UINT)pMsg->wParam, 2);
				if( chAccel)
				{
					BOOL bSelectOnly;
					int nIndex = m_pMenuBar->FindAccelEx(chAccel, bSelectOnly);
					if (nIndex != -1)
					{
						if (bSelectOnly) 
						{
							m_pMenuBar->SetTrackingMode(TRUE, FALSE);
							m_pMenuBar->SetSelected(nIndex, TRUE); 
						} 
						else m_pMenuBar->SetPopuped(nIndex, TRUE);
						return TRUE;
					}
				}
			}
		}
		
		if (XTPMouseManager()->IsTrackedLock(0) && pMsg->message == WM_SYSKEYDOWN)
		{
			XTPMouseManager()->SendTrackLost();
			return TRUE;
		}
		
		if (XTPMouseManager()->IsTrackedLock(0) && 
			(pMsg->message == WM_CONTEXTMENU || pMsg->message == WM_SYSKEYUP))
		{
			return TRUE;
		}
	
		return TBase::PreTranslateMessage(pMsg);
	}
	
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		if (XTPMouseManager()->IsTrackedLock())
		{
			switch(message)
			{
			case WM_ACTIVATEAPP:
				if (!wParam) 
					XTPMouseManager()->SendTrackLost();
				return FALSE;
			case WM_SYSCOMMAND:			
				if (wParam == 0 || LOWORD(wParam) != 0)
					XTPMouseManager()->SendTrackLost();
				return FALSE;
			case WM_COMMAND:
				if (wParam == 0 || (LOWORD(wParam) != 0 && HIWORD(wParam) == 0))
					XTPMouseManager()->SendTrackLost();
				return FALSE;
			}
		}
		
		return TBase::OnWndMsg(message, wParam, lParam, pResult);
	}
//:End Ignore

private:
	CXTPCommandBar* m_pMenuBar;
};

// Summary: CXTPDialog is a CXTPDialogBase derived class.
class CXTPDialog : public CXTPDialogBase<CDialog> 
{
public:
	// Summary: Constructs a CXTPDialog object
	CXTPDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		: CXTPDialogBase<CDialog>(nIDTemplate, pParentWnd)
	{

	}	

	// Summary: Constructs a CXTPDialog object
	CXTPDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		: CXTPDialogBase<CDialog>(lpszTemplateName, pParentWnd)
	{
		
	}

	// Summary: Constructs a CXTPDialog object
	CXTPDialog()
	{
		
	}

};

#endif // #if !defined(__XTPDIALOGBASE_H__)