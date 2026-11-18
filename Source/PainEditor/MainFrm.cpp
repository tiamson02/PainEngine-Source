// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PainEditor.h"
#include "Render/View.h"
#include "Game/Main.h"
#include "LoadLevelDlg.h"
#include "ConnectWayPointsDlg.h"
#include "GenerateWaypointsDlg.h"
#include "SelectFloorsDlg.h"
#include "SaveLevelDlg.h"
#include "FileSystemDlg.h"
#include "ChildFrm.h"
#include "SetValueDlg.h"
#include "SplashWnd.h"
#include "PropertiesBar.h"  
#include "GameObjectsBar.h" 
#include "TemplatesBar.h" 
#include "EntitiesBar.h" 
#include "NewGameObjectDlg.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Engine/InputSystem.h"
#include "Physics/PhysicsEngine.h"
#include "Audio/MilesEngine.h"
#include "Net/NetworkDevice2.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/World.h"
#include "Engine/PainMenu.h"

#include "MainFrm.h"
#include ".\mainfrm.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertiesPane window

class CPropertiesPane : public CWnd
{
	// Construction
public:
	CPropertiesPane();

	// Attributes
public:

	CXTTreeCtrl m_wndTree;
	CXTPToolBar m_wndToolBar;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesPane)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CPropertiesPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnPaneproperiesCategorized();
	afx_msg void OnUpdatePaneproperiesCategorized(CCmdUI* pCmdUI);
	afx_msg void OnPaneproperiesAlphabetic();
	afx_msg void OnUpdatePaneproperiesAlphabetic(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

//CPropertiesPane *pTrackEditor = NULL;

/////////////////////////////////////////////////////////////////////////////
// CPropertiesPane

CPropertiesPane::CPropertiesPane()
{
}

CPropertiesPane::~CPropertiesPane()
{
}


BEGIN_MESSAGE_MAP(CPropertiesPane, CWnd)
	//{{AFX_MSG_MAP(CPropertiesPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_PANEPROPERIES_CATEGORIZED, OnPaneproperiesCategorized)
//	ON_UPDATE_COMMAND_UI(ID_PANEPROPERIES_CATEGORIZED, OnUpdatePaneproperiesCategorized)
//	ON_COMMAND(ID_PANEPROPERIES_ALPHABETIC, OnPaneproperiesAlphabetic)
//	ON_UPDATE_COMMAND_UI(ID_PANEPROPERIES_ALPHABETIC, OnUpdatePaneproperiesAlphabetic)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertiesPane message handlers

int CPropertiesPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(m_wndToolBar.CreateToolBar(WS_TABSTOP|WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));	
	VERIFY(m_wndToolBar.LoadToolBar(IDR_LEVEL));


	if (m_wndTree.GetSafeHwnd() == 0)
	{
		m_wndTree.Create(0, CRect(0, 0, 0, 0), this, 0 );
		//m_wndPropertyGrid.ShowToolBar(TRUE);
		m_wndTree.SetOwner(this);
		m_wndTree.InsertItem("dupa");
		m_wndTree.ShowWindow(SW_SHOW);

		/*

		CFont font;
		font.CreatePointFont(60, _T("Marlett"));

		LOGFONT lf;
		font.GetLogFont(&lf);



		// create document settings category.
		CXTPPropertyGridItem* pSettings        = m_wndPropertyGrid.AddCategory(_T("Document Settings"));

		// add child items to category.
		CXTPPropertyGridItem* pItemSaveOnClose = pSettings->AddChildItem(new CXTPPropertyGridItemBool(_T("SaveOnClose"), TRUE));
		pSettings->AddChildItem(new CXTPPropertyGridItemFont(_T("WindowFont"), lf));
		pSettings->AddChildItem(new CXTPPropertyGridItemSize(_T("WindowSize"), CSize(100, 100)));

		pSettings->Expand();
		pItemSaveOnClose->Select();

		// create global settings category.
		CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("Global Settings"));

		// add child items to category.
		CXTPPropertyGridItem* pItemGreeting = pGlobals->AddChildItem(new CXTPPropertyGridItem(_T("Greeting Text"), _T("Welcome to your application!")));
		pGlobals->AddChildItem(new CXTPPropertyGridItemNumber(_T("ItemsInMRUList"), 4));
		CXTPPropertyGridItem* pItemRate     = pGlobals->AddChildItem(new CXTPPropertyGridItemNumber(_T("MaxRepeatRate"), 10));
		pGlobals->AddChildItem(new CXTPPropertyGridItemColor(_T("ToolbarColor"), RGB(255, 192,128)));

		pItemGreeting->SetReadOnly(TRUE);
		pItemRate->SetDescription(_T("The rate in milliseconds that the text will repeat."));

		// create version category.
		CXTPPropertyGridItem* pVersion      = m_wndPropertyGrid.AddCategory(_T("Version"));

		// add child items to category.
		CXTPPropertyGridItem* pItemVersion  = pVersion->AddChildItem(new CXTPPropertyGridItem(_T("AppVerion"), _T("1.0")));
		CXTPPropertyGridItem* pItemLanguage = pVersion->AddChildItem(new CXTPPropertyGridItem(_T("Language"), _T("English (United States)")));

		pItemVersion->SetReadOnly(TRUE);
		pVersion->Expand();

		CXTPItemConstraints* pList = pItemLanguage->GetConstraints();

		pList->AddConstraint(_T("Neutral"));
		pList->AddConstraint(_T("Arabic"));
		pList->AddConstraint(_T("German"));
		pList->AddConstraint(_T("Chinese(Taiwan)"));
		pList->AddConstraint(_T("English (United Kingdom)"));
		pList->AddConstraint(_T("English (United States)"));
		pList->AddConstraint(_T("France"));
		pList->AddConstraint(_T("Russian"));

		pItemLanguage->SetFlags(pgiHasComboButton | pgiHasEdit);

		// create custom items category.
		CXTPPropertyGridItem* pCustom   = m_wndPropertyGrid.AddCategory(_T("Custom Items"));


		// add multi level tree node.
		CXTPPropertyGridItem* pItemOne    = pCustom->AddChildItem(new CXTPPropertyGridItem(_T("First Level"), _T("")));
		CXTPPropertyGridItem* pItemTwo    = pItemOne->AddChildItem(new CXTPPropertyGridItem(_T("Second Level"), _T("")));
		CXTPPropertyGridItem* pItemThird	 = pItemTwo->AddChildItem(new CXTPPropertyGridItem(_T("Third Level"), _T("")));
		pItemThird->AddChildItem(new CXTPPropertyGridItem(_T("Fourth Level 1"), _T("")));
		pItemThird->AddChildItem(new CXTPPropertyGridItem(_T("Fourth Level 2"), _T("")));
		*/
	}

	return 0;
}

void CPropertiesPane::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	int nTop = 0;

	if (m_wndToolBar.GetSafeHwnd())
	{		
		CSize sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, nTop, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
		nTop += sz.cy;
	}

	if (m_wndTree.GetSafeHwnd())
	{	
		m_wndTree.MoveWindow(0, nTop, cx, cy - nTop);
		m_wndTree.Invalidate(FALSE);
	}	
}




void CPropertiesPane::OnPaneproperiesCategorized() 
{
//	m_wndPropertyGrid.SetPropertySort(Categorized);

}

void CPropertiesPane::OnUpdatePaneproperiesCategorized(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck(m_wndPropertyGrid.GetPropertySort() == Categorized);

}

void CPropertiesPane::OnPaneproperiesAlphabetic() 
{
//	m_wndPropertyGrid.SetPropertySort(Alphabetical);

}

void CPropertiesPane::OnUpdatePaneproperiesAlphabetic(CCmdUI* pCmdUI) 
{
//	pCmdUI->SetCheck(m_wndPropertyGrid.GetPropertySort() == Alphabetical);

}



void ExportToLua();

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd) 
	ON_WM_CREATE()
	// Global help commands
	#ifdef EDITOR
		ON_COMMAND(ID_HELP_FINDER, CXTPMDIFrameWnd::OnHelpFinder)
		ON_COMMAND(ID_HELP, CXTPMDIFrameWnd::OnHelp)
		ON_COMMAND(ID_CONTEXT_HELP, CXTPMDIFrameWnd::OnContextHelp)
		ON_COMMAND(ID_DEFAULT_HELP, CXTPMDIFrameWnd::OnHelpFinder)
	#endif
#ifdef EDITOR
	ON_XTP_CREATECONTROL()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
#endif
	ON_COMMAND(ID_FULLSCREEN, OnFullScreen)
	ON_COMMAND(ID_VIEW_WAYPOINTS, OnWayPoints)
	ON_COMMAND(ID_DISABLE_SCRIPTS, OnDisableScripts)	
	ON_UPDATE_COMMAND_UI(ID_DISABLE_SCRIPTS, OnUpdateDisableScripts)	
	ON_COMMAND(ID_DISABLE_AUDIO, OnDisableAudio)	
	ON_UPDATE_COMMAND_UI(ID_DISABLE_AUDIO, OnUpdateDisableAudio)	
	ON_COMMAND(ID_DISABLE_CACHE, OnDisableCache)	
	ON_UPDATE_COMMAND_UI(ID_DISABLE_CACHE, OnUpdateDisableCache)	
    ON_COMMAND(ID_VIEW_NEWWAYPOINTS, OnNewWayPoints)
	ON_COMMAND(ID_RELOAD_LEVEL, OnReloadLevel)	
    ON_COMMAND(ID_RELOAD_LEVELANDSCRIPTS, OnReloadLevelAndScripts)	
    ON_COMMAND(ID_RELOAD_LEVELANDSCRIPTSANDMAP, OnReloadLevelAndScriptsAndMap)	
	ON_COMMAND(ID_DISABLE_SCRIPTS, OnDisableScripts)	
	ON_UPDATE_COMMAND_UI(ID_DISABLE_SCRIPTS, OnUpdateDisableScripts)	
	ON_COMMAND(ID_PLAY, OnPlay)	
	ON_COMMAND(ID_RUNGAME, OnRunGame)	
	ON_COMMAND(ID_PAUSE, OnPause)	
	ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePause)	
	ON_UPDATE_COMMAND_UI(ID_PLAY, OnUpdatePlay)	
	ON_COMMAND(ID_FILE_SAVELEVEL, OnFileSaveLevel)	
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveAll)	
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateFileSaveAll)	
	ON_UPDATE_COMMAND_UI(ID_VIEW_WAYPOINTS, OnUpdateWayPoints)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NEWWAYPOINTS, OnUpdateNewWayPoints)
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_VIEW_JOINTS, OnViewJoints)
	ON_COMMAND(ID_VIEW_AREAS, OnViewAreas)
    ON_COMMAND(ID_VIEW_ACOUSTICENVS, OnViewAcousticEnvs)
	ON_COMMAND(ID_VIEW_BILLBOARDS, OnViewBillboards)
	ON_COMMAND(ID_VIEW_LIGHTS, OnViewLights)
	ON_COMMAND(ID_VIEW_SOUNDS, OnViewSounds)
	ON_COMMAND(ID_RELOAD_TEXTURES, OnReloadTextures)
    ON_COMMAND(ID_FILESYSTEM, OnFileSystem)    
	ON_UPDATE_COMMAND_UI(ID_VIEW_JOINTS, OnUpdateViewJoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AREAS, OnUpdateViewAreas)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ACOUSTICENVS, OnUpdateViewAcousticEnvs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BILLBOARDS, OnUpdateViewBillboards)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHTS, OnUpdateViewLights)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOUNDS, OnUpdateViewSounds)
	ON_COMMAND(ID_WAYPOINTS_UNSELECTALL, OnWaypointsUnselectall)
	ON_COMMAND(ID_WAYPOINTS_SELECTALL, OnWaypointsSelectall)
	ON_COMMAND(ID_PATHFINDING_RECALCULATEFLOORS, OnPathfindingRecalculatefloors)
	ON_COMMAND(ID_WAYPOINTS_DELETESELECTED, OnWaypointsDeleteselected)
    ON_COMMAND(ID_WAYPOINTS_MOVESELECTEDTOTHEZONE, OnWaypointsMoveSelectedToTheZone)
	ON_COMMAND(ID_WAYPOINTS_CONNECTDISCCONECT, OnWaypointsConnectDisconnect)
	ON_COMMAND(ID_WAYPOINTS_SELECTONFLOOR, OnWaypointsSelectOnFloor)
	ON_COMMAND(ID_WAYPOINTS_UNSELECTONFLOOR, OnWaypointsUnselectOnFloor)
	ON_COMMAND(ID_WAYPOINTS_CONNECTSELECTED, OnWaypointsConnectSelected)
	ON_COMMAND(ID_WAYPOINTS_DISCONNECTSELECTED, OnWaypointsDisconnectSelected)
	ON_COMMAND(ID_WAYPOINTS_REMOVECOLLIDINGCONNECTIONS, OnWaypointsRemoveCollidingConnections)	
	ON_COMMAND(ID_WAYPOINTS_GETSETFROMSELECTED, OnWaypointsGetSetFromSelected)
	ON_COMMAND(ID_WAYPOINTS_MAKENEWSETFROMSELECTED, OnWaypointsMakeNewSetFromSelected)
	ON_COMMAND(ID_WAYPOINTS_EXPANDCURRENT, OnWaypointsExpandCurrent)
	ON_COMMAND(ID_WAYPOINTS_CONTRACTCURRENT, OnWaypointsContractCurrent)
	ON_COMMAND(ID_WAYPOINTS_VALIDATESETS, OnWaypointsValidateSets)
	ON_COMMAND(ID_WAYPOINTS_MERGEWITHSELECTED, OnWaypointsMergeWithSelected)
	ON_COMMAND(ID_WAYPOINTS_MOVESELECTEDTOCURRENTSET, OnWaypointsMoveSelectedToCurrentSet)
	ON_COMMAND(ID_WAYPOINTS_SELECTALLINCURRENTSET, OnWaypointsSelectAllInCurrentSet)
	ON_COMMAND(ID_WAYPOINTS_UNSELECTALLINCURRENTSET, OnWaypointsUnselectAllInCurrentSet)
	ON_COMMAND(ID_WAYPOINTS_VIEWMORE, OnWaypointsViewMore)
	ON_COMMAND(ID_WAYPOINTS_VIEWLESS, OnWaypointsViewLess)
	ON_COMMAND(ID_WAYPOINTS_UNDODISPLACEMENT, OnWaypointsUndoDisplacement)
	ON_COMMAND(ID_WAYPOINTS_DRAWDISABLED, OnWaypointsDrawDisabledSets)
	ON_UPDATE_COMMAND_UI(ID_WAYPOINTS_DRAWDISABLED, OnUpdateDrawDisabledSets)
	ON_COMMAND(ID_WAYPOINTS_ENABLEALLSETS, OnWaypointsEnableAllSets)
	ON_COMMAND(ID_WAYPOINTS_DISABLEALLSETS, OnWaypointsDisableAllSets)
	ON_COMMAND(ID_WAYPOINTS_MERGECLOSE, OnWaypointsMergeClose)
	ON_COMMAND(ID_WAYPOINTS_RANDOMIZE_SETS, OnWaypointsRandomizeSets)

	ON_COMMAND(ID_SELECTUNSELECTFLOORS, OnSelectUnselectFloors)	
	ON_COMMAND(ID_WAYPOINTS_COPYSELECTED, OnWaypointsCopySelected)
	ON_COMMAND(ID_PATHFINDING_PREPAREPORTALNODES, OnPathfindingPrepareportalnodes)
	ON_COMMAND(ID_PATHFINDING_GENERATEAUTO, OnPathfindingGenerateAuto)
	ON_COMMAND(ID_PATHFINDING_LOADMAP, OnPathfindingLoadmap)
	ON_COMMAND(ID_PATHFINDING_LOADMAPWITHOUTPORTALS, OnPathfindingLoadmapWithoutPortals)
	ON_COMMAND(ID_PATHFINDING_MERGEMAP, OnPathfindingMergeMap)
	ON_COMMAND(ID_PATHFINDING_SAVEMAP, OnPathfindingSavemap)
	ON_COMMAND(ID_VIEW_FLOORS, OnViewFloors)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FLOORS, OnUpdateViewFloors)
	ON_COMMAND(ID_PATHFINDING_GENERATEWAYPOINTS, OnPathfindingGeneratewaypoints)
	ON_COMMAND(ID_VIEW_MODE, OnViewMode)
	ON_COMMAND(ID_RESOLUTION, OnResolution)
	ON_COMMAND(ID_NEW_VIEW, OnNewView)
	ON_COMMAND(ID_WAYPOINTS_SELECTNOTCONNECTEDTOANYTHINGINCURRENTROOM, OnWaypointsSelectnotconnectedtoanythingincurrentroom)
	ON_COMMAND(ID_PATHFINDING_CLEANSTRUCTURES, OnPathfindingCleanstructures)
	ON_COMMAND(ID_PATHFINDING_SCALECONTENTS, OnPathfindingScalecontents)
	ON_COMMAND(ID_WAYPOINTS_SELECTOUTSIDEOFCURRENTZONE, OnWaypointsSelectoutsideofcurrentzone)
	ON_COMMAND(ID_PATHFINDING_LEVELWAYPOINTSWITHFLOORS, OnPathfindingLevelwaypointswithfloors)
	ON_COMMAND(ID_WAYPOINTS_SETSELECTEDASFORALLMONSTERS, OnWaypointsSetselectedasforallmonsters)
	ON_COMMAND(ID_WAYPOINTS_SETSELECTEDASFORSMALLMONSTERSONLY, OnWaypointsSetselectedasforsmallmonstersonly)
	ON_COMMAND(ID_WAYPOINTS_INVERTSELECTION, OnWaypointsInvertselection)
	ON_COMMAND(ID_FILE_LOADLEVEL, OnFileLoadlevel)
	ON_COMMAND(ID_FILE_SAVELEVELAS, OnFileSaveAllAs)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_EXTRACTGAMEDATA, OnExtractGameData)
	ON_COMMAND(ID_RELOADTEMPLATES, OnReloadTemplates)	
	ON_COMMAND(ID_RENDERVIEW_TEXTURES, OnRenderviewTextures)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_TEXTURES, OnUpdateRenderviewTextures)
	ON_COMMAND(ID_RENDERVIEW_LIGHTMAPS, OnRenderviewLightmaps)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_LIGHTMAPS, OnUpdateRenderviewLightmaps)
	ON_COMMAND(ID_RENDERVIEW_WIREFRAME, OnRenderviewWireframe)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_WIREFRAME, OnUpdateRenderviewWireframe)
	ON_COMMAND(ID_RENDERVIEW_ZONES, OnRenderviewZones)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_ZONES, OnUpdateRenderviewZones)
	ON_COMMAND(ID_RENDERVIEW_BOXES, OnRenderviewBoxes)	
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_BOXES, OnUpdateRenderviewBoxes)

	ON_COMMAND(ID_RENDERVIEW_MODELS, OnRenderviewModels)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_MODELS, OnUpdateRenderviewModels)

	ON_COMMAND(ID_RENDERVIEW_VOLUMETRICS, OnRenderviewVolumetrics)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_VOLUMETRICS, OnUpdateRenderviewVolumetrics)

	ON_COMMAND(ID_RENDERVIEW_VISIBILITY, OnRenderviewVisibility)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_VISIBILITY, OnUpdateRenderviewVisibility)
	ON_COMMAND(ID_EDIT_COMMAND, OnEditCommand)
	ON_COMMAND(ID_RENDERVIEW_MONSTERSIGHT, OnRenderviewMonstersight)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_MONSTERSIGHT, OnUpdateRenderviewMonstersight)
	ON_COMMAND(ID_RENDERVIEW_PHYSICALMODEL, OnRenderviewPhysicalmodel)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_PHYSICALMODEL, OnUpdateRenderviewPhysicalmodel)
	ON_COMMAND(ID_RENDERVIEW_HAVOKPRIMITIVES, OnRenderviewHavokprimitives)
	ON_UPDATE_COMMAND_UI(ID_RENDERVIEW_HAVOKPRIMITIVES, OnUpdateRenderviewHavokprimitives)
	ON_COMMAND(ID_LIGHTS_SCALE, OnLightsScale)
    ON_COMMAND(ID_ITEMS_SCALE, OnItemsScale)
	ON_COMMAND(ID_LIGHTS_SAVEALL, OnLightsSaveAll)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_OBJECTS_NEWOBJECT, OnObjectsNewobject)
	ON_COMMAND(ID_ITEMS_SAVEALL, OnItemsSaveall)
	ON_COMMAND(ID_AREAS_SAVEALL, OnAreasSaveall)
	ON_COMMAND(ID_SOUNDS_SAVEALL, OnSoundsSaveall)
	ON_COMMAND(ID_TWEAK_EDIT, OnTweakEdit)
	ON_COMMAND(ID_TWEAK_APPLY, OnTweakApply)
	ON_COMMAND(ID_OBJECTS_RESETSELECTED, OnObjectsResetselected)
	ON_COMMAND(ID_OBJECTS_SAVESELECTED, OnObjectsSaveselected)
	ON_MESSAGE(WM_APP + 2555,OnScriptMessage)
    ON_COMMAND(ID_PATHFINDING_IMPORTOLDWAYPOINTS, OnPathfindingImportoldwaypoints)
	ON_COMMAND_RANGE(ID_VIEW_PROPERTIES, ID_VIEW_WATCHBAR, OnShowView)
    END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT arToolbars[] = {
	IDR_MAINFRAME,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
#ifdef EDITOR
 : m_EditStrBuf( m_EditLogger )
#endif
{
	m_bWayPoints = false;
	m_bFloors = false;
	m_bAreas = false;
    m_bAcousticEnvs = false;
	m_bBillboards = false;
	m_bLights = false;
	m_bSounds = false;
    m_bNewWayPoints = false;
	m_bPausedScripts = true;
	m_bEnabledSaveAll = true;
	m_bDisabledDataCache = false;
	m_bDisabledAudio = false;

#ifdef EDITOR
	xtAfxData.bXPMode = TRUE;
	xtAfxData.bMenuShadows = TRUE;
#endif

}

CMainFrame::~CMainFrame()
{
}


void CMainFrame::OnShowView(UINT nID)
{
#ifdef EDITOR
	GetDockingPaneManager()->ShowPane(nID);
#endif
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifdef EDITOR
	CSplashWnd::EnableSplashScreen(true);
	CSplashWnd::ShowSplashScreen(-1, IDB_SPLASH, this);
#endif

#ifdef EDITOR
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	if (!InitCommandBars())
		return -1;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(_T("Menu Bar"),IDR_MAINFRAME); 
	pMenuBar->SetFlags(xtpFlagIgnoreSetMenuMessage);

	m_pToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Standard Bar"), xtpBarTop);
	if (!m_pToolBar || !m_pToolBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}


	m_pPathfindingToolBar = (CXTPToolBar*)pCommandBars->Add(_T("Pathfinding Bar"), xtpBarTop);
	if (!m_pPathfindingToolBar || !m_pPathfindingToolBar->LoadToolBar(IDR_PATHFINDINGBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	XTPImageManager()->SetMaskColor(RGB(0xC0, 0xC0, 0xC0));
	XTPImageManager()->SetIcons(IDR_MAINFRAME);

	LoadCommandBars(_T("CommandBars"));

	GetDockingPaneManager()->InstallDockingPanes(this);
	GetDockingPaneManager()->SetTheme(xtpPaneThemeOffice2003);
	CXTPPaintManager::SetTheme(xtpThemeOffice2003);

	if (!CTemplatesBar::Get( ).Create(this))
	{
		TRACE0( "Unable to create tree control.\n" );
		return -1;
	}

	if (!m_PropertiesBar.Create(this))
	{
		TRACE0( "Unable to create tree control.\n" );
		return -1;
	}

	if (!CGameObjectsBar::Get( ).Create(this))
	{
		TRACE0( "Unable to create tree control.\n" );
		return -1;
	}

	if (!CEntitiesBar::Get( ).Create(this))
	{
		TRACE0( "Unable to create tree control.\n" );
		return -1;
	}

	if( !m_wndEmitterDlg.Create(NULL, _T("Emitter Editor"),0,
		CRect(0,0,200, 200),this,0))
	{
		TRACE0("Failed to create dock window\n");
		return -1;		// fail to create
	}
	m_wndEmitterDlg.CreateInnerDialog(new CEmitterEditorDlg(),IDD_EMITTEREDITOR);
	m_wndEmitterDlg.ShowWindow(SW_HIDE);


	if( !m_wndParticleFXDlg.Create(NULL,  _T("ParticleFX Editor"), 0,
		CRect(0,0,200, 200),this, 0))
	{
		TRACE0("Failed to create dock window\n");
		return -1;		// fail to create
	}
	m_wndParticleFXDlg.CreateInnerDialog(new CParticleFXEditorDlg(),IDD_PARTICLEFXEDITOR);
	m_wndParticleFXDlg.ShowWindow(SW_HIDE);

	// Create  the flat tab control.
	if (!m_WatchBar.Create(WS_CHILD|WS_VISIBLE|FTS_XT_BOTTOM|FTS_XT_HASARROWS|FTS_XT_HSCROLL,
		CRect(0,0,0,0), this, IDD_WATCHBAR))
	{
		TRACE0( "Failed to create flattab control\n" );
		return -1;
	}
	m_WatchBar.ModifyStyleEx(NULL, WS_EX_STATICEDGE);

	m_editOutput.Create(//WS_HSCROLL|ES_AUTOHSCROLL|ES_MULTILINE
		WS_CHILD|WS_VISIBLE|ES_MULTILINE | ES_AUTOVSCROLL |ES_WANTRETURN|
		ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL,CRect(0,0,0,0),this,0);

	m_editOutput.ModifyStyleEx(0, WS_EX_STATICEDGE);
	m_editOutput.SetFont(XTPPaintManager()->GetRegularFont());
	m_editOutput.SetMargins(10, 5);

	// Set the Edit-control as logger destination
	m_EditLogger.SetEditCtrl( m_editOutput.m_hWnd );


/*	pTrackEditor  = new CPropertiesPane();
	pTrackEditor->Create(NULL, _T("Track  Editor"),0,
		CRect(0,0,200, 200),this,0);

*/
	CXTPDockingPaneLayout layoutNormal(GetDockingPaneManager());
	if (layoutNormal.Load(_T("NormalLayout3"))) 
	{
		GetDockingPaneManager()->SetLayout(&layoutNormal);	
	}
	else
	{
		CXTPDockingPane* paneOutput = GetDockingPaneManager()->CreatePane(
			ID_VIEW_OUTPUT, CRect(0, 0, 210, 120), dockBottomOf);

		CXTPDockingPane* paneTemplates = GetDockingPaneManager()->CreatePane(
			ID_VIEW_TEMPLATES, CRect(0, 0, 210, 140), dockRightOf);

		CXTPDockingPane* paneProperties = GetDockingPaneManager()->CreatePane(
			ID_VIEW_PROPERTIES, CRect(0, 0, 210, 140), dockRightOf);

		CXTPDockingPane* paneWatch = GetDockingPaneManager()->CreatePane(
			ID_VIEW_WATCHBAR, CRect(0, 0, 210, 140), dockBottomOf);

		CXTPDockingPane* paneGObjects = GetDockingPaneManager()->CreatePane(
			ID_VIEW_GOBJECTS, CRect(0, 0, 210, 140), dockRightOf);

		CXTPDockingPane* paneEntities = GetDockingPaneManager()->CreatePane(
			ID_VIEW_ENTITIES, CRect(0, 0, 210, 140), dockRightOf);

		CXTPDockingPane* paneEmitter = GetDockingPaneManager()->CreatePane(
			ID_VIEW_EMITTER, CRect(0, 0, 210, 140), dockLeftOf);

		CXTPDockingPane* paneParticleFX = GetDockingPaneManager()->CreatePane(
			ID_VIEW_PARTICLEFX, CRect(0, 0, 210, 140), dockLeftOf);

		/*
		CXTPDockingPane* paneTrackEditor = GetDockingPaneManager()->CreatePane(
			ID_VIEW_TRACKEDITOR, CRect(0, 0, 210, 120), dockBottomOf);
			*/

		paneTemplates->SetTitle("Templates");
		paneProperties->SetTitle("Properties");
		paneGObjects->SetTitle("Game Objects");
		paneEntities->SetTitle("Entities");
		paneEmitter->SetTitle("Emitter");
		paneParticleFX->SetTitle("ParticleFX");
		paneOutput->SetTitle("Output");
		paneWatch->SetTitle("WatchBar");
//		paneTrackEditor->SetTitle("Track Editor");

		paneTemplates->Attach(&CTemplatesBar::Get( ));
		paneProperties->Attach(&m_PropertiesBar);
		paneGObjects->Attach(&CGameObjectsBar::Get( ));
		paneEntities->Attach(&CEntitiesBar::Get( ));
		paneEmitter->Attach(&m_wndEmitterDlg);
		paneParticleFX->Attach(&m_wndParticleFXDlg);
		paneOutput->Attach(&m_editOutput);
		paneWatch->Attach(&m_WatchBar);
//		paneTrackEditor->Attach(pTrackEditor);

		GetDockingPaneManager()->AttachPane(paneProperties,paneGObjects);
		GetDockingPaneManager()->AttachPane(paneTemplates,paneGObjects);
		GetDockingPaneManager()->AttachPane(paneEntities,paneGObjects);
		GetDockingPaneManager()->ShowPane(paneGObjects);
		GetDockingPaneManager()->AttachPane(paneEmitter,paneParticleFX);
		GetDockingPaneManager()->AttachPane(paneWatch,paneOutput);
		GetDockingPaneManager()->ShowPane(paneOutput);
		GetDockingPaneManager()->ShowPane(paneParticleFX);
		GetDockingPaneManager()->HidePane(paneParticleFX);
//		GetDockingPaneManager()->ShowPane(paneTrackEditor);
	}

	int nIDIcons[] = {
		ID_VIEW_ENTITIES, ID_VIEW_GOBJECTS,  ID_VIEW_TEMPLATES, ID_VIEW_PROPERTIES,
			ID_VIEW_EMITTER, ID_VIEW_PARTICLEFX
	};

	GetDockingPaneManager()->SetIcons(IDB_PANES, nIDIcons,
		sizeof(nIDIcons)/sizeof(nIDIcons[0]), RGB(0, 128, 128));
	

/*    
	
	// Create the "Watch" docking window.
	if( !m_wndWatchBar.Create(this, ID_VIEW_WATCHBAR, _T("Watch"),
		CSize(400, 100), CBRS_BOTTOM ))
	{
		TRACE0("Failed to create output dock window\n");
		return -1;		// fail to create
	}
	m_wndWatchBar.ShowWindow(SW_HIDE);
   
*/	

	VERIFY(m_MTIClientWnd.Attach(this, TRUE));
	m_MTIClientWnd.GetPaintManager()->m_bShowIcons = FALSE;
	m_MTIClientWnd.EnableToolTips();

#endif


	EnableSave(true);

	return 0;
}

void CMainFrame::LoadSettings()
{
#ifdef EDITOR
	m_bDisabledAudio = AfxGetApp()->GetProfileInt("Settings","DisabledAudio",0)?true:false;
	m_bDisabledDataCache = AfxGetApp()->GetProfileInt("Settings","DisabledDataCache",0)?true:false;
	m_bDisabledAudio = !m_bDisabledAudio;OnDisableAudio();
	m_bDisabledDataCache = !m_bDisabledDataCache; OnDisableCache();
#endif
}

#ifdef EDITOR
int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{

	if (lpCreateControl->bToolBar)
	{
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, lpCreateControl->pCommandBar);
		if (!pToolBar)
			return FALSE;

		if (lpCreateControl->nID == ID_RESOLUTION && pToolBar->GetBarID() == IDR_MAINFRAME)
		{
			//m_cmbResolution = (CXTPControlComboBox*)CXTPControlComboBox::CreateObject();
			m_cmbResolution.SetDropDownListStyle();
			m_cmbResolution.AddString("640x480");
			m_cmbResolution.AddString("800x600");
			m_cmbResolution.AddString("1024x768");
			m_cmbResolution.AddString("1280x1024");
			m_cmbResolution.AddString("1600x1200");
			m_cmbResolution.AddString("2048x1536");
			lpCreateControl->pControl = &m_cmbResolution;
			return TRUE;
		}
		
		if (lpCreateControl->nID == ID_VIEW_MODE && pToolBar->GetBarID() == IDR_MAINFRAME)
		{
			lpCreateControl->controlType = xtpControlSplitButtonPopup;
			lpCreateControl->buttonStyle = xtpButtonIcon;
			return TRUE;
		}


	}
	return FALSE;

}
#endif

void CMainFrame::DockControlBottomOf(CControlBar* Bar,CControlBar* BottomOf)
{
  CRect rect;
  DWORD dw;
  UINT n;
  
  RecalcLayout();
  BottomOf->GetWindowRect(&rect);
  rect.OffsetRect(0,1);
  dw=BottomOf->GetBarStyle();
  n = 0;
  n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
  n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
  n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
  n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
  
  DockControlBar(Bar,n,&rect);
}


void CMainFrame::AddObjectTemplate(LPCSTR folder,LPCSTR item,LPCSTR file,int icon)
{
//	m_wndTemplateBar.AddObjectTemplate(folder,item,file,icon);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	cs.lpszClass = AfxRegisterWndClass( 0, NULL, NULL,
		AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const 
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnClose()
{
	#ifdef EDITOR



	SaveCommandBars(_T("CommandBars"));

	CXTPDockingPaneLayout layoutNormal(GetDockingPaneManager());
	GetDockingPaneManager()->GetLayout(&layoutNormal);	
	layoutNormal.Save(_T("NormalLayout3"));

	// Save frame window size and position.
	CXTWindowPos m_wndPosition;
	m_wndPosition.SaveWindowPos(this);
	#endif
	
	static TCHAR BASED_CODE szSection[] = _T("Settings");
	static TCHAR BASED_CODE szWindowPos[] = _T("WindowPos");

	AfxGetApp()->WriteProfileInt("Settings","DisabledAudio", m_bDisabledAudio);
	AfxGetApp()->WriteProfileInt("Settings","DisabledDataCache", m_bDisabledDataCache);

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
#ifdef EDITOR
	CXTPMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
#endif
    if (Game() && GEngine->InputSys)	
		GEngine->InputSys->Reset();
}


void CMainFrame::OnFullScreen()
{
    SwitchToFullScreen(true);
}



void CMainFrame::OnWayPoints()
{
#ifdef EDITOR
	m_bWayPoints=!m_bWayPoints;
	GScript.DoString("Editor:OnMsg('OnEnableWPEdit',%d)",m_bWayPoints);
	
	m_bWayPoints ? GEngine->Video->GetActiveView()->Mode |= View::EModeShowWaypoints : GEngine->Video->GetActiveView()->Mode &= ~View::EModeShowWaypoints;
	
	m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_WAYPOINTS)->SetEnabled(true);
	if (m_bWayPoints)
		m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_WAYPOINTS)->SetChecked(true);
	else
		m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_WAYPOINTS)->SetChecked(false);
#endif
}

void CMainFrame::OnUpdateWayPoints(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bWayPoints);

}

void CMainFrame::OnNewWayPoints()
{
    m_bNewWayPoints = !m_bNewWayPoints;
    m_bNewWayPoints ? GEngine->Video->GetActiveView()->Mode |= View::EModeNewWaypoints : GEngine->Video->GetActiveView()->Mode &= ~View::EModeNewWaypoints;
#ifdef TRANSGAMING
	GEngine->pcfWorld->UseNewPaths = m_bNewWayPoints;
#else
	GEngine->World->UseNewPaths = m_bNewWayPoints;
#endif

#ifdef EDITOR
	m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_NEWWAYPOINTS)->SetEnabled(true);
	if (m_bWayPoints)
		m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_NEWWAYPOINTS)->SetChecked(true);
	else
		m_pPathfindingToolBar->GetControls()->FindControl(ID_VIEW_NEWWAYPOINTS)->SetChecked(false);
#endif
}

void CMainFrame::OnUpdateNewWayPoints(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bNewWayPoints);
}

void CMainFrame::OnFileLoadlevel()
{
#ifdef EDITOR
	CLoadLevelDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		RedrawWindow();
        ReleaseParticleEditor();
        GEngine->SwitchToState(PCFSystem::SingleGame);
#ifdef TRANSGAMING
		GEngine->pcfWorld->FileName = ""; // zawsze na nowo laduje mape
#else
        GEngine->World->FileName = ""; // zawsze na nowo laduje mape
#endif
		GScript.DoString("Game:LoadLevel('%s')",dlg.mLevelName);	

		EnableSave(true);
        CTemplatesBar::Get().UpdateLevelTemplates();
        CEntitiesBar::Get().UpdateView("");
	}
#endif
}


LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
#ifdef EDITOR
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case ID_VIEW_PROPERTIES: pPane->Attach(&m_PropertiesBar);  break;
			case ID_VIEW_GOBJECTS:	 pPane->Attach(&CGameObjectsBar::Get( )); break;
			case ID_VIEW_OUTPUT:	 pPane->Attach(&m_editOutput);			  break;
			case ID_VIEW_ENTITIES:	 pPane->Attach(&CEntitiesBar::Get( ));    break;
			case ID_VIEW_TEMPLATES:	 pPane->Attach(&CTemplatesBar::Get( ));   break;
			case ID_VIEW_PARTICLEFX: pPane->Attach(&m_wndParticleFXDlg);      break;
			case ID_VIEW_EMITTER:	 pPane->Attach(&m_wndEmitterDlg);         break;				
			case ID_VIEW_WATCHBAR:	 pPane->Attach(&m_WatchBar);         break;				
//			case ID_VIEW_TRACKEDITOR: pPane->Attach(pTrackEditor); break;								


				/*
			case ID_HELP_DYNAMICHELP:
				{
					// attach the dynamic help view to the pane.
					CWnd* pView = pPane->AttachView(this, RUNTIME_CLASS(CDynamicHelpView));

					// send a WM_INITIALUPDATE message to the view.
					if (pView && ::IsWindow(pView->m_hWnd)) {
						pView->SendMessage(WM_INITIALUPDATE);
					}
					break;
				}
				*/
			}
		}
		return 1;
	}
#endif
	return 0;
}

BOOL CMainFrame::ShowWindowEx(int nCmdShow)
{

#ifdef EDITOR
	// Restore frame window size and position.
	CXTWindowPos wndPosition;
	wndPosition.LoadWindowPos(this);
	nCmdShow = wndPosition.showCmd;
	RecalcLayout(true);
#endif
	return ShowWindow(nCmdShow);
}

void CMainFrame::OnViewMode()
{
}

void CMainFrame::OnResolution()
{
#ifdef EDITOR
	int i = m_cmbResolution.GetCurSel();	
	if (i>-1 && Game())
	{
		CString res;
		m_cmbResolution.GetLBText(i,res);
		res.Replace('x','X');
		GScript.DoString("Cfg.Resolution = '%s'",res);
	}
	SetFocus();
#endif
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{

#ifdef EDITOR
	CXTPMDIFrameWnd::OnSize(nType,cx,cy);
#else
	CMDIFrameWnd::OnSize(nType,cx,cy);
#endif
}

void CMainFrame::OnReloadLevel()	
{
    ReleaseParticleEditor();
    CString curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();
    GEngine->Video->PMenu->LoadScreen.Activate( true );
    GScript.DoString("Game:LoadLevel('%s')",curLevelName);
#ifdef EDITOR
    CTemplatesBar::Get().UpdateLevelTemplates();
    CEntitiesBar::Get().UpdateView("");
#endif
    EnableSave(true);
    GEngine->ResetTimer();
}

void CMainFrame::OnReloadLevelAndScripts()	
{
    ReleaseParticleEditor();
    CString curLevelName = GScript.Globals()["Lev"]["_Name"].GetString();
    GEngine->Video->PMenu->LoadScreen.Activate( true );
    GScript.DoString("Game:Close()");
    GScript.Release();
    GScript.Init();
    ExportToLua();
    GScript.DoFile("../Data/LScripts/Loader.lua");
	GScript.DoString("CfgFile = [[%s]]",(LPCSTR)((CPainEditorApp*)AfxGetApp())->CfgFileName);
	if (curLevelName == "NoName")
		GScript.DoString("Game:Init()");
	else
		GScript.DoString("Game:Init(true)");

#ifdef EDITOR
	m_bDisabledDataCache = !m_bDisabledDataCache; OnDisableCache();
	m_bDisabledAudio = !m_bDisabledAudio;OnDisableAudio();
    GScript.DoString("Editor.Enabled = true");
#else
    GScript.DoString("Editor.Enabled = false");
    GScript.DoString("Cfg.Shadows = %d",GEngine->Video->ShadowsEnabled);
#endif
    GScript.DoString("Game:LoadLevel('%s')",(LPCSTR) curLevelName);
    GScript.DoString("Game.Active = false");
#ifdef EDITOR
    m_bWayPoints = !m_bWayPoints; OnWayPoints();
    m_bAreas = !m_bAreas;         OnViewAreas();
    m_bAcousticEnvs = !m_bAcousticEnvs; OnViewAcousticEnvs();
    m_bLights = !m_bLights;       OnViewLights();
    m_bSounds = !m_bSounds;       OnViewSounds();
    m_bBillboards = !m_bBillboards;         OnViewBillboards();

    CGameObjectsBar::Get().UpdateView("",true);
    CTemplatesBar::Get().UpdateLevelTemplates();
    CEntitiesBar::Get().UpdateView("");
#endif
    EnableSave(true);
    GEngine->ResetTimer();
}

void CMainFrame::OnReloadLevelAndScriptsAndMap()	
{
#ifdef TRANSGAMING
	GEngine->pcfWorld->FileName = "";
#else
    GEngine->World->FileName = "";
#endif
    OnReloadLevelAndScripts();
}

void CMainFrame::EnableSave(bool enable)
{
	m_bEnabledSaveAll = enable;
	m_bPausedScripts = enable;
}

void CMainFrame::OnPlay()	
{
	if (Game())
	{
		GEngine->Physics->Tick(0.001f);
		m_bPausedScripts = false;
        if (m_bEnabledSaveAll)
			GScript.DoString("Game:OnPlay(true)");
		else
			GScript.DoString("Game:OnPlay()");
	}
	EnableSave(false);
}

void CMainFrame::OnRunGame()
{
	OnFullScreen();
	OnPlay();
    GScript.DoString("Game:SwitchPlayerToPhysics(true)");
}

void CMainFrame::OnPause()	
{
	m_bPausedScripts = true;
	if (Game())
		GScript.DoString("Game.Active = false");
}

void CMainFrame::OnUpdatePause(CCmdUI *pCmdUI)
{

	pCmdUI->Enable(m_bPausedScripts?0:1);
}

void CMainFrame::OnUpdatePlay(CCmdUI *pCmdUI)
{

	pCmdUI->Enable(m_bPausedScripts?1:0);
}

void CMainFrame::OnUpdateDisableScripts(CCmdUI *pCmdUI)
{
	if (Game())
		pCmdUI->SetCheck(!Game()->mUpdateLogic);
}

void CMainFrame::OnUpdateDisableAudio(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDisabledAudio);	
}

void CMainFrame::OnUpdateDisableCache(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDisabledDataCache);
}


void CMainFrame::OnUpdateFileSaveAll(CCmdUI *pCmdUI)
{
	 pCmdUI->Enable(m_bEnabledSaveAll?1:0);
}

void CMainFrame::OnDisableScripts()	
{
	Game()->mUpdateLogic = !Game()->mUpdateLogic;	
}

void CMainFrame::OnDisableAudio()	
{
	m_bDisabledAudio = !m_bDisabledAudio;
	if (GEngine && GEngine->Audio)
		GEngine->Audio->SetMasterVolumeLevel(m_bDisabledAudio?0.0f:1.0f);
}

void CMainFrame::OnDisableCache()	
{
	m_bDisabledDataCache = !m_bDisabledDataCache;
	GScript.DoString("PrecacheDisabled = %s",m_bDisabledDataCache?"true":"false");
}

void CMainFrame::OnViewJoints()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeShowJoints;
}

void CMainFrame::OnUpdateViewJoints(CCmdUI *pCmdUI)
{
    if (GEngine && GEngine->Video && GEngine->Video->GetActiveView())
        pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowJoints?1:0 );
}

void CMainFrame::OnViewAcousticEnvs()
{
    m_bAcousticEnvs=!m_bAcousticEnvs;
    GScript.DoString("Editor:OnMsg('OnEnableAcousticEnvs',%s)",m_bAcousticEnvs?"true":"false");
}

void CMainFrame::OnUpdateViewAcousticEnvs(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bAcousticEnvs);
}

void CMainFrame::OnViewAreas()
{
	m_bAreas=!m_bAreas;
	GScript.DoString("Editor:OnMsg('OnEnableAreas',%s)",m_bAreas?"true":"false");
}

void CMainFrame::OnUpdateViewAreas(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAreas);
}

void CMainFrame::OnViewBillboards()
{
	m_bBillboards=!m_bBillboards;
	GScript.DoString("Editor.EditBillboardsAndDecals=%s",m_bBillboards?"true":"false");
}

void CMainFrame::OnUpdateViewBillboards(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bBillboards);
}

void CMainFrame::OnViewLights()
{
	m_bLights=!m_bLights;
	GScript.DoString("Editor:OnMsg('OnEnableLights',%s)",m_bLights?"true":"false");
}

void CMainFrame::OnUpdateViewLights(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bLights);
}

void CMainFrame::OnViewSounds()
{
	m_bSounds=!m_bSounds;
	GScript.DoString("Editor:OnMsg('OnEnableSounds',%s)",m_bSounds?"true":"false");

}

void CMainFrame::OnUpdateViewSounds(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSounds);
}

void CMainFrame::OnReloadTextures()
{
	GEngine->Video->MaterialSys->ReloadTextures();
}

void CMainFrame::OnFileSystem()
{
    FileSystemDlg dlg;
    dlg.DoModal();
}

void CMainFrame::OnWaypointsUnselectall()
{
	if( m_bNewWayPoints )
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 0 );
		else if( m_bFloors )
			GEngine->pcfWorld->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 0 );
#else
			GEngine->World->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 0 );
		else if( m_bFloors )
			GEngine->World->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 0 );
#endif
	}
	else
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.Selection(0,NULL,0);
		else if( m_bFloors )
			GEngine->pcfWorld->Paths.FloorSelection(NULL,0);
#else
			GEngine->World->Paths.Selection(0,NULL,0);
		else if( m_bFloors )
			GEngine->World->Paths.FloorSelection(NULL,0);
#endif
	}
}

void CMainFrame::OnWaypointsSelectall()
{
	if( m_bNewWayPoints )
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 1 );
		else if( m_bFloors )
			GEngine->pcfWorld->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 1 );
#else
			GEngine->World->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 1 );
		else if( m_bFloors )
			GEngine->World->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 1 );
#endif
	}
	else
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.Selection(0,NULL,1);
		else if( m_bFloors )
			GEngine->pcfWorld->Paths.FloorSelection(NULL,1);
#else
			GEngine->World->Paths.Selection(0,NULL,1);
		else if( m_bFloors )
			GEngine->World->Paths.FloorSelection(NULL,1);
#endif
	}
}

void CMainFrame::OnPathfindingRecalculatefloors()
{
	if( m_bNewWayPoints )
	{
#ifdef FLOORS_ENABLED
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.PrepareFloors();
#else
		GEngine->World->Paths2.PrepareFloors();
#endif
#endif
	}
	else
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.PrepareFloors();
#else
		GEngine->World->Paths.PrepareFloors();
#endif
}

void CMainFrame::OnWaypointsConnectDisconnect()
{
	static CConnectWayPointsDlg dlg(1.0f,10.0f,true,true);

	if( m_bWayPoints && ( dlg.DoModal() == IDOK ) )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selected_ConnectDisconnect( dlg.MinDistance, dlg.MaxDistance, dlg.Connect, dlg.Disconnect );
		else
			GEngine->pcfWorld->Paths.ConnectDisconnectSelected( dlg.MinDistance, dlg.MaxDistance, dlg.Connect, dlg.Disconnect );
#else
			GEngine->World->Paths2.Selected_ConnectDisconnect( dlg.MinDistance, dlg.MaxDistance, dlg.Connect, dlg.Disconnect );
		else
			GEngine->World->Paths.ConnectDisconnectSelected( dlg.MinDistance, dlg.MaxDistance, dlg.Connect, dlg.Disconnect );
#endif
	}
}

void CMainFrame::OnWaypointsSelectOnFloor()
{
	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Select_OnSelectedFloors( true );
		else
			GEngine->pcfWorld->Paths.SelectUnselectWaypointsOnSelectedFloors(1);
#else
			GEngine->World->Paths2.Select_OnSelectedFloors( true );
		else
			GEngine->World->Paths.SelectUnselectWaypointsOnSelectedFloors(1);
#endif
	}
}

void CMainFrame::OnWaypointsUnselectOnFloor()
{
	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Select_OnSelectedFloors( false );
		else
			GEngine->pcfWorld->Paths.SelectUnselectWaypointsOnSelectedFloors(0);
#else
			GEngine->World->Paths2.Select_OnSelectedFloors( false );
		else
			GEngine->World->Paths.SelectUnselectWaypointsOnSelectedFloors(0);
#endif
	}
}

void CMainFrame::OnWaypointsConnectSelected()
{
	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selected_Connect();
		else
			GEngine->pcfWorld->Paths.ConnectSelected();
#else
			GEngine->World->Paths2.Selected_Connect();
		else
			GEngine->World->Paths.ConnectSelected();
#endif
	}
}

void CMainFrame::OnWaypointsDisconnectSelected()
{
	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selected_Disconnect();
		else
			GEngine->pcfWorld->Paths.DisconnectSelected();
#else
			GEngine->World->Paths2.Selected_Disconnect();
		else
			GEngine->World->Paths.DisconnectSelected();
#endif
	}
}

void CMainFrame::OnWaypointsCopySelected()
{
	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
		{
			Vector displacement( 0.5f, 0.0f, 0.0f );
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selected_Copy( displacement );
#else
			GEngine->World->Paths2.Selected_Copy( displacement );
#endif
		}
		else
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.CopySelected();
#else
			GEngine->World->Paths.CopySelected();
#endif
	}
}

void CMainFrame::OnWaypointsDeleteselected()
{
	if( m_bNewWayPoints )
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Selected_Remove();
#else
			GEngine->World->Paths2.Selected_Remove();
#endif
#ifdef FLOORS_ENABLED
		else if( m_bFloors )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.SelectedFloors_Remove();
#else
			GEngine->World->Paths2.SelectedFloors_Remove();
#endif // TRANSGAMING
#endif // FLOORS_ENABLED
	}
	else
	{
		if( m_bWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.RemoveSelected();
		else if( m_bFloors )
			GEngine->pcfWorld->Paths.RemoveSelectedFloors();
#else
			GEngine->World->Paths.RemoveSelected();
		else if( m_bFloors )
			GEngine->World->Paths.RemoveSelectedFloors();
#endif
	}
}

void CMainFrame::OnWaypointsMoveSelectedToTheZone()
{
    if( m_bWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.MoveSelectedToZoneTheyreIn();
#else
        GEngine->World->Paths.MoveSelectedToZoneTheyreIn();
#endif
}

void CMainFrame::OnWaypointsRemoveCollidingConnections()
{
	static CGenerateWaypointsDlg dlg("Enter Displacement Threshold","Threshold",0.3f);

	if( m_bWayPoints )
	{
		if( m_bNewWayPoints )
		{
			if( dlg.DoModal() == IDOK )
#ifdef TRANSGAMING
				GEngine->pcfWorld->Paths2.Selected_RemoveConnectionsCollidingWithGeometry( dlg.Param );
#else
				GEngine->World->Paths2.Selected_RemoveConnectionsCollidingWithGeometry( dlg.Param );
#endif
		}
		else
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.RemoveConnectionsCollidingWithGeometryInSelected();
#else
			GEngine->World->Paths.RemoveConnectionsCollidingWithGeometryInSelected();
#endif
	}
}

void CMainFrame::OnSelectUnselectFloors()
{
	static CSelectFloorsDlg dlg(true,true,1);

	if( m_bFloors )
	{
		dlg.DoModal();
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.Select_FloorsOfAreaLowerHigherThan( (dlg.Select != 0), (dlg.Lower != 0), dlg.Area );
		else
			GEngine->pcfWorld->Paths.SelectUnselectFloorsOfAreaLowerHigherThan( dlg.Select, dlg.Lower, dlg.Area );
#else
			GEngine->World->Paths2.Select_FloorsOfAreaLowerHigherThan( (dlg.Select != 0), (dlg.Lower != 0), dlg.Area );
		else
			GEngine->World->Paths.SelectUnselectFloorsOfAreaLowerHigherThan( dlg.Select, dlg.Lower, dlg.Area );
#endif
	}		
}

void CMainFrame::OnPathfindingPrepareportalnodes()
{
	static CGenerateWaypointsDlg dlg("Prepare Portal Nodes:","Param",10);
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.GenerateAutomaticStructures();
	else if (dlg.DoModal() == IDOK)
		GEngine->pcfWorld->Paths.PreparePortalNodes(dlg.Param);
#else
		GEngine->World->Paths2.GenerateAutomaticStructures();
	else if (dlg.DoModal() == IDOK)
		GEngine->World->Paths.PreparePortalNodes(dlg.Param);
#endif
}

void CMainFrame::OnPathfindingGenerateAuto()
{
	if( m_bNewWayPoints )
	{
		GLog.Print( "Recalculating automatic structures..." );
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ClearAutomaticStructures();
		GEngine->pcfWorld->Paths2.GenerateAutomaticStructures();
#else
		GEngine->World->Paths2.ClearAutomaticStructures();
		GEngine->World->Paths2.GenerateAutomaticStructures();
#endif
		GLog.Print( "done.\n" );
	}
}

void CMainFrame::OnPathfindingLoadmap()
{
	if( m_bNewWayPoints )
	{
		char szFilters[] = "New WalkMap Files (*.wps)|*.wps||";

		CFileDialog fileDlg( TRUE, "wps", "*.wps", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this );
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal() == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.LoadContents(pathName);
#else
			GEngine->World->Paths2.LoadContents(pathName);
#endif // TRANSGAMING
#ifdef FLOORS_ENABLED
			pathName.Replace(".wps",".fls");
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.LoadFloors(pathName);
#else
			GEngine->World->Paths2.LoadFloors(pathName);
#endif // TRANSGAMING
#endif // FLOORS_ENABLED
		}
	}
	else
	{
		char szFilters[] = "WalkMap Files (*.wpm)|*.wpm||";
		CFileDialog fileDlg (TRUE, "wpm", "*.wpm", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this);
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal () == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.LoadContents(pathName);
			pathName.Replace(".wpm",".flm");
			GEngine->pcfWorld->Paths.LoadFloors(pathName);
#else
			GEngine->World->Paths.LoadContents(pathName);
			pathName.Replace(".wpm",".flm");
			GEngine->World->Paths.LoadFloors(pathName);
#endif
		}
	}
}
void CMainFrame::OnPathfindingLoadmapWithoutPortals()
{
	if( m_bNewWayPoints )
	{
		char szFilters[]= "New WalkMap Files (*.wps)|*.wps||";
		CFileDialog fileDlg( TRUE, "wps", "*.wps", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this );
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal () == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.LoadContents( pathName, 1.0f, true );
#else
			GEngine->World->Paths2.LoadContents( pathName, 1.0f, true );
#endif

#ifdef FLOORS_ENABLED
			pathName.Replace( ".wps", ".fls" );
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.LoadFloors( pathName );
#else
			GEngine->World->Paths2.LoadFloors( pathName );
#endif // TRANSGAMING
#endif
		}
	}
	else
	{
		char szFilters[]= "WalkMap Files (*.wpm)|*.wpm||";

		CFileDialog fileDlg (TRUE, "wpm", "*.wpm", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this);
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal ()==IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.LoadContents(pathName,1.0f,true);
			pathName.Replace(".wpm",".flm");
			GEngine->pcfWorld->Paths.LoadFloors(pathName);
#else
			GEngine->World->Paths.LoadContents(pathName,1.0f,true);
			pathName.Replace(".wpm",".flm");
			GEngine->World->Paths.LoadFloors(pathName);
#endif
		}
	}
}

void CMainFrame::OnPathfindingMergeMap()
{
	if( m_bNewWayPoints )
	{
		char szFilters[]= "New WalkMap Files (*.wps)|*.wps||";
		CFileDialog fileDlg( TRUE, "wps", "*.wps", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY|OFN_NOCHANGEDIR, szFilters, this );
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal () == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.MergeContents( pathName, 1.0f );
#else
			GEngine->World->Paths2.MergeContents( pathName, 1.0f );
#endif
		}
	}
}

void CMainFrame::OnPathfindingSavemap()
{
	if( m_bNewWayPoints )
	{
		bool generateStructs = ( AfxMessageBox( "Save as Final Build (generate set/portal structures)?", MB_YESNO ) == IDYES );
		char szFilters[] = "New WalkMap Files (*.wps)|*.wps";

		CFileDialog fileDlg (FALSE, "wps", "*.wps", OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, szFilters, this);
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal () == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.SaveContents( pathName, 1.0f, generateStructs );
#else
			GEngine->World->Paths2.SaveContents( pathName, 1.0f, generateStructs );
#endif
#ifdef FLOORS_ENABLED
			pathName.Replace(".wps",".fls");
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.SaveFloors( pathName );
#else
			GEngine->World->Paths2.SaveFloors( pathName );
#endif // TRANSGAMING
#endif
		}
	}
	else
	{
		char szFilters[] = "WalkMap Files (*.wpm)|*.wpm";

		CFileDialog fileDlg (FALSE, "wpm", "*.wpm", OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR, szFilters, this);
		fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\Maps";
		if( fileDlg.DoModal () == IDOK )
		{
			CString pathName = fileDlg.GetPathName();
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths.SaveContents(pathName);
			pathName.Replace(".wpm",".flm");
			GEngine->pcfWorld->Paths.SaveFloors(pathName);
#else
			GEngine->World->Paths.SaveContents(pathName);
			pathName.Replace(".wpm",".flm");
			GEngine->World->Paths.SaveFloors(pathName);
#endif
		}
	}
}

void CMainFrame::OnViewFloors()
{
	m_bFloors=!m_bFloors;
	GScript.DoString("Editor:OnMsg('OnEnableFloorsEdit',%d)",m_bFloors);
	
    m_bFloors ? GEngine->Video->GetActiveView()->Mode |= View::EModeShowFloors : GEngine->Video->GetActiveView()->Mode &= ~View::EModeShowFloors;	
}

void CMainFrame::OnUpdateViewFloors(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bFloors);
}

void CMainFrame::OnPathfindingGeneratewaypoints()
{
	static CGenerateWaypointsDlg dlg("Generate Waypoints from Floors","Step",5);
	if( dlg.DoModal() == IDOK )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.SelectedFloors_AddGrid( dlg.Param );
		else
			GEngine->pcfWorld->Paths.AddGridOnSelectedFloors( dlg.Param );
#else
			GEngine->World->Paths2.SelectedFloors_AddGrid( dlg.Param );
		else
			GEngine->World->Paths.AddGridOnSelectedFloors( dlg.Param );
#endif
	}
}



void CMainFrame::OnWaypointsSelectnotconnectedtoanythingincurrentroom()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.Select_NotConnectedToAnything();
	else
		GEngine->pcfWorld->Paths.SelectWaypointsNotConnectedToAnythingInCurrentRoom();
#else
		GEngine->World->Paths2.Select_NotConnectedToAnything();
	else
		GEngine->World->Paths.SelectWaypointsNotConnectedToAnythingInCurrentRoom();
#endif
}

void CMainFrame::OnPathfindingCleanstructures()
{
#ifdef TRANSGAMING
	GEngine->pcfWorld->Paths.CleanStructures();
#else
	GEngine->World->Paths.CleanStructures();
#endif
}

void CMainFrame::OnPathfindingScalecontents()
{
	static CGenerateWaypointsDlg dlg("Scale Contents","Scale",1);
	if( dlg.DoModal() == IDOK )
	{
		if( m_bNewWayPoints )
#ifdef TRANSGAMING
			GEngine->pcfWorld->Paths2.ScaleContents( dlg.Param );
		else
			GEngine->pcfWorld->Paths.ScaleContents( dlg.Param );
#else
			GEngine->World->Paths2.ScaleContents( dlg.Param );
		else
			GEngine->World->Paths.ScaleContents( dlg.Param );
#endif
	}
}

void CMainFrame::OnWaypointsSelectoutsideofcurrentzone()
{
	static CGenerateWaypointsDlg dlg("Select Waypoints Outside of Current Zone","P.Thickness",10);
	if( ( dlg.DoModal() == IDOK ) && !m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.SelectWaypointsOutsideOfCurrentZone( dlg.Param );
#else
		GEngine->World->Paths.SelectWaypointsOutsideOfCurrentZone( dlg.Param );
#endif
}

void CMainFrame::OnPathfindingLevelwaypointswithfloors()
{
	static CGenerateWaypointsDlg dlg("Level Waypoints with Floors","Up Displ.",0.1f);
	if( dlg.DoModal() == IDOK )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.LevelWaypointsWithFloors( dlg.Param );
#else
		GEngine->World->Paths.LevelWaypointsWithFloors( dlg.Param );
#endif
}

void CMainFrame::OnWaypointsSetselectedasforsmallmonstersonly()
{
	if( !m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.SetSelectedAsForSmallMonstersOnly();
#else
		GEngine->World->Paths.SetSelectedAsForSmallMonstersOnly();
#endif
}

void CMainFrame::OnWaypointsSetselectedasforallmonsters()
{
	if( !m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths.SetSelectedAsForAllMonsters();
#else
		GEngine->World->Paths.SetSelectedAsForAllMonsters();
#endif
}

void CMainFrame::OnWaypointsInvertselection()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.Select_Invert();
	else
		GEngine->pcfWorld->Paths.InvertSelection();
#else
		GEngine->World->Paths2.Select_Invert();
	else
		GEngine->World->Paths.InvertSelection();
#endif
}

void CMainFrame::OnFileSaveLevel()
{	
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();
	if (strLevName == "NoName")
		OnFileSaveAllAs();
	else
		GScript.DoString("GObjects:SaveToCurrentLevel(%s)",strLevName);
}

void CMainFrame::OnFileSaveAll()
{
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();
	if (strLevName == "NoName")
		OnFileSaveAllAs();
	else
		GScript.DoString("Game:SaveLevel('%s',true)",strLevName);
}

void CMainFrame::OnFileSaveAllAs()
{	
	CSaveLevelDlg dlg;
	dlg.DoModal();
	//m_wndLevelExplorer.m_pLevelView->UpdateView("");
#ifdef EDITOR
    m_PropertiesBar.UpdateView("");
    CGameObjectsBar::Get().UpdateView("");
#endif
}

void CMainFrame::OnNewView()
{
	CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_MAINFRAME, NULL, NULL);
}

void CMainFrame::OnFileNew()
{
#ifdef TRANSGAMING
	GEngine->pcfWorld->FileName = ""; // zawsze na nowo laduje mape
#else
    GEngine->World->FileName = ""; // zawsze na nowo laduje mape
#endif
    GScript.DoString("Game:NewLevel('NoName')");
	CSaveLevelDlg dlg(NULL,"Create New Project");
	dlg.DoModal();
#ifdef EDITOR
    m_PropertiesBar.UpdateView("");
    CGameObjectsBar::Get().UpdateView("");
#endif
}

void CMainFrame::OnExtractGameData()
{
	if (MessageBox("Press 'OK' to start extract data (a few minutes)!", "Extract Game Data",MB_OKCANCEL) == IDOK)
	{
		CreateDirectory("../Data/Textures",0);
		CreateDirectory("../Data/Levels",0);
		CreateDirectory("../Data/Maps",0);
		GScript.DoString("FS.ExtractPack('../Data/Levels.pak','../Data/Levels')");
		GScript.DoString("FS.ExtractPack('../Data/Maps.pak','../Data/Maps')");
		GScript.DoString("FS.ExtractPack('../Data/Maps1.pak','../Data/Maps')");
		GScript.DoString("FS.ExtractPack('../Data/Maps2.pak','../Data/Maps')");
		GScript.DoString("FS.ExtractPack('../Data/Textures.pak','../Data/Textures')");
		GScript.DoString("FS.ExtractPack('../Data/Textures1.pak','../Data/Textures')");
		GScript.DoString("FS.ExtractPack('../Data/Textures2.pak','../Data/Textures')");
		MessageBox("Successful", "Extract Game Data");
	}
}

static HWND gHwnd = NULL;
BOOL CALLBACK mycompareproc(HWND hwndChild, LPARAM lParam) 
{
	LPCSTR pcs=(LPCSTR)lParam;	
	char lpstring[500];
	GetWindowText(hwndChild,lpstring,500);

	if(strcmp(lpstring,pcs)==0)
	{
		gHwnd = hwndChild;
		return FALSE; //stop the enumeration since we have found the 
	}

	return TRUE;
}

CMDIChildWnd* CMainFrame::FindChildWindow(CString title) 
{
	gHwnd = NULL;
	EnumChildWindows(m_hWndMDIClient,mycompareproc,(LPARAM)(LPCSTR)title);
	if (!gHwnd) return NULL;
	return (CMDIChildWnd*) CWnd::FromHandle(gHwnd);
}

void CMainFrame::OnRenderviewTextures()
{
    GEngine->Video->GetActiveView()->Mode ^= View::EModeShowTextures;
}

void CMainFrame::OnUpdateRenderviewTextures(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowTextures ? 1:0 );
}

void CMainFrame::OnRenderviewLightmaps()
{
    GEngine->Video->GetActiveView()->Mode ^= View::EModeShowLightmaps;
}

void CMainFrame::OnUpdateRenderviewLightmaps(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowLightmaps ? 1:0 );
}

void CMainFrame::OnRenderviewWireframe()
{
    GEngine->Video->GetActiveView()->Mode ^= View::EModeShowTris;
}

void CMainFrame::OnUpdateRenderviewWireframe(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowTris ? 1:0 );
}

void CMainFrame::OnRenderviewZones()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeShowZones;
}

void CMainFrame::OnUpdateRenderviewZones(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowZones ? 1:0 );
}

void CMainFrame::OnRenderviewBoxes()
{
    GEngine->Video->GetActiveView()->Mode ^= View::EModeShowBoxes;
}

void CMainFrame::OnUpdateRenderviewBoxes(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowBoxes ? 1:0 );
}

void CMainFrame::OnRenderviewModels()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeShowModels;
}

void CMainFrame::OnRenderviewVolumetrics()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeShowVolFog;
}

void CMainFrame::OnUpdateRenderviewModels(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowModels ? 1:0 );
}

void CMainFrame::OnUpdateRenderviewVolumetrics(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowVolFog ? 1:0 );
}

void CMainFrame::OnRenderviewVisibility()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeVisibility;
}

void CMainFrame::OnUpdateRenderviewVisibility(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeVisibility ? 1:0 );
}

void CMainFrame::OnEditCommand()
{
	try
	{
		static CSetValueDlg dlg(NULL,"LUA Script Command:");
		if(dlg.DoModal() == IDOK )
        {
            GEngine->ResetTimer();
            GScript.DoString(dlg.m_Value);
            GEngine->ResetTimer();
        }
	}
    catch(...) 
	{
		MessageBox("Script error","LUA",MB_TOPMOST);
    }
}

void CMainFrame::OnRenderviewMonstersight()
{
}

void CMainFrame::OnUpdateRenderviewMonstersight(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(0);
}

void CMainFrame::OnRenderviewPhysicalmodel()
{
}

void CMainFrame::OnUpdateRenderviewPhysicalmodel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(0);
}

void CMainFrame::OnRenderviewHavokprimitives()
{
	GEngine->Video->GetActiveView()->Mode ^= View::EModeShowHKPrimitives;
}

void CMainFrame::OnUpdateRenderviewHavokprimitives(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( GEngine->Video->GetActiveView()->Mode & View::EModeShowHKPrimitives? 1:0 );
}

void CMainFrame::OnLightsScale()
{
	static CGenerateWaypointsDlg dlg("Scale Lights","Factor",1.0f);
	if (dlg.DoModal() == IDOK)
    {
		GScript.DoString("GObjects:Rescale(%f,'CLight')",dlg.Param);
        GScript.DoString("GObjects:Rescale(%f,'CEnvironment')",dlg.Param);
    }
}

void CMainFrame::OnItemsScale()
{
    static CGenerateWaypointsDlg dlg("Scale Items","Factor",1.0f);
    if (dlg.DoModal() == IDOK)
        GScript.DoString("GObjects:Rescale(%f,'CItem')",dlg.Param);
}

void CMainFrame::OnLightsSaveAll()
{
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
	if (strLevName == "NoName")
		MessageBox("You must first create project.");
	else
    {
        GScript.DoString("GObjects:Save('CLight')");
        GScript.DoString("GObjects:Save('CEnvironment')");
    }    
}


void CMainFrame::OnFileClose()
{
#ifdef EDITOR
	if (MessageBox("Are you sure?","Close Project",MB_YESNO) == IDYES)
	{
        GScript.DoString("Game:NewLevel('NoName','','',0.3);Lev:Apply();EDITOR.OnMsg('SelectObject',Lev._Name)");
        CGameObjectsBar::Get().UpdateView("",true);
	}
#endif
}

void CMainFrame::OnObjectsNewobject()
{
#ifdef EDITOR
	static CNewGameObjectDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
#endif
}

void CMainFrame::OnItemsSaveall()
{
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
	if (strLevName == "NoName")
		MessageBox("You must first create project.");
	else
		GScript.DoString("GObjects:Save('CItem')");
}

void CMainFrame::OnAreasSaveall()
{
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
	if (strLevName == "NoName")
		MessageBox("You must first create project.");
	else
		GScript.DoString("GObjects:Save('CArea')");
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
#ifdef EDITOR
	CXTPMDIFrameWnd::OnSetFocus(pOldWnd);
#endif
	if( GEngine && GEngine->InputSys )
		GEngine->InputSys->Reset();
}

void CMainFrame::OnSoundsSaveall()
{
	CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
	if (strLevName == "NoName")
		MessageBox("You must first create project.");
	else
		GScript.DoString("GObjects:Save('CSound')");
}

void CMainFrame::OnTweakEdit()
{
	((CPainEditorApp*)AfxGetApp())->ExecuteProgram("wscite\\scite.exe tweak.ini",NULL);
}

void CMainFrame::OnTweakApply()
{
	GScript.DoString("Tweak:Load()");
}

void CMainFrame::NewFileView(LPCSTR file)
{
	AfxGetApp()->OpenDocumentFile(file);
}

void CMainFrame::OnReloadTemplates()
{
    GScript.DoString("Game:ReloadTemplates()");
#ifdef EDITOR
    CTemplatesBar::Get().UpdateView("../Data/LScripts/Templates");
#endif
}


void CMainFrame::UpdateResolutionCombo()
{
#ifdef EDITOR
	if (Game())
	{
		CString res = GScript.Globals()["Cfg"]["Resolution"].GetString();
		res.Replace('X','x');
		m_cmbResolution.SetCurSel(m_cmbResolution.FindString(-1,res));
	}
#endif
}


LRESULT CMainFrame::OnScriptMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1) // restart level
	{
        GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;
        GEngine->SwitchToState(PCFSystem::SingleGame);
#ifdef TRANSGAMING
		GEngine->pcfWorld->FileName = "";
#else
		GEngine->World->FileName = "";
#endif
        OnReloadLevel();
        OnPlay();
        GScript.DoString("Game:SwitchPlayerToPhysics(true)");
		GEngine->ResetTimer();
	}
	return 0;
}

void CMainFrame::OnPathfindingImportoldwaypoints()
{
#ifdef TRANSGAMING
	GEngine->pcfWorld->Paths2.ImportFromOldPathfinder();
#else
    GEngine->World->Paths2.ImportFromOldPathfinder();
#endif
}

void CMainFrame::OnWaypointsGetSetFromSelected()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.GetCurrentSetFromSelected();
#else
		GEngine->World->Paths2.GetCurrentSetFromSelected();
#endif
}

void CMainFrame::OnWaypointsMakeNewSetFromSelected()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.MakeNewSetFromSelected();
#else
		GEngine->World->Paths2.MakeNewSetFromSelected();
#endif
}

void CMainFrame::OnWaypointsExpandCurrent()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ExpandCurrentSet();
#else
		GEngine->World->Paths2.ExpandCurrentSet();
#endif
}

void CMainFrame::OnWaypointsContractCurrent()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ContractCurrentSet();
#else
		GEngine->World->Paths2.ContractCurrentSet();
#endif
}

void CMainFrame::OnWaypointsValidateSets()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ValidateSets();
#else
		GEngine->World->Paths2.ValidateSets();
#endif
}

void CMainFrame::OnWaypointsMergeWithSelected()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.MergeSetsFromSelected();
#else
		GEngine->World->Paths2.MergeSetsFromSelected();
#endif
}

void CMainFrame::OnWaypointsMoveSelectedToCurrentSet()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.Selected_MoveToCurrentSet();
#else
		GEngine->World->Paths2.Selected_MoveToCurrentSet();
#endif
}

void CMainFrame::OnWaypointsSelectAllInCurrentSet()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.Select_AllInSet( true, GEngine->pcfWorld->Paths2.GetCurrentSet() );
#else
		GEngine->World->Paths2.Select_AllInSet( true, GEngine->World->Paths2.GetCurrentSet() );
#endif
}

void CMainFrame::OnWaypointsUnselectAllInCurrentSet()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.Select_AllInSet( false, GEngine->pcfWorld->Paths2.GetCurrentSet() );
#else
		GEngine->World->Paths2.Select_AllInSet( false, GEngine->World->Paths2.GetCurrentSet() );
#endif
}

void CMainFrame::OnWaypointsViewMore()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ChangeViewLimits( true );
#else
		GEngine->World->Paths2.ChangeViewLimits( true );
#endif
}

void CMainFrame::OnWaypointsViewLess()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ChangeViewLimits( false );
#else
		GEngine->World->Paths2.ChangeViewLimits( false );
#endif
}

void CMainFrame::OnWaypointsUndoDisplacement()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.ApplyUndo();
#else
		GEngine->World->Paths2.ApplyUndo();
#endif
}

void CMainFrame::OnWaypointsDrawDisabledSets()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.DrawDisabledSets = !GEngine->pcfWorld->Paths2.DrawDisabledSets;
#else
		GEngine->World->Paths2.DrawDisabledSets = !GEngine->World->Paths2.DrawDisabledSets;
#endif
}

void CMainFrame::OnUpdateDrawDisabledSets(CCmdUI *pCmdUI)
{
#ifdef TRANSGAMING
	pCmdUI->SetCheck(m_bNewWayPoints?GEngine->pcfWorld->Paths2.DrawDisabledSets:false);
#else
	pCmdUI->SetCheck(m_bNewWayPoints?GEngine->World->Paths2.DrawDisabledSets:false);
#endif
}

void CMainFrame::OnWaypointsEnableAllSets()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.EnableSets( true );
#else
		GEngine->World->Paths2.EnableSets( true );
#endif
}

void CMainFrame::OnWaypointsDisableAllSets()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.EnableSets( false );
#else
		GEngine->World->Paths2.EnableSets( false );
#endif
}

void CMainFrame::OnWaypointsMergeClose()
{
	static CGenerateWaypointsDlg dlg("Merge Close Waypoints","Below:",0.01f);
	if( m_bNewWayPoints && ( dlg.DoModal() == IDOK ) )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.MergeWaypointsBelowDistance( dlg.Param );
#else
		GEngine->World->Paths2.MergeWaypointsBelowDistance( dlg.Param );
#endif
}

void CMainFrame::OnWaypointsRandomizeSets()
{
	if( m_bNewWayPoints )
#ifdef TRANSGAMING
		GEngine->pcfWorld->Paths2.RandomizeSets();
#else
		GEngine->World->Paths2.RandomizeSets();
#endif
}

void CMainFrame::ReleaseParticleEditor()
{
#ifdef EDITOR
    ParticleEffect *pfx1 = ((CParticleFXEditorDlg*) m_wndParticleFXDlg.m_InnerDlg)->Effect;
    ParticleEffect *pfx2 = ((CEmitterEditorDlg*)    m_wndEmitterDlg.m_InnerDlg)->PFX;
#ifdef TRANSGAMING
    if (pfx1)
        GEngine->pcfWorld->DeleteEntity(pfx1);
    if (pfx2)
        GEngine->pcfWorld->DeleteEntity(pfx2);
#else
    if (pfx1)
        GEngine->World->DeleteEntity(pfx1);
    if (pfx2)
        GEngine->World->DeleteEntity(pfx2);
#endif
    ((CParticleFXEditorDlg*) m_wndParticleFXDlg.m_InnerDlg)->Effect = NULL;
    ((CEmitterEditorDlg*)    m_wndEmitterDlg.m_InnerDlg)->PE = NULL;
    ((CEmitterEditorDlg*)    m_wndEmitterDlg.m_InnerDlg)->PFX = NULL;
#endif
}


#ifdef EDITOR

void CMainFrame::OutputText(LPCSTR txt)
{
	if (!m_editOutput.GetSafeHwnd() || !txt)
		return;

	if (txt[0] != '^' && txt[0] != '*')
	{
		CString string = txt;
		m_editOutput.SetRedraw(false);
		string.Replace("\n","\r\n");
		if (m_EditLogger.GetEditCtrl())
		{
			m_EditLogger.AddText(string);
			m_editOutput.LineScroll(m_editOutput.GetLineCount());
			m_editOutput.SetRedraw();
		}
	}
}

#endif

void CMainFrame::OnObjectsResetselected()
{
#ifdef EDITOR
	CGameObjectsBar::Get().OnCommand(PM_Reset,0);
#endif
}

void CMainFrame::OnObjectsSaveselected()
{
#ifdef EDITOR
	CGameObjectsBar::Get().OnCommand(PM_Save,0);
#endif
}
