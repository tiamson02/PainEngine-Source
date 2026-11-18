#pragma once

#include "afxwin.h"
#include "core/nommgr.h"
#include "EditLog.h"
#include "editlog_stream.h"
#include "core/mmgr.h"



// CDedicatedServerOutputDlg dialog

class CDedicatedServerOutputDlg : public CDialog
{
	DECLARE_DYNAMIC(CDedicatedServerOutputDlg)

public:
	CDedicatedServerOutputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDedicatedServerOutputDlg();

	CEditLog			m_EditLogger;
	std::editstreambuf	m_EditStrBuf; 
	CEdit m_editLog;
	void PrintText(LPCSTR txt);

// Dialog Data
	enum { IDD = IDD_DEDICATEDSERVER_OUTPUT };

protected:
	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel() {};
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString m_strCmd;
};
