

#include "Engine.h"
#include "Miles3DSound.h"
#include "MilesEngine.h"
#include "MilesLoadedFile.h"

#include "Physics/PhysicsWorld.h"	// for HitData

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Engine/World.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Physics/Havok/HavokPhysics.h"


// --------------------- Miles3DSound methods ---------------------
Miles3DSound::Miles3DSound( MilesLoadedFile* file, MilesEngine* miles, int soundIndex ) :
		mSampleIndex(-1), mLoopCount(1), mMiles(miles), mFile(file), mHearMin(0.01f),
		mHearMax(100.0f), mPosition(0,0,0), mFront(0,0,1), mUp(0,1,0), mVelocity(0,0,0),
		mPlayStart(0), mPlayLength(0), mSoundIndex( soundIndex ), mOffset(0),

		mVolume(1.0f), mDesiredVolume(1.0f), mDesiredTime(0.0f), mLastSetVolumeTime(0), Flags(0),
		mCurrentIntensity(1.0f), mDesiredIntensity(1.0f), mDesiredIntensityTime(0.0f), mLastSetIntensityTime(0),
		mCurrentObstruction(0.0f), mDesiredObstruction(0.0f), mDesiredObstructionTime(0.0f), mLastSetObstructionTime(0),

		mPriority(0), mSetIntensity(1.0f), mRealLowPass(1.0f)
{
	if( miles->mObstructedByDefault ) Flags |= Obstructed;
	mLowPass = miles->mLowPassForNewSounds;
	mSoundRateDiff = 1.0f + (2*(((float)rand())/RAND_MAX)-1)*miles->mSoundRateDiffRate;
	if( mSoundIndex != -1 )
		mMiles->m3DSoundHash.Insert( mSoundIndex, this );
}

Miles3DSound::~Miles3DSound()
{
	// Remove from paused sounds stack
	for( int i = 0; i < mMiles->PauseStack.Size(); ++i )
	{
		if( !mMiles->PauseStack[i] ) continue;
		for( int j = 0; j < mMiles->PauseStack[i]->Sounds3D.Size(); ++j )
			if( mMiles->PauseStack[i]->Sounds3D[j] == this )
			{
				mMiles->PauseStack[i]->Sounds3D.FastRemove(j);
				mMiles->PauseStack[i]->Sounds3DDeleteIfStopped.FastRemove(j);
                break;
			}
	}

	for( int i = 0; i < mMiles->m3DSounds.Size(); ++i )
	{
		if( mMiles->m3DSounds[i] == this )
		{
			mMiles->m3DSounds.Remove(i);
			break;
		}
	}

	if( mSampleIndex != -1 )
	{
		mMiles->Release3DSample( mSampleIndex );
		mSampleIndex = -1;
	}

	if( mFile )
		mFile->RemoveReference();

	Assert( mSoundIndex != -1 );
	mMiles->m3DSoundHash.Remove(mSoundIndex);
}

void Miles3DSound::Play( void )
{
	if( Flags & WannaPlay ) Stop();	// stop it first

	mOffset = 0;
	Flags |= WannaPlay;
	mPlayStart = GetCurrentTime();
	mMiles->TryToPlayRealSound( this );

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		DWORD playOnceTime = mFile->mFileSoundInfo.samples*1000/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff);
		mPlayLength = mLoopCount*playOnceTime;
	}
}

void Miles3DSound::Stop( void )
{
	Flags &= ~WannaPlay;
	mPlayLength = 0;
	if( mSampleIndex != -1 )
	{
		mOffset = AIL_3D_sample_offset( mMiles->m3DRealSamples[mSampleIndex] );
		// Get loop count
#ifdef _DEBUG
		bool wasNonZero = (mLoopCount > 0);
#endif
		mLoopCount = AIL_3D_sample_loop_count( mMiles->m3DRealSamples[mSampleIndex] );
		Assert( mLoopCount >= 0 );
#ifdef _DEBUG
		Assert( !( wasNonZero && (mLoopCount < 1) ) );
#endif
		mMiles->Release3DSample( mSampleIndex );
	}
	else
		mOffset = 0;
	mPlayStart = 0;
}

void Miles3DSound::Resume( void )
{
	Flags |= WannaPlay;
	mMiles->TryToPlayRealSound( this );
	mPlayStart = GetCurrentTime();

	if( mLoopCount <= 0 )
		mPlayLength = DWORD(-1);
	else
	{
		DWORD playOnceTime = mFile->mFileSoundInfo.samples*1000/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff);			// measured in bytes
		DWORD offsetTime = mOffset*1000*8/(((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff)/mFile->mFileSoundInfo.bits;	// measured in bytes
		mPlayLength = mLoopCount*playOnceTime - offsetTime;
	}
}

void Miles3DSound::SetLoopCount( int count )
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

int Miles3DSound::GetLoopCount( void )
{
	int loopCount = 0;
	if( mSampleIndex != -1 )
		loopCount = AIL_3D_sample_loop_count( mMiles->m3DRealSamples[mSampleIndex] );
	Assert( loopCount >= 0 );
	return loopCount;
}

void Miles3DSound::SetHearingDistance( float minDist, float maxDist )
{
	mHearMin = minDist;
	mHearMax = maxDist;
	if( minDist < 0 )
		minDist = 0;
	Assert( maxDist > 0 );
	if( mSampleIndex != -1 )
	{
		AIL_set_3D_sample_distances( mMiles->m3DRealSamples[mSampleIndex], maxDist, minDist );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set distances (in SetHearingDistances) to max:%f, min:%f\n", mMiles->m3DRealSamples[mSampleIndex], maxDist, minDist );
#endif
	}
}

void Miles3DSound::GetHearingDistance( float& minDist, float& maxDist )
{
	if( mSampleIndex != -1 )
		AIL_3D_sample_distances( mMiles->m3DRealSamples[mSampleIndex], &mHearMax, &mHearMin );
	Assert( minDist >= 0 );
	Assert( maxDist > 0 );
	minDist = mHearMin;
	maxDist = mHearMax;
}

void Miles3DSound::SetPosition( Vector& position )
{
	mPosition = position;
	if( mSampleIndex != -1 )
	{
		AIL_set_3D_position( mMiles->m3DRealSamples[mSampleIndex], mPosition.X, mPosition.Y, -mPosition.Z );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set position (in SetPosition) to (%f,%f,%f)\n", mMiles->m3DRealSamples[mSampleIndex], mPosition.X, mPosition.Y, mPosition.Z );
#endif
	}
}

Vector Miles3DSound::GetPosition( void )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_position( mMiles->m3DRealSamples[mSampleIndex], &mPosition.X, &mPosition.Y, &mPosition.Z );
		mPosition.Z = -mPosition.Z;
	}
	return mPosition;
}

void Miles3DSound::SetOrientation( Vector& front, Vector& up )
{
	mFront = front;
	mUp = up;
	if( mSampleIndex != -1 )
	{
		AIL_set_3D_orientation( mMiles->m3DRealSamples[mSampleIndex], mFront.X, mFront.Y, -mFront.Z, mUp.X, mUp.Y, -mUp.Z );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set orientation (in SetOrientation)\n", mMiles->m3DRealSamples[mSampleIndex] );
#endif
	}
}

void Miles3DSound::GetOrientation( Vector& front, Vector& up )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_orientation( mMiles->m3DRealSamples[mSampleIndex], &mFront.X, &mFront.Y, &mFront.Z, &mUp.X, &mUp.Y, &mUp.Z );
		mFront.Z = -mFront.Z;
		mUp.Z = -mUp.Z;
	}
	front = mFront;
	up = mUp;
}

void Miles3DSound::SetVelocity( Vector& velocity )
{
	mVelocity = velocity;
	if( mSampleIndex != -1 )
	{
		float magn = mVelocity.Magnitude();
		Vector vel = mVelocity;
		vel.Normalize();
		AIL_set_3D_velocity( mMiles->m3DRealSamples[mSampleIndex], vel.X, vel.Y, -vel.Z, 0.001f*magn );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set velocity (in SetVelocity) to (%f,%f,%f)\n", mMiles->m3DRealSamples[mSampleIndex],
			0.001f*mVelocity.X, 0.001f*mVelocity.Y, -0.001f*mVelocity.Z );
#endif
	}
}

Vector Miles3DSound::GetVelocity( void )
{
	if( mSampleIndex != -1 )
	{
		AIL_3D_velocity( mMiles->m3DRealSamples[mSampleIndex], &mVelocity.X, &mVelocity.Y, &mVelocity.Z );
		mVelocity.Z = -mVelocity.Z;
		mVelocity *= 1000.0f;
	}
	return mVelocity;
}

bool Miles3DSound::IsPlaying( void )
{
	if( mSampleIndex != -1 )
	{
		U32 out = AIL_3D_sample_status( mMiles->m3DRealSamples[mSampleIndex] );
		return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) );
	}
	else if( !(Flags&WannaPlay) )
		return false;
	else if( mLoopCount == 0 )
		return true;
	else if( mPlayLength == DWORD(-1) )
		return true;
	else
		return( ( GetCurrentTime() - mPlayStart ) < mPlayLength );
}

void Miles3DSound::SetVolume( float volume, float switchTime )
{
	if( volume == mVolume ) return;
	if( volume < 0.0f ) volume = 0.0f;
	else if( volume > 1.0f ) volume = 1.0f;

	if( !switchTime )
		mVolume = volume;

	mDesiredVolume = volume;
	mLastSetVolumeTime = GetCurrentTime();
	mDesiredTime = mLastSetVolumeTime + floor(switchTime*1000);

	if( !switchTime && ( mSampleIndex != -1 ) )
	{
		float targetVolume = (mMiles->mPrivilegedSounds.Size()&&((Flags&Privileged)==0))?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
		Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
		AIL_set_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex], targetVolume );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set volume (in SetVolume) to %f\n", mMiles->m3DRealSamples[mSampleIndex], targetVolume );
#endif
	}
}

void Miles3DSound::SetCurrentIntensity( float intensity, float switchTime )
{
	if( intensity == mDesiredIntensity ) return;
	if( intensity < 0.0f ) intensity = 0.0f;
	else if( intensity > 1.0f ) intensity = 1.0f;

	if( !switchTime )
		mCurrentIntensity = intensity;

	mDesiredIntensity = intensity;
	mLastSetIntensityTime = GetCurrentTime();
	mDesiredIntensityTime = mLastSetIntensityTime + floor(switchTime*1000);

	Assert( ( mCurrentIntensity >= 0.0f ) && ( mCurrentIntensity <= 1.0f ) );

	if( !switchTime && ( mSampleIndex != -1 ) )
	{
		AIL_set_3D_sample_effects_level( mMiles->m3DRealSamples[mSampleIndex], mCurrentIntensity );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set reverb (in SetCurrentIntensity) to %f\n", mMiles->m3DRealSamples[mSampleIndex], mCurrentIntensity );
#endif
	}
}

void Miles3DSound::SetCurrentObstruction( float obstruction, float switchTime )
{
	if( obstruction == mDesiredObstruction ) return;
	if( obstruction < 0.0f ) obstruction = 0.0f;
	else if( obstruction > 1.0f ) obstruction = 1.0f;

	if( !switchTime )
		mCurrentObstruction = obstruction;

	mDesiredObstruction = obstruction;
	mLastSetObstructionTime = GetCurrentTime();
	mDesiredObstructionTime = mLastSetObstructionTime + floor(switchTime*1000);

	Assert( ( mCurrentObstruction >= 0.0f ) && ( mCurrentObstruction <= 1.0f ) );

	if( !switchTime && ( mSampleIndex != -1 ) )
	{
		AIL_set_3D_sample_obstruction( mMiles->m3DRealSamples[mSampleIndex], mCurrentObstruction );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set obstruction (in SetCurrentObstruction) to %f\n", mMiles->m3DRealSamples[mSampleIndex], mCurrentObstruction );
#endif
	}
}

void Miles3DSound::SetLowPass( float lp )
{
	if( lp < 0.0f ) lp = 0.0f;
	if( lp > 1.0f ) lp = 1.0f;

	mLowPass = lp;
	float percent = lp;
	if( !(Flags&HasAlwaysSameSpeed) && mMiles->LowPassFrequencyPercent < lp )
		percent = mMiles->LowPassFrequencyPercent;

	if( ( mSampleIndex != -1 ) && ( mRealLowPass != percent ) )
	{
		mRealLowPass = 1.0f - percent;
		Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
		if( percent < 0.2f ) percent = 0.2f;
		AIL_set_3D_sample_occlusion( mMiles->m3DRealSamples[mSampleIndex], 1.0f - percent );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set low pass (in SetLowPass) to %f\n", mMiles->m3DRealSamples[mSampleIndex], 1.0f - percent );
#endif
	}
}

void Miles3DSound::SetAlwaysSameSpeed( bool set )
{
	if( set ) Flags |= HasAlwaysSameSpeed; else Flags &= ~HasAlwaysSameSpeed;
}

void Miles3DSound::UpdateVolume( void )
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
		AIL_set_3D_sample_volume( mMiles->m3DRealSamples[mSampleIndex], targetVolume );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set volume (in UpdateVolume) to %f\n", mMiles->m3DRealSamples[mSampleIndex], targetVolume );
#endif
	}
}

void Miles3DSound::UpdateIntensity( void )
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
		AIL_set_3D_sample_effects_level( mMiles->m3DRealSamples[mSampleIndex], mCurrentIntensity );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set intensity (in UpdateIntensity) to %f\n", mMiles->m3DRealSamples[mSampleIndex], mCurrentIntensity );
#endif
	}
}

void Miles3DSound::UpdateObstruction( void )
{
	DWORD curTime = GetCurrentTime();

	if( curTime >= mDesiredObstructionTime )
	{
		mCurrentObstruction = mDesiredObstruction;
		mDesiredObstructionTime = curTime;
	}
	else
		mCurrentObstruction = ((double)(mDesiredObstruction-mCurrentObstruction))*(curTime-mLastSetObstructionTime)/(mDesiredObstructionTime-mLastSetObstructionTime) + mCurrentObstruction;

	mLastSetObstructionTime = curTime;

	Assert( ( mCurrentObstruction >= 0.0f ) && ( mCurrentObstruction <= 1.0f ) );

	if( mSampleIndex != -1 )
	{
		AIL_set_3D_sample_obstruction( mMiles->m3DRealSamples[mSampleIndex], mCurrentObstruction );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set obstruction (in UpdateObstruction) to %f\n", mMiles->m3DRealSamples[mSampleIndex], mCurrentObstruction );
#endif
	}
}

bool Miles3DSound::ConnectToSample( int sampleIndex )
{
	Assert( mSampleIndex == -1 );
	DWORD time = GetCurrentTime();

	HSAMPLE sample = mMiles->m3DRealSamples[sampleIndex];
	if( AIL_set_3D_sample_file( sample, mFile->mFilePointer ) == 0 )
		return false;

	DWORD SPRate = ((Flags&HasAlwaysSameSpeed)?1.0f:mMiles->Speed)*mFile->mFileSoundInfo.rate*mSoundRateDiff;
	Assert( SPRate > 0 );
	AIL_set_3D_sample_playback_rate( sample, SPRate );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: set speed (in connect) to %d\n", sample, SPRate );
#endif

	float percent = ( mMiles->LowPassFrequencyPercent < mLowPass )?mMiles->LowPassFrequencyPercent:mLowPass;
	if( Flags & HasAlwaysSameSpeed ) percent = mLowPass;

	mRealLowPass = AIL_3D_sample_occlusion( sample );
	if( mRealLowPass != ( 1.0f - percent ) )
	{
		mRealLowPass = 1.0f - percent;
		Assert( ( percent >= 0.0f ) && ( percent <= 1.0f ) );
		AIL_set_3D_sample_occlusion( sample, mRealLowPass );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set occlusion (in connect) to %f\n", sample, 1.0f - percent );
#endif
	}

	mFile->mLastUsedTime = time;
	mMiles->m3DRealSounds[sampleIndex] = this;
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
	AIL_set_3D_sample_offset( sample, offset );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: set offset (in connect) to %d\n", sample, offset );
#endif

	// Set loop count
	Assert( loopCount >= 0 );
	AIL_set_3D_sample_loop_count( sample, loopCount );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: set loop count (in connect) to %d\n", sample, loopCount );
#endif

	// Set hearing distance
	Assert( mHearMin >= 0 );
	Assert( mHearMax > 0 );
	AIL_set_3D_sample_distances( sample, mHearMax, mHearMin );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: set distances (in connect) to max: %f, min: %f\n", sample, mHearMax, mHearMin );
#endif

	// Set position
	AIL_set_3D_position( sample, mPosition.X, mPosition.Y, -mPosition.Z );

	// Set orientation
	AIL_set_3D_orientation( sample, mFront.X, mFront.Y, -mFront.Z, mUp.X, mUp.Y, -mUp.Z );

	// Set velocity
	float magn = mVelocity.Magnitude();
	Vector vel = mVelocity;
	vel.Normalize();
	AIL_set_3D_velocity( sample, vel.X, vel.Y, -vel.Z, 0.001f*magn );

	// Set volume
	float targetVolume = (mMiles->mPrivilegedSounds.Size()&&((Flags&Privileged)==0))?mMiles->DigitalEffectsVolume3D*mVolume*mMiles->mVolumeMultiplier:mMiles->DigitalEffectsVolume3D*mVolume;
	Assert( ( targetVolume >= 0.0f ) && ( targetVolume <= 1.0f ) );
	AIL_set_3D_sample_volume( sample, targetVolume );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: set volume (in connect) to %f\n", sample, targetVolume );
#endif

	// Determine starting sample obstruction
	if( Flags&Obstructed )
	{
		Vector playerPos;
		AIL_listener_3D_position( mMiles->DigitalDriver, &playerPos.X, &playerPos.Y, &playerPos.Z );
		playerPos.Z = -playerPos.Z;

		HitData hitData;
		float obstr = 0.0f;
#ifdef	TRANSGAMING
		if( GEngine->pcfWorld->Physics && GEngine->pcfWorld->Physics->LineTraceFixedGeom( playerPos, mPosition, hitData ) )	// need distance
#else
		if( GEngine->World->Physics && GEngine->World->Physics->LineTraceFixedGeom( playerPos, mPosition, hitData ) )	// need distance
#endif
		{
			float dist = (playerPos-mPosition).Magnitude();
			if( dist < mMiles->MinObstructionDistance )
                obstr = mMiles->MaxObstructionLow;
			else if( dist > mMiles->MaxObstructionDistance )
				obstr = mMiles->MaxObstructionHigh;
			else
				obstr = mMiles->MaxObstructionLow+(mMiles->MaxObstructionHigh-mMiles->MaxObstructionLow)*
					(dist-mMiles->MinObstructionDistance)/(mMiles->MaxObstructionDistance-mMiles->MinObstructionDistance);
		}
		SetCurrentObstruction( obstr );
	}
	else
	{
		AIL_set_3D_sample_obstruction( sample, 0.0f );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set obstruction (in connect) to 0.0\n", sample );
#endif
	}

	mFile->PlayFile( !mMiles->StartManyAtOnce );

	if( mMiles->UsingEAX )
	{
		Assert( ( mCurrentIntensity >= 0.0f ) && ( mCurrentIntensity <= 1.0f ) );
		AIL_set_3D_sample_effects_level( sample, mCurrentIntensity );
#ifdef MILES_3DSOUND_DEBUG
		TRACE( "Frame %d - ", GEngine->World->Frame );
		TRACE( "3D sample %d: set intensity (in connect) to %f\n", sample, mCurrentIntensity );
#endif
	}

	// Start sample
	if( offset )
		AIL_resume_3D_sample( sample );
	else
		AIL_start_3D_sample( sample );
	return true;
}

void Miles3DSound::DisconnectFromSample( void )
{
	Assert( mSampleIndex != -1 );

	HSAMPLE sample = mMiles->m3DRealSamples[mSampleIndex];

	mFile->StopFile();

	// Stop sample
	AIL_stop_3D_sample( sample );
#ifdef MILES_3DSOUND_DEBUG
	TRACE( "Frame %d - ", GEngine->World->Frame );
	TRACE( "3D sample %d: stop\n", sample );
#endif

	// Get velocity
	AIL_3D_velocity( sample, &mVelocity.X, &mVelocity.Y, &mVelocity.Z );
	mVelocity.Z = -mVelocity.Z;
	mVelocity *= 1000.0f;

	// Get orientation
	AIL_3D_orientation( sample, &mFront.X, &mFront.Y, &mFront.Z, &mUp.X, &mUp.Y, &mUp.Z );
	mFront.Z = -mFront.Z;
	mUp.Z = -mUp.Z;

	// Get position
	AIL_3D_position( sample, &mPosition.X, &mPosition.Y, &mPosition.Z );
	mPosition.Z = -mPosition.Z;

	// Get hearing distances
	AIL_3D_sample_distances( sample, &mHearMax, &mHearMin );

	mMiles->m3DRealSounds[mSampleIndex] = 0;
	mSampleIndex = -1;
}

#define MILES3DSOUND_LOAD_SAVE_TOKEN 0x5e58

void Miles3DSound::LoadSound( GFile* f )
{
	if( !f->TestToken( MILES3DSOUND_LOAD_SAVE_TOKEN ) )
	{
		TRACE( "Token test failure: Miles3DSound::LoadSound()\n" );
		return;
	}

	DWORD t = GetCurrentTime();
	DWORD temp;

	Assert( mSoundIndex == -1 );
	*f >> mSoundIndex;
	mMiles->m3DSoundHash.Insert( mSoundIndex, this );
	*f >> (DWORD&)mOffset;
	String fileName;
	*f >> fileName;
	if( fileName.Len() )
	{
		mFile = mMiles->GetFileByName( fileName.AsChar() );
		Assert( mFile );
	}
	*f >> Flags;
	*f >> mPriority;
	*f >> mLoopCount;
	*f >> mHearMin;
	*f >> mHearMax;
	*f >> mPosition;
	*f >> mFront;
	*f >> mUp;
	*f >> mVelocity;

	*f >> temp;
	mPlayStart = t-temp;
	if( mPlayStart > t ) mPlayStart = 0;

	*f >> mPlayLength;

	*f >> mVolume;
	*f >> mDesiredVolume;
	*f >> temp; mDesiredTime = t-temp; if( mDesiredTime > t ) mDesiredTime = 0;
	*f >> temp; mLastSetVolumeTime = t-temp; if( mLastSetVolumeTime > t ) mLastSetVolumeTime = 0;

	*f >> mSetIntensity;
	*f >> mCurrentIntensity;
	*f >> mDesiredIntensity;
	*f >> temp; mDesiredIntensityTime = t-temp; if( mDesiredIntensityTime > t ) mDesiredIntensityTime = 0;
	*f >> temp; mLastSetIntensityTime = t-temp; if( mLastSetIntensityTime > t ) mLastSetIntensityTime = 0;

	*f >> mCurrentObstruction;
	*f >> mDesiredObstruction;
	*f >> temp; mDesiredObstructionTime = t-temp; if( mDesiredObstructionTime > t ) mDesiredObstructionTime = 0;
	*f >> temp; mLastSetObstructionTime = t-temp; if( mLastSetObstructionTime > t ) mLastSetObstructionTime = 0;

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
		ps[cntr]->Sounds3D.Add( this );
		ps[cntr]->Sounds3DDeleteIfStopped.Add( delIfSto );
	}
}

bool Miles3DSound::SaveSound( GFile* f )
{
	int i = 0;
	if( !f->PutToken( MILES3DSOUND_LOAD_SAVE_TOKEN ) ) return false;

	DWORD t = GetCurrentTime();

//	Assert( mSampleIndex == -1 );	// if it isn't yet, it should be 'coz its not playing!
	*f << mSoundIndex;
	*f << (DWORD)mOffset;
	if( mFile )
		*f << mFile->mFileName;
	else
		*f << String("");
	*f << Flags;
	*f << mPriority;
	*f << mLoopCount;
	*f << mHearMin;
	*f << mHearMax;
	*f << mPosition;
	*f << mFront;
	*f << mUp;
	*f << mVelocity;
	*f << t-mPlayStart;
	*f << mPlayLength;

	*f << mVolume;
	*f << mDesiredVolume;
	*f << t-mDesiredTime;
	*f << t-mLastSetVolumeTime;

	*f << mSetIntensity;
	*f << mCurrentIntensity;
	*f << mDesiredIntensity;
	*f << t-mDesiredIntensityTime;
	*f << t-mLastSetIntensityTime;

	*f << mCurrentObstruction;
	*f << mDesiredObstruction;
	*f << t-mDesiredObstructionTime;
	*f << t-mLastSetObstructionTime;

	*f << mLowPass;
	*f << mSoundRateDiff;

	DynamicArray<SoundList*>& ps = GEngine->Audio->PauseStack;

	for( i = 0; i < ps.Size(); ++i )
	{
		if( !ps[i] ) continue;
		int ind = ps[i]->Sounds3D.Find( this );
		if( ind != -1 )
		{
			*f << (char)1;
			*f << i;
			*f << (char)ps[i]->Sounds3DDeleteIfStopped[ind];
			break;
		}
	}
	if( i == ps.Size() )
		*f << (char)0;

	return true;
}

void Miles3DSound::Dump( void )
{
	TRACE( "------------- SOUND3D -----------\n" );

	DWORD t = GetCurrentTime();

	TRACE( "Sound Index: %d\n", mSoundIndex );
	TRACE( "Sample Index: %d\n", mSampleIndex );
	TRACE( "Offset: %d\n", (DWORD)mOffset );
	TRACE( "File: %s\n", mFile?mFile->mFileName.AsChar():"" );
	TRACE( "Priority: %d\n", mPriority );
	TRACE( "Loop Count: %d\n", mLoopCount );
	TRACE( "Flags: " );
	bool first = true;
	if( Flags & DeleteIfStopped ) { TRACE( "DeleteIfStopped" ); first = false; };
	if( Flags & WannaPlay ) { if( !first ) TRACE( "|" ); TRACE( "WannaPlay" ); first = false;  }
	if( Flags & Privileged ) { if( !first ) TRACE( "|" ); TRACE( "Privileged" ); first = false;  }
	if( Flags & HasAlwaysSameSpeed ) { if( !first ) TRACE( "|" ); TRACE( "AlwaysSameSpeed" ); first = false;  }
	if( Flags & DontSave ) { if( !first ) TRACE( "|" ); TRACE( "DontSave" ); first = false;  }
	if( Flags & Obstructed ) { if( !first ) TRACE( "|" ); TRACE( "Obstructed" ); first = false;  }
	TRACE( "\n" );

	TRACE( "Min hearing distance: %f\n", mHearMin );
	TRACE( "Max hearing distance: %f\n", mHearMax );
	TRACE( "Position: (%f,%f,%f)\n", mPosition.X, mPosition.Y, mPosition.Z );
	TRACE( "Front: (%f,%f,%f)\n", mFront.X, mFront.Y, mFront.Z );
	TRACE( "Up: (%f,%f,%f)\n", mUp.X, mUp.Y, mUp.Z );
	TRACE( "Velocity: (%f,%f,%f)\n", mVelocity.X, mVelocity.Y, mVelocity.Z );

	TRACE( "Started ms ago: %d\n", t-mPlayStart );
	TRACE( "Duration in ms: %d\n", mPlayLength );

	TRACE( "Volume: %f\n", mVolume );
	if( mDesiredVolume != mVolume )
	{
		TRACE( " - Desired volume: %f\n", mDesiredVolume );
		TRACE( " - Desired volume: %d ms from now\n", (int)((int)t-(int)mDesiredTime) );
		TRACE( " - Last set volume time: %d ms ago\n", t-mLastSetVolumeTime );
	}

	TRACE( "Set Intensity: %f\n", mSetIntensity );
	TRACE( "Current Intensity: %f\n", mCurrentIntensity );
	if( mDesiredIntensity != mCurrentIntensity )
	{
		TRACE( " - Desired Intensity: %f\n", mDesiredIntensity );
		TRACE( " - Desired intensity: %d ms from now\n", (int)((int)t-(int)mDesiredIntensityTime) );
		TRACE( " - Last set intensity time: %d ms ago\n", t-mLastSetIntensityTime );
	}

	TRACE( "Current Obstruction: %f\n", mCurrentObstruction );
	if( mDesiredObstruction != mCurrentObstruction )
	{
		TRACE( " - Desired Obstruction: %f\n", mDesiredObstruction );
		TRACE( " - Desired Obstruction: %d ms from now\n", (int)((int)t-(int)mDesiredObstructionTime) );
		TRACE( " - Last set obstruction time: %d ms ago\n", t-mLastSetObstructionTime );
	}

	TRACE( "Low Pass: %f\n", mLowPass );
	TRACE( "Sound Rate Diff: %f\n", mSoundRateDiff );

	TRACE( "---------------------------------\n" );
}
