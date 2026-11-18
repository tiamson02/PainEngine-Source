
#ifndef __PKTVTRANSPORTTCP_H__
#define __PKTVTRANSPORTTCP_H__

#include "PKTVTransport.h"
#include "Core/LeString.h"
#include "Net/NetworkDevice2.h"
#include "Net/UdpMessage.h"

#define PKTV_TEMP_BUFFER_SIZE 65536

class PKTVTransportTCP : public PKTVTransport
{
	SOCKET						mSocket;						// Socket
	IpAddress					mSocketAddress;					// Socket info

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

public:

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

	PKTVTransportTCP( PeerTypes myType, String interfaceToUse = "" );			// create unconnected object (make socket)
	PKTVTransportTCP( PeerTypes myType, SOCKET sock, IpAddress socketAddr );	// create object for already connected socket
	virtual ~PKTVTransportTCP( void );											// always disconnects socket

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

	unsigned char GetPeerType( void ) { return mPeerType; };
	IpAddress GetPeerAddress( void ) { return mAddressToConnectTo; };

	virtual bool ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk );
	virtual void SendChunk( unsigned char chunkType, UdpMessage* chunk );

	void SendErrorMessage( unsigned char errorType, String additionalInfo = "" );
};

#endif	// __PKTVTRANSPORTTCP_H__

