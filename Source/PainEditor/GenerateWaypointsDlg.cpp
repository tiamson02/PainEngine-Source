// GenerateWaypointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "GenerateWaypointsDlg.h"


// CGenerateWaypointsDlg dialog

IMPLEMENT_DYNAMIC(CGenerateWaypointsDlg, CDialog)
CGenerateWaypointsDlg::CGenerateWaypointsDlg(CString title,CString ptitle,float param,CWnd* pParent /*=NULL*/)
	: CDialog(CGenerateWaypointsDlg::IDD, pParent)
	, Param(param), Title(title), PTitle(ptitle)
{
}

CGenerateWaypointsDlg::~CGenerateWaypointsDlg()
{
}

void CGenerateWaypointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STEP, Param);
}


BEGIN_MESSAGE_MAP(CGenerateWaypointsDlg, CDialog)
END_MESSAGE_MAP()


// CGenerateWaypointsDlg message handlers

BOOL CGenerateWaypointsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(Title);
	GetDlgItem(IDC_PARAM)->SetWindowText(PTitle);
	return true;
}
