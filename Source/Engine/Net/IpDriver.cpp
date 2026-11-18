#error This file is obselete
//#include "Engine.h"

#ifdef OIUYTREWLKJHGFDS

#include "Net/GameSpyData.h"

// All-Seeing Eye include
extern "C" {
#include "Net/ASE/ASEQuerySDK.h"
}

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Net/UdpMessage.h"
#include "Net/IPChannel.h"
#include "Net/NetworkClient.h"
#include "Net/NetworkDevice.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

// Inclusion of common code
#include "Net/GameSpy2/nonport.h"
#include "Net/GameSpy2/available.h"
// Inclusion of GameSpy Query % Reporting SDK
#include "Net/GameSpy2/qr2/qr2.h"
// Inclusion of GameSpy CDKey SDK
#include "Net/GameSpy2/gcdkey/gcdkeyc.h"
#include "Net/GameSpy2/gcdkey/gcdkeys.h"
// Inclusion of Server Browser SDK
#include "Net/GameSpy2/serverbrowsing/sb_serverbrowsing.h"
// Inclusion of NAT Negotiation SDK
#include "Net/GameSpy2/natneg/natneg.h"

//#include "FileSystem/zlib.h"

#define PACKET_LOSS_PING_COUNT 10

#ifndef MPDEMO
#define CDKEY_ENABLED
#endif

#define CONNECTION_STAGES_LOG

// It's one-way so double of it is on screen
#define SIMULATED_LATENCY_DEFAULT 0
// ---------------------------- PASSWORD SCRAMBLER FUNCTIONS ---------------------------------------------------

bool MoveToBytecode( String input, char* output )
{
	for( int i = 0; i < input.Len(); ++i )
	{
		if( ( input[i] >= 48 ) && ( input[i] <= 57 ) )
			output[i] = input[i] - 48;
		else if( ( input[i] >= 97 ) && ( input[i] <= 122 ) )
			output[i] = input[i] - 87;
		else if( ( input[i] >= 65 ) && ( input[i] <= 92 ) )
			output[i] = input[i] - 29;
		else
			return false;
	}
	return true;
}

bool MoveFromBytecode( char* input, String& output, int length )
{
	output.Clear();
	for( int i = 0; i < length; ++i )
	{
		if( input[i] < 0 ) return false;
		else if( input[i] < 10 ) output += (char)(input[i] + 48);
		else if( input[i] < 36 ) output += (char)(input[i] + 87);
		else if( input[i] < 64 ) output += (char)(input[i] + 29);
		else return false;
	}
	return true;
}


bool Scramble( String password, String challenge, String& output )
{
	if( !password.Len() ) { output = password; return true; };
	Assert( challenge.Len() );

	char passwordBC[256];
	char outputBC[256];

	if( !MoveToBytecode( password, passwordBC ) ) return false;

	char Scrambler1[64], Scrambler2[64];

	unsigned long i, j, t, x;
	char temp, k;
	k = i = j = t = x = 0;
	temp = 0;

	for( i = 0; i < 64; ++i )
		Scrambler1[i] = i;

	j = 0;
	for( i = 0; i < 64; ++i )
	{
		if( j == (unsigned long)challenge.Len() )
			j = 0;
		Scrambler2[i] = challenge[j++];
	}

	j = 0;

	for( i = 0; i < 64; ++i )
	{
		j = (j + (unsigned long)Scrambler1[i] + (unsigned long)Scrambler2[i]) % 64U;
		temp = Scrambler1[i];
		Scrambler1[i] = Scrambler1[j];
		Scrambler1[j] = temp;
	}

	i = j = 0;
	unsigned long ps = (unsigned long)password.Len();
	for( x = 0; x < ps; ++x )
	{
		i = (i + 1U) % 64U;
		j = (j + (unsigned long)Scrambler1[i]) % 64U;

		temp = Scrambler1[i];
		Scrambler1[i] = Scrambler1[j];
		Scrambler1[j] = temp;

		t = ((unsigned long)Scrambler1[i] + (unsigned long)Scrambler1[j]) % 64U;
		k = Scrambler1[t];

		char out = (char)(passwordBC[x]^k);
		Assert( out < 64 );
		outputBC[x] = out;
	}

	return MoveFromBytecode( outputBC, output, password.Len() );
}

// Scrambler test - passed already

/*
	// Test scrambler
	String passwd;
	String challenge;
	String scrambled;
	String output;
	bool failed = false;
	GLog.Print( "Scrambler test started...\n" );
	for( int i = 0; i < 1000; ++i )
	{
		challenge = String::GenerateRandom();
		for( int j = 0; j < 1000; ++j )
		{
			passwd = String::GenerateRandom();
			if( !Scramble( passwd, challenge, scrambled ) )
			{
				GLog.Print( "Scramble test failed on crypting!!! (%s,%s)\n", passwd.AsChar(), challenge.AsChar() );
				failed = true;
				break;
			}
			if( !Scramble( scrambled, challenge, output ) )
			{
				GLog.Print( "Scramble test failed on decrypting!!! (%s,%s)\n", scrambled.AsChar(), challenge.AsChar() );
				failed = true;
				break;
			}
			if( output != passwd )
			{
				GLog.Print( "Scrambler test failed!!! (%s != %s)\n", passwd.AsChar(), output.AsChar() );
				failed = true;
				break;
			}
		}
		if( failed )
			break;
		GLog.Print( "Scrambler test %d/1000 ok\n", i );
	}
	if( !failed )
		GLog.Print( "Scrambler test success!!!\n" );
*/


// ---------------------------- GAMESPY FUNCTIONS ---------------------------------------------------

#ifdef CDKEY_ENABLED
static void StaticGameSpyCDKEYCallback( int productid, int localid, int authenticated, char* errmsg, void* instance )
{
	if( productid == GAMESPY_CDKEY_GAMEID )
		((IPDriver*)instance)->GameSpyCDKEYCallback( localid, authenticated, errmsg );
}
#endif
static void ServerKeyCallback( int keyid, qr2_buffer_t outbuf, void *userdata )
{
	int valueInt;
	String valueString;
	bool isNumber = GEngine->Net->GameSpyInfo.GetServerValue( keyid, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static void PlayerKeyCallback( int keyid, int index, qr2_buffer_t outbuf, void *userdata )
{
	int valueInt;
	String valueString;
	bool isNumber = GEngine->Net->GameSpyInfo.GetPlayerValue( keyid, index, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static void TeamKeyCallback( int keyid, int index, qr2_buffer_t outbuf, void *userdata )
{
	int valueInt;
	String valueString;
	bool isNumber = GEngine->Net->GameSpyInfo.GetTeamValue( keyid, index, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static int PlayerTeamCountCallback( qr2_key_type keytype, void* userdata )
{
	return ( keytype == key_player )?GEngine->Net->GameSpyInfo.GetPlayerCount():GEngine->Net->GameSpyInfo.GetTeamCount();
}

static void KeyListCallback( qr2_key_type keytype, qr2_keybuffer_t keybuffer, void* userdata )
{
	switch( keytype )
	{
	case key_server:
		qr2_keybuffer_add(keybuffer, HOSTNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMEVER_KEY);
//		qr2_keybuffer_add(keybuffer, HOSTPORT_KEY);	// don't touch that!
		qr2_keybuffer_add(keybuffer, MAPNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMETYPE_KEY);
		qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, NUMTEAMS_KEY);
		qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, GAMEMODE_KEY);
		qr2_keybuffer_add(keybuffer, TEAMPLAY_KEY);
		qr2_keybuffer_add(keybuffer, FRAGLIMIT_KEY);
		qr2_keybuffer_add(keybuffer, TIMELIMIT_KEY);
		qr2_keybuffer_add(keybuffer, GAMESPY_MAXSPECTATORS_KEY);
		qr2_keybuffer_add(keybuffer, GAMESPY_PASSWORDED_KEY);
		break;
	case key_player:
		qr2_keybuffer_add(keybuffer, PLAYER__KEY);
		qr2_keybuffer_add(keybuffer, SCORE__KEY);
		qr2_keybuffer_add(keybuffer, DEATHS__KEY);
		qr2_keybuffer_add(keybuffer, PING__KEY);
		qr2_keybuffer_add(keybuffer, TEAM__KEY);
		break;
	case key_team:
		qr2_keybuffer_add(keybuffer, TEAM_T_KEY);
		qr2_keybuffer_add(keybuffer, SCORE_T_KEY);
		break;
	}
}

static void AddErrorCallback( qr2_error_t error, char* errmsg, void* userdata )
{
	String errorText = String::Sprintf( "GameSpy timeout error!!!\nDescription: %s\n", errmsg );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

static void NatNegotiateProgressCallback( NegotiateState state, void* userdata )
{
	String errorText;
	switch( state )
	{
	case ns_initack:		// when the NAT Negotiation server acknowledges your connection request
		errorText = String::Sprintf( "NAT Negotiation: NN server acknowledged your connection request\n" );
		break;
	case ns_connectping:	// when direct negotiation with the other client has started
		errorText = String::Sprintf( "NAT Negotiation: direct negotiation with the other client has started\n" );
		break;
	default:
		errorText = String::Sprintf( "NAT Negotiation: unknown negotiate state returned by GameSpy!! State: %d\n", state );
		break;
	}
	GLog.Print( errorText.AsChar() );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
}

static void RealizeNegotiateCompleted( IPDriver* i, struct sockaddr_in* remoteaddr )
{
	Assert( i->State == IPDriver::NAT_NEGOTIATING );

	i->mServerAddr.IP = remoteaddr->sin_addr.S_un.S_addr;
	i->mServerAddr.Port = ntohs(remoteaddr->sin_port);
	i->mLastConnectTime = GEngine->SysDriver.GetCurrentTime();
	String addr = i->mServerAddr.AsString();
	String errorText = String::Sprintf( "NAT Negotiation complete! Connecting to %s\n", addr.AsChar() );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );

	UdpMessage mesg;
	mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
	mesg.WriteCheckToken( IPD_GETCH );
	mesg.EndWriting();
	i->SendMaintenanceMessage( i->mServerAddr, &mesg );
	i->mLastConnectPackets = 0;
	i->State = IPDriver::CONNECTING;
}

static void RealizeNegotiateFailed( IPDriver* i, struct sockaddr_in* remoteaddr )
{
	Assert( i->State == IPDriver::NAT_NEGOTIATING );
	i->State = IPDriver::INITIALIZED;
}

static void NatNegotiateCompletedCallback( NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata )
{
	IPDriver* ipdriver = (IPDriver*)userdata;

	String errorText;

	switch( result )
	{
		case nr_success:			// Successful negotiation, other parameters can be used to continue communications with the client
			RealizeNegotiateCompleted( ipdriver, remoteaddr );
			return;
		case nr_deadbeatpartner:	// Partner did not register with the NAT Negotiation Server
			errorText = String::Sprintf( "NAT Negotiation error: Partner did not register with the NAT Negotiation Server\n" );
			break;
		case nr_inittimeout:		// Unable to communicate with NAT Negotiation Server
			errorText = String::Sprintf( "NAT Negotiation error: Unable to communicate with NAT Negotiation Server\n" );
			break;
		default:
		case nr_unknownerror:		// NAT Negotiation server indicated an unknown error condition
			errorText = String::Sprintf( "NAT Negotiation error: NAT Negotiation server indicated an unknown error condition\n" );
			break;
	}
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
	RealizeNegotiateFailed( ipdriver, remoteaddr );
}

static void NatNegotiateCompletedCallbackServer( NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata )
{
	String errorText;
	switch( result )
	{
		case nr_success:			// Successful negotiation, other parameters can be used to continue communications with the client
			{
				IpAddress addr;
				addr.IP = remoteaddr->sin_addr.S_un.S_addr;
				addr.Port = ntohs(remoteaddr->sin_port);
				String addrStr = addr.AsString();
				errorText = String::Sprintf( "NAT Negotiation succesfull!: Client connecting from %s\n", addrStr.AsChar() );
			}
			break;
		case nr_deadbeatpartner:	// Partner did not register with the NAT Negotiation Server
			errorText = String::Sprintf( "NAT Negotiation error: Partner did not register with the NAT Negotiation Server\n" );
			break;
		case nr_inittimeout:		// Unable to communicate with NAT Negotiation Server
			errorText = String::Sprintf( "NAT Negotiation error: Unable to communicate with NAT Negotiation Server\n" );
			break;
		default:
		case nr_unknownerror:		// NAT Negotiation server indicated an unknown error condition
			errorText = String::Sprintf( "NAT Negotiation error: NAT Negotiation server indicated an unknown error condition\n" );
			break;
	}
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

static void NatNegotiationCallback( int cookie, void* userdata )
{
	IPDriver* ipdriver = (IPDriver*)userdata;

	NNBeginNegotiationWithSocket( ipdriver->mSocket, cookie, 1, NatNegotiateProgressCallback, NatNegotiateCompletedCallbackServer, userdata );

	String errorText = String::Sprintf( "NAT Negotiation: Received client cookie: %d\n", cookie );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

// ---------------------------- ALL_SEEING EYE FUNCTIONS -------------------------------------------

void ASEQuery_wantstatus(void)
{
	GLog.Print( "WantStatus called by All-Seeing Eye!\n" );

	ASEQuery_status(	GEngine->Net->GameSpyInfo.ServerName.AsChar(),
						GEngine->Net->GameSpyInfo.GameType.AsChar(),
						GEngine->Net->GameSpyInfo.MapName.AsChar(),
						GEngine->Net->GameSpyInfo.GameVersion.AsChar(),
						GEngine->Net->GameSpyInfo.Password.Len()?1:0,
						GEngine->Net->GameSpyInfo.GetPlayerCount(),
						GEngine->Net->GameSpyInfo.MaxPlayers );
}

void ASEQuery_wantrules(void)
{
	GLog.Print( "WantRules called by All-Seeing Eye!\n" );

	String t;

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.GameVariant );
	ASEQuery_addrule( "gamevariant", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.Teamplay );
	ASEQuery_addrule( "teamplay", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.FragLimit );
	ASEQuery_addrule( "fraglimit", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.TeamFragLimit );
	ASEQuery_addrule( "teamfraglimit", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.TimeElapsed );
	ASEQuery_addrule( "timeelapsed", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.TimeLimit );
	ASEQuery_addrule( "timelimit", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.MaxSpectators );
	ASEQuery_addrule( "maxspectators", t.AsChar() );
}

void ASEQuery_wantplayers(void)
{
	GLog.Print( "WantPlayers called by All-Seeing Eye!\n" );

	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( GEngine->Net->GameSpyInfo.Players[i].Valid )
		{
			String score = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.Players[i].Score );
			String ping = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.Players[i].Ping );
			ASEQuery_addplayer( GEngine->Net->GameSpyInfo.Players[i].Name.AsChar(),
								GEngine->Net->GameSpyInfo.Teams[GEngine->Net->GameSpyInfo.Players[i].Team].Name.AsChar(),
								0,
								score.AsChar(),
								ping.AsChar(),
								"" );
		}
}


// ---------------------------- IPAddress methods --------------------------------------------------

inline void IpAddress::FillSockaddrIn( struct sockaddr_in& addr )
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons( Port );
	addr.sin_addr.S_un.S_addr = IP;
}

bool GameSpyBackendTest( void )
{
	// GSIAC test
	GSIACResult result = GSIACWaiting;
	GSIStartAvailableCheck( GAMESPY_GAMENAME );
	do
	{
		GEngine->SysDriver.ExactSleep( 10 );
		result = GSIAvailableCheckThink();
	}
	while( result == GSIACWaiting );
	return ( result == GSIACAvailable );
}

// ---------------------------- IPDriver methods ---------------------------------------------------

IPDriver::IPDriver()
:	mGameSpySDK( false ), State( STARTUP ), IsPublic( false ), mLastConnectPackets(0), mSimulatedLatency(SIMULATED_LATENCY_DEFAULT), mASE_SDK(0)
{
	mSocket = INVALID_SOCKET;
	mConnReset = false;

	// Stats update data
	mLastStatsUpdateTime = 0;
	mStatsUpdateDelay = 1000;
	mLastUpdateBytesSent = mLastUpdateBytesReceived = mCurrentUpdateBytesSent = mCurrentUpdateBytesReceived = 0;

	CDKey = "1234-1234-1234-1234";		// just for now, a random one
}

IPDriver::~IPDriver()
{
	Close();
}

bool IPDriver::Open( unsigned short serverPort, bool server, String interfaceToUse )
{
	if( State != STARTUP )
		Close();

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1,1);

	if( WSAStartup(wVersionRequested, &wsaData ) )
	{
		String errorText = String::Sprintf( "Net Init Error: WSAStartup() failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return false;
	}

	if( ( LOBYTE(wsaData.wVersion) != 1 ) && ( HIBYTE(wsaData.wVersion) != 1 ) )
	{
		String errorText = String::Sprintf( "Net Init Error: Windows Sockets implementation too old!\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		WSACleanup();
		return false;
	}

	in_addr	hostAddr;
	hostAddr.S_un.S_addr = INADDR_ANY;
	if( server )
	{
		if( interfaceToUse.Len() )
		{
			IpAddress temp;
			if( !Resolve( interfaceToUse.AsChar(), temp ) )
			{
				String errorText = String::Sprintf( "Net Init Error: tried to resolve given interface and failed!\n" );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
				WSACleanup();
				return false;
			}
			else
				hostAddr.S_un.S_addr = temp.IP;
		}
		else
		{
			char hostnam[256];

			if( gethostname( hostnam, 255 ) )
			{
				String errorText = String::Sprintf( "Net Init Error: gethostname() call failed!! Can't find this computer's name! Returned: %s\n", GetSocketErrorString().AsChar() );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
				WSACleanup();
				return false;
			}

			hostent* host = gethostbyname( hostnam );
			if( !host )
			{
				String errorText = String::Sprintf( "Net Init Error: failed to resolve our IP with gethostbyname()!!\n" );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
				WSACleanup();
				return false;
			}

			hostAddr = *(in_addr*)(*host->h_addr_list);
		}
	}

	// Set default server connection on listening
	SOCKET sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( INVALID_SOCKET == sock )
	{
		String errorText = String::Sprintf( "Net Init Error: Couldn't open listening socket: %s\n", GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		WSACleanup();
		return false;
	}

	// non-blocking socket
	unsigned long one = 1;
	if( ioctlsocket( sock, FIONBIO, &one ) == -1)
	{
		String errorText = String::Sprintf( "Net Init Error: Couldn't make socket non-blocking: %s\n", GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		closesocket( sock );
		WSACleanup();
		return false;
	}

	// Win2k creeping ping fix
	{
		#define IOC_VENDOR 0x18000000
		#define SIO_UDP_CONNRESET (IOC_IN|IOC_VENDOR|12)

		DWORD noUdpConnReset = FALSE;
		if( ioctlsocket( sock, SIO_UDP_CONNRESET, &noUdpConnReset ) == SOCKET_ERROR )
			GLog.Print( "Net Init Warning: Failed to fix creeping ping for Win2k!\n" );
	}

	sockaddr_in addr;
	addr.sin_family           = AF_INET;
	if( server )
        addr.sin_addr = hostAddr;
	else
		addr.sin_addr.S_un.S_addr = INADDR_ANY;

	addr.sin_port = htons((u_short)serverPort);

	if( bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in) ) )
	{
		String errorText = String::Sprintf( "Net Init Error: Failed to bind socket on port %d: %s\n", serverPort, GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		closesocket( sock );
		WSACleanup();
		return false;
	}

	// Find out what port we were assigned
	sockaddr_in toRead;
	int nameLen = sizeof( sockaddr_in );
	if( getsockname( sock, (sockaddr*)&toRead, &nameLen ) )
	{
		String errorText = String::Sprintf( "Net Init Error: getsockname on bound socket failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		closesocket( sock );
		WSACleanup();
		return false;
	}

	mLocalAddr.IP = toRead.sin_addr.S_un.S_addr;
	mLocalAddr.Port = ntohs(toRead.sin_port);

	String t = String::Sprintf( "Socket bound: %d.%d.%d.%d:%d\n", mLocalAddr.ip1, mLocalAddr.ip2, mLocalAddr.ip3, mLocalAddr.ip4, mLocalAddr.Port );
	GLog.Print( t.AsChar() );

	mSocket = sock;
	mConnReset = false;

	State = INITIALIZED;
	return true;
}

void IPDriver::StartServer( void )
{
	if( State == SERVER ) return;
	if( State > INITIALIZED )
		Disconnect();

	// Initialize GameSpy packages

	// Query & Reporting SDK
	// CDKEY SDK

	if( !mGameSpySDK && GEngine->GameSpyOK )
	{
		char secret_key[9];

		secret_key[4] = 'd';
		secret_key[1] = '7';
		secret_key[5] = '2';
		secret_key[0] = 'k';
		secret_key[6] = '\0';
		secret_key[2] = 'F';
		secret_key[3] = '4';

		qr2_register_key(GAMESPY_MAXSPECTATORS_KEY, "max_spectators");
		qr2_register_key(GAMESPY_PASSWORDED_KEY, "passworded");

		qr2_error_t error = qr2_init_socket( NULL, mSocket, htons(mLocalAddr.Port), GAMESPY_GAMENAME, secret_key,
			IsPublic, 1, ServerKeyCallback, PlayerKeyCallback, TeamKeyCallback, KeyListCallback, PlayerTeamCountCallback,
			AddErrorCallback, this );
		if( error != e_qrnoerror )
		{
			String errorText;

			switch( error )
			{
			case e_qrdnserror:
				errorText = String::Sprintf( "GameSpy Init Error: a DNS lookup (for the master server) failed\nGameSpy inactive!\n" );
				break;
			case e_qrnochallengeerror:
				errorText = String::Sprintf( "GameSpy Init Error: no challenge was received from the master - either the master is down, or a firewall is blocking UDP\nGameSpy inactive!\n" );
				break;
			default:
				errorText = String::Sprintf( "GameSpy Init Error: unknown error value given by GameSpy: %d\nGameSpy inactive!\n", error );
			}

			mGameSpySDK = false;

			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );
		}
		else
		{
#ifdef CDKEY_ENABLED
			gcd_init_qr2( NULL, GAMESPY_CDKEY_GAMEID );
#endif
			mGameSpySDK = true;
		}

		qr2_register_natneg_callback( NULL, NatNegotiationCallback );
	}

	// All-Seeing-Eye SDK
	if( !mASE_SDK )
	{
		String ip = mLocalAddr.AsStringIP();
		mASE_SDK = ASEQuery_initialize( mLocalAddr.Port, IsPublic?1:0, (char*)ip.AsChar() );
		if( !mASE_SDK )
			GLog.Print( "All-Seeing Eye initialization returned: %s\n", ASEQuery_error );
	}

	State = SERVER;
}

void IPDriver::Disconnect( void )
{
	if( State <= INITIALIZED ) return;	// no need to disconnect

	// Close open channels
	while( mChannels.Size() ) delete mChannels[0];
	State = INITIALIZED;
	mConnReset = false;
}

void IPDriver::Close()
{
	if( State == STARTUP )
		return;

	ClearLatencySimulator();

	Disconnect();	// state -> INITIALIZED

	// Shut down ASE
	if( mASE_SDK )
	{
		ASEQuery_shutdown();
		mASE_SDK = 0;
	}

	// Shut down GameSpy packages
	//  - Query & Reporting SDK
	if( mGameSpySDK )
	{
#ifdef CDKEY_ENABLED
		gcd_disconnect_all(GAMESPY_CDKEY_GAMEID);
		gcd_shutdown();
#endif
		qr2_shutdown( NULL );
		mGameSpySDK = false;
	}

	// Close open sockets
	if( INVALID_SOCKET != mSocket )
	{
		closesocket( mSocket );
		mSocket = INVALID_SOCKET;
	}

	WSACleanup();

	State = STARTUP;
}

void IPDriver::GameSpyStateChanged( void )
{
	if( mGameSpySDK ) qr2_send_statechanged( NULL );
}

String IPDriver::GetSocketErrorString( void )
{
	int error = WSAGetLastError();
	switch( error )
	{
	case WSAEINTR:				return String("WSAEINTR");
	case WSAEBADF:				return String("WSAEBADF");
	case WSAEACCES:				return String("WSAEACCES");
	case WSAEDISCON:			return String("WSAEDISCON");
	case WSAEFAULT:				return String("WSAEFAULT");
	case WSAEINVAL:				return String("WSAEINVAL");
	case WSAEMFILE:				return String("WSAEMFILE");
	case WSAEWOULDBLOCK:		return String("WSAEWOULDBLOCK");
	case WSAEINPROGRESS:		return String("WSAEINPROGRESS");
	case WSAEALREADY:			return String("WSAEALREADY");
	case WSAENOTSOCK:			return String("WSAENOTSOCK");
	case WSAEDESTADDRREQ:		return String("WSAEDESTADDRREQ");
	case WSAEMSGSIZE:			return String("WSAEMSGSIZE");
	case WSAEPROTOTYPE:			return String("WSAEPROTOTYPE");
	case WSAENOPROTOOPT:		return String("WSAENOPROTOOPT");
	case WSAEPROTONOSUPPORT:	return String("WSAEPROTONOSUPPORT");
	case WSAESOCKTNOSUPPORT:	return String("WSAESOCKTNOSUPPORT");
	case WSAEOPNOTSUPP:			return String("WSAEOPNOTSUPP");
	case WSAEPFNOSUPPORT:		return String("WSAEPFNOSUPPORT");
	case WSAEAFNOSUPPORT:		return String("WSAEAFNOSUPPORT");
	case WSAEADDRINUSE:			return String("WSAEADDRINUSE");
	case WSAEADDRNOTAVAIL:		return String("WSAEADDRNOTAVAIL");
	case WSAENETDOWN:			return String("WSAENETDOWN");
	case WSAENETUNREACH:		return String("WSAENETUNREACH");
	case WSAENETRESET:			return String("WSAENETRESET");
	case WSAECONNABORTED:		return String("WSWSAECONNABORTEDAEINTR");
	case WSAECONNRESET:			return String("WSAECONNRESET");
	case WSAENOBUFS:			return String("WSAENOBUFS");
	case WSAEISCONN:			return String("WSAEISCONN");
	case WSAENOTCONN:			return String("WSAENOTCONN");
	case WSAESHUTDOWN:			return String("WSAESHUTDOWN");
	case WSAETOOMANYREFS:		return String("WSAETOOMANYREFS");
	case WSAETIMEDOUT:			return String("WSAETIMEDOUT");
	case WSAECONNREFUSED:		return String("WSAECONNREFUSED");
	case WSAELOOP:				return String("WSAELOOP");
	case WSAENAMETOOLONG:		return String("WSAENAMETOOLONG");
	case WSAEHOSTDOWN:			return String("WSAEHOSTDOWN");
	case WSASYSNOTREADY:		return String("WSASYSNOTREADY");
	case WSAVERNOTSUPPORTED:	return String("WSAVERNOTSUPPORTED");
	case WSANOTINITIALISED:		return String("WSANOTINITIALISED");
	case WSAHOST_NOT_FOUND:		return String("WSAHOST_NOT_FOUND");
	case WSATRY_AGAIN:			return String("WSATRY_AGAIN");
	case WSANO_RECOVERY:		return String("WSANO_RECOVERY");
	case WSANO_DATA:			return String("WSANO_DATA");
	default:					return String("UNKNOWN ERROR");
	} 
}


// Given hostName, fills up fields of ipAddr and returns true, or returns false if no known host
bool IPDriver::Resolve( const char* hostName, IpAddress& ipAddr )
{
	char buf[256];
	ipAddr.Port = 0;

	if( !hostName )
		return false;

	// Strip surroundings
	if( ( hostName[0] == '"' ) || ( hostName[0] == '\'' ) )
	{
		::strcpy( buf, hostName+1 );
		buf[::strlen(buf)-1] = 0;
	}
	else
		::strcpy( buf, hostName );

	// Get port if present
	for( char* colon = buf; *colon ; ++colon )
	{
		if( *colon == ':' )
		{
			*colon = 0;
			ipAddr.Port = (short)atoi( colon + 1 );
			break;
		}
	}

	// Check for a.b.c.d form
	if( buf[0] >= '0' && buf[0] <= '9' )
		ipAddr.IP = inet_addr( buf );
	else
	{
		hostent* host = gethostbyname( hostName ); 
		if( !host || PF_INET != host->h_addrtype )
			return false;
		ipAddr.IP = *(DWORD*)(*host->h_addr_list);
	}

	return true;
}

String IPDriver::GetMyIP( int* port )
{
	if( State == STARTUP )
	{
		char hostname[256];
		in_addr	hostAddr;
		hostAddr.S_un.S_addr = 0;

		if( gethostname( hostname, sizeof( hostname ) ) )
		{
			if( port ) *port = 0;
			return "";
		}

		hostent* host = gethostbyname( hostname );
		if( !host )
		{
			if( port ) *port = 0;
			return "";
		}

		hostAddr = *(in_addr*)(*host->h_addr_list);

		IpAddress address;
		address.IP = hostAddr.S_un.S_addr;
		String t = String::Sprintf( "%d.%d.%d.%d", address.ip1, address.ip2, address.ip3, address.ip4 );
		if( port ) *port = 0;
		return t;
	}
	else
	{
		String t;
		if( mLocalAddr.IP == INADDR_ANY )
			t = "127.0.0.1";
		else
			t = String::Sprintf( "%d.%d.%d.%d", mLocalAddr.ip1, mLocalAddr.ip2, mLocalAddr.ip3, mLocalAddr.ip4 );
		if( port ) *port = mLocalAddr.Port;
		return t;
	}
}

IPChannel* IPDriver::CreateIPChannel( IpAddress& ipAddr )
{
	IPChannel* chan;
    m_setTag("Net");
	chan = new IPChannel( this, &ipAddr );
	mChannels.Add( chan );
	return chan;
}

void IPDriver::RemoveChannel( IPChannel* c )
{
	for( int i = 0; i < mChannels.Size(); ++i )
	{
		if( mChannels[i] == c )
		{
			mChannels[i]->mDriver = 0;
			mChannels.FastRemove(i);
			return;
		}
	}
}

void IPDriver::Tick( float timeDelta )
{
	if( State == STARTUP ) return;

#ifdef PACKET_LOG
	GLog.Print( "*P: NET TICK - Start \n" );
#endif

	DWORD curTime = GetCurrentTime();
	if( curTime - mLastStatsUpdateTime > mStatsUpdateDelay )
	{
		mLastUpdateBytesSent = mCurrentUpdateBytesSent;
		mLastUpdateBytesReceived = mCurrentUpdateBytesReceived;
		mCurrentUpdateBytesSent = 0;
		mCurrentUpdateBytesReceived = 0;
		mLastStatsUpdateTime = curTime;
	}

	static UdpMessage mesg;

	NNThink();

	// Handle incoming messages
	IpAddress from;
	while( Read( from, &mesg ) )
	{
		// Ignore empty messages
		if( !mesg.GetSize() ) continue;

		unsigned char gameSpyTest0 = mesg.ReadUChar();
		unsigned char gameSpyTest1 = mesg.ReadUChar();
		unsigned char gameSpyTest2 = mesg.ReadUChar();
		unsigned char gameSpyTest3 = mesg.ReadUChar();
		unsigned char gameSpyTest4 = mesg.ReadUChar();
		unsigned char gameSpyTest5 = mesg.ReadUChar();

		mesg.SeekToBegin();

		if(   ( gameSpyTest0 == 0x3B ) ||								// Server Browser magic
			( ( gameSpyTest0 == 0xFE ) && (gameSpyTest1 == 0xFD ) ) ||	// Query & Reporting magic
			( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) && ( gameSpyTest2 == 0x1E ) && (gameSpyTest3 == 0x66 ) && ( gameSpyTest4 == 0x6A ) && (gameSpyTest5 == 0xB2 ) )	// NAT Negotiation magic
			)		// GameSpy packages
		{
			#ifdef PACKET_LOG
				GLog.Print( "*P: GameSpy \n" );
			#endif
			struct sockaddr_in addr;
			from.FillSockaddrIn( addr );

			if( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) )
				NNProcessData( mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr_in*)&addr );
			else if( mGameSpySDK )
				qr2_parse_query( NULL, mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr*)&addr );
		}
		else if( mesg.PeekLong() == -1 )						// IPDriver packages
		{
			#ifdef PACKET_LOG
				GLog.Print( "*P: HandleIPDriverPackage \n" );
			#endif
			HandleIPDriverPackage( &mesg, from );
		}
		else													// Channel packages
		{
			#ifdef PACKET_LOG
				GLog.Print( "*P: HandleChanneledPackage \n" );
			#endif
			IPChannel* channel = FindChannelByAddress( from );
			if( channel && ( channel->PrepareForReading( &mesg ) ) )
				channel->HandleChanneledPackage( &mesg );
		}
	}

	// Handle disconnecting of our socket
	// Servers ignore this, as on UDP socket it only means that previous send() operation got a 'port unreachable' answer.

	if( State == SERVER )
	{
		// Remove disconnected channels
		for( int i = 0; i < mChannels.Size(); ++i )
		{
			if( mChannels[i]->State == IPChannel::DISCONNECTED )
			{
				mChannels[i]->mDriver = 0;
				delete mChannels[i];
				mChannels.FastRemove(i);
				i--;
			}
		}
	}
	else if( ( State > INITIALIZED ) && ( mConnReset ) )
	{
		String errorText = String::Sprintf( "Net Error: Connection refused by server ( port unreachable )\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		Disconnect();
	}

	// Try to send connection packet if we're connecting and the previous one has been sent timeout ago
	if( State == CONNECTING )
	{
		if( GEngine->SysDriver.GetCurrentTime() - mLastConnectTime > CONNECTION_PACKET_TIMEOUT )
		{
			if( mLastConnectPackets < CONNECTION_PACKET_NUMBER )
			{
				mLastConnectTime = GEngine->SysDriver.GetCurrentTime();
				UdpMessage mesg2;
				mesg2.WriteUChar( IPDRIVER_GETCHALLENGE );
				mesg2.WriteCheckToken( IPD_GETCH );
				mesg2.EndWriting();
				SendMaintenanceMessage( mServerAddr, &mesg2 );
				mLastConnectPackets++;
			}
			else
			{
				String errorText = String::Sprintf( "Net Error: Given address does not reply\n" );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
				Disconnect();
			}
		}
	}

	// Tick channels
	TickChannels();

	// GameSpy Query & Report SDK tick
	if( mGameSpySDK )
	{
		qr2_think( NULL );
#ifdef CDKEY_ENABLED
		gcd_think();
#endif
	}

	if( mASE_SDK )
	{
		mASE_SDK = ASEQuery_check();
		if( !mASE_SDK )
		{
			GLog.Print( "Shutting down ASE, ASEQuery_check() returned: %s\n", ASEQuery_error );
			ASEQuery_shutdown();
		}
	}
#ifdef PACKET_LOG
	GLog.Print( "*P: NET TICK - End \n" );
#endif
}

void IPDriver::WasteMessages( void )
{
	static UdpMessage mesg;

	// Handle incoming messages
	IpAddress from;
	while( Read( from, &mesg ) )
	{
		// Ignore empty messages
		if( !mesg.GetSize() ) continue;

		unsigned char gameSpyTest0 = mesg.ReadUChar();
		unsigned char gameSpyTest1 = mesg.ReadUChar();
		unsigned char gameSpyTest2 = mesg.ReadUChar();
		unsigned char gameSpyTest3 = mesg.ReadUChar();
		unsigned char gameSpyTest4 = mesg.ReadUChar();
		unsigned char gameSpyTest5 = mesg.ReadUChar();

		mesg.SeekToBegin();

		if(   ( gameSpyTest0 == 0x3B ) ||								// Server Browser magic
			( ( gameSpyTest0 == 0xFE ) && (gameSpyTest1 == 0xFD ) ) ||	// Query & Reporting magic
			( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) && ( gameSpyTest2 == 0x1E ) && (gameSpyTest3 == 0x66 ) && ( gameSpyTest4 == 0x6A ) && (gameSpyTest5 == 0xB2 ) )	// NAT Negotiation magic
			)		// GameSpy packages
		{
			struct sockaddr_in addr;
			from.FillSockaddrIn( addr );

			if( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) )
				NNProcessData( mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr_in*)&addr );
			else if( mGameSpySDK )
				qr2_parse_query( NULL, mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr*)&addr );
		}
	}
}

void IPDriver::PlayTick( float timeDelta )
{
	static UdpMessage mesg;

	// Handle incoming messages
	while( GEngine->Net->Recording.PlayUdpMessage( &mesg ) )
	{
		// Empty message means end of recording
		if( !mesg.GetSize() )
		{
			String errorText = String::Sprintf( "The playback has finished\n" );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );
			delete mChannels[0];
			return;
		}

		mesg.SeekToBegin();
		Assert( mChannels.Size() == 1 );
		mChannels[0]->HandleChanneledPackage( &mesg );
	}
}

void IPDriver::TickChannels( void )
{
	for( int i = 0; i < mChannels.Size(); ++i )
		mChannels[i]->Tick();
}

IPChannel* IPDriver::FindChannelByAddress( IpAddress& addr )
{
	for( int i = 0; i < mChannels.Size(); ++i )
		if( addr == mChannels[i]->mAddress )
			return mChannels[i];
	return NULL;
}

void IPDriver::HandleIPDriverPackage( UdpMessage* message, IpAddress& from )
{
	message->ReadLong();	// remove starting -1
	unsigned char type = message->ReadUChar();
	switch( type )
	{
	case GENERAL_ERROR:	
		HandleCommand_Error( from, message );
		return;

	case GENERAL_DISCONNECT:	
		HandleCommand_Disconnect( from );
		return;

	case IPDRIVER_GETCHALLENGE:
		message->ReadCheckToken( IPD_GETCH );
		if( State == SERVER )
			HandleCommand_GetChallenge( from );
		return;

	case IPDRIVER_CHALLENGE:
		if( State == CONNECTING )
		{
			HandleCommand_Challenge( from, message );
			message->ReadCheckToken( IPD_CHALLENGE );
		}
		return;

	case IPDRIVER_CONNECT:
		if( State == SERVER )
		{
			HandleCommand_Connect( from, message );
			message->ReadCheckToken( IPD_CONNECT );
		}
		return;

	case IPDRIVER_CLIENTCONNECT:
		if( State == CHALLENGED )
		{
			HandleCommand_ClientConnect( from, message );
			message->ReadCheckToken( IPD_CLCONNECT );
		}
		return;

	default:
		return;
	}
}

void IPDriver::HandleCommand_Disconnect( IpAddress& from )
{
#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: Disconnect\n" );
#endif

	if( State <= INITIALIZED ) return;	// who cares; we're not connected anyway

	// Clean after disconnection
	IPChannel* channel = FindChannelByAddress( from );
	if( channel ) delete channel;

	if( State == SERVER ) return;

	if( State >= IPDriver::INITIALIZED )
	{
		String errorText = String::Sprintf( "Net Error: Connection closed from other side ( probably other side is down right now )\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		Disconnect();
	}
}

void IPDriver::HandleCommand_GetChallenge( IpAddress& from )
{
#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: GetChallenge\n" );
#endif

	if( State != SERVER )
	{
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( NOT_A_SERVER );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );
		return;
	}

	double curTime = GEngine->SysDriver.GetCurrentTime();
	double oldestTime = curTime;
	int oldestChallenge = 0;

	// check out the IpAddress in our list
	for( int i = 0; i < mChallenges.Size(); ++i )
	{
		if( mChallenges[i].ChallengeAddress == from )
		{
			if( ( mChallenges[i].State != IPConnectionData::FAILED ) &&
				( curTime - mChallenges[i].ChallengeTime < 3.0 ) )
				return;	// reply to challenge packets from a single address no more often that once every 3 seconds

			mChallenges[i].ChallengeTime = curTime;
			mChallenges[i].State = IPConnectionData::SENT_CHALLENGE;
			break;
		}

		if( mChallenges[i].ChallengeTime < oldestTime )
		{
			oldestTime = mChallenges[i].ChallengeTime;
			oldestChallenge = i;
		}
	}

	if( i == mChallenges.Size() )	// we haven't found the ip in our table
	{
		IPConnectionData newChallenge;

		newChallenge.Challenge = String::GenerateRandom();
		newChallenge.ChallengeAddress = from;
		newChallenge.ChallengeTime = GEngine->SysDriver.GetCurrentTime();
		newChallenge.State = IPConnectionData::SENT_CHALLENGE;

		if( i >= CHALLENGES_MAX )
		{
			mChallenges[oldestChallenge] = newChallenge;
			i = oldestChallenge;
		}
		else
			i = mChallenges.Add( newChallenge );
	}

	// Make sure last connection from this host is closed
	IPChannel* channel = FindChannelByAddress( from );
	if( channel ) delete channel;

	// Send the challenge to the client
	UdpMessage mesg2;
	mesg2.WriteUChar( IPDRIVER_CHALLENGE );
	mesg2.WriteString( mChallenges[i].Challenge.AsChar() );
#ifdef CDKEY_ENABLED
	mesg2.WriteBool( mGameSpySDK );
#else
	mesg2.WriteBool( false );
#endif
	mesg2.WriteCheckToken( IPD_CHALLENGE );
	mesg2.EndWriting();
	SendMaintenanceMessage( from, &mesg2 );
	return;
}

void IPDriver::HandleCommand_Challenge( IpAddress& from, UdpMessage* message )
{
	if( State != CONNECTING )
		return;		// no reply unless we're connecting

#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: Challenge\n" );
#endif

	String challenge = message->ReadString();

	if( challenge.Len() != 10 )
		return;	// anti-exploit

	String response = challenge;
	bool hasCdkey = message->ReadBool();

	if( hasCdkey )
	{
//		GLog.Print( "Our CD-Key is %s, computing response\n", CDKey.AsChar() );
		char resp[73];
		char nonConstCDKey[40];
		strcpy( nonConstCDKey, CDKey.AsChar() );
		char nonConstChallenge[40];
		strcpy( nonConstChallenge, challenge.AsChar() );
		gcd_compute_response( nonConstCDKey, nonConstChallenge, resp );
		resp[72]=0;
		response = resp;
	}

	if( from.Port == 0 ) from.Port = DEFAULT_SERVER_PORT;

	String scrambledPassword;
	Scramble( GEngine->Net->GameSpyInfo.Password, challenge, scrambledPassword );

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

#ifdef MPDEMO
	ourVer += "demo";
#endif

#ifdef SDK_BUILD
	ourVer += "sdk";
#endif


	UdpMessage mesg;
	mesg.WriteUChar( IPDRIVER_CONNECT );
	mesg.WriteString( ourVer.AsChar() );
	mesg.WriteString( ourNetVer.AsChar() );
	mesg.WriteString( response.AsChar() );
	mesg.WriteLong( GEngine->Net->mSpectator?1L:0L );
	mesg.WriteString( scrambledPassword.AsChar() );
	mTimeSent = GEngine->SysDriver.GetCurrentTimeMS();
	mesg.WriteULong( mTimeSent );
	mesg.WriteCheckToken( IPD_CONNECT );
	mesg.EndWriting();
	SendMaintenanceMessage( from, &mesg );

	State = CHALLENGED;
}

void IPDriver::HandleCommand_Connect( IpAddress& from, UdpMessage* message )
{
	if( State != SERVER )
		return;	// no answer if we're not a server

#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: Connect\n" );
#endif

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

#ifdef MPDEMO
	ourVer += "demo";
#endif

#ifdef SDK_BUILD
	ourVer += "sdk";
#endif

	// Test for old client
	if( message->PeekLong() == 1 )
	{
		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( DIFFERENT_CLIENT_VERSION );
		mesg.WriteString( ourVer.AsChar() );
		mesg.WriteString( "early development" );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': using early development client version!", from.AsString() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	String clientVer = "";
	String clientNetVer = "";
	String challengeResponse = "";
	String scrambledPassword = "";

	bool valid = true;
	bool isSpectator = false;

	clientVer = message->ReadString();
	if( clientVer.TestPrintable() && ( message->GetPos() < message->GetSize() ) )
	{
		clientNetVer = message->ReadString();
		if( clientNetVer.TestPrintable() && ( message->GetPos() < message->GetSize() ) )
		{
			challengeResponse = message->ReadString();
			if( challengeResponse.TestPrintable() && ( message->GetPos() < message->GetSize() ) )
			{
				isSpectator = (message->ReadLong()==1L);
				scrambledPassword = message->ReadString();
				if( scrambledPassword.TestPrintable() && ( message->GetPos() < message->GetSize() ) )
				{
					message->ReadULong();
					if( message->GetPos() > message->GetSize() )
						valid = false;
				}
				else
					valid = false;
			}
			else
				valid = false;
		}
		else
			valid = false;
	}
	else
		valid = false;

	if( !valid )
		clientVer = "???";

	// Check client version number
	if( !valid || ( ourVer != clientVer ) )
	{
		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( DIFFERENT_CLIENT_VERSION );
		mesg.WriteString( ourVer.AsChar() );
		mesg.WriteString( clientVer.AsChar() );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': different client version: %s", addr.AsChar(), clientVer.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Check client scripts checksum
	if( ourNetVer != clientNetVer )
	{
		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( DIFFERENT_CLIENT_SCRIPTS );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': different client scripts\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Check password length
	if( scrambledPassword.Len() > 253 )	// just in case
	{
		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( CLIENT_PASSWORD_TOO_LONG );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': gave too long password\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Unscramble the password with our challenge
	String unscrambledPassword;
	bool unscrambled = false;
	int challengeNum;
	for( challengeNum = 0; challengeNum < mChallenges.Size(); ++challengeNum )
		if( from == mChallenges[challengeNum].ChallengeAddress )
		{
			if( mChallenges[challengeNum].State != IPConnectionData::SENT_CHALLENGE )
			{
				UdpMessage mesg;
				mesg.WriteUChar( GENERAL_ERROR );
				mesg.WriteUChar( ANSWER_TO_NO_CHALLENGE );
				mesg.EndWriting();
				SendMaintenanceMessage( from, &mesg );

				String addr = from.AsString();
				String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': it tried to answer a challenge I didn't issue\n", addr.AsChar() );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );

				return;
			}

			unscrambled = Scramble( scrambledPassword, mChallenges[challengeNum].Challenge, unscrambledPassword );
			break;
		}

	if( challengeNum == mChallenges.Size() )
	{
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( NO_CHALLENGE );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': too many clients trying to connect\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Check password
	bool passok = false;
	if( unscrambled )
	{
		if( !unscrambledPassword.Len() && !GEngine->Net->GameSpyInfo.Password.Len() )
			passok = true;
		else if( unscrambledPassword.Len() && GEngine->Net->GameSpyInfo.Password.Len() &&
			( unscrambledPassword == GEngine->Net->GameSpyInfo.Password ) )
			passok = true;
	}

	if( !passok )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( BAD_PASSWORD );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': bad password given\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Make sure last connection from this host is closed
	IPChannel* channel = FindChannelByAddress( from );
	if( channel ) delete channel;

	if( !GEngine->Net->CanConnectNewClient( isSpectator ) )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( MAX_CLIENTS_ACHIEVED );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': maximal number of clients achieved\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	mChallenges[challengeNum].Spectator = isSpectator;
	mChallenges[challengeNum].State = IPConnectionData::AUTHENTICATING;

	// Check challenge validity
#ifdef CDKEY_ENABLED
	if( mGameSpySDK )	// Authenticate with GameSpy CDKEY
	{
		int index = mAddressesForCDKEY.Size();
		char nonConstChallenges[40];
		strcpy( nonConstChallenges, mChallenges[challengeNum].Challenge.AsChar() );
		char nonConstChallengeResponse[100];
		strcpy( nonConstChallengeResponse, challengeResponse.AsChar() );

		// Check if hash not banned
		for( int i = 0; i < mBannedHashes.Size(); ++i )
		{
			if( ::strncmp( nonConstChallengeResponse, mBannedHashes[i].AsChar(), 32 ) == 0 )
			{
				// This hash is banned!
				mChallenges[challengeNum].State = IPConnectionData::FAILED;

				UdpMessage mesg;
				mesg.WriteUChar( GENERAL_ERROR );
				mesg.WriteUChar( BANNED_HASH );
				mesg.EndWriting();
				SendMaintenanceMessage( from, &mesg );

				String addr = from.AsString();
				String errorText = String::Sprintf( "Net Info: Banned player tried to enter the server.\n", addr.AsChar() );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );

				return;
			}
		}

		gcd_authenticate_user( GAMESPY_CDKEY_GAMEID, index, from.IP, nonConstChallenges, nonConstChallengeResponse, StaticGameSpyCDKEYCallback, this );
		mAddressesForCDKEY.Add( from );
		mPlayerHashes.Add( nonConstChallengeResponse );
		GLog.Print( "GameSpy authentication underway\n" );
		return;	// Callback will do the rest of the work
	}
	else				// Authenticate the old way
	{
		if( mChallenges[challengeNum].Challenge != challengeResponse )
		{
			mChallenges[challengeNum].State = IPConnectionData::FAILED;

			// Send the info to the client
			UdpMessage mesg;
			mesg.WriteUChar( GENERAL_ERROR );
			mesg.WriteUChar( BAD_CHALLENGE );
			mesg.EndWriting();
			SendMaintenanceMessage( from, &mesg );

			String addr = from.AsString();
			String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': invalid challenge given\n", addr.AsChar() );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );

			return;
		}
	}
#else
	if( mChallenges[challengeNum].Challenge != challengeResponse )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( BAD_CHALLENGE );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': invalid challenge response given\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}
#endif

	// Challenge ok here

	// Make a new channel
	channel = CreateIPChannel( from );		// server socket, client address
	channel->mNetworkClient = GEngine->Net->MakeNewNetworkClient();
	channel->mNetworkClient->mNetChannel = channel;
	channel->mNetworkClient->mSpectator = isSpectator;

	UdpMessage mesg2;
	mesg2.WriteUChar( IPDRIVER_CLIENTCONNECT );
	mesg2.WriteLong( 0L );
	mesg2.WriteUChar( channel->mNetworkClient->GetClientIndex() );
	mesg2.WriteCheckToken( IPD_CLCONNECT );
	mesg2.EndWriting();
	SendMaintenanceMessage( from, &mesg2 );

	mChallenges[challengeNum].State = IPConnectionData::SENT_CONNECT;
}

int IPDriver::CountAuthenticatingClients( bool spectator )
{
	int count = 0;
	for( int i = 0; i < mChallenges.Size(); ++i )
		if( ( mChallenges[i].State == IPConnectionData::ConnectionState::AUTHENTICATING ) &&
			( mChallenges[i].Spectator == spectator ) )
			count++;
	return count;
}

void IPDriver::GameSpyCDKEYCallback( int localid, int authenticated, char* errmsg )
{
	IpAddress from = mAddressesForCDKEY[localid];

	int challengeNum;
	for( challengeNum = 0; challengeNum < mChallenges.Size(); ++challengeNum )
		if( from == mChallenges[challengeNum].ChallengeAddress )
			break;
	Assert( challengeNum < mChallenges.Size() );

	Assert( mChallenges[challengeNum].State == IPConnectionData::AUTHENTICATING );

	// Check authentication
	if( !authenticated )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		// GameSpy bug fix
		char myerrmsg[256];
		if( ::strstr( errmsg, "Your CD Key is disabled. Contact customer service." ) )
		{
			::strncpy( myerrmsg, errmsg, 50 );
			myerrmsg[50] = 0;
		}
		else
		{
			::strncpy( myerrmsg, errmsg, 255 );
			myerrmsg[255] = 0;
		}

		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( BAD_CD_KEY );
		mesg.WriteString( myerrmsg );
		mesg.EndWriting();
		SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': GameSpy says it has a bad CD key\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		GLog.Print( "GameSpy authentication rejected\n" );

		return;
	}

//	GLog.Print( "GameSpy authentication accepted. Reason: %s\n", errmsg );

	// Make sure last connection from this host is closed
	IPChannel* channel = FindChannelByAddress( from );
	if( channel ) delete channel;

	// Make a new channel
	channel = CreateIPChannel( from );		// server socket, client address
	channel->mNetworkClient = GEngine->Net->MakeNewNetworkClient();
	channel->mNetworkClient->mNetChannel = channel;
	channel->mNetworkClient->mSpectator = mChallenges[challengeNum].Spectator;
	channel->mNetworkClient->SetCdKeySDKIndexAndHash( localid, mPlayerHashes[localid] );

	UdpMessage mesg2;
	mesg2.WriteUChar( IPDRIVER_CLIENTCONNECT );
	mesg2.WriteLong( 0L );
	mesg2.WriteUChar( channel->mNetworkClient->GetClientIndex() );
	mesg2.WriteCheckToken( IPD_CLCONNECT );
	mesg2.EndWriting();
	SendMaintenanceMessage( from, &mesg2 );

	mChallenges[challengeNum].State = IPConnectionData::SENT_CONNECT;
}

void IPDriver::HandleCommand_ClientConnect( IpAddress& from, UdpMessage* message )
{
#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: ClientConnect\n" );
#endif

	message->ReadLong();

	unsigned char clientIndex = message->ReadUChar();

	// Make sure last connection from this host is closed
	IPChannel* channel = FindChannelByAddress( from );
	if( channel )
		delete channel;

	// Make a new channel
	channel = CreateIPChannel( from );	// server socket, client address
	channel->mNetworkClient = GEngine->Net->MakeNewNetworkClient();
	channel->mNetworkClient->mMyIndex = clientIndex;
	channel->mNetworkClient->mSpectator = GEngine->Net->mSpectator;
	channel->mNetworkClient->mNetChannel = channel;

	channel->mNetworkClient->State = NetworkClient::EStateConnecting;

	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_OPEN );
	mesg.WriteCheckToken( NCL_CLOPEN );
	mesg.EndWriting();
	channel->SendMaintenanceMessage( true, &mesg );

	// Start DemoRecording if it has been preset before connecting
	if( GEngine->mRecordAfterConnection )
	{
		GEngine->mRecordAfterConnection = false;
		GEngine->Net->Recording.StartRecording( GEngine->mRecordFilename.AsChar(), channel->mNetworkClient );
	}

	State = CLIENT;
}

bool IPDriver::ReadyForNetOpen( IPChannel* channel )
{
	int challengeNum;
	for( challengeNum = 0; challengeNum < mChallenges.Size(); ++challengeNum )
		if( channel->mAddress == mChallenges[challengeNum].ChallengeAddress )
			break;
	Assert( challengeNum < mChallenges.Size() );
	if( mChallenges[challengeNum].State == IPConnectionData::SENT_CONNECT )
	{
		mChallenges[challengeNum].State = IPConnectionData::CONNECTED;
		return true;
	}
	else
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( NET_OPEN_WITHOUT_CONNECT );
		mesg.EndWriting();
		SendMaintenanceMessage( channel->mAddress, &mesg );

		String addr = channel->mAddress.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected net open from '%s': it tried to do it without a prior connect\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return false;
	}
}

void IPDriver::HandleCommand_Error( IpAddress& from, UdpMessage* message )
{

	unsigned char error = message->ReadUChar();

#ifdef CONNECTION_STAGES_LOG
	GLog.Print( "Connection stage: Error %d\n", error );
#endif

	String errorText;

	switch( error )
	{
	case NOT_A_SERVER:
		errorText = String::Sprintf( "Net Error: trying to connect to host that is not a Painkiller server!!\n" );
		break;
	case DIFFERENT_CLIENT_VERSION:
		{
			String serverVersion = message->ReadString();
			String ourVersion = message->ReadString();
			errorText = String::Sprintf( "Net Error: the server is version '%s', we are '%s'.\nCan't connect!! Please upgrade the game.\n", serverVersion.AsChar(), ourVersion.AsChar() );
		}
		break;
	case DIFFERENT_CLIENT_SCRIPTS:
		errorText = String::Sprintf( "Net Error: we have different scripts than the server, but the same game version.\nPlease upgrade or reinstall the game.\n" );
		break;
	case CLIENT_PASSWORD_TOO_LONG:
		errorText = String::Sprintf( "Net Error: password given was too long.\n" );
		break;
	case BAD_PASSWORD:
		errorText = String::Sprintf( "Please enter a password to access this server\n" );
		if( State == CHALLENGED ) State = INITIALIZED;
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_BadPassword, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return;
	case BAD_CHALLENGE:
		errorText = String::Sprintf( "Net Error: failed challenge authentication!! A hack job, perhaps?\n" );
		break;
	case NO_CHALLENGE:
		// Silent error - can be handled here
		if( State == CHALLENGED )
		{
			State = CONNECTING;
			mLastConnectTime = GEngine->SysDriver.GetCurrentTime();
			UdpMessage mesg;
			mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			SendMaintenanceMessage( mServerAddr, &mesg );
			mLastConnectPackets = 0;
			errorText = String::Sprintf( "Network: server challenge timeout...reconnecting\n" );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );
		}
		return;
	case MAX_CLIENTS_ACHIEVED:
		errorText = String::Sprintf( "Net Error: No more players allowed on this server\n" );
		break;
	case BAD_CD_KEY:
		{
			String serverText = message->ReadString();
			errorText = String::Sprintf( "Net Error: your CD Key is currently in use. If you just disconnected from a server, please wait a bit before trying to connect again.\nServer said: %s\n", serverText.AsChar() );
			if( State == CHALLENGED ) State = INITIALIZED;
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_BadCDKey, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );
		}
		return;
	case ANSWER_TO_NO_CHALLENGE:
		{
			String serverText = message->ReadString();
			errorText = String::Sprintf( "Net Error: the client tried to answer a challenge that the server didn't provide!\nServer said: %s\n", serverText.AsChar() );
		}
		break;
	case NET_OPEN_WITHOUT_CONNECT:
		{
			String serverText = message->ReadString();
			errorText = String::Sprintf( "Net Error: the client tried to issue a Net Open command without connecting first!\nServer said: %s\n", serverText.AsChar() );
		}
		break;
	case BANNED_HASH:
		errorText = String::Sprintf( "Net Error: You have been banned from this server\n" );
		break;
	default:
		errorText = String::Sprintf( "Net Error: unknown error report received!!\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return;
	}

	if( State == CHALLENGED ) State = INITIALIZED;
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

bool IPDriver::SendMaintenanceMessage( IpAddress& to, UdpMessage* maintenance )
{
	UdpMessage mesg;
	mesg.WriteLong(-1); NPROF2(NPBasicOOBString,4); // Special packet
	mesg.WriteData( maintenance ); NPROF2(NPBasicOOBString,maintenance->GetSize());
	mesg.EndWriting();
	return Write( to, &mesg );
}

void IPDriver::GameSpyCDKEYClientDeletion( int index )
{
#ifdef CDKEY_ENABLED
	if( mGameSpySDK )
	{
		gcd_disconnect_user( GAMESPY_CDKEY_GAMEID, index );
		GLog.Print( "Sent request to disconnect user %d to GameSpy\n", index );
	}
#endif
}

bool IPDriver::Write( const IpAddress& ipAddr, UdpMessage* message )
{
	if( mSimulatedLatency > 0 )
	{
		DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();

		// Add new message to waiting messages list
		LatencySimEntry sim;
		sim.Time = curTime;
		sim.Address = ipAddr;
        m_setTag("Net");
		sim.Message = new UdpMessage;
		sim.Message->SeekToBegin();
		sim.Message->WriteData( message );
		sim.Message->EndWriting();
		mWaitingToSend.Add( sim );

		curTime -= mSimulatedLatency;

		// Send all messages from waiting messages list
		while( mWaitingToSend.Size() )
		{
			if( mWaitingToSend[0].Time > curTime )
				return true;

			bool ok = WriteInternal( mWaitingToSend[0].Address, mWaitingToSend[0].Message );
			if( !ok )
			{
				ClearAllMessagesForAddress( mWaitingToSend[0].Address );
				return false;
			}
			else
			{
				delete mWaitingToSend[0].Message;
				mWaitingToSend.Remove(0);
			}
		}
		return true;
	}
	else
		return WriteInternal( ipAddr, message );
}

bool IPDriver::WriteInternal( const IpAddress& ipAddr, UdpMessage* message )
{
	if( mSocket == INVALID_SOCKET )
		return false;

	sockaddr_in addr;
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(ipAddr.Port);
	addr.sin_addr.s_addr = ipAddr.IP;

	if( message->GetSize() >= 1400 )
	{
		GLog.Print( "Message sent of size: %d\n", message->GetSize() );
		GLog.Flush();
	}

	int bytesSend = sendto( mSocket, message->GetBuffer(), message->GetSize(), 0, (sockaddr*)&addr, sizeof(addr) );

	if( SOCKET_ERROR != bytesSend )
	{
		if( mLocalAddr.IP == INADDR_ANY )
		{
			// Find out what IP we were assigned
			sockaddr_in toRead;
			int nameLen = sizeof( sockaddr_in );
			if( !getsockname( mSocket, (sockaddr*)&toRead, &nameLen ) )
				mLocalAddr.IP = toRead.sin_addr.S_un.S_addr;
		}
		mCurrentUpdateBytesSent += bytesSend;
		return true;
	}

	if( WSAGetLastError() == WSAEWOULDBLOCK ) return true;		// that's ok, it's normal, we said: don't block and this would

	String errorText = String::Sprintf( "Net Error: Write: error in sendto(): %s\n", IPDriver::GetSocketErrorString().AsChar() );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );

	return false;
}

bool IPDriver::Read( IpAddress& ipAddr , UdpMessage* message )
{
	if( mSimulatedLatency > 0 )
	{
		// Read all waiting messages into queue with times
		static UdpMessage mesg;
		DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
		IpAddress from;

		while( ReadInternal( from, &mesg ) )
		{
			// Add new message to waiting messages list
			LatencySimEntry sim;
			sim.Time = curTime;
			sim.Address = from;
			m_setTag("Net");
            sim.Message = new UdpMessage;
			sim.Message->SeekToBegin();
			sim.Message->WriteData( &mesg );
			sim.Message->EndWriting();
			mWaitingToReceive.Add( sim );
			curTime = GEngine->SysDriver.GetCurrentTimeMS();
		}

		// Give first read message
		if( mWaitingToReceive.Size() && ( mWaitingToReceive[0].Time+mSimulatedLatency < curTime ) )
		{
			ipAddr = mWaitingToReceive[0].Address;
			message->SeekToBegin();
			message->WriteData( mWaitingToReceive[0].Message );
			message->EndWriting();
			delete mWaitingToReceive[0].Message;
			mWaitingToReceive.Remove(0);
			return true;
		}
		else
			return false;
	}
	else
	{
		// Must have been changed to zero. Clear waiting first.
		if( mWaitingToReceive.Size() )
		{
			ipAddr = mWaitingToReceive[0].Address;
			message->SeekToBegin();
			message->WriteData( mWaitingToReceive[0].Message );
			message->EndWriting();
			delete mWaitingToReceive[0].Message;
			mWaitingToReceive.Remove(0);
			return true;
		}
		else
			return ReadInternal( ipAddr, message );
	}
}

bool IPDriver::ReadInternal( IpAddress& ipAddr , UdpMessage* message )
{
	if( INVALID_SOCKET != mSocket )
	{
		sockaddr_in fromAddr;
		int fromAddrSize = sizeof(fromAddr);

		message->SeekToBegin();
		int bytesRecv = recvfrom( mSocket, message->GetBuffer(), MAX_UDP_PACKET_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize );

		if( SOCKET_ERROR != bytesRecv )
		{
			ipAddr.IP   = fromAddr.sin_addr.s_addr;
			ipAddr.Port = ntohs( fromAddr.sin_port );
			message->mSize = bytesRecv;
			mCurrentUpdateBytesReceived += bytesRecv;
			return true;
		}
		else
		{
			int error = WSAGetLastError();
			if( error == WSAEWOULDBLOCK )
			{
				// that's ok, it's normal, there was nothing to read and we said: don't block
			}
			else if( error == WSAECONNRESET )
			{
				mConnReset = true;	// the other side told us its unreachable. If we're a client, higher layer should know about it.
			}
			else
			{
				String errorText = String::Sprintf( "Net Error: Read: error in recvfrom(): %s\n", IPDriver::GetSocketErrorString().AsChar() );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
			}
		}
	}

	return false;
}

void IPDriver::ConnectAsClient( IpAddress& addr )
{
	if( State > INITIALIZED )
	{
		if( State == CLIENT )
			Disconnect();
		else
			return;	// i'm connecitng - wait patiently
	}

	State = CONNECTING;

	mServerAddr = addr;
	mLastConnectTime = GEngine->SysDriver.GetCurrentTime();

	UdpMessage mesg;
	mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
	mesg.WriteCheckToken( IPD_GETCH );
	mesg.EndWriting();
	SendMaintenanceMessage( mServerAddr, &mesg );
	mLastConnectPackets = 0;
}

void IPDriver::ConnectAsClient( void* serverBrowserData )
{
	if( State > INITIALIZED )
	{
		if( State == CLIENT )
			Disconnect();
		else
			return;	// i'm connecitng - wait patiently
	}

	State = CONNECTING;

	SBServer server = *(SBServer*)serverBrowserData;

	if( !SBServerHasPrivateAddress( server ) )
	{
		mServerAddr.IP = SBServerGetPublicInetAddress( server );
		mServerAddr.Port = SBServerGetPublicQueryPort( server );
		mLastConnectTime = GEngine->SysDriver.GetCurrentTime();

		UdpMessage mesg;
		mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
		mesg.WriteCheckToken( IPD_GETCH );
		mesg.EndWriting();
		SendMaintenanceMessage( mServerAddr, &mesg );
		mLastConnectPackets = 0;

		return;
	}
	else	// the server has private IP address - it may be neccessary to start NAT Negotiation
	{
		unsigned int serverPublicAddress = SBServerGetPublicInetAddress( server );

		char secret_key[9];

		secret_key[4] = 'd';
		secret_key[5] = '2';
		secret_key[2] = 'F';
		secret_key[6] = '\0';
		secret_key[0] = 'k';
		secret_key[1] = '7';
		secret_key[3] = '4';

		// To get informed Server Browser...
		ServerBrowser serverBrowser = ServerBrowserNew( GAMESPY_GAMENAME, GAMESPY_GAMENAME, secret_key, 0, 10, QVERSION_QR2, NULL, this );
		unsigned int localPublicAddress = GEngine->MyPublicIPAddress;
		if( serverPublicAddress == localPublicAddress )	// we're behind the same NAT
		{
			ServerBrowserFree( serverBrowser );
			mServerAddr.IP = SBServerGetPrivateInetAddress( server );
			mServerAddr.Port = SBServerGetPrivateQueryPort( server );
			mLastConnectTime = GEngine->SysDriver.GetCurrentTime();

			UdpMessage mesg;
			mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			SendMaintenanceMessage( mServerAddr, &mesg );
			mLastConnectPackets = 0;

			return;
		}

		// We're not behind the same NAT

		if( SBServerDirectConnect( server ) )
		{
			// Succeeded in direct connect
			ServerBrowserFree( serverBrowser );
			mServerAddr.IP = SBServerGetPublicInetAddress( server );
			mServerAddr.Port = SBServerGetPublicQueryPort( server );
			mLastConnectTime = GEngine->SysDriver.GetCurrentTime();

			UdpMessage mesg;
			mesg.WriteUChar( IPDRIVER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			SendMaintenanceMessage( mServerAddr, &mesg );
			mLastConnectPackets = 0;

			return;
		}
		else
		{
			// Direct connect failed - time to start negotiating
			unsigned int randomCookie = (unsigned int)rand();
			ServerBrowserSendNatNegotiateCookieToServer( serverBrowser, SBServerGetPublicAddress( server ), SBServerGetPublicQueryPort( server ), randomCookie );
			NNBeginNegotiationWithSocket( mSocket, randomCookie, 0, NatNegotiateProgressCallback, NatNegotiateCompletedCallback, this );
			State = NAT_NEGOTIATING;
		}
		ServerBrowserFree( serverBrowser );
	}
}

void IPDriver::SendAllWaitingMessages( void )
{
	for( int i = 0; i < mWaitingToSend.Size(); ++i )
	{
		WriteInternal( mWaitingToSend[i].Address, mWaitingToSend[i].Message );
		delete mWaitingToSend[i].Message;
	}
	mWaitingToSend.Clear();
}

void IPDriver::ClearAllMessagesForAddress( IpAddress& address )
{
	for( int i = 0; i < mWaitingToSend.Size(); ++i )
		if( mWaitingToSend[i].Address == address )
		{
			delete mWaitingToSend[i].Message;
			mWaitingToSend.Remove(i--);
		}
}

void IPDriver::ClearLatencySimulator( void )
{
	SendAllWaitingMessages();
	for( int i = 0; i < mWaitingToReceive.Size(); ++i )
		delete mWaitingToReceive[i].Message;
	mWaitingToReceive.Clear();
	mSimulatedLatency = SIMULATED_LATENCY_DEFAULT;
}

void IPDriver::SetSimulatedLatency( DWORD latency )
{
	latency /= 2;
	if( ( mSimulatedLatency != 0 ) && ( latency == 0 ) )
		SendAllWaitingMessages();
	mSimulatedLatency = latency;
}

#endif
