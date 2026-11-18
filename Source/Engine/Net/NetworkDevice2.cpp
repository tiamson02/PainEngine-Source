
#include "Engine.h"

#include "Net/GameSpyData.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Net/NetworkDevice2.h"
#include "Net/NetworkPeer2.h"
#include "Net/UdpMessage.h"
#include "Net/ObjectSynchronizer2.h"

#include "Render/Video.h"
#include "Engine/PainMenu.h"
#include "Engine/BaseGame.h"
#include "Engine/World.h"
#include "Engine/InputSystem.h"

#include "Render/View.h"


#ifdef PKTV_ENABLED
# include "PKTVManager.h"
#endif

extern ENGINE_API bool IsBlackEdition();

// GameSpy includes

#ifndef	TRANSGAMING_NO_GAMESPY
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
// Inclusion of Usage Tracking SDK
#include "Net/GameSpy2/pt/pt.h"
#endif

// All-Seeing Eye include
extern "C" {
#include "Net/Ase/ASEQuerySDK.h"
}

#define CHALLENGES_MAX 1000

#ifndef	TRANSGAMING_NO_GAMESPY

int mGameSpyCustomKeyNumber = GAMESPY_MODDERS_CUSTOM_KEY;

static bool registeredQR2Keys = false;
void RegisterQR2Keys( void )
{
	if( !registeredQR2Keys )
	{
		qr2_register_key(GAMESPY_MAXSPECTATORS_KEY, "max_spectators");
		qr2_register_key(GAMESPY_PASSWORDED_KEY, "passworded");
		qr2_register_key(GAMESPY_PKTV_PROXY_KEY, "PKTVProxy");
		registeredQR2Keys = true;
	}
}

#endif

// ---------------------------- UNIQUE NUMBER REUSER METHODS -----------------------------------------

UniqueNumberReuser2::UniqueNumberReuser2( void ) : mNextNumber(0), mStart(0), mEnd(0)
{
	::memset( (void*)mQuarantined, 0, UNIQUE_NUMBER_QUARANTINE_SIZE*sizeof(ulong32) );
}

ulong32 UniqueNumberReuser2::GetNext( void )
{
	if( mFree.Size() )
	{
		ulong32 next = mFree[mFree.Size()-1];
		mFree.RemoveLast();
		return next;
	}
	else
		return mNextNumber++;
}

void UniqueNumberReuser2::Free( ulong32 uniqueNumber )
{
	mQuarantined[mEnd] = uniqueNumber;
	if( mStart == ((mEnd+1)&(UNIQUE_NUMBER_QUARANTINE_SIZE-1)) )	// mQuarantined is full
	{
		mFree.Add( mQuarantined[mStart] );
		mEnd++;
		mEnd &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
		mStart++;
		mStart &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
	}
	else	// it is not full
	{
		mEnd++;
		mEnd &= (UNIQUE_NUMBER_QUARANTINE_SIZE-1);
	}
}

void UniqueNumberReuser2::Clear()
{
    mFree.ClearArray();
    mNextNumber = 0;
    mStart = 0;
    mEnd = 0;
    ::memset( (void*)mQuarantined, 0, UNIQUE_NUMBER_QUARANTINE_SIZE*sizeof(ulong32) );
}

bool UniqueNumberReuser2::Empty()
{
	return (mNextNumber == 0);
}

// ---------------------------- GENERIC NET FUNCTIONS ---------------------------------------------------

bool Resolve( const char* hostName, IpAddress& ipAddr )
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

// ---------------------------- PASSWORD SCRAMBLER FUNCTIONS ---------------------------------------------------

static bool MoveToBytecode( String input, char* output )
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

static bool MoveFromBytecode( char* input, String& output, int length )
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

static bool Scramble( String password, String challenge, String& output )
{
	if( !password.Len() ) { output = password; return true; };
	Assert( challenge.Len() );

	char passwordBC[256];
	char outputBC[256];

	if( !MoveToBytecode( password, passwordBC ) ) return false;

	char Scrambler1[64], Scrambler2[64];

	ulong32 i, j, t, x;
	char temp, k;
	k = i = j = t = x = 0;
	temp = 0;

	for( i = 0; i < 64; ++i )
		Scrambler1[i] = i;

	j = 0;
	for( i = 0; i < 64; ++i )
	{
		if( j == (ulong32)challenge.Len() )
			j = 0;
		Scrambler2[i] = challenge[j++];
	}

	j = 0;

	for( i = 0; i < 64; ++i )
	{
		j = (j + (ulong32)Scrambler1[i] + (ulong32)Scrambler2[i]) % 64U;
		temp = Scrambler1[i];
		Scrambler1[i] = Scrambler1[j];
		Scrambler1[j] = temp;
	}

	i = j = 0;
	ulong32 ps = (ulong32)password.Len();
	for( x = 0; x < ps; ++x )
	{
		i = (i + 1U) % 64U;
		j = (j + (ulong32)Scrambler1[i]) % 64U;

		temp = Scrambler1[i];
		Scrambler1[i] = Scrambler1[j];
		Scrambler1[j] = temp;

		t = ((ulong32)Scrambler1[i] + (ulong32)Scrambler1[j]) % 64U;
		k = Scrambler1[t];

		char out = (char)(passwordBC[x]^k);
		Assert( out < 64 );
		outputBC[x] = out;
	}

	return MoveFromBytecode( outputBC, output, password.Len() );
}

// ---------------------------- GAMESPY SDK CALLBACKS ----------------------------------------------------------
#ifndef	TRANSGAMING_NO_GAMESPY

#ifdef CDKEY_ENABLED
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
void StaticGameSpyCDKEYCallback_ND2( int productid, int localid, int authenticated, char* errmsg, void* instance )
#else
static void StaticGameSpyCDKEYCallback( int productid, int localid, int authenticated, char* errmsg, void* instance )
#endif
{
	if( productid == GAMESPY_CDKEY_GAMEID )
		((NetworkDevice2*)instance)->LowLayer_GameSpyCDKEYCallback( localid, authenticated, errmsg );
}
#endif

static void ServerKeyCallback( int keyid, qr2_buffer_t outbuf, void *userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;
	int valueInt;
	String valueString;
	bool isNumber = net->GameSpyInfo.GetServerValue( keyid, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static void PlayerKeyCallback( int keyid, int index, qr2_buffer_t outbuf, void *userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;
	int valueInt;
	String valueString;
	bool isNumber = net->GameSpyInfo.GetPlayerValue( keyid, index, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static void TeamKeyCallback( int keyid, int index, qr2_buffer_t outbuf, void *userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;
	int valueInt;
	String valueString;
	bool isNumber = net->GameSpyInfo.GetTeamValue( keyid, index, valueInt, valueString );
	if( isNumber )
		qr2_buffer_add_int( outbuf, valueInt );
	else
		qr2_buffer_add( outbuf, valueString.AsChar() );
}

static int PlayerTeamCountCallback( qr2_key_type keytype, void* userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;
	return ( keytype == key_player )?net->GameSpyInfo.GetPlayerCount():net->GameSpyInfo.GetTeamCount();
}

static void KeyListCallback( qr2_key_type keytype, qr2_keybuffer_t keybuffer, void* userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;

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
		qr2_keybuffer_add(keybuffer, PASSWORD_KEY);
		qr2_keybuffer_add(keybuffer, GAMESPY_MAXSPECTATORS_KEY);
		qr2_keybuffer_add(keybuffer, GAMESPY_PASSWORDED_KEY);
		qr2_keybuffer_add(keybuffer, GAMESPY_PKTV_PROXY_KEY);
		for( int i = 0; i < net->GameSpyInfo.NumofCustomKeys(); ++i )
			qr2_keybuffer_add(keybuffer, net->GameSpyInfo.CustomKeyKeyNumber( i ) );
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

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
void RealizeNegotiateCompleted_ND2( NetworkDevice2* i, struct sockaddr_in* remoteaddr )
#else
static void RealizeNegotiateCompleted( NetworkDevice2* i, struct sockaddr_in* remoteaddr )
#endif
{
	i->mServerAddr.IP = remoteaddr->sin_addr.S_un.S_addr;
	i->mServerAddr.Port = ntohs(remoteaddr->sin_port);
	i->mLastConnectTime = GEngine->SysDriver.GetCurrentTime();
	String addr = i->mServerAddr.AsString();
	String errorText = String::Sprintf( "NAT Negotiation complete! Connecting to %s\n", addr.AsChar() );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );

	UdpMessage mesg;
	mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
	mesg.WriteCheckToken( IPD_GETCH );
	mesg.EndWriting();
	i->LowLayer_SendMaintenanceMessage( i->mServerAddr, &mesg );
	i->mLastConnectPackets = 0;
	i->State = NetworkDevice2::CONNECTING;
}

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
void RealizeNegotiateFailed_ND2( NetworkDevice2* i, struct sockaddr_in* remoteaddr )
#else
static void RealizeNegotiateFailed( NetworkDevice2* i, struct sockaddr_in* remoteaddr )
#endif
{
	i->State = NetworkDevice2::INITIALIZED;
}

static void NatNegotiateCompletedCallback( NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata )
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;

	String errorText;

	switch( result )
	{
		case nr_success:			// Successful negotiation, other parameters can be used to continue communications with the client
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
			RealizeNegotiateCompleted_ND2( net, remoteaddr );
#else
			RealizeNegotiateCompleted( net, remoteaddr );
#endif			
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
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	RealizeNegotiateFailed_ND2( net, remoteaddr );
#else	
	RealizeNegotiateFailed( net, remoteaddr );
#endif
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

#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
void NatNegotiationCallback_ND2( int cookie, void* userdata )
#else
static void NatNegotiationCallback( int cookie, void* userdata )
#endif
{
	NetworkDevice2* net = (NetworkDevice2*)userdata;

	NNBeginNegotiationWithSocket( net->mSocket, cookie, 1, NatNegotiateProgressCallback, NatNegotiateCompletedCallbackServer, userdata );

	String errorText = String::Sprintf( "NAT Negotiation: Received client cookie: %d\n", cookie );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

#endif

// ---------------------------- MAIN FUNCTIONS -----------------------------------------------------------------

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
int NetworkDevice2::mMaxObjectSyncByEntity = 0;
int NetworkDevice2::mMaxObjectSync = 0;
#endif


NetworkDevice2::NetworkDevice2( void )
:	mSpectator( false )
,	State( STARTUP )
,	mSocket( INVALID_SOCKET )
,	mConnReset( false )
,	mPublic( false )
,	mGameSpySDK( false )
,	mAseSDK( 0 )
,	mLastConnectPackets(0)
,	mLastConnectTime(0.f)

,	mCurrentUpdateBytesSent(0)
,	mCurrentUpdateBytesReceived(0)
,	mLastUpdateBytesSent(0)
,	mLastUpdateBytesReceived(0)
,	mCurrentUpdatePacketsSent(0)
,	mCurrentUpdatePacketsReceived(0)
,	mLastUpdatePacketsSent(0)
,	mLastUpdatePacketsReceived(0)

,	mLastStatsUpdateTime(0)
,	mStatsUpdateDelay(1000)
,	mStatsNumberToAvg(1)
,	mPureScripts( false )
{
	GameSpyInfo.SetNet(this);
	mCDKey = "1234-1234-1234-1234";		// just for now, a random one

	SetBandwidthControl( -1 );	// turned off

	if( GEngine->Video )
	{
		View * view = GEngine->Video->GetActiveView();
		if( view )
			view->mCamera.Interpolation = false;
	}

	ScriptObject Config = GScript.Globals()["Cfg"];
	mStatsUpdateDelay = Config["NetcodeStatsUpdateDelay"].GetInt();
	mStatsNumberToAvg = Config["NetcodeStatsNumberToAverageFrom"].GetInt();

#ifdef PKTV_ENABLED
	if( GEngine->PKTV )
		GameSpyInfo.PKTVProxy = GEngine->PKTV->GetProxyAddress();
#endif
}

NetworkDevice2::~NetworkDevice2()
{
	NetworkClose();
}

bool NetworkDevice2::NetworkServer( String mapName, String password, String interfaceToUse, unsigned short port )
{
	char buf[1024];
	if( !MoveToBytecode( password, buf ) )
	{
		GLog.Print( "Server password can contain only letters and digits!\n" );
		return false;
	}

	NetworkClose();

	int fps = GScript.Globals()["Cfg"]["ServerFPS"].IsNil()?45:GScript.Globals()["Cfg"]["ServerFPS"].GetInt();

	if( fps < 30 ) fps = 30;
	else if( fps > 120 ) fps = 120;

#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
# ifdef	TRANSGAMING
	if( GEngine->GameState == PCFSystem::DedicatedServer )
# else
	if( GEngine->GameState == PCFSystem::PCFSystemStates::DedicatedServer )
# endif
		GEngine->MinSecondsPerFrame = fps?((double)1.0)/fps:0.0;
	else
#endif
#ifdef TRANSGAMING
		GEngine->MinSecondsPerFrame = 0.0f;
#else
		GEngine->MinSecondsPerFrame = 0.0;
#endif

	GameSpyInfo.SetMode( 0 );
	GameSpyInfo.MapName = mapName;
	GameSpyInfo.Password = password;

	if( port == 0 )
		port = DEFAULT_SERVER_PORT;

	bool ok = LowLayer_Open( port, true, interfaceToUse );
	if( ok )
	{
		LowLayer_StartServer();

		// Start synchronizing entities we have
#ifdef	TRANSGAMING
		for( Entity* entity = GEngine->pcfWorld->EntityList; entity; entity = entity->Next )
#else
		for( Entity* entity = GEngine->World->EntityList; entity; entity = entity->Next )
#endif
		{
			unsigned char state = entity->SynchroState();
			if( state & Entity::ESynchroEntity )
			{
				ulong32 index = mNumberManager.GetNext();
				mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex, index );
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
                if (mObjectSynchronizersHashByEntity.Size() > mMaxObjectSyncByEntity){
                    mMaxObjectSyncByEntity = mObjectSynchronizersHashByEntity.Size();
                    TRACE("@@@ NetworkDevice2::NetworkServer(): peak mObjectSynchronizersHashByEntity hash usage = %d\n", mMaxObjectSyncByEntity);
                }
#endif
            }
		}

#ifndef	TRANSGAMING_NO_GAMESPY
		// Track game usage
		ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);
#endif

		// Test PureScripts variable
		ScriptObject Cfg = GScript.Globals()["Cfg"];
		mPureScripts = Cfg["PureScripts"].GetBool();
		
		if( !ScriptsSecure( mCRC ) )
		{
			GLog.Print( "Set PureScripts on server; yet server scripts not pure!!!!\n" );
			mCRC = 0;
		}
	}

	return ok;
}

bool NetworkDevice2::NetworkClientAndConnect( const char* hostname, String password, unsigned short port, bool spectator )
{
	{
		char buf[1024];
		if( !MoveToBytecode( password, buf ) )
		{
			GLog.Print( "Server password can contain only letters and digits!\n" );
			return false;
		}
	}

	NetworkClose();

	if( !LowLayer_Open( 0 ) )
		return false;

	ScriptObject Config = GScript.Globals()["Cfg"];
	long32 bps = Config["NetcodeClientMaxBytesPerSecond"].GetInt();
	if( !bps )	// turn off if no data in config
		bps = -1;
	SetBandwidthControl( bps );

	IpAddress addr;
	addr.Port = 0;

	if( !Resolve( hostname, addr ) )
	{
		String errorText = String::Sprintf( "Net Error: Couldn't resolve address: %s\n", hostname );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return false;
	}

	GameSpyInfo.Password = password;
	mSpectator = spectator;

	if( port )
		addr.Port = port;
	else if( !addr.Port )
		addr.Port = DEFAULT_SERVER_PORT;

	GEngine->LastServerUpdateMethod = PCFSystem::IP;

	LowLayer_ConnectAsClient( addr );

#ifndef	TRANSGAMING_NO_GAMESPY
	// Track game usage
	ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);
#endif

	return true;
}

bool NetworkDevice2::NetworkClientAndConnect( void* hostFromGameSpy, String password, bool spectator )
{
	{
		char buf[1024];
		if( !MoveToBytecode( password, buf ) )
		{
			GLog.Print( "Server password can contain only letters and digits!\n" );
			return false;
		}
	}

	NetworkClose();

	if( !LowLayer_Open( 0 ) )
		return false;

	ScriptObject Config = GScript.Globals()["Cfg"];
	long32 bps = Config["NetcodeClientMaxBytesPerSecond"].GetInt();
	if( !bps )	// turn off if no data in config
		bps = -1;
	SetBandwidthControl( bps );

	GameSpyInfo.Password = password;
	mSpectator = spectator;

	LowLayer_ConnectAsClient( hostFromGameSpy );

#ifndef	TRANSGAMING_NO_GAMESPY
	// Track game usage
	ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);
#endif

	return true;
}

void NetworkDevice2::NetworkClose( void )
{
	SetBandwidthControl( -1 );		// turned off

	// Remove delayed script messages
	ClearScriptMessagesList();

	mObjectSynchronizersHashByEntity.Clear();
	while( mObjectSynchronizers.Size() )
		delete mObjectSynchronizers[0];
	mNumberManager.Clear();

	GameSpyInfo.SetMode( 3 );
	LowLayer_Close();
}

void NetworkDevice2::PreTick( float timeDelta )
{
	GProfiler.SetState(NET_PRE_TICK);

	GEngine->TimeMultiplier = 1.0f;

	if( State == STARTUP )
		return;

	DWORD curTime = GetCurrentTime();
	if( curTime - mLastStatsUpdateTime > mStatsUpdateDelay )
	{
		mLastUpdateBytesSent = mCurrentUpdateBytesSent;
		mLastUpdatePacketsSent = mCurrentUpdatePacketsSent;
		mLastUpdateBytesReceived = mCurrentUpdateBytesReceived;
		mLastUpdatePacketsReceived = mCurrentUpdatePacketsReceived;
		mCurrentUpdateBytesSent = 0;
		mCurrentUpdateBytesReceived = 0;
		mCurrentUpdatePacketsSent = 0;
		mCurrentUpdatePacketsReceived = 0;
		mLastStatsUpdateTime = curTime;

		if( mPeers.Size() )
		{
			for( int i = 0; i < mPeers.Size(); ++i )
				mPeers[i]->TickStats( curTime );

			mStatsData = mPeers[0]->mStatsDataTemp;
			for( int i = 1; i < mPeers.Size(); ++i )
				mStatsData += mPeers[i]->mStatsDataTemp;
		}
	}

	static UdpMessage mesg( 65537 );	// safeguard from BIG ASE/GameSpy messages

#ifndef	TRANSGAMING_NO_GAMESPY
	NNThink();
#endif

	// Handle incoming messages
	IpAddress from;
	while( LowLayer_Read( from, &mesg ) )
	{
		// Ignore empty messages
		if( !mesg.GetSize() ) continue;

#ifndef TRANSGAMING_NO_GAMESPY
		unsigned char gameSpyTest0 = mesg.ReadUChar();
		unsigned char gameSpyTest1 = mesg.ReadUChar();
		unsigned char gameSpyTest2 = mesg.ReadUChar();
		unsigned char gameSpyTest3 = mesg.ReadUChar();
		unsigned char gameSpyTest4 = mesg.ReadUChar();
		unsigned char gameSpyTest5 = mesg.ReadUChar();
#endif
		mesg.SeekToBegin();

#ifndef	TRANSGAMING_NO_GAMESPY
		if(   ( gameSpyTest0 == 0x3B ) ||								// Server Browser magic
			( ( gameSpyTest0 == 0xFE ) && (gameSpyTest1 == 0xFD ) ) ||	// Query & Reporting magic
			( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) && ( gameSpyTest2 == 0x1E ) && (gameSpyTest3 == 0x66 ) && ( gameSpyTest4 == 0x6A ) && (gameSpyTest5 == 0xB2 ) )	// NAT Negotiation magic
			)		// GameSpy packages
		{
			struct sockaddr_in addr;
			from.FillSockaddrIn( addr );

			if( ( gameSpyTest0 == 0xFD ) && (gameSpyTest1 == 0xFC ) )
			{
//				GLog.Print( "Got NN query from %s\n", from.AsString() );
				NNProcessData( mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr_in*)&addr );
			}
			else if( mGameSpySDK )
			{
//				GLog.Print( "Got QR2 query from %s\n", from.AsString() );
				qr2_parse_query( NULL, mesg.GetBuffer(), mesg.GetSize(), (struct sockaddr*)&addr );
			}
		}
		else if( mesg.PeekLong() == -1 )						// NetworkDevice2 packages
#else
		if( mesg.PeekLong() == -1 )						// NetworkDevice2 packages
#endif		
			LowLayer_HandleNetworkDevicePackage( &mesg, from );
		else													// NetworkPeer2 packages
		{
			NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
			if( peer && peer->PrepareMessageForReading( &mesg ) )
				peer->HandlePeerPackage( &mesg );
		}
	}

	// Handle disconnecting of our socket
	// Servers ignore this, as on UDP socket it only means that previous send() operation got a 'port unreachable' answer.

	if( State == SERVER )
	{
		// Remove disconnected peers
		for( int i = 0; i < mPeers.Size(); ++i )
		{
			if( mPeers[i]->mDisconnected )
			{
				delete mPeers[i];
				mPeers.FastRemove(i);
				--i;
			}
		}
	}
	else if( ( State > INITIALIZED ) && mConnReset )
	{
		String errorText = String::Sprintf( "Net Error: Connection refused by server ( port unreachable )\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		LowLayer_Disconnect();
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
				mesg2.WriteUChar( LOWLAYER_GETCHALLENGE );
				mesg2.WriteCheckToken( IPD_GETCH );
				mesg2.EndWriting();
				LowLayer_SendMaintenanceMessage( mServerAddr, &mesg2 );
				++mLastConnectPackets;
			}
			else
			{
				String errorText = String::Sprintf( "Net Error: Given address does not reply\n" );
				GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
				GLog.Print( errorText.AsChar() );
				LowLayer_Disconnect();
			}
		}
	}

	// Tick peers
	for( int i = 0; i < mPeers.Size(); ++i )
		mPeers[i]->Tick();

#ifndef	TRANSGAMING_NO_GAMESPY
	// GameSpy Query & Report SDK tick
	if( mGameSpySDK )
	{
		qr2_think( NULL );
#ifdef CDKEY_ENABLED
		gcd_think();
#endif
	}
#endif

#ifndef TRANSGAMING_NO_ASE
	if( mAseSDK )
	{
		mAseSDK = ASEQuery_check();
		if( !mAseSDK )
		{
			GLog.Print( "Shutting down ASE, ASEQuery_check() returned: %s\n", ASEQuery_error );
			ASEQuery_shutdown();
		}
	}
#endif
	// Set packet loss
	if( ( State != SERVER ) && mPeers.Size() )
	{
		// Test for receiving packets
		if( ClientPeer()->NotReceivingPacketsLately() )
			GScript.DoString( "Hud._showPacketLoss = true" );
		else
			GScript.DoString( "Hud._showPacketLoss = false" );
	}
	else
		GScript.DoString( "Hud._showPacketLoss = false" );
}

void NetworkDevice2::PostTick( void )
{
	GProfiler.SetState(NET_POST_TICK);

	DistributeScriptMessages();

	DWORD time = GEngine->SysDriver.GetCurrentTimeMS();

	if( IsServer() )
	{
		// Send delayed script messages to clients, and make them compose differential information into their messages
		for( int i = 0; i < mPeers.Size(); ++i )
		{
			if( mPeers[i]->mConnectionState < NetworkPeer2::EStatePlay )
				continue;

			if( !mPeers[i]->SendFrame( time ) )		// tried to send frame over 1400 bytes
			{
				UdpMessage mesg;
				mesg.WriteUChar( GENERAL_ERROR );
				mesg.WriteUChar( MAX_FRAME_SIZE_EXCEEDED );
				mesg.EndWriting();
				mPeers[i]->SendMaintenanceMessage( false, &mesg );
				mPeers[i]->SendMaintenanceMessage( false, &mesg );
				mPeers[i]->SendMaintenanceMessage( false, &mesg );
				mPeers[i]->Disconnect();
			}
		}
	}
	else if( IsClient() && mPeers.Size() )
	{
		if( ClientPeer()->mConnectionState > NetworkPeer2::EStateSynchronizing )
			ClientPeer()->SendClientFrame();
		else if( ClientPeer()->mConnectionState == NetworkPeer2::EStateSynchronizing )
		{
#ifdef TRANSGAMING
			if( GEngine->InputSys->GetKeyStatus( BUTTON_SELECT ) )
#else
			if( GEngine->InputSys->GetKeyStatus( VK_ESCAPE ) )
#endif
			{
				GScript.DoString( "NET.Disconnect()" );
				GScript.DoString( "Game.GameInProgress = false" );
				GScript.DoString( "Game.LevelStarted = false" );
				GEngine->InputSys->SetUseDInput( false );
				GEngine->Game->ShowMenu();
				return;
			}
		}
	}

	// Clear delayed script messages
	GProfiler.SetState(NET_OTHER);
	ClearDispatchedMessages();

	GProfiler.SetState(NET_TICK_CHANNELS);

	for( int i = 0; i < mPeers.Size(); ++i )
		mPeers[i]->Tick();

	GProfiler.SetState(NET_OTHER);

	static int counter = 300;
	counter--;
	if( counter == 0 )
	{
		counter = 300;
#ifndef TRANSGAMING_NO_GAMESPY
		GameSpyInfo.CheckGameMode();
#endif
	}
}

String NetworkDevice2::GetMyIP( int* port )
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

// ---------------------------- LOW LAYER FUNCTIONS ---------------------------------------------------

bool NetworkDevice2::LowLayer_Open( unsigned short serverPort, bool server, String interfaceToUse )
{
	if( State != STARTUP )
		LowLayer_Close();

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1,1);

	if( WSAStartup( wVersionRequested, &wsaData ) )
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
	u_long one = 1;
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

		u_long noUdpConnReset = FALSE;
		if( ioctlsocket( sock, SIO_UDP_CONNRESET, &noUdpConnReset ) == SOCKET_ERROR )
			GLog.Print( "Net Init Warning: Failed to fix creeping ping for Win2k!\n" );
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
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

void NetworkDevice2::LowLayer_Close( void )
{
	if( State == STARTUP )
		return;

	LowLayer_Disconnect();	// state -> INITIALIZED

#ifndef TRANSGAMING_NO_ASE
	// Shut down ASE
	if( mAseSDK )
	{
		ASEQuery_shutdown();
		mAseSDK = 0;
	}
#endif

#ifndef	TRANSGAMING_NO_GAMESPY
	// Shut down GameSpy packages
	//  - Query & Reporting SDK
	if( mGameSpySDK )
	{
#ifdef CDKEY_ENABLED
		gcd_disconnect_all(GAMESPY_CDKEY_GAMEID);
		gcd_shutdown();
#endif
		qr2_shutdown( NULL );
		registeredQR2Keys = false;
		RegisterQR2Keys();
		mGameSpySDK = false;
	}
#endif

	// Close open sockets
	if( INVALID_SOCKET != mSocket )
	{
		closesocket( mSocket );
		mSocket = INVALID_SOCKET;
	}

	WSACleanup();

	State = STARTUP;
}

void NetworkDevice2::LowLayer_Disconnect( void )
{
	if( State <= INITIALIZED )
		return;	// no need to disconnect

	// Close open channels
	while( mPeers.Size() )
	{
		delete mPeers[0];
		mPeers.FastRemove(0);
	}

	State = INITIALIZED;
	mConnReset = false;
}

void NetworkDevice2::LowLayer_ConnectAsClient( IpAddress& addr )
{
	if( State > INITIALIZED )
	{
		if( State == CLIENT )
			LowLayer_Disconnect();
		else
			return;	// i'm connecitng - wait patiently
	}

	State = CONNECTING;

	mServerAddr = addr;
	mLastConnectTime = GEngine->SysDriver.GetCurrentTime();

	UdpMessage mesg;
	mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
	mesg.WriteCheckToken( IPD_GETCH );
	mesg.EndWriting();
	LowLayer_SendMaintenanceMessage( mServerAddr, &mesg );
	mLastConnectPackets = 0;
}

void NetworkDevice2::LowLayer_ConnectAsClient( void* serverBrowserData )
{
#ifdef	TRANSGAMING_NO_GAMESPY
	return;		//joe: no use in ps2
#else

	if( State > INITIALIZED )
	{
		if( State == CLIENT )
			LowLayer_Disconnect();
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
		mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
		mesg.WriteCheckToken( IPD_GETCH );
		mesg.EndWriting();
		LowLayer_SendMaintenanceMessage( mServerAddr, &mesg );
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
			mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			LowLayer_SendMaintenanceMessage( mServerAddr, &mesg );
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
			mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			LowLayer_SendMaintenanceMessage( mServerAddr, &mesg );
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
#endif

}

void NetworkDevice2::LowLayer_StartServer( void )
{
	if( State == SERVER )
		return;

	if( State > INITIALIZED )
		LowLayer_Disconnect();

	State = SERVER;

	// Initialize third party packages

#ifndef	TRANSGAMING_NO_GAMESPY
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

		RegisterQR2Keys();

		GScript.Call( "Game_SetupCustomGameSpyVariables", 0, "" );

		qr2_error_t error = qr2_init_socket( NULL, mSocket, mLocalAddr.Port, GAMESPY_GAMENAME, secret_key,
			mPublic, 1, ServerKeyCallback, PlayerKeyCallback, TeamKeyCallback, KeyListCallback, PlayerTeamCountCallback,
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
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
		qr2_register_natneg_callback( NULL, NatNegotiationCallback_ND2 );
#else
		qr2_register_natneg_callback( NULL, NatNegotiationCallback );
#endif		
	}

#endif

#ifndef TRANSGAMING_NO_ASE
	// All-Seeing-Eye SDK
	if( !mAseSDK )
	{
		String ip = mLocalAddr.AsStringIP();
		mAseSDK = ASEQuery_initialize( mLocalAddr.Port, mPublic?1:0, (char*)ip.AsChar() );
		if( !mAseSDK )
			GLog.Print( "All-Seeing Eye initialization returned: %s\n", ASEQuery_error );
	}
#endif

}

String GetSocketErrorString( void )
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

bool NetworkDevice2::LowLayer_SendMaintenanceMessage( IpAddress& to, UdpMessage* maintenance )
{
	UdpMessage mesg;
	mesg.WriteLong( -1 );
	mesg.WriteData( maintenance );
	mesg.EndWriting();
	return LowLayer_Write( to, &mesg );
}

bool NetworkDevice2::LowLayer_Read( IpAddress& ipAddr , UdpMessage* message )
{
	if( INVALID_SOCKET == mSocket )
		return false;

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
		++mCurrentUpdatePacketsReceived;
		return true;
	}

	int error = WSAGetLastError();
	if( error == WSAEWOULDBLOCK )
		return false; // that's ok, it's normal, there was nothing to read and we said: don't block

	if( error == WSAECONNRESET )
		mConnReset = true;	// the other side told us its unreachable. If we're a client, higher layer should know about it.
	else
	{
		String errorText = String::Sprintf( "Net Error: Read: error in recvfrom(): %s\n", GetSocketErrorString().AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
	}

	return false;
}

bool NetworkDevice2::LowLayer_Write( const IpAddress& ipAddr, UdpMessage* message )
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
		++mCurrentUpdatePacketsSent;
		return true;
	}

	if( WSAGetLastError() == WSAEWOULDBLOCK )
		return true;		// that's ok, it's normal, we said: don't block and this would

	String errorText = String::Sprintf( "Net Error: Write: error in sendto(): %s\n", GetSocketErrorString().AsChar() );
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );

	return false;
}

void NetworkDevice2::LowLayer_HandleNetworkDevicePackage( UdpMessage* message, IpAddress& from )
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

	case LOWLAYER_GETCHALLENGE:
		message->ReadCheckToken( IPD_GETCH );
		if( State == SERVER )
			HandleCommand_GetChallenge( from );
		return;

	case LOWLAYER_CHALLENGE:
		if( State == CONNECTING )
		{
			HandleCommand_Challenge( from, message );
			message->ReadCheckToken( IPD_CHALLENGE );
		}
		return;

	case LOWLAYER_CONNECT:
		if( State == SERVER )
		{
			HandleCommand_Connect( from, message );
			message->ReadCheckToken( IPD_CONNECT );
		}
		return;

	case LOWLAYER_CLIENTCONNECT:
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

NetworkPeer2* NetworkDevice2::LowLayer_FindPeerByAddress( IpAddress& addr )
{
	for( int i = 0; i < mPeers.Size(); ++i )
		if( addr == mPeers[i]->mAddress )
			return mPeers[i];
	return 0;
}

bool NetworkDevice2::CanConnectNewClient( bool spectator )
{
	int maxClients = spectator?GameSpyInfo.MaxSpectators:GameSpyInfo.MaxPlayers;

	if( GEngine->GetGameState() == PCFSystem::MultiplayerServer )
	{
		if( ( spectator && mSpectator ) || ( !spectator && !mSpectator ) )
			maxClients--;
	}

	// count authenticating clients
	int count = 0;
	for( int i = 0; i < mChallenges.Size(); ++i )
#ifdef	TRANSGAMING
		if( ( mChallenges[i].State == IPConnectionData::AUTHENTICATING ) &&
			( mChallenges[i].Spectator == spectator ) )
#else
		if( ( mChallenges[i].State == IPConnectionData::ConnectionState::AUTHENTICATING ) &&
			( mChallenges[i].Spectator == spectator ) )
#endif
			count++;

	maxClients -= count;

	// Count current spectators
	int spects = 0;
	for( int i = 0; i < mPeers.Size(); ++i )
		if( mPeers[i]->mSpectator )
			spects++;

	if( spectator )
		return ( spects < maxClients );
	else
		return ( mPeers.Size()-spects < maxClients );
}

// ---------------------------- LOW LAYER CONNECTION STAGES ---------------------------------------------------

void NetworkDevice2::HandleCommand_Error( IpAddress& from, UdpMessage* message )
{
	unsigned char error = message->ReadUChar();

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
	case PURE_SCRIPTS_FAIL:
		errorText = String::Sprintf( "Net Error: failed connection due to pure server and different checksum of our scripts\n" );
		break;
	case NO_CHALLENGE:
		// Silent error - can be handled here
		if( State == CHALLENGED )
		{
			State = CONNECTING;
			mLastConnectTime = GEngine->SysDriver.GetCurrentTime();
			UdpMessage mesg;
			mesg.WriteUChar( LOWLAYER_GETCHALLENGE );
			mesg.WriteCheckToken( IPD_GETCH );
			mesg.EndWriting();
			LowLayer_SendMaintenanceMessage( mServerAddr, &mesg );
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
	case CHALLENGE_RESPONSE_TOO_LONG:
		errorText = String::Sprintf( "Net Error: Challenge response too long!!\n" );
		break;
	case MAX_FRAME_SIZE_EXCEEDED:
		errorText = String::Sprintf( "Net Error: Maximal frame size exceeded! Try lowering your fps or getting server frames more frequently\n" );
		break;
	default:
		errorText = String::Sprintf( "Net Error: unknown error report received!!\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return;
	}

	if( State == CHALLENGED )
		State = INITIALIZED;
	GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
	GLog.Print( errorText.AsChar() );
}

void NetworkDevice2::HandleCommand_Disconnect( IpAddress& from )
{
	if( State <= INITIALIZED )
		return;	// who cares; we're not connected anyway

	// Clean after disconnection
	NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
	if( peer )
	{
		for( int i = 0; i < mPeers.Size(); ++i )
			if( mPeers[i] == peer )
			{
				delete peer;
				peer = 0;
				mPeers.FastRemove(i);
				break;
			}
		Assert( !peer );
	}

	if( State == SERVER )
		return;

	if( State >= INITIALIZED )
	{
		String errorText = String::Sprintf( "Net Error: Connection closed from other side ( probably other side is down right now )\n" );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		LowLayer_Disconnect();
	}
}

void NetworkDevice2::HandleCommand_GetChallenge( IpAddress& from )
{
	int i=0;
	if( State != SERVER )
	{
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( NOT_A_SERVER );
		mesg.EndWriting();
		LowLayer_SendMaintenanceMessage( from, &mesg );
		return;
	}

#ifdef TRANSGAMING
	float  curTime = GEngine->SysDriver.GetCurrentTime();
	float  oldestTime = curTime;
#else
	double curTime = GEngine->SysDriver.GetCurrentTime();
	double oldestTime = curTime;
#endif
	int oldestChallenge = 0;

	// check out the IpAddress in our list
	for( i = 0; i < mChallenges.Size(); ++i )
	{
		if( mChallenges[i].ChallengeAddress == from )
		{
			if( ( mChallenges[i].State != IPConnectionData::FAILED ) &&
#ifdef TRANSGAMING
				( curTime - mChallenges[i].ChallengeTime < 3.0f ) )
#else
				( curTime - mChallenges[i].ChallengeTime < 3.0 ) )
#endif
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
	NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
	if( peer )
		delete peer;

	// Send the challenge to the client
	UdpMessage mesg2;
	mesg2.WriteUChar( LOWLAYER_CHALLENGE );
	mesg2.WriteString( mChallenges[i].Challenge.AsChar() );
#ifdef CDKEY_ENABLED
	mesg2.WriteBool( mGameSpySDK );
#else
	mesg2.WriteBool( false );
#endif
	mesg2.WriteCheckToken( IPD_CHALLENGE );
	mesg2.EndWriting();

	LowLayer_SendMaintenanceMessage( from, &mesg2 );
}

void NetworkDevice2::HandleCommand_Challenge( IpAddress& from, UdpMessage* message )
{
	if( State != CONNECTING )
		return;		// no reply unless we're connecting

	String challenge = message->ReadString();

	if( challenge.Len() != 10 )
		return;	// anti-exploit

	String response = challenge;
	bool hasCdkey = message->ReadBool();

	if( hasCdkey )
	{
		char resp[73];
		char nonConstCDKey[40];
		strcpy( nonConstCDKey, mCDKey.AsChar() );
		char nonConstChallenge[40];
		strcpy( nonConstChallenge, challenge.AsChar() );
#ifndef	TRANSGAMING_NO_GAMESPY		
		gcd_compute_response( nonConstCDKey, nonConstChallenge, resp );
#endif
		resp[72]=0;
		response = resp;
	}

	if( from.Port == 0 ) from.Port = DEFAULT_SERVER_PORT;

	String scrambledPassword;

	Scramble( GameSpyInfo.Password, challenge, scrambledPassword );

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

#ifdef MPDEMO
	ourVer += "demo";
#endif

#ifdef SDK_BUILD
	ourVer += "sdk";
#endif
	ourVer += "nv2";	// NEW NETCODE

	UdpMessage mesg;
	mesg.WriteUChar( LOWLAYER_CONNECT );
	mesg.WriteString( ourVer.AsChar() );
	mesg.WriteString( ourNetVer.AsChar() );
	mesg.WriteString( response.AsChar() );
	mesg.WriteUChar( mSpectator?1:0 );
	mesg.WriteString( scrambledPassword.AsChar() );
	mesg.WriteCheckToken( IPD_CONNECT );

	DWORD crc;
	bool ok = ScriptsSecure( crc );
	mesg.WriteBool( ok );
	mesg.WriteULong( crc );

	mesg.EndWriting();
	LowLayer_SendMaintenanceMessage( from, &mesg );

	State = CHALLENGED;
}

void NetworkDevice2::HandleCommand_Connect( IpAddress& from, UdpMessage* message )
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
	ourVer += "nv2";	// NEW NETCODE

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
				isSpectator = ( message->ReadUChar() != 0 );
				scrambledPassword = message->ReadString();
				if( !scrambledPassword.TestPrintable() || ( message->GetPos() > message->GetSize() ) )
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

	if( !valid && !clientVer.TestPrintable() )
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
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': different client version: %s\n", addr.AsChar(), clientVer.AsChar() );
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
		LowLayer_SendMaintenanceMessage( from, &mesg );

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
		LowLayer_SendMaintenanceMessage( from, &mesg );

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
				LowLayer_SendMaintenanceMessage( from, &mesg );

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
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': too many clients trying to connect\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Check password
	bool passok = false;

	if( unscrambled && ( ( !unscrambledPassword.Len() && !GameSpyInfo.Password.Len() ) ||
		( unscrambledPassword.Len() && GameSpyInfo.Password.Len() && ( unscrambledPassword == GameSpyInfo.Password ) ) ) )
			passok = true;

	if( !passok )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( BAD_PASSWORD );
		mesg.EndWriting();
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': bad password given\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Check pure scripts
	bool pureOnClient = message->ReadBool();
	DWORD crc = message->ReadULong();

	if( mPureScripts && ( !pureOnClient || ( crc != mCRC ) ) )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		// Send the info to the client
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( PURE_SCRIPTS_FAIL );
		mesg.EndWriting();
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': different scripts checksum\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	// Make sure last connection from this host is closed
	NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
	if( peer )
		delete peer;

	if( !CanConnectNewClient( isSpectator ) )
	{
		mChallenges[challengeNum].State = IPConnectionData::FAILED;

		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_ERROR );
		mesg.WriteUChar( MAX_CLIENTS_ACHIEVED );
		mesg.EndWriting();
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': maximal number of clients achieved\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return;
	}

	mChallenges[challengeNum].Spectator = isSpectator;
	mChallenges[challengeNum].State = IPConnectionData::AUTHENTICATING;

	// Special case for Polish LAN
	bool polishBooHLAN = false;
	if( !mPublic && !IsBlackEdition() )	// LAN server, not Black Edition, polish language
	{
		ScriptObject Cfg = GScript.Globals()["Cfg"];
		String lang = Cfg["Language"].GetString();
		if( lang == "polish" )
			polishBooHLAN = true;
	}

	// Check challenge validity
#ifdef CDKEY_ENABLED

#ifndef	TRANSGAMING_NO_GAMESPY
	if( mGameSpySDK && !polishBooHLAN )	// Authenticate with GameSpy CDKEY
	{
		int index = mAddressesForCDKEY.Size();
		char nonConstChallenges[40];
		strcpy( nonConstChallenges, mChallenges[challengeNum].Challenge.AsChar() );

		if( challengeResponse.Len() > 99 )
		{
			mChallenges[challengeNum].State = IPConnectionData::FAILED;

			UdpMessage mesg;
			mesg.WriteUChar( GENERAL_ERROR );
			mesg.WriteUChar( CHALLENGE_RESPONSE_TOO_LONG );
			mesg.EndWriting();
			LowLayer_SendMaintenanceMessage( from, &mesg );

			String addr = from.AsString();
			String errorText = String::Sprintf( "Net Info: Challenge response too long!\n", addr.AsChar() );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
			GLog.Print( errorText.AsChar() );

			return;
		}

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
				LowLayer_SendMaintenanceMessage( from, &mesg );

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
	else if( !polishBooHLAN )				// Authenticate the old way
#else
	if( !polishBooHLAN )				// Authenticate the old way
#endif	
	{
		if( mChallenges[challengeNum].Challenge != challengeResponse )
		{
			mChallenges[challengeNum].State = IPConnectionData::FAILED;

			// Send the info to the client
			UdpMessage mesg;
			mesg.WriteUChar( GENERAL_ERROR );
			mesg.WriteUChar( BAD_CHALLENGE );
			mesg.EndWriting();
			LowLayer_SendMaintenanceMessage( from, &mesg );

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
    m_setTag("Net");
	peer = new NetworkPeer2( this, &from );
	mPeers.Add( peer );
	peer->mMyIndex = IsServer()?(unsigned char)mClientNumberManager.GetNext():0;
	peer->mSpectator = isSpectator;

	UdpMessage mesg2;
	mesg2.WriteUChar( LOWLAYER_CLIENTCONNECT );
	mesg2.WriteUChar( peer->mMyIndex );
	mesg2.WriteCheckToken( IPD_CLCONNECT );
	mesg2.EndWriting();
	LowLayer_SendMaintenanceMessage( from, &mesg2 );

	mChallenges[challengeNum].State = IPConnectionData::SENT_CONNECT;
}

void NetworkDevice2::LowLayer_GameSpyCDKEYCallback( int localid, int authenticated, char* errmsg )
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
		LowLayer_SendMaintenanceMessage( from, &mesg );

		String addr = from.AsString();
		String errorText = String::Sprintf( "Net Info: Rejected connect from '%s': GameSpy says it has a bad CD key\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		GLog.Print( "GameSpy authentication rejected\n" );

		return;
	}

	// Make sure last connection from this host is closed
	NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
	if( peer )
		delete peer;

	// Make a new peer
    m_setTag("Net");
	peer = new NetworkPeer2( this, &from );
	mPeers.Add( peer );
	peer->mMyIndex = IsServer()?(unsigned char)mClientNumberManager.GetNext():0;
	peer->mSpectator = mChallenges[challengeNum].Spectator;
	peer->mGameSpyCDKeyIndex = localid;
	peer->mGameSpyHash = mPlayerHashes[localid];

	UdpMessage mesg2;
	mesg2.WriteUChar( LOWLAYER_CLIENTCONNECT );
	mesg2.WriteUChar( peer->mMyIndex );
	mesg2.WriteCheckToken( IPD_CLCONNECT );
	mesg2.EndWriting();
	LowLayer_SendMaintenanceMessage( from, &mesg2 );

	mChallenges[challengeNum].State = IPConnectionData::SENT_CONNECT;
}

void NetworkDevice2::HandleCommand_ClientConnect( IpAddress& from, UdpMessage* message )
{
	unsigned char clientIndex = message->ReadUChar();

	// Make sure last connection from this host is closed
	NetworkPeer2* peer = LowLayer_FindPeerByAddress( from );
	if( peer )
		delete peer;

    m_setTag("Net");
	peer = new NetworkPeer2( this, &from );
	mPeers.Add( peer );
	peer->mMyIndex = clientIndex;
	peer->mSpectator = mSpectator;

	peer->mConnectionState = NetworkPeer2::EStateConnecting;

	UdpMessage mesg;
	mesg.WriteUChar( HIGHLAYER_OPEN );
	mesg.WriteCheckToken( NCL_CLOPEN );
	mesg.EndWriting();
	peer->SendMaintenanceMessage( true, &mesg );

	State = CLIENT;
}

bool NetworkDevice2::ReadyForNetOpen( IpAddress* address )
{
	int challengeNum;
	for( challengeNum = 0; challengeNum < mChallenges.Size(); ++challengeNum )
		if( *address == mChallenges[challengeNum].ChallengeAddress )
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
		LowLayer_SendMaintenanceMessage( *address, &mesg );

		String addr = (*address).AsString();
		String errorText = String::Sprintf( "Net Info: Rejected net open from '%s': it tried to do it without a prior connect\n", addr.AsChar() );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Information, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );

		return false;
	}
}

void NetworkDevice2::LoadNewMap( const char *levelName )
{
	// Reset clients
	for( int i = 0; i < mPeers.Size(); ++i )
		mPeers[i]->ResetToConnecting();

	// Reset myself
	// ----------------------

	ClearScriptMessagesList();
	mObjectSynchronizersHashByEntity.Clear();
	while( mObjectSynchronizers.Size() )
		delete mObjectSynchronizers[0];
	mNumberManager.Clear();

	// If server, send load map command to clients on other side
	if( IsServer() )
    {
		for( int i = 0; i < mPeers.Size(); ++i )
			mPeers[i]->SendLoadMap( levelName );
	}
	PostTick();

	if( GEngine->Video )
		GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp", levelName );

	bool wasPaused = GEngine->Game->mPaused;
	GEngine->Game->mPaused = true;

#ifdef	TRANSGAMING
	GEngine->pcfWorld->FileName = "";
	GScript.DoString("Game:LoadLevel('%s')", levelName);
	bool maploadok = (!GEngine->pcfWorld->FileName.IsNull());
#else
	GEngine->World->FileName = "";
	GScript.DoString("Game:LoadLevel('%s')",levelName.AsChar());
	bool maploadok = (!GEngine->World->FileName.IsNull());
#endif

	GEngine->Game->mPaused = wasPaused;

	if( IsServer() )
	{
#ifdef	TRANSGAMING
		GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->pcfWorld->FileName.AsChar(),GEngine->pcfWorld->LevelName.AsChar());
#else
		GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->World->FileName.AsChar(),GEngine->World->LevelName.AsChar());
#endif
		GScript.DoString("Game:OnPlay(true)");
	}
	else
	{
		{
			String name("=framerate");
			String value = String::Sprintf( "%d", ClientPeer()->mServerFPS );
			ClientPeer()->TransferVariable( name, value );
			name = "=lpo";
			value = String::Sprintf( "%d", ClientPeer()->mLocalPlayerOptimizedEveryNFrames );
			ClientPeer()->TransferVariable( name, value );
			name = "=mpap";
			value = String::Sprintf( "%d", ClientPeer()->mMaxPlayerActionsPassed );
			ClientPeer()->TransferVariable( name, value );
		}
		ClientPeer()->SendMapLoaded( maploadok );
		ClientPeer()->mConnectionState = NetworkPeer2::EStateSynchronizing;
		ClientPeer()->PrepareForFirstFrame();
	}

	if( GEngine->InputSys )
		GEngine->InputSys->Reset();
}

void NetworkDevice2::SetSynchroState( Entity* entity, unsigned char state )
{
	unsigned char oldState = entity->SynchroState();

	ulong32 uniqueIndex = (DWORD)-1;

	if( !( oldState & Entity::ESynchroEntity ) )
	{
		if( !(state & Entity::ESynchroEntity) )
			return;	// we don't care

		// synchro start
		uniqueIndex = mNumberManager.GetNext();
		mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex, uniqueIndex );

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (mObjectSynchronizersHashByEntity.Size() > mMaxObjectSyncByEntity){
            mMaxObjectSyncByEntity = mObjectSynchronizersHashByEntity.Size();
            TRACE("@@@ NetworkDevice2::SetSynchroState(): peak mObjectSynchronizersHashByEntity hash usage = %d\n", mMaxObjectSyncByEntity);
        }
#endif
	}
	else
	{
		ulong32* ui = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
		Assert( ui || mNumberManager.Empty() );
		if( ui )
			uniqueIndex = *ui;
	}

	if( uniqueIndex != (DWORD)-1 )
	{
		if( ( oldState & Entity::ESynchroEntity ) && !( state & Entity::ESynchroEntity ) )
		{
			EntitySynchronizer2** es = mObjectSynchronizersHash.Find( uniqueIndex );
			if( es )
				delete *es;
		}
		else if( !(oldState & Entity::ESynchroEntity) && (state & Entity::ESynchroEntity) )
		{
			EntitySynchronizer2* e = 0;
			switch( entity->Type )
			{
			case Entity::ETypeMesh:
                m_setTag("Net");
				e = (EntitySynchronizer2*)new MeshSynchronizer2( uniqueIndex, this, (WorldMesh*)entity);
				break;
			case Entity::ETypeModel:
                m_setTag("Net");
				e = (EntitySynchronizer2*)new ModelSynchronizer2( uniqueIndex, this, (Model*)entity);
				break;
			default:	// We don't serve your kind here...
				break;
			}
		}
	}

	if( !(state & Entity::ESynchroEntity) ) // synchro stop - ESynchroDelete works right here - it stops ESynchroEntity
	{
		if( !mNumberManager.Empty() )
		{
			mObjectSynchronizersHashByEntity.Remove( entity, entity->GIndex );
			mNumberManager.Free( uniqueIndex );
		}
	}
}

void NetworkDevice2::SendScriptMessage( UdpMessage* message, bool reliable, bool oneClientOnly, unsigned char specialClient )
{
	DelayedScriptMessageTemp t;
	t.Message = message;
	t.Reliable = reliable;
	t.OneClientOnly = oneClientOnly;
	t.SpecialClient = specialClient;
	mDelayedScriptMessagesStorage.Add(t);
}

void NetworkDevice2::DistributeScriptMessages( void )
{
	for( int a = 0; a < mDelayedScriptMessagesStorage.Size(); ++a )
	{
		DelayedScriptMessageTemp& t = mDelayedScriptMessagesStorage[a];

        m_setTag("Net");
		DelayedScriptMessage2* dsm = new DelayedScriptMessage2;
		dsm->Message = t.Message;
		dsm->ReferenceCounter = 0;

		if( IsServer() )
		{
			for( int i = 0; i < mPeers.Size(); ++i )
			{
				bool ok = false;
				bool rel = t.Reliable;
				unsigned char clientIndex = mPeers[i]->mMyIndex;

				if( t.Reliable )
				{
					if( t.SpecialClient == clientIndex )
						ok = true;	// send as reliable
					else if( !t.OneClientOnly )
					{
						if( t.SpecialClient == SERVER_ID )
							ok = true;	// send as reliable
						else if( t.SpecialClient != clientIndex )
						{
							rel = false;	// send as unreliable
							ok = true;
						}
					}
				}
				else if( !t.OneClientOnly || ( t.SpecialClient == clientIndex ) )
					ok = true;	// send as unreliable

				if( !ok )
					continue;

				if( ( !rel && ( mPeers[i]->mConnectionState >= NetworkPeer2::EStateSynchronizing ) ) ||
					( mPeers[i]->mConnectionState >= NetworkPeer2::EStatePlay ) )
				{
					DelayedScriptMessagePeerData d;
					d.DSM = dsm;
					d.IsReliable = rel;
					d.Number = rel?mPeers[i]->mReliableMessagesMaxNumber++:(DWORD)-1;
					mPeers[i]->mMessagesToSend.Add( d );
					++dsm->ReferenceCounter;
				}
			}
		}
		else if( IsClient() )
		{
			for( int i = 0; i < mPeers.Size(); ++i )
			{
				if( ( !t.Reliable && ( mPeers[i]->mConnectionState >= NetworkPeer2::EStateSynchronizing ) ) ||
					( mPeers[i]->mConnectionState >= NetworkPeer2::EStatePlay ) )
				{
					DelayedScriptMessagePeerData d;
					d.DSM = dsm;
					d.IsReliable = t.Reliable;
					d.Number = t.Reliable?mPeers[i]->mReliableMessagesMaxNumber++:(DWORD)-1;
					mPeers[i]->mMessagesToSend.Add( d );
					++dsm->ReferenceCounter;
				}
			}
		}

		if( !dsm->ReferenceCounter )
		{
			delete dsm->Message;
			delete dsm;
		}
		else
			mDelayedScriptMessageList.Add( dsm );
	}
	mDelayedScriptMessagesStorage.ClearArray();
}

void NetworkDevice2::ClearDispatchedMessages( void )
{
	for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
		if( !mDelayedScriptMessageList[i]->ReferenceCounter )
		{
			delete mDelayedScriptMessageList[i]->Message;
			delete mDelayedScriptMessageList[i];
			mDelayedScriptMessageList.FastRemove(i);
			--i;
		}
}

void NetworkDevice2::ClearScriptMessagesList( void )
{
	for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
	{
		delete mDelayedScriptMessageList[i]->Message;
		delete mDelayedScriptMessageList[i];
	}
	mDelayedScriptMessageList.ClearArray();

	for( int i = 0; i < mPeers.Size(); ++i )
		mPeers[i]->mMessagesToSend.ClearArray();
}

void NetworkDevice2::OnActivateAnimation( Model* model, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve )
{
}

void NetworkDevice2::OnLoadAnimation( Model* model, String name, bool looped, int animIndex )
{
}

#ifdef TRANSGAMING
float  NetworkDevice2::Ping( unsigned char clientID )
#else
double NetworkDevice2::Ping( unsigned char clientID )
#endif
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() )
			return 0;
		if( mPeers.Size() )
			return ClientPeer()->Ping();
		else
			return 9999.f;
	}

	for( int i = 0; i < mPeers.Size(); ++i )
		if( mPeers[i]->GetClientIndex() == clientID )
			return mPeers[i]->Ping();
	return 9999.f;
}

ulong32 NetworkDevice2::GetLastFrameLatency( void )
{
	return ( !IsServer() && mPeers.Size() )?ClientPeer()->LastFrameLatency:0L;
}

bool NetworkDevice2::IsSpectator( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() )
			return mSpectator;
		if( mPeers.Size() )
			return ClientPeer()->mSpectator;
		else
			return false;
	}

	for( int i = 0; i < mPeers.Size(); ++i )
		if( mPeers[i]->GetClientIndex() == clientID )
			return mPeers[i]->mSpectator;

	return false;
}

void NetworkDevice2::SetSpectator( unsigned char clientID, bool spectator )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() )
			mSpectator = spectator;
		else if( mPeers.Size() )
			ClientPeer()->mSpectator = spectator;
	}
	else
	{
		for( int i = 0; i < mPeers.Size(); ++i )
			if( mPeers[i]->GetClientIndex() == clientID )
				mPeers[i]->mSpectator = spectator;
	}
}

unsigned char NetworkDevice2::GetClientID( void )
{
	if( State == SERVER )
		return SERVER_ID;
	return ClientPeer()->mMyIndex;
}

void NetworkDevice2::DisconnectClient( unsigned char clientID )
{
	if( clientID == SERVER_ID )
		return;

	if( !IsServer() )
		return;

	for( int i = 0; i < mPeers.Size(); ++i )
		if( mPeers[i]->GetClientIndex() == clientID )
			mPeers[i]->Disconnect();
}

void NetworkDevice2::BanClient( unsigned char clientID )
{
	if( clientID == SERVER_ID )
		return;

	if( !IsServer() )
		return;

	for( int i = 0; i < mPeers.Size(); ++i )
		if( mPeers[i]->GetClientIndex() == clientID )
			mBannedHashes.AddUnique( mPeers[i]->mGameSpyHash );
}

String NetworkDevice2::GetNetstat( void )
{
	String text = "NETWORK PROTOCOL STATISTICS\n\n";
	text += String::Sprintf( "    FPS: %g", GEngine->avgFPS );
	if( GEngine->MinSecondsPerFrame )
		text += String::Sprintf( "        Max FPS: %.3f", floor(1.0f/GEngine->MinSecondsPerFrame) );

	text += String::Sprintf( "\n    Stats update period: %d ms", mStatsUpdateDelay );
	if( IsServer() )
		text += "\n    Server";
	else
	{
		text += "\n    Client";
		if( mPeers.Size() )
		{
			switch( ClientPeer()->mConnectionState )
			{
			case NetworkPeer2::EStateDisconnected:
				text += ", disconnected";
				break;
			case NetworkPeer2::EStateConnecting:
				text += " ,connecting";
				break;
			case NetworkPeer2::EStateConnected:
				text += ", connected";
				break;
			case NetworkPeer2::EStateSynchronizing:
				text += ", synchronizing entities";
				break;
			case NetworkPeer2::EStatePlay:
				text += ", playing";
				break;
			default:
				Assert( false );
			}
		}
	}

	if( mStatsUpdateDelay <= 0.f )
		return text;

	text += "\n\n    SPEEDS\n";
	text += String::Sprintf( "        Sent: %.0f Bps, %.0f packets per sec\n",
		((float)mLastUpdateBytesSent*1000)/mStatsUpdateDelay,
		((float)mLastUpdatePacketsSent*1000)/mStatsUpdateDelay );
	text += String::Sprintf( "        Received: %.0f Bps, %.0f packets per sec",
		((float)mLastUpdateBytesReceived*1000)/mStatsUpdateDelay,
		((float)mLastUpdatePacketsReceived*1000)/mStatsUpdateDelay );

	if( mPeers.Size() )
	{
		text += String::Sprintf( "\n\n    THROUGH CHANNELS - avg of last %d updates\n", mStatsNumberToAvg );
		text += String::Sprintf( "        Sent: %.0f bytes, %.0f packets, %.0f frames",
			((float)mStatsData.mBytesSent)/mStatsNumberToAvg,
			((float)mStatsData.mPacketsSent)/mStatsNumberToAvg,
			((float)mStatsData.mFramesSent)/mStatsNumberToAvg );
		text += ( mStatsData.mFramesSent )?String::Sprintf( ", %.0f bytes per frame\n",
			((float)mStatsData.mBytesSent)/mStatsData.mFramesSent ):"\n";
		text += String::Sprintf( "        Received: %.0f bytes, %.0f packets, %.0f frames",
			((float)mStatsData.mBytesReceived)/mStatsNumberToAvg,
			((float)mStatsData.mPacketsReceived)/mStatsNumberToAvg,
			((float)mStatsData.mFramesReceived)/mStatsNumberToAvg );
		if( mStatsData.mFramesReceived )
		text += String::Sprintf( ", %.0f bytes per frame", ((float)mStatsData.mBytesReceived)/mStatsData.mFramesReceived );
		if( mStatsData.mReliablePacketsAcknowledged )
			text += String::Sprintf( "\n        Reliable packets acknowledged: %.0f", 
				((float)mStatsData.mReliablePacketsAcknowledged)/mStatsNumberToAvg );
		text += String::Sprintf( "\n        Packets lost ( in last %d updates ): %d", mStatsNumberToAvg, mStatsData.mPacketsLost );
		if( mStatsData.mPacketsShouldBeReceived )
			text += String::Sprintf( " ( %d%% packet loss )", mStatsData.mPacketsLost*100/mStatsData.mPacketsShouldBeReceived );
		text += "\n        Last downstream frame stats";
		text += String::Sprintf( "\n            Entity creation: %u, update: %u, deletion: %u",
			ClientPeer()->mDFCCreate, ClientPeer()->mDFCUpdate, ClientPeer()->mDFCDelete );
		text += String::Sprintf( "\n            script reliables: %u, unreliables: %u",
			ClientPeer()->mDFCReliable, ClientPeer()->mDFCUnreliable );
		text += "\n        Last upstream frame stats";
		text += String::Sprintf( "\n            player deltas: %u, script reliables: %u, unreliables: %u",
			ClientPeer()->mUFCPlayerDeltas, ClientPeer()->mUFCReliable, ClientPeer()->mUFCUnreliable );

		if( mPeers.Size() > 1 )
		{
			for( int i = 0; i < mPeers.Size(); ++i )
			{
				PeerStats& stats = mPeers[i]->mStatsDataTemp;
				text += String::Sprintf( "\n\n    PEER %d - average of last %d updates", i, mStatsNumberToAvg );
				text += String::Sprintf( "\n        Sent: %.0f bytes, %.0f packets, %.0f frames",
					((float)stats.mBytesSent)/mStatsNumberToAvg,
					((float)stats.mPacketsSent)/mStatsNumberToAvg,
					((float)stats.mFramesSent)/mStatsNumberToAvg );
				text += String::Sprintf( "\n        Received: %.0f bytes, %.0f packets, %.0f frames",
					((float)stats.mBytesReceived)/mStatsNumberToAvg,
					((float)stats.mPacketsReceived)/mStatsNumberToAvg,
					((float)stats.mFramesReceived)/mStatsNumberToAvg );
				if( mStatsData.mReliablePacketsAcknowledged )
					text += String::Sprintf( "\n        Reliable packets acknowledged: %d",
						((float)stats.mReliablePacketsAcknowledged)/mStatsNumberToAvg );
				DWORD avgPing = mPeers[i]->mPingMedian.GetAverage( PING_AVERAGE_STEPS );
				if( avgPing )
					text += String::Sprintf( "\n        Ping: %d ms (averaged %d ms)", mPeers[i]->mPing, avgPing );
				text += String::Sprintf( "\n        Server framerate: %d frames per second", mPeers[i]->mServerFPS );
				text += "\n        Last downstream frame stats";
				text += String::Sprintf( "\n            Entity creation: %u, update: %u, deletion: %u",
					mPeers[i]->mDFCCreate, mPeers[i]->mDFCUpdate, mPeers[i]->mDFCDelete );
				text += String::Sprintf( "\n            script reliables: %u, unreliables: %u",
					mPeers[i]->mDFCReliable, mPeers[i]->mDFCUnreliable );
				text += "\n        Last upstream frame stats";
				text += String::Sprintf( "\n            player deltas: %u, script reliables: %u, unreliables: %u",
					mPeers[i]->mUFCPlayerDeltas, mPeers[i]->mUFCReliable, mPeers[i]->mUFCUnreliable );
			}
		}
		else
		{
			DWORD avgPing = mPeers[0]->mPingMedian.GetAverage( PING_AVERAGE_STEPS );
			if( avgPing )
				text += String::Sprintf( "\n        Ping: %d ms (averaged %d ms)", mPeers[0]->mPing, avgPing );
			text += String::Sprintf( "\n        Server framerate: %d frames per second", mPeers[0]->mServerFPS );
			if( IsClient() )
			{
				long32 bc = GetBandwidthControl();
				if( bc != -1 )
					text += String::Sprintf( "\n        Client bandwidth limiter set to: %d bytes per second (%.2f kBps)", bc, ((float)bc)/1024.f );
				else
					text += String::Sprintf( "\n        Client bandwidth limiter off." );
			}
		}
	}
	else
		text += "\n\n    No peers.";

	return text;
}

ulong32 NetworkDevice2::GetEntityNumber( Entity* entity )
{
	if( !entity )
		return (ulong32)-1;

	ulong32* uniqueIndex = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
	if( !uniqueIndex )	// fixed mesh, probably
		return (ulong32)-1;
	return *uniqueIndex;
}

Entity* NetworkDevice2::GetEntityFromNumber( ulong32 number )
{
	Assert( mPeers.Size() );
	EntitySynchronizer2** es = mObjectSynchronizersHash.Find( number );
	if( !es )
		return 0;
	
	Assert( (*es)->UniqueNumber == number );
	return (*es)->GetEntity();
}

Entity* NetworkDevice2::GetPlayerByIndex( int index )
{
	int t = GScript.GetTop();
	GScript.Call( "GetPlayerEntityByClient", 1, "%d", index );
	int gindex = GScript.GetInt( -1 );
	GScript.SetTop(t);
#ifdef	TRANSGAMING
	return (Entity*)GEngine->pcfWorld->GetEntityByIndex( gindex );
#else
	return (Entity*)GEngine->World->GetEntityByIndex( gindex );
#endif
}

Entity* NetworkDevice2::GetLocalPlayer( void )
{
	int index = SERVER_ID;
	if( IsClient() )
		index = ClientPeer()->mMyIndex;

	int t = GScript.GetTop();
	GScript.Call( "GetPlayerEntityByClient", 1, "%d", index );
	int gindex = GScript.GetInt( -1 );
	GScript.SetTop(t);
#ifdef	TRANSGAMING
	return (Entity*)GEngine->pcfWorld->GetEntityByIndex( gindex );
#else
	return (Entity*)GEngine->World->GetEntityByIndex( gindex );
#endif
}

void NetworkDevice2::SetStatsNumberToAverage( int newNumber )
{
	if( ( newNumber == mStatsNumberToAvg ) || ( newNumber <= 0 ) || ( newNumber >= 1024 ) )
		return;

	GScript.DoString( "Cfg.NetcodeStatsNumberToAverageFrom = %d", newNumber );
	mStatsNumberToAvg = newNumber;
	for( int i = 0; i < mPeers.Size(); ++i )
		mPeers[i]->SetStatsNumberToAverage( newNumber );
}

void NetworkDevice2::TransferVariable( unsigned char clientID, String& name, String& value )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() )
			InterpretVariable( SERVER_ID, name, value );
		else if( mPeers.Size() )
			ClientPeer()->TransferVariable( name, value );
	}
	else
	{
		for( int i = 0; i < mPeers.Size(); ++i )
			if( mPeers[i]->GetClientIndex() == clientID )
				mPeers[i]->TransferVariable( name, value );
	}
}

void NetworkDevice2::InterpretVariable( unsigned char from, String& name, String& value )
{
	// Interpret local NetworkDevice-sent variables

	// Pass it to the script to interpret
	GScript.Call( "Game_InterpretVariable",0,"%s,%s", name.AsChar(), value.AsChar() );
}

void NetworkDevice2::SetServerFramerate( int newFramerate )
{
	if( !IsServer() && mPeers.Size() )
	{
		String name("=framerate");
		String value = String::Sprintf( "%d", newFramerate );
		ClientPeer()->mServerFPS = newFramerate;
		ClientPeer()->TransferVariable( name, value );
	}
}

float NetworkDevice2::PacketLoss( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
		if( !IsServer() && mPeers.Size() )
			return ClientPeer()->PacketLoss();
	}
	else
		for( int i = 0; i < mPeers.Size(); ++i )
			if( mPeers[i]->mMyIndex == clientID )
				return mPeers[i]->PacketLoss();

	return 0.0f;
}

void NetworkDevice2::SetBandwidthControl( long32 bytesPerSecond )
{
	mBandwidthControlStartMs = GEngine->SysDriver.GetCurrentTimeMS();
	const DWORD timeToBackUp = 10;		// seconds
	mBandwidthControlStartMs -= timeToBackUp*1000;
	
	if( bytesPerSecond < 0 )
	{
		mBandwidthControlBytes = 0;
		mBandwidthControlBPS = ULONG_MAX;
	}
	else
	{
		mBandwidthControlBPS = (DWORD)bytesPerSecond;
		mBandwidthControlBytes = timeToBackUp*mBandwidthControlBPS;
	}
}

bool NetworkDevice2::TestBandwidthControl( void )
{
	if( mBandwidthControlBPS == ULONG_MAX )
		return true;

	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
	DWORD timeDiff = curTime - mBandwidthControlStartMs;
	if( curTime < mBandwidthControlStartMs )
		timeDiff = 0;

	const int second = 1000;
	const int resetInterval = 10*second;	// 10 seconds

	if( timeDiff > 2*resetInterval )
	{
		DWORD bytesLess = mBandwidthControlBPS*resetInterval/second;
		mBandwidthControlBytes = (mBandwidthControlBytes>bytesLess)?(mBandwidthControlBytes-bytesLess):0;
		mBandwidthControlStartMs += resetInterval;
		timeDiff -= resetInterval;
	}

	return ( mBandwidthControlBytes * second < mBandwidthControlBPS * timeDiff );
}

bool NetworkDevice2::ScriptsSecure( DWORD& crc )
{
	GFileManager m;

	// 1. If directory "../Data/LScripts" exists, return false
#ifdef TRANSGAMING
# ifndef TRANSGAMING_USE_PHYSFS
    if (m.FileExist("../Data/LScripts")){
		GLog.Print( "LScripts directory exists; scripts not secure.\n" );
		return false;
    }

    if (!m.FileExist("../Data/LScripts.pak")){
        GLog.Print( "Couldn't open LScripts.pak file; scripts not secure.\n" );
		return false;
    }
# endif
#else
	if( m.TestDirectory( "../Data/LScripts" ) )
	{
		GLog.Print( "LScripts directory exists; scripts not secure.\n" );
		return false;
	}

	// 2. If no file LScripts.pak exists, return false
	GFile* lscripts = m.CreateFileReader( "../Data/LScripts.pak" );
	if( !lscripts )
	{
		GLog.Print( "Couldn't open LScripts.pak file; scripts not secure.\n" );
		return false;
	}

	// 3. Get CRC of LScripts.pak
	crc = lscripts->GetCRC();
	m.Close( lscripts );
#endif

	GLog.Print( "LScripts secure\n" );

	return true;
}

#ifndef	TRANSGAMING_NO_GAMESPY

// ---------------------------- GAME SPY DATA METHODS ---------------------------------------------------

void GameSpyData2::CheckGameMode( void )
{
	// 1. Find out what it should be
	String newMode;
	switch( internalMode )
	{
	case 3:		newMode = "exiting";														break;
	case 0:		newMode = "closedwaiting";													break;
	case 1:		newMode =  mNet->CanConnectNewClient(false)?"openwaiting":"closedwaiting";	break;
	case 2:		newMode =  mNet->CanConnectNewClient(false)?"openplaying":"closedplaying";	break;
	default:	GLog.Print( "Warning: GameSpy internal mode set to unknown value!!!\n" );	return;
	}

	// 2. Find out if this calls for a mode change and change mode if it does
	if( newMode != Mode )
	{
		GLog.Print( "Changing game mode to: %s\n", newMode.AsChar() );
		Mode = newMode;
		if( mNet->mGameSpySDK )
            qr2_send_statechanged( NULL );
	}
}

bool GameSpyData2::GetServerValue( int keyid, int& valueInt, String& valueString )
{
//	GLog.Print( "GetServerValue called by GameSpy!\n" );

	switch( keyid )
	{
	case HOSTNAME_KEY:		valueString = ServerName; return false;
	case GAMEVER_KEY:		valueString = GameVersion; return false;

//	This one can't be returned when using one socket only
//	case HOSTPORT_KEY:
//		{
//			int port = 0;
//			GEngine->Net->mLowLayer.GetMyIP( &port );
//			valueInt = port;
//			return true;
//		}

	case MAPNAME_KEY:				valueString = MapName; return false;
	case NUMPLAYERS_KEY:			valueInt = GetPlayerCount(); return true;
	case NUMTEAMS_KEY:				valueInt = GetTeamCount(); return true;
	case MAXPLAYERS_KEY:			valueInt = MaxPlayers; return true;
	case GAMETYPE_KEY:				valueString = GameType; return false;
	case GAMEVARIANT_KEY:			valueInt = GameVariant; return true;
	case GAMEMODE_KEY:				valueString = Mode; return false;
	case TEAMPLAY_KEY:				valueInt = Teamplay; return true;
	case FRAGLIMIT_KEY:				valueInt = FragLimit; return true;
	case TEAMFRAGLIMIT_KEY:			valueInt = TeamFragLimit; return true;
	case TIMEELAPSED_KEY:			valueInt = TimeElapsed;	return true;
	case TIMELIMIT_KEY:				valueInt = TimeLimit; return true;
	case PASSWORD_KEY:				valueInt = Password.Len()?1:0; return true;

	// Our custom keys
	case GAMESPY_MAXSPECTATORS_KEY:	valueInt = MaxSpectators; return true;
	case GAMESPY_PASSWORDED_KEY:	valueInt = Password.Len()?1:0; return true;
	case GAMESPY_PKTV_PROXY_KEY:	valueString = PKTVProxy; return false;
	default:
		break;
	}

	// Else return custom key
	if( keyid >= GAMESPY_MODDERS_CUSTOM_KEY )
		for( int i = 0; i < mCustomKeys.Size(); ++i )
		if( mCustomKeys[i].KeyNumber == keyid )
		{
			if( mCustomKeys[i].IsString )
				valueString = mCustomKeys[i].ValueString;
			else
				valueInt = mCustomKeys[i].ValueInt;
			return !mCustomKeys[i].IsString;
		}

	// Else return empty string
	valueString = "";
	return false;
}

bool GameSpyData2::GetPlayerValue( int keyid, int index, int& valueInt, String& valueString )
{
//	GLog.Print( "GetPlayerValue called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( Players[i].Valid )
		{
			if( count == index )
			{
				switch( keyid )
				{
				case PLAYER__KEY:	valueString = Players[i].Name; return false;
				case SCORE__KEY:	valueInt = Players[i].Score; return true;
				case PING__KEY:		valueInt = Players[i].Ping; return true;
				case TEAM__KEY:		valueInt = Players[i].Team; return true;
				case DEATHS__KEY:	valueInt = Players[i].Deaths; return true;
				default:			valueString = ""; return false;
				};
			}
			count++;
		}
	}
	valueString = "";
	return false;
}

bool GameSpyData2::GetTeamValue( int keyid, int index, int& valueInt, String& valueString )
{
//	GLog.Print( "GetTeamValue called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( Teams[i].Valid )
		{
			if( count == index )
			{
				switch( keyid )
				{
				case TEAM_T_KEY:	valueString = Teams[i].Name; return false;
				case SCORE_T_KEY:	valueInt = Teams[i].Score; return true;
				default:			valueString = ""; return false;
				};
			}
			count++;
		}
	}
	valueString = "";
	return false;
}

int GameSpyData2::GetPlayerCount( void )
{
//	GLog.Print( "GetPlayerCount called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( Players[i].Valid )
			count++;
	return count;
}

int GameSpyData2::GetTeamCount( void )
{
//	GLog.Print( "GetTeamCount called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( Teams[i].Valid )
			count++;
	return count;
}

void GameSpyData2::RegisterCustomKey( String keyname, bool stringType, String value )
{
	if( keyname.Find( " " ) )				// no spaces in names
		return;

	if( mGameSpyCustomKeyNumber == 252 )	// all custom keys in use
		return;

	GSDCustomKey key;
	key.KeyNumber = mGameSpyCustomKeyNumber++;
	key.Keyname = keyname;
	key.IsString = stringType;
	key.ValueString = value;
	if( !stringType )
		key.ValueInt = atoi( value.AsChar() );

	mCustomKeys.Add( key );

	qr2_register_key( key.KeyNumber, keyname.AsChar() );
}

void GameSpyData2::SetCustomKey( String keyname, String value )
{
	if( keyname.Find( " " ) )	// no spaces in names
		return;

	for( int i = 0; i < mCustomKeys.Size(); ++i )
		if( mCustomKeys[i].Keyname == keyname )
		{
			mCustomKeys[i].ValueString = value;
			if( !mCustomKeys[i].IsString )
				mCustomKeys[i].ValueInt = atoi( value.AsChar() );
		}
}
#endif

// ---------------------------- IPAddress methods --------------------------------------------------

inline void IpAddress::FillSockaddrIn( struct sockaddr_in& addr )
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons( Port );
	addr.sin_addr.S_un.S_addr = IP;
}

bool GameSpyBackendTest( void )
{
#ifdef	TRANSGAMING_NO_GAMESPY
	return	false;
#else
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
#endif
}

// ---------------------------- ALL_SEEING EYE FUNCTIONS -------------------------------------------

void ASEQuery_wantstatus(void)
{
//	GLog.Print( "WantStatus called by All-Seeing Eye!\n" );

#if !defined( TRANSGAMING_NO_ASE ) && !defined( TRANSGAMING_NO_GAMESPY )
	ASEQuery_status(	GEngine->Net->GameSpyInfo.ServerName.AsChar(),
						GEngine->Net->GameSpyInfo.GameType.AsChar(),
						GEngine->Net->GameSpyInfo.MapName.AsChar(),
						GEngine->Net->GameSpyInfo.GameVersion.AsChar(),
						GEngine->Net->GameSpyInfo.Password.Len()?1:0,
						GEngine->Net->GameSpyInfo.GetPlayerCount(),
						GEngine->Net->GameSpyInfo.MaxPlayers );
#endif
}

void ASEQuery_wantrules(void)
{
#ifndef TRANSGAMING_NO_ASE
//	GLog.Print( "WantRules called by All-Seeing Eye!\n" );

	String t;

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.GameVariant );
	ASEQuery_addrule( "gamevariant", t.AsChar() );

	t = GEngine->Net->GameSpyInfo.GameState;
	ASEQuery_addrule( "gamestate", t.AsChar() );

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

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.NumPlayers );
	ASEQuery_addrule( "numplayers", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.NumSpectators );
	ASEQuery_addrule( "numspectators", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.MaxPlayers );
	ASEQuery_addrule( "maxplayers", t.AsChar() );

	t = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.MaxSpectators );
	ASEQuery_addrule( "maxspectators", t.AsChar() );

	// Teams scores
	String gameMode = GScript.Globals()["MPCfg"]["GameMode"].GetString();
	if( gameMode == "Team Deathmatch" || gameMode == "Capture The Flag" || gameMode == "Last Man Standing" )
	{
		int redScore = 0;
		int blueScore = 0;
		String redTeam = "";
		String blueTeam = "";
		for( int i = 0; i < MAX_PLAYERS; ++i )
		{
			if( GEngine->Net->GameSpyInfo.Players[i].Valid )
			{
				if( GEngine->Net->GameSpyInfo.Players[i].Team == 1 )
				{
					redScore += GEngine->Net->GameSpyInfo.Players[i].Score;
					String num = String::Sprintf( "%d ", i + 1 );
					redTeam += num;
				}
				else
				{
					blueScore += GEngine->Net->GameSpyInfo.Players[i].Score;
					String num = String::Sprintf( "%d ", i + 1 );
					blueTeam += num;
				}
			}
		}

		t = String::Sprintf( "%d", redScore );
		ASEQuery_addrule( "redteamscore", t.AsChar() );

		t = String::Sprintf( "%d", blueScore );
		ASEQuery_addrule( "blueteamscore", t.AsChar() );

		if( redTeam.Len() > 0 )
			redTeam[redTeam.Len()-1] = 0;
		if( blueTeam.Len() > 0 )
			blueTeam[blueTeam.Len()-1] = 0;

		ASEQuery_addrule( "redteam", redTeam.AsChar() );
		ASEQuery_addrule( "blueteam", blueTeam.AsChar() );
	}

	ScriptObject cfg = GScript.Globals()["Cfg"];

	// Admin info
	String admin = cfg["Admin"].GetString();
	String email = cfg["Email"].GetString();
	String url = cfg["URL"].GetString();
	String location = cfg["Location"].GetString();
	String modName = cfg["ModName"].GetString();
#if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
	String os = GEngine->SysDriver.GetOSName();
#endif

	if( !admin.IsNull() )
		ASEQuery_addrule( "admin", admin.AsChar() );
	if( !email.IsNull() )
		ASEQuery_addrule( "email", email.AsChar() );
	if( !url.IsNull() )
		ASEQuery_addrule( "url", url.AsChar() );
	if( !location.IsNull() )
		ASEQuery_addrule( "location", location.AsChar() );
	if( !modName.IsNull() )
		ASEQuery_addrule( "modname", modName.AsChar() );
#ifdef TRANSGAMING_PSP
    ASEQuery_addrule("os", "PSP");
#elif defined(TRANSGAMING_PS2)
    ASEQuery_addrule("os", "PS2");
#else
	if( !os.IsNull() )
		ASEQuery_addrule( "os", os.AsChar() );
#endif


	// Server config
	ASEQuery_addrule( "cfg_allowbrightskins", cfg["AllowBrightskins"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_allowbunnyhopping", cfg["AllowBunnyhopping"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_allowforwardrj", cfg["AllowForwardRJ"].GetBool() ? "true" : "false" );
	t = String::Sprintf( "%d", cfg["CaptureLimit"].GetInt() );
	ASEQuery_addrule( "cfg_capturelimit", t.AsChar() );
	ASEQuery_addrule( "cfg_dedicatedserver", cfg["DedicatedServer"].GetBool() ? "true" : "false" );
	t = String::Sprintf( "%d", cfg["FragLimit"].GetInt() );
	ASEQuery_addrule( "cfg_fraglimit", t.AsChar() );
	ASEQuery_addrule( "cfg_gamemode", cfg["GameMode"].GetString() );
	ASEQuery_addrule( "cfg_limitfps", cfg["LimitServerFPS"].GetBool() ? "true" : "false" );
	t = String::Sprintf( "%d", cfg["MaxPlayers"].GetInt() );
	ASEQuery_addrule( "cfg_maxplayers", t.AsChar() );
	t = String::Sprintf( "%d", cfg["MaxSpectators"].GetInt() );
	ASEQuery_addrule( "cfg_maxspectators", t.AsChar() );
	ASEQuery_addrule( "cfg_nowarmup", cfg["NoWarmup"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_powerupdrop", cfg["PowerupDrop"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_powerups", cfg["Powerups"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_public", cfg["PublicServer"].GetBool() ? "true" : "false" );
	t = String::Sprintf( "%d", cfg["ServerFPS"].GetInt() );
	ASEQuery_addrule( "cfg_serverfps", t.AsChar() );
	ASEQuery_addrule( "cfg_servername", cfg["ServerName"].GetString() );
	t = String::Sprintf( "%d", cfg["ServerPort"].GetInt() );
	ASEQuery_addrule( "cfg_serverport", t.AsChar() );
	ASEQuery_addrule( "cfg_password", ( String( cfg["ServerPassword"].GetString() ).IsNull() ) ? "0" : "1" );
	ASEQuery_addrule( "cfg_stopmatchonteamquit", cfg["StopMatchOnTeamQuit"].GetBool() ? "true" : "false" );
	ASEQuery_addrule( "cfg_teamdamage", cfg["TeamDamage"].GetBool() ? "true" : "false" );
	t = String::Sprintf( "%d", cfg["TimeLimit"].GetInt() );
	ASEQuery_addrule( "cfg_timelimit", t.AsChar() );
	t = String::Sprintf( "%d", cfg["WarmUpTime"].GetInt() );
	ASEQuery_addrule( "cfg_warmuptime", t.AsChar() );
	t = String::Sprintf( "%d", cfg["WeaponRespawnTime"].GetInt() );
	ASEQuery_addrule( "cfg_weaponrespawntime", t.AsChar() );
	ASEQuery_addrule( "cfg_weaponsstay", cfg["WeaponsStay"].GetBool() ? "true" : "false" );

	String votingAllowed = "00000000000000000000";
	if( cfg["UserAllowBrightskins"].GetBool() )
		votingAllowed[0] = '1';
	if( cfg["UserAllowBunnyhopping"].GetBool() )
		votingAllowed[1] = '1';
	if( cfg["UserAllowForwardRJ"].GetBool() )
		votingAllowed[2] = '1';
	if( cfg["UserBankick"].GetBool() )
		votingAllowed[3] = '1';
	if( cfg["UserCaptureLimit"].GetBool() )
		votingAllowed[4] = '1';
	if( cfg["UserFragLimit"].GetBool() )
		votingAllowed[5] = '1';
	if( cfg["UserGameMode"].GetBool() )
		votingAllowed[6] = '1';
	if( cfg["UserKick"].GetBool() )
		votingAllowed[7] = '1';
	if( cfg["UserLMSLives"].GetBool() )
		votingAllowed[8] = '1';
	if( cfg["UserMapChange"].GetBool() )
		votingAllowed[9] = '1';
	if( cfg["UserMaxPlayers"].GetBool() )
		votingAllowed[10] = '1';
	if( cfg["UserMaxSpectators"].GetBool() )
		votingAllowed[11] = '1';
	if( cfg["UserPowerupDrop"].GetBool() )
		votingAllowed[12] = '1';
	if( cfg["UserPowerups"].GetBool() )
		votingAllowed[13] = '1';
	if( cfg["UserReloadMap"].GetBool() )
		votingAllowed[14] = '1';
	if( cfg["UserTeamDamage"].GetBool() )
		votingAllowed[15] = '1';
	if( cfg["UserTimeLimit"].GetBool() )
		votingAllowed[16] = '1';
	if( cfg["UserWeaponRespawnTime"].GetBool() )
		votingAllowed[17] = '1';
	if( cfg["UserWeaponsStay"].GetBool() )
		votingAllowed[18] = '1';
	if( cfg["UserStartupWeapon"].GetBool() )
		votingAllowed[19] = '1';

	ASEQuery_addrule( "cfg_votingallowed", votingAllowed.AsChar() );

	ASEQuery_addrule( "PKTVProxy", GEngine->Net->GameSpyInfo.PKTVProxy.AsChar() );

	for( int i = 0; i < GEngine->Net->GameSpyInfo.NumofCustomKeys(); ++i )
		ASEQuery_addrule( GEngine->Net->GameSpyInfo.CustomKeyName( i ).AsChar(),
		GEngine->Net->GameSpyInfo.CustomKeyStringValue( i ).AsChar() );
#endif
}

void ASEQuery_wantplayers(void)
{
//	GLog.Print( "WantPlayers called by All-Seeing Eye!\n" );

	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( GEngine->Net->GameSpyInfo.Players[i].Valid )
		{
			String score = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.Players[i].Score );
			String ping = String::Sprintf( "%d", GEngine->Net->GameSpyInfo.Players[i].Ping );
			String team = "blue";
			if( GEngine->Net->GameSpyInfo.Players[i].Team == 1 )
				team = "red";

#ifndef TRANSGAMING_NO_ASE
			ASEQuery_addplayer( GEngine->Net->GameSpyInfo.Players[i].Name.AsChar(),
								GEngine->Net->GameSpyInfo.Teams[GEngine->Net->GameSpyInfo.Players[i].Team].Name.AsChar(),
								team.AsChar(),
								score.AsChar(),
								ping.AsChar(),
								"" );
#endif
		}
}
