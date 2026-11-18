// SaveLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "SaveLevelDlg.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/LeString.h"

// CSaveLevelDlg dialog

IMPLEMENT_DYNAMIC(CSaveLevelDlg, CDialog)
CSaveLevelDlg::CSaveLevelDlg(CWnd* pParent /*=NULL*/,CString title)
	: CDialog(CSaveLevelDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Title (title)
{
}

CSaveLevelDlg::~CSaveLevelDlg()
{
}

void CSaveLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_Name);
}


BEGIN_MESSAGE_MAP(CSaveLevelDlg, CDialog)
END_MESSAGE_MAP()

BOOL CSaveLevelDlg::OnInitDialog()
{
	if (m_Title!= "") SetWindowText(m_Title);
	m_Name = GScript.Globals()["Lev"]["_Name"].GetString();
	UpdateData(false);
	return true;
}

void CSaveLevelDlg::OnOK()
{
	UpdateData(true);
	m_Name.Trim();

	FileManagerStd fm;
	DynamicArray<String> files;
	fm.FindFiles("../Data/Levels/"+m_Name,files,EFindDirs);

	if (files.Size()>0)
	{
		MessageBox("Project already exist! Please change the name.");
		return;
	}

	if (m_Name == "" || m_Name[0] < 'A' || m_Name[0] > 'z')
		MessageBox("Please enter a valid name.");
	else
	{
		GScript.DoString("Game:SaveLevel('%s')",m_Name);
		CDialog::OnOK();
	}
}


// CSaveLevelDlg message handlers
