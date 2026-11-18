#pragma once
#include "afxwin.h"
#include "core/nommgr.h"
#include "EditLog.h"
#include "editlog_stream.h"
#include "core/mmgr.h"


// COutputDlg dialog

class COutputDlg : public CDialog
{
	DECLARE_DYNAMIC(COutputDlg)

public:
	COutputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COutputDlg();

// Dialog Data
	enum { IDD = IDD_OUTPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    CEditLog			m_EditLogger;
    std::editstreambuf	m_EditStrBuf; 
    CEdit m_editLog;

    BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    void PrintText(LPCSTR txt);

};
