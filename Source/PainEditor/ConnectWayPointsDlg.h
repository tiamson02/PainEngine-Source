#pragma once


// CConnectWayPointsDlg dialog

class CConnectWayPointsDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectWayPointsDlg)

public:
	CConnectWayPointsDlg(float minDistance, float maxDistance, int connect, int disconnect,CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectWayPointsDlg();
	void Set( float minDistance, float maxDistance, int connect, int disconnect );

// Dialog Data
	enum { IDD = IDD_CONNECTWAYPOINS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float MinDistance;
	float MaxDistance;
	BOOL Connect;
	BOOL Disconnect;
};
