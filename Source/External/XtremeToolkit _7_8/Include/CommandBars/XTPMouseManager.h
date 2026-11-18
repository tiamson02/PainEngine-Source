// XTPMouseManager.h : interface for the CXTPMouseManager class.
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
#if !defined(__XTPMOUSEMANAGER_H__)
#define __XTPMOUSEMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTPCommandBar;

// Internal class.

class _XTP_EXT_CLASS CXTPMouseManager : public CNoTrackObject
{
	friend class CXTPMouseManager* XTPMouseManager();

public:
	// Summary: Constructs a CXTPMouseManager object
	CXTPMouseManager(void);

public:

//:Ignore
	// Summary: Array of the tracking bars.
	class _XTP_EXT_CLASS CTrackArray : public CArray<CXTPCommandBar*, CXTPCommandBar*>
	{
	public :
		int Find(const CXTPCommandBar* pItem)
		{
			for (int i = 0; i < GetSize(); i++)
			{
				if (pItem == GetAt(i)) 
					return  i;
			}
			return -1;
		}
	};
	class _XTP_EXT_CLASS CTrustedArray : public CArray<HWND, HWND>
	{
	public :
		int Find(const HWND hWnd)
		{
			for (int i = 0; i < GetSize(); i++)
			{
				if (hWnd == GetAt(i)) 
					return  i;
			}
			return -1;
		}
		void Remove(HWND hWnd)
		{
			int i = Find(hWnd);
			if (i != -1) RemoveAt(i);
		}
	};
//:End Ignore	

public:

	// Summary: Destroys a CXTPMouseManager object, handles cleanup and de-allocation.
	~CXTPMouseManager(void);

	// Input:	pTrack - Points to a CXTPCommandBar object
	//			bCaptureMouse - TRUE to capture the mouse.
	// Summary: Adds a tracking bar.
	void SetTrack(CXTPCommandBar* pTrack, BOOL bCaptureMouse = TRUE);
	
	// Input:	pTrack - Points to a CXTPCommandBar object
	// Summary: Removes the tracking bar.
	void RemoveTrack(CXTPCommandBar* pTrack);
	
	// Summary: Removes all tracking bars.
	void SendTrackLost();
	
	// Input:	hwnd - Window handle.
	// Summary: Hooks window for the mouse leave event.
	void TrackMouseLeave(HWND hwnd);

	// Input:	pItem - Points to a CXTPCommandBar object
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Checks if the command bar is tracked.
	BOOL IsTrackedLock(const CXTPCommandBar* pItem = NULL)
	{
		if (m_arrTracked.GetSize() > 0 && m_arrTracked.Find(pItem) == -1)
			return TRUE;
		return FALSE;
	}

	// Input:	pSelected - Points to a CXTPCommandBar object
	// Summary: Sets the selected command bar.
	void SetSelected(CXTPCommandBar* pSelected);
	
	// Input:	pSelected - Points to a CXTPCommandBar object
	// Summary: Removes the selected command bar.
	void RemoveSelected(CXTPCommandBar* pSelected);
	
	// Summary: Captures the mouse cursor.
	void LockMouseMove();
	
	// Summary: Unlocks the mouse cursor.
	void UnlockMouseMove();

	// Returns: TRUE if locked; otherwise returns FALSE
	// Summary: Checks if the mouse cursor locked.
	BOOL IsMouseLocked();
	
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: Check if the command bar needs to be expanded
	BOOL IsForceExpanded();
	
	// Input:	bForceExpanded - TRUE to expand bars.
	// Summary: Sets expanded state.
	void SetForceExpanded(BOOL bForceExpanded);
	
	// Input:	hWnd - Window handle.
	// Summary: Adds a window to the trusted window list.
	void AddTrustedWindow(HWND hWnd);
	
	// Input:	hWnd - Window handle.
	// Summary: Removes a window from the trusted window list.
	void RemoveTrustedWindow(HWND hWnd);

	static BOOL AFX_CDECL IsTopParentActive(HWND hWnd);


private:
	CTrackArray& GetTrackArray();
	void DeliverMessage(CXTPCommandBar* pCapture, WPARAM wParam, POINT pt);
	BOOL PreTranslateMouseEvents(WPARAM wParam, POINT point);
	void SetupHook(BOOL);
	
	static void CALLBACK TrackMouseTimerProc (HWND hWnd, UINT /*uMsg*/, UINT idEvent, DWORD /*dwTime*/);
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam );

private:
	int m_nLock;
	CTrackArray m_arrTracked;

#ifndef _XTP_COMPONENT
	static CThreadLocal<CXTPMouseManager> _xtpMouseThreadState;
#else
	static CXTPMouseManager _xtpMouseThreadState;
#endif
	
	HHOOK m_hHook;
	CXTPCommandBar* m_pSelected;
	HWND m_hwndLeave;
	BOOL m_bForceExpanded;
	CTrustedArray m_arrTrusted;

	friend class CXTPCustomizeSheet;
};


AFX_INLINE CXTPMouseManager* XTPMouseManager() { 
#ifndef _XTP_COMPONENT
	return CXTPMouseManager::_xtpMouseThreadState.GetData();
#else
	return &CXTPMouseManager::_xtpMouseThreadState;
#endif
}

AFX_INLINE CXTPMouseManager::CTrackArray& CXTPMouseManager::GetTrackArray() {
	return m_arrTracked;
}

AFX_INLINE void CXTPMouseManager::SetSelected(CXTPCommandBar* pSelected) {
	m_pSelected = pSelected;
}
AFX_INLINE void CXTPMouseManager::RemoveSelected(CXTPCommandBar* pSelected){
	if (m_pSelected == pSelected)
		m_pSelected = NULL;
}
AFX_INLINE void CXTPMouseManager::LockMouseMove() {
	m_nLock++;
}
AFX_INLINE void CXTPMouseManager::UnlockMouseMove() {
	m_nLock--;
}
AFX_INLINE	BOOL CXTPMouseManager::IsMouseLocked() {
	return m_nLock > 0;
}

AFX_INLINE	BOOL CXTPMouseManager::IsForceExpanded() {
	return m_bForceExpanded;
}
AFX_INLINE void CXTPMouseManager::SetForceExpanded(BOOL bForceExpanded) {
	m_bForceExpanded = bForceExpanded;
}

#endif //#if !defined(__XTPMOUSEMANAGER_H__)