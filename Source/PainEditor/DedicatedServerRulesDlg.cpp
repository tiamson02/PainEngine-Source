// DedicatedServerRulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerRulesDlg.h"

#include "Scripting/Scripting.h"
#include ".\dedicatedserverrulesdlg.h"
extern ENGINE_API Script	GScript;

// CDedicatedServerRulesDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerRulesDlg, CDialog)
CDedicatedServerRulesDlg::CDedicatedServerRulesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerRulesDlg::IDD, pParent)
    , m_FragLimit(0)
    , m_TimeLimit(0)
	, m_CaptureLimit(0)
	, m_LMSLivesLimit(0)
    , m_WeaponsStay(FALSE)
    , m_AllowBunnyhopping(FALSE)
    , m_AllowBrightSkin(FALSE)
    , m_AllowForwardRJ(FALSE)
    , m_PowerupDrop(FALSE)
    , m_Powerups(FALSE)
    , m_TeamDamage(FALSE)
	, m_ConsoleLock(FALSE)
	, m_WeaponRespawnTime(0)
{
}

CDedicatedServerRulesDlg::~CDedicatedServerRulesDlg()
{
}

void CDedicatedServerRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FRAGLIMIT, m_FragLimit);
	DDX_Text(pDX, IDC_TIMELIMIT, m_TimeLimit);
	DDX_Text(pDX, IDC_CAPTURELIMIT, m_CaptureLimit);
	DDX_Text(pDX, IDC_LMSLIVESLIMIT, m_LMSLivesLimit);
	DDX_Check(pDX, IDC_WEAPONSSTAY, m_WeaponsStay);
	DDX_Check(pDX, IDC_ALLOWBUNNYHOPPING, m_AllowBunnyhopping);
	DDX_Check(pDX, IDC_ALLOWFORWARDROCKETJUMP, m_AllowForwardRJ);
	DDX_Check(pDX, IDC_ALLOWBRIGHTSKIN, m_AllowBrightSkin);
	DDX_Check(pDX, IDC_POWERDROP, m_PowerupDrop);
	DDX_Check(pDX, IDC_POWERUPS, m_Powerups);
	DDX_Check(pDX, IDC_TEAMDAMAGE, m_TeamDamage);
	DDX_Check(pDX, IDC_CONSOLELOCK, m_ConsoleLock);
	DDX_Text(pDX, IDC_WEAPONRESPAWNTIME, m_WeaponRespawnTime);
	DDV_MinMaxUInt(pDX, m_WeaponRespawnTime, 0, 600);
}


BEGIN_MESSAGE_MAP(CDedicatedServerRulesDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_FRAGLIMITSPIN, OnDeltaposFraglimitspin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_TIMELIMITSPIN, OnDeltaposTimelimitspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CAPTURELIMITSPIN, OnDeltaposCapturelimitspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LMSLIVESLIMITSPIN, OnDeltaposLMSLiveslimitspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WEAPONRESPAWNTIMESPIN, OnDeltaposWeaponrespawntimespin)
END_MESSAGE_MAP()


// CDedicatedServerRulesDlg message handlers

void CDedicatedServerRulesDlg::GetFromCfg()
{
    ScriptObject Cfg = GScript.Globals()["Cfg"];
    m_FragLimit = Cfg["FragLimit"].GetInt();
    m_TimeLimit = Cfg["TimeLimit"].GetInt();
	m_CaptureLimit = Cfg["CaptureLimit"].GetInt();
	m_LMSLivesLimit = Cfg["LMSLives"].GetInt();
    m_WeaponsStay = Cfg["WeaponsStay"].GetBool();
    m_WeaponRespawnTime = Cfg["WeaponRespawnTime"].GetInt();	
    m_AllowBunnyhopping = Cfg["AllowBunnyhopping"].GetBool();
    m_AllowForwardRJ = Cfg["AllowForwardRJ"].GetBool();
    m_AllowBrightSkin =Cfg["AllowBrightskins"].GetBool();
    m_PowerupDrop = Cfg["PowerupDrop"].GetBool();
    m_Powerups = Cfg["Powerups"].GetBool();
    m_TeamDamage = Cfg["TeamDamage"].GetBool();
	m_ConsoleLock = Cfg["ClientConsoleLockdown"].GetBool();
    UpdateData(false);
}

void CDedicatedServerRulesDlg::SaveToCfg()
{
    UpdateData(true);
    GScript.DoString("Cfg.FragLimit         = %d"  , m_FragLimit);
    GScript.DoString("Cfg.TimeLimit         = %d"  , m_TimeLimit);    
	GScript.DoString("Cfg.CaptureLimit      = %d"  , m_CaptureLimit);
	GScript.DoString("Cfg.LMSLives          = %d"  , m_LMSLivesLimit);    
    GScript.DoString("Cfg.WeaponsStay       = %s"  , m_WeaponsStay?"true":"false");
    GScript.DoString("Cfg.WeaponRespawnTime = %d"  , m_WeaponRespawnTime);    
    GScript.DoString("Cfg.AllowBunnyhopping = %s"  , m_AllowBunnyhopping?"true":"false");
    GScript.DoString("Cfg.AllowForwardRJ    = %s"  , m_AllowForwardRJ?"true":"false");
    GScript.DoString("Cfg.AllowBrightskins  = %s"  , m_AllowBrightSkin?"true":"false");
    GScript.DoString("Cfg.PowerupDrop       = %s"  , m_PowerupDrop?"true":"false");
    GScript.DoString("Cfg.Powerups          = %s"  , m_Powerups?"true":"false");
    GScript.DoString("Cfg.TeamDamage        = %s"  , m_TeamDamage?"true":"false");
	GScript.DoString("Cfg.ClientConsoleLockdown = %s"  , m_ConsoleLock?"true":"false");
}

void CDedicatedServerRulesDlg::OnDeltaposFraglimitspin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    int l = m_FragLimit - pNMUpDown->iDelta;
    if (l<0) l = 0;
    m_FragLimit = l;
    UpdateData(false);
    *pResult = 0;
}

void CDedicatedServerRulesDlg::OnDeltaposTimelimitspin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    int l = m_TimeLimit - pNMUpDown->iDelta;
    if (l<0) l = 0;
    m_TimeLimit = l;
    UpdateData(false);
    *pResult = 0;
}

void CDedicatedServerRulesDlg::OnDeltaposCapturelimitspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int l = m_CaptureLimit - pNMUpDown->iDelta;
	if (l<0) l = 0;
	m_CaptureLimit = l;
	UpdateData(false);
	*pResult = 0;
}

void CDedicatedServerRulesDlg::OnDeltaposLMSLiveslimitspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int l = m_LMSLivesLimit - pNMUpDown->iDelta;
	if (l<0) l = 0;
	m_LMSLivesLimit = l;
	UpdateData(false);
	*pResult = 0;
}

void CDedicatedServerRulesDlg::OnDeltaposWeaponrespawntimespin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    int l = m_WeaponRespawnTime - pNMUpDown->iDelta;
    if (l<0) l = 0;
    m_WeaponRespawnTime = l;
    UpdateData(false);
	*pResult = 0;
}
