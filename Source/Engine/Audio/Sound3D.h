
#pragma once

#include "Audio.h"

class MilesLoadedFile;
class MilesEngine;

class ENGINE_API Sound3D
{
	friend class Miles3DSound;
	friend class AudioDevice;
protected:
	Sound3D( MilesLoadedFile* file, MilesEngine* miles );
public:
	~Sound3D();

	inline void Play( void );
	inline void PlayAndForget( void );
	inline void Forget( void );
	inline void Stop( void );
	inline void Resume( void );

	// Load/save
	Sound3D( FileObject* f );
	inline bool SaveSound( FileObject* f );

	// Sound volume ( 0.0-1.0 )
	inline void SetVolume( float volume, float switchTime = 0.0f );
	inline float GetVolume( void );

	// Loop count
	inline void SetLoopCount( int count );		// 0 - infinite loop
	inline int GetLoopCount( void );			// 0 - infinite loop

	// Low pass filter
	inline void SetLowPass( float lp );
	inline float GetLowPass( void );

	// Hearing distance
	inline void SetHearingDistance( float minDist, float maxDist );		// minDist MUST be higher than 0.0f
	inline void GetHearingDistance( float& minDist, float& maxDist );

	// Position
	inline void SetPosition( Vector& position );
	inline Vector GetPosition( void );

	// Orientation
	inline void SetOrientation( Vector& front, Vector& up );	// Those are used only with samples with cone
	inline void GetOrientation( Vector& front, Vector& up );	// Those are used only with samples with cone

	// Velocity
	inline void SetVelocity( Vector& velocity );
	inline Vector GetVelocity( void );

	// Using volume multiplier
	inline void UseVolumeMultiplier( bool use );
	inline bool UsingVolumeMultiplier( void );

	// Has always same speed
	inline void SetAlwaysSameSpeed( bool set );
	inline bool HasAlwaysSameSpeed( void );

	// Obstruction by fixed geometry
	inline void SetObstructed( bool obstruct );
	inline bool IsObstructed( void );

	inline void NoSpeedRandomize( void );
	inline void SetDontSave( void );

	inline bool IsPlaying( void );

protected:
	Miles3DSound* mSound;
};
