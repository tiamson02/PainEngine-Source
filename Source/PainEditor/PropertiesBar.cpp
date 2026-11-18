#include "stdafx.h"

#ifdef EDITOR

#include "MainFrm.h"
#include "PropertiesBar.h"
#include "SetValueDlg.h"
#include "ActionDlg.h"
#include "game/Main.h"
#include "GameObjectsBar.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_PROPERTYTREE 19999

enum PropControls
{
	PC_Text,
	PC_Double,
	PC_Combo,
	PC_Spin,
	PC_File,
	PC_Color,
};

class CCustomItemFileBox : public CXTPPropertyGridItem
{
	CFileDialog *m_pDlg;
public:
	bool m_bCutExtenstion;
	CString m_Path;
	CCustomItemFileBox(CString strCaption,LPCSTR strValue=NULL)
		: CXTPPropertyGridItem(strCaption,strValue)
	{
		m_pDlg = NULL;
		m_nFlags = pgiHasExpandButton|pgiHasEdit;
	}

	~CCustomItemFileBox()
	{
		if (m_pDlg)
			delete m_pDlg;
	}		
	void SetFileDialog(CFileDialog *pDlg)
	{
		m_pDlg = pDlg;
	}

protected:
	void CCustomItemFileBox::OnInplaceButtonDown()
	{	
		if ( m_pDlg->DoModal( ) == IDOK )
		{
			CString sEdit = m_pDlg->GetPathName();

			sEdit.MakeLower();	
			int l = sEdit.Find(m_Path);
			if (l>-1)
				sEdit=sEdit.Mid(l+m_Path.GetLength());

			if (m_bCutExtenstion)
			{
				int i = sEdit.ReverseFind('.');
				if (i>-1)
					sEdit = sEdit.Left(i);
			}
			
			sEdit.Replace('\\','/');

			OnValueChanged( sEdit);
			m_pGrid->Invalidate( FALSE );
		}
	};
};

class CCustomItemData : public CXTPPropertyGridItem
{
public:
	HashMap <String,int> Data;
	CCustomItemData(CString strCaption,LPCSTR strValue=NULL)
		: CXTPPropertyGridItem(strCaption,strValue)
	{
	}

	void CCustomItemData ::SetConstraintData(CString key, int data)
	{	
		Data.Insert((LPCSTR)key,data);
	};
};



class CCustomItemSpin;

class CCustomItemSpinInplaceButton : public CSpinButtonCtrl
{
	friend class CCustomItemSpin;
	CCustomItemSpin* m_pItem;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult);
};

class CCustomItemSpin : public CXTPPropertyGridItemDouble
{

	friend class CCustomItemSpinInplaceButton;
public:
	CString format;
	float minV;
	float maxV;
	float stepV;
	CCustomItemSpin(CString strCaption,double val );


protected:
	virtual void OnDeselect();
	virtual void OnSelect();
	virtual CRect GetValueRect();


private:
	CCustomItemSpinInplaceButton m_wndSpin;
};


class CXTPPropertyGridItemDoubleEx : public CXTPPropertyGridItemDouble
{
public:
		CXTPPropertyGridItemDoubleEx (CString strCaption,double val)
		: CXTPPropertyGridItemDouble(strCaption,val)
		{
			m_strFormat = "%g";
			SetDouble(m_fValue); 
		};
};



////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCustomItemSpinInplaceButton, CSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()


void CCustomItemSpinInplaceButton::OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_pItem->OnValidateEdit();
	double nValue = m_pItem->GetDouble() + (pNMUpDown->iDelta) * m_pItem->stepV;
	if (nValue < m_pItem->minV)
		nValue = m_pItem->minV;
	if (nValue > m_pItem->maxV)
		nValue = m_pItem->maxV;

	CString str;
	str.Format(m_pItem->format, nValue);		
	m_pItem->OnValueChanged(str);

	*pResult = 1;
}

CCustomItemSpin::CCustomItemSpin(CString strCaption,double val)
: CXTPPropertyGridItemDouble(strCaption,val)
{
	m_wndSpin.m_pItem = this;
}
void CCustomItemSpin::OnDeselect()
{
	CXTPPropertyGridItemDouble::OnDeselect();

	if (m_wndSpin.m_hWnd) m_wndSpin.ShowWindow(SW_HIDE);
}

void CCustomItemSpin::OnSelect()
{
	CXTPPropertyGridItem::OnSelect();

	if (!m_bReadOnly)
	{	
		CRect rc = GetItemRect();
		rc.left = rc.right - 15;
		if (!m_wndSpin.m_hWnd)
		{
			m_wndSpin.Create(UDS_ARROWKEYS|WS_CHILD, rc, (CWnd*)m_pGrid, 0);
			m_wndSpin.SetRange(0, 100);
		}
		m_wndSpin.MoveWindow(rc);	
		m_wndSpin.ShowWindow(SW_SHOW);
	}
}
CRect CCustomItemSpin::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.right -= 17;
	return rcValue;
}



/////////////////////////////////////////////////////////////////////////////
// CPropertiesBar

BEGIN_MESSAGE_MAP(CPropertiesBar, CWnd)
	//{{AFX_MSG_MAP(CPropertiesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(ID_LEVEL_SAVEOBJECT,ID_LEVEL_NEWGAMEOBJECT,OnToolbarCommand)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesBar construction/destruction

CPropertiesBar::CPropertiesBar()
{
}

CPropertiesBar::~CPropertiesBar()
{
}

void CPropertiesBar::OnSize(UINT nType, int cx, int cy) 
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

	if (m_wndGrid.GetSafeHwnd())
	{	
		m_wndGrid.MoveWindow(0, nTop, cx, cy - nTop);
		m_wndGrid.Invalidate(FALSE);
	}	
}


/////////////////////////////////////////////////////////////////////////////
// CPropertiesBar message handlers

static int CALLBACK  MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  int ret = 0;  
  CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
  if (!pListCtrl) return true;

  int t1 = (int)lParam1/1000;
  int t2 = (int)lParam2/1000;

  if (t1 < t2) return -1; 
  if (t1 > t2) return 1; 

  int n1 = (int)lParam1 - t1 *1000;
  int n2 = (int)lParam2 - t2 *1000;
  CString str1 = pListCtrl->GetItemText(n1, 0);
  CString str2 = pListCtrl->GetItemText(n2, 0);
    
  return strcmp(str1,str2);
}

CXTPPropertyGridItem* CPropertiesBar::FillTree(ScriptObject obj, CString key,CString fullkey, CXTPPropertyGridItem* pParent, bool group)
{
	int type = obj.GetType();
	CXTPPropertyGridItem* pAddedItem = NULL;

	if (key !="[new]" && type == LUA_TTABLE)
	{
		//if (key[0] == '_' || (key[0] == 's' && key[1] == '_'))
		//	goto END;

        if (!obj.GetMetatable().IsNil())
        {
            goto END;
        }

		CString classname = "";
		if (!obj["_Class"].IsNil())
			classname = obj["_Class"].GetString();
		
		
        if (key.Left(3) == "Obj" || key.Left(4) == "_Obj" || key.Left(2) == "r_") // reference
		{
			CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, "{Reference}"));				
			pAddedItem = pItem;
		}
		else if (classname == "Vector" || classname == "Color")
		{	
			
			if (classname == "Vector")
			{
				CString val;
				float x = obj["X"].GetFloat();
				float y = obj["Y"].GetFloat();
				float z = obj["Z"].GetFloat();
				val.Format("%.4f %.4f %.4f",x,y,z);

				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, val));				
				//pItem->SetMask("X:000 Y:000 Z:000", "X:___ Y:___ Z:___");

				pAddedItem = pItem;
			}					
			if (classname == "Color")
			{
				CString val;
				int r = obj["R"].GetInt();
				int g = obj["G"].GetInt();
				int b = obj["B"].GetInt();
				int a = obj["A"].GetInt();
				val.Format("%d %d %d %d",r,g,b,a);
				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItemColor(key, RGB(r, g, b)));				
				pAddedItem = pItem;
			}				
			
		}		
        else if (classname == "Quaternion")
		{	
			
            CString val;
            float w = obj["W"].GetFloat();
            float x = obj["X"].GetFloat();
            float y = obj["Y"].GetFloat();
            float z = obj["Z"].GetFloat();
            Quaternion q(w,Vector(x,y,z));
            Vector angles;
            q.ToEulerAnglesXYZ(angles);
            val.Format("%.2f %.2f %.2f",FRad2Deg(angles.X),FRad2Deg(angles.Y),FRad2Deg(angles.Z));
			CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, val));				
			pAddedItem = pItem;
        }
		else
		{
			CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, "{...}"));				

			bool addnew = true;
			for ( ScriptTableIterator it( obj ); it; it.Next() )
			{

				CString kname;
				int ktype = it.GetKey().GetType();
				if (ktype == LUA_TNUMBER)
				{
					char buf[20];
					sprintf(buf,"%d",it.GetKey().GetInt()); // because GetString changes type
					kname =	buf;
				}
				else
				{
					kname = it.GetKey().GetString();   
					addnew = false;
				}				
				FillTree(it.GetValue(),kname,fullkey+"."+kname,pItem,false);					
			}

			if (addnew)
			{				
				ScriptObject editControl = GScript.Globals()[m_ObjName]["s_Editor"][fullkey+".[new]"];
				if (!editControl.IsNil())
				{
					FillTree(obj,"[new]",fullkey+".[new]",pItem,false);					
				}
			}
			pAddedItem =  pItem;

			pAddedItem =  pItem;
		}
	}
	else
	{
		if (key[0] == '_' || (key[0] == 's' && key[1] == '_'))
			goto END;

		CString obj_string_value;float obj_number_value;bool  obj_bool_value;

		if (type == LUA_TSTRING) obj_string_value = obj.GetString();
		if (type == LUA_TNUMBER) obj_number_value = obj.GetFloat();
		if (type == LUA_TBOOLEAN) obj_bool_value = obj.GetBool();
	
		ScriptObject editControl = GScript.Globals()[m_ObjName]["s_Editor"][fullkey];
		if (editControl.IsNil()) // moze tablica ma definicje edycji jej elementow
		{
			CString tmp(fullkey);
			tmp.Replace(key,"[new]");
			editControl = GScript.Globals()[m_ObjName]["s_Editor"][tmp];
		}
		if (!editControl.IsNil())
		{
			CString ctype = editControl[1].GetString();
			if (ctype=="TextEdit")
			{
				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, obj_string_value));				
                if (key[0]>='0' && key[0]<='9')
                {
                    CString k;       
                    k.Format("%02d",atoi(key));
					pItem->SetCaption(k);
				}
				pAddedItem =  pItem;
			}
			if (ctype=="BrowseEdit")
			{
				CCustomItemFileBox* pItem = (CCustomItemFileBox*) pParent->AddChildItem(new CCustomItemFileBox(key,obj_string_value));
				CString ext = editControl[2][1].GetString();
				CString path = editControl[2][2].GetString();

				CFileDialog *pDlg = new CFileDialog(TRUE, "*.*", "..\\" + CString(path) + CString(ext),
                    OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST,
                    CString(ext) + "||", this );

				pItem->m_bCutExtenstion = editControl[2][3].GetBool();
				pItem->m_Path = path;
                pItem->SetFileDialog(pDlg);
				pAddedItem =  pItem;				                
			}
			if (ctype=="SpinEdit")
			{
				CCustomItemSpin* pItem = (CCustomItemSpin*) pParent->AddChildItem(new CCustomItemSpin(key,obj_number_value));
				pItem->format = editControl[2][1].GetString();
				pItem->minV = editControl[2][2].GetFloat();
				pItem->maxV = editControl[2][3].GetFloat();
				pItem->stepV = editControl[2][4].GetFloat();
				pAddedItem =  pItem;
			}
			
			if (ctype=="ComboBox")
			{
				CCustomItemData* pItem = (CCustomItemData*) pParent->AddChildItem(new CCustomItemData(key,obj_string_value));
				pItem->SetFlags(pgiHasComboButton);
				CXTPItemConstraints* pList = pItem->GetConstraints();
				
				ScriptObject tab = editControl[2];
				if (tab.IsString())
				{
  					GScript.DoString("%s(%s)",tab.GetString(),m_ObjName);
					tab = GScript.Globals()["tmp_tab"];
				}
				
				bool noValues = !editControl[3].IsNil();

				int count = tab.GetCount();
				for (int i=1;i<=count;i++)
				{

					CString str = tab[i].GetString();
					CString txt = str, val = "-1";

					if (!noValues)
					{
						int sep = str.Find('|');
						if (sep>-1)
						{
							txt = str.Mid(0,str.Find('|'));
							val = str.Mid(str.Find('|')+1);
						}
					}

					int id = pList->AddConstraint(txt);
					if (type == LUA_TNUMBER)
					{
						int data = atoi(val);
						pItem->SetConstraintData(txt,data);
						if (data == obj_number_value)
							pItem->SetValue(txt);
					}
				}
				pList->Sort();

                if (key[0]>='0' && key[0]<='9')
                {
                    CString k;       
                    k.Format("%02d",atoi(key));
                    pItem->SetCaption(k);
                }
				
				pAddedItem =  pItem;
				GScript.DoString("tmp_tab = nil");
			}			

			goto END;
		}

		
		switch (type)
		{	
			
			case LUA_TSTRING:
			{	
				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItem(key, obj_string_value));				
				pAddedItem =  pItem;
			}
			break;

			case LUA_TNUMBER:
			{
				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItemDoubleEx (key, obj.GetFloat()));				
				pAddedItem =  pItem;
			}
			break;

			case LUA_TBOOLEAN:
			{
				CXTPPropertyGridItem* pItem = pParent->AddChildItem(new CXTPPropertyGridItemBool (key, obj.GetBool()));
				pAddedItem =  pItem;
			}
			
		}
		
	}
END:

	if (pAddedItem)
	{
		/* TEGO NIE BYLO
		int t2 = GScript.GetTop();
		LPCSTR basename =  GScript.Globals()[m_ObjName]["BaseObj"].GetString();

		pAddedItem->Color =RGB(0,0,255);

		GScript.DoString("tmpxx = CompareObjToBaseObjVar(%s,'%s','%s')",m_ObjName,basename,fullkey);
		if (GScript.Globals()["tmpxx"].GetBool())
			pAddedItem->Color = -1;

		GScript.SetTop(t2);
		*/

		

		//MessageBox(fullkey);
		/*
		int t2 = GScript.GetTop();
		Script istemp =GScript.Globals()[m_ObjName]["BaseObj"];
		
		pAddedItem->Color =RGB(0,0,255);
		if (!istemp.IsNil())
		{
			CString tmlname = istemp.GetString();
			Script tml = GScript.Globals()["Templates"][tmlname];			
			if (tml.IsNil())
				tml = GScript.Globals()[tmlname];
			if ((!tml.IsNil() && !tml[key].IsNil()))
				pAddedItem->Color = -1;
		}
		GScript.SetTop(t2);
		*/
	}	

	return pAddedItem;
}

void SortTree(CXTPPropertyGridItem *pItem)
{
	
	CXTPPropertyGridItems *pI = pItem->GetChilds();	
	for (int i=0;i<pI->GetCount();i++)
		SortTree(pI->GetAt(i));
	pI->Sort(); //7.8
	
	//pItem->Expand();
}

BOOL CPropertiesBar::Create(CWnd* pParentWnd)
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

int CPropertiesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(m_wndToolBar.CreateToolBar(WS_TABSTOP|WS_VISIBLE|WS_CHILD|CBRS_TOOLTIPS, this));	
	VERIFY(m_wndToolBar.LoadToolBar(IDR_LEVEL));

	CXTPControl *btn1 = m_wndToolBar.GetControl(0);
	btn1->SetVisible(false);
	

	if (!m_wndGrid.Create( CRect(0, 0, 0, 0), this, ID_GOBJECTS_TREE ))
	{
		TRACE0( "Unable to create grid control.\n" );
		return false;
	}

	m_wndGrid.SetParent(this);
	m_wndGrid.SetOwner(this);

	m_wndGrid.ShowHelp(false);
	m_wndGrid.SetCustomColors( RGB(200, 200, 200), 0, RGB(182,190, 189), RGB(247, 243, 233), 0);

	return 0;
}


void CPropertiesBar::UpdateView(CString objname)
{
 	static CString oldname = "";
	if (!Game()) return;

	m_ObjName = objname;

	//int  oldsel = 0;
	//POSITION pos =  m_PropTree.GetFirstSelectedItemPosition();
	//if (pos) oldsel = m_PropTree.GetNextSelectedItem(pos);

	//SetRedraw(false);

	ScriptObject obj = GScript.Globals()[objname];
	//if (obj.IsNil() || oldname != objname)
	
	m_wndGrid.ResetContent();

	CXTPPropertyGridItem* pMain        = m_wndGrid.AddCategory(_T("Main"));
	CXTPPropertyGridItem* pProperties  = m_wndGrid.AddCategory(_T("Properties"));
	CXTPPropertyGridItem* pTemporaries = m_wndGrid.AddCategory(_T("Temporaries"));
	CXTPPropertyGridItem* pFunctions   = m_wndGrid.AddCategory(_T("Functions"));

	if (obj.IsNil())
	{
		m_wndGrid.SetRedraw();
		return;
	}


	if (obj.IsTable())
	{
		for ( ScriptTableIterator it( obj ); it; it.Next() )
		{
			CString key = it.GetKey().GetString();
			
			//LVFINDINFO info;
			//info.flags = LVFI_STRING;
			//info.psz = key;
			//int k =m_PropTree.FindItem(&info);
			//if (k<0)
			//	k = m_PropTree.InsertItem(key);
		    //
			
			
			int vtype = obj.GetType();
			if (key == "_Name" || key == "_Class")	// MAIN
			{
				CXTPPropertyGridItem* pItem = pMain->AddChildItem(new CXTPPropertyGridItem(key, it.GetValue().GetString()));
				pItem->SetReadOnly(true);
			}			
			else if (key[0] == '_' || (key[0] == 's' && key[1] == '_')) //TEMPORARIES
			{
				CXTPPropertyGridItem* pItem = NULL;
				
				if (vtype == LUA_TNUMBER)
					pItem = pTemporaries->AddChildItem(new CXTPPropertyGridItem(key, it.GetValue().GetString()));
				else if (vtype == LUA_TSTRING)
					pItem = pTemporaries->AddChildItem(new CXTPPropertyGridItem(key, it.GetValue().GetString()));
				else if (vtype == LUA_TBOOLEAN)
					pItem = pTemporaries->AddChildItem(new CXTPPropertyGridItem(key, it.GetValue().GetBool() ? "true" : "false"));
				else if (vtype == LUA_TTABLE)
                {					
					FillTree(it.GetValue(), key, key, pTemporaries, false);
                }
				else
					pItem = pTemporaries->AddChildItem(new CXTPPropertyGridItem(key, "???"));

				if (pItem)
					pItem->SetReadOnly(true);
				
            }
			else if (vtype == LUA_TFUNCTION)
			{
				CXTPPropertyGridItem* pItem = pFunctions->AddChildItem(new CXTPPropertyGridItem(key, "{lua func}"));				
				pItem->SetReadOnly(true);
			}
			else
			{
				FillTree(it.GetValue(), key, key, pProperties, false);				
			};
		}
	}
	
		

	/*
	CXTPPropertyGrid *pI = NULL;
	CXTPPropertyGridItems *pIs = pMain->GetChilds();
	
	CXTPPropertyGridItems *pI = pMain->GetChilds();
	
	while ()
	{

	}
	*/
	pMain->GetChilds()->Sort(); //7.8
	SortTree(pProperties);
	//pTemporaries->GetChilds()->Sort();
	//pFunctions->GetChilds()->Sort();

	pMain->Expand();
	pProperties->Expand();

	//m_wndGrid.SetPropertySort(Alphabetical);

	//SortItems();
	//SortItems(pProperties,true);
	//SortItems(pFunctions,true);
	//SortItems(pTemporaries,true);
	/*
    CPropTreeItem *pti =  pProperties->GetChild();
    while (pti)
    {
        if (strcmp(pti->GetLabelText(),"Actions") == 0)
        {
            pti->Expand();
            break;
        }
        pti = pti->GetSibling();
    }

	*/
	
	//SetRedraw();
	//RedrawWindow();

	oldname = objname;

	//3, 8
	//GetGridView().OnVScroll(3,21,NULL);
	//GetGridView().SetScrollPos(SB_VERT,20,TRUE);
	//ScrollWindow(0,-20);
	//GetGridView().UpdateWindow();
	//UpdateWindow();
}
//=============================================================================================
void CPropertiesBar::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
/*
	int t = GScript.GetTop();

	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	CPropTreeItem *pti = pNMPropTree->pItem;
	if (pti)
	{
		CString key = pti->GetLabelText();
		CString val = "?";
		if (pti->IsComboBox())
		{
			CPropTreeItemCombo *pCBox = (CPropTreeItemCombo *) pti;
			int i = pCBox->GetCurSel();
			if (i >= 0)
				pCBox->GetLBText(i,val);
		}
        if (pti->IsBrowseEdit())
		{
			CPropTreeItemBrowseEdit *pBEdit = (CPropTreeItemBrowseEdit *) pti;
            val = (LPCSTR) pBEdit->GetItemValue();
		}

		if (pti->IsItemEdit() && ((CPropTreeItemEdit *)pti)->m_nFormat == CPropTreeItemEdit::ValueFormatText)
			val = (LPCSTR) pti->GetItemValue();



        if (key == "[new]" && (val == "..." || val=="?" || val=="" || val == "-remove-" || val == "-r" || val == "-insert-" || val == "-i")) 
		{
			GScript.SetTop(t);
			return;
		}

		CString strVarName = key; // full key
		CPropTreeItem *parent = pti->GetParent();
		while(parent && parent->GetParent() && parent->GetParent()->GetParent()) // clip ".Properties."
		{		
			strVarName = CString(parent->GetLabelText()) + "." + strVarName;
			parent = parent->GetParent();
		}

        GScript.DoString("old = Clone(%s)",m_ObjName);		

		// sprawdzam, ile elementow ma tablica
		CString tab = m_ObjName+"."+strVarName;
		if (tab.Find(".[new]")>-1)
		{
			tab.Replace(".[new]","");
			GScript.DoString("tmpn = table.getn(%s)",tab);
			int n = GScript.Globals()["tmpn"].GetInt() + 1;			
			CString a; a.Format(".%d",n);
			GScript.DoString("table.insert(%s,'')",tab);
			strVarName.Replace(".[new]",a);	
			key = a;
		}	

		if (val == "-remove-" || val == "-r")
		{
			CString tab = m_ObjName+"."+strVarName;
			tab.Replace("."+key,"");
			GScript.DoString("table.remove(%s,%s)",tab,key);
			GScript.SetTop(t);
			pti->SetLabelText("[new]"); // to refresh
		}
        else if (val == "-insert-" || val == "-i")
		{
			CString tab = m_ObjName+"."+strVarName;
			tab.Replace("."+key,"");
            GScript.DoString("table.insert(%s,%s,'')",tab,key);
			GScript.SetTop(t);
			pti->SetLabelText("[new]"); // to refresh
		}
		else
		{
			Script var = GScript.Globals()[m_ObjName];
			int curPos = 0;

			CString varname = "";
			CString resToken= strVarName.Tokenize(".",curPos);
			while (resToken != "")
			{
				if (resToken[0]>='0' && resToken[0]<='9')
				{
					var = var[atoi(resToken)];
					varname = varname + "["+resToken+"]";
				}
				else
				{
					var = var[resToken];
					varname = varname + "['"+resToken+"']";
				}
				resToken= strVarName.Tokenize(".",curPos);			
			};
		
			int type = var.GetType();		
			switch (type)
			{
				case LUA_TNUMBER:
					if (pti->IsComboBox())
						GScript.DoString("%s%s=%d",m_ObjName,varname,pti->GetItemValue());
					else
						GScript.DoString("%s%s=%f",m_ObjName,varname,*(float*)pti->GetItemValue());
					break;
				case LUA_TSTRING:
					if (pti->IsComboBox())
					{
						CPropTreeItemCombo *pCBox = (CPropTreeItemCombo *) pti;
						CString txt;
						int i = pCBox->GetCurSel();
						if (i < 0)
							txt = "";
						else
							pCBox->GetLBText(i,txt);
						GScript.DoString("%s%s=[[%s]]",m_ObjName,varname,(LPCSTR)txt);
						pti->SetItemValue((LPARAM)(LPCSTR)txt);
					}
					else
						GScript.DoString("%s%s=[[%s]]",m_ObjName,varname,(LPCSTR)pti->GetItemValue());
					break;
				case LUA_TBOOLEAN:
					GScript.DoString("%s%s=%s",m_ObjName,varname,pti->GetItemValue()?"true":"false");
					break;
				case LUA_TTABLE:
					if (!var["_Class"].IsNil())
					{
						CString classname = var["_Class"].GetString();
						if (classname == "Color")
						{
							int r = GetRValue(pti->GetItemValue());
							int g = GetGValue(pti->GetItemValue());						
							int b = GetBValue(pti->GetItemValue());
							GScript.DoString("%s%s:Set(%d,%d,%d,0)",m_ObjName,varname,r,g,b);
						}					
						else if (classname == "Vector")
						{
							float x,y,z;
							if (sscanf((LPCSTR)pti->GetItemValue(),"%f %f %f",&x,&y,&z) == 3)
								GScript.DoString("%s%s:Set(%f,%f,%f)",m_ObjName,varname,x,y,z);
						}
                        else if (classname == "Quaternion")
                        {
                            float x,y,z;
                            if (sscanf((LPCSTR)pti->GetItemValue(),"%f %f %f",&x,&y,&z) == 3)
                                GScript.DoString("%s%s:FromEuler(%f,%f,%f)",m_ObjName,varname,FDeg2Rad(x),FDeg2Rad(y),FDeg2Rad(z));
                        }
					}
					break;
			}
		}
        GScript.DoString("%s:Apply(old)",m_ObjName);
        GScript.DoString("old = nil");
		GEngine->ResetTimer();
	}

	GScript.SetTop(t);
	*plResult = 0;
	*/
}

//=============================================================================================

void CPropertiesBar::OnAfterChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	/*
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
	CPropTreeItem *pti = pNMPropTree->pItem;
    int t = GScript.GetTop();
	if (pti)
	{
		CString strVarName = pti->GetLabelText();		
		if (strVarName == "[new]")
		{
            int itab = 0;
            CPropTreeItem * tab[10];
            CPropTreeItem * p = pti;
            while (p)
            {
                tab[itab] = p;
                p = p->GetParent();
                itab++;
            }
		
            //m_PropTree.SetRedraw(false);            

            Script obj = GScript.Globals()[m_ObjName];
            if (obj.IsNil())
            {
                GScript.SetTop(t);
                return;
            }

            CString fullkey;
            for (int i=itab-3;i>0;i--)
            {
                CString lkey =  tab[i]->GetLabelText();
                obj = obj[lkey];
                fullkey += (fullkey.GetLength()>0?".":"") + lkey;
            }

			/* XXX Pro
            CString key = pti->GetParent()->GetLabelText();
            m_PropTree.DeleteItem(tab[1]);			
            m_PropTree.ClearVisibleList();

            CPropTreeItem *pt = FillTree(obj,key,fullkey,tab[2],false);			
            m_PropTree.SortItems(tab[2],true);
			pt->Expand();
			m_PropTree.UpdatedItems();
			*/
			//m_PropTree.SetRedraw();
			//if (vpos>0)
			//	m_PropTree.m_pList->OnVScroll(SB_THUMBTRACK,vpos,NULL) ;
	//	}
		//else
			//m_PropTree.ClearVisibleList(); XX Pro

/*
		if (strVarName == "Template")
			UpdateView(m_ObjName);
	}
    GScript.SetTop(t);
*/
}

/*
BOOL CPropertiesBar::PreTranslateMessage(MSG* pMsg)
{

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_INSERT && !(GetAsyncKeyState( VK_LCONTROL)&0x8000) && !(GetAsyncKeyState( VK_LSHIFT)&0x8000))
    {
        
        CPropTreeItem *pItem = m_PropTree.GetFocusedItem();
        CPropTreeItem *pFItem = pItem;
        if (!pItem->GetChild())
        {
            while (pItem)
            {
                if (strcmp(pItem->GetLabelText(),"Actions") == 0)
                {
                    CActionDlg dlg;
                    CString val = (LPCSTR) pFItem->GetItemValue();

                    int i = val.Find(':');
                    if (i)
                    {
                        dlg.m_Action = val.Left(i);
                        dlg.m_Args = val.Mid(i+1);
                    }
                    else
                        dlg.m_Action = val;


                    if (dlg.DoModal() == IDOK)
                    {
                        pFItem->SetItemValue((LPARAM)(LPCSTR)(dlg.m_Action + ":" + dlg.m_Args));
                        m_PropTree.RefreshItems(pFItem);
                        
                    }
                    return true;
                }
                pItem = pItem->GetParent();
            }
        }
    }

    return CWnd::PreTranslateMessage(pMsg);
}
*/

LRESULT CPropertiesBar::OnValueChanged(CXTPPropertyGridItem *pItem)
{
	if (pItem)
	{
		ScriptObject obj = GScript.Globals()[m_ObjName];
		if (obj.IsNil())
		{
			char buf[255];
			sprintf(buf,"Object '%s' not found! (Not exist or not selected?)",(LPCSTR)m_ObjName);
			MessageBox(buf);
			return 0;
		}

		CString key = pItem->GetCaption();
		CString val = pItem->GetValue();		

		if (key == "[new]" && (val == "..." || val=="?" || val=="" || val == "-remove-" || val == "-r" || val == "-insert-" || val == "-i")) 
		{
			return 0;
		}

		CString strVarName = key; // full key
		CXTPPropertyGridItem *parent = pItem->GetParentItem();
		while(parent && parent->GetParentItem()) // clip ".Properties."
		{		
			strVarName = CString(parent->GetCaption()) + "." + strVarName;
			parent = parent->GetParentItem();
		}

		GScript.DoString("old = Clone(%s)",m_ObjName);		

		bool refreshParent = false;

		CString tab = m_ObjName+"."+strVarName;
		if (tab.Find(".[new]") > -1) // add new element
		{
			tab.Replace(".[new]","");
			GScript.DoString("tmpn = table.getn(%s)",tab);
			int n = GScript.Globals()["tmpn"].GetInt() + 1;			
			CString a; 
			a.Format(".%d",n);
			GScript.DoString("table.insert(%s,'')",tab);
			strVarName.Replace(".[new]",a);	
			key = a;
			refreshParent = true;
		}	

		if (val == "-remove-" || val == "-r") // remove element
		{
			CString tab = m_ObjName+"."+strVarName;
			tab.Replace("."+key,"");
			GScript.DoString("table.remove(%s,%s)",tab,key);
			//pItem->SetCaption("[new]"); // to refresh
			refreshParent = true;
		}
		else if (val == "-insert-" || val == "-i")
		{
			CString tab = m_ObjName+"."+strVarName;
			tab.Replace("."+key,"");
			GScript.DoString("table.insert(%s,%s,'')",tab,key);
			//pItem->SetCaption("[new]"); // to refresh
			refreshParent = true;
		}
		else
		{
			ScriptObject var = GScript.Globals()[m_ObjName];
			int curPos = 0;

			CString varname = "";
			CString resToken= strVarName.Tokenize(".",curPos);
			while (resToken != "")
			{
				if (resToken[0]>='0' && resToken[0]<='9')
				{
					var = var[atoi(resToken)];
					varname = varname + "["+resToken+"]";
				}
				else
				{
					var = var[resToken];
					varname = varname + "['"+resToken+"']";
				}
				resToken= strVarName.Tokenize(".",curPos);			
			};

			
			int type = var.GetType();		
			switch (type)
			{
			case LUA_TNUMBER:
				if (pItem->GetFlags()&pgiHasComboButton)
				{
					CCustomItemData* pID = (CCustomItemData*) pItem;					
					int *data = pID->Data.Find((LPCSTR)val);
					if (data)
						GScript.DoString("%s%s=%d",m_ObjName,varname,*data);
				}
				else
					GScript.DoString("%s%s=%s",m_ObjName,varname,val);
				break;
			case LUA_TSTRING:
				GScript.DoString("%s%s=[[%s]]",m_ObjName,varname,val);
				break;
			case LUA_TBOOLEAN:
				GScript.DoString("%s%s=%s",m_ObjName,varname,val.MakeLower());
				break;
			case LUA_TTABLE:
				if (!var["_Class"].IsNil())
				{
					CString classname = var["_Class"].GetString();
					if (classname == "Color")
					{
						CXTPPropertyGridItemColor *pIC = (CXTPPropertyGridItemColor *) pItem;
						int r = GetRValue(pIC->GetColor());
						int g = GetGValue(pIC->GetColor());						
						int b = GetBValue(pIC->GetColor());
						
						GScript.DoString("%s%s:Set(%d,%d,%d,0)",m_ObjName,varname,r,g,b);
					}					
					else if (classname == "Vector")
					{
						float x,y,z;
						if (sscanf(val,"%f %f %f",&x,&y,&z) == 3)
							GScript.DoString("%s%s:Set(%f,%f,%f)",m_ObjName,varname,x,y,z);
					}
					else if (classname == "Quaternion")
					{
						float x,y,z;
						if (sscanf(val,"%f %f %f",&x,&y,&z) == 3)
							GScript.DoString("%s%s:FromEuler(%f,%f,%f)",m_ObjName,varname,FDeg2Rad(x),FDeg2Rad(y),FDeg2Rad(z));
					}
					
				}
				break;
			}
			
		}
		
		

		GScript.DoString("%s:Apply(old)",m_ObjName);
		GScript.DoString("old = nil");		
		
		if (refreshParent)
		{
			ScriptObject var = GScript.Globals()[m_ObjName];
			int curPos = 0;
			CString fullkey;
			CString resToken= strVarName.Tokenize(".",curPos);
			while (resToken != "")
			{
				if (resToken[0]>='0' && resToken[0]<='9')
					var = var[atoi(resToken)];
				else
					var = var[resToken];
				
				fullkey = fullkey + (fullkey != "" ? "." : "") + resToken;
				key = resToken;
				resToken= strVarName.Tokenize(".",curPos);			
				
				if ( strVarName.Find('.',curPos-1) == -1)
					break;	
			};
			
			//int pos = m_wndGrid.GetGridView().GetScrollPos(); // 7.8
			RECT rc;
			m_wndGrid.GetGridView().GetWindowRect(&rc);
			ScreenToClient(&rc);
			//m_wndGrid.GetGridView().GetClientRect(&rc);


			CXTPPropertyGridItem *pPParent = pItem->GetParentItem()->GetParentItem();
			pItem->GetParentItem()->Remove();
			
			//CString a = pParent->GetCaption();
			//while (pParent->GetChilds()->GetCount())
			//	pParent->GetChilds()->RemoveAt(0);

			//SCROLLINFO sf;
			//ZeroMemory(&sf, sizeof(sf));
			//sf.cbSize = sizeof(sf);
			//sf.fMask = SIF_ALL;

			//BOOL ok = GetGridView().GetScrollInfo(SB_VERT,&sf);
			//CScrollBar * pSB = GetScrollBarCtrl(SB_VERT);
			//int pos = 0;
				//->GetScrollPos();
				//GetScrollPos(SB_VERT);
			
			pItem = FillTree(var,key,fullkey,pPParent,false);
			pPParent->GetChilds()->Sort(); //7.8
			pItem->Expand();			

			//GetGridView().OnVScroll(SB_THUMBPOSITION,pos,NULL);
			//GetGridView().ScrollWindow(0,pos);
			
			//m_wndGrid.GetGridView().SetScrollPos(SB_VERT , pos, true); //7.8
			//WINDOWPLACEMENT wpl;
			//m_wndGrid.GetGridView().GetWindowPlacement(&wpl);

			//m_wndGrid.GetGridView().ScrollWindow(0,-100);
			//m_wndGrid.GetGridView().SetWindowPos(NULL,rc.left,rc.top,rc.right,rc.bottom,0);

			//m_wndGrid.GetGridView().Refresh();
			//m_wndGrid.GetGridView().OnVScroll(5,-30,m_wndGrid.GetGridView().GetScrollBarCtrl(SB_VERT));
			//m_wndGrid.GetGridView().OnVScroll(4,-30,m_wndGrid.GetGridView().GetScrollBarCtrl(SB_VERT));
			//m_wndGrid.GetGridView().OnVScroll(8,-30,m_wndGrid.GetGridView().GetScrollBarCtrl(SB_VERT));
			//m_wndGrid.GetGridView().RedrawWindow();
			m_wndGrid.SetViewDivider(0.25);
			//m_wndGrid.GetGridView().OnMouseWheel(0,pos,CPoint(0,pos));

			//GetGridView().GetClientRect()
			//ScrollWindowEx(0,100,NULL,NULL,NULL,NULL,SW_ERASE);

			//m_wndGrid.GetGridView().ScrollWindow(0,pos);


			//m_wndGrid.RedrawWindow();

			//GetGridView().ScrollWindow(0,-pos);
			//RecalcLayout();
			//ScrollWindow(0, pos);
			//GetGridView().GetClientRect()
			//ScrollWindowEx(0,100,NULL,NULL,NULL,NULL,SW_ERASE);

		}

		GEngine->ResetTimer();
	}

	return 0;
}

void CPropertiesBar::OnToolbarCommand(UINT nID)
{
	switch (nID)
	{
	case ID_LEVEL_SAVEOBJECT:
		CGameObjectsBar::Get().OnCommand(PM_Save,0);
		break;
	case ID_LEVEL_REFRESHOBJECT:
		UpdateView(m_ObjName);
		break;
	case ID_LEVEL_RESETOBJECT:
		CGameObjectsBar::Get().OnCommand(PM_Reset,0);
		break;
	}
}

LRESULT CPropertiesBar::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		OnValueChanged(pItem);
	}
	return 0;
};

#endif