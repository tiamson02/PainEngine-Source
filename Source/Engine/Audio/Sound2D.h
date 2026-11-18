
#pragma once

class MilesLoadedFile;
class MilesEngine;
class FileObject;

class ENGINE_API Sound2D
{
	friend class Miles2DSound;
	friend class AudioDevice;
public:
	Sound2D( MilesLoadedFile* file, MilesEngine* miles );
	~Sound2D();

	inline void Play( void );
	inline void PlayAndForget( void );
	inline void Forget( void );
	inline void Stop( void );
	inline void Resume( void );

	// Load/save
	Sound2D( FileObject* f );
	inline bool SaveSound( FileObject* f );

	// Sound volume ( 0.0-1.0 )
	inline void SetVolume( float volume, float switchTime = 0.0f );
	inline float GetVolume( void );

	// Loop count
	inline void SetLoopCount( int count );									// 0 - infinite loop
	inline int GetLoopCount( void );										// 0 - infinite loop

	// Low pass filter
	inline void SetLowPass( float lp );
	inline float GetLowPass( void );

	// Using volume multiplier
	inline void UseVolumeMultiplier( bool use );
	inline bool UsingVolumeMultiplier( void );

	// Has always same speed
	inline void SetAlwaysSameSpeed( bool set );
	inline bool HasAlwaysSameSpeed( void );

	inline void NoSpeedRandomize( void );
	inline void SetDontSave( void );

	inline bool IsPlaying( void );

protected:
	Miles2DSound* mSound;
};
