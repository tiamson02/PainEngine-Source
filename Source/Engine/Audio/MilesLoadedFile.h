
// #pragma once
#ifndef _MILESLOADEDFILE_
#define _MILESLOADEDFILE_


#include "Core/LeString.h"
#include "Audio.h"
#include "FileSystem/GFileManager.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

extern String GetCorrectFileName(LPCSTR fname);

class MilesLoadedFile
{
public:
	MilesLoadedFile( const char* name, int defaultInstances, DWORD defaultDelay )
	{
		mFilePointer = 0;
		mFileName = GetCorrectFileName( name );
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
		if( mReferenceCount != 0 )
			GLog.Print( "AUDIO: Deleting data for file '%s', reference count == %d\n", mFileName.AsChar(), mReferenceCount );
		if( mFilePointer )
		{
			AIL_mem_free_lock( mFilePointer );
			mFilePointer = 0;
		}
	}

	void FillFile( void* filePointer )
	{
		mFilePointer = filePointer;
		// TO DO
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
		if( mMaximalInstances <= mPlayingReferenceCount ) return false;
		if( t - mLastStartedTime < mMinimalStartDelay ) return false;
		return true;
	}

	bool CanPlayFileTime( void ) { return true; } //( GetCurrentTime() - mLastStartedTime >= mMinimalStartDelay ); }
	bool CanPlayFileInstances( void ) { return( mMaximalInstances > mPlayingReferenceCount ); }

	void PlayFile( bool rememberTime = true )
	{
		if( rememberTime ) mLastStartedTime = GetCurrentTime();
		mPlayingReferenceCount++;
	};

	void StopFile( void )
	{
		Assert( mPlayingReferenceCount > 0 );
		mPlayingReferenceCount--;
	};

	void LoadFile( GFile* f, MilesEngine* miles )
	{
		DWORD t = GetCurrentTime();
		DWORD temp;

		*f >> mFileName;

		mFileName = GetCorrectFileName( mFileName.AsChar());

		*f >> temp;
		mLastUsedTime = t - temp;
		if( mLastUsedTime > t ) mLastUsedTime = 0;

		*f >> temp;
		mLastStartedTime = t - temp;
		if( mLastStartedTime > t ) mLastStartedTime = 0;

		*f >> mMinimalStartDelay;
		*f >> mMaximalInstances;
		*f >> mPlayingReferenceCount;
		*f >> mReferenceCount;

		char notEmpty;
		*f >> notEmpty;
		if( notEmpty )
		{
			mFilePointer = miles->LoadAndUnpackFile( mFileName.AsChar() );
			Assert( mFilePointer );
			mFileSize = AIL_file_size( mFileName.AsChar() );
			AIL_WAV_info( mFilePointer, &mFileSoundInfo );
		}
	}

	void SaveFile( GFile* f )
	{
		DWORD t = GetCurrentTime();

		*f << mFileName;
		*f << t-mLastUsedTime;
		*f << t-mLastStartedTime;
		*f << mMinimalStartDelay;
		*f << mMaximalInstances;
		*f << mPlayingReferenceCount;
		*f << mReferenceCount;

		if( mFilePointer )
			*f << (char)1;
		else
			*f << (char)0;
	}

	void Dump( void )
	{
		GLog.Print( "------------- FILE --------------\n" );

		DWORD t = GetCurrentTime();

		GLog.Print( "Filename: %s\n", mFileName.AsChar() );
		GLog.Print( "Last used time: %d ms ago\n", t-mLastUsedTime );
		GLog.Print( "Last started time: %d ms ago\n", t-mLastStartedTime );
		GLog.Print( "Minimal start delay: %d ms\n", mMinimalStartDelay );
		GLog.Print( "Maximal instances: %d\n", mMaximalInstances );
		GLog.Print( "Now playing: %d times\n", mPlayingReferenceCount );
		GLog.Print( "Referenced %d times\n", mReferenceCount );
		GLog.Print( mFilePointer?"Has file pointer\n":"No file pointer\n" );

		GLog.Print( "---------------------------------\n" );
	}

	void*			mFilePointer;
	S32				mFileSize;
	AILSOUNDINFO	mFileSoundInfo;

	String	mFileName;
	DWORD	mLastUsedTime;

private:
	DWORD	mLastStartedTime;
	DWORD	mMinimalStartDelay;

	int		mPlayingReferenceCount;
	int		mMaximalInstances;

	int		mReferenceCount;
};

#endif// _MILESLOADEDFILE_
