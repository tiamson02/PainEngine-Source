// CLevelExplorerDlg : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "LevelExplorerDlg.h"
#include "MainFrm.h"
#include "resource.h"
#include "NewGameObjectDlg.h"

#include "GameObjectsBar.h"
#include "PropertiesBar.h"
#include "EntitiesBar.h"
#include "TemplatesBar.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLevelExplorerDlg

CLevelExplorerDlg::CLevelExplorerDlg()
{
	m_IsLocked = false;
}

CLevelExplorerDlg::~CLevelExplorerDlg()
{
	// TODO: add destruction code here.

}

IMPLEMENT_DYNAMIC(CLevelExplorerDlg, CXTTabCtrlBar)
BEGIN_MESSAGE_MAP(CLevelExplorerDlg, CXTTabCtrlBar)
	//{{AFX_MSG_MAP(CLevelExplorerDlg)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZE()
	ON_COMMAND(ID_LEVEL_NEWGAMEOBJECT, OnNewGameObject)
	ON_COMMAND(ID_LEVEL_SAVEOBJECT, OnSaveGameObject)
	ON_COMMAND(ID_LEVEL_STARTOBJECT, OnStartGameObject)
	ON_COMMAND(ID_LEVEL_PAUSEOBJECT, OnPauseGameObject)
	ON_COMMAND(ID_LEVEL_RESETOBJECT, OnResetGameObject)
	ON_COMMAND(ID_LEVEL_REFRESHOBJECT, OnRefreshGameObject)
	ON_COMMAND(ID_LEVEL_LOCK, OnLock)
	ON_UPDATE_COMMAND_UI(ID_LEVEL_LOCK, OnUpdateLock)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLevelExplorerDlg message handlers

int CLevelExplorerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTabCtrlBar::OnCreate(lpCreateStruct) == -1)
		return -1;


	
    //GetTabCtrl().SetItemSize(CSize(40,20));
    //GetTabCtrl().ModifyStyle(TCS_SINGLELINE,TCS_MULTILINE, 0);
    GetTabCtrl().SetAutoCondense(true);    

    // create the tree control.
    if (!CGameObjectsBar::Get( ).Create( this ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }

    // create the tree control.
    if (!CPropertiesBar::Get( ).Create( this, 17822 ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }

    // create the tree control.
    if (!CEntitiesBar::Get( ).Create(this ))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }

    // create the tree control.
    if (!CTemplatesBar::Get( ).Create(this))
    {
        TRACE0( "Unable to create tree control.\n" );
        return -1;
    }

    AddControl( "GObjects"  , &CGameObjectsBar::Get());
    AddControl( "Properties", &CPropertiesBar::Get());
    AddControl( "Templates" , &CTemplatesBar::Get());
    AddControl( "Entities"  , &CEntitiesBar::Get() );


    /*
	
	m_pLevelView = (CTabLevelView*)GetView(RUNTIME_CLASS(CTabLevelView));
	ASSERT_VALID(m_pLevelView);  
	*/

    /*
    m_pGObjectsView.Create(this);
    //ASSERT_VALID(m_pGObjectsView);  
    AddControl("GObjects",m_pGObjectsView);
	
	//AddView("Properties", RUNTIME_CLASS(CTabPropertiesView),NULL);	
	m_pPropertiesView = (CTabPropertiesView*)RUNTIME_CLASS(CTabPropertiesView);
	//ASSERT_VALID(m_pPropertiesView);
    AddControl("Properties",m_pPropertiesView);

    //AddView("Entities", RUNTIME_CLASS(CTabEntitiesView),NULL);	
    m_pEntitiesView = (CTabEntitiesView*)GetView(RUNTIME_CLASS(CTabEntitiesView));
    ASSERT_VALID(m_pEntitiesView);
    AddControl("Entities",m_pEntitiesView);

    //AddView("Templates", RUNTIME_CLASS(CTabTemplatesView),NULL);	
	m_pTemplatesView = (CTabTemplatesView*)GetView(RUNTIME_CLASS(CTabTemplatesView));
	ASSERT_VALID(m_pTemplatesView);
    AddControl("Templates",m_pTemplatesView);
    */

		// Create the image list to use with the tab control
	if (!m_ilTabImages.Create(IDB_LEVEL_TAB, 16, 1, RGB(0x00,0x80,0x80)))
	{
		TRACE0("Failed to create image list.\n");
		return -1;
	}
	SetTabImageList( &m_ilTabImages );	
	//SetTabMenuID(IDR_POPUP, 1);


    // Add the tool bar to the docking window
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |
		CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CRect(0,0,0,0)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetWindowText("Level Explorer");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_wndCoolMenu.HookWindow(m_hWnd);
    m_wndCoolMenu.LoadToolbar(IDR_LEVEL);

	m_wndToolBar.LoadToolBar(IDR_LEVEL);
	m_wndToolBar.ShowWindow(SW_SHOW);
	SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);

	SetActiveView(0);

	//SetTimer(1,100,NULL);
    UpdateButtonDisplay();

	return 0;
/*
	// create the property grid.
	if ( m_wndPropertyGrid.Create( CRect(0,0,0,0), this, IDC_PROPERTY_GRID ) )
	{
		m_wndPropertyGrid.ShowToolBar(FALSE);
		m_wndPropertyGrid.ShowHelp(FALSE);

		XT_LOGFONT lf;
		xtAfxData.font.GetLogFont( &lf );


	//UpdateProperties("");
	
		// create document settings category.
		CXTPropertyGridItem* pSettings        = m_wndPropertyGrid.AddCategory(_T("Document Settings"));

		// add child items to category.
		CXTPropertyGridItem* pItemSaveOnClose = pSettings->AddChildItem(new CXTPropertyGridItemBool(_T("SaveOnClose"), FALSE));
		pItemSaveOnClose->SetFlags(pgiHasComboButton);

		CXTPropertyGridItem* pItemFont        = pSettings->AddChildItem(new CXTPropertyGridItemFont(_T("WindowFont"), lf));
		CXTPropertyGridItem* pItemSize        = pSettings->AddChildItem(new CXTPropertyGridItemSize(_T("WindowSize"), CSize(100, 100)));

		pSettings->Expand();
		pItemSaveOnClose->Select();

		// create global settings category.
		CXTPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("Global Settings"));

		// add child items to category.
		CXTPropertyGridItem* pItemGreeting = pGlobals->AddChildItem(new CXTPropertyGridItem(_T("Greeting Text"), _T("Welcome to your application!")));
		CXTPropertyGridItem* pMRUList      = pGlobals->AddChildItem(new CXTPropertyGridItemNumber(_T("ItemsInMRUList"), 4.4));
		CXTPropertyGridItem* pItemRate     = pGlobals->AddChildItem(new CXTPropertyGridItemNumber(_T("MaxRepeatRate"), 10));
		CXTPropertyGridItem* pToolbarColor = pGlobals->AddChildItem(new CXTPropertyGridItemColor(_T("ToolbarColor"), RGB(255, 192,128)));

		//pItemGreeting->SetReadOnly(TRUE);
		//pItemRate->SetDescription(_T("The rate in milliseconds that the text will repeat."));

		// create version category.
		CXTPropertyGridItem* pVersion      = m_wndPropertyGrid.AddCategory(_T("Version"));

		// add child items to category.
		CXTPropertyGridItem* pItemVersion  = pVersion->AddChildItem(new CXTPropertyGridItem(_T("AppVerion"), _T("1.0")));
		CXTPropertyGridItem* pItemLanguage = pVersion->AddChildItem(new CXTPropertyGridItem("Language"));

		pItemVersion->SetReadOnly(TRUE);
		pVersion->Expand();

		CXTItemConstraints* pList = pItemLanguage->GetConstraints();

		pList->AddConstraint(_T("Neutral"));
		pList->AddConstraint(_T("Arabic"));
		pList->AddConstraint(_T("German"));
		pList->AddConstraint(_T("Chinese(Taiwan)"));
		pList->AddConstraint(_T("English (United Kingdom)"));
		pList->AddConstraint(_T("English (United States)"));
		pList->AddConstraint(_T("French"));
		pList->AddConstraint(_T("Russian"));

		pItemLanguage->SetFlags(pgiHasComboButton);

		// create custom items category.
		CXTPropertyGridItem* pCustom   = m_wndPropertyGrid.AddCategory(_T("Custom Items"));

		// add child items to category.
		//CXTPropertyGridItem* pItemSpin = pCustom->AddChildItem(new CCustomItemSpin(_T("SpinButton")));
		//CXTPropertyGridItem* pItemDock = pCustom->AddChildItem(new CCustomItemChilds(_T("DockPadding"), CRect(100, 20, 400, 50)));
		//CXTPropertyGridItem* pItemC    = pCustom->AddChildItem(new CCustomItemColor(_T("CustomCombolist"), RGB(10, 40, 55)));

		//CXTPropertyGridItem* pItemIcon1 = pItemIcon->AddChildItem(new CCustomItemIcon(_T("Icon"), m_hIcon));
		//pItemIcon1->SetReadOnly(true);

		//pItemIcon->SetDescription(_T("This sample shows how to override draw function"));
		//pItemSpin->SetDescription(_T("This sample shows how to add new button type"));
		//pItemDock->SetDescription(_T("This sample shows how to add item with childs"));


		AddControl("Properties", &m_wndPropertyGrid);

		GetTabCtrl().m_bXPBorder = false;
		SetActiveView(0);
	}

	return 0;
    */
}

const int CY_TOOLBAR = 22;

void CLevelExplorerDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
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

void CLevelExplorerDlg::GetInsideRect(CRect& rect)
{
	CXTDockWindow::GetInsideRect(rect);    
    if (::IsWindow(m_wndToolBar.m_hWnd))
    {
        rect.top += (CY_TOOLBAR + 2);
    }
}

void CLevelExplorerDlg::OnTabSelChange(int nIDCtrl, CXTTabCtrl* pTabCtrl)
{
    CXTTabCtrlBar::OnTabSelChange(nIDCtrl, pTabCtrl);
    UpdateButtonDisplay();
}

void CLevelExplorerDlg::OnTabSelChanging(int nIDCtrl, CXTTabCtrl* pTabCtrl)
{
    CXTTabCtrlBar::OnTabSelChanging(nIDCtrl, pTabCtrl);
}

void CLevelExplorerDlg::UpdateButtonDisplay()
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

void CLevelExplorerDlg::RemoveMenuButtons()
{
	/*
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTALPHA));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_CLASSVIEW));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_SORTBYACCESS));
    m_wndToolBar.DeleteButton(m_wndToolBar.CommandToIndex(ID_GROUPBYTYPE));
	*/
}

void CLevelExplorerDlg::InitializeToolbar()
{
	
    

   	//m_wndToolBar.AddDropDownButton(ID_SORTALPHA_POPUP,    IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_CLASSVIEW_POPUP,    IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_SORTBYACCESS_POPUP, IDR_SOLUTION, TRUE);
   	//m_wndToolBar.AddDropDownButton(ID_GROUPBYTYPE_POPUP,  IDR_SOLUTION, TRUE);

    //RemoveMenuButtons();

    m_wndToolBar.ShowWindow(SW_SHOW);
    SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);	
}

BOOL CLevelExplorerDlg::PreTranslateMessage(MSG* pMsg) 
{
	UpdateDialogControls( this, TRUE );
	return CXTTabCtrlBar::PreTranslateMessage(pMsg);	
}


void CLevelExplorerDlg::OnNewFolder()
{
}

void CLevelExplorerDlg::OnNewGameObject()
{
	static CNewGameObjectDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
}
void CLevelExplorerDlg::OnStartGameObject()
{
    CGameObjectsBar::Get().OnCommand(PM_ExecStartCommand,0);
}
void CLevelExplorerDlg::OnResetGameObject()
{
    CGameObjectsBar::Get().OnCommand(PM_Reset,0);
}

void CLevelExplorerDlg::OnPauseGameObject()
{
    CGameObjectsBar::Get().OnCommand(PM_Pause,0);
}

void CLevelExplorerDlg::OnRefreshGameObject()
{
        
    CPropertiesBar::Get().UpdateView(CPropertiesBar::Get().m_ObjName);

    if (GetActiveView()->m_hWnd == CGameObjectsBar::Get().m_hWnd)
        CGameObjectsBar::Get().UpdateView(CPropertiesBar::Get().m_ObjName,true);

	if (GetActiveView()->m_hWnd == CTemplatesBar::Get().m_hWnd)
	{
        GScript.DoStringA("Game:ReloadTemplates()");
        CTemplatesBar::Get().UpdateView("../Data/LScripts/Templates");
	}

    if (GetActiveView()->m_hWnd == CEntitiesBar::Get().m_hWnd)
        CEntitiesBar::Get().UpdateView(CPropertiesBar::Get().m_ObjName,true);
}

void CLevelExplorerDlg::OnSaveGameObject()
{
    CGameObjectsBar::Get().OnCommand(PM_Save,0);
}

void CLevelExplorerDlg::OnLock()
{
	m_IsLocked = !m_IsLocked;
	if (m_IsLocked)
		m_wndToolBar.SetState(ID_VIEW_JOINTS,TBSTATE_CHECKED|TBSTATE_ENABLED);
	else
		m_wndToolBar.SetState(ID_VIEW_JOINTS,TBSTATE_ENABLED);
}

void CLevelExplorerDlg::OnUpdateLock(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_IsLocked);
}

void CLevelExplorerDlg::OnTimer(UINT nIDEvent)
{
    //if (nIDEvent==1  && m_IsLocked && GetActiveView() == m_pPropertiesView)
    //    m_pPropertiesView->UpdateView(m_pPropertiesView->m_ObjName);
	
	CXTTabCtrlBar::OnTimer(nIDEvent);
}

void CLevelExplorerDlg::OnSize(UINT nType, int cx, int cy)
{
	CXTTabCtrlBar::OnSize(nType,cx,cy);
    CPropertiesBar::Get().OnSize(nType,cx,cy);
}
