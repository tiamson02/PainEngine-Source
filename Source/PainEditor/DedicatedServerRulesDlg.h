#pragma once


// CDedicatedServerRulesDlg dialog

class CDedicatedServerRulesDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerRulesDlg)

public:
    void GetFromCfg();
    void SaveToCfg();

	CDedicatedServerRulesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerRulesDlg();

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER_RULES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel() {};
	virtual void OnOK() {};

	DECLARE_MESSAGE_MAP()
public:
    DWORD m_FragLimit;
    DWORD m_TimeLimit;
	DWORD m_CaptureLimit;
	DWORD m_LMSLivesLimit;
    afx_msg void OnDeltaposFraglimitspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposTimelimitspin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposCapturelimitspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposLMSLiveslimitspin(NMHDR *pNMHDR, LRESULT *pResult);
    BOOL m_WeaponsStay;
    BOOL m_AllowBunnyhopping;
    BOOL m_AllowForwardRJ;
    BOOL m_AllowBrightSkin;
    BOOL m_PowerupDrop;
    BOOL m_Powerups;
    BOOL m_TeamDamage;
	BOOL m_ConsoleLock;
	DWORD m_WeaponRespawnTime;
	afx_msg void OnDeltaposWeaponrespawntimespin(NMHDR *pNMHDR, LRESULT *pResult);
};
