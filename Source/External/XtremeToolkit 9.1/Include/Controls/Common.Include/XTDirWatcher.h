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
// Summary: The CXTDirWatcher is a thread class that will monitor a specified 
//          path for file and directory changes to occur.  Whenever an item is
//          created, deleted or renamed the thread will send a notification to
//          the its owner in the form of a XTWM_SHELL_NOTIFY message.  The 
//          WPARAM value for the notification will be set to SHN_XT_TREESELCHANGE 
//          if the folder list contents require updating or SHN_XT_CONTENTSCHANGED 
//          if the tree contents require updating.  The LPARAM value is set to
//          the address of a XT_TVITEMDATA struction that contains information
//          the path monitored.
//
// Example: <p>To use CXTDirWatcher, add a CXTDirWatcher* m_pDirThread member
//          to your class and instantiate a new thread using AfxBeginThread:
//
//			<pre>
//			// Start the directory monitoring thread.
//			m_pDirThread = (CXTDirWatcher*)AfxBeginThread(RUNTIME_CLASS(CXTDirWatcher),
//				THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
//			</pre>
//
//			<p>You can then specify what directory you wish to monitor by calling SetFolderData
//          or SetFolderPath:
//
//			<pre>
//          // Specify the folder to monitor.
//			m_pDirThread->SetFolderData(this, lpTVID);
//			m_pDirThread->ResumeThread();
//			</pre>
//
//			<p>You can add a message handler to the window receiving messages and handle your 
//			change event notifications.  The thread will send two notifications, the wParam indicates
//          what event has occured.  If wParam equal HN_XT_REFRESHFOLDER this indicates that the 
//          folder list has changed and needs to be updated.  If wParam equals SHN_XT_REFRESHTREE this
//          indicates the folder tree has changed and needs to be updated.  The lParam value represents
//          a pointer to a XT_TVITEMDATA structure for the folder:
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
//					case SHN_XT_REFRESHFOLDER:
//					case SHN_XT_REFRESHTREE:
//					{
//						// Directory monitory thread has issued an update notification,
//						// refresh the list control.
//						XT_TVITEMDATA*	lpTVID = (XT_TVITEMDATA*)lParam;
//						ASSERT(lpTVID);
//						
//						PopulateListView(lpTVID, lpTVID->lpsfParent);
//						break;
//					}
//				
//					default:
//						break;
//				}
//				
//				return 0;
//			}
//			</pre>
//
//			<p>When you are finished using the CXTDirWatcher thread, you will need to free the
//			memory associated with the object. To do so you can use the SAFE_DELETE
//			macro like so:
//
//			<pre>
//			SAFE_DELETE(m_pDirThread);
//			</pre>
//
//			<p>The CXTDirWatcher class is used internally by the CXTShellListCtrl and 
//			CXTShellListView classes to handle refreshing the list when a file or directory
//			change occurs.</p>
//===========================================================================
class _XT_EXT_CLASS CXTDirWatcher : public CWinThread
{
	DECLARE_DYNCREATE(CXTDirWatcher)

public:

	//-----------------------------------------------------------------------
    // Summary: Constructs a CXTDirWatcher object 
	//-----------------------------------------------------------------------
	CXTDirWatcher();
	
	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTDirWatcher object, handles cleanup and de-allocation 
	//-----------------------------------------------------------------------
	virtual ~CXTDirWatcher();

protected:
	
	DWORD			m_dwWaitStatus;			// Wait status.
	HANDLE			m_dwChangeHandles[2];	// Change event handles.
	CString			m_strFolderPath;		// Path that is monitored.
	XT_TVITEMDATA	m_tvid;					// Tree view item data.

public:

	//-----------------------------------------------------------------------
	// Input:	pMainWnd - Points to the window to receive change notifications.
	//          lpszFolderPath - Full path of directory to monitor.
    // Summary: Call this member function to monitor the directory specified by strPath.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFolderPath(CWnd* pMainWnd, LPCTSTR lpszFolderPath);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the full path to the directory that 
	//          is currently monitored.
	// Returns: A CString object representing the path monitored.
	//-----------------------------------------------------------------------
	CString GetFolderPath() const;
	
	//-----------------------------------------------------------------------
	// Input:	pMainWnd - Pointer to the window to receive update notifications.
	//          lpTVID - Pointer to tree view item data.
    // Summary: Call this member function to monitor the directory using a pointer
	//          to a XT_TVITEMDATA pointer.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFolderData(CWnd* pMainWnd, XT_TVITEMDATA* lpTVID);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to return the a pointer to a XT_TVITEMDATA 
	//          structure for the directory that is currently monitored.
	// Returns: A pointer to a XT_TVITEMDATA structure.
	//-----------------------------------------------------------------------
	XT_TVITEMDATA* GetFolderData();

	//-----------------------------------------------------------------------
	// Input:	lpszFolderPath - Full path to directory.
	//          lpTVID - Reference to tree view item data.
	// Summary: Call this member function to construct a XT_TVITEMDATA structure
	//          from the specified path.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL GetFolderData(LPCTSTR lpszFolderPath, XT_TVITEMDATA& lpTVID);

	//-----------------------------------------------------------------------
	// Input:	lpszFolderPath - Full path to directory.
	// Summary: Call this member function to determine if the specified path
	//          is a valid directory name.
	// Returns: TRUE if the path is valid, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsPathValid(LPCTSTR lpszFolderPath);

	//:Ignore
	//{{AFX_VIRTUAL(CXTDirWatcher)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	//:End Ignore

protected:

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the thread whenever the monitored
	//          directory list needs to be refreshed.
	//-----------------------------------------------------------------------
	virtual void RefreshFolder();

	//-----------------------------------------------------------------------
	// Summary: This member function is called by the thread whenever the monitored
	//          directory tree needs to be refreshed.
	//-----------------------------------------------------------------------
	virtual void RefreshTree();

	//:Ignore
	//{{AFX_MSG(CXTDirWatcher)
	//}}AFX_MSG
	//:End Ignore

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CString CXTDirWatcher::GetFolderPath() const {
	return m_strFolderPath;
}
AFX_INLINE XT_TVITEMDATA* CXTDirWatcher::GetFolderData() {
	return &m_tvid;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTDIRWATCHER_H__)
//:End Ignore