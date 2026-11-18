#include "StdAfx.h"
#include "PainEditor.h"
#include "WatchBar.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/LeString.h"

void MyExpandFunction(CNewTreeListCtrl *treelistctl,HTREEITEM item);
void FillTreeItem(CNewTreeListCtrl *treelistctl,HTREEITEM hti,ScriptObject obj);
/////////////////////////////////////////////////////////////////////////////
// CWatchBar

BEGIN_MESSAGE_MAP(CWatchBar, CXTFlatTabCtrl)
	//{{AFX_MSG_MAP(CWatchBar)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTabSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchBar message handlers


int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTFlatTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;



	
	CTLFrame::RegisterClass();
	CreateTabListCtrl(m_sheet1, IDC_SHEET1);
	CreateTabListCtrl(m_sheet2, IDC_SHEET2);
	CreateTabListCtrl(m_sheet3, IDC_SHEET3);
	CreateTabListCtrl(m_sheet4, IDC_SHEET4);

	InsertItem(0, "Main", &m_sheet1);
	InsertItem(1, "Globals", &m_sheet2);
	InsertItem(2, "Locals", &m_sheet3);
	InsertItem(3, "Watch", &m_sheet4);

	// Insert text into the list control.
	HTREEITEM hti = AddWatchVar(m_sheet1, _T("EntityToObject"), _T("{...}"));
	AddWatchVar(m_sheet1,"","",hti);	

	hti = AddWatchVar(m_sheet1, _T("Game"), _T("{...}"));
	AddWatchVar(m_sheet1,"","",hti);	

	hti = AddWatchVar(m_sheet1, _T("GObjects"), _T("{Collection}"));
	AddWatchVar(m_sheet1,"","",hti);	

	hti = AddWatchVar(m_sheet1, _T("Lev"), _T("{CLevel}"));
	AddWatchVar(m_sheet1,"","",hti);	

	hti = AddWatchVar(m_sheet1, _T("Player"), _T("{CActor}"));
	AddWatchVar(m_sheet1,"","",hti);	

	SetCurSel(0);

	return 0;
}


BOOL CWatchBar::CreateTabListCtrl(CTLFrame& listCtrl, UINT nID)
{
	CRect rc;
//	GetInsideRect(rc);
//	rc.DeflateRect(1,1);
	static char buf[100];
	sprintf(buf,"Sheet %d",nID);
		
	listCtrl.Create("LANTIVTREELISTCTRL", buf, WS_CHILD,rc, this, nID);
//	listCtrl.SetFont(&m_sheetFont);	

	listCtrl.Initialize();
    listCtrl.m_tree.UpdateData();

	listCtrl.m_tree.InsertColumn(0, _T("Name"),  LVCFMT_LEFT, 150);
	listCtrl.m_tree.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 300);
	listCtrl.m_tree.ExpandFunctionPtr = MyExpandFunction;

	//ListView_SetExtendedListViewStyle(listCtrl.m_tree.m_hWnd, LVS_EX_GRIDLINES);

	
	//listCtrl.m_tree.GetDlgItem(0)->SetFont(&xtAfxData.fontBold);
	return TRUE;
}

HTREEITEM CWatchBar::AddWatchVar(CTLFrame& listCtrl, LPCTSTR pszName, LPCTSTR pszVal,HTREEITEM hParent)
{
    //int iItem = listCtrl.m_tree.GetItemCount();
    HTREEITEM hti = listCtrl.m_tree.InsertItem(pszName,0,0,hParent);
	listCtrl.m_tree.SetItemText(hti,1,pszVal);
	return hti;
}


void CWatchBar::OnTabSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int t = GetCurSel();
	if (t == 1)
	{
		m_sheet2.m_tree.DeleteAllItems();
		FillTreeItem(&m_sheet2.m_tree,m_sheet2.m_tree.GetRootItem(),GScript.Globals());
		m_sheet2.m_tree.SortItems(0, true, NULL);
		m_sheet2.m_tree.UpdateWindow();
	}
}


void FillTreeItem(CNewTreeListCtrl *treelistctl,HTREEITEM hti,ScriptObject object)
{
	HTREEITEM hti_functions=NULL;

	if (!object.IsTable())
		return;

	for ( ScriptTableIterator it( object ); it; it.Next() )
	{		
		
		int key_type = it.GetKey().GetType();			
		if (key_type == LUA_TLIGHTUSERDATA)
		{
			treelistctl->InsertItem("ptr","???",hti);
			continue;
		};

		ScriptObject obj = it.GetValue();

		CString key;
		if (key_type == LUA_TNUMBER)
		{
			char buf[30];
			sprintf(buf,"%d",it.GetKey().GetInt());
			key = buf;
		}
		else
			key = it.GetKey().GetString();

		
		int type = obj.GetType();
		if (type == LUA_TNUMBER)
		{
			char buf[30];
			sprintf(buf,"%f",obj.GetFloat());
			treelistctl->InsertItem(key,buf,hti);
		}
		else if (type == LUA_TBOOLEAN)
			treelistctl->InsertItem(key,obj.GetBool()?"true":"false",hti);
		else if (type == LUA_TSTRING)
			treelistctl->InsertItem(key,CString("\"")+ obj.GetString() + CString("\""),hti);
		if (type == LUA_TLIGHTUSERDATA)
			treelistctl->InsertItem(key,"???",hti);
		else if (type == LUA_TTABLE)
		{				
			CString tclassname="...";				
			if (!obj["_Class"].IsNil()) // class
				tclassname = obj["_Class"].GetString();
			
			CString objname="...";				
			if (!obj["_Name"].IsNil()) // name
				objname = obj["_Name"].GetString();

			HTREEITEM ht = treelistctl->InsertItem(key,CString("{")+ tclassname + "-" +objname+ CString("}"),hti);
			treelistctl->InsertItem("","",ht);				
		}
		else if (type == LUA_TFUNCTION)
		{
			if (!hti_functions)
				hti_functions = treelistctl->InsertItem("Functions","{...}",hti);

			treelistctl->InsertItem(key,"function",hti_functions);
		}
	}
	
	
	CTLFrame*  pFrm = (CTLFrame*) treelistctl->GetParent();
	if (hti == NULL)
		hti = treelistctl->GetRootItem();


	pFrm->SortTree(0, TRUE, hti);
}

void MyExpandFunction(CNewTreeListCtrl *treelistctl,HTREEITEM item)
{
	if (strcmp(treelistctl->GetItemText(item),"Functions")==0) // already filled
		return;

	treelistctl->DeleteAllChildItems(item);

	DynamicArray<String> array;
	HTREEITEM parent = item;
	while(parent)
	{
		array.Add(String(treelistctl->GetItemText(parent)));
		parent = treelistctl->GetParentItem(parent);
	}

	ScriptObject obj = GScript.Globals();
	for (int i=array.Size()-1;i>-1;i--)
	{
		LPCSTR key = array[i].AsChar();		
		if (key[0]>='0' && key[0]<='9')
			obj = obj[atoi(key)];	
		else
			obj = obj[key];	
	}
	FillTreeItem(treelistctl,item,obj);
}