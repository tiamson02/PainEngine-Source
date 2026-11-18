
// Warning: playAtOnce flag in CreateXXSound() does not cause the sound to play, you need to Play() it specifically later.

#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Physics/PhysicsWorld.h"
#include "Engine/AudioDevice.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"
//#include "Engine/Entity.h"

#include "MSS.H"

#define MSS_REDIST_DIRNAME ".\\Miles\\"

//#define AUDIO_DEVICE_METHODS_LOGGING

class MilesLoadedFile
{
public:
	MilesLoadedFile( const char* name, int defaultInstances, DWORD defaultDelay )
	{
		mFilePointer = 0;
		mFileName = name;
		mLastUsedTime = GetCurrentTime();
		mLastStartedTime = 0;
		mMinimalStartDelay = defaultDelay;
		mMaximalInstances = defaultInstances;
		mReferenceCount = 0;
		mPlayingReferenceCount = 0;
	}

	~MilesLoadedFile()
	{
		EmptyFile();
	}

	// Changing between empty file and full file
	void EmptyFile( void )
	{
		Assert( mReferenceCount == 0 );
		AIL_mem_free_lock( mFilePointer );
		mFilePointer = 0;
	}

	void FillFile( void* filePointer )
	{
		mFilePointer = filePointer;
		mLastUsedTime = GetCurrentTime();
		mLastStartedTime = 0;
		mReferenceCount = 1;
		mPlayingReferenceCount = 0;
	}

	bool IsReferenced( void ) { return ( mReferenceCount > 0 ); };
	void RemoveReference( void ) { Assert( mReferenceCount > 0 ); mReferenceCount--; };
	void AddReference( void ) { mReferenceCount++; };

	// Mutual sound file playing exclusions
	void SetProperties( int maximalInstances, float minimalDelay )
	{
		mMaximalInstances = maximalInstances;
		mMinimalStartDelay = minimalDelay;
	};

	bool CanPlayFile( void )
	{
		DWORD t = GetCurrentTime();
//		GLog.Print( "Check canPlayFile: %s, inst: %d, max: %d, minDel: %d, del: %d, ",
//			mFileName.AsChar(), mPlayingReferenceCount, mMaximalInstances, mMinimalStartDelay, t - mLastStartedTime );
		if( mMaximalInstances <= mPlayingReferenceCount )
		{
//			GLog.Print("false\n");
			return false;
		}
		if( t - mLastStartedTime < mMinimalStartDelay )
		{
//			GLog.Print("false2\n");
			return false;
		}
//		GLog.Print("true\n");
		return true;
	}

	void PlayFile( void )
	{
		DWORD t = GetCurrentTime();
		Assert( mMaximalInstances > mPlayingReferenceCount );
		Assert( t - mLastStartedTime >= mMinimalStartDelay );
		mLastStartedTime = t;
		mPlayingReferenceCount++;
	};

	void StopFile( void )
	{
		Assert( mPlayingReferenceCount > 0 );
		mPlayingReferenceCount--;
	};

	void*	mFilePointer;
	String	mFileName;
	DWORD	mLastUsedTime;

private:
	DWORD	mLastStartedTime;
	DWORD	mMinimalStartDelay;

	int		mPlayingReferenceCount;
	int		mMaximalInstances;

	int		mReferenceCount;
};

class Miles3DSound
{
	friend class Sound3D;
	friend class MilesEngine;
private:

	Miles3DSound( MilesLoadedFile* file, MilesEngine* miles, Sound3D* extSound );
	~Miles3DSound();

	void Play( void );
	void Stop( void );
	void Resume( void );
	void SetLoopCount( int count );
	int GetLoopCount( void );
	void SetHearingDistance( float minDist, float maxDist );
	void GetHearingDistance( float& minDist, float& maxDist );
	void SetPosition( Vector& position );
	Vector GetPosition( void );
	void SetOrientation( Vector& front, Vector& up );
	void GetOrientation( Vector& front, Vector& up );
	void SetVelocity( Vector& velocity );
	Vector GetVelocity( void );
	bool IsPlaying( void );
	void SetVolume( float volume, float switchTime = 0.0f );
	void UpdateVolume( void );
	void SetLowPass( float lp );
	void UseVolumeMultiplier( bool use );
	void SetAlwaysSameSpeed( bool set );

	bool ConnectToSample( int sampleIndex );
	void DisconnectFromSample( void );

	int mSampleIndex;
	bool deleteIfStopped;
	float mVolume;
	unsigned int mOffset;		// measured in bytes
	MilesLoadedFile* mFile;
	int mLoopCount;
	float mHearMin;
	float mHearMax;
	Vector mPosition;
	Vector mFront;
	Vector mUp;
	Vector mVelocity;
	DWORD mPlayStart;
	DWORD mPlayLength;		// measured in bytes
	bool mWannaPlay;
	bool mObstructed;
	MilesEngine* mMiles;
	Sound3D* mExternalSound;

	float mDesiredVolume;
	DWORD mDesiredTime;
	DWORD mLastSetVolumeTime;
	float mLowPass;
	bool mUseVolumeMultiplier;
	bool mHasAlwaysSameSpeed;
	float mSoundRateDiff;
};

class Miles2DSound
{
	friend class Sound2D;
	friend class MilesEngine;

private:
	Miles2DSound( MilesLoadedFile* file, MilesEngine* miles, Sound2D* extSound );
	~Miles2DSound();

	void Play( void );
	void Stop( void );
	void Resume( void );
	void SetLoopCount( int count );
	int GetLoopCount( void );
	bool IsPlaying( void );
	void SetVolume( float volume, float switchTime = 0.0f );
	void UpdateVolume( void );
	void SetLowPass( float lp );
	void UseVolumeMultiplier( bool use );
	void SetAlwaysSameSpeed( bool set );

	bool ConnectToSample( int sampleIndex );
	void DisconnectFromSample( void );

	int mSampleIndex;
	unsigned int mOffset;		// measured in bytes
	bool deleteIfStopped;
	float mVolume;
	MilesLoadedFile* mFile;
	int mLoopCount;
	DWORD mPlayStart;			// measured in bytes
	DWORD mPlayLength;			// measured in bytes
	bool mWannaPlay;
	MilesEngine* mMiles;
	Sound2D* mExternalSound;

	float mDesiredVolume;
	DWORD mDesiredTime;
	DWORD mLastSetVolumeTime;

	float mLowPass;
	bool mUseVolumeMultiplier;
	bool mHasAlwaysSameSpeed;
	float mSoundRateDiff;
};

class MilesEngine
{
	struct DigitalDriverProperties
	{
		F32 MasterVolumeLevel;
		S32 RoomType;
	};

public:
	HDIGDRIVER DigitalDriver;			// Digital playback device
	HPROVIDER DigitalProvider;			// 3D Sound Provider
	int CurrentRoomType;				// Current room type - reverberation etc.
	bool UsingEAX;						// Is our current sound provider using EAX?
	bool DontStartNewSounds;			// Useful in destructor

	H3DPOBJECT Listener;				// Our default listener - we

//	float DigitalEffectsVolume;			// Volume for samples
	float DigitalEffectsPan;			// Pan for samples
	float DigitalEffectsVolume3D;		// Volume for 3D samples
	float StreamingVolume;				// Volume for samples
	float StreamingPan;					// Pan for samples

	float Speed;						// Speed for playing sounds
private:
	float mDesiredSpeed;
	DWORD mLastSetSpeedTime;
	DWORD mDesiredSpeedTime;

public:
	float mVolumeMultiplier;
	float mLowPassForNewSounds;
	float mSoundRateDiffRate;
	float LowPassFrequencyPercent;		// Low pass cutoff frequency

	int mCurrentSoundProvider;			// Remember which sound provider we use

	bool mSpeakersFlip;					// Are sound speakers flipped?

	// This is a buffer for streaming sounds
	DynamicArray<HSTREAM> mStreams;
	DynamicArray<String> mStreamNames;
	DynamicArray<int> mStreamsFill;
	DynamicArray<float> mStreamLowPass;
	DynamicArray<int> mStreamSpeed;

	DynamicArray<MilesLoadedFile*> mLoadedFiles;
	int mFilledFiles;
	int mMaxFiles;

	// This is a buffer for 2D samples
	DynamicArray<HSAMPLE> m2DRealSamples;
	DynamicArray<Miles2DSound*> m2DRealSounds;
	int mMax2DSamples;			// I find out the limit in course of the game

	// Our 2D sounds list - every 2D sound made it here.
	DynamicArray<Miles2DSound*> m2DSounds;

	// This is a buffer for 3D samples
	DynamicArray<H3DSAMPLE> m3DRealSamples;
	DynamicArray<Miles3DSound*> m3DRealSounds;
	int mMax3DSamples;			// I find out the limit in course of the game

	// Our 3D sounds list - every 3D sound made it here.
	DynamicArray<Miles3DSound*> m3DSounds;

	// 3D Sound Provider lists
	DynamicArray<HPROVIDER> mProviders;		// All providers in the system - handles
	DynamicArray<String> mProviderNames;	// All providers in the system - names

private:
	int mDefaultInstances;					// for loaded sound files
	DWORD mDefaultDelay;					// for loaded sound files
public:
	// Construction/destruction
	MilesEngine(void);						// Constructor
	~MilesEngine(void);						// Destructor

	// Maintenance
	void Tick( float timeDelta );

	// Utilities
	void SetDefaultSoundProperties( int instances, DWORD delay ) { mDefaultInstances = instances; mDefaultDelay = delay; };
	void* LoadAndUnpackFile( const char* name );
	void Set3DDigitalEffectsVolume( float volume );
	void SetDigitalEffectsPan( float pan );
#ifdef TRANSGAMING
	void SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay );
#else
	void SetSoundProperties( String soundName, int maximalInstances, int minimalDelay );
#endif
	void SetSpeed( float speed, float switchTime );
	void UpdateSpeed( void );
	void SetLowPass( float percent );
	void SetAllSoundsToLowPass( float percent );
	void SetAllSoundsVolumeMultiplier( float percent );
	void SetDopplerFactor( float factor );

	// 2D samples methods
	MilesLoadedFile* GetFileByName( const char* name );			// Returns pointer to file with that name, or NULL if not found
	MilesLoadedFile* GetEmptyFileByName( const char* name );	// Returns pointer to file with that name, or NULL if not found
	MilesLoadedFile* PrepareSample( const char* name, bool playAtOnce );	// Makes sure that the sample is loaded into buffer.
	MilesLoadedFile* SureGetFile( const char* name );

	// 3D Sound Provider methods
	void Enumerate3DSoundProviders( void );
	int NumOfProviders( void ) { return mProviders.Size(); };
	bool Set3DSoundProvider( int providerIndex, float falloffSpeed );
	void Release3DSoundProvider( void );

	// 3D samples methods
	bool Start3DSample( Miles3DSound* sound, int sampleIndex );
	void Release3DSample( int sampleIndex, Miles3DSound* soundToStart = 0 );
	int Find3DSoundToStop( float& distance );
	Miles3DSound* Find3DSoundToStart( void );
	void TryToPlayRealSound( Miles3DSound* sound );

	// 2D samples methods
	bool Start2DSample( Miles2DSound* sound, int sampleIndex );
	void Release2DSample( int sampleIndex, Miles2DSound* soundToStart = 0 );
	int Find2DSoundToStop( void );
	Miles2DSound* Find2DSoundToStart( void );
	void TryToPlayRealSound2D( Miles2DSound* sound );
	void Play2DSound( MilesLoadedFile* file, float volume );

	// Streaming methods
	void ReleaseStream( int index );
};

MilesEngine::MilesEngine( void ) : DigitalDriver(0), DigitalEffectsPan(0.5f), DigitalProvider(0), UsingEAX(false),
		CurrentRoomType(ENVIRONMENT_GENERIC), Listener(0), mMaxFiles(128), mCurrentSoundProvider(-1),
		DigitalEffectsVolume3D(1.0f), StreamingVolume(1.0f), StreamingPan(0.5f), mMax3DSamples(-1),
		mMax2DSamples(-1), DontStartNewSounds( false ), mFilledFiles( 0 ), mDefaultDelay( 0 ),
		mDefaultInstances( 100 ), Speed( 1.0f ), LowPassFrequencyPercent( 1.0f ),
		mDesiredSpeed(1.0f), mLastSetSpeedTime(0), mDesiredSpeedTime(0), mLowPassForNewSounds(1.0f),
		mVolumeMultiplier(1.0f), mSpeakersFlip(false), mSoundRateDiffRate(0.0f)
{
	Enumerate3DSoundProviders();
}

MilesEngine::~MilesEngine( void )
{
	DontStartNewSounds = true;

	// Delete 3D sounds
	while( m3DSounds.Size() )
		delete m3DSounds[0];
	m3DSounds.ClearArray();

	Assert( m3DRealSamples.Size() == 0 );

	// Delete 2D sounds
	while( m2DSounds.Size() )
		delete m2DSounds[0];
	m2DSounds.ClearArray();

	Assert( m2DRealSamples.Size() == 0 );

	// Free files in memory
	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		delete mLoadedFiles[i];
	mLoadedFiles.ClearArray();

	// Close streams
	for( int i = 0; i < mStreams.Size(); ++i )
	{
		if( mStreams[i] )
			AIL_close_stream( mStreams[i] );
	};
	mStreams.ClearArray();
	mStreamNames.ClearArray();
	mStreamsFill.ClearArray();
	mStreamLowPass.ClearArray();
	mStreamSpeed.ClearArray();

	Release3DSoundProvider();

	// Dispose of digital driver
	if( DigitalDriver )
	{
		AIL_close_digital_driver( DigitalDriver );
		DigitalDriver = 0;
	};
}

void MilesEngine::Enumerate3DSoundProviders( void )
{
	HPROENUM next = HPROENUM_FIRST;

	HPROVIDER id;
	char* name;

	while( AIL_enumerate_3D_providers( &next, &id, &name ) )
	{
		mProviders.Add( id );
		mProviderNames.Add( String(name) );
	};

	if( !mProviders.Size() )
		GLog.Print( "Audio Warning: No 3D sound providers in the system!!\n" );
}

bool MilesEngine::Set3DSoundProvider( int providerIndex, float falloffSpeed )
{
	DWORD result;

	// Close the opened provider, if it exists
	Release3DSoundProvider();

	// Load the new provider
	result = AIL_open_3D_provider( mProviders[providerIndex] );

	if( result != M3D_NOERR )
	{
#ifdef AUDIO_WARNINGS_ON
		GLog.Print("Warning: AIL_open_3D_provider() error\n");
#endif
		mCurrentSoundProvider = -1;
		return false;
    }
    else
    {
		DontStartNewSounds = false;
		DigitalProvider = mProviders[ providerIndex ];
		AIL_set_3D_rolloff_factor( DigitalProvider, falloffSpeed );

		// See if we're running under an EAX compatible provider
		result = AIL_3D_room_type( DigitalProvider );
		UsingEAX = (((S32)result)!=-1)?true:false;			// will be something other than -1 on EAX

		// Create listener
		Listener = AIL_open_3D_listener( DigitalProvider );
		Assert(Listener);

		if( UsingEAX )
			AIL_set_3D_room_type( DigitalProvider, CurrentRoomType );
	}
	mCurrentSoundProvider = providerIndex;
	return true;
}

void MilesEngine::Release3DSoundProvider( void )
{
	DontStartNewSounds = true;

	// Release playing 3D sounds
	while( m3DRealSounds.Size() )
		Release3DSample( 0, 0 );

	// Release listener, if we have one
	if( Listener )
	{
		AIL_close_3D_listener( Listener );
		Listener = 0;
	}

	if( DigitalProvider != 0 )
	{
		AIL_close_3D_provider( DigitalProvider );
		DigitalProvider = 0;
		mCurrentSoundProvider = -1;
	}
}

// --------------------------- Miles 2D sound methods -----------------------

MilesLoadedFile* MilesEngine::PrepareSample( const char* name, bool playAtOnce )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "PrepareSample\n" );
	GLog.Flush();
	#endif

	// Check if file exists
	MilesLoadedFile* file = GetFileByName( name );
	if( file )
	{
		if( playAtOnce && !file->CanPlayFile() )
			return NULL;
		file->AddReference();
		return file;
	}

	// File does not exist - take it.
	file = GetEmptyFileByName( name );
	if( !file )
	{
        m_setTag("AudioMiles");
		file = new MilesLoadedFile( name, mDefaultInstances, mDefaultDelay );
		mLoadedFiles.Add( file );
	}

	// Load new file
	void* fptr = LoadAndUnpackFile( name );
	if( fptr == NULL )
	{
		GLog.Print( "Error: Couldn't load audio file: %s\n", name );
		return NULL;
	}

	file->AddReference();

	// Remove Least Recently Used free file, if neccessary

	if( mFilledFiles > mMaxFiles )
	{
		int toRemove = -1;
		DWORD lruTime = GetCurrentTime();
		for( int i = 0; i < mLoadedFiles.Size(); i++ )
		{
			MilesLoadedFile* lf = mLoadedFiles[i];
			if( lf->mFilePointer && !lf->IsReferenced() )
			{
				if( lf->mLastUsedTime < lruTime )
				{
					toRemove = i;
					lruTime = lf->mLastUsedTime;
				}
			}
		}

		if( toRemove != -1 )
		{
			mLoadedFiles[toRemove]->EmptyFile();
			mFilledFiles--;
		}
	}

	// Load new file
	file->FillFile( fptr );
	mFilledFiles++;
	return file;
}

MilesLoadedFile* MilesEngine::SureGetFile( const char* name )
{
	MilesLoadedFile* file = GetFileByName( name );
	if( file ) return file;
	file = GetEmptyFileByName( name );
	if( file ) return file;
    m_setTag("AudioMiles");
    file = new MilesLoadedFile( name, mDefaultInstances, mDefaultDelay );
	mLoadedFiles.Add( file );
	return file;
}

MilesLoadedFile* MilesEngine::GetFileByName( const char* name )
{
	String strName(name);

	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		if( ( mLoadedFiles[i]->mFileName == strName ) && ( mLoadedFiles[i]->mFilePointer != 0 ) )
			return mLoadedFiles[i];
	return 0;
}

MilesLoadedFile* MilesEngine::GetEmptyFileByName( const char* name )
{
	String strName(name);

	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		if( ( mLoadedFiles[i]->mFileName == strName ) && ( mLoadedFiles[i]->mFilePointer == 0 ) )
			return mLoadedFiles[i];
	return 0;
}

void* MilesEngine::LoadAndUnpackFile( const char* name )
{
	void* memFile;
	void* wavFile;
	S32 fileType;
	AILSOUNDINFO info;

	int size = AIL_file_size( name );
	memFile = AIL_file_read( name, 0 );

	if( memFile == NULL )
		return NULL;

	fileType = AIL_file_type( memFile, size );

	switch( fileType )
	{
	case AILFILETYPE_PCM_WAV:
		return memFile;

	case AILFILETYPE_ADPCM_WAV:
		AIL_WAV_info( memFile, &info );
		AIL_decompress_ADPCM( &info, &wavFile, 0);
		AIL_mem_free_lock( memFile );
		return wavFile;

	case AILFILETYPE_MPEG_L3_AUDIO:
		AIL_decompress_ASI( memFile, size, name, &wavFile, 0, 0 );
		AIL_mem_free_lock( memFile );
		return wavFile;

	default:
		AIL_mem_free_lock( memFile );
		return 0;
	}
}

// --------- Miles 3D sounds --------------------------------

// If sampleIndex == -1, we ask to try to alloc new sample. This may fail.
// Else we know it won't fail 'coz we just freed a sample in Release3DSample().
// If it fails then there's a problem with a sound file.

bool MilesEngine::Start3DSample( Miles3DSound* sound, int sampleIndex )
{
	if( sampleIndex == -1 )
	{
		if( mMax3DSamples == -1 )
		{
			H3DSAMPLE sample = AIL_allocate_3D_sample_handle( DigitalProvider );
			if( sample == NULL )
			{
				mMax3DSamples = m3DRealSamples.Size();
				return false;
			}
			else
			{
				m3DRealSamples.Add(sample);
				m3DRealSounds.Add(0);
			}
		}
		else if( m3DRealSamples.Size() < mMax3DSamples )
		{
			m3DRealSamples.Add(AIL_allocate_3D_sample_handle( DigitalProvider ));
			m3DRealSounds.Add(0);
		}
		else
			return false;
		sampleIndex = m3DRealSamples.Size()-1;
	}
	else
	{
		if( m3DRealSamples[sampleIndex] == 0 )
		{
			m3DRealSamples[sampleIndex] = AIL_allocate_3D_sample_handle( DigitalProvider );
			Assert( m3DRealSamples[sampleIndex] != 0 );
		}
	}

	if( !sound->ConnectToSample( sampleIndex ) )
	{
		Assert( false );
		delete sound;
		Release3DSample( sampleIndex );
		return false;
	}

	return true;
}

bool MilesEngine::Start2DSample( Miles2DSound* sound, int sampleIndex )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "Start2DSample\n" );
	GLog.Flush();
	#endif

	if( sampleIndex == -1 )
	{
		if( mMax2DSamples == -1 )
		{
			HSAMPLE sample = AIL_allocate_sample_handle( DigitalDriver );
			if( sample == NULL )
			{
				mMax2DSamples = m2DRealSamples.Size();
				return false;
			}
			else
			{
				m2DRealSamples.Add(sample);
				m2DRealSounds.Add(0);
				AIL_init_sample( m2DRealSamples[m2DRealSamples.Size()-1] );
//				S32 rate = AIL_sample_playback_rate( m2DRealSamples[m2DRealSamples.Size()-1] );
//				rate *= Speed;
//				AIL_set_sample_playback_rate( m2DRealSamples[m2DRealSamples.Size()-1], rate );
			}
		}
		else if( m2DRealSamples.Size() < mMax2DSamples )
		{
			m2DRealSamples.Add(AIL_allocate_sample_handle( DigitalDriver ));
			m2DRealSounds.Add(0);
			AIL_init_sample( m2DRealSamples[m2DRealSamples.Size()-1] );
//			S32 rate = AIL_sample_playback_rate( m2DRealSamples[m2DRealSamples.Size()-1] );
//			rate *= Speed;
//			AIL_set_sample_playback_rate( m2DRealSamples[m2DRealSamples.Size()-1], rate );
		}
		else
			return false;
		sampleIndex = m2DRealSamples.Size()-1;
	}
	else
	{
		if( m2DRealSamples[sampleIndex] == 0 )
		{
			m2DRealSamples[sampleIndex] = AIL_allocate_sample_handle( DigitalDriver );
			Assert( m2DRealSamples[sampleIndex] != 0 );
			AIL_init_sample( m2DRealSamples[sampleIndex] );
//			S32 rate = AIL_sample_playback_rate( m2DRealSamples[sampleIndex] );
//			rate *= Speed;
//			AIL_set_sample_playback_rate( m2DRealSamples[sampleIndex], rate );
		}
	}

	if( !sound->ConnectToSample( sampleIndex ) )
	{
		Assert( false );
		delete sound;
		Release2DSample( sampleIndex );
		return false;
	}

	return true;
}

void MilesEngine::Release3DSample( int sampleIndex, Miles3DSound* soundToStart )
{
	Assert( m3DRealSamples[sampleIndex] != 0 );

	// Disconnect sound from sample
	Miles3DSound* sound = m3DRealSounds[sampleIndex];
	if( sound )
		sound->DisconnectFromSample();

	// Check what should go in its place
	Miles3DSound* newSound = soundToStart?soundToStart:Find3DSoundToStart();

	if( ( !newSound ) || ( !Start3DSample( newSound, sampleIndex ) ) )
	{
		AIL_release_3D_sample_handle( m3DRealSamples[sampleIndex] );
		m3DRealSamples.Remove( sampleIndex );
		m3DRealSounds.Remove( sampleIndex );
		for( int i = sampleIndex; i < m3DRealSamples.Size(); ++i )
			if( m3DRealSounds[i] )
				m3DRealSounds[i]->mSampleIndex--;
	}
}

void MilesEngine::Release2DSample( int sampleIndex, Miles2DSound* soundToStart )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "Release2DSample\n" );
	GLog.Flush();
	#endif

	Assert( m2DRealSamples[sampleIndex] != 0 );

	// Disconnect sound from sample
	Miles2DSound* sound = m2DRealSounds[sampleIndex];
	if( sound )
		sound->DisconnectFromSample();

	// Check what should go in its place
	Miles2DSound* newSound = soundToStart?soundToStart:Find2DSoundToStart();

	if( ( !newSound ) || ( !Start2DSample( newSound, sampleIndex ) ) )
	{
		AIL_release_sample_handle( m2DRealSamples[sampleIndex] );
		m2DRealSamples.Remove( sampleIndex );
		m2DRealSounds.Remove( sampleIndex );
		for( int i = sampleIndex; i < m2DRealSamples.Size(); ++i )
			if( m2DRealSounds[i] )
				m2DRealSounds[i]->mSampleIndex--;
	}
}

void MilesEngine::TryToPlayRealSound( Miles3DSound* sound )
{
	Vector playerPos;
	AIL_3D_position( Listener, &playerPos.X, &playerPos.Y, &playerPos.Z );
	playerPos.Z = -playerPos.Z;
	Vector pos = sound->mPosition;
	float dist = (playerPos-pos).Magnitude();
	float vol = sound->mHearMin/dist;

	if( dist > sound->mHearMax )	// don't bother
		return;

	if( !sound->mFile->CanPlayFile() )
		return;

	if( !Start3DSample( sound, -1 ) )
	{
		// Couldn't start. Let's find out if we shouldn't remove some other sound.
		float minVolume;

		int indexToStop = Find3DSoundToStop( minVolume );
		Assert( indexToStop != -1 );

		if( ( minVolume == 0 ) || minVolume < vol - 0.1f )
			Release3DSample( indexToStop, sound );
	}
}

void MilesEngine::TryToPlayRealSound2D( Miles2DSound* sound )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "TryToPlayRealSound2D\n" );
	GLog.Flush();
	#endif

	if( !sound->mFile->CanPlayFile() )
		return;

	if( !Start2DSample( sound, -1 ) )
	{
		// Couldn't start. Let's find out if we shouldn't remove some other sound.
		int indexToStop = Find2DSoundToStop();
		Assert( indexToStop != -1 );
		if( m2DRealSounds[indexToStop]->mPlayStart < sound->mPlayStart )
			Release2DSample( indexToStop, sound );
	}
}

// Choose least slightly heard sound ( by hearing distance )
int MilesEngine::Find3DSoundToStop( float& volume )
{
	Vector playerPos;
	AIL_3D_position( Listener, &playerPos.X, &playerPos.Y, &playerPos.Z );

	float minVolume = 10.0f;
	float maxDistance = 0;
	int farthest = -1;
	for( int i = 0; i < m3DRealSamples.Size(); ++i )
	{
		Assert( m3DRealSounds[i]->mSampleIndex == i );
		Vector pos;
		AIL_3D_position( m3DRealSamples[i], &pos.X, &pos.Y, &pos.Z );
		float dist = (playerPos-pos).Magnitude();
		if( dist > m3DRealSounds[i]->mHearMax )
		{
			minVolume = 0;
			if( dist > maxDistance )
			{
				maxDistance = dist;
				farthest = i;
			}
		}
		else if( minVolume > 0 )
		{
			float vol = m3DRealSounds[i]->mHearMin/dist;
			if( vol < minVolume )
			{
				minVolume = vol;
				maxDistance = dist;
				farthest = i;
			}
		}
	}

	volume = minVolume;
	return farthest;
}

// Stop the sound that was started earliest
int MilesEngine::Find2DSoundToStop( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "Find2DSoundToStop\n" );
	GLog.Flush();
	#endif

	DWORD time = GetCurrentTime();
	int index = -1;
	for( int i = 0; i < m2DRealSounds.Size(); ++i )
	{
		if( m2DRealSounds[i]->mPlayStart < time )
		{
			time = m2DRealSounds[i]->mPlayStart;
			index = i;
		}
	}
	return index;
}

// Choose closest sound ( by hearing distance ) and return 0 if it's still beyond hearing
Miles3DSound* MilesEngine::Find3DSoundToStart( void )
{
	if( DontStartNewSounds )
		return 0;

	Vector playerPos;
	AIL_3D_position( Listener, &playerPos.X, &playerPos.Y, &playerPos.Z );
	playerPos.Z = -playerPos.Z;

	float maxVolume = 100000.0f;
	int closest = -1;
	for( int i = 0; i < m3DSounds.Size(); ++i )
	{
		if( m3DSounds[i]->mSampleIndex == -1 )
		{
			if( m3DSounds[i]->IsPlaying() && m3DSounds[i]->mFile->CanPlayFile() )
			{
				Vector pos = m3DSounds[i]->mPosition;
				float dist = (playerPos-pos).Magnitude();
				float vol = m3DSounds[i]->mHearMin/dist;

				if( ( dist < m3DSounds[i]->mHearMax ) && ( vol < maxVolume ) )
				{
					maxVolume = vol;
					closest = i;
				}
			}
		}
	}
	return (closest == -1)?0:m3DSounds[closest];
}

// Start the sound that was started latest - chances are it will last longer
Miles2DSound* MilesEngine::Find2DSoundToStart( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "Find2DSoundToStart\n" );
	GLog.Flush();
	#endif

	if( DontStartNewSounds )
		return 0;

	DWORD time = 0;
	int index = -1;

	for( int i = 0; i < m2DSounds.Size(); ++i )
	{
		if( ( m2DSounds[i]->mSampleIndex == -1 ) && ( m2DSounds[i]->IsPlaying() ) )
		{
			if( m2DSounds[i]->mPlayStart > time )
			{
				if( m2DSounds[i]->mFile->CanPlayFile() )
				{
					time = m2DSounds[i]->mPlayStart;
					index = i;
				}
			}
		}
	}

	return (index == -1)?0:m2DSounds[index];
}

void MilesEngine::ReleaseStream( int index )
{
	if( mStreams[index] != 0 )
	{
		AIL_close_stream( mStreams[index] );
		mStreams[index] = 0;
		mStreamNames[index] = "";
		mStreamLowPass[index] = 1.0f;
		mStreamSpeed[index] = 0;
		mStreamsFill.Add(index);
	};
}

void MilesEngine::Tick( float timeDelta )
{
	// Execute yourself not more frequently than 10 times per second
//	static DWORD frequencyTimer = GetCurrentTime();
//	DWORD currentTime = GetCurrentTime();
//	if( currentTime - frequencyTimer < 100 )
//		return;
//	frequencyTimer = currentTime;

	// Fix sound volume change
	if( Speed != mDesiredSpeed )
		UpdateSpeed();

	static int counter = 0;

    static Miles3DSound* mSoundsToReleaseStatic[256] = {0};
	static int mSoundsToReleaseStaticCounter = 0;

    static Miles2DSound* m2DSoundsToReleaseStatic[256] = {0};
	static int m2DSoundsToReleaseStaticCounter = 0;

	mSoundsToReleaseStaticCounter = 0;
	m2DSoundsToReleaseStaticCounter = 0;

	// Update things for 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
	{
		// Fix sound volume change
		if( m3DSounds[i]->mVolume != m3DSounds[i]->mDesiredVolume )
			m3DSounds[i]->UpdateVolume();

		if( !m3DSounds[i]->IsPlaying() && m3DSounds[i]->deleteIfStopped )
		{
			Assert( mSoundsToReleaseStaticCounter < 255 );
			mSoundsToReleaseStatic[mSoundsToReleaseStaticCounter++] = m3DSounds[i];
		}
		else if( m3DSounds[i]->mSampleIndex != -1 )
		{
			U32 status = AIL_3D_sample_status( m3DRealSamples[m3DSounds[i]->mSampleIndex] );
			switch( status )
			{
			case SMP_PLAYING:
//				AIL_update_3D_position( m3DRealSamples[m3DSounds[i]->mSampleIndex], timeDelta*1000.0f );
				break;
			case SMP_STOPPED:
			case SMP_DONE:
				m3DSounds[i]->mWannaPlay = false;
				if( m3DSounds[i]->deleteIfStopped )
				{
					Assert( mSoundsToReleaseStaticCounter < 255 );
					mSoundsToReleaseStatic[mSoundsToReleaseStaticCounter++] = m3DSounds[i];
				}
				else
					Release3DSample( m3DSounds[i]->mSampleIndex );
				break;
			default:
				;
			};
		}
		else
		{
			// this should be done in the sound entity, by set position ( much more accurate )
//			m3DSounds[i]->mPosition += ( m3DSounds[i]->mVelocity * timeDelta*1000.0f );
			if( !m3DSounds[i]->IsPlaying() )
			{
				if( (GetCurrentTime() - m3DSounds[i]->mPlayStart) > m3DSounds[i]->mPlayLength )
				{
					m3DSounds[i]->mWannaPlay = false;
					if( m3DSounds[i]->deleteIfStopped )
					{
						Assert( mSoundsToReleaseStaticCounter < 255 );
						mSoundsToReleaseStatic[mSoundsToReleaseStaticCounter++] = m3DSounds[i];
					}
				}
			}
		}
	}

	for( int i = mSoundsToReleaseStaticCounter-1; i >= 0; --i )
		delete mSoundsToReleaseStatic[i];

	// Update things for 2D sounds
	for( int i = 0; i < m2DRealSounds.Size(); ++i )
	{
		U32 status = AIL_sample_status( m2DRealSamples[i] );
		switch( status )
		{
		case SMP_PLAYING:
		default:
			break;
		case SMP_STOPPED:
		case SMP_DONE:
//			{
//				DWORD t = GetCurrentTime() - m2DRealSounds[i]->mPlayStart;
//				DWORD u = m2DRealSounds[i]->mPlayLength;
//				GLog.Print("Stopped/should: %d/%d = %f\n", t, u, ((float)(t)/(float(u))) );
//			}
			m2DRealSounds[i]->mWannaPlay = false;
			if( m2DRealSounds[i]->deleteIfStopped )
			{
				Assert( m2DSoundsToReleaseStaticCounter < 255 );
				m2DSoundsToReleaseStatic[m2DSoundsToReleaseStaticCounter++] = m2DRealSounds[i];
			}
			else
				Release2DSample( m2DRealSounds[i]->mSampleIndex );
			break;
		}
	}

	for( int i = m2DSoundsToReleaseStaticCounter-1; i >= 0; --i )
		delete m2DSoundsToReleaseStatic[i];
	m2DSoundsToReleaseStaticCounter = 0;

	for( int i = 0; i < m2DSounds.Size(); ++i )
	{
		// Fix sound volume change
		if( m2DSounds[i]->mVolume != m2DSounds[i]->mDesiredVolume )
			m2DSounds[i]->UpdateVolume();

		if( !m2DSounds[i]->IsPlaying() && m2DSounds[i]->deleteIfStopped )
		{
			Assert( m2DSoundsToReleaseStaticCounter < 255 );
			m2DSoundsToReleaseStatic[m2DSoundsToReleaseStaticCounter++] = m2DSounds[i];
		}
		else if( m2DSounds[i]->mSampleIndex == -1 )
		{
			if( !m2DSounds[i]->IsPlaying() )
			{
				if( (GetCurrentTime() - m2DSounds[i]->mPlayStart) > m2DSounds[i]->mPlayLength )
				{
//					{
//						DWORD t = GetCurrentTime() - m2DSounds[i]->mPlayStart;
//						DWORD u = m2DSounds[i]->mPlayLength;
//						GLog.Print("Stopping/should: %d/%d = %f\n", t, u, ((float)(t)/(float(u))) );
//					}
					m2DSounds[i]->mWannaPlay = false;
					if( m2DSounds[i]->deleteIfStopped )
					{
						Assert( m2DSoundsToReleaseStaticCounter < 255 );
						m2DSoundsToReleaseStatic[m2DSoundsToReleaseStaticCounter++] = m2DSounds[i];
					}
				}
			}
		}
	}

	for( int i = m2DSoundsToReleaseStaticCounter-1; i >= 0; --i )
		delete m2DSoundsToReleaseStatic[i];

	// Stop farthest sound if it's far from here
	float minVolume;
	int toStop = Find3DSoundToStop( minVolume );
	while( minVolume == 0.0f )
	{
		Miles3DSound* toStart = Find3DSoundToStart();
		Release3DSample( toStop, toStart );
		toStop = Find3DSoundToStop( minVolume );
	}

	// Start closest sounds if we have a place
	Miles3DSound* toStart = 0;
	Miles3DSound* lastToStart = 0;
	while( (toStart = Find3DSoundToStart()) != 0 )
	{
		if( ( mMax3DSamples == -1 ) || ( m3DRealSamples.Size() < mMax3DSamples ) )
		{
			Assert( lastToStart != toStart );
			TryToPlayRealSound( toStart );
		}
		else
			break;
		lastToStart = toStart;
	}

	// And for listener
	if( Listener )
	{
//		AIL_update_3D_position( Listener, timeDelta*1000.0f );

		// Advanced 3D obstacles check
		Vector playerPos;
		AIL_3D_position( Listener, &playerPos.X, &playerPos.Y, &playerPos.Z );
		playerPos.Z = -playerPos.Z;

		// So those are not really obstructed right now, just as Skorpik wanted
		counter++;
		if( counter >= m3DRealSamples.Size() )
			counter = 0;

		if( ( m3DRealSamples.Size() > counter ) &&
			( m3DRealSounds[counter]->mObstructed ) &&
			( AIL_3D_sample_status( m3DRealSamples[counter] ) == SMP_PLAYING ) )
		{
			Vector soundPos;
			HitData hitData;
			AIL_3D_position( m3DRealSamples[counter], &soundPos.X, &soundPos.Y, &soundPos.Z );
			soundPos.Z = -soundPos.Z;

			float dist = (soundPos-playerPos).Magnitude();
			float currObstr = AIL_3D_sample_obstruction( m3DRealSamples[counter] );
			float targetObstr;
			if( GEngine->World->Physics->LineTraceFixedGeom( playerPos, soundPos, hitData ) )	// need distance
			{
				float max = m3DRealSounds[counter]->mHearMax;
				targetObstr = 0.5f + 0.5f*dist/max;
			}
			else
				targetObstr = 0.0f;

			float newObstr;
			if( targetObstr - currObstr > 0.3f )
				newObstr = currObstr + 0.3f;
			else if( targetObstr - currObstr < -0.3f )
				newObstr = currObstr - 0.3f;
			else
				newObstr = targetObstr;

			AIL_set_3D_sample_obstruction( m3DRealSamples[counter], newObstr );
		}
	}
}

void MilesEngine::Set3DDigitalEffectsVolume( float volume )
{
	if( !DigitalDriver ) return;

	bool changed = false;

	if( volume > 1.0f ) volume = 1.0f;
	if( volume < 0.0f ) volume = 0.0f;
	float currMas = AIL_digital_master_volume_level( DigitalDriver );
	volume *= currMas;
	if( DigitalEffectsVolume3D != volume ) changed = true;
	DigitalEffectsVolume3D = volume;

	// Update digital samples
	if( changed )
		for( int i = 0; i < m3DRealSamples.Size(); i++ )
		{
			if( ( AIL_3D_sample_status( m3DRealSamples[i] ) == SMP_PLAYING ) ||
				( AIL_3D_sample_status( m3DRealSamples[i] ) == SMP_STOPPED ) )
			{
				float targetVolume = m3DRealSounds[i]->mUseVolumeMultiplier?DigitalEffectsVolume3D*m3DRealSounds[i]->mVolume*mVolumeMultiplier:DigitalEffectsVolume3D*m3DRealSounds[i]->mVolume;
				AIL_set_3D_sample_volume( m3DRealSamples[i], targetVolume );
			}
		}
}

void MilesEngine::SetDigitalEffectsPan( float pan )
{
	if( !DigitalDriver ) return;
	bool changed = false;

	if( pan > 1.0f ) pan = 1.0f;
	if( pan < 0.0f ) pan = 0.0f;
	if( DigitalEffectsPan != pan ) changed = true;
	DigitalEffectsPan = pan;

	// Update digital samples
	if( changed )
		for( int i = 0; i < m2DRealSamples.Size(); i++ )
			if( ( AIL_sample_status( m2DRealSamples[i] ) == SMP_PLAYING ) ||
				( AIL_sample_status( m2DRealSamples[i] ) == SMP_STOPPED ) )
			{
				float targetVolume = m2DRealSounds[i]->mUseVolumeMultiplier?DigitalEffectsVolume3D*m2DRealSounds[i]->mVolume*mVolumeMultiplier:DigitalEffectsVolume3D*m2DRealSounds[i]->mVolume;
//				AIL_set_sample_volume_pan( m2DRealSamples[i], targetVolume, DigitalEffectsPan );
				AIL_set_sample_volume_levels( m2DRealSamples[i], targetVolume, targetVolume );
			}
}

void MilesEngine::Play2DSound( MilesLoadedFile* file, float volume )
{
    m_setTag("AudioMiles");
	Miles2DSound* sound = new Miles2DSound( file, this, 0 );
	m2DSounds.Add( sound );
	sound->deleteIfStopped = true;
	sound->SetVolume( volume );
	sound->Play();
}

#ifdef TRANSGAMING
void MilesEngine::SetSoundProperties(const char *soundName, int maximalInstances, int minimalDelay){
	if (!strcmp(soundName, "default"))
		SetDefaultSoundProperties(maximalInstances, minimalDelay);

	else{
		MilesLoadedFile* file = SureGetFile(soundName);
		file->SetProperties(maximalInstances, minimalDelay);
	}
}
#else
void MilesEngine::SetSoundProperties( String soundName, int maximalInstances, int minimalDelay )
{
	if( soundName == "default" )
		SetDefaultSoundProperties( maximalInstances, minimalDelay );
	else
	{
		MilesLoadedFile* file = SureGetFile( soundName.AsChar() );
		file->SetProperties( maximalInstances, minimalDelay );
	}
}
#endif

void MilesEngine::SetSpeed( float speed, float switchTime )
{
	if( !switchTime )
	{
		mDesiredSpeed = speed;
		mLastSetSpeedTime = GetCurrentTime();
		mDesiredSpeedTime = mLastSetSpeedTime-1;
		UpdateSpeed();
	}
	else
	{
		mDesiredSpeed = speed;
		mLastSetSpeedTime = GetCurrentTime();
		mDesiredSpeedTime = mLastSetSpeedTime + floor(switchTime*1000);
	}
}

void MilesEngine::UpdateSpeed( void )
{
	DWORD time = GetCurrentTime();

	float newSpeed = 1.0f;

	if( time >= mDesiredSpeedTime )
	{
		newSpeed = mDesiredSpeed;
		mDesiredSpeedTime = time;
	}
	else
		newSpeed = ((double)(time-mLastSetSpeedTime))/((double)(mDesiredSpeedTime-mLastSetSpeedTime))*((double)(mDesiredSpeed-Speed)) + Speed;

	mLastSetSpeedTime = time;

	Assert( newSpeed < 1000.0f );
	Assert( newSpeed > 0.0001f );

	double speedChange = newSpeed/Speed;

	if( speedChange == 1.0f ) return;

	Speed = newSpeed;

	// Change speed for 2D sounds
	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( !m2DSounds[i]->mHasAlwaysSameSpeed )
		{
			if( m2DSounds[i]->mPlayStart )
				m2DSounds[i]->mPlayStart = time - ( time - m2DSounds[i]->mPlayStart ) / speedChange;

			if( m2DSounds[i]->mPlayLength != DWORD(-1) )
				m2DSounds[i]->mPlayLength /= speedChange;
		}

	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !m2DRealSounds[i]->mHasAlwaysSameSpeed )
		{
			AILSOUNDINFO si;
			int res = AIL_WAV_info( m2DRealSounds[i]->mFile->mFilePointer, &si );
			Assert( res );
			if( res ) AIL_set_sample_playback_rate( m2DRealSamples[i], si.rate*Speed*m2DRealSounds[i]->mSoundRateDiff );
		}

	// Change speed for 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !m3DSounds[i]->mHasAlwaysSameSpeed )
		{
			if( m3DSounds[i]->mPlayStart )
				m3DSounds[i]->mPlayStart = time - ( time - m3DSounds[i]->mPlayStart ) / speedChange;

			if( m3DSounds[i]->mPlayLength != DWORD(-1) )
				m3DSounds[i]->mPlayLength /= speedChange;
		}

	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !m3DRealSounds[i]->mHasAlwaysSameSpeed )
		{
			AILSOUNDINFO si;
			int res = AIL_WAV_info( m3DRealSounds[i]->mFile->mFilePointer, &si );
			Assert( res );
			if( res ) AIL_set_3D_sample_playback_rate( m3DRealSamples[i], si.rate*Speed*m3DRealSounds[i]->mSoundRateDiff );
		}

	// Change speed for streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i] )
			AIL_set_stream_playback_rate( mStreams[i], mStreamSpeed[i]*Speed );
}

void MilesEngine::SetLowPass( float percent )
{
	if( percent > 1.0f ) percent = 1.0f;
	if( percent < 0.0f ) percent = 0.0f;

	LowPassFrequencyPercent = percent;

	// Change frequency for 2D sounds
	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !m2DRealSounds[i]->mHasAlwaysSameSpeed && ( m2DRealSounds[i]->mLowPass > percent ) )
			AIL_set_sample_low_pass_cut_off( m2DRealSamples[i], percent );

	// Change frequency for 3D sounds
	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !m3DRealSounds[i]->mHasAlwaysSameSpeed && ( m3DRealSounds[i]->mLowPass > percent ) )
			AIL_set_3D_sample_occlusion( m3DRealSamples[i], 1.0f - percent );

	// Change frequency for streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i] && ( mStreamLowPass[i] > percent ) )
			AIL_set_stream_low_pass_cut_off( mStreams[i], percent );
}

void MilesEngine::SetAllSoundsToLowPass( float percent )
{
	if( percent > 1.0f ) percent = 1.0f;
	if( percent < 0.0f ) percent = 0.0f;

	mLowPassForNewSounds = percent;

	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( !m2DSounds[i]->mHasAlwaysSameSpeed )
			m2DSounds[i]->mLowPass = percent;

	float percentToSet = (percent > LowPassFrequencyPercent)?LowPassFrequencyPercent:percent;

	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !m2DRealSounds[i]->mHasAlwaysSameSpeed )
			AIL_set_sample_low_pass_cut_off( m2DRealSamples[i], percentToSet );

	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !m3DSounds[i]->mHasAlwaysSameSpeed )
			m3DSounds[i]->mLowPass = percent;

	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !m3DRealSounds[i]->mHasAlwaysSameSpeed )
			AIL_set_3D_sample_occlusion( m3DRealSamples[i], 1.0f - percentToSet );

	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i] )
			AIL_set_stream_low_pass_cut_off( mStreams[i], percentToSet );
}

void MilesEngine::SetAllSoundsVolumeMultiplier( float percent )
{
	if( percent > 1.0f ) percent = 1.0f;
	if( percent < 0.001f ) percent = 0.001f;

	if( mVolumeMultiplier == percent ) return;

	double newToOld = percent/mVolumeMultiplier;
	mVolumeMultiplier = percent;

	F32 volume;
	F32 pan;

	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( m2DRealSounds[i]->mUseVolumeMultiplier )
		{
			AIL_sample_volume_pan( m2DRealSamples[i], &volume, &pan );
			volume *= newToOld;
//			AIL_set_sample_volume_pan( m2DRealSamples[i], volume, pan );
			AIL_set_sample_volume_levels( m2DRealSamples[i], volume, volume );
		}

	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( m3DRealSounds[i]->mUseVolumeMultiplier )
		{
			volume = AIL_3D_sample_volume( m3DRealSamples[i] );
			volume *= newToOld;
			AIL_set_3D_sample_volume( m3DRealSamples[i], volume );
		}

	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i] )
		{
			AIL_stream_volume_pan( mStreams[i], &volume, &pan );
			volume *= newToOld;
			AIL_set_stream_volume_pan( mStreams[i], volume, pan );
		}
}

void MilesEngine::SetDopplerFactor( float factor )
{
	AIL_set_3D_doppler_factor( DigitalProvider, factor );
}

// --------------------- Miles3DSound methods ---------------------
Miles3DSound::Miles3DSound( MilesLoadedFile* file, MilesEngine* miles, Sound3D* extSound ) : mSampleIndex(-1), mLoopCount(1), deleteIfStopped(false),
		mMiles(miles), mFile(file), mHearMin(0.01f), mHearMax(100.0f), mPosition(0,0,0),
		mFront(0,0,1), mUp(0,1,0), mVelocity(0,0,0), mPlayStart(0), mPlayLength(0),
		mWannaPlay(false), mObstructed(false), mExternalSound( extSound ), mVolume(1.0f), mOffset(0),
		mDesiredVolume(1.0f), mDesiredTime(0.0f), mLastSetVolumeTime(0), mUseVolumeMultiplier(true), mHasAlwaysSameSpeed(false)
{
	mLowPass = miles->mLowPassForNewSounds;
	mSoundRateDiff = 1.0f + (2*(((float)rand())/RAND_MAX)-1)*miles->mSoundRateDiffRate;
}

Miles3DSound::~Miles3DSound()
{
	for( int i = 0; i < mMiles->m3DSounds.Size(); ++i )
	{
		if( mMiles->m3DSounds[i] == this )
		{
			mMiles->m3DSounds.Remove(i);
			break;
		}
	}

	if( mSampleIndex != -1 )
	{
		mMiles->Release3DSample( mSampleIndex );
		mSampleIndex = -1;
	}

	if( mFile )
		mFile->RemoveReference();

	if( mExternalSound )
	{
		mExternalSound->mSound = 0;
		delete mExternalSound;
	}
}

void Miles3DSound::Play( void )
{
	if( mWannaPlay ) Stop();	// stop it first

	mOffset = 0;
	mWannaPlay = true;
	mPlayStart = GetCurrentTime();
	mMiles->TryToPlayRealSound( this );

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);
			mPlayLength = mLoopCount*playOnceTime;
		}
		else
			mPlayLength = 0;
	}
}

void Miles3DSound::Stop( void )
{
	mWannaPlay = false;
	mPlayLength = 0;
	if( mSampleIndex != -1 )
	{
		mOffset = AIL_3D_sample_offset( mMiles->m3DRealSamples[mSampleIndex] );
		// Get loop count
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_3D_sample_loop_count( mMiles->m3DRealSamples[mSampleIndex] );
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
		mMiles->Release3DSample( mSampleIndex );
	}
	else
		mOffset = 0;
}

void Miles3DSound::Resume( void )
{
	mWannaPlay = true;
	mMiles->TryToPlayRealSound( this );
	mPlayStart = GetCurrentTime();

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);			// measured in bytes
			DWORD offsetTime = mOffset*1000*8/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff)/si.bits;	// measured in bytes
			mPlayLength = mLoopCount*playOnceTime - offsetTime;
		}
		else
			mPlayLength = 0;
	}
}

void Miles3DSound::SetLoopCount( int count )
{
	mLoopCount = count;
	if( mSampleIndex != -1 )
		AIL_set_3D_sample_loop_count( mMiles->m3DRealSamples[mSampleIndex], count );
}

int Miles3DSound::GetLoopCount( void )
{
	int loopCount = 0;
	if( mSampleIndex != -1 )
		loopCount = AIL_3D_sample_loop_count( mMiles->m3DRealSamples[mSampleIndex] );
	return loopCount;
}

void Miles3DSound::SetHearingDistance( float minDist, float maxDist )
{
	mHearMin = minDist;
	mHearMax = maxDist;
	if( mSampleIndex != -1 )
		AIL_set_3D_sample_distances( mMiles->m3DRealSamples[mSampleIndex], maxDist, minDist );
}

void Miles3DSound::GetHearingDistance( float& minDist, float& maxDist )
{
	if( mSampleIndex != -1 )
		AIL_3D_sample_distances( mMiles->m3DRealSamples[mSampleIndex], &mHearMax, &mHearMin );
	minDist = mHearMin;
	maxDist = mHearMax;
}

void Miles3DSound::SetPosition( Vector& position )
{
	mPosition = position;
	if( mSampleIndex != -1 )
		AIL_set_3D_position( mMiles->m3DRealSamples[mSampleIndex], mPosition.X, mPosition.Y, -mPosition.Z );
}

Vector Miles3DSound::GetPosition( void )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_position( mMiles->m3DRealSamples[mSampleIndex], &mPosition.X, &mPosition.Y, &mPosition.Z );
		mPosition.Z = -mPosition.Z;
	}
	return mPosition;
}

void Miles3DSound::SetOrientation( Vector& front, Vector& up )
{
	mFront = front;
	mUp = up;
	if( mSampleIndex != -1 )
		AIL_set_3D_orientation( mMiles->m3DRealSamples[mSampleIndex], mFront.X, mFront.Y, -mFront.Z, mUp.X, mUp.Y, -mUp.Z );
}

void Miles3DSound::GetOrientation( Vector& front, Vector& up )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_orientation( mMiles->m3DRealSamples[mSampleIndex], &mFront.X, &mFront.Y, &mFront.Z, &mUp.X, &mUp.Y, &mUp.Z );
		mFront.Z = -mFront.Z;
		mUp.Z = -mUp.Z;
	}
	front = mFront;
	up = mUp;
}

void Miles3DSound::SetVelocity( Vector& velocity )
{
	mVelocity = velocity;
	if( mSampleIndex != -1 )
	{
		float magn = mVelocity.Magnitude();
		Vector vel = mVelocity;
		vel.Normalize();
		AIL_set_3D_velocity( mMiles->m3DRealSamples[mSampleIndex], vel.X, vel.Y, -vel.Z, 0.001f*magn );
	}
}

Vector Miles3DSound::GetVelocity( void )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_velocity( mMiles->m3DRealSamples[mSampleIndex], &mVelocity.X, &mVelocity.Y, &mVelocity.Z );
		mVelocity.Z = -mVelocity.Z;
		mVelocity *= 1000.0f;
	}
	return mVelocity;
}

bool Miles3DSound::IsPlaying( void )
{
	if( mSampleIndex != -1 )
	{
		U32 out = AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] );
		return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) );
	}
	else if( !mWannaPlay )
		return false;
	else if( mLoopCount == 0 )
		return true;
	else if( mPlayLength == DWORD(-1) )
		return true;
	else
		return( ( GetCurrentTime() - mPlayStart ) < mPlayLength );
}

void Miles3DSound::SetVolume( float volume, float switchTime )
{
	if( volume < 0.0f ) volume = 0.0f;
	if( volume > 1.0f ) volume = 1.0f;

	if( !switchTime )
		mVolume = volume;

	mDesiredVolume = volume;
	mLastSetVolumeTime = GetCurrentTime();
	mDesiredTime = mLastSetVolumeTime + floor(switchTime*1000);

	if( !switchTime && ( mSampleIndex != -1 ) &&
		( ( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
		AIL_set_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex], targetVolume );
	}
}

void Miles3DSound::SetLowPass( float lp )
{
	if( lp < 0.0f ) lp = 0.0f;
	if( lp > 1.0f ) lp = 1.0f;

	mLowPass = lp;
	float percent = lp;
	if( !mHasAlwaysSameSpeed && mMiles->LowPassFrequencyPercent < lp )
		percent = mMiles->LowPassFrequencyPercent;

	if( ( mSampleIndex != -1 ) &&( ( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
		AIL_set_3D_sample_occlusion( mMiles->m3DRealSamples[mSampleIndex], percent );
}

void Miles3DSound::UseVolumeMultiplier( bool use )
{
	if( use == mUseVolumeMultiplier ) return;

	mUseVolumeMultiplier = use;

	double newToOld = use?mMiles->mVolumeMultiplier:1.0/mMiles->mVolumeMultiplier;
	if( newToOld == 1.0 ) return;
	if( ( mSampleIndex != -1 ) &&( ( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		F32 volume = AIL_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex] );
		volume *= newToOld;
		AIL_set_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex], volume );
	}
}

void Miles3DSound::SetAlwaysSameSpeed( bool set )
{
	mHasAlwaysSameSpeed = set;
}

void Miles3DSound::UpdateVolume( void )
{
//	if( mDesiredVolume == mVolume )
//		return;

	DWORD curTime = GetCurrentTime();

	if( curTime >= mDesiredTime )
	{
		mVolume = mDesiredVolume;
		mDesiredTime = curTime;
	}
	else
		mVolume = ((double)(curTime-mLastSetVolumeTime))/((double)(mDesiredTime-mLastSetVolumeTime))*((double)(mDesiredVolume-mVolume)) + mVolume;

	mLastSetVolumeTime = curTime;

	if( ( mSampleIndex != -1 ) &&
		( ( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
		AIL_set_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex], targetVolume );
	}

}

bool Miles3DSound::ConnectToSample( int sampleIndex )
{
	Assert( mSampleIndex == -1 );
	DWORD time = GetCurrentTime();

	H3DSAMPLE sample = mMiles->m3DRealSamples[sampleIndex];
	if( AIL_set_3D_sample_file( sample, mFile->mFilePointer ) == 0 )
		return false;

	AILSOUNDINFO si;
#ifdef _DEBUG
	int result = AIL_WAV_info( mFile->mFilePointer, &si );
	Assert( result );
#else
	AIL_WAV_info( mFile->mFilePointer, &si );
#endif
	AIL_set_3D_sample_playback_rate( sample, (mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff );
//	GLog.Print( "mSoundRateDiff = %f\n", mSoundRateDiff );

	float percent = ( mMiles->LowPassFrequencyPercent < mLowPass )?mMiles->LowPassFrequencyPercent:mLowPass;
	if( mHasAlwaysSameSpeed ) percent = mLowPass;
	AIL_set_3D_sample_occlusion( sample, 1.0f - percent );

	mFile->mLastUsedTime = time;
	mMiles->m3DRealSounds[sampleIndex] = this;
	mSampleIndex = sampleIndex;

	// Set appropriate offset
	DWORD timeSinceStarted = time - mPlayStart;
	DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);

	int loopCount = mLoopCount;
	if( loopCount > 0 )
	{
		loopCount -= ( timeSinceStarted / playOnceTime );
		if( loopCount < 1 ) loopCount = 1;			// for release, if debug still not fixed
	}

	timeSinceStarted %= playOnceTime;
	DWORD offset = timeSinceStarted*((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff)/1000;

	offset *= si.bits/8;
	if( offset % (si.bits/8) != 0 ) offset--;
	AIL_set_3D_sample_offset( sample, offset );

	// Set loop count
	AIL_set_3D_sample_loop_count( sample, loopCount );

	// Set hearing distance
	AIL_set_3D_sample_distances( sample, mHearMax, mHearMin );

	// Set position
	AIL_set_3D_position( sample, mPosition.X, mPosition.Y, -mPosition.Z );

	// Set orientation
	AIL_set_3D_orientation( sample, mFront.X, mFront.Y, -mFront.Z, mUp.X, mUp.Y, -mUp.Z );

	// Set velocity
	float magn = mVelocity.Magnitude();
	Vector vel = mVelocity;
	vel.Normalize();
	AIL_set_3D_velocity( sample, vel.X, vel.Y, -vel.Z, 0.001f*magn );

	// Set volume
	float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
	AIL_set_3D_sample_volume( sample, targetVolume );

	// Determine starting sample obstruction
	if( mObstructed )
	{
		Vector playerPos;
		AIL_listener_3D_position( &playerPos.X, &playerPos.Y, &playerPos.Z );
		playerPos.Z = -playerPos.Z;

		Vector soundPos = mPosition;
		soundPos.Z = -soundPos.Z;

		HitData hitData;
		float dist = (mPosition-playerPos).Magnitude();
		float obstr = 0.0f;
		if( GEngine->World->Physics->LineTraceFixedGeom( playerPos, soundPos, hitData ) )	// need distance
			obstr = 0.5f + 0.5f*dist/mHearMax;

		AIL_set_3D_sample_obstruction( sample, obstr );
	}
	else
		AIL_set_3D_sample_obstruction( sample, 0.0f );

	mFile->PlayFile();
//	GLog.Print( "Time: %d, File: %s played\n", time, mFile->mFileName );

	// Start sample

	if( offset )
		AIL_resume_3D_sample( sample );
	else
		AIL_start_3D_sample( sample );
	return true;
}

void Miles3DSound::DisconnectFromSample( void )
{
	Assert( mSampleIndex != -1 );

	H3DSAMPLE sample = mMiles->m3DRealSamples[mSampleIndex];

//	GLog.Print("Sound Discon: ind: %d, snd: %d\n", mSampleIndex, (int)this );

	mFile->StopFile();

	// Stop sample
	AIL_stop_3D_sample( sample );

	// Get velocity
	AIL_3D_velocity( sample, &mVelocity.X, &mVelocity.Y, &mVelocity.Z );
	mVelocity.Z = -mVelocity.Z;
	mVelocity *= 1000.0f;

	// Get orientation
	AIL_3D_orientation( sample, &mFront.X, &mFront.Y, &mFront.Z, &mUp.X, &mUp.Y, &mUp.Z );
	mFront.Z = -mFront.Z;
	mUp.Z = -mUp.Z;

	// Get position
	AIL_3D_position( sample, &mPosition.X, &mPosition.Y, &mPosition.Z );
	mPosition.Z = -mPosition.Z;

	// Get hearing distances
	AIL_3D_sample_distances( sample, &mHearMax, &mHearMin );

	mMiles->m3DRealSounds[mSampleIndex] = 0;
	mSampleIndex = -1;
}

// --------------------- Miles2DSound methods ---------------------
Miles2DSound::Miles2DSound( MilesLoadedFile* file, MilesEngine* miles, Sound2D* extSound ) : mSampleIndex(-1),
		mLoopCount(1), deleteIfStopped(false), mMiles(miles), mFile(file), mOffset(0), mPlayStart(0),
		mPlayLength(0), mWannaPlay(false), mExternalSound( extSound ), mVolume(1.0f),
		mDesiredVolume(1.0f), mDesiredTime(0.0f), mLastSetVolumeTime(0), mUseVolumeMultiplier(true), mHasAlwaysSameSpeed(false)
{
	mLowPass = miles->mLowPassForNewSounds;
	mSoundRateDiff = 1.0f + (2*(((float)rand())/RAND_MAX)-1)*miles->mSoundRateDiffRate;
}

Miles2DSound::~Miles2DSound()
{
	for( int i = 0; i < mMiles->m2DSounds.Size(); ++i )
	{
		if( mMiles->m2DSounds[i] == this )
		{
			mMiles->m2DSounds.Remove(i);
			break;
		}
	}

	if( mSampleIndex != -1 )
	{
		mMiles->Release2DSample( mSampleIndex );
		mSampleIndex = -1;
	}

	if( mFile )
		mFile->RemoveReference();

	if( mExternalSound )
	{
		mExternalSound->mSound = 0;
		delete mExternalSound;
	}
}

void Miles2DSound::Play( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "2DSound::Play\n" );
	GLog.Flush();
	#endif

	if( mWannaPlay ) Stop();	// stop it first

	mOffset = 0;
	mWannaPlay = true;
	mPlayStart = GetCurrentTime();
	mMiles->TryToPlayRealSound2D( this );

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);
			mPlayLength = mLoopCount*playOnceTime;
		}
		else
			mPlayLength = 0;
	}
}

void Miles2DSound::Stop( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "2DSound::Stop\n" );
	GLog.Flush();
	#endif

//	if( mFile )
//		GLog.Print( "Sound %s stopped\n", mFile->mFileName.AsChar() );

	mWannaPlay = false;
	mPlayLength = 0;
	if( mSampleIndex != -1 )
	{
		mOffset = AIL_sample_position( mMiles->m2DRealSamples[mSampleIndex] );
		// Get loop count
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_sample_loop_count( mMiles->m2DRealSamples[mSampleIndex] );
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
		mMiles->Release2DSample( mSampleIndex );
	}
	else
		mOffset = 0;
}

void Miles2DSound::Resume( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "2DSound::Resume\n" );
	GLog.Flush();
	#endif

	mWannaPlay = true;
	mMiles->TryToPlayRealSound2D( this );
	mPlayStart = GetCurrentTime();

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);
			DWORD offsetTime = mOffset*1000*8/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff)/si.bits;
			mPlayLength = mLoopCount*playOnceTime - offsetTime;
		}
		else
			mPlayLength = 0;
	}
}

void Miles2DSound::SetLoopCount( int count )
{
	mLoopCount = count;
	if( mSampleIndex != -1 )
		AIL_set_sample_loop_count( mMiles->m2DRealSamples[mSampleIndex], count );
}

int Miles2DSound::GetLoopCount( void )
{
	if( mSampleIndex != -1 )
	{
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_sample_loop_count( mMiles->m2DRealSamples[mSampleIndex] );
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
	}
	return mLoopCount;
}

bool Miles2DSound::IsPlaying( void )
{
	if( mSampleIndex != -1 )
	{
		U32 out = AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] );
		return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) );
	}
	else if( !mWannaPlay )
		return false;
	else if(
		mLoopCount == 0 )
		return true;
	else if( mPlayLength == DWORD(-1) )
		return true;
	else
	{
		DWORD t = GetCurrentTime();
//		GLog.Print("GCT: %d, PlayStart: %d, PlayLength: %d, Diff: %d, LC: %d, %s\n", t, mPlayStart, mPlayLength,
//			t-mPlayStart, mLoopCount, (( t - mPlayStart ) < mPlayLength)?"true":"false" );
		return( ( t - mPlayStart ) < mPlayLength );
	}
}

void Miles2DSound::SetVolume( float volume, float switchTime )
{
	if( volume < 0.0f ) volume = 0.0f;
	if( volume > 1.0f ) volume = 1.0f;

	if( !switchTime )
		mVolume = volume;

	mDesiredVolume = volume;
	mLastSetVolumeTime = GetCurrentTime();
	mDesiredTime = mLastSetVolumeTime + floor(switchTime*1000);

	if( !switchTime && ( mSampleIndex != -1 ) &&
		( ( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
//		AIL_set_sample_volume_pan( mMiles->m2DRealSamples[mSampleIndex], targetVolume, mMiles->DigitalEffectsPan );
		AIL_set_sample_volume_levels( mMiles->m2DRealSamples[mSampleIndex], targetVolume, targetVolume );
	}
}

void Miles2DSound::SetLowPass( float lp )
{
	if( lp < 0.0f ) lp = 0.0f;
	if( lp > 1.0f ) lp = 1.0f;

	mLowPass = lp;
	float percent = lp;
	if( !mHasAlwaysSameSpeed && ( mMiles->LowPassFrequencyPercent < lp ) )
		percent = mMiles->LowPassFrequencyPercent;

	if( ( mSampleIndex != -1 ) &&( ( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
		AIL_set_sample_low_pass_cut_off( mMiles->m2DRealSamples[mSampleIndex], percent );
}

void Miles2DSound::UseVolumeMultiplier( bool use )
{
	if( use == mUseVolumeMultiplier ) return;

	mUseVolumeMultiplier = use;

	double newToOld = use?mMiles->mVolumeMultiplier:1.0/mMiles->mVolumeMultiplier;

	if( newToOld == 1.0 ) return;

	if( ( mSampleIndex != -1 ) &&( ( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		F32 volume;
		F32 pan;
		AIL_sample_volume_pan( mMiles->m2DRealSamples[mSampleIndex], &volume, &pan );
		volume *= newToOld;
//		AIL_set_sample_volume_pan( mMiles->m2DRealSamples[mSampleIndex], volume, pan );
		AIL_set_sample_volume_levels( mMiles->m2DRealSamples[mSampleIndex], volume, volume );
	}
}

void Miles2DSound::SetAlwaysSameSpeed( bool set )
{
	mHasAlwaysSameSpeed = set;
}

void Miles2DSound::UpdateVolume( void )
{
//	if( mDesiredVolume == mVolume )
//		return;

	DWORD curTime = GetCurrentTime();

	if( curTime >= mDesiredTime )
	{
		mVolume = mDesiredVolume;
		mDesiredTime = curTime;
	}
	else
		mVolume = ((double)(curTime-mLastSetVolumeTime))/((double)(mDesiredTime-mLastSetVolumeTime))*((double)(mDesiredVolume-mVolume)) + mVolume;

	mLastSetVolumeTime = curTime;

	if( ( mSampleIndex != -1 ) &&
		( ( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_PLAYING ) ||
		( AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] ) == SMP_STOPPED ) ) )
	{
		float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
//		AIL_set_sample_volume_pan( mMiles->m2DRealSamples[mSampleIndex], targetVolume, mMiles->DigitalEffectsPan );
		AIL_set_sample_volume_levels( mMiles->m2DRealSamples[mSampleIndex], targetVolume, targetVolume );
	}
}

bool Miles2DSound::ConnectToSample( int sampleIndex )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "2DSound::ConnectToSample\n" );
	GLog.Flush();
	#endif

	Assert( mSampleIndex == -1 );
	HSAMPLE sample = mMiles->m2DRealSamples[sampleIndex];

	DWORD time = GetCurrentTime();

	if( !AIL_set_named_sample_file( sample, mFile->mFileName.AsChar(),
		mFile->mFilePointer, AIL_file_size(mFile->mFileName.AsChar()), 0 ) )
		return false;

	AILSOUNDINFO si;
#ifdef _DEBUG
	int result = AIL_WAV_info( mFile->mFilePointer, &si );
	Assert( result );
#else
	AIL_WAV_info( mFile->mFilePointer, &si );
#endif
	AIL_set_sample_playback_rate( sample, (mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff );
//	GLog.Print( "mSoundRateDiff = %f\n", mSoundRateDiff );

	float percent = (mMiles->LowPassFrequencyPercent < mLowPass)?mMiles->LowPassFrequencyPercent:mLowPass;
	if( mHasAlwaysSameSpeed ) percent = mLowPass;
	AIL_set_sample_low_pass_cut_off( sample, percent );

	mFile->mLastUsedTime = time;
	mMiles->m2DRealSounds[sampleIndex] = this;
	mSampleIndex = sampleIndex;

	// Set appropriate offset
	DWORD timeSinceStarted = time - mPlayStart;
	DWORD playOnceTime = si.samples*1000/((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);

	int loopCount = mLoopCount;
	if( loopCount > 0 )
	{
		loopCount -= ( timeSinceStarted / playOnceTime );
		if( loopCount < 1 ) loopCount = 1;			// for release, if debug still not fixed
	}

	timeSinceStarted %= playOnceTime;

	DWORD offset = timeSinceStarted*((mHasAlwaysSameSpeed?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff)/1000;

	offset *= si.bits/8;
	if( offset % (si.bits/8) != 0 ) offset--;

	AIL_set_sample_position( sample, offset );

	// Set loop count
	if( mLoopCount > 1 )
		AIL_set_sample_loop_count( sample, mLoopCount-1 );

	// Set volume
	float targetVolume = mUseVolumeMultiplier?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
//	AIL_set_sample_volume_pan( sample, targetVolume, mMiles->DigitalEffectsPan );
	AIL_set_sample_volume_levels( sample, targetVolume, targetVolume );

	mFile->PlayFile();

	// Start sample
	if( offset )
		AIL_resume_sample( sample );
	else
		AIL_start_sample( sample );
	return true;
}

void Miles2DSound::DisconnectFromSample( void )
{
	#ifdef AUDIO_DEVICE_METHODS_LOGGING
	GLog.Print( "2DSound::DisconnectFromSample\n" );
	GLog.Flush();
	#endif

	Assert( mSampleIndex != -1 );

	HSAMPLE sample = mMiles->m2DRealSamples[mSampleIndex];

	mFile->StopFile();

	// Stop sample
	AIL_stop_sample( sample );

	mMiles->m2DRealSounds[mSampleIndex] = 0;
	mSampleIndex = -1;
}

// ---------- Audio Device methods ---------------------------
AudioDevice::AudioDevice() : mMiles(0), mCurrentFalloffSpeed(0.5f)
{
}

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

    m_setTag("AudioMiles");
	mMiles = new MilesEngine();
	mMiles->DigitalDriver = AIL_open_digital_driver( 44100, 16, 2, 0 );

    return TRUE;
}

StreamingSound* AudioDevice::CreateStreamingSound( const char* name )
{
	if( !mMiles || !mMiles->DigitalDriver )
		return NULL;

	HSTREAM s = AIL_open_stream( mMiles->DigitalDriver, name, 0 );
	if( s == NULL )
	{
		GLog.Print( "Ambient stream open error: %s\n", AIL_last_error() );
		return NULL;
	}

	// Find free index
	int sIndex = -1;
	if( mMiles->mStreamsFill.Size() )
	{
		sIndex = mMiles->mStreamsFill[ mMiles->mStreamsFill.Size()-1 ];
		mMiles->mStreamsFill.RemoveLast();
	}
	else
	{
		sIndex = mMiles->mStreams.Size();
		mMiles->mStreams.Add(NULL);
		mMiles->mStreamNames.Add("");
		mMiles->mStreamLowPass.Add(mMiles->mLowPassForNewSounds);
		mMiles->mStreamSpeed.Add(0);
	}

	mMiles->mStreams[sIndex] = s;
	mMiles->mStreamNames[sIndex] = name;
	mMiles->mStreamLowPass[sIndex] = mMiles->mLowPassForNewSounds;

    m_setTag("Audio");
    StreamingSound* sound = new StreamingSound( sIndex, mMiles );
	sound->SetVolumeAndPan( mMiles->StreamingVolume, mMiles->StreamingPan );
	AIL_set_stream_user_data(s,0,(int)sound);	// remember last stream pointer in a stream
	S32 rate = AIL_stream_playback_rate( s );
	mMiles->mStreamSpeed[sIndex] = rate;

	if( mMiles->Speed != 1.0f )
		AIL_set_stream_playback_rate( s, rate*mMiles->Speed );

	float percent = (mMiles->LowPassFrequencyPercent > mMiles->mLowPassForNewSounds)?mMiles->mLowPassForNewSounds:mMiles->LowPassFrequencyPercent;
	AIL_set_stream_low_pass_cut_off( s, percent );

	return sound;
}

void AudioDevice::GetStreamNames( DynamicArray<String>& names )
{
	names = mMiles->mStreamNames;
}

void AudioDevice::StartStreams( DynamicArray<String>& names )
{
	for( int i = 0; i < names.Size(); ++i )
	{
		if( !( names[i].IsNull()) )
		{
			StreamingSound* channel = GEngine->Audio->CreateStreamingSound(names[i].AsChar());
			if (channel)
			{
				channel->SetLoopCount(0);
				channel->Play();
			}
		}
	}
}

void AudioDevice::DeleteStreamingSound( StreamingSound* sound )
{
	delete sound;
}

void AudioDevice::Delete3DSound( Sound3D* sound )
{
	delete sound;
}

float AudioDevice::GetMasterVolumeLevel( void )
{
	return ( mMiles && mMiles->DigitalDriver )?AIL_digital_master_volume_level( mMiles->DigitalDriver ):1.0f;
}

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
	if( !mMiles	|| !mMiles->DigitalDriver ) return;
	MilesLoadedFile* file = mMiles->PrepareSample( name, playAtOnce );
	if( file )
		mMiles->Play2DSound( file, volume );
}

void AudioDevice::GetDigitalEffectsPan( float& pan )
{
	if( !mMiles || !mMiles->DigitalDriver )
		return;
	pan = mMiles->DigitalEffectsPan;
}

void AudioDevice::SetDigitalEffectsPan( float pan )
{
	if( !mMiles ) return;
	mMiles->SetDigitalEffectsPan( pan );
}

void AudioDevice::GetStreamingVolumeAndPan( float& volume, float& pan )
{
	volume = mMiles->StreamingVolume;
	pan = mMiles->StreamingPan;
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

float AudioDevice::Get3DDigitalEffectsVolume( void )
{
	float currMas = AIL_digital_master_volume_level( mMiles->DigitalDriver );
	return mMiles->DigitalEffectsVolume3D / currMas;
}

inline void AudioDevice::Set3DDigitalEffectsVolume( float volume )
{
	if( !mMiles ) return;
	mMiles->Set3DDigitalEffectsVolume( volume );
}

Sound3D* AudioDevice::Create3DSound( const char* name, bool playAtOnce )
{
	if( !mMiles || !mMiles->DigitalProvider )
		return NULL;

	MilesLoadedFile* file = mMiles->PrepareSample( name, playAtOnce );
	if( !file )
		return NULL;

    m_setTag("Audio");
	Sound3D* sound = new Sound3D( file, mMiles );
	mMiles->m3DSounds.Add( sound->mSound );
	return sound;
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

bool AudioDevice::Set3DSoundProvider( int providerIndex )
{
	if( !mMiles )
		return false;
	return mMiles->Set3DSoundProvider( providerIndex, mCurrentFalloffSpeed );
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

int AudioDevice::GetNumberOfProviders( void )
{
	return mMiles?mMiles->NumOfProviders():0;
}

String AudioDevice::GetProviderName( int providerIndex )
{
	return mMiles?mMiles->mProviderNames[providerIndex]:String("");
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

inline void AudioDevice::Tick( float timeDelta )
{
	if (!mMiles) return;
	mMiles->Tick( timeDelta );
}

void AudioDevice::SetPlayerPosition( Vector& position )
{
	if( mMiles && mMiles->Listener )
	{
//		float x,y,z;
//		AIL_3D_position( mMiles->Listener, &x, &y, &z );
//		z *= -1.0f;
//		if( x != position.X || y != position.Y || z != position.Z )
			AIL_set_3D_position( mMiles->Listener, position.X, position.Y, -position.Z );
	}
}

Vector AudioDevice::GetPlayerPosition( void )
{
	Vector pos;
	if( mMiles && mMiles->Listener )
	{
		AIL_3D_position( mMiles->Listener, &pos.X, &pos.Y, &pos.Z );
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
//		Vector vel = GetPlayerVelocity();
//		float magn2 = vel.Magnitude();
//		vel.Normalize();
//		if( !( vel == velocity ) || magn != magn2 )
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

int AudioDevice::GetCurrentSoundProvider( void )
{
	return mMiles?mMiles->mCurrentSoundProvider:-1;
}

Sound2D* AudioDevice::Create2DSound( const char* name, bool playAtOnce )
{
	if( !mMiles || !mMiles->DigitalDriver )
		return 0;

	MilesLoadedFile* file = mMiles->PrepareSample( name, playAtOnce );
	if( !file )
		return NULL;

    m_setTag("Audio");
	Sound2D* sound = new Sound2D( file, mMiles );
	mMiles->m2DSounds.Add( sound->mSound );
	return sound;
}

void AudioDevice::Delete2DSound( Sound2D* sound )
{
	delete sound;
}

void AudioDevice::SetFalloffSpeed( float speed )
{
	if( !mMiles || !mMiles->DigitalDriver )
		return;

	Assert( mMiles );
	Assert( mMiles->DigitalProvider );
	AIL_set_3D_rolloff_factor( mMiles->DigitalProvider, speed );
	mCurrentFalloffSpeed = speed;
}

float AudioDevice::GetFalloffSpeed( void )
{
	Assert( mMiles );
	if( mMiles->DigitalProvider )
		return AIL_3D_rolloff_factor( mMiles->DigitalProvider );
	else
		return mCurrentFalloffSpeed;
}

#ifdef TRANSGAMING
void AudioDevice::SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay )
#else
void AudioDevice::SetSoundProperties( String soundName, int maximalInstances, int minimalDelay )
#endif
{
	if (!mMiles) return;
	mMiles->SetSoundProperties( soundName, maximalInstances, minimalDelay );
}


void AudioDevice::SetSpeed( float speed, float switchTime ) { if (!mMiles) return; mMiles->SetSpeed( speed, switchTime ); };
void AudioDevice::SetLowPass( float percent ) { if (!mMiles) return; mMiles->SetLowPass( percent ); };
void AudioDevice::SetAllSoundsToLowPass( float percent ) { if (!mMiles) return; mMiles->SetAllSoundsToLowPass( percent ); };
void AudioDevice::SetAllSoundsVolumeMultiplier( float percent ) { if (!mMiles) return; mMiles->SetAllSoundsVolumeMultiplier( percent ); };
void AudioDevice::SetSpeakersFlip( bool flipped ) { if (!mMiles) return; mMiles->mSpeakersFlip = flipped; };
void AudioDevice::SetSoundSpeedRandomizer( float value )
{
	if (!mMiles) return;
	value = fabs( value );
	if( value >= 0.99f ) value = 0.99f;
	mMiles->mSoundRateDiffRate = value;
};
void AudioDevice::SetDopplerFactor( float factor ) { if( !mMiles ) return; mMiles->SetDopplerFactor( factor ); };

// ------------------------------ Sound3D methods -----------------------------------

Sound3D::Sound3D( MilesLoadedFile* file, MilesEngine* miles ) {
    m_setTag("Audio");

    mSound = new Miles3DSound( file, miles, this );
};

Sound3D::~Sound3D( void ) { if( mSound ) { mSound->mExternalSound = 0; delete mSound; }; };
inline void Sound3D::PlayAndForget( void ) { mSound->deleteIfStopped = true; Play(); };
inline void Sound3D::Forget( void ) { mSound->deleteIfStopped = true; };
inline float Sound3D::GetVolume( void ) { return mSound->mVolume; };
inline void Sound3D::SetObstructed( bool obstruct ) { mSound->mObstructed = obstruct; };
inline bool Sound3D::IsObstructed( void ) { return mSound->mObstructed; };
inline void Sound3D::Play( void ) { mSound->Play(); };
inline void Sound3D::Stop( void ) { mSound->Stop(); };
inline void Sound3D::Resume( void ) { mSound->Resume(); };
inline void Sound3D::SetLoopCount( int count ) { mSound->SetLoopCount( count ); };
inline int Sound3D::GetLoopCount( void ) { return mSound->GetLoopCount(); };
inline void Sound3D::SetHearingDistance( float minDist, float maxDist ) { mSound->SetHearingDistance( minDist, maxDist ); };
inline void Sound3D::GetHearingDistance( float& minDist, float& maxDist ) { mSound->GetHearingDistance( minDist, maxDist ); };
inline void Sound3D::SetPosition( Vector& position ) { mSound->SetPosition( position ); };
inline Vector Sound3D::GetPosition( void ) { return mSound->GetPosition(); };
inline void Sound3D::SetOrientation( Vector& front, Vector& up ) { mSound->SetOrientation( front, up ); };
inline void Sound3D::GetOrientation( Vector& front, Vector& up ) { mSound->GetOrientation( front, up ); };
inline void Sound3D::SetVelocity( Vector& velocity ) { mSound->SetVelocity( velocity ); };
inline Vector Sound3D::GetVelocity( void ) { return mSound->GetVelocity(); };
inline bool Sound3D::IsPlaying( void ) { return mSound->IsPlaying(); };
inline void Sound3D::SetVolume( float volume, float switchTime ) { mSound->SetVolume( volume, switchTime ); };
inline void Sound3D::SetLowPass( float lp ) { mSound->SetLowPass( lp ); };
inline float Sound3D::GetLowPass( void ) { return mSound->mLowPass; };
inline void Sound3D::UseVolumeMultiplier( bool use ) { mSound->UseVolumeMultiplier( use ); };
inline bool Sound3D::UsingVolumeMultiplier( void ) { return mSound->mUseVolumeMultiplier; };
inline void Sound3D::SetAlwaysSameSpeed( bool set ) { mSound->SetAlwaysSameSpeed( set ); };
inline bool Sound3D::HasAlwaysSameSpeed( void ) { return mSound->mHasAlwaysSameSpeed; };
inline void Sound3D::NoSpeedRandomize( void ) { mSound->mSoundRateDiff = 1.0f; };

// ------------------------------ Sound2D methods -----------------------------------

Sound2D::Sound2D( MilesLoadedFile* file, MilesEngine* miles ) {
    m_setTag("Audio");

    mSound = new Miles2DSound( file, miles, this );
};

Sound2D::~Sound2D( void ) { if( mSound ) { mSound->mExternalSound = 0; delete mSound; }; };
inline void Sound2D::PlayAndForget( void ) { mSound->deleteIfStopped = true; Play(); };
inline void Sound2D::Forget( void ) { mSound->deleteIfStopped = true; };
inline float Sound2D::GetVolume( void ) { return mSound->mVolume; };
inline void Sound2D::Play( void ) { mSound->Play(); };
inline void Sound2D::Stop( void ) { mSound->Stop(); };
inline void Sound2D::Resume( void ) { mSound->Resume(); };
inline void Sound2D::SetLoopCount( int count ) { mSound->SetLoopCount( count ); };
inline int Sound2D::GetLoopCount( void ) { return mSound->GetLoopCount(); };
inline bool Sound2D::IsPlaying( void ) { return mSound->IsPlaying(); };
inline void Sound2D::SetVolume( float volume, float switchTime ) { mSound->SetVolume( volume, switchTime ); };
inline void Sound2D::SetLowPass( float lp ) { mSound->SetLowPass( lp ); };
inline float Sound2D::GetLowPass( void ) { return mSound->mLowPass; };
inline void Sound2D::UseVolumeMultiplier( bool use ) { mSound->UseVolumeMultiplier( use ); };
inline bool Sound2D::UsingVolumeMultiplier( void ) { return mSound->mUseVolumeMultiplier; };
inline void Sound2D::SetAlwaysSameSpeed( bool set ) { mSound->SetAlwaysSameSpeed( set ); };
inline bool Sound2D::HasAlwaysSameSpeed( void ) { return mSound->mHasAlwaysSameSpeed; };
inline void Sound2D::NoSpeedRandomize( void ) { mSound->mSoundRateDiff = 1.0f; };

// ------------------------------ StreamingSound methods -------------------------------------

StreamingSound::~StreamingSound( void )
{
	if( mInternalIndex != -1 )
	{
		mMiles->ReleaseStream( mInternalIndex );
		mInternalIndex = -1;
	}
}

void StreamingSound::Play( void )
{
	AIL_start_stream( mMiles->mStreams[mInternalIndex] );
}

void StreamingSound::SetVolumeAndPan( float volume, float pan )
{
	AIL_set_stream_volume_pan( mMiles->mStreams[mInternalIndex], volume*mMiles->mVolumeMultiplier, pan );
}

void StreamingSound::GetVolumeAndPan( float& volume, float& pan )
{
	AIL_stream_volume_pan( mMiles->mStreams[mInternalIndex], &volume, &pan );
	volume /= mMiles->mVolumeMultiplier;
}

void StreamingSound::Stop( void )
{
	AIL_pause_stream( mMiles->mStreams[mInternalIndex], 1 );
}

void StreamingSound::Resume( void )
{
	AIL_pause_stream( mMiles->mStreams[mInternalIndex], 0 );
}

void StreamingSound::SetLoopCount( int count )
{
	AIL_set_stream_loop_count( mMiles->mStreams[mInternalIndex], count );
}

int StreamingSound::GetLoopCount( void )
{
	return AIL_stream_loop_count( mMiles->mStreams[mInternalIndex] );
}

bool StreamingSound::IsPlaying( void )
{
	U32 out = AIL_stream_status( mMiles->mStreams[mInternalIndex] );
	return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) );
}

void StreamingSound::SetLowPass( float lp )
{
	mMiles->mStreamLowPass[mInternalIndex] = lp;
	float percent = ( mMiles->LowPassFrequencyPercent > lp )?lp:mMiles->LowPassFrequencyPercent;
	AIL_set_stream_low_pass_cut_off( mMiles->mStreams[mInternalIndex], percent );
}

float StreamingSound::GetLowPass( void )
{
	return mMiles->mStreamLowPass[mInternalIndex];
}

#ifdef AUDIO_DEVICE_METHODS_LOGGING
#undef AUDIO_DEVICE_METHODS_LOGGING
#endif