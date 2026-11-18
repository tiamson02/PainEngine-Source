
#include "Engine.h"
#include "StreamingSound.h"
#include "MilesEngine.h"

// ------------------------------ StreamingSound methods -------------------------------------

StreamingSound::~StreamingSound( void ) { if( mInternalIndex != -1 ) { mMiles->ReleaseStream( mInternalIndex ); mInternalIndex = -1; } }
void StreamingSound::Play( void ) { AIL_start_stream( mMiles->mStreams[mInternalIndex] ); }
void StreamingSound::SetVolumeAndPan( float volume, float pan ) { AIL_set_stream_volume_pan( mMiles->mStreams[mInternalIndex], volume*mMiles->mVolumeMultiplier, pan ); }
void StreamingSound::GetVolumeAndPan( float& volume, float& pan ) { AIL_stream_volume_pan( mMiles->mStreams[mInternalIndex], &volume, &pan ); volume /= mMiles->mVolumeMultiplier; }
void StreamingSound::Stop( void ) { AIL_pause_stream( mMiles->mStreams[mInternalIndex], 1 ); }
void StreamingSound::Resume( void ) { AIL_pause_stream( mMiles->mStreams[mInternalIndex], 0 ); }
void StreamingSound::SetLoopCount( int count ) { AIL_set_stream_loop_count( mMiles->mStreams[mInternalIndex], count ); }
int StreamingSound::GetLoopCount( void ) { return AIL_stream_loop_count( mMiles->mStreams[mInternalIndex] ); }
bool StreamingSound::IsPlaying( void ) { U32 out = AIL_stream_status( mMiles->mStreams[mInternalIndex] ); return( (out == SMP_PLAYING) || (out == SMP_PLAYINGBUTRELEASED) ); }
float StreamingSound::GetLowPass( void ) { return mMiles->mStreamLowPass[mInternalIndex]; }

void StreamingSound::SetLowPass( float lp )
{
	mMiles->mStreamLowPass[mInternalIndex] = lp;
	float percent = ( mMiles->LowPassFrequencyPercent > lp )?lp:mMiles->LowPassFrequencyPercent;
	AIL_set_stream_low_pass_cut_off( mMiles->mStreams[mInternalIndex], percent );
}
