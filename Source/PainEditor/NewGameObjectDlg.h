#pragma once
#include "afxwin.h"


// CNewGameObjectDlg dialog

class CNewGameObjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewGameObjectDlg)

public:
	CNewGameObjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewGameObjectDlg();

// Dialog Data
	enum { IDD = IDD_NEWGAMEOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	CComboBox m_cmbClasses;
public:
	CString m_txtName,m_txtLastClass;
};
