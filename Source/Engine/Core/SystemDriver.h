// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

//#pragma once
#ifndef _SYSTEMDRIVER_
#define _SYSTEMDRIVER_

class LogBuffer;

#ifdef TRANSGAMING_PSP
# include <kernel.h>
# include <sysmem.h>
#endif
#include "Core/LeString.h" 


#ifndef TRANSGAMING
struct PreciseTimer
{
	DWORD mLastFiredLow;
	DWORD mLastFiredHigh;
	DWORD mFirePeriodLow;
	DWORD mFirePeriodHigh;
};
#endif

class ENGINE_API SystemDriver
{
//#ifndef TRANSGAMING
    enum {
        EOS_WindowsNT,
        EOS_Windows2000,
        EOS_WindowsXP,
        EOS_Windows95,
        EOS_Windows98,
        EOS_WindowsMe,  
        EOS_WindowsWin32s,
# if defined( TRANSGAMING_PS2 )
	    EOS_PS2,
# elif defined( TRANSGAMING_PSP )
	    EOS_PSP,
# endif
        EOS_None
    };
//#endif

public:
    SystemDriver();
    ~SystemDriver();

    void Initialize(HINSTANCE hinstance);

#ifdef TRANSGAMING
    const float     GetSecondsPerCycle() const { return mSecondsPerCycle; }
#else
    const char *    GetBaseDir() const { return mBaseDir.AsChar(); }
    String          GetExePath() const { String s = mBaseDir; s += mExeFileName; return s; }
    const double    GetSecondsPerCycle() const { return mSecondsPerCycle; }
#endif
    const char*     GetTimeAsString() const;

    void Dump(LogBuffer& log);

#ifdef TRANSGAMING
# ifdef SIMPLE_PROFILER
	inline float GetTSCLowDword();
# endif
    inline void FudgeCurrentTime(float deltaTime);
	//inline float GetTimeFromTimerReset();
    inline float GetCurrentTime() const;
#else
# ifdef SIMPLE_PROFILER
	inline double GetTSCLowDword();
# endif
	inline double GetTimeFromTimerReset();
    inline double GetCurrentTime() const;
#endif
	inline DWORD  GetCurrentTimeMS() const;
#ifdef TRANSGAMING
    inline float GetTimeStep();
	inline float GetLastTimeStep();
#else
    inline double GetTimeStep();
	inline double GetLastTimeStep();
#endif
	inline void   ResetTimer();
	void ExactSleep( DWORD miliseconds );

#ifndef TRANSGAMING
	// By Krystian; for ultimately precise timer
	inline void   GetPureTickCount( DWORD& low, DWORD& high );
	inline void   MilisecondsToTickCount( DWORD& MS, DWORD& LO, DWORD& HO );
	inline bool   Test( void );

	// Precise timers subsystem
	inline int    CreateTimer( DWORD periodMs, bool fireAtOnce = true );
	inline void   SetTimerPeriod( int timerIndex, DWORD periodMs );
	inline bool   TestTimer( int timerIndex );

#endif
#if !defined ( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS)
    String GetOSName() const { return mOSName; }
#endif

private:
//#ifndef TRANSGAMING
# if defined( TRANSGAMING_PS2 )
    void GetPS2Info();
# elif defined( TRANSGAMING_PSP )
    void GetPSPInfo();
# else
    void GetWindowsInfo();
# endif
//#endif
    void GetCPUInfo();

#if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
	// for ExactSleep()
	void*		mTimerCapabilities;
	bool		mTimeCapsOk;
#endif

private:
#if defined( TRANSGAMING ) && !defined( TRANSGAMING_WINDOWS )
    float       mLastTimeStep;
#else
    double      mLastTimeStep;
    String      mExeFileName;
    String      mBaseDir;
    String      mComputerName;
    String      mOSName;

    int         mOSType;
#endif

#ifdef TRANSGAMING_PSP
	ulong128  mTime;
    float               mSecondsPerCycle;
#else
	DWORD		mLow,mHigh;
    double      mSecondsPerCycle;
#endif

#ifndef TRANSGAMING
	// Precise timer subsystem
	DynamicArray<PreciseTimer> mPreciseTimers;
#endif
};

inline void SystemDriver::ResetTimer()
{
#ifdef TRANSGAMING_PSP
    mTime = sceKernelGetSystemTimeWide();
#elif defined(TRANSGAMING_PS2)
	//TRACE("SystemDriver::ResetTimer() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER t;

    QueryPerformanceCounter(&t);

    mLow = t.QuadPart & 0xffffffff;
	mHigh = (t.QuadPart >> 32) & 0xffffffff;
#else
	DWORD L, H;

	__asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		mov   [L],eax   // Save low value.
		mov   [H],edx   // Save high value.
	}

	mLow = L;
	mHigh = H;
#endif
}

#ifdef TRANSGAMING
inline float SystemDriver::GetCurrentTime() const
#else
inline double SystemDriver::GetCurrentTime() const
#endif
{
	DWORD L,H;

#ifdef TRANSGAMING_PSP
    return sceKernelGetSystemTimeWide() * 0.000001f;
#elif defined(TRANSGAMING_PS2)
	//TRACE("SystemDriver::GetCurrentTime() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER t;

    QueryPerformanceCounter(&t);

    L = t.QuadPart & 0xffffffff;
	H = (t.QuadPart >> 32) & 0xffffffff;

	return ((float)L +  (4096.0 * 1024.0 * 1024.0) * (float)H) * mSecondsPerCycle;
#else
	__asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		mov   [L],eax   // Save low value.
		mov   [H],edx   // Save high value.
	}

    return ((double)L +  (4096.0 * 1024.0 * 1024.0) * (double)H) * mSecondsPerCycle;
#endif
}

inline DWORD SystemDriver::GetCurrentTimeMS() const
{
#ifdef TRANSGAMING_PSP
    return (DWORD)(sceKernelGetSystemTimeWide() * 0.001f);
    
#elif defined(TRANSGAMING_PS2)
    const DWORD millisecondsPerCycle = (DWORD)floor(mSecondsPerCycle * 0.001);
	LARGE_INTEGER t;


    QueryPerformanceCounter(&t);

	//TRACE("SystemDriver::GetCurrentTimeMS() - HACK!! : This function is not working in it's current state.");
    return (DWORD)(t.QuadPart * millisecondsPerCycle);
#else
	DWORD cyclesPerMilisecond = floor(0.001/mSecondsPerCycle);

	DWORD result;

	__asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		div   [cyclesPerMilisecond]
		mov   [result],eax   // Save low value.
	}

	return result;
#endif
}

#ifdef SIMPLE_PROFILER
# ifdef TRANSGAMING
inline float SystemDriver::GetTSCLowDword() {
# else
inline double SystemDriver::GetTSCLowDword() {
# endif

# ifdef TRANSGAMING_PSP
    return sceKernelGetSystemTimeLow() * 0.000001f;

# elif defined(TRANSGAMING_PS2)
	//TRACE("SystemDriver::GetTSCLowDword() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER t;


    QueryPerformanceCounter(&t);

    return (t.QuadPart & 0xfffffff0) * mSecondsPerCycle;
# else
	DWORD L_8;

    __asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31
		shrd eax,edx,4	// decrase precision a little
		mov   [L_8],eax   // Save low value.

	}

	return L_8 * mSecondsPerCycle * 16.0f;
# endif
}
#endif

#ifdef TRANSGAMING
//inline float SystemDriver::GetTimeFromTimerReset()
//{
#else
inline double SystemDriver::GetTimeFromTimerReset()
{
#endif
	//DWORD mLow = this->mLow;
	//DWORD mHigh = this->mHigh;
#ifdef TRANSGAMING_PSP
    //return (sceKernelGetSystemTimeWide() - mTime) * 0.000001f;

#elif defined(TRANSGAMING_PS2)
	/*//TRACE("SystemDriver::GetTimeFromTimerReset() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER       currTime;
    const ulong128     lastTime = mLow + (((ulong128)mHigh) << 32);


    QueryPerformanceCounter(&currTime);

    return ((float)(currTime.QuadPart - lastTime)) * mSecondsPerCycle;//*/
#else
	DWORD L,H;


    __asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		mov   [L],eax   // Save low value.
		mov   [H],edx   // Save high value.
	}

    
    DWORD high = mHigh;
    if (L<mLow)
	{
		high++;	// overflow in low DWORD
	}

	double timeStep = ((double)(L-mLow) +  (4096.0 * 1024.0 * 1024.0) * (double)(H-high)) * mSecondsPerCycle;

	return timeStep;
#endif
#ifdef TRANSGAMING
//}
#else
}
#endif


#ifdef TRANSGAMING
inline void SystemDriver::FudgeCurrentTime(float deltaTime){
# ifdef TRANSGAMING_PSP
    ulong128 currTime = sceKernelGetSystemTimeWide();

    mTime = currTime + ((ulong128)(deltaTime * 1000000.0f));
# elif defined(TRANSGAMING_PS2)
    LARGE_INTEGER   t;
    ulong128        currTime;


    QueryPerformanceCounter(&t);

    currTime = t.QuadPart;

    currTime += ((ulong128)(deltaTime / mSecondsPerCycle));
    mLow = (DWORD)(currTime & 0xffffffff);
    mHigh = (DWORD)((currTime >> 32) & 0xffffffff);
# endif
}
#endif

#ifdef TRANSGAMING
inline float SystemDriver::GetTimeStep()
#else
inline double SystemDriver::GetTimeStep()
#endif
{
#ifdef TRANSGAMING_PSP
    ulong128 currTime = sceKernelGetSystemTimeWide();


    mLastTimeStep = (currTime - mTime) * 0.000001f;
    mTime = currTime;

    return mLastTimeStep;

#elif defined(TRANSGAMING_PS2)
	//TRACE("SystemDriver::GetTimeStep() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER currTime;
    const ulong128 lastTime = mLow + (((ulong128)mHigh) << 32);

    QueryPerformanceCounter(&currTime);

    float timeStep = (float)((currTime.QuadPart - lastTime) * mSecondsPerCycle);

    mLow = currTime.QuadPart & 0xffffffff;
    mHigh = (currTime.QuadPart >> 32) & 0xffffffff;


    mLastTimeStep = timeStep;

    return timeStep;
#else
	DWORD L,H;

    
    __asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		mov   [L],eax   // Save low value.
		mov   [H],edx   // Save high value.
	}

    if (L<mLow)
	{
		mHigh++;	// overflaw in low DWORD
	}

	double timeStep = ((double)(L-mLow) +  (4096.0 * 1024.0 * 1024.0) * (double)(H-mHigh)) * mSecondsPerCycle;
	mLow = L;
	mHigh = H;

	mLastTimeStep = timeStep;
	return timeStep;
#endif
}

#ifdef TRANSGAMING
inline float SystemDriver::GetLastTimeStep()
#else
inline double SystemDriver::GetLastTimeStep()
#endif
{
	return mLastTimeStep;
}


#ifndef TRANSGAMING
inline void SystemDriver::GetPureTickCount( DWORD& l, DWORD& h )
{
#ifdef TRANSGAMING_PSP
    ulong128 t = sceKernelGetSystemTimeWide();


    l = (DWORD)(t & 0xffffffff);
    h = (DWORD)((t >> 32) & 0xffffffff);
    
#elif defined(TRANSGAMING_PS2)
	//TRACE("SystemDriver::GetPureTickCount() - HACK!! : This function is not working in it's current state.");
    LARGE_INTEGER t;


    QueryPerformanceCounter(&t);

    l = (t.QuadPart & 0xffffffff);
    h = (t.QuadPart >> 32) & 0xffffffff;
#else
	DWORD L,H;

    
    __asm
	{
		xor   eax,eax	// Required so that VC++ realizes EAX is modified.
		xor   edx,edx	// Required so that VC++ realizes EDX is modified.
		_emit 0x0F		// Read Time Stamp Counter -  Pentium+ time stamp register to EDX:EAX.
		_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
		mov   [L],eax   // Save low value.
		mov   [H],edx   // Save high value.
	}

	l = L; h = H;
#endif
}


inline void GetTickCountDifference( DWORD& l1, DWORD& h1, DWORD& l2, DWORD& h2, DWORD& lo, DWORD& ho )
{
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
    union int64_int{
        struct{
            DWORD lo;
            DWORD hi;
        };

        ulong128 value;
    } t1, t2;

    
    
    //TRACE("SystemDriver::GetTickCountDifference() - HACK!! : This function is not working in it's current state.");
    t1.lo = l1;
    t1.hi = h1;
    t2.lo = l2;
    t2.hi = h2;

    t2.value -= t1.value; 

    lo = t2.lo;
    ho = t2.hi;
#else
	DWORD L1=l1,H1=h1,L2=l2,H2=h2,LO,HO;

    
    __asm
	{
		mov   eax,[L2]
		mov   edx,[H2]
		sub   eax,[L1]
		sbb   edx,[H1]
		mov   [LO],eax
		mov   [HO],edx
	}

	lo = LO; ho = HO;
#endif
}


inline DWORD TickCount_SecondHigherThanFirst( DWORD& l1, DWORD& h1, DWORD& l2, DWORD& h2 )
{
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	//TRACE("SystemDriver::TickCount_SecondHigherThanFirst() - HACK!! : This function is not working in it's current state.");
    
    //compare high words for a quick discard
    if (h2 > h1)
        return 1;

    else if (h2 < h1)
        return 0;

    
    //compare lo words if high words are equal
    if (l2 > l1)
        return 1;

    else
        return 0;
#else
	DWORD L1=l1,H1=h1,L2=l2,H2=h2;
	DWORD H = 1;

    
    __asm
	{
		mov   eax,[H2]
		cmp	  eax,[H1]
		ja    labelHigher
		jb    labelLowerEqual
		mov   eax,[L2]
		cmp   eax,[L1]
		ja    labelHigher
labelLowerEqual:
		xor   eax,eax
		mov   [H],eax
labelHigher:
	}

	return H;
#endif
}


inline void SystemDriver::MilisecondsToTickCount( DWORD& ms, DWORD& lo, DWORD& ho )
{
	const DWORD CPS = (DWORD)(0.001 / mSecondsPerCycle);	// cycles per milisecond
#if defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP )
	//TRACE("SystemDriver::MillisecondsToTickCount() - HACK!! : This function is not working in it's current state.");
    union int64_int{
        struct{
            DWORD lo;
            DWORD hi;
        };

        ulong128 value;
    } tmp;


    tmp.value = ms;
    tmp.value *= CPS;

    lo = tmp.lo;
    ho = tmp.hi;
#else
	DWORD MS = ms,LO,HO;


	__asm
	{
		mov   eax,[MS]
		mov   ecx,[CPS]
		mul   ecx
		mov   [LO],eax
		mov   [HO],edx
	}

	lo=LO;ho=HO;
#endif
}


inline int SystemDriver::CreateTimer( DWORD periodMs, bool fireAtOnce )
{
	PreciseTimer timer;
	MilisecondsToTickCount( periodMs, timer.mFirePeriodLow, timer.mFirePeriodHigh );
	DWORD l,h;
	GetPureTickCount( l, h );
	if( fireAtOnce )
	{
		DWORD diffL, diffH;
		GetTickCountDifference( timer.mFirePeriodLow, timer.mFirePeriodHigh, l, h, diffL, diffH );
		timer.mLastFiredLow = diffL;
		timer.mLastFiredHigh = diffH;
	}
	else
	{
		timer.mLastFiredLow = l;
		timer.mLastFiredHigh = h;
	}

	return mPreciseTimers.Add( timer );
}

inline void SystemDriver::SetTimerPeriod( int timerIndex, DWORD periodMs )
{
	Assert( ( timerIndex >= 0 ) && ( timerIndex < mPreciseTimers.Size() ) );
	PreciseTimer& timer = mPreciseTimers[timerIndex];
	MilisecondsToTickCount( periodMs, timer.mFirePeriodLow, timer.mFirePeriodHigh );
}

inline bool SystemDriver::TestTimer( int timerIndex )
{
	Assert( ( timerIndex >= 0 ) && ( timerIndex < mPreciseTimers.Size() ) );
	PreciseTimer& timer = mPreciseTimers[timerIndex];
	DWORD l,h, diffL, diffH;
	GetPureTickCount( l, h );
	GetTickCountDifference( timer.mLastFiredLow, timer.mLastFiredHigh, l, h, diffL, diffH );
	if( TickCount_SecondHigherThanFirst( timer.mFirePeriodLow, timer.mFirePeriodHigh, diffL, diffH ) )
	{
		timer.mLastFiredLow = l;
		timer.mLastFiredHigh = h;
		return true;
	}

	return false;
}
#endif

#endif //_SYSTEMDRIVER_


