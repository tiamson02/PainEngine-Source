// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/09 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _SIMPLEPROFILER_
#define _SIMPLEPROFILER_


#include "Engine.h"
#include "Core/LeString.h"

#ifdef TRANSGAMING
# undef SIMPLE_PROFILER
#else
# ifndef FINAL_BUILD
#  define SIMPLE_PROFILER
# endif
#endif

#ifdef TRANSGAMING_MEMORY_CONSTRAINTS
// Blair - Now that we have more PSP memory allocated, we can maybe ramp this up,
// but since we still have lots of things to load, perhaps we should keep things
// lean until we know how much we can loosen the belt.
// Eric - NOTE: the actual number of named states for the profiler is 50 and in
//   the current version it will crash when attempting to set the state for an
//   out-of-range state name because bounds aren't checked.  I've disabled the
//   profiler for now and will fix it later 
//* FIXED: The mCurrentState member variable was never being set and was them
//*  being used to access the various profiler arrays.
#define PROFILER_STATES_NUMBER 5   
#else
#define PROFILER_STATES_NUMBER 100
#endif

enum ProfilerStates {
	WARMUP,	/* this one must be first */
	INPUTSYS_TICK,

	PHYSICS_TICK,

	PHYSICS_TICK_STEP_DELTA_TIME,
	PHYSICS_TICK_TICK_PO,
	PHYSICS_TICK_TICK_RAGDOLL,
	PHYSICS_TICK_UPDATE_ENTITIES,
	PHYSICS_TICK_PLAYER_CONTROLLER,
	PHYSICS_TICK_MISSILE_CONTROLLER,

	AUDIO_TICK,

	AUDIO_TICK_STOP_FINISHED,
	AUDIO_TICK_UPDATE_SPEED,
	AUDIO_TICK_UPDATE_3D,
	AUDIO_TICK_UPDATE_2D,
	AUDIO_TICK_FLIP_3D,
	AUDIO_TICK_OBSTRUCT,

	WORLD_TICK,
	UPDATE_LOGIC,
	UPDATE_LOGIC1,
	UPDATE_LOGIC2a,
	UPDATE_LOGIC2b,
	UPDATE_LOGIC2c,
	UPDATE_LOGIC3a,
	UPDATE_LOGIC3b,
	UPDATE_LOGIC3c,
	UPDATE_LOGIC4a,
	UPDATE_LOGIC5,
	CLIENT_RENDER,
	CLIENT_RENDER_RENDER_WORLD,
	CLIENT_RENDER_LUA,
	CLIENT_RENDER_HUD,
	CLIENT_RENDER_OCCLUDE,
	CLIENT_RENDER_OCCLUDE_ANTIPORTALS_CALCULATION,
	CLIENT_RENDER_OCCLUDE_ENTITY_VISIBILITY,
	CLIENT_RENDER_OCCLUDE_PORTAL_VISIBILITY,
	CLIENT_RENDER_OCCLUDE_ANTIP_BOXTEST,
	NET_PRE_TICK,
	NET_GSCRIPTS,
	NET_POST_TICK,
	NET_SEND_UPDATE_MESSAGE,
	NET_SEND_SCRIPT_RELIABLE,
	NET_SEND_SCRIPT_UNRELIABLE,
	NET_SEND_UPDATE_MESGS,
	NET_SEND_CREATE_DELETE_OTHER_MESGS,
	NET_TICK_CHANNELS,
	NET_OTHER,
	MESSAGE_PUMP,
	PATHFINDING,
	HAVOK_INDEPENDENT_PLAYER_ACTION,

	TEMP_STATE,

	OTHER, /* this one must be last */
};

#ifdef SIMPLE_PROFILER

class ENGINE_API SimpleProfiler
{
public:
	SimpleProfiler( bool lastFrames = true, int frames = 10 );
	~SimpleProfiler( void );
	void SetState( int state );
	int GetState( void ) { return mCurrentState; };

	// Last frames methods
	int GiveNumberOfStates( void ) { return PROFILER_STATES_NUMBER; };
	int GiveNumberOfFrames( void ) { return mFrames; };
	void AdvanceFrame( void );

#ifdef TRANSGAMING
    float GiveSummarizedTimeOfLastFrames( void );
	float GiveAvgTimeForState( int stateNumber );
	float GiveMaxTimeForState( int stateNumber );
#else
    double GiveSummarizedTimeOfLastFrames( void );
	double GiveAvgTimeForState( int stateNumber );
	double GiveMaxTimeForState( int stateNumber );
#endif
	const char* GetStateName( int state );

	// Average stats methods
	void ViewUpdate( void );
#ifdef TRANSGAMING
	float GiveSummarizedAverageOfLastFrames( void );
	float GiveLastFramesAverageForState( int stateNumber );
    float GiveTimeSinceViewUpdate( void );
#else
	double GiveSummarizedAverageOfLastFrames( void );
	double GiveLastFramesAverageForState( int stateNumber );
    double GiveTimeSinceViewUpdate( void );
#endif
    String GetTextInfo() {return mTextInfo;};

protected:
#ifdef TRANSGAMING
	float mTimes[PROFILER_STATES_NUMBER];
	float mFramesAvgTable[PROFILER_STATES_NUMBER];
	float mFramesMaxTable[PROFILER_STATES_NUMBER];
	float mFramesViewAvgTable[PROFILER_STATES_NUMBER];
	float mFramesViewMaxTable[PROFILER_STATES_NUMBER];
	float mTimer;
	float mStartTime;
#else
	double mTimes[PROFILER_STATES_NUMBER];
	double mFramesAvgTable[PROFILER_STATES_NUMBER];
	double mFramesMaxTable[PROFILER_STATES_NUMBER];
	double mFramesViewAvgTable[PROFILER_STATES_NUMBER];
	double mFramesViewMaxTable[PROFILER_STATES_NUMBER];
	double mTimer;
	double mStartTime;
#endif
    int mCurrentState;
    String mTextInfo;

	// Last frames data
	int mFrames;
	int mCurrentFrame;
#ifdef TRANSGAMING
	float * mFramesTimeTable;
	float   mViewAvgTime;
	float   mFrameTime;
	float   mViewUpdate;
#else
	double* mFramesTimeTable;
	double  mViewAvgTime;
	double  mFrameTime;
	double  mViewUpdate;
#endif
};

#else
// not defined SIMPLE_PROFILER

class ENGINE_API SimpleProfiler
{
public:
	SimpleProfiler( bool lastFrames = true, int frames = 10 ) {};
	~SimpleProfiler( void ) {};
	int GetState( void ) { return 0; };
	void SetState( int state ) {};

	// Last frames methods
	void AdvanceFrame( void ) {};
#ifdef TRANSGAMING
	float  GiveSummarizedTimeOfLastFrames( void ) { return 1.0f; };
    float  GiveAvgTimeForState( int stateNumber ) { return 1.0f; };
    float  GiveMaxTimeForState( int stateNumber ) { return 1.0f; };
	float  GiveLastFramesTimeForState( int stateNumber ) { return 1.0f; };
#else
	double GiveSummarizedTimeOfLastFrames( void ) { return 1.0; };
    double GiveAvgTimeForState( int stateNumber ) { return 1.0; };
    double GiveMaxTimeForState( int stateNumber ) { return 1.0; };
	double GiveLastFramesTimeForState( int stateNumber ) { return 1.0; };
#endif
    int GiveNumberOfStates( void ) { return PROFILER_STATES_NUMBER; };
	int GiveNumberOfFrames( void ) { return 0; };
    const char* GetStateName( int state ) {return "";};
    String GetTextInfo() {return "";};

	// Average stats methods
	void ViewUpdate( void ) {};
#ifdef TRANSGAMING
	float  GiveSummarizedAverageOfLastFrames( void ) { return 1.0f; };
	float  GiveLastFramesAverageForState( int stateNumber ) { return 0.0f; };
	float  GiveTimeSinceViewUpdate( void ) { return 1.0f; };
#else
	double GiveSummarizedAverageOfLastFrames( void ) { return 1.0; };
	double GiveLastFramesAverageForState( int stateNumber ) { return 0.0; };
	double GiveTimeSinceViewUpdate( void ) { return 1.0; };
#endif
};

#endif


#endif //_SIMPLEPROFILER_

