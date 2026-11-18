// XTShellListBase.h : header file
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
#ifndef __XTSHELLLISTBASE_H__
#define __XTSHELLLISTBASE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTDirWatcher;

//===========================================================================
// Remarks: CXTShellListBase is a multiple inheritance class derived from CXTListView
//			and CXTShellPidl. It is used to create a CXTShellListBase class object.
//===========================================================================
class _XT_EXT_CLASS CXTShellListBase : public CXTListBase, public CXTShellPidl
{
public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTShellListBase object
	//-----------------------------------------------------------------------
    CXTShellListBase();

	//-----------------------------------------------------------------------
    // Summary: Destroys a CXTShellListBase object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTShellListBase();

protected:

	UINT				m_uFlags;		// Flags indicating which items to include in the enumeration.
	BOOL				m_bUninitOLE;	// TRUE if OleUninitialize has to be called. 
	BOOL				m_bContextMenu; // TRUE to display the shell context menu on right item click.
	LPITEMIDLIST		m_pidlINet;		// Points to the CSIDL_INTERNET folder location.
	CXTShellSettings	m_shSettings;	// Contains SHELLFLAGSTATE info.
	CXTDirWatcher*		m_pDirThread;	// Thread used to monitor directory activity.
	int					m_nNameColumnWidth;

	//-----------------------------------------------------------------------
	// Input:	lptvid - Pointer to tree view item data.
	//			lpsf   - Pointer to the parent shell folder.
    // Summary:	This member function adds items to the list view. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL InitListViewItems(XT_TVITEMDATA* lptvid,LPSHELLFOLDER lpsf);

	//-----------------------------------------------------------------------
	// Input:	lpifq	 - Fully qualified item ID list for the current item.
	//			lptvitem - Pointer to the tree view item that is about to be added to the tree.
    // Summary:	This member function gets the index for the normal and selected
	//			icons of the current item.
	//-----------------------------------------------------------------------
    virtual void GetNormalAndSelectedIcons(LPITEMIDLIST lpifq,LPTV_ITEM lptvitem);

public:

	//-----------------------------------------------------------------------
	// Input:	bEnable - TRUE to display a context menu.
    // Summary:	Call this member function to enable or disable the display of the
	//			shell context menu on the right click of the item.
	//-----------------------------------------------------------------------
    virtual void EnableContextMenu(BOOL bEnable);

	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	uFlags - Determines the type of items included in an enumeration and can
	//					 be one or more of the following:
    //			[ul]
    //			[li]<b>SHCONTF_FOLDERS</b> Include items that are folders in
	//			the enumeration.[/li]
    //			[li]<b>SHCONTF_NONFOLDERS</b> Include items that are not folders
	//			in the enumeration.[/li]
    //			[li]<b>SHCONTF_INCLUDEHIDDEN</b> Include hidden items in the
	//			enumeration.[/li]
    //			[li]<b>SHCONTF_INIT_ON_FIRST_NEXT</b> IShellFolder::EnumObjects
	//			can return without validating the enumeration object. Validation
	//			can be postponed until the first call to IEnumIDList::Next.
	//			This flag is intended to be used when a user interface may be
	//			displayed prior to the first IEnumIDList::Next call. For a
	//			user interface to be presented, 'hwndOwner' must be set to a valid
	//			window handle.[/li]
    //			[li]<b>SHCONTF_NETPRINTERSRCH</b> The caller is looking for
	//			printer objects.[/li]
    //			[/ul]
    // Summary:	Call this member function to determine the type of items included
	//			in the shell enumeration. The default is SHCONTF_FOLDERS | SHCONTF_NONFOLDERS.
	//-----------------------------------------------------------------------
    virtual void SetEnumFlags(UINT uFlags);

	//-----------------------------------------------------------------------
    // Summary:	This member function associates the system image list with the list
	//			control. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL InitSystemImageLists();

	//-----------------------------------------------------------------------
	// Input:	lptvid - Pointer to TreeView item data.
	//			lpsf   - Pointer to the parent shell folder.
    // Summary:	This member function populates the list view control. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL PopulateListView(XT_TVITEMDATA* lptvid,LPSHELLFOLDER lpsf);

	//-----------------------------------------------------------------------
    // Summary:	This member function returns the index of the list view item that
	//			was double clicked on.  
	// Returns: The index of the item that was double clicked; or -1, if the item 
	//			was not found.
	//-----------------------------------------------------------------------
    virtual int GetDoubleClickedItem();

	//-----------------------------------------------------------------------
    // Summary:	This member function displays the system popup menu for the selected
	//			item or folder.  
	// Returns: The index of the item selected; or -1, if the item was not found.
	//-----------------------------------------------------------------------
    virtual int GetContextMenu();

	//-----------------------------------------------------------------------
	// Input:	iItem - Index of the list view item clicked on.
    // Summary:	This member function executes the item clicked on
	//			in the list control via the Shell.  
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
    virtual bool ShellOpenItem(int iItem);

	//-----------------------------------------------------------------------
	// Input:	lplvid - Pointer to the list view item data.
    // Summary:	This member function executes the item clicked on
	//			in the list control via the Shell.  
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	virtual bool ShellOpenItem(XT_LVITEMDATA* lplvid);

	//-----------------------------------------------------------------------
	// Input:	iItem		- Index of the list view item to get the path of.
	//			strItemPath - Reference to a CString object that receives the path string.
    // Summary:	This member function retrieves the path for the specified list view
	//			item. 
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
    virtual BOOL GetItemPath(int iItem,CString &strItemPath);

	//-----------------------------------------------------------------------
    // Summary: This member function creates default columns for the list view
	//-----------------------------------------------------------------------
    virtual void BuildDefaultColumns();

	//-----------------------------------------------------------------------
	// Input:	nCol		- Passed in from the control. The index of the column clicked.
	//			bAscending  - Passed in from the control. true if the sort order should be ascending.
    // Summary:	Override this member function in your derived class to perform custom
	//			sort routines. 
	// Returns: true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
    virtual bool SortList(int nCol,bool bAscending );

    //:Ignore
	//{{AFX_VIRTUAL(CXTShellListBase)
	public:
	protected:
	
	virtual void UpdateList(int nMessage, XT_TVITEMDATA* pItemData);
	//}}AFX_VIRTUAL

	virtual void OnDragDrop(NM_LISTVIEW* pNMListView);
	virtual TCHAR* InsertCommas(LONGLONG value, TCHAR* szBufferOut, UINT nSize);
	
	virtual void InitializeControl();
	//:End Ignore

protected:

	//-----------------------------------------------------------------------
	// Input:	iItem		 - Index of the item to set the attributes for.
	//			dwAttributes - Flags retrieved from SHELLFOLDER::GetAttributesOf.
    // Summary:	This member function sets the shell attribute flags for the specified
	//			list item.
	//-----------------------------------------------------------------------
    virtual void SetAttributes(int iItem,DWORD dwAttributes);

    //:Ignore
	//{{AFX_MSG(CXTShellListBase)
	afx_msg void OnRclickImpl(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	//:End Ignore
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTShellListBase::SetEnumFlags(UINT uFlags) {
	m_uFlags = uFlags;
}
AFX_INLINE void CXTShellListBase::EnableContextMenu(BOOL bEnable) {
	m_bContextMenu = bEnable;
}



#define DECLATE_SHELLLIST_BASE(ClassName, List, Base)\
DECLATE_LIST_BASE(Base##List, List, Base)\
class _XT_EXT_CLASS ClassName : public Base##List\
{\
protected:	\
	void OnRclick(NMHDR* pNMHDR, LRESULT* pResult) {\
		OnRclickImpl(pNMHDR, pResult);\
	}\
	void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) {\
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;\
		OnDragDrop(pNMListView);	\
		*pResult = 0;\
	}	\
};

#define ON_SHELLLIST_REFLECT\
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)\
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)\
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)\
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnBeginDrag)\
	ON_LISTCTRL_REFLECT

//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __XTSHELLLISTBASE_H__
//:End Ignore