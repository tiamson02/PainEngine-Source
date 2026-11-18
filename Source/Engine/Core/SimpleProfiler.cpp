#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Core/SimpleProfiler.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#ifdef SIMPLE_PROFILER

char *ProfilerStateNames[] = {
	"Warmup Code",
	"Input System Tick",
	"Physics Tick - miscellaneous",
	"Physics Tick - stepDeltaTime()",
	"Physics Tick - tick physics objects",
	"Physics Tick - tick ragdolls",
	"Physics Tick - update entities",
	"Physics Tick - player controller",
	"Physics Tick - missile controller",
	"Audio Tick",

	"Audio Tick - Stop Finished",
	"Audio Tick - Update Speed",
	"Audio Tick - Update 3D",
	"Audio Tick - Update 2D",
	"Audio Tick - Flip 3D",
	"Audio Tick - Obstruct",

	"World Tick",
	"Update Logic - start",
	"Update Logic - editor tick/player clienttick",
	"Update Logic - gobject tick: Actors",
	"Update Logic - gobject tick: Items",
	"Update Logic - gobject tick: Rest",
	"Update Logic - gobject update: Actors",
	"Update Logic - gobject update: Items",
	"Update Logic - gobject update: Rest",
	"Update Logic - game:update, hud:tick, menu:tick",
	"Update Logic - rest",
	"Client Render - start & stop",
	"Client Render - RenderWorld()",
	"Client Render - LUA",
	"Client Render - HUD",
	"Client Render - Occlude",
	"Client Render - Occlude - Antiportals Calculation",
	"Client Render - Occlude - Entity Visibility",
	"Client Render - Occlude - Portal Visibility",
	"Client Render - Occlude - Antiportal Box Test",
	"Network - Reading Input",
	"Network - GScript.Call",
	"Network - Sending Output",
	"Network - Sending Update Message",
	"Network - Sending Scripts Reliable",
	"Network - Sending Scripts Unreliable",
	"Network - Sending Entity Updates",
	"Network - Sending Entity Create/Delete/Other",
	"Network - Tick Channels",
	"Network - Other",
	"Message Pump",
	"Pathfinding",
	"Havok Independent Player Action",

	"Temp State",

	"Other Code",

	// Just in case something reads past here in RenderHUD

	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

//LogBuffer ProfilerLog( "profiler.log" );

SimpleProfiler::SimpleProfiler( bool lastFrames, int frames )
{
	for (int i=0; i<PROFILER_STATES_NUMBER; i++)
	{
		mFramesAvgTable[i] = 0;
		mFramesMaxTable[i] = 0;
		mFramesViewAvgTable[i] = 0;
		mFramesViewMaxTable[i] = 0;
		mTimes[i] = 0;
	}

	if( lastFrames )
	{
		mFrames = frames;

		// Create times table
        m_setTag("Debug");
#ifdef TRANSGAMING
		mFramesTimeTable = new float[ frames*PROFILER_STATES_NUMBER ];
#else
		mFramesTimeTable = new double[ frames*PROFILER_STATES_NUMBER ];
#endif
        for( int i = 0; i < frames*PROFILER_STATES_NUMBER; ++i ) {
			mFramesTimeTable[i] = 0;
		}

		mCurrentFrame = 0;
	}
	else
		mFrames = 0;

	if (GEngine)
		mTimer = GEngine->SysDriver.GetTSCLowDword();
	else
		mTimer = 0;
	mStartTime = mTimer;
	mFrameTime = mTimer;
	mViewUpdate = mTimer;
	mViewAvgTime = 1;

    mCurrentState = WARMUP;

	SetState(0);
    mTextInfo = "";
};

SimpleProfiler::~SimpleProfiler( void )
{
	SetState(0);

	if( mFrames )
	{
		delete[] mFramesTimeTable;
	}
};

void SimpleProfiler::SetState( int state )
{
	if (!GEngine) return;

    
    //out-of-range state name => ignore
    if (state > PROFILER_STATES_NUMBER)
        return;

#ifdef TRANSGAMING
	float time = GEngine->SysDriver.GetTSCLowDword();
	float diff = time - mTimer;
#else
	double time = GEngine->SysDriver.GetTSCLowDword();
	double diff = time - mTimer;
#endif

    if (diff < 0.0) {
		DWORD max = 0xffffffff;
		mTimer = mTimer - GEngine->SysDriver.GetSecondsPerCycle() * 16.0f * max;
		diff = time - mTimer;
	}

	if( mFrames == 0 )
		mTimes[mCurrentState] += diff;
	else
	{
		mFramesTimeTable[mCurrentFrame*PROFILER_STATES_NUMBER+mCurrentState] += diff;
		mFramesAvgTable[mCurrentState] += diff;
	}
	mCurrentState = state;
	mTimer = time;
};

void SimpleProfiler::AdvanceFrame( void )
{
	if( !mFrames ) return;

	int nextFrame = mCurrentFrame+1;
	if( nextFrame == mFrames )
		nextFrame = 0;
	for( int i = 0; i < PROFILER_STATES_NUMBER; ++i )
	{
#ifdef TRANSGAMING
		float &t = mFramesTimeTable[nextFrame*PROFILER_STATES_NUMBER+i];
#else
		double &t = mFramesTimeTable[nextFrame*PROFILER_STATES_NUMBER+i];
#endif
        mTimes[i] += t;
		if (mFramesMaxTable[i]<t)
			mFramesMaxTable[i]=t;
		t = 0;
	}
	mCurrentFrame = nextFrame;

	if (GEngine)
		mFrameTime = GEngine->SysDriver.GetCurrentTime();
}

#ifdef TRANSGAMING
float SimpleProfiler::GiveSummarizedTimeOfLastFrames( void ){
	if( !mFrames ) return 1;
	float summaryTime = 0;
	for( int i = 0; i < mFrames*PROFILER_STATES_NUMBER; ++i )
		summaryTime += mFramesTimeTable[i];
	return summaryTime;
}
#else
double SimpleProfiler::GiveSummarizedTimeOfLastFrames( void )
{
	if( !mFrames ) return 1;
	double summaryTime = 0;
	for( int i = 0; i < mFrames*PROFILER_STATES_NUMBER; ++i )
		summaryTime += mFramesTimeTable[i];
	return summaryTime;
}
#endif


void SimpleProfiler::ViewUpdate( void )
{
	if( !mFrames ) return;

	//double totalTime = 0;
	for( int i = 0; i < PROFILER_STATES_NUMBER; ++i )
	{
		mFramesViewAvgTable[i] = mFramesAvgTable[i];
		mFramesViewMaxTable[i] = mFramesMaxTable[i];
		mFramesAvgTable[i] = 0;
		mFramesMaxTable[i] = 0;

	}
	if (GEngine) {
		mViewAvgTime = GEngine->SysDriver.GetCurrentTime() - mViewUpdate;
		mViewUpdate = GEngine->SysDriver.GetCurrentTime();
	}

    // text info
#ifdef TRANSGAMING
    float summTime = GiveSummarizedTimeOfLastFrames();
    float aver = GiveSummarizedAverageOfLastFrames();
#else
    double summTime = GiveSummarizedTimeOfLastFrames();
    double aver = GiveSummarizedAverageOfLastFrames();
#endif
    mTextInfo = String::Sprintf("Summarized ms of last %d frames: %f\n", GiveNumberOfFrames(), summTime );
    if (GEngine && GEngine->GameState == PCFSystem::DedicatedServer)
        mTextInfo += String::Sprintf("Current FPS: %f\n",GEngine->avgFPS);
	
    mTextInfo += String::Sprintf("LUA: %d/%d%/%d\n\n", GScript.GetGCCount(), GScript.GetGCThreshold(), GScript.GetTop());

    int states = GiveNumberOfStates();
    for( int i = 0; i < states; ++i )
    {
#ifdef TRANSGAMING
        float avgTime = GiveAvgTimeForState( i );
        float maxTime = GiveMaxTimeForState( i );
#else
        double avgTime = GiveAvgTimeForState( i );
        double maxTime = GiveMaxTimeForState( i );
#endif
        const char* stateName = GetStateName( i );
        if( stateName )
            mTextInfo += String::Sprintf("[%.1f %%] [%.1fms] %s\n",  ((avgTime*100)/aver), maxTime*1000, stateName);
    }
}

#ifdef TRANSGAMING
float SimpleProfiler::GiveSummarizedAverageOfLastFrames( void ){
	return mViewAvgTime; 
};

float SimpleProfiler::GiveAvgTimeForState( int stateNumber ){
    if (stateNumber > GiveNumberOfStates())
        return 0.0;

	return mFramesViewAvgTable[stateNumber];
}

float SimpleProfiler::GiveMaxTimeForState( int stateNumber ){
    if (stateNumber > GiveNumberOfStates())
        return 0.0;

	return mFramesViewMaxTable[stateNumber];
}
#else
double SimpleProfiler::GiveSummarizedAverageOfLastFrames( void ) 
{ 
	return mViewAvgTime; 
};

double SimpleProfiler::GiveAvgTimeForState( int stateNumber )
{
    if (stateNumber > GiveNumberOfStates())
        return 0.0;

	return mFramesViewAvgTable[stateNumber];
}

double SimpleProfiler::GiveMaxTimeForState( int stateNumber )
{
    if (stateNumber > GiveNumberOfStates())
        return 0.0;

	return mFramesViewMaxTable[stateNumber];
}
#endif

const char* SimpleProfiler::GetStateName( int state )
{
    if (stateNumber > GiveNumberOfStates())
        return "<BadState>";

    return ProfilerStateNames[state];
}

#ifdef TRANSGAMING
float SimpleProfiler::GiveTimeSinceViewUpdate() {
#else
double SimpleProfiler::GiveTimeSinceViewUpdate() {
#endif
	if (GEngine)
		return (GEngine->SysDriver.GetCurrentTime() - mViewUpdate);
	else
		return 0;
}

#endif

