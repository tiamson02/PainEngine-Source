// XTDirWatcher.h : header file
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
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

//:Ignore
#if !defined(__XTDIRWATCHER_H__)
#define __XTDIRWATCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

/////////////////////////////////////////////////////////////////////////////
// Summary: CXTDirWatcher is a CWinThread derived class. This class monitors
//			a specified directory for file change notifications. When a file change
//			has been made a notification message is sent to a predefined notification 
//			window set by the user. Example of notifications are file or directory 
//			creation, file or directory deletion, name change, size change, attribute 
//			change and so on.
//
// Example: <p>To use CXTDirWatcher, instanciate a new thread using AfxBeginThread, 
//			add a CXTDirWatcher* m_pDirThread to your class header file:
//
//			<pre>
//			// Start the directory monitoring thread.
//			m_pDirThread = (CXTDirWatcher*)AfxBeginThread(
//				RUNTIME_CLASS(CXTDirWatcher), THREAD_PRIORITY_IDLE);
//
//			// Set the notification window handle.
//			if (m_pDirThread != NULL) {
//				m_pDirThread->SetNotifyWindow(GetSafeHandle());
//			}
//			</pre>
//
//			<p>You can then specify what directory you wish to monitor by calling SetFolderPath:
//
//			<pre>
//			if (m_pDirThread != NULL) {
//				m_pDirThread->SetFolderData(lptvid, lpsf);
//				}
//			</pre>
//			<p>You can add a message handler to the window receiving messages and handle your 
//			change event notifications:
//
//			<pre>
//			BEGIN_MESSAGE_MAP(CNotifyWnd, CWnd)
//				//{{AFX_MSG_MAP(CNotifyWnd)
//				//}}AFX_MSG_MAP
//				ON_MESSAGE(XTWM_SHELL_NOTIFY, OnUpdateShell)
//			END_MESSAGE_MAP()
//
//			LRESULT CNotifyWnd::OnUpdateShell(WPARAM wParam, LPARAM lParam)
//			{
//				switch (wParam)
//				{
//				case SHN_XT_LISTUPDATE:
//					{
//						// TODO: Handle update notification.
//						break;
//					}
//				default:
//					break;
//				}
//	
//				return 0;
//			}
//			</pre>
//
//			<p>When you are finished using the CXTDirWatcher thread, you will need to free the
//			memory associated with the object. To do so you can use the convient SAFE_DELETE
//			macro like so:
//
//			<pre>
//			SAFE_DELETE(m_pDirThread);
//			</pre>
//
//			<p>The CXTDirWatcher class is also used internally by the CXTShellListCtrl and 
//			CXTShellListView classes to handle refreshing the list when a file or directory
//			change occurs.</p>
class CXTDirWatcher : public CWinThread
{
	DECLARE_DYNCREATE(CXTDirWatcher)

public:

    // Summary: Constructs a CXTDirWatcher object 
	CXTDirWatcher();

    // Summary: Destroys a CXTDirWatcher object, handles cleanup and de-allocation 
	virtual ~CXTDirWatcher();

protected:

	HANDLE	       m_hNotify;       // Event object handle
	CString	       m_strFolderPath; // Path to monitor
	XT_TVITEMDATA* m_lptvid;        // Pointer to tree view item data.

public:

	// Input:	strPath - Full path of directory to monitor.
    // Summary: Call this member function to monitor the directory specified by strPath.
	bool SetFolderPath(CString strPath);

	// Summary: Call this member function to retrieve the full path to the directory that 
	//          is currently monitored.
	// Returns: A CString object.
	CString GetFolderPath() const;

	// Input:	pMainWnd - Pointer to the window to receive update notifications.
	//          lptvid - Pointer to tree view item data.
    // Summary: This member function is called by the CXTShellListCtrl to perform directory 
	//          monitoring using tree view item data defined by the associated CXTShellTreeCtrl.
	bool SetFolderData(CWnd* pMainWnd, XT_TVITEMDATA* lptvid);

	// Summary: This member function is called by the CXTShellListCtrl to used to return the 
	//          XT_TVITEMDATA data that was set for the directory that is currently monitored.
	// Returns: A pointer to a XT_TVITEMDATA structure.
	XT_TVITEMDATA* GetItemData() const;

	//:Ignore
	//{{AFX_VIRTUAL(CXTDirWatcher)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTDirWatcher)
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE XT_TVITEMDATA* CXTDirWatcher::GetItemData() const {
	return m_lptvid;
}
AFX_INLINE CString CXTDirWatcher::GetFolderPath() const {
	return m_strFolderPath;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTDIRWATCHER_H__)
//:End Ignore