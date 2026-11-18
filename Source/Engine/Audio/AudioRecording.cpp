
#include "Engine.h"
#include "Audio/AudioRecording.h"
#include "Net/UdpMessage.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include <stdio.h>
#include <stdarg.h>

AudioRecorder::AudioRecorder( MilesEngine* engineToRecord ) : mMiles( engineToRecord ), mBuffers( 0 ), mPlaying( false )
{
#ifndef FINAL_BUILD
	if( !mMiles )
		GLog.Print( "Got nil audio engine !!! Crash imminent!!!\n" );
#endif
	Assert( mMiles );
	mMiles->SetAudioRecorder( this );

    m_setTag("AudioRecorder");
    mBuffers = new AudioActionBuffer;
	mBuffers->Next = 0;
}

AudioRecorder::~AudioRecorder( void )
{
	while( mBuffers )
	{
		AudioActionBuffer* t = mBuffers;
		mBuffers = mBuffers->Next;
		delete t;
	}
	if( mMiles )
		mMiles->SetAudioRecorder( 0 );
	mMiles = 0;
}

void AudioRecorder::SetAction( unsigned char action, ... )
{
	if( mPlaying ) return;	// don't record actions when you're playing the recording

	va_list argPtr;
	va_start( argPtr, action );

	UdpMessage mesg;
	mesg.WriteUChar( action );

	switch( action )
	{
	case PreloadFile:										// 1 string
	case StreamingSound_Create:
		{
			const char* name = va_arg(argPtr,const char*);
			mesg.WriteString( name );
		}
		break;
	case SetSpeed:											// 2 float
	case SetRoomIntensity2D:
	case SetRoomIntensity3D:
		{
			float firstFloat = (float)va_arg(argPtr,double);
			float secondFloat = (float)va_arg(argPtr,double);
			mesg.WriteFloat( firstFloat );
			mesg.WriteFloat( secondFloat );
		}
		break;
	case SetLowPass:										// 1 float
	case SetMasterVolumeLevel:
	case SetFalloffSpeed:
	case SetStreamingVolume:
	case Set3DDigitalEffectsVolume:
	case SetSoundSpeedRandomizer:
	case SetAllSoundsToLowPass:
		{
			float oneFloat = (float)va_arg(argPtr,double);
			mesg.WriteFloat( oneFloat );
		}
		break;
	case SetSpeakerType:									// 1 int
	case ResumeSounds:
	case Sound2D_Play:
	case Sound2D_PlayAndForget:
	case Sound2D_Resume:
	case Sound2D_Stop:
	case Sound2D_Forget:
	case Sound2D_Delete:
	case Sound2D_SetDontSave:
	case Sound2D_NoSpeedRandomize:
	case Sound3D_Play:
	case Sound3D_PlayAndForget:
	case Sound3D_Resume:
	case Sound3D_Stop:
	case Sound3D_Forget:
	case Sound3D_Delete:
	case Sound3D_SetDontSave:
	case Sound3D_SetNoSpeedRandomize:
	case StreamingSound_Play:
	case StreamingSound_Resume:
	case StreamingSound_Stop:
	case StreamingSound_Delete:
		{
			int oneInt = va_arg(argPtr,int);
			mesg.WriteLong( oneInt );
		}
		break;
	case SetSpeakersFlip:									// 1 bool
	case SetDefaultObstruction:
		{
//joe		bool oneBool = va_arg(argPtr,bool);
			bool oneBool = (bool)va_arg(argPtr,int);
			mesg.WriteBool( oneBool );
		}
		break;
	case SetObstructionProperties:							// 7 float
		{
			float heightenDelay = (float)va_arg(argPtr,double);
			float lowerDelay = (float)va_arg(argPtr,double);
			float minObstruction = (float)va_arg(argPtr,double);
			float maxObstructionLow = (float)va_arg(argPtr,double);
			float maxObstructionHigh = (float)va_arg(argPtr,double);
			float minObstructionDistance = (float)va_arg(argPtr,double);
			float maxObstructionDistance = (float)va_arg(argPtr,double);
			mesg.WriteFloat( heightenDelay );
			mesg.WriteFloat( lowerDelay );
			mesg.WriteFloat( minObstruction );
			mesg.WriteFloat( maxObstructionLow );
			mesg.WriteFloat( maxObstructionHigh );
			mesg.WriteFloat( minObstructionDistance );
			mesg.WriteFloat( maxObstructionDistance );
		}
		break;
	case SetRoom:											// 1 long32, 1 bool
	case Sound2D_SetAlwaysSameSpeed:
	case Sound3D_SetObstructed:
		{
			int roomType = va_arg(argPtr,int);
//joe		bool set2D = va_arg(argPtr,bool);
			bool set2D = (bool)va_arg(argPtr,int);	
			mesg.WriteLong( roomType );
			mesg.WriteBool( set2D );
		}
		break;
	case PauseCurrentlyPlayingSounds:						// void
	case StopAllSounds:
	case LoadFilesOffMemory:
		break;
	case SetSoundProperties:								// 1 string, 2 long32
		{
			const char* name = va_arg(argPtr,const char*);
			mesg.WriteString( name );
			int maximalInstances = va_arg(argPtr,int);
			mesg.WriteLong( maximalInstances );
			int minimalDelay = va_arg(argPtr,int);
			mesg.WriteLong( minimalDelay );
		}
		break;
	case Sound2D_Create:									// 1 string, 1 unsigned char
	case Sound3D_Create:
		{
			const char* name = va_arg(argPtr,const char*);
			mesg.WriteString( name );
//joe		unsigned char priority = va_arg(argPtr,unsigned char);
			unsigned char priority = (unsigned char)va_arg(argPtr,int);	
			mesg.WriteUChar( priority );
		}
		break;
	case Sound2D_CreateEx:									// string, unsigned char, float, 3 bool, float, bool
		{
			const char* name = va_arg(argPtr,const char*);
			mesg.WriteString( name );
//joe		unsigned char priority = va_arg(argPtr,unsigned char);
			unsigned char priority = (unsigned char)va_arg(argPtr,int);
			mesg.WriteUChar( priority );
			float volume = (float)va_arg(argPtr,double);
//joe		bool samespeed = va_arg(argPtr,bool);
			bool samespeed = (bool)va_arg(argPtr,int);	
//joe		bool noSpeedRandomize = va_arg(argPtr,bool);
			bool noSpeedRandomize = (bool)va_arg(argPtr,int);
//joe		bool playAndForget = va_arg(argPtr,bool);
			bool playAndForget = (bool)va_arg(argPtr,int);
			float intensity = (float)va_arg(argPtr,double);
//joe		bool dontSave = va_arg(argPtr,bool);
			bool dontSave = (bool)va_arg(argPtr,int);
			mesg.WriteFloat( volume );
			mesg.WriteBool( samespeed );
			mesg.WriteBool( noSpeedRandomize );
			mesg.WriteBool( playAndForget );
			mesg.WriteFloat( intensity );
			mesg.WriteBool( dontSave );
		}
		break;
	case Sound2D_SetLoopCount:								// 2 int
	case Sound3D_SetLoopCount:
	case StreamingSound_SetLoopCount:
		{
			int index = va_arg(argPtr,int);
			int count = va_arg(argPtr,int);
			mesg.WriteLong( index );
			mesg.WriteLong( count );
		}
		break;
	case Sound2D_SetVolume:									// int, 2 float
	case Sound3D_SetVolume:
	case Sound3D_SetIntensity:
	case Sound3D_SetHearingDistance:
		{
			int index = va_arg(argPtr,int);
			float volume = (float)va_arg(argPtr,double);
			float switchTime = (float)va_arg(argPtr,double);
			mesg.WriteLong( index );
			mesg.WriteFloat( volume );
			mesg.WriteFloat( switchTime );
		}
		break;
	case Sound2D_SetSoundSpeed:								// int, float
	case Sound2D_SetLowPass:
	case Sound3D_SetSoundSpeed:
	case Sound3D_SetLowPass:
	case StreamingSound_SetVolume:
	case StreamingSound_SetLowPass:
		{
			int index = va_arg(argPtr,int);
			float speed = (float)va_arg(argPtr,double);
			mesg.WriteLong( index );
			mesg.WriteFloat( speed );
		}
		break;
	case Sound3D_CreateEx:
		{
//joe		unsigned char priority = va_arg(argPtr,unsigned char);
			unsigned char priority = (unsigned char)va_arg(argPtr,int);
			mesg.WriteUChar( priority );
//joe		bool playAtOnce = va_arg(argPtr,bool);
			bool playAtOnce = (bool)va_arg(argPtr,int);
			mesg.WriteBool( playAtOnce );
			float loopCount = (float)va_arg(argPtr,double);
			mesg.WriteFloat( loopCount );
			float posX = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posX );
			float posY = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posY );
			float posZ = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posZ );
			float mindist = (float)va_arg(argPtr,double);
			mesg.WriteFloat( mindist );
			float maxdist = (float)va_arg(argPtr,double);
			mesg.WriteFloat( maxdist );
			float volume = (float)va_arg(argPtr,double);
			mesg.WriteFloat( volume );
//joe		bool noSpeedRandomize = va_arg(argPtr,bool);
			bool noSpeedRandomize = (bool)va_arg(argPtr,int);
			mesg.WriteBool( noSpeedRandomize );
//joe		bool dontSave = va_arg(argPtr,bool);
			bool dontSave = (bool)va_arg(argPtr,int);
			mesg.WriteBool( dontSave );
//joe		bool obstructed = va_arg(argPtr,bool);
			bool obstructed = (bool)va_arg(argPtr,int);
			mesg.WriteBool( obstructed );
			float intensity = (float)va_arg(argPtr,double);
			mesg.WriteFloat( intensity );
			const char* name = va_arg(argPtr,const char*);
			mesg.WriteString( name );
		}
		break;
	case Sound3D_SetPosition:								// int, 3 float
	case Sound3D_SetVelocity:
		{
			int index = va_arg(argPtr,int);
			mesg.WriteLong( index );
			float posX = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posX );
			float posY = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posY );
			float posZ = (float)va_arg(argPtr,double);
			mesg.WriteFloat( posZ );
		}
		break;
	case Sound3D_SetOrientation:
		{
			int index = va_arg(argPtr,int);
			mesg.WriteLong( index );
			float f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
		}
		break;
	case SetPlayerPosition:
	case SetPlayerVelocity:
		{
			float f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
		}
		break;
	case SetPlayerOrientation:
		{
			float f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
			f = (float)va_arg(argPtr,double);
			mesg.WriteFloat( f );
		}
		break;
	default:
		GLog.Print( "Unknown action called in AudioRecorder::SetAction()!!!\n" );
		break;
	};

	va_end( argPtr );
	mesg.EndWriting();
	mesg.SeekToBegin();

	if( mesg.GetSize()+mBuffers->Buffer.GetPos() >= AUDIO_RECORDER_BUFFER_SIZE )
	{
		mBuffers->Buffer.EndWriting();

        m_setTag("AudioRecorder");
        AudioActionBuffer* buf = new AudioActionBuffer;
		buf->Next = mBuffers;
		mBuffers = buf;
	}
	mBuffers->Buffer.WriteData( &mesg );
}


void AudioRecorder::FlushActionsTo( FileObject* file )
{
	Assert( mBuffers );

	DynamicArray<AudioActionBuffer*> bufs;
	AudioActionBuffer* t = mBuffers;
	while( t )
	{
		bufs.Add(t);
		t = t->Next;
	}

	if( mBuffers ) mBuffers->Buffer.EndWriting();

	unsigned char bufsToWrite = bufs.Size();
	file->Write( &bufsToWrite, sizeof(unsigned char) );
	for( int i = bufs.Size()-1; i >= 0; --i )
	{
		unsigned short size = bufs[i]->Buffer.GetSize();
		file->Write( &size, sizeof(unsigned short) );
		file->Write( bufs[i]->Buffer.GetBuffer(), size );
	}

	while( mBuffers->Next )
	{
		AudioActionBuffer* t = mBuffers;
		mBuffers = mBuffers->Next;
		delete t;
	}

	mBuffers->Buffer.SeekToBegin();
	mBuffers->Buffer.EndWriting();
}

void AudioRecorder::ExecuteActionsFrom( FileObject* file, bool skipOnly )
{
	unsigned char bufsToRead;
	file->Read( &bufsToRead, sizeof(unsigned char) );
	unsigned char buffer[AUDIO_RECORDER_BUFFER_SIZE];

	for( long32 i = 0; i < bufsToRead; ++i )
	{
		unsigned short mesgSize;
		file->Read( &mesgSize, sizeof(unsigned short) );
		Assert( mesgSize <= AUDIO_RECORDER_BUFFER_SIZE );
		file->Read( &buffer, mesgSize );
		if( skipOnly )
			continue;
		UdpMessage mesg;
		mesg.WriteData( (char*)buffer, mesgSize );
		mesg.EndWriting();
		mesg.SeekToBegin();

		while( !mesg.ReadFinished() )
		{
			unsigned char action = mesg.ReadUChar();
			switch( action )
			{
			case PreloadFile:
				{
					String name = mesg.ReadString();
					mMiles->PreloadFile( name.AsChar() );
				}
				break;
			case SetSpeed:
				{
					float speed = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->SetSpeed( speed, switchTime );
				}
				break;
			case SetObstructionProperties:
				{
					float heightenDelay = mesg.ReadFloat();
					float lowerDelay = mesg.ReadFloat();
					float minObstruction = mesg.ReadFloat();
					float maxObstructionLow = mesg.ReadFloat();
					float maxObstructionHigh = mesg.ReadFloat();
					float minObstructionDistance = mesg.ReadFloat();
					float maxObstructionDistance = mesg.ReadFloat();
					mMiles->SetObstructionProperties( heightenDelay, lowerDelay, minObstruction, maxObstructionLow,
						maxObstructionHigh, minObstructionDistance, maxObstructionDistance );
				}
				break;
			case SetRoom:
				{
					int roomType = mesg.ReadLong();
					bool set2D = mesg.ReadBool();
					mMiles->SetRoom( roomType, set2D );
				}
				break;
			case SetRoomIntensity2D:
				{
					float intensity = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->SetRoomIntensity2D( intensity, switchTime );
				}
				break;
			case SetRoomIntensity3D:
				{
					float intensity = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->SetRoomIntensity3D( intensity, switchTime );
				}
				break;
			case SetSoundProperties:
				{
					String name = mesg.ReadString();
					int maximalInstances = mesg.ReadLong();
					int minimalDelay = mesg.ReadLong();
#ifdef TRANSGAMING
					mMiles->SetSoundProperties( name.AsChar(), maximalInstances, minimalDelay );
#else
					mMiles->SetSoundProperties( name, maximalInstances, minimalDelay );
#endif
				}
				break;
			case Sound2D_Create:
				{
					String name = mesg.ReadString();
					unsigned char priority = mesg.ReadUChar();
					mMiles->Sound2D_Create( name.AsChar(), priority );
				}
				break;
			case Sound2D_CreateEx:
				{
					String name = mesg.ReadString();
					unsigned char priority = mesg.ReadUChar();
					float volume = mesg.ReadFloat();
					bool samespeed = mesg.ReadBool();
					bool noSpeedRandomize = mesg.ReadBool();
					bool playAndForget = mesg.ReadBool();
					float intensity = mesg.ReadFloat();
					bool dontSave = mesg.ReadBool();
					mMiles->Sound2D_CreateEx( name.AsChar(), priority, volume, samespeed, noSpeedRandomize, playAndForget,
						intensity, dontSave );
				}
				break;
			case Sound2D_SetLoopCount:
				{
					int index = mesg.ReadLong();
					int count = mesg.ReadLong();
					mMiles->Sound2D_SetLoopCount( index, count );
				}
				break;
			case StreamingSound_SetLoopCount:
				{
					int index = mesg.ReadLong();
					int count = mesg.ReadLong();
					mMiles->StreamingSound_SetLoopCount( index, count );
				}
				break;
			case Sound3D_SetLoopCount:
				{
					int index = mesg.ReadLong();
					int count = mesg.ReadLong();
					mMiles->Sound3D_SetLoopCount( index, count );
				}
				break;
			case Sound2D_SetVolume:
				{
					int index = mesg.ReadLong();
					float volume = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->Sound2D_SetVolume( index, volume, switchTime );
				}
				break;
			case Sound3D_SetVolume:
				{
					int index = mesg.ReadLong();
					float volume = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->Sound3D_SetVolume( index, volume, switchTime );
				}
				break;
			case Sound3D_SetIntensity:
				{
					int index = mesg.ReadLong();
					float intensity = mesg.ReadFloat();
					float switchTime = mesg.ReadFloat();
					mMiles->Sound3D_SetIntensity( index, intensity, switchTime );
				}
				break;
			case Sound3D_SetHearingDistance:
				{
					int index = mesg.ReadLong();
					float minDist = mesg.ReadFloat();
					float maxDist = mesg.ReadFloat();
					mMiles->Sound3D_SetIntensity( index, minDist, maxDist );
				}
				break;
			case Sound2D_SetSoundSpeed:
				{
					int index = mesg.ReadLong();
					float speed = mesg.ReadFloat();
					mMiles->Sound2D_SetSoundSpeed( index, speed );
				}
				break;
			case Sound2D_SetLowPass:
				{
					int index = mesg.ReadLong();
					float lowPass = mesg.ReadFloat();
					mMiles->Sound2D_SetLowPass( index, lowPass );
				}
				break;
			case Sound3D_SetSoundSpeed:
				{
					int index = mesg.ReadLong();
					float speed = mesg.ReadFloat();
					mMiles->Sound3D_SetSoundSpeed( index, speed );
				}
				break;
			case Sound3D_SetLowPass:
				{
					int index = mesg.ReadLong();
					float lowPass = mesg.ReadFloat();
					mMiles->Sound3D_SetLowPass( index, lowPass );
				}
				break;
			case StreamingSound_SetVolume:
				{
					int index = mesg.ReadLong();
					float volume = mesg.ReadFloat();
					mMiles->StreamingSound_SetVolume( index, volume );
				}
				break;
			case StreamingSound_SetLowPass:
				{
					int index = mesg.ReadLong();
					float lowPass = mesg.ReadFloat();
					mMiles->StreamingSound_SetLowPass( index, lowPass );
				}
				break;
			case Sound2D_SetAlwaysSameSpeed:
				{
					int index = mesg.ReadLong();
					bool set = mesg.ReadBool();
					mMiles->Sound2D_SetAlwaysSameSpeed( index, set );
				}
				break;
			case Sound3D_Create:
				{
					String name = mesg.ReadString();
					unsigned char priority = mesg.ReadUChar();
					mMiles->Sound3D_Create( name.AsChar(), priority );
				}
				break;
			case Sound3D_CreateEx:
				{
                    unsigned char priority = mesg.ReadUChar();
					bool playAtOnce = mesg.ReadBool();
					float loopCount = mesg.ReadFloat();
					float posX = mesg.ReadFloat();
					float posY = mesg.ReadFloat();
					float posZ = mesg.ReadFloat();
					float mindist = mesg.ReadFloat();
					float maxdist = mesg.ReadFloat();
					float volume = mesg.ReadFloat();
					bool noSpeedRandomize = mesg.ReadBool();
					bool dontSave = mesg.ReadBool();
					bool obstructed = mesg.ReadBool();
					float intensity = mesg.ReadFloat();
					String name = mesg.ReadString();
					mMiles->Sound3D_CreateEx( name.AsChar(), priority, playAtOnce, loopCount, Vector(posX,posY,posZ), mindist,
						maxdist, volume, noSpeedRandomize, dontSave, obstructed, intensity );
				}
				break;
			case Sound3D_SetPosition:
				{
					int index = mesg.ReadLong();
					float posX = mesg.ReadFloat();
					float posY = mesg.ReadFloat();
					float posZ = mesg.ReadFloat();
					Vector pos( posX, posY, posZ );
					mMiles->Sound3D_SetPosition( index, pos );
				}
				break;
			case Sound3D_SetVelocity:
				{
					int index = mesg.ReadLong();
					float velX = mesg.ReadFloat();
					float velY = mesg.ReadFloat();
					float velZ = mesg.ReadFloat();
					Vector vel( velX, velY, velZ );
					mMiles->Sound3D_SetVelocity( index, vel );
				}
				break;
			case Sound3D_SetObstructed:
				{
					int index = mesg.ReadLong();
					bool obstructed = mesg.ReadBool();
					mMiles->Sound3D_SetObstructed( index, obstructed );
				}
				break;
			case Sound3D_SetOrientation:
				{
					int index = mesg.ReadLong();
					float frontX = mesg.ReadFloat();
					float frontY = mesg.ReadFloat();
					float frontZ = mesg.ReadFloat();
					Vector front( frontX, frontY, frontZ );
					float upX = mesg.ReadFloat();
					float upY = mesg.ReadFloat();
					float upZ = mesg.ReadFloat();
					Vector up( upX, upY, upZ );
					mMiles->Sound3D_SetOrientation( index, front, up );
				}
				break;
			case StreamingSound_Create:
				{
					String name = mesg.ReadString();
					mMiles->StreamingSound_Create( name.AsChar() );
				}
				break;
			case SetLowPass:					mMiles->SetLowPass( mesg.ReadFloat() );					break;
			case SetMasterVolumeLevel:			mMiles->SetMasterVolumeLevel( mesg.ReadFloat() );		break;
			case SetFalloffSpeed:				mMiles->SetFalloffSpeed( mesg.ReadFloat() );			break;
			case SetStreamingVolume:			mMiles->SetStreamingVolume( mesg.ReadFloat() );			break;
			case Set3DDigitalEffectsVolume:		mMiles->Set3DDigitalEffectsVolume( mesg.ReadFloat() );	break;
			case SetSpeakerType:				mMiles->SetSpeakerType( mesg.ReadLong() );				break;
			case SetSpeakersFlip:				mMiles->SetSpeakersFlip( mesg.ReadBool() );				break;
			case SetSoundSpeedRandomizer:		mMiles->SetSoundSpeedRandomizer( mesg.ReadFloat() );	break;
			case SetAllSoundsToLowPass:			mMiles->SetAllSoundsToLowPass( mesg.ReadFloat() );		break;
			case SetDefaultObstruction:			mMiles->SetDefaultObstruction( mesg.ReadBool() );		break;
			case PauseCurrentlyPlayingSounds:	mMiles->PauseCurrentlyPlayingSounds();					break;
			case ResumeSounds:					mMiles->ResumeSounds( mesg.ReadLong() );				break;
			case StopAllSounds:					mMiles->StopAllSounds();								break;
			case LoadFilesOffMemory:			mMiles->LoadFilesOffMemory();							break;
			case Sound2D_Play:					mMiles->Sound2D_Play( mesg.ReadLong() );				break;
			case Sound2D_PlayAndForget:			mMiles->Sound2D_PlayAndForget( mesg.ReadLong() );		break;
			case Sound2D_Resume:				mMiles->Sound2D_Resume( mesg.ReadLong() );				break;
			case Sound2D_Stop:					mMiles->Sound2D_Stop( mesg.ReadLong() );				break;
			case Sound2D_Forget:				mMiles->Sound2D_Forget( mesg.ReadLong() );				break;
			case Sound2D_Delete:				mMiles->Sound2D_Delete( mesg.ReadLong() );				break;
			case Sound2D_SetDontSave:			mMiles->Sound2D_SetDontSave( mesg.ReadLong() );			break;
			case Sound2D_NoSpeedRandomize:		mMiles->Sound2D_NoSpeedRandomize( mesg.ReadLong() );	break;
			case Sound3D_Play:					mMiles->Sound3D_Play( mesg.ReadLong() );				break;
			case Sound3D_PlayAndForget:			mMiles->Sound3D_PlayAndForget( mesg.ReadLong() );		break;
			case Sound3D_Resume:				mMiles->Sound3D_Resume( mesg.ReadLong() );				break;
			case Sound3D_Stop:					mMiles->Sound3D_Stop( mesg.ReadLong() );				break;
			case Sound3D_Forget:				mMiles->Sound3D_Forget( mesg.ReadLong() );				break;
			case Sound3D_Delete:				mMiles->Sound3D_Delete( mesg.ReadLong() );				break;
			case Sound3D_SetDontSave:			mMiles->Sound3D_SetDontSave( mesg.ReadLong() );			break;
			case Sound3D_SetNoSpeedRandomize:	mMiles->Sound3D_SetNoSpeedRandomize( mesg.ReadLong() );	break;
			case StreamingSound_Play:			mMiles->StreamingSound_Play( mesg.ReadLong() );			break;
			case StreamingSound_Resume:			mMiles->StreamingSound_Resume( mesg.ReadLong() );		break;
			case StreamingSound_Stop:			mMiles->StreamingSound_Stop( mesg.ReadLong() );			break;
			case StreamingSound_Delete:			mMiles->StreamingSound_Delete( mesg.ReadLong() );		break;
			case SetPlayerPosition:
				{
					float posX = mesg.ReadFloat();
					float posY = mesg.ReadFloat();
					float posZ = mesg.ReadFloat();
					Vector pos( posX, posY, posZ );
					mMiles->SetPlayerPosition( pos );
				}
				break;
			case SetPlayerVelocity:
				{
					float velX = mesg.ReadFloat();
					float velY = mesg.ReadFloat();
					float velZ = mesg.ReadFloat();
					Vector vel( velX, velY, velZ );
					mMiles->SetPlayerVelocity( vel );
				}
				break;
			case SetPlayerOrientation:
				{
					float frontX = mesg.ReadFloat();
					float frontY = mesg.ReadFloat();
					float frontZ = mesg.ReadFloat();
					Vector front( frontX, frontY, frontZ );
					float upX = mesg.ReadFloat();
					float upY = mesg.ReadFloat();
					float upZ = mesg.ReadFloat();
					Vector up( upX, upY, upZ );
					mMiles->SetPlayerOrientation( front, up );
				}
				break;
			default:
				GLog.Print( "Unknown action called in AudioRecorder::SetAction()!!!\n" );
				break;
			}
		}
	}
}

void AudioRecorder::StartRecordingTo( FileObject* file )
{
	mMiles->SaveGame_PauseSounds();
	mMiles->SaveAudio( file );
	mMiles->SaveGame_ResumeSounds();
	mPlaying = false;
}

void AudioRecorder::StartPlayingFrom( FileObject* file, bool skipOnly )
{
	mPlaying = true;
	mMiles->LoadAudio( file );
	if( skipOnly )
		mMiles->LoadReset();
	else
		mMiles->SaveGame_ResumeSounds();
}
