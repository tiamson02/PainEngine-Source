// ChildFrm.cpp : implementation of the CChildFrame class

#include "stdafx.h"

#include "ChildFrm.h"
#include "Game/Main.h"
#include "Engine/InputSystem.h"

#include "Engine/PCFSystem.h"
#include ".\childfrm.h"
extern ENGINE_API PCFSystem* GEngine;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CView1 : public CView
{
protected:
	CView1();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CView1)

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CView1)
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CView1();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CView1)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


IMPLEMENT_DYNCREATE(CView1, CView)

CView1::CView1()
{
}

CView1::~CView1()
{
}


BEGIN_MESSAGE_MAP(CView1, CView)
	//{{AFX_MSG_MAP(CView1)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CView1 drawing

void CView1::OnDraw(CDC* pDC)
{
	// TODO: add draw code here
	CRect r;
	GetClientRect(&r);
	pDC->SetBkMode(TRANSPARENT);

	int x = r.Width()/2 -50;
	int y = r.Height()/2-25;

	CRect rcSquare(x,y,x+100,y+50);

	int iSavedDC = pDC->SaveDC();
	pDC->FillSolidRect(&rcSquare, GetXtremeColor(XPCOLOR_HIGHLIGHT));
	pDC->Draw3dRect(&rcSquare, GetXtremeColor(COLOR_HIGHLIGHT), GetXtremeColor(COLOR_HIGHLIGHT));
	pDC->SelectObject(&XTAuxData().fontBold);
	pDC->SetTextColor(GetXtremeColor(XPCOLOR_HIGHLIGHT_TEXT));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(_T("View One"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->RestoreDC(iSavedDC);
}

/////////////////////////////////////////////////////////////////////////////
// CView1 diagnostics

#ifdef _DEBUG
void CView1::AssertValid() const
{
	CView::AssertValid();
}

void CView1::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CView1 message handlers

void CView1::OnPaint() 
{
	CPaintDC dc(this);

	// Get the client rect.
	CRect rectClient;
	GetClientRect(&rectClient);

	// Paint to a memory device context to reduce screen flicker.
	CXTMemDC memDC(&dc, rectClient, GetXtremeColor(COLOR_WINDOW));

	OnPrepareDC(&memDC);
	OnDraw(&memDC);
}

BOOL CView1::OnEraseBkgnd(CDC* pDC) 
{
	UNUSED_ALWAYS(pDC);
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMDIChildWnd::OnCreate(lpCreateStruct);
	return TRUE;
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext) 
{
	/*
	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 2,2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0,0, RUNTIME_CLASS(CView1),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView1\n"); 
		return FALSE;
	}
    else
    {
        m_mapRowCol.SetAt(m_wndSplitter.IdFromRowCol(0,0),
            m_wndSplitter.GetPane(0,0));
    }
	
	if (!m_wndSplitter.CreateView(0,1, RUNTIME_CLASS(CView1),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView2\n"); 
		return FALSE;
	}
    else
    {
        m_mapRowCol.SetAt(m_wndSplitter.IdFromRowCol(0,1),
            m_wndSplitter.GetPane(0,1));
    }
	
	if (!m_wndSplitter.CreateView(1,0, RUNTIME_CLASS(CView1),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView3\n"); 
		return FALSE;
	}
    else
    {
        m_mapRowCol.SetAt(m_wndSplitter.IdFromRowCol(1,0),
            m_wndSplitter.GetPane(1,0));
    }
	
	if (!m_wndSplitter.CreateView(1,1, RUNTIME_CLASS(CView1),
		CSize(0, 0), pContext))
	{
		TRACE0("Failed to create CView4\n"); 
		return FALSE;
	}
    else
    {
        m_mapRowCol.SetAt(m_wndSplitter.IdFromRowCol(1,1),
            m_wndSplitter.GetPane(1,1));
    }

	CRect r;
	GetClientRect(&r);

	m_wndSplitter.SetRowInfo( 0, r.Height()/2, 0 );
	m_wndSplitter.SetColumnInfo( 0, r.Width()/2, 0 );
	*/

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	// TODO: Modify this function to change how the frame is activated.

	// first window should be maximized
	if (GetParent()->GetWindow(GW_CHILD) == this)
	{
//		nCmdShow = SW_SHOWMAXIMIZED;
	}
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

LRESULT CChildFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( GEngine && GEngine->InputSys )
		GEngine->InputSys->HandleMessages( m_hWnd, message, wParam, lParam );

	return CMDIChildWnd::WindowProc( message, wParam, lParam );
}

void CChildFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{

	/* XXX
	CMDIChildWnd::OnActivate(nState, pWndOther, bMinimized);
		*/
}

void CChildFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetForegroundWindow();
	SetFocus();
	CMDIChildWnd::OnLButtonDown(nFlags, point);
}


void CChildFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	SetForegroundWindow();
	SetFocus();
	CMDIChildWnd::OnRButtonDown(nFlags, point);
}


void CChildFrame::OnSetFocus(CWnd* pOldWnd)
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	if( GEngine && GEngine->InputSys )
		GEngine->InputSys->Reset();
}

void CChildFrame::OnKillFocus(CWnd* pOldWnd)
{
	CMDIChildWnd::OnKillFocus(pOldWnd);
	if( GEngine && GEngine->InputSys )
		GEngine->InputSys->Reset();
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//cs.lpszClass = AfxRegisterWndClass( 0, NULL, NULL,
	//	AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	return TRUE;
}
