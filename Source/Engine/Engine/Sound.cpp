#include "Engine.h"
#include "Render/View.h"
//#include "Audio/AudioDevice.h"
#ifndef TRANSGAMING_NO_MILES
#include "Audio/MilesEngine.h"
#endif
#include "Engine/Sound.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/World.h"
#include "FileSystem/GFChunk.h"

extern ENGINE_API LogBuffer GLog;

// =========================================================================
Sound::Sound()
:	Entity(Entity::ETypeSound)
,	VelScale(1.0f)
,	Interval(0.0f)
,	RandomShift(0.0f)
,	FalloffStart(5.0f)
,	FalloffEnd(10.0f)
,	Volume(100.0f)
,	Timer(0)
,	FadeIn(-1.0f)
,	SoundIndex(-1)
,	SoundFlags(Is3D|IsStopped|AutoDelete)
{
    Flags |= Entity::EEntNoDraw;
#ifndef TRANSGAMING_NO_MILES
	if( GEngine->Audio && GEngine->Audio->GetDefaultObstruction() )
		SoundFlags |= IsObstructed;
#endif
}
// =========================================================================
Sound::~Sound()
{
    Release();
}
// =========================================================================
void Sound::Release(bool now)
{
#ifdef TRANSGAMING_NO_MILES
	// Currently, we don't support Miles...
#else
    if( SoundIndex != -1 )
    {
        if( SoundFlags & Is3D )
        {
            if( !now && GEngine->Audio->Sound3D_IsPlaying( SoundIndex ) )
                GEngine->Audio->Sound3D_Forget( SoundIndex );
            else
            {
                GEngine->Audio->Sound3D_Stop( SoundIndex );
                GEngine->Audio->Sound3D_Delete( SoundIndex );
            }
        }
        else
        {
            if( !now && GEngine->Audio->Sound2D_IsPlaying( SoundIndex ) )
                GEngine->Audio->Sound2D_Forget( SoundIndex );
            else
            {
                GEngine->Audio->Sound2D_Stop( SoundIndex );
                GEngine->Audio->Sound2D_Delete( SoundIndex );
            }
        }
    }
#endif
    SoundIndex = -1;
}
// =========================================================================
void Sound::Setup2D(LPCSTR fileName, float volume, float balance)
{
    FileName = fileName;
    Volume = volume;
    Balance = balance;
    SoundFlags &= ~Is3D;
}
// =========================================================================
void Sound::Setup3D(LPCSTR fileName, float falloffStart, float falloffEnd, float interval, float randomShift, bool doNotAutoDelete)
{
    FileName = fileName;
	SoundFlags |= Is3D;
    FalloffStart = falloffStart;
    FalloffEnd = falloffEnd;
    Interval = interval;
    RandomShift = randomShift;
	if( doNotAutoDelete )
		SoundFlags &= ~AutoDelete;
	else
		SoundFlags |= AutoDelete;

}
// =========================================================================
void Sound::Play(float fadeIn)
{
    Release(false);
    SoundFlags &= ~IsStopped;

    FadeIn = fadeIn;
    fade   = fadeIn;

#ifdef TRANSGAMING
    DynamicArray<const char *>  stab;
    char *                      tmp = new char[FileName.Len() + 1];
    char *                      tok;


    strncpy(tmp, FileName.AsChar(), FileName.Len() + 1);

    tok = strtok(tmp, ";");
    while (tok){
        stab.Add(tok);
        tok = strtok(NULL, ";");
    }

    CurrentSoundName = stab[rand() % stab.Size()];
    stab.ClearArray();
    delete []tmp;
#else
    DynamicArray<String> stab;
    String::Tokenize(FileName.AsChar(),";",stab);

    int i = rand()%stab.Size();
    CurrentSoundName = stab[i];
#endif

    static char buf[MAX_PATH];
#ifndef TRANSGAMING_NO_MILES	// @todo remove this once psp sound is implemented
    sprintf(buf,soundFileFormatStr,CurrentSoundName.AsChar());
#endif
    int loops = 1;
    if (Interval == 0)
    {
        if (!strchr(FileName.AsChar(),';')) // manual random loop
            loops = 0;
    }

#ifdef TRANSGAMING_NO_MILES
	// Currently, we don't support Bink...
#else

    if (SoundFlags&Is3D)
    {
        View * view = GEngine->Video->GetActiveView();
        Vector pos = ObjectToWorld.GetTranslation();
        if (loops > 0 && Vector(view->mCamera.GetPosition()-pos).Magnitude() > FalloffEnd*2) // sprawdzam odleglosc od kamery
        {
            if (Interval < 0)
                WorldLink->DeleteEntityDelayed(this);
            return;
        }

		SoundIndex = GEngine->Audio->Sound3D_CreateEx( buf, 0, true, loops, pos, FalloffStart, FalloffEnd,
			(FadeIn>0)?0.0f:Volume/100.0f, true, true, (SoundFlags&IsObstructed)!=0 );
        if( SoundIndex == -1 )
			GLog.Print( "ERROR: BindSound: Sound not found: %s.", CurrentSoundName);
/*
        SoundIndex = GEngine->Audio->Sound3D_Create( buf, 0 );
        if( SoundIndex != -1 )
        {
			GEngine->Audio->Sound3D_SetDontSave( SoundIndex );
			GEngine->Audio->Sound3D_SetPosition( SoundIndex, pos );
			GEngine->Audio->Sound3D_SetLoopCount( SoundIndex, loops );
			GEngine->Audio->Sound3D_SetVolume( SoundIndex, (FadeIn>0)?0.0f:Volume/100.0f );
			GEngine->Audio->Sound3D_SetHearingDistance( SoundIndex, FalloffStart, FalloffEnd );
			GEngine->Audio->Sound3D_SetObstructed( SoundIndex, (SoundFlags&IsObstructed)!=0 );
			GEngine->Audio->Sound3D_SetNoSpeedRandomize( SoundIndex );
			GEngine->Audio->Sound3D_Play( SoundIndex );
        }
		else
			GLog.Print( "ERROR: BindSound: Sound not found: %s.", CurrentSoundName);
*/
    }
    else
    {
        SoundIndex = GEngine->Audio->Sound2D_Create( buf, 0 );
		if( SoundIndex != -1 )
		{
			GEngine->Audio->Sound2D_SetDontSave( SoundIndex );
			GEngine->Audio->Sound2D_SetLoopCount( SoundIndex, loops );
			GEngine->Audio->Sound2D_SetVolume( SoundIndex, (FadeIn>0)?0.0f:(Volume/100.0f) );
			GEngine->Audio->Sound2D_NoSpeedRandomize( SoundIndex );
			GEngine->Audio->Sound2D_Play( SoundIndex );
		}
		else
			GLog.Print( "ERROR: BindSound: Sound not found: %s.", CurrentSoundName);
    }
#endif
}
// =========================================================================
void Sound::Stop()
{
	if( SoundIndex == -1 ) return;
#ifndef TRANSGAMING_NO_MILES
    if( SoundFlags & Is3D )
		GEngine->Audio->Sound3D_Stop( SoundIndex );
#endif
    SoundFlags |= IsStopped;
}
// =========================================================================
int Sound::GetSound3DIndex()
{
	return ( SoundFlags & Is3D )?SoundIndex:-1;
}
// =========================================================================
void Sound::Tick(const float timeDelta)
{
    if( fade > 0 && ( SoundIndex != -1 ) )
    {
        fade -= timeDelta;
        float v = (Volume/100.0f) * 1.0f-(fade/FadeIn);

        if (fade < 0)
        {
            fade = -1.0f;
            v = Volume;
        }

#ifndef TRANSGAMING_NO_MILES
		if( SoundFlags & Is3D )
			GEngine->Audio->Sound3D_SetVolume( SoundIndex, v );
		else
			GEngine->Audio->Sound2D_SetVolume( SoundIndex, v );
#endif
    }

    if (Interval > 0)
    {
        Timer -= timeDelta;
        if (Timer <= 0)
        {
            Play();
            Timer = Interval + ((float) rand() / (float) RAND_MAX) * RandomShift;
        }
        return;
    }

    if( SoundIndex == -1 )
        return;
#ifndef TRANSGAMING_NO_MILES
	bool playing = (SoundFlags&Is3D)?GEngine->Audio->Sound3D_IsPlaying( SoundIndex ):GEngine->Audio->Sound2D_IsPlaying( SoundIndex );
    if( !playing && !(SoundFlags&IsStopped) )
    {
        if (Interval<0 && (SoundFlags&AutoDelete))
            WorldLink->DeleteEntityDelayed(this);
        else
            Play();
    }
#endif
}
// =========================================================================
void Sound::SetPosition(const Vector &v)
{
    Entity::SetPosition(v);
    if( ( SoundIndex != -1 ) && ( SoundFlags & Is3D ) )
    {
#ifndef TRANSGAMING_NO_MILES
        Vector p = ObjectToWorld.GetTranslation();
		GEngine->Audio->Sound3D_SetPosition( SoundIndex, p );
#endif
    }
}
// =========================================================================
void Sound::UpdateTransform(bool updateInWorld)
{
    Entity::UpdateTransform(false);
    if( ( SoundIndex != -1 ) && ( SoundFlags & Is3D ) )
    {
#ifndef TRANSGAMING_NO_MILES
        Vector p = ObjectToWorld.GetTranslation();
		GEngine->Audio->Sound3D_SetPosition( SoundIndex, p );
        if( Parent && Parent->PO )
        {
            Vector v = Parent->PO->GetVel() * VelScale;
			GEngine->Audio->Sound3D_SetVelocity( SoundIndex, v );
        }
#endif
    }
}
// =========================================================================
void Sound::SetVelocityScaleFactor(float factor)
{
    VelScale = factor;
}
// =========================================================================
#define SOUND_LOAD_SAVE_TOKEN 0x5e56

bool Sound::LoadEntity( FileObject* f )
{
	if( !f->TestToken( SOUND_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Sound::LoadEntity()\n" );
		return false;
	}

	Entity::LoadEntity(f);

	*f >> Volume >> Balance >> Interval >> RandomShift >> FalloffStart >> FalloffEnd >> Timer >> FadeIn >> VelScale >> fade;
	*f >> CurrentSoundName >> SoundFlags;

	GFChunkReader ch(f);
	char sndptr;
	*f >> sndptr;
	if( sndptr )	// Create and load Sound3D or Sound2D
	{
#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
			SoundIndex = (SoundFlags&Is3D)?GEngine->Audio->Sound3D_Create(f):GEngine->Audio->Sound2D_Create( f );
		else
		{
			SoundIndex = -1;
			ch.Next(f);
		}
#endif
	}

	return true;
}
// =========================================================================
bool Sound::SaveEntity( FileObject* f )
{
	// Data for World to recreate us
	*f << (int)ETypeSound << FileName << Name << (float)1.0f << (char)0;

	if( !f->PutToken( SOUND_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	*f << Volume << Balance << Interval << RandomShift << FalloffStart << FalloffEnd << Timer << FadeIn << VelScale << fade;
	*f << CurrentSoundName << SoundFlags;

	GFChunkWriter ch(f);
	if( SoundIndex != -1 )
	{
#ifndef TRANSGAMING_NO_MILES
		*f << (char)1;
		if( SoundFlags & Is3D )		// Save 3D sound
		{
			if( !GEngine->Audio->Sound3D_Save( SoundIndex, f ) )
				return false;
		}
		else						// Save 2D sound
		{
			if( !GEngine->Audio->Sound2D_Save( SoundIndex, f ) )
				return false;
		}
#endif
	}
	else
		*f << (char)0;
	ch.Complete(f);

	return true;
}

