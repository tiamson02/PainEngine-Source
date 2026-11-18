#error This file is obselete
#include "Engine.h"


NetClient::NetClient( EngineGame* game )
	: State( EStateDisconnected ), mNetChannel( NULL ), mMyPlayer( NULL ), mMyGame( game )
	, MaxMessageSize( MAX_UDP_PACKET_SIZE ), mCameraRotation(0,0,0), MaxSynchroDistance( 1000000.0f )
	, MinTimeBetweenPackets( 0.01f ), mLastAction(0), mActionBeforeLast(0), PlayerName("")
	, mGameSpyCDSDKIndex(-1), mScriptMessagesSentUnreliable(0), mScriptMessagesSentReliable(0)
	, mScriptMessagesReceived(0), mScriptMessagesSentBytesUnreliable(0), mScriptMessagesSentBytesReliable(0)
	, mScriptMessagesReceivedBytes(0), mLastReceivedScriptMesg(0), mLastSentScriptMesg(0)
	, Score(0), Frags(0), Deaths(0)
{
#ifdef TRANSGAMING
	mLastTimePacketSent = GEngine->SysDriver.GetCurrentTime();
#else
	mLastTimePacketSent = (float)GEngine->SysDriver.GetCurrentTime();
#endif
}

NetClient::~NetClient( void )
{
	GLog.Print( "*********** NET CLIENT STATS ************************************\n" );
	GLog.Print( "    Script packets unreliable sent: %d, bytes = %d\n", mScriptMessagesSentUnreliable, mScriptMessagesSentBytesUnreliable );
	GLog.Print( "    Script packets   reliable sent: %d, bytes = %d\n", mScriptMessagesSentReliable, mScriptMessagesSentBytesReliable );
	GLog.Print( "    Script packets        received: %d, bytes = %d\n", mScriptMessagesReceived, mScriptMessagesReceivedBytes );
	GLog.Print( "*****************************************************************\n" );

	if( mGameSpyCDSDKIndex != -1 )
	{
		GEngine->Net->GameSpyCDKEYClientDeletion( mGameSpyCDSDKIndex );
		mGameSpyCDSDKIndex = -1;
	}

	if( State != EStateDisconnected )
	{
		State = EStateDisconnected;

		if( mMyGame->mClient )
		{
			if( mMyGame->mClient->Mode & Client::EModeMultiplClient )	// we're the client
			{
				if( mMyPlayer && mMyPlayer != mMyGame->mClient->Player )
				{
					if( mMyGame->mClient->Player )
						mMyGame->mWorld.DeleteEntity( mMyGame->mClient->Player );
					mMyGame->mClient->Player = mMyPlayer;
					mMyGame->mWorld.RemoveEntity( mMyPlayer );
				}
				mMyGame->mClient->Mode &= ~Client::EModeMultiplClient;
			}
			else	// we're the server, or unconnected client
			{
				if( mMyPlayer && mMyPlayer != mMyGame->mClient->Player )
				{
					GScript.Call("Game_RemovePlayer",0,"%p",mMyPlayer);
					mMyGame->mWorld.DeleteEntity( mMyPlayer );					
				}
			}
		}
		mMyPlayer = 0;
	}

	if( mNetChannel )
	{
		GLog.Print( "Disconnecting due to destruction of NetClient here\n" );
		mNetChannel->Disconnect();
		mNetChannel->mNetClient = NULL;	// make it forget about us
		// don't delete; IPDriver Tick will do this next time
	}
	mNetChannel = 0;

	// Delete myself from game
	mMyGame->RemoveNetClient( this );
}

void NetClient::HandleDisconnect( void )
{
	State = EStateDisconnected;

	if( mMyGame->mClient )
	{
		if( mMyGame->mClient->Mode & Client::EModeMultiplClient )	// we're the client
		{
			if( mMyPlayer && mMyPlayer != mMyGame->mClient->Player )
			{
				if( mMyGame->mClient->Player )
					mMyGame->mWorld.DeleteEntity( mMyGame->mClient->Player );
				mMyGame->mClient->Player = mMyPlayer;
				mMyGame->mWorld.RemoveEntity( mMyPlayer );
			}
			mMyGame->mClient->Mode &= ~Client::EModeMultiplClient;
		}
		else	// we're the server, or unconnected client
		{
			if( mMyPlayer && mMyPlayer != mMyGame->mClient->Player )
			{
				GScript.Call("Game_RemovePlayer",0,"%p",mMyPlayer);
				mMyGame->mWorld.DeleteEntity( mMyPlayer );
			}
		}
	}
	mMyPlayer = 0;

	delete this;
}

bool NetClient::HandleClientConnected( void )
{
	if( mMyGame->GetNumberOfNetClients() < mMyGame->mMaxNetClients )
	{
		State = EStateFileListGetting;	// waiting for synchro, to be true
		return true;	// accept everything
	}
	else
		return false;	// we need no more clients
}

// Client side
void NetClient::HandleConnectedToServer( void )
{
	State = EStateFileListGetting;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str = "/getfilelist\n";
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /getfilelist\n");
#endif
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );
}

// Server side
void NetClient::HandleRequestedFilelist( void )
{
	char buffer[20];

	DynamicArray<String> filelist;
	DynamicArray<int> filesizes;
	mMyGame->FillFilenamesList( filelist, filesizes );

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str = "/filelist ";	// / means command
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /filelist\n");
#endif
	sprintf( buffer, "%d ", filelist.Size() );
	str += buffer;
	for( int i = 0; i < filelist.Size(); ++i )
	{
		str += filelist[i];
		str += " ";
		sprintf( buffer, "%d ", filesizes[i] );
		str += buffer;
		str += " ";
	}
	mesg->WriteString( str.AsChar() );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );
	State = EStateFileSynchronizing;
}

// Client side
void NetClient::HandleFilelist( DynamicArray<String>& filenames )
{
	// ignore filenames[0] - it contains "/filelist"

	// get number of files you need from filenames[1]
	int howManyFiles = atoi( filenames[1].AsChar() );

	mFileNames.ClearArray();
	for( int i = 0; i < howManyFiles; ++i )
	{
		mFileNames.Add( filenames[2*i+2] );
		mFileSizes.Add( atoi( filenames[2*i+3].AsChar() ) );
	}

	State = EStateFileSynchronizing;

	HandleFileCompleted( "" );	// to start downloading files
}

bool NetClient::ReadMessage( UdpMessage* message )
{
	World& world = mMyGame->mWorld;
	unsigned char command = message->ReadUChar() & 0x0f;	// get the client command
	if( GEngine->Net->IsServer() )
	{
		switch( command )
		{
		case CLICOM_ACTION:
			{
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print( "Received action packet\n" );
#endif
				DWORD plActionMinus2 = message->ReadULong();
				DWORD plActionMinus1 = message->ReadULong();
				DWORD plActionNow = message->ReadULong();
				mCameraRotation.X = message->ReadFloat();
				mCameraRotation.Y = message->ReadFloat();

				if( !mMyPlayer->PO )
					return true;	// read the message; ignore it

				// Execute backupped actions
				Vector forward = mMyPlayer->PO->GetForwardVec();		// test this one!!!
				Vector right = mMyPlayer->PO->GetRightVec();
				if( mNetChannel->PacketsDroppedBeforeThisOne() > 1 )
					mMyPlayer->PO->PlayerAction( forward, right, plActionMinus2, 0 );
				if( mNetChannel->PacketsDroppedBeforeThisOne() > 0 )
					mMyPlayer->PO->PlayerAction( forward, right, plActionMinus1, 0 );

				// Set pawn orientation
				Vector up(0,-1,0);
				Quaternion q( (float)FDeg2Rad(mCameraRotation.X+180.0), up );
				mMyPlayer->PO->ZeroAngularVelocity();
				mMyPlayer->PO->SetRotQuaternion( q );

				// Execute current action
				forward = mMyPlayer->PO->GetForwardVec();		// test this one!!!
				right = mMyPlayer->PO->GetRightVec();
				mMyPlayer->PO->PlayerAction( forward, right, plActionNow, 0 );

				if (plActionNow & (1<<EInputFire))
					GScript.Call("Player_ServerAction",0,"%p,%d",mMyPlayer,(1<<EInputFire));
				
				if (plActionNow & (1<<EInputAltFire))
					GScript.Call("Player_ServerAction",0,"%p,%d",mMyPlayer,(1<<EInputAltFire));
			}
			return true;
		case CLICOM_SCRIPTMESG:
			{
				mScriptMessagesReceived++;
				mScriptMessagesReceivedBytes += message->GetSize();
				unsigned long t = message->ReadULong();
				char buf[256];
				unsigned char size = message->ReadUChar();
				message->ReadData( buf, size );
				if( ( t != 0xFFFFFFFF ) && ( t <= mLastReceivedScriptMesg ) )
					GLog.Print( "Warning: reliable message came twice!\n" );
				else
				{
					if( State == EStatePlaying )
					{
                        m_setTag("Net");
						UdpMessage* mesg = new UdpMessage();
						mesg->WriteData( buf, size );
						mesg->EndWriting();
						mesg->SeekToBegin();
						mMyGame->HandleScriptMessage( mesg );
					}
				}
				if( t != 0xFFFFFFFF )
					mLastReceivedScriptMesg = t;
			}
			return true;
		default:
			GLog.Print( "Unknown server command!!\n" );
			return false;
		}
	}
	else
	{
		switch( command )
		{
		case CLICOM_FULLENTITYSTATE:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving full entity state (%d)\n", index );
#endif

				// Get the message contents
				unsigned long type = (unsigned long)message->ReadUChar();
				String filename = message->ReadString();
				String entityName = message->ReadString();
				float scale = message->ReadFloat();

				Entity* entity = 0;
				// Check if we have to remove old entity
				if( world.EntityNetIndices[index] )
				{
					if( !world.EntityNetIndices[index]->NetEqual( (unsigned char)type, filename, entityName, scale ) )
					{
						if( mMyGame->mClient->Player == world.EntityNetIndices[index] )
							mMyGame->mClient->Player = 0;
						world.DeleteEntity( world.EntityNetIndices[index] );
					}
					entity = world.EntityNetIndices[index];
				}

				// Check if we have to make a new one
				if( !entity )
				{
					GLog.Print( "Remote create entity!\n" );
					// Create new entity with those contents
					entity = world.CreateEntity( type, filename.AsChar(), entityName.AsChar(), scale, false, false, index );
					if( !entity )
					{
						GLog.Print( "Was unable to create entity! type = %d, filename = %s, entityName = %s, scale = %f, index = %d\n",
							type, filename.AsChar(), entityName.AsChar(), scale, index );
						return false;
					}
				}
				world.AddEntity( entity );
				entity->ReadRestOfFullFromMessage( message, ConnectionType );
			}
			return true;
		case CLICOM_FULLENTITYSTATEONCE:
			{
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving full entity state once\n" );
#endif

				// Get the message contents
				unsigned long type = (unsigned long)message->ReadUChar();
				String filename = message->ReadString();
				String entityName = message->ReadString();
				float scale = message->ReadFloat();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Type = %d, Filename = %s, Entity = %s, Scale = %f\n", type, filename.AsChar(), entityName.AsChar(), scale );
#endif

				Entity* entity = 0;
				GLog.Print( "Remote create entity once!\n" );
				// Create new entity with those contents
				entity = world.CreateEntity( type, filename.AsChar(), entityName.AsChar(), scale, true, true );
				if( !entity )
				{
					GLog.Print( "Was unable to create entity! type = %d, filename = %s, entityName = %s, scale = %f\n",
						type, filename.AsChar(), entityName.AsChar(), scale );
					return false;
				}
				entity->SetSynchronizeOnce();
				world.AddEntity( entity );
				entity->ReadRestOfFullFromMessage( message, ConnectionType );
			}
			return true;
		case CLICOM_POCREATE:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving PO create message (%d)\n", index );
#endif
				Entity* ent = mMyGame->mWorld.GetEntityByNetNumber( index );
				if( !ent )
					return false;

				// Get the message contents
				ent->SetPartialFromMessage( message, ConnectionType );
				DWORD poState = message->ReadULong();
				bool player = message->ReadBool();
				float scale = message->ReadFloat();

				world.AddEntity( ent );
				if( ent == mMyPlayer )
				{
					ent->Flags |= Entity::EEntAlwaysVisible|Entity::EEntNoDraw;		// don't draw player entity
					State = EStatePlaying;	// that will cause the start of acquiring unreliable script mesgs
				}
				ent->CreatePhysicsObject( poState, player, scale );
			}
			return true;
		case CLICOM_ENTITYUPDATE:
			{
//				mCameraRotation.X = message->ReadFloat();
//				mCameraRotation.Y = message->ReadFloat();
				unsigned char howMany = message->ReadUChar();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving entity update (%d items)\n", howMany );
#endif
				bool hasPlayerVel = message->ReadBool();
				Vector playerVel(0,0,0);
//				Vector playerPos(0,0,0);
				if( hasPlayerVel )
				{
					playerVel = message->ReadVector();
//					playerPos = message->ReadVector();
				}

				for( int i = 0; i < howMany; ++i )
				{
					unsigned short entityIndex = message->ReadUpTo32767Optimized();
					int pos = message->GetPos();
					unsigned char recordLength = message->ReadUChar();
					Entity* ent = mMyGame->mWorld.GetEntityByNetNumber( entityIndex );
					if( !ent )
						message->ScrapData( recordLength-1 );
					else if( !ent->SetPartialFromMessage( message, ConnectionType ) )
					{
						message->Seek( pos );
						message->ScrapData( recordLength );
					}
				}

				if( mMyPlayer && mMyPlayer->PO && hasPlayerVel )
				{
					mMyPlayer->PO->SetVel(playerVel);
//					t = playerPos.X; playerPos.X = playerPos.Z; playerPos.Z = t;
//					mMyPlayer->PO->SetPos(playerPos);
				}
			}
			return true;
		case CLICOM_ENTITYDELETE:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving entity delete (%d)\n", index );
#endif
				Entity* ent = world.GetEntityByNetNumber( index );
				Assert( ent );
				world.DeleteEntity( ent );
				world.EntityNetIndices[index] = 0;
				Assert( ent != mMyPlayer );
			}
			return true;
		case CLICOM_PODELETE:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print("Receiving physical object delete (%d)\n", index );
#endif
				Entity* ent = world.GetEntityByNetNumber( index );
				Assert( ent );
				ent->RemovePhysicsObject(false);
				world.RemoveEntity( ent );		// new hack for the bugfix - unsure
				if( ent == mMyPlayer )
					HandleStartPlaying();
			}
			return true;
		case CLICOM_ANIMACTIVATE:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
				Entity* ent = world.GetEntityByNetNumber( index );
				Assert( ent );
				Assert( ent->Type == Entity::ETypeModel );
				unsigned char c = message->ReadUChar();
				bool looped = ((c&0x80)!=0);
				unsigned char animNumber = (c & 0x7f);
				float blendTime = (float)(((double)message->ReadUShort())*10.0/65536.0);
				float timeScale = message->ReadFloat();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
				GLog.Print( "Received ANIMACTIVATE - index: %d, number: %d, %s, blend time: %f, timeScale: %f\n", index, animNumber, looped?"looped":"not looped", blendTime, timeScale );
#endif
				((Model*)ent)->ActivateAnimation( animNumber, looped, timeScale, blendTime );
			}
			return true;
		case CLICOM_ANIMLOAD:
			{
				unsigned short index = message->ReadUpTo32767Optimized();
				Entity* ent = world.GetEntityByNetNumber( index );
				Assert( ent );
				Assert( ent->Type == Entity::ETypeModel );
				String animName = message->ReadString();
				bool looped = message->ReadBool();
				unsigned char animIndex = message->ReadUChar();
				((Model*)ent)->LoadAnimation( animName.AsChar(), looped, animIndex );
			}
			return true;
		case CLICOM_SCRIPTMESG:
			{
				mScriptMessagesReceived++;
				mScriptMessagesReceivedBytes += message->GetSize();
				unsigned long t = message->ReadULong();
				char buf[256];
				unsigned char size = message->ReadUChar();
				message->ReadData( buf, size );
				if( ( t != 0xFFFFFFFF ) && ( t <= mLastReceivedScriptMesg ) )
					GLog.Print( "Warning: reliable message came twice!\n" );
				else
				{
					if( State == EStatePlaying )
					{
                        m_setTag("Net");
						UdpMessage* mesg = new UdpMessage();
						mesg->WriteData( buf, size );
						mesg->EndWriting();
						mesg->SeekToBegin();
						mMyGame->HandleScriptMessage( mesg );
					}
				}
				if( t != 0xFFFFFFFF )
					mLastReceivedScriptMesg = t;
			}
			return true;
		default:
			GLog.Print( "Unknown client command!!\n" );
			return false;
		}
	}
}

// Client side
void NetClient::HandleFileCompleted( String filename )
{
	// Remove completed filename from the list
	if( !(filename.IsNull()) )
	{
		for( int i = 0; i < mFileNames.Size(); ++i )
		{
			if( mFileNames[i] == filename )
			{
				mFileNames.Remove(i);
				mFileSizes.Remove(i);
				break;
			}
		}
	}

	while( mFileNames.Size() )
	{
		unsigned long size = GFileMan->FileSize( mFileNames[0].AsChar() );
		if( size != mFileSizes[0] )	// file not found or bad size
		{
			// download next file
			mNetChannel->StartDownload( mFileNames[0].AsChar() );
			return;
		}
		else
		{
			mFileNames.Remove(0);
			mFileSizes.Remove(0);
		}
	}

	// If we're here, it means that every file has been synchronized!!!
	State = EStateEntitySynchronizing;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str = "/startstreaming\n";
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /startstreaming\n");
#endif
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );
}

// Client side
void NetClient::HandleFileNotFound( String filename )
{
	GLog.Print( "Wow! Server said to synchronize a file which is not on a server? Shouldn't ever happen, unless you download out of band.\n" );
	mNetChannel->Disconnect();	// hard penalty
}

void NetClient::HandleStartStreamingFromClient( void )
{
	// First tell which map to load
	SendNameOfMapToLoad( mMyGame->mWorld.GetFirstPackageName(), mMyGame->mWorld.Scale, mMyGame->mWorld.PhysicsFile );
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /loadmap\n");
#endif
}

void NetClient::SendStateMessage( int howMany, UdpMessage* data )
{
#ifdef TRANSGAMING
	float ct = GEngine->SysDriver.GetCurrentTime();
#else
	float ct = (float)GEngine->SysDriver.GetCurrentTime();
#endif
	if( ct - mLastTimePacketSent < MinTimeBetweenPackets )
		return;	// no need to send more often than that
	mLastTimePacketSent = ct;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_ENTITYUPDATE;
	mesg->WriteUChar( command );					// command

	mCameraRotation = mMyGame->mClient->mCamera.Rotation;
	mesg->WriteUChar( (unsigned char)howMany );
	*mesg += *data;
	mesg->EndWriting();
	mNetChannel->Send( mesg );						// send unreliable message

}

void NetClient::SendFullEntityState( int netIndex )
{
	World& world = mMyGame->mWorld;
	if( world.EntityNetIndices[netIndex] != NULL )
	{
		// Make reliable message for that entity and put it into our message queue to send
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage();

		unsigned char command = 0x80;
		command |= (BINCOM_MESSAGE<<4);
		command |= CLICOM_FULLENTITYSTATE;
		mesg->WriteUChar( command );					// command
		mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
		world.EntityNetIndices[netIndex]->WriteFullIntoMessage( mesg, ConnectionType );
		mesg->EndWriting();
		mNetChannel->AddReliableMessageToSend( mesg );			// send message
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
		GLog.Print("Sent full entity state (%d)\n", netIndex );
#endif
	}
}

void NetClient::SendFullEntityStateOnce( Entity* entity )
{
	// Make reliable message for that entity and put it into our message queue to send
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_FULLENTITYSTATEONCE;
	mesg->WriteUChar( command );					// command
	entity->WriteFullIntoMessage( mesg, ConnectionType );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );			// send message
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent full entity state once\n" );
#endif
}

void NetClient::SendPOCreate( int netIndex, DWORD poType, bool player, float scale )
{
	World& world = mMyGame->mWorld;
	if( world.EntityNetIndices[netIndex] != NULL )
	{
		// Make reliable message for that entity and put it into our message queue to send
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage();

		unsigned char command = 0x80;
		command |= (BINCOM_MESSAGE<<4);
		command |= CLICOM_POCREATE;
		mesg->WriteUChar( command );					// command

		Entity* ent = mMyGame->mWorld.GetEntityByNetNumber( (unsigned short)netIndex );
		mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
		ent->WritePartialIntoMessage( mesg, ConnectionType );
		mesg->WriteULong( poType );
		mesg->WriteBool( player );
		mesg->WriteFloat( scale );
		mesg->EndWriting();
		mNetChannel->AddReliableMessageToSend( mesg );			// send message
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
		GLog.Print("Sent PO creation (%d)\n", netIndex );
#endif
	}
}

void NetClient::SendDeleteEntityInfo( int netIndex )
{
	// Make reliable message for that entity and put it into our message queue to send
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_ENTITYDELETE;

	mesg->WriteUChar( command );					// command
	mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );			// send message
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent delete entity order (%d)\n", netIndex );
#endif
}

void NetClient::SendDeletePOInfo( int netIndex )
{
	// Make reliable message for that entity and put it into our message queue to send
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_PODELETE;

	mesg->WriteUChar( command );					// command
	mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );			// send message
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent PO deletion (%d)\n", netIndex );
#endif
}

void NetClient::HandleStartPlaying( void )
{
	State = EStateWaitingForSpawn;

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str;
	str = str.Sprintf( "/spawnme %s", PlayerName );
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );

}

void NetClient::HandleSpawnMe( DynamicArray<String>& tokens )
{
	// Save player name
	PlayerName = "";
	for( int i = 1; i < tokens.Size(); ++i )
	{
		PlayerName += tokens[i];
		PlayerName += String(" ");
	}
	if( ( PlayerName.Len() > 0 ) && ( PlayerName[PlayerName.Len()-1] == ' ' ) )
		PlayerName = PlayerName.SubStr( PlayerName.Size()-1 );
	if( PlayerName.IsNull() )
		PlayerName = "Unknown";

//	PlayerName = ( tokens.Size() > 1 )?tokens[1]:"Unknown";

	// Make physical representation of the virtual camera here
	mCameraRotation = mMyGame->mClient->mCamera.Rotation;
	if( mMyPlayer )
	{
		mMyPlayer->CreatePhysicsObject( EPawnBodyPlayer, true, 1.0f );
		mMyPlayer->Flags &= ~Entity::EEntNoDraw;		// start drawing player entity
	}
	
	GScript.Call("Game_AddPlayer",0,"%p,%s,%s",mMyPlayer,PlayerName.AsChar(),PLAYER_MODEL);
}

void NetClient::SendPlayerEntityIndex( unsigned short index )
{
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str;
	str = str.Sprintf( "/yourentity %d\n", index );
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /yourentity\n");
#endif
	mNetChannel->AddReliableMessageToSend( mesg );
}

void NetClient::HandleEntityIndexSetting( unsigned char index )
{
	Entity* e = mMyGame->mWorld.GetEntityByNetNumber( index );
	Assert( e );
	mMyPlayer = e;
	GScript.Call("Game_SetPlayerEntity",0,"%p",mMyPlayer);
	e->Flags |= Entity::EEntAlwaysVisible|Entity::EEntNoDraw;		// don't draw player entity
	mMyGame->mClient->Mode |= Client::EModeMultiplClient;
}

void NetClient::SendNameOfMapToLoad( String mapName, float scale, String physicsName )
{
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str;
	str = str.Sprintf( "/loadmap %s %f %s\n", mapName.AsChar(), scale, physicsName.AsChar() );
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print( "Sent %s\n", str );
#endif
	mNetChannel->AddReliableMessageToSend( mesg );
	mNetChannel->LoadingMap( true );
}

void NetClient::HandleLoadMap( DynamicArray<String>& tokens )
{
	Assert( tokens.Size()>=2 );
	String mapName = "../Data/Maps/";
	mapName += tokens[1];
	float scale = (tokens.Size()>=3)?(float)atof( tokens[2].AsChar() ):1.0f;
	String physicsName = (tokens.Size()>=4)?tokens[3]:"";

	State = EStateMapLoading;
	mMyGame->mClient->LoadingScreen();
	mMyGame->mWorld.Release();
	Assert(strstr(mapName.AsChar(),".dat"));
	bool maploadok = true;

    TRACE("NetClient::HandleLoadMap(): loading mesh pack from '%s'\n", mapName.AsChar());
	if( !mMyGame->mWorld.LoadMeshPak( mapName.AsChar() ) )
		maploadok = false;
	else
	{
		mMyGame->mWorld.PhysicsFile = physicsName;
		mMyGame->mWorld.Init( scale );
	}

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str;
	if( maploadok )
		str = "/maploading ok";
	else
		str = "/maploading error";
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent %s\n", str );
#endif
	mNetChannel->AddReliableMessageToSend( mesg );
}

void NetClient::HandleMapLoadReport( bool ok )
{
	mNetChannel->LoadingMap( false );
	if( !ok )
	{
		HandleDisconnect();
		return;
	}

	// Make a new player entity, invisible, without physical body, connected to NetClient.
	// NetClient should delete this entity from World on exit, on client AND on server side.
	Vector spawn = mMyGame->mWorld.GetRandomSpawnPoint();
	mMyPlayer = mMyGame->mWorld.CreateEntity( Entity::ETypeModel, PLAYER_MODEL, "", 0.115f );	// on the server side
	mMyPlayer->SetPositionAndRotation( spawn,Quaternion(0,Vector(0,1,0)) );
	mMyPlayer->Flags |= Entity::EEntNoDraw;		// don't draw player entity
	mMyGame->mWorld.AddEntity( mMyPlayer );

	// Warning: it does some thing that are normally done by world tick only!
	for( int i = 0; i < 256; ++i )
	{
		Entity* ent = mMyGame->mWorld.GetEntityByNetNumber( (unsigned short)i );
        if( !ent )
			continue;
		if( ent->FirstTimeSynchro() )
		{
			mMyGame->CreateEntityInClients( (unsigned char)i );
			SendFullEntityState( i );
			ent->Synchroed();
		}
		else
			SendFullEntityState( i );
	}

	// Now synchronize all entities which have to be synchronized once
    for (Entity* entity=mMyGame->mWorld.EntityList; entity; entity=entity->Next)
		if( entity->ToBeSynchronizedOnce() )
		{
			if( entity->FirstTimeSynchro() )
			{
				mMyGame->CreateEntityInClientsOnce( entity );
				SendFullEntityStateOnce( entity );
				entity->Synchroed();
			}
			else
				SendFullEntityStateOnce( entity );
		}

	SendPlayerEntityIndex( mMyPlayer->GetSynchroNumber() );	// tell client side NetClient which player entity is his

	State = EStateEntitySynchronizing;

	// This will be deciphered after all entities were read
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	String str = "/startplaying\n";
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print("Sent /startplaying\n");
#endif
	mesg->WriteString( str.AsChar() );	// / means command
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );
}

void NetClient::MakeSureMyPlayerPtrDel( Entity* entity )
{
	if( entity == mMyPlayer )
		mMyPlayer = 0;
}

void NetClient::SendActionPacket( DWORD action, Vector camRotation )
{
#ifdef TRANSGAMING
	float ct = GEngine->SysDriver.GetCurrentTime();
#else
	float ct = (float)GEngine->SysDriver.GetCurrentTime();
#endif
	//	Done that way to speed up the acknowledgements of server reliable messages from client
//	if( ct - mLastTimePacketSent < MinTimeBetweenPackets )
//		return;	// no need to send more often than that
	mLastTimePacketSent = ct;
#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print( "Sent action packet\n" );
#endif

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_ACTION;
	mesg->WriteUChar( command );					// command
	mesg->WriteULong( mActionBeforeLast );
	mesg->WriteULong( mLastAction );
	mesg->WriteULong( action );
	mActionBeforeLast = mLastAction;
	mLastAction = action;
	mesg->WriteFloat( camRotation.X );
	mesg->WriteFloat( camRotation.Y );
	mesg->EndWriting();
	mNetChannel->Send( mesg );						// send unreliable message
}

bool NetClient::ReadTextMessage( bool server, DynamicArray<String> & tokens )
{
	if( server )	// server side messages
	{
		if( tokens[0] == "/getfilelist" )	// getfilelist only on server side
			HandleRequestedFilelist();
		else if( tokens[0] == "/maploading" )	// getfilelist only on server side
		{
			bool ok;
			Assert( tokens.Size() > 1 );
			if( tokens[1] == "ok" )
				ok = true;
			else if( tokens[1] == "error" )
				ok = false;
			else
			{
				ok = false;
				Assert( ok );
			}

			HandleMapLoadReport( ok );
		}
		else if( tokens[0] == "/startstreaming" )	// startstreaming only on server side
			HandleStartStreamingFromClient();
		else if( tokens[0] == "/spawnme" )		// spawnme only on server side
			HandleSpawnMe( tokens );
		else
		{
			GLog.Print( "Unknown channel message received (server)\n" );
			return false;
		}
	}
	else			// client side messages
	{
		if( tokens[0] == "/filelist" )	// filelist only on client side
			HandleFilelist( tokens );
		else if( tokens[0] == "/yourentity" )		// entity index setting only on client side
			HandleEntityIndexSetting( (unsigned char)atoi(tokens[1].AsChar()) );
		else if( tokens[0] == "/loadmap" )			// map loading only on client side
			HandleLoadMap( tokens );
		else if( tokens[0] == "/startplaying" )	// startplaying only on client side
			HandleStartPlaying();
		else
		{
			GLog.Print( "Unknown channel message received (client)\n" );
			return false;
		}
	}

	return true;
}

void NetClient::SendActivateAnimationInfo( int netIndex, int animNumber, bool looped, float blendTime, float timeScale )
{
	// Make reliable message for that entity and put it into our message queue to send
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_ANIMACTIVATE;

#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print( "Sent ANIMACTIVATE - index: %d, number: %d, %s, blend time: %f, time scale: %f\n", netIndex, animNumber, looped?"looped":"not looped", blendTime, timeScale );
#endif

	mesg->WriteUChar( command );					// command
	mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
	Assert( animNumber < 128 );
	unsigned char c = (unsigned char)animNumber;
	if( looped )
		c |= 0x80;
	mesg->WriteUChar( c );
	Assert( blendTime < 10.0 );	// we shouldn't need blend time higher that that, right?
	mesg->WriteUShort( (unsigned short)floor(blendTime*65536.0/10.0) );
	mesg->WriteFloat( timeScale );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );			// send message
}

void NetClient::SendLoadAnimationInfo( int netIndex, String animName, bool looped, int animIndex )
{
	// Make reliable message for that entity and put it into our message queue to send
    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();

	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_ANIMLOAD;

#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print( "Sent ANIMLOAD - index: %d, name: %s, %s\n", netIndex, animName.AsChar(), looped?"looped":"not looped" );
#endif

	mesg->WriteUChar( command );					// command
	mesg->WriteUpTo32767Optimized( (unsigned short)netIndex );
	mesg->WriteString( animName.AsChar() );
	mesg->WriteBool( looped );
	mesg->WriteUChar( (unsigned char)animIndex );
	mesg->EndWriting();
	mNetChannel->AddReliableMessageToSend( mesg );			// send message
}

void NetClient::SetConnectionType( int connectionType )
{
	String connType;
	ConnectionType = connectionType;
	switch( connectionType )
	{
	case ConnModem56K:
		connType = "Modem 56K";
		MinTimeBetweenPackets = 0.1f;
		MaxMessageSize = 700;
		MaxSynchroDistance = 50.0f;
		break;
	case ConnISDN112K:
		connType = "ISDN 112K";
		MinTimeBetweenPackets = 0.05f;
		MaxMessageSize = 1400;
		MaxSynchroDistance = 50.0f;
		break;
	case ConnADSL256K:
		connType = "ADSL 256K";
		MinTimeBetweenPackets = 0.03f;
		MaxMessageSize = 1400;
		MaxSynchroDistance = 100.0f;
		break;
	default:
	case ConnLANT3_1M:
		connType = "LAN T3 > 1M";
		MinTimeBetweenPackets = 0.00f;	// flood it!!! :)))
		MaxMessageSize = 1400;
		MaxSynchroDistance = 1000000.0f;
		break;
	}

	GLog.Print( "*****************************************************************\n" );
	GLog.Print( "                     CONNECTION PARAMETERS\n" );
	GLog.Print( " Type: %s\n", connType );
	GLog.Print( " Minimal Time Between Server Packets: %f seconds\n", MinTimeBetweenPackets );
	GLog.Print( " Maximal Message Size: %d bytes\n", MaxMessageSize );
	GLog.Print( " Maximal Synchronization Distance: %f meters\n", MaxSynchroDistance );
	GLog.Print( "*****************************************************************\n" );

}

void NetClient::SendScriptMessage( UdpMessage* message, bool reliable )
{
	Assert( mNetChannel );

    m_setTag("Net");
	UdpMessage* mesg = new UdpMessage();
	unsigned char command = 0x80;
	command |= (BINCOM_MESSAGE<<4);
	command |= CLICOM_SCRIPTMESG;

#ifdef LOG_PROTOCOL_DEBUG_MESSAGES
	GLog.Print( "Sent Script Message\n" );
#endif

	mesg->WriteUChar( command );					// command
	mesg->WriteULong( reliable?++mLastSentScriptMesg:0xFFFFFFFF );
	Assert( message->GetSize() < 256 );
	mesg->WriteUChar( (unsigned char)message->GetSize() );			// size
	mesg->WriteData( message );
	mesg->EndWriting();
	if( reliable )
	{
		mScriptMessagesSentReliable++;
		mScriptMessagesSentBytesReliable += mesg->GetSize();
		mNetChannel->AddReliableMessageToSend( mesg );			// send message
	}
	else
	{
		mScriptMessagesSentUnreliable++;
		mScriptMessagesSentBytesUnreliable += mesg->GetSize();
		mNetChannel->Send( mesg );
	}
}
