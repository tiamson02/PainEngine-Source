// CTabCtrlBar : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Resource.h"
#include "TabCtrlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlBar



IMPLEMENT_DYNAMIC(CTabCtrlBar, CXTTabCtrlBar)

BEGIN_MESSAGE_MAP(CTabCtrlBar, CXTTabCtrlBar)
	//{{AFX_MSG_MAP(CTabCtrlBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlBar message handlers

void CTabCtrlBar::OnTabSelChange(int nIDCtrl, CXTTabCtrl* pTabCtrl)
{
    CXTTabCtrlBar::OnTabSelChange(nIDCtrl, pTabCtrl);

	if ( pTabCtrl && ::IsWindow( pTabCtrl->m_hWnd ) )
	{
		CWnd* pWnd = GetActiveView( );
		if ( !pWnd )
		{
			return;
		}

		POSITION pos;
		for ( pos = GetTabCtrl().m_tcbItems.GetHeadPosition( ); pos; GetTabCtrl().m_tcbItems.GetNext( pos ) )
		{
			// Get the private data for the view
			XT_TCB_ITEM* pMember = GetTabCtrl().m_tcbItems.GetAt( pos) ;
			ASSERT( pMember );
			
			if ( pMember->pWnd == pWnd )
			{
				SetCaption( pMember->szTabLabel );
				break;
			}
		}
	}
}
