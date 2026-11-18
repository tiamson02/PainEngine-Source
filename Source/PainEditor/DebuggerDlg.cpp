// DebuggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DebuggerDlg.h"


// CDebuggerDlg dialog

IMPLEMENT_DYNAMIC(CDebuggerDlg, CDialog)
CDebuggerDlg::CDebuggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebuggerDlg::IDD, pParent)
{
}

CDebuggerDlg::~CDebuggerDlg()
{
}

void CDebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDebuggerDlg, CDialog)
END_MESSAGE_MAP()


// CDebuggerDlg message handlers
