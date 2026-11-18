#pragma once


// CProfilerDlg dialog

class CProfilerDlg : public CDialog
{
	DECLARE_DYNAMIC(CProfilerDlg)

public:
	CProfilerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProfilerDlg();

// Dialog Data
	enum { IDD = IDD_PROFILER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CString m_txtStats;
    afx_msg void OnTimer(UINT nIDEvent);
};
