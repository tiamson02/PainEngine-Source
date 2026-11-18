
#include "Engine.h"
#include "PKTVTransportUDP.h"
#include "PKTVProxyConnection.h"
#include "LogBuffer.h"
#include "PKTVManager.h"
#include "ProxySystem.h"
#include "SystemDriver.h"

#define UDP_CONNECTION_TIMEOUT 5000			// miliseconds
#define UDP_CONNECTION_RETRY_COUNT 5		// times resend connection request
#define RESEND_REQUEST_INTERVAL	1000		// miliseconds
#define PACKET_RESEND_REQUEST_TIMEOUT 5000	// miliseconds
#define UDP_MESSAGE_TCP_SIZE 500			// bytes
#define PKTV_TCP_TIMEOUT 120000				// miliseconds
//#define PKTV_KEEPALIVE_INTERVAL 3000		// miliseconds

#define STATS_STEP_MS 1000
#define STATS_STEPS_TO_STORE 5

extern String GetSocketErrorString( void );	// translate WSA last error to string
extern bool Resolve( const char* hostName, IpAddress& ipAddr );

PKTVTransportUDP::PKTVTransportUDP( PeerTypes myType, IpAddress address, bool connected )
:	PKTVTransport()
,	mBufferPosition(0)
,	mState(IDLE)
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
,	ExpectedFrame(0)
,	GetFullFrame(false)
,	mLastRequestTime(0)
,	mRequestNumber(0)
,	SendingQueueStart(0)
,	SendingQueueEnd(0)
,	NextToSend(0)
,	ReceivingQueueStart(0)
,	ReceivingQueueEnd(0)
,	NextToReceive(0)
,	mLastResendRequestTime(0)
,	mLastIncomingMessageTime(0)
,	mLastOutgoingMessageTime(0)
,	mTimeSinceLastStatsChange(0)
{
	mTimeOfLastStatsTest = GSystem->SysDriver->GetCurrentTimeMS();
	mLastStats.Add(mOverallStats);

	mAddressToConnectTo = address;
	// if connected, wait for mesgs, else initiate connection messages sending
	if( connected )
	{
		mState = CONNECTED;
		mLastIncomingMessageTime = mTimeOfLastStatsTest;	// curtime

		PKTVTransportUDP** tr = GSystem->PKTV->mUDP->mTransports.Find( mAddressToConnectTo );
		if( tr )
		{
			if( *tr )
				GLog->Print( "PKTV Transport Warning: a transport already connected to given address, address wants to connect to us!!!" );
			else
				*tr = this;
		}
		else
			GSystem->PKTV->mUDP->mTransports.Insert( mAddressToConnectTo, this );
	}
}

PKTVTransportUDP::~PKTVTransportUDP( void )
{
	Close();
}

void PKTVTransportUDP::SendRequest( void )
{
	++mRequestNumber;
	UdpMessage mesg;
	mesg.WriteUChar( PKTVManager::CMD_TRANSPORT_REQUEST );
	mesg.EndWriting();
	GSystem->PKTV->mUDP->Send( &mesg, mAddressToConnectTo, false, false );
	mLastRequestTime = mLastOutgoingMessageTime = GSystem->SysDriver->GetCurrentTimeMS();
}

void PKTVTransportUDP::Close( void )
{
	SendThings();

	// Send empty message
	UdpMessage mesg;
	mesg.WriteUChar( PKTVManager::CMD_TRANSPORT_MESG );
	mesg.WriteUChar( MSG_HDR_DATA );
	mesg.WriteULongOptimized( NextToSend );
	mesg.EndWriting();
	GSystem->PKTV->mUDP->Send( &mesg, mAddressToConnectTo, false, false );

	// Make proxy forget us
	PKTVTransportUDP** t = GSystem->PKTV->mUDP->mTransports.Find( mAddressToConnectTo );
	Assert( t );
	*t = 0;

	// Remove queuese
	while( SendingQueueStart ) { MessageInQueue* miq = SendingQueueStart; SendingQueueStart = miq->Next; delete miq->Mesg; delete miq; }
	SendingQueueEnd = 0;
	while( ReceivingQueueStart ) { MessageInQueue* miq = ReceivingQueueStart; ReceivingQueueStart = miq->Next; delete miq->Mesg; delete miq; }
	ReceivingQueueEnd = 0;

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

bool PKTVTransportUDP::ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk )
{
	ReceiveThings();	// receive newest data
	if( mReceivedChunks.Size() )
	{
		*chunk = mReceivedChunks[0];
		chunkType = mReceivedTypes[0];
		mReceivedChunks.Remove(0);
		mReceivedTypes.Remove(0);
		return true;
	}
	return false;
}

void PKTVTransportUDP::SendChunk( unsigned char chunkType, UdpMessage* chunk )
{
	Assert( chunk->GetSize() );
	if( !chunk->GetSize() ) return;
	chunk->SeekToBegin();
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

void PKTVTransportUDP::Tick( void )
{
	switch( mState )
	{
	case BROKEN:
	case IDLE:
		return;
	case CONNECTING:			// Test to find out if the connection succeeded or not
		{
			Assert( !mServerSide );

			DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
			if( curTime - mLastRequestTime > UDP_CONNECTION_TIMEOUT )
			{
				if( mRequestNumber > UDP_CONNECTION_RETRY_COUNT )
				{
					mState = IDLE;
					GLog->Print( "PKTV Transport Error: connection failure!!\n" );
					return;
				}
				else
					SendRequest();
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
					GLog->Print( "PKTV Transport Error: authentication failure!!: %s\n", GetSocketErrorString().AsChar() );
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

						if( type == PKTVManager::ERR_TOO_MANY_CLIENTS )
							GLog->Print( "Too many clients!!!\n" );
						else if( type == PKTVManager::ERR_WRONG_PASSWORD )
							GLog->Print( "Wrong password!!!\n" );
						else
							GLog->Print( "Unknown error!!!\n" );
						return;
					}
				}
				delete mesg;

				// Something went wrong
				mState = BROKEN;
				GLog->Print( "PKTV Transport Error: authentication failure!!: %s\n", GetSocketErrorString().AsChar() );
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

void PKTVTransportUDP::SendThings( void )
{
	if( !mBuffers.Size() ) return;		// nothing to send

	UdpMessage* toSend = new UdpMessage;
	ulong32 size = 0;
	toSend->WriteUChar( PKTVManager::CMD_TRANSPORT_MESG );
	toSend->WriteUChar( MSG_HDR_DATA );
	toSend->WriteULongOptimized( NextToSend );

	while( mBuffers.Size() )
	{
		if( (mBuffers[0]->GetSize()-mBufferPosition) < (int)(UDP_MESSAGE_TCP_SIZE-size) )
		{
			Assert( mBuffers[0]->GetSize() > mBufferPosition );
			toSend->WriteData( mBuffers[0], mBufferPosition, mBuffers[0]->GetSize()-mBufferPosition );
			size += (mBuffers[0]->GetSize()-mBufferPosition);
			delete mBuffers[0];
			mBuffers.Remove(0);
			mBufferPosition = 0;
		}
		else
		{
			Assert( UDP_MESSAGE_TCP_SIZE > size );
			toSend->WriteData( mBuffers[0], mBufferPosition, (UDP_MESSAGE_TCP_SIZE-size) );
			if( (mBuffers[0]->GetSize()-mBufferPosition) == (int)(UDP_MESSAGE_TCP_SIZE-size) )
			{
				delete mBuffers[0];
				mBuffers.Remove(0);
				mBufferPosition = 0;
			}
			else
				mBufferPosition += (UDP_MESSAGE_TCP_SIZE-size);

			toSend->EndWriting();

			// Stats
			mOverallStats.BytesUL += toSend->GetSize();
			mLastStats[mLastStats.Size()-1].BytesUL += toSend->GetSize();

			if( !GSystem->PKTV->mUDP->Send( toSend, mAddressToConnectTo, false, false ) )
			{
				mState = BROKEN;
				delete toSend;
				return;
			}
			AddToSendingQueue( toSend );

			mLastOutgoingMessageTime = GSystem->SysDriver->GetCurrentTimeMS();

			size = 0;
			toSend = new UdpMessage;
			toSend->WriteUChar( PKTVManager::CMD_TRANSPORT_MESG );
			toSend->WriteUChar( MSG_HDR_DATA );
			toSend->WriteULongOptimized( NextToSend );
		}
	}

	if( size )
	{
		toSend->EndWriting();

		// Stats
		mOverallStats.BytesUL += toSend->GetSize();
		mLastStats[mLastStats.Size()-1].BytesUL += toSend->GetSize();

		if( !GSystem->PKTV->mUDP->Send( toSend, mAddressToConnectTo, false, false ) )
		{
			mState = BROKEN;
			delete toSend;
			return;
		}
		AddToSendingQueue( toSend );
		mLastOutgoingMessageTime = GSystem->SysDriver->GetCurrentTimeMS();
	}
	else
		delete toSend;
}

void PKTVTransportUDP::SendKeepalive( void )
{
	UdpMessage keepalive;
	keepalive.WriteUChar( PKTVManager::CMD_TRANSPORT_MESG );
	keepalive.WriteUChar( MSG_HDR_KEEPALIVE );
	keepalive.EndWriting();
	GSystem->PKTV->mUDP->Send( &keepalive, mAddressToConnectTo, false, false );
	mLastOutgoingMessageTime = GSystem->SysDriver->GetCurrentTimeMS();
}

void PKTVTransportUDP::ReceiveThings( void )
{
	if( mState == BROKEN ) return;

	// Check if there are things to receive

	// Put next bytes to receive into mTempBuffer.GetBuffer()+mTempBufferPos, up to PKTV_TEMP_BUFFER_SIZE-mTempBufferPos
	// and put numer of received bytes in result

	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
	if( curTime - mLastIncomingMessageTime > PKTV_TCP_TIMEOUT )
	{
		mState = BROKEN;
#ifdef PKTV_CONNECTION_DEBUG
		PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
		PKTVDLog->Print( "PKTV Transport: connection timeout!\n" );
#endif
		return;
	}
/*
	if( curTime - mLastOutgoingMessageTime > PKTV_KEEPALIVE_INTERVAL )
		SendKeepalive();
*/
	CheckSendResendRequest( curTime );

	MessageInQueue* miq = ReceivingQueueStart;
	if( !miq ) return;

	Assert( ( miq->MesgNumber != NextToReceive ) || ( miq->Mesg->GetSize() <= (int)(PKTV_TEMP_BUFFER_SIZE-mTempBufferPos) ) );

	int result = 0;

	if( !miq->Mesg->GetSize() )	// graceful disconnect
	{
		mState = BROKEN;
#ifdef PKTV_CONNECTION_DEBUG
		PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
		PKTVDLog->Print( "PKTV Transport: connection reset gracefully from other side.\n" );
#endif
		return;
	}

	while( miq && ( miq->MesgNumber == NextToReceive ) && ( miq->Mesg->GetSize() <= (int)(PKTV_TEMP_BUFFER_SIZE-mTempBufferPos-result) ) )
	{
		if( !miq->Mesg->GetSize() )
			break;
		miq->Mesg->ReadData( mTempBuffer.GetBuffer()+mTempBufferPos+result, miq->Mesg->GetSize() );
		result += miq->Mesg->GetSize();

		ReceivingQueueStart = miq->Next;
		delete miq->Mesg;
		delete miq;
		miq = ReceivingQueueStart;
		++NextToReceive;
	}

	if( !ReceivingQueueStart ) ReceivingQueueEnd = 0;
	if( !result ) return;

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
				Assert( mReceivingBuffer->GetSize() == (int)mReceivingSize );

				mTempBuffer.ScrapData( leftToRead );

				if( ( mReceivingType == COMMAND_ERROR ) &&
					( mReceivingBuffer->PeekUChar() == PKTVManager::ERR_TRANSPORT_OVERFLOW ) )
				{
					delete mReceivingBuffer;
					mState = BROKEN;

					if( mServerSide )
					{
						GLog->Print( "Transport overflow on client connection!!!\n" );
#ifdef PKTV_ENABLED
						++GEngine->PKTV->ServerOverflows;
#endif
					}
					else
					{
						GLog->Print( "Transport overflow on source!!!\n" );
#ifdef PKTV_ENABLED
						++GEngine->PKTV->ClientOverflows;
#endif
					}

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

void PKTVTransportUDP::ConnectTo( IpAddress address )
{
	if( mState != IDLE )
		return;

	// Set up connecting to the given address
	mServerSide = false;
	mState = CONNECTING;
	mAddressToConnectTo = address;

#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "connecting to: %s\n", address.AsString().AsChar() );
#endif

	PKTVTransportUDP** tr = GSystem->PKTV->mUDP->mTransports.Find( mAddressToConnectTo );
	if( tr )
	{
		if( *tr )
			GLog->Print( "PKTV Transport Error: a transport already connected to given address!!!" );
		else
			*tr = this;
	}
	else
		GSystem->PKTV->mUDP->mTransports.Insert( mAddressToConnectTo, this );
	mRequestNumber = 0;
	SendRequest();
}

void PKTVTransportUDP::SendErrorMessage( unsigned char errorType, String additionalInfo )
{
	UdpMessage error;
	error.WriteUChar( COMMAND_ERROR );
	error.WriteUChar( errorType );
	error.WriteString( additionalInfo.AsChar() );
	error.EndWriting();
	SendChunk( PKTVTransport::CHUNK_COMMAND, &error );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Sent UDP error %u (%s)\n", errorType, additionalInfo.AsChar() );
#endif
}

void PKTVTransportUDP::TestOverflow( void )
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

void PKTVTransportUDP::ExecuteOverflowDisconnect( void )
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
#ifdef PKTV_ENABLED
			if( mServerSide )
				++GEngine->PKTV->ServerOverflows;
			else
				++GEngine->PKTV->ClientOverflows;
#endif
			return;
		}
		++i;
	}
	Assert( false );
}

void PKTVTransportUDP::HandleMessage( UdpMessage* mesg )
{
	mesg->ReadUChar();			// pass mesg marker
	unsigned char command = mesg->ReadUChar();

	if( mState == CONNECTING )
		mState = CONNECTED;

	DWORD curTime = mLastIncomingMessageTime = GSystem->SysDriver->GetCurrentTimeMS();

	switch( command )
	{
	case MSG_HDR_DATA:
		{
			ulong32 messageNumber = mesg->ReadULongOptimized();

			// Assumming in receiving queue mesgs are stored only as data

			// - if data, add it to queue at the right place (and forget repeating)
			// - fill up/change mToResend queue
			MessageInQueue* miq = ReceivingQueueStart;
			MessageInQueue* lastMiq = 0;
			while( miq && ( miq->MesgNumber < messageNumber ) )
			{
				lastMiq = miq;
				miq = miq->Next;
			}

			if( miq )	// that means we either don't have this mesg in queue, and we should have, or that we just found it
			{
				if( miq->MesgNumber == messageNumber )
					break;	// already received; forget about it

				MessageInQueue* newMiq = new MessageInQueue;

				newMiq->Mesg = new UdpMessage;
				newMiq->Mesg->WriteData( mesg, mesg->GetSize()-mesg->GetPos() );
				newMiq->Mesg->EndWriting();
				newMiq->Mesg->SeekToBegin();

				newMiq->MesgNumber = messageNumber;
				newMiq->Next = miq;

				if( lastMiq )
	                lastMiq->Next = newMiq;
				else
					ReceivingQueueStart = newMiq;

				for( int i = 0; i < mToResend.Size(); ++i )
					if( mToResend[i].Index == messageNumber )
						mToResend.Remove( i );
			}
			else	// that means we went to end of queue; add mesg at end and add any numbers between last received and this one to mToResend
			{
				MessageInQueue* newMiq = new MessageInQueue;

				newMiq->Mesg = new UdpMessage;
				newMiq->Mesg->WriteData( mesg, mesg->GetSize()-mesg->GetPos() );
				newMiq->Mesg->EndWriting();
				newMiq->Mesg->SeekToBegin();

				newMiq->MesgNumber = messageNumber;
				newMiq->Next = 0;

				if( lastMiq )
				{
					lastMiq->Next = newMiq;
					for( ulong32 i = lastMiq->MesgNumber+1; i < messageNumber; ++i )
					{
						ToResend d;
						d.Index = i;
						d.Time = curTime;
						mToResend.Add( d );
					}
				}
				else
				{
					ReceivingQueueStart = newMiq;
					for( ulong32 i = NextToReceive; i < messageNumber; ++i )
					{
						ToResend d;
						d.Index = i;
						d.Time = curTime;
						mToResend.Add( d );
					}
				}
				ReceivingQueueEnd = newMiq;
			}
		}
		break;
	case MSG_HDR_RESEND:
		{
			ulong32 toResend = mesg->ReadULongOptimized();

#ifdef PKTV_CONNECTION_DEBUG
			PKTVDLog->Print( "%u: ", curTime );
			PKTVDLog->Print( "Got request to resend %u msgs:", toResend );
#endif

			MessageInQueue* miq = SendingQueueStart;

			if( !miq )
				break;

			for( ulong32 i = 0; i < toResend; ++i )
			{
				ulong32 indexToResend = mesg->ReadULongOptimized();

#ifdef PKTV_CONNECTION_DEBUG
				PKTVDLog->Print( " %u", indexToResend );
#endif

				if( miq->MesgNumber > indexToResend )
					continue;

				while( miq->MesgNumber < indexToResend )
					miq = miq->Next;

				// Assumming full mesgs in message queue
				// Resend this mesg
				GSystem->PKTV->mUDP->Send( miq->Mesg, mAddressToConnectTo, false, false );
				mLastOutgoingMessageTime = curTime;
			}

			// remove all mesgs from current pos until this index
			ulong32 firstToKeep = mesg->ReadULongOptimized();

#ifdef PKTV_CONNECTION_DEBUG
			PKTVDLog->Print( ", got until %u\n", firstToKeep );
#endif

			while( SendingQueueStart && ( SendingQueueStart->MesgNumber < firstToKeep ) )
			{
				MessageInQueue* toDel = SendingQueueStart;
				SendingQueueStart = SendingQueueStart->Next;
				delete toDel->Mesg;
				delete toDel;
			}
			if( !SendingQueueStart )
				SendingQueueEnd = 0;

		}
		break;
	default:
		GLog->Print( "Warning: Unknown transport message reseived!!! Ignoring" );
	case MSG_HDR_KEEPALIVE:
		break;
	}
}

#define HOW_MANY_TO_SEND_IN_SINGLE_PACKET 200

void PKTVTransportUDP::CheckSendResendRequest( DWORD curTime )
{
	if( curTime - mLastResendRequestTime < RESEND_REQUEST_INTERVAL )
		return;
	mLastResendRequestTime = curTime;

	// make resend request ( = info about what we have )
	DynamicArray<ulong32> toResend;

	for( int i = 0; i < mToResend.Size(); ++i )
		if( curTime - mToResend[i].Time < PACKET_RESEND_REQUEST_TIMEOUT )
		{
			toResend.Add( mToResend[i].Index );
			mToResend[i].Time = curTime;
		}

	int index = 0;

	do
	{
		int toSendNow = ( toResend.Size()-index > HOW_MANY_TO_SEND_IN_SINGLE_PACKET )?HOW_MANY_TO_SEND_IN_SINGLE_PACKET:(toResend.Size()-index);

#ifdef PKTV_CONNECTION_DEBUG
		PKTVDLog->Print( "%u: ", curTime );
		if( toSendNow )
			PKTVDLog->Print( "Sent request to resend %u msgs:", toSendNow );
		else
			PKTVDLog->Print( "Sent request to resend no msgs\n" );
#endif

		UdpMessage mesg;
		mesg.WriteUChar( PKTVManager::CMD_TRANSPORT_MESG );
		mesg.WriteUChar( MSG_HDR_RESEND );
		mesg.WriteULongOptimized( toSendNow );
		for( int i = index; i < index+toSendNow; ++i )
		{
#ifdef PKTV_CONNECTION_DEBUG
			PKTVDLog->Print( " %u", toResend[i] );
#endif
			mesg.WriteULongOptimized( toResend[i] );
		}
		mesg.WriteULongOptimized( NextToReceive );
		mesg.EndWriting();

#ifdef PKTV_CONNECTION_DEBUG
		if( toSendNow )
			PKTVDLog->Print( "\n" );
		PKTVDLog->Print( "%u: ", curTime );
		PKTVDLog->Print( "Next To Receive: %u\n", NextToReceive );
#endif

		GSystem->PKTV->mUDP->Send( &mesg, mAddressToConnectTo, false, false );

		index += HOW_MANY_TO_SEND_IN_SINGLE_PACKET;
	}
	while( toResend.Size() > index );

	mLastOutgoingMessageTime = curTime;
}

void PKTVTransportUDP::TryStepStats( void )
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

DlUl PKTVTransportUDP::GetAverage( void )
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
