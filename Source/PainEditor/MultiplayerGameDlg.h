#pragma once
#include "afxwin.h"


// CMultiplayerGameDlg dialog

class CMultiplayerGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CMultiplayerGameDlg)

public:
	CMultiplayerGameDlg(CWnd* pParent = NULL,bool servermode=true,CString hostname="",int port=3455);   // standard constructor
	virtual ~CMultiplayerGameDlg();

// Dialog Data
	enum { IDD = IDD_MULTIPLAYERGAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SwitchMode(BOOL server);

	DECLARE_MESSAGE_MAP()
public:
	BOOL ServerMode;
	CString HostName,PlayerName,Password,Map;
	UINT Port,FragsLimit;
	int Speed;
	afx_msg void OnBnClickedClientmode();
	afx_msg void OnBnClickedServermode();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox m_cmbMaps;
    CString CDKey;
    BOOL IsPublic;
};
