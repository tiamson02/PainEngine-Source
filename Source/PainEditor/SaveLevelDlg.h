#pragma once


// CSaveLevelDlg dialog

class CSaveLevelDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveLevelDlg)

public:

	CSaveLevelDlg(CWnd* pParent = NULL,CString title = "");   // standard constructor
	virtual ~CSaveLevelDlg();

// Dialog Data
	enum { IDD = IDD_SAVELEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CString m_Name;
	CString m_Title;


};
