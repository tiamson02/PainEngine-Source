
// #pragma once
#ifndef _NETWORKDEVICE2_
#define _NETWORKDEVICE2_


#define DEFAULT_SERVER_PORT 3455
#ifdef TRANSGAMING
# define CONNECTION_PACKET_TIMEOUT 5.0f
#else
# define CONNECTION_PACKET_TIMEOUT 5
#endif
#define CONNECTION_PACKET_NUMBER 2
#define SERVER_ID 255

#define CDKEY_ENABLED

#include <winsock2.h>
#include "Core/HashMap.h"
#include "Net/ObjectSynchronizer2.h"

class NetworkPeer2;
class UdpMessage;
class Entity;

class IpAddress
{
public:
	union {
		DWORD	IP;
		struct	{ BYTE ip1,ip2,ip3,ip4; };
	};
	unsigned short Port;

	IpAddress()
	{}
	IpAddress(BYTE a, BYTE b, BYTE c, BYTE d, unsigned short port)
	:	ip1(a), ip2(b), ip3(c), ip4(d), Port(port)
	{}

	bool operator==(const IpAddress& addr) const { return (IP == addr.IP) && (Port == addr.Port); };
	IpAddress& operator=(const IpAddress& addr) { IP = addr.IP; Port = addr.Port; return *this; };
	String AsString( void ) const { return String::Sprintf( "%d.%d.%d.%d:%d", ip1, ip2, ip3, ip4, Port ); };
	String AsStringIP( void ) const { return String::Sprintf( "%d.%d.%d.%d", ip1, ip2, ip3, ip4 ); };
	void FillSockaddrIn( struct sockaddr_in& addr );
};

template <>
inline DWORD HashFunc<IpAddress>(const IpAddress& addr)
{
	char buf[4];
	buf[0] = addr.ip1;
	buf[1] = addr.ip1;
	buf[2] = addr.ip1;
	buf[3] = addr.ip1;
    return Crc32(buf,4);
}

class PeerStats
{
public:
	DWORD mTimeOfCreation;
	ulong32 mBytesSent;
	ulong32 mBytesReceived;
	ulong32 mPacketsSent;
	ulong32 mPacketsReceived;
	ulong32 mFramesSent;
	ulong32 mFramesReceived;
	ulong32 mReliablePacketsAcknowledged;

	// Packet loss
	ulong32 mPacketsShouldBeReceived;
	ulong32 mPacketsLost;


	PeerStats( DWORD time = 0 ) : mBytesSent(0), mBytesReceived(0), mPacketsSent(0),
		mPacketsReceived(0), mFramesSent(0), mFramesReceived(0), mReliablePacketsAcknowledged(0),
		mPacketsShouldBeReceived(0), mPacketsLost(0), mTimeOfCreation(time) {};
	~PeerStats( void ) {};

	inline void Init( DWORD time = 0 )
	{
		mBytesSent = mBytesReceived = mPacketsSent = mPacketsReceived = mFramesSent = mFramesReceived =
			mPacketsShouldBeReceived = mPacketsLost = mReliablePacketsAcknowledged = 0;
		mTimeOfCreation = time;
	}

	inline PeerStats& operator=( const PeerStats& other )
	{
		mBytesSent = other.mBytesSent;
		mBytesReceived = other.mBytesReceived;
		mPacketsSent = other.mPacketsSent;
		mPacketsReceived = other.mPacketsReceived;
		mFramesSent = other.mFramesSent;
		mFramesReceived = other.mFramesReceived;
		mPacketsShouldBeReceived = other.mPacketsShouldBeReceived;
		mPacketsLost = other.mPacketsLost;
		mTimeOfCreation = other.mTimeOfCreation;
		return *this;
	}

	inline PeerStats& operator+=( const PeerStats& other )
	{
		mBytesSent += other.mBytesSent;
		mBytesReceived += other.mBytesReceived;
		mPacketsSent += other.mPacketsSent;
		mPacketsReceived += other.mPacketsReceived;
		mFramesSent += other.mFramesSent;
		mFramesReceived += other.mFramesReceived;
		mPacketsShouldBeReceived += other.mPacketsShouldBeReceived;
		mPacketsLost += other.mPacketsLost;
		if( mTimeOfCreation > other.mTimeOfCreation )
			mTimeOfCreation = other.mTimeOfCreation;
		return *this;
    }
};

enum MenuReportErrorCodes
{
	MREC_Information = 0,
	MREC_Disconnected = 1,
	MREC_BadPassword = 2,
	MREC_BadCDKey = 3,
};

enum MaintenanceIdent
{
	GENERAL_ERROR = 0,
	GENERAL_DISCONNECT = 1,

	// Low layer maintenance packets
	LOWLAYER_GETCHALLENGE = 2,
	LOWLAYER_CHALLENGE = 3,
	LOWLAYER_CONNECT = 4,
	LOWLAYER_CLIENTCONNECT = 5,

	// High layer maintenance packets
	HIGHLAYER_OPEN = 10,
	HIGHLAYER_LOADMAP = 11,
	HIGHLAYER_MAPLOADING = 12,
	HIGHLAYER_SYNCHRONIZED = 13,
	HIGHLAYER_SYNCHRONIZED_ACKNOWLEDGE = 14,
	HIGHLAYER_VARIABLE = 15,

	// Frame packets (above 15)
	HIGHLAYER_FRAME = 16,
	HIGHLAYER_SCRIPTMESGS = 17,
	HIGHLAYER_FIRSTFRAME = 18,
};

enum FramePartIdents
{
	FRAME_CREATE			= (1<<0),
	FRAME_UPDATE			= (1<<1),
	FRAME_SCRIPT			= (1<<2),
	FRAME_FIRSTACTIONFULL	= (1<<3),
	FRAME_DELETE			= (1<<4),
	FRAME_FILLER			= (1<<5),

	FIRSTFRAME_FRAMEDATA = (1<<0),
};

enum MaintenanceErrorCodes
{
	NOT_A_SERVER = 0,
	DIFFERENT_CLIENT_VERSION = 1,
	DIFFERENT_CLIENT_SCRIPTS = 2,
	BAD_PASSWORD = 3,
	BAD_CHALLENGE = 4,
	NO_CHALLENGE = 5,
	MAX_CLIENTS_ACHIEVED = 6,
	BAD_CD_KEY = 7,
	ANSWER_TO_NO_CHALLENGE = 8,
	NET_OPEN_WITHOUT_CONNECT = 9,
	BANNED_HASH = 10,
	CLIENT_PASSWORD_TOO_LONG = 11,
	MAX_FRAME_SIZE_EXCEEDED = 12,
	CHALLENGE_RESPONSE_TOO_LONG = 13,
	PURE_SCRIPTS_FAIL = 14,
};

enum NetcodeTokens
{
	MESH_CREATE		= 0,
	MESH_UPDATE		= 1,
	MESH_DELETE		= 2,
	MODEL_CREATE	= 3,
	MODEL_UPDATE	= 4,
	MODEL_DELETE	= 5,
	MODEL_OTHER		= 6,
	PHO_CREATE		= 7,
	PHO_UPDATE		= 8,
	PHO_DELETE		= 9,
	PHO_OTHER		= 10,

	NCL_LOADMAP		= 11,
	NCL_MAPLOADING	= 12,
	NCL_SYNCHRO		= 13,
	NCL_SYNCHROACK	= 14,
	NCL_UPDATECL	= 15,
	NCL_UPDATEMPS	= 16,
	NCL_LATENCY		= 17,
	NCL_PLACTION	= 18,
	NCL_CLOPEN		= 19,

	IPD_GETCH		= 20,
	IPD_CHALLENGE	= 21,
	IPD_CONNECT		= 22,
	IPD_CLCONNECT	= 23,

	NCL_ENTITYMSG	= 24,
	NCL_SCRIPTMSG	= 25,

	NCL_VARIABLE	= 26,
};

class IPConnectionData
{
public:

	enum ConnectionState
	{
		FAILED = 0,
		SENT_CHALLENGE = 1,
		AUTHENTICATING = 2,
		SENT_CONNECT = 3,
		CONNECTED = 4,
	} State;

	IpAddress ChallengeAddress;	// Address from which challenge came
#ifdef TRANSGAMING
	float  ChallengeTime;		// Times at which challenge came
#else
	double ChallengeTime;		// Times at which challenge came
#endif
	String Challenge;			// Challenge string
	bool Spectator;				// Is a spectator
};

// Has to be the power of 2
#define UNIQUE_NUMBER_QUARANTINE_SIZE 1024

class UniqueNumberReuser2
{
public:
	UniqueNumberReuser2( void );
	~UniqueNumberReuser2( void ) {};
	ulong32 GetNext( void );
	void Free( ulong32 uniqueNumber );
    void Clear();
    bool Empty();

private:
	ulong32 mNextNumber;
	ulong32 mQuarantined[UNIQUE_NUMBER_QUARANTINE_SIZE];
	int mStart, mEnd;
	DynamicArray<ulong32> mFree;
};

// ****************** GameSpy Data *********************************

// Additional GameSpy keys
#define GAMESPY_MAXSPECTATORS_KEY 100
#define GAMESPY_PASSWORDED_KEY 101
#define GAMESPY_PKTV_PROXY_KEY 102
#define GAMESPY_MODDERS_CUSTOM_KEY 103

extern int mGameSpyCustomKeyNumber;

#define MAX_PLAYERS 16

class GameSpyPlayerData2
{
public:
	bool			Valid;
	String			Name;			// a player name (may include spaces)
	int				Score;			// contains the score (kills/points) for a single player
	unsigned short	Ping;			// the ping for a player (as measured between the player and the server)
	int				Team;			// the team a player is on, either numeric or string
	int				Deaths;			// number of deaths a player has had

	GameSpyPlayerData2( void ) : Valid(false), Score(0), Ping(999), Team(0), Deaths(0) {};
};

class GameSpyTeamData2
{
public:
	bool			Valid;
	String			Name;
	int				Score;

	GameSpyTeamData2( void ) : Valid(false), Name("Undefined"), Score(0) {};
};

class NetworkDevice2;

struct GSDCustomKey
{
	int KeyNumber;
	String Keyname;
	bool IsString;
	String ValueString;
	int ValueInt;
};

class GameSpyData2
{
public:

	enum GameMode
	{
		Start = 0,
		Waiting = 1,
		Playing = 2,
		Exiting = 3,
	};

	String	ServerName;				// the name of the server
	String	MapName;				// the map name (either filename or descriptive name)
	String	GameType;				// string which specifies the type of game, or the mod being played
	int		GameVariant;			// if the particular game type has multiple variants, you can report it using this key
	String	GameState;				// warmup/countdown/playing/finished
	String	GameVersion;			// Game Version

	String	PKTVProxy;

	int		MaxPlayers;				// max number of players for this server
	int		MaxSpectators;			// max number of spectators for this server

	int		Teamplay;				// number which defines the type of teamplay in use, or 0 for no teamplay. Values > 0 are up to the developer.

	int		FragLimit;				// number of total kills or points before a level change or game restart
	int		TeamFragLimit;			// number of total kills or points for a team before a level change or game restart

	int		TimeLimit;				// amount of total time before a level change or game restart occurs (generally in minutes)
	int		TimeElapsed;			// amount of time (in seconds) since the current level or game started (zamienic na timeremaining, jesli GameSpy to potrafi wyswietlic.)

	int		NumPlayers;				// number of players currently on the server
	int		NumSpectators;			// number of spectators currently on the server

	String	Password;				// server password

    GameSpyPlayerData2	Players[MAX_PLAYERS];	// Player data
	GameSpyTeamData2	Teams[MAX_PLAYERS];		// Team data

	GameSpyData2( void ) : mNet(0), MapName("None"), GameType("Default"), GameVariant(1), GameState("Finished"), MaxPlayers(MAX_PLAYERS), MaxSpectators(1),
		Teamplay(0), FragLimit(0), TeamFragLimit(0), TimeLimit(0), TimeElapsed(0), NumPlayers(0), NumSpectators(0), Password(""),
		ServerName("Painkiller MP"), Mode("closedwaiting"), GameVersion("0.0"), internalMode(0), PKTVProxy("") {};
	~GameSpyData2( void ) {};

	void SetMode( int mode )
	{
		internalMode = mode;
#ifndef TRANSGAMING_NO_GAMESPY
		CheckGameMode();
#endif
	}

	void SetNet( NetworkDevice2* net ) { mNet = net; };

	// Custom keys
	void RegisterCustomKey( String keyname, bool stringType, String value );
	void SetCustomKey( String keyname, String value );
#ifndef TRANSGAMING_NO_GAMESPY
	void CheckGameMode( void );
#endif
	// Information callbacks - has to be public - else I would have to declare GameSpy types here
	bool GetServerValue( int keyid, int& valueInt, String& valueString );
	bool GetPlayerValue( int keyid, int index, int& valueInt, String& valueString );
	bool GetTeamValue( int keyid, int index, int& valueInt, String& valueString );
	int GetPlayerCount( void );
	int GetTeamCount( void );

	int NumofCustomKeys( void ) { return mCustomKeys.Size(); };
	int CustomKeyKeyNumber( int i ) { return mCustomKeys[i].KeyNumber; };
	String CustomKeyName( int i ) { return mCustomKeys[i].Keyname; };
	String CustomKeyStringValue( int i ) { return mCustomKeys[i].ValueString; };

protected:
	String Mode;
	int internalMode;
	NetworkDevice2* mNet;
	DynamicArray<GSDCustomKey> mCustomKeys;
};

struct DelayedScriptMessageTemp
{
	UdpMessage* Message;
	bool Reliable;
	bool OneClientOnly;
	unsigned char SpecialClient;
};

struct DelayedScriptMessage2
{
	UdpMessage* Message;
	unsigned char ReferenceCounter;
};

String GetSocketErrorString( void );	// translate WSA last error to string

// ***************************** Main class ******************************

class ENGINE_API NetworkDevice2
{
	// Friend classes and functions
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	friend void StaticGameSpyCDKEYCallback_ND2( int productid, int localid, int authenticated, char* errmsg, void* instance );
	friend void RealizeNegotiateCompleted_ND2( NetworkDevice2* i, struct sockaddr_in* remoteaddr );
	friend void RealizeNegotiateFailed_ND2( NetworkDevice2* i, struct sockaddr_in* remoteaddr );
	friend void NatNegotiationCallback_ND2( int cookie, void* userdata );
#else
	friend static void StaticGameSpyCDKEYCallback( int productid, int localid, int authenticated, char* errmsg, void* instance );
	friend static void RealizeNegotiateCompleted( NetworkDevice2* i, struct sockaddr_in* remoteaddr );
	friend static void RealizeNegotiateFailed( NetworkDevice2* i, struct sockaddr_in* remoteaddr );
	friend static void NatNegotiationCallback( int cookie, void* userdata );
#endif

	friend class NetworkPeer2;
	friend class GameSpyData2;
	friend class EntitySynchronizer2;

public:
	// External methods
	// ----------------------------------------------
	NetworkDevice2();
	~NetworkDevice2();

	// Maintenance
	void PreTick( float timeDelta );
	void PostTick( void );	

	// Start & Stop
	bool NetworkServer( String mapName, String password, String interfaceToUse, unsigned short port = DEFAULT_SERVER_PORT );
	bool NetworkClientAndConnect( const char* hostname, String password, unsigned short port = 0, bool spectator = false );
	bool NetworkClientAndConnect( void* hostFromGameSpy, String password, bool spectator = false );
	void NetworkClose( void );

	bool ScriptsSecure( DWORD& crc );

	void LoadNewMap(const char *levelName);

	// Information
	bool IsServer( void ) { return State == SERVER; };					// Tells if we are a server or a client now
	bool IsClient( void ) { return State == CLIENT; };					// Tells if we are a server or a client now
	String GetMyIP( int* port );
	bool CanConnectNewClient( bool spectator );
#ifdef TRANSGAMING
	float Ping( unsigned char clientID );
#else
	double Ping( unsigned char clientID );
#endif
	float PacketLoss( unsigned char clientID );
	void PingReset( unsigned char clientID ) {};	//nothing, for now; maybe if we average the ping from more packets...
	bool IsSpectator( unsigned char clientID );
	void SetSpectator( unsigned char clientID, bool spectator );
	unsigned char GetClientID( void );
	void DisconnectClient( unsigned char clientID );
	void BanClient( unsigned char clientID );

	// Entity number transfer
	ulong32 GetEntityNumber( Entity* entity );
	Entity* GetEntityFromNumber( ulong32 number );
	Entity* GetPlayerByIndex( int index );
	Entity* GetLocalPlayer( void );

	// Variable transfer ( using reliable messages )
	void TransferVariable( unsigned char clientID, String& name, String& value );
	void InterpretVariable( unsigned char from, String& name, String& value );

	void SetCDKeyAndIsPublic( String cdkey, bool isPublic ) { mCDKey = cdkey; mPublic = isPublic; };
//	void BanHash( String hash ) { mBannedHashes.AddUnique( hash ); };

	// Stats
	inline void SetStatsUpdateDelay( DWORD delay ) { mStatsUpdateDelay = delay; };
	inline void GetLastUpdateStats( ulong32& sent, ulong32& received ) { sent = mLastUpdateBytesSent; received = mLastUpdateBytesReceived; };
	String GetNetstat( void );
	void SetStatsNumberToAverage( int newNumber );

	ulong32 GetLastFrameLatency( void );

	// Protocol control
	void SetServerFramerate( int newFramerate );	// works only from client

	// Animation synchro calls
	void OnActivateAnimation( Model* model, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve );
	void OnLoadAnimation( Model* model, String name, bool looped, int animIndex );

	GameSpyData2 GameSpyInfo;

	void SetSynchroState( Entity* entity, unsigned char state );

	// Script messages interface
	void SendScriptMessage( UdpMessage* message, bool reliable, bool oneClientOnly, unsigned char specialClient );

	// Client bandwidth control
	void SetBandwidthControl( long32 bytesPerSecond );
	long32 GetBandwidthControl( void ) { return (mBandwidthControlBPS == ULONG_MAX)?-1:(long32)mBandwidthControlBPS; }

private:

	// Client bandwidth control
	DWORD mBandwidthControlStartMs;
	DWORD mBandwidthControlBytes;
	DWORD mBandwidthControlBPS;
	bool TestBandwidthControl( void );
	void UpdateBandwidthControl( DWORD bytesSent ) { mBandwidthControlBytes += bytesSent; }

	void DistributeScriptMessages( void );
	// High Level Variables
	// ----------------------------------------------
	bool mSpectator;		// is our player in player mode, or in the spectator mode?

	// Both Levels Variables
	// ----------------------------------------------

	DynamicArray<NetworkPeer2*> mPeers;				// Each of those represents one host
	NetworkPeer2* ClientPeer( void ) { return mPeers[0]; };

	// Low Level Variables
	// ----------------------------------------------

	enum LowLayerStates
	{
		STARTUP = 0,
		INITIALIZED = 1,
		CONNECTING = 2,
		CHALLENGED = 3,
		CLIENT = 4,
		SERVER = 5,
		NAT_NEGOTIATING = 6,
	} State;			// node of the low-layer state machine

	IpAddress mLocalAddr;							// Our local address in my structure
	IpAddress mServerAddr;							// Server address - usable only if we're client
	SOCKET mSocket;									// Our socket
	bool mConnReset;								// did we get a 'Port Unreachable' ICMP message?
	bool mGameSpySDK;								// this tells if GameSpy SDK is initialized or not
	String mCDKey;									// Our cdkey
	bool mPublic;									// this tells if our server is public or not
	int mAseSDK;									// this tells if All-Seeing-Eye SDK is initialized or not
#ifdef TRANSGAMING
    float  mLastConnectTime;						// this remembers when was the last time we sent connection request
#else
    double mLastConnectTime;						// this remembers when was the last time we sent connection request
#endif
	int mLastConnectPackets;						// number of packets already sent without answer

	bool mPureScripts;								// strict scripts checking?
	DWORD mCRC;										// LScripts crc

	DynamicArray<IPConnectionData> mChallenges;		// Quake-style challenges table - first communication struct

	// Low level stats measurement
	PeerStats mStatsData;
	unsigned short mStatsNumberToAvg;
	ulong32 mCurrentUpdateBytesSent;			// bytes sent since last stats update
	ulong32 mCurrentUpdateBytesReceived;		// bytes received since last update
	ulong32 mLastUpdateBytesSent;				// bytes sent in last update
	ulong32 mLastUpdateBytesReceived;			// bytes received in last update
	ulong32 mCurrentUpdatePacketsSent;		// packets sent since last stats update
	ulong32 mCurrentUpdatePacketsReceived;	// packets received since last update
	ulong32 mLastUpdatePacketsSent;			// packets sent in last update
	ulong32 mLastUpdatePacketsReceived;		// packets received in last update
	DWORD mLastStatsUpdateTime;						// last time stats were updated
	DWORD mStatsUpdateDelay;						// how often do we update stats

	// Challenge data
	DynamicArray<IpAddress> mAddressesForCDKEY;
	DynamicArray<String> mPlayerHashes;
	DynamicArray<String> mBannedHashes;

	UniqueNumberReuser2 mClientNumberManager;		// Peer indexes
	DynamicArray<DelayedScriptMessage2*> mDelayedScriptMessageList;
	DynamicArray<DelayedScriptMessageTemp> mDelayedScriptMessagesStorage;

	void ClearDispatchedMessages( void );
	void ClearScriptMessagesList( void );

	// Entity-to-Synchronizer hash and synchronizer number manager
	UniqueNumberReuser2 mNumberManager;
	DynamicArray<EntitySynchronizer2*>						mObjectSynchronizers;

#ifdef TRANSGAMING
	//@@@// eric-> reduced the size of these hash tables for the PSP and PS2
    HashMapSpecial<void*,unsigned int,ulong32,256>	mObjectSynchronizersHashByEntity;
	HashMap<ulong32,EntitySynchronizer2*,1024>		mObjectSynchronizersHash;
# ifdef FIND_HASH_PEAK
    static int mMaxObjectSyncByEntity;
    static int mMaxObjectSync;
# endif
#else
    HashMapSpecial<void*,unsigned int,ulong32,256>	mObjectSynchronizersHashByEntity;
	HashMap<ulong32,EntitySynchronizer2*,1024>		mObjectSynchronizersHash;
#endif

	// Low Layer Methods
	// ----------------------------------------------

	// State switches
	bool LowLayer_Open( unsigned short serverPort,
		bool server = false, String interfaceToUse = "" );				// any state -> INITIALIZED, destroys socket
	void LowLayer_Close();												// any state -> STARTUP
	void LowLayer_Disconnect();											// CONNECTED, CHALLENGED, CONNECTING -> INITIALIZED, does not destroy socket
	void LowLayer_ConnectAsClient( IpAddress& addr );					// any state -> CONNECTING
	void LowLayer_ConnectAsClient( void* serverBrowserData );			// this allows to connect to servers behind NAT too.
	void LowLayer_StartServer( void );									// any state -> SERVER

	// Generic net methods
	void LowLayer_GameSpyCDKEYCallback( int localid, int authenticated, char* errmsg );

	bool LowLayer_SendMaintenanceMessage( IpAddress& to, UdpMessage* maintenance );
	bool LowLayer_Write( const IpAddress& ipAddr, UdpMessage* message );
	bool LowLayer_Read( IpAddress& ipAddr, UdpMessage* message );
	NetworkPeer2* LowLayer_FindPeerByAddress( IpAddress& addr );

	// Command handlers
	void HandleCommand_Error( IpAddress& from, UdpMessage* message );
	void HandleCommand_Disconnect( IpAddress& from );
	void HandleCommand_GetChallenge( IpAddress& from );
	void HandleCommand_Challenge( IpAddress& from, UdpMessage* message );
	void HandleCommand_Connect( IpAddress& from, UdpMessage* message );
	void HandleCommand_ClientConnect( IpAddress& from, UdpMessage* message );
	bool ReadyForNetOpen( IpAddress* address );

	// This contains reactions for game-addressed packages ( the ones not addressed to a certain peer object )
	void LowLayer_HandleNetworkDevicePackage( UdpMessage* message, IpAddress& from );

};


#endif //_NETWORKDEVICE2_

