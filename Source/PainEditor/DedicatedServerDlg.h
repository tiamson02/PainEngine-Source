#pragma once

#include "DedicatedServerGeneralDlg.h"
#include "DedicatedServerMapsDlg.h"
#include "DedicatedServerRulesDlg.h"
#include "DedicatedServerUserCfgDlg.h"
#include "DedicatedServerOutputDlg.h"
#include "afxcmn.h"

// CDedicatedServerDlg dialog

class CDedicatedServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerDlg)

public:
    CDedicatedServerGeneralDlg  m_dlgGeneral;
    CDedicatedServerMapsDlg     m_dlgMaps;
    CDedicatedServerRulesDlg    m_dlgRules;
	CDedicatedServerUserCfgDlg  m_dlgUserCfg;
	CDedicatedServerOutputDlg  m_dlgOutput;
	CDedicatedServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerDlg();

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    void UpdateDialogsPosition(RECT *pRect);

	DECLARE_MESSAGE_MAP()
public:
//#ifdef EDITOR
    CXTTabCtrl	m_tabCtrl;
//#else
//	CTabCtrl	m_tabCtrl;
//#endif
    afx_msg void OnBnClickedStart();
    afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

private:
	bool	mRunning;
public:
	afx_msg void OnBnClickedLua();
};
