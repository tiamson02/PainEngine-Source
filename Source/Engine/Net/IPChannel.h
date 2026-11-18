#error This file is obselete

// #pragma once
#ifndef _IPCHANNEL_
#define _IPCHANNEL_


//#define CRC_IN_PACKETS

// 4 bytes are added here just in case, to avoid 1400-byte packets. Some routers cut them, I heard.
#ifdef CRC_IN_PACKETS
#define MAX_PACKET_HEADER_BYTES (16+4+1)
#else
#define MAX_PACKET_HEADER_BYTES (8+4+1)
#endif

// +1 is added just in case && for tokens at the end of each message

#define ICON_SHOW_TIMEOUT 1000

#include "Net/IpDriver.h"

class IPChannelStats
{
public:
	DWORD mTimeOfCreation;
	unsigned long mBytesSent;
	unsigned long mBytesReceived;
	unsigned long mPacketsSent;
	unsigned long mPacketsReceived;
	unsigned long mReliablePacketsAcknowledged;

	IPChannelStats( DWORD time = 0 ) : mBytesSent(0), mBytesReceived(0), mPacketsSent(0),
		mPacketsReceived(0), mReliablePacketsAcknowledged(0), mTimeOfCreation(time) {};
	~IPChannelStats( void ) {};

	inline void Init( DWORD time = 0 )
	{
		mBytesSent = mBytesReceived = mPacketsSent = mPacketsReceived = mReliablePacketsAcknowledged = 0;
		mTimeOfCreation = time;
	}

	inline IPChannelStats& operator=( const IPChannelStats& other )
	{
		mBytesSent = other.mBytesSent;
		mBytesReceived = other.mBytesReceived;
		mPacketsSent = other.mPacketsSent;
		mPacketsReceived = other.mPacketsReceived;
		mTimeOfCreation = other.mTimeOfCreation;
		return *this;
	}

	inline IPChannelStats& operator+=( const IPChannelStats& other )
	{
		mBytesSent += other.mBytesSent;
		mBytesReceived += other.mBytesReceived;
		mPacketsSent += other.mPacketsSent;
		mPacketsReceived += other.mPacketsReceived;
		if( mTimeOfCreation > other.mTimeOfCreation )
			mTimeOfCreation = other.mTimeOfCreation;
		return *this;
    }

	inline String Print( void );
};

class IPChannel
{
	friend class IPDriver;
	friend class DemoRecording;
	friend class NetworkClient;
public:

	enum IPChannelStates
	{
		OK = 0,
		DISCONNECTED = 10,
	};

	enum BinaryCommands
	{
		NETCLIENT = (0<<4),
		MAINTENANCE = (1<<4),
	};

	IPChannel( IPDriver* driver, IpAddress* address );
	~IPChannel( void );
	void Disconnect( void );

	void Tick( void );

	// Sending
	bool Send( UdpMessage* unreliable );
	void SendReliable( UdpMessage* reliable ) {	mReliableMessages.Add( reliable ); };	// Actually, will send it as soon as it can
	bool SendMaintenanceMessage( bool reliable, UdpMessage* message );

	// Miscellaneous
	double GetPing( void ) { return mAveragePing; };
	void PingReset( void ) { mPingReset = true; };
	float GetPacketLoss( void ) { return mPacketLoss; };
	void LoadingMap( bool loading ) { if( loading ) mStreamingTimeout = STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD; else mStreamingTimeout = STREAMING_TIMEOUT_TIME; };
	void LoadingMapOnClient( bool loading ) { if( loading ) mStreamingTimeout = STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD; else mStreamingTimeout = STREAMING_TIMEOUT_TIME; };
	void ClearReliableMessages( void );
	unsigned short GetMaxUdpPacketSize( void ) { return mMaxUdpPacketSize; };
	void SetMaxUdpPacketSize( unsigned short mups );
	void ReportUnsentReliables( void );

	bool NotReceivingPacketsLately( void ) { return ( GetCurrentTime() - mLastReceivedTime  > ICON_SHOW_TIMEOUT ); };

	// Public data
	NetworkClient* mNetworkClient;
	DWORD State;
	String Stats( bool temporary );
	DWORD PacketLatency( void ) { return mLastPacketApproxLatency; };

	// GetTickCount() difference
	DWORD mStartTime;

private:
	// -------------- Methods ------------------------
	void SendLatencyPacket( void );
	bool PrepareForReading( UdpMessage* incoming );
	void HandleChanneledPackage( UdpMessage* message );
	bool HandleSingleMessage( UdpMessage* message );
	inline bool CanSendReliableMessage( void )
	{
		return ( mReliableMessages.Size() &&
			( mAck >= mLastReliableMessageToAck ) &&
			( !mReliableMessages[0]->mWasSent || ( ( mAck > mLastReliable ) && ( mReliableAck != mReliableSent ) ) ) );
	}

	// ------------- Variables -----------------------
	IPDriver* mDriver;
	IpAddress mAddress;
	DynamicArray<UdpMessage*> mReliableMessages;

	unsigned short mMaxUdpPacketSize;

	DWORD mLastSendTime;
	DWORD mLastReceivedTime;
	DWORD mStreamingTimeout;

	// Statistics measurement
	IPChannelStats mStatsData;
	IPChannelStats mStatsDataTemp;
	IPChannelStats mStatsChunks[IPCHANNEL_STATS_FROM];
	unsigned short mStatsCursor;
	DWORD mStatsUpdateTime;

	DWORD mLastPacketsSendTime[PING_PACKETS_AVERAGED];
	double mAveragePing;
	DWORD mLastPacketApproxLatency;
	bool mPingReset;

	int mPacketLoss_Sent[PACKET_LOSS_FROM];
	int mPacketLoss_Lost[PACKET_LOSS_FROM];
	int mPacketsLost;
	int mPacketsLostOutOfOrder;
	unsigned short mPacketLoss_Cursor;
	DWORD mPacketLoss_UpdateTime;
	float mPacketLoss;

	// Streaming variables
	unsigned long mSent;			// packet number counter - each sent packet has a number higher by one, thanks to this.
	unsigned long mIncoming;		// remembers highest received packet number to discard old packets (those with numbers less than highest received )
	unsigned long mAck;				// remembers highest message number acknowledged by the other host
	unsigned long mLastReliable;	// remembers number of highest packet we sent our last reliable message in

	bool mReliableSent;				// remembers last reliable number (even/odd) sent by us.
	bool mReliableIncoming;			// remembers last reliable number (even/odd) acknowledged by us. It's constantly being sent to other host.
	bool mReliableAck;				// remembers last reliable number (even/odd) acknowledged by the other host

	unsigned long mLastReliableMessageToAck;	// special case for map loading!

	// for sending latency once every x ms
	DWORD mLastTimeLatencySent;
};


#endif //_IPCHANNEL_


