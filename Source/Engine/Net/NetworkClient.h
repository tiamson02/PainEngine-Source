#error This file is obselete

//#define LOG_PROTOCOL_DEBUG_MESSAGES

#include "Core/HashMap.h"
#include "Core/Median.h"
#include "Net/StringHashSystem.h"
#include "Net/ObjectSynchronizer.h"
#include "Net/DemoRecording.h"
#include "Net/IPChannel.h"
#include "Net/NetworkDevice.h"

#define LATENCY_RESULTS_SET_SIZE 7

class UdpMessage;
class Entity;

struct UpdateClassParams
{
	DWORD MinSynchroDelay;
	DWORD MinSynchroDelayLow;
	DWORD MinSynchroDelayHigh;

	float MinLinearVelocityDiff;
	float MinAngularVelocityDiff;
	float MinPositionDiff;
};

class ENGINE_API NetworkClient
{
	friend class IPDriver;
	friend class IPChannel;
	friend class NetworkDevice;
	friend class ObjectSynchronizer;
	friend class MeshSynchronizer;
	friend class ModelSynchronizer;
	friend class POSynchronizer;
//	friend class RagdollSynchronizer;
	friend class DemoRecording;
public:

	enum NetworkClientStates
	{
		EStateDisconnected = 0,
		EStateConnecting = 1,
		EStateConnected = 2,
		EStateSynchronizing = 3,
		EStatePlay = 4,
	};

	enum CommandTypes
	{
		SCRIPTMESG = 0,
		UPDATE = 1,
		CREATEOBJ = 2,
		DELETEOBJ = 3,
		OTHEROBJ = 4,
		SCRIPTMESGREL = 5,
	};

public:
	NetworkClient( NetworkDevice* device );
	~NetworkClient();

	// Map change
	void ResetToConnecting( void );
	void SendLoadMap(String levelName);
	void SendMapLoaded( bool ok );

	inline unsigned char GetClientIndex( void ) { return mMyIndex; };

	// Message read/write
	bool ReadMaintenanceMessage( bool server, UdpMessage* message );

	// Main synchronization method
	void CreateAllSynchronizableEntitiesOnClient( void );

	// Main state message composition method
	void MaintainObjectsOnClient( void );
	void SendUpdateMessage( void );
	void SendClientScriptReliableMessages( void );
	void SendClientScriptUnreliableMessages( void );
	void SendServerScriptUnreliableMessages( void );

	// Main binary messages resolution method
	bool ReadBinaryMessage( UdpMessage* message );

	inline double Ping( void );
	inline float PacketLoss( void );
	inline String Stats( bool temporary );
	DWORD GetState( void ) { return State; };
	void BanMe( void );
	inline SetPushLatency( long push ) { mPushLatency = -abs(push); };
	inline long GetPushLatency( void ) { return -mPushLatency; };

	StringHashSystem StringTransport;

	inline void SendEmptyPacket( void );
	inline void ReportUnsentReliables( void );

	void SetCdKeySDKIndexAndHash( int index, String hash ) { mGameSpyCDKeyIndex = index; mGameSpyHash = hash; };

	// Temporary - average, max, min update packet size measurement
	unsigned long mSummaryUpdatePacketSize;
	int mSummaryUpdatePacketCount;
	int mMaxUpdatePacketSize;
	int mMinUpdatePacketSize;

	inline bool NotReceivingPacketsLately( void );

	// New experimental player prediction
	DynamicArray<PlayerMovementData*> PlayerActions;
	DWORD SaveTime;
	DWORD LastSaveSent;

	int AddPlayerAction( PlayerMovementData* data );
	void RemoveOlderPlayerActions( long serverFrameTime );	// leave one, for interpolation
	void SendPlayerAction( PlayerMovementData* data );
	unsigned long mFrameDiff;
	bool mFrameDiffPlus;
	bool mFrameDiffInit;
	short mLastPlayerActionLatency;
	long mLastLatencyResult;

private:
	DWORD			State;
	IPChannel*		mNetChannel;
	NetworkDevice*	mMyDevice;
	DemoRecording*	mRecording;
	HashMap<unsigned char,UpdateClassParams> ConnectionParams;

	unsigned char	mMyIndex;
	int				mGameSpyCDKeyIndex;
	String			mGameSpyHash;
	int				mHowManyUnreliableAssumedToPass;
	bool			mSpectator;

	DynamicArray<ObjectSynchronizer*>					mObjectSynchronizers;
	DynamicArray<ObjectSynchronizer*>					mObjectSynchronizersToCreate;
	DynamicArray<ObjectSynchronizer*>					mObjectSynchronizersToDelete;
	HashMap<unsigned long,ObjectSynchronizer*,1024>		mObjectSynchronizersHash;

	PONetCreationCache POCreationCache;

	// Propagated from NetworkDevice
	void SetSynchroState( unsigned long uniqueIndex, Entity* entity, unsigned char state, unsigned char updateClass );
	void OnActivateAnimation( unsigned long uniqueIndex, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve );
	void OnLoadAnimation( unsigned long uniqueIndex, String name, bool looped, int animIndex );
	void OnUpdatePOAction( unsigned long uniqueIndex, int actionType );

	inline ObjectSynchronizer* SureGetEntitySynchronizer( unsigned long uniqueEntityIndex, unsigned char entityType );
	ObjectSynchronizer* CreateObjectForEntity( unsigned long uniqueIndex, Entity* entity, unsigned char updateClass );

	// Object Synchronizers
	void MakeObjectSynchronizersForEntities( void );
	void RemoveZombieObjectSynchronizers( void );
	void RemoveClientZombieObjectSynchronizers( void );
	void RemoveObjectSynchronizers( void );

	// Shortcuts
	void UpdateUpdateClasses( void );
	DWORD mLastClassesUpdate;
	unsigned short GetMaxPacketSize( void );
	void UpdateMaxPacketSize( unsigned short mps, bool sendNow = true );
	DWORD mLastMaxPacketUpdate;
	inline void PackConnectionParamsIntoMessage( UdpMessage* message );
	inline void GetConnectionParamsFromMessage( UdpMessage* message, bool update = true );

	void AddLatencyResult( long result );

	// Object Synchronizers find out how late is the info from the net this way
	DWORD PacketLatency( void ) { return mNetChannel->PacketLatency(); };

	// HBC - HandleBinaryCommand
	// ReadBinaryMessage sub-methods - each of them handles one type of message
	inline bool HBC_EntityMessage( UdpMessage* message, unsigned char command );
	bool HBC_ScriptMessageBatch( UdpMessage* message );

	// Latency tests
	long mAverageLatency;
	long mPushLatency;
	Median<long,LATENCY_RESULTS_SET_SIZE> LatencyMedian;
	unsigned short mLatencyUpdateCounter;
	bool mLatencyFirstTime;
};
