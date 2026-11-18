// ---------------------------------------------------------------------------
// Copyright (c) 2005 TransGaming Technologies. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2006/13
//----------------------------------------------------------------------------

#include "Engine.h"
#include "Core/FileSystem/LogBuffer.h"
#include "Core/SystemDriver.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include <msvcrt/time.h>
#include <mmsystem.h>

#include <tg_pc2ps2.h>

SystemDriver::SystemDriver()
:   
#ifndef TRANSGAMING
mOSType(EOS_None), 
#endif
mSecondsPerCycle(1), mLow(0), mHigh(0),mLastTimeStep(0)
{
#ifndef TRANSGAMING
    m_setTag("System");
	mTimerCapabilities = new TIMECAPS;
	mTimeCapsOk = ( timeGetDevCaps( (LPTIMECAPS)mTimerCapabilities, sizeof(TIMECAPS) ) == TIMERR_NOERROR );
	if( !mTimeCapsOk )
		GLog.Print( "Exact timer not available, exact sleep will be inaccurate!\n" );
	else
		GLog.Print( "Exact timer resolution: %d ms\n", ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
#endif
}

SystemDriver::~SystemDriver()
{
#ifndef TRANSGAMING
	delete (TIMECAPS*)mTimerCapabilities;
#endif
}

void SystemDriver::Initialize(HINSTANCE hinstance)
{
    char tmp[1024];
    
#ifndef TRANSGAMING
    GetModuleFileName(hinstance,tmp,sizeof(tmp));

    mExeFileName=String::BaseName(tmp);
    tmp[::strlen(tmp)-mExeFileName.Len()]=0;
    mBaseDir=tmp;
        
    DWORD size=sizeof(tmp);
    GetComputerName(tmp,&size);
    mComputerName=tmp;
#endif

    GetCPUInfo();
#ifndef TRANSGAMING
    GetPS2Info();
#endif

    // init time
    _tzset();
    srand((unsigned)time(NULL));

    ResetTimer();
}

#ifndef TRANSGAMING
void SystemDriver::GetPS2Info()
{
    mOSName = "PS2";
    mOSType = EOS_PS2;
}
#endif

/*
void SystemDriver::GetCPUInfo()
{
	LARGE_INTEGER waitTime, startCount, curCount;
	QueryPerformanceCounter(&startCount);
	QueryPerformanceFrequency(&waitTime);
	int scale = 5;      // Take 1/32 of a second for the measurement.


	waitTime.QuadPart >>= scale;

	double timestep = GetTimeStep();
	do
	{
		QueryPerformanceCounter(&curCount);
	}
	while(curCount.QuadPart - startCount.QuadPart < waitTime.QuadPart);
	timestep = GetTimeStep();

	mSecondsPerCycle = 1.0/(timestep*32.0); // Multiply by 32
}
*/

void SystemDriver::GetCPUInfo()
{
	LARGE_INTEGER waitTime, startCount, curCount;
	QueryPerformanceCounter(&startCount);
	QueryPerformanceFrequency(&waitTime);
	int scale = 5;      // Take 1/32 of a second for the measurement.
	waitTime.QuadPart >>= scale;

	__int64 start = 0;
	start = PS2_GetTimerSystemTime();
	do
	{
		QueryPerformanceCounter(&curCount);
	}
	while(curCount.QuadPart - startCount.QuadPart < waitTime.QuadPart);

	__int64 end = 0;
	end = PS2_GetTimerSystemTime();

	__int64 output = (end - start) << scale;
	mSecondsPerCycle = ((double)1.0/output);
}

const char* SystemDriver::GetTimeAsString() const
{
    static char tmp[64];
#ifdef TRANSGAMING_HACK
    tmp[0] = 0;
#else
    /* localtime() seems to enter an infinite loop because time() returns zero. */
    time_t ltime;
    time(&ltime);
    struct tm* today=localtime(&ltime);
    strftime(tmp,sizeof(tmp),"%#c",today);
#endif
    return tmp;
}

void SystemDriver::Dump(LogBuffer& log)
{
#ifndef TRANSGAMING
    const char* null="<null>";
    log.Print("CPU MHZ          : %f\n", 1.0e-6/mSecondsPerCycle);
    log.Print("Module Name      : %s\n", mExeFileName.Size() ? mExeFileName.AsChar() : null);
    log.Print("Base Directory   : %s\n", mBaseDir.Size() ? mBaseDir.AsChar() : null);
    log.Print("Computer Name    : %s\n", mComputerName.Size() ? mComputerName.AsChar() : null);
    log.Print("Operating System : %s\n", mOSName.Size() ? mOSName.AsChar() : null);
#endif
}

void SystemDriver::ExactSleep( DWORD miliseconds )
{
#ifdef TRANSGAMING
	Sleep( miliseconds );
#else
	if( mTimeCapsOk ) timeBeginPeriod( ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
	Sleep( miliseconds );
	if( mTimeCapsOk ) timeEndPeriod( ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
#endif
}

#ifndef TRANSGAMING
bool SystemDriver::Test( void )
{
	DWORD a,b,c,d,e,f,g,h;

	DWORD time = 35;
	MilisecondsToTickCount( time, a, b );

	GetPureTickCount( c, d );

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return false;

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return false;

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return false;

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return true;

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return false;

	Sleep(10);
	GetPureTickCount( e, f );
	GetTickCountDifference( c, d, e, f, g, h );
	if( TickCount_SecondHigherThanFirst( a, b, g, h ) )
		return false;
	return false;
}
#endif
