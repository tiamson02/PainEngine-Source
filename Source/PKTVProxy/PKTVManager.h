#ifndef __PKTVMANAGER_H__
#define __PKTVMANAGER_H__

#include "PKTVEntityProxies.h"
#include "PKTVTransport.h"
#include "PKTVStringDict.h"
#include "DynArray.h"
#include "IpAddress.h"
#include "PKTVProxyConnection.h"
#include "SocketClosingModule.h"

#define HOST_PING_TIMEOUT 10000	// ms
#define HOST_IPS_TO_PING 10

#define DEFAULT_LISTENING_UDP_PORT 3461

// The packed frame can be stored in the UdpMessage.
class PKTVTransport;
class PKTVTransportFile;
class PKTVTransportUDP;
class PKTVDatabase;
class PKTVUnpackedFrame;

class IpAddress2
{
	IpAddress	mAddress;
	String		mName;
public:
	IpAddress2() : mName("") {};
	bool SetName( String name, unsigned short port = 0 );
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
	friend class PKTVTransportUDP;
public:
	enum PKTVMode
	{
		PROXY_IDLE = 0,
		PROXY_CONNECTED = 1,
		PROXY_PLAYING_FILE = 2,
	};

	enum ErrorTypes
	{
		ERR_WRONG_PASSWORD = 0,
		ERR_TOO_MANY_CLIENTS = 1,
		ERR_TRANSPORT_OVERFLOW = 2,
	};

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

private:
	PKTVMode mWorkMode;

	enum PKTVStages
	{
		CONN_IDLE = 0,
		CONN_INTENT = 1,
		CONN_AUTHORIZING = 2,
	}
	mConnectionStages;

	// Members

	// Slots Database
	// -----------------------------------

	// Connection data
	// -----------------------------------
	IpAddress2			mSourceAddr;		// the source we want to connect to
	String				mSourcePassword;	// if the source uses a password, give it to it
	String				mInterfaceToUse;	// which interface to use; may be ""
	unsigned short		mUDPPort;			// our proxy UDP listening port
	bool				mConnect;			// a desire to connect; autoconnect; don't sit idle; try to connect
	
	// Transmitting to lower clients
	// -------------------------------------------
	void GiveFramesToLowerClients( DWORD time );
	bool mTransmitPaused;
	DWORD mTransmitTime;
	DWORD mTransmitBufferTime;

	// Network subsystem
	// ----------------------------------
	bool				mNetworkStarted;	// did WSAStartup() go right?
	bool StartNetwork( void );
	void StopNetwork( void );

	String				mErrorString;		// if there was some error, its stored here

	// Frame making timers
	DWORD				mTimeSinceLastTick;
	DWORD				mFrameMakingTime;

	DWORD				mLastSentTime;

	unsigned short		mFps;
	DWORD				mDelayMs;

	PKTVTransportFile*	mPlayingFromFile;
	PKTVTransportFile*	mRecordingToFile;
	PKTVTransportUDP*	mTransport;

	void PeriodicallyTransferAdminMessage( String message );
	DWORD				mLastSentAdminMessage;

	DynamicArray<PKTVTransportUDP*> mClientTransports;
	DynamicArray<PKTVTransportUDP*> mProxyTransports;

	void DisconnectAllClientsAndProxies( void );
	void HandleLowerSockets( void );

	// Proxy management database
	PKTVDatabase* mDatabase;

	// Frames recording/file buffering subsystem
	// ---------------------------------------------
	void RemoveOldFrames( void );
	void RemoveAllFrames( void );

	PKTVUnpackedFrame*	mFirstFrame;
	PKTVUnpackedFrame*	mLastFrame;

	DWORD				mMaxFrameBufferTime;	// max difference between first and last frames ( tells you when frames get deleted )
	DWORD				mMaxSizeTakenByFrames;	// max memory taken by frames before removing them

	// Proxy (UDP) Socket Subsystem
	// ---------------------------------------------
	PKTVProxyConnection* mUDP;
	void ProxyHandleUDPRequests( void );
	void DeleteUDPSocket( void );

	// Sending messages
	void SendErrorMessage( const IpAddress& addr, unsigned char errorType, String additionalInfo = "" );
	void SendProxyUpdateInfo( void );
	void PeriodicallySendProxyUpdate( void );
	DWORD				mLastSentProxyUpdate;

	// Ticks
	// -------------------------------------------
	void FilePlayTick( DWORD time );
	void ProxyIdleTick( DWORD time );
	void ProxyConnectedTick( DWORD time );

	// Headers
	// -------------------------------------------
	void WriteStreamHeader( UdpMessage* header );
	void ReadAndExecuteStreamHeader( UdpMessage* header );

	bool mFullFrameNext;

	unsigned short mMaxClients;
	unsigned short mLastTickFreePlaces;

	DWORD mLastTickTime;

	// used only if connected to another proxy
	DWORD mCurrentProxyTime;
	String mLastAdminMessage;

	DWORD mLastTimeRefreshedOurData;

	// Frame renumbering
	DWORD mFrameRenumber;
	DWORD mTimeRenumber;
	DWORD mLastFrameReadTime;

public:
	// Maintenance
	PKTVManager( DWORD delayMs, unsigned short udpPort, unsigned short maxClients, String interfaceToUse = "" );
	~PKTVManager( void );
	bool GetError( String& text );	// if there was some error, it is returned in the given string. Otherwise, return false.

	SocketClosingModule SocketTrashcan;

	// If you're a client, you can start connection process with this. hostname should contain port after ':'
	void Connect( const char* hostname, String password );

	// Alternatively, you can also play file with this method
	void PlayFile( const char* filename );

	// Use this to start recording a demo file
	void RecordStart( const char* filename );

	// Use this to stop recording a demo file
	void RecordStop( void );

	void Tick( void );
	inline bool IsPlayingFromFile( void ) { return ( mWorkMode == PROXY_PLAYING_FILE ); };

	// How long32 should the proxy wait before giving out the frames
	inline void SetDelay( DWORD delayMs ) { mDelayMs = delayMs; };
	inline DWORD GetDelay( void ) { return mDelayMs; };

	// Message sending and receiving
	// ============================
	void AddMessage( String message );			// Give chat message to everyone else if possible
	void HandleMessage( String message );		// This is called when the chat message is given to us

	// Other
	void SetInterface( String interf ) { mInterfaceToUse = interf; };
	String GetInterface( void ) { return mInterfaceToUse; };

	void SetUDPPort( unsigned short port );

	unsigned short GetMaxClients( void ) { return mMaxClients; };
	void SetMaxClients( unsigned short clients ) { mMaxClients = clients; };

	unsigned short GetFreeSlots( void ) { return mLastTickFreePlaces; };

	void SetBufferTime( DWORD time ) { mTransmitBufferTime = time; };
	DWORD GetBufferTime( void ) { return mTransmitBufferTime; };

	// Stats
	ulong32 GetNumberOfClients( void ) { return mClientTransports.Size(); };
	ulong32 GetNumberOfProxies( void ) { return mProxyTransports.Size(); };
	bool HasSource( void ) { return ( mTransport != 0 ); };
	PKTVMode GetWorkMode( void ) { return mWorkMode; };

	DlUl GetOverallUDPStats( void );
	DlUl GetLatestUDPStats( void );

	DlUl GetOverallClientTCPStats( void );
	DlUl GetLatestClientTCPStats( void );

	DlUl GetOverallProxyTCPStats( void );
	DlUl GetLatestProxyTCPStats( void );

	DlUl GetOverallSourceTCPStats( void );
	DlUl GetLatestSourceTCPStats( void );

	void ListDB( void );	// for debugging

	// Overflow management
	unsigned char ServerOverflows;	// how many times we disconnected a client on overflow
	unsigned char ClientOverflows;	// how many times we were disconnected from server on overflow

	PKTVStringDictionary Dict;

	ulong32 MemoryTaken( bool report = false );

	void ProcessTransportRequest( IpAddress address );
};

#endif	// __PKTVMANAGER_H__
