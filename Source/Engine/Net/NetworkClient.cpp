#error This file is obselete
//#include "Engine.h"

#ifdef OIUYTREWLKJHGFDS

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Render/View.h"
#include "Net/UdpMessage.h"
#include "Physics/PhysicsEngine.h"
#include "Net/NetworkClient.h"
#include "Net/NetworkDevice.h"
#include "Render/Video.h"
#include "Net/IPChannel.h"
#include "Engine/World.h"
#include "Engine/BaseGame.h"
#include "Engine/PainMenu.h"

NetworkClient::NetworkClient( NetworkDevice* device )
	: State( EStateDisconnected ), mNetChannel( NULL ), mMyDevice( device ), mMyIndex(SERVER_ID)
	, mGameSpyCDKeyIndex(-1), mHowManyUnreliableAssumedToPass(7), mLastClassesUpdate(0)
	, mLastMaxPacketUpdate(0), mSummaryUpdatePacketSize(0), mSummaryUpdatePacketCount(0)
	, mMaxUpdatePacketSize(0), mMinUpdatePacketSize(2000), mRecording(0), mSpectator(0), SaveTime(0)
	, LastSaveSent(0), mAverageLatency(0), mLatencyUpdateCounter(0), mPushLatency(0)
	, mLatencyFirstTime(true), mFrameDiff(0), mFrameDiffPlus(true), mFrameDiffInit(false)
	, mLastPlayerActionLatency(1), mLastLatencyResult(0)
{
//	Assert(StringTransport.SelfTest());
	if( !GEngine->Net->IsServer() )
	{
		mPushLatency = -abs(GScript.Globals()["Cfg"]["StoreFrames"].IsNil()?0:GScript.Globals()["Cfg"]["StoreFrames"].GetInt());
		if( -mPushLatency < -9 )
			mPushLatency = 0;	// This is a safeguard for the transition from old netcode to new one
	}
}

NetworkClient::~NetworkClient( void )
{
	for( int i = 0; i < PlayerActions.Size(); ++i ) delete PlayerActions[i];
	PlayerActions.Clear();

	if( mMyDevice->IsServer() ) mMyDevice->mClientNumberManager.Free( mMyIndex );
	RemoveObjectSynchronizers();

	// Write summary
	GLog.Print( "*********************************************\n" );
	GLog.Print( "Average update packet size: %f bytes\n", mSummaryUpdatePacketCount?((float)mSummaryUpdatePacketSize)/mSummaryUpdatePacketCount:0 );
	GLog.Print( "Max update packet size: %d\n", mMaxUpdatePacketSize );
	GLog.Print( "Min update packet size: %d\n", mMinUpdatePacketSize );
	GLog.Print( "*********************************************\n" );

	if( mGameSpyCDKeyIndex != -1 )
	{
		mMyDevice->mLowLayer.GameSpyCDKEYClientDeletion( mGameSpyCDKeyIndex );
		mGameSpyCDKeyIndex = -1;
	}

	State = EStateDisconnected;

	if( GEngine->Net->IsServer() )
		GScript.DoString("Game:AfterClientDisconnected(%d)", mMyIndex );

	if( mNetChannel )
	{
		mNetChannel->Disconnect();
		mNetChannel->mNetworkClient = NULL;	// make it forget about us
		mNetChannel = 0;
	}

	if( mRecording && !mRecording->IsIdle() )
		mRecording->Stop();

	// Delete myself from game
	mMyDevice->RemoveNetworkClient( this );
}

void NetworkClient::ResetToConnecting( void )
{
	RemoveObjectSynchronizers();
	mObjectSynchronizersToCreate.Clear();
	mObjectSynchronizersToDelete.Clear();
	mNetChannel->ClearReliableMessages();
	if( mMyDevice->IsServer() )
	{
		POCreationCache.Clear();
		StringTransport.Clear();
	}

	State = EStateConnecting;
}

void NetworkClient::SendLoadMap(String levelName)
{
	Assert( GEngine->Net->IsServer() );

	Assert( levelName.Len() );

	int t = GScript.GetTop();
	GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName.AsChar() );
	int levSize = GScript.GetInt( -1 );
	GScript.SetTop(t);

	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_LOADMAP );
	mesg.WriteString( levelName.AsChar() );

//	GLog.Print( "Sent LoadMap from SendLoadMap\n" );

	GEngine->Physics->PackTweaksIntoMessage( &mesg );

	mesg.WriteLong( levSize );
	mesg.WriteLong( GEngine->Physics->PhysicsFPS );

	mesg.WriteCheckToken( NCL_LOADMAP );
	mesg.EndWriting();
	mNetChannel->SendMaintenanceMessage( true, &mesg );
	mNetChannel->LoadingMap( true );
	State = EStateConnected;
}

void NetworkClient::SendMapLoaded( bool ok )
{
	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_MAPLOADING );
	mesg.WriteBool( ok );
	PackConnectionParamsIntoMessage( &mesg );
	mesg.WriteCheckToken( NCL_MAPLOADING );
	mesg.EndWriting();
	mNetChannel->SendMaintenanceMessage( true, &mesg );

	if( !ok )
		State = EStateDisconnected;
	State = EStateSynchronizing;
}

bool NetworkClient::ReadMaintenanceMessage( bool server, UdpMessage* message )
{
	unsigned char messageType = ( message->ReadUChar() & 0x0f );

	switch( messageType )
	{
	case NETCLIENT_OPEN:
		{
			message->ReadCheckToken( NCL_CLOPEN );
			if( !server ) return true;

			if( !mMyDevice->mLowLayer.ReadyForNetOpen( mNetChannel ) )
			{
				delete this;	// this looks risky, but from now on it only returns
				return false;
			}

			if( State > EStateConnecting )
			{
				GLog.Print( "Got NetClient_Open when already connected from client %d!! Ignoring.\n", mMyIndex );
				message->DumpToLogfile( "NetClient_Open when EStateConnecting" );
				return true;
			}

			Assert( GEngine->World->LevelName.Len() );

			// Start DemoRecording if it has been preset before connecting
			if( GEngine->mRecordAfterConnection )
			{
				// That means we're the first client to connect to server after its started
				GEngine->mRecordAfterConnection = false;
				GEngine->Net->Recording.StartRecording( GEngine->mRecordFilename.AsChar(), this );
			}

			int t = GScript.GetTop();
			GScript.Call( "Game_GetLevelMapSize", 1, "%s", GEngine->World->LevelName.AsChar() );
			int levSize = GScript.GetInt( -1 );
			GScript.SetTop(t);

			UdpMessage mesg;
			mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_LOADMAP );
            mesg.WriteString( GEngine->World->LevelName.AsChar() );
			GEngine->Physics->PackTweaksIntoMessage( &mesg );
			mesg.WriteLong( levSize );
			mesg.WriteLong( GEngine->Physics->PhysicsFPS );
			mesg.WriteCheckToken( NCL_LOADMAP );
			mesg.EndWriting();
			mNetChannel->SendMaintenanceMessage( true, &mesg );

//			GLog.Print( "Sent LoadMap in response to NetClient:Open\n" );

			mNetChannel->LoadingMap( true );
			State = EStateConnected;
		}
		return true;
	case NETCLIENT_LOADMAP:
		{
			GEngine->NetworkSpeedRegulator = 1.0f;

			POCreationCache.Clear();
			StringTransport.Clear();

            String levelName = message->ReadString();
			if( !server )
			{
				mNetChannel->mStartTime = GEngine->SysDriver.GetCurrentTimeMS();
				// reset of prediction
				SaveTime = 0;
				for( int i = 0; i < PlayerActions.Size(); ++i ) delete PlayerActions[i];
				PlayerActions.Clear();
				LastSaveSent = 0;

				if( State > EStateConnecting ) // already playing
				{
					if( State == EStateSynchronizing )
					{
						GLog.Print( "LoadMap came when I was just waiting for synchronizable entities!!!\n" );
						message->DumpToLogfile( "LoadMap when EStateSynchronizing" );
					}

					GEngine->Physics->GetTweaksFromMessage( message );

					int fileCRC = message->ReadLong();
					GEngine->Physics->PhysicsFPS = message->ReadLong();

					message->ReadCheckToken( NCL_LOADMAP );

					int t = GScript.GetTop();
					GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName.AsChar() );
					int levSize = GScript.GetInt( -1 );
					GScript.SetTop(t);

					if( fileCRC != levSize )
					{
						State = EStateDisconnected;
						String errorText = String::Sprintf( "Net Error: the map we are trying to load is different than the map on the server!" );
						GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
						GLog.Print( errorText.AsChar() );
					}
					else
					{
						mMyDevice->LoadNewMap(levelName);
						mNetChannel->LoadingMapOnClient( true );
					}
					return true;
				}

				bool wasPaused = GEngine->Game->mPaused;
				GEngine->Game->mPaused = true;

                GEngine->Video->PMenu->LoadScreen.Activate( true );

                GEngine->Physics->GetTweaksFromMessage( message );

//				GLog.Print( "Loading new map in LOADMAP: %s\n", levelName.AsChar() );

				int fileCRC = message->ReadLong();
				GEngine->Physics->PhysicsFPS = message->ReadLong();
				bool maploadok;

				int t = GScript.GetTop();
				GScript.Call( "Game_GetLevelMapSize", 1, "%s", levelName.AsChar() );
				int levSize = GScript.GetInt( -1 );
				GScript.SetTop(t);

				if( fileCRC != levSize )
				{
					State = EStateDisconnected;
					String errorText = String::Sprintf( "Net Error: the map we are trying to load is different than the map on the server!" );
					GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
					GLog.Print( errorText.AsChar() );
					maploadok = false;
				}
				else
				{
	                GEngine->World->FileName = "";
	                GScript.DoString( "Game:LoadLevel('%s')",levelName.AsChar() );

	                maploadok = (!GEngine->World->FileName.IsNull());
				}

				GEngine->Game->mPaused = wasPaused;

				if( !mMyDevice->Recording.IsPlaying() )
				{
					UdpMessage mesg;
					mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_MAPLOADING );
					mesg.WriteBool( maploadok );
					PackConnectionParamsIntoMessage( &mesg );
					mesg.WriteCheckToken( NCL_MAPLOADING );
					mesg.EndWriting();
					mNetChannel->SendMaintenanceMessage( true, &mesg );
				}
				else
					mMyDevice->Recording.AdjustTimeToNextMessage();

				if( !maploadok )
					State = EStateDisconnected;
                else
                    State = EStateSynchronizing;

				if( !maploadok )
					GEngine->Physics->GetTweaksFromScript();

				mNetChannel->LoadingMapOnClient( true );
			}
			else
			{
				// Dump tweaks
                GEngine->Physics->GetTweaksFromMessage( message );
				GEngine->Physics->GetTweaksFromScript();
				message->ReadLong();
				message->ReadLong();
			}
			message->ReadCheckToken( NCL_LOADMAP );
		}
		return true;
	case NETCLIENT_MAPLOADING:
		{
			bool ok = message->ReadBool();
			GetConnectionParamsFromMessage( message, server );
			message->ReadCheckToken( NCL_MAPLOADING );
			if( server )
			{
				mNetChannel->mStartTime = GEngine->SysDriver.GetCurrentTimeMS();
				if( State >= EStateSynchronizing )
				{
					GLog.Print( "Client said that it loaded the map when I thought it was playing!! Ignoring.\n" );
					message->DumpToLogfile( "MapLoading when state >= EStateSynchronizing" );
					return true;
				}

				mNetChannel->LoadingMap( false );

				if( !ok )
					State = EStateDisconnected;
				else
				{
					CreateAllSynchronizableEntitiesOnClient();

					GLog.Print( "Created synchro entities for client %d\n", mMyIndex );

					UdpMessage mesg;
					mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_SYNCHRONIZED );
					mesg.WriteFloat( GEngine->MinSecondsPerFrame );
					mesg.WriteCheckToken( NCL_SYNCHRO );
					mesg.EndWriting();
					mNetChannel->SendMaintenanceMessage( true, &mesg );

					State = EStateSynchronizing;
				}
			}
		}
		return true;
	case NETCLIENT_SYNCHRONIZED:
		{
			message->ReadFloat();
			message->ReadCheckToken( NCL_SYNCHRO );

			if( !server )
			{
				SaveTime = LastSaveSent = GEngine->World->Physics->GetPhysicsFrame();
				GEngine->NetworkSpeedRegulator = 1.0f;

				if( mMyDevice->Recording.IsPlaying() )
				{
					mMyDevice->Recording.CheckRestoreCaches();
					mMyDevice->Recording.AdjustTimeToNextMessage();
				}

				mNetChannel->LoadingMapOnClient( false );
				UdpMessage mesg;
				mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_SYNCHRONIZED_ACKNOWLEDGE );
				mesg.WriteCheckToken( NCL_SYNCHROACK );
				mesg.EndWriting();
				mNetChannel->SendMaintenanceMessage( true, &mesg );

				State = EStatePlay;

	            GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->World->FileName.AsChar(),GEngine->World->LevelName.AsChar());
	            GScript.DoString("Game:OnPlay(true)");
				mNetChannel->mStartTime = GEngine->SysDriver.GetCurrentTimeMS();
			}
		}
		return true;
	case NETCLIENT_SYNCHRONIZED_ACKNOWLEDGE:
		{
			message->ReadCheckToken( NCL_SYNCHROACK );
			if( server )
			{
				mFrameDiffInit = false;
				mFrameDiff = 0;
				mFrameDiffPlus = true;
				for( int i = 0; i < PlayerActions.Size(); ++i ) delete PlayerActions[i];
				PlayerActions.Clear();
				mNetChannel->mStartTime = GEngine->SysDriver.GetCurrentTimeMS();
				GScript.DoString("Game:AfterNewClientConnected(%d)", mMyIndex );

				State = EStatePlay;
			}
		}
		return true;
	case NETCLIENT_UPDATECLASSES:
		GetConnectionParamsFromMessage( message, server );
		message->ReadCheckToken( NCL_UPDATECL );
		return true;
	case NETCLIENT_UPDATEMAXPACKETSIZE:
		{
			unsigned short newMaxPacketSize = message->ReadUShort();
			message->ReadCheckToken( NCL_UPDATEMPS );
			if( server )
			{
				mNetChannel->SetMaxUdpPacketSize( newMaxPacketSize );
				GLog.Print( "New max packet size for client %d: %d\n", mMyIndex, newMaxPacketSize );
			}
		}
		return true;
	case NETCLIENT_LATENCY:
		{
			message->ReadULong();
			message->ReadLong();
			message->ReadCheckToken( NCL_LATENCY );

//			GLog.Print( "Latency to me: %d\n", mAverageLatency );
		}
		return true;
	case NETCLIENT_PLAYERACTION:
		{
			DWORD action = message->ReadULong();
			unsigned short pitch = message->ReadUShort();
			unsigned short yaw = message->ReadUShort();
			unsigned long frame = message->ReadULong();

			if( server  && ( State > EStateSynchronizing ) )
			{
                m_setTag("Net");
				PlayerMovementData* data = new PlayerMovementData;
				data->Action = action;
				data->Pitch = pitch/(65535/(F_PI*2));
				data->Yaw = yaw/(65535/F_PI) - F_PI/2;
				data->PhysFrame = frame;
				data->VariablesIn = false;
//				data->Latency = GEngine->SysDriver.GetCurrentTimeMS();	// the moment it came in

//				GLog.Print( "Action from client, action: %d, pitch: %f, yaw: %f, phys frame: %d\n",
//					data->Action, data->Pitch, data->Yaw, data->PhysFrame );

				AddPlayerAction( data );
				mLastPlayerActionLatency = data->PhysFrame-GEngine->World->Physics->GetPhysicsFrame();

				GScript.Call( "CPlayer_SetupAction",0,"%d,%d,%d,%d",mMyIndex,action,pitch,yaw );
			}
			message->ReadCheckToken( NCL_PLACTION );
		}
		return true;
	default:
		if( server )
			GLog.Print( "Unknown channel message received (server)\n" );
		else
			GLog.Print( "Unknown channel message received (client)\n" );
		message->DumpToLogfile( "Unknown channel message received" );
		return false;
	}
}

void NetworkClient::CreateAllSynchronizableEntitiesOnClient( void )
{
	MakeObjectSynchronizersForEntities();

	static UdpMessage buffer( 16384 );	// big buffer
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	// Send creation reliable messages for all entities that need it

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;
	unsigned short howManyEntities = 0;
	int pos, pos2;

	mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	// Synchronize models & meshes
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		if( ( !mObjectSynchronizers[i]->GetZombie() ) &&
			( ( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeMesh ) ||
			( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeModel ) ) )
		{
			mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, this );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}

	// Synchronize POs
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		if( ( !mObjectSynchronizers[i]->GetZombie() ) &&
			( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypePO ) &&
			( ((POSynchronizer*)mObjectSynchronizers[i])->HasPO() ) )
		{
			mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, this );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES  )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}
		
			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
/*
	// Synchronize Ragdolls
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		if( ( !mObjectSynchronizers[i]->GetZombie() ) &&
			( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeRagdoll ) &&
			( ((RagdollSynchronizer*)mObjectSynchronizers[i])->HasRagdoll() ) )
		{
			mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, this );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
*/
	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_ENTITYMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->SendReliable( mesg );
	}
	else
		delete mesg;

	// Clear objects to delete and create on clients' lists
	mObjectSynchronizersToCreate.Clear();
}

void NetworkClient::SendServerScriptUnreliableMessages( void )
{
	if( State < EStatePlay ) return;

	// Send unreliable script messages for server
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;
	unsigned short howManyEntities = 0;
	int pos, pos2;
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	static UdpMessage buffer( 16384 );	// big buffer

	howManyEntities = 0;

	mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESG ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mMyDevice->mDelayedScriptMessageList.Size(); ++i )
	{
		DelayedScriptMessage& dsm = mMyDevice->mDelayedScriptMessageList[i];

		bool ok = false;
		if( !dsm.Reliable )
		{
			if( !dsm.OneClientOnly || ( dsm.SpecialClient == mMyIndex ) )
				ok = true;
		}
		else if( !dsm.OneClientOnly && ( dsm.SpecialClient != mMyIndex ) && ( dsm.SpecialClient != SERVER_ID ) )
			ok = true;

		if( ok )
		{
			buffer.WriteUShort( (unsigned short)(dsm.Message->GetSize()) ); NPROF(NPScriptMessageSize);	// size
			buffer.WriteData( dsm.Message );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_SCRIPTMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->Send( mesg );
				mesg->SeekToBegin();
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESG ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			Assert( dsm.DeliveryReceipt == -1 );
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
	}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_SCRIPTMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->Send( mesg );
	}
	delete mesg;
}

void NetworkClient::SendClientScriptUnreliableMessages( void )
{
	if( State < EStatePlay ) return;

	// Send unreliable script messages for server
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;
	unsigned short howManyEntities = 0;
	int pos, pos2;
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	static UdpMessage buffer( 16384 );	// big buffer

	howManyEntities = 0;

	mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESG ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mMyDevice->mDelayedScriptMessageList.Size(); ++i )
	{
		DelayedScriptMessage& dsm = mMyDevice->mDelayedScriptMessageList[i];
		if( !dsm.Reliable )
		{
			buffer.WriteUShort( (unsigned short)(dsm.Message->GetSize()) ); NPROF(NPScriptMessageSize);	// size
			buffer.WriteData( dsm.Message );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_SCRIPTMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->Send( mesg );
				mesg->SeekToBegin();
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESG ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			Assert( dsm.DeliveryReceipt == -1 );
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
	}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_SCRIPTMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->Send( mesg );
	}
	delete mesg;
}

void NetworkClient::SendClientScriptReliableMessages( void )
{
	// Send reliable script messages for all entities that need it
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;
	unsigned short howManyEntities = 0;
	int pos, pos2;
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	static UdpMessage buffer( 16384 );	// big buffer

	howManyEntities = 0;

	mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESGREL ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mMyDevice->mDelayedScriptMessageList.Size(); ++i )
	{
		DelayedScriptMessage& dsm = mMyDevice->mDelayedScriptMessageList[i];

		if( dsm.Reliable )
		{
			buffer.WriteUShort( (unsigned short)(dsm.Message->GetSize()) ); NPROF(NPScriptMessageSize);	// size
			buffer.WriteData( dsm.Message );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_SCRIPTMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESGREL ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			if( dsm.DeliveryReceipt != -1 )
				mesg->DeliveryReceipts.Add( dsm.DeliveryReceipt );
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
	}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_SCRIPTMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->SendReliable( mesg );
	}
	else
		delete mesg;
}

void NetworkClient::MaintainObjectsOnClient( void )
{
	// Maintain all objects
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		mObjectSynchronizers[i]->ClientMaintain();
}

void NetworkClient::SendUpdateMessage( void )
{
	// Maintain all objects
	GProfiler.SetState(NET_SEND_UPDATE_MESSAGE);
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		mObjectSynchronizers[i]->Maintain();

	static UdpMessage buffer( 16384 );	// big buffer

	// Send creation reliable messages for all entities that need it

	GProfiler.SetState(NET_SEND_CREATE_DELETE_OTHER_MESGS);

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage;
	unsigned short howManyEntities;
	int pos, pos2;
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	howManyEntities = 0;

	mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mObjectSynchronizersToCreate.Size(); ++i )
		if( !mObjectSynchronizersToCreate[i]->GetZombie() )
		{
			mObjectSynchronizersToCreate[i]->AddCreateObjMessage( &buffer, this );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|CREATEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_ENTITYMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPCreateEntitiesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->SendReliable( mesg );
        m_setTag("Net");
		mesg = new UdpMessage;
	}
	else
		mesg->SeekToBegin();

	// Send reliable script messages for all entities that need it
	maxMessageSize = MAX_UDP_PACKET_SIZE;
//	if( State >= EStatePlay )
//	{
	if( State >= EStateSynchronizing )
	{
		GProfiler.SetState(NET_SEND_SCRIPT_RELIABLE);

		howManyEntities = 0;

		mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESGREL ); NPROF(NPIPChannelMsgTypeMarker);	// command
		pos = mesg->GetPos();
		mesg->WriteUShort( 0 );

		for( int i = 0; i < mMyDevice->mDelayedScriptMessageList.Size(); ++i )
		{
			DelayedScriptMessage& dsm = mMyDevice->mDelayedScriptMessageList[i];

			if( dsm.Reliable && ( ( dsm.SpecialClient == mMyIndex ) || ( !dsm.OneClientOnly && ( dsm.SpecialClient == SERVER_ID ) ) ) )
			{
				buffer.WriteUShort( (unsigned short)(dsm.Message->GetSize()) ); NPROF(NPScriptMessageSize);	// size
				buffer.WriteData( dsm.Message );
				buffer.EndWriting();

				if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
				{
					// Send message and prepare next one
					mesg->WriteCheckToken( NCL_SCRIPTMSG );
					pos2 = mesg->GetPos();
					mesg->Seek(pos);
					mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
					mesg->Seek(pos2);
					mesg->EndWriting();
					mNetChannel->SendReliable( mesg );
                    m_setTag("Net");
					mesg = new UdpMessage;
					howManyEntities = 0;
					mesg->WriteUChar( IPChannel::NETCLIENT|SCRIPTMESGREL ); NPROF(NPIPChannelMsgTypeMarker);	// command
					pos = mesg->GetPos();
					mesg->WriteUShort( 0 );
					maxMessageSize = MAX_UDP_PACKET_SIZE;
				}

				mesg->WriteData( &buffer );
				buffer.SeekToBegin();
				if( dsm.DeliveryReceipt != -1 )
					mesg->DeliveryReceipts.Add( dsm.DeliveryReceipt );
				howManyEntities++;
				maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
			}
		}

		if( howManyEntities )
		{
			mesg->WriteCheckToken( NCL_SCRIPTMSG );
			pos2 = mesg->GetPos();
			mesg->Seek(pos);
			mesg->WriteUShort(howManyEntities); NPROF(NPScriptMessagesCount);
			mesg->Seek(pos2);
			mesg->EndWriting();
			mNetChannel->SendReliable( mesg );
            m_setTag("Net");
			mesg = new UdpMessage;
		}
		else
			mesg->SeekToBegin();
	}

	// Send other reliable messages for all entities that want to do it
	GProfiler.SetState(NET_SEND_CREATE_DELETE_OTHER_MESGS);

	howManyEntities = 0;

	maxMessageSize = MAX_UDP_PACKET_SIZE;
	mesg->WriteUChar( IPChannel::NETCLIENT|OTHEROBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
	{
		int howMany = mObjectSynchronizers[i]->HowManyOtherMessages();
		if( howMany )
		{
			for( int j = 0; j < howMany; ++j )
				mObjectSynchronizers[i]->AddOtherObjMessage( &buffer, this, j );
			buffer.EndWriting();

			if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg->WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg->GetPos();
				mesg->Seek(pos);
				mesg->WriteUShort(howManyEntities); NPROF(NPOtherMessagesCount);
				mesg->Seek(pos2);
				mesg->EndWriting();
				mNetChannel->SendReliable( mesg );
                m_setTag("Net");
				mesg = new UdpMessage;
				howManyEntities = 0;
				mesg->WriteUChar( IPChannel::NETCLIENT|OTHEROBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
				pos = mesg->GetPos();
				mesg->WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg->WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities += howMany;
			maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
		}
	}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_ENTITYMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPOtherMessagesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->SendReliable( mesg );
        m_setTag("Net");
		mesg = new UdpMessage;
	}
	else
		mesg->SeekToBegin();

	// Send deletion reliable messages for all entities that need it
	GProfiler.SetState(NET_SEND_CREATE_DELETE_OTHER_MESGS);

	howManyEntities = 0;
	maxMessageSize = MAX_UDP_PACKET_SIZE;

	mesg->WriteUChar( IPChannel::NETCLIENT|DELETEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
	pos = mesg->GetPos();
	mesg->WriteUShort( 0 );

	for( int i = 0; i < mObjectSynchronizersToDelete.Size(); ++i )
	{
		mObjectSynchronizersToDelete[i]->AddDeleteObjMessage( &buffer, this );
		buffer.EndWriting();

		if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
		{
			// Send message and prepare next one
			mesg->WriteCheckToken( NCL_ENTITYMSG );
			pos2 = mesg->GetPos();
			mesg->Seek(pos);
			mesg->WriteUShort(howManyEntities); NPROF(NPDeleteEntitiesCount);
			mesg->Seek(pos2);
			mesg->EndWriting();
			mNetChannel->SendReliable( mesg );
            m_setTag("Net");
			mesg = new UdpMessage;
			howManyEntities = 0;
			mesg->WriteUChar( IPChannel::NETCLIENT|DELETEOBJ ); NPROF(NPIPChannelMsgTypeMarker);	// command
			pos = mesg->GetPos();
			mesg->WriteUShort( 0 );
			maxMessageSize = MAX_UDP_PACKET_SIZE;
		}

		mesg->WriteData( &buffer );
		buffer.SeekToBegin();
		howManyEntities++;
		maxMessageSize = mNetChannel->GetMaxUdpPacketSize();
	}

	if( howManyEntities )
	{
		mesg->WriteCheckToken( NCL_ENTITYMSG );
		pos2 = mesg->GetPos();
		mesg->Seek(pos);
		mesg->WriteUShort(howManyEntities); NPROF(NPDeleteEntitiesCount);
		mesg->Seek(pos2);
		mesg->EndWriting();
		mNetChannel->SendReliable( mesg );
        m_setTag("Net");
		mesg = new UdpMessage;
	}
	else
		mesg->SeekToBegin();

	// Synchronize unreliably everything else

	if( State > EStateSynchronizing )
	{
		GProfiler.SetState(NET_SEND_UPDATE_MESGS);

		maxMessageSize = MAX_UDP_PACKET_SIZE;
		howManyEntities = 0;
		mesg->WriteUChar( IPChannel::NETCLIENT|UPDATE ); NPROF(NPIPChannelMsgTypeMarker);	// command
		pos = mesg->GetPos();
		mesg->WriteUShort( 0 );

		unsigned long clientFrame = GEngine->World->Physics->GetPhysicsFrame();

		if( mFrameDiffPlus )
			clientFrame -= mFrameDiff;
		else
			clientFrame += mFrameDiff;

		int howManyUpdatePacketsInTick = 0;

		mesg->WriteULong( mFrameDiffInit?clientFrame:0 );
		mesg->WriteShort( mLastPlayerActionLatency );
//		GLog.Print( "Sent latency to client: %d\n", mLastPlayerActionLatency );
		maxMessageSize = MAX_UDP_PACKET_SIZE;

		for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		{
			mObjectSynchronizers[i]->AddUpdateObjMessage(&buffer);
			if( buffer.GetPos() )
			{
				if( mesg->GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
				{
					// Send message and prepare next one
					mesg->WriteCheckToken( NCL_ENTITYMSG );
					pos2 = mesg->GetPos();
					mesg->Seek(pos);
					mesg->WriteUShort(howManyEntities); NPROF(NPUpdateEntitiesCount);
					mesg->Seek(pos2);
					mesg->EndWriting();

					// Update summary
					mSummaryUpdatePacketCount++;
					mSummaryUpdatePacketSize += mesg->GetSize();
					if( mesg->GetSize() > mMaxUpdatePacketSize ) mMaxUpdatePacketSize = mesg->GetSize();
					if( mesg->GetSize() < mMinUpdatePacketSize ) mMinUpdatePacketSize = mesg->GetSize();

					++howManyUpdatePacketsInTick;
					mNetChannel->Send( mesg );
					delete mesg;
                    m_setTag("Net");
					mesg = new UdpMessage;
					howManyEntities = 0;
					mesg->WriteUChar( IPChannel::NETCLIENT|UPDATE ); NPROF(NPIPChannelMsgTypeMarker);	// command
					pos = mesg->GetPos();
					mesg->WriteUShort( 0 );
					unsigned long clientFrame = GEngine->World->Physics->GetPhysicsFrame();

					if( mFrameDiffPlus )
						clientFrame -= mFrameDiff;
					else
						clientFrame += mFrameDiff;

					mesg->WriteULong( mFrameDiffInit?clientFrame:0 );
					mesg->WriteShort( mLastPlayerActionLatency );
//					GLog.Print( "Sent latency to client: %d\n", mLastPlayerActionLatency );
				}

				buffer.EndWriting();
				mesg->WriteData( &buffer );
				buffer.SeekToBegin();
				howManyEntities++;
			}
		}

		if( howManyEntities || !howManyUpdatePacketsInTick )
		{
			mesg->WriteCheckToken( NCL_ENTITYMSG );
			pos2 = mesg->GetPos();
			mesg->Seek(pos);
			mesg->WriteUShort(howManyEntities); NPROF(NPUpdateEntitiesCount);
			mesg->Seek(pos2);
			mesg->EndWriting();

			// Update summary
			mSummaryUpdatePacketCount++;
			mSummaryUpdatePacketSize += mesg->GetSize();
			if( mesg->GetSize() > mMaxUpdatePacketSize ) mMaxUpdatePacketSize = mesg->GetSize();
			if( mesg->GetSize() < mMinUpdatePacketSize ) mMinUpdatePacketSize = mesg->GetSize();

			++howManyUpdatePacketsInTick;
			mNetChannel->Send( mesg );
		}

		if( howManyUpdatePacketsInTick > 1 )
			GLog.Print( "More than one update packet in tick!!\n" );
	}

	delete mesg;

	GProfiler.SetState(NET_SEND_UPDATE_MESSAGE);

	// Clear objects to delete and create on clients' lists
	mObjectSynchronizersToCreate.Clear();
	mObjectSynchronizersToDelete.Clear();

	// Delete other (sent) messages
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		mObjectSynchronizers[i]->ClearOtherMessages();

	RemoveZombieObjectSynchronizers();
}

bool NetworkClient::ReadBinaryMessage( UdpMessage* message )
{
	unsigned char command = message->ReadUChar() & 0x0f;	// get the command type

	if( mMyDevice->mLowLayer.State == IPDriver::SERVER )
	{
		// *****************
		// Server commands
		// *****************

		switch( command )
		{
		case SCRIPTMESG:
		case SCRIPTMESGREL:		return HBC_ScriptMessageBatch( message );
		default:				GLog.Print( "Unknown server command: %d!!\n", command );	return false;
		}
	}
	else
	{
		// *****************
		// Client commands
		// *****************

		switch( command )
		{
		case SCRIPTMESG:
		case SCRIPTMESGREL:		
		{
			#ifdef PACKET_LOG
				GLog.Print( "*P: HBC_ScriptMessageBatch \n" );
			#endif
			return HBC_ScriptMessageBatch( message );
		}
		case OTHEROBJ:
		case CREATEOBJ:
		case UPDATE:
		case DELETEOBJ:			
		{		
			#ifdef PACKET_LOG
			GLog.Print( "*P: HBC_EntityMessage: %d\n", command );
			#endif
			return HBC_EntityMessage( message, command );
		}
		default:				GLog.Print( "Unknown client command: %d!!\n", command ); return false;
		}
	}
}

bool NetworkClient::HBC_ScriptMessageBatch( UdpMessage* message )
{
	static UdpMessage mesg;

	unsigned short howMany = message->ReadUShort();
	Assert( howMany < 300 );
	for( int i = 0; i < howMany; ++i )
	{
		unsigned short size = message->ReadUShort();

		if( State >= EStateSynchronizing )
		{
			mesg.SeekToBegin();
			mesg.WriteData( message, size );
			mesg.EndWriting();
			mesg.SeekToBegin();

			int oldState = GProfiler.GetState();
			GProfiler.SetState(NET_GSCRIPTS);
			if( mMyDevice->IsServer() )
				GScript.Call( "GetNetEvent", 0, "%p,%d", &mesg, mMyIndex );
			else
				GScript.Call( "GetNetEvent", 0, "%p,%d", &mesg, SERVER_ID );
			GProfiler.SetState(oldState);
		}
		message->ScrapData( size );
	}
	message->ReadCheckToken( NCL_SCRIPTMSG );
	return true;
}

inline ObjectSynchronizer* NetworkClient::SureGetEntitySynchronizer( unsigned long uniqueEntityIndex, unsigned char entityType )
{
	ObjectSynchronizer** os = mObjectSynchronizersHash.Find( uniqueEntityIndex );

	if( os )
		return *os;

	ObjectSynchronizer* os2 = 0;

    m_setTag("Net");

	switch( entityType )
	{
	case ObjectSynchronizer::STypeMesh:		os2 = (ObjectSynchronizer*) new MeshSynchronizer(uniqueEntityIndex,this,0); break;
	case ObjectSynchronizer::STypeModel:	os2 = (ObjectSynchronizer*) new ModelSynchronizer(uniqueEntityIndex,this,0); break;
	case ObjectSynchronizer::STypePO:		os2 = (ObjectSynchronizer*) new POSynchronizer(uniqueEntityIndex,this,0,0); break;
//	case ObjectSynchronizer::STypeRagdoll:	os2 = (ObjectSynchronizer*) new RagdollSynchronizer(uniqueEntityIndex,this,0,0); break;
	default:								Assert( entityType == ObjectSynchronizer::STypeMesh ); break;	// this will fire!
	}
	return os2;
}

inline bool NetworkClient::HBC_EntityMessage( UdpMessage* message, unsigned char command )
{
	unsigned short howMany = message->ReadUShort();

	if( command == UPDATE )
	{
		DWORD newSave = message->ReadULong();
		mLastPlayerActionLatency = message->ReadShort();

		if( newSave == 0 )
		{
			LastSaveSent = GEngine->World->Physics->GetPhysicsFrame();
			AddLatencyResult( mLastPlayerActionLatency );
		}
		else
		{
			if( LastSaveSent < newSave )
				AddLatencyResult( mLastPlayerActionLatency );
			LastSaveSent = newSave;
		}
	}

	for( int i = 0; i < howMany; ++i )
	{
		unsigned long uniqueEntityIndex;
		unsigned char entityType;
		ObjectSynchronizer::ReadSynchronizer( message, uniqueEntityIndex, entityType );
		#ifdef PACKET_LOG
		GLog.Print( "*P: Entity: %d,%d\n", uniqueEntityIndex, entityType );
		#endif
		if( uniqueEntityIndex == -1 )
		{
			GLog.Print( "Bug manifestation: client received uniqueEntityIndex == -1, not given by server!! working around it.  This may cause more bugs!\n" );
			message->DumpToLogfile( "uniqueEntityIndex == -1" );
			return false;
		}
		ObjectSynchronizer* os = SureGetEntitySynchronizer( uniqueEntityIndex, entityType );

		if( os->GetType() != entityType )
		{
			GLog.Print( "Bug: problem in interpreting entity message! This may cause more bugs!\n" );
			GLog.Print( "Message to entity of type %d, found entity of type %d\n", entityType, os->GetType() );
			message->DumpToLogfile( "Entity type different that the one the message was addressed to!" );
			return false;
		}

		bool ok = os->InterpretMessage( message, command, this );
		if( !ok )
		{
			GLog.Print( "Bug: problem in interpreting entity message! This may cause more bugs!\n" );
			message->DumpToLogfile( "InterpretMesage() returned false" );
			return false;
		}
	}

	message->ReadCheckToken( NCL_ENTITYMSG );

	return true;
}

void NetworkClient::SetSynchroState( unsigned long uniqueIndex, Entity* entity, unsigned char state, unsigned char updateClass )
{
    if( State < EStateSynchronizing ) return;

	unsigned char oldState = entity->SynchroState();
	if( ( oldState & Entity::ESynchroEntity ) && !( state & Entity::ESynchroEntity ) )
	{
		// Remove synchro object for entity and give a hint to delete it on other side
		ObjectSynchronizer** os = mObjectSynchronizersHash.Find( uniqueIndex );
        if( os )
			(*os)->SetZombie();
		return;
	}

	if( !(oldState & Entity::ESynchroEntity) && (state & Entity::ESynchroEntity) )
		CreateObjectForEntity( uniqueIndex, entity, updateClass );

	// I don't inform synchronizer of the change - it will know the next time I tell him to maintain itself
}

ObjectSynchronizer* NetworkClient::CreateObjectForEntity( unsigned long uniqueIndex, Entity* entity, unsigned char updateClass )
{
	ObjectSynchronizer* os = 0;

    m_setTag("Net");
	switch( entity->Type )
	{
	case Entity::ETypeMesh:		os = (ObjectSynchronizer*) new MeshSynchronizer( uniqueIndex, this, (WorldMesh*)entity); break;
	case Entity::ETypeModel:	os = (ObjectSynchronizer*) new ModelSynchronizer( uniqueIndex, this, (Model*)entity ); break;
	default:					break;	// We don't serve your kind here...
	}

	if( os )
	{
		os->UpdateClass = updateClass;
		UpdateClassParams* updateParams = ConnectionParams.Find(updateClass);
		if( updateParams && updateParams->MinSynchroDelay )
		{
			DWORD curTimeLow = 0;
			DWORD curTimeHigh = 0;
			GEngine->SysDriver.GetPureTickCount( curTimeLow, curTimeHigh );
			DWORD diffLow, diffHigh;
			DWORD minLow = updateParams->MinSynchroDelayLow;
			if( !minLow ) minLow = 1;
			DWORD minHigh = updateParams->MinSynchroDelayHigh;
			if( !minHigh ) minHigh = 1;
			DWORD percentLow = rand()%minLow;
			DWORD percentHigh = rand()%minHigh;
			GetTickCountDifference( curTimeLow, curTimeHigh, percentLow, percentHigh, diffLow, diffHigh );
			os->SetLastUpdateTime( diffLow, diffHigh );
		}
	}

	return os;
}

void NetworkClient::MakeObjectSynchronizersForEntities( void )
{
	World* world = GEngine->World;
	if( !world ) return;	// no entities to make objects for

	for( Entity* entity = world->EntityList; entity; entity = entity->Next )
	{
		unsigned char state = entity->SynchroState();
		if( state & Entity::ESynchroEntity )
		{
			unsigned long* index = GEngine->Net->mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
			Assert( index != 0 );
			unsigned char* updateClass = GEngine->Net->mUpdateClassHashByEntity.Find( entity, entity->GIndex );
			Assert( updateClass != 0 );
			CreateObjectForEntity( *index, entity, *updateClass );
		}
	}
}

void NetworkClient::RemoveObjectSynchronizers( void )
{
	while( mObjectSynchronizers.Size() )
		delete mObjectSynchronizers[0];
	mObjectSynchronizers.Clear();
	mObjectSynchronizersHash.Clear();
}

void NetworkClient::RemoveZombieObjectSynchronizers( void )
{
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		if( mObjectSynchronizers[i]->GetZombie() )
		{
			if( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypePO )
			{
				Assert( ((POSynchronizer*)mObjectSynchronizers[i])->mObject ); 
				Assert( ((POSynchronizer*)mObjectSynchronizers[i])->EntityGIndex ); 
				mMyDevice->DelayedDeletePtrIndex( ((POSynchronizer*)mObjectSynchronizers[i])->mObject, ((POSynchronizer*)mObjectSynchronizers[i])->EntityGIndex );
//				GLog.Print( "DelayedDeletePtrIndex %d\n", mObjectSynchronizers[i]->UniqueNumber );
			}
/*
			else if( mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeRagdoll )
			{
				Assert( ((RagdollSynchronizer*)mObjectSynchronizers[i])->mObject ); 
				Assert( ((RagdollSynchronizer*)mObjectSynchronizers[i])->EntityGIndex ); 
				mMyDevice->DelayedDeletePtrIndex( ((RagdollSynchronizer*)mObjectSynchronizers[i])->mObject, ((RagdollSynchronizer*)mObjectSynchronizers[i])->EntityGIndex );
//				GLog.Print( "DelayedDeletePtrIndex %d\n", mObjectSynchronizers[i]->UniqueNumber );
			}
*/
			delete mObjectSynchronizers[i];
			i--;
		}
}

void NetworkClient::RemoveClientZombieObjectSynchronizers( void )
{
	for( int i = 0; i < mObjectSynchronizers.Size(); ++i )
		if( mObjectSynchronizers[i]->GetZombie() )
		{
			delete mObjectSynchronizers[i];
			i--;
		}
}

void NetworkClient::OnActivateAnimation( unsigned long uniqueIndex, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve )
{
	ObjectSynchronizer** os = mObjectSynchronizersHash.Find( uniqueIndex );
	if( os )
	{
		Assert( ((EntitySynchronizer*)(*os))->GetType() == ObjectSynchronizer::STypeModel );
		if( mObjectSynchronizersToCreate.Find( *os ) == -1 )
			((ModelSynchronizer*)(*os))->AddAnimActivation( animNumber, looped, blendTime, timeScale, hasMovementCurve );
	}
}

void NetworkClient::OnLoadAnimation( unsigned long uniqueIndex, String name, bool looped, int animIndex )
{
	ObjectSynchronizer** os = mObjectSynchronizersHash.Find( uniqueIndex );
	if( os )
	{
		Assert( ((EntitySynchronizer*)(*os))->GetType() == ObjectSynchronizer::STypeModel );
		if( mObjectSynchronizersToCreate.Find( *os ) == -1 )
			((ModelSynchronizer*)(*os))->AddAnimLoad( name, looped, animIndex );
	}
}

void NetworkClient::OnUpdatePOAction( unsigned long uniqueIndex, int actionType )
{
	ObjectSynchronizer** os = mObjectSynchronizersHash.Find( uniqueIndex );
	if( os )
	{
		POSynchronizer* posynch = ((EntitySynchronizer*)(*os))->GetPO();
		if( posynch )
			posynch->AddActionUpdate( actionType );
	}
}

void NetworkClient::UpdateUpdateClasses( void )
{
	// Anti-flooding control
	DWORD curTime = GetCurrentTime();
	if( curTime - mLastClassesUpdate < 1.0f ) return;
	mLastClassesUpdate = curTime;

	// Sending message
	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_UPDATECLASSES );
	PackConnectionParamsIntoMessage( &mesg );
	mesg.WriteCheckToken( NCL_UPDATECL );
	mesg.EndWriting();
	mNetChannel->SendMaintenanceMessage( true, &mesg );
}

unsigned short NetworkClient::GetMaxPacketSize( void )
{
	return mNetChannel?mNetChannel->GetMaxUdpPacketSize():MAX_UDP_PACKET_SIZE;
}

void NetworkClient::UpdateMaxPacketSize( unsigned short mps, bool sendNow )
{
	// Anti-flooding control
	DWORD curTime = GetCurrentTime();
	if( curTime - mLastMaxPacketUpdate < 1.0f ) return;
	mLastMaxPacketUpdate = curTime;

	// Updating
	Assert( mNetChannel );
	mNetChannel->SetMaxUdpPacketSize( mps );

	if( sendNow )
	{
		// Sending message
		UdpMessage mesg;
		mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_UPDATEMAXPACKETSIZE );
		mesg.WriteUShort( mNetChannel->GetMaxUdpPacketSize() );
		mesg.WriteCheckToken( NCL_UPDATEMPS );
		mesg.EndWriting();
		mNetChannel->SendMaintenanceMessage( true, &mesg );
	}
}

inline void NetworkClient::PackConnectionParamsIntoMessage( UdpMessage* message )
{
	int pos = message->GetPos();
	message->WriteUChar(0);	// placeholder for number of connection params
	int written = 0;

	UpdateClassParams* updateParams;
	for( int i = 0; i < 256; ++i )
	{
		updateParams = ConnectionParams.Find(i);
		if( updateParams && updateParams->MinSynchroDelay )
		{
			message->WriteUChar(i);
			message->WriteULong( updateParams->MinSynchroDelay );
			message->WriteFloat( updateParams->MinLinearVelocityDiff );
			message->WriteFloat( updateParams->MinAngularVelocityDiff );
			message->WriteFloat( updateParams->MinPositionDiff );
			written++;
		}
	}

	// Latest addition
	message->WriteUShort( mNetChannel->GetMaxUdpPacketSize() );

	int pos2 = message->GetPos();
	message->Seek( pos );
	message->WriteUChar(written);
	message->Seek( pos2 );
}

inline void NetworkClient::GetConnectionParamsFromMessage( UdpMessage* message, bool update )
{
	if( update ) ConnectionParams.Clear();
	unsigned char howMany = message->ReadUChar();

	for( int i = 0; i < howMany; ++i )
	{
		unsigned char updateClass = message->ReadUChar();
		UpdateClassParams params;

		params.MinSynchroDelay = message->ReadULong();
		GEngine->SysDriver.MilisecondsToTickCount( params.MinSynchroDelay, params.MinSynchroDelayLow, params.MinSynchroDelayHigh );
		params.MinLinearVelocityDiff = message->ReadFloat();
		params.MinAngularVelocityDiff = message->ReadFloat();
		params.MinPositionDiff = message->ReadFloat();

		if( update ) ConnectionParams.Insert( updateClass, params );
	}

	// Latest addition
	unsigned short maxUdpPacketSize = mNetChannel->GetMaxUdpPacketSize();
	if( message->GetPos() <= message->GetSize() - (int)sizeof(unsigned short) )
	{
		maxUdpPacketSize = message->ReadUShort();
		mNetChannel->SetMaxUdpPacketSize( maxUdpPacketSize );
	}

	GLog.Print( "New connection params set for client %d\n", mMyIndex );
}

inline double NetworkClient::Ping( void ) { return mNetChannel?mNetChannel->GetPing():9.999f; }
inline float NetworkClient::PacketLoss( void ) { return mNetChannel?mNetChannel->GetPacketLoss():0.0f; }
inline void NetworkClient::SendEmptyPacket( void ) { if(mNetChannel) mNetChannel->Send(NULL); };
inline String NetworkClient::Stats( bool temporary ) { return mNetChannel?mNetChannel->Stats( temporary ):"No connection\n"; };
inline void NetworkClient::ReportUnsentReliables( void ) { if( mNetChannel ) mNetChannel->ReportUnsentReliables(); };
void NetworkClient::BanMe( void ) { mMyDevice->mLowLayer.BanHash( this->mGameSpyHash ); }

void NetworkClient::RemoveOlderPlayerActions( long serverFrameTime )
{
	int count;
	for( count = 0; count < PlayerActions.Size(); ++count )
		if( PlayerActions[count]->PhysFrame > (unsigned long)serverFrameTime )
			break;

	count--;										// always leave last one, for interpolation
	if( count <= 0 ) return;						// nothing to remove

	for( int i = 0; i < count; ++i ) delete PlayerActions[i];
	for( int i = 0; i < PlayerActions.Size()-count; ++i ) PlayerActions[i] = PlayerActions[i+count];
	for( int i = 0; i < count; ++i ) PlayerActions.RemoveLast();
}

void NetworkClient::SendPlayerAction( PlayerMovementData* action )
{
	Assert( !GEngine->Net->IsServer() );

	if( State > EStateSynchronizing )
	{
		UdpMessage mesg;
		mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_PLAYERACTION );
		mesg.WriteULong( action->Action );
		mesg.WriteUShort( floor( action->Pitch*(65535/(F_PI*2) ) ) );
		mesg.WriteUShort( floor((action->Yaw+F_PI/2)*(65535/F_PI)) );
		mesg.WriteULong( action->PhysFrame );
		mesg.WriteCheckToken( NCL_PLACTION );
		mesg.EndWriting();
		mNetChannel->SendMaintenanceMessage( false, &mesg );
//		GLog.Print( "Action to server, action: %d, pitch: %f, yaw: %f, phys frame: %d\n",
//			action->Action, action->Pitch, action->Yaw, action->PhysFrame );
	}
}

int NetworkClient::AddPlayerAction( PlayerMovementData* data )
{
	if( mMyDevice->IsServer() )
	{
//		GLog.Print( "Player actions: %d\n", PlayerActions.Size() );
		if( !mFrameDiffInit )	// First action - assumming its for the next phys tick
		{
			unsigned long ourPhys = GEngine->World->Physics->GetPhysicsFrame();
			if( ourPhys >= data->PhysFrame )
			{
				mFrameDiff = ourPhys-data->PhysFrame;
				mFrameDiffPlus = true;
			}
			else
			{
				mFrameDiff = data->PhysFrame-ourPhys;
				mFrameDiffPlus = false;
			}

			mFrameDiffInit = true;
			for( int i = 0; i < PlayerActions.Size(); ++i ) delete PlayerActions[i];
			PlayerActions.Clear();
		}

		if( mFrameDiffPlus )
			data->PhysFrame += mFrameDiff;
		else
			data->PhysFrame -= mFrameDiff;
	}

	if( !PlayerActions.Size() || ( data->PhysFrame > PlayerActions[PlayerActions.Size()-1]->PhysFrame ) )
	{
		PlayerActions.Add( data );
		return PlayerActions.Size()-1;
	}
	else
	{
		for( int i = PlayerActions.Size()-2; i >= 0; --i )
		{
			if( data->PhysFrame > PlayerActions[i]->PhysFrame )
			{
				PlayerActions.Insert( data, i+1 );
				return i+1;
			}
			else if( data->PhysFrame == PlayerActions[i]->PhysFrame )
			{
				delete PlayerActions[i];
				PlayerActions[i] = data;
				return i;
			}
		}

		PlayerActions.Insert( data, 0 );
		return 0;
	}
}

void NetworkClient::AddLatencyResult( long result )
{
	long pl = -mPushLatency;

	if( result < -30+pl )
		GEngine->NetworkSpeedRegulator = 2.0;
	if( result < -10+pl )
		GEngine->NetworkSpeedRegulator = 1.5;
	else if( result < 1+pl )
		GEngine->NetworkSpeedRegulator = 1.07;
	else if( result > 30+pl )
		GEngine->NetworkSpeedRegulator = 0.5;
	else if( result > 10+pl )
		GEngine->NetworkSpeedRegulator = 0.75;
	else if( result > 2+pl )
		GEngine->NetworkSpeedRegulator = 0.93;
	else
		GEngine->NetworkSpeedRegulator = 1.0f;

	if( ( result < -10+pl ) || ( result > 10+pl ) )
		GLog.Print( "Client before server by %d frames, time multiplier: %f\n", result, GEngine->NetworkSpeedRegulator );
/*
	static DWORD mLastTime = 0;
	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
//	GLog.Print( "Result: %d, regulator: %f, frame time: %d\n", result, GEngine->NetworkSpeedRegulator, curTime-mLastTime );
	GLog.Print( "Result: %d, regulator: %f, frame time: %d, display: %d, physics: %d, server: %d\n",
		result, GEngine->NetworkSpeedRegulator, curTime-mLastTime, GEngine->World->Frame, GEngine->World->Physics->GetPhysicsFrame(), LastSaveSent );
	mLastTime = curTime;
*/
	mLastLatencyResult = result;
}

inline bool NetworkClient::NotReceivingPacketsLately( void ) { return mNetChannel->NotReceivingPacketsLately(); }

#endif
