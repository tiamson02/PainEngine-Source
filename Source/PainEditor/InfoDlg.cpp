// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "InfoDlg.h"
#include ".\Game\Main.h"
#include ".\infodlg.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Engine/InputSystem.h"
#include "Render/Video.h"

// CInfoDlg dialog

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)
CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
    , m_Text(_T(""))
{
}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TEXT, m_Text);
    DDX_Control(pDX, IDC_RESOLUTION, m_cmbResolution);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_RESOLUTION, OnCbnSelchangeResolution)
    ON_BN_CLICKED(IDC_INVERSEMOUSE, OnBnClickedInversemouse)
    ON_BN_CLICKED(IDC_SHADOWS, OnBnClickedShadows)
END_MESSAGE_MAP()


BOOL CInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_cmbResolution.AddString("640x480");
    m_cmbResolution.AddString("800x600");
    m_cmbResolution.AddString("1024x768");
    m_cmbResolution.AddString("1280x1024");
    m_cmbResolution.AddString("1600x1200");

    CString res = GScript.Globals()["Cfg"]["Resolution"].GetString();
    res.Replace('X','x');
    m_cmbResolution.SetCurSel(m_cmbResolution.FindString(-1,res));

    GEngine->Video->ShadowsEnabled = 1;
    return true;
}

// CInfoDlg message handlers

void CInfoDlg::OnCbnSelchangeResolution()
{
    int i = m_cmbResolution.GetCurSel();	
    if (i>-1)
    {
        CString res;
        m_cmbResolution.GetLBText(i,res);
        res.Replace('x','X');
        GScript.DoString("Cfg.Resolution = '%s'",res);
//        GScript.DoString("MENU.SetText('VideoMenuResolutionButton','Resolution: %s')",res);
    }
}

void CInfoDlg::OnBnClickedInversemouse()
{
    GEngine->InputSys->InvertMouse = !GEngine->InputSys->InvertMouse;    
}

void CInfoDlg::OnBnClickedShadows()
{
    GEngine->Video->ShadowsEnabled = !GEngine->Video->ShadowsEnabled;
}
