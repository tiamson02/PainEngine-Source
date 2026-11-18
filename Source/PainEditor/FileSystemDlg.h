#pragma once
#include "afxcmn.h"


// FileSystemDlg dialog

class FileSystemDlg : public CDialog
{
	DECLARE_DYNAMIC(FileSystemDlg)

public:
	FileSystemDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~FileSystemDlg();
	void FillTree(FIdx *pFIdx, HTREEITEM hti);

// Dialog Data
	enum { IDD = IDD_FILESYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_lstFileMonitor;
	CTreeCtrl m_FileTree;
};
