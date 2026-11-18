// MainFrm.h : interface of the CMainFrame class
//
#include "resource.h"
#include "EmitterEditorDlg.h" 
#include "ParticleFXEditorDlg.h" 
#include "ScrolledDialogBar.h" 
#include "TemplatesBar.h" 
#include "OutputDlg.h" 
#include "WatchBar.h" 
#include "PropertiesBar.h"

#pragma once

#ifdef EDITOR
	class CMainFrame : public CXTPMDIFrameWnd
#else
	class CMainFrame : public CMDIFrameWnd
#endif
{


	bool m_bPausedScripts;
	bool m_bEnabledSaveAll;
    bool m_bWayPoints,m_bFloors,m_bNewWayPoints,m_bAreas,m_bLights,m_bSounds,m_bBillboards,m_bAcousticEnvs;
	bool m_bDisabledDataCache,m_bDisabledAudio;
	
public:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
#ifdef EDITOR
	CXTPTabClientWnd m_MTIClientWnd;
	CTemplatesBar m_wndTemplatesBar;   
	CXTPEdit			m_editOutput;
	CEditLog			m_EditLogger;
	std::editstreambuf	m_EditStrBuf;
	CWatchBar      m_WatchBar;
	CPropertiesBar m_PropertiesBar;

	//CResourceExplorerDlg  m_wndResourceExplorer;
	CScrolledDialogBar m_wndEmitterDlg;
	CScrolledDialogBar m_wndParticleFXDlg;
//	CWatchBar           m_wndWatchBar; 
	///CTemplateBar m_wndTemplateBar;
	CXTPControlComboBox m_cmbResolution;

	CXTPDockingPaneManager m_paneManager;
	CXTPDockingPaneManager* GetDockingPaneManager() 
	{
		return &m_paneManager;
	}

	void OutputText(LPCSTR txt);
#endif
	CMDIChildWnd * FindChildWindow(CString title);


// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void LoadSettings();

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	FILE *fout;
#ifdef EDITOR
	CXTPToolBar*  m_pToolBar;
	CXTPToolBar*  m_pPathfindingToolBar;
	CXTStatusBar  m_wndStatusBar;
#endif

// Generated message map functions
protected:
	void DockControlBottomOf(CControlBar* Bar,CControlBar* BottomOf);

#ifdef EDITOR
	afx_msg int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);
#endif
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnFullScreen();
	afx_msg void OnWayPoints();
    afx_msg void OnNewWayPoints();
    afx_msg void OnReloadLevel();
    afx_msg void OnReloadLevelAndScripts();
    afx_msg void OnReloadLevelAndScriptsAndMap();
	afx_msg void OnDisableScripts();
	afx_msg void OnDisableAudio();
	afx_msg void OnDisableCache();
	afx_msg void OnUpdateWayPoints(CCmdUI* pCmdUI);
    afx_msg void OnUpdateNewWayPoints(CCmdUI* pCmdUI);

public:
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnUpdatePause(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlay(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisableScripts(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisableAudio(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisableCache(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveLevel();	
	afx_msg void OnFileSaveAllAs();
	afx_msg void OnFileSaveAll();
	afx_msg void OnUpdateFileSaveAll(CCmdUI *pCmdUI);
	afx_msg void OnShowView(UINT nID);

	afx_msg void OnRunGame();
	void UpdateResolutionCombo();
	void AddObjectTemplate(LPCSTR folder,LPCSTR item,LPCSTR file,int icon);
	void NewFileView(LPCSTR file);
	void EnableSave(bool enable);
    void ReleaseParticleEditor();
	BOOL ShowWindowEx(int nCmdShow);


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewWalkmap();
	afx_msg void OnFileLoadlevel();
	
	afx_msg void OnViewJoints();
	afx_msg void OnUpdateViewJoints(CCmdUI *pCmdUI);
	afx_msg void OnViewAreas();
	afx_msg void OnUpdateViewAreas(CCmdUI *pCmdUI);
    afx_msg void OnViewAcousticEnvs();
    afx_msg void OnUpdateViewAcousticEnvs(CCmdUI *pCmdUI);
	afx_msg void OnViewBillboards();
	afx_msg void OnUpdateViewBillboards(CCmdUI *pCmdUI);
	afx_msg void OnViewLights();
	afx_msg void OnUpdateViewLights(CCmdUI *pCmdUI);
	afx_msg void OnViewSounds();
	afx_msg void OnReloadTextures();
    afx_msg void OnFileSystem();
	afx_msg void OnUpdateViewSounds(CCmdUI *pCmdUI);
	afx_msg void OnWaypointsUnselectall();
	afx_msg void OnWaypointsSelectall();
	afx_msg void OnPathfindingRecalculatefloors();
	afx_msg void OnWaypointsConnectDisconnect();
	afx_msg void OnWaypointsDeleteselected();
    afx_msg void OnWaypointsMoveSelectedToTheZone();
	afx_msg void OnWaypointsSelectOnFloor();
	afx_msg void OnWaypointsUnselectOnFloor();
	afx_msg void OnWaypointsConnectSelected();
	afx_msg void OnWaypointsDisconnectSelected();
	afx_msg void OnWaypointsCopySelected();
	afx_msg void OnWaypointsRemoveCollidingConnections();
	afx_msg void OnSelectUnselectFloors();
	afx_msg void OnWaypointsGetSetFromSelected();
	afx_msg void OnWaypointsMakeNewSetFromSelected();
	afx_msg void OnWaypointsExpandCurrent();
	afx_msg void OnWaypointsContractCurrent();
	afx_msg void OnWaypointsValidateSets();
	afx_msg void OnWaypointsMergeWithSelected();
	afx_msg void OnWaypointsMoveSelectedToCurrentSet();
	afx_msg void OnWaypointsSelectAllInCurrentSet();
	afx_msg void OnWaypointsUnselectAllInCurrentSet();
	afx_msg void OnWaypointsViewMore();
	afx_msg void OnWaypointsViewLess();
	afx_msg void OnWaypointsUndoDisplacement();
	afx_msg void OnWaypointsDrawDisabledSets();
	afx_msg void OnUpdateDrawDisabledSets(CCmdUI *pCmdUI);
	afx_msg void OnWaypointsEnableAllSets();
	afx_msg void OnWaypointsDisableAllSets();
	afx_msg void OnWaypointsMergeClose();
	afx_msg void OnWaypointsRandomizeSets();

	afx_msg void OnPathfindingPrepareportalnodes();
	afx_msg void OnPathfindingGenerateAuto();
	afx_msg void OnPathfindingLoadmap();
	afx_msg void OnPathfindingLoadmapWithoutPortals();
	afx_msg void OnPathfindingMergeMap();
	afx_msg void OnPathfindingSavemap();
	afx_msg void OnViewFloors();
	afx_msg void OnUpdateViewFloors(CCmdUI *pCmdUI);
	afx_msg void OnPathfindingGeneratewaypoints();
	afx_msg void OnViewMode();
	afx_msg void OnResolution();
	afx_msg void OnFileNew();
	afx_msg void OnExtractGameData();
	afx_msg void OnReloadTemplates();
	afx_msg void OnNewView();
	afx_msg void OnWaypointsSelectnotconnectedtoanythingincurrentroom();
	afx_msg void OnPathfindingCleanstructures();
	afx_msg void OnPathfindingScalecontents();
	afx_msg void OnWaypointsSelectoutsideofcurrentzone();
	afx_msg void OnPathfindingLevelwaypointswithfloors();
	afx_msg void OnWaypointsSetselectedasforallmonsters();
	afx_msg void OnWaypointsSetselectedasforsmallmonstersonly();
	afx_msg void OnWaypointsInvertselection();
	afx_msg void OnRenderviewTextures();
	afx_msg void OnUpdateRenderviewTextures(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewLightmaps();
	afx_msg void OnUpdateRenderviewLightmaps(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewWireframe();
	afx_msg void OnUpdateRenderviewWireframe(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewZones();
	afx_msg void OnUpdateRenderviewZones(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewBoxes();
	afx_msg void OnUpdateRenderviewBoxes(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewModels();
	afx_msg void OnUpdateRenderviewModels(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewVolumetrics();
	afx_msg void OnUpdateRenderviewVolumetrics(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewVisibility();
	afx_msg void OnUpdateRenderviewVisibility(CCmdUI *pCmdUI);
	afx_msg void OnEditCommand();
	afx_msg void OnRenderviewMonstersight();
	afx_msg void OnUpdateRenderviewMonstersight(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewPhysicalmodel();
	afx_msg void OnUpdateRenderviewPhysicalmodel(CCmdUI *pCmdUI);
	afx_msg void OnRenderviewHavokprimitives();
	afx_msg void OnUpdateRenderviewHavokprimitives(CCmdUI *pCmdUI);
	afx_msg void OnLightsScale();
	afx_msg void OnLightsSaveAll();
	afx_msg void OnFileMultiplayergame();
	afx_msg void OnFileClose();
	afx_msg LRESULT OnOutbarNotify(WPARAM wParam, LPARAM lParam);

	afx_msg void OnObjectsNewobject();
    afx_msg void OnItemsScale();
	afx_msg void OnItemsSaveall();
	afx_msg void OnAreasSaveall();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg UINT OnGetDlgCode() {return DLGC_WANTALLKEYS;};
	afx_msg void OnSoundsSaveall();
	afx_msg void OnTweakEdit();
	afx_msg void OnTweakApply();
	afx_msg LRESULT OnScriptMessage(WPARAM wParam, LPARAM lParam);
    afx_msg void OnPathfindingImportoldwaypoints();
	afx_msg void OnObjectsResetselected();
	afx_msg void OnObjectsSaveselected();
	LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
};


