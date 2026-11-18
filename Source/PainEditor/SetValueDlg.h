#pragma once


// CSetValueDlg dialog

class CSetValueDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetValueDlg)

public:
	CSetValueDlg(CWnd* pParent = NULL,CString title="");   // standard constructor
	virtual ~CSetValueDlg();

// Dialog Data
	enum { IDD = IDD_SETVALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_Title;
	CString m_Value;
};
