// ScrolledInnerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "ScrolledInnerDlg.h"


// CScrolledInnerDlg dialog
IMPLEMENT_DYNAMIC(CScrolledInnerDlg, CDialog)
CScrolledInnerDlg::CScrolledInnerDlg(CWnd* pParent /*=NULL*/)
	: CDialog()
{
}

CScrolledInnerDlg::~CScrolledInnerDlg()
{
}


BEGIN_MESSAGE_MAP(CScrolledInnerDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CScrolledInnerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	// START DRAGGING 
	SetCapture();
	m_bDragging = TRUE;
	m_ptDragFrom = point;
	::SetCursor(AfxGetApp()->LoadCursor (IDC_CURSOR_HAND));
	CDialog::OnLButtonDown(nFlags, point);

}

void CScrolledInnerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	/// IF WE WHERE DRAGGING THEN RELEASE THE MOUSE CAPTURE
	if(m_bDragging)
	{
		ReleaseCapture();
		m_bDragging = FALSE;
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CScrolledInnerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{

	int ydiff;

	if(m_bDragging && nFlags & MK_LBUTTON)
	{
	   // GET DIALOGS WINDOW SCREEN COORDINATES
	   CRect rcWnd;
	   GetWindowRect(&rcWnd);

	   // GET PARENTS CLIENT RECTANGLE
	   CRect prect;
	   GetParent()->GetClientRect (prect);

	   // IF WE HAVE TO GO DOWN OR UP // 
	   if (m_ptDragFrom.y>point.y) {	
		  ydiff = point.y - m_ptDragFrom.y;
		   posY+=ydiff;
	   }

	   if (m_ptDragFrom.y<point.y) {	
		  ydiff = m_ptDragFrom.y -point.y;
		   posY-=ydiff;
	   }
	   //////////////////////////////


       // CALCULATE IF WE ARE GOING TO EXCEED BOTTOM DIALOG BORDER
	   int tmp=prect.Height ()-rcWnd.Height ();
	   
	   // CONSTRAINTS !
	   if (posY<tmp+1) posY=tmp+1;
	   if (posY>-1) posY=-1;

	   // MOVE THE DIALOG 
		SetWindowPos(NULL, 0, 
			         posY, 
					 rcWnd.Width(), 
					 rcWnd.Height(), SWP_SHOWWINDOW|SWP_NOSIZE);


	}	
	
	CDialog::OnMouseMove(nFlags, point);
}

