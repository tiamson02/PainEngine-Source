// CInfoBar : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "resource.h"
#include "InfoBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COLOR_MASK	RGB(0x00,0x80,0x80)

/////////////////////////////////////////////////////////////////////////////
// CInfoBar

CInfoBar::CInfoBar() : m_EditStrBuf( m_EditLogger )
{
	// TODO: add construction code here.

}

CInfoBar::~CInfoBar()
{
	// TODO: add destruction code here.

}

IMPLEMENT_DYNAMIC(CInfoBar, CXTTabCtrlBar)

BEGIN_MESSAGE_MAP(CInfoBar, CXTTabCtrlBar)
	//{{AFX_MSG_MAP(CInfoBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoBar message handlers

/*static int arHeaderBmps[] = {
	IDB_HEADER_1,
	IDB_HEADER_3
};*/

static int arColWidths[] = {
	16,
	16,
	24,
	120,
	120,
	120
};

static CString arColLabels[] = {
	_T("!"),
	_T(" "),
	_T(" "),
	_T("Description"),
	_T("File"),
	_T("Line")
};

int CInfoBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTabCtrlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the output control.
	if (!m_editOutput.Create( WS_CHILD|WS_VISIBLE|ES_MULTILINE | ES_AUTOVSCROLL |ES_WANTRETURN|
		ES_AUTOHSCROLL | ES_READONLY | WS_VSCROLL, CRect(0,0,0,0), this, 0xff ))
	{
		TRACE0( "Failed to create list control.\n" );
		return FALSE;
	}

	// Set the Edit-control as logger destination
	m_EditLogger.SetEditCtrl( m_editOutput.m_hWnd );

	// Create the console control.
	if (!m_editConsole.Create( WS_CHILD|WS_VISIBLE|ES_MULTILINE | ES_AUTOVSCROLL | 
		ES_AUTOHSCROLL | WS_VSCROLL, CRect(0,0,0,0), this, 0xff ))
	{
		TRACE0( "Failed to create list control.\n" );
		return FALSE;
	}

	// Create the list control.
	if (!m_listCtrl.Create( LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS |
		WS_CHILD | WS_VSCROLL | WS_TABSTOP, CRect(0,0,0,0), this, 0xff ))
	{
		TRACE0( "Failed to create list control.\n" );
		return FALSE;
	}

	// initialize the list control.
	InitListControl();

	// Create the image list to use with the tab control
	if (!m_ilTabImages.Create(IDB_INFO_TABS, 16, 1, COLOR_MASK))
	{
		TRACE0("Failed to create image list.\n");
		return -1;
	}
	
	// Set the image list for the tabbed "Workspace" docking window..
	SetTabImageList( &m_ilTabImages );
	
	// Add the new tab item.
	AddControl( _T( "Output" ), &m_editOutput, 0, 4 );
	AddControl( _T( "Console" ), &m_editConsole, 1, 5 );
	AddControl( _T( "Task List" ), &m_listCtrl, 2, 5 );
	SetActiveView(0);
	
	OutputText("PainEditor Initialized!");
	return 0;
}

void CInfoBar::InitListControl()
{
	// Insert the columns.
	m_listCtrl.BuildColumns(_countof(arColWidths), arColWidths, arColLabels);

	// Subclass the flat header control.
	m_listCtrl.SubclassHeader();

    // lock the first two colums from sizing operations.
	CXTFlatHeaderCtrl* pHeaderCtrl = m_listCtrl.GetFlatHeaderCtrl( );
	if ( pHeaderCtrl != NULL )
	{
		pHeaderCtrl->FreezeColumn(0);
		pHeaderCtrl->FreezeColumn(1);
		pHeaderCtrl->FreezeColumn(2);
		
		// enable autosizing for columns.
		pHeaderCtrl->EnableAutoSize();

		pHeaderCtrl->SetBitmap(2, IDB_HEADER, HDF_STRING);
	}

	ListView_SetExtendedListViewStyle(
		m_listCtrl.m_hWnd, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_listCtrl.InsertItem(0, NULL, 0);
	m_listCtrl.SetItemText(0, 3, _T("Click here to add a new task"));
}

void CInfoBar::OutputText(LPCSTR txt)
{
	if (!m_editOutput.m_hWnd)
		return;
    CString string = txt;
	m_editOutput.SetRedraw(false);
    string.Replace("\n","\r\n");
	m_EditLogger.AddText(string);
	m_editOutput.LineScroll(m_editOutput.GetLineCount());
	m_editOutput.SetRedraw();
}
