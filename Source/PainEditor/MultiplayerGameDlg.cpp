// MultiplayerGameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "MultiplayerGameDlg.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

// CMultiplayerGameDlg dialog

IMPLEMENT_DYNAMIC(CMultiplayerGameDlg, CDialog)
CMultiplayerGameDlg::CMultiplayerGameDlg(CWnd* pParent,bool servermode,CString hostname,int port)
	: CDialog(CMultiplayerGameDlg::IDD, pParent)
	, ServerMode(servermode)
	, HostName(hostname)
	, Port(port)	
	, PlayerName("No Name")
	, Speed(3)
	, FragsLimit(20)
	, Password(_T(""))
	, Map(_T(""))
    , CDKey(_T(""))
    , IsPublic(FALSE)
{
}

CMultiplayerGameDlg::~CMultiplayerGameDlg()
{
}

void CMultiplayerGameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_HOSTNAME, HostName);
    DDX_Text(pDX, IDC_PORT, Port);
    DDX_Text(pDX, IDC_PLAYERNAME, PlayerName);
    DDX_CBIndex(pDX, IDC_SPEED, Speed);
    DDX_Text(pDX, IDC_FRAGSLIMIT, FragsLimit);
    DDX_Control(pDX, IDC_MAPS, m_cmbMaps);
    DDX_Text(pDX, IDC_PASSWORD, Password);
    DDX_Text(pDX, IDC_CDKEY, CDKey);
    DDX_Check(pDX, IDC_PUBLIC, IsPublic);
}


BEGIN_MESSAGE_MAP(CMultiplayerGameDlg, CDialog)
	ON_BN_CLICKED(IDC_CLIENTMODE, OnBnClickedClientmode)
	ON_BN_CLICKED(IDC_SERVERMODE, OnBnClickedServermode)
END_MESSAGE_MAP()


BOOL CMultiplayerGameDlg::OnInitDialog()
{
	HostName = GEngine->Registry.LastHostName.AsChar();
	PlayerName = GEngine->Registry.PlayerName.AsChar();
	Password = GEngine->Registry.LastPassword.AsChar();
	Port = GEngine->Registry.LastPort;
	FragsLimit = GEngine->Registry.FragsLimit;
	CDKey = GEngine->Registry.CDKey.AsChar();
	IsPublic = GEngine->Registry.IsPublic;
	ServerMode = GEngine->Registry.ServerMode;

    CheckRadioButton(IDC_SERVERMODE, IDC_CLIENTMODE,ServerMode?IDC_SERVERMODE:IDC_CLIENTMODE);  
    SwitchMode(ServerMode);	

	UpdateData(false);
	FileManagerStd fm;
	DynamicArray<String> files;
	fm.FindFiles("../Data/Levels/*.*",files,EFindDirs);

	m_cmbMaps.ResetContent();
	for (int i=0;i<files.Size();i++)
		m_cmbMaps.AddString(files[i].AsChar());	
	
	CString map = GEngine->Registry.Map.AsChar();
	int s = m_cmbMaps.FindString(-1,map);
	m_cmbMaps.SetCurSel(max(s,0));
	
	return CDialog::OnInitDialog();
}

void CMultiplayerGameDlg::SwitchMode(BOOL server)
{
	ServerMode = server;
	if (ServerMode) 
	{
		GetDlgItem(IDC_HOSTNAME)->EnableWindow(false);
		GetDlgItem(IDC_MAPS)->EnableWindow(true);
		GetDlgItem(IDC_FRAGSLIMIT)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_HOSTNAME)->EnableWindow(!false);
		GetDlgItem(IDC_MAPS)->EnableWindow(!true);
		GetDlgItem(IDC_FRAGSLIMIT)->EnableWindow(!true);
	}

}

// CMultiplayerGameDlg message handlers

void CMultiplayerGameDlg::OnBnClickedClientmode()
{
	SwitchMode(false);
}

void CMultiplayerGameDlg::OnBnClickedServermode()
{
	SwitchMode(true);
}

void CMultiplayerGameDlg::OnOK()
{
	UpdateData();

	GEngine->Registry.LastHostName = LPCSTR(HostName);
	GEngine->Registry.PlayerName = LPCSTR(PlayerName);
	GEngine->Registry.LastPassword = LPCSTR(Password);
	GEngine->Registry.CDKey = LPCSTR(CDKey);
	GEngine->Registry.LastPort = Port;
	GEngine->Registry.FragsLimit = FragsLimit;
	GEngine->Registry.IsPublic = (IsPublic!=0);
	GEngine->Registry.ServerMode = (ServerMode!=0);

	Map = "";
	int s = m_cmbMaps.GetCurSel();
	if (s>-1)
	{
		m_cmbMaps.GetLBText(s,Map);
		GEngine->Registry.Map = LPCSTR(Map);
	}

	CDialog::OnOK();
}

