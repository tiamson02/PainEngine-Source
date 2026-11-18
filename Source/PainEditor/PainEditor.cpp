// PainEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PainEditor.h"
#include "MainFrm.h"
#include "Game/Main.h"
#include "ChildFrm.h"
#include "SplashWnd.h"
#include "LoadLevelDlg.h"
#include "InfoDlg.h"
#include "GameObjectsBar.h" 
#include "TemplatesBar.h" 
#include "Engine/InputSystem.h"
#include "Engine/PainMenu.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Render/Video.h"
#include "Render/View.h"

#include "OutputDlg.h"

void ExportToLua();
// CPainEditorApp

BEGIN_MESSAGE_MAP(CPainEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//ON_COMMAND(ID_FILE_LOADLEVEL, CWinApp::OnFileOpen)//OnFileLoadlevel)
END_MESSAGE_MAP()


// CPainEditorApp construction

CPainEditorApp::CPainEditorApp()
{
	//EnableHtmlHelp();
    Dedicated = false;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPainEditorApp object

CPainEditorApp theApp;

void EditorLog(const char *txt)
{
#ifdef EDITOR

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (theApp.Dedicated)
    {
		//theApp.m_dlgDedicatedServer.m_dlgOutput.PrintText(txt);
        extern COutputDlg dlgOutput;
        dlgOutput.PrintText(txt);
    }
    else
    {
		//MessageBox(NULL,txt,txt,MB_TOPMOST);
        if (AfxGetApp()->m_pMainWnd)
            pFrame->OutputText(txt);
    }
#endif
}


// CPainEditorApp initialization
BOOL CPainEditorApp::InitInstance()
{
	CfgFileName = "config.ini";
	// Set working dir to module dir
    char dir[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(),dir,MAX_PATH);
	String fileName=String::BaseName(dir);
	dir[::strlen(dir)-fileName.Len()]=0;
	SetCurrentDirectory(dir);

#ifdef EDITOR
	strlwr(dir);
	char *as = strstr(dir,"\\editor");
	if (as)
	{
		as[1] = '\0';
		SetCurrentDirectory(dir);
	}	
#endif

#ifdef STAND_ALONE_SERVER
	Dedicated = true;
#else
    if (strstr(GetCommandLine(),"-dedicated"))
        Dedicated = true;
#endif

    if (!Dedicated)
    {
        // prevent multiple instances
        CreateMutex(NULL,TRUE,"PCFPainEditor");
        if(ERROR_ALREADY_EXISTS==GetLastError()) {
            MessageBox(NULL,"Already running!","ERROR",MB_OK);        
            return 0;
        }
    }

	CWinApp::InitInstance();
	SetRegistryKey(_T("PeopleCanFly"));

	CString LScriptsPak = "lscripts.pak";
	char * a = strstr(GetCommandLine(),"-lscripts");
	if (a)
	{		
		char *b = strstr(a,".pak");
		if (b)
		{
			LScriptsPak = a+10;
			LScriptsPak = LScriptsPak.Left((b+4)-(a+10));
			LScriptsPak = LScriptsPak.Trim();
		}
	}

	GScript.Init();
	ExportToLua();
	GFileMan.RegisterPack(new GPack(), "../data/"+LScriptsPak, "../data/lscripts/", false);
	GFileMan.RegisterPack(new GPack(), "../data/levels.pak"  , "../data/levels/"  , false);
	GFileMan.RegisterPack(new GPack(), "../data/scripts.pak" , "../data/scripts/" , false);

    CMainFrame* pFrame = NULL;
    if (!Dedicated)
    {
        #ifdef EDITOR
	        DEVMODE mode;
	        EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&mode);
	        if (mode.dmBitsPerPel !=32 ) 
	        {
		        MessageBox(NULL,"Please change desktop mode to TrueColor (32 bit)","Warning Message",0);
		        return false;
	        }
	        InitCommonControls();
        #endif

	    // XXX - tutaj stworzyc jakis fakowy mainframe ... a nie CMainFrame
	    pFrame = new CMainFrame();
	    m_pMainWnd = pFrame;

	    // create main MDI frame window
	    if (!pFrame->LoadFrame(IDR_MAINFRAME))
		    return FALSE;

        #ifdef EDITOR
	        // try to load shared MDI menus and accelerator table
	        HINSTANCE hInst = AfxGetResourceHandle();
	        m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	        m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	        // Enable DDE Execute open
	        EnableShellOpen();
	        //RegisterShellFileTypes(TRUE);
	        OnFileNew();
             GLog.SetCallbackFunc(EditorLog);
        #endif
    }


    a = strstr(GetCommandLine(),"-script");
    if (a)
    {
        GScript.DoString(a+8);
        return 0;
    }
	
	a = strstr(GetCommandLine(),"-cfg");
	if (a)
	{
		char *b = strstr(a,".ini");
		if (b)
		{
			CfgFileName = a+5;
			CfgFileName = CfgFileName.Left((b+4)-(a+5));
			CfgFileName = CfgFileName.Trim();
		}
	}
	
	DynamicArray<String> files;
	GFileMan.FindFiles("../Data/*.pkm",files,EFindFiles);
	for (int i=0;i<files.Size();i++)
	{
		GFileMan.RegisterPack(new GPack(), String( String("../data/") + files[i]) .AsChar(), "../data/",false);
	}


    GScript.DoFile("../Data/LScripts/Main/Tweak.lua");
    GScript.DoFile("../Data/LScripts/Main/Cfg.lua");
    GScript.DoFile("../Data/LScripts/Main/Definitions.lua");
	GScript.DoString("CfgFile = [[%s]]",(LPCSTR)CfgFileName);
    GScript.DoString("Cfg:Load();Tweak:Load()");

    if (Dedicated)
    {
        CPainEditorApp* pApp = (CPainEditorApp*)AfxGetApp();
        if (pApp->Dedicated)
        {
            pApp->m_dlgDedicatedServer.Create(IDD_DEDICATEDSERVER);
            pApp->m_dlgDedicatedServer.ShowWindow(SW_SHOW);
            pApp->m_pMainWnd = &pApp->m_dlgDedicatedServer;
#ifndef FINAL_BUILD
            GLog.SetCallbackFunc(EditorLog);
#endif
			if (strstr(GetCommandLine(),"-start"))
				pApp->m_dlgDedicatedServer.OnBnClickedStart();

            // nie tworze GEnginu
            return 1;
        }
    }

	if (!Game_Init(AfxGetInstanceHandle(),GetCommandLine()))
		return false;

#ifdef EDITOR
	GScript.DoString("Editor.Enabled = true"); 
	// The main window has been initialized, so show and update it
	pFrame->ShowWindowEx(m_nCmdShow);	
	SwitchToFullScreen(false);
	pFrame->UpdateWindow();	
	pFrame->UpdateResolutionCombo();
	pFrame->OnPause();
	pFrame->LoadSettings();
    CGameObjectsBar::Get().UpdateView("",true);
    CTemplatesBar::Get().UpdateView("../Data/LScripts/Templates");
    

    //pFrame->ShowWindow(SW_SHOW);
#else
	GScript.DoString("Editor.Enabled = false");
    /*
        CInfoDlg idlg;
        char buf[50000]; buf[0] = NULL;
        FILE *f = fopen("info.txt","rb");
        if (f)
        {
            int l = fread(buf,1,50000,f);
            buf[l];
            fclose(f);
        }        
        idlg.m_Text = buf;
        if (idlg.DoModal() != IDOK)
            return false;
    */
	ShowCursor(FALSE);
	SwitchToFullScreen(true);
	
	ShowWindowAsync(GEngine->Video->FullScreenHWND(),SW_SHOWNORMAL);
	SetForegroundWindow(GEngine->Video->FullScreenHWND());
	SetFocus(GEngine->Video->FullScreenHWND());
	GEngine->InputSys->SetWindow(GEngine->Video->FullScreenHWND());
//	if( GEngine->Video )
//		GEngine->Video->PMenu->LoadScreen.Activate( true );

	if( !GEngine->StartMultiplayerClient && !GEngine->StartPlayingRecording )
	{
		Game()->SwitchMenu();
		// EXPO STUFF
		//GScript.DoString("StringToDo = [[Game:OnPlay(); AddAction({{'Wait:1'},{\"L:StringToDo = 'Console:Cmd_EXPO()'\"}})]]");
	}
	else
	{
		GScript.DoString("PainMenu.showStartMovies = false");
		GEngine->Video->PMenu->EnableSplash( false );
	}

	GEngine->ResetTimer();	
#endif

	CSplashWnd::m_bToHide = true;



	return TRUE;
}


// CPainEditorApp message handlers

//int CPainEditorApp::ExitInstance() 
//{
//	//TODO: handle additional resources you may have added
//	if (m_hMDIMenu != NULL)
//		FreeResource(m_hMDIMenu);
//	if (m_hMDIAccel != NULL)
//		FreeResource(m_hMDIAccel);
//
//	return CWinApp::ExitInstance();
//}

void CPainEditorApp::OnFileNew() 
{
#ifdef EDITOR
	CXTPMDIFrameWnd* pFrame = (CXTPMDIFrameWnd*)m_pMainWnd;
	// create a new MDI child window
	pFrame->CreateNewChild(RUNTIME_CLASS(CChildFrame), IDR_MAINFRAME, NULL, NULL);
#endif
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CPainEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPainEditorApp message handlers

UINT MainLoop( LPVOID pParam )
{
	while (true)
		((CPainEditorApp*)AfxGetApp())->Tick();
	
	return 0;
}

void CPainEditorApp::Tick()
{
    if (!GEngine)
        return;

    bool minimized = false;
    if (!Dedicated)
    {
        HWND hWnd = NULL;
        if (GEngine->Video && GEngine->Video->IsFullScreen)
		    hWnd = GEngine->Video->FullScreenHWND();
	    else
	    {
		    hWnd = GetMainWnd()->m_hWnd;
		    CMDIChildWnd * pWnd = ((CMainFrame*)GetMainWnd())->MDIGetActive();
		    if (!pWnd) 
			    hWnd = NULL;
		    else
		    {
			    CDocument *pDoc =  pWnd->GetActiveDocument();
			    if (pDoc)
			    {
				    CDocTemplate *pDTml = pDoc->GetDocTemplate();
				    CString str;
				    pDTml->GetDocString(str,CDocTemplate::regFileTypeId);
				    hWnd = NULL;
			    }
		    }
	    }

	    if(!hWnd || GetForegroundWindow() != hWnd || IsIconic(hWnd))
	    {
		    minimized = true;
	    }
	    else
	    {
		    #ifdef EDITOR
		    static pressed = false; 
		    if (!pressed && GetAsyncKeyState( VK_SPACE))
		    {
			    CMainFrame *pWnd = (CMainFrame *) GetMainWnd();
			    if (pWnd)
			    {
				    if (((CParticleFXEditorDlg*) pWnd->m_wndParticleFXDlg.m_InnerDlg)->m_Preview)
					    ((CParticleFXEditorDlg*) pWnd->m_wndParticleFXDlg.m_InnerDlg)->OnBnClickedRestart();
    				
				    if (((CEmitterEditorDlg*) pWnd->m_wndEmitterDlg.m_InnerDlg)->m_Preview)
					    ((CEmitterEditorDlg*) pWnd->m_wndEmitterDlg.m_InnerDlg)->OnBnClickedRestart();
			    }
			    pressed = true;
		    }
		    if (!GetAsyncKeyState( VK_SPACE))
			    pressed = false;

		    if( GetAsyncKeyState( VK_LCONTROL)&0x8000 && GetAsyncKeyState(83)&0x8000)
		    {
			    CMainFrame *pWnd = (CMainFrame *) GetMainWnd();
				CGameObjectsBar::Get().OnCommand(PM_Save,0);
		    }
		    #endif
	    }
    }

	if (Game()) 
		GEngine->TickEngine(minimized);				
}

BOOL CPainEditorApp::OnIdle(LONG lCount)
{
	//static CWinThread* pThread = AfxBeginThread(MainLoop, 0);
	//pThread->m_pMainWnd = m_pMainWnd;

	Tick();

	CWinApp::OnIdle(lCount);
    if (!GEngine) return false;
	return true;
}


int CPainEditorApp::ExitInstance()
{

	Game_Release();
    if (Dedicated)
        m_dlgDedicatedServer.DestroyWindow();

	return CWinApp::ExitInstance();
}


void CPainEditorApp::ExecuteProgram(LPCSTR  strPath, LPCSTR dir)
{
    PROCESS_INFORMATION pi;  
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));  
    si.cb = sizeof(si); 
	//  si.wShowWindow = SW_SHOW;
	//  si.dwFlags = STARTF_USESHOWWINDOW;    
    CreateProcess(NULL,(char*)(LPCSTR) strPath, NULL,   NULL,   FALSE, 0,   NULL, dir,  &si, &pi);
    Sleep(300); // max mocy na zaladowanie
};



CDocument *CPainEditorApp::FindDocument(CString title)
{
   POSITION pos = GetFirstDocTemplatePosition();

   while (pos)
   {
      CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
      POSITION pos2 = pTemplate->GetFirstDocPosition();
      while (pos2)
      {
         CDocument * pDocument;
         if ((pDocument=pTemplate->GetNextDoc(pos2)) != NULL)
			 if (pDocument->GetTitle() == title)
				 return pDocument;
      }
   }

   /*
   		CDocument *pDoc = ((CPainEditorApp*)AfxGetApp())->FindDocument(tree.GetItemText(pItem));
		if (pDoc)
		{
			POSITION pos = pDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = pDoc->GetNextView(pos);
				((CMainFrame*)AfxGetMainWnd())->MDIActivate(pView->GetParentFrame());
				return;
			}   
		}
		*/

   return NULL;
}
/*
CLuaView* CPainEditorApp::OpenScriptView(LPCSTR fname, int nLine)
{
	CLuaView *pView = NULL;
	POSITION pos = m_pScriptEditDocTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pScriptEditDocTemplate->GetNextDoc(pos);
		CString str(pDoc->GetPathName());
		str.Replace('\\','/');
		int i = str.Find("/Data/");	
		str = ".." + str.Mid(i);

		if (str.CompareNoCase(fname) == 0)
		{
			pView =  pDoc->GetView();
			break;
		}
	}

	if (!pView)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pScriptEditDocTemplate->OpenDocumentFile(fname,TRUE);
		if ( pDoc )
			pView = pDoc->GetView();
	}

	if ( pView )
	{
		pView->Activate();
		//((CMainFrame *)m_pMainWnd)->SetActiveView(pView);
	}

	if ( pView && nLine>=0 )
		pView->GetEditor()->GotoLine(nLine);

	return pView;
}
*/