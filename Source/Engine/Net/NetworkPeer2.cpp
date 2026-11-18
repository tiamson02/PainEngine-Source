
#include "Engine.h"

#include "Net/GameSpyData.h"
#include "Net/NetworkPeer2.h"
#include "Net/ObjectSynchronizer2.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Engine/World.h"
#include "Engine/BaseGame.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsWorld.h"

#ifndef TRANSGAMING_NO_HAVOK
// We currently don't have PSP Havok support.
#include "Physics/Havok/HavokCharacterController.h"
#include "Physics/Havok/HavokMissileController.h"
#endif
#include "Render/Video.h"
#include "Engine/PainMenu.h"

#ifdef _DEBUG
#define STREAMING_TIMEOUT_TIME 300000
#define STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD 1200000
#define STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD 600000
#else
#define STREAMING_TIMEOUT_TIME 60000
#define STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD 120000
#define STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD 60000
#endif

#define MAX_TIME_BETWEEN_PACKETS 300

#define MIN_PLAYER_ACTIONS_PASSED 1
#define MAX_PLAYER_ACTIONS_PASSED 50

// 4 bytes are added here just in case, to avoid 1400-byte packets. Some routers cut them, I heard.
#ifdef CRC_IN_PACKETS
#define MAX_PACKET_HEADER_BYTES (16+4+1)
#else
#define MAX_PACKET_HEADER_BYTES (8+4+1)
#endif

#ifdef CDKEY_ENABLED
#ifndef	TRANSGAMING_NO_GAMESPY
// Inclusion of GameSpy CDKey SDK
#include "Net/GameSpy2/gcdkey/gcdkeyc.h"
#include "Net/GameSpy2/gcdkey/gcdkeys.h"
#endif
#endif

//#define PEER_LOGGING
#define CONNECTION_STATES_LOGGING

//#define PLAYER_MOVEMENT_TRANSPORT_TESTING
#ifdef PLAYER_MOVEMENT_TRANSPORT_TESTING
LogBuffer PMTTLog;
#endif

NetworkPeer2::NetworkPeer2( NetworkDevice2* device, IpAddress* address )
:	mIncoming(0)
,	mAck(0)
,	mLastReliable(0)
,	mSent(1)
,	mReliableAck(false)
,	mReliableIncoming(false)
,	mReliableSent(true)
,	mStreamingTimeout(STREAMING_TIMEOUT_TIME)
,	mLastReliableMessageToAck(0)
,	mAddress(*address)
,	mDisconnected(false)
,	mSpectator(false)
,	mMyIndex(SERVER_ID)
,	mGameSpyCDKeyIndex(-1)
,	mNet(device)
,	mConnectionState( EStateDisconnected )
,	mLastFrameTime( 0 )
,	mServerFPS( 25 )
,	mMaxFrameNumber( 1 )
,	mHighestReliableReceivedOnClient( 0 )
,	mPing( 255 )
,	mFrames( 0 )
,	mReliableMessagesMaxNumber(1)
,	mStatsCursor( 0 )
,	mDFCCreate(0)
,	mDFCUpdate(0)
,	mDFCDelete(0)
,	mDFCReliable(0)
,	mDFCUnreliable(0)
,	mUFCPlayerDeltas(0)
,	mUFCReliable(0)
,	mUFCUnreliable(0)
,	mMinSendFrameSize(0)
,	LastFrameLatency(0)
{
	DWORD curTime = GetCurrentTime();
	mLastReceivedTime = mStatsData.mTimeOfCreation = mStatsDataTemp.mTimeOfCreation = curTime;
	mLastSendTime = mLastReceivedTime - MAX_TIME_BETWEEN_PACKETS/2;

	// Init stats structures
	PeerStats zeroStat( curTime );
	for( int i = 0; i < mNet->mStatsNumberToAvg; ++i )
		mStatsChunks.Add( zeroStat );

	ScriptObject Config = GScript.Globals()["Cfg"];
	mServerFPS = Config["NetcodeServerFramerate"].GetInt();
	mLocalPlayerOptimizedEveryNFrames = Config["NetcodeLocalPlayerSynchroEveryNFrames"].GetInt();

	mMaxPlayerActionsPassed = Config["NetcodeMaxPlayerActionsPassed"].GetInt();
	if( mMaxPlayerActionsPassed < MIN_PLAYER_ACTIONS_PASSED )
		mMaxPlayerActionsPassed = MIN_PLAYER_ACTIONS_PASSED;
	else if( mMaxPlayerActionsPassed > MAX_PLAYER_ACTIONS_PASSED )
		mMaxPlayerActionsPassed = MAX_PLAYER_ACTIONS_PASSED;

	mMinSendFrameSize = Config["NetcodeMinUpstreamFrameSize"].GetInt();
	if( mMinSendFrameSize < 0 ) mMinSendFrameSize = 0;
	else if( mMinSendFrameSize > MAX_UDP_PACKET_SIZE-100 )
		mMinSendFrameSize = MAX_UDP_PACKET_SIZE-100;

#ifdef PLAYER_MOVEMENT_TRANSPORT_TESTING
	static logOpen = false;
	if( !logOpen )
	{
		PMTTLog.Open("pmtt.log");
		logOpen = true;
	}
#endif
}

NetworkPeer2::~NetworkPeer2( void )
{
	DeleteFrames();

	// TO DO - check if everything is deleted here

	if( mNet->IsServer() )
		mNet->mClientNumberManager.Free( mMyIndex );

	if( mGameSpyCDKeyIndex != -1 )
	{
#ifdef CDKEY_ENABLED
#ifndef	TRANSGAMING_NO_GAMESPY
		if( mNet->mGameSpySDK )
		{
			gcd_disconnect_user( GAMESPY_CDKEY_GAMEID, mGameSpyCDKeyIndex );
			GLog.Print( "Sent request to disconnect user %d to GameSpy\n", mGameSpyCDKeyIndex );
		}
#endif		
#endif
		mGameSpyCDKeyIndex = -1;
	}

	if( mNet->IsServer() )
		GScript.DoString("Game:AfterClientDisconnected(%d)", mMyIndex );

	mConnectionState = EStateDisconnected;
	mLastFrameTime = 0;
	Disconnect();

	// Remove remaining reliable messages
	while( mReliableMessages.Size() )
	{
		delete mReliableMessages[0];
		mReliableMessages.FastRemove(0);
	}
}

void NetworkPeer2::ClearMessages( bool unreliableOnly )
{
	if( unreliableOnly )
	{
		for( int i = 0; i < mMessagesToSend.Size(); ++i )
		{
			if( !mMessagesToSend[i].IsReliable )
			{
				--mMessagesToSend[i].DSM->ReferenceCounter;
				mMessagesToSend.Remove(i);
				--i;
			}
		}
	}
	else
	{
		for( int i = 0; i < mMessagesToSend.Size(); ++i )
			--mMessagesToSend[i].DSM->ReferenceCounter;
		mMessagesToSend.ClearArray();
	}
}

// Message may be NULL
bool NetworkPeer2::Send( UdpMessage* unreliable )
{
	if( unreliable && ( unreliable->GetSize() >= MAX_UDP_PACKET_SIZE ) )
	{
		GLog.Print( "Tried to send unreliable of size %d\n", unreliable->GetSize() );
		unreliable = 0;	// forget it
	}

	bool unreliableSent = ( unreliable == NULL );
	bool sendingOk = true;

	do
	{
		// Determine if we need to append reliable message
		bool sendReliableMessage = CanSendReliableMessage();

		UdpMessage mesg;
		// Write the packet header
		ulong32 h1 = mSent & 0x7FFFFFFF;
		if( sendReliableMessage ) h1 |= 0x80000000;
		ulong32 h2 = mIncoming & 0x7FFFFFFF;
		if( mReliableIncoming ) h2 |= 0x80000000;

		// Make sure highest is written first - this way collision with GameSpy messages won't occur too fast
		mesg.WriteUChar( (unsigned char)((h1&0xff000000)>>24) );
		mesg.WriteUChar( (unsigned char)((h1&0xff0000)>>16) );
		mesg.WriteUChar( (unsigned char)((h1&0xff00)>>8) );
		mesg.WriteUChar( (unsigned char)(h1&0xff) );

		mesg.WriteUChar( (unsigned char)((h2&0xff000000)>>24) );
		mesg.WriteUChar( (unsigned char)((h2&0xff0000)>>16) );
		mesg.WriteUChar( (unsigned char)((h2&0xff00)>>8) );
		mesg.WriteUChar( (unsigned char)(h2&0xff) );

		int tempSize = MAX_PACKET_HEADER_BYTES;
		if( sendReliableMessage )
		{
			int i;
			for( i = 0; i < mReliableMessages.Size(); ++i )
				mReliableMessages[i]->mWasSent = false;

			i = 0;
			do
			{
				// Find out the size of new reliable
				int newSize = mReliableMessages[i]->GetSize();

				if( newSize + tempSize >= MAX_UDP_PACKET_SIZE )
					break;

				tempSize += newSize;
				mesg += *(mReliableMessages[i]);

				mReliableMessages[i]->mWasSent = true;
				++i;
			}
			while( i < mReliableMessages.Size() );

			mLastReliable = mSent;
		}
		mSent++;

		// Eventually append unreliable message
		if( unreliable )
		{
			if( tempSize == MAX_PACKET_HEADER_BYTES )
			{
				Assert( tempSize + unreliable->GetSize() < MAX_UDP_PACKET_SIZE );
				mesg += *unreliable;
				unreliableSent = true;
			}
			else if( tempSize + unreliable->GetSize() < MAX_UDP_PACKET_SIZE )
			{
				mesg += *unreliable;
				unreliableSent = true;
			}
		}

#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 1;
#endif
		mesg.EndWriting();

#ifdef CRC_IN_PACKETS
		mesg.MakeCRCStamp();
#endif

//		mLastPacketsSendTime[(mSent-1)%PING_PACKETS_AVERAGED] = curTime;
		mStatsData.mPacketsSent++;
		mStatsChunks[mStatsCursor].mPacketsSent++;
		mStatsData.mBytesSent += mesg.GetSize();
		mStatsChunks[mStatsCursor].mBytesSent += mesg.GetSize();

		if( !mNet->LowLayer_Write( mAddress, &mesg ) )
			sendingOk = false;
	}
	while( !unreliableSent && sendingOk );

	mLastSendTime = GetCurrentTime();
	return sendingOk;
}

bool NetworkPeer2::PrepareMessageForReading( UdpMessage* incoming )
{
	++mStatsChunks[mStatsCursor].mPacketsReceived;
	++mStatsData.mPacketsReceived;
	mStatsChunks[mStatsCursor].mBytesReceived += incoming->GetSize();
	mStatsData.mBytesReceived += incoming->GetSize();

#ifdef CRC_IN_PACKETS
	if( !incoming->CheckRemoveCRCStamp() ) return false;
#endif

	ulong32 sent = 0;
	sent |= (((ulong32)incoming->ReadUChar())<<24);
	sent |= (((ulong32)incoming->ReadUChar())<<16);
	sent |= (((ulong32)incoming->ReadUChar())<<8);
	sent |= (ulong32)incoming->ReadUChar();

	ulong32 ack = 0;
	ack |= (((ulong32)incoming->ReadUChar())<<24);
	ack |= (((ulong32)incoming->ReadUChar())<<16);
	ack |= (((ulong32)incoming->ReadUChar())<<8);
	ack |= (ulong32)incoming->ReadUChar();

	bool hasReliable = ((sent&0x80000000)!=0);		sent &= 0x7FFFFFFF;
	bool relAck =      ((ack &0x80000000)!=0);		ack  &= 0x7FFFFFFF;

	mLastReceivedTime = GetCurrentTime();

	// discard old packets
	if( sent <= mIncoming )
		return false;

	// recalculate ping
	if( mAck < ack )
		mAck = ack;

	// Packet loss stats gathering
	mStatsChunks[mStatsCursor].mPacketsLost				+= sent - mIncoming - 1;
	mStatsChunks[mStatsCursor].mPacketsShouldBeReceived	+= sent - mIncoming;
	mStatsData.mPacketsLost								+= sent - mIncoming - 1;
	mStatsData.mPacketsShouldBeReceived					+= sent - mIncoming;

	if( relAck == mReliableSent )	// Our last reliable message has been received
	{
		while( mReliableMessages.Size() && mReliableMessages[0]->mWasSent )
		{
			for( int i = 0; i < mReliableMessages[0]->DeliveryReceipts.Size(); ++i )
				GScript.Call( "AcknowledgeNetMsgDelivery", 0, "%d", mReliableMessages[0]->DeliveryReceipts[i] );

			delete mReliableMessages[0];
			mReliableMessages.Remove(0);
		}
		mReliableSent = !mReliableSent;
		mStatsData.mReliablePacketsAcknowledged++;
		mStatsChunks[mStatsCursor].mReliablePacketsAcknowledged++;
	}

	mIncoming = sent;
	mReliableAck = relAck;

	if( hasReliable )
		mReliableIncoming = !mReliableIncoming;

	if( incoming->ReadFinished() )
		return false;	// empty packet

	return true;
}

void NetworkPeer2::TickStats( DWORD time )
{
	if( !mStatsChunks.Size() )
		return;

	// Calculate new TempStats
	mStatsDataTemp = mStatsChunks[0];
	for( int i = 1; i < mStatsChunks.Size(); ++i )
		mStatsDataTemp += mStatsChunks[i];

	// Move cursor
	mStatsCursor++;
	if( mStatsCursor == mStatsChunks.Size() )
		mStatsCursor = 0;

	mStatsChunks[mStatsCursor].Init( time );
}

void NetworkPeer2::HandlePeerPackage( UdpMessage* message )
{
	while( message->GetPos() < message->GetSize() )
		if( !HandleSingleMessage( message ) )
			return;
}

void NetworkPeer2::ClearReliableMessages( void )
{
	if( mReliableMessages.Size() && mReliableMessages[0]->mWasSent )
		mLastReliableMessageToAck = mLastReliable;

	for( int i = 0; i < mReliableMessages.Size(); ++i )
		delete mReliableMessages[i];
	mReliableMessages.ClearArray();
}

void NetworkPeer2::DeleteFrames( void )
{
	while( mFrames )
	{
		NetworkFrame* temp = mFrames;
		mFrames = mFrames->Next;
		delete temp;
	}

}

void NetworkPeer2::Disconnect( void )
{
	if( !mDisconnected )
	{
		// To be sure, three times :)
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_DISCONNECT );
		mesg.EndWriting();

		mNet->LowLayer_SendMaintenanceMessage( mAddress, &mesg );
		mNet->LowLayer_SendMaintenanceMessage( mAddress, &mesg );
		mNet->LowLayer_SendMaintenanceMessage( mAddress, &mesg );

		mDisconnected = true;
		mConnectionState = EStateDisconnected;
	}
}

bool NetworkPeer2::SendMaintenanceMessage( bool reliable, UdpMessage* message )
{
	if( reliable )
	{
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage();
		mesg->WriteData( message );
#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 2;
#endif
		mesg->EndWriting();
		SendReliable( mesg );
		return true;
	}
	else
	{
		UdpMessage mesg;
		mesg.WriteData( message );
#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 3;
#endif
		mesg.EndWriting();
		return Send( &mesg );
	}
}

// This returns false if the message is unrecognized and cannot be subtracted from the buffer
bool NetworkPeer2::HandleSingleMessage( UdpMessage* message )
{
	if( message->PeekUChar() & 0xF0 )
	{
		if( mNet->IsServer() )
			return HandleFrameOnServer( message );
		else
		{
			if( message->PeekUChar() == HIGHLAYER_FIRSTFRAME )
				return HandleFirstFrameOnClient( message );
			else
				return HandleFrameOnClient( message );
		}
	}

	unsigned char messageType = ( message->ReadUChar() & 0x0f );
	bool server = ( mNet->State == NetworkDevice2::SERVER );

	switch( messageType )
	{
	case HIGHLAYER_OPEN:
		{
#ifdef CONNECTION_STATES_LOGGING
			GLog.Print( "HIGHLAYER_OPEN received\n" );
#endif
			message->ReadCheckToken( NCL_CLOPEN );
			if( !server )
				return true;


			if( !mNet->ReadyForNetOpen( &mAddress ) )
			{
				delete this;	// this looks risky, but from now on it only returns
				return false;
			}

			if( mConnectionState > EStateConnecting )
			{
				GLog.Print( "Got NetClient_Open when already connected from client %d!! Ignoring.\n", mMyIndex );
				message->DumpToLogfile( "NetClient_Open when EStateConnecting" );
				return true;
			}

#ifdef TRANSGAMING
			Assert( GEngine->pcfWorld->LevelName.Len() );
#else
			Assert( GEngine->World->LevelName.Len() );
#endif

			int t = GScript.GetTop();
#ifdef	TRANSGAMING
			GScript.Call( "Game_GetLevelMapSize", 1, "%s", GEngine->pcfWorld->LevelName.AsChar() );
			int levSize = GScript.GetInt( -1 );
			GScript.SetTop(t);

			UdpMessage mesg;
			mesg.WriteUChar( HIGHLAYER_LOADMAP );
            mesg.WriteString( GEngine->pcfWorld->LevelName.AsChar() );
#else
			GScript.Call( "Game_GetLevelMapSize", 1, "%s", GEngine->World->LevelName.AsChar() );
			int levSize = GScript.GetInt( -1 );
			GScript.SetTop(t);

			UdpMessage mesg;
			mesg.WriteUChar( HIGHLAYER_LOADMAP );
            mesg.WriteString( GEngine->World->LevelName.AsChar() );
#endif
#ifndef	TRANSGAMING_NO_HAVOK
			GEngine->Physics->PackTweaksIntoMessage( &mesg );
#endif
			mesg.WriteLong( levSize );
			mesg.WriteLong( GEngine->Physics->PhysicsFPS );
			mesg.WriteCheckToken( NCL_LOADMAP );
#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 4;
#endif
			mesg.EndWriting();
			SendMaintenanceMessage( true, &mesg );
#ifdef CONNECTION_STATES_LOGGING
			GLog.Print( "HIGHLAYER_LOADMAP sent\n" );
#endif
			mStreamingTimeout = STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD;
			mConnectionState = EStateConnected;
		}
		return true;

	case HIGHLAYER_LOADMAP:
		{
#ifdef CONNECTION_STATES_LOGGING
			GLog.Print( "HIGHLAYER_LOADMAP received\n" );
#endif
			StringTransport.Clear();
			POCreationCache.Clear();

			String levelName = message->ReadString();
			if( !server )
			{
				if( mConnectionState > EStateConnecting ) // already playing
				{
					if( mConnectionState == EStateSynchronizing )
					{
						GLog.Print( "LoadMap came when I was just waiting for synchronizable entities!!!\n" );
						message->DumpToLogfile( "LoadMap when EStateSynchronizing" );
					}
#ifndef	TRANSGAMING_NO_HAVOK
					GEngine->Physics->GetTweaksFromMessage( message );
#endif
					int fileCRC = message->ReadLong();
					GEngine->Physics->PhysicsFPS = message->ReadLong();
					message->ReadCheckToken( NCL_LOADMAP );

					int t = GScript.GetTop();
					GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName.AsChar() );
					int levSize = GScript.GetInt( -1 );
					GScript.SetTop(t);

					if( fileCRC != levSize )
					{
						mConnectionState = EStateDisconnected;
						String errorText = String::Sprintf( "Net Error: the map we are trying to load is different than the map on the server!" );
						GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
						GLog.Print( errorText.AsChar() );
					}
					else
					{
						mNet->LoadNewMap(levelName.AsChar());
						mStreamingTimeout = STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD;
					}
					return true;
				}

				bool wasPaused = GEngine->Game->mPaused;
				GEngine->Game->mPaused = true;

				GEngine->Video->PMenu->LoadScreen.Activate( true );
#ifndef	TRANSGAMING_NO_HAVOK
				GEngine->Physics->GetTweaksFromMessage( message );
#endif
				int fileCRC = message->ReadLong();
				GEngine->Physics->PhysicsFPS = message->ReadLong();
				bool maploadok;

				int t = GScript.GetTop();
				GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName.AsChar() );
				int levSize = GScript.GetInt( -1 );
				GScript.SetTop(t);

				if( fileCRC != levSize )
				{
					mConnectionState = EStateDisconnected;
					String errorText = String::Sprintf( "Net Error: the map we are trying to load is different than the map on the server!" );
					GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
					GLog.Print( errorText.AsChar() );
					maploadok = false;
				}
				else
				{
#ifdef	TRANSGAMING
					GEngine->pcfWorld->FileName = "";
					GScript.DoString( "Game:LoadLevel('%s')",levelName.AsChar() );
					maploadok = (!GEngine->pcfWorld->FileName.IsNull());
#else
					GEngine->World->FileName = "";
					GScript.DoString( "Game:LoadLevel('%s')",levelName.AsChar() );
					maploadok = (!GEngine->World->FileName.IsNull());
#endif
				}

				GEngine->Game->mPaused = wasPaused;

				{
					String name("=framerate");
					String value = String::Sprintf( "%d", mServerFPS );
					TransferVariable( name, value );
					name = "=lpo";
					value = String::Sprintf( "%d", mLocalPlayerOptimizedEveryNFrames );
					TransferVariable( name, value );
					name = "=mpap";
					value = String::Sprintf( "%d", mMaxPlayerActionsPassed );
					TransferVariable( name, value );
				}

				UdpMessage mesg;
				mesg.WriteUChar( HIGHLAYER_MAPLOADING );
				mesg.WriteBool( maploadok );
				mesg.WriteCheckToken( NCL_MAPLOADING );
				mesg.EndWriting();
				SendMaintenanceMessage( true, &mesg );
#ifdef CONNECTION_STATES_LOGGING
				GLog.Print( "HIGHLAYER_MAPLOADING sent\n" );
#endif

				mStreamingTimeout = STREAMING_CLIENT_TIMEOUT_TIME_DURING_LEVEL_LOAD;

				if( !maploadok )
				{
					mConnectionState = EStateDisconnected;
#ifndef	TRANSGAMING_NO_HAVOK
					GEngine->Physics->GetTweaksFromScript();
#endif
				}
                else
				{
                    mConnectionState = EStateSynchronizing;
					PrepareForFirstFrame();
				}
			}
			else
			{
				// Dump tweaks
#ifndef	TRANSGAMING_NO_HAVOK
				GEngine->Physics->GetTweaksFromMessage( message );
				GEngine->Physics->GetTweaksFromScript();
#endif
				message->ReadLong();
				message->ReadLong();
			}
			message->ReadCheckToken( NCL_LOADMAP );
		}
		return true;

	case HIGHLAYER_MAPLOADING:
		{
#ifdef CONNECTION_STATES_LOGGING
			GLog.Print( "HIGHLAYER_MAPLOADING received\n" );
#endif
			bool ok = message->ReadBool();
			message->ReadCheckToken( NCL_MAPLOADING );
			if( server )
			{
				if( mConnectionState >= EStateSynchronizing )
				{
					GLog.Print( "Client said that it loaded the map when I thought it was playing!! Ignoring.\n" );
					message->DumpToLogfile( "MapLoading when state >= EStateSynchronizing" );
					return true;
				}

				mStreamingTimeout = STREAMING_TIMEOUT_TIME;

				if( !ok )
					mConnectionState = EStateDisconnected;
				else
				{
					SendFirstFrame();
					GLog.Print( "Created synchro entities for client %d\n", mMyIndex );

					UdpMessage mesg;
					mesg.WriteUChar( HIGHLAYER_SYNCHRONIZED );
					mesg.WriteCheckToken( NCL_SYNCHRO );
					mesg.EndWriting();
					SendMaintenanceMessage( true, &mesg );
#ifdef CONNECTION_STATES_LOGGING
					GLog.Print( "HIGHLAYER_SYNCHRONIZED sent\n" );
#endif
					mConnectionState = EStateSynchronizing;
				}
			}
		}
		return true;

	case HIGHLAYER_SYNCHRONIZED:
		{
#ifdef CONNECTION_STATES_LOGGING
			GLog.Print( "HIGHLAYER_SYNCHRONIZED received\n" );
#endif
			message->ReadCheckToken( NCL_SYNCHRO );

			if( !server )
			{
				mStreamingTimeout = STREAMING_TIMEOUT_TIME;
				UdpMessage mesg;
				mesg.WriteUChar( HIGHLAYER_SYNCHRONIZED_ACKNOWLEDGE );
				mesg.WriteCheckToken( NCL_SYNCHROACK );
				mesg.EndWriting();
				SendMaintenanceMessage( true, &mesg );
#ifdef CONNECTION_STATES_LOGGING
				GLog.Print( "HIGHLAYER_SYNCHRONIZED_ACKNOWLEDGE sent\n" );
#endif

				mConnectionState = EStatePlay;

#ifdef	TRANSGAMING
				GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->pcfWorld->FileName.AsChar(),GEngine->pcfWorld->LevelName.AsChar());
#else
				GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->World->FileName.AsChar(),GEngine->World->LevelName.AsChar());
#endif
				GScript.DoString("Game:OnPlay(true)");
			}
		}
		return true;

	case HIGHLAYER_SYNCHRONIZED_ACKNOWLEDGE:
		{
#ifdef CONNECTION_STATES_LOGGING
				GLog.Print( "HIGHLAYER_SYNCHRONIZED_ACKNOWLEDGE received\n" );
#endif
			message->ReadCheckToken( NCL_SYNCHROACK );

			if( server )
			{
				GScript.DoString("Game:AfterNewClientConnected(%d)", mMyIndex );
				mConnectionState = EStatePlay;
			}
		}
		return true;

	case HIGHLAYER_VARIABLE:
		{
			String name = message->ReadString();
			String value = message->ReadString();
			bool ok = message->ReadCheckToken( NCL_VARIABLE );
			if( ok )
				InterpretVariable( name, value );
			return ok;
		}
	default:

		if( server )
			GLog.Print( "Unknown channel message received (server)\n" );
		else
			GLog.Print( "Unknown channel message received (client)\n" );
		message->DumpToLogfile( "Unknown channel message received" );
		return false;
	}
}

bool NetworkPeer2::HandleFrameOnServer( UdpMessage* message )
{
	static UdpMessage mesg( 65536 );	// safeguard just-in-case

	unsigned char command = message->ReadUChar();

	switch( command )
	{
	case HIGHLAYER_SCRIPTMESGS:
		{
			int logPos = message->GetPos();
			unsigned char howMany = message->ReadUChar();

			for( int i = 0; i < howMany; ++i )
			{
				unsigned short size = message->ReadUShort();

				if( mConnectionState >= EStateSynchronizing )
				{
					mesg.SeekToBegin();
					mesg.WriteData( message, size );
#ifdef UDP_END_WRITING_CRASH_LOG
					EndWritingCrashNumber = 5;
#endif
					mesg.EndWriting();
					mesg.SeekToBegin();

					int oldState = GProfiler.GetState();
					GProfiler.SetState(NET_GSCRIPTS);
					GScript.Call( "GetNetEvent", 0, "%p,%d", &mesg, mMyIndex );
					GProfiler.SetState(oldState);
				}
				message->ScrapData( size );
			}
			message->ReadCheckToken( NCL_SCRIPTMSG );
			mUFCReliable = message->GetPos()-logPos;
		}
		return true;
	case HIGHLAYER_FRAME:
		{
			if( mConnectionState < EStateSynchronizing )
				return false;	// ignore frame; its probably last one from previous map

			++mStatsChunks[mStatsCursor].mFramesReceived;
			++mStatsData.mFramesReceived;

			unsigned char flags = message->ReadUChar();
			ulong32 frameNumber = message->ReadULong();
			ulong32 highestReliableReceivedOnClient = message->ReadULong();
			mPing = message->ReadUShort();
			mPingMedian.AddResult( mPing );

			unsigned short deltaFrom = message->ReadUShort();
			unsigned char actionCount = message->ReadUChar();

#ifdef PEER_LOGGING
			GLog.Print( "Server handled frame: %u\n", frameNumber );
#endif

			// New deltaFrom came - remove all reliable messages acknowledged as already received.
			for( int i = 0; i < mMessagesToSend.Size(); ++i )
				if( mMessagesToSend[i].Number <= highestReliableReceivedOnClient )
				{
					--mMessagesToSend[i].DSM->ReferenceCounter;
					mMessagesToSend.Remove(i);
					--i;
				}

			// Check if the frame is newer than the last
			NetworkFrame* lastFrame = GetNewestFrame();
			if( lastFrame && ( lastFrame->Number >= frameNumber ) )
				return false;	// scrap the frame

			Assert( deltaFrom );
			ulong32 fromFrameNumber = frameNumber-deltaFrom;
			ulong32 actionsFromFrameNumber = frameNumber-actionCount;
			Assert( mFrames );

			// Remove all unneeded frames
			do
			{
				NetworkFrame* temp = mFrames;
				if( mFrames->Number == fromFrameNumber )
				{
					Assert( mFrames->Answer );
					break;
				}
				mFrames = mFrames->Next;
				delete temp;
			}
			while( mFrames );

			Assert( mFrames );	// Every frame is differential from something!! At the start, from frame 0
			NetworkFrame* fromFrame = mFrames;

			int logPos = message->GetPos();

			// Get again all frames we had but the client doesn't know that we had
			bool firstAction = (( flags & FRAME_FIRSTACTIONFULL ) != 0 );
			++fromFrameNumber;
			while( fromFrame->Next )
			{
				Assert( fromFrame->Next->Number == fromFrameNumber );
				if( actionsFromFrameNumber < fromFrameNumber )
				{
					if( firstAction )
					{
						fromFrame->Next->Input.Read( message );
						firstAction = false;
					}
					else
						fromFrame->Next->Input.ReadDelta( message, &fromFrame->Input );
				}
				++fromFrameNumber;
				fromFrame = fromFrame->Next;
			}

			// Get all the frames we didn't have yet
			NetworkFrame* newFrame = 0;

			if( actionsFromFrameNumber >= fromFrameNumber )
			{
				GLog.Print( "Too many frames passed!! Some frames have invalid move!\n" );
				while( actionsFromFrameNumber >= fromFrameNumber )	// this way to work aroung unsigneds
				{
                    m_setTag("Net");
					newFrame = new NetworkFrame;
					newFrame->Number = fromFrameNumber++;
					newFrame->DeltaFrom = 0;
					fromFrame->Next = newFrame;
					fromFrame = newFrame;
				}
			}

			Entity* e = GEngine->Net->GetPlayerByIndex( mMyIndex );
			PhysicsObject* po = e?e->PO:0;

			while( fromFrameNumber <= frameNumber )
			{
                m_setTag("Net");
				newFrame = new NetworkFrame;
				newFrame->Number = fromFrameNumber++;
				newFrame->DeltaFrom = 0;
				if( firstAction )
				{
					newFrame->Input.Read( message );
					firstAction = false;
				}
				else
					newFrame->Input.ReadDelta( message, &fromFrame->Input );
				newFrame->Next = 0;
				fromFrame->Next = newFrame;
				fromFrame = newFrame;

#ifndef	TRANSGAMING_NO_HAVOK
				if( po )
					po->MultiPlayerActionNonPhysic( &newFrame->Input, true, true );
#endif
			}
#ifdef PLAYER_MOVEMENT_TRANSPORT_TESTING
			PMTTLog.Print( "Frame %d\n", newFrame->Number );
			if( po )
			{
				PlayerState s;
				s.GetFromPlayer( po );
				s.Print( PMTTLog );
			}
			newFrame->Input.Print( PMTTLog );
#endif
			newFrame->SentTime = GEngine->SysDriver.GetCurrentTimeMS();	// remember received time for this frame

			mUFCPlayerDeltas = message->GetPos()-logPos;
			logPos = message->GetPos();

			GScript.Call( "CPlayer_SetupAction",0,"%d,%d,%d,%d",
				 mMyIndex, newFrame->Input.Action, ((long32)-newFrame->Input.Yaw)+32768L,
				 ((long32)-newFrame->Input.Pitch)+32768L );

			newFrame->DeltaFrom = deltaFrom;
			Assert( newFrame->Number == frameNumber );

			// Receive unreliable messages, and execute them ( they're sure to be here only once )
			if( flags & FRAME_SCRIPT )
			{
				unsigned char howMany = message->ReadUChar();

				for( int i = 0; i < howMany; ++i )
				{
					unsigned short size = message->ReadUShort();

					if( mConnectionState >= EStateSynchronizing )
					{
						mesg.SeekToBegin();
						mesg.WriteData( message, size );
#ifdef UDP_END_WRITING_CRASH_LOG
						EndWritingCrashNumber = 6;
#endif
						mesg.EndWriting();
						mesg.SeekToBegin();

						int oldState = GProfiler.GetState();
						GProfiler.SetState(NET_GSCRIPTS);
						GScript.Call( "GetNetEvent", 0, "%p,%d", &mesg, mMyIndex );
						GProfiler.SetState(oldState);
					}
					message->ScrapData( size );
				}
				message->ReadCheckToken( NCL_SCRIPTMSG );
			}
			mUFCUnreliable = message->GetPos()-logPos;

			if( flags & FRAME_FILLER )
			{
				unsigned short howMany = message->ReadUShort();
				message->ScrapData( howMany );
			}
		}
		return true;
	default:
		GLog.Print( "Unknown frame type: %d !!\n", command );
		message->DumpToLogfile( "Unknown frame type" );
		return false;
	}
}

bool NetworkPeer2::HandleFrameOnClient( UdpMessage* message )
{
	static UdpMessage mesg( 65536 );	// safeguard, just-in-case

#ifdef _DEBUG
	unsigned char command = message->ReadUChar();
	Assert( command == HIGHLAYER_FRAME );
#else
	message->ReadUChar();
#endif

	++mStatsChunks[mStatsCursor].mFramesReceived;
	++mStatsData.mFramesReceived;

	unsigned char flags = message->ReadUChar();
	StringTransport.SetFirstUnsureStringNumberToReceive( message->ReadUShort() );
	POCreationCache.SetFirstUnsureNumberToReceive( message->ReadUShort() );
	ulong32 frameNumber = message->ReadULong();

	NetworkFrame* lastAnsweredFrame = GetLastAnsweredFrame();
	if( lastAnsweredFrame && ( lastAnsweredFrame->Number >= frameNumber ) )
	{
		GLog.Print( "Server frame out of order ( %u, should be higher than %u )!\n", frameNumber, lastAnsweredFrame->Number );
		return false;	// this means that the frame came out of order
	}

#ifdef PEER_LOGGING
	GLog.Print( "Client handled frame: %u\n", frameNumber );
#endif

	NetworkFrame* answeredFrame = GetFrameByNumber( frameNumber );
	Assert( answeredFrame );

	// Determine delta frame for this frame
	// If there is, remove frames earlier than the one on which this one depends

	ulong32 soughtNumber = answeredFrame->Number - answeredFrame->DeltaFrom;

	while( mFrames->Number != soughtNumber )
	{
		NetworkFrame* tempFrame = mFrames;
		mFrames = mFrames->Next;
		delete tempFrame;
		Assert( mFrames );
	}
	NetworkFrame* deltaFrame = mFrames;

	// Create the received part of the last frame for current client
    m_setTag("Net");
	answeredFrame->Answer = new NetworkAnswerFrame;
	answeredFrame->Answer->LocalPlayerEntity = GEngine->Net->GetLocalPlayer();

	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
	answeredFrame->Answer->Latency = ( curTime < answeredFrame->SentTime )?0:curTime-answeredFrame->SentTime;
	LastFrameLatency = answeredFrame->Answer->Latency;

#ifdef	TRANSGAMING
#ifndef	TRANSGAMING_NO_HAVOK
	TempPhysFrames = GEngine->pcfWorld->Physics->GetPhysicsFrame() - answeredFrame->PhysicsFrame;
#endif // TRANSGAMING_NO_HAVOK
#else
	TempPhysFrames = GEngine->World->Physics->GetPhysicsFrame() - answeredFrame->PhysicsFrame;
#endif

	DynamicArray<ulong32> targetFrameSynchroIndices;
	for( NetworkObjectState* state = deltaFrame->Answer->EntitiesStateList; state; state = state->Next )
		targetFrameSynchroIndices.Add( state->UniqueNumber );

	// QW-style latency calculations ( on client side )
	if( ( LastFrameLatency >= 0 ) && ( LastFrameLatency <= 1000 ) )
	{
		if( LastFrameLatency < mPing )
			mPing = LastFrameLatency;
		else
			++mPing;
		mPingMedian.AddResult( mPing );
	}

	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
		mNet->mObjectSynchronizers[i]->NotRefreshed = true;

	int logPos = message->GetPos();

	if( flags & FRAME_CREATE )
	{
		unsigned char howMany = message->ReadUChar();
		for( int i = 0; i < howMany; ++i )
		{
			ulong32 uniqueEntityIndex;
			unsigned char entityType;
			EntitySynchronizer2::ReadSynchronizer( message, uniqueEntityIndex, entityType );
			Assert( uniqueEntityIndex != (DWORD)-1 );

			EntitySynchronizer2* es = 0;
			EntitySynchronizer2** esTemp = mNet->mObjectSynchronizersHash.Find( uniqueEntityIndex );
			if( esTemp )					// object already created; skip this
				es = *esTemp;
			else							// create object
			{
				switch( entityType )
				{
					case EntitySynchronizer2::STypeMesh:
                        m_setTag("Net");
						es = (EntitySynchronizer2*) new MeshSynchronizer2(uniqueEntityIndex,mNet,0);
						break;
					case EntitySynchronizer2::STypeModel:
                        m_setTag("Net");
						es = (EntitySynchronizer2*) new ModelSynchronizer2(uniqueEntityIndex,mNet,0);
						break;
					default:
						Assert( false );	// this will fire
						break;
				}
			}
			if( !es->InterpretCreateMessage( message, this, answeredFrame ) )
				return false;
			targetFrameSynchroIndices.Add( uniqueEntityIndex );
			es->NotRefreshed = false;
		}
		mDFCCreate = message->GetPos()-logPos;
		logPos = message->GetPos();
	}
	else
		mDFCCreate = 0;

	if( flags & FRAME_UPDATE )
	{
		unsigned char howMany = message->ReadUChar();
		for( int i = 0; i < howMany; ++i )
		{
			ulong32 uniqueEntityIndex;
			unsigned char entityType;
			EntitySynchronizer2::ReadSynchronizer( message, uniqueEntityIndex, entityType );
			Assert( uniqueEntityIndex != (DWORD)-1 );
			EntitySynchronizer2** es = mNet->mObjectSynchronizersHash.Find( uniqueEntityIndex );
			Assert( es );
			if( !(*es)->InterpretUpdateMessage( message, this, answeredFrame, deltaFrame ) )
				return false;

			(*es)->NotRefreshed = false;
		}

		mDFCUpdate = message->GetPos()-logPos;
		logPos = message->GetPos();
	}
	else
		mDFCUpdate = 0;

	DynamicArray<ulong32> toDelete;
	if( flags & FRAME_DELETE )
	{
		unsigned char howMany = message->ReadUChar();
		for( int i = 0; i < howMany; ++i )
		{
			ulong32 indexToDelete = message->ReadULong();
			toDelete.Add( indexToDelete );
			int index = targetFrameSynchroIndices.Find( indexToDelete );
			if( index != -1 )
				targetFrameSynchroIndices.FastRemove( index );
			else
				GLog.Print( "To delete not found of tfsi!!\n" );
		}

		mDFCDelete = message->GetPos()-logPos;
		logPos = message->GetPos();
	}
	else
		mDFCDelete = 0;

	message->ReadCheckToken( NCL_SCRIPTMSG );

	if( flags & FRAME_SCRIPT )
	{
		bool reliable = true;
		unsigned char howMany = message->ReadUChar();
		logPos = message->GetPos();
		for( int i = 0; i < howMany; ++i )
		{
			ulong32 number = (DWORD)-1;
			if( reliable )
			{
				number = message->ReadULong();
				if( number == (DWORD)-1 )
				{
					reliable = false;
					mDFCReliable = message->GetPos()-logPos-4;
					logPos = message->GetPos()-4;
				}
			}

			unsigned short size = message->ReadUShort();

			if( ( mConnectionState >= EStateSynchronizing ) && ( !reliable || ( number > mHighestReliableReceivedOnClient ) ) )
			{
				mesg.SeekToBegin();
				mesg.WriteData( message, size );
#ifdef UDP_END_WRITING_CRASH_LOG
				EndWritingCrashNumber = 7;
#endif
				mesg.EndWriting();
				mesg.SeekToBegin();

				int oldState = GProfiler.GetState();
				GProfiler.SetState(NET_GSCRIPTS);
				GScript.Call( "GetNetEvent", 0, "%p,%d", &mesg, SERVER_ID );
				GProfiler.SetState(oldState);
			}

			if( reliable && ( number > mHighestReliableReceivedOnClient ) )
				mHighestReliableReceivedOnClient = number;

			message->ScrapData( size );
		}

		if( reliable )
		{
			mDFCReliable = message->GetPos()-logPos;
			mDFCUnreliable = 0;
		}
		else
			mDFCUnreliable = message->GetPos()-logPos;

		message->ReadCheckToken( NCL_SCRIPTMSG );
	}
	else
	{
		mDFCReliable = mDFCUnreliable = 0;
	}
/*
	if( flags & FRAME_FILLER )
	{
		unsigned short howMany = message->ReadUShort();
		message->ScrapData( howMany );
	}
*/
	// Additional delete check
	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
	{
		int index = targetFrameSynchroIndices.Find( mNet->mObjectSynchronizers[i]->UniqueNumber );
		if( index != -1 )
			targetFrameSynchroIndices.FastRemove( index );
		else
			toDelete.Add( mNet->mObjectSynchronizers[i]->UniqueNumber );
	}

	// Delete entities marked for deletion
	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
	{
		int index = toDelete.Find( mNet->mObjectSynchronizers[i]->UniqueNumber );
		if( index != -1 )
		{
			mNet->mObjectSynchronizers[i]->DeleteSubject();
			delete mNet->mObjectSynchronizers[i];
			--i;
			toDelete.FastRemove( index );
		}
	}

	// Make a copy of state for every object for which the state hasn't been refreshed
	ulong32 playerUniqueNumber = mNet->GetEntityNumber( answeredFrame->Answer->LocalPlayerEntity );
	for( NetworkObjectState* s = deltaFrame->Answer->EntitiesStateList; s; s = s->Next )
	{
		EntitySynchronizer2** e = mNet->mObjectSynchronizersHash.Find( s->UniqueNumber );
		if( e && (*e)->NotRefreshed )
		{
			NetworkObjectState* s2 = s->MakeExactCopy();
			if( s2->UniqueNumber == playerUniqueNumber )
			{
				if( answeredFrame->PlayerData == *s2->Player )
					answeredFrame->Answer->PredictAgainLocalPlayer = false;
				else
				{
					Assert( answeredFrame->Answer->PredictAgainLocalPlayer == true );
					s2->Player->SetIntoPlayer( answeredFrame->Answer->LocalPlayerEntity->PO );
#ifdef PLAYER_COMPARE
					Assert( answeredFrame->PlayerData.CompareWithPlayer( answeredFrame->Answer->LocalPlayerEntity->PO ) );
					GLog.Print( "###1### Diff for frame: %d\n", answeredFrame->Number );
#endif
				}
			}
			s2->Next = answeredFrame->Answer->EntitiesStateList;
			answeredFrame->Answer->EntitiesStateList = s2;
		}
	}

	// Recalculate local player prediction
	if( answeredFrame->Answer->PredictAgainLocalPlayer && answeredFrame->Answer->LocalPlayerEntity && answeredFrame->Answer->LocalPlayerEntity->PO )
		for( NetworkFrame* frame = answeredFrame->Next; frame; frame = frame->Next )
		{
#ifndef	TRANSGAMING_NO_HAVOK
			answeredFrame->Answer->LocalPlayerEntity->PO->MultiPlayerActionNonPhysic( &frame->Input, false, false );
#endif
			frame->PlayerData.GetFromPlayer( answeredFrame->Answer->LocalPlayerEntity->PO );
		}

	// Move the other players forward by the specified prediction time
	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
	{
		if( mNet->mObjectSynchronizers[i]->UniqueNumber == playerUniqueNumber )
			continue;
		Entity* e = ( static_cast<EntitySynchronizer2*>(mNet->mObjectSynchronizers[i]) )->GetEntity();
		if( e->PO && e->PO->mPlayerVariables )
		{
#ifndef	TRANSGAMING_NO_HAVOK
			e->PO->MultiPlayerGetOrientFromController();
			if( mNet->mObjectSynchronizers[i]->NotRefreshed )
				e->PO->MultiPlayerActionSetupPredictionTime( answeredFrame->SentTime );
#endif
		}
	}

	return true;
}

void NetworkPeer2::Tick( void )
{
	if( mDisconnected )
		return;

	mUFCReliable = 0;

	// Keep the streaming alive so reliable packets have a chance to get to the other side
	DWORD currentTime = GetCurrentTime();

	if( CanSendReliableMessage() || currentTime - mLastSendTime > MAX_TIME_BETWEEN_PACKETS )
		Send( 0 );

	// Check streaming timeout
	if( currentTime - mLastReceivedTime > mStreamingTimeout )
	{
		Disconnect();

		if( !mNet->IsServer() )
		{
			String errorText = String::Sprintf( "Disconnected on timeout: %d ms\n", mStreamingTimeout );
			GLog.Print( errorText.AsChar() );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		}
	}
}

void NetworkPeer2::ResetToConnecting( void )
{
	ClearReliableMessages();
	if( mNet->IsServer() )
	{
		POCreationCache.Clear();
		StringTransport.Clear();
	}

	mConnectionState = EStateConnecting;
}

void NetworkPeer2::SendLoadMap( const char *levelName )
{
	Assert( mNet->IsServer() );
	Assert( levelName[0] );

	int t = GScript.GetTop();
	GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName );
	int levSize = GScript.GetInt( -1 );
	GScript.SetTop(t);

	UdpMessage mesg;
	mesg.WriteUChar( HIGHLAYER_LOADMAP );
	mesg.WriteString( levelName );
#ifndef	TRANSGAMING_NO_HAVOK
	GEngine->Physics->PackTweaksIntoMessage( &mesg );
#endif
	mesg.WriteLong( levSize );
	mesg.WriteLong( GEngine->Physics->PhysicsFPS );
	mesg.WriteCheckToken( NCL_LOADMAP );
#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 8;
#endif
	mesg.EndWriting();

	SendMaintenanceMessage( true, &mesg );
	mStreamingTimeout = STREAMING_TIMEOUT_TIME_DURING_LEVEL_LOAD;
	mConnectionState = EStateConnected;
}

void NetworkPeer2::SendMapLoaded( bool ok )
{
	UdpMessage mesg;
	mesg.WriteUChar( HIGHLAYER_MAPLOADING );
	mesg.WriteBool( ok );
	mesg.WriteCheckToken( NCL_MAPLOADING );
	mesg.EndWriting();
	SendMaintenanceMessage( true, &mesg );

	mConnectionState = ok?EStateSynchronizing:EStateDisconnected;
}

bool NetworkPeer2::SendFrame( DWORD time )
{
	if( time - mLastFrameTime < 1000/mServerFPS )
		return true;

	UdpMessage buffer( 65536 );	// big buffer

	// Startup return
	if( !mFrames )
		return true;	// waiting for client to send the first frame

	// Check if we got any updates from client since last frame sent - if not, don't send update - client has problems
	NetworkFrame* newFrame = GetNewestFrame();
	if( newFrame->Answer )
		return true;	// we already answered that; client has problems

#ifdef PEER_LOGGING
	GLog.Print( "Server sent frame: %u\n", newFrame->Number );
#endif

	// Create the received part of the last frame for current client
    m_setTag("Net");
	newFrame->Answer = new NetworkAnswerFrame;

	unsigned char flags = 0;
	unsigned short lastSynchroStringNumber = 0;
	unsigned short lastSynchroPONumber = 0;

	NetworkFrame* lastFrame = 0;
	Assert( newFrame->DeltaFrom );

	ulong32 soughtNumber = newFrame->Number-newFrame->DeltaFrom;
	lastFrame = mFrames;
	while( lastFrame )
	{
		if( lastFrame->Number == soughtNumber )
			break;
		lastFrame = lastFrame->Next;
	}
	Assert( lastFrame );

	// If lastFrame isn't the first frame, remove all frames remembered before lastFrame
	while( lastFrame != mFrames )
	{
		NetworkFrame* tempFrame = mFrames;
		mFrames = mFrames->Next;
		delete tempFrame;
	}

	lastSynchroStringNumber = lastFrame->Answer->LastSynchroStringNumber;
	lastSynchroPONumber = lastFrame->Answer->LastSynchroPONumber;

	// Categorize object synchronizers to create, update, delete
	DynamicArray<EntitySynchronizer2*> toCreate;
	DynamicArray<EntitySynchronizer2*> toUpdate;
	DynamicArray<NetworkObjectState*> toUpdateFrom;
	DynamicArray<ulong32> toDelete;

	for( NetworkObjectState* state = lastFrame->Answer->EntitiesStateList; state; state = state->Next )
	{
		EntitySynchronizer2** e = mNet->mObjectSynchronizersHash.Find( state->UniqueNumber );
		if( !e )	// Add to delete list
			toDelete.Add( state->UniqueNumber );
		else		// Add to update list
		{
			toUpdate.Add(*e);
			toUpdateFrom.Add(state);
		}
	}

	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
		if( toUpdate.Find( mNet->mObjectSynchronizers[i] ) == -1 )
			toCreate.Add( mNet->mObjectSynchronizers[i] );

	// Start frame packet
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;

	mesg->WriteUChar( HIGHLAYER_FRAME );	// command
	int pos = mesg->GetPos();	// placeholder for content flags
	mesg->WriteUChar( 0 );
	mesg->WriteUShort( lastSynchroStringNumber );
	StringTransport.SetFirstUnsureStringNumberToSend( lastSynchroStringNumber );
	mesg->WriteUShort( lastSynchroPONumber );
	POCreationCache.SetFirstUnsureNumberToSend( lastSynchroPONumber );
	mesg->WriteULong( newFrame->Number );

	bool dontWriteAnymore = false;

	int logPos = mesg->GetPos();

	newFrame->Answer->LocalPlayerEntity = GEngine->Net->GetPlayerByIndex( mMyIndex );

	// Add all differential data to frame packet ( for entities creation )
	if( toCreate.Size() )
	{
		mesg->WriteUChar( toCreate.Size() );	// command
		flags |= FRAME_CREATE;
		for( int i = 0; i < toCreate.Size(); ++i )
		{
			buffer.SeekToBegin();
			toCreate[i]->AddCreateObjMessage( &buffer, this, newFrame );
#ifdef UDP_END_WRITING_CRASH_LOG
			EndWritingCrashNumber = 9;
#endif
			buffer.EndWriting();
			if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
			{
				delete mesg;
				return false;
			}
			mesg->WriteData( &buffer );
		}
	}

	mDFCCreate = mesg->GetPos()-logPos;
	logPos = mesg->GetPos();

	// Add all differential data to frame packet ( for entities update )
	if( toUpdate.Size() )
	{
		unsigned char toUpdateReally = 0;
		int p = mesg->GetPos();
		mesg->WriteUChar(0);
		for( int i = 0; i < toUpdate.Size(); ++i )
		{
			buffer.SeekToBegin();
			if( toUpdate[i]->AddUpdateObjMessage( &buffer, this, newFrame, toUpdateFrom[i] ) )
			{
				++toUpdateReally;
#ifdef UDP_END_WRITING_CRASH_LOG
				EndWritingCrashNumber = 10;
#endif
				buffer.EndWriting();
				if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
				{
					delete mesg;
					return false;
				}
				mesg->WriteData( &buffer );
			}
		}

		if( toUpdateReally )
		{
			int p2 = mesg->GetPos();
			mesg->Seek( p );
			mesg->WriteUChar( toUpdateReally );
			mesg->Seek( p2 );
			flags |= FRAME_UPDATE;
		}
		else
			mesg->Seek( p );
	}

	mDFCUpdate = mesg->GetPos()-logPos;
	logPos = mesg->GetPos();

	// Add all differential data to frame packet ( for entities creation )
	if( toDelete.Size() )
	{
		mesg->WriteUChar( toDelete.Size() );	// command
		flags |= FRAME_DELETE;
		for( int i = 0; i < toDelete.Size(); ++i )
			mesg->WriteULong( toDelete[i] );
	}

	mDFCDelete = mesg->GetPos()-logPos;
	logPos = mesg->GetPos();

	mesg->WriteCheckToken( NCL_SCRIPTMSG );

	if( newFrame->Number == 1 )	// on the first frame, clear all unreliable messages
		ClearMessages( true );

	// Add all reliable messages to frame packet ( for reliable scripts )
	unsigned char scriptsNum = 0;
	int pos2 = 0;
	if( !dontWriteAnymore )
	{
		if( mMessagesToSend.Size() )
		{
			for( int i = 0; i < mMessagesToSend.Size(); ++i )
				if( mMessagesToSend[i].IsReliable )
				{
					buffer.SeekToBegin();
					buffer.WriteUShort( (unsigned short)(mMessagesToSend[i].DSM->Message->GetSize()) );
					buffer.WriteData( mMessagesToSend[i].DSM->Message );
#ifdef UDP_END_WRITING_CRASH_LOG
					EndWritingCrashNumber = 11;
#endif
					buffer.EndWriting();

					if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
					{
						dontWriteAnymore = true;
						break;
					}

					if( !scriptsNum )
					{
						pos2 = mesg->GetPos();	// placeholder for script messages number
						mesg->WriteUChar( 0 );
					}

					mesg->WriteULong( mMessagesToSend[i].Number );
					mesg->WriteData( &buffer );
					newFrame->Answer->LastReliableMessageSentInFrame = mMessagesToSend[i].Number;
					++scriptsNum;
				}
		}
	}

	mDFCReliable = mesg->GetPos()-logPos;
	logPos = mesg->GetPos();

	// Add all unreliable messages to frame packet ( for unreliable scripts )
	if( !dontWriteAnymore )
	{
		bool first = true;
		for( int i = 0; i < mMessagesToSend.Size(); ++i )
			if( !mMessagesToSend[i].IsReliable )
			{
				buffer.SeekToBegin();
				buffer.WriteUShort( (unsigned short)(mMessagesToSend[i].DSM->Message->GetSize()) );
				buffer.WriteData( mMessagesToSend[i].DSM->Message );
#ifdef UDP_END_WRITING_CRASH_LOG
				EndWritingCrashNumber = 12;
#endif
				buffer.EndWriting();

				if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
				{
					dontWriteAnymore = true;
					break;
				}

				if( !scriptsNum )
				{
					pos2 = mesg->GetPos();	// placeholder for script messages number
					mesg->WriteUChar( 0 );
				}

				if( first )
				{
					mesg->WriteULong((DWORD)-1);	// marker of start of ureliable messages
					first = false;
				}

				mesg->WriteData( &buffer );
				++scriptsNum;
			}
	}

	mDFCUnreliable = mesg->GetPos()-logPos;
	logPos = mesg->GetPos();
/*
	if( !dontWriteAnymore )
	{
		if( mesg->GetPos() < mMinSendFrameSize-2 )
		{
			flags |= FRAME_FILLER;
			unsigned short toFill = mMinSendFrameSize-2-mesg->GetPos();
			mesg->WriteUShort( toFill );
			mesg->FillWith(0, toFill );
		}
	}
*/
	// Enter number of scripts
	if( scriptsNum )
	{
		int posTemp = mesg->GetPos();
		mesg->Seek(pos2);
		mesg->WriteUChar(scriptsNum);
		mesg->Seek(posTemp);
		mesg->WriteCheckToken( NCL_SCRIPTMSG );
		flags |= FRAME_SCRIPT;
	}

	// Enter flags
	{
		int posTemp = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUChar(flags);
		mesg->Seek(posTemp);
	}

	// Send all of this by means of unreliable message
#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 13;
#endif
	mesg->EndWriting();
#ifdef PEER_LOGGING
	GLog.Print( "Server frame size: %u ( c: %u, u: %u, d: %u, rel: %u, unr: %u )\n", mesg->GetSize(),
		mDFCCreate, mDFCUpdate, mDFCDelete, mDFCReliable, mDFCUnreliable );
#endif

	Send( mesg );
	++mStatsChunks[mStatsCursor].mFramesSent;
	++mStatsData.mFramesSent;
	delete mesg;

	ClearMessages( true );	// clear unreliable messages; they've been sent once already; enough

	// Update last frame time
	{
		int delay = 1000/mServerFPS;
		if( !delay )
			mLastFrameTime = time;
		else
		{
			int times = (time - mLastFrameTime)/delay;
			mLastFrameTime += times*delay;
		}
	}

	newFrame->Answer->LastSynchroStringNumber = StringTransport.GetFirstUnusedStringNumber();
	newFrame->Answer->LastSynchroPONumber = POCreationCache.GetFirstUnusedNumber();
	return true;
}

void NetworkPeer2::SendFirstFrame( void )
{
#ifdef PEER_LOGGING
	GLog.Print( "Server sent first frame!\n" );
#endif

	UdpMessage buffer( 65536 );	// big buffer

	// Make a new frame with number 0
    m_setTag("Net");
	NetworkFrame* newFrame = new NetworkFrame;
	newFrame->Number = 0;
	newFrame->DeltaFrom = 0;
    m_setTag("Net");
	newFrame->Answer = new NetworkAnswerFrame;
	mFrames = newFrame;

	// Start frame packet
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;

	mesg->WriteUChar( HIGHLAYER_FIRSTFRAME );	// command

	// Add entities creation data to message
	unsigned short howManyEntities = 0;

	int pos = mesg->GetPos();	// placeholder for entities number
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mNet->mObjectSynchronizers.Size(); ++i )
	{
		mNet->mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, this, newFrame );
#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 14;
#endif
		buffer.EndWriting();

		if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
		{
			mesg->WriteCheckToken( NCL_ENTITYMSG );
			int pos2 = mesg->GetPos();
			mesg->Seek(pos);
			mesg->WriteUShort(howManyEntities);
			mesg->Seek(pos2);
#ifdef UDP_END_WRITING_CRASH_LOG
			EndWritingCrashNumber = 15;
#endif
			mesg->EndWriting();
			SendReliable( mesg );
            m_setTag("Net");
			mesg = new UdpMessage;
			howManyEntities = 0;
			mesg->WriteUChar( HIGHLAYER_FIRSTFRAME );	// command
			pos = mesg->GetPos();
			mesg->WriteUShort( 0 );
		}

		mesg->WriteData( &buffer );
		buffer.SeekToBegin();
		howManyEntities++;
	}

	mesg->WriteCheckToken( NCL_ENTITYMSG );
	int pos2 = mesg->GetPos();
	mesg->Seek(pos);
	mesg->WriteUShort(howManyEntities);
	mesg->Seek(pos2);
#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 16;
#endif
	mesg->EndWriting();
	SendReliable( mesg );

	newFrame->Answer->LastSynchroStringNumber = StringTransport.GetFirstUnusedStringNumber();
	newFrame->Answer->LastSynchroPONumber = POCreationCache.GetFirstUnusedNumber();
}

void NetworkPeer2::PrepareForFirstFrame( void )
{
#ifdef PEER_LOGGING
	GLog.Print( "Preparing for first frame\n" );
#endif

	// Delete all frames
	while( mFrames )
	{
		NetworkFrame* temp = mFrames;
		mFrames = mFrames->Next;
		delete temp;
	}

	// Make a new frame with number 0
    m_setTag("Net");
	NetworkFrame* newFrame = new NetworkFrame;
	newFrame->SentTime = GEngine->SysDriver.GetCurrentTimeMS();
	newFrame->Number = 0;
	newFrame->DeltaFrom = 0;
    m_setTag("Net");
	newFrame->Answer = new NetworkAnswerFrame;
#ifdef	TRANSGAMING
#ifndef	TRANSGAMING_NO_HAVOK
	newFrame->PhysicsFrame = GEngine->pcfWorld->Physics->GetPhysicsFrame();
#endif // TRANSGAMING_NO_HAVOK
#else
	newFrame->PhysicsFrame = GEngine->World->Physics->GetPhysicsFrame();
#endif
	mFrames = newFrame;
	mMaxFrameNumber = 1;
}

bool NetworkPeer2::HandleFirstFrameOnClient( UdpMessage* message )
{
#ifdef PEER_LOGGING
	GLog.Print( "Handling first frame\n" );
#endif

#ifdef _DEBUG
	unsigned char command = message->ReadUChar();
	Assert( command == HIGHLAYER_FIRSTFRAME );
#else
	message->ReadUChar();
#endif
	StringTransport.SetFirstUnsureStringNumberToReceive( 0 );
	POCreationCache.SetFirstUnsureNumberToReceive( 0 );

	NetworkFrame* answeredFrame = mFrames;
	Assert( answeredFrame );

	TempPhysFrames = 0;

	unsigned short howMany = message->ReadUShort();
	for( int i = 0; i < howMany; ++i )
	{
		ulong32 uniqueEntityIndex;
		unsigned char entityType;
		EntitySynchronizer2::ReadSynchronizer( message, uniqueEntityIndex, entityType );
		Assert( uniqueEntityIndex != (DWORD)-1 );
		Assert( !mNet->mObjectSynchronizersHash.Find( uniqueEntityIndex ) );

		EntitySynchronizer2* es = 0;
		switch( entityType )
		{
			case EntitySynchronizer2::STypeMesh:
                m_setTag("Net");
				es = (EntitySynchronizer2*) new MeshSynchronizer2(uniqueEntityIndex,mNet,0);
				break;
			case EntitySynchronizer2::STypeModel:
                m_setTag("Net");
				es = (EntitySynchronizer2*) new ModelSynchronizer2(uniqueEntityIndex,mNet,0);
				break;
			default:
				Assert( false );	// this will fire
				break;
		}

		if( !es->InterpretCreateMessage( message, this, answeredFrame ) )
		{
			GLog.Print( "Problem with interpreting create message!!\n" );
			return false;
		}
	}

	message->ReadCheckToken( NCL_ENTITYMSG );
	return true;
}

void NetworkPeer2::SendClientFrame( void )
{
	UdpMessage buffer( 65536 );	// big buffer

	// Add reliable messages to be sent to queue and forget them( preferably a bigger packet )
	mUFCReliable = 0;
	if( mMessagesToSend.Size() )
	{
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage;

		mesg->WriteUChar( HIGHLAYER_SCRIPTMESGS );
		int pos = mesg->GetPos();
		mesg->WriteUChar( 0 );

		unsigned char scriptNum = 0;
		for( int i = 0; i < mMessagesToSend.Size(); ++i )
		{
			if( mMessagesToSend[i].IsReliable )
			{
				buffer.SeekToBegin();
				buffer.WriteUShort( (unsigned short)(mMessagesToSend[i].DSM->Message->GetSize()) );
				buffer.WriteData( mMessagesToSend[i].DSM->Message );
#ifdef UDP_END_WRITING_CRASH_LOG
				EndWritingCrashNumber = 17;
#endif
				buffer.EndWriting();

				if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
				{
					Assert( scriptNum );

					// Send message
					int posTemp = mesg->GetPos();
					mesg->Seek(pos);
					mesg->WriteUChar(scriptNum);
					mesg->Seek(posTemp);
					mesg->WriteCheckToken( NCL_SCRIPTMSG );
#ifdef UDP_END_WRITING_CRASH_LOG
					EndWritingCrashNumber = 18;
#endif
					mesg->EndWriting();
					SendReliable( mesg );
#ifdef PEER_LOGGING
					GLog.Print( "Client sent reliable of size: %u\n", mesg->GetSize() );
#endif
					mUFCReliable += mesg->GetSize();

					// Prepare new message
                    m_setTag("Net");
					mesg = new UdpMessage;
					scriptNum = 0;
					mesg->WriteUChar( HIGHLAYER_SCRIPTMESGS );
					pos = mesg->GetPos();
					mesg->WriteUChar( 0 );
				}

				mesg->WriteData( &buffer );
				++scriptNum;
			}
		}

		if( scriptNum )
		{
			int posTemp = mesg->GetPos();
			mesg->Seek(pos);
			mesg->WriteUChar(scriptNum);
			mesg->Seek(posTemp);
			mesg->WriteCheckToken( NCL_SCRIPTMSG );
#ifdef UDP_END_WRITING_CRASH_LOG
			EndWritingCrashNumber = 19;
#endif
			mesg->EndWriting();
			SendReliable( mesg );
#ifdef PEER_LOGGING
			GLog.Print( "Client sent reliable of size: %u\n", mesg->GetSize() );
#endif
			mUFCReliable += mesg->GetSize();
		}
		else
			delete mesg;
	}

	// Make an unreliable packet out of player action and eventually unreliable messages

    m_setTag("Net");
	NetworkFrame* newFrame = new NetworkFrame;
#ifdef	TRANSGAMING
#ifndef	TRANSGAMING_NO_HAVOK
	newFrame->PhysicsFrame = GEngine->pcfWorld->Physics->GetPhysicsFrame();
#endif // TRANSGAMING_NO_HAVOK
#else
	newFrame->PhysicsFrame = GEngine->World->Physics->GetPhysicsFrame();
#endif
	newFrame->Number = mMaxFrameNumber++;
	newFrame->SentTime = GEngine->SysDriver.GetCurrentTimeMS();
	NetworkFrame* prevFrame = GetNewestFrame();
	if( prevFrame )
		prevFrame->Next = newFrame;
	else
		mFrames = newFrame;

	Assert( prevFrame );

#ifdef PEER_LOGGING
	GLog.Print( "Client sent frame: %u\n", newFrame->Number );
#endif

	unsigned char flags = 0;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;

	mesg->WriteUChar( HIGHLAYER_FRAME );	// command
	int pos = mesg->GetPos();	// placeholder for content flags
	mesg->WriteUChar( 0 );

	mesg->WriteULong( newFrame->Number );
	mesg->WriteULong( mHighestReliableReceivedOnClient );
	mesg->WriteUShort( (mPing>USHRT_MAX)?USHRT_MAX:mPing );

	// Write player action from the Net device into current frame ( should be the last one, but if it isn't, not our problem )

	// Move the local player
	Entity* pe = GEngine->Net->GetPlayerByIndex( mMyIndex );

#ifndef TRANSGAMING_NO_HAVOK
// Blair - We currently don't support Havok on the PSP.
	if( pe && pe->PO && pe->PO->mPlayerVariables && pe->PO->mPlayerVariables->Controller )
	{
		newFrame->Input = pe->PO->mPlayerVariables->Controller->LastMove;
		newFrame->PlayerData.GetFromPlayer( pe->PO );
#ifdef PLAYER_MOVEMENT_TRANSPORT_TESTING
		PMTTLog.Print( "Frame %d\n", newFrame->Number );
		newFrame->PlayerData.Print( PMTTLog );
#endif
	}
	else
#endif // TRANSGAMING_NO_HAVOK
		newFrame->Input.TimeDelta = (newFrame->SentTime<=prevFrame->SentTime)?0:newFrame->SentTime-prevFrame->SentTime;

	int logPos = 0;
	{
		NetworkFrame* lastFrame = GetLastAnsweredFrame();
		newFrame->DeltaFrom = newFrame->Number-lastFrame->Number;
		if( newFrame->DeltaFrom > 65000 )
		{
			Disconnect();
			delete mesg;
			return;
		}

		NetworkFrame* startFromFrame = lastFrame;
		int maxInputs = mMaxPlayerActionsPassed;
		bool ok = true;
		NetworkFrame* prevFrame;

		mesg->WriteUShort( newFrame->DeltaFrom );
		do
		{
			ok = true;
			unsigned char deltas = 0;
			// Safeguard against sending too much
			if( newFrame->DeltaFrom > maxInputs )
			{
				flags |= FRAME_FIRSTACTIONFULL;
				int dFrom = newFrame->DeltaFrom;
				while( dFrom > maxInputs )
				{
					--dFrom;
					startFromFrame = startFromFrame->Next;
				}
			}

			// Send full action every 100 frames
			if( ( newFrame->Number % 100 ) == 0 )
				flags |= FRAME_FIRSTACTIONFULL;

			// Write player actions from all unanswered frames into message
			int startDeltasPos = mesg->GetPos();
			mesg->WriteUChar( 0 );	// placeholder
			logPos = mesg->GetPos();
			if( ( flags & FRAME_FIRSTACTIONFULL ) && startFromFrame->Next )
			{
				startFromFrame->Next->Input.Write( mesg );
				startFromFrame = startFromFrame->Next;
				++deltas;
			}

			prevFrame = startFromFrame;
			for( NetworkFrame* f = startFromFrame->Next; f; prevFrame = f, f = f->Next )
			{
				f->Input.WriteDelta( mesg, &prevFrame->Input );
				++deltas;
				if( mesg->GetPos() > SAFE_UDP_PACKET_SIZE )
				{
					ok = false;
					if( maxInputs > 30 )
						maxInputs = maxInputs-20;
					else
						maxInputs = (int)ceil(maxInputs/2.0f);
					mesg->Seek( startDeltasPos );
					startFromFrame = lastFrame;
					break;
				}
			}

			if( ok )
			{
				int curPos = mesg->GetPos();
				mesg->Seek( startDeltasPos );
				mesg->WriteUChar( deltas );
				mesg->Seek( curPos );
			}
		}
		while( !ok );

//		if( newFrame->DeltaFrom > maxInputs )
//			GLog.Print( "Delta from safeguard - sending only last %d actions out of %d!\n", maxInputs, newFrame->DeltaFrom );

#ifdef PLAYER_MOVEMENT_TRANSPORT_TESTING
		PMTTLog.Print( "Frame %d\n", prevFrame->Number );
		prevFrame->Input.Print( PMTTLog );
#endif
		mUFCPlayerDeltas = mesg->GetPos()-logPos;
		logPos = mesg->GetPos();
	}

	if( mMessagesToSend.Size() && ( mConnectionState >= NetworkPeer2::EStatePlay ) )
	{
		int pos2 = 0;
		unsigned char scriptNum = 0;
		for( int i = 0; i < mMessagesToSend.Size(); ++i )
		{
			if( !mMessagesToSend[i].IsReliable )
			{
				buffer.SeekToBegin();
				buffer.WriteUShort( (unsigned short)(mMessagesToSend[i].DSM->Message->GetSize()) );
				buffer.WriteData( mMessagesToSend[i].DSM->Message );
#ifdef UDP_END_WRITING_CRASH_LOG
				EndWritingCrashNumber = 20;
#endif
				buffer.EndWriting();

				if( mesg->GetPos() + buffer.GetPos() > SAFE_UDP_PACKET_SIZE )
					break;

				if( !scriptNum )
				{
					pos2 = mesg->GetPos();	// placeholder for script messages number
					mesg->WriteUChar( 0 );
				}

				mesg->WriteData( &buffer );
				++scriptNum;
				flags |= FRAME_SCRIPT;
			}
		}

		if( scriptNum )
		{
			mesg->WriteCheckToken( NCL_SCRIPTMSG );
			int posTemp = mesg->GetPos();
			mesg->Seek(pos2);
			mesg->WriteUChar(scriptNum);
			mesg->Seek(posTemp);
		}
	}

	if( mesg->GetPos() < mMinSendFrameSize-2 )
	{
		flags |= FRAME_FILLER;
		unsigned short toFill = mMinSendFrameSize-2-mesg->GetPos();
		mesg->WriteUShort( toFill );
		mesg->FillWith(0, toFill );
	}

	mUFCUnreliable = mesg->GetPos()-logPos;

	// Enter flags
	{
		int posTemp = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUChar(flags);
		mesg->Seek(posTemp);
	}

#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 21;
#endif
	mesg->EndWriting();

#ifdef PEER_LOGGING
	GLog.Print( "Client frame size: %u ( reliable %u, player deltas %u, unreliables %u)\n", mesg->GetSize(), mUFCReliable, mUFCPlayerDeltas, mUFCUnreliable );
#endif

	// Sanity check for stupidly set bandwidth control: at least every (mMaxPlayerActionsPassed/2)th frame is sent
//	if( newFrame->DeltaFrom > (mMaxPlayerActionsPassed-5) )
//		GLog.Print( "Upstream warning: %d mesgs unacknowledged by server; max possible to send: %d\n", newFrame->DeltaFrom, mMaxPlayerActionsPassed );

	// Check if we didn't waste too much bandwidth already
	if( mNet->TestBandwidthControl() )
	{
		ulong32 tempBytesSent = mNet->mCurrentUpdateBytesSent;

		// Send message
		Send( mesg );

		// Update the bandwidth control with real number of bytes sent( including packet header etc. )
		mNet->UpdateBandwidthControl( mNet->mCurrentUpdateBytesSent - tempBytesSent );

		// Update stats
		++mStatsChunks[mStatsCursor].mFramesSent;
		++mStatsData.mFramesSent;
	}
	delete mesg;

	ClearMessages( false );	// clear all messages ( they've been sent, and reliable ones are in the reliable queue in the low layer
}

void NetworkPeer2::SetStatsNumberToAverage( int newNumber )
{
	mStatsChunks.ClearArray();
	PeerStats zeroStat( GetCurrentTime() );
	for( int i = 0; i < newNumber; ++i )
		mStatsChunks.Add( zeroStat );
	mStatsCursor = 0;
}

void NetworkPeer2::TransferVariable( String& name, String& value )
{
	UdpMessage mesg;
	mesg.WriteUChar( HIGHLAYER_VARIABLE );
	mesg.WriteString( name.AsChar() );
	mesg.WriteString( value.AsChar() );
	mesg.WriteCheckToken( NCL_VARIABLE );
#ifdef UDP_END_WRITING_CRASH_LOG
	EndWritingCrashNumber = 22;
#endif
	mesg.EndWriting();
	SendMaintenanceMessage( true, &mesg );
}

void NetworkPeer2::InterpretVariable( String& name, String& value )
{
	// Interpret internal peer variables
	if( name == "=framerate" )
	{
		int val = atoi( value.AsChar() );
		if( val > 0 )
			mServerFPS = val;
	}
	else if( name == "=lpo" )
		mLocalPlayerOptimizedEveryNFrames = atoi( value.AsChar() );
	else if( name == "=mpap" )
	{
		mMaxPlayerActionsPassed = atoi( value.AsChar() );
		if( mMaxPlayerActionsPassed < MIN_PLAYER_ACTIONS_PASSED )
			mMaxPlayerActionsPassed = MIN_PLAYER_ACTIONS_PASSED;
		else if( mMaxPlayerActionsPassed > MAX_PLAYER_ACTIONS_PASSED )
			mMaxPlayerActionsPassed = MAX_PLAYER_ACTIONS_PASSED;
	}
	else	// Pass it on higher
		mNet->InterpretVariable( mMyIndex, name, value );
}
