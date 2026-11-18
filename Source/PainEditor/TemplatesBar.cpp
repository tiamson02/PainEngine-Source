// TemplatesBar.cpp : implementation file
//

#include "stdafx.h"
#include "TemplatesBar.h"
#include "PropertiesBar.h"
#include "GameObjectsBar.h"
#include "PainEditor.h"
#include "MainFrm.h"
#include "game/Main.h" 

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplatesBar

#ifdef EDITOR

CTemplatesBar::CTemplatesBar()
{
}

CTemplatesBar::~CTemplatesBar()
{
}

IMPLEMENT_DYNAMIC(CTemplatesBar, CXTTreeCtrl)

BEGIN_MESSAGE_MAP(CTemplatesBar, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CTemplatesBar)
	ON_WM_CREATE()
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclkObjtree)
    ON_NOTIFY_REFLECT(NM_RCLICK,OnRclickObjtree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplatesBar message handlers

BOOL CTemplatesBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

int CTemplatesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_TREE, 16, 1, RGB(0x00,0x80,0x80)))
		return -1;

	// Set the image list for the tree control.
	SetImageList( &m_imageList, TVSIL_NORMAL );	
	return 0;
}


CTemplatesBar& CTemplatesBar::Get()
{
	static CTemplatesBar theTree;
	return theTree;
}

BOOL CTemplatesBar::Create(CWnd* pParentWnd)
{
	if (::IsWindow(m_hWnd))
	{
		SetParent(pParentWnd);
	}
	else if (!CXTTreeCtrl::Create( WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), pParentWnd, 0xFF ))
	{
		TRACE0( "Unable to create tree control.\n" );
		return false;
	}

	return true;
}


// Sort the item in reverse alphabetical order.
static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    // lParamSort contains a pointer to the tree control.
    // The lParam of an item is just its handle, 
    // as specified with SetItemData
    CTreeCtrl* pmyTreeCtrl = (CTreeCtrl*) lParamSort;

    int i1,i2;
    pmyTreeCtrl->GetItemImage((HTREEITEM) lParam1,i1,i2);
    int ii1,ii2;
    pmyTreeCtrl->GetItemImage((HTREEITEM) lParam2,ii1,ii2);
    if (i1>ii1) return 1;
    if (ii1>i1) return -1;

    CString    strItem1 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1);
    CString    strItem2 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam2);
    return strcmp(strItem2, strItem1);
}


void CTemplatesBar::FillNode(CString path,HTREEITEM hti)
{
    SetBkColor(RGB(255,255,240));

    DynamicArray<String> files;
    GFileMan.FindFiles(path+"/*.*",files,EFindDirs);
    for (int i=0;i<files.Size();i++)
    {
        HTREEITEM hci = InsertItem(files[i].AsChar(), 1,1, hti);
        FillNode(path+CString("/")+files[i].AsChar(),hci);
    } 

    files.Clear();
    GFileMan.FindFiles(path+"/*.C*",files,EFindFiles);
    for (int i=0;i<files.Size();i++)
    {
        HTREEITEM ni = InsertItem(files[i].AsChar(), 6, 6, hti,TVI_SORT);
        //SetItemState(ni,0,TVIS_DROPHILITED);
        //SetItemState(ni,0,TVIS_DROPHILITED);
        //sprawdzam czy ma skrypt		
        CString script =  CString(String::StripExtension(files[i]).AsChar()) + ".lua";
        if (GFileMan.FileExist(path + "/" + script))
            InsertItem(script, 4, 4, ni);
    }

    TVSORTCB tvs;
    tvs.hParent = hti;
    tvs.lpfnCompare = MyCompareProc;
    tvs.lParam = (LPARAM) this;

    //SortChildrenCB(&tvs);
}

HTREEITEM CopyItem(CTreeCtrl *tree, HTREEITEM hItem, HTREEITEM htiNewParent, 
                   HTREEITEM htiAfter = TVI_LAST)
{
    TV_INSERTSTRUCT         tvstruct;
    HTREEITEM                       hNewItem;
    CString                         sText;

    // get information of the source item
    tvstruct.item.hItem = hItem;
    tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | 
        TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tree->GetItem(&tvstruct.item);  
    sText = tree->GetItemText( hItem );

    tvstruct.item.cchTextMax = sText.GetLength();
    tvstruct.item.pszText = sText.LockBuffer();

    // Insert the item at proper location
    tvstruct.hParent = htiNewParent;
    tvstruct.hInsertAfter = htiAfter;
    tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
    hNewItem = tree->InsertItem(&tvstruct);
    sText.ReleaseBuffer();

    // Now copy item data and item state.
    tree->SetItemData( hNewItem, tree->GetItemData( hItem ));
    tree->SetItemState( hNewItem, tree->GetItemState( hItem, TVIS_STATEIMAGEMASK ), 
        TVIS_STATEIMAGEMASK );

    return hNewItem;
}

HTREEITEM CopyBranch(CTreeCtrl *tree,HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter = TVI_LAST)
{
    HTREEITEM hChild;

    HTREEITEM hNewItem = CopyItem( tree,htiBranch, htiNewParent, htiAfter );
    hChild = tree->GetChildItem(htiBranch);
    while( hChild != NULL)
    {
        // recursively transfer all the items
        CopyBranch(tree,hChild, hNewItem);  
        hChild = tree->GetNextSiblingItem( hChild );
    }
    return hNewItem;
}


void CTemplatesBar::SortByBaseObj(HTREEITEM hti)
{
    while (hti)
    {
        HTREEITEM ht = hti;
        hti = GetNextSiblingItem(hti);

        int i1,i2;
        GetItemImage(ht,i1,i2);
        if (GetChildItem(ht))
            SortByBaseObj(GetChildItem(ht));		

        HTREEITEM p = GetParentItem(ht);
        if (i1 == 6 && p && GetItemText(p) != "#CurrentLevel#")
        {			
            CString name = GetItemText(ht);

			ScriptObject t = GScript.Globals()["Templates"][name];
			if (!t.IsNil()) {
				CString basename = t["BaseObj"].GetString();
				HTREEITEM parent = FindItem(basename,false,true,true,GetRootItem());
				if (parent && parent)
				{
					CString nm = GetItemText(parent);
					if (basename == nm)
					{
						CopyBranch(this,ht,parent);
						DeleteItem(ht);
					}
					//DeleteItem(ht);				
					//InsertItem(name, 6, 6, parent,TVI_SORT);
				}
			}
        }
    }
}



void CTemplatesBar::UpdateView(CString path)
{
    CString selected;
    HTREEITEM hti = GetSelectedItem();
    if (hti)
        selected = GetItemText(hti);	

    SetRedraw(false);
    DeleteAllItems();

    FillNode(path,NULL);

    UpdateLevelTemplates();

    // sortowanie po baseobj
    if (GScript.Initialized())
        SortByBaseObj(GetRootItem());

    SelectItem(FindItem(selected,false,true,true,GetRootItem()));
    SetRedraw();
}

void CTemplatesBar::UpdateLevelTemplates()
{
    HTREEITEM hti = FindItem("#CurrentLevel#",false,true,true,GetRootItem());
    if (hti)
        DeleteItem(hti);

    hti = InsertItem("#CurrentLevel#", 1,1, TVI_ROOT,TVI_SORT);

    ScriptObject level = GScript.Globals()["Lev"];
    CString name = level["_Name"].GetString();
    FillNode("../Data/Levels/"+name+"/Templates",hti);
}




void CTemplatesBar::OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CPoint CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(&CursorPos);

    unsigned int uFlags;
    HTREEITEM hI = HitTest(CursorPos, &uFlags);

    int i1,i2;
    GetItemImage(hI,i1,i2);
    if (i1 != 6 && i1 != 4)
        return;

    if ((TVHT_ONITEM & uFlags) && i1 == 6)
    {
        OnCommand(PM_Clone,0);
        SetRedraw(false);
        Expand(hI, TVE_TOGGLE);
        SetRedraw();
    }
    *pResult = 0;
}

void CTemplatesBar::OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CPoint CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(&CursorPos);

	CMenu popMenu;
	popMenu.CreatePopupMenu();
	popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Refresh,"Refresh Tree");

    unsigned int uFlags;
    HTREEITEM hI = HitTest(CursorPos, &uFlags);
    int i1,i2;
    GetItemImage(hI,i1,i2);
    
	if (i1 == 6 || i1 == 4)
	{
		popMenu.AppendMenu(MF_SEPARATOR);
        Select(hI,TVGN_CARET);
		if (i1 == 6)
		{
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Clone,"Create Object");
			popMenu.SetDefaultItem(popMenu.GetMenuItemCount()-1,true);
			popMenu.AppendMenu(MF_SEPARATOR);
		}
		popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_EditScript,"Edit");
	}

    POINT pt;  
    GetCursorPos(&pt);
    popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);    

	*pResult = 0;
}


void CTemplatesBar::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LPNMTREEVIEW pTV = (LPNMTREEVIEW) pNMHDR;
        

    int i1,i2;
    GetItemImage(pTV->itemNew.hItem,i1,i2);

    if (i1 != 1 && i1 != 2)
        return;

    if (i1 == 1)
        SetItemImage(pTV->itemNew.hItem,2,2);
    else
        SetItemImage(pTV->itemNew.hItem,1,1);

    *pResult = 0;
}

BOOL CTemplatesBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	HTREEITEM hI = GetSelectedItem();
	if (hI)
	{
		int i1,i2;
		GetItemImage(hI,i1,i2);

		CString name  = GetItemText(hI);
		switch (wParam)
		{
		case PM_Refresh:
			UpdateView("../Data/LScripts/Templates");
			break;
		case PM_Clone:
			GScript.DoString("Editor:AddObject(CloneTemplate('%s'))",name);
			break;
		case PM_EditScript:
			{
				CString path;
				HTREEITEM parent = GetParentItem(hI);
				while(parent)
				{
					int i1,i2;
					GetItemImage(parent,i1,i2);
					if (i1 != 6)
						path = GetItemText(parent) + "/" + path; 
					parent = GetParentItem(parent);
				} 
				path = "../Data/LScripts/templates/" + path;
				CString strPath = CString("wscite/scite.exe ") + path + name;
				((CPainEditorApp*)AfxGetApp())->ExecuteProgram(strPath,NULL);
			}
			break;
		};     
	};
	return CXTTreeCtrl::OnCommand(wParam, lParam);
}

#endif