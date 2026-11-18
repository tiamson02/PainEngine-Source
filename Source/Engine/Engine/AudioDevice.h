
#define AUDIO_WARNINGS_ON

#include "Core/Math/LeMath.h"

typedef int SOUNDHANDLE;

class MilesEngine;

enum SpeakerTypes
{
	Speakers2 = 0,
	Headphones = 1,
	Surround = 2,
	Speakers4 = 3,
	Speakers51 = 4,
	Speakers71 = 5,
};

enum RoomTypes
{
	Generic,
	PaddedCell,
	Room,
	Bathroom,
	LivingRoom,
	StoneRoom,
	Auditorium,
	ConcertHall,
	Cave,
	Arena,
	Hangar,
	CarpetedHallway,
	Hallway,
	StoneCorridor,
	Alley,
	Forest,
	City,
	Mountains,
	Quarry,
	Plain,
	ParkingLot,
	SewerPipe,
	UnderWater,
	Drugged,
	Dizzy,
	Psychotic,
};

class MilesLoadedFile;

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

	inline bool IsPlaying( void );

protected:
	Miles2DSound* mSound;
};

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

	inline bool IsPlaying( void );

protected:
	Miles3DSound* mSound;
};

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

class ENGINE_API AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();

    BOOL Init( void );								// make sure main window exists
	inline void Tick( float timeDelta );

	// ****************** Master settings - one for device *******************
//	void ChangeDeviceProperties( int digitalRate, int digitalBits, int digitalChannels );
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
	StreamingSound* CreateStreamingSound( const char* name );
	void DeleteStreamingSound( StreamingSound* sound );
	void GetStreamNames( DynamicArray<String>& names );
	void StartStreams( DynamicArray<String>& names );

	// ****************** 2D audio methods ***********************************
	void Play2DSound( const char* name, float volume = 1.0f, bool playAtOnce = false );
	Sound2D* Create2DSound( const char* name, bool playAtOnce = false );
	void Delete2DSound( Sound2D* sound );

	// ****************** 3D audio methods ***********************************
	Sound3D* Create3DSound( const char* name, bool playAtOnce = false );	// create new sound
	void Delete3DSound( Sound3D* sound );

protected:
	MilesEngine* mMiles;
	float mCurrentFalloffSpeed;
};
