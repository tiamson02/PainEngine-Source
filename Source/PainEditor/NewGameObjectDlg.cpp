// NewGameObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "NewGameObjectDlg.h"
#include "MainFrm.h"
#include "GameObjectsBar.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

// CNewGameObjectDlg dialog

#ifdef EDITOR
IMPLEMENT_DYNAMIC(CNewGameObjectDlg, CDialog)
CNewGameObjectDlg::CNewGameObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewGameObjectDlg::IDD, pParent)
	, m_txtName(_T(""))
{
}

CNewGameObjectDlg::~CNewGameObjectDlg()
{
}

BOOL CNewGameObjectDlg::OnInitDialog()
{
	UpdateData(false);

	DynamicArray<String> files;
	GFileMan.FindFiles("../Data/LScripts/Classes/*.lua",files);

	for (int i=0;i<files.Size();i++)
    {
        CString cls = String::StripExtension(files[i]).AsChar();
        if (cls != "CObject" && cls != "CLevel" && cls != "CWeapon")
            m_cmbClasses.AddString(cls);		
    }

	int sel = m_cmbClasses.FindStringExact(-1,m_txtLastClass);
	m_cmbClasses.SetCurSel(max(sel,0));
	return TRUE;
}

void CNewGameObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASSES, m_cmbClasses);
	DDX_Text(pDX, IDC_NAME, m_txtName);
}


BEGIN_MESSAGE_MAP(CNewGameObjectDlg, CDialog)
END_MESSAGE_MAP()

void CNewGameObjectDlg::OnOK()
{
	CString cls;
	UpdateData();
	m_txtName = m_txtName.Trim();	
	m_cmbClasses.GetLBText(m_cmbClasses.GetCurSel(),cls);

	if (m_txtName == "" || m_txtName[0] == ' ' || m_txtName[0] < 'A' || m_txtName[0] > 'z')
		MessageBox("Please enter a valid name.");
	else 
		if (!GScript.Globals()[m_txtName].IsNil())
			MessageBox("This name is already in use.");
	else
	{
		GScript.DoString("Editor:AddObject(CloneTemplate('%s'),'%s')",cls,m_txtName);
        CGameObjectsBar::Get().UpdateView(m_txtName);
		m_txtLastClass =  cls;
		CDialog::OnOK();
		GEngine->ResetTimer();
	}
}
#endif