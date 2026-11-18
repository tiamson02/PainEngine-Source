#pragma once
#include "afxwin.h"


// CDedicatedServerMapsDlg dialog

class CDedicatedServerMapsDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerMapsDlg)

public:
    void GetFromCfg(bool getmode=true);
    void SaveToCfg();
	CDedicatedServerMapsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerMapsDlg();

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER_MAPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnCancel() {};
	virtual void OnOK() {};
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_lstLevels;
    CListBox m_lstSelectedLevels;
    afx_msg void OnLbnDblclkLevellist();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnLbnDblclkSelectedlevellist();
    afx_msg void OnBnClickedRemovemap();
    CComboBox m_cmbGameMode;
    afx_msg void OnCbnSelchangeGamemode();
};
