#error This file is obselete

// #pragma once
#ifndef _IPDRIVER_
#define _IPDRIVER_


class IPDriver;
class UdpMessage;
class IPChannel;
class NetworkClient;

#include "Core/LeString.h"

#include <winsock2.h>

//#define PACKET_LOG

#define HAVOK_COLLISION_TOLERANCE 0.01f
//#define LOG_STRING_PACKETS

#define CONNECTION_PACKET_TIMEOUT 5
#define CONNECTION_PACKET_NUMBER 2
#define DEFAULT_SERVER_PORT 3455
#define CHALLENGES_MAX 1000
#define STREAMING_TIMEOUT_TIME 30000
#define STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD 120000
#define STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD 60000
#define MAX_TIME_BETWEEN_PACKETS 300
#ifndef TRANSGAMING
# define FILE_CHUNK_SIZE 1024
#endif

#define PING_PACKETS_AVERAGED 30
#define PACKET_LOSS_FROM 10			// from last 10 updates
#define PACKET_LOSS_TICK 1000		// update every second ( 1000 ms )

#define IPCHANNEL_STATS_FROM 5		// from last 5 updates
#define IPCHANNEL_STATS_TICK 1000	// update every second ( 1000 ms )

enum MaintenanceIdent
{
	GENERAL_ERROR = 0,
	GENERAL_DISCONNECT = 1,

	// IP Driver maintenance packets
	IPDRIVER_GETCHALLENGE = 2,
	IPDRIVER_CHALLENGE = 3,
	IPDRIVER_CONNECT = 4,
	IPDRIVER_CLIENTCONNECT = 5,

	// Net Client maintenance packets
	NETCLIENT_PLAYERACTION = 7,
	NETCLIENT_LATENCY = 8,
	NETCLIENT_UPDATEMAXPACKETSIZE = 9,
	NETCLIENT_OPEN = 10,
	NETCLIENT_LOADMAP = 11,
	NETCLIENT_MAPLOADING = 12,
	NETCLIENT_SYNCHRONIZED = 13,
	NETCLIENT_SYNCHRONIZED_ACKNOWLEDGE = 14,
	NETCLIENT_UPDATECLASSES = 15,
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
};

enum MenuReportErrorCodes
{
	MREC_Information = 0,
	MREC_Disconnected = 1,
	MREC_BadPassword = 2,
	MREC_BadCDKey = 3,
};

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
	String AsString( void ) { return String::Sprintf( "%d.%d.%d.%d:%d", ip1, ip2, ip3, ip4, Port ); };
	String AsStringIP( void ) { return String::Sprintf( "%d.%d.%d.%d", ip1, ip2, ip3, ip4 ); };
	void FillSockaddrIn( struct sockaddr_in& addr );
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
	double ChallengeTime;		// Times at which challenge came
	String Challenge;			// Challenge string
	bool Spectator;				// Is a spectator
};

class IPDriver
{
	friend class IPChannel;
	friend class NetworkDevice;
	friend class NetworkClient;
	friend static void StaticGameSpyCDKEYCallback( int productid, int localid, int authenticated, char* errmsg, void* instance );
public:

	enum IPDriverStates
	{
		STARTUP = 0,
		INITIALIZED = 1,
		CONNECTING = 2,
		CHALLENGED = 3,
		CLIENT = 4,
		SERVER = 5,
		NAT_NEGOTIATING = 6,
	};

	// Methods
	IPDriver();									// Construction
	~IPDriver();								// Destruction

	void Tick( float timeDelta );				// Tick
	void PlayTick( float timeDelta );			// Tick when playing a recording
	void WasteMessages( void );
	void TickChannels( void );

	bool Resolve(const char* hostName, IpAddress& ipAddr);		// Makes IP address out of string
	static String GetSocketErrorString( void );

	// GameSpy handling
	void GameSpyStateChanged( void );
	void GameSpyCDKEYClientDeletion( int index );

	// State switches
	bool Open( unsigned short serverPort, bool server = false, String interfaceToUse = "" );
														// any state -> INITIALIZED, destroys socket
	void Close();										// any state -> STARTUP
	void Disconnect();									// CONNECTED, CHALLENGED, CONNECTING -> INITIALIZED, does not destroy socket
	void ConnectAsClient( IpAddress& addr );			// any state -> CONNECTING
	void ConnectAsClient( void* serverBrowserData );	// this allows to connect to servers behind NAT too.
	void StartServer( void );							// any state -> SERVER

	String GetMyIP( int* port );
	void SetCDKeyAndIsPublic( String cdkey, bool isPublic ) { CDKey = cdkey; IsPublic = isPublic; };
	void BanHash( String hash ) { mBannedHashes.AddUnique( hash ); };

	// Stats
	inline void SetStatsUpdateDelay( DWORD delay ) { mStatsUpdateDelay = delay; };
	inline void GetLastUpdateStats( unsigned long& sent, unsigned long& received ) { sent = mLastUpdateBytesSent; received = mLastUpdateBytesReceived; };

	DWORD mTimeSent;

private:
	void HandleCommand_GetChallenge( IpAddress& from );							// client -> get challenge	-> server
	void HandleCommand_Challenge( IpAddress& from, UdpMessage* message );		// client <- challenge		<- server
	void HandleCommand_Connect( IpAddress& from, UdpMessage* message );			// client -> connect		-> server
	void HandleCommand_ClientConnect( IpAddress& from, UdpMessage* message );	// client <- client_connect	<- server
	void HandleCommand_Disconnect( IpAddress& from );
	void HandleCommand_Error( IpAddress& from, UdpMessage* message );
	void HandleCommand_Ping( IpAddress& from, UdpMessage* message );
	void HandleCommand_Pong( IpAddress& from, UdpMessage* message );

	bool ReadyForNetOpen( IPChannel* channel );

	int CountAuthenticatingClients( bool spectator );

	// First packet problems variables
	double mLastConnectTime;
	int mLastConnectPackets;

	// Methods
	// -------------------------------------------------------------

	// This contains reactions for game-addressed packages ( the ones not addressed to a certain channel )
	void HandleIPDriverPackage( UdpMessage* message, IpAddress& from );

	// Read from and write to game socket
	bool SendMaintenanceMessage( IpAddress& to, UdpMessage* maintenance );
	bool Write( const IpAddress& ipAddr, UdpMessage* message );
	bool WriteInternal( const IpAddress& ipAddr, UdpMessage* message );
	bool Read( IpAddress& ipAddr, UdpMessage* message );
	bool ReadInternal( IpAddress& ipAddr , UdpMessage* message );
	void SetSimulatedLatency( DWORD latency );
	DWORD GetSimulatedLatency( void ) { return mSimulatedLatency*2; };

	// Maintenance
	void RemoveChannel( IPChannel* c );					// IPDrivers call this when they want to destroy themselves
	IPChannel* FindChannelByAddress( IpAddress& addr );	// We can easily find channel from address with this
	IPChannel* CreateIPChannel( IpAddress& ipAddr );	// Creates a new transmission channel

	// Variables
	// -------------------------------------------------------------
	DWORD	State;

	IpAddress mLocalAddr;							// Our local address in my structure
	IpAddress mServerAddr;							// Server address - usable only if we're client
	
	DynamicArray<IPConnectionData> mChallenges;		// Quake-style challenges table - first communication struct
	DynamicArray<IPChannel*> mChannels;				// Array of channels for communication with other hosts - second comm struct

	// GameSpy integration
	void GameSpyCDKEYCallback( int localid, int authenticated, char* errmsg );	// that's part of handle_connect
	bool mGameSpySDK;
	int mASE_SDK;
	String CDKey;
	bool IsPublic;

	// Challenge data
	DynamicArray<IpAddress> mAddressesForCDKEY;
	DynamicArray<String> mPlayerHashes;

	DynamicArray<String> mBannedHashes;

	friend static void NatNegotiationCallback( int cookie, void* userdata );
	friend static void RealizeNegotiateCompleted( IPDriver* i, struct sockaddr_in* remoteaddr );
	friend static void RealizeNegotiateFailed( IPDriver* i, struct sockaddr_in* remoteaddr );

	// Stats measurement
	DWORD mLastStatsUpdateTime;
	DWORD mStatsUpdateDelay;
	unsigned long mCurrentUpdateBytesSent;
	unsigned long mCurrentUpdateBytesReceived;
	unsigned long mLastUpdateBytesSent;
	unsigned long mLastUpdateBytesReceived;

	// Internal socket parameters
	SOCKET mSocket;									// Our socket
	bool mConnReset;								// did we get a 'Port Unreachable' ICMP message?

	// Latency simulator
	struct LatencySimEntry
	{
		DWORD Time;
		IpAddress Address;
		UdpMessage* Message;
	};

	void ClearLatencySimulator( void );
	void SendAllWaitingMessages( void );
	void ClearAllMessagesForAddress( IpAddress& address );

	DWORD mSimulatedLatency;
	DynamicArray<LatencySimEntry> mWaitingToSend;
	DynamicArray<LatencySimEntry> mWaitingToReceive;
};



#endif //_IPDRIVER_

