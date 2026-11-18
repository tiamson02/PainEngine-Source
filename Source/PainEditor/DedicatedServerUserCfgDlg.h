#pragma once


// CDedicatedServerUserCfgDlg dialog

class CDedicatedServerUserCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerUserCfgDlg)

public:
	void GetFromCfg();
    void SaveToCfg();

	CDedicatedServerUserCfgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerUserCfgDlg();

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER_USERCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel() {};
	virtual void OnOK() {};

	DECLARE_MESSAGE_MAP()

public:

	BOOL mMap;
	BOOL mReloadMap;
	BOOL mGameMode;
	BOOL mKick;
	BOOL mBankick;
	BOOL mMaxPlayers;
	BOOL mMaxSpectators;
	BOOL mTimeLimit;
	BOOL mFragLimit;
	BOOL mCaptureLimit;
	BOOL mLMSLivesLimit;
	BOOL mPowerups;
	BOOL mPowerupDrop;
	BOOL mWeaponsStay;
	BOOL mWeaponRespawnTime;
	BOOL mTeamDamage;
	BOOL mAllowBunnyhopping;
	BOOL mAllowBrightskins;
	BOOL mAllowForwardRJ;
	BOOL mStartupWeapon;
};
