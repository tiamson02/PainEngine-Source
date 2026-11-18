
#include "Engine.h"
#include "MilesEngine.h"
#include "MilesLoadedFile.h"
#include "Miles3DSound.h"
#include "Miles2DSound.h"

#include "rad_iop.h"
#include "sifdev.h"
#include "sifrpc.h"
#include "shellCore.h"

const char * soundFileFormatStr = "../Data/Sounds/%s-adpcm.wav";

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Physics/PhysicsWorld.h"	// for HitData

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"

#define MSS_REDIST_DIRNAME ".\\Miles\\"

extern int TickEngineList;

#include "Render/Video.h"
#include "Engine/PainMenu.h"

#define MILES_USES_OUR_FILE_METHODS

#ifdef MILES_USES_OUR_FILE_METHODS
//#define MILES_CALLBACKS_USE_CRITICAL_SECTIONS

#include "FileSystem/GFileManager.h"
extern ENGINE_API FileManagerStd GFileManAudio;

#include "Audio/AudioRecording.h"
//#define AUDIO_RECORD_STREAMS

#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
CRITICAL_SECTION ourMilesCS;
#endif

U32 AILCALLBACK PK_AIL_file_open_callback( char const* filename, U32* coercedGFilePtrOut )
{
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::EnterCriticalSection( &ourMilesCS );
#endif
	GFile* file = GFileManAudio.CreateFileReader( filename );
	if( !file )
	{
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::LeaveCriticalSection( &ourMilesCS );
#endif
		return 0;
	}
    GFileManAudio.DetachFile(file);
	*coercedGFilePtrOut = reinterpret_cast< U32 >(file);
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::LeaveCriticalSection( &ourMilesCS );
#endif
	return 1;
}

void AILCALLBACK PK_AIL_file_close_callback( U32 coercedGFilePtr )
{
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::EnterCriticalSection( &ourMilesCS );
#endif
	GFile* file = reinterpret_cast< GFile* >(coercedGFilePtr);
	delete file;
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::LeaveCriticalSection( &ourMilesCS );
#endif
}

S32 AILCALLBACK PK_AIL_file_seek_callback( U32 file_handle, S32 offset, U32 type )
{
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::EnterCriticalSection( &ourMilesCS );
#endif
	GFile* file = (GFile*)file_handle;
	int origin;

	switch( type )
	{
	default:
		TRACE( "Critical error: Miles gave us unknown origin for file seek\n" );
	case AIL_FILE_SEEK_BEGIN: origin = SEEK_SET; break;
	case AIL_FILE_SEEK_CURRENT: origin = SEEK_CUR; break;
	case AIL_FILE_SEEK_END: origin = SEEK_END; break;
	}

	if( file->Seek( offset, origin ) == -1)
	{
		TRACE( "Critical error: Miles file seek failure!\n" );
		file->Seek(0);
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
		::LeaveCriticalSection( &ourMilesCS );
#endif
		return 0;
	}
	S32 tell = file->Tell();
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::LeaveCriticalSection( &ourMilesCS );
#endif
	return tell;
}

U32 AILCALLBACK PK_AIL_file_read_callback( U32 file_handle, void* buffer, U32 bytes )
{
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::EnterCriticalSection( &ourMilesCS );
#endif
	GFile* file = (GFile*)file_handle;
	U32 res = file->Read( buffer, bytes );
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::LeaveCriticalSection( &ourMilesCS );
#endif
	return res;
}

#endif

void *ME_malloc(size_t Size)
{
	m_setTag("AudioMiles");
	return(malloc(Size));
}

void ME_free(void *ptr)
{
	free(ptr);
}


MilesEngine::MilesEngine( void ) : DigitalDriver(0), DigitalProvider(0), UsingEAX(false),
		CurrentRoomType(ENVIRONMENT_GENERIC), mMaxFiles(4), mCurrentSoundProvider(-1),
		DigitalEffectsVolume3D(1.0f), StreamingVolume(1.0f), mMax3DSamples(-1),
		mMax2DSamples(-1), DontStartNewSounds( false ), mFilledFiles( 0 ), mDefaultDelay( 0 ),
		mDefaultInstances( 100 ), Speed( 1.0f ), LowPassFrequencyPercent( 1.0f ),
		mDesiredSpeed(1.0f), mLastSetSpeedTime(0), mDesiredSpeedTime(0), mLowPassForNewSounds(1.0f),
		mVolumeMultiplier(1.0f), mSpeakersFlip(false), mSoundRateDiffRate(0.0f), m2DSoundNumbers(0),
		m3DSoundNumbers(0), SaveGameSoundList(0), mCurrentFalloffSpeed(0.5f), mInitialized(false),
		Current3DIntensity(1.0f), Current2DIntensity(1.0f), StartManyAtOnce(false), mObstructedByDefault(false),
		HeightenObstructionTime(0.0f), LowerObstructionTime(0.0f), MinObstruction(0.5f),
		MaxObstructionLow(1.0f), MaxObstructionHigh(1.0f), MinObstructionDistance(14.0f), MaxObstructionDistance(100.0f),
		mAudioRecorder(0)
{
#ifndef TRANSGAMING_PS2
	AIL_set_redist_directory( MSS_REDIST_DIRNAME );
#endif

#ifdef TRANSGAMING_PS2
  #define CORE     RAD_IOP_NEED_CORE1                    // Use core 1 for normal analog output

	// Tell the Miles SDK which functions to use for its memory management needs.
	// @note If you replace these with custom allocators, note that the returned
	// memory must be aligned on a multiple of 16 bytes!

	//+DDP - override memory allocator so MEMORY_TEST tracker can be used
	AIL_mem_use_malloc( ME_malloc );
	AIL_mem_use_free( ME_free );


	// Make sure that all modules we depend on are loaded.
	sceSifInitRpc(0);


	int loadResult = -1;
    const char * strFormatLoadError = "@sound Error: could not load '%s.irx'\n";

	loadResult = shellFileLoadStartModule("libsd", 0, "", NULL);
	if (loadResult < 0 )
	{
			TRACE( strFormatLoadError, "libsd" );
	}

	loadResult = shellFileLoadStartModule("sio2man", 0, "", NULL);
	if (loadResult < 0 )
	{
			TRACE( strFormatLoadError, "sio2man" );
	}

	// init IOP heap
	sceSifInitIopHeap();

	// Initialize the RAD IOP module

	// allocate some IOP memory for MSS
	void *iop_addr = sceSifAllocIopHeap( RAD_IOPMemoryAmount( CORE ) );

	// start up the RAD file/sound IOP module

	if (!RAD_IOPStartUp( 1, -1, iop_addr, CORE ) )
	{
		sceSifFreeIopHeap( iop_addr );
		TRACE("@sound Error: RAD_IOPStartUp() failed.\n");
		return;
	}

	// use core1's pcm channel for Miles
	RAD_IOPHardwareVolumes( 1, 0x3fff, 0x3fff, 0x7fff, 0x7fff );

	// turn on core0 for spu sounds
	RAD_IOPHardwareVolumes( 0, 0x3fff, 0x3fff, 0, 0 );

	// Initalize the optional MP3 playback module
//	Register_RIB( MP3Dec );

#endif

	if( AIL_startup() )
	{
		TRACE( "@audio Initialized audio with AIL_startup()\n" );
	}
	else
	{
		TRACE( "@audio Warning: AIL_startup() reports audio already initialized.\n" );
	};

#ifdef MILES_USES_OUR_FILE_METHODS
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::InitializeCriticalSection( &ourMilesCS );
#endif
	AIL_set_file_callbacks( PK_AIL_file_open_callback, PK_AIL_file_close_callback, PK_AIL_file_seek_callback, PK_AIL_file_read_callback );
#endif

	Enumerate3DSoundProviders();
	DigitalDriver = AIL_open_digital_driver( 44100, 16, MSS_MC_STEREO, 0 );
	if( !DigitalDriver )
	{
		TRACE( "@audio AIL_open_digital_driver failed with error: '%s'\n", AIL_last_error() );
		AIL_shutdown();
		return;
	}

	TRACE( "-------------------------------------------------------\n" );
	TRACE( "*                        Miles Engine Settings        *\n" );
	TRACE( "-------------------------------------------------------\n" );
	TRACE( "DIG_MAX_PREDELAY_MS: %d ms\n", AIL_get_preference( DIG_MAX_PREDELAY_MS ) );
	TRACE( "DIG_ENABLE_RESAMPLE_FILTER: %s\n", AIL_get_preference( DIG_ENABLE_RESAMPLE_FILTER )?"yes":"no" );
	TRACE( "DIG_MIXER_CHANNELS: %d\n", AIL_get_preference( DIG_MIXER_CHANNELS ) );
	TRACE( "DIG_RESAMPLING_TOLERANCE: %f%% (%d/65536)\n", ((float)AIL_get_preference( DIG_RESAMPLING_TOLERANCE ))/65536.0f, AIL_get_preference( DIG_RESAMPLING_TOLERANCE ) );
#ifdef TRANSGAMING_WINDOWS
	TRACE( "DIG_USE_WAVEOUT: %s\n", AIL_get_preference( DIG_USE_WAVEOUT )?"yes":"no" );
	TRACE( "DIG_OUTPUT_BUFFER_SIZE: %d bytes\n", AIL_get_preference( DIG_OUTPUT_BUFFER_SIZE ) );
	TRACE( "DIG_DS_USE_PRIMARY: %s\n", AIL_get_preference( DIG_DS_USE_PRIMARY )?"yes":"no" );
#endif
	TRACE( "DIG_DS_FRAGMENT_SIZE: %d ms\n", AIL_get_preference( DIG_DS_FRAGMENT_SIZE ) );
	TRACE( "DIG_DS_FRAGMENT_CNT: %d fragments\n", AIL_get_preference( DIG_DS_FRAGMENT_CNT ) );
#ifdef TRANSGAMING_WINDOWS
	TRACE( "DIG_DS_MIX_FRAGMENT_CNT: %d fragments\n", AIL_get_preference( DIG_DS_MIX_FRAGMENT_CNT ) );
#endif
	TRACE( "-------------------------------------------------------\n" );
	TRACE( "AIL_TIMERS: %d\n", AIL_get_preference( AIL_TIMERS ) );
#ifdef TRANSGAMING_WINDOWS
	TRACE( "AIL_ENABLE_MMX_SUPPORT: %s\n", AIL_get_preference( AIL_ENABLE_MMX_SUPPORT )?"yes":"no" );
#endif
	TRACE( "AIL_MM_PERIOD: %d ms\n", AIL_get_preference( AIL_MM_PERIOD ) );
	TRACE( "AIL_MUTEX_PROTECTION: %s\n", AIL_get_preference( AIL_MUTEX_PROTECTION )?"yes":"no" );
	TRACE( "AIL_LOCK_PROTECTION: %s\n", AIL_get_preference( AIL_LOCK_PROTECTION )?"yes":"no" );
	TRACE( "-------------------------------------------------------\n" );

	/*
	AIL_set_preference( AIL_MUTEX_PROTECTION, 0 );
	AIL_set_preference( DIG_DS_MIX_FRAGMENT_CNT, 96 );
	AIL_serve();
	*/

	mInitialized = true;
}

MilesEngine::~MilesEngine( void )
{
	if( !mInitialized ) return;

	ResetFlagsFromPauseStack();
	SaveGameSoundList = -1;

	// Turn off reverbs so they don't stay on SB Audigy...hopefully
	SetRoom(0);
	SetRoomIntensity2D(0,0);
	SetRoomIntensity3D(0,0);

	DontStartNewSounds = true;

	// Delete 3D sounds
	while( m3DSounds.Size() )
	{
		if( !( m3DSounds[0]->Flags & Miles3DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 3D - '%s'\n", m3DSounds[0]->mFile->mFileName.AsChar() );
		delete m3DSounds[0];
	}

	Assert( m3DRealSamples.Size() == 0 );

	// Delete 2D sounds
	while( m2DSounds.Size() )
	{
		if( !(m2DSounds[0]->Flags & Miles2DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 2D - '%s'\n", m2DSounds[0]->mFile->mFileName.AsChar() );
		delete m2DSounds[0];
	}

	Assert( m2DRealSamples.Size() == 0 );

	// Free files in memory
	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		delete mLoadedFiles[i];
	mLoadedFiles.ClearArray();
	mLoadedFilesHash.Clear();

	// Close streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid )
		{
			AIL_close_stream( mStreams[i].Stream );
#ifdef MILES_STREAM_DEBUG
			TRACE( "Close stream %d\n", i );
#endif
		}
	mStreams.ClearArray();
	mStreamsFill.ClearArray();

	Release3DSoundProvider();

	// Dispose of digital driver
	if( DigitalDriver )
	{
		AIL_close_digital_driver( DigitalDriver );
		DigitalDriver = 0;
	}

#ifdef MILES_USES_OUR_FILE_METHODS
#ifdef MILES_CALLBACKS_USE_CRITICAL_SECTIONS
	::DeleteCriticalSection( &ourMilesCS );
#endif
#endif

	AIL_shutdown();
}

void MilesEngine::Clear(void)
{
	// Delete 3D sounds
	while( m3DSounds.Size() )
	{
		if( !( m3DSounds[0]->Flags & Miles3DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 3D - '%s'\n", m3DSounds[0]->mFile->mFileName.AsChar() );
		delete m3DSounds[0];
	}

	Assert( m3DRealSamples.Size() == 0 );

	// Delete 2D sounds
	while( m2DSounds.Size() )
	{
		if( !(m2DSounds[0]->Flags & Miles2DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 2D - '%s'\n", m2DSounds[0]->mFile->mFileName.AsChar() );
		delete m2DSounds[0];
	}

	Assert( m2DRealSamples.Size() == 0 );

	// Free files in memory
	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		delete mLoadedFiles[i];
	mLoadedFiles.ClearArray();
	mLoadedFilesHash.Clear();
}

void MilesEngine::Enumerate3DSoundProviders( void )
{
	// 3D providers are no longer supported in Miles 7+
/*
	HPROENUM next = HPROENUM_FIRST;

	HPROVIDER id;
	char* name;

	while( AIL_enumerate_3D_providers( &next, &id, &name ) )
	{
		mProviders.Add( id );
		mProviderNames.Add( String(name) );
	}

	if( !mProviders.Size() )
		TRACE( "Audio Warning: No 3D sound providers in the system!!\n" );
*/
}

bool MilesEngine::Set3DSoundProvider( int providerIndex )
{
//	DWORD result;

	// Close the opened provider, if it exists
//	Release3DSoundProvider();

//	if( providerIndex < 0 )
//		return false;

	// Load the new provider
//	result = AIL_open_3D_provider( mProviders[providerIndex] );

//	if( result != M3D_NOERR )
//	{
#ifdef AUDIO_WARNINGS_ON
//		TRACE("Warning: AIL_open_3D_provider() error\n");
#endif
//		mCurrentSoundProvider = -1;
//		return false;
//    }
//    else
//    {
		DontStartNewSounds = false;
//		DigitalProvider = mProviders[ providerIndex ];
		Assert( mCurrentFalloffSpeed >= 0.0f );
		AIL_set_3D_rolloff_factor( DigitalDriver, mCurrentFalloffSpeed );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Set 3D rolloff factor in Set3DSoundProvider to: %f\n", mCurrentFalloffSpeed );
#endif

		// See if we're running under an EAX compatible provider
//		result = AIL_3D_room_type( DigitalProvider );
//		UsingEAX = (((S32)result)!=-1)?true:false;			// will be something other than -1 on EAX
		UsingEAX = false;

		// Create listener
//		Listener = AIL_open_3D_listener( DigitalProvider );
//		Assert(Listener);
#ifdef MILES_3DSOUND_DEBUG
//		TRACE( "Frame %d - ", GEngine->World->Frame );
//		TRACE( "%s\n", Listener?"3D listener opened\n":"3D listener opening error" );
#endif

//		if( UsingEAX )
//		{
//			Assert( ( CurrentRoomType >= 0 ) && ( CurrentRoomType <= ENVIRONMENT_PSYCHOTIC ) );
//			AIL_set_3D_room_type( DigitalProvider, CurrentRoomType );
#ifdef MILES_3DSOUND_DEBUG
//			TRACE( "Frame %d - ", GEngine->World->Frame );
//			TRACE( "3D room type set to %d\n", CurrentRoomType );
#endif
//		}
//	}
//	mCurrentSoundProvider = providerIndex;
	return true;
}

void MilesEngine::Release3DSoundProvider( void )
{
	DontStartNewSounds = true;

	// Release playing 3D sounds
//	while( m3DRealSounds.Size() )
//		Release3DSample( 0, 0 );

	// Release listener, if we have one
//	if( Listener )
//	{
//		AIL_close_3D_listener( Listener );
#ifdef MILES_3DSOUND_DEBUG
//		TRACE( "Frame %d - ", GEngine->World->Frame );
//		TRACE( "3D listener closed\n" );
#endif
//		Listener = 0;
//	}

//	if( DigitalProvider != 0 )
//	{
//		AIL_close_3D_provider( DigitalProvider );
#ifdef MILES_3DSOUND_DEBUG
//		TRACE( "Frame %d - ", GEngine->World->Frame );
//		TRACE( "3D provider closed\n" );
#endif
		DigitalProvider = 0;
		mCurrentSoundProvider = -1;
//	}
}

MilesLoadedFile* MilesEngine::PrepareSample( const char* name )
{
	// Check if file exists
	MilesLoadedFile* file = GetFileByName( name );
	if( file )
	{
		file->AddReference();
		return file;
	}

	// File does not exist - take it.
	file = GetEmptyFileByName( name );
	if( !file )
	{
        m_setTag("AudioMiles");
		file = new MilesLoadedFile( name, mDefaultInstances, mDefaultDelay );
		int fIndex = mLoadedFiles.Add( file );
		mLoadedFilesHash.Insert( file->mFileName, fIndex );
	}

	// Load new file
	void* fptr = LoadAndUnpackFile( name );
	if( fptr == NULL )
	{
		TRACE( "Error: Couldn't load audio file: %s\n", name );
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
	file->mFileSize = AIL_file_size( name );
	AIL_WAV_info( fptr, &file->mFileSoundInfo );
	mFilledFiles++;
	return file;
}

void MilesEngine::LoadFilesOffMemory( void )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::LoadFilesOffMemory );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::LoadFilesOffMemory );
#endif

	for( int i = 0; i < mLoadedFiles.Size(); ++i )
		if( !mLoadedFiles[i]->IsReferenced() )
			mLoadedFiles[i]->EmptyFile();
}

void MilesEngine::PreloadFile( const char* name )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::PreloadFile, name );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::PreloadFile, name );
#endif

	MilesLoadedFile* file = PrepareSample( name );
    if (file)
        file->RemoveReference();
}

MilesLoadedFile* MilesEngine::SureGetFile( const char* name )
{
	String strName = GetCorrectFileName( name );
	unsigned short* index = mLoadedFilesHash.Find( strName );
	if( index ) return mLoadedFiles[*index];

    m_setTag("AudioMiles");
    MilesLoadedFile* file = new MilesLoadedFile( name, mDefaultInstances, mDefaultDelay );
	int index2 = mLoadedFiles.Add( file );
	mLoadedFilesHash.Insert( file->mFileName, index2 );
	return file;
}

MilesLoadedFile* MilesEngine::GetFileByName( const char* name )
{
	String strName = GetCorrectFileName( name );
	strName.ToLower();
	unsigned short* index = mLoadedFilesHash.Find( strName );
	return( index && ( mLoadedFiles[*index]->mFilePointer != 0 ) )?mLoadedFiles[*index]:0;
}

MilesLoadedFile* MilesEngine::GetEmptyFileByName( const char* name )
{
	String strName = GetCorrectFileName( name );
	unsigned short* index = mLoadedFilesHash.Find( strName );
	return( index && ( mLoadedFiles[*index]->mFilePointer == 0 ) )?mLoadedFiles[*index]:0;
}

void* MilesEngine::LoadAndUnpackFile( const char* name )
{
	void* memFile;
	void* wavFile;
	S32 fileType;
	AILSOUNDINFO info;


    int size = GFileManAudio.FileSize(name);

    //int size = AIL_file_size( name );
    //if( size == -1 )

	if( size == 0 )
	{
		TRACE( "Audio warning: file size check failed - file '%s'\n", name );
		return 0;
	}

	memFile = AIL_file_read( name, 0 );

	if( memFile == NULL )
	{
		TRACE( "Audio warning: file read failed on file '%s' ( error %d, last audio error '%s' )\n", name, AIL_file_error(), AIL_last_error() );
		return 0;
	}

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
			HSAMPLE sample = AIL_allocate_sample_handle( DigitalDriver );
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D sample handle allocated: %d\n", (int)sample );
#endif
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
			HSAMPLE sample = AIL_allocate_sample_handle( DigitalDriver );
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D sample handle allocated: %d\n", (int)sample );
#endif
			m3DRealSamples.Add(sample);
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
			m3DRealSamples[sampleIndex] = AIL_allocate_sample_handle( DigitalDriver );
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D sample handle allocated: %d\n", (int)m3DRealSamples[sampleIndex] );
#endif
			Assert( m3DRealSamples[sampleIndex] != 0 );
		}
	}

	if( !sound->ConnectToSample( sampleIndex ) )
	{
		AIL_release_sample_handle( m3DRealSamples[sampleIndex] );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample handle released: %d\n", (int)m3DRealSamples[sampleIndex] );
#endif
		m3DRealSamples.Remove( sampleIndex );
		m3DRealSounds.Remove( sampleIndex );
		for( int i = sampleIndex; i < m3DRealSamples.Size(); ++i )
			if( m3DRealSounds[i] )
				m3DRealSounds[i]->mSampleIndex--;
		return false;
	}

	return true;
}

bool MilesEngine::Start2DSample( Miles2DSound* sound, int sampleIndex )
{
	if( sampleIndex == -1 )
	{
		if( mMax2DSamples == -1 )
		{
			HSAMPLE sample = AIL_allocate_sample_handle( DigitalDriver );
#ifdef MILES_2DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D sample handle allocated: %d\n", (int)sample );
#endif
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
				AIL_set_sample_volume_levels( m2DRealSamples[m2DRealSamples.Size()-1], 1.0f, 1.0f );
			}
		}
		else if( m2DRealSamples.Size() < mMax2DSamples )
		{
			HSAMPLE sample = AIL_allocate_sample_handle( DigitalDriver );
#ifdef MILES_2DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D sample handle allocated: %d\n", (int)sample );
#endif
			m2DRealSamples.Add(sample);
			m2DRealSounds.Add(0);
			AIL_init_sample( m2DRealSamples[m2DRealSamples.Size()-1] );
			AIL_set_sample_volume_levels( m2DRealSamples[m2DRealSamples.Size()-1], 1.0f, 1.0f );
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
#ifdef MILES_2DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D sample handle allocated: %d\n", (int)m2DRealSamples[sampleIndex] );
#endif
			Assert( m2DRealSamples[sampleIndex] != 0 );
			AIL_init_sample( m2DRealSamples[sampleIndex] );
			AIL_set_sample_volume_levels( m2DRealSamples[sampleIndex], 1.0f, 1.0f );
		}
	}

	if( !sound->ConnectToSample( sampleIndex ) )
	{
		AIL_release_sample_handle( m2DRealSamples[sampleIndex] );
#ifdef MILES_2DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "2D sample handle released: %d\n", (int)m2DRealSamples[sampleIndex] );
#endif
		m2DRealSamples.Remove( sampleIndex );
		m2DRealSounds.Remove( sampleIndex );
		for( int i = sampleIndex; i < m2DRealSamples.Size(); ++i )
			if( m2DRealSounds[i] )
				m2DRealSounds[i]->mSampleIndex--;
		return false;
	}

	return true;
}

void MilesEngine::Release3DSample( int sampleIndex, Miles3DSound* soundToStart, bool dontStartNew )
{
	Assert( m3DRealSamples[sampleIndex] != 0 );

	// Disconnect sound from sample
	Miles3DSound* sound = m3DRealSounds[sampleIndex];
	if( sound )
		sound->DisconnectFromSample();

	// Check what should go in its place
	Miles3DSound* newSound = dontStartNew?0:(soundToStart?soundToStart:Find3DSoundToStart());

	if( newSound )
		Start3DSample( newSound, sampleIndex );
	else
	{
		AIL_release_3D_sample_handle( m3DRealSamples[sampleIndex] );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample handle released: %d\n", (int)m3DRealSamples[sampleIndex] );
#endif
		m3DRealSamples.Remove( sampleIndex );
		m3DRealSounds.Remove( sampleIndex );
		for( int i = sampleIndex; i < m3DRealSamples.Size(); ++i )
			if( m3DRealSounds[i] )
				m3DRealSounds[i]->mSampleIndex--;
	}
}

void MilesEngine::Release2DSample( int sampleIndex, Miles2DSound* soundToStart, bool dontStartNew )
{
	Assert( m2DRealSamples[sampleIndex] != 0 );

	// Disconnect sound from sample
	Miles2DSound* sound = m2DRealSounds[sampleIndex];
	if( sound )
		sound->DisconnectFromSample();

	// Check what should go in its place
	Miles2DSound* newSound = dontStartNew?0:(soundToStart?soundToStart:Find2DSoundToStart());

	if( newSound )
		Start2DSample( newSound, sampleIndex );
	else
	{
		AIL_release_sample_handle( m2DRealSamples[sampleIndex] );
#ifdef MILES_2DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "2D sample handle released: %d\n", (int)m2DRealSamples[sampleIndex] );
#endif
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
	AIL_listener_3D_position( DigitalDriver, &playerPos.X, &playerPos.Y, &playerPos.Z );
	playerPos.Z = -playerPos.Z;
	Vector pos = sound->mPosition;
	float dist = (playerPos-pos).Magnitude();
	float vol = sound->mHearMin/dist;

	if( dist > sound->mHearMax )	// don't bother
		return;

	if( !StartManyAtOnce && !sound->mFile->CanPlayFileTime() )
		return;

	if( !sound->mFile->CanPlayFileInstances() )
	{
		float minVolume;
		int indexToStop = Find3DSoundToStop( minVolume, sound->mFile );
		if( ( indexToStop != -1 ) && ( ( minVolume == 0 ) || ( minVolume < vol - 0.1f ) ) )
			Release3DSample( indexToStop, sound );
		return;
	}

	if( !Start3DSample( sound, -1 ) )
	{
		// Couldn't start. Let's find out if we shouldn't remove some other sound.
		float minVolume;
		int indexToStop = Find3DSoundToStop( minVolume );
		if( (indexToStop != -1) && ( ( minVolume == 0 ) || ( minVolume < vol - 0.1f ) ) )
			Release3DSample( indexToStop, sound );
	}
}

void MilesEngine::TryToPlayRealSound2D( Miles2DSound* sound )
{
	if( !StartManyAtOnce && !sound->mFile->CanPlayFileTime() )
		return;

	if( !sound->mFile->CanPlayFileInstances() )
	{
		int indexToStop = GetLongestPlaying2DSoundForFile( sound->mFile );
		if( ( indexToStop != -1 ) && ( m2DRealSounds[indexToStop]->mPlayStart < sound->mPlayStart ) )
			Release2DSample( indexToStop, sound );
		return;
	}

	if( !Start2DSample( sound, -1 ) )
	{
		// Couldn't start. Let's find out if we shouldn't remove some other sound.
		int indexToStop = Find2DSoundToStop();
		if( (indexToStop != -1 ) && ( m2DRealSounds[indexToStop]->mPlayStart < sound->mPlayStart ) )
			Release2DSample( indexToStop, sound );
	}
}

// Choose least slightly heard sound ( by hearing distance )
int MilesEngine::Find3DSoundToStop( float& volume, MilesLoadedFile* file )
{
	Vector playerPos;
	AIL_listener_3D_position( DigitalDriver, &playerPos.X, &playerPos.Y, &playerPos.Z );

	int priority = 256;
	float minVolume = 10.0f;
	float maxDistance = 0;
	int farthest = -1;
	for( int i = 0; i < m3DRealSamples.Size(); ++i )
	{
		if( file && ( m3DRealSounds[i]->mFile != file ) ) continue;

		Vector pos;
		AIL_3D_position( m3DRealSamples[i], &pos.X, &pos.Y, &pos.Z );
		float dist = (playerPos-pos).Magnitude();
		if( dist > m3DRealSounds[i]->mHearMax )
		{
			if( minVolume == 0 )
			{
				int realPrio = m3DRealSounds[i]->mPriority;
				if( m3DRealSounds[i]->Flags & Miles3DSound::Privileged ) realPrio = 256;

				if( realPrio > priority ) continue;
				else if( realPrio < priority )
				{
					priority = realPrio;
					maxDistance = dist;
					farthest = i;
				}
				else
				{
					if( dist > maxDistance )
					{
						maxDistance = dist;
						farthest = i;
					}
				}
			}
			else
			{
				int realPrio = m3DRealSounds[i]->mPriority;
				if( m3DRealSounds[i]->Flags & Miles3DSound::Privileged ) realPrio = 256;

				minVolume = 0;
				priority = realPrio;
				maxDistance = dist;
				farthest = i;
			}
		}
		else if( minVolume > 0 )
		{
			int realPrio = m3DRealSounds[i]->mPriority;
			if( m3DRealSounds[i]->Flags & Miles3DSound::Privileged ) realPrio = 256;

			if( realPrio > priority ) continue;
			else if( realPrio < priority )
			{
				priority = realPrio;
				maxDistance = dist;
				farthest = i;
			}
			else
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
	}

	volume = minVolume;
	return farthest;
}

// Stop the sound that was started earliest
int MilesEngine::Find2DSoundToStop( void )
{
	DWORD time = GetCurrentTime();
	int priority = 256;
	int index = -1;
	for( int i = 0; i < m2DRealSounds.Size(); ++i )
	{
		int realPrio = m2DRealSounds[i]->mPriority;
		if( m2DRealSounds[i]->Flags & Miles2DSound::Privileged ) realPrio = 256;

		if( realPrio > priority ) continue;
		else if( realPrio < priority )
		{
			priority = realPrio;
			time = m2DRealSounds[i]->mPlayStart;
			index = i;
		}
		else if( m2DRealSounds[i]->mPlayStart < time )
		{
			time = m2DRealSounds[i]->mPlayStart;
			index = i;
		}
	}
	return index;
}

// Find the sound that was started earliest and plays from a file
int MilesEngine::GetLongestPlaying2DSoundForFile( MilesLoadedFile* file )
{
	DWORD time = GetCurrentTime();
	int priority = 256;
	int index = -1;
	for( int i = 0; i < m2DRealSounds.Size(); ++i )
	{
		if( m2DRealSounds[i]->mFile != file ) continue;
		int realPrio = m2DRealSounds[i]->mPriority;
		if( m2DRealSounds[i]->Flags & Miles2DSound::Privileged ) realPrio = 256;

		if( realPrio > priority ) continue;
		else if( realPrio < priority )
		{
			priority = realPrio;
			time = m2DRealSounds[i]->mPlayStart;
			index = i;
		}
		else if( m2DRealSounds[i]->mPlayStart < time )
		{
			time = m2DRealSounds[i]->mPlayStart;
			index = i;
		}
	}
	return index;
}

// Choose closest sound ( by hearing distance ) and return 0 if it's still beyond hearing
Miles3DSound* MilesEngine::Find3DSoundToStart( MilesLoadedFile* file )
{
	if( DontStartNewSounds )
		return 0;

	Vector playerPos;
	AIL_listener_3D_position( DigitalDriver, &playerPos.X, &playerPos.Y, &playerPos.Z );
	playerPos.Z = -playerPos.Z;

	int priority = 0;
	float maxVolume = 0.0f;
	int closest = -1;
	for( int i = 0; i < m3DSounds.Size(); ++i )
	{
		if( m3DSounds[i]->mSampleIndex == -1 )
		{
			if( m3DSounds[i]->IsPlaying() && ( m3DSounds[i]->mFile->CanPlayFileInstances() || m3DSounds[i]->mFile == file ) )
			{
				int realPrio = m3DSounds[i]->mPriority;
				if( m3DSounds[i]->Flags & Miles3DSound::Privileged ) realPrio = 256;

				if( realPrio < priority ) continue;
				else if( realPrio > priority )
				{
					float dist = (playerPos-m3DSounds[i]->mPosition).Magnitude();

					if( dist < m3DSounds[i]->mHearMax )
					{
						priority = realPrio;
						maxVolume = m3DSounds[i]->mHearMin/dist;
						closest = i;
					}
				}
				else
				{
					Vector pos = m3DSounds[i]->mPosition;
					float dist = (playerPos-pos).Magnitude();
					float vol = m3DSounds[i]->mHearMin/dist;

					if( ( dist < m3DSounds[i]->mHearMax ) && ( vol > maxVolume ) )
					{
						maxVolume = vol;
						closest = i;
					}
				}
			}
		}
	}
	return (closest == -1)?0:m3DSounds[closest];
}

// Start the sound that was started latest - chances are it will last longer
Miles2DSound* MilesEngine::Find2DSoundToStart( void )
{
	if( DontStartNewSounds )
		return 0;

	int priority = 0;
	DWORD time = 0;
	int index = -1;

	for( int i = 0; i < m2DSounds.Size(); ++i )
	{
		if( ( m2DSounds[i]->mSampleIndex == -1 ) && m2DSounds[i]->IsPlaying() )
		{
			int realPrio = m2DSounds[i]->mPriority;
			if( m2DSounds[i]->Flags & Miles2DSound::Privileged ) realPrio = 256;

			if( realPrio < priority )
				continue;
			else if( m2DSounds[i]->mFile->CanPlayFileInstances() )
			{
				if( realPrio > priority )
				{
					priority = realPrio;
					time = m2DSounds[i]->mPlayStart;
					index = i;
				}
				else if( m2DSounds[i]->mPlayStart > time )
				{
					time = m2DSounds[i]->mPlayStart;
					index = i;
				}
			}
		}
	}

	return (index == -1)?0:m2DSounds[index];
}

void MilesEngine::StreamingSound_Delete( int index )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_Delete, index );
#endif
	if( index >= 0 && index < mStreams.Size() && mStreams[index].Valid )
	{
		AIL_close_stream( mStreams[index].Stream );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Close stream %d\n", index );
#endif
		mStreams[index].Valid = false;
		mStreams[index].Name = "";
		mStreams[index].LowPass = 1.0f;
		mStreams[index].LoopCount = 1;
		mStreams[index].Speed = 0;
		mStreams[index].Volume = 1.0f;
		mStreamsFill.Add(index);
	};
}

#define SOUNDS_TO_RELEASE_IN_ONE_TICK 1024

void MilesEngine::StopFinishedSounds( void )
{
    static Miles3DSound* mSoundsToReleaseStatic[SOUNDS_TO_RELEASE_IN_ONE_TICK] = {0};
	static int mSoundsToReleaseStaticCounter = 0;

    static Miles2DSound* m2DSoundsToReleaseStatic[SOUNDS_TO_RELEASE_IN_ONE_TICK] = {0};
	static int m2DSoundsToReleaseStaticCounter = 0;

	mSoundsToReleaseStaticCounter = 0;
	m2DSoundsToReleaseStaticCounter = 0;

	// Update things for 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
	{
		if( m3DSounds[i]->mSampleIndex != -1 )
		{
			U32 status = AIL_3D_sample_status( m3DRealSamples[m3DSounds[i]->mSampleIndex] );
			switch( status )
			{
			default:
			case SMP_PLAYING:
				break;
			case SMP_STOPPED:
			case SMP_DONE:
				m3DSounds[i]->Flags &= ~Miles3DSound::WannaPlay;
				if( m3DSounds[i]->Flags & Miles3DSound::DeleteIfStopped )
				{
					Assert( mSoundsToReleaseStaticCounter < SOUNDS_TO_RELEASE_IN_ONE_TICK );
					mSoundsToReleaseStatic[mSoundsToReleaseStaticCounter++] = m3DSounds[i];
				}
				else
					Release3DSample( m3DSounds[i]->mSampleIndex );
				continue;
			}
		}

		if( !m3DSounds[i]->IsPlaying() )
		{
			m3DSounds[i]->Flags &= ~Miles3DSound::WannaPlay;
			if( m3DSounds[i]->Flags & Miles3DSound::DeleteIfStopped )
			{
				Assert( mSoundsToReleaseStaticCounter < SOUNDS_TO_RELEASE_IN_ONE_TICK );
				mSoundsToReleaseStatic[mSoundsToReleaseStaticCounter++] = m3DSounds[i];
			}
		}
	}

	for( int i = mSoundsToReleaseStaticCounter-1; i >= 0; --i )
		delete mSoundsToReleaseStatic[i];

	// Update things for 2D sounds
	for( int i = 0; i < m2DSounds.Size(); ++i )
	{
		if( m2DSounds[i]->mSampleIndex != -1 )
		{
			U32 status = AIL_sample_status( m2DRealSamples[m2DSounds[i]->mSampleIndex] );
			switch( status )
			{
			case SMP_PLAYING:
			default:
				break;
			case SMP_STOPPED:
			case SMP_DONE:
				m2DSounds[i]->Flags &= ~Miles2DSound::WannaPlay;
				if( m2DSounds[i]->Flags & Miles2DSound::DeleteIfStopped )
				{
					Assert( m2DSoundsToReleaseStaticCounter < SOUNDS_TO_RELEASE_IN_ONE_TICK );
					m2DSoundsToReleaseStatic[m2DSoundsToReleaseStaticCounter++] = m2DSounds[i];
				}
				else
					Release2DSample( m2DSounds[i]->mSampleIndex );
				continue;
			}
		}

		if( !m2DSounds[i]->IsPlaying() )
		{
			m2DSounds[i]->Flags &= ~Miles2DSound::WannaPlay;
			if( m2DSounds[i]->Flags & Miles2DSound::DeleteIfStopped )
			{
				Assert( m2DSoundsToReleaseStaticCounter < SOUNDS_TO_RELEASE_IN_ONE_TICK );
				m2DSoundsToReleaseStatic[m2DSoundsToReleaseStaticCounter++] = m2DSounds[i];
			}
		}
	}

	for( int i = m2DSoundsToReleaseStaticCounter-1; i >= 0; --i )
		delete m2DSoundsToReleaseStatic[i];
}

void MilesEngine::Tick( float timeDelta )
{
	GProfiler.SetState(AUDIO_TICK_UPDATE_SPEED);
	if( Speed != mDesiredSpeed ) UpdateSpeed();

	static int counter = 0;

	// Update things for 3D sounds
	GProfiler.SetState(AUDIO_TICK_UPDATE_3D);
	for( int i = 0; i < m3DSounds.Size(); ++i )
	{
		// Sound volume change
		if( m3DSounds[i]->mVolume != m3DSounds[i]->mDesiredVolume ) m3DSounds[i]->UpdateVolume();

		// Sound intensity change
		if( m3DSounds[i]->mCurrentIntensity != m3DSounds[i]->mDesiredIntensity ) m3DSounds[i]->UpdateIntensity();

		// Sound obstruction change
		if( ( m3DSounds[i]->Flags&Miles3DSound::Obstructed ) &&
			( m3DSounds[i]->mCurrentObstruction != m3DSounds[i]->mDesiredObstruction ) )
				m3DSounds[i]->UpdateObstruction();
	}

	// Update things for 2D sounds
	GProfiler.SetState(AUDIO_TICK_UPDATE_2D);
	for( int i = 0; i < m2DSounds.Size(); ++i )
	{
		// Fix sound volume change
		if( m2DSounds[i]->mVolume != m2DSounds[i]->mDesiredVolume )
			m2DSounds[i]->UpdateVolume();

		// Fix sound intensity change
		if( m2DSounds[i]->mCurrentIntensity != m2DSounds[i]->mDesiredIntensity )
			m2DSounds[i]->UpdateIntensity();
	}

	GProfiler.SetState(AUDIO_TICK_FLIP_3D);
	// Stop farthest sound if it's far from here
	float minVolume;
	int toStop = Find3DSoundToStop( minVolume );
	while( minVolume == 0.0f )
	{
		Miles3DSound* toStart = Find3DSoundToStart( m3DRealSounds[toStop]->mFile );
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
			TryToPlayRealSound( toStart );
			if( lastToStart == toStart )
				break;
		}
		else
			break;
		lastToStart = toStart;
	}

	// And for listener
	GProfiler.SetState(AUDIO_TICK_OBSTRUCT);
//	if( Listener )
//	{
		// Advanced 3D obstacles check
		Vector playerPos;
		AIL_listener_3D_position( DigitalDriver, &playerPos.X, &playerPos.Y, &playerPos.Z );
		playerPos.Z = -playerPos.Z;

		// So those are not really obstructed right now, just as Skorpik wanted
		counter++;
		if( counter >= m3DRealSamples.Size() )
			counter = 0;

		if( ( m3DRealSamples.Size() > counter ) &&
			( m3DRealSounds[counter]->Flags & Miles3DSound::Obstructed ) &&
			( AIL_3D_sample_status( m3DRealSamples[counter] ) == SMP_PLAYING ) )
		{
			Vector soundPos;
			HitData hitData;
			AIL_3D_position( m3DRealSamples[counter], &soundPos.X, &soundPos.Y, &soundPos.Z );
			soundPos.Z = -soundPos.Z;

			float targetObstruction;
#ifdef	TRANSGAMING
			if( GEngine->pcfWorld->Physics && GEngine->pcfWorld->Physics->LineTraceFixedGeom( playerPos, soundPos, hitData ) )	// need distance
#else
			if( GEngine->World->Physics && GEngine->World->Physics->LineTraceFixedGeom( playerPos, soundPos, hitData ) )	// need distance
#endif
			{
				float dist = (playerPos-soundPos).Magnitude();
				if( dist < MinObstructionDistance )
					targetObstruction = MaxObstructionLow;
				else if( dist > MaxObstructionDistance )
					targetObstruction = MaxObstructionHigh;
				else
					targetObstruction = MaxObstructionLow - (MaxObstructionLow - MaxObstructionHigh)*
						(dist - MinObstructionDistance)/(MaxObstructionDistance - MinObstructionDistance);
			}
			else
				targetObstruction = 0.0f;

			float currentObstruction = m3DRealSounds[counter]->mCurrentObstruction;
			m3DRealSounds[counter]->SetCurrentObstruction( targetObstruction, (targetObstruction>currentObstruction)?HeightenObstructionTime:LowerObstructionTime );
		}
//	}

	StartManyAtOnce = false;
}

void MilesEngine::Set3DDigitalEffectsVolume( float volume )
{
	if( !DigitalDriver ) return;
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Set3DDigitalEffectsVolume, volume );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Set3DDigitalEffectsVolume, volume );
#endif

	if( volume > 1.0f ) volume = 1.0f;
	if( volume < 0.0f ) volume = 0.0f;
	float currMas = AIL_digital_master_volume_level( DigitalDriver );
	Assert( ( currMas >= 0.0f ) && ( currMas <= 1.0f ) );
	volume *= currMas;
	if( DigitalEffectsVolume3D == volume ) return;
	DigitalEffectsVolume3D = volume;

	// Update digital samples
	for( int i = 0; i < m3DRealSamples.Size(); i++ )
	{
		float targetVolume = (mPrivilegedSounds.Size()&&((m3DRealSounds[i]->Flags&Miles3DSound::Privileged)==0))?DigitalEffectsVolume3D*m3DRealSounds[i]->mVolume*mVolumeMultiplier:DigitalEffectsVolume3D*m3DRealSounds[i]->mVolume;
		Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
		AIL_set_3D_sample_volume( m3DRealSamples[i], targetVolume );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D Sample %d: volume set to %f\n", (int)m3DRealSamples[i], targetVolume );
#endif
	}
}

#ifdef TRANSGAMING
void MilesEngine::SetSoundProperties( const char *soundName, int maximalInstances, int minimalDelay ){
	if (mAudioRecorder)
        mAudioRecorder->SetAction(AudioRecorder::SetSoundProperties, soundName, maximalInstances, minimalDelay);

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
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetSoundProperties, soundName.AsChar(), maximalInstances, minimalDelay );

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
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetSpeed, speed, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetSpeed, speed, switchTime );
#endif
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
		if( !(m2DSounds[i]->Flags & Miles2DSound::HasAlwaysSameSpeed ) )
		{
			if( m2DSounds[i]->mPlayStart )
			{
				m2DSounds[i]->mPlayStart = time - ( time - m2DSounds[i]->mPlayStart ) / speedChange;
				if( m2DSounds[i]->mPlayStart > time )
					m2DSounds[i]->mPlayStart = time;
			}

			if( m2DSounds[i]->mPlayLength != DWORD(-1) )
			{
				m2DSounds[i]->mPlayLength /= speedChange;
				Assert( m2DSounds[i]->mPlayLength >= 0 );
				// == 0 for sounds that have been created but never started
				Assert( ( m2DSounds[i]->mPlayLength > 0 ) || !m2DSounds[i]->mPlayStart )
			}
		}

	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !(m2DRealSounds[i]->Flags & Miles2DSound::HasAlwaysSameSpeed ) )
		{
			int SPRate = m2DRealSounds[i]->mFile->mFileSoundInfo.rate*Speed*m2DRealSounds[i]->mSoundRateDiff;
			Assert( SPRate > 0 );
			AIL_set_sample_playback_rate( m2DRealSamples[i], SPRate );
#ifdef MILES_2DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D Sample %d: speed set to %f\n", (int)m2DRealSamples[i], SPRate );
#endif
		}

	// Change speed for 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !(m3DSounds[i]->Flags & Miles3DSound::HasAlwaysSameSpeed) )
		{
			if( m3DSounds[i]->mPlayStart )
			{
				m3DSounds[i]->mPlayStart = time - ( time - m3DSounds[i]->mPlayStart ) / speedChange;
				if( m3DSounds[i]->mPlayStart > time )
					m3DSounds[i]->mPlayStart = time;
			}

			if( m3DSounds[i]->mPlayLength != DWORD(-1) )
				m3DSounds[i]->mPlayLength /= speedChange;
		}

	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !(m3DRealSounds[i]->Flags&Miles3DSound::HasAlwaysSameSpeed) )
		{
			int SPRate = m3DRealSounds[i]->mFile->mFileSoundInfo.rate*Speed*m3DRealSounds[i]->mSoundRateDiff;
			Assert( SPRate > 0 );
			AIL_set_3D_sample_playback_rate( m3DRealSamples[i], SPRate );
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D Sample %d: speed set to %f\n", (int)m3DRealSamples[i], SPRate );
#endif
		}

	// Change speed for streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid )
		{
			int SPRate = mStreams[i].Speed*Speed;
			Assert( SPRate > 0 );
			AIL_set_stream_playback_rate( mStreams[i].Stream, SPRate );
#ifdef MILES_STREAM_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "Create: set stream %d playback rate: %f\n", i, SPRate );
#endif
		}
}

void MilesEngine::SetLowPass( float percent )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetLowPass, percent );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetLowPass, percent );
#endif

	Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
	if( percent > 1.0f ) percent = 1.0f;
	if( percent < 0.2f ) percent = 0.2f;

	LowPassFrequencyPercent = percent;

	// Change frequency for 2D sounds
	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !( m2DRealSounds[i]->Flags & Miles2DSound::HasAlwaysSameSpeed ) && ( m2DRealSounds[i]->mLowPass > percent ) &&
			( m2DRealSounds[i]->mRealLowPass != percent ) )
		{
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D sample %d: set cut off (set low pass) to %f\n", m2DRealSamples[i], percent );
#endif
			m2DRealSounds[i]->mRealLowPass = percent;
			AIL_set_sample_low_pass_cut_off( m2DRealSamples[i], percent );
		}

	// Change frequency for 3D sounds
	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !(m3DRealSounds[i]->Flags&Miles3DSound::HasAlwaysSameSpeed) && ( m3DRealSounds[i]->mLowPass > percent ) &&
			( m3DRealSounds[i]->mRealLowPass != ( 1.0f - percent ) ) )
		{
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D sample %d: set occlusion (set low pass) to %f\n", m3DRealSamples[i], 1.0f - percent );
#endif
			m3DRealSounds[i]->mRealLowPass = 1.0f - percent;
			AIL_set_3D_sample_occlusion( m3DRealSamples[i], 1.0f - percent );
		}

	// Change frequency for streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid && ( mStreams[i].LowPass > percent ) )
		{
			mStreams[i].LowPass = percent;
			AIL_set_stream_low_pass_cut_off( mStreams[i].Stream, percent );
#ifdef MILES_STREAM_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "Create: set stream %d low pass cutoff(in set low pass): %f\n", i, percent );
#endif
		}
}

void MilesEngine::SetDefaultObstruction( bool obstructed )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetDefaultObstruction, obstructed );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetDefaultObstruction, obstructed );
#endif
	mObstructedByDefault = obstructed;
}

void MilesEngine::SetObstructionProperties( float heightenDelay, float lowerDelay, float minObstruction, float maxObstructionLow, float maxObstructionHigh, float minObstructionDistance, float maxObstructionDistance )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetObstructionProperties,
		heightenDelay, lowerDelay, minObstruction, maxObstructionLow, maxObstructionHigh, minObstructionDistance, maxObstructionDistance );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetObstructionProperties,
		heightenDelay, lowerDelay, minObstruction, maxObstructionLow, maxObstructionHigh, minObstructionDistance, maxObstructionDistance );
#endif

	HeightenObstructionTime = heightenDelay;
	LowerObstructionTime = lowerDelay;
	if( minObstruction > 1.0f ) minObstruction = 1.0f;
	else if( minObstruction < 0.0f ) minObstruction = 0.0f;
	MinObstruction = minObstruction;

	if( maxObstructionLow > 1.0f ) maxObstructionLow = 1.0f;
	else if( maxObstructionLow < 0.0f ) maxObstructionLow = 0.0f;
	MaxObstructionLow = maxObstructionLow;

	if( maxObstructionHigh > 1.0f ) maxObstructionHigh = 1.0f;
	else if( maxObstructionHigh < 0.0f ) maxObstructionHigh = 0.0f;
	MaxObstructionHigh = maxObstructionHigh;

	if( minObstructionDistance <= 0.01f ) minObstructionDistance = 0.01f;
	MinObstructionDistance = minObstructionDistance;

	if( maxObstructionDistance <= minObstructionDistance + 0.01f ) maxObstructionDistance = minObstructionDistance + 0.01f;
	MaxObstructionDistance = maxObstructionDistance;
}

void MilesEngine::SetAllSoundsToLowPass( float percent )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetAllSoundsToLowPass, percent );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetAllSoundsToLowPass, percent );
#endif
	Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
	if( percent > 1.0f ) percent = 1.0f;
	if( percent < 0.0f ) percent = 0.0f;

	mLowPassForNewSounds = percent;

	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( !( m2DSounds[i]->Flags & Miles2DSound::HasAlwaysSameSpeed ) )
			m2DSounds[i]->mLowPass = percent;

	float percentToSet = (percent > LowPassFrequencyPercent)?LowPassFrequencyPercent:percent;

	for( int i = 0; i < m2DRealSamples.Size(); ++i )
		if( !( m2DRealSounds[i]->Flags & Miles2DSound::HasAlwaysSameSpeed ) && ( m2DRealSounds[i]->mRealLowPass != percentToSet ) )
		{
#ifdef MILES_2DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "2D sample %d: set cut off (all sounds to low pass) to %f\n", m2DRealSamples[i], percentToSet );
#endif
			m2DRealSounds[i]->mRealLowPass = percentToSet;
			AIL_set_sample_low_pass_cut_off( m2DRealSamples[i], percentToSet );
		}

	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !(m3DSounds[i]->Flags&Miles3DSound::HasAlwaysSameSpeed) )
			m3DSounds[i]->mLowPass = percent;

	for( int i = 0; i < m3DRealSamples.Size(); ++i )
		if( !(m3DRealSounds[i]->Flags&Miles3DSound::HasAlwaysSameSpeed) && ( m3DRealSounds[i]->mRealLowPass != ( 1.0f-percentToSet ) ) )
		{
#ifdef MILES_3DSOUND_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "3D sample %d: set occlusion (all sounds to low pass) to %f\n", m3DRealSamples[i], 1.0f - percentToSet );
#endif
			m3DRealSounds[i]->mRealLowPass = 1.0f - percentToSet;
			AIL_set_3D_sample_occlusion( m3DRealSamples[i], 1.0f - percentToSet );
		}

	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid && ( mStreams[i].LowPass != percentToSet ) )
		{
			mStreams[i].LowPass = percentToSet;
			AIL_set_stream_low_pass_cut_off( mStreams[i].Stream, percentToSet );
#ifdef MILES_STREAM_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "Create: set stream %d low pass cutoff(in set all sounds to low pass): %f\n", i, percent );
#endif
		}
}

void MilesEngine::SetDopplerFactor( float factor )
{
	Assert( factor > 0 );
	AIL_set_3D_doppler_factor( DigitalDriver, factor );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "Set doppler factor to %f\n", factor );
#endif
}

int MilesEngine::PauseCurrentlyPlayingSounds( void )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::PauseCurrentlyPlayingSounds );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::PauseCurrentlyPlayingSounds );
#endif

    m_setTag("AudioMiles");
	SoundList* sounds = new SoundList;

	StopFinishedSounds();

	// Stop 2D sounds
	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( m2DSounds[i]->IsPlaying() )
		{
//			TRACE( "Pausing 2D sound: '%s'\n", m2DSounds[i]->mFile->mFileName );
			sounds->Sounds2D.Add( m2DSounds[i] );
			sounds->Sounds2DDeleteIfStopped.Add( m2DSounds[i]->Flags & Miles2DSound::DeleteIfStopped );
			m2DSounds[i]->Flags &= ~Miles2DSound::DeleteIfStopped;
			m2DSounds[i]->Stop();
		}

	// Stop 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( m3DSounds[i]->IsPlaying() )
		{
			sounds->Sounds3D.Add( m3DSounds[i] );
			sounds->Sounds3DDeleteIfStopped.Add( m3DSounds[i]->Flags & Miles3DSound::DeleteIfStopped );
			m3DSounds[i]->Flags &= ~Miles3DSound::DeleteIfStopped;
			m3DSounds[i]->Stop();
		}

	// Stop streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid )
		{
			S32 status = AIL_stream_status( mStreams[i].Stream );
			bool toResume = ( ( status == SMP_PLAYING ) || ( status == SMP_PLAYINGBUTRELEASED ) );
			if( toResume )
			{
				AIL_pause_stream( mStreams[i].Stream, 1 );
#ifdef MILES_STREAM_DEBUG
				TRACE( "Frame %d - ", GEngine->World->Frame );
				TRACE( "Pause (curplay) stream %d\n", i );
#endif
			}
			sounds->Streams.Add( i );
			sounds->StreamsResume.Add( toResume );
		}

	int stackIndex;
	if( PauseStackFill.Size() )
	{
		stackIndex = PauseStackFill[PauseStackFill.Size()-1];
		PauseStackFill.RemoveLast();
		PauseStack[stackIndex] = sounds;
	}
	else
		stackIndex = PauseStack.Add( sounds );
//	TRACE( "Pausing sounds group %d\n", stackIndex );
	return stackIndex;
}

void MilesEngine::ResumeSounds( int stackIndex )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::ResumeSounds, stackIndex );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::ResumeSounds, stackIndex );
#endif

	if( !PauseStack[stackIndex] )
	{
		TRACE( "Error!!! Tried to resume sounds from non-existent group!\n" );
		return;
	}
//	TRACE( "Resuming sounds group %d\n", stackIndex );

	SoundList* sounds = PauseStack[stackIndex];
	PauseStack[stackIndex] = 0;
	PauseStackFill.Add(stackIndex);

	StartManyAtOnce = true;

	// Resume 2D sounds
	for( int i = 0; i < sounds->Sounds2D.Size(); ++i )
	{
		Miles2DSound* s = sounds->Sounds2D[i];
		bool exists = false;
		for( int j = 0; j < m2DSounds.Size(); ++j )
			if( m2DSounds[j] == s )
			{
				exists = true;
				break;
			}

		if( exists )
		{
			Assert( !s->IsPlaying() );
			if( sounds->Sounds2DDeleteIfStopped[i] ) s->Flags |= Miles2DSound::DeleteIfStopped; else s->Flags &= ~Miles2DSound::DeleteIfStopped;
			s->Resume();
		}
	}

	// Resume 3D sounds
	for( int i = 0; i < sounds->Sounds3D.Size(); ++i )
	{
		Miles3DSound* s = sounds->Sounds3D[i];
		bool exists = false;
		for( int j = 0; j < m3DSounds.Size(); ++j )
			if( m3DSounds[j] == s )
			{
				exists = true;
				break;
			}

		if( exists )
		{
			Assert( !s->IsPlaying() );
			if( sounds->Sounds3DDeleteIfStopped[i] ) s->Flags |= Miles3DSound::DeleteIfStopped; else s->Flags &= ~Miles3DSound::DeleteIfStopped;
			s->Resume();
		}
	}

	// Resume streams
	for( int i = 0; i < sounds->Streams.Size(); ++i )
	{
		int index = sounds->Streams[i];
		if( ( index < mStreams.Size() ) && mStreams[index].Valid )
		{
			if( sounds->StreamsResume[i] )
				AIL_pause_stream( mStreams[index].Stream, 0 );
#ifdef MILES_STREAM_DEBUG
			TRACE( "Frame %d - ", GEngine->World->Frame );
			TRACE( "Unpause (resume) stream %d\n", index );
#endif
		}
	}

	delete sounds;

	StartManyAtOnce = false;
}

void MilesEngine::ResetFlagsFromPauseStack( void )
{
	for( int j = PauseStack.Size()-1; j >= 0; --j )
	{
		SoundList* sounds = PauseStack[j];
		if( !sounds ) continue;

		for( int i = 0; i < sounds->Sounds2D.Size(); ++i )
		{
			Miles2DSound* s = sounds->Sounds2D[i];
			bool exists = false;
			for( int j = 0; j < m2DSounds.Size(); ++j )
			if( m2DSounds[j] == s )
			{
				exists = true;
				break;
			}

			if( exists )
				if( sounds->Sounds2DDeleteIfStopped[i] ) s->Flags |= Miles2DSound::DeleteIfStopped; else s->Flags &= ~Miles2DSound::DeleteIfStopped;
		}

		for( int i = 0; i < sounds->Sounds3D.Size(); ++i )
		{
			Miles3DSound* s = sounds->Sounds3D[i];
			bool exists = false;
			for( int j = 0; j < m3DSounds.Size(); ++j )
				if( m3DSounds[j] == s )
				{
					exists = true;
					break;
				}

			if( exists )
				if( sounds->Sounds3DDeleteIfStopped[i] ) s->Flags |= Miles3DSound::DeleteIfStopped; else s->Flags &= ~Miles3DSound::DeleteIfStopped;
		}

		delete sounds;
	}

	PauseStack.ClearArray();
	PauseStackFill.ClearArray();
}

void MilesEngine::Reset( void )
{
	int list = PauseCurrentlyPlayingSounds();
/*
#ifndef FINAL_BUILD
	Dump();
#endif
*/
	ResumeSounds( list );
}

extern int channels[4];

#define MILESENGINE_LOAD_SAVE_TOKEN 0x5e60
#define MILESENGINE_LOAD_SAVE_TOKEN_1 0x5f00
#define MILESENGINE_LOAD_SAVE_TOKEN_2 0x5f01
#define MILESENGINE_LOAD_SAVE_TOKEN_3 0x5f02
#define MILESENGINE_LOAD_SAVE_TOKEN_4 0x5f03

void MilesEngine::LoadReset( void )
{
	ResetFlagsFromPauseStack();

	StartManyAtOnce = true; // this will get cleared at the end of next tick :)

	// Clear audio engine; hmm, I guess it's already been done when LoadingMap() was called. But anyway...

	// Delete remaining 3D sounds
	while( m3DSounds.Size() )
	{
		if( !( m3DSounds[0]->Flags & Miles3DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 3D - '%s'\n", m3DSounds[0]->mFile->mFileName.AsChar() );
		delete m3DSounds[0];
	}
	Assert( m3DRealSamples.Size() == 0 );

	// Delete remaining 2D sounds
	while( m2DSounds.Size() )
	{
		if( !(m2DSounds[0]->Flags & Miles2DSound::DeleteIfStopped ) )
			TRACE( "AUDIO: leaked sound 2D - '%s'\n", m2DSounds[0]->mFile->mFileName.AsChar() );
		delete m2DSounds[0];
	}
	Assert( m2DRealSamples.Size() == 0 );

	if( mStreams.Size() > mStreamsFill.Size() )
	{
		// Stop streams
		for( int i = 0; i < mStreams.Size(); ++i )
			if( mStreams[i].Valid )
				AIL_close_stream( mStreams[i].Stream );
	}
	mStreams.ClearArray();
	mStreamsFill.ClearArray();

	// Clear loaded files, just in case
	for( int i = 0; i < mLoadedFiles.Size(); i++ )
		delete mLoadedFiles[i];

	mLoadedFiles.ClearArray();
	mLoadedFilesHash.Clear();

	// Clear internal engine values
	mCurrentFalloffSpeed = 0.5f;
	mFilledFiles = 0;
	LowPassFrequencyPercent = 1.0f;
	mLowPassForNewSounds = 1.0f;
	mVolumeMultiplier = 1.0f;
	mDesiredSpeed = 1.0f;
	mLastSetSpeedTime = 0;
	mDesiredSpeedTime = 0;
	Speed = 1.0f;
	HeightenObstructionTime = 0.0f;
	LowerObstructionTime = 0.0f;
	MinObstruction = 0.5f;
	MaxObstructionLow = 1.0f;
	MaxObstructionHigh = 1.0f;
	MinObstructionDistance = 14.0f;
	MaxObstructionDistance = 100.0f;
	m2DSoundNumbers = 0;
	m3DSoundNumbers = 0;

	// Load external indices
	TickEngineList = -1;
	GEngine->Video->PMenu->mPausedSounds = -1;
	SaveGameSoundList = -1;
}

bool MilesEngine::LoadAudio( GFile* f )
{
	LoadReset();

	if( !f->TestToken( MILESENGINE_LOAD_SAVE_TOKEN ) )
	{
		TRACE( "Token test failure: MilesEngine::LoadAudio()\n" );
		return false;
	}

	int count;

	// Load internal engine values
	*f >> mCurrentFalloffSpeed;
	*f >> mFilledFiles;
	*f >> LowPassFrequencyPercent;
	*f >> mLowPassForNewSounds;
	*f >> mVolumeMultiplier;
	*f >> mDesiredSpeed;
	*f >> mLastSetSpeedTime;
	*f >> mDesiredSpeedTime;
	*f >> Speed;
	*f >> HeightenObstructionTime;
	*f >> LowerObstructionTime;
	*f >> MinObstruction;
	*f >> MaxObstructionLow;
	*f >> MaxObstructionHigh;
	*f >> MinObstructionDistance;
	*f >> MaxObstructionDistance;
	*f >> m2DSoundNumbers;
	*f >> m3DSoundNumbers;

	// Load external indices
	*f >> TickEngineList;
	*f >> GEngine->Video->PMenu->mPausedSounds;
	*f >> SaveGameSoundList;

	// Load loaded files ( restoring indices, why not )
	*f >> count;
	for( int i = 0; i < count; ++i )
	{
        m_setTag("AudioMiles");
        MilesLoadedFile* file = new MilesLoadedFile( "",0,0 );
		file->LoadFile( f, this );
		int fileInd = mLoadedFiles.Add( file );
		mLoadedFilesHash.Insert( file->mFileName, fileInd );
	}

	// Load stack ( only streams )
	*f >> count;
	Assert( PauseStack.Size() == PauseStackFill.Size() );
	PauseStack.ClearArray();
	PauseStackFill.ClearArray();
	for( int i = 0; i < count; ++i )
	{
		char t;
		*f >> t;
		if( !t )
		{
			PauseStack.Add( 0 );
			PauseStackFill.Add( i );
			continue;
		}

        m_setTag("AudioMiles");
		SoundList* sounds = new SoundList;
		PauseStack.Add( sounds );

		// Load streams to paused sounds table
		*f >> PauseStack[i]->Streams;

		char a;
		int count2;
		*f >> count2;
		for( int j = 0; j < count2; ++j )
		{
			*f >> a;
			PauseStack[i]->StreamsResume.Add( a?true:false );
		}
	}

	if( !f->TestToken( MILESENGINE_LOAD_SAVE_TOKEN_1 ) )
	{
		TRACE( "Token test failure: MilesEngine::LoadAudio() 1\n" );
		return false;
	}

	// Load streaming sounds
	*f >> count;
	for( int i = 0; i < count; ++i )
	{
		char ok;
		*f >> ok;
		if( ok )
		{
			String name;
			float lowPass;
			int speed;
			float volume;
			int loopCount;
			*f >> name >> lowPass >> speed >> volume >> loopCount;

			StreamingSoundData streamingData;
			HSTREAM s = AIL_open_stream( DigitalDriver, name.AsChar(), 0 );
			Assert( s );

#ifdef _DEBUG
			int ind = mStreams.Add(streamingData);
			Assert( ind == i );
#else
			mStreams.Add(streamingData);
#endif
			mStreams[i].Valid = true;
			mStreams[i].Stream = s;
			mStreams[i].Name = name;
			mStreams[i].LowPass = lowPass;
			mStreams[i].LoopCount = loopCount;
#ifdef _DEBUG
			S32 rate = AIL_stream_playback_rate( s );
			Assert( speed == rate );
#endif
			mStreams[i].Speed = speed;
			mStreams[i].Volume = volume;

			AIL_set_stream_loop_count( mStreams[i].Stream, loopCount );

			float strVol = StreamingVolume*volume;
			Assert( ( strVol >= 0.0f ) && ( strVol <= 1.0f ) );
			AIL_set_stream_volume_levels( mStreams[i].Stream, strVol, strVol );
			AIL_set_stream_user_data( s, 0, i );	// remember last stream pointer in a stream

			if( Speed != 1.0f )
			{
				Assert( speed*Speed > 0 );
				AIL_set_stream_playback_rate( s, speed*Speed );
			}

			float percent = (LowPassFrequencyPercent > mLowPassForNewSounds)?mLowPassForNewSounds:LowPassFrequencyPercent;
			Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
			AIL_set_stream_low_pass_cut_off( s, percent );

			int pos;
			*f >> pos;
			Assert( pos >= 0 );
			AIL_set_stream_position(mStreams[i].Stream,pos);
		}
		else
		{
			StreamingSoundData data( mLowPassForNewSounds );
			mStreams.Add(data);
			mStreamsFill.Add(i);
		}
	}

	*f >> channels[0] >> channels[1] >> channels[2] >> channels[3];

	if( !f->TestToken( MILESENGINE_LOAD_SAVE_TOKEN_2 ) )
	{
		TRACE( "Token test failure: MilesEngine::LoadAudio() 2\n" );
		return false;
	}

	// Load 2D sounds
	m2DSoundHash.Clear();
	*f >> count;
	for( int i = 0; i < count; ++i )
	{
        m_setTag("AudioMiles");
        Miles2DSound* sound = new Miles2DSound( 0, this, -1 );
		sound->LoadSound( f );
		m2DSounds.Add( sound );
	}

	if( !f->TestToken( MILESENGINE_LOAD_SAVE_TOKEN_3 ) )
	{
		TRACE( "Token test failure: MilesEngine::LoadAudio() 3\n" );
		return false;
	}

	// Load 3D sounds
	*f >> count;
	for( int i = 0; i < count; ++i )
	{
        m_setTag("AudioMiles");
        Miles3DSound* sound = new Miles3DSound( 0, this, -1 );
		sound->LoadSound( f );
		m3DSounds.Add( sound );
	}

	if( !f->TestToken( MILESENGINE_LOAD_SAVE_TOKEN_4 ) )
	{
		TRACE( "Token test failure: MilesEngine::LoadAudio() 4\n" );
		return false;
	}

	return true;
}

bool MilesEngine::SaveAudio( GFile* f )
{
	if( !f->PutToken( MILESENGINE_LOAD_SAVE_TOKEN ) ) return false;

	int count;

	// Save internal engine values
	*f << mCurrentFalloffSpeed;
	*f << mFilledFiles;
	*f << LowPassFrequencyPercent;
	*f << mLowPassForNewSounds;
	*f << mVolumeMultiplier;
	*f << mDesiredSpeed;
	*f << mLastSetSpeedTime;
	*f << mDesiredSpeedTime;
	*f << Speed;
	*f << HeightenObstructionTime;
	*f << LowerObstructionTime;
	*f << MinObstruction;
	*f << MaxObstructionLow;
	*f << MaxObstructionHigh;
	*f << MinObstructionDistance;
	*f << MaxObstructionDistance;
	*f << m2DSoundNumbers;
	*f << m3DSoundNumbers;

	// Save external indices
	*f << TickEngineList;
	*f << GEngine->Video->PMenu->mPausedSounds;
	*f << SaveGameSoundList;

	// Save loaded files ( preserving indices, why not )
	*f << mLoadedFiles.Size();
	for( int i = 0; i < mLoadedFiles.Size(); ++i )
		mLoadedFiles[i]->SaveFile( f );

	// Save stack ( only streams )
	*f << PauseStack.Size();
	for( int i = 0; i < PauseStack.Size(); ++i )
	{
		char t = PauseStack[i]?1:0;
		*f << t;
		if( !PauseStack[i] ) continue;

		// Save streams from paused sounds table
		*f << PauseStack[i]->Streams;

		count = PauseStack[i]->StreamsResume.Size();
		*f << count;
		for( int j = 0; j < count; ++j )
		{
			char a = PauseStack[i]->StreamsResume[j]?1:0;
			*f << a;
		}
	}

	if( !f->PutToken( MILESENGINE_LOAD_SAVE_TOKEN_1 ) ) return false;

	// Save streaming sounds
	*f << mStreams.Size();
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid )
		{
			*f << (char)1;
			*f << mStreams[i].Name;
			*f << mStreams[i].LowPass;
			*f << mStreams[i].Speed;
			*f << mStreams[i].Volume;
			*f << mStreams[i].LoopCount;
			int pos = AIL_stream_position(mStreams[i].Stream);
			Assert( pos >= 0 );
			*f << pos;
		}
		else
			*f << (char)0;

	*f << channels[0] << channels[1] << channels[2] << channels[3];

	if( !f->PutToken( MILESENGINE_LOAD_SAVE_TOKEN_2 ) ) return false;

	// Save 2D sounds
	count = 0;
	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( !( m2DSounds[i]->Flags & Miles2DSound::DontSave ) )
			count++;
	*f << count;
	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( !( m2DSounds[i]->Flags & Miles2DSound::DontSave ) )
			if( !m2DSounds[i]->SaveSound( f ) )
				return false;

	if( !f->PutToken( MILESENGINE_LOAD_SAVE_TOKEN_3 ) ) return false;

	// Save 3D sounds
	count = 0;
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !(m3DSounds[i]->Flags & Miles3DSound::DontSave) )
			count++;
	*f << count;
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( !(m3DSounds[i]->Flags & Miles3DSound::DontSave) )
			if( !m3DSounds[i]->SaveSound( f ) )
				return false;

	if( !f->PutToken( MILESENGINE_LOAD_SAVE_TOKEN_4 ) ) return false;

	return true;
}

int MilesEngine::StreamingSound_Create( const char* name )
{
	if( !DigitalDriver ) return -1;

#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_Create, name );
#endif

	HSTREAM s = AIL_open_stream( DigitalDriver, name, 0 );
	if( s == NULL )
	{
		TRACE( "Stream open error: %s\n", AIL_last_error() );
		return -1;
	}

	// Find free index
	int sIndex = -1;
	if( mStreamsFill.Size() )
	{
		sIndex = mStreamsFill[ mStreamsFill.Size()-1 ];
		mStreamsFill.RemoveLast();
	}
	else
	{
		sIndex = mStreams.Size();
		StreamingSoundData data( mLowPassForNewSounds );
		mStreams.Add(data);
	}

	mStreams[sIndex].Valid = true;
	mStreams[sIndex].Stream = s;
	mStreams[sIndex].Name = name;
	mStreams[sIndex].LowPass = mLowPassForNewSounds;
	mStreams[sIndex].LoopCount = 1;
	float strVol = StreamingVolume*mStreams[sIndex].Volume;
	Assert( ( strVol >= 0.0f ) && ( strVol <= 1.0f ) );
	AIL_set_stream_volume_levels( mStreams[sIndex].Stream, strVol, strVol );
	AIL_set_stream_user_data(s,0,sIndex);	// remember last stream pointer in a stream
	S32 rate = AIL_stream_playback_rate( s );
	Assert( rate > 0 );
	mStreams[sIndex].Speed = rate;

	if( Speed != 1.0f )
	{
		Assert( rate*Speed > 0 );
		AIL_set_stream_playback_rate( s, rate*Speed );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Create: set stream %d playback rate: %f\n", sIndex, rate*Speed );
#endif
	}

	float percent = (LowPassFrequencyPercent > mLowPassForNewSounds)?mLowPassForNewSounds:LowPassFrequencyPercent;
	Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
	AIL_set_stream_low_pass_cut_off( s, percent );

#ifdef MILES_STREAM_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "Create: set stream %d low pass cut off: %f\n", sIndex, percent );
#endif

	return sIndex;
}

int MilesEngine::StreamingSound_GetLoopCount( int index )
{
	if( ( index < 0 ) || ( index >= mStreams.Size() ) || !mStreams[index].Valid ) return 0;
	int lc = AIL_stream_loop_count( mStreams[index].Stream );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Stream %d get loop count %d\n", index, lc );
#endif
	Assert( lc >= 0 );
	mStreams[index].LoopCount = lc;
	return lc;
}

void MilesEngine::StreamingSound_SetLoopCount( int index, int count )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_SetLoopCount, index, count );
#endif

	Assert( count >= 0 );
	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		mStreams[index].LoopCount = count;
		AIL_set_stream_loop_count( mStreams[index].Stream, count );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Set stream %d loop count to %d\n", index, count );
#else
		TRACE( "Set stream %d loop count to %d\n", index, count );
#endif
	}
}

void MilesEngine::StreamingSound_Play( int index )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_Play, index );
#endif

	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		AIL_start_stream( mStreams[index].Stream );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Start stream %d\n", index );
#endif
	}
}

void MilesEngine::StreamingSound_SetVolume( int index, float volume )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_SetVolume, index, volume );
#endif

	if( volume < 0.0f ) volume = 0.f;
	Assert( StreamingVolume*volume <= 1.0f );
	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		mStreams[index].Volume = volume;
		AIL_set_stream_volume_levels( mStreams[index].Stream, StreamingVolume*volume, StreamingVolume*volume );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Stream %d set volume (%f)\n", index, StreamingVolume*volume );
#endif
	}
}

void MilesEngine::StreamingSound_GetVolume( int index, float& volume )
{
	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
        volume = mStreams[index].Volume;
        Assert( ( volume >= 0.0f ) && ( volume <= 1.0f ) );
	}
}

void MilesEngine::StreamingSound_SetLowPass( int index, float lp )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_SetLowPass, index, lp );
#endif

	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		mStreams[index].LowPass = lp;
		float reallp = (LowPassFrequencyPercent>lp)?lp:LowPassFrequencyPercent;
		Assert( ( reallp >= 0.0f ) && ( reallp <= 1.0f ) );
		if( reallp < 0.2f ) reallp = 0.2f;
		AIL_set_stream_low_pass_cut_off( mStreams[index].Stream, reallp );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Stream %d set low pass to %d\n", index, reallp );
#endif
	}
}

float MilesEngine::StreamingSound_GetLowPass( int index )
{
	return ( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )?mStreams[index].LowPass:1.0f;
}

void MilesEngine::StreamingSound_Stop( int index )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_Stop, index );
#endif

	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		AIL_pause_stream( mStreams[index].Stream, 1 );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Pause stream %d (from outside)\n", index );
#endif
	}
}

void MilesEngine::StreamingSound_Resume( int index )
{
#ifdef AUDIO_RECORD_STREAMS
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StreamingSound_Resume, index );
#endif
	if( ( index >= 0 ) && ( index < mStreams.Size() ) && ( mStreams[index].Valid ) )
	{
		AIL_pause_stream( mStreams[index].Stream, 0 );
#ifdef MILES_STREAM_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Unpause stream %d (from outside)\n", index );
#endif
	}
}

bool MilesEngine::StreamingSound_IsPlaying( int index )
{
	U32 out = AIL_stream_status( mStreams[index].Stream );
	bool isPlaying = (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED);
#ifdef MILES_STREAM_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "Stream %d is playing? %s\n", index, isPlaying?"Yes":"No" );
#endif
	return isPlaying;
}

int MilesEngine::Sound2D_Create( const char* name, unsigned char priority )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Create, name, priority );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Create, name, priority );
#endif

	if( !DigitalDriver ) return -1;
	MilesLoadedFile* file = PrepareSample( name );
	if( !file ) return -1;

    m_setTag("AudioMiles");
	Miles2DSound* sound = new Miles2DSound( file, this, m2DSoundNumbers++ );
	sound->mPriority = priority;
	m2DSounds.Add( sound );
	return sound->mSoundIndex;
}

int MilesEngine::Sound2D_CreateEx( const char* name, unsigned char priority, float volume, bool samespeed, bool noSpeedRandomize, bool playAndForget, float intensity, bool dontSave )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_CreateEx,
		name, priority, volume, samespeed, noSpeedRandomize, playAndForget, intensity, dontSave );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_CreateEx,
		name, priority, volume, samespeed, noSpeedRandomize, playAndForget, intensity, dontSave );
#endif

	if( !DigitalDriver ) return -1;
	MilesLoadedFile* file = PrepareSample( name );
	if( !file ) return -1;


    m_setTag("AudioMiles");
    Miles2DSound* sound = new Miles2DSound( file, this, m2DSoundNumbers++ );
	sound->mPriority = priority;
	m2DSounds.Add( sound );
	sound->SetVolume( volume );

	if( samespeed )
	{
		sound->SetAlwaysSameSpeed( true );
		sound->SetLowPass( 1.0f );
	}

	if( noSpeedRandomize )
		sound->mSoundRateDiff = 1.0f;

	sound->mSetIntensity = intensity;
	sound->SetCurrentIntensity( intensity*Current2DIntensity );

	if( dontSave )
		sound->Flags |= Miles2DSound::DontSave;

	if( playAndForget )
	{
		sound->Flags |= Miles2DSound::DeleteIfStopped;
		sound->Play();
	}

	return sound->mSoundIndex;
}

int MilesEngine::Sound2D_Create( GFile* f )
{
	Assert( SaveGameSoundList != -1 );

    m_setTag("AudioMiles");
    Miles2DSound* sound = new Miles2DSound( 0, this, -1 );
	sound->LoadSound( f );
	m2DSounds.Add( sound );
	return sound->mSoundIndex;
}

void MilesEngine::Sound2D_Play( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Play, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Play, index );
#endif

	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->Play();
}

void MilesEngine::Sound2D_PlayAndForget( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_PlayAndForget, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_PlayAndForget, index );
#endif

	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s )
	{
		(*s)->Flags |= Miles2DSound::DeleteIfStopped;
		(*s)->Play();
	}
}

void MilesEngine::Sound2D_Stop( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Stop, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Stop, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->Stop();
}

void MilesEngine::Sound2D_Resume( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Resume, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Resume, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->Resume();
}

void MilesEngine::Sound2D_Delete( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Delete, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Delete, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) delete *s;
}

void MilesEngine::Sound2D_Forget( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_Forget, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_Forget, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->Flags |= Miles2DSound::DeleteIfStopped;
}

bool MilesEngine::Sound2D_IsPlaying( int index )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	return s?(*s)->IsPlaying():false;
}

void MilesEngine::Sound2D_SetDontSave( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetDontSave, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetDontSave, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->Flags |= Miles2DSound::DontSave;
}

void MilesEngine::Sound2D_SetLoopCount( int index, int count )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetLoopCount, index, count );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetLoopCount, index, count );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->SetLoopCount( count );
}

float MilesEngine::Sound2D_GetVolume( int index )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	return s?(*s)->mVolume:1.0f;
}

void MilesEngine::Sound2D_SetVolume( int index, float volume, float switchTime )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetVolume, index, volume, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetVolume, index, volume, switchTime );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->SetVolume( volume, switchTime );
}
/*
void MilesEngine::Sound2D_SetIntensity( int index, float intensity, float switchTime )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s )
	{
		(*s)->mSetIntensity = intensity;
		(*s)->SetCurrentIntensity( intensity*Current2DIntensity, switchTime );
	}
}
*/

void MilesEngine::Sound2D_NoSpeedRandomize( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_NoSpeedRandomize, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_NoSpeedRandomize, index );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->mSoundRateDiff = 1.0f;
}

void MilesEngine::Sound2D_SetSoundSpeed( int index, float speed )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetSoundSpeed, index, speed );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetSoundSpeed, index, speed );
#endif

	if( speed < 0.001f ) speed = 0.001f;
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s )
	{
		DWORD time = GetCurrentTime();
		double speedChange = speed/(*s)->mSoundRateDiff;
		(*s)->mSoundRateDiff = speed;

		if( (*s)->mPlayStart ) (*s)->mPlayStart = time - ( time - (*s)->mPlayStart ) / speedChange;
		if( (*s)->mPlayLength != DWORD(-1) ) (*s)->mPlayLength /= speedChange;

		int sampleIndex = (*s)->mSampleIndex;
		if( sampleIndex != -1 )
		{
			int spbr = (*s)->mFile->mFileSoundInfo.rate*Speed*(*s)->mSoundRateDiff;
			Assert( spbr > 0 );
			AIL_set_sample_playback_rate( m2DRealSamples[sampleIndex], spbr );
		}
	}
}


bool MilesEngine::Sound2D_Save( int index, GFile* f )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	return s?(*s)->SaveSound( f ):false;
}

void MilesEngine::Sound2D_SetAlwaysSameSpeed( int index, bool set )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetAlwaysSameSpeed, index, set );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetAlwaysSameSpeed, index, set );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->SetAlwaysSameSpeed( set );
}

void MilesEngine::Sound2D_SetLowPass( int index, float lowPass )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound2D_SetLowPass, index, lowPass );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound2D_SetLowPass, index, lowPass );
#endif
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->SetLowPass( lowPass );
}

float MilesEngine::Sound2D_GetLowPass( int index )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	return s?(*s)->mLowPass:1.0f;
}

int MilesEngine::Sound3D_Create( const char* name, unsigned char priority )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Create, name, priority );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Create, name, priority );
#endif

	if( !DigitalProvider )
		return -1;

	MilesLoadedFile* file = PrepareSample( name );
	if( !file )
		return -1;

    m_setTag("AudioMiles");
	Miles3DSound* sound = new Miles3DSound( file, this, m3DSoundNumbers++ );
	sound->mPriority = priority;
	m3DSounds.Add( sound );
	return sound->mSoundIndex;
}

void MilesEngine::Sound3D_Resume( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Resume, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Resume, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->Resume();
}

void MilesEngine::Sound3D_Delete( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Delete, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Delete, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) delete *s;
}

bool MilesEngine::Sound3D_IsPlaying( int index )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(*s)->IsPlaying():false;
}

bool MilesEngine::Sound3D_IsObstructed( int index )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(((*s)->Flags & Miles3DSound::Obstructed) != 0):false;
}

bool MilesEngine::Sound3D_HasAlwaysSameSpeed( int index )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(((*s)->Flags&Miles3DSound::HasAlwaysSameSpeed) != 0):false;
}

void MilesEngine::Sound3D_Forget( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Forget, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Forget, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->Flags |= Miles3DSound::DeleteIfStopped;
}

void MilesEngine::Sound3D_Stop( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Stop, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Stop, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->Stop();
}

void MilesEngine::Sound3D_SetDontSave( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetDontSave, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetDontSave, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->Flags |= Miles3DSound::DontSave;
}

void MilesEngine::Sound3D_SetPosition( int index, Vector& position )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetPosition, index, position.X, position.Y, position.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetPosition, index, position.X, position.Y, position.Z );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetPosition( position );
}

void MilesEngine::Sound3D_SetLoopCount( int index, int count )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetLoopCount, index, count );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetLoopCount, index, count );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetLoopCount( count );
}

float MilesEngine::Sound3D_GetVolume( int index )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(*s)->mVolume:1.0f;
}

void MilesEngine::Sound3D_SetVolume( int index, float volume, float switchTime )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetVolume, index, volume, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetVolume, index, volume, switchTime );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetVolume( volume, switchTime );
}

void MilesEngine::Sound3D_SetIntensity( int index, float intensity, float switchTime )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetIntensity, index, intensity, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetIntensity, index, intensity, switchTime );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s )
	{
		(*s)->mSetIntensity = intensity;
		(*s)->SetCurrentIntensity( intensity*Current3DIntensity, switchTime );
	}
}

void MilesEngine::Sound3D_SetHearingDistance( int index, float minDist, float maxDist )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetHearingDistance, index, minDist, maxDist );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetHearingDistance, index, minDist, maxDist );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetHearingDistance( minDist, maxDist );
}

void MilesEngine::Sound3D_SetObstructed( int index, bool obstructed )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetObstructed, index, obstructed );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetObstructed, index, obstructed );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s )
	{
		if( obstructed )
			(*s)->Flags |= Miles3DSound::Obstructed;
		else
			(*s)->Flags &= ~Miles3DSound::Obstructed;
	}
}

void MilesEngine::Sound3D_SetNoSpeedRandomize( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetNoSpeedRandomize, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetNoSpeedRandomize, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->mSoundRateDiff = 1.0f;
}

void MilesEngine::Sound3D_SetSoundSpeed( int index, float speed )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetSoundSpeed, index, speed );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetSoundSpeed, index, speed );
#endif
	if( speed < 0.001f ) speed = 0.001f;
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s )
	{
		DWORD time = GetCurrentTime();
		double speedChange = speed/(*s)->mSoundRateDiff;
		(*s)->mSoundRateDiff = speed;

		if( (*s)->mPlayStart ) (*s)->mPlayStart = time - ( time - (*s)->mPlayStart ) / speedChange;
		if( (*s)->mPlayLength != DWORD(-1) ) (*s)->mPlayLength /= speedChange;

		int sampleIndex = (*s)->mSampleIndex;
		if( sampleIndex != -1 )
		{
			int spbr = (*s)->mFile->mFileSoundInfo.rate*Speed*(*s)->mSoundRateDiff;
			Assert( spbr > 0 );
			AIL_set_3D_sample_playback_rate( m3DRealSamples[sampleIndex], spbr );
		}
	}
}

void MilesEngine::Sound3D_Play( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_Play, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_Play, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->Play();
}

void MilesEngine::Sound3D_SetVelocity( int index, Vector& velocity )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetVelocity, index, velocity.X, velocity.Y, velocity.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetVelocity, index, velocity.X, velocity.Y, velocity.Z );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetVelocity( velocity );
}

int MilesEngine::Sound3D_Create( GFile* f )
{
	Assert( SaveGameSoundList != -1 );

    m_setTag("AudioMiles");
    Miles3DSound* sound = new Miles3DSound( 0, this, -1 );
	sound->LoadSound( f );
	m3DSounds.Add( sound );
	return sound->mSoundIndex;
}

bool MilesEngine::Sound3D_Save( int index, GFile* f )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(*s)->SaveSound( f ):false;
}

void MilesEngine::Sound3D_PlayAndForget( int index )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_PlayAndForget, index );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_PlayAndForget, index );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s )
	{
		(*s)->Flags |= Miles3DSound::DeleteIfStopped;
		(*s)->Play();
	}
}

void MilesEngine::Sound3D_SetLowPass( int index, float lowPass )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetLowPass, index, lowPass );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetLowPass, index, lowPass );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetLowPass( lowPass );
}

float MilesEngine::Sound3D_GetLowPass( int index )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	return s?(*s)->mLowPass:1.0f;
}

void MilesEngine::Sound3D_SetOrientation( int index, Vector& front, Vector& up )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_SetOrientation, index,
		front.X, front.Y, front.Z, up.X, up.Y, up.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_SetOrientation, index,
		front.X, front.Y, front.Z, up.X, up.Y, up.Z );
#endif
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->SetOrientation( front, up );
}

void MilesEngine::SaveGame_PauseSounds( void )
{
	SaveGameSoundList = PauseCurrentlyPlayingSounds();
}

void MilesEngine::SaveGame_ResumeSounds( void )
{
	// Before that, we need to recreate the list of privileged sounds, if some are playing
	mPrivilegedSounds.ClearArray();

	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( m2DSounds[i]->Flags & Miles2DSound::Privileged )
		{
			PrivilegedSound s;
			s.Index = m2DSounds[i]->mSoundIndex;
			s.Is3D = false;
			mPrivilegedSounds.Add(s);
		}

	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( m3DSounds[i]->Flags & Miles3DSound::Privileged )
		{
			PrivilegedSound s;
			s.Index = m3DSounds[i]->mSoundIndex;
			s.Is3D = true;
			mPrivilegedSounds.Add(s);
		}

	// Now resume sounds, if we have a list ( we may not have one if we load a save from computer no audio card )
	if( SaveGameSoundList != -1 )
		ResumeSounds(SaveGameSoundList);
	SaveGameSoundList = -1;
}

void MilesEngine::SetFalloffSpeed( float speed )
{
	if( !DigitalDriver ) return;
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetFalloffSpeed, speed );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetFalloffSpeed, speed );
#endif
//	Assert( DigitalProvider );
	Assert( speed >= 0.0f );
	AIL_set_3D_rolloff_factor( DigitalDriver, speed );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "set 3D rolloff factor to %f\n", speed );
#endif
	mCurrentFalloffSpeed = speed;
}

float MilesEngine::GetFalloffSpeed( void )
{
	if( DigitalDriver )
		return AIL_3D_rolloff_factor( DigitalDriver );
	else
		return mCurrentFalloffSpeed;
}

void MilesEngine::SetSoundSpeedRandomizer( float value )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetSoundSpeedRandomizer, value );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetSoundSpeedRandomizer, value );
#endif
	value = fabs( value );
	if( value >= 0.99f ) value = 0.99f;
	mSoundRateDiffRate = value;
}

bool MilesEngine::SetFirstSuccesfulProvider( int startingProvider )
{
	if( !NumOfProviders() )
		return false;

	int i = startingProvider;
	do
	{
		i++;
		if( i >= NumOfProviders() )
			i = 0;
		if( Set3DSoundProvider(i) )
			return true;
	}
	while( i != startingProvider );
	return false;
}

bool MilesEngine::SetMostSuitableProvider( void )
{
	if( !NumOfProviders() ) return false;
//	if( Set3DSoundProvider( "Aureal A3D 2.0 (TM)" ) ) return true;
//	if( Set3DSoundProvider( "Aureal A3D Interactive (TM)" ) ) return true;
	if( Set3DSoundProvider( "Creative Labs EAX 3 (TM)" ) ) return true;
	if( Set3DSoundProvider( "Creative Labs EAX 2 (TM)" ) ) return true;
	if( Set3DSoundProvider( "Creative Labs EAX (TM)" ) ) return true;
	if( Set3DSoundProvider( "DirectSound3D Hardware Support" ) ) return true;
	if( Set3DSoundProvider( "Miles Fast 2D Positional Audio" ) ) return true;
	if( Set3DSoundProvider( "Dolby Surround Filter" ) ) return true;
	if( Set3DSoundProvider( "DirectSound3D 7+ Software - Full HRTF" ) ) return true;
	if( Set3DSoundProvider( "DirectSound3D 7+ Software - Light HRTF" ) ) return true;
	if( Set3DSoundProvider( "DirectSound3D 7+ Software - Pan and Volume" ) ) return true;
	if( Set3DSoundProvider( "DirectSound3D Software Emulation" ) ) return true;
	if( Set3DSoundProvider( "RAD Game Tools RSX 3D Audio" ) ) return true;
	return false;
}

bool MilesEngine::Set3DSoundProvider( String providerName )
{
	for( int i = 0; i < NumOfProviders(); ++i )
	{
		if( mProviderNames[i] == providerName )
			return Set3DSoundProvider( i );
	}
	return false;
}

void MilesEngine::SetMasterVolumeLevel( float level )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetMasterVolumeLevel, level );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetMasterVolumeLevel, level );
#endif
	Assert( ( level >= 0.0f ) && ( level <= 1.0f ) );
	if( level < 0.0001f ) level = 0.0001f;
	if( DigitalDriver )
	{
		float digMastVolLev = AIL_digital_master_volume_level( DigitalDriver );
		if( digMastVolLev < 0.0001f ) digMastVolLev = 0.0001f;
		if( digMastVolLev == level ) return;
		float curr3DVol = DigitalEffectsVolume3D / digMastVolLev;
		AIL_set_digital_master_volume_level( DigitalDriver, level );
#ifdef MILES_2DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "Set digital master volume level to %f\n", level );
#endif
		Set3DDigitalEffectsVolume( curr3DVol );
	}
}

float MilesEngine::Get3DDigitalEffectsVolume( void )
{
	float currMas = AIL_digital_master_volume_level( DigitalDriver );
	Assert( ( currMas >= 0.0f ) && ( currMas <= 1.0f ) );
	if( currMas < 0.0001f ) currMas = 0.0001f;
	return DigitalEffectsVolume3D / currMas;
}

void MilesEngine::SetStreamingVolume( float volume )
{
	if( !DigitalDriver ) return;
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetStreamingVolume, volume );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetStreamingVolume, volume );
#endif

	bool changed = false;

	if( volume > 1.0f ) volume = 1.0f;
	if( volume < 0.0f ) volume = 0.0f;
	if( StreamingVolume != volume ) changed = true;
	StreamingVolume = volume;

	// Update digital samples
	if( changed )
		for( int i = 0; i < mStreams.Size(); ++i )
			if( mStreams[i].Valid )
			{
				float strVol = StreamingVolume*mStreams[i].Volume;
				Assert( ( strVol >= 0.0f ) && ( strVol <= 1.0f ) );
				AIL_set_stream_volume_levels( mStreams[i].Stream, strVol, strVol );
#ifdef MILES_STREAM_DEBUG
				TRACE( "Frame %d - ", GEngine->World->Frame );
				TRACE( "Create: set stream %d volume pan (in streaming volume pan): %f\n", i, strVol );
#endif
			}
}

void MilesEngine::SetSpeakerType( int type )
{
	if( !DigitalProvider ) return;
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetSpeakerType, type );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetSpeakerType, type );
#endif

//	S32 speakerType = AIL_3D_2_SPEAKER;
//	switch( type )
//	{
//	default:
//	case Speakers2:		speakerType = AIL_3D_2_SPEAKER;		break;
//	case Headphones:	speakerType = AIL_3D_HEADPHONE;		break;
//	case Surround:		speakerType = AIL_3D_SURROUND;		break;
//	case Speakers4:		speakerType = AIL_3D_4_SPEAKER;		break;
//	case Speakers51:	speakerType = AIL_3D_51_SPEAKER;	break;
//	case Speakers71:	speakerType = AIL_3D_71_SPEAKER;	break;
//	};

//	S32 oldType = AIL_3D_speaker_type( DigitalProvider );
//	if( oldType == speakerType ) return;
//	AIL_set_3D_speaker_type( DigitalProvider, speakerType );
#ifdef MILES_3DSOUND_DEBUG
//	TRACE( "Frame %d - ", GEngine->World->Frame );
//	TRACE( "set 3D speaker type to %d\n", speakerType );
#endif
}

void MilesEngine::SetSpeakersFlip( bool flipped )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetSpeakersFlip, flipped );
	mSpeakersFlip = flipped;
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetSpeakersFlip, flipped );
	mSpeakersFlip = flipped;
#endif
}

void MilesEngine::SetRoom( int roomType, bool set2D )
{
	int rType;	// for conversion from our internal into Miles
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetRoom, roomType, set2D );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetRoom, roomType, set2D );
#endif

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

	if( CurrentRoomType == rType ) return;

	CurrentRoomType = rType;

//	if( DigitalDriver ) AIL_set_digital_master_room_type( DigitalDriver, rType );

	if( DigitalDriver /* && UsingEAX */ )
//	{
		AIL_set_room_type( DigitalDriver, rType );
#ifdef MILES_3DSOUND_DEBUG
//		TRACE( "Frame %d - ", GEngine->World->Frame );
//		TRACE( "Set 3D room type to %d\n", rType );
#endif
//	}
}

void MilesEngine::SetRoomIntensity2D( float intensity, float switchTime )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetRoomIntensity2D, intensity, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetRoomIntensity2D, intensity, switchTime );
#endif

	if( intensity == -1.0f ) intensity = Current2DIntensity;
	if( intensity > 1.0f ) intensity = 1.0f; else if( intensity < 0.0f ) intensity = 0.0f;

	if( Current2DIntensity != intensity )
	{
		Current2DIntensity = intensity;
		for( int i = 0; i < m2DSounds.Size(); ++i )
			m2DSounds[i]->SetCurrentIntensity( m2DSounds[i]->mSetIntensity*Current2DIntensity, switchTime );
	}
}

void MilesEngine::SetRoomIntensity3D( float intensity, float switchTime )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetRoomIntensity3D, intensity, switchTime );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetRoomIntensity3D, intensity, switchTime );
#endif
	if( intensity == -1.0f ) intensity = Current3DIntensity;
	if( intensity > 1.0f ) intensity = 1.0f; else if( intensity < 0.0f ) intensity = 0.0f;

	if( Current3DIntensity != intensity )
	{
		Current3DIntensity = intensity;
		for( int i = 0; i < m3DSounds.Size(); ++i )
			m3DSounds[i]->SetCurrentIntensity( m3DSounds[i]->mSetIntensity*Current3DIntensity, switchTime );
	}
}

/*
void MilesEngine::SetCustom2DReverb( float reverbTime, float reverbPreDelay, float reverbDamping, float wetLevel, float dryLevel )
{
	if( DigitalDriver )
	{
		AIL_set_digital_master_reverb( DigitalDriver, reverbTime, reverbPreDelay, reverbDamping );
		AIL_set_digital_master_reverb_levels( DigitalDriver, wetLevel, dryLevel );
	}
}
*/

Vector MilesEngine::GetPlayerPosition( void )
{
	Vector pos;
//	if( Listener )
//	{
		AIL_listener_3D_position( DigitalDriver, &pos.X, &pos.Y, &pos.Z );
		pos.Z = -pos.Z;
		return pos;
//	}
	return Vector(0.0f,0.0f,0.0f);
}

void MilesEngine::SetPlayerOrientation( Vector& front, Vector& up )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetPlayerOrientation,
		front.X, front.Y, front.Z, up.X, up.Y, up.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetPlayerOrientation,
		front.X, front.Y, front.Z, up.X, up.Y, up.Z );
#endif

//	if( Listener )
//	{
		if( mSpeakersFlip )
			AIL_set_listener_3D_orientation( DigitalDriver, -front.X, -front.Y, front.Z, up.X, up.Y, -up.Z );
		else
			AIL_set_listener_3D_orientation( DigitalDriver, front.X, front.Y, -front.Z, up.X, up.Y, -up.Z );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D listener orientation set\n" );
#endif
//	}
}

void MilesEngine::GetPlayerOrientation( Vector& front, Vector& up )
{
//	if( Listener )
//	{
		AIL_listener_3D_orientation( DigitalDriver, &front.X, &front.Y, &front.Z, &up.X, &up.Y, &up.Z );
		if( mSpeakersFlip ) front = -front;
		front.Z = -front.Z;
		up.Z = -up.Z;
//	};
}

void MilesEngine::SetPlayerVelocity( Vector& velocity )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetPlayerVelocity, velocity.X, velocity.Y, velocity.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetPlayerVelocity, velocity.X, velocity.Y, velocity.Z );
#endif

//	if( Listener )
//	{
		float magn = velocity.Magnitude();
		velocity.Normalize();
		AIL_set_listener_3D_velocity( DigitalDriver, velocity.X, velocity.Y, -velocity.Z, 0.001f*magn );
#ifdef MILES_2DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D listener velocity set to %f\n", 0.001f*magn );
#endif
//	}
}

Vector MilesEngine::GetPlayerVelocity( void )
{
	Vector vel;
//	if( Listener )
//	{
		AIL_listener_3D_velocity( DigitalDriver, &vel.X, &vel.Y, &vel.Z );
		vel.Z = -vel.Z;
		vel *= 1000.0f;
		return vel;
//	}
	return Vector(0.0f,0.0f,0.0f);
}

int MilesEngine::Sound3D_CreateEx( const char* name, unsigned char priority, bool playAtOnce, float loopCount, Vector position,
	float mindist, float maxdist, float volume, bool noSpeedRandomize, bool dontSave, bool obstructed, float intensity )
{
	if( !DigitalProvider ) return -1;

#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::Sound3D_CreateEx, priority,
		playAtOnce, loopCount, position.X, position.Y, position.Z, mindist, maxdist, volume, noSpeedRandomize, dontSave,
		obstructed, intensity, name);
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::Sound3D_CreateEx, priority,
		playAtOnce, loopCount, position.X, position.Y, position.Z, mindist, maxdist, volume, noSpeedRandomize, dontSave,
		obstructed, intensity, name);
#endif

	MilesLoadedFile* file = PrepareSample( name );
	if( !file ) return -1;

    m_setTag("AudioMiles");
	Miles3DSound* sound = new Miles3DSound( file, this, m3DSoundNumbers++ );
	sound->mPriority = priority;
	m3DSounds.Add( sound );

	sound->SetLoopCount( loopCount );
	sound->SetPosition( position );
	sound->SetHearingDistance( mindist, maxdist );
	sound->SetVolume( volume );
	sound->mSetIntensity = intensity;
	sound->SetCurrentIntensity( intensity*Current3DIntensity );
	if( noSpeedRandomize ) sound->mSoundRateDiff = 1.0f;
	if( dontSave ) sound->Flags |= Miles3DSound::DontSave;
	if( obstructed ) sound->Flags |= Miles3DSound::Obstructed; else sound->Flags &= ~Miles3DSound::Obstructed;
	if( playAtOnce ) sound->Play();
	return sound->mSoundIndex;
}
/*
void MilesEngine::Sound2D_SetPriority( int index, unsigned char priority )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( s ) (*s)->mPriority = priority;
}
*/
/*
void MilesEngine::Sound3D_SetPriority( int index, unsigned char priority )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( s ) (*s)->mPriority = priority;
}
*/
void MilesEngine::UpdateAllSoundsToPrivileged( void )
{
	if( mPrivilegedSounds.Size() )	// to privileged; else from
	{
		PrivilegedSound s = mPrivilegedSounds[0];

		for( int i = 0; i < m2DRealSounds.Size(); ++i )
			if( s.Is3D || ( s.Index != m2DRealSounds[i]->mSoundIndex ) )
			{
				m2DRealSounds[i]->mDesiredTime = GetCurrentTime();
				m2DRealSounds[i]->mDesiredVolume = m2DRealSounds[i]->mVolume;
				m2DRealSounds[i]->UpdateVolume();
		}

		for( int i = 0; i < m3DRealSounds.Size(); ++i )
			if( !s.Is3D || ( s.Index != m3DRealSounds[i]->mSoundIndex ) )
			{
				m3DRealSounds[i]->mDesiredTime = GetCurrentTime();
				m3DRealSounds[i]->mDesiredVolume = m3DRealSounds[i]->mVolume;
				m3DRealSounds[i]->UpdateVolume();
			}
	}
	else
	{
		for( int i = 0; i < m2DRealSounds.Size(); ++i )
		{
			m2DRealSounds[i]->mDesiredTime = GetCurrentTime();
			m2DRealSounds[i]->mDesiredVolume = m2DRealSounds[i]->mVolume;
			m2DRealSounds[i]->UpdateVolume();
		}

		for( int i = 0; i < m3DRealSounds.Size(); ++i )
		{
			m3DRealSounds[i]->mDesiredTime = GetCurrentTime();
			m3DRealSounds[i]->mDesiredVolume = m3DRealSounds[i]->mVolume;
			m3DRealSounds[i]->UpdateVolume();
		}
	}
}

/*
void MilesEngine::Sound2D_SetPrivileged( int index, bool privileged )
{
	Miles2DSound** s = m2DSoundHash.Find( index );
	if( !s ) return;

	// 1. check if we need to change anything

	if( ( ( (*s)->Flags & Miles2DSound::Privileged ) && privileged ) ||
		( !( (*s)->Flags & Miles2DSound::Privileged ) && !privileged ) )
		return;

	// 2. if true, change the info in sound
	// 3. change the privileged sounds list
	bool wasEmpty = ( mPrivilegedSounds.Size() > 0 );

	if( privileged )
	{
		(*s)->Flags |= Miles2DSound::Privileged;
		PrivilegedSound t;
		t.Index = (*s)->mSoundIndex;
		t.Is3D = false;
		mPrivilegedSounds.Add( t );
	}
	else
	{
		(*s)->Flags &= ~Miles2DSound::Privileged;
		for( int i = 0; i < mPrivilegedSounds.Size(); ++i )
			if( !mPrivilegedSounds[i].Is3D && ( mPrivilegedSounds[i].Index == (*s)->mSoundIndex ) )
			{
				mPrivilegedSounds.FastRemove(i);
				break;
			}
	}

	bool isEmpty = ( mPrivilegedSounds.Size() > 0 );

	// 4. change volume of all sounds if need be
	if( isEmpty != wasEmpty )
		UpdateAllSoundsToPrivileged();
	else // 5. if no need for all, just change volume in this sound if need be
	{
		(*s)->mDesiredTime = GetCurrentTime();
		(*s)->mDesiredVolume = (*s)->mVolume;
		(*s)->UpdateVolume();
	}

	// TO DO - remember that all privileged sounds have 255 priority in ToStart and ToStop and make it so
}
*/
/*
void MilesEngine::Sound3D_SetPrivileged( int index, bool privileged )
{
	Miles3DSound** s = m3DSoundHash.Find( index );
	if( !s ) return;

	// 1. check if we need to change anything

	if( ( ( (*s)->Flags & Miles3DSound::Privileged ) && privileged ) ||
		( !( (*s)->Flags & Miles3DSound::Privileged ) && !privileged ) )
		return;

	// 2. if true, change the info in sound
	// 3. change the privileged sounds list
	bool wasEmpty = ( mPrivilegedSounds.Size() > 0 );

	if( privileged )
	{
		(*s)->Flags |= Miles3DSound::Privileged;
		PrivilegedSound t;
		t.Index = (*s)->mSoundIndex;
		t.Is3D = true;
		mPrivilegedSounds.Add( t );
	}
	else
	{
		(*s)->Flags &= ~Miles3DSound::Privileged;
		for( int i = 0; i < mPrivilegedSounds.Size(); ++i )
			if( mPrivilegedSounds[i].Is3D && ( mPrivilegedSounds[i].Index == (*s)->mSoundIndex ) )
			{
				mPrivilegedSounds.FastRemove(i);
				break;
			}
	}

	bool isEmpty = ( mPrivilegedSounds.Size() > 0 );

	// 4. change volume of all sounds if need be
	if( isEmpty != wasEmpty )
		UpdateAllSoundsToPrivileged();
	else // 5. if no need for all, just change volume in this sound if need be
	{
		(*s)->mDesiredTime = GetCurrentTime();
		(*s)->mDesiredVolume = (*s)->mVolume;
		(*s)->UpdateVolume();
	}
}
*/
void MilesEngine::Dump( void )
{
	TRACE( "-------- AUDIO DUMP START -------------\n" );
#ifdef	TRANSGAMING
	TRACE( "Dump frame: %d\n", GEngine->pcfWorld->Frame );
#else
	TRACE( "Dump frame: %d\n", GEngine->World->Frame );
#endif

	DWORD t = GetCurrentTime();

	// Save internal engine values
	TRACE( "*** Internal Engine Data ***\n" );
	TRACE( "Falloff speed: %f\n", mCurrentFalloffSpeed );
	TRACE( "Filled files: %d of max %d\n", mFilledFiles, mMaxFiles );
	TRACE( "Low pass frequency percent: %f\n", LowPassFrequencyPercent );
	TRACE( "Low pass for new sounds: %f\n", mLowPassForNewSounds );
	TRACE( "Volume multiplier: %f\n", mVolumeMultiplier );

	TRACE( "Speed: %f\n", Speed );
	if( mDesiredSpeed != Speed )
	{
		TRACE( " - Desired speed: %f\n", mDesiredSpeed );
		TRACE( " - Last set speed time: %d ms ago\n", t-mLastSetSpeedTime );
		TRACE( " - Desired speed in: %d ms\n", t-mDesiredSpeedTime );
	}

	TRACE( "Heightening obstruction delay: %f\n", HeightenObstructionTime );
	TRACE( "Lowering obstruction delay: %f\n", LowerObstructionTime );
	TRACE( "Minimal obstruction: %f\n", MinObstruction );
	TRACE( "Maximal obstruction low: %f\n", MaxObstructionLow );
	TRACE( "Maximal obstruction high: %f\n", MaxObstructionHigh );
	TRACE( "Minimal obstruction distance: %f\n", MinObstructionDistance );
	TRACE( "Maximal obstruction distance: %f\n", MaxObstructionDistance );

/*
	TRACE( "\n" );
	TRACE( "*** Loaded Files Data ***\n" );
	for( int i = 0; i < mLoadedFiles.Size(); ++i )
		mLoadedFiles[i]->Dump();
*/
	TRACE( "\n" );
	TRACE( "*** Streaming Sounds Data ***\n" );
	for( int i = 0; i < mStreams.Size(); ++i )
	{
		TRACE( "----------- STREAMSOUND ---------\n" );
		if( mStreams[i].Valid )
		{
			TRACE( "Stream: %d\n", i );
			TRACE( "Name: %s\n", mStreams[i].Name.AsChar() );
			TRACE( "Low pass: %f\n", mStreams[i].LowPass );
			TRACE( "Speed: %d\n", mStreams[i].Speed );
			TRACE( "Volume: %f\n", mStreams[i].Volume );
			TRACE( "Position: %d\n", AIL_stream_position(mStreams[i].Stream) );
			S32 datarate, type, length, memory;
			AIL_stream_info( mStreams[i].Stream, &datarate, &type, &length, &memory );
			TRACE( "Max position: %d\n", length );
			TRACE( "Datarate: %d\n", datarate );
			TRACE( "Loop count: %d\n", mStreams[i].LoopCount );
		}
		else
			TRACE( "Stream %d: empty\n", i );
		TRACE( "---------------------------------\n" );
	}

	TRACE( "\n" );
	TRACE( "*** 2D Sounds Data ***\n" );
	for( int i = 0; i < m2DSounds.Size(); ++i ) m2DSounds[i]->Dump();

	TRACE( "\n" );
	TRACE( "*** 3D Sounds Data ***\n" );
	for( int i = 0; i < m3DSounds.Size(); ++i ) m3DSounds[i]->Dump();

	TRACE( "-------- AUDIO DUMP END ---------------\n" );
}

void MilesEngine::GetSoundCounts( int& sounds2D, int& sounds3D, int& streams, int& sounds2DHeard, int& sounds3DHeard, int& streamsHeard )
{
	sounds2D = m2DSounds.Size();
	sounds3D = m3DSounds.Size();
	streams = mStreams.Size() - mStreamsFill.Size();
	sounds2DHeard = m2DRealSamples.Size();
	sounds3DHeard = m3DRealSamples.Size();
	streamsHeard = 0;
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid && StreamingSound_IsPlaying( i ) )
			streamsHeard++;
}

void MilesEngine::StopAllSounds( void )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::StopAllSounds );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::StopAllSounds );
#endif

	StopFinishedSounds();

	// Stop 2D sounds
	for( int i = 0; i < m2DSounds.Size(); ++i )
		if( m2DSounds[i]->IsPlaying() )
			m2DSounds[i]->Stop();

	// Stop 3D sounds
	for( int i = 0; i < m3DSounds.Size(); ++i )
		if( m3DSounds[i]->IsPlaying() )
			m3DSounds[i]->Stop();

	// Stop streams
	for( int i = 0; i < mStreams.Size(); ++i )
		if( mStreams[i].Valid )
			AIL_pause_stream( mStreams[i].Stream, 1 );
}

void MilesEngine::SetPlayerPosition( Vector& position )
{
#ifdef	TRANSGAMING
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::SetPlayerPosition, position.X, position.Y, position.Z );
#else
	if( mAudioRecorder ) mAudioRecorder->SetAction( AudioRecorder::AudioRecordingActions::SetPlayerPosition, position.X, position.Y, position.Z );
#endif
//	if( Listener )
		AIL_set_listener_3D_position( DigitalDriver, position.X, position.Y, -position.Z );
}


