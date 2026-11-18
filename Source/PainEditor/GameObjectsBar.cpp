// GameObjectsBar.cpp : implementation file
//

#include "stdafx.h"
#include "GameObjectsBar.h"
#include "PainEditor.h"
#include "MainFrm.h"
#include "game/Main.h" 
#include "PropertiesBar.h" 
#include "TemplatesBar.h" 

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameObjectsBar

#ifdef EDITOR

CGameObjectsBar::CGameObjectsBar()
{
}

CGameObjectsBar::~CGameObjectsBar()
{
}

IMPLEMENT_DYNAMIC(CGameObjectsBar, CWnd)

BEGIN_MESSAGE_MAP(CGameObjectsBar, CWnd)
	//{{AFX_MSG_MAP(CGameObjectsBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
    ON_NOTIFY(TVN_BEGINLABELEDIT,ID_GOBJECTS_TREE, OnBeginlabeledit)
    ON_NOTIFY(TVN_ENDLABELEDIT,ID_GOBJECTS_TREE, OnEndlabeledit)
    ON_NOTIFY(TVN_SELCHANGED,ID_GOBJECTS_TREE, OnSelchanged)
    ON_NOTIFY(NM_DBLCLK,ID_GOBJECTS_TREE, OnDblclkObjtree)
	ON_NOTIFY(NM_RCLICK,ID_GOBJECTS_TREE,OnRclickObjtree)
    ON_NOTIFY(NM_CLICK,ID_GOBJECTS_TREE,OnLclickObjtree)
	ON_COMMAND_RANGE(ID_LEVEL_SAVEOBJECT,ID_LEVEL_NEWGAMEOBJECT,OnToolbarCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameObjectsBar message handlers

/*
BOOL CGameObjectsBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

*/

int CGameObjectsBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(m_wndToolBar.CreateToolBar(WS_TABSTOP|WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));	
	VERIFY(m_wndToolBar.LoadToolBar(IDR_LEVEL));


	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_TREE, 16, 1, RGB(0x00,0x80,0x80)))
		return -1;

	if (!m_wndObjectsView.Create( WS_VISIBLE | WS_CHILD| TVS_EDITLABELS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		CRect(0,0,0,0), this, ID_GOBJECTS_TREE))
	{
		TRACE0( "Unable to create tree control.\n" );
		return false;
	}

	m_wndObjectsView.SetParent(this);


	// Set the image list for the tree control.
	m_wndObjectsView.SetImageList( &m_imageList, TVSIL_NORMAL );	
	return 0;
}


CGameObjectsBar& CGameObjectsBar::Get()
{
	static CGameObjectsBar theTree;
	return theTree;
}


BOOL CGameObjectsBar::Create(CWnd* pParentWnd)
{
	if (::IsWindow(m_hWnd))
	{
		SetParent(pParentWnd);
	}
	else if (!CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), pParentWnd, 0))
	{
		TRACE0( "Unable to create window.\n" );
		return false;
	}

	return true;
}


HTREEITEM CGameObjectsBar::FindItemOK(CString name)
{
	HTREEITEM hti = m_wndObjectsView.FindItem(name,false,true,true,m_wndObjectsView.GetRootItem());
    while(hti)
    {
        if (m_wndObjectsView.GetItemText(hti) == name)
        {
            m_wndObjectsView.SelectItem(hti);
            return hti;
        }
		hti = ((CTreeCtrl*)&m_wndObjectsView)->GetNextItem(hti,TVGN_NEXT);
        if (!hti)
            break;
        hti = m_wndObjectsView.FindItem(name,false,true,true,hti);
    }	
    return NULL;
}


void CGameObjectsBar::UpdateView(CString objname,bool refresh)
{
	if (!Game()) return;

	m_wndObjectsView.SetBkColor(RGB(247,251,248));

	HTREEITEM hti = FindItemOK(objname);
	if ((hti && !refresh) || objname == "EdEntity")
	{
        m_wndObjectsView.SelectItem(hti);
        return;
	}	
	
	m_wndObjectsView.SetRedraw(false);
	m_wndObjectsView.DeleteAllItems();
	ScriptObject level = GScript.Globals()["Lev"];
	CString name = level["_Name"].GetString();
	HTREEITEM hti_class = m_wndObjectsView.InsertItem("_CLevel",3,3);
	m_wndObjectsView.InsertItem(name,4,4,hti_class);			
	m_wndObjectsView.Expand (hti_class, TVE_EXPAND);

	HashMap <String,HTREEITEM> Classes;
	
	ScriptObject gameobjects = GScript.Globals()["GObjects"]["Elements"];			
	for ( ScriptTableIterator it( gameobjects ); it; it.Next() )
	{
		ScriptObject obj = it.GetValue();
		if (obj.IsNil()) continue;
		CString name = obj["_Name"].GetString();
		CString cls = obj["_Class"].GetString();
		HTREEITEM *hti = Classes.Find((LPCSTR)cls);
		if (hti)
			hti_class = *hti;
		else
		{
			hti_class = m_wndObjectsView.InsertItem(cls,3,3,TVI_ROOT,TVI_SORT);
			Classes[(LPCSTR)cls] = hti_class;
		}
		m_wndObjectsView.InsertItem(name,4,4,hti_class,TVI_SORT);						
	}

	m_wndObjectsView.SetRedraw();	
	m_wndObjectsView.SelectItem(FindItemOK(objname));
} 

bool ObjectHasFiles(CString oname,CString oclass)
{
    bool ret = true;
    CString LevelName = GScript.Globals()["Lev"]["_Name"].GetString();
    CString fileName = "../data/levels/" + LevelName + "/" + oclass + "/" + oname + "." +oclass;
    FILE *f = fopen(fileName,"rb");
    if (!f)
        ret = false;
    else
        fclose(f);
    return ret;
}


BOOL CGameObjectsBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	HTREEITEM hI = m_wndObjectsView.GetSelectedItem();
	if (hI)
	{
		CString name  = m_wndObjectsView.GetItemText(hI);
		CString oclass  = m_wndObjectsView.GetItemText(m_wndObjectsView.GetParentItem(hI));
		switch (wParam)
		{
		case PM_Refresh:
			UpdateView(name,true);
			break;
		case PM_LookAt:
			GScript.DoString("Lev:LookAt(%s)",name);
			break;
		case PM_Clone:
			GScript.DoString("Editor:CloneObject(%s)",name);
			//UpdateView("");
			break;
        case PM_CloneInTheSamePlace:
			GScript.DoString("Editor:CloneObject(%s,true)",name);
			//UpdateView("");
			break;
		case PM_Copy:
			CopiedObject = name;
			break;
		case PM_Paste:
			GScript.DoString("Editor:CloneObject(%s)",CopiedObject);
			break;			
		case PM_Reset:
			if (oclass!= "" && oclass!="CPlayer" && oclass!="_CLevel")
			{
			if (ObjectHasFiles(name,oclass))
			{
				GScript.DoString("GObjects:ResetObject(%s);Editor.SelObj=%s",name,name);
                pWnd->m_PropertiesBar.UpdateView(name);
			}
			else
				MessageBox("Object is not saved!");
			}
			break;
		case PM_ExecStartCommand:
			GScript.DoString("o=%s;dostring(o.StartCommand);o=nil",name);
			break;
		case PM_Delete:
			{
				HTREEITEM hNext = ((CTreeCtrl*)&m_wndObjectsView)->GetNextItem(hI,TVGN_PREVIOUS);
				if (!hNext) hNext = ((CTreeCtrl*)&m_wndObjectsView)->GetNextItem(hI,TVGN_NEXT);				
				CString fname;
				if (hNext) 
					fname = m_wndObjectsView.GetItemText(hNext);

				GScript.DoString("GObjects:ToKill(%s)",name);
				GScript.DoString("GObjects:DeleteKilled()");
				GScript.DoString("Editor:SelectObject(nil,true)");		
				if (ObjectHasFiles(name,oclass) && MessageBox("Remove object files from the level?",name,MB_YESNO) == IDYES)
				{
					CreateDirectory("../Data/Levels/_Trash_",NULL);
				
					CString LevelName = GScript.Globals()["Lev"]["_Name"].GetString();
					CString mask;
					mask.Format("../Data/Levels/%s/%s/%s.*",LevelName,oclass,name);
					
					CFileFind fFile;
					BOOL is = fFile.FindFile(mask);
					while(is)
					{
						is = fFile.FindNextFile();
						CString src,dst;

						src = fFile.GetFilePath();
						dst.Format("../Data/Levels/_Trash_/%s",fFile.GetFileName());

						DeleteFile(dst);
						MoveFile(src,dst);
					}
				}
				UpdateView("",true);				
				hNext = m_wndObjectsView.FindItem(fname,false,true,true,m_wndObjectsView.GetRootItem());
				if (hNext)
					m_wndObjectsView.SelectItem(hNext);					
				else
					m_wndObjectsView.Expand(m_wndObjectsView.FindItem(oclass,false,true,true,m_wndObjectsView.GetRootItem()),TVE_EXPAND);
			}
			break;
		case PM_EditScript:
			{
				CString strPath = GScript.Globals()["Lev"]["_Name"].GetString();			
                strPath = CString("wscite\\scite.exe ") + "../Data/Levels/" + strPath + "/" + oclass + "/" + name + ".lua";
				((CPainEditorApp*)AfxGetApp())->ExecuteProgram(strPath,NULL);
			}
			break;
		
		case PM_Save:
			{
			if (oclass!= "" && oclass!="CPlayer")
			{
				CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
				if (strLevName == "NoName")
					MessageBox("You must first create level.");
				else
                    GScript.DoString("GObjects:SaveToCurrentLevel(%s,true)",pWnd->m_PropertiesBar.m_ObjName);
			}
			}
			break;		
		case PM_SaveAllClass:
			GScript.DoString("GObjects:Save('%s')",name);
			break;
		case PM_SaveAsTemplate:
		case PM_SaveAsLevelTemplate:
			{
				if (oclass!= "" && oclass!="CPlayer")
				{					
					CString szFilters = "Object File (*."+oclass+")|*."+oclass;
					CFileDialog fileDlg (FALSE, oclass, "*."+oclass,
						OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR|OFN_EXPLORER, szFilters, this);

					fileDlg.m_pOFN->lpstrInitialDir = "..\\Data\\LScripts\\Templates";
					
					CString LevelName = GScript.Globals()["Lev"]["_Name"].GetString();
					if (wParam == PM_SaveAsLevelTemplate)
					{
						char buf[MAX_PATH];
						sprintf(buf,"..\\Data\\Levels\\%s\\Templates",LevelName);
						CreateDirectory(buf,NULL);
						fileDlg.m_pOFN->lpstrInitialDir = buf;
					}

					if( fileDlg.DoModal () == IDOK )
					{
						bool tml = !GScript.Globals()["Templates"][fileDlg.GetFileName()].IsNil();
						if ( !tml || MessageBox("This template name is already used.\nDo you want to save?","Warning",MB_YESNO) == IDYES)
						{
							// jesli save'wowany obiekt ma baseobj jak nazwa pliku
							// to zamieniam go na baseobj z biezacega templata							
							LPCSTR baseobj = GScript.Globals()[name]["BaseObj"].GetString();
							if (tml && stricmp(fileDlg.GetFileName(),baseobj) == 0)
							{
								LPCSTR tbaseobj = GScript.Globals()["Templates"][fileDlg.GetFileName()]["BaseObj"].GetString();
								GScript.DoString("%s.BaseObj = [[%s]]",name, tbaseobj);
							}

							String title = fileDlg.GetFileTitle();
							title = String::StripExtension(title).AsChar();

							GScript.DoString("%s._Name = [[%s]]",name, title.AsChar());
							GScript.DoString("SaveObj([[%s]],%s,true)",(LPCSTR)fileDlg.GetPathName(),(LPCSTR)name);
							GScript.DoString("%s._Name = [[%s]]",name, name);
							GScript.DoString("%s.BaseObj = [[%s]]",name, baseobj);

                            //GScript.DoString("Game:ReloadTemplates()");
							GScript.DoString("LoadTML([[%s]],[[%s]])",String::GetPath((LPCSTR)fileDlg.GetPathName()).AsChar(),fileDlg.GetFileName());
                            CTemplatesBar::Get().UpdateView("../Data/LScripts/Templates");
						}
					}

				}
			}
			break;

		case PM_Properties:
            pWnd->m_PropertiesBar.UpdateView(name);
			CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
			pWnd->GetDockingPaneManager()->ShowPane(pWnd->GetDockingPaneManager()->FindPane(ID_VIEW_PROPERTIES));
			break;
		};     
	}
    else
    {
		if (wParam == PM_Refresh)
			UpdateView("",true);

        // savowanie zaznaczonego entity
        if (wParam == PM_Save)
        {
            CString strLevName = GScript.Globals()["Lev"]["_Name"].GetString();			
            if (strLevName == "NoName")
                MessageBox("You must first create level.");
            else
            {
                ScriptObject EdEntity = GScript.Globals()["EdEntity"];
                if (!EdEntity.IsNil() && !EdEntity["_MapEntity"].IsNil())        
                    GScript.DoString("GObjects:SaveToCurrentLevel(EdEntity,true)");
            }
        }
    }

	return CWnd::OnCommand(wParam, lParam);
}

void CGameObjectsBar::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	*pResult = 0;


	HTREEITEM parent = m_wndObjectsView.GetParentItem(pTVDispInfo->item.hItem);
	if (parent == NULL) return;
	
	CString nname = pTVDispInfo->item.pszText;    
	CString oclass = m_wndObjectsView.GetItemText(parent);
	if (oclass == "_CLevel" || oclass == "CPlayer" || nname == "" || nname[0] < 'A' || nname[0] > 'z')
		return;

	// TODO: Add your control notification handler code here
	if (pTVDispInfo->item.pszText && strlen(pTVDispInfo->item.pszText)>0)
	{
		if (GScript.Globals()[pTVDispInfo->item.pszText].IsNil())
		{
			CString oldname = m_wndObjectsView.GetItemText(pTVDispInfo->item.hItem);
			GScript.DoString("GObjects:Rename(%s,'%s')",oldname,pTVDispInfo->item.pszText);
			m_wndObjectsView.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);			

			if (ObjectHasFiles(oldname,oclass))
			{
				CreateDirectory("../Data/Levels/_Trash_",NULL);
				CString LevelName = GScript.Globals()["Lev"]["_Name"].GetString();				
				DeleteFile("../Data/Levels/_Trash_/"      +oldname+"."+oclass);
				MoveFile("../Data/Levels/"+LevelName+"/"+oclass+"/"+oldname+"."+oclass,
 						 "../Data/Levels/_Trash_/"      +oldname+"."+oclass);
				GScript.DoString("GObjects:SaveToCurrentLevel(%s,true)",pTVDispInfo->item.pszText);
			}
            CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
            pWnd->m_PropertiesBar.UpdateView(nname);
		}
		else
			MessageBox("This name is already used.");
	}

	m_wndObjectsView.SortChildren(parent);
	
}

void CGameObjectsBar::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	HTREEITEM parent = m_wndObjectsView.GetParentItem(pTVDispInfo->item.hItem);
	if (parent == NULL) {
		// Don't edit
		*pResult = 10;
	}
	else {
		CString oclass = m_wndObjectsView.GetItemText(parent);
		if (oclass == "_CLevel" || oclass == "CPlayer")
			*pResult = 10;
		else
			*pResult = 0;// Allow editing
	}
}



void CGameObjectsBar::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM htItem = m_wndObjectsView.GetSelectedItem();
	if (htItem && m_wndObjectsView.GetParentItem(htItem))
	{
		CString name = m_wndObjectsView.GetItemText(htItem);
		GScript.DoString("Editor:SelectObject(%s,true)",name);		
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
        pWnd->m_PropertiesBar.UpdateView(name);
	}
	*pResult = 0;
}

void CGameObjectsBar::OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint CursorPos;
	GetCursorPos(&CursorPos);
	m_wndObjectsView.ScreenToClient(&CursorPos);

	unsigned int uFlags;
	HTREEITEM hI = m_wndObjectsView.HitTest(CursorPos, &uFlags);

	if (hI && m_wndObjectsView.GetParentItem(hI))
	{
		CString name = m_wndObjectsView.GetItemText(hI);		
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		pWnd->m_PropertiesBar.UpdateView(name);		
		pWnd->GetDockingPaneManager()->ShowPane(pWnd->GetDockingPaneManager()->FindPane(ID_VIEW_PROPERTIES));
	}
	else
		m_wndObjectsView.SelectItem(NULL);

	*pResult = 0;
}
void CGameObjectsBar::OnLclickObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint CursorPos;
	GetCursorPos(&CursorPos);
	m_wndObjectsView.ScreenToClient(&CursorPos);

	unsigned int uFlags;
	HTREEITEM hI = m_wndObjectsView.HitTest(CursorPos, &uFlags);

	if (hI && m_wndObjectsView.GetParentItem(hI))
		OnSelchanged(pNMHDR,pResult);
	else
		m_wndObjectsView.SelectItem(hI);
}

void CGameObjectsBar::OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint CursorPos;
	GetCursorPos(&CursorPos);
	m_wndObjectsView.ScreenToClient(&CursorPos);
	  
	CMenu popMenu;
	popMenu.CreatePopupMenu();	  
	popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Refresh, "Refresh Tree");
		
	unsigned int uFlags;
	HTREEITEM hI = m_wndObjectsView.HitTest(CursorPos, &uFlags);
	HTREEITEM hParent = m_wndObjectsView.GetParentItem(hI);
	if (hParent != NULL)
	{
		m_wndObjectsView.Select(hI,TVGN_CARET);
		popMenu.AppendMenu(MF_SEPARATOR);
		CString oclass = m_wndObjectsView.GetItemText(hParent);

		if (oclass!="_CLevel" && oclass!="CPlayer")
		{
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_LookAt,"Look At");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Clone, "Clone");
            popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_CloneInTheSamePlace, "Clone In The Same Place");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Copy, "Copy");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Paste, "Paste");
			popMenu.AppendMenu(MF_SEPARATOR);
		}

		if (oclass!="CPlayer")
		{
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Reset, "Reset");
			///popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_ExecStartCommand, "Exec Start Command");
			popMenu.AppendMenu(MF_SEPARATOR);
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Save, "Save");
		}
		
		if (oclass!="_CLevel" && oclass!="CPlayer")
		{
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_SaveAsTemplate, "Save As Global Template");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_SaveAsLevelTemplate, "Save As Level Template");
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Delete, "Delete");
		}
		
		if (oclass!="CPlayer")
		{
			popMenu.AppendMenu(MF_SEPARATOR);
			popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_EditScript, "Edit Script");
		}
		popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Properties, "Properties");
		popMenu.SetDefaultItem(popMenu.GetMenuItemCount()-1,true);
	}
	else
	{
		if (hI)
		{
			CString oclass = m_wndObjectsView.GetItemText(hI);
			if (oclass != "_CLevel" && oclass != "CPlayer" )
				popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_SaveAllClass, "Save All " + oclass + "s");
		}
	}

	POINT pt;  
	GetCursorPos(&pt);
	popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
	*pResult = 0;
}

void CGameObjectsBar::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	int nTop = 0;

	if (m_wndToolBar.GetSafeHwnd())
	{		
		CSize sz = m_wndToolBar.CalcDockingLayout(cx, /*LM_HIDEWRAP|*/ LM_HORZDOCK|LM_HORZ | LM_COMMIT);

		m_wndToolBar.MoveWindow(0, nTop, cx, sz.cy);
		m_wndToolBar.Invalidate(FALSE);
		nTop += sz.cy;
	}

	if (m_wndObjectsView.GetSafeHwnd())
	{	
		m_wndObjectsView.MoveWindow(0, nTop, cx, cy - nTop);
		m_wndObjectsView.Invalidate(FALSE);
	}	
}

void CGameObjectsBar::OnToolbarCommand(UINT nID)
{
	switch (nID)
	{
	case ID_LEVEL_SAVEOBJECT:
		OnCommand(PM_Save,0);
		break;
	case ID_LEVEL_REFRESHOBJECT:
		OnCommand(PM_Refresh,0);
		break;
	case ID_LEVEL_RESETOBJECT:
		OnCommand(PM_Reset,0);
		break;
	case ID_LEVEL_NEWGAMEOBJECT:
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		pWnd->OnObjectsNewobject();
		break;
	}
}
#endif