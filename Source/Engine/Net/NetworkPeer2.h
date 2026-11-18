
// #pragma once
#ifndef _NETWORKPEER2_
#define _NETWORKPEER2_


#define ICON_SHOW_TIMEOUT 1000

#include "Core/LeString.h"
#include "Core/Median.h"
#include "Net/UdpMessage.h"
#include "Net/NetworkDevice2.h"
#include "Net/ObjectSynchronizer2.h"
#include "Net/StringHashSystem.h"

#define PING_AVERAGE_STEPS 10

struct DelayedScriptMessagePeerData
{
	DelayedScriptMessage2* DSM;
	bool IsReliable;
	ulong32 Number;
};

class NetworkPeer2
{
	friend class NetworkDevice2;
	friend class EntitySynchronizer2;
	friend class ModelSynchronizer2;
	friend class MeshSynchronizer2;
public:

	NetworkPeer2( NetworkDevice2* device, IpAddress* address );
	~NetworkPeer2( void );

	inline unsigned char GetClientIndex( void ) { return mMyIndex; };
	bool NotReceivingPacketsLately( void ) { return ( GetCurrentTime() - mLastReceivedTime  > ICON_SHOW_TIMEOUT ); };
	inline DWORD Ping( void ) { return mPingMedian.GetAverage( PING_AVERAGE_STEPS ); }
	inline float PacketLoss( void )
	{
		return mStatsDataTemp.mPacketsShouldBeReceived?
			(((float)mStatsDataTemp.mPacketsLost)/mStatsDataTemp.mPacketsShouldBeReceived):
		0.f;
	}

	void Disconnect( void );

	ulong32 LastFrameLatency;

private:
	int mLocalPlayerOptimizedEveryNFrames;
	int mMaxPlayerActionsPassed;
	int mMinSendFrameSize;

	NetworkDevice2* mNet;

	// High-level methods
	void ResetToConnecting( void );
	void SendLoadMap( const char *levelName );
	void SendMapLoaded( bool ok );
	void TickStats( DWORD time );

	// Variable transfer ( using reliable messages )
	void TransferVariable( String& name, String& value );
	void InterpretVariable( String& name, String& value );

	// High-level variables

	enum NetworkClientStates
	{
		EStateDisconnected = 0,
		EStateConnecting = 1,
		EStateConnected = 2,
		EStateSynchronizing = 3,
		EStatePlay = 4,
	} mConnectionState;

	bool			mSpectator;
	unsigned char	mMyIndex;
	int				mGameSpyCDKeyIndex;
	String			mGameSpyHash;
	DWORD			mLastFrameTime;
	DWORD			mServerFPS;
	DWORD			mPing;
	Median<DWORD,PING_AVERAGE_STEPS> mPingMedian;

	StringHashSystem2	StringTransport;
	PONetCreationCache2	POCreationCache;

	NetworkFrame* mFrames;
	ulong32 mMaxFrameNumber;
	void DeleteFrames( void );

	NetworkFrame* GetLastAnsweredFrame( void )
	{
		NetworkFrame* lastAnswered = 0;
		NetworkFrame* f = mFrames;
		while( f )
		{
			if( f->Answer )
				lastAnswered = f;
			f = f->Next;
		}
		return lastAnswered;
	}

	NetworkFrame* GetNewestFrame( void )
	{
		NetworkFrame* t = mFrames;
		if( t )
			while( t->Next )
				t = t->Next;
		return t;
	}

	NetworkFrame* GetFrameByNumber( ulong32 frameNumber )
	{
		NetworkFrame* t = mFrames;
		while( t )
		{
			if( t->Number == frameNumber )
				return t;
			t = t->Next;
		}
		return 0;
	}

	ulong32 mHighestReliableReceivedOnClient;
	ulong32 mReliableMessagesMaxNumber;
	DynamicArray<DelayedScriptMessagePeerData> mMessagesToSend;

	void ClearMessages( bool unreliableOnly );

	// To be used only by friend class of myself
	void Tick( void );
	bool Send( UdpMessage* unreliable );
	void SendReliable( UdpMessage* reliable ) {	mReliableMessages.Add( reliable ); };	// Actually, will send it as soon as it can
	bool SendMaintenanceMessage( bool reliable, UdpMessage* message );
	bool PrepareMessageForReading( UdpMessage* incoming );
	void HandlePeerPackage( UdpMessage* message );
	bool HandleSingleMessage( UdpMessage* message );
	bool HandleFrameOnServer( UdpMessage* message );
	bool HandleFrameOnClient( UdpMessage* message );
	bool HandleFirstFrameOnClient( UdpMessage* message );

	bool SendFrame( DWORD time );
	void PrepareForFirstFrame( void );
	void SendFirstFrame( void );
	void SendClientFrame( void );

	inline bool CanSendReliableMessage( void )
	{
		return ( mReliableMessages.Size() &&
			( mAck >= mLastReliableMessageToAck ) &&
			( !mReliableMessages[0]->mWasSent || ( ( mAck > mLastReliable ) && ( mReliableAck != mReliableSent ) ) ) );
	}
	void ClearReliableMessages( void );

	DWORD mLastSendTime;
	DWORD mLastReceivedTime;
	DWORD mStreamingTimeout;

	IpAddress mAddress;
	bool mDisconnected;

	// Reliable messages
	DynamicArray<UdpMessage*> mReliableMessages;

	// Streaming variables
	ulong32 mSent;			// packet number counter - each sent packet has a number higher by one, thanks to this.
	ulong32 mIncoming;		// remembers highest received packet number to discard old packets (those with numbers less than highest received )
	ulong32 mAck;				// remembers highest message number acknowledged by the other host
	ulong32 mLastReliable;	// remembers number of highest packet we sent our last reliable message in

	bool mReliableSent;				// remembers last reliable number (even/odd) sent by us.
	bool mReliableIncoming;			// remembers last reliable number (even/odd) acknowledged by us. It's constantly being sent to other host.
	bool mReliableAck;				// remembers last reliable number (even/odd) acknowledged by the other host

	ulong32 mLastReliableMessageToAck;	// special case for map loading!

	// Statistics
	PeerStats mStatsData;
	PeerStats mStatsDataTemp;
	DynamicArray<PeerStats> mStatsChunks;
	unsigned short mStatsCursor;
	void SetStatsNumberToAverage( int newNumber );

	ulong32 TempPhysFrames;	// this variable is used only to give the missiles info about prediction

	// Stats for frame contents
	// - downstream
	unsigned short mDFCCreate;
	unsigned short mDFCUpdate;
	unsigned short mDFCDelete;
	unsigned short mDFCReliable;
	unsigned short mDFCUnreliable;
	// - upstream
	unsigned short mUFCPlayerDeltas;
	unsigned short mUFCUnreliable;
	unsigned short mUFCReliable;
};



#endif //_NETWORKPEER2_

