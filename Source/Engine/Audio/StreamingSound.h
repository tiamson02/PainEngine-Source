
#pragma once

class MilesEngine;

class ENGINE_API StreamingSound
{
public:
	StreamingSound( int internalIndex, MilesEngine* miles ) { mInternalIndex = internalIndex; mMiles = miles; };
	~StreamingSound();

	void Play( void );
	void Stop( void );
	void Resume( void );

	// Low pass filter
	inline void SetLowPass( float lp );
	inline float GetLowPass( void );

	// Loop count
	void SetLoopCount( int count );									// 0 - infinite loop
	int GetLoopCount( void );										// 0 - infinite loop

	void SetVolumeAndPan( float volume, float pan );		// range 0-1
	void GetVolumeAndPan( float& volume, float& pan );

	bool IsPlaying( void );

protected:
	int mInternalIndex;
	MilesEngine* mMiles;
};
