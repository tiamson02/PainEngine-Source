// DedicatedServerUserCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerUserCfgDlg.h"

#include "Scripting/Scripting.h"
#include ".\dedicatedserverusercfgdlg.h"
extern ENGINE_API Script	GScript;


// CDedicatedServerUserCfgDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerUserCfgDlg, CDialog)
CDedicatedServerUserCfgDlg::CDedicatedServerUserCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerUserCfgDlg::IDD, pParent)
{
	mMap = FALSE;
	mReloadMap = FALSE;
	mGameMode = FALSE;
	mKick = FALSE;
	mBankick = FALSE;
	mMaxPlayers = FALSE;
	mMaxSpectators = FALSE;
	mTimeLimit = FALSE;
	mFragLimit = FALSE;
	mCaptureLimit = FALSE;
	mLMSLivesLimit = FALSE;
	mPowerups = FALSE;
	mPowerupDrop = FALSE;
	mWeaponsStay = FALSE;
	mWeaponRespawnTime = FALSE;
	mTeamDamage = FALSE;
	mAllowBunnyhopping = FALSE;
	mAllowBrightskins = FALSE;
	mAllowForwardRJ = FALSE;
	mStartupWeapon = FALSE;
}

CDedicatedServerUserCfgDlg::~CDedicatedServerUserCfgDlg()
{
}

void CDedicatedServerUserCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MAP, mMap);
	DDX_Check(pDX, IDC_RELOADMAP, mReloadMap);
	DDX_Check(pDX, IDC_GAMEMODE, mGameMode);
	DDX_Check(pDX, IDC_KICK, mKick);
	DDX_Check(pDX, IDC_BANKICK, mBankick);
	DDX_Check(pDX, IDC_MAXPLAYERS, mMaxPlayers);
	DDX_Check(pDX, IDC_MAXSPECTATORS, mMaxSpectators);
	DDX_Check(pDX, IDC_USRTIMELIMIT, mTimeLimit);
	DDX_Check(pDX, IDC_USRFRAGLIMIT, mFragLimit);
	DDX_Check(pDX, IDC_USRCAPTURELIMIT, mCaptureLimit);
	DDX_Check(pDX, IDC_USRLMSLIVESLIMIT, mLMSLivesLimit);
	DDX_Check(pDX, IDC_POWERUPS, mPowerups);
	DDX_Check(pDX, IDC_POWERUPDROP, mPowerupDrop);
	DDX_Check(pDX, IDC_USRWEAPONSSTAY, mWeaponsStay);
	DDX_Check(pDX, IDC_USRWEAPONRESPAWNTIME, mWeaponRespawnTime);
	DDX_Check(pDX, IDC_TEAMDAMAGE, mTeamDamage);
	DDX_Check(pDX, IDC_USRALLOWBUNNYHOPPING, mAllowBunnyhopping);
	DDX_Check(pDX, IDC_USRALLOWBRIGHTSKINS, mAllowBrightskins);
	DDX_Check(pDX, IDC_USRALLOWFORWARDRJ, mAllowForwardRJ);
	DDX_Check(pDX, IDC_USRSTARTUPWEAPON, mStartupWeapon);
}


BEGIN_MESSAGE_MAP(CDedicatedServerUserCfgDlg, CDialog)
END_MESSAGE_MAP()


// CDedicatedServerUserCfgDlg message handlers

void CDedicatedServerUserCfgDlg::SaveToCfg()
{
	UpdateData(true);
    GScript.DoString("Cfg.UserMapChange         = %s"  , mMap?"true":"false");
	GScript.DoString("Cfg.UserReloadMap         = %s"  , mReloadMap?"true":"false");
	GScript.DoString("Cfg.UserGameMode          = %s"  , mGameMode?"true":"false");
	GScript.DoString("Cfg.UserKick              = %s"  , mKick?"true":"false");
	GScript.DoString("Cfg.UserBankick           = %s"  , mBankick?"true":"false");
	GScript.DoString("Cfg.UserMaxPlayers        = %s"  , mMaxPlayers?"true":"false");
	GScript.DoString("Cfg.UserMaxSpectators     = %s"  , mMaxSpectators?"true":"false");
	GScript.DoString("Cfg.UserTimeLimit         = %s"  , mTimeLimit?"true":"false");
	GScript.DoString("Cfg.UserFragLimit         = %s"  , mFragLimit?"true":"false");
	GScript.DoString("Cfg.UserCaptureLimit     = %s"   , mCaptureLimit?"true":"false");
	GScript.DoString("Cfg.UserLMSLives         = %s"   , mLMSLivesLimit?"true":"false");	
	GScript.DoString("Cfg.UserPowerups          = %s"  , mPowerups?"true":"false");
	GScript.DoString("Cfg.UserPowerupDrop       = %s"  , mPowerupDrop?"true":"false");
	GScript.DoString("Cfg.UserWeaponsStay       = %s"  , mWeaponsStay?"true":"false");
	GScript.DoString("Cfg.UserWeaponRespawnTime = %s"  , mWeaponRespawnTime?"true":"false");
	GScript.DoString("Cfg.UserTeamDamage        = %s"  , mTeamDamage?"true":"false");
	GScript.DoString("Cfg.UserAllowBunnyhopping = %s"  , mAllowBunnyhopping?"true":"false");
	GScript.DoString("Cfg.UserAllowBrightskins  = %s"  , mAllowBrightskins?"true":"false");
	GScript.DoString("Cfg.UserAllowForwardRJ    = %s"  , mAllowForwardRJ?"true":"false");
	GScript.DoString("Cfg.UserStartupWeapon     = %s"  , mStartupWeapon?"true":"false");
}


void CDedicatedServerUserCfgDlg::GetFromCfg()
{
    ScriptObject Cfg = GScript.Globals()["Cfg"];
	mMap = Cfg["UserMapChange"].GetBool();
	mReloadMap = Cfg["UserReloadMap"].GetBool();
	mGameMode = Cfg["UserGameMode"].GetBool();
	mKick = Cfg["UserKick"].GetBool();
	mBankick = Cfg["UserBankick"].GetBool();
	mMaxPlayers = Cfg["UserMaxPlayers"].GetBool();
	mMaxSpectators = Cfg["UserMaxSpectators"].GetBool();
	mTimeLimit = Cfg["UserTimeLimit"].GetBool();
	mFragLimit = Cfg["UserFragLimit"].GetBool();
	mCaptureLimit = Cfg["UserCaptureLimit"].GetBool();
	mLMSLivesLimit = Cfg["UserLMSLives"].GetBool();
	mPowerups = Cfg["UserPowerups"].GetBool();
	mPowerupDrop = Cfg["UserPowerupDrop"].GetBool();
	mWeaponsStay = Cfg["UserWeaponsStay"].GetBool();
	mWeaponRespawnTime = Cfg["UserWeaponRespawnTime"].GetBool();
	mTeamDamage = Cfg["UserTeamDamage"].GetBool();
	mAllowBunnyhopping = Cfg["UserAllowBunnyhopping"].GetBool();
	mAllowBrightskins = Cfg["UserAllowBrightskins"].GetBool();
	mAllowForwardRJ = Cfg["UserAllowForwardRJ"].GetBool();
	mStartupWeapon = Cfg["UserStartupWeapon"].GetBool();
    UpdateData(false);
}
