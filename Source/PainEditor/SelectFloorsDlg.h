#pragma once


// CSelectFloorsDlg dialog

class CSelectFloorsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectFloorsDlg)

public:
	CSelectFloorsDlg(int select,int lower,float area,CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectFloorsDlg();

// Dialog Data
	enum { IDD = IDD_SELECTFLOORS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float Area;
	BOOL Select;
	BOOL Lower;
};
