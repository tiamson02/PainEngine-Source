#include "stdafx.h"
#include <time.h>
#include "Engine.h"
#include "Render/HUD.h"
#include "EditorGame.h"
#include "Mainfrm.h"
#include "PainEditor.h"
#include "Main.h"
#include "GameObjectsBar.h" 
#include "PropertiesBar.h" 

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Render/Video.h"
#include "Render/View.h"

void EditorLog(const char *txt);
#pragma warning( disable : 4311 4312 4244 4800)

// =========================================================================
// STANDARD
// =========================================================================
static int luaMsgBox(lua_State* state)
{
	Script L(state);
    if (GEngine && GEngine->Video && GEngine->Video->IsFullScreen)
    {
        ShowWindow(GEngine->Video->FullScreenHWND(),SW_MINIMIZE);
        UpdateWindow(GEngine->Video->FullScreenHWND());
        SetFocus(GetDesktopWindow());
        SetForegroundWindow(GetDesktopWindow());
    }
        

    MessageBox(NULL,L.GetString(1),"Script Message",MB_TOPMOST|MB_SETFOREGROUND);
    	
    if (GEngine)
		GEngine->ResetTimer();
	return 0;
}
// =========================================================================
static int luaDoFile(lua_State* state)
{
	Script L(state);
#ifdef EDITOR    
    //CString str = L.GetString(1);
    //EditorLog("DoFile: " + str + "\n");
#endif
    L.DoFile(L.GetString(1),L.GetBool(2,1));
	return 0;
}
// =========================================================================
static int luaErrorMsgBox(lua_State* state)
{
	Script L(state);

    if (GEngine && GEngine->Video && GEngine->Video->IsFullScreen)
    {
        ShowWindow(GEngine->Video->FullScreenHWND(),SW_MINIMIZE);
        UpdateWindow(GEngine->Video->FullScreenHWND());
        SetFocus(GetDesktopWindow());
        SetForegroundWindow(GetDesktopWindow());
    }

	CString msg = L.GetString(1);
	FILE *f = fopen("script_error.log","at");
	if (f)
	{
		fprintf(f,(LPCSTR)GEngine->SysDriver.GetTimeAsString());		
		fprintf(f,":\n");
		fprintf(f,(LPCSTR)msg);
		fprintf(f,"\n\n");
		fclose(f);
	};
	msg += "\n\n Press \"Retry\" to Debug.";
	int m = MessageBox(NULL,msg,"Script Error",MB_TOPMOST|MB_SETFOREGROUND|MB_ABORTRETRYIGNORE);	
	if (m == IDRETRY)
	{
        CString ad;
        int o1=0,o2=1;
		int s = msg.Find("\"]:");
        if (s<0) 
        {
            ad = "\".";
            s =  msg.Find(":");
        }
        else
        {
            o1 = 8; 
            o2=3;
        };

		if (s)
		{
            int e = msg.Find(":",s+3);
			if (e)
			{
                CString exe = "wscite/scite.exe \" -open:" + ad + msg.Mid(o1,s-o1) + "\" -goto:" + msg.Mid(s+o2,e-s-o2);
				((CPainEditorApp*)AfxGetApp())->ExecuteProgram(exe,NULL);
			}
		}
	}

	if (GEngine)
		GEngine->ResetTimer();

	if (m == IDABORT)
	{
		GScript.Release();
		delete GEngine;
        AfxPostQuitMessage(0); 
		exit(0); //XXX hmm ;)
	}

	return 0;
}
// =========================================================================
static int luaExit(lua_State* state)
{
	AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
	return 0;
}
// =========================================================================
// EDITOR
// =========================================================================
static int luaEditor_OnMsg(lua_State* state)
{	
	Script L(state);
    
    if (!GEngine || GEngine->GameState == PCFSystem::DedicatedServer )
        return 0;

#ifdef EDITOR
    const char * msg = L.GetString(1);
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	///if (!pWnd || pWnd->m_wndLevelExplorer.m_hWnd) return 0;
	if (strcmp(msg,"SelectObject")==0)
	{
        CGameObjectsBar::Get().UpdateView(L.GetString(2));
        pWnd->m_PropertiesBar.UpdateView(L.GetString(2));
	}
    if (strcmp(msg,"AddedObject")==0)
        CGameObjectsBar::Get().UpdateView(L.GetString(2));
#endif
	return 0;
}//*/
// =========================================================================
static int luaEditor_PostMessage(lua_State* state)
{	
	Script L(state);
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->PostMessage(WM_APP + 2555,L.GetInt(1),L.GetInt(2));		
	return 0;
}//*/
// =========================================================================
static int luaEditor_AddObjectTemplate(lua_State* state)
{	
	//Script L(state);
	//CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	//if (pWnd)
	//	pWnd->AddObjectTemplate(L.GetString(1),L.GetString(2),L.GetString(3),2);
	return 0;
}//*/
// =========================================================================
static int luaEditor_OutputText(lua_State* state)
{	
	Script L(state);
#ifdef EDITOR
    EditorLog(L.GetString(1));
    EditorLog("\n");
#endif
	return 0;
}//*/
// =========================================================================
static const luaL_reg editor_lib[] = {
	{"OnMsg", luaEditor_OnMsg},  
	{"AddObjectTemplate", luaEditor_AddObjectTemplate},  	
	{"OutputText", luaEditor_OutputText},  			
	{"PostMessage", luaEditor_PostMessage},  				
	{NULL, NULL}
};//*/
// =========================================================================
static int luaGetPainkillerVersionString(lua_State* state)
{
	Script L(state);
	char ver[] = { '1', '.', '4', 0 };
	L.PushString( ver );
    return 1;
}
// =========================================================================
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
static int luaDSConsole_AddMessage(lua_State* state)
{
	Script L(state);
	if (theApp.Dedicated)
	{
		theApp.m_dlgDedicatedServer.m_dlgOutput.PrintText(HUD::StripColorInfo(L.GetString(1)).AsChar());
		theApp.m_dlgDedicatedServer.m_dlgOutput.PrintText("\n");
	}
	return 0;
}
// =========================================================================
static int luaDSConsole_SetCurrText(lua_State* state)
{
	Script L(state);
	if (theApp.Dedicated)
	{
		theApp.m_dlgDedicatedServer.m_dlgOutput.m_strCmd = L.GetString(1);
		theApp.m_dlgDedicatedServer.m_dlgOutput.UpdateData(false);
		CEdit *pEdit = (CEdit *)theApp.m_dlgDedicatedServer.m_dlgOutput.GetDlgItem(IDC_CMD);
		if (pEdit)
			pEdit->SetSel(255,255);

	}
	return 0;
}
#endif
extern int luaIsCDCheckEnabled(lua_State *state);
// =========================================================================
#pragma warning( default : 4311 4312 4244 4800)
// =========================================================================
void ExportToLua()
{
	// overrides
	GScript.RegisterFunction("_ALERT",luaErrorMsgBox);
	GScript.RegisterFunction("MsgBox",luaMsgBox);	
	GScript.RegisterFunction("Exit",luaExit);	
	GScript.RegisterFunction("DoFile",luaDoFile);	
	GScript.RegisterFunction("GetPainkillerVersionString",luaGetPainkillerVersionString);
	GScript.RegisterFunction("IsCDCheckEnabled",luaIsCDCheckEnabled);
	GScript.RegisterLibrary("EDITOR",editor_lib);	
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
	GScript.RegisterFunction("DSConsole_AddMessage",luaDSConsole_AddMessage);
	GScript.RegisterFunction("DSConsole_SetCurrText",luaDSConsole_SetCurrText);
#endif
}
// =========================================================================
