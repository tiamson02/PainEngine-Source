// DedicatedServerGeneralDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerGeneralDlg.h"
#include ".\dedicatedservergeneraldlg.h"
#include "Engine.h"
#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "net/ipdriver.h"

// CDedicatedServerGeneralDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerGeneralDlg, CDialog)
CDedicatedServerGeneralDlg::CDedicatedServerGeneralDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerGeneralDlg::IDD, pParent)
    , m_ServerName(_T(""))
    , m_ServerPassword(_T(""))
    , m_Port(0)
    , m_MaxSpectators(0)
    , m_MaxPlayers(1)
    , m_FPS(0)
{
}

CDedicatedServerGeneralDlg::~CDedicatedServerGeneralDlg()
{
}

void CDedicatedServerGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVERNAME, m_ServerName);
	DDV_MaxChars(pDX, m_ServerName, 15);
	DDX_Text(pDX, IDC_SERVERPASSWORD, m_ServerPassword);
	DDV_MaxChars(pDX, m_ServerPassword, 15);
	DDX_Text(pDX, IDC_PORT, m_Port);
	DDX_Text(pDX, IDC_MAXSPECTATORS, m_MaxSpectators);
	DDV_MinMaxByte(pDX, m_MaxSpectators, 0, 8);
	DDX_Text(pDX, IDC_MAXPLAYERS, m_MaxPlayers);
	DDV_MinMaxByte(pDX, m_MaxPlayers, 1, 16);
	DDX_Text(pDX, IDC_FPS, m_FPS);
	DDV_MinMaxUInt(pDX, m_FPS, 30, 120);
	DDX_Control(pDX, IDC_NETINTERFACE, m_cmbNetInterface);
}


BEGIN_MESSAGE_MAP(CDedicatedServerGeneralDlg, CDialog)
    ON_BN_CLICKED(IDC_DEFAULTPORT, OnBnClickedDefaultport)
    ON_NOTIFY(UDN_DELTAPOS, IDC_MAXPLAYERSSPIN, OnDeltaposMaxplayersspin)
//    ON_BN_CLICKED(IDC_NOLIMITFPS, OnBnClickedNolimitfps)
    ON_NOTIFY(UDN_DELTAPOS, IDC_FPSSPIN, OnDeltaposFpsspin)
    ON_BN_CLICKED(IDC_R2_LIMIT, OnBnClickedR2Limit)
    ON_BN_CLICKED(IDC_R1_NOLIMIT, OnBnClickedR1Nolimit)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin4)
END_MESSAGE_MAP()


DynamicArray<String>	NetInterfaces;
bool FillNetInterfaces()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1,1);
	if( WSAStartup(wVersionRequested, &wsaData ) ) return false;
	if( ( LOBYTE(wsaData.wVersion) != 1 ) && ( HIBYTE(wsaData.wVersion) != 1 ) ) { WSACleanup(); return false; };

	in_addr	hostAddr;
	hostAddr.S_un.S_addr = INADDR_ANY;
	char hostnam[256];
	if( gethostname( hostnam, 255 ) ) { WSACleanup(); return false; };	// this is enough to popup one-style of ZoneAlarm window

	hostent* host = gethostbyname( hostnam );
	if( !host ) { WSACleanup(); return false; };

	in_addr** ia = (in_addr**)host->h_addr_list;
	while( *ia )
	{
		in_addr* ia2 = *ia;
		String newStr = String::Sprintf( "%d.%d.%d.%d", ia2->S_un.S_un_b.s_b1, ia2->S_un.S_un_b.s_b2, ia2->S_un.S_un_b.s_b3, ia2->S_un.S_un_b.s_b4 );
		NetInterfaces.AddUnique( newStr );
		ia++;
	}

	WSACleanup();
	return true;
}


extern ENGINE_API bool IsPKInstalled();

void CDedicatedServerGeneralDlg::GetFromCfg()
{
    ScriptObject Cfg = GScript.Globals()["Cfg"];
    m_ServerName = Cfg["ServerName"].GetString();
    m_ServerPassword = Cfg["ServerPassword"].GetString();
    m_MaxPlayers = Cfg["MaxPlayers"].GetInt();
    m_MaxSpectators = Cfg["MaxSpectators"].GetInt();
    m_Port = Cfg["ServerPort"].GetInt();
    if (m_Port == DEFAULT_SERVER_PORT)
    {
        GetDlgItem(IDC_PORT)->EnableWindow(false);
        CheckDlgButton(IDC_DEFAULTPORT,BST_CHECKED);
    }

    bool publicServer = Cfg["PublicServer"].GetBool();
    CheckRadioButton(IDC_PUBLICSRV, IDC_LOCALSRV, publicServer?IDC_PUBLICSRV:IDC_LOCALSRV);
#ifndef STAND_ALONE_SERVER
	if( !IsPKInstalled() )
		GetDlgItem(IDC_PUBLICSRV)->EnableWindow(false);
#endif

    m_FPS = Cfg["ServerFPS"].GetInt();
    bool limit = Cfg["LimitServerFPS"].GetBool();
    CheckRadioButton(IDC_R1_NOLIMIT, IDC_R2_LIMIT, limit?IDC_R2_LIMIT:IDC_R1_NOLIMIT);    
    limit?OnBnClickedR2Limit():OnBnClickedR1Nolimit();

	UpdateData(false);

	FillNetInterfaces();
	for (int i=0;i<NetInterfaces.Size();i++)
		m_cmbNetInterface.AddString(NetInterfaces[i].AsChar());
	
	m_cmbNetInterface.SetCurSel(0);
}

void CDedicatedServerGeneralDlg::SaveToCfg()
{
    UpdateData(true);
    GScript.DoString("Cfg.ServerName     = \"%s\"", (LPCSTR) m_ServerName);
    GScript.DoString("Cfg.ServerPassword = \"%s\"", (LPCSTR) m_ServerPassword);
    GScript.DoString("Cfg.MaxPlayers     = %d"  , m_MaxPlayers);
    GScript.DoString("Cfg.MaxSpectators  = %d"  , m_MaxSpectators);
    GScript.DoString("Cfg.ServerPort     = %d"  , m_Port);    
    if (GetCheckedRadioButton(IDC_PUBLICSRV,IDC_LOCALSRV) == IDC_LOCALSRV) 
        GScript.DoString("Cfg.PublicServer = false");
    else
        GScript.DoString("Cfg.PublicServer = true");


    GScript.DoString("Cfg.ServerFPS     = %d"  , m_FPS);
    if (GetCheckedRadioButton(IDC_R1_NOLIMIT,IDC_R2_LIMIT) == IDC_R1_NOLIMIT)
	{
        GScript.DoString("Cfg.LimitServerFPS = false");
		GScript.DoString("Cfg.ServerFPS     = 45");
	}
    else
        GScript.DoString("Cfg.LimitServerFPS = true");
}

// CDedicatedServerGeneralDlg message handlers

void CDedicatedServerGeneralDlg::OnBnClickedDefaultport()
{
    if (IsDlgButtonChecked(IDC_DEFAULTPORT) == BST_CHECKED)
    {
        GetDlgItem(IDC_PORT)->EnableWindow(false);        
        m_Port = DEFAULT_SERVER_PORT;
    }
    else
        GetDlgItem(IDC_PORT)->EnableWindow(true);
    UpdateData(false);
}

void CDedicatedServerGeneralDlg::OnDeltaposMaxplayersspin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    m_MaxPlayers -= pNMUpDown->iDelta;
    if (m_MaxPlayers <1) m_MaxPlayers = 1;
    if (m_MaxPlayers >16) m_MaxPlayers = 16;
    UpdateData(false);
    *pResult = 0;
}


void CDedicatedServerGeneralDlg::OnDeltaposFpsspin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    m_FPS -= pNMUpDown->iDelta;
    if (m_FPS <30) m_FPS = 30;
    if (m_FPS >120) m_FPS = 120;
    UpdateData(false);
    *pResult = 0;
}

void CDedicatedServerGeneralDlg::OnBnClickedR2Limit()
{
    GetDlgItem(IDC_FPS)->EnableWindow(true);
    GetDlgItem(IDC_FPSSPIN)->EnableWindow(true);
}

void CDedicatedServerGeneralDlg::OnBnClickedR1Nolimit()
{
    GetDlgItem(IDC_FPS)->EnableWindow(false);        
    GetDlgItem(IDC_FPSSPIN)->EnableWindow(false);
}

void CDedicatedServerGeneralDlg::OnDeltaposSpin4(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    m_MaxSpectators -= pNMUpDown->iDelta;
    if (m_MaxSpectators <1) m_MaxSpectators = 1;
    if (m_MaxSpectators >8) m_MaxSpectators = 8;
    UpdateData(false);
    *pResult = 0;
}
