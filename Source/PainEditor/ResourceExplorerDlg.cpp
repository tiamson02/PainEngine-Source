// CResourceExplorerDlg : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ResourceExplorerDlg.h"
#include "MainFrm.h"
#include "resource.h"
#include "NewGameObjectDlg.h"
#include "PainEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceExplorerDlg

CResourceExplorerDlg::CResourceExplorerDlg()
{
	// TODO: add construction code here.
	/*m_pClassView          = NULL;
	m_pFileView           = NULL; 
	m_pResourceView       = NULL;
	m_strSolutionExplorer = _T("Solution Explorer");
    m_strWorkspace        = _T("Workspace");
	m_strClassView        = _T("Class View");
	m_strResourceView     = _T("Resource View");
    m_eSortTypes          = XT_SORTALPHA;*/
}

CResourceExplorerDlg::~CResourceExplorerDlg()
{
	// TODO: add destruction code here.

}

IMPLEMENT_DYNAMIC(CResourceExplorerDlg, CXTTabCtrlBar)
BEGIN_MESSAGE_MAP(CResourceExplorerDlg, CXTTabCtrlBar)
	//{{AFX_MSG_MAP(CResourceExplorerDlg)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZE()
	ON_COMMAND(ID_LEVEL_NEWGAMEOBJECT, OnNewGameObject)
	ON_COMMAND(ID_LEVEL_LOCK, OnLock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceExplorerDlg message handlers

int CResourceExplorerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTabCtrlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetTabCtrl().ModifyStyle(TCS_SINGLELINE,TCS_MULTILINE, 0);
	GetTabCtrl().SetAutoCondense(false);

	AddView("Models", RUNTIME_CLASS(CTabModelsView),NULL);	
	m_pModelsView = (CTabModelsView*)GetView(0);
	ASSERT_VALID(m_pModelsView);  
	m_pModelsView->UpdateView("../Data/Models/",AnimatedMesh::DefaultExt);

	AddView("Materials", RUNTIME_CLASS(CTabFilesView),NULL);	
	m_pMaterialsView = (CTabFilesView*)GetView(1);
	ASSERT_VALID(m_pMaterialsView);  
	m_pMaterialsView->UpdateView("../Data/Textures/","*.*");

	AddView("Sounds", RUNTIME_CLASS(CTabFilesView),NULL);	
	m_pSoundsView = (CTabFilesView*)GetView(2);
	ASSERT_VALID(m_pSoundsView);  
	m_pSoundsView->UpdateView("../Data/Sounds/","*.*");

	AddView("Music", RUNTIME_CLASS(CTabFilesView),NULL);	
	m_pMusicView = (CTabFilesView*)GetView(3);
	ASSERT_VALID(m_pMusicView);  
	m_pMusicView->UpdateView("../Data/Music/","*.*");

	AddView("Templates", RUNTIME_CLASS(CTabTemplatesView),NULL);	
	m_pTemplatesView = (CTabTemplatesView*)GetView(4);
	ASSERT_VALID(m_pTemplatesView);  
	m_pTemplatesView->UpdateView("../Data/LScripts/Templates/");

	// Create the image list to use with the tab control
	if (!m_ilTabImages.Create(IDB_RESOURCE_TAB, 16, 1, RGB(0x00,0x80,0x80)))
	{
		TRACE0("Failed to create image list.\n");
		return -1;
	}
	SetTabImageList( &m_ilTabImages );	
	//SetTabMenuID(IDR_POPUP, 1);

	/*
    // Add the tool bar to the docking window
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |
		CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CRect(0,0,0,0)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.LoadToolBar(IDR_LEVEL);
	
	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_wndCoolMenu.HookWindow(this);
    m_wndCoolMenu.LoadToolbar(IDR_LEVEL);
	*/
    

	SetActiveView(4);
    UpdateButtonDisplay();
	return 0;
}

const int CY_TOOLBAR = 22;

void CResourceExplorerDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CXTTabCtrlBar::OnWindowPosChanged(lpwndpos);

	
	// Reposition the toolbar.
	if (::IsWindow(m_wndToolBar.m_hWnd))
	{
		if (IsFloating()) {
			m_wndToolBar.MoveWindow(3, 3, lpwndpos->cx, CY_TOOLBAR);
		}
		else
		{
			if (IsVertDocked()) {
				m_wndToolBar.MoveWindow(0, GetMinExt(), lpwndpos->cx, CY_TOOLBAR);
			}
			else {
				m_wndToolBar.MoveWindow(GetMinExt(), 0, lpwndpos->cx, CY_TOOLBAR);
			}
		}
	}
	
}

void CResourceExplorerDlg::GetInsideRect(CRect& rect)
{
	CXTDockWindow::GetInsideRect(rect);    
    if (::IsWindow(m_wndToolBar.m_hWnd))
    {
        rect.top += (CY_TOOLBAR + 2);
    }
}

void CResourceExplorerDlg::OnTabSelChange(int nIDCtrl, CXTTabCtrl* pTabCtrl)
{
    CXTTabCtrlBar::OnTabSelChange(nIDCtrl, pTabCtrl);
    UpdateButtonDisplay();
}

void CResourceExplorerDlg::OnTabSelChanging(int nIDCtrl, CXTTabCtrl* pTabCtrl)
{
    CXTTabCtrlBar::OnTabSelChanging(nIDCtrl, pTabCtrl);
}

void CResourceExplorerDlg::UpdateButtonDisplay()
{

	
    int iTab = GetTabCtrl().GetCurSel();

    InitializeToolbar();

	/*
    switch (iTab)
    {
    case 0:
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTALPHA_POPUP));
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_CLASSVIEW_POPUP));
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTBYACCESS_POPUP));
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_GROUPBYTYPE_POPUP));
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_NEWFOLDER));
        m_wndToolBar.DeleteButton(1); // separator
        break;
    case 1:
        m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_PROPERTIES));
        if (m_eSortTypes != XT_SORTALPHA) {
            m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTALPHA_POPUP));
        }
        if (m_eSortTypes != XT_CLASSVIEW) {
            m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_CLASSVIEW_POPUP));
        }
        if (m_eSortTypes != XT_SORTBYACCESS) {
            m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTBYACCESS_POPUP));
        }
        if (m_eSortTypes != XT_GROUPBYTYPE) {
            m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_GROUPBYTYPE_POPUP));
        }
        break;
    case 2:
        m_wndToolBar.ShowWindow(SW_HIDE);
        SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
        break;
    }
	*/
}

void CResourceExplorerDlg::RemoveMenuButtons()
{
	/*
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTALPHA));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_CLASSVIEW));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTBYACCESS));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_GROUPBYTYPE));
	*/
}

void CResourceExplorerDlg::InitializeToolbar()
{
	
    

   	//m_wndToolBar.AddDropDownButton(ID_SORTALPHA_POPUP,    IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_CLASSVIEW_POPUP,    IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_SORTBYACCESS_POPUP, IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_GROUPBYTYPE_POPUP,  IDR_SOLUTION, TRUE);

    //RemoveMenuButtons();

    //m_wndToolBar.ShowWindow(SW_SHOW);
    SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);	
}

BOOL CResourceExplorerDlg::PreTranslateMessage(MSG* pMsg) 
{
	UpdateDialogControls( this, TRUE );
	return CXTTabCtrlBar::PreTranslateMessage(pMsg);	
}


void CResourceExplorerDlg::OnNewFolder()
{
}

void CResourceExplorerDlg::OnNewGameObject()
{
	static CNewGameObjectDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
}

void CResourceExplorerDlg::OnLock()
{
}

IMPLEMENT_DYNCREATE(CTabModelsView,CTabFilesView)
void CTabModelsView::OnItemDoubleClick(HTREEITEM  pItem)
{
	if (!pItem) return;

	CTreeCtrl &tree =  GetTreeCtrl();	
	int iN,iS;

	tree.GetItemImage(pItem,iN,iS);
	if (iN == 6)  // item
	{		
		CMDIChildWnd * pWnd = ((CMainFrame*)AfxGetMainWnd())->FindChildWindow(tree.GetItemText(pItem));
		if (pWnd)
			((CMainFrame*)AfxGetMainWnd())->MDIActivate(pWnd);
		else
		{
			CString strPath = "../Data/Models/" + GetTreePath(pItem);
			((CPainEditorApp*)AfxGetApp())->m_pModelEditDocTemplate->OpenDocumentFile(strPath);
		}
	}; 
}
