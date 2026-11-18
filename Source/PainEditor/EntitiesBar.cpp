// EntitiesBar.cpp : implementation file
//

#include "stdafx.h"
#include "EntitiesBar.h"
#include "PainEditor.h"
#include "MainFrm.h"
#include "game/Main.h" 
#include "PropertiesBar.h" 
#include "GameObjectsBar.h" 

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Engine/Mesh.h"
#include "Engine/Portal.h"
#include "Engine/PCFSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntitiesBar

#ifdef EDITOR

CEntitiesBar::CEntitiesBar()
{
}

CEntitiesBar::~CEntitiesBar()
{
}

IMPLEMENT_DYNAMIC(CEntitiesBar, CXTTreeCtrl)

BEGIN_MESSAGE_MAP(CEntitiesBar, CXTTreeCtrl)
	//{{AFX_MSG_MAP(CEntitiesBar)
	ON_WM_CREATE()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclkObjtree)
    ON_NOTIFY_REFLECT(NM_RCLICK,OnRclickObjtree)
    ON_NOTIFY_REFLECT(NM_CLICK,OnLclickObjtree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntitiesBar message handlers

BOOL CEntitiesBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CXTTreeCtrl::PreCreateWindow( cs ) )
		return FALSE;

	// Set the style for the tree control.
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS;

	// TODO: Add your specialized code here and/or call the base class
	
	return TRUE;
}

int CEntitiesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// load the tree images bitmap and add it to the image list.
	if (!m_imageList.Create(IDB_TREE, 16, 1, RGB(0x00,0x80,0x80)))
		return -1;

	// Set the image list for the tree control.
	SetImageList( &m_imageList, TVSIL_NORMAL );	

    // Initialize the view.
    UpdateView("");

    EnableMultiSelect(false);

	return 0;
}


CEntitiesBar& CEntitiesBar::Get()
{
	static CEntitiesBar theTree;
	return theTree;
}

BOOL CEntitiesBar::Create(CWnd* pParentWnd)
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


void CEntitiesBar::UpdateView(CString objname,bool refresh)
{
	SetBkColor(RGB(247,251,248));

	if (!Game()) return;

    HTREEITEM hti = FindItem(objname,false,true,true,GetRootItem());
    if (hti && !refresh)
    {
        SelectItem(hti);
        return;
    }	

    SetRedraw(false);
    DeleteAllItems();

    Entity* next;
#ifdef TRANSGAMING
	for (Entity* entity = GEngine->pcfWorld->EntityList; entity; entity=next)
#else
    for (Entity* entity = GEngine->World->EntityList; entity; entity=next)
#endif
    {
        next = entity->Next;

        CString name = entity->Name.AsChar() + CString(" : ") + entity->FileName.AsChar();
        CString cls;
        cls.Format("%d",entity->Type);

        switch (entity->Type)
        {
        case Entity::ETypeNone:       cls = "None"; break;
        case Entity::ETypeMesh:      
            cls = "EMesh"; 
            if (entity->AsMesh()->FixedMeshIndex != 65535) cls = "EMeshFixed"; 
            if (entity->AsMesh()->PO) cls = "EMeshPhysical"; 
            break;
        case Entity::ETypeLight:      cls = "Light"; break;
        case Entity::ETypeParticleFx: cls = "ParticleFX"; break;
        case Entity::ETypeModel:      cls = "Model"; break;
        case Entity::ETypeRegion:     cls = "Region"; break;
        case Entity::ETypeDecal:      cls = "Decal"; break;
        case Entity::ETypeVolume:     cls = "EVolumetric"; break;
        case Entity::ETypeBillboard:  cls = "Billboard"; break;
        case Entity::ETypeEnv:        cls = "Environment"; break;
        case Entity::ETypeTrail:      cls = "Trail"; break;
        case Entity::ETypeSound:      cls = "Sound"; break;
        };

        HTREEITEM hti_class = FindItem(cls,false,true,false,GetRootItem());
        if (!hti_class)
            hti_class = InsertItem(cls,3,3,TVI_ROOT,TVI_SORT);

        HTREEITEM hti = InsertItem(name,4,4,hti_class,TVI_SORT);						
        SetItemData(hti,(DWORD)entity);
    }

#ifdef TRANSGAMING
	if (GEngine->pcfWorld->DeathZones.Size() > 0)
#else
    if (GEngine->World->DeathZones.Size() > 0)
#endif
	{
        HTREEITEM hti_class = InsertItem("DeathZone",3,3,TVI_ROOT,TVI_SORT);
#ifdef TRANSGAMING
		for( int i=0; i<GEngine->pcfWorld->DeathZones.Size(); ++i ) 
        {
            CString str = GEngine->pcfWorld->DeathZones[i]->Name.AsChar();
            if (GEngine->pcfWorld->DeathZones[i]->Enabled)
#else
        for( int i=0; i<GEngine->World->DeathZones.Size(); ++i ) 
        {
            CString str = GEngine->World->DeathZones[i]->Name.AsChar();
            if (GEngine->World->DeathZones[i]->Enabled)
#endif
                str += " (Enabled)";
            else
                str += " (Disabled)";
            HTREEITEM hti = InsertItem(str,4,4,hti_class,TVI_SORT);
#ifdef TRANSGAMING
			SetItemData(hti,(DWORD)GEngine->pcfWorld->DeathZones[i]);
#else
            SetItemData(hti,(DWORD)GEngine->World->DeathZones[i]);
#endif
        }
    }
#ifdef TRANSGAMING
	if (GEngine->pcfWorld->Portals.Size() > 0)
#else
	if (GEngine->World->Portals.Size() > 0)
#endif
    {
        HTREEITEM hti_class = InsertItem("Portals",3,3,TVI_ROOT,TVI_SORT);
#ifdef TRANSGAMING
		for( int i=0; i<GEngine->pcfWorld->Portals.Size(); ++i ) 
        {
            CString str = GEngine->pcfWorld->Portals[i]->Name.AsChar();
            if (GEngine->pcfWorld->Portals[i]->IsDisabled)
#else
		for( int i=0; i<GEngine->World->Portals.Size(); ++i ) 
        {
            CString str = GEngine->World->Portals[i]->Name.AsChar();
            if (GEngine->World->Portals[i]->IsDisabled)
#endif
                str += " (Disabled)";
            else
                str += " (Enabled)";
            HTREEITEM hti = InsertItem(str,4,4,hti_class,TVI_SORT);
#ifdef TRANSGAMING
			SetItemData(hti,(DWORD)GEngine->pcfWorld->Portals[i]);
#else
            SetItemData(hti,(DWORD)GEngine->World->Portals[i]);
#endif
        }
    }

#ifdef TRANSGAMING
	if (GEngine->pcfWorld->AntiPortals.Size() > 0)
#else
    if (GEngine->World->AntiPortals.Size() > 0)
#endif
    {
        HTREEITEM hti_class = InsertItem("AntiPortals",3,3,TVI_ROOT,TVI_SORT);
#ifdef TRANSGAMING
        for( int i=0; i<GEngine->pcfWorld->AntiPortals.Size(); ++i ) 
        {
            CString str = GEngine->pcfWorld->AntiPortals[i]->Name.AsChar();
            if (GEngine->pcfWorld->AntiPortals[i]->Disabled)
#else
        for( int i=0; i<GEngine->World->AntiPortals.Size(); ++i ) 
        {
            CString str = GEngine->World->AntiPortals[i]->Name.AsChar();
            if (GEngine->World->AntiPortals[i]->Disabled)
#endif
                str += " (Disabled)";
            else
                str += " (Enabled)";
            HTREEITEM hti = InsertItem(str,4,4,hti_class,TVI_SORT);
#ifdef TRANSGAMING
			SetItemData(hti,(DWORD)GEngine->pcfWorld->AntiPortals[i]);
#else
            SetItemData(hti,(DWORD)GEngine->World->AntiPortals[i]);
#endif
        }
    }

    if (GEngine->Video && GEngine->Video->MaterialSys)
    {
        HTREEITEM hti_class = InsertItem("Textures",3,3,TVI_ROOT,TVI_SORT);
        DynamicArray<String> info;
        GEngine->Video->MaterialSys->EnumerateTextures(info);

        for( int i=0; i<info.Size(); ++i ) 
        {
            CString str = info[i].AsChar();
            HTREEITEM hti = InsertItem(str,4,4,hti_class,TVI_SORT);						
            SetItemData(hti,(DWORD)0);
        }
    }


    SetRedraw();
    SelectItem(FindItem(objname,false,true,true,GetRootItem()));

    return;

    SetRedraw(false);
    DeleteAllItems();
    ScriptObject level = GScript.Globals()["Lev"];
    CString name = level["_Name"].GetString();
    HTREEITEM hti_class = InsertItem("_CLevel",3,3);
    InsertItem(name,4,4,hti_class);			
    Expand (hti_class, TVE_EXPAND);

    ScriptObject gameobjects = GScript.Globals()["GObjects"]["Elements"];
    if (gameobjects.IsTable())
    {
        int n = gameobjects.GetCount();
        for (int i=1;i<=n;i++)
        {
            ScriptObject key = gameobjects[i];
            if (key.IsNil()) break;
            CString name = key["_Name"].GetString();
            CString cls = key["_Class"].GetString();
            HTREEITEM hti_class = FindItem(cls,false,true,false,GetRootItem());
            if (!hti_class)
                hti_class = InsertItem(cls,3,3,TVI_ROOT,TVI_SORT);
            InsertItem(name,4,4,hti_class,TVI_SORT);						
            //if (cls == "CPlayer")
            //	Expand (hti_class, TVE_EXPAND);
        }		
    }

    SetRedraw();

} 



BOOL CEntitiesBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
    HTREEITEM hI = GetSelectedItem();
	
	if (wParam == PM_Refresh)
	{
		UpdateView("",true);
		return CXTTreeCtrl::OnCommand(wParam, lParam);
	}

    if (hI)
    {
        CString name  = GetItemText(hI);
        CString oclass  = GetItemText(GetParentItem(hI));
		switch (wParam)
		{
			case PM_LookAt:
            if (oclass == "DeathZone")
            {
                Zone * z = (Zone *) GetItemData(hI);
                Vector v = z->BBox.GetCenter();
                GScript.Call("LookAt",0,"%f,%f,%f",v.X,v.Y,v.Z);
            }
            else if (oclass == "Portals")
            {
                Portal * p = (Portal *) GetItemData(hI);
                Vector v = p->Box.GetCenter();
                GScript.Call("LookAt",0,"%f,%f,%f",v.X,v.Y,v.Z);
            }
            else if (oclass == "AntiPortals")
            {
                AntiPortal * p = (AntiPortal *) GetItemData(hI);
                Vector v = p->Box.GetCenter();
                GScript.Call("LookAt",0,"%f,%f,%f",v.X,v.Y,v.Z);
            }
            else
                GScript.Call("LookAtEntity",0,"%e",GetItemData(hI));
            break;
        case PM_Reset:
            if (oclass!= "" && oclass!="CPlayer" && oclass!="_CLevel")
            {
                //if (ObjectHasFiles(name,oclass))
                //{
                //	GScript.DoString("Entities:ResetObject(%s);Editor.SelObj=%s",name,name);
                //	pWnd->m_wndLevelExplorer.m_pPropertiesView->UpdateView(name);
                //}
                //else
                //	MessageBox("Object is not saved!");
            }
            break;

        case PM_Save:
            {
                CGameObjectsBar::Get().OnCommand(wParam,lParam);
            }
            break;		
        case PM_Properties:            
			CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
			pWnd->m_PropertiesBar.UpdateView("EdEntity");
			pWnd->GetDockingPaneManager()->ShowPane(pWnd->GetDockingPaneManager()->FindPane(ID_VIEW_PROPERTIES));
            break;
        };     
    };
	return CXTTreeCtrl::OnCommand(wParam, lParam);
}



void CEntitiesBar::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    HTREEITEM htItem = GetSelectedItem();
    if (htItem && GetParentItem(htItem))
    {
        HTREEITEM htItemParent = GetParentItem(htItem);
        CString name = GetItemText(htItem);
        //GScript.DoString("Editor:SelectObject(%s,true)",name);		
        void *e = (void*)GetItemData(htItem);
        GScript.Call("Editor_EditEntity",0,"%e,%s",e,GetItemText(htItemParent));
        CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
        pWnd->m_PropertiesBar.UpdateView("EdEntity");
    }
    *pResult = 0;
}

void CEntitiesBar::OnDblclkObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CPoint CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(&CursorPos);

    unsigned int uFlags;
    HTREEITEM hI = HitTest(CursorPos, &uFlags);

    if (hI && GetParentItem(hI))
    {
        OnSelchanged(pNMHDR,pResult);
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		pWnd->GetDockingPaneManager()->ShowPane(pWnd->GetDockingPaneManager()->FindPane(ID_VIEW_PROPERTIES));
    }
    else
        SelectItem(NULL);

    *pResult = 0;
}
void CEntitiesBar::OnLclickObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CPoint CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(&CursorPos);

    unsigned int uFlags;
    HTREEITEM hI = HitTest(CursorPos, &uFlags);

    if (hI && GetParentItem(hI))
        OnSelchanged(pNMHDR,pResult);
    else
        SelectItem(hI);
}

void CEntitiesBar::OnRclickObjtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CPoint CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(&CursorPos);

    OnLclickObjtree(pNMHDR,pResult);

    CMenu popMenu;
    popMenu.CreatePopupMenu();	  
    popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Refresh, "Refresh Tree");

    unsigned int uFlags;
    HTREEITEM hI = HitTest(CursorPos, &uFlags);
    HTREEITEM hParent = GetParentItem(hI);
    if (hParent != NULL)
    {
        Select(hI,TVGN_CARET);
        popMenu.AppendMenu(MF_SEPARATOR);
        CString oclass = GetItemText(hParent);

        popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_LookAt,"Look At");
        popMenu.AppendMenu(MF_SEPARATOR);

        popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Reset, "Reset");
        ///popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_ExecStartCommand, "Exec Start Command");
        popMenu.AppendMenu(MF_SEPARATOR);
        popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Save, "Save");

        popMenu.AppendMenu(MF_STRING | MF_ENABLED, PM_Properties, "Properties");
        popMenu.SetDefaultItem(popMenu.GetMenuItemCount()-1,true);
    }

    POINT pt;  
    GetCursorPos(&pt);
    popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);
    *pResult = 0;
}

#endif