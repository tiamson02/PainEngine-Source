#pragma once
#include "afxcmn.h"
#include "afxwin.h"


#ifdef EDITOR

// CLoadLevelDlg dialog

class CLoadLevelDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadLevelDlg)

public:
	CString mLevelName;
	CLoadLevelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadLevelDlg();

// Dialog Data
	enum { IDD = IDD_LOADLEVEL };

protected:
	CString GetSelected();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void FillLevelList();

	DECLARE_MESSAGE_MAP()
public:
	CXTListCtrl m_LevelList;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkLevellist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedCreatepkm();
};

#endif