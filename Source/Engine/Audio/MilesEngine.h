
// #pragma once
#ifndef _MILESENGINE_
#define _MILESENGINE_

#include "Core/HashMap.h"
#include "Audio/Audio.h"

#include "mss.h"

extern const char * soundFileFormatStr;

class AudioRecorder;

class SoundList
{
	friend class MilesEngine;
	friend class Miles2DSound;
	friend class Miles3DSound;
public:
	SoundList() {};
	~SoundList() {};
protected:
	DynamicArray<int> Streams;
	DynamicArray<bool> StreamsResume;
	DynamicArray<Miles2DSound*> Sounds2D;
	DynamicArray<bool> Sounds2DDeleteIfStopped;
	DynamicArray<Miles3DSound*> Sounds3D;
	DynamicArray<bool> Sounds3DDeleteIfStopped;
};

class StreamingSoundData
{
public:
	bool Valid;
	HSTREAM Stream;
	String Name;
	float LowPass;
	int Speed;
	float Volume;
	int LoopCount;

	StreamingSoundData( void ) {};
	StreamingSoundData( float lp ) : Valid(false), Stream(0), Name(""), LowPass(lp), Speed(0), Volume(0), LoopCount(1) {};
	~StreamingSoundData( void ) {};
};

#ifdef TRANSGAMING_WINDOWS
class ENGINE_API MilesEngine
#else
class MilesEngine
#endif
{
	friend class MilesLoadedFile;
	friend class Miles2DSound;
	friend class Miles3DSound;

	struct PrivilegedSound
	{
		bool Is3D;
		int Index;
	};

	DynamicArray<PrivilegedSound> mPrivilegedSounds;

	struct DigitalDriverProperties
	{
		F32 MasterVolumeLevel;
		S32 RoomType;
	};

	AudioRecorder* mAudioRecorder;

	HDIGDRIVER DigitalDriver;			// Digital playback device
	HPROVIDER DigitalProvider;			// 3D Sound Provider
	int CurrentRoomType;				// Current room type - reverberation etc.
	float Current3DIntensity;			// Current room effects 3D intensity
	float Current2DIntensity;			// Current room effects 2D intensity

	bool UsingEAX;						// Is our current sound provider using EAX?
	bool DontStartNewSounds;			// Useful in destructor
	bool StartManyAtOnce;				// Useful when resuming or loading group of sounds; passes restrictions in MilesLoadedFile

	float DigitalEffectsVolume3D;		// Volume for 3D samples
	float StreamingVolume;				// Volume for samples

	float Speed;						// Speed for playing sounds

	float mDesiredSpeed;
	DWORD mLastSetSpeedTime;
	DWORD mDesiredSpeedTime;

	float mVolumeMultiplier;
	float mLowPassForNewSounds;
	float mSoundRateDiffRate;
	float LowPassFrequencyPercent;		// Low pass cutoff frequency

	int mCurrentSoundProvider;			// Remember which sound provider we use

	bool mSpeakersFlip;					// Are sound speakers flipped?
	bool mObstructedByDefault;			// Are all 3D sounds obstructed by default of not?

	// This is a buffer for streaming sounds
	DynamicArray<StreamingSoundData> mStreams;
	DynamicArray<int> mStreamsFill;

	HashMap<String,unsigned short,64> mLoadedFilesHash;
	DynamicArray<MilesLoadedFile*> mLoadedFiles;
	int mFilledFiles;
	int mMaxFiles;

	// This is a buffer for 2D samples
	DynamicArray<HSAMPLE> m2DRealSamples;
	DynamicArray<Miles2DSound*> m2DRealSounds;
	int mMax2DSamples;			// I find out the limit in course of the game

	// Our 2D sounds list - every 2D sound made it here.
	DynamicArray<Miles2DSound*> m2DSounds;
	HashMap<int,Miles2DSound*> m2DSoundHash;
	int m2DSoundNumbers;

	// This is a buffer for 3D samples
	DynamicArray<HSAMPLE> 		m3DRealSamples;
	DynamicArray<Miles3DSound*> m3DRealSounds;
	int mMax3DSamples;			// I find out the limit in course of the game

	// Our 3D sounds list - every 3D sound made it here.
	DynamicArray<Miles3DSound*> m3DSounds;
	HashMap<int,Miles3DSound*> m3DSoundHash;
	int m3DSoundNumbers;

	// 3D Sound Provider lists
	DynamicArray<HPROVIDER> mProviders;		// All providers in the system - handles
	DynamicArray<String> mProviderNames;	// All providers in the system - names

	int mDefaultInstances;					// for loaded sound files
	DWORD mDefaultDelay;					// for loaded sound files

	float mCurrentFalloffSpeed;

	bool mInitialized;

	// Sound obstruction settings
	float HeightenObstructionTime;
	float LowerObstructionTime;
	float MinObstruction;
	float MaxObstructionLow;
	float MaxObstructionHigh;
	float MinObstructionDistance;
	float MaxObstructionDistance;

	// ****** methods ********

	// Utilities
	void SetDefaultSoundProperties( int instances, DWORD delay ) { mDefaultInstances = instances; mDefaultDelay = delay; };
	void* LoadAndUnpackFile( const char* name );
	void UpdateSpeed( void );
	void SetPrivilegedSoundsVolumeMultiplier( float percent = 1.0f ) { Assert( percent > 0.0f ); mVolumeMultiplier = 1.0f/percent; };
	void UpdateAllSoundsToPrivileged( void );
	void SetDopplerFactor( float factor = 1.0f );

	// 2D samples methods
	MilesLoadedFile* GetFileByName( const char* name );			// Returns pointer to file with that name, or NULL if not found
	MilesLoadedFile* GetEmptyFileByName( const char* name );	// Returns pointer to file with that name, or NULL if not found
	MilesLoadedFile* PrepareSample( const char* name );			// Makes sure that the sample is loaded into buffer.
	MilesLoadedFile* SureGetFile( const char* name );

	// 3D Sound Provider methods
	void Enumerate3DSoundProviders( void );
	void Release3DSoundProvider( void );

	// 3D samples methods
	bool Start3DSample( Miles3DSound* sound, int sampleIndex );
	void Release3DSample( int sampleIndex, Miles3DSound* soundToStart = 0, bool dontStartNew = false );
	int Find3DSoundToStop( float& distance, MilesLoadedFile* file = 0 );
	Miles3DSound* Find3DSoundToStart( MilesLoadedFile* file = 0 );
	void TryToPlayRealSound( Miles3DSound* sound );

	// 2D samples methods
	bool Start2DSample( Miles2DSound* sound, int sampleIndex );
	void Release2DSample( int sampleIndex, Miles2DSound* soundToStart = 0, bool dontStartNew = false );
	int Find2DSoundToStop( void );
	int GetLongestPlaying2DSoundForFile( MilesLoadedFile* file );
	Miles2DSound* Find2DSoundToStart( void );
	void TryToPlayRealSound2D( Miles2DSound* sound );

	// private 3D sounds methods - move to public if needed
	bool Sound3D_IsObstructed( int index );
	bool Sound3D_HasAlwaysSameSpeed( int index );
	float Sound3D_GetVolume( int index );

	// private 2D sounds methods - move to public if needed
	float Sound2D_GetVolume( int index );

	// Streaming methods
	int  StreamingSound_GetLoopCount( int index );
	bool StreamingSound_IsPlaying( int index );

	// Various methods, private only so I know what is used outside; move to public if you need them
	float Get3DDigitalEffectsVolume( void );
	void GetStreamingVolume( float& volume ) { volume = StreamingVolume; };
	float GetMasterVolumeLevel( void ) { return DigitalDriver?AIL_digital_master_volume_level( DigitalDriver ):1.0f; }
//	void GetStreamNames( DynamicArray<String>& names ) { names = mStreamNames; }
//	void Play2DSound( const char* name, float volume );
	Vector GetPlayerPosition( void );
	void GetPlayerOrientation( Vector& front, Vector& up );
	Vector GetPlayerVelocity( void );

	DynamicArray<SoundList*> PauseStack;
	DynamicArray<int> PauseStackFill;
	int SaveGameSoundList;
	void ResetFlagsFromPauseStack( void );

public:
	// Construction/destruction
	MilesEngine(void);									// Constructor
	~MilesEngine(void);									// Destructor
	bool StartedOk( void ) { return mInitialized; };	// Tester
	void Clear(void);

	// Maintenance
	void StopFinishedSounds( void );
	void Tick( float timeDelta );
	void PreloadFile( const char* name );

	// Utilities
	void SetSpeed( float speed = 1.0f, float switchTime = 0.0f );
	void SetLowPass( float percent = 1.0f );
	void SetMasterVolumeLevel( float level );
	void SetFalloffSpeed( float speed = 1.0f );
	void SetStreamingVolume( float volume );
	void Set3DDigitalEffectsVolume( float volume );
	void SetSpeakerType( int type );
	void SetSpeakersFlip( bool flipped );
	void SetSoundSpeedRandomizer( float value );
	void SetAllSoundsToLowPass( float percent = 1.0f );
	float GetFalloffSpeed( void );

	HDIGDRIVER GetDigitalDriver() const { return DigitalDriver; }

	void GetSoundCounts( int& sounds2D, int& sounds3D, int& streams, int& sounds2DHeard, int& sounds3DHeard, int& streamsHeard );

	void SetDefaultObstruction( bool obstructed );
	bool GetDefaultObstruction( void ) { return mObstructedByDefault; };
	void SetObstructionProperties( float heightenDelay, float lowerDelay, float minObstruction, float maxObstructionLow, float maxObstructionHigh, float minObstructionDistance, float maxObstructionDistance );

	// Reverbs
	void SetRoom( int roomType, bool set2D = true );
	void SetRoomIntensity2D( float intensity, float switchTime );
	void SetRoomIntensity3D( float intensity, float switchTime );
//	void SetCustom2DReverb( float reverbTime, float reverbPreDelay, float reverbDamping, float wetLevel, float dryLevel );

	int GetCurrentRoomType() const { return CurrentRoomType; }
	float GetCurrent3DIntensity() const { return Current3DIntensity; }
	float GetCurrent2DIntensity() const { return Current2DIntensity; }

	// Pause/play
	int PauseCurrentlyPlayingSounds( void );
	void ResumeSounds( int sounds );
	void StopAllSounds( void );

	void LoadFilesOffMemory( void );
	void Reset( void );
	void Dump( void );

	// sound file methods
#ifdef TRANSGAMING
	void SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay );
#else
	void SetSoundProperties( String soundName, int maximalInstances, int minimalDelay );
#endif

	// sound provider methods
	int NumOfProviders( void ) { return mProviders.Size(); }
	int GetCurrentSoundProvider( void ) { return mCurrentSoundProvider; };
	bool SetFirstSuccesfulProvider( int startingProvider = 0 );
	bool SetMostSuitableProvider( void );
	String GetProviderName( int providerIndex ) { return mProviderNames[providerIndex]; };
	bool Set3DSoundProvider( String providerName );
	bool Set3DSoundProvider( int providerIndex );

	// 2D sounds methods
	int  Sound2D_Create( const char* name, unsigned char priority );
	int  Sound2D_CreateEx( const char* name, unsigned char priority, float volume = 1.0f,
		bool samespeed = false, bool noSpeedRandomize = false, bool playAndForget = false, float intensity = 1.0f, bool dontSave = false );
	int  Sound2D_Create( GFile* f );
	void Sound2D_Play( int index );
	void Sound2D_PlayAndForget( int index );
	void Sound2D_Resume( int index );
	void Sound2D_Stop( int index );
	void Sound2D_Forget( int index );
	bool Sound2D_Save( int index, GFile* f );
	void Sound2D_Delete( int index );

	bool Sound2D_IsPlaying( int index );

//	void Sound2D_SetPrivileged( int index, bool privileged );
//	void Sound2D_SetPriority( int index, unsigned char priority );
	void Sound2D_SetDontSave( int index );
	void Sound2D_SetLoopCount( int index, int count );
	void Sound2D_SetVolume( int index, float volume, float switchTime = 0.0f );
//	void Sound2D_SetIntensity( int index, float intensity, float switchTime = 0.0f );
	void Sound2D_NoSpeedRandomize( int index );
	void Sound2D_SetSoundSpeed( int index, float speed );
	void Sound2D_SetAlwaysSameSpeed( int index, bool set );
	void Sound2D_SetLowPass( int index, float lowPass );

	float Sound2D_GetLowPass( int index );

	// 3D sounds methods
	int Sound3D_Create( const char* name, unsigned char priority );	// create new sound
	int Sound3D_Create( GFile* f );
	int Sound3D_CreateEx( const char* name, unsigned char priority, bool playAtOnce, float loopCount, Vector position,
		float mindist, float maxdist, float volume, bool noSpeedRandomize = false, bool dontSave = false,
		bool obstructed = false, float intensity = 1.0f );
	void Sound3D_Play( int index );
	void Sound3D_PlayAndForget( int index );
	void Sound3D_Resume( int index );
	void Sound3D_Stop( int index );
	void Sound3D_Forget( int index );
	bool Sound3D_Save( int index, GFile* f );
	void Sound3D_Delete( int index );

	bool Sound3D_IsPlaying( int index );
	float Sound3D_GetLowPass( int index );

//	void Sound3D_SetPrivileged( int index, bool privileged );
//	void Sound3D_SetPriority( int index, unsigned char priority );
	void Sound3D_SetDontSave( int index );
	void Sound3D_SetPosition( int index, Vector& position );
	void Sound3D_SetLoopCount( int index, int count );
	void Sound3D_SetVolume( int index, float volume, float switchTime = 0.0f );
	void Sound3D_SetIntensity( int index, float intensity, float switchTime = 0.0f );
	void Sound3D_SetHearingDistance( int index, float minDist, float maxDist );
	void Sound3D_SetObstructed( int index, bool obstructed );
	void Sound3D_SetNoSpeedRandomize( int index );
	void Sound3D_SetSoundSpeed( int index, float speed );
	void Sound3D_SetVelocity( int index, Vector& velocity );
	void Sound3D_SetLowPass( int index, float lowPass );
	void Sound3D_SetOrientation( int index, Vector& front, Vector& up );

	// streaming sounds methods
	int  StreamingSound_Create( const char* name );
	void StreamingSound_Play( int index );
	void StreamingSound_Resume( int index );
	void StreamingSound_Stop( int index );
	void StreamingSound_Delete( int index );

	void StreamingSound_SetLoopCount( int index, int count );
	void StreamingSound_SetVolume( int index, float volume );
	void StreamingSound_SetLowPass( int index, float lowPass );

	void StreamingSound_GetVolume( int index, float& volume );
	float StreamingSound_GetLowPass( int index );

	// player movement methods
	void SetPlayerPosition( Vector& position );
	void SetPlayerVelocity( Vector& velocity );
	void SetPlayerOrientation( Vector& front, Vector& up );

	// Load/Save methods
	bool LoadAudio( GFile* f );
	void LoadReset( void );
	bool SaveAudio( GFile* f );
	void SaveGame_PauseSounds( void );
	void SaveGame_ResumeSounds( void );

	// Audio recording connection
	void SetAudioRecorder( AudioRecorder* rec ) { mAudioRecorder = rec; };
};

// jaba->
// Add functions and macros to get the PCF-derived Miles code working
// with Miles 7.0+ on the PS2.

#define AIL_init_sample(s) AIL_init_sample(s, DIG_F_MONO_16, 0)

#define AIL_start_3D_sample AIL_start_sample
#define AIL_resume_3D_sample AIL_resume_sample
#define AIL_stop_3D_sample AIL_stop_sample

#define AIL_3D_sample_status AIL_sample_status
#define AIL_3D_orientation AIL_sample_3D_orientation
#define AIL_3D_position AIL_sample_3D_position
#define AIL_3D_sample_distances(sample, max_dist, min_dist) AIL_sample_3D_distances((sample), (max_dist), (min_dist), NULL)
#define AIL_3D_sample_loop_count AIL_sample_loop_count
#define AIL_3D_sample_obstruction AIL_sample_obstruction
#define AIL_3D_sample_occlusion AIL_sample_occlusion
#define AIL_3D_sample_offset AIL_sample_position
#define AIL_3D_velocity AIL_sample_3D_velocity

#define AIL_release_3D_sample_handle AIL_release_sample_handle

#define AIL_set_3D_orientation AIL_set_sample_3D_orientation
#define AIL_set_3D_position AIL_set_sample_3D_position
#define AIL_set_3D_sample_file(s, file_image) AIL_set_sample_file(s, file_image, 0)
#define AIL_set_3D_sample_distances(sample, max_dist, min_dist) AIL_set_sample_3D_distances((sample), (max_dist), (min_dist), 1)
#define AIL_set_3D_sample_loop_count AIL_set_sample_loop_count
#define AIL_set_3D_sample_obstruction AIL_set_sample_obstruction
#define AIL_set_3D_sample_occlusion AIL_set_sample_occlusion
#define AIL_set_3D_sample_offset AIL_set_sample_position
#define AIL_set_3D_sample_playback_rate AIL_set_sample_playback_rate
#define AIL_set_3D_velocity AIL_set_sample_3D_velocity

#define AIL_set_3D_sample_volume(s, v) AIL_set_sample_volume_levels(s, v, v)
#define AIL_set_3D_sample_effects_level(s, intensity) AIL_set_sample_reverb_levels(s, 1.f, intensity)

inline void AIL_set_stream_low_pass_cut_off( HSTREAM stream, F32 cut_off )
{
	const HSAMPLE sample = AIL_stream_sample_handle( stream );
	AIL_set_sample_low_pass_cut_off( sample, cut_off );
}

inline void AIL_set_stream_playback_rate( HSTREAM stream, S32 playback_rate )
{
	const HSAMPLE sample = AIL_stream_sample_handle( stream );
	AIL_set_sample_playback_rate( sample, playback_rate );
}

inline void AIL_set_stream_volume_levels( HSTREAM stream, F32 left_level, F32 right_level )
{
	const HSAMPLE sample = AIL_stream_sample_handle( stream );
	AIL_set_sample_volume_levels( sample, left_level, right_level );
}


inline S32 AIL_stream_playback_rate( HSTREAM stream )
{
	const HSAMPLE sample = AIL_stream_sample_handle( stream );
	return AIL_sample_playback_rate( sample );
}


#endif //_MILESENGINE_

