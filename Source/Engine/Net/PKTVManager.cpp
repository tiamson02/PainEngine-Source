
#include "Engine.h"
#include "PKTVManager.h"
//#include "PKTVEntityProxies.h"
#include "PKTVTransportUDP.h"
#include "PKTVTransportFile.h"
#include "PKTVProxyConnection.h"
#include "UdpMessage.h"
#include "PKTVUnpackedFrame.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#define MESSAGE_SHOW_TIMEOUT 5
#define PROXY_REPORT_SEND_PERIOD 30000	// 2 times a minute should be enough for everyone ;)))

#include "Render/Video.h"
#include "Engine/PainMenu.h"

#ifdef PKTV_CONNECTION_DEBUG
LogBuffer PKTVDLog;
#endif
// ====================================================================================================
#include <stdarg.h>

void PKTV_Log( const char* fmt, ... )
{
	char buf[4096];

	va_list argPtr;
	va_start( argPtr, fmt );
	vsprintf( buf, fmt, argPtr );
	va_end( argPtr );
	GLog.Print( buf );
	GLog.Print( "\n" );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( buf );
	PKTVDLog.Print( "\n" );
#endif
	GScript.DoString( "PainMenu_PKTVErrorCallback( 0, [[%s]] )", buf );
}
// ====================================================================================================
void PKTV_Error( const char* fmt, ... )
{
	char buf[4096];
	va_list argPtr;
	va_start( argPtr, fmt );
	vsprintf( buf, fmt, argPtr );
	va_end( argPtr );
	GLog.Print( buf );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( buf );
	PKTVDLog.Print( "\n" );
#endif
	if( GEngine->PKTV && GEngine->PKTV->IsServer() )
		GScript.DoString( "PainMenu_PKTVErrorCallback( 0, [[%s]] )", buf );
	else
		GScript.DoString( "PainMenu_PKTVErrorCallback( 1, [[%s]] )", buf );
}
// ====================================================================================================

extern String GetSocketErrorString( void );	// translate WSA last error to string
extern bool Resolve( const char* hostName, IpAddress& ipAddr );

bool IpAddress2::SetName( String name )
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
		GLog.Print( s.AsChar() );
		return false;
	}

	mName = name;
	return true;
}

// =================================================================================================================
// =================================================================================================================
// =================================================================================================================
#ifdef UDP_MESSAGE_PROFILER
extern UdpMessageProfiler GUMP;
#endif

PKTVManager::PKTVManager( bool server, bool multiGame, unsigned short fps, DWORD delayMs )
:	mTimeSinceLastTick(0)
,	mDelayMs( delayMs )
,	mFps( fps )
,	mFrameMakingTime(0)
,	mLastFrame(0)
,	mLastSentFrame(0)
,	mFirstFrame(0)
,	mRecordingToFile(0)
,	mNetworkStarted( false )
,	mPlayingFromFile( 0 )
,	mServerPort( DEFAULT_PKTV_SERVER_PORT )
,	mDisplayPaused( false )
,	mProxy(0)
,	mLocalPassword("")
,	mConnectionStages( CONN_IDLE )
,	mTransport(0)
,	mTransmitPaused( true )
,	mTransmitTime( 0 )
,	mTransmitBufferTime( 10000 )
,	mSendingFrames( false )
,	mFullFrameNext( true )
,	mMaxClients( 1 )
,	mFreeSlotsLastTime( 0 )
,	mProxyReportLastSent( 0 )
,	ServerOverflows(0)
,	ClientOverflows(0)
,	JustLoadedMap( false )
,	MapLoading( false )
,	mLastDisplayedFrame(0)
,	mMaxFrameBufferTime(40000)		// ms
,	mMaxSizeTakenByFrames(40000000)	// bytes
,	mCurrentProxyTime(0)
,	mLastAdminMessage("")
/*
,	T1Monitor( "TextureName" )
,	T2Monitor( "ScaleFactor" )
,	T3Monitor( "ConstColor" )
,	T4Monitor( "TimeToDie" )
,	T5Monitor( "NeverDie" )
,	T6Monitor( "SpawnMatrix" )
,	T7Monitor( "NumElems" )
,	T8Monitor( "VertPos" )
,	T9Monitor( "VertColor" )
,	TAMonitor( "VertU" )
,	TBMonitor( "VertV" )
,	TCMonitor( "Pos" )
,	TDMonitor( "Rotation" )
,	TEMonitor( "Scale" )
,	TFMonitor( "Scale" )
*/
{
#ifdef ENTITY_CONTENTS_FLAG_MONITORING
	for( int i = 0; i < 32; ++i )
		BitTesting.Add(0);
#endif

#ifdef PKTV_CONNECTION_DEBUG
	if( !PKTVDLog.IsOpen() )
		PKTVDLog.Open( "PKTVDebug.log" );
#endif

	mWorkMode = multiGame?(server?SERVER_IDLE:CLIENT_IDLE):SINGLE_GAME;
	mLastTickTime = GEngine->SysDriver.GetCurrentTimeMS();

	if( GEngine->Net )
		GEngine->Net->GameSpyInfo.PKTVProxy = "";

	GScript.Call( "Game_DemoGetMapName", 1, "" );
	CurrentMap = GScript.GetString( -1, "" );
}

PKTVManager::~PKTVManager( void )
{
#ifdef ENTITY_CONTENTS_FLAG_MONITORING
	GLog.Print( "Content bits report\n" );
	GLog.Print( "=================================\n" );
	for( int i = 0; i < 32; ++i )
		GLog.Print( "Bit %2d true: %d times\n", i, BitTesting[i] );
#endif

	if( mWorkMode == CLIENT_CONNECTED )
		SendLeavingReport();

	DisconnectAllClients();

	// Disconnect proxy stream
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

	if( mProxy )
		delete mProxy;
	mProxy = 0;

	if( GEngine->Net )
		GEngine->Net->GameSpyInfo.PKTVProxy = "";

#ifdef UDP_MESSAGE_PROFILER
	GUMP.PrintFullStats();
	GUMP.Reset();
#endif
}

void PKTVManager::ResetProxy( void )
{
	if( mProxy )
	{
		unsigned short port = mProxy->GetLocalPort();
		delete mProxy;
		mProxy = new PKTVProxyConnection( mInterfaceToUse, port );
	}
}

void PKTVManager::DisconnectAllClients( void )
{
	// Remove client transports
	while( mClientTransports.Size() )
	{
		delete mClientTransports[0];	// this should disconnect them properly
		mClientTransports.FastRemove(0);
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
		PKTV_Error( "PKTV Init Error: WSAStartup() failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		return false;
	}

	if( ( LOBYTE(wsaData.wVersion) != 1 ) && ( HIBYTE(wsaData.wVersion) != 1 ) )
	{
		PKTV_Error( "PKTV Init Error: Windows Sockets implementation too old!\n" );
		WSACleanup();
		return false;
	}
	mNetworkStarted = true;
	return true;
}

void PKTVManager::StopNetwork( void )
{
//	Assert( mNetworkStarted );
	if( !mNetworkStarted ) return;
	WSACleanup();
	mNetworkStarted = false;
}

void PKTVManager::ProcessTransportRequest( IpAddress source )
{
	if( IsServer() ) mMaxClients = 1;

	if( mMaxClients <= mClientTransports.Size() )	// no more than max client connections
	{
		// Send error message to client (too many clients)
		PKTVTransportUDP temp( IsServer()?PKTVTransportUDP::PEER_SERVER:PKTVTransportUDP::PEER_CLIENT, source, false );
		temp.SendErrorMessage( ERR_TOO_MANY_CLIENTS );
	}
	else
	{
		// Make a new connection
		// Connection should be started in a non-authorized state and should authorize itself later.
		PKTVTransportUDP* tr = new PKTVTransportUDP( IsServer()?PKTVTransportUDP::PEER_SERVER:PKTVTransportUDP::PEER_CLIENT, source, true );
		tr->AuthenticationString = mLocalPassword;
		mClientTransports.Add( tr );
		tr->SendKeepalive();
	}
}

void PKTVManager::Tick()
{
	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();

	DWORD diff = curTime-mLastTickTime;
	if( !diff ) return;

	mLastTickTime = curTime;

	switch( mWorkMode )
	{
	default:													break;
	case SINGLE_GAME:			SingleGameTick( diff );			break;
	case SERVER_IDLE:			ServerIdleTick( diff );			break;
	case SERVER_PROXY:			ServerProxyTick( diff );		break;
	case CLIENT_IDLE:			ClientIdleTick( diff );			break;
	case CLIENT_CONNECTED:		ClientConnectedTick( diff );	break;
	case CLIENT_PLAYING_FILE:	FilePlayTick( diff );			break;
	}

	SocketTrashcan.Tick();

	// Overflows report
	if( ClientOverflows )
	{
		PKTV_Log( "Disconnected from the source because of the overflow on the line!!! Set up better connection" );
		--ClientOverflows;
	}

	if( ServerOverflows )
	{
		PKTV_Log( "Client disconnected because too much data was coming too fast. Lower your clients number or get a better connection." );
		--ServerOverflows;
	}
}

void PKTVManager::FilePlayTick( DWORD timeDiff )
{
	if( mNetworkStarted ) StopNetwork();

	if( timeDiff > 200 )
		timeDiff = 200;

	if( !mPlayingFromFile )
	{
		// Move to client idle
		RemoveAllFrames();
		StopNetwork();
		mWorkMode = SINGLE_GAME;
		return;
	}

	if( MapLoading )
		return;			// do nothing while map is loading

	if( JustLoadedMap )
	{
		DisplayFrame( timeDiff );
		return;
	}

	if( mDisplayPaused )
	{
		DisplayFrame( 0 );
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
			// If finished buffering, but not finished playing, play on
			if( mLastFrame && mDisplayTime < mLastFrame->Time )
				break;

			// Finish playing, close file, move to client_idle
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
			RemoveAllFrames();
			StopNetwork();
			mWorkMode = SINGLE_GAME;
			PKTV_Error( "File play complete" );
			return;
		}

		if( type != PKTVTransport::CHUNK_FRAME )
		{
			delete chunk;
			continue;
		}

		PKTVUnpackedFrame* frame = new PKTVUnpackedFrame( chunk, mLastFrame );

//		GLog.Print("Frame: %u, time: %u\n", frame->FrameNumber, frame->Time );

		if( !mLastFrame )
		{
			mFirstFrame = frame;
			mLastSentFrame = 0;
		}
		else
			mLastFrame->NextFrame = frame;

		delete frame->PackedFrame;	// == delete chunk;
		frame->PackedFrame = frame->MakePackedFrame( mFullFrameNext?0:mLastFrame );
		mLastFrame = frame;
		mFullFrameNext = false;
		if( mRecordingToFile )
			mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );

//		frame->MemoryTaken( true );
	}

	// Apply the new frame to the engine
	DisplayFrame( timeDiff );
}

void PKTVManager::ClientIdleTick( DWORD timeDiff )
{
	switch( mConnectionStages )
	{
	case CONN_IDLE:
		break;
	case CONN_INTENT:
		if( !mNetworkStarted )
			StartNetwork();

		// Set up an UDP socket
		if( !mProxy )
			mProxy = new PKTVProxyConnection( mInterfaceToUse ,0 );

		// Connect to proxy
		mIPsAlreadyTested = 0;
		{
			UdpMessage* mesg = new UdpMessage;
			mesg->WriteUChar( CMD_GET_CONNECTION_INFO );
			mesg->WriteUShort( mIPsAlreadyTested );
			mesg->WriteString( mLocalPassword.AsChar() );
			mesg->EndWriting();
			mProxy->Send( mesg, mProxyAddr.GetAddress(), true );
#ifdef PKTV_CONNECTION_DEBUG
			PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
			PKTVDLog.Print( "Sent GET_CONNECTION_INFO: %u, pass: %s\n", mIPsAlreadyTested, mLocalPassword.AsChar() );
#endif
		}
		mConnectionStages = CONN_SENTTOPROXY;
		break;
	case CONN_SENTTOPROXY:
		{
			// Get the IP list from proxy
			UdpMessage* mesg = 0;
			IpAddress addr;
			if( !mProxy->Receive( mesg, addr ) )	// there was some problem
			{
				ResetProxy();
				mConnectionStages = CONN_IDLE;
			}

			if( mesg )
			{
				unsigned char command = mesg->ReadUChar();
				if( command == CMD_PING )
				{
					// Answer ping
					unsigned char number = mesg->ReadUChar();
					UdpMessage* answer = new UdpMessage;
					answer->WriteUChar( CMD_PONG );
					answer->WriteUChar( number );
					answer->EndWriting();
					mProxy->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
					GLog.Print( "Ping replied with %u\n", number );
#endif
				}
				else if( addr == mProxyAddr.GetAddress() )
				{
					// From our proxy
					switch( command )
					{
					case CMD_CONNECTION_INFO:
						{
							mProxy->Answered();

							// Read connection info
							unsigned short howMany = mesg->ReadUShort();

							PKTV_Log( "Received connection info with %u addresses", howMany );

							if( !howMany )
							{
								PKTV_Error( "Proxy has no free IPs to connect to!!!" );
								mConnectionStages = CONN_IDLE;
								break;
							}

							mHostIPs.ClearArray();
							for( int i = 0; i < howMany; ++i )
							{
								IpAddress addr;
								addr.ip1 = mesg->ReadUChar();
								addr.ip2 = mesg->ReadUChar();
								addr.ip3 = mesg->ReadUChar();
								addr.ip4 = mesg->ReadUChar();
								addr.Port = mesg->ReadUShort();
								mHostIPs.Add( addr );
							}
							mConnectionStages = CONN_PINGING;
							mCurrentBatchToPing = 0;
							mPinged = GEngine->SysDriver.GetCurrentTimeMS()-HOST_PING_TIMEOUT;
						}
						break;
					case CMD_REDIRECT_TO_PROXY:
						{
							IpAddress addr;
							addr.ip1 = mesg->ReadUChar();
							addr.ip2 = mesg->ReadUChar();
							addr.ip3 = mesg->ReadUChar();
							addr.ip4 = mesg->ReadUChar();
							addr.Port = mesg->ReadUShort();
							mProxyAddr.SetAddress( addr );

#ifdef PKTV_CONNECTION_DEBUG
							PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
							PKTVDLog.Print( "Received proxy redirection to %u.%u.%u.%u:%u\n", addr.ip1, addr.ip2, addr.ip3, addr.ip4, addr.Port );
#endif
							PKTV_Log( "Redirected to another proxy..." );

							// Connect to another proxy
							mIPsAlreadyTested = 0;
							{
								UdpMessage* mesg = new UdpMessage;
								mesg->WriteUChar( CMD_GET_CONNECTION_INFO );
								mesg->WriteUShort( mIPsAlreadyTested );
								mesg->WriteString( mLocalPassword.AsChar() );
								mesg->EndWriting();
								mProxy->Send( mesg, mProxyAddr.GetAddress() );
#ifdef PKTV_CONNECTION_DEBUG
								PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
								PKTVDLog.Print( "Sent GET_CONNECTION_INFO: %u, pass: %s\n", mIPsAlreadyTested, mLocalPassword.AsChar() );
#endif
							}
						}
						break;
					case CMD_ERROR:
						{
							unsigned char type = mesg->ReadUChar();
							String desc = mesg->ReadString();
							switch( type )
							{
							case ERR_WRONG_PASSWORD:
								PKTV_Error( "Wrong password!!!\n" );
								break;
							default:
								PKTV_Error( "Unexpected or unknown error: %d, description: %s\n", type, desc.AsChar() );
								break;
							}
							// Disconnect
							mConnectionStages = CONN_IDLE;
						}
						break;
					default:					// else ignore
#ifdef PKTV_CONNECTION_DEBUG
						PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
						PKTVDLog.Print( "Received unknown command %u\n", command );
#endif
						Assert( false );
						break;
					}
				}
				// else ignore

				delete mesg;
			}
		}
		break;
	case CONN_PINGING:

		// Handle other pings
		{
			UdpMessage* mesg = 0;
			IpAddress addr;
			if( !mProxy->Receive( mesg, addr ) )	// there was some problem
			{
				ResetProxy();
				mConnectionStages = CONN_INTENT;	// try to reconnect
			}
			else if( mesg )
			{
				unsigned char command = mesg->ReadUChar();
				if( command == CMD_PING )
				{
					// Answer ping
					unsigned char number = mesg->ReadUChar();
					UdpMessage* answer = new UdpMessage;
					answer->WriteUChar( CMD_PONG );
					answer->WriteUChar( number );
					answer->EndWriting();
					mProxy->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
					PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
					PKTVDLog.Print( "Ping replied with %u\n", number );
#endif
				}
				else if( command == CMD_PONG )
				{
					// Remember ping reply
					unsigned char number = mesg->ReadUChar();

#ifdef PKTV_CONNECTION_DEBUG
					PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
					PKTVDLog.Print( "Received pong with %u\n", number );
#endif
					if( number == mCurrentBatchToPing-1 )
					{
						for( int i = 0 ; i < mHostIPs.Size(); ++i )
							if( mHostIPs[i] == addr )
							{
								mHostAddresses.AddUnique( mHostIPs[i] );
								break;
							}
					}
				}
#ifdef PKTV_CONNECTION_DEBUG
				else
				{
					PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
					PKTVDLog.Print( "Received unknown command %u\n", command );
				}
#endif
				// else ignore
				delete mesg;
			}
		}

		// Check if maybe all hosts answered already?
		if( mHostIPs.Size() == mHostAddresses.Size() )
		{
			mConnectionStages = CONN_CONNECTING;
//			mHostAddr.SetAddress( mProxy->GetTranslatedAddress( mHostAddresses[0] ) );
			mHostAddr.SetAddress( mHostAddresses[0] );
			mHostAddresses.Remove(0);

			Assert( !mTransport );
			PKTVTransportUDP* transport = new PKTVTransportUDP( PKTVTransportUDP::PEER_CLIENT, mHostAddr.GetAddress(), false );
			transport->AuthenticationString = mLocalPassword;
			if( mLastFrame ) transport->ExpectedFrame = mLastFrame->FrameNumber+1;
			transport->ConnectTo( mHostAddr.GetAddress() );
			PKTV_Log( "Connecting to stream source..." );
			mTransport = transport;
		}
		else		// Ping IPs to determine which have the best connection
		{
			DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
			if( mPinged + HOST_PING_TIMEOUT < curTime )
			{
				// Determine hosts that didn't answer ping and report them
				if( mCurrentBatchToPing && mHostIPs.Size() )
				{
					DynamicArray<IpAddress> toReport = mHostIPs;
					for( int i = 0; i < mHostAddresses.Size(); ++i )
						for( int j = 0; j < toReport.Size(); ++j )
							if( mHostAddresses[i] == toReport[j] )
							{
								toReport.FastRemove(j);
								break;
							}
					SendPingTimeoutReport( toReport );
				}

				if( mHostAddresses.Size() )
				{
					mConnectionStages = CONN_CONNECTING;
//					mHostAddr.SetAddress( mProxy->GetTranslatedAddress( mHostAddresses[0] ) );
					mHostAddr.SetAddress( mHostAddresses[0] );
					mHostAddresses.Remove(0);

					Assert( !mTransport );
					PKTVTransportUDP* transport = new PKTVTransportUDP( PKTVTransportUDP::PEER_CLIENT, mHostAddr.GetAddress(), false );
					transport->AuthenticationString = mLocalPassword;
					if( mLastFrame ) transport->ExpectedFrame = mLastFrame->FrameNumber+1;
					transport->ConnectTo( mHostAddr.GetAddress() );
					PKTV_Log( "Connecting to stream source..." );
					mTransport = transport;
				}
				else
				{
					mPinged = curTime;

					if( mHostIPs.Size() <= mCurrentBatchToPing*HOST_IPS_TO_PING )
					{
						// Get more IPs
						{
							mIPsAlreadyTested += mHostIPs.Size();
							UdpMessage* mesg = new UdpMessage;
							mesg->WriteUChar( CMD_GET_CONNECTION_INFO );
							// Add a req number here so we don't get the same IPs
							mesg->WriteUShort( mIPsAlreadyTested );
							mesg->WriteString( mLocalPassword.AsChar() );
							mesg->EndWriting();
							mProxy->Send( mesg, mProxyAddr.GetAddress() );
#ifdef PKTV_CONNECTION_DEBUG
							PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
							PKTVDLog.Print( "Sent GET_CONNECTION_INFO: %u, pass: %s\n", mIPsAlreadyTested, mLocalPassword.AsChar() );
#endif
						}
						mConnectionStages = CONN_SENTTOPROXY;
						break;
					}

					mHostAddresses.ClearArray();
					int max = (mCurrentBatchToPing+1)*HOST_IPS_TO_PING;

					if( mHostIPs.Size() < max )
						max = mHostIPs.Size();

					for( int i = 0; i < max; ++i )
					{
						UdpMessage* ping = new UdpMessage;
						ping->WriteUChar( CMD_PING );
						ping->WriteUChar( mCurrentBatchToPing );
						ping->EndWriting();
						mProxy->Send( ping, mHostIPs[mCurrentBatchToPing*HOST_IPS_TO_PING+i] );
#ifdef PKTV_CONNECTION_DEBUG
						PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
						PKTVDLog.Print( "Pinged with number %u\n", mCurrentBatchToPing );
#endif
					}

					++mCurrentBatchToPing;
				}
			}
		}

		break;
	case CONN_CONNECTING:
		// Connect to chosen IP
		Assert( mTransport );
		if( mTransport->Broken() )
		{
			delete mTransport;
			mTransport = 0;

			// Get next IP to connect to
			if( mHostAddresses.Size() )
			{
//				mHostAddr.SetAddress( mProxy->GetTranslatedAddress( mHostAddresses[0] ) );
				mHostAddr.SetAddress( mHostAddresses[0] );
				mHostAddresses.Remove(0);

				PKTVTransportUDP* transport = new PKTVTransportUDP( PKTVTransportUDP::PEER_CLIENT, mHostAddr.GetAddress(), false );
				transport->AuthenticationString = mLocalPassword;
				if( mLastFrame ) transport->ExpectedFrame = mLastFrame->FrameNumber+1;
				transport->ConnectTo( mHostAddr.GetAddress() );
				PKTV_Log( "Connecting to stream source..." );
				mTransport = transport;
			}
			else	// Get more ping data
			{
				mConnectionStages = CONN_PINGING;
				mPinged = GEngine->SysDriver.GetCurrentTimeMS()-HOST_PING_TIMEOUT;
				break;
			}
		}
		else
			mTransport->Tick();

		if( mTransport->Ready() )
		{
			// When connected, set up a listening socket and change state to connected
//			StartListeningSocket( mServerPort, mInterfaceToUse );
			mWorkMode = CLIENT_CONNECTED;
			Assert( mProxy );
		}
		else		// The transport is now negotiating connection - handle other pings
		{
			UdpMessage* mesg = 0;
			IpAddress addr;
			if( !mProxy->Receive( mesg, addr ) )	// there was some problem
				ResetProxy();
			else if( mesg )
			{
				unsigned char command = mesg->ReadUChar();
				if( command == CMD_PING )
				{
					// Answer ping
					unsigned char number = mesg->ReadUChar();
					UdpMessage* answer = new UdpMessage;
					answer->WriteUChar( CMD_PONG );
					answer->WriteUChar( number );
					answer->EndWriting();
					mProxy->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
					PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
					PKTVDLog.Print( "Ping replied with %u\n", number );
#endif
				}
				else if( command == CMD_PONG )
				{
					// Remember ping reply
					unsigned char number = mesg->ReadUChar();
#ifdef PKTV_CONNECTION_DEBUG
					PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
					PKTVDLog.Print( "Got pong with %u\n", number );
#endif
					if( number == mCurrentBatchToPing-1 )
					{
						for( int i = 0 ; i < mHostIPs.Size(); ++i )
							if( mHostIPs[i] == addr )
							{
								mHostAddresses.AddUnique( mHostIPs[i] );
								break;
							}
					}
				}
				delete mesg;
			}
		}
		break;
	default:
		Assert( false );
		break;
	}

	HandleLowerSockets();
	GiveFramesToLowerClients( timeDiff );

	RemoveOldFrames();
}

void PKTVManager::SendProxyReport( void )
{
	Assert( mProxy );
	if( !mProxy ) return;
	// Send info that you have free space to the proxy you connected to
	UdpMessage* mesg = new UdpMessage;
	mesg->WriteUChar( CMD_PROXY_REPORT );
	mesg->WriteUChar( mMaxClients-mClientTransports.Size() );
	mesg->WriteULong( mCurrentProxyTime );
	mesg->WriteULong( mLastFrame?mLastFrame->FrameNumber:0 );
	mesg->EndWriting();

	mProxy->Send( mesg, mProxyAddr.GetAddress() );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( "Sent proxy report with %u slots\n", mMaxClients-mClientTransports.Size() );
#endif
}

void PKTVManager::SendReportOnFreeSlotsChange( void )
{
	unsigned short curFreeSlots = mMaxClients-mClientTransports.Size();

	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
	if( ( curTime - mProxyReportLastSent > PROXY_REPORT_SEND_PERIOD ) || ( curFreeSlots != mFreeSlotsLastTime ) )
	{
		mProxyReportLastSent = curTime;
		mFreeSlotsLastTime = curFreeSlots;
		Assert( mProxy );
		SendProxyReport();
	}
}


void PKTVManager::SendPingTimeoutReport( DynamicArray<IpAddress>& toReport )
{
	Assert( mProxy );
	if( !mProxy || !toReport.Size() ) return;
	UdpMessage* mesg = new UdpMessage;
	mesg->WriteUChar( CMD_PING_TIMEOUT_REPORT );
	Assert( toReport.Size() < 256 );
	mesg->WriteUChar( toReport.Size() );
	for( int i = 0; i < toReport.Size(); ++i )
	{
		mesg->WriteUChar( toReport[i].ip1 );
		mesg->WriteUChar( toReport[i].ip2 );
		mesg->WriteUChar( toReport[i].ip3 );
		mesg->WriteUChar( toReport[i].ip4 );
		mesg->WriteUShort( toReport[i].Port );
	}
	mesg->EndWriting();
	mProxy->Send( mesg, mProxyAddr.GetAddress() );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( "Sent Ping Timeout Report of %d hosts\n", toReport.Size() );
#endif
}

void PKTVManager::SendLeavingReport( void )
{
	Assert( mProxy );
	if( !mProxy ) return;
	UdpMessage* mesg = new UdpMessage;
	mesg->WriteUChar( CMD_LEAVING_REPORT );
	mesg->EndWriting();
	mProxy->Send( mesg, mProxyAddr.GetAddress() );
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( "Sent Leaving report: my port: %u\n", mProxy->GetLocalPort() );
#endif
}

void PKTVManager::ClientConnectedTick( DWORD timeDiff )
{
	Assert( mNetworkStarted );

	// Lost connection on higher socket?
	mTransport->Tick();
	if( !mTransport->Connected() )
	{
		mWorkMode = CLIENT_IDLE;

		if( mTransport->GetPeerAddress() == mProxyAddr.GetAddress() )
		{
			PKTV_Error( "Broken stream connection to proxy! Proxy offline.\n" );
			mConnectionStages = CONN_IDLE;		// proxy crashed! no sense in connecting to it.
		}
		else
			mConnectionStages = CONN_INTENT;	// initiate reconnection mode

		delete mTransport;
		mTransport = 0;

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
						Assert( mLastFrame->FrameNumber+1 == mTransport->ExpectedFrame );
#endif

					PKTVUnpackedFrame* frame = new PKTVUnpackedFrame( chunk, mLastFrame );
					if( mLastFrame )
						mLastFrame->NextFrame = frame;
					else
						mFirstFrame = frame;
					mLastFrame = frame;

					mTransport->ExpectedFrame = frame->FrameNumber+1;

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
				// Interpret and execute command from the proxy
				chunk->SeekToBegin();
				if( chunk->PeekUChar() == CMD_ADMINMESG )
				{
					chunk->ReadUChar();
					mCurrentProxyTime = chunk->ReadULong();
					String am = chunk->ReadString();
					if( am != mLastAdminMessage )
					{
						mLastAdminMessage = am;
						PKTV_Log( am.AsChar() );
					}
					chunk->SeekToBegin();

					// Send it to all your clients
					for( int i = 0; i < mClientTransports.Size(); ++i )
						if( mClientTransports[i]->Ready() )
							mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_COMMAND, chunk );
				}
				break;
			case PKTVTransport::CHUNK_HEADER:
				// Read header and prepare the world
				if( !ReadAndExecuteStreamHeader( chunk ) )
				{
					PKTV_Error( "Breaking connection - different scripts version!!!\n" );
					delete mTransport;
					mTransport = 0;
					mWorkMode = CLIENT_IDLE;
					mConnectionStages = CONN_IDLE;
					return;
				}
				break;
			default:
				GLog.Print( "Unrecognized stream chunk type: %d\n", chunkType );
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}

	HandleLowerSockets();
	GiveFramesToLowerClients( timeDiff );
	Assert( mProxy );
	SendReportOnFreeSlotsChange();
	HandleUDPPings();

	// Apply the new frame to the engine
	if( !MapLoading )
		DisplayFrame( timeDiff );

	RemoveOldFrames();
}

void PKTVManager::HandleUDPPings( void )
{
	UdpMessage* mesg = 0;
	IpAddress addr;
	if( !mProxy->Receive( mesg, addr ) )	// there was some problem
		ResetProxy();
	else if( mesg )
	{
		unsigned char command = mesg->ReadUChar();
		if( command == CMD_PING )
		{
			// Answer ping
			unsigned char number = mesg->ReadUChar();
			UdpMessage* answer = new UdpMessage;
			answer->WriteUChar( CMD_PONG );
			answer->WriteUChar( number );
			answer->EndWriting();
			mProxy->Send( answer, addr );
#ifdef PKTV_CONNECTION_DEBUG
			PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
			PKTVDLog.Print( "Ping replied with %u\n", number );
#endif
		}
		delete mesg;
	}
}

void PKTVManager::HandleLowerSockets( void )
{
	unsigned char chunkType;
	UdpMessage* chunk = 0;

	// Receive and interpret all data from lower TCP/IP sockets
	for( int i = 0; i < mClientTransports.Size(); ++i )
	{
		mClientTransports[i]->Tick();

		// Lost connection on lower scoket?
		if( !mClientTransports[i]->Connected() )
		{
			delete mClientTransports[i];
			mClientTransports.FastRemove(i);
			--i;
			continue;
		}

		if( !mClientTransports[i]->Ready() )
			continue;

		while( mClientTransports[i]->ReceiveChunk( chunkType, &chunk ) )
		{
			switch( chunkType )
			{
			case PKTVTransport::CHUNK_FRAME:	// received new frame
				GLog.Print( "Received a frame from client!...discarding\n" );
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
				chunk->SeekToBegin();

				// If recording locally, write message to file
				if( mRecordingToFile )
					mRecordingToFile->SendChunk( PKTVTransport::CHUNK_MESSAGE, chunk );

				break;
			case PKTVTransport::CHUNK_COMMAND:
			case PKTVTransport::CHUNK_HEADER:
				break;
			default:
				GLog.Print( "Unrecognized stream chunk type: %d\n", chunkType );
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}
}

void PKTVManager::SingleGameTick( DWORD timeDiff )
{
	if( mNetworkStarted ) StopNetwork();

	// Make new frames ( for the recording only )
	if( !MapLoading )	// time does not fly while loading map
		MakeNewFrames( timeDiff );

	RemoveOldFrames();
}

void PKTVManager::ServerIdleTick( DWORD timeDiff )
{
	if( !mNetworkStarted ) StartNetwork();
	if( !mProxy ) mProxy = new PKTVProxyConnection( mInterfaceToUse ,DEFAULT_PKTV_PROXY_UDP_PORT );

	HandleUDPPings();	// to tick proxy socket

	if( mClientTransports.Size() )
	{
		// If there is: accept, make transport, drop listening socket, change state to connected
		mClientTransports[0]->Tick();

		if( mClientTransports[0]->Broken() )
		{
			// Broken connection to proxy; revert to idle
			delete mClientTransports[0];
			mClientTransports.FastRemove(0);
			ServerIdleTick( timeDiff );
			return;
		}

		if( mClientTransports[0]->Ready() )
		{
			if( !mClientTransports[0]->ApprovedByManager )
			{
				if( mClientTransports[0]->GetPeerType() != PKTVTransportUDP::PEER_PROXY )
				{
					// Allow only proxy to connect to us
					delete mClientTransports[0];
					mClientTransports.FastRemove(0);
					ServerIdleTick( timeDiff );
					return;
				}
				mClientTransports[0]->ApprovedByManager = true;
			}

			PKTV_Log( "PKTV Proxy connected" );

			// Write header
			UdpMessage message;
			WriteStreamHeader( &message );
			mClientTransports[0]->SendChunk( PKTVTransport::CHUNK_HEADER, &message );
			mWorkMode = SERVER_PROXY;
			if( GEngine->Net )
				GEngine->Net->GameSpyInfo.PKTVProxy = GetProxyAddress();
		}
	}

	// Make new frames ( for the recording only ) ( they're recorded in this method too )
	if( !MapLoading )	// time does not fly while loading map
		MakeNewFrames( timeDiff );

	RemoveOldFrames();
}

void PKTVManager::ServerProxyTick( DWORD timeDiff )
{
	Assert( mNetworkStarted );
	Assert( mProxy );
	Assert( mClientTransports.Size() );

	HandleUDPPings();	// to tick proxy socket

	mClientTransports[0]->Tick();
	if( !mClientTransports[0]->Connected() )	// Lost connection?
	{
		delete mClientTransports[0];
		mClientTransports.FastRemove(0);
		mWorkMode = SERVER_IDLE;
		if( GEngine->Net )
			GEngine->Net->GameSpyInfo.PKTVProxy = "";
		PKTV_Log( "Proxy disconnected" );
		return;
	}

	// Make new frames
	if( !MapLoading )
		MakeNewFrames( timeDiff );

	if( mClientTransports[0]->Ready() )
	{
		// Receive commands from the proxy
		unsigned char chunkType;
		UdpMessage* chunk = 0;
		while( mClientTransports[0]->ReceiveChunk( chunkType, &chunk ) )
		{
			switch( chunkType )
			{
			case PKTVTransport::CHUNK_FRAME:
			case PKTVTransport::CHUNK_MESSAGE:
			default:
				Assert( false );	// this is unexpected
				break;
			case PKTVTransport::CHUNK_COMMAND:
			case PKTVTransport::CHUNK_HEADER:
				break;
			}
			if( chunk )
				delete chunk;
			chunk = 0;
		}
	}

	if( mClientTransports[0]->Ready() )
	{
		// Send frames to the proxy
		PKTVUnpackedFrame* frameToSend = mLastSentFrame?mLastSentFrame->NextFrame:mFirstFrame;
		while( frameToSend && ( frameToSend->Time + mDelayMs < mFrameMakingTime ) )
		{
			Assert( frameToSend->PackedFrame );
			mClientTransports[0]->SendChunk( PKTVTransport::CHUNK_FRAME, frameToSend->PackedFrame );
			mLastSentFrame = frameToSend;
			frameToSend = frameToSend->NextFrame;
		}
	}

	RemoveOldFrames();
}

void PKTVManager::GiveFramesToLowerClients( DWORD timeDiff )
{
	// No frames = nothing to send
	if( !mFirstFrame )
		return;

	if( mTransmitPaused )
	{
		// Check if we should start it
		if( mLastFrame->Time - mTransmitTime > mTransmitBufferTime )
			mTransmitPaused = false;
	}
	else
	{
		if( mTransmitTime )
			mTransmitTime += timeDiff;
		if( mTransmitTime < mFirstFrame->Time )
			mTransmitTime = mFirstFrame->Time;

		// If there are no frames to send, pause the transmit
		if( mTransmitTime >= mLastFrame->Time )
			mTransmitPaused = true;
	}

	ulong32 earliestExpectedFrame = ULONG_MAX;
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

	while( frame && ( frame->Time < mTransmitTime ) )
	{
		// Send it (packed) to all your clients
		Assert( frame->PackedFrame );

		UdpMessage* fullFrame = 0;

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
					mClientTransports[i]->ExpectedFrame = frame->FrameNumber+1;
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

		if( fullFrame )
			delete fullFrame;

		frame = frame->NextFrame;
	}
}

void PKTVManager::DisplayFrame( DWORD timeDiff )
{
#ifdef PKTV_DISPLAY
	// Return if nothing to display or we're paused
	if( mDisplayPaused || !mFirstFrame )
		return;

//	timeDiff /= 10;

	// Fix for map loading
	if( JustLoadedMap )
	{
		timeDiff = 0;
		JustLoadedMap = false;
	}

	// Return if we're trying to display the data we don't have yet
	if( mDisplayTime >= mLastFrame->Time )
	{
		mDisplayTime = mLastFrame->Time;
		return;
	}
	mDisplayTime += timeDiff;
	if( mDisplayTime < mFirstFrame->Time )
		mDisplayTime = mFirstFrame->Time;

	// Make frames with given framerate
	PKTVUnpackedFrame* frame = mFirstFrame;
	PKTVUnpackedFrame* lastFrame = 0;
	while( frame->NextFrame && frame->NextFrame->Time < mDisplayTime )
	{
		lastFrame = frame;
		frame = frame->NextFrame;
	}

	if( frame->NextFrame )
	{
		if( frame != mLastDisplayedFrame )
		{
			frame->UpdateMap();
			if( lastFrame && ( mLastDisplayedFrame == lastFrame ) )
				lastFrame->DeleteEntitiesNonexistentInNextFrame();	// optimization; much faster
			else
				frame->DeleteEntitiesNonexistentInThisFrame();
			frame->CreateEntitiesExistingInNextFrame();
			mLastDisplayedFrame = frame;
		}
		frame->DisplayInterpolatedWith( ((float)( mDisplayTime-frame->Time ))/(frame->NextFrame->Time-frame->Time), frame->NextFrame->Time-frame->Time );
	}
	else if( lastFrame )
	{
		if( lastFrame != mLastDisplayedFrame )	// should be very rare; but who knows
		{
			lastFrame->UpdateMap();
			frame->DeleteEntitiesNonexistentInThisFrame();
			mLastDisplayedFrame = lastFrame;
		}
		lastFrame->DisplayInterpolatedWith( 1.0f, 1 );
	}
	else
		GLog.Print( "No frame to display\n" );
	// else don't do anything; only one frame is a rare case
#endif
}

void PKTVManager::AddMessage( String message )
{
	UdpMessage chunk;
	chunk.WriteString( message.AsChar() );
	chunk.EndWriting();

	if( ( mWorkMode != SERVER_IDLE ) && ( mWorkMode != SERVER_PROXY ) )
	{
		// Send it to your proxy
		if( mTransport && mTransport->Ready() )
			mTransport->SendChunk( PKTVTransport::CHUNK_MESSAGE, &chunk );

		// Send it to all your clients
		for( int i = 0; i < mClientTransports.Size(); ++i )
			if( mClientTransports[i]->Ready() )
				mClientTransports[i]->SendChunk( PKTVTransport::CHUNK_MESSAGE, &chunk );
	}

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
		// Make sure that you don't delete from under a running demo display, or a transmit
		if( mDisplayTime && ( mFirstFrame->NextFrame->Time >= mDisplayTime ) )
			return;
		if( mTransmitTime && ( mFirstFrame->Time >= mTransmitTime ) )
			return;

		PKTVUnpackedFrame* frame = mFirstFrame;
		mFirstFrame = mFirstFrame->NextFrame;
		delete frame;
	}
}

void PKTVManager::ClientConnect( const char* hostname, String password, String interfaceToUse )
{
	switch( mWorkMode )
	{
	case CLIENT_PLAYING_FILE:
		// Close playing file
		if( mPlayingFromFile )
		{
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
		}
	case SINGLE_GAME:
	case SERVER_IDLE:
	case SERVER_PROXY:

		RemoveAllFrames();
		StopNetwork();
		mWorkMode = CLIENT_IDLE;
		break;

	case CLIENT_CONNECTED:
		// Disconnect from current PKTV network
		DisconnectAllClients();
		delete mTransport;
		mTransport = 0;
		RemoveAllFrames();
		SendLeavingReport();
		mWorkMode = CLIENT_IDLE;
		break;
	case CLIENT_IDLE:
	default:
		break;
	}

	String hn( hostname );
	if( !::strstr( hostname, ":" ) )
		hn += String::Sprintf( ":%u",DEFAULT_PKTV_PROXY_UDP_PORT);

	if( !mProxyAddr.SetName( hn.AsChar() ) )
	{
		PKTV_Error( "Cannot resolve proxy address: %s\n", hostname );
		mConnectionStages = CONN_IDLE;
	}
	else
	{
		mConnectionStages = CONN_INTENT;		// express the desire to connect ;)
		if( GEngine->Video )
			GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "", "" );
	}

	mLocalPassword = password;
	mInterfaceToUse = interfaceToUse;
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
	mLastSentFrame = 0;
}

void PKTVManager::ClientPlayFile( const char* filename )
{
	switch( mWorkMode )
	{
	case CLIENT_PLAYING_FILE:
		// Close playing file
		if( mPlayingFromFile )
		{
			delete mPlayingFromFile;
			mPlayingFromFile = 0;
		}
	case SINGLE_GAME:
	case SERVER_IDLE:
	case SERVER_PROXY:
		RemoveAllFrames();
		StopNetwork();
		mWorkMode = CLIENT_IDLE;
		break;
	case CLIENT_CONNECTED:
		{
			// Disconnect from current PKTV network
			DisconnectAllClients();
			delete mTransport;
			mTransport = 0;
			RecordStop();
			RemoveAllFrames();
			SendLeavingReport();
			if( mProxy )
				delete mProxy;
			mProxy = 0;
			StopNetwork();
		}
		mWorkMode = CLIENT_IDLE;
		break;
	case CLIENT_IDLE:
	default:
		break;
	}

	mConnectionStages = CONN_IDLE;
	mPlayingFromFile = new PKTVTransportFile( filename, true );

	if( !mPlayingFromFile->Ready() )
	{
		PKTV_Error( "Incorrect demo file: %s\n", filename );
		delete mPlayingFromFile;
		mPlayingFromFile = 0;
		mWorkMode = CLIENT_IDLE;
		return;
	}

	UdpMessage* mesg = 0;
	unsigned char type;
	if( !mPlayingFromFile->ReceiveChunk( type, &mesg ) ||
		( type != PKTVTransport::CHUNK_HEADER ) ||
		!ReadAndExecuteStreamHeader( mesg ) )
	{
		PKTV_Error( "Incorrect demo file: %s\n", filename );
		delete mPlayingFromFile;
		mPlayingFromFile = 0;
		mWorkMode = CLIENT_IDLE;
	}
	else
	{
		// Read header and prepare the world
		mWorkMode = CLIENT_PLAYING_FILE;
		mFrameMakingTime = 0;
		PKTV_Log( "File play started" );
		FilePlayTick(0);	// to reload map at once, not waiting a tick
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

	if( !mFirstFrame )
		return;		// it will be saved as full frame when the next frame is made

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
	}
}

void PKTVManager::MakeNewFrames( DWORD timeDiff )
{
	TestSendingSwitch();

	if( !mSendingFrames )
		return;

	// Make frames with given framerate
	mTimeSinceLastTick += timeDiff;
	if( mTimeSinceLastTick > (DWORD)(1000/mFps) )	// ticks come less frequent than we should make frames
	{
		PKTVUnpackedFrame* frame = new PKTVUnpackedFrame;	// this will fill it

		if( mLastFrame )
		{
			frame->Time = mLastFrame->Time + mTimeSinceLastTick;
			frame->FrameNumber = mLastFrame->FrameNumber+1;
		}
		else
		{
			frame->Time = 0;
			frame->FrameNumber = 0;
			mFirstFrame = frame;
			mLastSentFrame = 0;
		}

		if( mLastFrame )
			mLastFrame->NextFrame = frame;

		mFrameMakingTime = frame->Time;
		mTimeSinceLastTick = 0;

		frame->PackedFrame = frame->MakePackedFrame( mFullFrameNext?0:mLastFrame );
		mLastFrame = frame;
		mFullFrameNext = false;
		if( mRecordingToFile )
			mRecordingToFile->SendChunk( PKTVTransport::CHUNK_FRAME, frame->PackedFrame );
	}
	else
		mFrameMakingTime = mLastFrame?mLastFrame->Time + mTimeSinceLastTick:0;
}

void PKTVManager::TestSendingSwitch( void )
{
	// Applicable only on server
	if( mWorkMode >= CLIENT_IDLE )
		return;

	// Should make new frames when recording or sending away only
	bool shouldSend = mRecordingToFile || ( mClientTransports.Size() && mClientTransports[0]->Ready() );

	// If its doing what it should, don't interfere
	if( shouldSend == mSendingFrames )
		return;

	if( mSendingFrames )	// Should stop sending and forget about the frames
		RemoveAllFrames();
	// else should start sending

	mFullFrameNext = true;
	mSendingFrames = shouldSend;
}

void PKTVManager::HandleMessage( String message )
{
	PKTV_Log( "PKTV: %s", message.AsChar() );
}

void PKTVManager::WriteStreamHeader( UdpMessage* header )
{
	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

	header->WriteString( ourVer.AsChar() );
	header->WriteString( ourNetVer.AsChar() );

	header->EndWriting();
}

bool PKTVManager::ReadAndExecuteStreamHeader( UdpMessage* header )
{
	String ver = header->ReadString();
	String netVer = header->ReadString();

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

//	if( ver == ourVer && netVer == ourNetVer )
		return true;
//	else
//	{
//		GLog.Print( "Different scripts version!!!\n" );
//		return false;
//	}
}

String PKTVManager::GetProxyAddress( void )
{
	if( mWorkMode != SERVER_PROXY )
		return "";
	return mClientTransports[0]->GetPeerAddress().AsString();
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

		GLog.Print( "Memory taken: %u bytes ( %u frames, taking %u bytes, %u packed frames, taking %u bytes )\n",
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
