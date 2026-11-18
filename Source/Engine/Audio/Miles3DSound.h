
// #pragma once
#ifndef _MILES3DSOUND_
#define _MILES3DSOUND_

#include "Audio.h"

class MilesLoadedFile;
class FileObject;

class Miles3DSound
{
	friend class Sound3D;
	friend class MilesEngine;
private:

	enum
	{
		DeleteIfStopped			= (1<<0),
		WannaPlay				= (1<<1),
		Privileged				= (1<<2),
		HasAlwaysSameSpeed		= (1<<3),
		DontSave				= (1<<4),
		Obstructed				= (1<<5),
	};

	// Methods

	Miles3DSound( MilesLoadedFile* file, MilesEngine* miles, int soundIndex );
	~Miles3DSound();

	// Load/save
	void LoadSound( FileObject* f );
	bool SaveSound( FileObject* f );

	// Commands
	void Play( void );
	void Stop( void );
	void Resume( void );

	// Properties
	void SetLoopCount( int count );
	int GetLoopCount( void );
	void SetLowPass( float lp );
	void SetAlwaysSameSpeed( bool set );
	bool IsPlaying( void );

	// 3D
	void SetHearingDistance( float minDist, float maxDist );
	void GetHearingDistance( float& minDist, float& maxDist );
	void SetPosition( Vector& position );
	Vector GetPosition( void );
	void SetOrientation( Vector& front, Vector& up );
	void GetOrientation( Vector& front, Vector& up );
	void SetVelocity( Vector& velocity );
	Vector GetVelocity( void );

	// Volume
	void SetVolume( float volume, float switchTime = 0.0f );
	void UpdateVolume( void );

	// Intensity
	void SetCurrentIntensity( float intensity, float switchTime = 0.0f );
	void UpdateIntensity( void );

	// Obstruction
	void SetCurrentObstruction( float obstruction, float switchTime = 0.0f );
	void UpdateObstruction( void );

	// Internal maintenance
	bool ConnectToSample( int sampleIndex );
	void DisconnectFromSample( void );

	void Dump( void );

	// Members
	unsigned char Flags;
	int mSampleIndex;
	unsigned char mPriority;

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

	// Obstruction settings
	float mDesiredObstruction;
	float mCurrentObstruction;
	DWORD mDesiredObstructionTime;
	DWORD mLastSetObstructionTime;

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
	MilesEngine* mMiles;
	int mSoundIndex;

	float mLowPass;
	float mRealLowPass;		// this is noted every time I set occlusion, and next occlusion isn't set if equal to this
	float mSoundRateDiff;
};

#endif //_MILES3DSOUND_

