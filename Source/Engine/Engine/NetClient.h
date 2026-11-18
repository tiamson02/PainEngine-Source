#error boo
//#define LOG_PROTOCOL_DEBUG_MESSAGES

class EngineGame;

class ENGINE_API NetClient
{
public:
	enum ConnectionType
	{
		ConnModem56K = 0,
		ConnISDN112K = 1,
		ConnADSL256K = 2,
		ConnLANT3_1M = 3,
	};

	enum NetClientStates
	{
		EStateDisconnected = 0,
		EStateConnecting = 1,
		EStateFileListGetting = 2,
		EStateFileSynchronizing = 3,
		EStateEntitySynchronizing = 4,	// first state in which we send state messages - states below also send them
		EStateMapLoading = 5,
		EStateWaitingForSpawn = 6,
		EStatePlaying = 7,
	};

	enum NetClientCommands
	{
		CLICOM_FULLENTITYSTATE = 0,
		CLICOM_ENTITYUPDATE = 1,
		CLICOM_ENTITYDELETE = 2,
		CLICOM_ACTION = 3,
//		CLICOM_STATSUPDATE = 4,
		CLICOM_POCREATE = 5,
		CLICOM_PODELETE = 6,
		CLICOM_ANIMACTIVATE = 7,
		CLICOM_ANIMLOAD = 8,
		CLICOM_SCRIPTMESG = 9,
		CLICOM_FULLENTITYSTATEONCE = 10,
		// up to 15 available
	};

public:
	NetClient( EngineGame* game );
	~NetClient();

	void SetConnectionType( int connectionType );

	void SetChannel( IPChannel* channel ) { mNetChannel = channel; };
	void HandleDisconnect( void );			// Information from the channel that it's been disconnected ( from either side )
	bool HandleClientConnected( void );		// Answer true to accept, false to refuse client connection. You're server.
	void HandleConnectedToServer( void );	// You're connected. Prepare for the next stage. Usually this happens right after object creation.
	bool ReadMessage( UdpMessage* message );	// Interpret the message. More than one message can come in one tick.
												// Return true if you understand it, false if you don't.

	void HandleFileCompleted( String filename );	// The file download has completed - handle it.
	void HandleFileNotFound( String filename );		// The file you've been trying to download was not found - handle it.
	void HandleRequestedFilelist( void );			// Client asked us about filelist - give it out
	void HandleFilelist( DynamicArray<String>& filenames );		// Server gave us filelist - remember it and start getting files
	void HandleStartStreamingFromClient( void );	// Client said he wants us to start streaming
	void HandleStartPlaying( void );			// Server said we already got all his entities - can spawn now
	void HandleSpawnMe( DynamicArray<String>& tokens );
	void HandleEntityIndexSetting( unsigned char index );
	void HandleLoadMap( DynamicArray<String>& tokens );
	void HandleMapLoadReport( bool ok );

	bool ReadTextMessage( bool server, DynamicArray<String> & tokens );

	void SendStateMessage( int howMany, UdpMessage* data );
	void SendFullEntityState( int netIndex );
	void SendFullEntityStateOnce( Entity* entity );
	void SendPOCreate( int netIndex, DWORD poType, bool player, float scale );
	void SendDeleteEntityInfo( int netIndex );
	void SendDeletePOInfo( int netIndex );
	void SendPlayerEntityIndex( unsigned short index );
	void SendNameOfMapToLoad( String mapName, float scale, String physicsName );
	void SendActionPacket( DWORD action, Vector camRotation );
	void SendActivateAnimationInfo( int netIndex, int animNumber, bool looped, float blendTime, float timeScale );
	void SendLoadAnimationInfo( int netIndex, String animName, bool looped, int animIndex );
	void SendScriptMessage( UdpMessage* message, bool reliable );

	void MakeSureMyPlayerPtrDel( Entity* entity );

	// Player stats
	String PlayerName;
	unsigned long Score;
	unsigned long Frags;
	unsigned long Deaths;
	double Ping( void ) { return mNetChannel?mNetChannel->GetPing():9.999f; };

	// Connection parameters - they influence what and how often server sends (and receives)
	int ConnectionType;
	unsigned long MaxMessageSize;
	float MaxSynchroDistance;
	float MinTimeBetweenPackets;

	Entity* GetPlayer( void ) { return mMyPlayer; };
	Vector GetPlayerHeadPosition( void ) { return mMyPlayer->PO?mMyPlayer->PO->GetPawnHeadPos():mMyPlayer->GetPosition(); };
	Vector GetCameraRotation( void ) { return mCameraRotation; };
	DWORD GetState( void ) { return State; };

	void ZeroChannel( void ) { mNetChannel = 0; };	// Access to protected member; done this way to avoid setting NetChannel as friend class

	void SetCdKeySDKIndex( int index ) { mGameSpyCDSDKIndex = index; };	// Special for GameSpy CD SDK

private:
	float mLastTimePacketSent;
	DWORD			State;
	IPChannel*		mNetChannel;
	Entity*			mMyPlayer;
	EngineGame*		mMyGame;
	Vector			mCameraRotation;
	DWORD			mLastAction;
	DWORD			mActionBeforeLast;
	int				mGameSpyCDSDKIndex;
	unsigned long	mLastReceivedScriptMesg;
	unsigned long	mLastSentScriptMesg;

	// Statistics
	unsigned long mScriptMessagesSentUnreliable;
	unsigned long mScriptMessagesSentReliable;
	unsigned long mScriptMessagesReceived;
	unsigned long mScriptMessagesSentBytesUnreliable;
	unsigned long mScriptMessagesSentBytesReliable;
	unsigned long mScriptMessagesReceivedBytes;

	// File download
	DynamicArray<String> mFileNames;
	DynamicArray<unsigned long> mFileSizes;
};
