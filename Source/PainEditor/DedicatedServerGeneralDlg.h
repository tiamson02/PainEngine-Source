#pragma once
#include "afxwin.h"


// CDedicatedServerGeneralDlg dialog

class CDedicatedServerGeneralDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerGeneralDlg)

public:
	CDedicatedServerGeneralDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerGeneralDlg();

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel() {};
	virtual void OnOK() {};

	DECLARE_MESSAGE_MAP()
public:
    void GetFromCfg();
    void SaveToCfg();

    CString m_ServerName;
    CString m_ServerPassword;
    DWORD m_Port;
    BYTE m_MaxSpectators;
    BYTE m_MaxPlayers;
    afx_msg void OnBnClickedDefaultport();
    afx_msg void OnDeltaposMaxplayersspin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedNolimitfps();
    DWORD m_FPS;
    afx_msg void OnDeltaposFpsspin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedR2Limit();
    afx_msg void OnBnClickedR1Nolimit();
    afx_msg void OnDeltaposSpin4(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_cmbNetInterface;
};
