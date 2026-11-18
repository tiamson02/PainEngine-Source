#pragma once


// CInfoDlg dialog

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();

// Dialog Data
	enum { IDD = IDD_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();
public:
    CString m_Text;
    CComboBox m_cmbResolution;
    afx_msg void OnCbnSelchangeResolution();
    afx_msg void OnBnClickedInversemouse();
    afx_msg void OnBnClickedShadows();
};
