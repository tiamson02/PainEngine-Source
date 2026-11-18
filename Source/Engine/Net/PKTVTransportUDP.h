
#ifndef __PKTVTRANSPORTUDP_H__
#define __PKTVTRANSPORTUDP_H__

#include "PKTVTransport.h"
#ifdef	TRANSGAMING
#include "LeString.h"
#include "NetworkDevice2.h"
#include "UdpMessage.h"
#else
#include "Core\LeString.h"
#include "Net\NetworkDevice2.h"
#include "Net\UdpMessage.h"
#endif

#define PKTV_TEMP_BUFFER_SIZE 65536

class PKTVTransportUDP : public PKTVTransport
{
	friend class PKTVProxyConnection;

	DynamicArray<UdpMessage*>	mBuffers;						// buffers for sending
	int							mBufferPosition;				// first unsent position in first buffer

	UdpMessage					mTempBuffer;					// for receiving
	unsigned int				mTempBufferPos;

	UdpMessage*					mReceivingBuffer;				// buffer for receiving
	unsigned char				mReceivingType;					// chunk type of chunk being received
	unsigned int				mReceivingSize;					// chunk size of chunk being received
	DynamicArray<UdpMessage*>	mReceivedChunks;				// full chunks already received
	DynamicArray<unsigned char>	mReceivedTypes;					// types of those chunks

	enum
	{
		BROKEN = 0,
		IDLE = 1,
		CONNECTING = 2,
		CONNECTED = 3,
		AUTHENTICATING = 4,
		READY = 5,
	}
	mState;

	IpAddress					mAddressToConnectTo;
	unsigned char				mPeerType;
	unsigned char				mMyType;

	bool						mServerSide;					// Are we on server-side, or on client-side? ( for the connection process )
	void SendThings( void );
	void ReceiveThings( void );

	// Overflow
	void TestOverflow( void );
	void ExecuteOverflowDisconnect( void );
	void Close( void );

	// UDP to TCP (resending, requests etc.)
	enum
	{
		MSG_HDR_DATA = 0,
		MSG_HDR_RESEND = 1,
		MSG_HDR_KEEPALIVE = 2,
	};

	// Connection requesting
	void SendRequest( void );
	DWORD mLastRequestTime;
	unsigned char mRequestNumber;

	// Packet receiving
	void HandleMessage( UdpMessage* mesg );

	// Queues

	struct MessageInQueue
	{
		UdpMessage* Mesg;
		MessageInQueue* Next;
		ulong32 MesgNumber;
	};

	MessageInQueue* SendingQueueStart;
	MessageInQueue* SendingQueueEnd;
	ulong32 NextToSend;

	MessageInQueue* ReceivingQueueStart;
	MessageInQueue* ReceivingQueueEnd;
	ulong32 NextToReceive;	// to give to ReceiveThings, really

	void AddToSendingQueue( UdpMessage* mesg )
	{
		MessageInQueue* miq = new MessageInQueue;
		miq->Mesg = mesg;
		miq->Next = 0;
		miq->MesgNumber = NextToSend++;

		if( SendingQueueEnd )
			SendingQueueEnd->Next = miq;
		else
			SendingQueueStart = miq;
		SendingQueueEnd = miq;
	}

	// Requests for resending
	void CheckSendResendRequest( DWORD curTime );
	DWORD mLastResendRequestTime;

	struct ToResend
	{
		ulong32 Index;
		DWORD Time;
	};

	DynamicArray<ToResend> mToResend;

	DWORD mLastIncomingMessageTime;
	DWORD mLastOutgoingMessageTime;

public:

	void SendKeepalive( void );

	enum
	{
		COMMAND_AUTH = 0,
		COMMAND_AUTH_ANSWER = 1,
		COMMAND_ERROR = 2,
	};

	enum PeerTypes
	{
		PEER_UNKNOWN = 0,
		PEER_SERVER = 1,
		PEER_PROXY = 2,
		PEER_CLIENT = 3,
	};

	PKTVTransportUDP( PeerTypes myType, IpAddress address, bool connected = false );
	virtual ~PKTVTransportUDP( void );

	void Tick( void );
	void ConnectTo( IpAddress address );

	virtual bool Ready( void ) { return ( mState == READY ); }	// connected and authorized here
	bool Connected( void ) { return ( mState >= CONNECTED ); }	// connected here
	bool Broken( void ) { return ( mState == BROKEN ); }		// broken here

	// Used for authentication
	String			AuthenticationString;

	bool			ApprovedByManager;

	ulong32	MinFramesToOverflow;
	ulong32	ExpectedFrame;
	bool			GetFullFrame;

	unsigned char GetPeerType( void ) { return mPeerType; };
	IpAddress GetPeerAddress( void ) { return mAddressToConnectTo; };

	virtual bool ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk );
	virtual void SendChunk( unsigned char chunkType, UdpMessage* chunk );

	void SendErrorMessage( unsigned char errorType, String additionalInfo = "" );
};

#endif	// __PKTVTRANSPORTTCP_H__

