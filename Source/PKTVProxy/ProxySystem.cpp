
#include "Engine.h"
#include "ProxySystem.h"
#include "ConfigFile.h"
#include "SystemDriver.h"
#include "LogBuffer.h"
#include "Console.h"
#include "PKTVManager.h"

#define INI_NAME "PKProxy.ini"

char* Commands[] =
{
	"autorecord",
	"ar",
	"clientpassword",
	"cp",
	"commands",
	"config",
	"connect",
	"c",
	"delay",
	"d",
	"exit",
	"x",
	"idlemessage",
	"idle",
	"i",
	"interface",
	"help",
	"h",
	"maxclients",
	"mc",
	"maxfps",
	"mesg",
	"m",
	"mesgs",
	"nodispatch",
	"nd",
	"noproxydispatch",
	"npd",
	"play",
	"p",
	"proxypassword",
	"pp",
	"quit",
	"q",
	"record",
	"r",
	"rec",
	"saveini",
	"sv",
	"sourcepassword",
	"sp",
	"stats",
	"s",
	"stoprecord",
	"stoprec",
	"tcpport",
	"udpport",
	0
};

int GetTabCompletion( String toComplete, DynamicArray<String>& toReturn )
{
	int fitting = 0;
	toReturn.Clear();

	for( int i = 0; Commands[i]; ++i )
	{
		int j = 0;
		while( ( Commands[i][j] == toComplete[j] ) && toComplete[j] )
			++j;
		if( fitting < j )
		{
			toReturn.Clear();
			fitting = j;
			toReturn.Add( Commands[i] );
		}
		else if( fitting > j )
			continue;
		else
			toReturn.Add( Commands[i] );
	}
	return fitting;
}

ProxySystem::ProxySystem( void )
:	mTimeSum( 0.0 )
,	MinSecondsPerFrame( 0.033 )
,	PKTV(0)
,	mQuit( false )
,	InterfaceToUse("")
,	NoClientDispatch( false )
,	NoProxyDispatch( false )
,	SourcePassword( "" )
,	PasswordForProxies( "" )
,	PasswordForClients( "" )
,	MaxClients( 100 )
,	BroadcastDelayMS( 0 )
,	BufferTime( 10000 )
,	IdleMessage( "No program" )
,	AutoRecordOnConnect( false )
,	AutoRecordFilename( "demo.pktv" )
,	ListeningUDPPort( DEFAULT_LISTENING_UDP_PORT )
,	DisplayViewersMesgs( false )
{
	Console		= new CursConsole;
	Console->Init();

	GLog		= new LogBuffer("PKTVProxy.log");
	GLog->SetConsole( Console );

#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog	= new LogBuffer("PKTVProxyDebug.log");
	PKTVDLog->SetConsole( Console );
#endif

	SysDriver	= new SystemDriver;

	SysDriver->Initialize();
#ifdef _DEBUG
	SysDriver->Test();
#endif
	GLog->Print("PKTV Log started: %s\n", SysDriver->GetTimeAsString() );
	SysDriver->Dump( *GLog );
}

ProxySystem::~ProxySystem( void )
{
	if( PKTV ) delete PKTV;	PKTV = 0;

	delete SysDriver;	SysDriver = 0;

#ifdef PKTV_CONNECTION_DEBUG
	delete PKTVDLog;	PKTVDLog = 0;
#endif
	delete GLog;		GLog = 0;
	Console->Release();
	delete Console;		Console = 0;
}

void ProxySystem::LoadIniFile( void )
{
	ConfigFile cfg;
	if( cfg.Load( String("PKProxy.ini") ) )
	{
		const char* s;
		const char* section = "General";

		s = cfg.GetString( section, "BroadcastDelayMS" );
		if( s )
		{
			long32 bdms = atol( s );
			if( bdms < 0 ) bdms = 0;
			BroadcastDelayMS = bdms;
		}

		s = cfg.GetString( section, "BufferTimeMS" );
		if( s )
		{
			long32 bt = atol( s );
			if( bt < 0 ) bt = 0;
			BufferTime = bt;
		}

		s = cfg.GetString( section, "ListeningUDPPort" );
		if( s )
			ListeningUDPPort = (unsigned short)(atol( s ));

		s = cfg.GetString( section, "MaxTickFPS" );
		if( s )
		{
			double maxfps = atof( s );
			if( maxfps < 1.0 ) maxfps = 1.0;
			MinSecondsPerFrame = 1.0/maxfps;
		}

		s = cfg.GetString( section, "InterfaceToUse" );
		if( s )
			InterfaceToUse = s;

		s = cfg.GetString( section, "IdleMessage" );
		if( s )
			IdleMessage = s;

		s = cfg.GetString( section, "AutoRecordOnConnect" );
		if( s )
		{
			if( ( ::strcmp( s, "true" ) == 0 ) || ( ::strcmp( s, "True" ) == 0 ) || ( ::strcmp( s, "TRUE" ) == 0 ) || ( ::strcmp( s, "1" ) == 0 ) )
				AutoRecordOnConnect = true;
			else if( ( ::strcmp( s, "false" ) == 0 ) || ( ::strcmp( s, "False" ) == 0 ) || ( ::strcmp( s, "FALSE" ) == 0 ) || ( ::strcmp( s, "0" ) == 0 ) )
				AutoRecordOnConnect = false;
			else
				GLog->Print( "AutoRecordOnConnect undefined in config file, assumming '%s'\n", AutoRecordOnConnect?"true":"false" );
		}

		s = cfg.GetString( section, "AutoRecordFilename" );
		if( s && ::strlen(s) )
			AutoRecordFilename = s;

		s = cfg.GetString( section, "NoClientDispatch" );
		if( ( ::strcmp( s, "true" ) == 0 ) || ( ::strcmp( s, "True" ) == 0 ) || ( ::strcmp( s, "TRUE" ) == 0 ) || ( ::strcmp( s, "1" ) == 0 ) )
			NoClientDispatch = true;
		else if( ( ::strcmp( s, "false" ) == 0 ) || ( ::strcmp( s, "False" ) == 0 ) || ( ::strcmp( s, "FALSE" ) == 0 ) || ( ::strcmp( s, "0" ) == 0 ) )
			NoClientDispatch = false;
		else
			GLog->Print( "NoClientDispatch undefined in config file, assumming '%s'\n", NoClientDispatch?"true":"false" );

		s = cfg.GetString( section, "NoProxyDispatch" );
		if( ( ::strcmp( s, "true" ) == 0 ) || ( ::strcmp( s, "True" ) == 0 ) || ( ::strcmp( s, "TRUE" ) == 0 ) || ( ::strcmp( s, "1" ) == 0 ) )
			NoProxyDispatch = true;
		else if( ( ::strcmp( s, "false" ) == 0 ) || ( ::strcmp( s, "False" ) == 0 ) || ( ::strcmp( s, "FALSE" ) == 0 ) || ( ::strcmp( s, "0" ) == 0 ) )
			NoProxyDispatch = false;
		else
			GLog->Print( "NoProxyDispatch undefined in config file, assumming '%s'\n", NoProxyDispatch?"true":"false" );

		s = cfg.GetString( section, "MaxClients" );
		if( s )
			MaxClients = atol( s );

		section = "Passwords";
		s = cfg.GetString( section, "SourcePassword" ); if( s ) SourcePassword = s;
		s = cfg.GetString( section, "PasswordForProxies" ); if ( s ) PasswordForProxies = s;
		s = cfg.GetString( section, "PasswordForClients" ); if( s ) PasswordForClients = s;
		GLog->Print( "Config file loaded succesfully." );
	}
}

void ProxySystem::SaveIniFile( void )
{
	ConfigFile cfg;
	String str;

	const char* section = "General";
	str = String::Sprintf( "%d", BroadcastDelayMS );
	cfg.AddOption( section, "BroadcastDelayMS", str.AsChar() );
	str = String::Sprintf( "%d", BufferTime );
	cfg.AddOption( section, "BufferTimeMS", str.AsChar() );
	str = String::Sprintf( "%d", ListeningUDPPort );
	cfg.AddOption( section, "ListeningUDPPort", str.AsChar() );
	str = String::Sprintf( "%f", 1.0/MinSecondsPerFrame );
	cfg.AddOption( section, "MaxTickFPS", str.AsChar() );
	cfg.AddOption( section, "InterfaceToUse", InterfaceToUse.AsChar() );
	cfg.AddOption( section, "IdleMessage", IdleMessage.AsChar() );
	str = AutoRecordOnConnect?"true":"false";
	cfg.AddOption( section, "AutoRecordOnConnect", str.AsChar() );
	cfg.AddOption( section, "AutoRecordFilename", AutoRecordFilename.AsChar() );
	str = NoClientDispatch?"true":"false";
	cfg.AddOption( section, "NoClientDispatch", str.AsChar() );
	str = NoProxyDispatch?"true":"false";
	cfg.AddOption( section, "NoProxyDispatch", str.AsChar() );
	str = String::Sprintf( "%d", MaxClients );
	cfg.AddOption( section, "MaxClients", str.AsChar() );
	section = "Passwords";
	cfg.AddOption( section, "SourcePassword", SourcePassword.AsChar() );
	cfg.AddOption( section, "PasswordForProxies", PasswordForProxies.AsChar() );
	cfg.AddOption( section, "PasswordForClients", PasswordForClients.AsChar() );

	cfg.Save( String(INI_NAME) );

	GLog->Print( "Config file saved succesfully." );
}

bool ProxySystem::Tick( void )
{
	if( !PKTV )
		PKTV = new PKTVManager( BroadcastDelayMS, ListeningUDPPort, MaxClients, InterfaceToUse );

	mTimeSum += SysDriver->GetTimeStep();

	if( mTimeSum < MinSecondsPerFrame )
	{
		SysDriver->ExactSleep(1);
		return !mQuit;
	}
	mTimeSum = 0;

	PKTV->Tick();
	Console->Update();
	Console->ExecLastCommand();
	return !mQuit;
}

void ProxySystem::ExecuteCommand( String command )
{
	DynamicArray<String> tokens;
	String::TokenizeWithLiteral( command.AsChar(), " ", tokens, '"' );

	if( !tokens.Size() )
		return;

	bool help = false;

	if( ( tokens[0] == "help" ) || ( tokens[0] == "h" ) )
	{
		for( int i = 0; i < tokens.Size()-1; ++i )
			tokens[i] = tokens[i+1];
		tokens.RemoveLast();
		help = true;

		if( !tokens.Size() )
		{
			GLog->Print( "Type 'help <command>' to get info about a command.\n" );
			GLog->Print( "Type 'commands' to get a command list\n\n" );
			return;
		}
	}

	if( ( tokens[0] == "q" ) || ( tokens[0] == "x" ) || ( tokens[0] == "quit" ) || ( tokens[0] == "exit" ) )
	{
		if( help ) { GLog->Print( "quits/exits the program\n\n" ); return; }
		mQuit = true;
	}
	else if( ( tokens[0] == "connect" ) || ( tokens[0] == "c" ) )
	{
		if( help || ( tokens.Size() == 1 ) ) { GLog->Print( "connect <server[:port]> [password] connects to the given server or proxy\n\n" ); return; }

		if( tokens.Size() > 2 )
			SourcePassword = tokens[2];

		PKTV->Connect( tokens[1].AsChar(), SourcePassword.AsChar() );
	}
	else if( ( tokens[0] == "help" ) || ( tokens[0] == "h" ) )
	{
		if( help ) { GLog->Print( "prints information about the program\n\n" ); return; }
	}
	else if( tokens[0] == "commands" )
	{
		if( help ) { GLog->Print( "prints the list of commands\n\n" ); return; }
		GLog->Print( "COMMANDS\n\n" );
		GLog->Print( "autorecord(ar),clientpassword(cp),commands,config,connect(c),delay(d),exit(x),idlemessage(idle,i),interface,help(h),maxclients(mc),maxfps,mesg(m),mesgs,nodispatch(nd),noproxydispatch(npd),play(p),proxypassword(pp),quit(q),record(r,rec),saveini(sv),sourcepassword(sp),stats(s),stoprecord(stoprec),udpport" );
	}
	else if( ( tokens[0] == "interface" ) )
	{
		if( help ) { GLog->Print( "interface <interface> lets you choose on which network interface the proxy should operate. Type 'interface' to find out which interface it is using now. 'interface default' makes the proxy use the default interface supplied by the system\n\n" ); return; }

		if( tokens.Size() > 1 )
		{
			InterfaceToUse = tokens[1];
			if( InterfaceToUse == "default" )
				InterfaceToUse = "";
			GLog->Print( "Interface to be used by proxy will be '%s' when you save settings and restart proxy.\n", InterfaceToUse );
		}

		if( InterfaceToUse.IsNull() )  //!InterfaceToUse.Len() || ( InterfaceToUse == "" ) )
			GLog->Print( "Currently set default interface\n" );
		else
			GLog->Print( "Currently set interface '%s'\n", InterfaceToUse );

		String interf = PKTV->GetInterface();
		if( interf != InterfaceToUse )
		{
			if( InterfaceToUse.IsNull() )  //!InterfaceToUse.Len() || ( InterfaceToUse == "" ) )
				GLog->Print( "Using default interface\n" );
			else
				GLog->Print( "Using interface '%s'\n", InterfaceToUse );
			GLog->Print( "Save settings and reset to change interface\n" );
		}
	}
	else if( ( tokens[0] == "play" ) || ( tokens[0] == "p" ) )
	{
		if( help || ( tokens.Size() == 1 ) ) { GLog->Print( "play <filename> plays back the demo recorded in a file\n\n" ); return; }
		PKTV->PlayFile( tokens[1].AsChar() );
	}
	else if( ( tokens[0] == "idle" ) || ( tokens[0] == "idlemessage" ) || ( tokens[0] == "i" ) )
	{
		// Set message to send when not sending anything else
		if( help ) { GLog->Print( "idlemessage <message> sets the default message that is given to the clients when there's no TV program ;)\n\n" ); return; }
		IdleMessage = ( tokens.Size() == 1 )?"":tokens[1];
		GLog->Print( "Idle Message now: '%s'\n\n", IdleMessage.AsChar() );
	}
	else if( ( tokens[0] == "sp" ) || ( tokens[0] == "sourcepassword" ) )
	{
		// Set source password ( the one we give to our source )
		if( help ) { GLog->Print( "sourcepassword <password> sets the password that is given to our stream source (server or other proxy) on connection\n\n" ); return; }
		SourcePassword = ( tokens.Size() == 1 )?"":tokens[1];
		GLog->Print( "Source Password now: '%s'\n\n", SourcePassword.AsChar() );
	}
	else if( ( tokens[0] == "pp" ) || ( tokens[0] == "proxypassword" ) )
	{
		// Set password for other proxies ( they have to give it when connecting to us )
		if( help ) { GLog->Print( "proxypassword <password> sets the password that other proxies have to give us when connecting to us\n\n" ); return; }
		PasswordForProxies = ( tokens.Size() == 1 )?"":tokens[1];
		GLog->Print( "Password For Proxies now: '%s'\n\n", PasswordForProxies.AsChar() );
	}
	else if( ( tokens[0] == "cp" ) || ( tokens[0] == "clientpassword" ) )
	{
		// Set password for other clients ( they have to give it when connecting to us )
		if( help ) { GLog->Print( "clientpassword <password> sets the password that other clients have to give us when connecting to us\n\n" ); return; }
		PasswordForClients = ( tokens.Size() == 1 )?"":tokens[1];
		GLog->Print( "Password For Clients now: '%s'\n\n", PasswordForClients.AsChar() );
	}
	else if( ( tokens[0] == "mc" ) || ( tokens[0] == "maxclients" ) )
	{
		if( help ) { GLog->Print( "maxclients <number> lets you limit the number of client that can connect directly to this proxy" ); return; }
		if( tokens.Size() > 1 )
		{
			MaxClients = atoi( tokens[1].AsChar() );
			PKTV->SetMaxClients( MaxClients );
		}
		GLog->Print( "Maximal number of clients is now %d\n", PKTV->GetMaxClients() );
	}
	else if( ( tokens[0] == "record" ) || ( tokens[0] == "r" ) || ( tokens[0] == "rec" ) )
	{
		if( help || ( tokens.Size() == 1 ) ) { GLog->Print( "record <filename> starts recording the stream in the given file\n\n" ); return; }
		PKTV->RecordStart( tokens[1].AsChar() );
	}
	else if( ( tokens[0] == "stoprecord" ) || ( tokens[0] == "stoprec" ) )
	{
		if( help ) { GLog->Print( "stoprecord <filename> stops recording the stream\n\n" ); return; }
		PKTV->RecordStop();
	}
	else if( ( tokens[0] == "d" ) || ( tokens[0] == "delay" ) )
	{
		if( help ) { GLog->Print( "delay <ms> lets you delay the proxy broadcast by the given number of miliseconds" ); return; }
		if( tokens.Size() > 1 )
		{
			long32 bdms = atol( tokens[1].AsChar() );
			if( bdms < 0 ) bdms = 0;
			BroadcastDelayMS = bdms;
		}

		GLog->Print( "Delay is now %d ms\n", BroadcastDelayMS );
	}
	else if( ( tokens[0] == "bt" ) || ( tokens[0] == "buffertime" ) )
	{
		if( help ) { GLog->Print( "buffertime <ms> sets the number of ms for buffering. If the proxy had no data to retransmit and paused transmission, it will renew the transmission when it has data from more than period set here" ); return; }
		if( tokens.Size() > 1 )
		{
			DWORD bt = atol( tokens[1].AsChar() );
			if( bt < 0 ) bt = 0;
			PKTV->SetBufferTime( bt );
		}

		BufferTime = PKTV->GetBufferTime();
		GLog->Print( "Buffering time is now %d ms\n", BufferTime );
	}
	else if( ( tokens[0] == "nd" ) || ( tokens[0] == "nodispatch" ) )
	{
		if( help ) { GLog->Print( "nodispatch [true/false] allows/disallows the proxy to redirect clients to connect to other clients. If true, client can connect only to proxy" ); return; }
		if( tokens.Size() > 1 )
		{
			if( tokens[1] == "true" )
				NoClientDispatch = true;
			else if( tokens[1] == "false" )
				NoClientDispatch = false;
			else
				GLog->Print( "Unrecognized value. Use 'true' or 'false'." );
		}
		GLog->Print( "NoClientDispatch is now %s\n", NoClientDispatch?"true":"false" );
	}
	else if( ( tokens[0] == "npd" ) || ( tokens[0] == "noproxydispatch" ) )
	{
		if( help ) { GLog->Print( "noproxydispatch [true/false] allows/disallows the proxy to redirect clients to connect to other proxies." ); return; }
		if( tokens.Size() > 1 )
		{
			if( tokens[1] == "true" )
				NoProxyDispatch = true;
			else if( tokens[1] == "false" )
				NoProxyDispatch = false;
			else
				GLog->Print( "Unrecognized value. Use 'true' or 'false'." );
		}
		GLog->Print( "NoProxyDispatch is now %s\n", NoProxyDispatch?"true":"false" );
	}
	else if( ( tokens[0] == "ar" ) || ( tokens[0] == "autorecord" ) )
	{
		if( help ) { GLog->Print( "autorecord [true/false] [filename] sitches the ability to automatically start recording on connect to proxy. The optional filename setting can be used to change the name of file to autorecord to." ); return; }
		if( tokens.Size() > 1 )
		{
			tokens[1].ToLower();
			if( tokens[1] == "true" )
				AutoRecordOnConnect = true;
			else if( tokens[1] == "false" )
				AutoRecordOnConnect = false;
			else
				GLog->Print( "Unrecognized value. Use 'true' or 'false'." );
			if( tokens.Size() > 2 )
				AutoRecordFilename = tokens[2];
		}
		GLog->Print( "AutoRecordOnConnect is now %s\n", AutoRecordOnConnect?"true":"false" );
		if( AutoRecordOnConnect )
			GLog->Print( "AutoRecordFilename is now '%s'\n", AutoRecordFilename.AsChar() );
	}
	else if( ( tokens[0] == "saveini" ) || ( tokens[0] == "sv" ) )
	{
		if( help ) { GLog->Print( "saveini saves current proxy settings in the %s file\n\n", INI_NAME ); return; }
		SaveIniFile();
	}
	else if( ( tokens[0] == "stats" ) || ( tokens[0] == "s" ) )
	{
		if( help ) { GLog->Print( "This command gives you all kind of proxy stats\n\n" ); return; }
		PrintProxyStats();
	}
	else if( tokens[0] == "config" )
	{
		if( help ) { GLog->Print( "This command gives you the current proxy configuration\n\n" ); return; }
		PrintProxyConfig();
	}
	else if( tokens[0] == "mesgs" )
	{
		if( help ) { GLog->Print( "Toggles display of chat messages from other players\n\n" ); return; }
		DisplayViewersMesgs = !DisplayViewersMesgs;
	}
	else if( ( tokens[0] == "m" ) || ( tokens[0] == "mesg" ) )
	{
		if( help ) { GLog->Print( "Sends the chat message to the viewers watching the PKTV\n\n" ); return; }
		String mesg = ( tokens[0] == "m" )?command.SubStr( command.Len()-2,2 ):command.SubStr( command.Len()-5,5 );
		PKTV->AddMessage( mesg );
	}
	else if( tokens[0] == "maxfps" )
	{
		if( help ) { GLog->Print( "maxfps <fps> sets local proxy max fps. Lower FPS lowers the processor use, but causes reaction delays and lowers transfer speeds. It's an advanced command - don't use it unless you know what you're doing." ); return; }
		if( tokens.Size() > 1 )
		{
			double maxfps = atof( tokens[1].AsChar() );
			if( maxfps < 1.0 )
			{
				GLog->Print( "Minimal maxfps is 1.0, setting to 1.0\n" );
				maxfps = 1.0;
			}
			MinSecondsPerFrame = 1.0/maxfps;
		}

		GLog->Print( "Maxfps is now %f\n", 1.0/MinSecondsPerFrame );
	}
	else if( tokens[0] == "listdb" )
		PKTV->ListDB();
	else if( ( tokens[0] == "udpport" ) )
	{
		if( help ) { GLog->Print( "udpport <port> lets you set the port on which UDP listening socket waits for request packets" ); return; }
		if( tokens.Size() > 1 )
		{
			ListeningUDPPort = (unsigned short) atol( tokens[1].AsChar() );
			GLog->Print( "Listening UDP port will be %d when you save settings and restart proxy.\n", ListeningUDPPort );
		}
		else
			GLog->Print( "Listening UDP port is now %d.\n", ListeningUDPPort );
	}

	// HINT: add new commands here
}

void ProxySystem::PrintProxyConfig( void )
{
	// Prepare
	String bools;
	bools += NoClientDispatch?"No client dispatch":"Client dispatch";
	bools += NoProxyDispatch?", no proxy dispatch":", proxy dispatch";
	bools += AutoRecordOnConnect?", auto record on connect":"";
	bools += DisplayViewersMesgs?", displaying mesgs":", not displaying mesgs";
	String interf = InterfaceToUse.Len()?String("'")+InterfaceToUse+"'":"system default";

	// Print
	GLog->Print( "CURRENT PROXY CONFIGURATION\n" );
	GLog->Print( "========================================================\n" );
	GLog->Print( bools.AsChar() );
	GLog->Print( "Interface to use: %s\n", interf.AsChar() );
	GLog->Print( "Source password: '%s'\n", SourcePassword.AsChar() );
	GLog->Print( "Password for proxies: '%s'\n", PasswordForProxies.AsChar() );
	GLog->Print( "Password for clients: '%s'\n", PasswordForClients.AsChar() );
	GLog->Print( "Maximal number of clients: %u\n", MaxClients );
	GLog->Print( "Broadcast delay: %d ms (%.2f sec)'\n", BroadcastDelayMS, ((float)BroadcastDelayMS)/1000.0f );
	GLog->Print( "Buffer time: %d ms (%.2f sec)'\n", BufferTime, ((float)BufferTime)/1000.0f );
	GLog->Print( "Maximal proxy FPS: %.2f\n", 1.0f/MinSecondsPerFrame );
	GLog->Print( "Idle message: '%s'\n", IdleMessage.AsChar() );
	GLog->Print( "UDP Port: %d\n", ListeningUDPPort );
	GLog->Print( "Auto record filename: %s\n", AutoRecordFilename.AsChar() );
	GLog->Print( "========================================================\n" );
}

void ProxySystem::PrintProxyStats( void )
{
	// Prepare
	String numOfClients = String::Sprintf( "%d", PKTV->GetNumberOfClients() );
	if( !PKTV->GetNumberOfClients() ) numOfClients = "no";

	String numOfProxies = String::Sprintf( "%d", PKTV->GetNumberOfProxies() );
	if( !PKTV->GetNumberOfProxies() ) numOfProxies = "no";

	String modeStr;
	PKTVManager::PKTVMode mode = PKTV->GetWorkMode();
	if( mode == PKTVManager::PROXY_IDLE )
		modeStr = "Work mode: Idle\n";
	else if( mode == PKTVManager::PROXY_PLAYING_FILE )
		modeStr = "Work mode: Playing file\n";
	else
		modeStr = "Work mode: Streaming from server\n";

	// Display
	GLog->Print( "CURRENT PROXY STATS\n" );
	GLog->Print( "========================================================\n" );
	GLog->Print( modeStr.AsChar() );
	GLog->Print( "Currently connected: %s clients, %s proxies\n", numOfClients.AsChar(), numOfProxies.AsChar() );
	GLog->Print( "UDP socket:" );
	DlUl dlul = PKTV->GetOverallUDPStats();
	GLog->Print( "    overall incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
	GLog->Print( "    overall outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
	dlul = PKTV->GetLatestUDPStats();
	GLog->Print( "    latest incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
	GLog->Print( "    latest outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
	if( PKTV->GetNumberOfClients() )
	{
		GLog->Print( "Client TCP sockets:" );
		dlul = PKTV->GetOverallClientTCPStats();
		GLog->Print( "    overall incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    overall outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
		dlul = PKTV->GetLatestClientTCPStats();
		GLog->Print( "    latest incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    latest outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
	}
	if( PKTV->GetNumberOfProxies() )
	{
		GLog->Print( "Proxy TCP sockets:" );
		dlul = PKTV->GetOverallProxyTCPStats();
		GLog->Print( "    overall incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    overall outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
		dlul = PKTV->GetLatestProxyTCPStats();
		GLog->Print( "    latest incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    latest outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
	}
	if( PKTV->HasSource() )
	{
		GLog->Print( "Source TCP socket:" );
		dlul = PKTV->GetOverallSourceTCPStats();
		GLog->Print( "    overall incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    overall outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
		dlul = PKTV->GetLatestSourceTCPStats();
		GLog->Print( "    latest incoming: %u packets (%u bytes)\n", dlul.PacketsDL, dlul.BytesDL );
		GLog->Print( "    latest outgoing: %u packets (%u bytes)\n", dlul.PacketsUL, dlul.BytesUL );
	}
}
