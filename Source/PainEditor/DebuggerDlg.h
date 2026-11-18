#pragma once


// CDebuggerDlg dialog

class CDebuggerDlg : public CDialog
{
	DECLARE_DYNAMIC(CDebuggerDlg)

public:
	CDebuggerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebuggerDlg();

// Dialog Data
	enum { IDD = IDD_DEBUGGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
