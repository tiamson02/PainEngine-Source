#error this file is obselete
//#include "Engine.h"

#ifdef OIUYTREWLKJHGFDS

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "DemoRecording.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Net/NetworkClient.h"
#include "Net/NetworkDevice.h"
#include "Net/IPDriver.h"
#include "Net/IPChannel.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"
#include "Engine/BaseGame.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"

#define RECORDING_DIRECTORY "../Recordings/"
#define DEMO_CHUNK_TOKEN_UDP 0x5e65
#define DEMO_CHUNK_TOKEN_DATA 0x5e66

DemoRecording::DemoRecording( void ) : mNet(0), State(IDLE), mRecordingFile(0), mClient(0), mMessageSize(0)
,	mMessageTime(0), mTimeDiffSet(false), mTimeDiff(0), mTimeDiffPlus(true), mPause( false ), mPauseStart( 0 )
,	RestoreCaches(false), mLastTimeTimeAdjusted(0), mWasSpectator(false)
{
	// check directory for mopp streaming objects
	FileManagerStd fm;
	fm.CreateDirectory( RECORDING_DIRECTORY );
}

void DemoRecording::ClearPlayersInfo( void )
{
	for( int i = 0; i < PlayersInfo.Size(); ++i )
		if( PlayersInfo[i] != 0 )
			delete PlayersInfo[i];
	PlayersInfo.Clear();
}

void DemoRecording::StartRecording( const char* filename, NetworkClient* client )
{
	if( State != IDLE ) Stop();
	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
	mRecordingFile = GFileMan.CreateFileWriter( str.AsChar() );

	if( !mRecordingFile )
	{
		GScript.DoString( "CONSOLE.AddMessage( 'Couldn't open a recording file! Not recording.' )" );
		GLog.Print( "Couldn't open a recording file!\n" );
		return;
	}

	if( !client ) client = ChooseBestClientToRecord();

	if( !client )
	{
		GScript.DoString( "CONSOLE.AddMessage( 'Couldn't find suitable client! Not recording.' )" );
		GLog.Print( "Couldn't find suitable client!\n" );
		return;
	}

	if( client->GetState() == NetworkClient::EStateSynchronizing )	// no chance now to start recording
	{
		GScript.DoString( "CONSOLE.AddMessage( 'Couldn't start recording during synchro! Not recording.' )" );
		GLog.Print( "Couldn't start recording during synchro!\n" );
		return;
	}

	mClient = client;
	mClient->mRecording = this;
	State = RECORDING;

	// Write header info
	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

	bool fakeStart = ( mClient->GetState() == NetworkClient::EStatePlay );
	WriteHeader( mRecordingFile, ourVer, ourNetVer, mClient->mMyIndex, fakeStart, 0L, true );
	if( fakeStart ) WriteFakeConnection();
	mWasSpectator = mClient->mSpectator;
}

void DemoRecording::WriteFakeConnection( void )
{
	// Backup caches
	POCache = mClient->POCreationCache;
	StringTransport = mClient->StringTransport;
	mClient->POCreationCache.Clear();
	mClient->StringTransport.Clear();

	int t = GScript.GetTop();
	GScript.Call( "Game_GetLevelMapSize", 1, "%s", GEngine->World->LevelName.AsChar() );
	int levSize = GScript.GetInt( -1 );
	GScript.SetTop(t);

	// Write message from server about map loading
	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_LOADMAP );
	mesg.WriteString( GEngine->World->LevelName.AsChar() );
	GEngine->Physics->PackTweaksIntoMessage( &mesg );
	mesg.WriteLong( levSize );
	mesg.WriteLong( GEngine->Physics->PhysicsFPS );

	mesg.WriteCheckToken( NCL_LOADMAP );
	mesg.EndWriting();
	RecordUdpMessage( &mesg );

	// Write synchronizable entites data

	static UdpMessage buffer( 16384 );	// big buffer
	unsigned short maxMessageSize = MAX_UDP_PACKET_SIZE;

	// Send creation reliable messages for all entities that need it

	unsigned short howManyEntities = 0;
	int pos, pos2;

	mesg.SeekToBegin();
	mesg.WriteUChar( IPChannel::NETCLIENT|NetworkClient::CREATEOBJ );
	pos = mesg.GetPos();
	mesg.WriteUShort( 0 );

	// Synchronize models & meshes
	for( int i = 0; i < mClient->mObjectSynchronizers.Size(); ++i )
		if( ( !mClient->mObjectSynchronizers[i]->GetZombie() ) &&
			( ( mClient->mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeMesh ) ||
			( mClient->mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeModel ) ) )
		{
			mClient->mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, mClient );
			buffer.EndWriting();

			if( mesg.GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg.WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg.GetPos();
				mesg.Seek(pos);
				mesg.WriteUShort(howManyEntities);
				mesg.Seek(pos2);
				mesg.EndWriting();
				RecordUdpMessage( &mesg );
				mesg.SeekToBegin();
				howManyEntities = 0;
				mesg.WriteUChar( IPChannel::NETCLIENT|NetworkClient::CREATEOBJ );
				pos = mesg.GetPos();
				mesg.WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg.WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mClient->mNetChannel->GetMaxUdpPacketSize();
		}

	// Synchronize POs
	for( int i = 0; i < mClient->mObjectSynchronizers.Size(); ++i )
		if( ( !mClient->mObjectSynchronizers[i]->GetZombie() ) &&
			( mClient->mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypePO ) &&
			( ((POSynchronizer*)mClient->mObjectSynchronizers[i])->HasPO() ) )
		{
			mClient->mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, mClient );
			buffer.EndWriting();

			if( mesg.GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES  )
			{
				// Send message and prepare next one
				mesg.WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg.GetPos();
				mesg.Seek(pos);
				mesg.WriteUShort(howManyEntities);
				mesg.Seek(pos2);
				mesg.EndWriting();
				RecordUdpMessage( &mesg );
				mesg.SeekToBegin();
				howManyEntities = 0;
				mesg.WriteUChar( IPChannel::NETCLIENT|NetworkClient::CREATEOBJ );
				pos = mesg.GetPos();
				mesg.WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}
		
			mesg.WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mClient->mNetChannel->GetMaxUdpPacketSize();
		}
/*
	// Synchronize Ragdolls
	for( int i = 0; i < mClient->mObjectSynchronizers.Size(); ++i )
		if( ( !mClient->mObjectSynchronizers[i]->GetZombie() ) &&
			( mClient->mObjectSynchronizers[i]->GetType() == ObjectSynchronizer::STypeRagdoll ) &&
			( ((RagdollSynchronizer*)mClient->mObjectSynchronizers[i])->HasRagdoll() ) )
		{
			mClient->mObjectSynchronizers[i]->AddCreateObjMessage( &buffer, mClient );
			buffer.EndWriting();

			if( mesg.GetPos() + buffer.GetPos() > maxMessageSize - MAX_PACKET_HEADER_BYTES )
			{
				// Send message and prepare next one
				mesg.WriteCheckToken( NCL_ENTITYMSG );
				pos2 = mesg.GetPos();
				mesg.Seek(pos);
				mesg.WriteUShort(howManyEntities);
				mesg.Seek(pos2);
				mesg.EndWriting();
				RecordUdpMessage( &mesg );
				mesg.SeekToBegin();
				howManyEntities = 0;
				mesg.WriteUChar( IPChannel::NETCLIENT|NetworkClient::CREATEOBJ );
				pos = mesg.GetPos();
				mesg.WriteUShort( 0 );
				maxMessageSize = MAX_UDP_PACKET_SIZE;
			}

			mesg.WriteData( &buffer );
			buffer.SeekToBegin();
			howManyEntities++;
			maxMessageSize = mClient->mNetChannel->GetMaxUdpPacketSize();
		}
*/
	if( howManyEntities )
	{
		mesg.WriteCheckToken( NCL_ENTITYMSG );
		pos2 = mesg.GetPos();
		mesg.Seek(pos);
		mesg.WriteUShort(howManyEntities);
		mesg.Seek(pos2);
		mesg.EndWriting();
		RecordUdpMessage( &mesg );
	}

	// Write message from server about synchro end
	mesg.SeekToBegin();
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_SYNCHRONIZED );
	mesg.WriteFloat( GEngine->MinSecondsPerFrame );
	mesg.WriteCheckToken( NCL_SYNCHRO );
	mesg.EndWriting();
	RecordUdpMessage( &mesg );

	// Now I guess we're ready

	// Restore caches to our client
	mClient->POCreationCache = POCache;
	mClient->StringTransport = StringTransport;
}

void DemoRecording::Stop( void )
{
	switch( State )
	{
	case IDLE:
		return;
	case RECORDING:
		delete mRecordingFile;
		mRecordingFile = 0;
		mClient->mRecording = 0;
		mClient = 0;
		State = IDLE;
		mWasSpectator = false;
		break;
	case PLAYING:
		delete mRecordingFile;
		mRecordingFile = 0;
		mClient->mRecording = 0;
		mClient = 0;
		State = IDLE;
		mTimeDiffSet = false;
		mMessageSize = 0;
		ClearPlayersInfo();
		GEngine->SetTimeMultiplier( 1.0f );
		break;
	}
}

bool DemoRecording::FileExists( const char* filename )
{
	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
    int size = GFileMan.FileSize( str.AsChar() );
	return ( size != 0 );
}

bool DemoRecording::ReadHeader( GFile* f, String& fileVer, String& fileNetVer, unsigned char& clientIndex, bool& restoreCaches, bool dumpCaches, long& timeDiff, bool& predictionOn )
{
	char buffer[1400];
	int stringSize;
	if( f->Read( &stringSize, sizeof(int) ) != sizeof(int) ) return false;
	if( f->Read( buffer, stringSize ) != stringSize ) return false;
	buffer[stringSize] = 0;
	fileVer = buffer;
	if( f->Read( &stringSize, sizeof(int) ) != sizeof(int) ) return false;
	if( f->Read( buffer, stringSize ) != stringSize ) return false;
	buffer[stringSize] = 0;
	fileNetVer = buffer;
	if( f->Read( &clientIndex, sizeof(unsigned char) ) != sizeof(unsigned char) ) return false;

	long tDiff;
	if( f->Read( &tDiff, sizeof(long) ) != sizeof(long) ) return false;
	timeDiff = tDiff;

	char sc;
	if( f->Read( &sc, sizeof(unsigned char) ) != sizeof(unsigned char) ) return false;
	restoreCaches = (sc==1);

	char pred;
	if( f->Read( &pred, sizeof(unsigned char) ) != sizeof(unsigned char) ) return false;
	predictionOn = (pred==1);

	if( restoreCaches )
	{
		if( dumpCaches )
		{
			PONetCreationCache c1; c1.DumpRestore( f );
			StringHashSystem c2; c2.DumpRestore( f );
		}
		else
		{
			POCache.Restore( f );
			StringTransport.Restore( f );
		}
	}

	return true;
}

void DemoRecording::CheckRestoreCaches( void )
{
	// Only after loading from fake connection
	if( RestoreCaches )
	{
		mClient->POCreationCache = POCache;
		mClient->StringTransport = StringTransport;
	}
}

bool DemoRecording::WriteHeader( GFile* f, String fileVer, String fileNetVer, unsigned char clientIndex, bool synchroCaches, long timeDiff, bool predictionOn )
{
	int stringSize = fileVer.Len();
	if( !f->Write( &stringSize, sizeof(int) ) ) return false;
	if( !f->Write( fileVer.AsChar(), stringSize ) ) return false;
	stringSize = fileNetVer.Len();
	if( !f->Write( &stringSize, sizeof(int) ) ) return false;
	if( !f->Write( fileNetVer.AsChar(), stringSize ) ) return false;
	if( !f->Write( &clientIndex, sizeof(unsigned char) ) ) return false;
	if( !f->Write( &timeDiff, sizeof(long) ) ) return false;
	char sc = synchroCaches?1:0;
	if( !f->Write( &sc, sizeof(unsigned char) ) ) return false;
	if( synchroCaches )
	{
		mClient->POCreationCache.Save( f );
		mClient->StringTransport.Save( f );
	}
	char p = predictionOn?1:0;
	if( !f->Write( &p, sizeof(unsigned char) ) ) return false;
	return true;
}

bool DemoRecording::CanPlayFile( const char* filename, unsigned char& clientIndex, String* errorText, bool& predictionOn )
{
	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
	GFile* f = GFileMan.CreateFileReader( str.AsChar() );
	if( !f )
	{
		if( errorText )
			*errorText = "Can not open demo file!";
		return false;
	}

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

	String fileVer;
	String fileNetVer;
	bool restoreCaches;
	long timeDiff = 0;
	if( !ReadHeader( f, fileVer, fileNetVer, clientIndex, restoreCaches, true, timeDiff, predictionOn ) )
	{
		if( errorText )
			*errorText = "Incorrect recording header!";
		return false;
	}

	delete f;

	if( fileVer != ourVer )
	{
		if( errorText )
			*errorText = "Painkiller can't play this recording. This Painkiller version differs from the one that it was recorded in.\n";
		return false;
	}

	if( fileNetVer != ourNetVer )
	{
		if( errorText )
			*errorText = "Painkiller can't play this recording. Current scripts differ from those present during recording.\n";
		return false;
	}

	return true;
}

bool DemoRecording::StartPlaying( const char* filename, NetworkClient* client, String* errorString )
{
	if( State != IDLE ) Stop();
	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
	mRecordingFile = GFileMan.CreateFileReader( str.AsChar() );

	if( !mRecordingFile )
	{
		if( errorString )
			*errorString = "Can not open demo file!";
		return false;
	}

	if( !PrereadPlayerData( filename, errorString ) ) return false;

	mClient = client;
	mClient->mRecording = this;
	State = PLAYING;

	// Pass the header
	String fileVer;
	String fileNetVer;
	unsigned char clientIndex;
	long serverTime;
	bool predictionOn;
	ReadHeader( mRecordingFile, fileVer, fileNetVer, clientIndex, RestoreCaches, false, serverTime, predictionOn );
	return true;
}

NetworkClient* DemoRecording::ChooseBestClientToRecord( void )
{
	if( !mNet->IsServer() )
		return mNet->mNetworkClients[0];

	int bytesSent = 0;
	int num = -1;
	for( int i = 0; i < mNet->mNetworkClients.Size(); ++i )
	{
		int bytesSentHere = mNet->mNetworkClients[i]->mNetChannel->mStatsData.mBytesSent;
		if( bytesSentHere > bytesSent )
		{
			num = i;
			bytesSent = bytesSentHere;
		}
	}

	if( num == -1 ) return 0;
	return mNet->mNetworkClients[num];
}

void DemoRecording::RecordUdpMessage( UdpMessage* message, bool fromPos )
{
	Assert( State == RECORDING );

	// If the client changed state between spectator and no spectator, stop recording
	if( mWasSpectator != mClient->mSpectator )
	{
		Stop();
		GScript.DoString( "CONSOLE.AddMessage( 'Demo recording stopped - mode change' )" );
		return;
	}

	// Save chunk
	unsigned short chunk = DEMO_CHUNK_TOKEN_UDP;
	mRecordingFile->Write( &chunk, sizeof(unsigned short) );

	// Save time of sending
	DWORD curTime = GetTickCount();
	mRecordingFile->Write( &curTime, sizeof(DWORD) );

	// Save message size
	unsigned short size = message->GetSize();
	if( fromPos ) size -= message->GetPos();
	mRecordingFile->Write( &size, sizeof(unsigned short) );

	// Save message
	if( fromPos )
		mRecordingFile->Write( message->GetBuffer()+message->GetPos(), size );
	else
		mRecordingFile->Write( message->GetBuffer(), size );
    mRecordingFile->Flush();
}

bool DemoRecording::PrereadPlayerData( const char* filename, String* errorString )
{
	static char buffer[MAX_UDP_PACKET_SIZE];

	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
	GFile* f = GFileMan.CreateFileReader( str.AsChar() );
	if( !f )
	{
		if( errorString )
			*errorString = "Can not open demo file!";
		return false;
	}

	// Pass the header
	String fileVer;
	String fileNetVer;
	unsigned char clientIndex;
	bool restoreCaches;
	long timeDiff = 0;
	bool predictionOn;
	if( !ReadHeader( f, fileVer, fileNetVer, clientIndex, restoreCaches, true, timeDiff, predictionOn ) )
	{
		delete f;
		if( errorString )
			*errorString = "Invalid file header!!";
		return false;
	}

	// Pass the mesgs, read the data until the end of file
	unsigned short chunk;
	do
	{
		// Read message time
		if( f->Read( &chunk, sizeof(unsigned short) ) != sizeof(unsigned short) )
		{
			delete f;
			return true;
		}

		// If we received normal message, pass it
		if( chunk == DEMO_CHUNK_TOKEN_UDP )
		{
			DWORD time;
			// Read message time
			if( f->Read( &time, sizeof(DWORD) ) != sizeof(DWORD) )
			{
				delete f;
				return true;
			}

			// Read message size
			unsigned short size;
			if( f->Read( &size, sizeof(unsigned short) ) != sizeof(unsigned short) )
			{
				delete f;
				if( errorString )
					*errorString = "File corrupted or premature end of file!!";
				return false;
			}

			Assert( size <= MAX_UDP_PACKET_SIZE );
			if( size > MAX_UDP_PACKET_SIZE )
			{
				delete f;
				if( errorString )
					*errorString = "Wrong packet size > 1400 bytes!!!";
				return false;
			}

			// Read message
			if( f->Read( buffer, size ) != size )
			{
				delete f;
				if( errorString )
					*errorString = "File corrupted or premature end of file!!";
				return false;
			}
			continue;
		}

		Assert( chunk == DEMO_CHUNK_TOKEN_DATA );

		// Read data
		PlayerDataEntry e;
		unsigned char playerNumber;

		if( f->Read( &e.Time, sizeof(DWORD) ) != sizeof(DWORD) )
		{
			delete f;
			if( errorString )
				*errorString = "File corrupted or premature end of file!!";
			return false;
		}

		if( f->Read( &playerNumber, sizeof(unsigned char) ) != sizeof(unsigned char) )
		{
			delete f;
			if( errorString )
				*errorString = "File corrupted or premature end of file!!";
			return false;
		}

		if( f->Read( &e.Yaw, sizeof(unsigned short) ) != sizeof(unsigned short) )
		{
			delete f;
			if( errorString )
				*errorString = "File corrupted or premature end of file!!";
			return false;
		}

		if( f->Read( &e.Dir, sizeof(float) ) != sizeof(float) )
		{
			delete f;
			if( errorString )
				*errorString = "File corrupted or premature end of file!!";
			return false;
		}

		// Input data
		while( PlayersInfo.Size() <= playerNumber ) PlayersInfo.Add(NULL);
		if( !PlayersInfo[playerNumber] ) PlayersInfo[playerNumber] = new PlayerDataRecord;
		PlayersInfo[playerNumber]->Entries.Add(e);
	}
	while( true );
}

void DemoRecording::UpdateOurPlayerData( void )
{
	if( ( State != PLAYING ) || mPause ) return;

	// Get time and adjust it to recorded time
	DWORD curTime = GetTickCount();
	Assert( mTimeDiffSet );
	if( mTimeDiffPlus ) curTime -= mTimeDiff; else curTime += mTimeDiff;

	Assert( mClient && ( mClient->mMyIndex != -1 ) );
	int playerIndex = mClient->mMyIndex;
	if( ( PlayersInfo.Size() <= playerIndex ) || !PlayersInfo[playerIndex] ) return;

	PlayerDataRecord* pdr = PlayersInfo[playerIndex];

	if( pdr->CurrentEntryMarker == -1 ) return;				// after last entry
	if( !pdr->Entries.Size() ) return;						// no entries
	if( !pdr->CurrentEntryMarker )
	{
		if( pdr->Entries[0].Time > curTime ) return;		// before first entry
		pdr->CurrentEntryMarker++;							// start updating
	}

	PlayerDataEntry* last = &(pdr->Entries[pdr->CurrentEntryMarker-1]);
	PlayerDataEntry* next = &(pdr->Entries[pdr->CurrentEntryMarker]);

	// Check if we have to pass entries
	while( next->Time <= curTime )
	{
		pdr->CurrentEntryMarker++;
		if( pdr->CurrentEntryMarker == pdr->Entries.Size() )	// Last entry passed
		{
			pdr->CurrentEntryMarker = -1;
			return;
		}
		last = next;
		next = &(pdr->Entries[pdr->CurrentEntryMarker]);
	}

	// Spectators don't need to set camera pos
	if( mClient->mSpectator ) return;

	DWORD up = curTime-last->Time;
	DWORD down = next->Time-last->Time;
	double percent = ((double)up)/down;

	unsigned short shortYaw = (double)last->Yaw + percent*((double)(next->Yaw-last->Yaw));
	double realYaw = ((shortYaw-32767)*90.0)/32768;

	// Calculate dir
	float realDir = 1000.0f;
	if( ( next->Dir != 1000.0f ) && ( last->Dir != 1000.0f ) )
	{
		float dirDiff = next->Dir-last->Dir;
		if( dirDiff > F_PI ) dirDiff -= 2*F_PI;
		else if( dirDiff < -F_PI ) dirDiff += 2*F_PI;
		realDir = last->Dir + percent*dirDiff;
/*
		if( !( realDir <= 10.0f ) && !( realDir >= 10.0f ) )
		{
			GLog.Print( "realDir is NaN\n" );
			GLog.Print( "last.Dir: %f, next.Dir: %f, dirDiff: %f, percent: %f\n", last.Dir, next.Dir, dirDiff, percent );
			GLog.Print( "up: %d, down: %d\n", up, down );
		}
*/
	}

	// Get client entity
	Entity* pe = mNet->GetPlayerByIndex( playerIndex );
	if( !pe ) return;

//	GScript.DoString( "Game.CameraFromPlayer = true" );
	GEngine->Game->RecordingUpdateViewFromPlayer( pe, realYaw, realDir );
}

void DemoRecording::WritePlayerData( unsigned char playerNumber, unsigned short yaw )
{
	unsigned short chunk = DEMO_CHUNK_TOKEN_DATA;
	mRecordingFile->Write( &chunk, sizeof(unsigned short) );		// Marker
	DWORD curTime = GetTickCount();
	mRecordingFile->Write( &curTime, sizeof(DWORD) );				// Time
	mRecordingFile->Write( &playerNumber, sizeof(unsigned char) );	// Player num
	mRecordingFile->Write( &yaw, sizeof(unsigned short) );			// Yaw

	float dir = 1000.0f;

	// Get client entity
	Entity* pe = mNet->GetPlayerByIndex( playerNumber );
	if( pe && pe->PO )
		dir = pe->PO->GetOrientation();

	mRecordingFile->Write( &dir, sizeof(float) );					// Dir
}

bool DemoRecording::PassPlayerData( void )
{
	const int size = sizeof(DWORD)+sizeof(unsigned char)+sizeof(unsigned short)+sizeof(float);
	static char dump[size];
	return( mRecordingFile->Read( dump, size ) == size );
}


bool DemoRecording::PlayUdpMessage( UdpMessage* message )
{
	// If we're not playing anymore and the game still wants to read packets, give it the empty one so it disconnects
	if( State != PLAYING )
	{
		message->SeekToBegin();
		message->EndWriting();
		return true;
	}

	if( mPause ) return false;

	DWORD curTime = GetTickCount();

	// If there's no message waiting, read one
	if( !mMessageSize && !ReadUdpMessage( curTime ) )
	{
		// There's been an error reading the file!! Let's assume it came to an end and send special "finish" message.
		message->SeekToBegin();
		message->EndWriting();
		return true;
	}

	// Now check if we can play this message
	if( mMessageTime > curTime ) return false;

	// We can - do it!
	message->SeekToBegin();
	message->WriteData( mMessageBuffer, mMessageSize );
	message->EndWriting();
	mMessageSize = 0;	// this one is invalidated that way
	return true;
}

void DemoRecording::Pause( bool pause )
{
	if( State != PLAYING ) { mPause = false; return; };
	if( mPause == pause ) return;
	mPause = pause;
	if( pause )
		mPauseStart = GetTickCount();
	else
	{
		DWORD pauseTime = GetTickCount() - mPauseStart;
		if( mTimeDiffPlus )
		{
			mTimeDiff += pauseTime;
		}
		else
		{
			if( mTimeDiff > pauseTime )
				mTimeDiff -= pauseTime;
			else
			{
				mTimeDiff = pauseTime-mTimeDiff;
				mTimeDiffPlus = true;
			}
		}
		if( GEngine->Net )
			GEngine->Net->ShiftStartTime( pauseTime );
	}
}

bool DemoRecording::ReadUdpMessage( DWORD curTime )
{
	unsigned short chunk;
	do
	{
		// Read demo chunk token
		if( mRecordingFile->Read( &chunk, sizeof(unsigned short) ) != sizeof(unsigned short) ) return false;

		// Check if we received the players information
		if( ( chunk == DEMO_CHUNK_TOKEN_DATA ) && !PassPlayerData() ) return false;
	}
	while( chunk == DEMO_CHUNK_TOKEN_DATA );

	Assert( chunk == DEMO_CHUNK_TOKEN_UDP );

	// Read message time
	DWORD gotTime;
	if( mRecordingFile->Read( &gotTime, sizeof(DWORD) ) != sizeof(DWORD) ) return false;

	// Set time difference if not synchronized yet
	if( !mTimeDiffSet )
	{
		if( curTime > gotTime )
			mTimeDiff = curTime-gotTime;
		else
			mTimeDiff = gotTime-curTime;
		mTimeDiffPlus = ( curTime > gotTime );
		mTimeDiffSet = true;
		mMessageTime = curTime;
	}
	else	// Adjust time received to our clock
	{
		if( mTimeDiffPlus )
			mMessageTime = gotTime + mTimeDiff;
		else
			mMessageTime = gotTime - mTimeDiff;
	}

	// Read message size
	unsigned short size;
	if( mRecordingFile->Read( &size, sizeof(unsigned short) ) != sizeof(unsigned short) ) return false;

	// Read message
	Assert( size <= MAX_UDP_PACKET_SIZE );
	if( (size > MAX_UDP_PACKET_SIZE ) || ( mRecordingFile->Read( mMessageBuffer, size ) != size ) ) return false;

	mMessageSize = size;
	return true;
}

void DemoRecording::AdjustTimeToNextMessage( void )
{
	Assert( State == PLAYING );
	Assert( mTimeDiffSet );

	if( mPause ) mPause = false;	// break from pausing

	DWORD curTime = GetTickCount();

	// If there's no message waiting, read one
	if( !mMessageSize && !ReadUdpMessage( curTime ) ) return;

	// Adjust time diff so message time is the same as cur time
	DWORD gotTime = mTimeDiffPlus?mMessageTime-mTimeDiff:mMessageTime+mTimeDiff;
	mTimeDiffPlus = (curTime>gotTime);
	mTimeDiff = mTimeDiffPlus?curTime-gotTime:gotTime-curTime;
	mMessageTime = curTime;

	return;
}

void DemoRecording::AdjustTimeToSlowdown( float timeMultiplier )
{
	DWORD curTime = GetTickCount();
	if( mLastTimeTimeAdjusted && ( timeMultiplier != 1.0f ) )
	{
		DWORD diff = curTime-mLastTimeTimeAdjusted;
		if( !diff ) return;
		DWORD realDiff = (DWORD)floor(timeMultiplier*diff);

		if( diff > realDiff )	// slower time
		{
			DWORD diff2 = diff-realDiff;
			if( mTimeDiffPlus )
				mTimeDiff += diff2;
			else
			{
				if( mTimeDiff > diff2 )
					mTimeDiff -= diff2;
				else
				{
					mTimeDiff = diff2-mTimeDiff;
					mTimeDiffPlus = true;
				}
			}

			if( GEngine->Net )
			{
				GEngine->Net->ShiftStartTime( diff2 );
				GEngine->TimeMultiplier = timeMultiplier;
			}
		}
		else					// faster time
		{
			DWORD diff2 = realDiff-diff;
			if( !mTimeDiffPlus )
				mTimeDiff += diff2;
			else
			{
				if( mTimeDiff > diff2 )
					mTimeDiff -= diff2;
				else
				{
					mTimeDiff = diff2-mTimeDiff;
					mTimeDiffPlus = false;
				}
			}

			if( GEngine->Net )
			{
				GEngine->Net->ShiftStartTime( diff2, false );
				GEngine->TimeMultiplier = timeMultiplier;
			}
		}
	}

	mLastTimeTimeAdjusted = curTime;
}

#endif
