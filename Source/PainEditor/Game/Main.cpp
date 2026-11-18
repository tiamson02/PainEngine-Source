#include "stdafx.h"
#include <time.h>

#include "Engine.h"

#include "Main.h"
#include "Mainfrm.h"
#include "EditorView.h"
#include "PainEditor.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/RenderWindow.h"

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
extern ENGINE_API GFileManager  	GFileManAudio;

#include "Engine/InputSystem.h"
#include "Render/Video.h"

extern ENGINE_API EngineGame* (*OurGame)(void);
extern ENGINE_API View* (*OurView)(void);

static EngineGame* MakeAGame( void ) { return (EngineGame*) new EditorGame; };
static View* MakeAView( void ) { return (View*) new EditorView(); };

void ExportToLua();

EditorGame* Game() { return GEngine?(EditorGame*)GEngine->Game:0; };


static bool enableCDCheck = false;
static void CheckCD();

// =========================================================================

int Game_Init(HINSTANCE hInstance, const char* commandLine)
{ 

	if (!((CPainEditorApp*)AfxGetApp())->Dedicated)
	{
	#ifdef ENABLE_CD_CHECK
		enableCDCheck = true;
		CheckCD();
	#endif
	}

	try {        
		GFileMan.RegisterPack(new GPack(), "../data/textures.pak", "../data/textures/",false);
        GFileMan.RegisterPack(new GPack(), "../data/textures1.pak", "../data/textures/",false);
		GFileMan.RegisterPack(new GPack(), "../data/textures2.pak", "../data/textures/",false);
        GFileMan.RegisterPack(new GPack(), "../data/shaders.pak", "../data/shaders/",false);
        GFileMan.RegisterPack(new GPack(), "../data/maps.pak", "../data/maps/",false);
        GFileMan.RegisterPack(new GPack(), "../data/maps1.pak", "../data/maps/",false);
		GFileMan.RegisterPack(new GPack(), "../data/maps2.pak", "../data/maps/",false);
        GFileMan.RegisterPack(new GPack(), "../data/items.pak", "../data/items/",false);
        GFileMan.RegisterPack(new GPack(), "../data/models.pak", "../data/models/",false);
		GFileMan.RegisterPack(new GPack(), "../data/models1.pak", "../data/models/",false);
        GFileMan.RegisterPack(new GPack(), "../data/fonts.pak", "../data/fonts/",false);

        GFileManAudio.RegisterPack(new GPack(), "../data/sounds.pak", "../data/sounds/",false);
		GFileManAudio.RegisterPack(new GPack(), "../data/sounds1.pak", "../data/sounds/",false);

		GScript.DoFile("../Data/LScripts/Loader.lua");
		GScript.DoString("CfgFile = [[%s]]",(LPCSTR)((CPainEditorApp*)AfxGetApp())->CfgFileName);
        GScript.DoString("Cfg:Load();Tweak:Load()"); // przed odpaleniem video
//		GScript.Globals()["Game"].NewKey("dupa").SetInteger(5);
		//GScript.DoString("Game.dupa = -1");
		//int i = GScript.Globals()["Game"]["dupa"].GetInt();
		//GScript.Globals()["Game"]["dupa"].AssignInteger(5);
//		GScript.DoString("MsgBox(Game.dupa)");

        // start the engine 
		OurGame = MakeAGame;
		OurView = MakeAView;
        GEngine=new PCFSystem();
        if(GEngine) {
			String cmdline = commandLine;
			cmdline += " -game editorgame";
            if( !GEngine->Initialize(hInstance, cmdline.AsChar()) )
                return 0;
		}     
    }
    catch(const char *error) {
        GLog.Close();
		if (error) 
			MessageBox(NULL,error,"Exception Error:",MB_TOPMOST);
		else
			StackTracer::ErrorMessageBox();
		return 0;
    }
    return 1;
}
// =========================================================================
void  Game_Release()
{
	try
	{
        GLog.SetCallbackFunc(NULL);
#ifdef TRANSGAMING
		if (GEngine && GScript.Initialized() && !GScript.Globals()["Game"].IsNil() && GEngine->pcfWorld)
#else
		if (GEngine && GScript.Initialized() && !GScript.Globals()["Game"].IsNil() && GEngine->World)
#endif
			GScript.DoString("Game:Close()");
		GScript.Release();
		if (GEngine)
		{
			delete GEngine;
			GEngine = 0;
		}
    }
    catch(const char *error) {
        GLog.Close();
		if (error) 
			MessageBox(NULL,error,"Exception Error:",MB_TOPMOST);
		else
			StackTracer::ErrorMessageBox();
    }
}
// =========================================================================
void SwitchToFullScreen(bool enable)
{
	String res="640X480";
	float g=1.0f, b=0.5f, c=0.5f;

	if( !GScript.Globals()["Cfg"]["Resolution"].IsNil() )
	{
		res = GScript.Globals()["Cfg"]["Resolution"].GetString();
	}
	GScript.DoString("R3D.ApplyVideoSettings(\"%s\",%s)",res.AsChar(),enable?"true":"false");
}
// =========================================================================
static bool Check();

void CheckCD()
{
	char lang[10];
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, lang, 10 );

	char text[100] = "";

	if( ::strcmp( lang, "Polish" ) == 0 )
		::strcpy( text, "Prosz� w�o�y� odpowiedni CD do nap�du" );
	else if( ::strncmp( lang, "Spanish", 7 ) == 0 )
		::strcpy( text, "Por favor, introduzca el CD-ROM correcto" );
	else if( ::strncmp( lang, "German", 6 ) == 0 )
		::strcpy( text, "Legen Sie die korrekte CD-ROM ein" );
	else if( ::strncmp( lang, "Italian", 7 ) == 0 )
		::strcpy( text, "Inserire il CD-ROM corretto" );
	else if( ::strncmp( lang, "French", 6 ) == 0 )
		::strcpy( text, "Veuillez ins�rer le CD-ROM appropri�" );
	else
		::strcpy( text, "Please insert the correct CD-ROM" );


	do
	{
		if( Check() )
			return;

		if( ( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) &&
			( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) &&
			( GetAsyncKeyState( VK_LMENU ) & 0x8000 ) &&
			( GetAsyncKeyState( 'Z' ) & 0x8000 ) )
		{
			enableCDCheck = false;
			return;
		}
	}
	while( ::MessageBox( NULL, text, "Painkiller", MB_RETRYCANCEL ) != IDCANCEL );

	exit( 0 );
}

bool Check( void )
{
    printf("[main.cpp]::Check(): checking CD label\n");
	char dbits[113];
	GetLogicalDriveStrings( 113, dbits );
	DWORD mf, sf, sno;

	for( int i = 0; *(dbits+i); i+= 4 )
	{
		char vol[256];
		vol[0] = '\0';
		char drive[4];
		strncpy(drive,dbits+i,4);
		if( GetDriveType( drive ) == DRIVE_CDROM )
		{
			GetVolumeInformation( drive, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '1' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '1' ) && ( vol[7] == 0 ) ) ) )
				return true;

			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( vol[3] == 'P' ) &&
				( vol[4] == 'L' ) &&
				( vol[5] == '_' ) &&
				( vol[6] == '1' ) &&
				( vol[7] == 0 ) )
				return true;
		}
	}
	return false;
}

int luaIsCDCheckEnabled(lua_State *state)
{
	Script L(state);
    L.PushBool(enableCDCheck);
    return 1;
}
