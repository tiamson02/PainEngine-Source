// FileSystemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "FileSystemDlg.h"

// FileSystemDlg dialog

IMPLEMENT_DYNAMIC(FileSystemDlg, CDialog)
FileSystemDlg::FileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FileSystemDlg::IDD, pParent)
{
}

FileSystemDlg::~FileSystemDlg()
{
}

void FileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_lstFileMonitor);
	DDX_Control(pDX, IDC_FILETREE, m_FileTree);
}


BEGIN_MESSAGE_MAP(FileSystemDlg, CDialog)
END_MESSAGE_MAP()


// FileSystemDlg message handlers

BOOL FileSystemDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_lstFileMonitor.InsertColumn(0,"Nr",LVCFMT_LEFT,40);
    m_lstFileMonitor.InsertColumn(1,"File",LVCFMT_LEFT,400);
    m_lstFileMonitor.InsertColumn(3,"Pack",LVCFMT_LEFT,120);

    if (GFileMan.pRegisteredFiles)
    {
        for(int i=0; i<GFileMan.pRegisteredFiles->Size(); ++i)
        {
            char buf[1000];
            int item = m_lstFileMonitor.InsertItem(i,itoa(i,buf,10));          
            FIdx * pFIdx = GFileMan.pRegisteredFiles->GetPair(i).Value;
            sprintf(buf,pFIdx?"%s":"[d] %s",GFileMan.pRegisteredFiles->GetPair(i).Key.AsChar());
            m_lstFileMonitor.SetItemText(item,1,buf);
            if (pFIdx)
                m_lstFileMonitor.SetItemText(item,2,pFIdx->pPack->GetFName());

        }
		char buf[1000];
		sprintf(buf,"Files: %d",GFileMan.pRegisteredFiles->Size());
		SetDlgItemText(IDC_HOWMANYFILES,buf);
    }

	if (GFileMan.pFolders)
	{
		for(int i=0; i<GFileMan.pFolders->Size(); ++i)
		{
			char buf[1000];
			FIdx * pFIdx = GFileMan.pFolders->GetPair(i).Value;
			if (!pFIdx)
				continue;
			sprintf(buf,pFIdx?"%s":"[d] %s",GFileMan.pFolders->GetPair(i).Key.AsChar());
			HTREEITEM hti = m_FileTree.InsertItem(buf);
			while (pFIdx)
			{
				m_FileTree.InsertItem(pFIdx->name,hti);
				pFIdx = pFIdx->next;
			}
			//m_lstFileMonitor.SetItemText(item,1,buf);
			//if (pFIdx)
			//	m_lstFileMonitor.SetItemText(item,2,pFIdx->pPack->GetFName());
		}
	}
    
    return true;
}


void FileSystemDlg::FillTree(FIdx *pFIdx, HTREEITEM hti)
{
	/*
	while (node)
	{
		HTREEITEM hti2 = m_FileTree.InsertItem(pFIdx->pPack->strPath,hti);
		if (node->FirstChild)
			FillTree(node->FirstChild, hti2);			
		node = node->Next;
	}
	*/
}
