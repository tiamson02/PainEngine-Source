// ActionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "ActionDlg.h"
#include ".\actiondlg.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

// CActionDlg dialog

IMPLEMENT_DYNAMIC(CActionDlg, CDialog)
CActionDlg::CActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActionDlg::IDD, pParent)
    , m_Args(_T(""))
    , m_Description(_T(""))
{
}

CActionDlg::~CActionDlg()
{
}

void CActionDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ACTIONLIST, m_ActionList);
    DDX_Control(pDX, IDC_FILTER, m_Filter);
    DDX_Text(pDX, IDC_ARGS, m_Args);
    DDX_Text(pDX, IDC_DESCRIPTION, m_Description);
}


BEGIN_MESSAGE_MAP(CActionDlg, CDialog)
    ON_LBN_SELCHANGE(IDC_ACTIONLIST, OnLbnSelchangeActionlist)
END_MESSAGE_MAP()


// CActionDlg message handlers

BOOL CActionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    
    ScriptObject actions = GScript.Globals()["CAction"];
	/* XXX NW
    actions.PushNil();
    while (actions.Next(-2))
    {
        CString key = actions.GetString(-2);        
        int type = actions.GetType();        
        if (type == LUA_TFUNCTION && key.Find("Action_") != -1)
        {
            m_ActionList.AddString(key.Mid(7));
        }
        actions.Pop(1); 
    }
    actions.Pop(1); 
	*/

    m_ActionList.SelectString(-1,m_Action);

    OnLbnSelchangeActionlist();
    return TRUE;
}

void CActionDlg::OnLbnSelchangeActionlist()
{
    m_Description = "";
    int i = m_ActionList.GetCurSel();
    if (i>-1)
    {
        m_ActionList.GetText(i,m_Action);

        ScriptObject desc = GScript.Globals()["TXT"]["Actions"][m_Action];
        if (!desc.IsNil())
            m_Description = desc.GetString();
    }
    UpdateData(false);
}
