// OutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "OutputDlg.h"


// COutputDlg dialog

IMPLEMENT_DYNAMIC(COutputDlg, CDialog)
COutputDlg::COutputDlg(CWnd* pParent): CDialog(COutputDlg::IDD, pParent), m_EditStrBuf( m_EditLogger )

{
}

COutputDlg::~COutputDlg()
{
}

BOOL COutputDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    // Set the Edit-control as logger destination
    m_EditLogger.SetEditCtrl( m_editLog.m_hWnd );
    return TRUE;
}


void COutputDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOG, m_editLog);
}

void COutputDlg::PrintText(LPCSTR txt)
{
    if (!m_editLog) return;

    CString string = txt;
    m_editLog.SetRedraw(false);
    string.Replace("\n","\r\n");
    m_EditLogger.AddText(string);
    m_editLog.LineScroll(m_editLog.GetLineCount());
    m_editLog.SetRedraw();
}

BEGIN_MESSAGE_MAP(COutputDlg, CDialog)
END_MESSAGE_MAP()


// COutputDlg message handlers
