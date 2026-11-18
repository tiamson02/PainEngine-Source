// DedicatedServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerDlg.h"
#include "ProfilerDlg.h"
#include "OutputDlg.h"
#include "Game/Main.h"
#include "Engine.h"
#include "Engine/PCFSystem.h"
#include "SetValueDlg.h"

extern ENGINE_API PCFSystem* GEngine;
#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
extern ENGINE_API GFileManager  	GFileManAudio;


#include "Scripting/Scripting.h"
#include ".\dedicatedserverdlg.h"
extern ENGINE_API Script	GScript;

// CDedicatedServerDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerDlg, CDialog)
CDedicatedServerDlg::CDedicatedServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerDlg::IDD, pParent)
{
}

CDedicatedServerDlg::~CDedicatedServerDlg()
{
}

void CDedicatedServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB, m_tabCtrl);
}

CProfilerDlg dlgProfiler;
COutputDlg dlgOutput;
BOOL CDedicatedServerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    CString ver = GScript.Globals()["PK_VERSION"].GetString();

    SetWindowText("Painkiller Dedicated Server ver. " + ver);
    UpdateWindow();

	#ifndef FINAL_BUILD

    dlgProfiler.Create(IDD_PROFILER,this);
    dlgProfiler.SetTimer(1,250,NULL);
    dlgProfiler.ShowWindow(SW_SHOW);

	
    dlgOutput.Create(IDD_OUTPUT,this);
    dlgOutput.ShowWindow(SW_SHOW);	
	#endif

//#ifdef EDITOR
    m_tabCtrl.SendInitialUpdate(TRUE);
    m_tabCtrl.SetPadding(CSize(6, 3));
    // Set the tab to auto condense (shrink) its tabs when sized.
    m_tabCtrl.SetAutoCondense(true);
//#endif
    // Clip children of the tab control from paint routines to reduce flicker.
    m_tabCtrl.ModifyStyle(0L,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

    m_dlgGeneral.Create(IDD_DEDICATEDSERVER_GENERAL ,this);
    m_dlgMaps.Create(IDD_DEDICATEDSERVER_MAPS ,this);
    m_dlgRules.Create(IDD_DEDICATEDSERVER_RULES ,this);
	m_dlgUserCfg.Create(IDD_DEDICATEDSERVER_USERCFG, this);
	m_dlgOutput.Create(IDD_DEDICATEDSERVER_OUTPUT, this);

//#ifdef EDITOR
    if (!m_tabCtrl.AddControl (_T("Server"), &m_dlgGeneral, 0, 0))
    {
        TRACE0("Failed to create tab.\n");
        return -1;
    }
    if (!m_tabCtrl.AddControl (_T("Mode && Maps"), &m_dlgMaps, 1, 1))
    {
        TRACE0("Failed to create tab.\n");
        return -1;
    }
    if (!m_tabCtrl.AddControl (_T("Rules"), &m_dlgRules, 2, 2))
    {
        TRACE0("Failed to create tab.\n");
        return -1;
    }
	if (!m_tabCtrl.AddControl (_T("Voting"), &m_dlgUserCfg, 3, 3))
    {
        TRACE0("Failed to create tab.\n");
        return -1;
    }
	if (!m_tabCtrl.AddControl (_T("Console"), &m_dlgOutput, 4, 4))
	{
		TRACE0("Failed to create tab.\n");
		return -1;
	}
//#endif
    m_dlgGeneral.GetFromCfg();
    m_dlgRules.GetFromCfg();
    m_dlgMaps.GetFromCfg();
	m_dlgUserCfg.GetFromCfg();

//#ifdef EDITOR
    m_tabCtrl.SetActiveView(0);    
    m_tabCtrl.UpdateWindow();
//#endif

	mRunning = false;

    return true;
}


BEGIN_MESSAGE_MAP(CDedicatedServerDlg, CDialog)
    ON_BN_CLICKED(ID_START, OnBnClickedStart)
    ON_WM_MOVING()
    ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_LUA, OnBnClickedLua)
END_MESSAGE_MAP()

void CDedicatedServerDlg::OnCancel()
{
	m_dlgGeneral.SaveToCfg();
	m_dlgRules.SaveToCfg();
	m_dlgMaps.SaveToCfg();
	m_dlgUserCfg.SaveToCfg();
	GScript.DoString("Cfg:Save()");
    CDialog::OnCancel();
    AfxPostQuitMessage(0); 
}

void ExportToLua();


// CDedicatedServerDlg message handlers

void CDedicatedServerDlg::OnBnClickedStart()
{
    if (!m_dlgGeneral.UpdateData()) return;
    if (!m_dlgRules.UpdateData()) return;

	if( !mRunning )
	{
		if (m_dlgMaps.m_lstSelectedLevels.GetCount() == 0)
		{
			MessageBox("Please select a map!");
			return;
		}

		m_dlgGeneral.SaveToCfg();
		m_dlgRules.SaveToCfg();
		m_dlgMaps.SaveToCfg();
		m_dlgUserCfg.SaveToCfg();
		GScript.DoString("Cfg:Save()");

		CString map;
		m_dlgMaps.m_lstSelectedLevels.GetText(0,map);

		// savujemy parametry do cfg.ini

		CString cmd;
		cmd.Format("+dedicatedserver +map %s +port %d",(LPCSTR) map ,m_dlgGeneral.m_Port);
		if (m_dlgGeneral.GetCheckedRadioButton(IDC_PUBLICSRV,IDC_LOCALSRV) == IDC_LOCALSRV) 
			cmd += " +private";
		
		CString ip;
		m_dlgGeneral.m_cmbNetInterface.GetLBText(m_dlgGeneral.m_cmbNetInterface.GetCurSel(),ip);
		if (ip.GetLength()>5)
			cmd += " +interface " + ip;

		GScript.Release();
		GScript.Init();
		ExportToLua();

		Game_Init(AfxGetInstanceHandle(),cmd);
//		GetDlgItem(ID_START)->EnableWindow(false);
		GetDlgItem(ID_START)->SetWindowText( "Stop" );

		GScript.DoString("Game_BackupServerSettings()");

		// przekazuje do lua liste map
		CString maps = "PainMenu.mapsOnServer = {";
		for (int i=0; i<m_dlgMaps.m_lstSelectedLevels.GetCount(); i++)
		{
			m_dlgMaps.m_lstSelectedLevels.GetText(i,map);
			maps += "'"+ map + "',";
		}             
		maps += "}";

		GScript.DoString(maps);    

		mRunning = true;
	}
	else
	{
		GScript.DoString("Game:Close()");
		delete GEngine;
		GEngine = NULL;

		mRunning = false;
		GetDlgItem(ID_START)->SetWindowText( "Start" );
	}
}

void CDedicatedServerDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
    CDialog::OnMoving(fwSide, pRect);
    UpdateDialogsPosition(pRect);
}

void CDedicatedServerDlg::UpdateDialogsPosition(RECT *pRect)
{
	if (!dlgProfiler.m_hWnd) return;

    RECT rc;    
    dlgOutput.GetWindowRect(&rc);
    dlgOutput.MoveWindow(pRect->left,pRect->bottom,rc.right-rc.left,rc.bottom-rc.top,true);
    dlgProfiler.GetWindowRect(&rc);
    int w = rc.right-rc.left;
    dlgProfiler.MoveWindow(pRect->left-w,pRect->top,w,rc.bottom-rc.top,true);
}

void CDedicatedServerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    RECT rc;
    GetWindowRect(&rc);
    UpdateDialogsPosition(&rc);
}

void CDedicatedServerDlg::OnBnClickedLua()
{
	static CSetValueDlg dlg(NULL,"LUA Script Command:");
	if(dlg.DoModal() == IDOK )
	{
		GEngine->ResetTimer();
		GScript.DoString(dlg.m_Value);
		GEngine->ResetTimer();
	}
}
