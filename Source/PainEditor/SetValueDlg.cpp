// SetValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "SetValueDlg.h"


// CSetValueDlg dialog

IMPLEMENT_DYNAMIC(CSetValueDlg, CDialog)
CSetValueDlg::CSetValueDlg(CWnd* pParent /*=NULL*/,CString title)
	: CDialog(CSetValueDlg::IDD, pParent)
	, m_Value(_T(""))
	, m_Title(title)	
{
}

CSetValueDlg::~CSetValueDlg()
{
}

BOOL CSetValueDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_Title!="")
		SetWindowText(m_Title);
	return true;
}

void CSetValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VALUE, m_Value);
}


BEGIN_MESSAGE_MAP(CSetValueDlg, CDialog)
END_MESSAGE_MAP()


// CSetValueDlg message handlers
