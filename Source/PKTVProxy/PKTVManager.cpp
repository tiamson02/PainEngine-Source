
#include "Engine.h"
#include "PKTVManager.h"
#include "PKTVTransportUDP.h"
#include "PKTVTransportFile.h"
#include "PKTVProxyConnection.h"
#include "PKTVDatabase.h"
#include "PKTVUnpackedFrame.h"
#include "UdpMessage.h"

#include "LogBuffer.h"

#include "ProxySystem.h"
#include "SystemDriver.h"

#define PKTVDelete 0
#define PKTVSkip 1
#define PKTVCreate 2
#define PKTVUpdate 3

#define MESSAGE_SHOW_TIMEOUT 5
#define ADMIN_MESSAGE_TRANSFER_PERIOD 1000
#define PROXY_UPDATE_TRANSFER_PERIOD 10000
#define DEFAULT_NUMBER_OF_IP_SLOTS 10
#define OUR_DATA_REFRESH_PERIOD 10000

extern String GetSocketErrorString( void );	// translate WSA last error to string
extern bool Resolve( const char* hostName, IpAddress& ipAddr );

bool IpAddress2::SetName( String name, unsigned short port )
{
	if( name == mName )
		return true;
	else if( name.IsNull() )	// reset
	{
		mName = name;
		return true;
	}

	if( !Resolve( name.AsChar(), mAddress ) )
	{
		String s = String::Sprintf( "Name Resolution Error: tried to resolve host '%s' and failed!\n", name.AsChar() );
		GLog->Print( s.AsChar() );
		return false;
	}

	mName = name;
	if( mAddress.Port == 0 )
	{
		mAddress.Port = port;
		mName += String::Sprintf( ":%d", port );
	}
	return true;
}

// =================================================================================================================
// =================================================================================================================
// =================================================================================================================

PKTVManager::PKTVManager( DWORD delayMs, unsigned short udpPort, unsigned short maxClients, String interfaceToUse )
:	mTimeSinceLastTick(0)
,	mDelayMs( delayMs )
,	mFrameMakingTime(0)
,	mLastFrame(0)
,	mFirstFrame(0)
,	mRecordingToFile(0)
,	mErrorString("")
,	mNetworkStarted( false )
,	mPlayingFromFile( 0 )
,	mUDPPort( udpPort )
,	mUDP(0)
,	mWorkMode( PROXY_IDLE )
,	mConnectionStages( CONN_IDLE )
,	mTransport(0)
,	mLastSentAdminMessage(0)
,	mLastSentProxyUpdate(0)
,	mTransmitPaused( true )
,	mTransmitTime( 0 )
,	mTransmitBufferTime( 10000 )
,	mFullFrameNext( true )
,	mInterfaceToUse( interfaceToUse )
,	mMaxClients( maxClients )
,	mLastTickFreePlaces( maxClients )
,	ServerOverflows(0)
,	ClientOverflows(0)
,	mMaxFrameBufferTime(40000)		// ms
,	mMaxSizeTakenByFrames(40000000)	// bytes
,	mCurrentProxyTime(0)
,	mLastAdminMessage("")
,	mFrameRenumber(0)
,	mTimeRenumber(0)
,	mLastFrameReadTime(0)
{
	mLastTimeRefreshedOurData = mLastTickTime = GSystem->SysDriver->GetCurrentTimeMS();

	mWorkMode = PROXY_IDLE;
	StartNetwork();
	mUDP = new PKTVProxyConnection( mInterfaceToUse, mUDPPort );
	mDatabase = new PKTVDatabase( mUDP->GetLocalAddress() );

	// Input our own data
	mDatabase->ProcessUpdate( mUDP->GetLocalAddress(), mMaxClients, 0, 0 );
	mDatabase->UpdateLocalLoad();
}

PKTVManager::~PKTVManager( void )
{
	DisconnectAllClientsAndProxies();

	// Disconnect proxy-to-server stream
	if( mTransport )
	{
		delete mTransport;
		mTransport = 0;
	}

	RecordStop();

	// Close playing file
	if( mPlayingFromFile )
	{
		delete mPlayingFromFile;
		mPlayingFromFile = 0;
	}

	RemoveAllFrames();
	DeleteUDPSocket();
	StopNetwork();

	delete mDatabase;
	mDatabase = 0;
}

void PKTVManager::DeleteUDPSocket( void ) { if( mUDP ) { delete mUDP; mUDP = 0; } }

void PKTVManager::DisconnectAllClientsAndProxies( void )
{
	// Remove client transports
	while( mClientTransports.Size() )
	{
		delete mClientTransports[0];	// this should disconnect them properly
		mClientTransports.FastRemove(0);
	}

	// Remove proxy transports
	while( mProxyTransports.Size() )
	{
		delete mProxyTransports[0];	// this should disconnect them properly
		mProxyTransports.FastRemove(0);
	}
}

bool PKTVManager::StartNetwork( void )
{
	Assert( !mNetworkStarted );
	if( mNetworkStarted ) return true;

	// Init the net, if it was not initiated before
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1,1);

	if( WSAStartup( wVersionRequested, &wsaData ) )
	{
		mErrorString = String::Sprintf( "PKTV Init Error: WSAStartup() failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		GLog->Print( mErrorString.AsChar() );
		return false;
	}

	if( ( LOBYTE(wsaData.wVersion) != 1 ) && ( HIBYTE(wsaData.wVersion) != 1 ) )
	{
		mErrorString = String::Sprintf( "PKTV Init Error: Windows Sockets implementation too old!\n" );
		GLog->Print( mErrorString.AsChar() );
		WSACleanup();
		return false;
	}
	mNetworkStarted = true;
	return true;
}

void PKTVManager::StopNetwork( void )
{
	if( !mNetworkStarted ) return;
	WSACleanup();
	mNetworkStarted = false;
}

void PKTVManager::ProcessTransportRequest( IpAddress address )
{
	// Make a new connection
	// Connection should be started in a non-authorized state and should authorize itself later.
	PKTVTransportUDP* udp = new PKTVTransportUDP( PKTVTransportUDP::PEER_PROXY, address, true );
	udp->AuthenticationStringForClients = GSystem->PasswordForClients;
	udp->AuthenticationStringForProxies = GSystem->PasswordForProxies;
	mClientTransports.Add( udp );
	udp->SendKeepalive();
}

void PKTVManager::Tick( void )
{
	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();

	DWORD timeDiff = curTime-mLastTickTime;
	if( !timeDiff ) return;

	mLastTickTime = curTime;

	if( !mNetworkStarted )
		StartNetwork();

	// Periodically send load updates to higher proxy
	if( mWorkMode == PROXY_CONNECTED )
		PeriodicallySendProxyUpdate();

	if( mTransport )
		mTransport->Tick();

	// Transfers between states, connections etc.
	switch( mWorkMode )
	{
	default:													break;
	case PROXY_IDLE:			ProxyIdleTick( timeDiff );		break;
	case PROXY_CONNECTED:		ProxyConnectedTick( timeDiff );	break;
	case PROXY_PLAYING_FILE:	FilePlayTick( timeDiff );		break;
	}

	ProxyHandleUDPRequests();
	HandleLowerSockets();
	if( mWorkMode == PROXY_IDLE )
		PeriodicallyTransferAdminMessage( GSystem->IdleMessage );
	else
		GiveFramesToLowerClients( timeDiff );
	RemoveOldFrames();

	SocketTrashcan.Tick();

	// Overflows report
	if( ClientOverflows )
	{
		GLog->Print( "Disconnected from the source because of the overflow on the line!!! Set up better connection\n" );
		--ClientOverflows;
	}

	if( ServerOverflows )
	{
		GLog->Print( "Client disconnected because too much data was coming too fast. Lower your clients number or get a better connection.\n" );
		--ServerOverflows;
	}
}

void PKTVManager::FilePlayTick( DWORD timeDiff )
{
	if( !mPlayingFromFile )
	{
		// Move to client idle
//		RemoveAllFrames();
		mWorkMode = PROXY_IDLE;
		GLog->Print( "Finished playing file\n" );
		return;
	}

	// Create/buffer needed new frames from file
	mFrameMakingTime += timeDiff;
	while( !mLastFrame || ( mFrameMakingTime > mLastFrame->Time ) )
	{
		unsigned char type;
		UdpMessage* chunk;
		if( !mPlayingFromFile->ReceiveChunk( type, &chunk ) )
		{
			// If finished buffering, but not finished transmitting, play on
			if( mLastFrame && mTransmitTime < mLastFrame->Time )
				break;

			GLog->Print( "Finished playing file\n" );

			// Finish playing, close file, move to client_idle
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
//			RemoveAllFrames();
			mWorkMode = PROXY_IDLE;
			return;
		}

		if( type != PKTVTransport::CHUNK_FRAME )
		{
			delete chunk;
			continue;
		}

		// renumber mLastFrame here
		if( mLastFrame )
		{
			if( mFrameRenumber )
				mLastFrame->FrameNumber -= mFrameRenumber;
			if( mTimeRenumber )
				mLastFrame->Time -= mTimeRenumber;
		}

		PKTVUnpackedFrame* frame = new PKTVUnpackedFrame(chunk,mLastFrame);	// this will fill it

		DWORD toAdd = 0;
		if( mLastFrame && ( mLastFrame->FrameNumber+1 > frame->FrameNumber ) )
			toAdd = mLastFrame->FrameNumber+1-frame->FrameNumber;

		if( mTimeRenumber )
		{
			if( mLastFrame )
				mLastFrame->Time += mTimeRenumber;
			frame->Time += mTimeRenumber;
		}

		if( mLastFrame && ( mLastFrame->Time > frame->Time ) )
		{
			DWORD timeToAdd = mLastFrame->Time - frame->Time + 20;	// 20 ms one frame after the next
			frame->Time += timeToAdd;
			mTimeRenumber += timeToAdd;
		}

		// renumber mLastFrame here
		if( mLastFrame && mFrameRenumber )
			mLastFrame->FrameNumber += mFrameRenumber;

		if( !mLastFrame )
			mFirstFrame = frame;
		else
			mLastFrame->NextFrame = frame;

		delete frame->PackedFrame;	// == delete chunk;

		mFrameRenumber += toAdd;

		// renumber here
		if( mFrameRenumber )
			frame->FrameNumber += mFrameRenumber;

		frame->PackedFrame = frame->MakePackedFrame( mFullFrameNext?0:mLastFrame );
		mLastFrame = frame;
		mFullFrameNext = false;
		if( mRecordingToFile )
			mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );
	}
}

void PKTVManager::ProxyIdleTick( DWORD time )
{
	switch( mConnectionStages )
	{
	case CONN_IDLE:
		break;
	case CONN_INTENT:
		if( !mNetworkStarted )
			StartNetwork();

		// Connect to chosen IP
		if( !mTransport )
		{
			PKTVTransportUDP* transport = new PKTVTransportUDP( PKTVTransportUDP::PEER_PROXY, mSourceAddr.GetAddress(), false );
			transport->AuthenticationStringForProxies = GSystem->SourcePassword;
			if( mLastFrame ) transport->ExpectedFrame = mLastFrame->FrameNumber-mFrameRenumber+1;
			transport->ConnectTo( mSourceAddr.GetAddress() );
			mTransport = transport;
		}
		else if( mTransport->Broken() )
		{
			delete mTransport;
			mTransport = 0;
			GLog->Print( "Connection to the data source has been broken!\n" );
			mConnectionStages = CONN_IDLE;
			mWorkMode = PROXY_IDLE;
		}
		else if( mTransport->Ready() )
		{
			mWorkMode = PROXY_CONNECTED;
			GLog->Print( "Proxy connected\n" );

			// Send info about your free space to your source (if it is a proxy)
			if( mTransport->GetPeerType() == PKTVTransportUDP::PEER_PROXY )
				SendProxyUpdateInfo();

			// Test recording on filename
			if( GSystem->AutoRecordOnConnect && !mRecordingToFile )
				RecordStart( GSystem->AutoRecordFilename.AsChar() );
		}
		// else the transport is now negotiating connection - handle other pings
		break;
	default:
		Assert( false );
		break;
	}
}

void PKTVManager::ProxyConnectedTick( DWORD timeDiff )
{
	if( mFrameRenumber && mTransport->Ready() && ( mTransport->GetPeerType() == PKTVTransportUDP::PEER_PROXY ) )
	{
		if( mClientTransports.Size() )
		{
			GLog->Print( "WARNING: Connected as a helper proxy, but renumbering frames!!! This WILL break the PKTV protocol. Helper proxies either CAN'T have any clients before connection, or connection as helper proxy HAS to be the first transmission after startup! Disconnecting children.\n" );
			DisconnectAllClientsAndProxies();
		}
		mFrameRenumber = 0;	// make sure we don't mess up when acting as a helper proxy
	}

	// Lost connection on higher socket?
	if( !mTransport->Connected() )
	{
		if( mTransport->GetPeerType() == PKTVTransportUDP::PEER_PROXY )
			GLog->Print( "Disconnected from master proxy\n" );
		else
			GLog->Print( "Disconnected from server\n" );

		// The other side will find that out and remove proxy from its database
		delete mTransport;
		mTransport = 0;
		mWorkMode = PROXY_IDLE;
		mConnectionStages = CONN_INTENT;	// initiate reconnection mode
		return;
	}

	// Receive and interpret all data from higher TCP/IP socket
	if( mTransport->Ready() )
	{
		unsigned char chunkType;
		UdpMessage* chunk = 0;
		while( mTransport->ReceiveChunk( chunkType, &chunk ) )
		{
			switch( chunkType )
			{
			case PKTVTransport::CHUNK_FRAME:	// received new frame
				{
#ifdef _DEBUG
					if( mLastFrame )
						Assert( mLastFrame->FrameNumber-mFrameRenumber+1 == mTransport->ExpectedFrame );
#endif
					if( mLastFrame )
					{
						if( mFrameRenumber )
							mLastFrame->FrameNumber -= mFrameRenumber;
						if( mTimeRenumber )
							mLastFrame->Time -= mTimeRenumber;
					}

					PKTVUnpackedFrame* frame = new PKTVUnpackedFrame( chunk, mLastFrame );

					DWORD toAdd = 0;
					if( mLastFrame && ( mLastFrame->FrameNumber+1 > frame->FrameNumber ) )
						toAdd = mLastFrame->FrameNumber+1-frame->FrameNumber;

					if( mTimeRenumber )
					{
						if( mLastFrame )
							mLastFrame->Time += mTimeRenumber;
						frame->Time += mTimeRenumber;
					}

					if( mLastFrame && ( mLastFrame->Time > frame->Time ) )
					{
						DWORD timeToAdd = mLastFrame->Time - frame->Time + 20;	// add 20 ms between frames
						frame->Time += timeToAdd;
						mTimeRenumber += timeToAdd;
					}

					if( mLastFrame )
						mLastFrame->NextFrame = frame;
					else
						mFirstFrame = frame;
					mLastFrame = frame;

					mTransport->ExpectedFrame = frame->FrameNumber+1;

					if( mFrameRenumber && mLastFrame )
						mLastFrame->FrameNumber += mFrameRenumber;

					mFrameRenumber += toAdd;

					if( mFrameRenumber )
					{
						frame->FrameNumber += mFrameRenumber;
						delete frame->PackedFrame;
						chunk = frame->PackedFrame = frame->MakePackedFrame(mLastFrame);
					}

					// If recording locally, write frame to file
					if( mRecordingToFile )
						mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, chunk );
					chunk = 0;	// claimed for UnpackedFrame
				}
				break;
			case PKTVTransport::CHUNK_MESSAGE:
				// Send it to all your clients
				for( int i = 0; i < mClientTransports.Size(); ++i )
					if( mClientTransports[i]->Ready() )
						mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );

				// Give it to your game, if possible
				HandleMessage( chunk->ReadString() );
				chunk->SeekToBegin();

				// If recording locally, write message to file
				if( mRecordingToFile )
					mRecordingToFile->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );
			
				break;
			case PKTVTransport::CHUNK_COMMAND:
				{
					// Interpret and execute command from the proxy
					chunk->SeekToBegin();
					unsigned char command = chunk->ReadUChar();;

					switch( command )
					{
					case CMD_ADMINMESG:
						{
							mCurrentProxyTime = chunk->ReadULong();
							String str = chunk->ReadString();
							if( str != mLastAdminMessage )
							{
								mLastAdminMessage = str;
								GLog->Print( "Admin message: '%s'\n", str.AsChar() );
							}

							// Send it to all your clients
							for( int i = 0; i < mClientTransports.Size(); ++i )
								if( mClientTransports[i]->Ready() )
									mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_COMMAND, chunk );

							// Send it to all your proxies
							for( int i = 0; i < mProxyTransports.Size(); ++i )
								if( mProxyTransports[i]->Ready() )
									mProxyTransports[i]->SendChunk( PKTVTransport::CHUNK_COMMAND, chunk );
						}
						break;
					case CMD_PROXYUPDATE:
						mDatabase->UpdateProxyLoad( mTransport->GetPeerAddress(), chunk->ReadFloat() );
						break;
					default:
						Assert( false );
						break;
					}
				}
				break;
			case PKTVTransport::CHUNK_HEADER:
				// Read header and prepare the world
				ReadAndExecuteStreamHeader( chunk );
				break;
			default:
				GLog->Print( "Unrecognized stream chunk type: %d\n", chunkType );
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}
}

void PKTVManager::SendProxyUpdateInfo( void )
{
	Assert( mTransport && mTransport->Ready() );
	if( !mTransport || !mTransport->Ready() || ( mTransport->GetPeerType() != PKTVTransportUDP::PEER_PROXY ) ) return;

	UdpMessage chunk;
	chunk.WriteUChar( CMD_PROXYUPDATE );
	chunk.WriteFloat( mDatabase->SummarizeProxyLoad() );
	chunk.EndWriting();

	// Send it to your proxy
	if( mTransport && mTransport->Ready() )
		mTransport->SendChunk( PKTVTransport::CHUNK_COMMAND, &chunk );
}

void PKTVManager::ProxyHandleUDPRequests( void )
{
	if( !mUDP )
		return;

	UdpMessage* mesg = 0;
	IpAddress addr;

	while( true )
	{

		if( !mUDP->Receive( mesg, addr ) )	// there was some problem
		{
			GLog->Print( "Warning: proxy UDP socket closed!!! This is a bug! Trying to avoid it.\n" );
			SetUDPPort( mUDPPort );			// try to recreate socket on the same port
			return;
		}
		else if( !mesg )
			return;	// nothing to handle

		unsigned char command = mesg->ReadUChar();
		switch( command )
		{
		case CMD_GET_CONNECTION_INFO:
			{
				unsigned short fromIP = mesg->ReadUShort();
				String passwd = mesg->ReadString();
				if( passwd != GSystem->PasswordForClients )
					SendErrorMessage( addr, ERR_WRONG_PASSWORD );
				else
				{
					IpAddress newAddress;
					if( !GSystem->NoProxyDispatch && mDatabase->RedirectNewClient( newAddress ) )
					{
						// Redirect to another proxy
						UdpMessage* answer = new UdpMessage;
						answer->WriteUChar( CMD_REDIRECT_TO_PROXY );
						answer->WriteUChar( newAddress.ip1 );
						answer->WriteUChar( newAddress.ip2 );
						answer->WriteUChar( newAddress.ip3 );
						answer->WriteUChar( newAddress.ip4 );
						answer->WriteUShort( newAddress.Port );
						answer->EndWriting();
						mUDP->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
						PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
						PKTVDLog->Print( "Sent redirection to: %u.%u.%u.%u:%u\n", newAddress.ip1, newAddress.ip2, newAddress.ip3, newAddress.ip4, newAddress.Port );
#endif
					}
					else
					{
						// Or give IP list in CMD_CONNECTION_INFO message
						// Respect req number

						DynamicArray<IpAddress> slots;

						if( !GSystem->NoClientDispatch )	// can dispatch to clients
							mDatabase->GetListOfRandomSlots( DEFAULT_NUMBER_OF_IP_SLOTS, slots, fromIP, addr );
						else if( mLastTickFreePlaces )		// have free slots here
							slots.Add( mUDP->GetLocalAddress() );
						// else you just can't connect

						// Write connection info
						UdpMessage* answer = new UdpMessage;
						answer->WriteUChar( CMD_CONNECTION_INFO );
						answer->WriteUShort( slots.Size() );
						for( int i = 0; i < slots.Size(); ++i )
						{
							answer->WriteUChar( slots[i].ip1 );
							answer->WriteUChar( slots[i].ip2 );
							answer->WriteUChar( slots[i].ip3 );
							answer->WriteUChar( slots[i].ip4 );
							answer->WriteUShort( slots[i].Port );
						}
						answer->EndWriting();
						mUDP->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
						PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
						PKTVDLog->Print( "Sent connection info with %u IPs to %s\n", slots.Size(), addr.AsString().AsChar() );
#endif
					}
				}
			}
			break;
		case CMD_PING:
			{
				// Answer ping
				unsigned char number = mesg->ReadUChar();
				UdpMessage* answer = new UdpMessage;
				answer->WriteUChar( CMD_PONG );
				answer->WriteUChar( number );
				answer->EndWriting();
				mUDP->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
				PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
				PKTVDLog->Print( "Ping replied with %u to %s\n", number, addr.AsString().AsChar() );
#endif
			}
			break;
		case CMD_PROXY_REPORT:
			{
				unsigned char maxClients = mesg->ReadUChar();
				DWORD givenProxyTime = mesg->ReadULong();
				ulong32 givenLastFrame = mesg->ReadULong();

#ifdef PKTV_CONNECTION_DEBUG
				PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
				PKTVDLog->Print( "Got free slots report from %s with %u free places\n", addr.AsString().AsChar(), maxClients );
#endif

				// Add this info to your database
				DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
				mDatabase->ProcessUpdate( addr, maxClients, curTime-givenProxyTime, givenLastFrame-(mLastFrame?mLastFrame->FrameNumber:0) );
			}
			break;
		case CMD_PING_TIMEOUT_REPORT:
			{
				IpAddress t;
				unsigned char howMany = mesg->ReadUChar();
				for( int i = 0; i < howMany; ++i )
				{
					t.ip1 = mesg->ReadUChar();
					t.ip2 = mesg->ReadUChar();
					t.ip3 = mesg->ReadUChar();
					t.ip4 = mesg->ReadUChar();
					t.Port = mesg->ReadUShort();
					mDatabase->ProcessBan( t );
				}
			}
			break;
		case CMD_LEAVING_REPORT:
			{
#ifdef PKTV_CONNECTION_DEBUG
				PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
				PKTVDLog->Print( "Got leaving report from %s\n", addr.AsString().AsChar() );
#endif
				// Add this info to your database
				mDatabase->ProcessLeaving( addr );
			}
			break;
		default:
			break;
		}

		delete mesg;
		mesg = 0;
	}
}

void PKTVManager::HandleLowerSockets( void )
{
	unsigned char chunkType;
	UdpMessage* chunk = 0;

	// Receive and interpret all data from lower TCP/IP client sockets
	for( int i = 0; i < mClientTransports.Size(); ++i )
	{
		mClientTransports[i]->Tick();

		// Lost connection on lower scoket?
		if( !mClientTransports[i]->Connected() )
		{
			if( mClientTransports[i]->GetPeerType() == PKTVTransportUDP::PEER_PROXY )
				mDatabase->RemoveProxy( mClientTransports[i]->GetPeerAddress() );
			else
				mDatabase->ProcessLeaving( mClientTransports[i]->GetPeerAddress() );
			delete mClientTransports[i];
			mClientTransports.FastRemove(i);
			--i;
			continue;
		}

		if( !mClientTransports[i]->Ready() )
			continue;

		if( !mClientTransports[i]->ApprovedByManager )
		{
			// Approve or disapprove
			if( mClientTransports[i]->GetPeerType() == PKTVTransportUDP::PEER_PROXY )
			{
				// This is a proxy client; move it to proxies
				mClientTransports[i]->ApprovedByManager = true;
				mProxyTransports.Add( mClientTransports[i] );
				mClientTransports.FastRemove(i);
				--i;
				continue;
			}
			else if( mClientTransports[i]->GetPeerType() != PKTVTransportUDP::PEER_CLIENT )
			{
				// This is an unknown client; disconnect
				delete mClientTransports[i];
				mClientTransports.FastRemove(i);
				--i;
				continue;
			}
			else if( ((ulong32)mClientTransports.Size()) > mMaxClients )
			{
				// Too many clients; disconnect
				// Send error message so client knows what's going on 
				mClientTransports[i]->SendErrorMessage( ERR_TOO_MANY_CLIENTS );
				delete mClientTransports[i];
				mClientTransports.FastRemove(i);
				--i;
				continue;
			}
			else
				mClientTransports[i]->ApprovedByManager = true;
		}

		while( mClientTransports[i]->ReceiveChunk( chunkType, &chunk ) )
		{
			switch( chunkType )
			{
			case PKTVTransport::CHUNK_FRAME:	// received new frame
				GLog->Print( "Received a frame from client!!!! Serious error!\n" );
				break;
			case PKTVTransport::CHUNK_MESSAGE:
				// Send it to all your clients except the one who gave it to you
				for( int j = 0; j < mClientTransports.Size(); ++j )
					if( ( j != i ) && mClientTransports[i]->Ready() )
						mClientTransports[j]->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );

				// Send it to your proxy
				if( mTransport && mTransport->Ready() )
					mTransport->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );

				// Give it to your game, if possible
				HandleMessage( chunk->ReadString() );

				// If recording locally, write message to file
				if( mRecordingToFile )
					mRecordingToFile->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );

				break;
			case PKTVTransport::CHUNK_COMMAND:
			case PKTVTransport::CHUNK_HEADER:
				break;
			default:
				GLog->Print( "Unrecognized stream chunk type: %d\n", chunkType );
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}

	// Update proxy
	unsigned short newFreePlaces = mMaxClients;
	for( int i = 0; i < mClientTransports.Size(); ++i )
		if( mClientTransports[i]->ApprovedByManager )
		{
			--newFreePlaces;
			if( !newFreePlaces )
				break;
		}

	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
	if( curTime-mLastTimeRefreshedOurData > OUR_DATA_REFRESH_PERIOD )
	{
		mLastTimeRefreshedOurData = curTime;
		mDatabase->ProcessUpdate( mUDP->GetLocalAddress(), mMaxClients, 0, 0 );	// don't let this get old
	}

	if( newFreePlaces != mLastTickFreePlaces )
	{
		// Input our own data
		mLastTickFreePlaces = newFreePlaces;
		mLastTimeRefreshedOurData = curTime;
		mDatabase->ProcessUpdate( mUDP->GetLocalAddress(), mMaxClients, 0, 0 );
		mDatabase->UpdateLocalLoad();
	}

	// Receive and interpret all data from lower TCP/IP proxy sockets
	for( int i = 0; i < mProxyTransports.Size(); ++i )
	{
		mProxyTransports[i]->Tick();

		// Lost connection on lower scoket?
		if( !mProxyTransports[i]->Connected() )
		{
			mDatabase->RemoveProxy( mProxyTransports[i]->GetPeerAddress() );
			delete mProxyTransports[i];
			mProxyTransports.FastRemove(i);
			--i;
			continue;
		}

		Assert( mProxyTransports[i]->Ready() );
		Assert( mProxyTransports[i]->ApprovedByManager );

		while( mProxyTransports[i]->ReceiveChunk( chunkType, &chunk ) )
		{
			switch( chunkType )
			{
			case PKTVTransport::CHUNK_FRAME:	// received new frame
				GLog->Print( "Received a frame from lower proxy!!!! Serious error!\n" );
				break;
			case PKTVTransport::CHUNK_MESSAGE:
				GLog->Print( "Received a message from lower proxy!!!! Serious error!\n" );
				break;
			case PKTVTransport::CHUNK_COMMAND:
				break;
			case PKTVTransport::CHUNK_HEADER:
				GLog->Print( "Received a header from lower proxy!!!! Serious error!\n" );
				break;
			default:
				GLog->Print( "Unrecognized proxy stream chunk type: %d\n", chunkType );
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}
}

void PKTVManager::GiveFramesToLowerClients( DWORD timeDiff )
{
	// No frames = nothing to send
	if( !mFirstFrame )
		return;

	if( mWorkMode == PROXY_PLAYING_FILE )
		mTransmitPaused = false;

	if( mTransmitPaused )
	{
		// Check if we should start it
		if( mLastFrame->Time - mTransmitTime > mTransmitBufferTime+GSystem->BroadcastDelayMS )
			mTransmitPaused = false;
	}
	else
	{
		if( mTransmitTime < mFirstFrame->Time )
			mTransmitTime = mFirstFrame->Time;
		else
			mTransmitTime += timeDiff;

		// If there are no frames to send, or the frames to send have to be delayed, pause the transmit
		if( mTransmitTime >= mLastFrame->Time+GSystem->BroadcastDelayMS )
			mTransmitPaused = true;
	}

	ulong32 earliestExpectedFrame = ULONG_MAX;
	for( int i = 0; i < mProxyTransports.Size(); ++i )
		if( mProxyTransports[i]->Ready() )
		{
			if( !mProxyTransports[i]->ExpectedFrame )	// just connected, have no idea what frame to get
			{
				mProxyTransports[i]->ExpectedFrame = (mLastFrame->FrameNumber>=10)?(mLastFrame->FrameNumber-10):0;
				mProxyTransports[i]->GetFullFrame = true;
			}

			if( mProxyTransports[i]->ExpectedFrame < earliestExpectedFrame )
				earliestExpectedFrame = mProxyTransports[i]->ExpectedFrame;
		}
	for( int i = 0; i < mClientTransports.Size(); ++i )
		if( mClientTransports[i]->Ready() )
		{
			if( !mClientTransports[i]->ExpectedFrame )	// just connected, have no idea what frame to get
			{
				mClientTransports[i]->ExpectedFrame = (mLastFrame->FrameNumber>=10)?(mLastFrame->FrameNumber-10):0;
				mClientTransports[i]->GetFullFrame = true;
			}

			if( mClientTransports[i]->ExpectedFrame < earliestExpectedFrame )
				earliestExpectedFrame = mClientTransports[i]->ExpectedFrame;
		}

	// Send the frames you should send
	PKTVUnpackedFrame* frame = mFirstFrame;
	while( frame && frame->FrameNumber < earliestExpectedFrame )
		frame = frame->NextFrame;

	while( frame )
	{
		// Send it (packed) to all your clients
		Assert( frame->PackedFrame );

		UdpMessage* fullFrame = 0;

		for( int i = 0; i < mProxyTransports.Size(); ++i )
			if( mProxyTransports[i]->Ready() )
			{
				if( mProxyTransports[i]->ExpectedFrame > frame->FrameNumber )
					continue;
				else if( mProxyTransports[i]->ExpectedFrame < frame->FrameNumber )
				{
					if( !fullFrame )	// Make it
						fullFrame = frame->MakePackedFrame(0);
					mProxyTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, fullFrame );
					mProxyTransports[i]->ExpectedFrame = mFirstFrame->FrameNumber+1;
				}
				else if( mProxyTransports[i]->GetFullFrame )
				{
					mProxyTransports[i]->GetFullFrame = false;
					if( !fullFrame )	// Make it
						fullFrame = frame->MakePackedFrame(0);
					mProxyTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, fullFrame );
				}
				else
					mProxyTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );
				++mProxyTransports[i]->ExpectedFrame;
			}

		if( frame->Time < mTransmitTime )
		{
			for( int i = 0; i < mClientTransports.Size(); ++i )
				if( mClientTransports[i]->Ready() )
				{
					if( mClientTransports[i]->ExpectedFrame > frame->FrameNumber )
						continue;
					else if( mClientTransports[i]->ExpectedFrame < frame->FrameNumber )
					{
						if( !fullFrame )	// Make it
							fullFrame = frame->MakePackedFrame(0);
						mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, fullFrame );
						mClientTransports[i]->ExpectedFrame = mFirstFrame->FrameNumber+1;
					}
					else if( mClientTransports[i]->GetFullFrame )
					{
						mClientTransports[i]->GetFullFrame = false;
						if( !fullFrame )	// Make it
							fullFrame = frame->MakePackedFrame(0);
						mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, fullFrame );
					}
					else
						mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );
					++mClientTransports[i]->ExpectedFrame;
				}
		}

		if( fullFrame )
			delete fullFrame;
		frame = frame->NextFrame;
	}

}

void PKTVManager::AddMessage( String message )
{
	UdpMessage chunk;
	chunk.WriteString( message.AsChar() );
	chunk.EndWriting();

	// Send it to all your clients
	for( int i = 0; i < mClientTransports.Size(); ++i )
		if( mClientTransports[i]->Ready() )
			mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_MESSAGE, &chunk );

	// If writing a demo, add it to file
	if( mRecordingToFile && mRecordingToFile->Ready() )
		mRecordingToFile->SendChunk( PKTVTransport::CHUNK_MESSAGE, &chunk );
}

void PKTVManager::RemoveOldFrames( void )
{
	if( !mFirstFrame || !mFirstFrame->NextFrame )
		return;

	while( mLastFrame->Time - mFirstFrame->Time > mMaxFrameBufferTime )
	{
		PKTVUnpackedFrame* frame = mFirstFrame;
		mFirstFrame = mFirstFrame->NextFrame;
		delete frame;
	}
}

/*
void PKTVManager::RemoveOldFrames( void )
{
	if( !mFirstFrame || !mFirstFrame->NextFrame )
		return;

	ulong32 sizeTaken = MemoryTaken();

	while( sizeTaken > mMaxSizeTakenByFrames )
	{
		PKTVUnpackedFrame* frame = mFirstFrame;
		mFirstFrame = mFirstFrame->NextFrame;

		sizeTaken -= frame->MemoryTaken(false);
		if( frame->PackedFrame )
			sizeTaken -= sizeof(frame->PackedFrame)+ frame->PackedFrame->GetSize();

		delete frame;
	}
}
*/

void PKTVManager::Connect( const char* hostname, String password )
{
	switch( mWorkMode )
	{
	case PROXY_PLAYING_FILE:
		// Close file, move to client idle

		// Close playing file
		if( mPlayingFromFile )
		{
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
		}

//		RemoveAllFrames();
		mWorkMode = PROXY_IDLE;
		break;
	case PROXY_CONNECTED:
		{
			// Disconnect from current PKTV network
//			DisconnectAllClientsAndProxies();
			delete mTransport;
			mTransport = 0;
//			RemoveAllFrames();
		}
		mWorkMode = PROXY_IDLE;
		break;
	case PROXY_IDLE:
	default:
		break;
	}

	if( !mSourceAddr.SetName( hostname, DEFAULT_LISTENING_UDP_PORT ) )
	{
		GLog->Print( "Cannot resolve source address: %s\n", hostname );
		mConnectionStages = CONN_IDLE;
	}
	else
		mConnectionStages = CONN_INTENT;		// express the desire to connect ;)

	mSourcePassword = password;
}

void PKTVManager::RemoveAllFrames( void )
{
	// Remove frames
	while( mFirstFrame )
	{
		PKTVUnpackedFrame* frame = mFirstFrame;
		mFirstFrame = frame->NextFrame;
		delete frame;
	}
	mLastFrame = 0;
}

void PKTVManager::PlayFile( const char* filename )
{
	switch( mWorkMode )
	{
	case PROXY_PLAYING_FILE:
		// Close file, move to client idle

		// Close playing file
		if( mPlayingFromFile )
		{
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
		}

//		RemoveAllFrames();
		mWorkMode = PROXY_IDLE;
		break;
	case PROXY_CONNECTED:
		{
			// Disconnect from current PKTV network
//			DisconnectAllClientsAndProxies();
			delete mTransport;
			mTransport = 0;
//			RecordStop();
//			RemoveAllFrames();
		}
		mWorkMode = PROXY_IDLE;
		break;
	case PROXY_IDLE:
	default:
		break;
	}

	mConnectionStages = CONN_IDLE;
	mPlayingFromFile = new PKTVTransportFile( filename, true );

	if( !mPlayingFromFile->Ready() )
	{
		String filenameWithEnding( filename );
		filenameWithEnding += ".pktv";

		delete mPlayingFromFile;
		mPlayingFromFile = new PKTVTransportFile( filenameWithEnding, true );

		if( !mPlayingFromFile->Ready() )
		{
			GLog->Print( "Incorrect demo file: %s\n", filename );
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
			mWorkMode = PROXY_IDLE;
			return;
		}
	}

	UdpMessage* mesg = 0;
	unsigned char type;
	if( !mPlayingFromFile->ReceiveChunk( type, &mesg ) || ( type != PKTVTransport::CHUNK_HEADER ) )
	{
		GLog->Print( "Incorrect demo file: %s\n", filename );
		delete mPlayingFromFile;
		mPlayingFromFile = 0;
		mWorkMode = PROXY_IDLE;
	}
	else
	{
		// Read header and prepare the world
		ReadAndExecuteStreamHeader( mesg );
		mWorkMode = PROXY_PLAYING_FILE;
		GLog->Print( "Started playing file\n" );
	}
	if( mesg )
		delete mesg;
}

void PKTVManager::RecordStart( const char* filename )
{
	if( mRecordingToFile )
		RecordStop();
	mRecordingToFile = new PKTVTransportFile( filename, false );

	// Write header
	UdpMessage message;
	WriteStreamHeader( &message );
	mRecordingToFile->SendChunk( PKTVTransport::CHUNK_HEADER, &message );

	// Write all frames we have
	GLog->Print( "Recording to file '%s'\n", filename );

	if( !mFirstFrame )
		return;		// it will be saved as full frame when the next frame is made

	// Make sure it is a full frame
	UdpMessage* fullFrame = mFirstFrame->MakePackedFrame(0);
	mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, fullFrame );
	delete fullFrame;

	// Save all frames we have ( catch-up )
	for( PKTVUnpackedFrame* frame = mFirstFrame->NextFrame; frame; frame = frame->NextFrame )
		mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );
}

void PKTVManager::RecordStop( void )
{
	if( mRecordingToFile )
	{
		delete mRecordingToFile;
		mRecordingToFile = 0;
		GLog->Print( "Stopped recording\n" );
	}
}

void PKTVManager::HandleMessage( String message )
{
	if( GSystem->DisplayViewersMesgs )
	{
		String t( "*Message*: " );
		t += message;
		t += "\n";
		GLog->Print( t.AsChar() );
	}
}

void PKTVManager::WriteStreamHeader( UdpMessage* header )
{
	// TO DO - write all needed data to the message. Just write, the message is ready for that
	header->WriteUChar(0);	// dummy byte

	header->EndWriting();
}

void PKTVManager::ReadAndExecuteStreamHeader( UdpMessage* header )
{
	// TO DO - read all needed data from the message. Just read, the message is ready for that

	header->ReadUChar();	// dummy byte

#ifdef PKTV_DISPLAY
	// TO DO - use this data to set engine to correct state
#endif
}

void PKTVManager::PeriodicallyTransferAdminMessage( String message )
{
	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
	if( curTime-mLastSentAdminMessage > ADMIN_MESSAGE_TRANSFER_PERIOD )
	{
		mLastSentAdminMessage = curTime;

		// Make admin message
		UdpMessage command;
		command.WriteUChar( CMD_ADMINMESG );
		command.WriteULong( curTime );		// this can safely be local time, because it only gets called when proxy is in idle state
		command.WriteString( message.AsChar() );
		command.EndWriting();

		// Send it to all your clients and proxies
		for( int i = 0; i < mClientTransports.Size(); ++i )
			if( mClientTransports[i]->Ready() )
				mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_COMMAND, &command );

		for( int i = 0; i < mProxyTransports.Size(); ++i )
			if( mProxyTransports[i]->Ready() )
				mProxyTransports[i]->SendChunk( PKTVTransport::CHUNK_COMMAND, &command );
	}
}

void PKTVManager::SendErrorMessage( const IpAddress& addr, unsigned char errorType, String additionalInfo )
{
	UdpMessage* error = new UdpMessage;
	error->WriteUChar( CMD_ERROR );
	error->WriteUChar( errorType );
	error->WriteString( additionalInfo.AsChar() );
	error->EndWriting();
	mUDP->Send( error, addr );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Sent error %u (%s)\n", errorType, additionalInfo.AsChar() );
#endif
}

void PKTVManager::PeriodicallySendProxyUpdate( void )
{
	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();
	if( curTime-mLastSentProxyUpdate > PROXY_UPDATE_TRANSFER_PERIOD )
	{
		mLastSentProxyUpdate = curTime;
		SendProxyUpdateInfo();
	}
}

void PKTVManager::SetUDPPort( unsigned short port )
{
	if( mUDPPort == port )
		return;
	mUDPPort = port;

	if( mUDP )
	{
		DeleteUDPSocket();
		mUDP = new PKTVProxyConnection( mInterfaceToUse, mUDPPort );
	}
}

DlUl PKTVManager::GetOverallUDPStats( void )
{
	if( mUDP )
		return mUDP->GetOverallStats();
	else
	{
		DlUl t;
		return t;
	}
}

DlUl PKTVManager::GetLatestUDPStats( void )
{
	if( mUDP )
		return mUDP->GetAverage();
	else
	{
		DlUl t;
		return t;
	}
}

DlUl PKTVManager::GetOverallClientTCPStats( void )
{
	DlUl sum;
	for( int i = 0; i < mClientTransports.Size(); ++i )
		sum += mClientTransports[i]->GetOverallStats();
	return sum;
}

DlUl PKTVManager::GetLatestClientTCPStats( void )
{
	DlUl sum;
	if( !mClientTransports.Size() )
		return sum;

	for( int i = 0; i < mClientTransports.Size(); ++i )
		sum += mClientTransports[i]->GetAverage();
	sum /= mClientTransports.Size();
	return sum;
}

DlUl PKTVManager::GetOverallProxyTCPStats( void )
{
	DlUl sum;
	for( int i = 0; i < mProxyTransports.Size(); ++i )
		sum += mProxyTransports[i]->GetOverallStats();
	return sum;
}

DlUl PKTVManager::GetLatestProxyTCPStats( void )
{
	DlUl sum;
	if( !mProxyTransports.Size() )
		return sum;

	for( int i = 0; i < mProxyTransports.Size(); ++i )
		sum += mProxyTransports[i]->GetAverage();
	sum /= mProxyTransports.Size();
	return sum;
}

DlUl PKTVManager::GetOverallSourceTCPStats( void )
{
	return mTransport->GetOverallStats();
}

DlUl PKTVManager::GetLatestSourceTCPStats( void )
{
	return mTransport->GetAverage();
}

void PKTVManager::ListDB( void )
{
	mDatabase->ListDB();
}

ulong32 PKTVManager::MemoryTaken( bool report )
{
	if( report )
	{
		ulong32 frames = 0;
		ulong32 framesNum = 0;
		ulong32 packedFrames = 0;
		ulong32 packedFramesNum = 0;

		PKTVUnpackedFrame* frame = mFirstFrame;
		while( frame )
		{
			frames += frame->MemoryTaken(false);
			++framesNum;
			if( frame->PackedFrame )
			{
				packedFrames += sizeof(frame->PackedFrame);
				packedFrames += frame->PackedFrame->GetSize();
				++packedFramesNum;
			}
			frame = frame->NextFrame;
		}

		GLog->Print( "Memory taken: %u bytes ( %u frames, taking %u bytes, %u packed frames, taking %u bytes )\n",
			frames+packedFrames, framesNum, frames, packedFramesNum, packedFrames );
		return frames+packedFrames;
	}
	else
	{
		ulong32 taken = 0;

		PKTVUnpackedFrame* frame = mFirstFrame;
		while( frame )
		{
			taken += frame->MemoryTaken(false);
			if( frame->PackedFrame )
				taken += sizeof(frame->PackedFrame) + frame->PackedFrame->GetSize();
			frame = frame->NextFrame;
		}

		return taken;
	}
}
