#error This file is obselete
/*#include "Engine.h"
#include "Net/GameSpyData.h"//*/

#ifdef OIUYTREWLKJHGFDS

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Render/View.h"
#include "Net/UdpMessage.h"
#include "Net/NetworkClient.h"
#include "Net/NetworkDevice.h"
#include "Net/IPChannel.h"
#include "Render/Video.h"
#include "Engine/World.h"
#include "Engine/InputSystem.h"
#include "Engine/BaseGame.h"
#include "Engine/PainMenu.h"

#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsObject.h"

#include "Net/GameSpy2/qr2/qr2regkeys.h"
#include "Net/GameSpy2/pt/pt.h"

//#define PREDICTION_DEBUG_LOG

extern Quaternion EulerToQuat(Vector &ea);

extern bool MoveToBytecode( String input, char* output );

NetworkDevice::NetworkDevice( void ) : mClientIndices(0), mSpectator(false), mPlayerPredictionOn(false)
,	LoadedInPreTick(false), mFramerateLock(0), mServerFramerateLock(-1), UpdatesSinceLoaded(0)
{
	Recording.SetNet( this );
}

NetworkDevice::~NetworkDevice() { NetworkClose(); };

bool NetworkDevice::NetworkServer( String mapName, String password, String interfaceToUse, unsigned short port )
{
	char buf[1024];
	if( !MoveToBytecode( password, buf ) )
	{
		GLog.Print( "Server password can contain only letters and digits!\n" );
		return false;
	}

	NetworkClose();

	mPlayerPredictionOn = false;

	int fps = GScript.Globals()["Cfg"]["ServerFPS"].IsNil()?45:GScript.Globals()["Cfg"]["ServerFPS"].GetInt();

	if( fps < 30 ) fps = 30;
	else if( fps > 120 ) fps = 120;

	if( GEngine->GameState == PCFSystem::PCFSystemStates::DedicatedServer )
		GEngine->MinSecondsPerFrame = fps?((double)1.0)/fps:0.0;
	else
		GEngine->MinSecondsPerFrame = 0.0;

	GEngine->Physics->PhysicsFPS = fps;

	GameSpyInfo.SetMode( 0 );
	GameSpyInfo.MapName = mapName;
	GameSpyInfo.Password = password;
	if( port == 0 ) port = DEFAULT_SERVER_PORT;

	bool ok = mLowLayer.Open( port, true, interfaceToUse );
	if( ok )
	{
		mLowLayer.StartServer();

		// Start synchronizing entities we have
		for( Entity* entity = GEngine->World->EntityList; entity; entity = entity->Next )
		{
			unsigned char state = entity->SynchroState();
			if( state & Entity::ESynchroEntity )
			{
#ifdef _DEBUG
				unsigned long* indexTest = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
				Assert( indexTest == 0 );
#endif
				unsigned long index = mNumberManager.GetNext();
				mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex, index );
				mUpdateClassHashByEntity.Insert( entity, entity->GIndex, 0 );
				// WARNING: all objects that started to synchronize before init of network will have a 0 update class
			}
		}

		// Track game usage
		ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);
	}
	return ok;
}

bool NetworkDevice::NetworkClientAndConnect( const char* hostname, String password, unsigned short port, bool spectator )
{
	{
		char buf[1024];
		if( !MoveToBytecode( password, buf ) )
		{
			GLog.Print( "Server password can contain only letters and digits!\n" );
			return false;
		}
	}

	NetworkClose();
	if( !mLowLayer.Open( 0 ) )
		return false;
	IpAddress addr;
	addr.Port = 0;

	if( !mLowLayer.Resolve( hostname, addr) )
	{
		String errorText = String::Sprintf( "Net Error: Couldn't resolve address: %s\n", hostname );
		GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		GLog.Print( errorText.AsChar() );
		return false;
	}

	GameSpyInfo.Password = password;
	mSpectator = spectator;
	mPlayerPredictionOn = true;
	mFramerateLock = 0;

	if( !addr.Port ) addr.Port = DEFAULT_SERVER_PORT;
	if( port ) addr.Port = port;

	mLowLayer.ConnectAsClient( addr );

	// Track game usage
	ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);

	return true;
}

bool NetworkDevice::NetworkClientAndConnect( void* hostFromGameSpy, String password, bool spectator )
{
	char buf[1024];
	if( !MoveToBytecode( password, buf ) )
	{
		GLog.Print( "Server password can contain only letters and digits!\n" );
		return false;
	}

	NetworkClose();

	if( !mLowLayer.Open( 0 ) ) return false;
	GameSpyInfo.Password = password;
	mSpectator = spectator;

	mPlayerPredictionOn = true;
	mFramerateLock = 0;

	mLowLayer.ConnectAsClient( hostFromGameSpy );

	// Track game usage
	ptTrackUsage( 0, GAMESPY_PRODUCT_ID, GAMESPY_VERSION_ID, 0, PTFalse);

	return true;
}

bool NetworkDevice::PlayRecording( const char* filename, String* errorText )
{
	// Check if file exists
	if( !Recording.FileExists( filename ) )
	{
		if( errorText )
			*errorText = "Can not open demo file!";
		return false;
	}

	// Check if we can play this file and get the client index if we can
	unsigned char clientIndex;
	bool predictionOn;
	if( !Recording.CanPlayFile( filename, clientIndex, errorText, predictionOn ) )
		return false;

	NetworkClose();

	mPlayerPredictionOn = predictionOn;

	IpAddress addr;	// any will do; not used while playing
	IPChannel* channel = mLowLayer.CreateIPChannel( addr );
	channel->mNetworkClient = MakeNewNetworkClient();
	channel->mNetworkClient->mMyIndex = clientIndex;
	channel->mNetworkClient->mNetChannel = channel;
	channel->mNetworkClient->mSpectator = false;	// for now
	if( !Recording.StartPlaying( filename, channel->mNetworkClient, errorText ) )
		return false;

	mLowLayer.State = IPDriver::IPDriverStates::CLIENT;
	channel->State = IPChannel::IPChannelStates::OK;
	channel->mNetworkClient->State = NetworkClient::EStateConnecting;
	return true;
}

void NetworkDevice::NetworkClose( void )
{
	mServerFramerateLock = -1;
	if( Recording.IsPlaying() )
	{
        if (GEngine->GameState != PCFSystem::PCFSystemStates::ShutDown)
        {
		    GScript.DoString( "Game.LevelStarted = false" );
		    GEngine->Game->ShowMenu();
		    GEngine->Video->PMenu->PauseSounds();
        }
	}

	// Remove network clients
	while( mNetworkClients.Size() )
		delete mNetworkClients[0];

	// Remove delayed script messages
	for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
		delete mDelayedScriptMessageList[i].Message;
	mDelayedScriptMessageList.Clear();

	mObjectSynchronizersHashByEntity.Clear();
	mUpdateClassHashByEntity.Clear();
	mPtrsToDelete.Clear();
	mNumberManager.Clear();

	GameSpyInfo.SetMode( 3 );
	mLowLayer.Close();
}

NetworkClient* NetworkDevice::MakeNewNetworkClient( void )
{
    m_setTag("Net");
	NetworkClient* nc = new NetworkClient( this );
	nc->mMyIndex = IsServer()?(unsigned char)mClientNumberManager.GetNext():0;
	mNetworkClients.Add( nc );
	return nc;
}

void NetworkDevice::RemoveNetworkClient( NetworkClient* client )
{
	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i] == client )
		{
			mNetworkClients.Remove( i );
			return;
		}
}

void NetworkDevice::PreTick( float timeDelta )
{
	GProfiler.SetState(NET_PRE_TICK);

	LoadedInPreTick = false;

	if( !Recording.IsPlaying() )
		mLowLayer.Tick( timeDelta );
	else
	{
		Recording.AdjustTimeToSlowdown( GEngine->TimeMultiplier );
		mLowLayer.PlayTick( timeDelta );
		Recording.UpdateOurPlayerData();
	}

	if( !IsServer() && mNetworkClients.Size() )
	{
		if( LoadedInPreTick )
		{
//			GLog.Print( "SaveTime: %d\n", mNetworkClients[0]->SaveTime );
			mNetworkClients[0]->RemoveOlderPlayerActions( mNetworkClients[0]->SaveTime );
			UpdateToLastLocalAction();
			GEngine->World->Physics->MPPredRestore();
			GEngine->World->Physics->MPPredAddRestToWorld();
		}

		// Test for receiving packets
		if( !Recording.IsPlaying() && mNetworkClients[0]->NotReceivingPacketsLately() )
			GScript.DoString( "Hud._showPacketLoss = true" );
		else
			GScript.DoString( "Hud._showPacketLoss = false" );
	}
	else
		GScript.DoString( "Hud._showPacketLoss = false" );
}

void NetworkDevice::PostTick( void )
{
	GProfiler.SetState(NET_POST_TICK);

	// What to do if we're just playing recording?
	if( Recording.IsPlaying() )
	{
		// Maintain objects on clients
		mNetworkClients[0]->MaintainObjectsOnClient();
		mNetworkClients[0]->RemoveClientZombieObjectSynchronizers();

		// Clear delayed script messages
		GProfiler.SetState(NET_OTHER);
		for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
			delete mDelayedScriptMessageList[i].Message;
		mDelayedScriptMessageList.Clear();

		GProfiler.SetState(NET_OTHER);
		DeleteUniqueIndices();

		return;
	}

	if( mLowLayer.State == IPDriver::SERVER )
	{
		// Send delayed script messages to clients, and make them compose differential information into their messages
		for( int i = 0; i < mNetworkClients.Size(); ++i )
		{
			// Send update message
			if( mNetworkClients[i]->GetState() >= NetworkClient::EStateSynchronizing )
				mNetworkClients[i]->SendUpdateMessage();

			// Send unreliable delayed script messages
			GProfiler.SetState(NET_SEND_SCRIPT_UNRELIABLE);

			if( mNetworkClients[i]->GetState() >= NetworkClient::EStatePlay )
				mNetworkClients[i]->SendServerScriptUnreliableMessages();	// Script message batching update
		}
	}
	else if( ( mLowLayer.State == IPDriver::CLIENT ) && mNetworkClients.Size() && ( mNetworkClients[0]->State >= NetworkClient::EStateSynchronizing ) )
	{
		// Maintain objects on clients
		mNetworkClients[0]->MaintainObjectsOnClient();

		GProfiler.SetState(NET_SEND_SCRIPT_RELIABLE);
		mNetworkClients[0]->SendClientScriptReliableMessages();
		mNetworkClients[0]->RemoveClientZombieObjectSynchronizers();

		// Send unreliable delayed script messages
		GProfiler.SetState(NET_SEND_SCRIPT_UNRELIABLE);
		if( mNetworkClients[0]->GetState() >= NetworkClient::EStatePlay )
			mNetworkClients[0]->SendClientScriptUnreliableMessages();	// Script message batching update
	}

	// Clear delayed script messages
	GProfiler.SetState(NET_OTHER);
	for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
		delete mDelayedScriptMessageList[i].Message;
	mDelayedScriptMessageList.Clear();

	GProfiler.SetState(NET_TICK_CHANNELS);
	mLowLayer.TickChannels();	// to speed-up sending

	GProfiler.SetState(NET_OTHER);
	DeleteUniqueIndices();

#ifndef FINAL_BUILD
	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->ReportUnsentReliables();
#endif

	static int counter = 300;
	counter--;
	if( counter == 0 )
	{
		counter = 300;
#ifndef TRANSGAMING_NO_GAMESPY
		GameSpyInfo.CheckGameMode();
#endif
	}
}

bool NetworkDevice::CanConnectNewClient( bool spectator )
{
	int maxClients = spectator?GameSpyInfo.MaxSpectators:GameSpyInfo.MaxPlayers;

	if( GEngine->GetGameState() == PCFSystem::MultiplayerServer )
	{
		if( ( spectator && mSpectator ) || ( !spectator && !mSpectator ) )
			maxClients--;
	}

	maxClients -= mLowLayer.CountAuthenticatingClients( spectator );

	// Count current spectators
	int spects = 0;
	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->mSpectator )
			spects++;

	if( spectator )
		return ( spects < maxClients );
	else
		return ( mNetworkClients.Size()-spects < maxClients );
}

void NetworkDevice::SetSynchroState( Entity* entity, unsigned char state, unsigned char updateClass, unsigned short specialClient, unsigned char specialClass )
{
	unsigned char oldState = entity->SynchroState();

	unsigned long uniqueIndex = (DWORD)-1;

	if( !( oldState & Entity::ESynchroEntity ) )
	{
		if( !(state & Entity::ESynchroEntity) )
			return;	// we don't care

		// synchro start
		uniqueIndex = mNumberManager.GetNext();
		mObjectSynchronizersHashByEntity.Insert( entity, entity->GIndex, uniqueIndex );
		mUpdateClassHashByEntity.Insert( entity, entity->GIndex, updateClass );
	}
    else
	{
		unsigned long* ui = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
        Assert( ui || mNumberManager.Empty() );
        if( ui )
       		uniqueIndex = *ui;
	}

    if( uniqueIndex != (DWORD)-1 )
	{
		if( specialClient != SERVER_ID )
		{
	    	for( int i = 0; i < mNetworkClients.Size(); ++i )
				mNetworkClients[i]->SetSynchroState( uniqueIndex, entity, state, ( mNetworkClients[i]->mMyIndex == specialClient )?specialClass:updateClass );
		}
		else
	    	for( int i = 0; i < mNetworkClients.Size(); ++i )
				mNetworkClients[i]->SetSynchroState( uniqueIndex, entity, state, updateClass );
	}

	if( !(state & Entity::ESynchroEntity) ) // synchro stop - ESynchroDelete works right here - it stops ESynchroEntity
	{
        if( !mNumberManager.Empty() )
        {
    		mObjectSynchronizersHashByEntity.Remove( entity, entity->GIndex );
    		mUpdateClassHashByEntity.Remove( entity, entity->GIndex );
    		mNumberManager.Free( uniqueIndex );
        }
	}
}

void NetworkDevice::OnActivateAnimation( Model* model, int animNumber, bool looped, float blendTime, float timeScale, bool hasMovementCurve )
{
	unsigned long* uniqueIndex = mObjectSynchronizersHashByEntity.Find( model, ((Entity*)model)->GIndex );
	if( uniqueIndex == 0 )
		return;

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->OnActivateAnimation( *uniqueIndex, animNumber, looped, blendTime, timeScale, hasMovementCurve );
}

void NetworkDevice::OnLoadAnimation( Model* model, String name, bool looped, int animIndex )
{
	unsigned long* uniqueIndex = mObjectSynchronizersHashByEntity.Find( model, ((Entity*)model)->GIndex );
	if( uniqueIndex == 0 )
		return;

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->OnLoadAnimation( *uniqueIndex, name, looped, animIndex );
}

void NetworkDevice::OnUpdatePOAction( Entity* entity, int actionType )
{
	unsigned long* uniqueIndex = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
	if( uniqueIndex == 0 )
		return;

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->OnUpdatePOAction( *uniqueIndex, actionType );
}

unsigned long NetworkDevice::GetEntityNumber( Entity* entity )
{
	if( !entity ) return (unsigned long)-1;
	unsigned long* uniqueIndex = mObjectSynchronizersHashByEntity.Find( entity, entity->GIndex );
	if( !uniqueIndex )	// fixed mesh, probably
		return (unsigned long)-1;
	return *uniqueIndex;
}

Entity* NetworkDevice::GetEntityFromNumber( unsigned long number )
{
	Assert( mNetworkClients.Size() );
	ObjectSynchronizer** os = mNetworkClients[0]->mObjectSynchronizersHash.Find( number );
	if( !os ) return 0;
	
	Assert( (*os)->UniqueNumber == number );
	Assert( (*os)->GetType() != ObjectSynchronizer::STypePO );
//	Assert( (*os)->GetType() != ObjectSynchronizer::STypeRagdoll );
	return ((EntitySynchronizer*)(*os))->GetEntity();
}

unsigned char NetworkDevice::GetClientID( void )
{
	if( mLowLayer.State == IPDriver::SERVER )
		return SERVER_ID;
	return mNetworkClients[0]->mMyIndex;
}

double NetworkDevice::Ping( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
        if( IsServer() ) return 0;
		if( mNetworkClients.Size() )
			return mNetworkClients[0]->Ping();
		else
			return 9.999f;
	}

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			return mNetworkClients[i]->Ping();
	return 9.999f;
}

void NetworkDevice::PingReset( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
		if( !IsServer() && mNetworkClients.Size() )
			mNetworkClients[0]->mNetChannel->PingReset();
	}
	else
		for( int i = 0; i < mNetworkClients.Size(); ++i )
			if( mNetworkClients[i]->GetClientIndex() == clientID )
				mNetworkClients[i]->mNetChannel->PingReset();
}

void NetworkDevice::UpdateUpdateClasses( void )
{
	if( IsServer() ) return;
	mNetworkClients[0]->UpdateUpdateClasses();
}

unsigned short NetworkDevice::GetMaxPacketSize( void )
{
	if( IsServer() ) return 1440;
	return mNetworkClients[0]->GetMaxPacketSize();
}

void NetworkDevice::UpdateMaxPacketSize( unsigned short maxPacketSize, bool sendNow )
{
	if( IsServer() || Recording.IsPlaying() ) return;
	mNetworkClients[0]->UpdateMaxPacketSize( maxPacketSize, sendNow );
}

void NetworkDevice::SendEmptyPacket( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
		if( !IsServer() && mNetworkClients.Size() ) mNetworkClients[0]->SendEmptyPacket();
	}
	else
	{
		for( int i = 0; i < mNetworkClients.Size(); ++i )
			if( mNetworkClients[i]->GetClientIndex() == clientID )
				mNetworkClients[i]->SendEmptyPacket();
	}
}


float NetworkDevice::PacketLoss( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
        if( IsServer() ) return 0.0f;
		if( mNetworkClients.Size() )
			return mNetworkClients[0]->PacketLoss();
		else
			return 0.0f;
	}

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			return mNetworkClients[i]->PacketLoss();
	return 0.0f;
}

String NetworkDevice::Stats( unsigned char clientID, bool temporary )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() ) return "Choose a client\n";
		if( mNetworkClients.Size() )
			return mNetworkClients[0]->Stats( temporary );
		else
			return "No connection to server\n";
	}

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			return mNetworkClients[i]->Stats( temporary );

	return "No client has this ID\n";
}

void NetworkDevice::DisconnectClient( unsigned char clientID )
{
	if( clientID == SERVER_ID ) return;
	if( !IsServer() ) return;

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			mNetworkClients[i]->mNetChannel->Disconnect();
}

void NetworkDevice::BanClient( unsigned char clientID )
{
	if( clientID == SERVER_ID ) return;
	if( !IsServer() ) return;

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			mNetworkClients[i]->BanMe();
}

void NetworkDevice::InputPlayerMovementAction( unsigned char clientID, PlayerMovementData* data )
{
	if( clientID == SERVER_ID )	// that would be for the local player, if we moved it this way
	{
		if ( IsServer() ) return;

		// This is for remembering actions of local client in its tick
		mNetworkClients[0]->AddPlayerAction( data );
	}
	else
		for( int i = 0; i < mNetworkClients.Size(); ++i )
			if( mNetworkClients[i]->GetClientIndex() == clientID )
				mNetworkClients[i]->AddPlayerAction( data );
}

void NetworkDevice::SendPlayerActionToServer( PlayerMovementData* action )
{
	if( IsServer() ) return;
	mNetworkClients[0]->SendPlayerAction( action );
}

bool NetworkDevice::IsSpectator( unsigned char clientID )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() ) return mSpectator;
		if( mNetworkClients.Size() )
			return mNetworkClients[0]->mSpectator;
		else
			return false;
	}

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		if( mNetworkClients[i]->GetClientIndex() == clientID )
			return mNetworkClients[i]->mSpectator;

	return false;
}

void NetworkDevice::SetSpectator( unsigned char clientID, bool spectator )
{
	if( clientID == SERVER_ID )
	{
		if( IsServer() )
			mSpectator = spectator;
		else if( mNetworkClients.Size() )
			mNetworkClients[0]->mSpectator = spectator;
	}
	else
	{
		for( int i = 0; i < mNetworkClients.Size(); ++i )
			if( mNetworkClients[i]->GetClientIndex() == clientID )
				mNetworkClients[i]->mSpectator = spectator;
	}
}

void NetworkDevice::SetPushLatency( long push )
{
	if( IsServer() ) return;
	if( mNetworkClients.Size() )
		mNetworkClients[0]->SetPushLatency( push );
}

long NetworkDevice::GetPushLatency( void )
{
	return IsServer()?0:(mNetworkClients.Size()?mNetworkClients[0]->GetPushLatency():0);
}

unsigned long NetworkDevice::SureGetPtrIndex( void* ptr, unsigned int gindex )
{
	unsigned long* ui = mObjectSynchronizersHashByEntity.Find( ptr, gindex );
	if( !ui )
	{
		unsigned long uniqueIndex = mNumberManager.GetNext();
		mObjectSynchronizersHashByEntity.Insert( ptr, gindex, uniqueIndex );
		return uniqueIndex;
	}
	else
		return *ui;
}

void NetworkDevice::DeleteUniqueIndices( void )
{
	for( int i = 0; i < mPtrsToDelete.Size(); ++i )
	{
		void* ptr = mPtrsToDelete[i].ptr;
		unsigned int gindex = mPtrsToDelete[i].num;
		unsigned long* ui = mObjectSynchronizersHashByEntity.Find( ptr, gindex );
		Assert( ui );
		if( !ui ) // XXX - na serwerze w DM_Unseen zdarzylo sie, ze ui bylo NULLEM
			GLog.Print( "Warning: in NetworkDevice::DeleteUniqueIndices() the ptr was not in number manager hash!" );
		else
		{
			mNumberManager.Free( *ui );
			mObjectSynchronizersHashByEntity.Remove( ptr, gindex );
		}
	}
	mPtrsToDelete.Clear();
}

inline void NetworkDevice::DelayedDeletePtrIndex( void* ptr, unsigned int gindex )
{
	PtrsToDelete p;
	p.ptr = ptr;
	p.num = gindex;
	mPtrsToDelete.AddUnique( p );
}

void NetworkDevice::SetUpdateClassData( unsigned char updateClass, float minSynchroDelay, float minLinearVelocityDiff, float minAngularVelocityDiff, float minPositionDiff )
{
	if( IsServer() ) return;
	Assert( mNetworkClients.Size() );
	mNetworkClients[0]->ConnectionParams.Remove( updateClass );
	if( minSynchroDelay != 0.0f )
	{
		UpdateClassParams params;
		params.MinSynchroDelay = minSynchroDelay*1000.0f;
		GEngine->SysDriver.MilisecondsToTickCount( params.MinSynchroDelay, params.MinSynchroDelayLow, params.MinSynchroDelayHigh );
		params.MinLinearVelocityDiff = minLinearVelocityDiff;
		params.MinAngularVelocityDiff = minAngularVelocityDiff;
		params.MinPositionDiff = minPositionDiff;

		mNetworkClients[0]->ConnectionParams.Insert( updateClass, params );
    }
}

bool NetworkDevice::GetUpdateClassData( unsigned char updateClass, float& minSynchroDelay, float& minLinearVelocityDiff, float& minAngularVelocityDiff, float& minPositionDiff )
{
	if( IsServer() ) return false;
	Assert( mNetworkClients.Size() );
	UpdateClassParams* updateParams = mNetworkClients[0]->ConnectionParams.Find(updateClass);
	if( updateParams && updateParams->MinSynchroDelay )
	{
		minSynchroDelay = updateParams->MinSynchroDelay*0.001f;
		minLinearVelocityDiff = updateParams->MinLinearVelocityDiff;
		minAngularVelocityDiff = updateParams->MinAngularVelocityDiff;
		minPositionDiff = updateParams->MinPositionDiff;
		return true;
	}
	return false;
}

void NetworkDevice::LoadNewMap(String levelName)
{
	// Reset clients
	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->ResetToConnecting();

	// Reset myself
	// ----------------------

	// Remove delayed script messages
	for( int i = 0; i < mDelayedScriptMessageList.Size(); ++i )
		delete mDelayedScriptMessageList[i].Message;

    mDelayedScriptMessageList.Clear();
    mObjectSynchronizersHashByEntity.Clear();
    mUpdateClassHashByEntity.Clear();
    mPtrsToDelete.Clear();
    mNumberManager.Clear();

	// If server, send load map command to clients on other side
	if( IsServer() )
    {
		for( int i = 0; i < mNetworkClients.Size(); ++i )
			mNetworkClients[i]->SendLoadMap(levelName);
        PostTick();
    }

    if (GEngine->Video)
		GEngine->Video->PMenu->LoadScreen.Activate( true, 0, "HUD/Map/sketch_mp", levelName );

	bool wasPaused = GEngine->Game->mPaused;
	GEngine->Game->mPaused = true;

//	GLog.Print( "Loading new map in LoadNewMap: %s\n", levelName.AsChar() );

    GEngine->World->FileName = "";
    GScript.DoString("Game:LoadLevel('%s')",levelName);
    bool maploadok = (!GEngine->World->FileName.IsNull());

	GEngine->Game->mPaused = wasPaused;

    if( IsServer() )
    {
        GScript.DoString("Game:AfterWorldSynchronization('%s','%s')",GEngine->World->FileName.AsChar(),GEngine->World->LevelName.AsChar());
        GScript.DoString("Game:OnPlay(true)");
    }
    else
        mNetworkClients[0]->SendMapLoaded( maploadok );

    if (GEngine->InputSys)
        GEngine->InputSys->Reset();
}

void NetworkDevice::SetStatsUpdateDelay( DWORD delay )
{
	mLowLayer.SetStatsUpdateDelay( delay );
}

void NetworkDevice::GetLastUpdateStats( unsigned long& sent, unsigned long& received )
{
	mLowLayer.GetLastUpdateStats( sent, received );
}

void NetworkDevice::ServerPhysicsTick( float timeDelta )
{
	if( !GEngine->World || ! GEngine->World->Physics )
		return;

	if( !GEngine->World->Physics->TickWillMovePhysics( timeDelta ) )
	{
		GEngine->World->Physics->Tick( timeDelta );
		return;
	}

//	DWORD tickTime = GEngine->SysDriver.GetCurrentTimeMS();

	for( int i = 0; i < mNetworkClients.Size(); ++i )
		mNetworkClients[i]->RemoveOlderPlayerActions( GEngine->World->Physics->GetPhysicsFrame() );

	for( int i = 0; i < mNetworkClients.Size(); ++i )
	{
		Entity* pe = GetPlayerByIndex( mNetworkClients[i]->mMyIndex );

		// Last action is in pe->PO
		// Get yaw && pitch
		float yaw = 0;
		float pitch = 0;

		PlayerMovementData* data = 0;
		if( mNetworkClients[i]->PlayerActions.Size() )
		{
			data = mNetworkClients[i]->PlayerActions[0];

//			mNetworkClients[i]->mLastPlayerActionLatency = (short)(tickTime-data->Latency) - (short)floor((GEngine->World->Physics->GetPhysicsFrame()-data->PhysFrame)*1000.0f/GEngine->Physics->PhysicsFPS);
//			GLog.Print( "Serv fr: %d, cli fr: %d, packet latency: %d\n", GEngine->World->Physics->GetPhysicsFrame(), data->PhysFrame, (tickTime-data->Latency) );
	
			yaw = data->Yaw;
			pitch = data->Pitch;
		}
//		else
//			GLog.Print( "No player %d actions!\n", i );

		Vector t(yaw,pitch,0);
		Quaternion q = EulerToQuat(t);
		q.Normalize();
	    Vector forward( 0,0,1 );
		forward = q.UnitInverse()*forward;
		forward.Z = -forward.Z;
		Vector u(0,pitch,0);
		q = EulerToQuat(u);
		Vector right(1,0,0);
		right = q * right;

		if( pe && pe->PO )
		{
			pe->PO->Action = data?data->Action:0;
			pe->PO->mPlayerVariables->AccumulatedDelta = 1.0f/GEngine->Physics->PhysicsFPS;
			pe->PO->MultiPlayerAction( forward, right );
		}
	}

	// Local player
	Entity* pe = GetPlayerByIndex( SERVER_ID );
	if( GEngine->Video && pe && pe->PO )
	{
		View * view = GEngine->Video->GetActiveView();
		float pitch = FDeg2Rad(view->mCamera.GetRotation().X);
		float yaw = FDeg2Rad(view->mCamera.GetRotation().Y);
		pitch = fmod( 2*F_PI + fmod(pitch,2*F_PI), 2*F_PI );
		Vector t2(yaw,pitch,0);
		Quaternion q = EulerToQuat(t2);
		q.Normalize();
		Vector forward( 0,0,1 );
		forward = q.UnitInverse()*forward;
		forward.Z = -forward.Z;
		Vector u(0,pitch,0);
		q = EulerToQuat(u);
		Vector right(1,0,0);
		right = q * right;

		pe->PO->mPlayerVariables->AccumulatedDelta = 1.0f/GEngine->Physics->PhysicsFPS;
		pe->PO->MultiPlayerAction( forward, right );
	}

	GEngine->Physics->Tick( 1.0f/GEngine->Physics->PhysicsFPS, true );
	timeDelta -= 1.0f/GEngine->Physics->PhysicsFPS;
	if( GEngine->World->Physics->TickWillMovePhysics( timeDelta ) )
		ServerPhysicsTick( timeDelta );
	else
		GEngine->World->Physics->Tick( timeDelta );
}

Entity* NetworkDevice::GetPlayerByIndex( int index )
{
	int t = GScript.GetTop();
	GScript.Call( "GetPlayerEntityByClient", 1, "%d", index );
	int gindex = GScript.GetInt( -1 );
	GScript.SetTop(t);
	return (Entity*)GEngine->World->GetEntityByIndex( gindex );
}

void NetworkDevice::ClientPhysicsTick( float timeDelta )
{
	if( !GEngine->World->Physics )
		return;

	#ifdef ENGINE_LOG
	GLog.Print("^* CPT1\n");
	#endif
	if( !mNetworkClients.Size() || !GEngine->World->Physics->TickWillMovePhysics(timeDelta) )
	{
		GEngine->Physics->Tick( timeDelta, true );
		return;
	}

	#ifdef ENGINE_LOG
	GLog.Print("^* CPT2\n");
	#endif
	Entity* pe = GetPlayerByIndex( mNetworkClients[0]->mMyIndex );

	#ifdef ENGINE_LOG
	GLog.Print("^* CPT3\n");
	#endif

	if( pe && pe->PO )
	{
		// Get yaw && pitch
		float yaw = mLastLocalAction.Yaw;
		float pitch = mLastLocalAction.Pitch;

		Vector t(yaw,pitch,0);
		Quaternion q = EulerToQuat(t);
		q.Normalize();
		Vector forward( 0,0,1 );
		forward = q.UnitInverse()*forward;
		forward.Z = -forward.Z;
		Vector u(0,pitch,0);
		q = EulerToQuat(u);
		Vector right(1,0,0);
		right = q * right;

		#ifdef ENGINE_LOG
		GLog.Print("^* CPT4\n");
		#endif

		while( GEngine->World->Physics->TickWillMovePhysics(timeDelta) )
		{
			#ifdef ENGINE_LOG
			GLog.Print("^* CPT4.1\n");
			#endif
			mLastLocalAction.PhysFrame = GEngine->World->Physics->GetPhysicsFrame();
            m_setTag("Net");
			PlayerMovementData* data = new PlayerMovementData( mLastLocalAction );
			mNetworkClients[0]->AddPlayerAction( data );	// that's for local actions
			SendPlayerActionToServer( data );

			data->Variables = *pe->PO->mPlayerVariables;
			data->VariablesIn = true;

			pe->PO->Action = mLastLocalAction.Action;
			pe->PO->mPlayerVariables->AccumulatedDelta = 1.0f/GEngine->Physics->PhysicsFPS;
			pe->PO->MultiPlayerAction( forward, right );

			timeDelta -= 1.0f/GEngine->Physics->PhysicsFPS;

			#ifdef ENGINE_LOG
			GLog.Print("^* CPT5\n");
			#endif

			if( mLastLocalAction.PhysFrame >= mNetworkClients[0]->SaveTime )	// else the server already gave us the data for this calculation
				GEngine->Physics->Tick( 1.0f/GEngine->Physics->PhysicsFPS, true );
			else if( Recording.IsPlaying() )
			{
				mLastLocalAction.PhysFrame = mNetworkClients[0]->SaveTime-1;
				GEngine->World->Physics->SetPhysicsFrame( mNetworkClients[0]->SaveTime-1 );
				GEngine->Physics->Tick( 1.0f/GEngine->Physics->PhysicsFPS, true );
			}
			else
			{
				GLog.Print( "Server %d frames before client!\n", mNetworkClients[0]->SaveTime - mLastLocalAction.PhysFrame );

				#ifdef ENGINE_LOG
				GLog.Print("^* CPT6\n");
				#endif

				// do this to maintain the illusion of fluid movement, even though it may be wrong. The Server will give us the right data soon anyway.
				if( !GEngine->World->Physics->TickWillMovePhysics(timeDelta) )
					GEngine->Physics->Tick( 1.0f/GEngine->Physics->PhysicsFPS, true );
				else
					GEngine->World->Physics->AdvancePhysicsFrame();
			}
			#ifdef ENGINE_LOG
			GLog.Print("^* CPT7\n");
			#endif

		}

	}
	else if( mNetworkClients[0]->State > NetworkClient::EStateSynchronizing )
	{
		#ifdef ENGINE_LOG
		GLog.Print("^* CPT8\n");
		#endif

		if( GEngine->World->Physics->TickWillMovePhysics(timeDelta) )
		{
			mLastLocalAction.PhysFrame = GEngine->World->Physics->GetPhysicsFrame();
			SendPlayerActionToServer( &mLastLocalAction );
		}
	}
	#ifdef ENGINE_LOG
	GLog.Print("^* CPT9\n");
	#endif


	GEngine->Physics->Tick( timeDelta, true );
	#ifdef ENGINE_LOG
	GLog.Print("^* CPT10\n");
	#endif

}

void NetworkDevice::UpdateToLastLocalAction( void )
{
	static DWORD LastSaveTime;

	if( mNetworkClients[0]->State < NetworkClient::EStatePlay ) return;

	unsigned long startFrame = mNetworkClients[0]->SaveTime;
	unsigned long endFrame = GEngine->World->Physics->GetPhysicsFrame();

//	GLog.Print( "UTTLA frames - client: %d, server: %d\n", endFrame, startFrame );

//	if( mNetworkClients[0]->SaveTime-LastSaveTime == 2 )
//		GLog.Print( "Server skipped a frame.\n" );
//	else
	if( LastSaveTime )
	{
		if( mNetworkClients[0]->SaveTime-LastSaveTime > 4 )
			GLog.Print( "Server skipped %d frames.\n", mNetworkClients[0]->SaveTime-LastSaveTime-1 );
		else
		if( mNetworkClients[0]->SaveTime-LastSaveTime == 0 )
		{
			if( GEngine->Net->UpdatesSinceLoaded )
				GLog.Print( "Server gave us a frame again! New updates: %d\n", GEngine->Net->UpdatesSinceLoaded );
			return;
		}
		else	// unlikely
		if ( mNetworkClients[0]->SaveTime-LastSaveTime < 0 )
		{
			GLog.Print( "Server went back %d frame(s)!! Bug!!\n", LastSaveTime-mNetworkClients[0]->SaveTime );
			return;
		}
	}

	LastSaveTime = mNetworkClients[0]->SaveTime;

	if( endFrame <= startFrame ) return;	// no need to update anything, server gave us the latest data possible

	if( endFrame - startFrame > 20 )
	{
		GLog.Print( "Skipped UTTLA - %d frames to calculate - our frame: %d, server: %d\n", endFrame - startFrame, endFrame, startFrame );
		return;	// no UTTLA for more than 20 frames
	}

	Entity* pe = GetPlayerByIndex( mNetworkClients[0]->mMyIndex );

	for( unsigned long i = startFrame; i < endFrame; ++i )
	{
		if( pe && pe->PO )
		{
			PlayerMovementData* data = 0;
			
			for( int count = 0; count < mNetworkClients[0]->PlayerActions.Size(); ++count )
				if( mNetworkClients[0]->PlayerActions[count]->PhysFrame == i )
				{
					data = mNetworkClients[0]->PlayerActions[count];
					break;
				}

			DWORD saveAction = pe->PO->Action;
			pe->PO->Action = data?data->Action:0;
			float yaw = data?data->Yaw:0;
			float pitch = data?data->Pitch:0;
			Vector t(yaw,pitch,0);
			Quaternion q = EulerToQuat(t);
			q.Normalize();
		    Vector forward( 0,0,1 );
			forward = q.UnitInverse()*forward;
			forward.Z = -forward.Z;
			Vector u(0,pitch,0);
			q = EulerToQuat(u);
			Vector right(1,0,0);
			right = q * right;

			// This should usually work because every action that comes into UTTLA has first been in CPT (I hope )
			if( data && data->VariablesIn )
				*pe->PO->mPlayerVariables = data->Variables;
			pe->PO->mPlayerVariables->AccumulatedDelta = 1.0f/GEngine->Physics->PhysicsFPS;
			pe->PO->MultiPlayerAction( forward, right );
			pe->PO->Action = saveAction;
		}

//		GLog.Print( "UTTLA from frame %d\n", i );
		GEngine->Physics->UpdatePredictionTick( 1.0f/GEngine->Physics->PhysicsFPS );
	}
}

DWORD NetworkDevice::GetStartTime( void )
{
	if( IsServer() ) return 0L;
	return mNetworkClients.Size()?mNetworkClients[0]->mNetChannel->mStartTime:0L;
}

// This is needed to shift start time when paused playing demo recording
void NetworkDevice::ShiftStartTime( DWORD shift, bool plus )
{
	if( IsServer() || !mNetworkClients.Size() ) return;
	if( plus )
		mNetworkClients[0]->mNetChannel->mStartTime += shift;
	else
		mNetworkClients[0]->mNetChannel->mStartTime -= shift;
}
void NetworkDevice::SetSimulatedLatency( DWORD latency ) { mLowLayer.SetSimulatedLatency( latency ); };
DWORD NetworkDevice::GetSimulatedLatency( void ) { return mLowLayer.GetSimulatedLatency(); };

void NetworkDevice::UpdateFramerateLock( void )
{
	mFramerateLock = 0;
}

// ***************************
// GameSpyData methods
// ***************************

void GameSpyData::CheckGameMode( void )
{
	// 1. Find out what it should be
	String newMode;
	switch( internalMode )
	{
	case 3:		newMode = "exiting";																break;
	case 0:		newMode = "closedwaiting";															break;
	case 1:		newMode =  GEngine->Net->CanConnectNewClient(false)?"openwaiting":"closedwaiting";	break;
	case 2:		newMode =  GEngine->Net->CanConnectNewClient(false)?"openplaying":"closedplaying";	break;
	default:	GLog.Print( "Warning: GameSpy internal mode set to unknown value!!!\n" );			return;
	}

	// 2. Find out if this calls for a mode change and change mode if it does
	if( newMode != Mode )
	{
		GLog.Print( "Changing game mode to: %s\n", newMode.AsChar() );
		Mode = newMode;
		GEngine->Net->GameSpyStateChanged();
	}
}

bool GameSpyData::GetServerValue( int keyid, int& valueInt, String& valueString )
{
	GLog.Print( "GetServerValue called by GameSpy!\n" );

	switch( keyid )
	{
	case HOSTNAME_KEY:		valueString = ServerName; return false;
	case GAMEVER_KEY:		valueString = GameVersion; return false;
/*
//	This one can't be returned when using one socket only
	case HOSTPORT_KEY:
		{
			int port = 0;
			GEngine->Net->mLowLayer.GetMyIP( &port );
			valueInt = port;
			return true;
		}
*/
	case MAPNAME_KEY:				valueString = MapName; return false;
	case NUMPLAYERS_KEY:			valueInt = GetPlayerCount(); return true;
	case NUMTEAMS_KEY:				valueInt = GetTeamCount(); return true;
	case MAXPLAYERS_KEY:			valueInt = MaxPlayers; return true;
	case GAMETYPE_KEY:				valueString = GameType; return false;
	case GAMEVARIANT_KEY:			valueInt = GameVariant; return true;
	case GAMEMODE_KEY:				valueString = Mode; return false;
	case TEAMPLAY_KEY:				valueInt = Teamplay; return true;
	case FRAGLIMIT_KEY:				valueInt = FragLimit; return true;
	case TEAMFRAGLIMIT_KEY:			valueInt = TeamFragLimit; return true;
	case TIMEELAPSED_KEY:			valueInt = TimeElapsed;	return true;
	case TIMELIMIT_KEY:				valueInt = TimeLimit; return true;

	// Our custom keys
	case GAMESPY_MAXSPECTATORS_KEY:	valueInt = MaxSpectators; return true;
	case GAMESPY_PASSWORDED_KEY:	valueInt = Password.Len()?1:0; return true;
	default:						valueString = ""; return false;
	}
}

bool GameSpyData::GetPlayerValue( int keyid, int index, int& valueInt, String& valueString )
{
	GLog.Print( "GetPlayerValue called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( Players[i].Valid )
		{
			if( count == index )
			{
				switch( keyid )
				{
				case PLAYER__KEY:	valueString = Players[i].Name; return false;
				case SCORE__KEY:	valueInt = Players[i].Score; return true;
				case PING__KEY:		valueInt = Players[i].Ping; return true;
				case TEAM__KEY:		valueInt = Players[i].Team; return true;
				case DEATHS__KEY:	valueInt = Players[i].Deaths; return true;
				default:			valueString = ""; return false;
				};
			}
			count++;
		}
	}
	valueString = "";
	return false;
}

bool GameSpyData::GetTeamValue( int keyid, int index, int& valueInt, String& valueString )
{
	GLog.Print( "GetTeamValue called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( Teams[i].Valid )
		{
			if( count == index )
			{
				switch( keyid )
				{
				case TEAM_T_KEY:	valueString = Teams[i].Name; return false;
				case SCORE_T_KEY:	valueInt = Teams[i].Score; return true;
				default:			valueString = ""; return false;
				};
			}
			count++;
		}
	}
	valueString = "";
	return false;
}

int GameSpyData::GetPlayerCount( void )
{
	GLog.Print( "GetPlayerCount called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( Players[i].Valid )
			count++;
	return count;
}

int GameSpyData::GetTeamCount( void )
{
	GLog.Print( "GetTeamCount called by GameSpy!\n" );

	int count = 0;
	for( int i = 0; i < MAX_PLAYERS; ++i )
		if( Teams[i].Valid )
			count++;
	return count;
}

#endif
