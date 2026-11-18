
#include "Engine.h"
#include "Miles2DSound.h"
#include "MilesEngine.h"
#include "MilesLoadedFile.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"

// --------------------- Miles2DSound methods ---------------------
Miles2DSound::Miles2DSound( MilesLoadedFile* file, MilesEngine* miles, int soundIndex ) : mSampleIndex(-1),
		mLoopCount(1), mMiles(miles), mFile(file), mOffset(0), mPlayStart(0),
		mPlayLength(0), mSoundIndex(soundIndex), Flags(0), mPriority(0),
		mVolume(1.0f), mDesiredVolume(1.0f), mDesiredTime(0.0f), mLastSetVolumeTime(0),
		mSetIntensity(1.0f), mCurrentIntensity(1.0f), mDesiredIntensity(1.0f),
		mDesiredIntensityTime(0.0f), mLastSetIntensityTime(0), mRealLowPass(1.0f)
{
	mLowPass = miles->mLowPassForNewSounds;
	mSoundRateDiff = 1.0f + (2*(((float)rand())/RAND_MAX)-1)*miles->mSoundRateDiffRate;
	if( mSoundIndex != -1 )
		mMiles->m2DSoundHash.Insert( mSoundIndex, this );
}

Miles2DSound::~Miles2DSound()
{
	// Remove from paused sounds stack
	for( int i = 0; i < mMiles->PauseStack.Size(); ++i )
	{
		if( !mMiles->PauseStack[i] ) continue;
		for( int j = 0; j < mMiles->PauseStack[i]->Sounds2D.Size(); ++j )
			if( mMiles->PauseStack[i]->Sounds2D[j] == this )
			{
				mMiles->PauseStack[i]->Sounds2D.FastRemove(j);
				mMiles->PauseStack[i]->Sounds2DDeleteIfStopped.FastRemove(j);
                break;
			}
	}

	for( int i = 0; i < mMiles->m2DSounds.Size(); ++i )
	{
		if( mMiles->m2DSounds[i] == this )
		{
			mMiles->m2DSounds.Remove(i);
			break;
		}
	}

	if( mSampleIndex != -1 )
	{
		mMiles->Release2DSample( mSampleIndex );
		mSampleIndex = -1;
	}

	if( mFile )
		mFile->RemoveReference();

	Assert( mSoundIndex != -1 );
	mMiles->m2DSoundHash.Remove(mSoundIndex);
}

void Miles2DSound::Play( void )
{
	if( Flags & WannaPlay ) Stop();	// stop it first

	mOffset = 0;
	Flags |= WannaPlay;
	mPlayStart = GetCurrentTime();
	mMiles->TryToPlayRealSound2D( this );

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);
			mPlayLength = mLoopCount*playOnceTime;
		}
		else
		{
#ifdef MILES_2DSOUND_DEBUG
			if( mFile->mFilePointer )
				GLog.Print( "Audio Warning: In Miles2DSound::Play(), AIL_WAV_info failed with non-zero WAV file '%s'\n", mFile->mFileName.AsChar() );
#endif
			mPlayLength = 0;
		}
	}
}

void Miles2DSound::Stop( void )
{
	Flags &= ~WannaPlay;
	mPlayLength = 0;
	if( mSampleIndex != -1 )
	{
		mOffset = AIL_sample_position( mMiles->m2DRealSamples[mSampleIndex] );
		Assert( mOffset >= 0 );
		// Get loop count
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_sample_loop_count( mMiles->m2DRealSamples[mSampleIndex] );
		Assert( mLoopCount >= 0 );
#ifdef _DEBUG
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
#endif
		mMiles->Release2DSample( mSampleIndex );
	}
	else
		mOffset = 0;
	mPlayStart = 0;
}

void Miles2DSound::Resume( void )
{
	Flags |= WannaPlay;
	mMiles->TryToPlayRealSound2D( this );
	mPlayStart = GetCurrentTime();

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		AILSOUNDINFO si;
		if( AIL_WAV_info( mFile->mFilePointer, &si ) )
		{
			DWORD playOnceTime = si.samples*1000/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff);
			DWORD offsetTime = mOffset*1000*8/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*si.rate*mSoundRateDiff)/si.bits;
			mPlayLength = mLoopCount*playOnceTime - offsetTime;
		}
		else
		{
#ifdef MILES_2DSOUND_DEBUG
			if( mFile->mFilePointer )
				GLog.Print( "Audio Warning: In Miles2DSound::Resume(), AIL_WAV_info failed with non-zero WAV file '%s'\n", mFile->mFileName.AsChar() );
#endif
			mPlayLength = 0;
		}
	}
}

void Miles2DSound::SetLoopCount( int count )
{
	Assert( count >= 0 );
	bool stopPlay = ( mSampleIndex != -1 );
	if( stopPlay ) Stop();
	mLoopCount = count;
	if( stopPlay )
	{
		bool smao = mMiles->StartManyAtOnce;
		mMiles->StartManyAtOnce = true;
		Resume();
		mMiles->StartManyAtOnce = smao;
	}
}

int Miles2DSound::GetLoopCount( void )
{
	if( mSampleIndex != -1 )
	{
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_sample_loop_count( mMiles->m2DRealSamples[mSampleIndex] );
		Assert( mLoopCount >= 0 );
#ifdef _DEBUG
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
#endif
	}
	return mLoopCount;
}

bool Miles2DSound::IsPlaying( void )
{
	if( mSampleIndex != -1 )
	{
		U32 out = AIL_sample_status( mMiles->m2DRealSamples[mSampleIndex] );
		return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) );
	}
	else if( !(Flags & WannaPlay ) )
		return false;
	else if( mLoopCount == 0 )
		return true;
	else if( mPlayLength == DWORD(-1) )
		return true;
	else
		return( ( GetCurrentTime() - mPlayStart ) < mPlayLength );
}

void Miles2DSound::SetVolume( float volume, float switchTime )
{
	if( volume < 0.0f ) volume = 0.0f;
	if( volume > 1.0f ) volume = 1.0f;

	if( !switchTime )
		mVolume = volume;

	mDesiredVolume = volume;
	mLastSetVolumeTime = GetCurrentTime();
	mDesiredTime = mLastSetVolumeTime + floor(switchTime*1000);

	if( !switchTime && ( mSampleIndex != -1 ) )
	{
		float targetVolume = (mMiles->mPrivilegedSounds.Size()&&((Flags&Privileged)==0))?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
		Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
		AIL_set_sample_volume_levels( mMiles->m2DRealSamples[mSampleIndex], targetVolume, targetVolume );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set volume (in SetVolume) to %f\n", mMiles->m2DRealSamples[mSampleIndex], targetVolume );
#endif
	}
}

void Miles2DSound::SetCurrentIntensity( float intensity, float switchTime )
{
	if( intensity < 0.0f ) intensity = 0.0f;
	if( intensity > 1.0f ) intensity = 1.0f;

	if( !switchTime )
		mCurrentIntensity = intensity;

	mDesiredIntensity = intensity;
	mLastSetIntensityTime = GetCurrentTime();
	mDesiredIntensityTime = mLastSetIntensityTime + floor(switchTime*1000);

	Assert( ( mCurrentIntensity >= 0.0f ) && ( mCurrentIntensity <= 1.0f ) );

	if( !switchTime && ( mSampleIndex != -1 ) )
	{
		AIL_set_sample_reverb_levels( mMiles->m2DRealSamples[mSampleIndex], 1.0f, mCurrentIntensity );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set reverb (in SetCurrentIntensity) to %f\n", mMiles->m2DRealSamples[mSampleIndex], mCurrentIntensity );
#endif
	}
}

void Miles2DSound::SetLowPass( float lp )
{
	if( lp < 0.0f ) lp = 0.0f;
	if( lp > 1.0f ) lp = 1.0f;

	mLowPass = lp;
	float percent = lp;
	if( !(Flags&HasAlwaysSameSpeed) && ( mMiles->LowPassFrequencyPercent < lp ) )
		percent = mMiles->LowPassFrequencyPercent;

	if( ( mSampleIndex != -1 ) && ( mRealLowPass != percent ) )
	{
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "Set 2D sample cut off (from outside)\n" );
#endif
		mRealLowPass = percent;
		Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
		if( percent < 0.2f ) percent = 0.2f;
		AIL_set_sample_low_pass_cut_off( mMiles->m2DRealSamples[mSampleIndex], percent );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set cut off (in SetLowPass) to %f\n", mMiles->m2DRealSamples[mSampleIndex], percent );
#endif
	}
}

void Miles2DSound::SetAlwaysSameSpeed( bool set )
{
	if( set ) Flags |= HasAlwaysSameSpeed; else Flags &= ~HasAlwaysSameSpeed;
}

void Miles2DSound::UpdateVolume( void )
{
	DWORD curTime = GetCurrentTime();

	if( curTime >= mDesiredTime )
	{
		mVolume = mDesiredVolume;
		mDesiredTime = curTime;
	}
	else
		mVolume = ((double)(curTime-mLastSetVolumeTime))/((double)(mDesiredTime-mLastSetVolumeTime))*((double)(mDesiredVolume-mVolume)) + mVolume;

	mLastSetVolumeTime = curTime;

	if( mSampleIndex != -1 )
	{
		float targetVolume = (mMiles->mPrivilegedSounds.Size()&&((Flags&Privileged)==0))?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
		Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
		AIL_set_sample_volume_levels( mMiles->m2DRealSamples[mSampleIndex], targetVolume, targetVolume );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set volume (in UpdateVolume) to %f\n", mMiles->m2DRealSamples[mSampleIndex], targetVolume );
#endif
	}
}

void Miles2DSound::UpdateIntensity( void )
{
	DWORD curTime = GetCurrentTime();

	if( curTime >= mDesiredIntensityTime )
	{
		mCurrentIntensity = mDesiredIntensity;
		mDesiredIntensityTime = curTime;
	}
	else
		mCurrentIntensity = ((double)(curTime-mLastSetIntensityTime))/((double)(mDesiredIntensityTime-mLastSetIntensityTime))*((double)(mDesiredIntensity-mCurrentIntensity)) + mCurrentIntensity;

	mLastSetIntensityTime = curTime;

	Assert( ( mCurrentIntensity >= 0.0f ) && ( mCurrentIntensity <= 1.0f ) );

	if( mSampleIndex != -1 )
	{
		AIL_set_sample_reverb_levels( mMiles->m2DRealSamples[mSampleIndex], 1.0f, mCurrentIntensity );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set reverb (in UpdateIntensity) to %f\n", mMiles->m2DRealSamples[mSampleIndex], mCurrentIntensity );
#endif
	}
}

bool Miles2DSound::ConnectToSample( int sampleIndex )
{
	Assert( mSampleIndex == -1 );
	HSAMPLE sample = mMiles->m2DRealSamples[sampleIndex];

	DWORD time = GetCurrentTime();

	if( !AIL_set_named_sample_file( sample, mFile->mFileName.AsChar(),
		mFile->mFilePointer, mFile->mFileSize, 0 ) )
		return false;

	AIL_set_sample_playback_rate( sample, ((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff );

	float percent = (mMiles->LowPassFrequencyPercent < mLowPass)?mMiles->LowPassFrequencyPercent:mLowPass;
	if( Flags & HasAlwaysSameSpeed ) percent = mLowPass;

	mRealLowPass = AIL_sample_low_pass_cut_off( sample );
	if( mRealLowPass != percent )
	{
		mRealLowPass = percent;
		Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
		AIL_set_sample_low_pass_cut_off( sample, percent );
#ifdef MILES_2DSOUND_DEBUG
		GLog.Print( "Frame %d - ", GEngine->World->Frame );
		GLog.Print( "2D sample %d: set cut off (connect) to %f\n", sample, percent );
#endif
	}

	mFile->mLastUsedTime = time;
	mMiles->m2DRealSounds[sampleIndex] = this;
	mSampleIndex = sampleIndex;

	// Set appropriate offset
	Assert( time >= mPlayStart );
	DWORD timeSinceStarted = time - mPlayStart;
	DWORD playOnceTime = mFile->mFileSoundInfo.samples*1000/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff);

	int loopCount = mLoopCount;
	if( loopCount > 0 )
	{
		loopCount -= ( timeSinceStarted / playOnceTime );
		if( loopCount < 1 )
			loopCount = 1;
	}

	timeSinceStarted %= playOnceTime;
	DWORD offset = timeSinceStarted*(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff)/1000;

	// Make sure (just in case) we don't start the sample after it ends
	if( offset >= mFile->mFileSoundInfo.samples )
		offset = mFile->mFileSoundInfo.samples-1;
	offset *= mFile->mFileSoundInfo.bits/8;
	AIL_set_sample_position( sample, offset );
#ifdef MILES_2DSOUND_DEBUG
	GLog.Print( "Frame %d - ", GEngine->World->Frame );
	GLog.Print( "2D sample %d: set offset (connect) to %d\n", sample, offset );
#endif

	// Set loop count
	Assert( loopCount >= 0 );
	AIL_set_sample_loop_count( sample, loopCount );
#ifdef MILES_2DSOUND_DEBUG
	GLog.Print( "Frame %d - ", GEngine->World->Frame );
	GLog.Print( "2D sample %d: set loop count (connect) to %d\n", sample, loopCount );
#endif

	// Set volume
	float targetVolume = (mMiles->mPrivilegedSounds.Size()&&((Flags&Privileged)==0))?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
	Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
	AIL_set_sample_volume_levels( sample, targetVolume, targetVolume );
#ifdef MILES_2DSOUND_DEBUG
	GLog.Print( "Frame %d - ", GEngine->World->Frame );
	GLog.Print( "2D sample %d: set volume (connect) to %f\n", sample, targetVolume );
#endif

	mFile->PlayFile( !mMiles->StartManyAtOnce );

	Assert( ( mCurrentIntensity >= 0.0f ) && ( mCurrentIntensity <= 1.0f ) );
	AIL_set_sample_reverb_levels( sample, 1.0f, mCurrentIntensity );
#ifdef MILES_2DSOUND_DEBUG
	GLog.Print( "Frame %d - ", GEngine->World->Frame );
	GLog.Print( "2D sample %d: set reverb (connect) to %f\n", sample, mCurrentIntensity );
#endif

	// Start sample
	if( offset )
		AIL_resume_sample( sample );
	else
		AIL_start_sample( sample );
	return true;
}

void Miles2DSound::DisconnectFromSample( void )
{
	Assert( mSampleIndex != -1 );

	HSAMPLE sample = mMiles->m2DRealSamples[mSampleIndex];

	mFile->StopFile();

	// Stop sample
	AIL_stop_sample( sample );
#ifdef MILES_2DSOUND_DEBUG
	GLog.Print( "Frame %d - ", GEngine->World->Frame );
	GLog.Print( "2D sample %d: stop\n" );
#endif

	mMiles->m2DRealSounds[mSampleIndex] = 0;
	mSampleIndex = -1;
}

#define MILES2DSOUND_LOAD_SAVE_TOKEN 0x5e57

void Miles2DSound::LoadSound( FileObject* f )
{
	if( !f->TestToken( MILES2DSOUND_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Miles2DSound::LoadSound()\n" );
		return;
	}

	DWORD t = GetCurrentTime();
	DWORD temp;

	Assert( mSoundIndex == -1 );
	*f >> mSoundIndex;
	mMiles->m2DSoundHash.Insert( mSoundIndex, this );
	*f >> (DWORD&)mOffset;
	*f >> mVolume;
	String fileName;
	*f >> fileName;
	if( fileName.Len() )
	{
		mFile = mMiles->GetFileByName( fileName.AsChar() );
		Assert( mFile );
	}
	*f >> mPriority;
	*f >> mLoopCount;
	*f >> Flags;

	*f >> temp;
	mPlayStart = t-temp;
	if( mPlayStart > t ) mPlayStart = 0;

	*f >> mPlayLength;
	*f >> mDesiredVolume;

	*f >> temp;
	mDesiredTime = t-temp;
	if( mDesiredTime > t ) mDesiredTime = 0;

	*f >> temp;
	mLastSetVolumeTime = t-temp;
	if( mLastSetVolumeTime > t ) mLastSetVolumeTime = 0;

	*f >> mSetIntensity;
	*f >> mCurrentIntensity;
	*f >> mDesiredIntensity;

	*f >> temp;
	mDesiredIntensityTime = t-temp;
	if( mDesiredIntensityTime > t ) mDesiredIntensityTime = 0;

	*f >> temp;
	mLastSetIntensityTime = t-temp;
	if( mLastSetIntensityTime > t ) mLastSetIntensityTime = 0;

	*f >> mLowPass;
	*f >> mSoundRateDiff;

	char start;
	*f >> start;
	if( start )
	{
		int cntr;
		*f >> cntr;
		bool delIfSto;
		*f >> (char&)delIfSto;

		DynamicArray<SoundList*>& ps = GEngine->Audio->PauseStack;
		Assert( ( cntr >= 0 ) && ( cntr < ps.Size() ) )
		ps[cntr]->Sounds2D.Add( this );
		ps[cntr]->Sounds2DDeleteIfStopped.Add( delIfSto );
	}
}

bool Miles2DSound::SaveSound( FileObject* f )
{
	int i=0;
	if( !f->PutToken( MILES2DSOUND_LOAD_SAVE_TOKEN ) ) return false;

	DWORD t = GetCurrentTime();

//	Assert( mSampleIndex == -1 );	// if it isn't yet, it should be 'coz its not playing!
	*f << mSoundIndex;
	*f << (DWORD)mOffset;
	*f << mVolume;
	if( mFile )
		*f << mFile->mFileName;
	else
		*f << String("");
	*f << mPriority;
	*f << mLoopCount;
	*f << Flags;
	*f << t-mPlayStart;
	*f << mPlayLength;
	*f << mDesiredVolume;
	*f << t-mDesiredTime;
	*f << t-mLastSetVolumeTime;
	*f << mSetIntensity;
	*f << mCurrentIntensity;
	*f << mDesiredIntensity;
	*f << t-mDesiredIntensityTime;
	*f << t-mLastSetIntensityTime;

	*f << mLowPass;
	*f << mSoundRateDiff;

	DynamicArray<SoundList*>& ps = GEngine->Audio->PauseStack;

	for( i = 0; i < ps.Size(); ++i )
	{
		if( !ps[i] ) continue;
		int ind = ps[i]->Sounds2D.Find( this );
		if( ind != -1 )
		{
			*f << (char)1;
			*f << i;
			*f << (char)ps[i]->Sounds2DDeleteIfStopped[ind];
			break;
		}
	}
	if( i == ps.Size() )
		*f << (char)0;

	return true;
}

void Miles2DSound::Dump( void )
{
	GLog.Print( "------------- SOUND2D -----------\n" );

	DWORD t = GetCurrentTime();

	GLog.Print( "Sound Index: %d\n", mSoundIndex );
	GLog.Print( "Sample Index: %d\n", mSampleIndex );
	GLog.Print( "Offset: %d\n", (DWORD)mOffset );
	GLog.Print( "File: %s\n", mFile?mFile->mFileName.AsChar():"" );
	GLog.Print( "Priority: %d\n", mPriority );
	GLog.Print( "Loop Count: %d\n", mLoopCount );
	GLog.Print( "Flags: " );
	bool first = true;
	if( Flags & DeleteIfStopped ) { GLog.Print( "DeleteIfStopped" ); first = false; };
	if( Flags & WannaPlay ) { if( !first ) GLog.Print( "|" ); GLog.Print( "WannaPlay" ); first = false;  }
	if( Flags & Privileged ) { if( !first ) GLog.Print( "|" ); GLog.Print( "Privileged" ); first = false;  }
	if( Flags & HasAlwaysSameSpeed ) { if( !first ) GLog.Print( "|" ); GLog.Print( "AlwaysSameSpeed" ); first = false;  }
	if( Flags & DontSave ) { if( !first ) GLog.Print( "|" ); GLog.Print( "DontSave" ); first = false;  }
	GLog.Print( "\n" );
	GLog.Print( "Started ms ago: %d\n", t-mPlayStart );
	GLog.Print( "Duration in ms: %d\n", mPlayLength );

	GLog.Print( "Volume: %f\n", mVolume );
	if( mDesiredVolume != mVolume )
	{
		GLog.Print( " - Desired volume: %f\n", mDesiredVolume );
		GLog.Print( " - Desired volume: %d ms from now\n", (int)((int)t-(int)mDesiredTime) );
		GLog.Print( " - Last set volume time: %d ms ago\n", t-mLastSetVolumeTime );
	}

	GLog.Print( "Set Intensity: %f\n", mSetIntensity );
	GLog.Print( "Current Intensity: %f\n", mCurrentIntensity );
	if( mDesiredIntensity != mCurrentIntensity )
	{
		GLog.Print( " - Desired Intensity: %f\n", mDesiredIntensity );
		GLog.Print( " - Desired intensity: %d ms from now\n", (int)((int)t-(int)mDesiredIntensityTime) );
		GLog.Print( " - Last set intensity time: %d ms ago\n", t-mLastSetIntensityTime );
	}

	GLog.Print( "Low Pass: %f\n", mLowPass );
	GLog.Print( "Sound Rate Diff: %f\n", mSoundRateDiff );

	GLog.Print( "---------------------------------\n" );
}
