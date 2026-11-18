// SelectFloorsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "SelectFloorsDlg.h"


// CSelectFloorsDlg dialog

IMPLEMENT_DYNAMIC(CSelectFloorsDlg, CDialog)
CSelectFloorsDlg::CSelectFloorsDlg( int select, int lower, float area, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectFloorsDlg::IDD, pParent)
	, Area(area)
	, Select(select)
	, Lower(lower)
{
}

CSelectFloorsDlg::~CSelectFloorsDlg()
{
}

void CSelectFloorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_AREA, Area);
	DDX_Check(pDX, IDC_SELECT, Select);
	DDX_Check(pDX, IDC_LOWER, Lower);
}


BEGIN_MESSAGE_MAP(CSelectFloorsDlg, CDialog)
END_MESSAGE_MAP()


// CSelectFloorsDlg message handlers
