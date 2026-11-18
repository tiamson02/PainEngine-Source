#pragma once
#include "afxwin.h"


// CActionDlg dialog

class CActionDlg : public CDialog
{
	DECLARE_DYNAMIC(CActionDlg)

public:

    CString m_Action,m_Args;
	CActionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CActionDlg();

// Dialog Data
	enum { IDD = IDD_ACTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_ActionList;
    CComboBox m_Filter;
    afx_msg void OnLbnSelchangeActionlist();
    CString m_Description;
};
