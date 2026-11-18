
// #pragma once
#ifndef _MILES2DSOUND_
#define _MILES2DSOUND_


#include "FileSystem/GFile.h"

class MilesLoadedFile;
class FileObject;
class SoundList;

class Miles2DSound
{
	friend class Sound2D;
	friend class MilesEngine;

private:

	enum
	{
		DeleteIfStopped			= (1<<0),
		WannaPlay				= (1<<1),
		Privileged				= (1<<2),
		HasAlwaysSameSpeed		= (1<<3),
		DontSave				= (1<<4),
	};

	Miles2DSound( MilesLoadedFile* file, MilesEngine* miles, int soundIndex );
	~Miles2DSound();

	// Load/save
	void LoadSound( FileObject* f );
	bool SaveSound( FileObject* f );

	void Play( void );
	void Stop( void );
	void Resume( void );
	void SetLoopCount( int count );
	int GetLoopCount( void );
	bool IsPlaying( void );

	void SetVolume( float volume, float switchTime = 0.0f );
	void UpdateVolume( void );

	void SetCurrentIntensity( float intensity, float switchTime = 0.0f );
	void UpdateIntensity( void );

	void SetLowPass( float lp );
	void SetAlwaysSameSpeed( bool set );

	bool ConnectToSample( int sampleIndex );
	void DisconnectFromSample( void );

	void Dump( void );

	unsigned char Flags;

	int mSoundIndex;
	unsigned char mPriority;

	int mSampleIndex;
	unsigned int mOffset;		// measured in bytes
	MilesLoadedFile* mFile;
	int mLoopCount;
	DWORD mPlayStart;			// measured in bytes
	DWORD mPlayLength;			// measured in bytes
	MilesEngine* mMiles;

	// Volume settings
	float mVolume;
	float mDesiredVolume;
	DWORD mDesiredTime;
	DWORD mLastSetVolumeTime;

	// Intensity settings
	float mSetIntensity;

	float mCurrentIntensity;
	float mDesiredIntensity;
	DWORD mDesiredIntensityTime;
	DWORD mLastSetIntensityTime;

	float mLowPass;
	float mRealLowPass;
	float mSoundRateDiff;
};

#endif //_MILES2DSOUND_

