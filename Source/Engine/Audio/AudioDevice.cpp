
#include "Engine.h"
#include "AudioDevice.h"
#include "MilesEngine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#define MSS_REDIST_DIRNAME ".\\Miles\\"


AudioDevice::AudioDevice() : mMiles(0) {}

AudioDevice::~AudioDevice()
{
	if( mMiles )	// engine has been started - stop it
	{
		delete mMiles;
		mMiles = 0;
		AIL_shutdown();
	};
}

BOOL AudioDevice::Init( void )
{
	if( mMiles )	// Already started
		return TRUE;

	AIL_set_redist_directory( MSS_REDIST_DIRNAME );
	if( !AIL_startup() )
	{
		GLog.Print( "Audio Library initialization error: %s\n", AIL_last_error() );
		return FALSE;
	};

    m_setTag("Audio");
	mMiles = new MilesEngine();
	mMiles->DigitalDriver = AIL_open_digital_driver( 44100, 16, 2, 0 );

    return TRUE;
}

void AudioDevice::StartStreams( DynamicArray<String>& names )
{
	for( int i = 0; i < names.Size(); ++i )
		if( !( names[i].IsNull()) )
		{
			int channel = GEngine->Audio->StreamingSound_Create(names[i].AsChar());
			if( channel != -1 )
			{
				GEngine->Audio->StreamingSound_SetLoopCount( channel, 0 );
				GEngine->Audio->StreamingSound_Play( channel );
			}
		}
}

int AudioDevice::StreamingSound_Create( const char* name ) { Assert( mMiles ); return mMiles->StreamingSound_Create( name ); }
void AudioDevice::StreamingSound_Play( int index ) { Assert( mMiles ); mMiles->StreamingSound_Play( index ); }
void AudioDevice::StreamingSound_Stop( int index ) { Assert( mMiles ); mMiles->StreamingSound_Stop( index ); }
void AudioDevice::StreamingSound_Resume( int index ) { Assert( mMiles ); mMiles->StreamingSound_Resume( index ); }
int AudioDevice::StreamingSound_GetLoopCount( int index ) { Assert( mMiles ); return mMiles->StreamingSound_GetLoopCount( index ); };
void AudioDevice::StreamingSound_SetLoopCount( int index, int count ) { Assert( mMiles ); mMiles->StreamingSound_SetLoopCount( index, count ); }
void AudioDevice::StreamingSound_SetVolumeAndPan( int index, float volume, float pan ) { Assert( mMiles ); mMiles->StreamingSound_SetVolumeAndPan( index, volume, pan ); };
void AudioDevice::StreamingSound_GetVolumeAndPan( int index, float& volume, float& pan ) { Assert( mMiles ); mMiles->StreamingSound_GetVolumeAndPan( index, volume, pan ); };
void AudioDevice::StreamingSound_SetLowPass( int index, float lowPass ) { Assert( mMiles ); mMiles->StreamingSound_SetLowPass( index, lowPass ); }
float AudioDevice::StreamingSound_GetLowPass( int index ) { Assert( mMiles ); return mMiles->StreamingSound_GetLowPass( index ); }
bool AudioDevice::StreamingSound_IsPlaying( int index ) { Assert( mMiles ); return mMiles->StreamingSound_IsPlaying( index ); };

void AudioDevice::GetStreamNames( DynamicArray<String>& names ) { names = mMiles->mStreamNames; }
void AudioDevice::StreamingSound_Delete( int index ) { Assert( mMiles ); mMiles->StreamingSound_Delete( index ); }
float AudioDevice::GetMasterVolumeLevel( void ) { return ( mMiles && mMiles->DigitalDriver )?AIL_digital_master_volume_level( mMiles->DigitalDriver ):1.0f; }
void AudioDevice::GetDigitalEffectsPan( float& pan ) { if( !mMiles || !mMiles->DigitalDriver ) return; pan = mMiles->DigitalEffectsPan; }
void AudioDevice::SetDigitalEffectsPan( float pan ) { if( !mMiles ) return; mMiles->SetDigitalEffectsPan( pan ); }
void AudioDevice::GetStreamingVolumeAndPan( float& volume, float& pan ) { volume = mMiles->StreamingVolume; pan = mMiles->StreamingPan; }
float AudioDevice::Get3DDigitalEffectsVolume( void ) { float currMas = AIL_digital_master_volume_level( mMiles->DigitalDriver ); return mMiles->DigitalEffectsVolume3D / currMas; }
inline void AudioDevice::Set3DDigitalEffectsVolume( float volume ) { if( !mMiles ) return; mMiles->Set3DDigitalEffectsVolume( volume ); }
bool AudioDevice::Set3DSoundProvider( int providerIndex ) { if( !mMiles ) return false; return mMiles->Set3DSoundProvider( providerIndex ); }
int AudioDevice::GetNumberOfProviders( void ) { return mMiles?mMiles->NumOfProviders():0; }
String AudioDevice::GetProviderName( int providerIndex ) { return mMiles?mMiles->mProviderNames[providerIndex]:String(""); }
inline void AudioDevice::Tick( float timeDelta ) { if (!mMiles) return; mMiles->Tick( timeDelta ); }
void AudioDevice::SetPlayerPosition( Vector& position ) { if( mMiles && mMiles->Listener ) AIL_set_3D_position( mMiles->Listener, position.X, position.Y, -position.Z ); }
int AudioDevice::GetCurrentSoundProvider( void ) { return mMiles?mMiles->mCurrentSoundProvider:-1; }
#ifdef TRANSGAMING
void AudioDevice::SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay ) { if (!mMiles) return; mMiles->SetSoundProperties( soundName, maximalInstances, minimalDelay ); }
#else
void AudioDevice::SetSoundProperties( String soundName, int maximalInstances, int minimalDelay ) { if (!mMiles) return; mMiles->SetSoundProperties( soundName, maximalInstances, minimalDelay ); }
#endif
inline void AudioDevice::SetSpeed( float speed, float switchTime ) { if (!mMiles) return; mMiles->SetSpeed( speed, switchTime ); };
inline void AudioDevice::SetLowPass( float percent ) { if (!mMiles) return; mMiles->SetLowPass( percent ); };
inline void AudioDevice::SetAllSoundsToLowPass( float percent ) { if (!mMiles) return; mMiles->SetAllSoundsToLowPass( percent ); };
inline void AudioDevice::SetAllSoundsVolumeMultiplier( float percent ) { if (!mMiles) return; mMiles->SetAllSoundsVolumeMultiplier( percent ); };
inline void AudioDevice::SetSpeakersFlip( bool flipped ) { if (!mMiles) return; mMiles->mSpeakersFlip = flipped; };
inline void AudioDevice::SetDopplerFactor( float factor ) { if( !mMiles ) return; mMiles->SetDopplerFactor( factor ); }
inline SoundList* AudioDevice::PauseCurrentlyPlayingSounds( void ) { return mMiles?mMiles->PauseCurrentlyPlayingSounds():0; }
inline void AudioDevice::ResumeSounds( SoundList* sounds ) { if( mMiles ) mMiles->ResumeSounds( sounds ); if( sounds ) delete sounds; }

void AudioDevice::SetMasterVolumeLevel( float level )
{
	if( mMiles && mMiles->DigitalDriver )
	{
		float curr3DVol = Get3DDigitalEffectsVolume();
		AIL_set_digital_master_volume_level( mMiles->DigitalDriver, level );
		Set3DDigitalEffectsVolume( curr3DVol );
	}
}

void AudioDevice::Play2DSound( const char* name, float volume, bool playAtOnce )
{
	if( !mMiles || !mMiles->DigitalDriver ) return;
	MilesLoadedFile* file = mMiles->PrepareSample( name, playAtOnce );
	if( file )
		mMiles->Play2DSound( file, volume );
}

void AudioDevice::SetStreamingVolumeAndPan( float volume, float pan )
{
	if( !mMiles || !mMiles->DigitalDriver )
		return;

	bool changed = false;

	if( volume > 1.0f ) volume = 1.0f;
	if( volume < 0.0f ) volume = 0.0f;
	if( mMiles->StreamingVolume != volume ) changed = true;
	mMiles->StreamingVolume = volume;

	if( pan > 1.0f ) pan = 1.0f;
	if( pan < 0.0f ) pan = 0.0f;
	if( mMiles->StreamingPan != pan ) changed = true;
	mMiles->StreamingPan = pan;

	// Update digital samples
	if( changed )
	{
		for( int i = 0; i < mMiles->mStreams.Size(); i++ )
		{
			if( mMiles->mStreams[i] )
			{
				if( ( AIL_stream_status( mMiles->mStreams[i] ) == SMP_PLAYING ) ||
					( AIL_stream_status( mMiles->mStreams[i] ) == SMP_STOPPED ) )
					AIL_set_stream_volume_pan( mMiles->mStreams[i], mMiles->StreamingVolume*mMiles->mVolumeMultiplier, mMiles->StreamingPan );
			}
		}
	}
}

void AudioDevice::SetSpeakerType( int type )
{
	if( !mMiles || !mMiles->DigitalProvider )
		return;

	S32 speakerType = 0;
	switch( type )
	{
	default:
	case Speakers2:		speakerType = AIL_3D_2_SPEAKER;		break;
	case Headphones:	speakerType = AIL_3D_HEADPHONE;		break;
	case Surround:		speakerType = AIL_3D_SURROUND;		break;
	case Speakers4:		speakerType = AIL_3D_4_SPEAKER;		break;
	case Speakers51:	speakerType = AIL_3D_51_SPEAKER;	break;
	case Speakers71:	speakerType = AIL_3D_71_SPEAKER;	break;
	};

    AIL_set_3D_speaker_type( mMiles->DigitalProvider, speakerType);
}

bool AudioDevice::Set3DSoundProvider( String providerName )
{
	if( !mMiles )
		return false;
	for( int i = 0; i < GetNumberOfProviders(); ++i )
	{
		if( mMiles->mProviderNames[i] == providerName )
			return Set3DSoundProvider( i );
	}
	return false;
}

void AudioDevice::SetRoom( int roomType )
{
	if( !mMiles ) return;

	int rType;	// for conversion from our internal into Miles

	switch( roomType )
	{
	default:
	case Generic:			rType = ENVIRONMENT_GENERIC;			break;
	case PaddedCell:		rType = ENVIRONMENT_PADDEDCELL;			break;
	case Room:				rType = ENVIRONMENT_ROOM;				break;
	case Bathroom:			rType = ENVIRONMENT_BATHROOM;			break;
	case LivingRoom:		rType = ENVIRONMENT_LIVINGROOM;			break;
	case StoneRoom:			rType = ENVIRONMENT_STONEROOM;			break;
	case Auditorium:		rType = ENVIRONMENT_AUDITORIUM;			break;
	case ConcertHall:		rType = ENVIRONMENT_CONCERTHALL;		break;
	case Cave:				rType = ENVIRONMENT_CAVE;				break;
	case Arena:				rType = ENVIRONMENT_ARENA;				break;
	case Hangar:			rType = ENVIRONMENT_HANGAR;				break;
	case CarpetedHallway:	rType = ENVIRONMENT_CARPETEDHALLWAY;	break;
	case Hallway:			rType = ENVIRONMENT_HALLWAY;			break;
	case StoneCorridor:		rType = ENVIRONMENT_STONECORRIDOR;		break;
	case Alley:				rType = ENVIRONMENT_ALLEY;				break;
	case Forest:			rType = ENVIRONMENT_FOREST;				break;
	case City:				rType = ENVIRONMENT_CITY;				break;
	case Mountains:			rType = ENVIRONMENT_MOUNTAINS;			break;
	case Quarry:			rType = ENVIRONMENT_QUARRY;				break;
	case Plain:				rType = ENVIRONMENT_PLAIN;				break;
	case ParkingLot:		rType = ENVIRONMENT_PARKINGLOT;			break;
	case SewerPipe:			rType = ENVIRONMENT_SEWERPIPE;			break;
	case UnderWater:		rType = ENVIRONMENT_UNDERWATER;			break;
	case Drugged:			rType = ENVIRONMENT_DRUGGED;			break;
	case Dizzy:				rType = ENVIRONMENT_DIZZY;				break;
	case Psychotic:			rType = ENVIRONMENT_PSYCHOTIC;			break;
	};

	mMiles->CurrentRoomType = rType;

	if( mMiles->DigitalDriver )
		AIL_set_digital_master_room_type( mMiles->DigitalDriver, rType );

	if( mMiles->DigitalProvider && mMiles->UsingEAX )
	{
		AIL_set_3D_room_type( mMiles->DigitalProvider, rType );

		// Turn off EAX, if they choose no reverb
		float lev = ( rType == ENVIRONMENT_GENERIC )?0.0f:1.0f;
		for( int i = 0; i < mMiles->m3DRealSamples.Size(); ++i )
		{
			if( mMiles->m3DRealSamples[i] )
			{
				if( ( AIL_3D_sample_status( mMiles->m3DRealSamples[i] ) == SMP_PLAYING ) ||
					( AIL_3D_sample_status( mMiles->m3DRealSamples[i] ) == SMP_STOPPED ) )
				{
					AIL_set_3D_sample_effects_level( mMiles->m3DRealSamples[i], lev );
				}
			}
		}
	}
}

Vector AudioDevice::GetPlayerPosition( void )
{
	Vector pos;
	if( mMiles && mMiles->Listener )
	{
		AIL_listener_3D_position( &pos.X, &pos.Y, &pos.Z );
		pos.Z = - pos.Z;
		return pos;
	}
	return Vector(0.0f,0.0f,0.0f);
}

void AudioDevice::SetPlayerOrientation( Vector& front, Vector& up )
{
	if( mMiles && mMiles->Listener )
		if( mMiles->mSpeakersFlip )
			AIL_set_3D_orientation( mMiles->Listener, -front.X, -front.Y, front.Z, up.X, up.Y, -up.Z );
		else
			AIL_set_3D_orientation( mMiles->Listener, front.X, front.Y, -front.Z, up.X, up.Y, -up.Z );
}

void AudioDevice::GetPlayerOrientation( Vector& front, Vector& up )
{
	if( mMiles && mMiles->Listener )
	{
		AIL_3D_orientation( mMiles->Listener, &front.X, &front.Y, &front.Z, &up.X, &up.Y, &up.Z );
		front.Z = - front.Z;
		up.Z = -up.Z;
	};
}

void AudioDevice::SetPlayerVelocity( Vector& velocity )
{
	if( mMiles && mMiles->Listener )
	{
		float magn = velocity.Magnitude();
		velocity.Normalize();
		AIL_set_3D_velocity( mMiles->Listener, velocity.X, velocity.Y, -velocity.Z, 0.001f*magn );
	}
}

Vector AudioDevice::GetPlayerVelocity( void )
{
	Vector vel;
	if( mMiles && mMiles->Listener )
	{
		AIL_3D_velocity( mMiles->Listener, &vel.X, &vel.Y, &vel.Z );
		vel.Z = -vel.Z;
		vel *= 1000.0f;
		return vel;
	}
	return Vector(0.0f,0.0f,0.0f);
}

bool AudioDevice::SetFirstSuccesfulProvider( int startingProvider )
{
	if( !GetNumberOfProviders() )
		return false;

	int i = startingProvider;
	do {
		i++;
		if( i >= GetNumberOfProviders() )
			i = 0;
		if( Set3DSoundProvider(i) )
			return true;
	} while( i != startingProvider );
	return false;
}

int AudioDevice::Sound2D_Create( const char* name, bool playAtOnce ) { Assert( mMiles ); return mMiles->Sound2D_Create( name, playAtOnce ); }
int AudioDevice::Sound2D_Create( FileObject* f ) { Assert( mMiles ); return mMiles->Sound2D_Create( f ); }
void AudioDevice::Sound2D_Play( int index ) { Assert( mMiles ); mMiles->Sound2D_Play( index ); }
void AudioDevice::Sound2D_PlayAndForget( int index ) { Assert( mMiles ); mMiles->Sound2D_PlayAndForget( index ); }
void AudioDevice::Sound2D_Stop( int index ) { Assert( mMiles ); mMiles->Sound2D_Stop( index ); }
void AudioDevice::Sound2D_Resume( int index ) { Assert( mMiles ); mMiles->Sound2D_Resume( index ); }
void AudioDevice::Sound2D_Forget( int index ) { Assert( mMiles ); mMiles->Sound2D_Forget( index ); }
void AudioDevice::Sound2D_Delete( int index ) { Assert( mMiles ); mMiles->Sound2D_Delete( index ); }
void AudioDevice::Sound2D_SetDontSave( int index ) { Assert( mMiles ); mMiles->Sound2D_SetDontSave( index ); }
void AudioDevice::Sound2D_SetLoopCount( int index, int count ) { Assert( mMiles ); mMiles->Sound2D_SetLoopCount( index, count ); }
float AudioDevice::Sound2D_GetVolume( int index ) { Assert( mMiles ); return mMiles->Sound2D_GetVolume( index ); };
void AudioDevice::Sound2D_SetVolume( int index, float volume, float switchTime ) { Assert( mMiles ); mMiles->Sound2D_SetVolume( index, volume, switchTime ); };
bool AudioDevice::Sound2D_IsPlaying( int index ) { Assert( mMiles ); return mMiles->Sound2D_IsPlaying( index ); }
void AudioDevice::Sound2D_NoSpeedRandomize( int index ) { Assert( mMiles ); mMiles->Sound2D_NoSpeedRandomize( index ); }
bool AudioDevice::Sound2D_Save( int index, FileObject* f ) { Assert( mMiles ); return mMiles->Sound2D_Save( index, f ); }
void AudioDevice::Sound2D_SetAlwaysSameSpeed( int index, bool set ) { Assert( mMiles ); mMiles->Sound2D_SetAlwaysSameSpeed( index, set ); }
void AudioDevice::Sound2D_SetLowPass( int index, float lowPass ) { Assert( mMiles ); mMiles->Sound2D_SetLowPass( index, lowPass ); };
float AudioDevice::Sound2D_GetLowPass( int index ) { Assert( mMiles ); return mMiles->Sound2D_GetLowPass( index ); };

int AudioDevice::Sound3D_Create( const char* name, bool playAtOnce ) { Assert( mMiles ); return mMiles->Sound3D_Create( name, playAtOnce ); }
int AudioDevice::Sound3D_Create( FileObject* f ) { Assert( mMiles ); return mMiles->Sound3D_Create( f ); }
void AudioDevice::Sound3D_Forget( int index ) { Assert( mMiles ); mMiles->Sound3D_Forget( index ); }
void AudioDevice::Sound3D_Resume( int index ) { Assert( mMiles ); mMiles->Sound3D_Resume( index ); }
void AudioDevice::Sound3D_Delete( int index ) { Assert( mMiles ); mMiles->Sound3D_Delete( index ); }
bool AudioDevice::Sound3D_IsPlaying( int index ) { Assert( mMiles ); return mMiles->Sound3D_IsPlaying( index ); }
void AudioDevice::Sound3D_Play( int index ) { Assert( mMiles ); mMiles->Sound3D_Play( index ); }
void AudioDevice::Sound3D_PlayAndForget( int index ) { Assert( mMiles ); mMiles->Sound3D_PlayAndForget( index ); }
void AudioDevice::Sound3D_Stop( int index ) { Assert( mMiles ); mMiles->Sound3D_Stop( index ); }
void AudioDevice::Sound3D_SetDontSave( int index ) { Assert( mMiles ); mMiles->Sound3D_SetDontSave( index ); }
void AudioDevice::Sound3D_SetPosition( int index, Vector& position ) { Assert( mMiles ); mMiles->Sound3D_SetPosition( index, position ); }
void AudioDevice::Sound3D_SetLoopCount( int index, int count ) { Assert( mMiles ); mMiles->Sound3D_SetLoopCount( index, count ); }
float AudioDevice::Sound3D_GetVolume( int index ) { Assert( mMiles ); return mMiles->Sound3D_GetVolume( index ); }
void AudioDevice::Sound3D_SetVolume( int index, float volume, float switchTime ) { Assert( mMiles ); mMiles->Sound3D_SetVolume( index, volume, switchTime ); }
void AudioDevice::Sound3D_SetHearingDistance( int index, float minDist, float maxDist ) { Assert( mMiles ); mMiles->Sound3D_SetHearingDistance( index, minDist, maxDist ); }
void AudioDevice::Sound3D_SetObstructed( int index, bool obstructed ) { Assert( mMiles ); mMiles->Sound3D_SetObstructed( index, obstructed ); }
void AudioDevice::Sound3D_SetNoSpeedRandomize( int index ) { Assert( mMiles ); mMiles->Sound3D_SetNoSpeedRandomize( index ); }
void AudioDevice::Sound3D_SetVelocity( int index, Vector& velocity ) { Assert( mMiles ); mMiles->Sound3D_SetVelocity( index, velocity ); }
bool AudioDevice::Sound3D_Save( int index, FileObject* f ) { Assert( mMiles ); return mMiles->Sound3D_Save( index, f ); }
void AudioDevice::Sound3D_SetLowPass( int index, float lowPass ) { Assert( mMiles ); mMiles->Sound3D_SetLowPass( index, lowPass ); }
float AudioDevice::Sound3D_GetLowPass( int index ) { Assert( mMiles ); return mMiles->Sound3D_GetLowPass( index ); }
void AudioDevice::Sound3D_SetOrientation( int index, Vector& front, Vector& up ) { Assert( mMiles ); mMiles->Sound3D_SetOrientation( index, front, up ); }
void AudioDevice::SetFalloffSpeed( float speed ) { Assert( mMiles ); mMiles->SetFalloffSpeed( speed ); }
float AudioDevice::GetFalloffSpeed( void ) { Assert( mMiles ); return mMiles->GetFalloffSpeed(); }

inline void AudioDevice::SetSoundSpeedRandomizer( float value )
{
	if (!mMiles) return;
	value = fabs( value );
	if( value >= 0.99f ) value = 0.99f;
	mMiles->mSoundRateDiffRate = value;
}


inline bool AudioDevice::LoadAudio( FileObject* f ) { Assert( mMiles ); return mMiles->LoadAudio( f ); }
inline bool AudioDevice::SaveAudio( FileObject* f ) { Assert( mMiles ); return mMiles->SaveAudio( f ); }
inline void AudioDevice::SaveGame_PauseSounds( void ) { Assert( mMiles ); mMiles->SaveGame_PauseSounds(); }
inline void AudioDevice::SaveGame_ResumeSounds( void ) { Assert( mMiles ); mMiles->SaveGame_ResumeSounds(); }
