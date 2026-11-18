#ifndef __PKTVMANAGER_H__
#define __PKTVMANAGER_H__

#include "NetworkDevice2.h"
#include "PKTVEntityProxies.h"
#include "PKTVTransport.h"
#include "PKTVStringDict.h"
#include "SocketClosingModule.h"
#include "Core/ValuesMonitor.h"

#define HOST_PING_TIMEOUT 10000	// ms
#define HOST_IPS_TO_PING 10

#define DEFAULT_PKTV_SERVER_PORT 3459
#define DEFAULT_PKTV_PROXY_UDP_PORT 3461

//#define ENTITY_CONTENTS_FLAG_MONITORING
#define PKTV_CONNECTION_DEBUG

// The packed frame can be stored in the UdpMessage.
class PKTVTransport;
class PKTVTransportFile;
class PKTVTransportTCP;
class PKTVUnpackedFrame;

class IpAddress2
{
	IpAddress	mAddress;
	String		mName;
public:
	IpAddress2() : mName("") {};
	bool SetName( String name );
	void SetAddress( IpAddress addr ) { mName = ""; mAddress = addr; };
	IpAddress GetAddress( void ) { return mAddress; };
	const String& GetName( void ) { return mName; };

	IpAddress2& operator=(const IpAddress2& other )
    {
		mAddress = other.mAddress;
		mName = other.mName;
        return (*this);
    }

};

class PKTVManager
{
	friend class PKTVTransportTCP;
	friend class PKTVTransportUDP;

	enum PKTVMode
	{
		SINGLE_GAME = 0,
		SERVER_IDLE = 1,
		SERVER_PROXY = 2,
		CLIENT_IDLE = 3,
		CLIENT_CONNECTED = 4,
		CLIENT_PLAYING_FILE = 5,
	}
	mWorkMode;

	enum PKTVStages
	{
		CONN_IDLE = 0,
		CONN_INTENT = 1,
		CONN_SENTTOPROXY = 2,
		CONN_PINGING = 3,
		CONN_CONNECTING = 4,
	}
	mConnectionStages;

	enum ErrorTypes
	{
		ERR_WRONG_PASSWORD = 0,
		ERR_TOO_MANY_CLIENTS = 1,
		ERR_TRANSPORT_OVERFLOW = 2,
	};

	// Members

	// Tick
	DWORD mLastTickTime;

	// Connection data
	// -----------------------------------
	IpAddress2			mProxyAddr;			// the proxy we want to connect to
	String				mLocalPassword;		// if the proxy uses a password, give it to it
	String				mInterfaceToUse;	// which interface to use; may be ""
	unsigned short		mServerPort;		// our server listening port
	bool				mConnect;			// a desire to connect; autoconnect; don't sit idle; try to connect

	IpAddress2			mHostAddr;			// the host we're connected to
	
	// IP pinging
	// -----------------------------------
	DynamicArray<IpAddress>		mHostIPs;
	unsigned short				mCurrentBatchToPing;
	DynamicArray<IpAddress>		mHostAddresses;		// the hosts we want to connect to
	DWORD						mPinged;

	// Network subsystem
	// ----------------------------------
	bool				mNetworkStarted;	// did WSAStartup() go right?
	bool StartNetwork( void );
	void StopNetwork( void );

	// Frame making timers
	DWORD				mTimeSinceLastTick;
	DWORD				mFrameMakingTime;

	DWORD				mLastSentTime;

	unsigned short		mFps;
	DWORD				mDelayMs;

	PKTVTransportFile*	mPlayingFromFile;
	PKTVTransportFile*	mRecordingToFile;
	PKTVTransportUDP*	mTransport;

	DynamicArray<PKTVTransportUDP*> mClientTransports;
	void DisconnectAllClients( void );
	void HandleLowerSockets( void );

	// Transmitting to lower clients
	// -------------------------------------------
	void GiveFramesToLowerClients( DWORD time );
	bool mTransmitPaused;
	DWORD mTransmitTime;
	DWORD mTransmitBufferTime;

	// Frames recording/file buffering subsystem
	// ---------------------------------------------
	void MakeNewFrames( DWORD time );	// Used by server to make new frames from current gameplay
	void RemoveOldFrames( void );
	void RemoveAllFrames( void );
	PKTVUnpackedFrame*	mFirstFrame;
	PKTVUnpackedFrame*	mLastSentFrame;
	PKTVUnpackedFrame*	mLastFrame;

	DWORD				mMaxFrameBufferTime;	// max difference between first and last frames ( tells you when frames get deleted )
	DWORD				mMaxSizeTakenByFrames;	// max memory taken by frames before removing them

	// Proxy (UDP) Socket Subsystem
	// ---------------------------------------------
	PKTVProxyConnection* mProxy;
	void ResetProxy( void );

	// Reports to the proxy
	void SendProxyReport( void );
	void SendPingTimeoutReport( DynamicArray<IpAddress>& toReport );
	void SendLeavingReport( void );

	void SendReportOnFreeSlotsChange( void );
	unsigned short mFreeSlotsLastTime;

	unsigned short mIPsAlreadyTested;

	void SendTCPErrorMessage( PKTVTransport* transport, unsigned char errorType, String additionalInfo = "" );
	void HandleUDPPings( void );

	// Ticks
	// -------------------------------------------
	void SingleGameTick( DWORD time );
	void ServerIdleTick( DWORD time );
	void ServerProxyTick( DWORD time );
	void FilePlayTick( DWORD time );
	void ClientIdleTick( DWORD time );
	void ClientConnectedTick( DWORD time );

	// Display
	// -------------------------------------------
	void DisplayFrame( DWORD time );
	bool				mDisplayPaused;
	DWORD				mDisplayTime;
	PKTVUnpackedFrame*	mLastDisplayedFrame;

	// Headers
	// -------------------------------------------
	void WriteStreamHeader( UdpMessage* header );
	bool ReadAndExecuteStreamHeader( UdpMessage* header );

	bool mSendingFrames;
	bool mFullFrameNext;

	unsigned char mMaxClients;

	// Are we currently giving the frames to the proxy?
	void TestSendingSwitch( void );
	inline bool Sending( void ) { return mSendingFrames; }

	DWORD mCurrentProxyTime;
	String mLastAdminMessage;
	DWORD mProxyReportLastSent;

public:
	enum PKTVCommands
	{
		CMD_GET_CONNECTION_INFO = 0,
		CMD_PING = 1,
		CMD_ERROR = 2,
		CMD_CONNECTION_INFO = 3,
		CMD_PROXYUPDATE = 4,
		CMD_ADMINMESG = 5,
		CMD_PROXY_REPORT = 6,
		CMD_LEAVING_REPORT = 8,
		CMD_REDIRECT_TO_PROXY = 9,
		CMD_PONG = 10,
		CMD_PING_TIMEOUT_REPORT = 11,
		CMD_TRANSPORT_MESG = 12,
		CMD_TRANSPORT_REQUEST = 13,
	};

	SocketClosingModule SocketTrashcan;

	// Maintenance
	PKTVManager( bool server, bool multiGame, unsigned short fps, DWORD delayMs = 30000 );
	~PKTVManager( void );
	bool GetError( String& text );	// if there was some error, it is returned in the given string. Otherwise, return false.

	// If you're a client, you can start connection process with this. hostname should contain port after ':'
	void ClientConnect( const char* hostname, String password, String interfaceToUse );

	// Alternatively, you can also play file with this method
	void ClientPlayFile( const char* filename );

	// Use this to start recording a demo file
	void RecordStart( const char* filename );

	// Use this to stop recording a demo file
	void RecordStop( void );

	void Tick( void );
	bool IsServer( void ) const { return ( ( mWorkMode == SERVER_IDLE ) || ( mWorkMode == SERVER_PROXY ) ); }
	bool IsPlayingFromFile( void ) const { return ( mWorkMode == CLIENT_PLAYING_FILE ); }
	bool IsPlaying( void ) const { return ( mWorkMode == CLIENT_PLAYING_FILE || mWorkMode == CLIENT_CONNECTED ); }

	void SetPassword( String pass ) { mLocalPassword = pass; };

	// Master Server methods
	// ============================
	inline void SetServerPort( unsigned short port ) { mServerPort = port; };	// use this before server tick!

	// How long should the server wait before giving out the frame
	inline void SetDelay( DWORD delayMs ) { mDelayMs = delayMs; };
	inline DWORD GetDelay( void ) { return mDelayMs; };

	// How often should the server make the frame
	inline void SetFramerate( unsigned short fps ) { mFps = fps; };
	inline unsigned short GetFramerate( void ) { return mFps; };
	String GetProxyAddress( void );

	// Client methods
	// ============================
	// maximal number of clients that can connect to a single client
	void SetMaxClients( unsigned char clients ) { if( !IsServer() ) mMaxClients = clients; if ( mMaxClients < 1 ) mMaxClients = 1; };
	unsigned char GetMaxClients( void ) { return mMaxClients; };

	// Display
	// ============================
	inline void Pause( bool pause ) { mDisplayPaused = pause; };
	DWORD GetDisplayTime() const { return mDisplayTime; }
//	inline void JumpTo( DWORD time ) { mDisplayTime = time; }
//	inline void JumpBy( DWORD time, bool forward ) { if( forward ) mDisplayTime += time; else mDisplayTime -= time; }

	// Message sending and receiving
	// ============================
	void AddMessage( String message );			// Give chat message to everyone else if possible
	void HandleMessage( String message );		// This is called when the chat message is given to us

	// Overflow management
	// ============================
	unsigned char ServerOverflows;	// how many times we disconnected a client on overflow
	unsigned char ClientOverflows;	// how many times we were disconnected from server on overflow

	// Map loading fixes
	// ============================
	bool JustLoadedMap;				// if this is set, it means that the PKTV just
									// experienced a problem with display due to map loading and needs to fix timers
	bool MapLoading;				// this is used to show that the tick has been called from inside
									// map loading routine
	String CurrentMap;

	bool IsConnecting() const { return( mConnectionStages != CONN_IDLE ); }

	// String Dictionary
	// ============================
	PKTVStringDictionary Dict;

	// Memory Allocation
	// ============================
	ulong32 MemoryTaken( bool report = false );
/*
	// For stream optimization only; not used in-game
	ValuesMonitor<String>			T1Monitor;
	ValuesMonitor<float>			T2Monitor;
	ValuesMonitor<ulong32>	T3Monitor;
	ValuesMonitor<float>			T4Monitor;
	ValuesMonitor<bool>				T5Monitor;
	ValuesMonitor<Matrix>			T6Monitor;
	ValuesMonitor<long32>				T7Monitor;
	ValuesMonitor<Vector>			T8Monitor;
	ValuesMonitor<ulong32>	T9Monitor;
	ValuesMonitor<float>			TAMonitor;
	ValuesMonitor<float>			TBMonitor;
	ValuesMonitor<Vector>			TCMonitor;
	ValuesMonitor<Vector>			TDMonitor;
	ValuesMonitor<float>			TEMonitor;
	ValuesMonitor<float>			TFMonitor;
*/
#ifdef ENTITY_CONTENTS_FLAG_MONITORING
	DynamicArray<ulong32>		BitTesting;
#endif

	void ProcessTransportRequest( IpAddress addr );
};

#endif	// __PKTVMANAGER_H__
