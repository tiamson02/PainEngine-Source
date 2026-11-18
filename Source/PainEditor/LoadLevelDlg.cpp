// LoadLevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "LoadLevelDlg.h"
#include ".\loadleveldlg.h"
#include ".\Game\Main.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

// CLoadLevelDlg dialog

#ifdef EDITOR

IMPLEMENT_DYNAMIC(CLoadLevelDlg, CDialog)
CLoadLevelDlg::CLoadLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadLevelDlg::IDD, pParent)
{
}

CLoadLevelDlg::~CLoadLevelDlg()
{
}

void CLoadLevelDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LEVELLIST, m_LevelList);
}


BEGIN_MESSAGE_MAP(CLoadLevelDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LEVELLIST, OnNMDblclkLevellist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_CREATEPKM, OnBnClickedCreatepkm)
END_MESSAGE_MAP()


// CLoadLevelDlg message handlers

BOOL CLoadLevelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_LevelList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 150);
	m_LevelList.InsertColumn(1, _T("Description"), LVCFMT_LEFT, 200);
	m_LevelList.SetExtendedStyle (LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	FillLevelList();

    return TRUE;
}

void CLoadLevelDlg::FillLevelList()
{
	m_LevelList.DeleteAllItems();

	DynamicArray<String> files;
	GFileMan.FindFiles("../Data/Levels/*.*",files,EFindDirs);


	for (int i=0;i<files.Size();i++)
	{
		const char *lname = files[i].AsChar();
		if (lname[0] != '_')
			m_LevelList.InsertItem(i,lname);
	}

	m_LevelList.SetSelectionMark(0);
	m_LevelList.UpdateData(false);

}

CString CLoadLevelDlg::GetSelected()
{
  char buf[255];
  POSITION pos = m_LevelList.GetFirstSelectedItemPosition();
  if (!pos) return "";
  int  n = m_LevelList.GetNextSelectedItem(pos);
  m_LevelList.GetItemText(n,0,buf,254);

  return buf;
}


void CLoadLevelDlg::OnNMDblclkLevellist(NMHDR *pNMHDR, LRESULT *pResult)
{
	mLevelName = GetSelected();
	if (mLevelName=="") return;
	EndModalLoop(IDOK);
	*pResult = 0;
}

void CLoadLevelDlg::OnBnClickedOk()
{
	mLevelName = GetSelected();
	if (mLevelName=="") return;
	OnOK();
}

void DelTree(const char *path)
{
	// sprawdzam czy moge usunac katalog

	if (!RemoveDirectory(path)) // jezeli nie
	{
		char SerchFile[MAX_PATH];
#ifdef TRANSGAMING_BOOGALOOGA
        _finddata_t     FindFileData;
        long32          hFind;
#else
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
#endif

		sprintf(SerchFile,"%s/*.*",path);


#ifdef TRANSGAMING_BOOGALOOGA
		hFind = _findfirst(SerchFile, &FindFileData);
		while(hFind != -1)
		{
			sprintf(SerchFile,"%s/%s",path,FindFileData.name);    
			if (FindFileData.attrib & _A_SUBDIR)
			{
				if (FindFileData.name[0] !='.')
					DelTree(SerchFile);
			}
			else
			{
				SetFileAttributes(SerchFile,FILE_ATTRIBUTE_NORMAL);
				DeleteFile(SerchFile);
			};
			if (_findnext(hFind, &FindFileData) == 0) break;
		}
		if (hFind != -1)
            _findclose(hFind);
	};
#else
		hFind = FindFirstFile(SerchFile, &FindFileData);
		while(hFind != INVALID_HANDLE_VALUE)
		{
			sprintf(SerchFile,"%s/%s",path,FindFileData.cFileName);    
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (FindFileData.cFileName[0] !='.')
					DelTree(SerchFile);
			}
			else
			{
				SetFileAttributes(SerchFile,FILE_ATTRIBUTE_NORMAL);
				DeleteFile(SerchFile);
			};
			if (!FindNextFile(hFind,&FindFileData)) break;
		}
		if (hFind != INVALID_HANDLE_VALUE) FindClose(hFind);
	};
#endif

    RemoveDirectory(path);
};

//=========================================================================  
void CopyDir(const char *path,const char * dest_path)
{
	char SerchFile[MAX_PATH];
	char DestFile[MAX_PATH];
#ifdef TRANSGAMING_BOOGALOOGA
    _finddata_t     FindFileData;
    long32          hFind;
#else
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
#endif

	CreateDirectory(dest_path,NULL);

	sprintf(SerchFile,"%s/*.*",path);

#ifdef TRANSGAMING_BOOGALOOGA
	hFind = _findfirst(SerchFile, &FindFileData);
	while(hFind != -1)
	{
		sprintf(SerchFile,"%s/%s",path,FindFileData.name);    
		sprintf(DestFile,"%s/%s",dest_path,FindFileData.name);    
		if (FindFileData.attrib & _A_SUBDIR)
		{
			if (FindFileData.name[0] != '.')
				CopyDir(SerchFile,DestFile);
		}
		else
		{
			CopyFile(SerchFile,DestFile,false);
		};
		if (_findnext(hFind,&FindFileData) == 0) break;
	};
	if (hFind != -1) 
        _findclose(hFind);
#else
	hFind = FindFirstFile(SerchFile, &FindFileData);
	while(hFind != INVALID_HANDLE_VALUE)
	{
		sprintf(SerchFile,"%s/%s",path,FindFileData.cFileName);    
		sprintf(DestFile,"%s/%s",dest_path,FindFileData.cFileName);    
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
				CopyDir(SerchFile,DestFile);
		}
		else
		{
			CopyFile(SerchFile,DestFile,false);
		};
		if (!FindNextFile(hFind,&FindFileData)) break;
	};
	if (hFind != INVALID_HANDLE_VALUE) FindClose(hFind);
#endif
};

void CopyTexture(LPCSTR _src,LPCSTR _dst)
{
	char src[MAX_PATH],dst[MAX_PATH];

	String texName(String::StripExtension(_src));
	if (texName.Size()>3 && !strcmp(texName.AsChar()+(texName.Size()-4),"_00"))
	{
		texName = texName.SubStr(texName.Size()-4);
		
		String texNameDest = String::StripExtension(_dst);
		texNameDest = texNameDest.SubStr(texNameDest.Size()-4);
		for (int i=1; i<99; ++i) 
		{
			String name = texName + String::Sprintf("_%02d",i);
			String dest = texNameDest + String::Sprintf("_%02d",i);			
			CopyTexture(name.AsChar(),dest.AsChar());
		}
	}

	sprintf(src,"%s.bmp",_src);
	sprintf(dst,"%s.bmp",_dst);
	CopyFile(src,dst,FALSE);

	sprintf(src,"%s.tga",_src);
	sprintf(dst,"%s.tga",_dst);
	CopyFile(src,dst,FALSE);

	sprintf(src,"%s.dds",_src);
	sprintf(dst,"%s.dds",_dst);
	CopyFile(src,dst,FALSE);
}

void CLoadLevelDlg::OnBnClickedDelete()
{
	CString levName = GetSelected();
	if (levName=="") return;
	if (MessageBox("Are you sure?","Delete Level",MB_YESNO) == IDYES)
	{
		DelTree("../Data/Levels/"+levName);
		FillLevelList();
	}

}

void CLoadLevelDlg::OnBnClickedCreatepkm()
{
	DelTree("../Data/_Tmp");

	CString levName = GetSelected();
	if (levName=="") return;
	char src[MAX_PATH],dst[MAX_PATH];

	CreateDirectory("../Data/_Tmp",0);
	CreateDirectory("../Data/_Tmp/Levels",0);	
	CreateDirectory("../Data/_Tmp/Maps",0);
	CreateDirectory("../Data/_Tmp/Maps/MOPPCode",0);
	CreateDirectory("../Data/_Tmp/Textures",0);
	CreateDirectory("../Data/_Tmp/Textures/Levels",0);
	CreateDirectory("../Data/_Tmp/Textures/Special",0);	
	CreateDirectory("../Data/_Tmp/Textures/Skies",0);
	CreateDirectory("../Data/_Tmp/Textures/Skies/LowQuality",0);


	GScript.DoString("									\
		o = Clone(CLevel)								\
		DoFile('../Data/Levels/%s/%s.CLevel')			\
		", (LPCSTR) levName, (LPCSTR) levName);

	const char * mapName = GScript.Globals()["o"]["Map"].GetString();
	
	// level
	sprintf(src,"../Data/Levels/%s",LPCSTR(levName));
	sprintf(dst,"../Data/_Tmp/Levels/%s",LPCSTR(levName));
	CopyDir(src,dst);

	// map
	sprintf(src,"../Data/Maps/%s",LPCSTR(mapName));
	sprintf(dst,"../Data/_Tmp/Maps/%s",LPCSTR(mapName));
	CopyFile(src,dst,FALSE);

	// sky
	const char * skyName = GScript.Globals()["o"]["SkyDome"]["Map"].GetString();
	sprintf(src,"../Data/Maps/%s",LPCSTR(skyName));
	sprintf(dst,"../Data/_Tmp/Maps/%s",LPCSTR(skyName));
	CopyFile(src,dst,FALSE);
	const char * skyLQName = GScript.Globals()["o"]["SkyDome"]["LowQuality"]["Map"].GetString();
	sprintf(src,"../Data/Maps/%s",LPCSTR(skyLQName));
	sprintf(dst,"../Data/_Tmp/Maps/%s",LPCSTR(skyLQName));
	CopyFile(src,dst,FALSE);

	// sky textures
	const char * skyLQTex = GScript.Globals()["o"]["SkyDome"]["LowQuality"]["Tex"].GetString();
	sprintf(src,"../Data/Textures/Skies/%s",LPCSTR(skyLQTex));
	sprintf(dst,"../Data/_Tmp/Textures/Skies/%s",LPCSTR(skyLQTex));
	CopyTexture(src,dst);

	for (int layer = 1; layer<=4 ; layer ++)
	{
		char layerName[20];
		sprintf(layerName,"Layer%d",layer);
		const char * skyTex_Mask = GScript.Globals()["o"]["SkyDome"][layerName]["Tex_Mask"].GetString();
		const char * skyTex_LMap = GScript.Globals()["o"]["SkyDome"][layerName]["Tex_LMap"].GetString();
		const char * skyTex1 = GScript.Globals()["o"]["SkyDome"][layerName]["Tex1"].GetString();
		const char * skyTex2 = GScript.Globals()["o"]["SkyDome"][layerName]["Tex2"].GetString();
		sprintf(src,"../Data/Textures/Skies/%s",LPCSTR(skyTex_Mask));
		sprintf(dst,"../Data/_Tmp/Textures/Skies/%s",LPCSTR(skyTex_Mask));
		CopyTexture(src,dst);
		sprintf(src,"../Data/Textures/Skies/%s",LPCSTR(skyTex_LMap));
		sprintf(dst,"../Data/_Tmp/Textures/Skies/%s",LPCSTR(skyTex_LMap));
		CopyTexture(src,dst);
		sprintf(src,"../Data/Textures/Skies/%s",LPCSTR(skyTex1));
		sprintf(dst,"../Data/_Tmp/Textures/Skies/%s",LPCSTR(skyTex1));
		CopyTexture(src,dst);
		sprintf(src,"../Data/Textures/Skies/%s",LPCSTR(skyTex2));
		sprintf(dst,"../Data/_Tmp/Textures/Skies/%s",LPCSTR(skyTex2));
		CopyTexture(src,dst);
	}

	// DetailMap
	const char * tmp = GScript.Globals()["o"]["DetailMap"]["Tex"].GetString();
	sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
	sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
	CopyTexture(src,dst);

	//NormalMap
	tmp = GScript.Globals()["o"]["NormalMap"]["Tex"].GetString();
	sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
	sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
	CopyTexture(src,dst);

	//CubeMap
	tmp = GScript.Globals()["o"]["CubeMap"]["Tex"].GetString();
	sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
	sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
	CopyTexture(src,dst);

	// MOPPCode
	sprintf(src,"../Data/Maps/MOPPCode/%s",LPCSTR(mapName));
	sprintf(dst,"../Data/_Tmp/Maps/MOPPCode/%s",LPCSTR(mapName));
	CopyDir(src,dst);

	// textures
	sprintf(src,"../Data/Textures/Levels/%s",String::StripExtension(LPCSTR(mapName)).AsChar());
	sprintf(dst,"../Data/_Tmp/Textures/Levels/%s",String::StripExtension(LPCSTR(mapName)).AsChar());
	CopyDir(src,dst);

	// map entities
	sprintf(src,"../Data/Levels/%s/MapEntities/*.EMesh",LPCSTR(levName));
	DynamicArray<String> files;
	GFileMan.FindFiles(src,files,EFindFiles);
	for (int i=0;i<files.Size();i++)
	{
		const char *lname = files[i].AsChar();
		GScript.DoString("											\
						 o = Clone(EMesh)							\
						 DoFile('../Data/Levels/%s/MapEntities/%s')	\
						 ", (LPCSTR) levName, (LPCSTR) lname);
		
		// DetailMap
		const char * tmp = GScript.Globals()["o"]["DetailMap"]["Tex"].GetString();
		sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
		sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
		CopyTexture(src,dst);

		//NormalMap
		tmp = GScript.Globals()["o"]["NormalMap"]["Tex"].GetString();
		sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
		sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
		CopyTexture(src,dst);

		//CubeMap
		tmp = GScript.Globals()["o"]["CubeMap"]["Tex"].GetString();
		sprintf(src,"../Data/Textures/%s",LPCSTR(tmp));
		sprintf(dst,"../Data/_Tmp/Textures/%s",LPCSTR(tmp));
		CopyTexture(src,dst);
	}



	GScript.DoString("o = nil");
	GScript.DoString("FS.CreatePack('../Data/%s.pkm','../Data/_Tmp',true)",(LPCSTR)levName);

	MessageBox("Successful!","Create PKM");

	
	//DelTree("../Data/_Tmp");
}

#endif

