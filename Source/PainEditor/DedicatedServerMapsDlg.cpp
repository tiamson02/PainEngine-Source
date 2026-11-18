// DedicatedServerMapsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerMapsDlg.h"

#include "FileSystem/GFileManager.h"
#include ".\dedicatedservermapsdlg.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

// CDedicatedServerMapsDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerMapsDlg, CDialog)
CDedicatedServerMapsDlg::CDedicatedServerMapsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerMapsDlg::IDD, pParent)
{
}

CDedicatedServerMapsDlg::~CDedicatedServerMapsDlg()
{
}

void CDedicatedServerMapsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LEVELLIST, m_lstLevels);
    DDX_Control(pDX, IDC_SELECTEDLEVELLIST, m_lstSelectedLevels);
    DDX_Control(pDX, IDC_GAMEMODE, m_cmbGameMode);
}


BEGIN_MESSAGE_MAP(CDedicatedServerMapsDlg, CDialog)
    ON_LBN_DBLCLK(IDC_LEVELLIST, OnLbnDblclkLevellist)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_LBN_DBLCLK(IDC_SELECTEDLEVELLIST, OnLbnDblclkSelectedlevellist)
    ON_BN_CLICKED(IDC_REMOVEMAP, OnBnClickedRemovemap)
    ON_CBN_SELCHANGE(IDC_GAMEMODE, OnCbnSelchangeGamemode)
END_MESSAGE_MAP()

BOOL CDedicatedServerMapsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    UpdateData();

#ifdef MPDEMO		
	m_cmbGameMode.AddString("Free For All");
	m_cmbGameMode.AddString("The Light Bearer");
#else
    m_cmbGameMode.AddString("Free For All");
    m_cmbGameMode.AddString("Team Deathmatch");
    m_cmbGameMode.AddString("Voosh");
    m_cmbGameMode.AddString("The Light Bearer");
    m_cmbGameMode.AddString("People Can Fly");
	m_cmbGameMode.AddString("Capture The Flag");
	m_cmbGameMode.AddString("Duel");
	m_cmbGameMode.AddString("Last Man Standing");
    m_cmbGameMode.SetCurSel(0);
#endif

    return TRUE;
}


// CDedicatedServerMapsDlg message handlers

void CDedicatedServerMapsDlg::OnLbnDblclkLevellist()
{
    OnBnClickedAdd();
}

void CDedicatedServerMapsDlg::OnBnClickedAdd()
{
    int i = m_lstLevels.GetCurSel();
    if (i>-1)
    {
        CString map;
        m_lstLevels.GetText(i,map);
        m_lstLevels.DeleteString(i);
        m_lstSelectedLevels.AddString(map);
    }
}

void CDedicatedServerMapsDlg::OnLbnDblclkSelectedlevellist()
{
    OnBnClickedRemovemap();
}

void CDedicatedServerMapsDlg::OnBnClickedRemovemap()
{
    int i = m_lstSelectedLevels.GetCurSel();
    if (i>-1)
    {
        CString map;
        m_lstSelectedLevels.GetText(i,map);
        m_lstSelectedLevels.DeleteString(i);
        m_lstLevels.AddString(map);
    }
}


extern ENGINE_API bool IsPKInstalled();


void CDedicatedServerMapsDlg::GetFromCfg(bool getmode)
{
    m_lstLevels.ResetContent();
    m_lstSelectedLevels.ResetContent();

    if (getmode)
    {
        ScriptObject GameMode = GScript.Globals()["Cfg"]["GameMode"];
        m_cmbGameMode.SelectString(-1,GameMode.GetString());
    }

    int m = m_cmbGameMode.GetCurSel();


    DynamicArray<String> files;
    if (m == 4)
        GFileMan.FindFiles("../Data/Levels/DMPCF_*.*",files,EFindDirs);
	else if( m == 5 )
		GFileMan.FindFiles("../Data/Levels/CTF_*.*",files,EFindDirs);
    else
        GFileMan.FindFiles("../Data/Levels/DM_*.*",files,EFindDirs);
    for (int i=0;i<files.Size();i++)
    {
        const char *lname = files[i].AsChar();

#ifndef STAND_ALONE_SERVER
		if( !IsPKInstalled() && 
			( !stricmp( lname, "DM_Sacred" ) || !stricmp( lname, "DM_Cursed" ) || !stricmp( lname, "DM_Illuminati" )
			 || !stricmp( lname, "DM_Unseen" ) || !stricmp( lname, "DM_Psycho" ) || !stricmp( lname, "DMPCF_Tower" )
			 || !stricmp( lname, "DMPCF_Warehouse" ) ) )
			 continue;
#endif

		if (lname[0] != '_')
            m_lstLevels.AddString(lname);
    }

    CString maps = "ServerMapsFFA";
#ifdef MPDEMO		
	if (m == 1) maps = "ServerMapsTLB";
#else
    switch (m)
    {
        case 1: maps = "ServerMapsTDM"; break;
        case 2: maps = "ServerMapsVSH"; break;
        case 3: maps = "ServerMapsTLB"; break;
        case 4: maps = "ServerMapsPCF"; break;
		case 5: maps = "ServerMapsCTF"; break;
		case 6: maps = "ServerMapsDUE"; break;
		case 7: maps = "ServerMapsLMS"; break;
    }
#endif

    ScriptObject CfgMaps = GScript.Globals()["Cfg"][(LPCSTR)maps];

    int cnt = CfgMaps.GetCount();
    for (int i = 1; i <= cnt; i++)
    {
        CString str = CfgMaps[i].GetString();
        int s = m_lstLevels.FindString(-1,str);
        if (s>-1)
        {
            m_lstLevels.DeleteString(s);
            m_lstSelectedLevels.AddString(str);
        }
    }

    UpdateData(false);
}

void CDedicatedServerMapsDlg::SaveToCfg()
{
    UpdateData(true);

    int m = m_cmbGameMode.GetCurSel();
    CString maps = "ServerMapsFFA";
#ifdef MPDEMO		
	if (m == 1) maps = "ServerMapsTLB";
#else
    switch (m)
    {
    case 1: maps = "ServerMapsTDM"; break;
    case 2: maps = "ServerMapsVSH"; break;
    case 3: maps = "ServerMapsTLB"; break;
    case 4: maps = "ServerMapsPCF"; break;
	case 5: maps = "ServerMapsCTF"; break;
	case 6: maps = "ServerMapsDUE"; break;
	case 7: maps = "ServerMapsLMS"; break;
    }
#endif

	CString str= "Cfg." + maps + "= {";
	CString str2 = "Cfg.ServerMaps = {";
    for (int i=0; i<m_lstSelectedLevels.GetCount(); i++)
    {
        CString map;
        m_lstSelectedLevels.GetText(i,map);
        str += "'" + map + "',";
		str2 += "'" + map + "',";
    }
    str += "}";
	str2 += "}";
    GScript.DoString(str);
	GScript.DoString(str2);

    int n = m_cmbGameMode.GetCurSel();
    if (n > -1)
    {
        CString str;
        m_cmbGameMode.GetLBText(n,str);
        GScript.DoString("Cfg.GameMode     = \"%s\"", (LPCSTR) str);
    }
}

void CDedicatedServerMapsDlg::OnCbnSelchangeGamemode()
{
    GetFromCfg(false);
}
