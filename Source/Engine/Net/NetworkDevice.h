#error This file is obselete

// #pragma once
#ifndef _NETWORKDEVICE_
#define _NETWORKDEVICE_


#include "Core/HashMap.h"
#include "Net/NetworkProfiler.h"
#include "Net/ObjectSynchronizer.h"
#include "Net/IpDriver.h"
#include "Net/DemoRecording.h"
#include "Physics/PhysicsObject.h"


#define SERVER_ID 255

class PlayerMovementData
{
public:
	PlayerMovementData():PhysFrame(0),Action(0),Yaw(0.0f),Pitch(0.0f),VariablesIn(false),Latency(0) {};
	unsigned long PhysFrame;
	DWORD Latency;
	DWORD Action;
	float Pitch;
	float Yaw;
	bool VariablesIn;
	PhysicsObjectPlayerVariables Variables;
};

class DelayedScriptMessage
{
public:
	UdpMessage* Message;
	bool Reliable;
	bool OneClientOnly;
	unsigned char SpecialClient;
	int DeliveryReceipt;
};

// ****************** GameSpy Data *********************************

// Additional GameSpy keys
#define GAMESPY_MAXSPECTATORS_KEY 100
#define GAMESPY_PASSWORDED_KEY 101

#define MAX_PLAYERS 16

class GameSpyPlayerData
{
public:
	bool			Valid;
	String			Name;			// a player name (may include spaces)
	int				Score;			// contains the score (kills/points) for a single player
	unsigned short	Ping;			// the ping for a player (as measured between the player and the server)
	int				Team;			// the team a player is on, either numeric or string
	int				Deaths;			// number of deaths a player has had

	GameSpyPlayerData( void ) : Valid(false), Score(0), Ping(999), Team(0), Deaths(0) {};
	~GameSpyPlayerData( void ) {};
};

class GameSpyTeamData
{
public:
	bool			Valid;
	String			Name;
	int				Score;

	GameSpyTeamData( void ) : Valid(false), Name("Undefined"), Score(0) {};
	~GameSpyTeamData( void ) {};
};

class GameSpyData
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
	String	GameVersion;			// Game Version

	int		MaxPlayers;				// max number of players for this server
	int		MaxSpectators;			// max number of spectators for this server

	int		Teamplay;				// number which defines the type of teamplay in use, or 0 for no teamplay. Values > 0 are up to the developer.

	int		FragLimit;				// number of total kills or points before a level change or game restart
	int		TeamFragLimit;			// number of total kills or points for a team before a level change or game restart

	int		TimeLimit;				// amount of total time before a level change or game restart occurs (generally in minutes)
	int		TimeElapsed;			// amount of time (in seconds) since the current level or game started (zamienic na timeremaining, jesli GameSpy to potrafi wyswietlic.)

	String	Password;				// server password

    GameSpyPlayerData	Players[MAX_PLAYERS];	// Player data
	GameSpyTeamData		Teams[MAX_PLAYERS];		// Team data

	GameSpyData( void ) : MapName("None"), GameType("Default"), GameVariant(1), MaxPlayers(MAX_PLAYERS), MaxSpectators(1),
		Teamplay(0), FragLimit(0), TeamFragLimit(0), TimeLimit(0), TimeElapsed(0), Password(""),
		ServerName("Painkiller MP"), Mode("closedwaiting"), GameVersion("0.0"), internalMode(0) {};
	~GameSpyData( void ) {};

	void SetMode( int mode )
	{
		internalMode = mode;
		CheckGameMode();
	}

	void CheckGameMode( void );

	// Information callbacks - has to be public - else I would have to declare GameSpy types here
	bool GetServerValue( int keyid, int& valueInt, String& valueString );
	bool GetPlayerValue( int keyid, int index, int& valueInt, String& valueString );
	bool GetTeamValue( int keyid, int index, int& valueInt, String& valueString );
	int GetPlayerCount( void );
	int GetTeamCount( void );

protected:
	String Mode;
	int internalMode;
};

class PtrsToDelete
{
public:
	void* ptr;
	unsigned int num;

	bool operator==( const PtrsToDelete& other ) const { return ( ( ptr == other.ptr ) && ( num == other.num ) ); }
};

// ***************************** Main class ******************************

class ENGINE_API NetworkDevice
{
	friend class NetworkClient;
	friend class ObjectSynchronizer;
	friend class POSynchronizer;
	friend class IPDriver;
	friend class GameSpyData;
	friend class DemoRecording;
public:

	// Methods
	NetworkDevice();
	~NetworkDevice();

	// Maintenance
	void PreTick( float timeDelta );						// Called before world processing to get all the input from clients
	void PostTick( void );									// Called after world processing to send the newest information to them
	void ServerPhysicsTick( float timeDelta );				// Called only on the server to calculate sub-ticks exactly so all player movement will be working
	void ClientPhysicsTick( float timeDelta );				// Called only on the client
	void UpdateToLastLocalAction( void );					// Called only on client after new sever data came

	// Start & Stop
	bool NetworkServer( String mapName, String password, String interfaceToUse, unsigned short port = DEFAULT_SERVER_PORT );	// start listening server
	bool NetworkClientAndConnect( const char* hostname, String password, unsigned short port = 0, bool spectator = false );		// start client trying to connect to hostname
	bool NetworkClientAndConnect( void* hostFromGameSpy, String password, bool spectator = false );								// start client trying to connect to GameSpy SBServer
	bool PlayRecording( const char* filename, String* errorText );
	void NetworkClose( void );								// close network interfaces, sockets, disconnect clients etc.

	void LoadNewMap(String levelName);

	// Script messages interface
	void SendScriptMessage( DelayedScriptMessage& dsm ) { mDelayedScriptMessageList.Add( dsm ); };

	// Shortcuts
	bool IsServer( void ) { return mLowLayer.State == IPDriver::SERVER; };					// Tells if we are a server or a client now
	bool IsClient( void ) { return mLowLayer.State == IPDriver::CLIENT; };					// Tells if we are a server or a client now
	String GetMyIP( int* port ) { return mLowLayer.GetMyIP( port ); };
	DWORD GetStartTime( void );

	// Client Data
	unsigned char GetClientID( void );
	double Ping( unsigned char clientID );
	void PingReset( unsigned char clientID );
	void SendEmptyPacket( unsigned char clientID );
	float PacketLoss( unsigned char clientID );
	String Stats( unsigned char clientID, bool temporary );
	bool IsSpectator( unsigned char clientID );
	void SetSpectator( unsigned char clientID, bool spectator );
	bool CanConnectNewClient( bool spectator );
	void DisconnectClient( unsigned char clientID );
	void BanClient( unsigned char clientID );
	void InputPlayerMovementAction( unsigned char clientID, PlayerMovementData* data );
	void SetPushLatency( long push );
	long GetPushLatency( void );

	// Transfer stats
	void SetStatsUpdateDelay( DWORD delay );
	void GetLastUpdateStats( unsigned long& sent, unsigned long& received );

	// Synchronization
	bool GetUpdateClassData( unsigned char updateClass, float& minSynchroDelay, float& minLinearVelocityDiff, float& minAngularVelocityDiff, float& minPositionDiff );
	void SetUpdateClassData( unsigned char updateClass, float minSynchroDelay, float minLinearVelocityDiff = 0.01f, float minAngularVelocityDiff = 0.1f, float minPositionDiff = 0.15f );
	void UpdateUpdateClasses( void );
	unsigned short GetMaxPacketSize( void );
	void UpdateMaxPacketSize( unsigned short maxPacketSize, bool sendNow = true );
	void SetSynchroState( Entity* entity, unsigned char state, unsigned char updateClass, unsigned short specialClient = SERVER_ID, unsigned char specialClass = 0 );
	void OnActivateAnimation( Model* model, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve );
	void OnLoadAnimation( Model* model, String name, bool looped, int animIndex );
	void OnUpdatePOAction( Entity* entity, int actionType );

	// Entity number transfer
	unsigned long GetEntityNumber( Entity* entity );
	Entity* GetEntityFromNumber( unsigned long number );

	inline void SetCDKeyAndIsPublic( String cdkey, bool isPublic ) { mLowLayer.SetCDKeyAndIsPublic( cdkey, isPublic ); };

	// Network profiler, used for information gathering from all data-sending modules
	NetworkProfiler Profiler;

	unsigned long SureGetPtrIndex( void* ptr, unsigned int gindex );
	inline void DelayedDeletePtrIndex( void* ptr, unsigned int gindex );

	// This stores last time sent with script message
	DWORD LastReadCurrentTime;

	// GameSpy variables
	GameSpyData	GameSpyInfo;
	bool couldConnectLastTick;

	// Demo Recording module
	DemoRecording Recording;
	void ShiftStartTime( DWORD shift, bool plus = true );

	PlayerMovementData mLastLocalAction;
	void SendPlayerActionToServer( PlayerMovementData* action );
	void UpdateFramerateLock( void );
	double mServerFramerateLock;

	// Latency simulator
	void SetSimulatedLatency( DWORD latency );
	DWORD GetSimulatedLatency( void );

protected:
	// Methods
	NetworkClient* MakeNewNetworkClient();
	void RemoveNetworkClient( NetworkClient* client );	// to be used only by NetworkClient destructor
	void GameSpyStateChanged( void ) { mLowLayer.GameSpyStateChanged(); };
	Entity* GetPlayerByIndex( int index );

	// Members
	IPDriver mLowLayer;
	DynamicArray<NetworkClient*> mNetworkClients;
	unsigned short mClientIndices;
	bool mSpectator;
	float mEVA;		// experimental value add feature

	void DeleteUniqueIndices( void );

	UniqueNumberReuser mClientNumberManager;

	UniqueNumberReuser mNumberManager;
	HashMapSpecial<void*,unsigned int,unsigned long,256>	mObjectSynchronizersHashByEntity;
	HashMapSpecial<void*,unsigned int,unsigned char,256>	mUpdateClassHashByEntity;

	DynamicArray<PtrsToDelete>								mPtrsToDelete;

	// Connection params
	int mHowManyUnreliableAssumedToPass;	// after how many messages we assume that one of them had to achieve the other side?

	DynamicArray<DelayedScriptMessage> mDelayedScriptMessageList;

	// Prediction data
	bool mPlayerPredictionOn;
	bool LoadedInPreTick;
	int UpdatesSinceLoaded;
	int mFramerateLock;
};


#enidf //_NETWORKDEVICE_
