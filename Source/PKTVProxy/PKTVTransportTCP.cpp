
#include "Engine.h"
#include "PKTVTransportTCP.h"

#include "LogBuffer.h"
#include "ProxySystem.h"
#include "SystemDriver.h"
#include "PKTVManager.h"

#define STATS_STEP_MS 1000
#define STATS_STEPS_TO_STORE 5

extern String GetSocketErrorString( void );	// translate WSA last error to string
extern bool Resolve( const char* hostName, IpAddress& ipAddr );

PKTVTransportTCP::PKTVTransportTCP( PeerTypes myType, String interfaceToUse )
:	PKTVTransport()
,	mErrorString("")
,	mSocket(INVALID_SOCKET)
,	mBufferPosition(0)
,	mState(IDLE)
,	mReceivingBuffer(0)
,	mReceivingType(0)
,	mReceivingSize(0)
,	mTempBuffer(PKTV_TEMP_BUFFER_SIZE)
,	mTempBufferPos(0)
,	mServerSide( false )
,	AuthenticationStringForProxies("")
,	AuthenticationStringForClients("")
,	mPeerType(PEER_UNKNOWN)
,	mMyType(myType)
,	ApprovedByManager( false )
,	mTimeSinceLastStatsChange(0)
,	MinFramesToOverflow(6000)	// about 5 minutes
,	ExpectedFrame(0)
{
	mTimeOfLastStatsTest = GSystem->SysDriver->GetCurrentTimeMS();
	mLastStats.Add(mOverallStats);

	// Open new socket on the given interface
	in_addr	hostAddr;
	hostAddr.S_un.S_addr = INADDR_ANY;

	if( interfaceToUse.Len() )
	{
		IpAddress temp;
		if( !Resolve( interfaceToUse.AsChar(), temp ) )
		{
			mErrorString = String::Sprintf( "PKTV Transport Init Error: tried to resolve given interface and failed!\n" );
			GLog->Print( mErrorString.AsChar() );
			mState = BROKEN;
			return;
		}
		else
			hostAddr.S_un.S_addr = temp.IP;
	}
	else
	{
		char hostnam[256];

		if( gethostname( hostnam, 255 ) )
		{
			mErrorString = String::Sprintf( "PKTV Transport Init Error: gethostname() call failed!! Can't find this computer's name! Returned: %s\n", GetSocketErrorString().AsChar() );
			GLog->Print( mErrorString.AsChar() );
			mState = BROKEN;
			return;
		}

		hostent* host = gethostbyname( hostnam );
		if( !host )
		{
			mErrorString = String::Sprintf( "PKTV Transport Init Error: failed to resolve our IP with gethostbyname()!!\n" );
			GLog->Print( mErrorString.AsChar() );
			mState = BROKEN;
			return;
		}

		hostAddr = *(in_addr*)(*host->h_addr_list);
	}

	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( INVALID_SOCKET == sock )
	{
		mErrorString = String::Sprintf( "PKTV Transport Init Error: Couldn't open connection socket: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		mState = BROKEN;
		return;
	}

	// non-blocking socket
	ulong32 one = 1;
	if( ioctlsocket( sock, FIONBIO, &one ) == -1)
	{
		mErrorString = String::Sprintf( "PKTV Transport Init Error: Couldn't make socket non-blocking: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		closesocket( sock );
		mState = BROKEN;
		return;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = hostAddr;
	addr.sin_port = 0;	// any port

	if( bind( sock, (sockaddr*)&addr, sizeof(sockaddr_in) ) )
	{
		mErrorString = String::Sprintf( "PKTV Transport Init Error: Failed to bind socket: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		closesocket( sock );
		mState = BROKEN;
		return;
	}

	// Find out what port we were assigned
	sockaddr_in toRead;
	int nameLen = sizeof( sockaddr_in );
	if( getsockname( sock, (sockaddr*)&toRead, &nameLen ) )
	{
		mErrorString = String::Sprintf( "PKTV Transport Init Error: getsockname on bound socket failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		closesocket( sock );
		mState = BROKEN;
		return;
	}

	mSocket = sock;
	mSocketAddress.IP = toRead.sin_addr.S_un.S_addr;
	mSocketAddress.Port = ntohs(toRead.sin_port);

	String t = String::Sprintf( "TCP Socket bound: %d.%d.%d.%d:%d\n",
		mSocketAddress.ip1, mSocketAddress.ip2,
		mSocketAddress.ip3, mSocketAddress.ip4, mSocketAddress.Port );
	GLog->Print( t.AsChar() );
}

PKTVTransportTCP::PKTVTransportTCP( PeerTypes myType, SOCKET sock, IpAddress socketAddr )
:	PKTVTransport()
,	mSocket( sock )
,	mSocketAddress( socketAddr )
,	mErrorString("")
,	mBufferPosition(0)
,	mState(CONNECTED)
,	mReceivingBuffer(0)
,	mReceivingType(0)
,	mReceivingSize(0)
,	mTempBuffer(PKTV_TEMP_BUFFER_SIZE)
,	mTempBufferPos(0)
,	mServerSide( true )
,	AuthenticationStringForProxies("")
,	AuthenticationStringForClients("")
,	mPeerType(PEER_UNKNOWN)
,	mMyType(myType)
,	mTimeSinceLastStatsChange(0)
,	ExpectedFrame(0)
{
	mTimeOfLastStatsTest = GSystem->SysDriver->GetCurrentTimeMS();
	mLastStats.Add(mOverallStats);

	// Find out to whom we are connected
	sockaddr_in toRead;
	int nameLen = sizeof( sockaddr_in );
	if( getpeername( sock, (sockaddr*)&toRead, &nameLen ) )
	{
		mErrorString = String::Sprintf( "PKTV Transport Init Error: getpeername on connected socket failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		GSystem->PKTV->SocketTrashcan.AddSocketToClose( sock );
//		closesocket( sock );
		mState = BROKEN;
		return;
	}

	mAddressToConnectTo.IP = toRead.sin_addr.S_un.S_addr;
	mAddressToConnectTo.Port = ntohs(toRead.sin_port);
}

PKTVTransportTCP::~PKTVTransportTCP( void )
{
	Close();
}

void PKTVTransportTCP::Close( void )
{
	// Close socket
	if( mSocket != INVALID_SOCKET )
	{
		GSystem->PKTV->SocketTrashcan.AddSocketToClose( mSocket );
//		closesocket( mSocket );
	}
	mSocket = INVALID_SOCKET;

	// Remove buffers
	while( mBuffers.Size() )
	{
		delete mBuffers[0];
		mBuffers.FastRemove(0);
	}
	mBufferPosition = 0;

	// Remove received chunks
	for( int i = 0; i < mReceivedChunks.Size(); ++ i )
		delete mReceivedChunks[i];
	mReceivedChunks.Clear();
	mReceivedTypes.Clear();

	// Delete receiving buffer
	if(	mReceivingBuffer )
		delete mReceivingBuffer;
	mReceivingBuffer = 0;
}

bool PKTVTransportTCP::ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk )
{
	ReceiveThings();	// receive newest data
	if( mReceivedChunks.Size() )
	{
		*chunk = mReceivedChunks[0];
		chunkType = mReceivedTypes[0];
		mReceivedChunks.Remove(0);
		mReceivedTypes.Remove(0);
		GLog->Print( "Received chunk %d of size %d\n", chunkType, (*chunk)->GetSize() );
		return true;
	}
	return false;
}

void PKTVTransportTCP::SendChunk( unsigned char chunkType, UdpMessage* chunk )
{
	Assert( chunk->GetSize() );
	if( !chunk->GetSize() ) return;
	chunk->SeekToBegin();
	GLog->Print( "Sent chunk %d of size %d\n", chunkType, chunk->GetSize() );
	UdpMessage* mesg = new UdpMessage( chunk->GetSize() + sizeof(unsigned char) + sizeof(ulong32) );
	mesg->WriteUChar( chunkType );
	mesg->WriteULong( chunk->GetSize() );
	mesg->WriteData( chunk );
	mesg->EndWriting( true );
	mesg->SeekToBegin();
	mBuffers.Add( mesg );

	TestOverflow();

	SendThings();
}

void PKTVTransportTCP::Tick( void )
{
	switch( mState )
	{
	case BROKEN:
	case IDLE:
		return;
	case CONNECTING:			// Test to find out if the connection succeeded or not
		{
			Assert( !mServerSide );
			TIMEVAL timeout = {0,0};
			fd_set okSet;
			FD_ZERO(&okSet);
			FD_SET(mSocket,&okSet);
			fd_set badSet;
			FD_ZERO(&badSet);
			FD_SET(mSocket,&badSet);
			if( ::select( 0, 0, &okSet, &badSet, &timeout ) )
			{
				if( FD_ISSET( mSocket, &okSet ) )	// Connected
				{
					mState = CONNECTED;
					Tick();
				}
				else								// Connection failure - try to reconnect
				{
					Assert( FD_ISSET( mSocket, &badSet ) );
					mState = IDLE;
					mErrorString = String::Sprintf( "PKTV Transport Error: connection failure!!\n" );
					GLog->Print( mErrorString.AsChar() );
					return;
				}
			}
		}
		return;
	case CONNECTED:
		if( mServerSide )
		{
			// Wait for authentication data and respond to it. If answered ok, go to READY state. Else discnnect.

			unsigned char chunkType;
			UdpMessage* mesg = 0;
			if( ReceiveChunk( chunkType, &mesg ) )
			{
				if( chunkType == CHUNK_COMMAND )
				{
					unsigned char command = mesg->ReadUChar();
					if( command == COMMAND_AUTH )
					{
						String auth = mesg->ReadString();
						mPeerType = mesg->ReadUChar();
						ExpectedFrame = mesg->ReadULongOptimized();
						if( ( ( mPeerType == PEER_PROXY ) && ( auth == AuthenticationStringForProxies ) ) ||
							( ( mPeerType == PEER_CLIENT ) && ( auth == AuthenticationStringForClients ) ) )
						{
							delete mesg;

							// Answer
							// Prepare mesg
							UdpMessage mesg2;
							mesg2.WriteUChar( COMMAND_AUTH_ANSWER );
							mesg2.WriteBool( true );
							mesg2.WriteUChar( mMyType );
							mesg2.EndWriting();
							mesg2.Trim();

							// Send mesg
							SendChunk( CHUNK_COMMAND, &mesg2 );
							if( mState != BROKEN )	// no error
								mState = READY;
							return;
						}
						else
						{
							delete mesg;
							mState = BROKEN;
							SendErrorMessage( PKTVManager::ERR_WRONG_PASSWORD );
							GSystem->PKTV->SocketTrashcan.AddSocketToClose( mSocket );
							mSocket = INVALID_SOCKET;
							return;
						}
					}
				}

				// Something went wrong
				delete mesg;

				// Prepare mesg
				UdpMessage mesg2;
				mesg2.WriteUChar( COMMAND_AUTH_ANSWER );
				mesg2.WriteBool( false );
				mesg2.WriteUChar( mMyType );
				mesg2.EndWriting();
				mesg2.Trim();

				// Send mesg
				SendChunk( CHUNK_COMMAND, &mesg2 );
				if( mState != BROKEN )	// no error
					mState = READY;
				else
				{
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: authentication failure!!: %s\n", GetSocketErrorString().AsChar() );
					GLog->Print( mErrorString.AsChar() );
					GSystem->PKTV->SocketTrashcan.AddSocketToClose( mSocket );
//					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
				}
				return;
			}
		}
		else
		{
			// Send authentication data and go to AUTHENTICATING

			// Prepare mesg
			UdpMessage mesg;
			mesg.WriteUChar( COMMAND_AUTH );
			mesg.WriteString( (mMyType==PEER_PROXY)?AuthenticationStringForProxies.AsChar():AuthenticationStringForClients.AsChar() );
			mesg.WriteUChar( mMyType );
			mesg.WriteULongOptimized( ExpectedFrame );
			mesg.EndWriting();
			mesg.Trim();

			// Send mesg
			SendChunk( CHUNK_COMMAND, &mesg );
			if( mState != BROKEN )	// no error
				mState = AUTHENTICATING;
			else
				return;
		}
		break;
	case AUTHENTICATING:
		{
			Assert( !mServerSide );
			// If the answer is ok, go to READY state. Else disconnect and go to seeking new IP.
			unsigned char chunkType;
			UdpMessage* mesg = 0;
			if( ReceiveChunk( chunkType, &mesg ) )
			{
				if( chunkType == CHUNK_COMMAND )
				{
					unsigned char command = mesg->ReadUChar();
					if( command == COMMAND_AUTH_ANSWER )
					{
						bool answer = mesg->ReadBool();
						mPeerType = mesg->ReadUChar();
						if( answer )
						{
							mState = READY;
							delete mesg;
							return;
						}
					}
					else if( command == COMMAND_ERROR )
					{
						unsigned char type = mesg->ReadUChar();
						String desc = mesg->ReadString();

						delete mesg;
						mState = BROKEN;

						if( type == PKTVManager::ERR_WRONG_PASSWORD )
							mErrorString = "Wrong password!!!\n";
						else if( type == PKTVManager::ERR_TOO_MANY_CLIENTS )
							mErrorString = "Too many clients!!!\n";

						GLog->Print( mErrorString.AsChar() );
						GSystem->PKTV->SocketTrashcan.AddSocketToClose( mSocket );
						mSocket = INVALID_SOCKET;
						return;
					}
				}
				delete mesg;

				// Something went wrong
				mState = BROKEN;
				mErrorString = String::Sprintf( "PKTV Transport Error: authentication failure on client!!: %s\n", GetSocketErrorString().AsChar() );
				GLog->Print( mErrorString.AsChar() );
				GSystem->PKTV->SocketTrashcan.AddSocketToClose( mSocket );
//				closesocket( mSocket );
				mSocket = INVALID_SOCKET;
				return;
			}
		}

		break;
	case READY:
		SendThings();
		ReceiveThings();
		break;
	default:
		Assert( false );
		break;
	}
}

void PKTVTransportTCP::SendThings( void )
{
	TryStepStats();

	if( mBuffers.Size() )		// there are things to send
	{
		bool canSend = true;

		while( canSend && mBuffers.Size() )
		{
			int result = ::send( mSocket, mBuffers[0]->GetBuffer()+mBufferPosition, mBuffers[0]->GetSize()-mBufferPosition, 0 );
			if( result == SOCKET_ERROR )
			{
				switch( WSAGetLastError() )
				{
				case WSAEWOULDBLOCK:	// cannot send more now
					canSend = false;
					break;
				case WSAENETRESET:
					canSend = false;
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: connection closed due to net reset!!: %s\n", GetSocketErrorString().AsChar() );
					GLog->Print( mErrorString.AsChar() );
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					return;
				case WSAETIMEDOUT:		// connection aborted due to timeout
					canSend = false;
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: connection timeout!!: %s\n", GetSocketErrorString().AsChar() );
					GLog->Print( mErrorString.AsChar() );
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					return;
				case WSAECONNABORTED:	// connection aborted due to some failure
					canSend = false;
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: connection failure!!: %s\n", GetSocketErrorString().AsChar() );
					GLog->Print( mErrorString.AsChar() );
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					return;
				case WSAECONNRESET:		// connection aborted hard from other side
					canSend = false;
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: connection reset from other side!!: %s\n", GetSocketErrorString().AsChar() );
					GLog->Print( mErrorString.AsChar() );
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					return;
				default:
					mState = BROKEN;
					mErrorString = String::Sprintf( "PKTV Transport Error: Unhandled error in ::recv(): %d\n", WSAGetLastError() );
					GLog->Print( mErrorString.AsChar() );
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					Assert( false );
					return;
				}
			}
			else
			{
				// Stats
				{
					mOverallStats.BytesUL += result;
					mLastStats[mLastStats.Size()-1].BytesUL += result;
				}

				if( result < (mBuffers[0]->GetSize()-mBufferPosition) )
				{
					mBufferPosition += result;
					canSend = false;
				}
				else
				{
					delete mBuffers[0];
					mBuffers.Remove(0);
					mBufferPosition = 0;
				}
			}
		}
	}
}

void PKTVTransportTCP::ReceiveThings( void )
{
	if( mState == BROKEN ) return;
	// Check if there are things to receive
	int result = ::recv( mSocket, mTempBuffer.GetBuffer()+mTempBufferPos, PKTV_TEMP_BUFFER_SIZE-mTempBufferPos, 0 );
	if( result == SOCKET_ERROR )
	{
		switch( WSAGetLastError() )
		{
		case WSAEWOULDBLOCK:		// no data
			break;
		case WSAECONNRESET:
		case WSAECONNABORTED:
			mState = BROKEN;
			mErrorString = String::Sprintf( "PKTV Transport Error: connection reset from other side!!: %s\n", GetSocketErrorString().AsChar() );
			GLog->Print( mErrorString.AsChar() );
			closesocket( mSocket );
			mSocket = INVALID_SOCKET;
			return;
		default:
			mState = BROKEN;
			mErrorString = String::Sprintf( "PKTV Transport Error: Unhandled error in ::recv(): %d\n", WSAGetLastError() );
			GLog->Print( mErrorString.AsChar() );
			Assert( false );
			closesocket( mSocket );
			mSocket = INVALID_SOCKET;
			return;
		}
	}
	else if( result == 0 )	// graceful disconnect
	{
		mState = BROKEN;
		mErrorString = "";
		GLog->Print( "PKTV Transport: connection reset gracefully from other side.\n" );
		closesocket( mSocket );
		mSocket = INVALID_SOCKET;
		return;
	}
	else
	{
		// Stats
		{
			mOverallStats.BytesDL += result;
			mLastStats[mLastStats.Size()-1].BytesDL += result;
		}

		mTempBufferPos += result;
		mTempBuffer.SeekToBegin();

		bool finished = false;
		do
		{
			if( !mReceivingBuffer )
			{
				if( mTempBufferPos-mTempBuffer.GetPos() >= sizeof(unsigned char) + sizeof(ulong32) )
				{
					mReceivingType = mTempBuffer.ReadUChar();
					mReceivingSize = mTempBuffer.ReadULong();
					Assert( mReceivingSize );
					mReceivingBuffer = new UdpMessage( mReceivingSize );
				}
				else	// those will be left for the new recv()
				{
					char temp[sizeof(unsigned char) + sizeof(ulong32)];
					unsigned int start = mTempBuffer.GetPos();
					for( unsigned int i = 0; i < mTempBufferPos-start; ++i )
						temp[i] = mTempBuffer.ReadChar();
					mTempBufferPos -= start;
					mTempBuffer.SeekToBegin();
					mTempBuffer.WriteData( temp, mTempBufferPos );
					finished = true;
				}
			}
			else	// we have a receiving buffer, and it is not full
			{
				unsigned int leftToRead = mReceivingSize-mReceivingBuffer->GetPos();
				if( mTempBufferPos >= leftToRead+mTempBuffer.GetPos() )
				// really it is: "mTempBufferPos-mTempBuffer.GetPos() >= leftToRead", but comparing unsigneds is tricky
				{
					mReceivingBuffer->WriteData( &mTempBuffer, leftToRead );
					mReceivingBuffer->EndWriting( true );
					mReceivingBuffer->SeekToBegin();

					mTempBuffer.ScrapData( leftToRead );

					if( ( mReceivingType == COMMAND_ERROR ) &&
						( mReceivingBuffer->PeekUChar() == PKTVManager::ERR_TRANSPORT_OVERFLOW ) )
					{
						delete mReceivingBuffer;
						mState = BROKEN;

						if( mServerSide )
						{
							mErrorString = "Transport overflow on client connection!!!\n";
							++GSystem->PKTV->ServerOverflows;
						}
						else
						{
							mErrorString = "Transport overflow on source!!!\n";
							++GSystem->PKTV->ClientOverflows;
						}

						GLog->Print( mErrorString.AsChar() );
						Close();
						return;
					}
					else
					{
						mReceivedChunks.Add( mReceivingBuffer );
						mReceivedTypes.Add( mReceivingType );
					}
					mReceivingBuffer = 0;
				}
				else
				{
					mReceivingBuffer->WriteData( &mTempBuffer, mTempBufferPos-mTempBuffer.GetPos() );
					mTempBufferPos = 0;
					mTempBuffer.SeekToBegin();
					finished = true;
				}
			}
		}
		while( !finished );
	}
}

void PKTVTransportTCP::ConnectTo( IpAddress address )
{
	if( mState != IDLE )
		return;

	// Set up connecting to the given address
	mServerSide = false;
	mState = CONNECTING;
	mAddressToConnectTo = address;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = address.IP;
	addr.sin_port = htons( address.Port );

#ifdef _DEBUG
	Assert( ::connect( mSocket, (sockaddr*)&addr, sizeof(sockaddr_in) ) == SOCKET_ERROR );
#else
	::connect( mSocket, (sockaddr*)&addr, sizeof(sockaddr_in) );
#endif

	switch( WSAGetLastError() )
	{
	case WSAEWOULDBLOCK:	// that's normal
		break;
	default:
		mErrorString = String::Sprintf( "PKTV Transport Error: Unhandled error in ::connect(): %d\n", WSAGetLastError() );
		GLog->Print( mErrorString.AsChar() );
		mState = BROKEN;
		Assert( false );
		closesocket( mSocket );
		mSocket = INVALID_SOCKET;
		break;
	}
}

void PKTVTransportTCP::TryStepStats( void )
{
	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
	DWORD diff = curTime-mTimeOfLastStatsTest;
	mTimeOfLastStatsTest = curTime;

	mTimeSinceLastStatsChange += diff;
	while( mTimeSinceLastStatsChange > STATS_STEP_MS )
	{
		if( mLastStats.Size() >= STATS_STEPS_TO_STORE )
			mLastStats.Remove(0);
		DlUl t;
		mLastStats.Add(t);
		mTimeSinceLastStatsChange -= STATS_STEP_MS;
	}
}

DlUl PKTVTransportTCP::GetAverage( void )
{
	DlUl sum;
	if( mLastStats.Size() < 2 )
		return sum;

	for( int i = 0; i < mLastStats.Size()-1; ++i )
		sum += mLastStats[i];

	sum.BytesDL /= (mLastStats.Size()-1);
	sum.BytesUL /= (mLastStats.Size()-1);
	sum.PacketsDL = sum.PacketsUL = 0;

	return sum;
}

void PKTVTransportTCP::SendErrorMessage( unsigned char errorType, String additionalInfo )
{
	UdpMessage error;
	error.WriteUChar( COMMAND_ERROR );
	error.WriteUChar( errorType );
	error.WriteString( additionalInfo.AsChar() );
	error.EndWriting();
	SendChunk( PKTVTransport::CHUNK_COMMAND, &error );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Sent TCP error %u (%s)\n", errorType, additionalInfo.AsChar() );
#endif
}

void PKTVTransportTCP::TestOverflow( void )
{
	int size = mBuffers.Size();
	if( !size ) return;

	int i = 0;
	if( mBuffers[0]->GetPos() )
		++i;	// pass first chunk

	ulong32 frames = 0;
	while( i < size )
	{
		if( mBuffers[i]->PeekUChar() == CHUNK_FRAME )
		{
			++frames;
			if( frames > MinFramesToOverflow )
			{
				ExecuteOverflowDisconnect();
				return;
			}
		}
		++i;
	}
}

void PKTVTransportTCP::ExecuteOverflowDisconnect( void )
{
	int i = 0;
	if( mBuffers[0]->GetPos() )
		++i;	// pass first chunk

	while( i < mBuffers.Size() )
	{
		// Find first frame
		if( mBuffers[i]->PeekUChar() == CHUNK_FRAME )
		{
			// Remove everything after first frame
			while( mBuffers.Size() > i )
			{
				UdpMessage* mesg = mBuffers[i];
				mBuffers.FastRemove(i);
				delete mesg;
			}

			// Add overflow disconnection message instead
			SendErrorMessage( PKTVManager::ERR_TRANSPORT_OVERFLOW );
			if( mServerSide )
				++GSystem->PKTV->ServerOverflows;
			else
				++GSystem->PKTV->ClientOverflows;
			return;
		}
		++i;
	}
	Assert( false );
}
