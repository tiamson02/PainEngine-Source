#include "StdAfx.h"
#include "Resource.h"
#include "ScriptExplorerDlg.h"
#include "MainFrm.h"
#include "PainEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDockWinEx1

CDockWinEx1::CDockWinEx1()
{
	// TODO: add construction code here.
}

CDockWinEx1::~CDockWinEx1()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNAMIC(CDockWinEx1, CXTDockWindow)

BEGIN_MESSAGE_MAP(CDockWinEx1, CXTDockWindow)
	//{{AFX_MSG_MAP(CDockWinEx1)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_COMMAND(XT_IDC_BTN_CLOSE, OnButtonClose)
	ON_UPDATE_COMMAND_UI(XT_IDC_BTN_CLOSE, OnUpdateButtonClose)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CDockWinEx1::OnButtonClose()
{
	GetDockingFrame()->ShowControlBar(this,FALSE,FALSE);
}

void CDockWinEx1::OnUpdateButtonClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

/////////////////////////////////////////////////////////////////////////////
// CDockWinEx1 message handlers

int CDockWinEx1::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTDockWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the tree control

#if _MSC_VER <= 1200 // MFC 6.0 or earlier
	if (!m_treeCtrl.CreateEx(WS_EX_STATICEDGE, WC_TREEVIEW, NULL,
		WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,
		CRect(0,0,0,0), this, 0x0))
#else
	if (!m_treeCtrl.CreateEx(WS_EX_STATICEDGE,
		WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT,
		CRect(0,0,0,0), this, 0x0))
#endif // MFC 6.0 or earlier
	{
		TRACE0("Failed to create tree control.\n");
		return -1;
	}

	// Create the image list used by the tree control.
	if (!m_imageList.Create(IDB_TREE, 16, 1, RGB(0x00,0x80,0x80)))
		return -1;

	m_treeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);


	// Set the font for the tree control.
	//m_treeCtrl.SetFont (&xtAfxData.font);

	// Set the child for the docking window.
	SetChild (&m_treeCtrl);

	// Add items to the tree control.
	AddTreeItems();

	return 0;
}

void CDockWinEx1::AddDirectory(CString path, HTREEITEM parent)
{
	FileManagerStd fm;
	DynamicArray<String> dirs;	
    DynamicArray<String> files;	

	fm.FindFiles(path+"*.*",dirs,EFindDirs);
	for (int i=0;i<dirs.Size();i++)
	{
		HTREEITEM htItem = m_treeCtrl.InsertItem(dirs[i].AsChar(),1,1,parent);
		AddDirectory(path+dirs[i].AsChar()+"/", htItem);
	}	
	
	fm.FindFiles(path+"*.*",files);
	for (int i=0;i<files.Size();i++)
	{
		if (!strstr(files[i].AsChar(),".scc"))
			HTREEITEM htItem = m_treeCtrl.InsertItem(files[i].AsChar(),3,3,parent);
	}

	if (m_treeCtrl.ItemHasChildren(parent))
	{
		//m_treeCtrl.Expand(parent,TVE_EXPAND);
		//m_treeCtrl.SetItemImage(parent,2,2);
	}
}

void CDockWinEx1::AddTreeItems()
{
	AddDirectory("../Data/LScripts/",NULL);
}

void CDockWinEx1::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CXTDockWindow::OnWindowPosChanged(lpwndpos);
/*	
	// TODO: Add your message handler code here
	if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		TRACE0("SWP_HIDEWINDOW\n");
	}
	else if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		TRACE0("SWP_SHOWWINDOW\n");
	}
	*/
}

BOOL CDockWinEx1::PreTranslateMessage(MSG* pMsg) 
{
	UpdateDialogControls( this, TRUE );
	return CXTDockWindow::PreTranslateMessage(pMsg);	
}


BEGIN_MESSAGE_MAP(CSETreeControl, CXTTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK,OnRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
END_MESSAGE_MAP()

void CSETreeControl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;	
	HTREEITEM  pSel = GetSelectedItem();
	if (!pSel) return;
	
	int iN,iS;
	GetItemImage(pSel,iN,iS);
	switch (iN)
	{
		case 1: // zwiniety katalog
			if (ItemHasChildren(pSel)) 
				SetItemImage(pSel,2,2);
		break;
		
		case 2: // rozwiniety katalog
			SetItemImage(pSel,1,1);
			// tu bedzie rozwijanie automatyczne  z dysku
		break;		
		
		case 3: // item
			OnCommand(1,0);
		break;
	}; 
	*pResult = 0;
}

void CSETreeControl::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint CursorPos;
	GetCursorPos(&CursorPos);
	ScreenToClient(&CursorPos);

	unsigned int uFlags;
	HTREEITEM hI = HitTest(CursorPos, &uFlags);	
	if (hI)
	{
		Select(hI,TVGN_CARET);

		int i,si;
		GetItemImage(hI,i,si);
		if (i==3)
		{
			CMenu popMenu;
			popMenu.CreatePopupMenu();	  
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, 1, "Internal Edit");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, 2, "External Edit");
			popMenu.SetDefaultItem(0,true);

			POINT pt;  
			GetCursorPos(&pt);
			popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
		}
	}
	*pResult = 0;
		
}

BOOL CSETreeControl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	HTREEITEM pSel = GetSelectedItem();
	if (pSel)
	{
		CString name  = GetItemText(pSel);
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();			
		CString strPath = GetItemText(pSel);
		for (;;)
		{
			pSel = GetParentItem(pSel);
			if (!pSel) break;
			strPath = GetItemText(pSel) + CString("/") + strPath;
		};
		switch (wParam)
		{
		case 1:
			//((CPainEditorApp*)AfxGetApp())->OpenScriptView("../Data/LScripts/" +strPath);
			break;
		case 2:
			strPath = CString("wscite\\scite.exe ") + "../Data/LScripts/" +strPath;
			((CPainEditorApp*)AfxGetApp())->ExecuteProgram(strPath,NULL);
			break;
		}
	};
	return CXTTreeCtrl::OnCommand(wParam, lParam);
}
