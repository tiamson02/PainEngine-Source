#include "Engine.h"
#include "Engine/BaseGame.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
extern ENGINE_API GFileManager  	GFileManAudio;
#include "Engine/PCFSystem.h"
extern ENGINE_API	PCFSystem*	GEngine;

#include "Console.h"


CursConsole gConsole;


class ConsoleGame : public EngineGame
{
public:

	ConsoleGame() {}
	virtual ~ConsoleGame() {}

	void ProcessInput() {}
};


extern ENGINE_API EngineGame* (*OurGame)(void);
static EngineGame* MakeAGame( void ) { return (EngineGame*) new ConsoleGame; }


void ExportToLua();


static bool Init( char *cmdLine )
{
	GScript.Init();
    ExportToLua();

	GFileMan.RegisterPack( new GPack(), "../data/lscripts.pak", "../data/lscripts/", false );
	GFileMan.RegisterPack( new GPack(), "../data/levels.pak", "../data/levels/", false );

	DynamicArray<String> files;
	GFileMan.FindFiles( "../Data/*.pkm", files, EFindFiles );
	for( int i = 0; i < files.Size(); i++ )
	{
		GFileMan.RegisterPack(new GPack(), String( String("../data/") + files[i]) .AsChar(), "../data/",false);
	}

	gConsole.Init();

	return true;
}


static void Quit()
{
	GScript.DoString( "Cfg:Save()" );

	GLog.SetCallbackFunc(NULL);
#ifdef TRANSGAMING
	if( GEngine && GScript.Initialized() && !GScript.Globals()["Game"].IsNil() && GEngine->pcfWorld )
#else
	if( GEngine && GScript.Initialized() && !GScript.Globals()["Game"].IsNil() && GEngine->World )
#endif
		GScript.DoString("Game:Close()");
	GScript.Release();
	if( GEngine )
	{
		delete GEngine;
		GEngine = 0;
	}

	gConsole.Release();
}


static bool ServerStart()
{
	char cfgFile[MAX_PATH];
	sprintf( cfgFile, "config.ini" );

	GScript.Release();
	GScript.Init();
	ExportToLua();

	GScript.DoFile( "../Data/LScripts/Main/Tweak.lua" );
	GScript.DoFile( "../Data/LScripts/Main/Cfg.lua" );
	GScript.DoFile( "../Data/LScripts/Main/Definitions.lua" );
	GScript.DoString( "CfgFile = [[%s]]", cfgFile );
	GScript.DoString( "Cfg:Load();Tweak:Load()" );

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
	GFileMan.RegisterPack( new GPack(), "../data/scripts.pak", "../data/scripts/", false );
	GFileMan.RegisterPack(new GPack(), "../data/fonts.pak", "../data/fonts/",false);

	GFileManAudio.RegisterPack(new GPack(), "../data/sounds.pak", "../data/sounds/",false);
	GFileManAudio.RegisterPack(new GPack(), "../data/sounds1.pak", "../data/sounds/",false);

	GScript.DoFile( "../Data/LScripts/Loader.lua");
	GScript.DoString( "CfgFile = [[%s]]", cfgFile );
	GScript.DoString( "Cfg:Load();Tweak:Load()" );

	char *gameMode = ( char *)( GScript.Globals()["Cfg"]["GameMode"].GetString() );
	char *maps = NULL;
	if( strcmp( gameMode, "Free For All" ) == 0 )
		maps = "ServerMapsFFA";
	else if( strcmp( gameMode, "Team Deathmatch" ) == 0 )
		maps = "ServerMapsTDM";
	else if( strcmp( gameMode, "Voosh" ) == 0 )
		maps = "ServerMapsVSH";
	else if( strcmp( gameMode, "The Light Bearer" ) == 0 )
		maps = "ServerMapsTLB";
	else if( strcmp( gameMode, "People Can Fly" ) == 0 )
		maps = "ServerMapsPCF";
	else if( strcmp( gameMode, "Capture The Flag" ) == 0 )
		maps = "ServerMapsCTF";
	else if( strcmp( gameMode, "Duel" ) == 0 )
		maps = "ServerMapsDUE";
	else if( strcmp( gameMode, "Last Man Standing" ) == 0 )
		maps = "ServerMapsLMS";

	GScript.DoString( "Cfg.ServerMaps = {}; for i=1,table.getn(Cfg.%s) do Cfg.ServerMaps[i] = Cfg.%s[i] end", maps, maps );

	String map = "DM_Sacred";
	int port = 3455;
	ScriptObject cfgMaps = GScript.Globals()["Cfg"]["ServerMaps"];
	if( cfgMaps.GetCount() > 0 )
		map = cfgMaps[1].GetString();
	port = GScript.Globals()["Cfg"]["ServerPort"].GetInt();

	char cmdLine[1024];
	sprintf( cmdLine, "+dedicatedserver +map %s +port %d", map.AsChar(), port );
/*	if (m_dlgGeneral.GetCheckedRadioButton(IDC_PUBLICSRV,IDC_LOCALSRV) == IDC_LOCALSRV) 
		cmd += " +private";*/
	
/*	CString ip;
	m_dlgGeneral.m_cmbNetInterface.GetLBText(m_dlgGeneral.m_cmbNetInterface.GetCurSel(),ip);
	if (ip.GetLength()>5)
		cmd += " +interface " + ip;*/

	// start the engine 
	OurGame = MakeAGame;
	GEngine = new PCFSystem();
	if( GEngine )
	{
		String cmdline = cmdLine;
		cmdline += " -game editorgame";
		if( !GEngine->Initialize( NULL, cmdline.AsChar() ) )
			return false;
	}

	GScript.DoString( "Game_BackupServerSettings()" );

	// przekazuje do lua liste map
/*	String maps = "PainMenu.mapsOnServer = {";
	for (int i=0; i<m_dlgMaps.m_lstSelectedLevels.GetCount(); i++)
	{
		m_dlgMaps.m_lstSelectedLevels.GetText(i,map);
		maps += "'"+ map + "',";
	}
	maps += "}";
*/
//	GScript.DoString(maps);

	GScript.DoString( "PainMenu.mapsOnServer = {}; for i=1,table.getn(Cfg.ServerMaps) do PainMenu.mapsOnServer[i] = Cfg.ServerMaps[i] end" );
	GScript.DoString( "Cfg:Save()" );

	return true;
}


static bool Update()
{
	if( gConsole.Update() )
	{
		GEngine->TickEngine( false );
		gConsole.ExecLastCommand();
		return true;
	}
	else
		return false;
}


int main( int argc, char **argv )
{
	String cmdLine = "";

	for( int i = 0; i < argc; i++ )
	{
		cmdLine += argv[i];
		cmdLine += " ";
	}

	if( !Init( "" ) )
	{
		printf( "Cannot initialize Painkiller.\n" );
		return 0;
	}

	ServerStart();

	while( Update() )
	{
	}

	Quit();

	return 0;
}
