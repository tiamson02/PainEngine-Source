#include "StdAfx.h"
#include "Resource.h"
#include "MainFrm.h"
#include "PainEditor.h"
#include "ScrolledDialogBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScrolledDialogBar

CScrolledDialogBar::CScrolledDialogBar()
{
	m_InnerDlg = NULL;
	// TODO: add construction code here.
}

CScrolledDialogBar::~CScrolledDialogBar()
{
	if (m_InnerDlg)
		delete m_InnerDlg;
	// TODO: add destruction code here.
}

IMPLEMENT_DYNAMIC(CScrolledDialogBar, CWnd)

BEGIN_MESSAGE_MAP(CScrolledDialogBar, CWnd)
	//{{AFX_MSG_MAP(CScrolledDialogBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()


int CScrolledDialogBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndPlace.Create ("STATIC","",WS_CHILD|WS_VISIBLE,CRect (0,22,253,500),this,0,NULL);
	return 0;
}

void CScrolledDialogBar::CreateInnerDialog(CDialog *dlg,int dialog_resource)
{
	m_InnerDlg = dlg;
	m_InnerDlg->Create (dialog_resource,&m_wndPlace);
	m_InnerDlg->ShowWindow (SW_NORMAL);
}


void CScrolledDialogBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType,cx,cy);
	Refresh();
}

void CScrolledDialogBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);
	Refresh();
}

void CScrolledDialogBar::Refresh()
{
	int yMargin = 1;


	RECT drc;
	GetWindowRect(&drc);
	m_wndPlace.SetWindowPos(NULL,0,yMargin,drc.right - drc.left,drc.bottom - drc.top-yMargin,SWP_DRAWFRAME);
	if (m_InnerDlg)
	{
		RECT crc;
		m_InnerDlg->GetWindowRect(&crc);
		m_InnerDlg->SetWindowPos(NULL,0,0,drc.right-drc.left,crc.bottom-crc.top,SWP_DRAWFRAME);
	}
}

BOOL CScrolledDialogBar::PreTranslateMessage(MSG* pMsg) 
{
	UpdateDialogControls( this, TRUE );
	return CWnd::PreTranslateMessage(pMsg);	
}


