#ifndef TRANSGAMING

#include "Engine.h"
#include "DemoRecording2.h"
#ifndef TRANSGAMING_PSP
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Render/Video.h"
#include "Render/View.h"
#include "Engine/World.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"
#include "Engine/BaseGame.h"
#include "Render/HUD.h"
#include "Engine/InputSystem.h"
#include "Engine/Mesh.h"
#include "Engine/PainMenu.h"
#include "Engine/MPStats.h"

#ifndef TRANSGAMING_NO_MILES
// We currently don't support Audio on the PSP...
#include "Audio/AudioRecording.h"
#endif
#endif      //TRANSGAMING_PSP

DemoRecording2 *gDemoRec = NULL;

#ifndef TRANSGAMING_PSP

#define SAVE_AUDIO


#define RECORDING_DIRECTORY "../Recordings/"
#define SAFE_READ(var,size)			if( mRecordingFile->Read( var, size ) != size ) { Stop(); return; }
#define SAFE_READ_BOOL(var,size)	if( mRecordingFile->Read( var, size ) != size ) { Stop(); return false; }
#define SAFE_WRITE(var,size)		if( !mRecordingFile->Write( var, size ) ) { Stop(); return; }
#define SAFE_WRITE_BOOL(var,size)	if( !mRecordingFile->Write( var, size ) ) { Stop(); return false; }


#define NOSAVEGAME
#endif

DemoRecording2::DemoRecording2() 
#ifndef TRANSGAMING
                                    : mState( IDLE ), 
                                   mRecordingFile( NULL ), mStartTime( 0 ),
								   mLastMap( "" ), mSpeed( 1.0f ), mReady( false ), mIsTimeDemo( false ),
								   mNumFrames( 0 ), mLastFPS( 0.0f ), mTakeScreenshots( false ), mFileName( "" ),
								   mAudioRec( NULL )
#endif //TRANSGAMING
{
#ifndef TRANSGAMING
	mEntities.Clear();
	mToDelete.Clear();
#endif
}


DemoRecording2::~DemoRecording2()
{
	Stop();
}


void DemoRecording2::StartRecording( const char* filename )
{
#ifndef TRANSGAMING

	if( mState != IDLE ) Stop();

	GFileMan.CreateDirectory( RECORDING_DIRECTORY );

	String str = RECORDING_DIRECTORY;
	str += filename;
//	GFileMan.CreateDirectory( str.AsChar() );
	str += ".pkd";
	mRecordingFile = GFileMan.CreateFileWriter( str.AsChar() );
	if( !mRecordingFile )
	{
//		if( GEngine->Video )
//			GEngine->Video->Hud->mConsole.AddMessage( "Couldn't open a recording file! Not recording." );
		GScript.DoString( "CONSOLE.AddMessage('Couldn\\'t open a recording file! Not recording.')" );
		GLog.Print( "Couldn't open a recording file!\n" );
		return;
	}

	// Write header info
	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

	WriteHeader( ourVer, ourNetVer, 0 );

	mState = RECORDING;

	mEntities.Clear();

#ifndef NOSAVEGAME
	for( int i = 0; i < GEngine->World->EntityGIndexes.Size(); i++ )
	{
		if( GEngine->World->EntityGIndexes[i] )
			mEntities.Insert( i, 1 );
	}
#endif

	mLastMap = "";
	mToDelete.Clear();

	mTakeScreenshots = false;

	mFileName = filename;

#ifdef SAVE_AUDIO
#ifndef TRANSGAMING_NO_MILES
	mAudioRec = new AudioRecorder( GEngine->Audio );
	mAudioRec->StartRecordingTo( mRecordingFile );
#endif // TRANSGAMING_NO_MILES
#endif
#endif //TRANSGAMING
}


void DemoRecording2::Stop( void )
{

#ifndef TRANSGAMING

	switch( mState )
	{
	case IDLE:
		break;

	case RECORDING:
	case PLAYING:
		delete mRecordingFile;
		mRecordingFile = 0;
		mEntities.Clear();
		if( mState == PLAYING )
		{
			if( mIsTimeDemo )
			{
				mLastFPS = ( float )mNumFrames * 1000.0f / ( float )( GetTickCount() - mStartTime );
				GScript.DoString( "CONSOLE.AddMessage('TimeDemo FPS: %.2f')", mLastFPS );
			}
			GScript.DoString( "Game.LevelStarted = false; Game.GameInProgress = false; PMENU.Activate(true); Game:NewLevel('NoName','','',0.3); WORLD.Release(); Game.CameraFromPlayer = true" );
		}
		GLog.Print( "DemoRec::Stop()\n" );
		mLastMap = "";
		mToDelete.Clear();
		mSpeed = 1.0f;
		mReady = false;
		mStartTime = 0;
		mNumFrames = 0;
		mTakeScreenshots = false;
		mFileName = "";
#ifndef TRANSGAMING_NO_MILES
		if( mAudioRec )
			delete mAudioRec;
#endif
		mAudioRec = NULL;

		GScript.DoString( "SOUND.StreamSetVolume(0,Cfg.AmbientVolume); SOUND.StreamSetVolume(1,Cfg.MusicVolume)" );
/*
		if( mState == PLAYING )
		{
#ifndef TRANSGAMING_NO_MILES
			if( GEngine->Audio )
				delete GEngine->Audio;

			GEngine->Audio = new MilesEngine;
			if( !GEngine->Audio->StartedOk() )
			{
				GLog.Print("ERROR: Could not initialize AudioDevice\n");
				delete GEngine->Audio;
				GEngine->Audio = 0;
			}

			if( GEngine->Audio )
			{
				GEngine->Audio->SetDefaultObstruction( false );
				GEngine->Audio->SetObstructionProperties( 4.0f, 0.2f, 0.0f, 1.0f, 1.0f, 1000.0f, 1000.0f );
			}
#endif
		}
*/
		mState = IDLE;
		break;
	}
#endif //TRANGAMING
}


bool DemoRecording2::StartPlaying( const char* filename, String *errorString, bool timeDemo, bool takeScreenshots )
{
#ifndef TRANSGAMING

	if( mState != IDLE ) Stop();

	mTakeScreenshots = takeScreenshots;
	if( mTakeScreenshots )
		timeDemo = true;

	mIsTimeDemo = timeDemo;
	mNumFrames = 0;
	mSpeed = 1.0f;

	String str = RECORDING_DIRECTORY;
	str += filename;
	str += ".pkd";
	mRecordingFile = GFileMan.CreateFileReader( str.AsChar() );
	if( !mRecordingFile )
	{
		GScript.DoString( "CONSOLE.AddMessage('Couldn\\'t open a recording file! Not playing.')" );
//		if( GEngine->Video )
//			GEngine->Video->Hud->mConsole.AddMessage( "Couldn't open a recording file! Not playing." );
		GLog.Print( "Couldn't open a recording file!\n" );
		return false;
	}

	// Write header info
	String fileVer;
	String fileNetVer;
	long32 timeDiff;
	if( !ReadHeader( fileVer, fileNetVer, timeDiff ) )
		return false;

	String ourVer = GScript.Globals()["PK_VERSION"].GetString();
	String ourNetVer = GScript.Globals()["PK_NETVERSION"].GetString();

	if( fileVer != ourVer )
	{
		if( errorString )
			*errorString = "Painkiller can't play this recording. This Painkiller version differs from the one that it was recorded in.\n";

		GScript.DoString( "CONSOLE.AddMessage('Painkiller can\\'t play this recording. This Painkiller version differs from the one that it was recorded in.')" );

		return false;
	}
/*
	if( fileNetVer != ourNetVer )
	{
		if( errorString )
			*errorString = "Painkiller can't play this recording. Current scripts differ from those present during recording.\n";
		return false;
	}
*/

	GScript.DoString( "SOUND.StreamSetVolume(0,0); SOUND.StreamSetVolume(1,0)" );

	mEntities.Clear();

	mState = PLAYING;

	GScript.DoString( "Game.CameraFromPlayer = false; Hud.Enabled = false; GlobalAIDisable()" );

	mLastMap = "";

	mReady = false;

	mNumFrames = 0;
	mLastFPS = 0.0f;

	mFileName = filename;

#ifdef SAVE_AUDIO
#ifndef TRANSGAMING_NO_MILES
/*
	if( GEngine->Audio )
		delete GEngine->Audio;

	GEngine->Audio = new MilesEngine;
	if( !GEngine->Audio->StartedOk() )
	{
		GLog.Print("ERROR: Could not initialize AudioDevice\n");
		delete GEngine->Audio;
		GEngine->Audio = 0;
	}

	if( GEngine->Audio )
	{
		GEngine->Audio->SetDefaultObstruction( false );
		GEngine->Audio->SetObstructionProperties( 4.0f, 0.2f, 0.0f, 1.0f, 1.0f, 1000.0f, 1000.0f );
*/
		mAudioRec = new AudioRecorder( GEngine->Audio );
		mAudioRec->StartPlayingFrom( mRecordingFile, timeDemo );
//	}
#endif // TRANSGAMING_NO_MILES
#endif
#endif //TRANSGAMING
	return true;
}


void DemoRecording2::Update( void )
{
#ifndef TRANSGAMING

	if( mState == RECORDING )
	{
		UpdateRecording();
	}
	else if( mState == PLAYING )
	{
		UpdatePlaying();
	}
#endif //TRANSGAMING
}


void DemoRecording2::UpdateRecording( void )
{
#ifndef TRANSGAMING

	if( !mRecordingFile )
		return;

	if( GEngine->Video->PMenu->Active() )
		return;

	static DWORD lastTime = 0;
	DWORD currTime = GetTickCount();
	if( mStartTime == 0 )
	{
		mStartTime = currTime;
		lastTime = currTime;
	}

	DWORD time = currTime - lastTime;

	// Max 25 fps
	if( currTime - lastTime < 40 )
		return;

#ifdef NOSAVEGAME
	GScript.Call( "Game_DemoGetMapName", 1, "" );
	String map = GScript.GetString( -1, "" );
	if( map != mLastMap )
	{
		*mRecordingFile << ( char )1;
		*mRecordingFile << map;
		mLastMap = map;
		mStartTime = currTime;
		lastTime = currTime;
		time = currTime - mStartTime;
		*mRecordingFile << ( char )0;
	}
	else
	{
		*mRecordingFile << ( char )0;
	}
#endif

	lastTime = currTime;

	SAFE_WRITE( &time, sizeof( DWORD ) );

	if( GEngine->Video->Hud->MPStats.Active() )
	{
		*mRecordingFile << ( char )1;
		GEngine->Video->Hud->MPStats.SaveForDemo( mRecordingFile );
	}
	else
	{
		*mRecordingFile << ( char )0;
	}

	*mRecordingFile << ( char )( GScript.Globals()["Hud"]["_zoom"].GetBool() );

	GScript.DoString( "Game:RecordPlayers()" );

	int numEntities = 0;

	static DynamicArray<int> writeArr;
	writeArr.Clear();
	static HashMap<int, int> writeFullArr;
	writeFullArr.Clear();

#ifdef	TRANSGAMING
	for( int i = 0; i < GEngine->pcfWorld->EntityGIndexes.Size(); i++ )
	{
		Entity *e = GEngine->pcfWorld->EntityGIndexes[i];
#else
	for( int i = 0; i < GEngine->World->EntityGIndexes.Size(); i++ )
	{
		Entity *e = GEngine->World->EntityGIndexes[i];
#endif
		if( !e ) continue;

		if( !e->WorldLink ) continue;
		if( e->Type != Entity::ETypeLight && e->Type != Entity::ETypeEnv )
		{
			if( e->Flags & Entity::EEntNoDraw ) continue;
			if( e->VisCounter < GEngine->Video->GetActiveView()->mScene.VisCounter )
				continue;
		}

		char writeFull = 0;
/*		for( int j = 0; j < writeFullArr.Size(); j++ )
		{
			if( writeFullArr[j] == i )
			{
				writeFull = 1;
				break;
			}
		}*/
		if( !mEntities.Find( i ) )
		{
			writeFullArr.Insert( i, 1 );
			writeFull = 1;
		}

		int size = 0;
		DWORD contents = 0;
		if( !e->DemoCalcSize( size, contents ) && !writeFull )
			continue;

		bool inc = false;
		switch( e->Type )
		{
		case Entity::ETypeMesh:
#ifndef TRANSGAMING_NO_HAVOK
			if( e->AsMesh()->MPKIndex == INVALID_MPKINDEX  && !( e->PO && e->PO->GetGlass() ) )
				inc = true;
#endif
			break;

		case Entity::ETypeDecal:
			if( e->FileName != "shadow" )
				if( writeFull )
					inc = true;
			break;

		case Entity::ETypeParticleFx:
//			if( writeFull )
				inc = true;
			break;

		case Entity::ETypeModel:
			if( e->FileName != "player_box" )
				inc = true;
			break;

		case Entity::ETypeTrail:
//		case Entity::ETypeSound:
        case Entity::ETypeBillboard:
		case Entity::ETypeEnv:
			inc = true;
			break;

		case Entity::ETypeLight:
			inc = true;
			break;
		}

		if( inc && e->WorldLink/* && ( writeFull || ( e->VisCounter == GEngine->Video->GetActiveView()->mScene.VisCounter ) )*/ )
			writeArr.Add( e->GIndex );
	}

	numEntities = writeArr.Size();
	SAFE_WRITE( &numEntities, sizeof( int ) );

	for( int i = 0; i < writeArr.Size(); i++ )
	{
#ifdef	TRANSGAMING
		Entity *e = GEngine->pcfWorld->EntityGIndexes[writeArr[i]];
#else
		Entity *e = GEngine->World->EntityGIndexes[writeArr[i]];
#endif
		if( !e ) continue;

		char writeFull = 0;
/*		for( int j = 0; j < writeFullArr.Size(); j++ )
		{
			if( writeFullArr[j] == writeArr[i] )
			{
				writeFull = 1;
				break;
			}
		}*/
		if( writeFullArr.Find( writeArr[i] ) )
			writeFull = 1;

		char etype = ( char )e->Type;
		char draw = 1;//( e->Flags & Entity::EEntNoDraw ) ? 0 : 1;
//		if( e->VisCounter < GEngine->Video->GetActiveView()->mScene.VisCounter )
//			draw = 0;
		SAFE_WRITE( &writeArr[i], sizeof( int ) );
		SAFE_WRITE( &etype, sizeof( char ) );
		SAFE_WRITE( &writeFull, sizeof( char ) );
		SAFE_WRITE( &draw, sizeof( char ) );
		if( writeFull )
		{
//			mEntities.Add( writeArr[i] );
			mEntities.Insert( writeArr[i], 1 );
			e->DemoWriteFull( mRecordingFile );
			e->DemoWriteEntity( mRecordingFile );
		}
		else if( draw )
		{
			e->DemoWriteEntity( mRecordingFile );
		}
	}

	mToDelete.Clear();
	for( int i = 0; i < mEntities.Size(); i++ )
	{
		Entity *e = NULL;
#ifdef	TRANSGAMING
		if( GEngine->pcfWorld->EntityGIndexes.Size() > mEntities.GetPair(i).Key )
			e = GEngine->pcfWorld->EntityGIndexes[mEntities.GetPair(i).Key];
#else
		if( GEngine->World->EntityGIndexes.Size() > mEntities.GetPair(i).Key )
			e = GEngine->World->EntityGIndexes[mEntities.GetPair(i).Key];
#endif
		if( e == NULL || !e->WorldLink || e->Flags & Entity::EEntNoDraw || ( e->VisCounter < GEngine->Video->GetActiveView()->mScene.VisCounter ) )
		{
			if( mEntities.GetPair(i).Value )
			{
				if( !e || ( e->Type != 2 && e->Type != 9 ) )
					mToDelete.Add( mEntities.GetPair(i).Key );
			}
		}
	}

	int numToDel = mToDelete.Size();
	SAFE_WRITE( &numToDel, sizeof( int ) );
	for( int i = 0; i < numToDel; i++ )
	{
		int del = mToDelete[i];
		SAFE_WRITE( &del, sizeof( int ) );
//		mEntities[mToDelete[i]] = 0;
		mEntities.ChangeValue( mToDelete[i], 0 );
		mEntities.Remove( mToDelete[i] );
	}

#ifdef SAVE_AUDIO
#ifndef TRANSGAMING_NO_MILES
	mAudioRec->FlushActionsTo( mRecordingFile );
#endif // TRANSGAMING_NO_MILES
#endif
#endif //TRANSGAMING_PSP
}


void DemoRecording2::UpdatePlaying( void )
{
#ifndef TRANSGAMING

	if( !mRecordingFile )
		return;

	DWORD currTime = GetTickCount();
	if( mStartTime == 0 ) mStartTime = currTime;

	static DWORD time = 0;
	static DWORD lastTime = 0;

	if( !mReady )
	{
#ifdef NOSAVEGAME
		char newMap = 0;
		SAFE_READ( &newMap, sizeof( char ) );
		if( newMap )
		{
			String map = "";
			*mRecordingFile >> map;
			GScript.DoString( "Game_DemoLoadLevel('%s')", map.AsChar() );
//			GScript.DoString( "PainMenu:LoadLevel('%s')", map.AsChar() );
//			FILE *ff = fopen( "entities.txt", "w" );
#ifdef	TRANSGAMING
			GEngine->pcfWorld->DeleteDelayedEntities();
			for( int i = 0; i < GEngine->pcfWorld->EntityGIndexes.Size(); i++ )
			{
				Entity *e = GEngine->pcfWorld->EntityGIndexes[i];
				if( !e ) continue;
//				fprintf( ff, "Entity %04d: %s (%s), type: %d\n", i, e->Name.AsChar(), e->FileName.AsChar(), e->Type );
				if( e->Type == Entity::ETypeModel || e->Type == Entity::ETypeLight )
//				if( e->Type != 1 )
					GEngine->pcfWorld->DeleteEntity( e );
			}
//			fclose( ff );

			GEngine->pcfWorld->AnimMeshManager.Clear();
#else
			GEngine->World->DeleteDelayedEntities();
			for( int i = 0; i < GEngine->World->EntityGIndexes.Size(); i++ )
			{
				Entity *e = GEngine->World->EntityGIndexes[i];
				if( !e ) continue;
//				fprintf( ff, "Entity %04d: %s (%s), type: %d\n", i, e->Name.AsChar(), e->FileName.AsChar(), e->Type );
				if( e->Type == Entity::ETypeModel || e->Type == Entity::ETypeLight )
//				if( e->Type != 1 )
					GEngine->World->DeleteEntity( e );
			}
//			fclose( ff );

			GEngine->World->AnimMeshManager.Clear();
#endif

			currTime = GetTickCount();
			mStartTime = currTime;
			time = 0;
			return;
		}
		else
#endif
		{
			SAFE_READ( &time, sizeof( DWORD ) );
		}

		char mpStats = 0;
		SAFE_READ( &mpStats, sizeof( char ) );

		if( mpStats )
		{
			GEngine->Video->Hud->MPStats.LoadForDemo( mRecordingFile );
		}
		else if( GEngine->Video->Hud->MPStats.Active() )
		{
			GEngine->Video->Hud->MPStats.Activate( false );
		}

		char zoom = 0;
		SAFE_READ( &zoom, sizeof( char ) );
		if( zoom )
			GScript.DoString( "Hud._zoom = true; if not Hud._matZoom then Hud._matZoom = MATERIAL.Create('HUD/zoom', TextureFlags.NoLOD + TextureFlags.NoMipMaps) end" );
		else
			GScript.DoString( "Hud._zoom = false; if Hud._matZoom then MATERIAL.Release(Hud._matZoom) end; Hud._matZoom = nil" );

		mReady = true;
	}

	if( ( ( time <= ( DWORD )( ( float )( currTime - lastTime ) * mSpeed ) ) || mIsTimeDemo )&& mReady )
	{
		int playerIndex = 0;
		unsigned short yaw = 0;
		float dir = 1000.0f;

		mNumFrames++;

		if( mTakeScreenshots )
		{
			FileManagerStd fm;
			fm.CreateDirectory( "../Data/DemoScreenshots" );

			String string = String::Sprintf("../Data/DemoScreenshots/%s%04d.bmp",mFileName.AsChar(),mNumFrames-1);

			GEngine->Video->RenderDev->TakeScreenshot( string.AsChar() );
		}

		SAFE_READ( &playerIndex, sizeof( int ) );
		if( playerIndex > -1 )
		{
			SAFE_READ( &yaw, sizeof( short ) );
			SAFE_READ( &dir, sizeof( float ) );

			double realYaw = ( ( yaw - 32767 ) * 90.0 ) / 32768;
	/*
			Entity *pe = GEngine->World->GetEntityByIndex( playerIndex );
			if( !pe )
			{
				//if( GEngine->Video )
				//    GEngine->Video->Hud->mConsole.AddMessage( "ERROR: Couldn't find player entity. Playing stopped." );
				return Stop();
			}
	*/
			Vector pos( 0.0f, 0.0f, 0.0f );
			SAFE_READ( &pos.X, sizeof( float ) );
			SAFE_READ( &pos.Y, sizeof( float ) );
			SAFE_READ( &pos.Z, sizeof( float ) );
	/*
			if( pe->PO )
				pe->PO->SetPawnHeadPos( pos );
			else
				pe->SetPosition( pos );

			GEngine->Game->RecordingUpdateViewFromPlayer( pe, realYaw, dir );*/
			UpdateView( pos, realYaw, dir );

			int health, armor;
			SAFE_READ( &health, sizeof( int ) );
			SAFE_READ( &armor, sizeof( int ) );

			GScript.DoString( "if Player then\nPlayer.Health = %d\nPlayer.Armor = %d\nend", health, armor );

			DWORD actions;
			SAFE_READ( &actions, sizeof( DWORD ) );
	//		GEngine->InputSys->SetActionStatus( actions );
		}
		else
		{
			View* view = GEngine->Video->GetActiveView();
			Vector camRot( 0.0f, 0.0f, 0.0f );
			Vector camPos( 0.0f, 0.0f, 0.0f );
			float camFOV = 90.0f;
			SAFE_READ( &camRot.X, sizeof( float ) );
			SAFE_READ( &camRot.Y, sizeof( float ) );
			SAFE_READ( &camRot.Z, sizeof( float ) );
			SAFE_READ( &camPos.X, sizeof( float ) );
			SAFE_READ( &camPos.Y, sizeof( float ) );
			SAFE_READ( &camPos.Z, sizeof( float ) );
			SAFE_READ( &camFOV, sizeof( float ) );
			view->mCamera.SetPosition( camPos );
			view->mCamera.SetRotation( camRot );
			view->mCamera.InverseTransform();
			view->mCamera.SetFOV( camFOV );

			char ghosts;
			char demonEnabled;
			char demonSuper;
			SAFE_READ( &ghosts, sizeof( char ) );
			SAFE_READ( &demonEnabled, sizeof( char ) );
			SAFE_READ( &demonSuper, sizeof( char ) );
#ifdef	TRANSGAMING
			GEngine->pcfWorld->GhostsFX = ghosts ? true : false;
			GEngine->pcfWorld->DemonFX.Enabled = demonEnabled ? true : false;
			GEngine->pcfWorld->DemonFX.Super = demonSuper ? true : false;
			SAFE_READ( &GEngine->pcfWorld->DemonFX.Warp, sizeof( float ) );
#else
			GEngine->World->GhostsFX = ghosts ? true : false;
			GEngine->World->DemonFX.Enabled = demonEnabled ? true : false;
			GEngine->World->DemonFX.Super = demonSuper ? true : false;
			SAFE_READ( &GEngine->World->DemonFX.Warp, sizeof( float ) );
#endif
		}

		// Entities
		int numEntities = 0;
		SAFE_READ( &numEntities, sizeof( int ) );

		char prevType = 0;
		char prevFull = 0;
		unsigned int prevIndex = 0;

		for( int i = 0; i < numEntities; i++ )
		{
			unsigned int index = 0;
			SAFE_READ( &index, sizeof( int ) );

			if( index > 65534 )
			{
				Stop();
				return;
			}

			char type = 0;
			SAFE_READ( &type, sizeof( char ) );

			char fullInfo = 0;
			SAFE_READ( &fullInfo, sizeof( char ) );

			if( fullInfo )
				GLog.Print( "reading entity %d, type %d\n", index, type );

			char draw = 0;
			SAFE_READ( &draw, sizeof( char ) );

			if( fullInfo )
			{
				char fnameLen = 0;
				SAFE_READ( &fnameLen, sizeof( char ) );
				char fname[255];
				SAFE_READ( fname, fnameLen );
				fname[fnameLen] = 0;
				char nameLen = 0;
				SAFE_READ( &nameLen, sizeof( char ) );
				char name[255];
				SAFE_READ( name, nameLen );
				name[nameLen] = 0;
//				GLog.Print( "create entity - %d, %s (%s)\n", index, name, fname );
				float scale = 0.0f;
				SAFE_READ( &scale, sizeof( float ) );
				DWORD flags = 0;
				SAFE_READ( &flags, sizeof( flags ) );
				float timeToDel, fadeTime;
				SAFE_READ( &timeToDel, sizeof( float ) );
				SAFE_READ( &fadeTime, sizeof( float ) );
				int sort;
				SAFE_READ( &sort, sizeof( int ) );

#ifdef	TRANSGAMING
				while( GEngine->pcfWorld->EntityGIndexes.Size() < ( int )index )
				{
					GEngine->pcfWorld->EntityGIndexes.Add( NULL );
				}

				bool addToWorld = false;
				Entity *e = NULL;
				if( index < ( unsigned int )GEngine->pcfWorld->EntityGIndexes.Size() )
					e = GEngine->pcfWorld->EntityGIndexes[index];
				if( e && e->Type == type && e->Name == name && e->FileName == fname )
				{
//					GLog.Print( "Entity %s already loaded\n", e->FileName.AsChar() );
				}
				else
				{
					e = GEngine->pcfWorld->CreateEntity( type, fname, name, scale, ( flags & Entity::EEntCenteredGeometry ) != 0 );
				}
#else
				while( GEngine->World->EntityGIndexes.Size() < ( int )index )
				{
					GEngine->World->EntityGIndexes.Add( NULL );
				}

				bool addToWorld = false;
				Entity *e = NULL;
				if( index < ( unsigned int )GEngine->World->EntityGIndexes.Size() )
					e = GEngine->World->EntityGIndexes[index];
				if( e && e->Type == type && e->Name == name && e->FileName == fname )
				{
//					GLog.Print( "Entity %s already loaded\n", e->FileName.AsChar() );
				}
				else
				{
					e = GEngine->World->CreateEntity( type, fname, name, scale, ( flags & Entity::EEntCenteredGeometry ) != 0 );
				}
#endif
				addToWorld = true;

				if( e )
				{
					e->Flags = flags;
					e->TimeToDelete = timeToDel;
					e->FadeTime = fadeTime;
					e->Sort = sort;
#ifdef	TRANSGAMING
					if( addToWorld )
						GEngine->pcfWorld->AddEntity( e );

					if( index != e->GIndex )
					{
						int oldIndex = e->GIndex;
						e->GIndex = index;
						GEngine->pcfWorld->EntityGIndexes[index] = e;
						GEngine->pcfWorld->EntityGIndexes[oldIndex] = NULL;
					}
#else
					if( addToWorld )
						GEngine->World->AddEntity( e );

					if( index != e->GIndex )
					{
						int oldIndex = e->GIndex;
						e->GIndex = index;
						GEngine->World->EntityGIndexes[index] = e;
						GEngine->World->EntityGIndexes[oldIndex] = NULL;
					}
#endif

					e->DemoReadFull( mRecordingFile );
				}
				else
				{
//					GLog.Print( "Cannot create entity %d (%s)\n", index, name );
					return Stop();
				}
			}

			if( draw || fullInfo )
			{
				int size = 0;
				SAFE_READ( &size, sizeof( int ) );

				Entity *e = NULL;
#ifdef	TRANSGAMING
				if( GEngine->pcfWorld->EntityGIndexes.Size() > (int)index )
					e = GEngine->pcfWorld->EntityGIndexes[index];
#else
				if( GEngine->World->EntityGIndexes.Size() > (int)index )
					e = GEngine->World->EntityGIndexes[index];
#endif
				if( !e )
				{
//					GLog.Print( "Cannot find entity %d (type %d). Skipping %d bytes\n", index, type, size );
					mRecordingFile->Seek( size, SEEK_CUR );
				}
				else
				{
					e->DemoReadEntity( mRecordingFile );
				}
			}
			else if( !draw )
			{
#ifdef	TRANSGAMING
				if( GEngine->pcfWorld->EntityGIndexes.Size() > (int)index )
				{
					Entity *e = GEngine->pcfWorld->EntityGIndexes[index];
					if( e ) e->EnableDraw( false );
				}
#else
				if( GEngine->World->EntityGIndexes.Size() > (int)index )
				{
					Entity *e = GEngine->World->EntityGIndexes[index];
					if( e ) e->EnableDraw( false );
				}
#endif
			}

			prevType = type;
			prevFull = fullInfo;
			prevIndex = index;
		}

		int numToDelete = 0;
		SAFE_READ( &numToDelete, sizeof( int ) );
		for( int i = 0; i < numToDelete; i++ )
		{
			int del = 0;
			SAFE_READ( &del, sizeof( int ) );
			Entity *e = NULL;
#ifdef	TRANSGAMING
			if( GEngine->pcfWorld->EntityGIndexes.Size() > del )
				e = GEngine->pcfWorld->EntityGIndexes[del];
			if( e )
			{
				GEngine->pcfWorld->DeleteEntity( e );
			}
#else
			if( GEngine->World->EntityGIndexes.Size() > del )
				e = GEngine->World->EntityGIndexes[del];
			if( e )
			{
				GEngine->World->DeleteEntity( e );
			}
#endif
		}
#ifdef SAVE_AUDIO
#ifndef	TRANSGAMING_NO_MILES
		mAudioRec->ExecuteActionsFrom( mRecordingFile, mIsTimeDemo );
#endif // TRANSGAMING_NO_MILES
#endif

		mReady = false;

		// First frame is very time consuming - the time difference between first and second is very big and
		// causes wrong playback speed, so we reset start time after first frame to prevent this
		if( time == 0 )
			mStartTime = currTime;
		lastTime = currTime;
	}
#endif //TRANSGAMING
}


void DemoRecording2::UpdateView( Vector &pos, float yaw, float dir )
{
#ifndef TRANSGAMING

	static Vector lastCamPos( 0.0f, 0.0f, 0.0f );

	if( !IsPlaying() ) return;

	View* view = GEngine->Video->GetActiveView();
	Vector camRot = view->mCamera.GetRawRotation();

	camRot.Z = 0.0f;
	if( dir != 1000.0f )
	{
		camRot.X = 180.0f-dir*180.0/F_PI;
		if( camRot.X < 0.0f ) camRot.X += 360.0f;
		if( camRot.X >= 360.0f ) camRot.X -= 360.0f;
	}

	Vector destPos = pos;

	if( view->mCamera.Interpolation )
	{
		Vector diff = destPos - lastCamPos;
		if( diff.Magnitude() < 3 )				// interpolation
			destPos = lastCamPos + diff * 0.33f;
	}

	view->mCamera.SetPosition( destPos );
	lastCamPos = destPos;

	// y lock
	camRot.Y = yaw;
	if( camRot.Y > +80.0f ) camRot.Y = +80.0f;
	if( camRot.Y < -80.0f ) camRot.Y = -80.0f;

	view->mCamera.SetRotation(camRot);
	view->mCamera.InverseTransform();
#endif //TRANSGAMING
}


void DemoRecording2::RecordPlayer( int playerIndex, unsigned short yaw, int health, int armor )
{
#ifndef TRANSGAMING

	Entity *pe = NULL; // GEngine->World->GetEntityByIndex( playerIndex );
	if( !pe )
	{
//		if( GEngine->Video )
//			GEngine->Video->Hud->mConsole.AddMessage( "ERROR: Couldn't find player entity. Recording stopped." );
//		return Stop();
		playerIndex = -1;
		SAFE_WRITE( &playerIndex, sizeof( int ) );

		View* view = GEngine->Video->GetActiveView();
		Vector camRot = view->mCamera.GetRawRotation();
		Vector camPos = view->mCamera.GetPosition();
		float camFOV = view->mCamera.GetFOV();
		SAFE_WRITE( &camRot.X, sizeof( float ) );
		SAFE_WRITE( &camRot.Y, sizeof( float ) );
		SAFE_WRITE( &camRot.Z, sizeof( float ) );
		SAFE_WRITE( &camPos.X, sizeof( float ) );
		SAFE_WRITE( &camPos.Y, sizeof( float ) );
		SAFE_WRITE( &camPos.Z, sizeof( float ) );
		SAFE_WRITE( &camFOV, sizeof( float ) );

#ifdef	TRANSGAMING
		char ghosts = ( char )GEngine->pcfWorld->GhostsFX;
		char demonEnabled = ( char )GEngine->pcfWorld->DemonFX.Enabled;
		char demonSuper = ( char )GEngine->pcfWorld->DemonFX.Super;
		SAFE_WRITE( &ghosts, sizeof( char ) );
		SAFE_WRITE( &demonEnabled, sizeof( char ) );
		SAFE_WRITE( &demonSuper, sizeof( char ) );
		SAFE_WRITE( &GEngine->pcfWorld->DemonFX.Warp, sizeof( float ) );
#else
		char ghosts = ( char )GEngine->World->GhostsFX;
		char demonEnabled = ( char )GEngine->World->DemonFX.Enabled;
		char demonSuper = ( char )GEngine->World->DemonFX.Super;
		SAFE_WRITE( &ghosts, sizeof( char ) );
		SAFE_WRITE( &demonEnabled, sizeof( char ) );
		SAFE_WRITE( &demonSuper, sizeof( char ) );
		SAFE_WRITE( &GEngine->World->DemonFX.Warp, sizeof( float ) );
#endif

		return;
	}

	SAFE_WRITE( &playerIndex, sizeof( int ) );
	SAFE_WRITE( &yaw, sizeof( short ) );

	float dir = 1000.0f;
	Vector pos( 0.0f, 0.0f, 0.0f );
	if( pe->PO )
	{
#ifndef TRANSGAMING_NO_HAVOK
		dir = pe->PO->GetOrientation();
		pos = pe->PO->GetPawnHeadPos();
#endif
	}
	else
	{
		dir = pe->GetOrientation();
		pos = pe->GetPosition();
	}

	SAFE_WRITE( &dir, sizeof( float ) );
	SAFE_WRITE( &pos.X, sizeof( float ) );
	SAFE_WRITE( &pos.Y, sizeof( float ) );
	SAFE_WRITE( &pos.Z, sizeof( float ) );

	SAFE_WRITE( &health, sizeof( int ) );
	SAFE_WRITE( &armor, sizeof( int ) );

	DWORD actions = GEngine->InputSys->GetActionStatus();
	SAFE_WRITE( &actions, sizeof( DWORD ) );
#endif //TRANSGAMING
}


// Header
bool DemoRecording2::ReadHeader( String& fileVer, String& fileNetVer, long32& timeDiff )
{
#ifndef TRANSGAMING

	char buffer[1400];
	int stringSize;
	SAFE_READ_BOOL( &stringSize, sizeof( int ) );
	SAFE_READ_BOOL( buffer, stringSize );
	buffer[stringSize] = 0;
	fileVer = buffer;
	SAFE_READ_BOOL( &stringSize, sizeof( int ) );
	SAFE_READ_BOOL( buffer, stringSize );
	buffer[stringSize] = 0;
	fileNetVer = buffer;

	long32 tDiff;
	SAFE_READ_BOOL( &tDiff, sizeof( long32 ) );
	timeDiff = tDiff;
#endif //TRANSGAMING
	return true;
}


bool DemoRecording2::WriteHeader( String fileVer, String fileNetVer, long32 timeDiff )
{
#ifndef TRANSGAMING

	int stringSize = fileVer.Len();
	SAFE_WRITE_BOOL( &stringSize, sizeof( int ) );
	SAFE_WRITE_BOOL( fileVer.AsChar(), stringSize );
	stringSize = fileNetVer.Len();
	SAFE_WRITE_BOOL( &stringSize, sizeof( int ) );
	SAFE_WRITE_BOOL( fileNetVer.AsChar(), stringSize );
	SAFE_WRITE_BOOL( &timeDiff, sizeof( long32 ) );

#endif //TRANSGAMING
    return true;
}
#endif
