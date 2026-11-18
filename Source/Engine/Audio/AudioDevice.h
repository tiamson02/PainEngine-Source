
// #pragma once
#ifndef _AUDIODEVICE_
#define _AUDIODEVICE_


#include "Audio.h"

class ENGINE_API AudioDevice
{
	friend class MilesEngine;
public:
    AudioDevice();
    ~AudioDevice();

    BOOL Init( void );								// make sure main window exists
	inline void Tick( float timeDelta );

	// ****************** Master settings - one for device *******************
	void SetFalloffSpeed( float speed = 1.0f );
	float GetFalloffSpeed( void );
#ifdef TRANSGAMING
	void SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay );
#else
	void SetSoundProperties( String soundName, int maximalInstances, int minimalDelay );
#endif

	float GetMasterVolumeLevel( void );
	void SetMasterVolumeLevel( float level );		// range 0-1

	// 2D audio settings
	void GetDigitalEffectsPan( float& pan );
	void SetDigitalEffectsPan( float pan );
	void GetStreamingVolumeAndPan( float& volume, float& pan );
	void SetStreamingVolumeAndPan( float volume, float pan );

	// 3D audio settings
	inline void Set3DDigitalEffectsVolume( float volume );
	float Get3DDigitalEffectsVolume( void );

	// Provider enumeration and choice
	bool Set3DSoundProvider( int providerIndex );			// returns true if provider opened ok, false otherwise.
	bool Set3DSoundProvider( String providerName );			// returns true if provider opened ok, false otherwise.
	int GetNumberOfProviders( void );
	String GetProviderName( int providerIndex );
	bool SetFirstSuccesfulProvider( int startingProvider = 0 );
	int GetCurrentSoundProvider( void );

	// Other 3D audio settings
	void SetSpeakerType( int type );						// Speaker type for selected provider
	void SetRoom( int roomType );							// Room we are currently in.
	void SetSpeed( float speed = 1.0f, float switchTime = 0.0f );
	void SetLowPass( float percent = 1.0f );
	void SetAllSoundsToLowPass( float percent = 1.0f );
	void SetAllSoundsVolumeMultiplier( float percent = 1.0f );
	void SetSpeakersFlip( bool flipped );
	void SetSoundSpeedRandomizer( float value );
	void SetDopplerFactor( float factor = 1.0f );

	// Player (listener) methods
	void SetPlayerPosition( Vector& position );
	Vector GetPlayerPosition( void );
	void SetPlayerOrientation( Vector& front, Vector& up );
	void GetPlayerOrientation( Vector& front, Vector& up );
	void SetPlayerVelocity( Vector& velocity );
	Vector GetPlayerVelocity( void );

	// ****************** Streaming sound methods ****************************
	int StreamingSound_Create( const char* name );
	void StreamingSound_Play( int index );
	void StreamingSound_Stop( int index );
	void StreamingSound_Resume( int index );
	void StreamingSound_Delete( int index );
	int  StreamingSound_GetLoopCount( int index );
	void StreamingSound_SetLoopCount( int index, int count );
	void StreamingSound_SetVolumeAndPan( int index, float volume, float pan );
	void StreamingSound_GetVolumeAndPan( int index, float& volume, float& pan );
	void StreamingSound_SetLowPass( int index, float lowPass );
	float StreamingSound_GetLowPass( int index );
	bool StreamingSound_IsPlaying( int index );

	void GetStreamNames( DynamicArray<String>& names );
	void StartStreams( DynamicArray<String>& names );

	// ****************** 2D audio methods ***********************************
	void Play2DSound( const char* name, float volume = 1.0f, bool playAtOnce = false );
	int  Sound2D_Create( const char* name, bool playAtOnce = false );
	int  Sound2D_Create( FileObject* f );
	void Sound2D_Play( int index );
	void Sound2D_PlayAndForget( int index );
	void Sound2D_Stop( int index );
	void Sound2D_Resume( int index );
	void Sound2D_Delete( int index );
	void Sound2D_Forget( int index );
	bool Sound2D_IsPlaying( int index );
	void Sound2D_SetDontSave( int index );
	void Sound2D_SetLoopCount( int index, int count );
	float Sound2D_GetVolume( int index );
	void Sound2D_SetVolume( int index, float volume, float switchTime = 0.0f );
	void Sound2D_NoSpeedRandomize( int index );
	bool Sound2D_Save( int index, FileObject* f );
	void Sound2D_SetAlwaysSameSpeed( int index, bool set );
	void Sound2D_SetLowPass( int index, float lowPass );
	float Sound2D_GetLowPass( int index );

	// ****************** 3D audio methods ***********************************
	int Sound3D_Create( const char* name, bool playAtOnce = false );	// create new sound
	int Sound3D_Create( FileObject* f );
	void Sound3D_Forget( int index );
	void Sound3D_Resume( int index );
	void Sound3D_Delete( int index );
	void Sound3D_Play( int index );
	void Sound3D_PlayAndForget( int index );
	void Sound3D_Stop( int index );
	void Sound3D_SetDontSave( int index );
	void Sound3D_SetPosition( int index, Vector& position );
	void Sound3D_SetLoopCount( int index, int count );
	float Sound3D_GetVolume( int index );
	void Sound3D_SetVolume( int index, float volume, float switchTime = 0.0f );
	void Sound3D_SetHearingDistance( int index, float minDist, float maxDist );
	void Sound3D_SetObstructed( int index, bool obstructed );
	void Sound3D_SetNoSpeedRandomize( int index );
	bool Sound3D_IsPlaying( int index );
	void Sound3D_SetVelocity( int index, Vector& velocity );
	bool Sound3D_Save( int index, FileObject* f );
	void Sound3D_SetLowPass( int index, float lowPass );
	float Sound3D_GetLowPass( int index );
	void Sound3D_SetOrientation( int index, Vector& front, Vector& up );

	// ****************** pause/play methods *********************************
	SoundList* PauseCurrentlyPlayingSounds( void );
	void ResumeSounds( SoundList* sounds );

	// ****************** load/save methods **********************************
	inline bool LoadAudio( FileObject* f );
	inline bool SaveAudio( FileObject* f );
	inline void SaveGame_PauseSounds( void );
	inline void SaveGame_ResumeSounds( void );

protected:
	MilesEngine* mMiles;
};

#endif //_AUDIODEVICE_

