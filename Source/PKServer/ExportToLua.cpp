#include <time.h>
#include "Engine.h"
#include "Render/HUD.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Render/Video.h"
#include "Render/View.h"

#include "Console.h"

void EditorLog(const char *txt);
#pragma warning( disable : 4311 4312 4244 4800)

// =========================================================================
// STANDARD
// =========================================================================
static int luaMsgBox(lua_State* state)
{
	Script L(state);
	printf( "Script Message: %s\n", L.GetString(1) );
	return 0;
}
// =========================================================================
static int luaDoFile(lua_State* state)
{
	Script L(state);
    L.DoFile(L.GetString(1),L.GetBool(2,1));
	return 0;
}
// =========================================================================
static int luaErrorMsgBox(lua_State* state)
{
	Script L(state);

	printf( "Script Error: %s\n", L.GetString(1) );

	GScript.Release();
	delete GEngine;
	exit(0);

	return 0;
}
// =========================================================================
static int luaExit(lua_State* state)
{
	GScript.Release();
	delete GEngine;
	exit( 0 );
	return 0;
}
// =========================================================================
// EDITOR
// =========================================================================
static int luaEditor_OnMsg(lua_State* state)
{	
	Script L(state);
	return 0;
}
// =========================================================================
static int luaEditor_PostMessage(lua_State* state)
{	
	return 0;
}
// =========================================================================
static int luaEditor_AddObjectTemplate(lua_State* state)
{	
	return 0;
}
// =========================================================================
static int luaEditor_OutputText(lua_State* state)
{	
	Script L(state);
//	printf( "%s\n", L.GetString(1) );
	return 0;
}
// =========================================================================
static const luaL_reg editor_lib[] = {
	{"OnMsg", luaEditor_OnMsg},  
	{"AddObjectTemplate", luaEditor_AddObjectTemplate},  	
	{"OutputText", luaEditor_OutputText},  			
	{"PostMessage", luaEditor_PostMessage},  				
	{NULL, NULL}
};
// =========================================================================
static int luaGetPainkillerVersionString(lua_State* state)
{
	Script L(state);
	char ver[] = { '1', '.', '4', 0 };
	L.PushString( ver );
    return 1;
}
extern CursConsole gConsole;
// =========================================================================
static int luaDSConsole_AddMessage(lua_State* state)
{
	Script L(state);
/*	if (theApp.Dedicated)
	{
		theApp.m_dlgDedicatedServer.m_dlgOutput.PrintText(HUD::StripColorInfo(L.GetString(1)).AsChar());
		theApp.m_dlgDedicatedServer.m_dlgOutput.PrintText("\n");
	}*/
	gConsole.AddMessage( HUD::StripColorInfo(L.GetString(1)).AsChar() );
	gConsole.AddToBuffer( HUD::StripColorInfo(L.GetString(1)).AsChar() );
	return 0;
}
// =========================================================================
static int luaDSConsole_SetCurrText(lua_State* state)
{
	Script L(state);
/*	if (theApp.Dedicated)
	{
		theApp.m_dlgDedicatedServer.m_dlgOutput.m_strCmd = L.GetString(1);
		theApp.m_dlgDedicatedServer.m_dlgOutput.UpdateData(false);
		CEdit *pEdit = (CEdit *)theApp.m_dlgDedicatedServer.m_dlgOutput.GetDlgItem(IDC_CMD);
		if (pEdit)
			pEdit->SetSel(255,255);

	}*/
	gConsole.SetText( L.GetString( 1 ) );
	return 0;
}
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
	GScript.RegisterLibrary("EDITOR",editor_lib);	
	GScript.RegisterFunction("DSConsole_AddMessage",luaDSConsole_AddMessage);
	GScript.RegisterFunction("DSConsole_SetCurrText",luaDSConsole_SetCurrText);
}
// =========================================================================
