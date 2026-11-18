
// #pragma once
#ifndef _SOUND_
#define _SOUND_


#include "Engine/Entity.h"

// ==========================================================================
class ENGINE_API Sound : public Entity
{

public:

protected:

	enum
	{
		Is3D			= (1<<0),
		IsObstructed	= (1<<1),
		IsStarted		= (1<<2),
		IsStopped		= (1<<3),
		AutoDelete		= (1<<4),
	};

    int SoundIndex;
    float Volume, Balance, Interval, RandomShift, FalloffStart, FalloffEnd, Timer, FadeIn, VelScale; 
    float fade;
    // Interval -- w jakich odstepach czasowych odtwarzac kolejne dzwieki
    // Interval  < 0 - once
    // Interval == 0 - looped
    // Interval  > 0 - sec. delay
    // RandomShift -- losowe przesuniecie dodawane do intervalu (o ile i > 0)
    // time = Interval + Rand(-RandomShift,RandomShift)
	unsigned char SoundFlags;
    String CurrentSoundName;

    void Release(bool now = true);

public:
    Sound();
    ~Sound();

	// Load/save
	virtual bool LoadEntity( FileObject* f );
	virtual bool SaveEntity( FileObject* f );
	void EnableDraw(bool) {}

    void Setup2D(LPCSTR fileName, float volume, float balance);
    void Setup3D(LPCSTR fileName, float falloffStart, float falloffEnd, float interval = -1.0f, float randomShift = 0.0f, bool autoDelete = true);

    void Play(float fadeIn = -1.0f);
    void SetVelocityScaleFactor(float factor);
    void Stop();
    void Tick(const float timeDelta);
    void SetPosition(const Vector &v);
    void UpdateTransform(bool updateInWorld);
	int GetSound3DIndex();
};



#endif //_SOUND_

