// ConnectWayPointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "ConnectWayPointsDlg.h"


// CConnectWayPointsDlg dialog

IMPLEMENT_DYNAMIC(CConnectWayPointsDlg, CDialog)
CConnectWayPointsDlg::CConnectWayPointsDlg(float minDistance, float maxDistance, int connect, int disconnect,CWnd* pParent /*=NULL*/)
	: CDialog(CConnectWayPointsDlg::IDD, pParent)
	, MinDistance(minDistance)
	, MaxDistance(maxDistance)
	, Connect(connect)
	, Disconnect(disconnect)
{
}


CConnectWayPointsDlg::~CConnectWayPointsDlg()
{
}

void CConnectWayPointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MINDISTANCE, MinDistance);
	DDX_Text(pDX, IDC_MAXDISTANCE, MaxDistance);
	DDX_Check(pDX, IDC_CONNECT, Connect);
	DDX_Check(pDX, IDC_DISCONNECT, Disconnect);
}

void CConnectWayPointsDlg::Set( float minDistance, float maxDistance, int connect, int disconnect )
{
	 MinDistance =  minDistance;
	 MaxDistance = maxDistance;
	 Connect = connect;
	 Disconnect = disconnect ;
}

BEGIN_MESSAGE_MAP(CConnectWayPointsDlg, CDialog)
END_MESSAGE_MAP()

// CConnectWayPointsDlg message handlers
