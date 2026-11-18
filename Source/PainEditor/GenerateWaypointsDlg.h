#pragma once


// CGenerateWaypointsDlg dialog

class CGenerateWaypointsDlg : public CDialog
{
	DECLARE_DYNAMIC(CGenerateWaypointsDlg)

public:
	CGenerateWaypointsDlg(CString title,CString ptitle,float param,CWnd* pParent = NULL);   // standard constructor
	virtual ~CGenerateWaypointsDlg();

// Dialog Data
	enum { IDD = IDD_GENERATEWAYPOINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float Param;
	CString Title,PTitle;
	virtual BOOL OnInitDialog();
};
