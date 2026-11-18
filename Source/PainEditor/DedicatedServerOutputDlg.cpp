// DedicatedServerOutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "DedicatedServerOutputDlg.h"
#include ".\dedicatedserveroutputdlg.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


// CDedicatedServerOutputDlg dialog

IMPLEMENT_DYNAMIC(CDedicatedServerOutputDlg, CDialog)
CDedicatedServerOutputDlg::CDedicatedServerOutputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDedicatedServerOutputDlg::IDD, pParent), m_EditStrBuf( m_EditLogger )
	, m_strCmd(_T(""))
{
}

CDedicatedServerOutputDlg::~CDedicatedServerOutputDlg()
{
}

void CDedicatedServerOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_editLog);
	DDX_Text(pDX, IDC_CMD, m_strCmd);
}

BOOL CDedicatedServerOutputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Set the Edit-control as logger destination
	m_EditLogger.SetEditCtrl( m_editLog.m_hWnd );
	return TRUE;
}


void CDedicatedServerOutputDlg::PrintText(LPCSTR txt)
{
	if (!m_editLog) return;

	CString string = txt;
	m_editLog.SetRedraw(false);
	string.Replace("\n","\r\n");
	m_EditLogger.AddText(string);
	m_editLog.LineScroll(m_editLog.GetLineCount());
	m_editLog.SetRedraw();
	m_editLog.LineScroll(m_editLog.GetLineCount());
}


BEGIN_MESSAGE_MAP(CDedicatedServerOutputDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// CDedicatedServerOutputDlg message handlers

void CDedicatedServerOutputDlg::OnEnChangeEdit1()
{
	//MessageBox("dupa");
}

void CDedicatedServerOutputDlg::OnOK() 
{
	UpdateData();
	if (GScript.Initialized() && !GScript.Globals()["Console"].IsNil())
		GScript.Call("Hud_OnConsoleCommand",0,"%s",(LPCSTR) (/*"\\"+*/m_strCmd));
	else
		MessageBox("Server is not started!");
};
