// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#pragma once

class LogBuffer;

#include "LeString.h"

struct PreciseTimer
{
	DWORD mLastFiredLow;
	DWORD mLastFiredHigh;
	DWORD mFirePeriodLow;
	DWORD mFirePeriodHigh;
};

class	SystemDriver
{
	enum
	{
		EOS_WindowsNT,
		EOS_Windows2000,
		EOS_WindowsXP,
		EOS_Windows95,
		EOS_Windows98,
		EOS_WindowsMe,
		EOS_WindowsWin32s,
		EOS_None
	};

public:
	SystemDriver();
	~SystemDriver();

	void			Initialize(void);

	const char*		GetBaseDir() const { return mBaseDir.AsChar(); }
	String			GetExePath() const { String s = mBaseDir; s += mExeFileName; return s; }
	const double	GetSecondsPerCycle() const { return mSecondsPerCycle; }
	const char*		GetTimeAsString() const;
	double			mLastTimeStep;

	void			Dump(LogBuffer& log);

	inline double	GetTSCLowDword();
	inline double	GetTimeFromTimerReset();
	inline double	GetCurrentTime() const;
	inline DWORD	GetCurrentTimeMS() const;
	inline double	GetTimeStep();
	inline double	GetLastTimeStep();
	inline void		ResetTimer();
	void			ExactSleep( DWORD miliseconds );

	// By Krystian; for ultimately precise timer
	inline void		GetPureTickCount( DWORD& low, DWORD& high );
	inline void		MilisecondsToTickCount( DWORD& MS, DWORD& LO, DWORD& HO );
	bool			Test( void );

	// Precise timers subsystem
	inline int		CreateTimer( DWORD periodMs, bool fireAtOnce = true );
	inline void		SetTimerPeriod( int timerIndex, DWORD periodMs );
	inline bool		TestTimer( int timerIndex );

	String			GetOSName() const { return mOSName; }

private:
	void			GetWindowsInfo();
	void			GetCPUInfo();

	// for ExactSleep()
	void*			mTimerCapabilities;
	bool			mTimeCapsOk;

private:
	String			mExeFileName;
	String			mBaseDir;
	String			mComputerName;
	String			mOSName;
	int				mOSType;
	DWORD			mLow, mHigh;

	double			mSecondsPerCycle;

	// Precise timer subsystem
	DynamicArray<PreciseTimer> mPreciseTimers;
};

inline void SystemDriver::ResetTimer()
{
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
}


inline double SystemDriver::GetCurrentTime() const
{
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

	return ((double)L +  (4096.0 * 1024.0 * 1024.0) * (double)H) * mSecondsPerCycle;
}

inline DWORD SystemDriver::GetCurrentTimeMS() const
{
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
}

inline double SystemDriver::GetTSCLowDword() {
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
}

inline double SystemDriver::GetTimeFromTimerReset()
{
	DWORD mLow = this->mLow;
	DWORD mHigh = this->mHigh;
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
		mHigh++;	// overflow in low DWORD
	}

	double timeStep = ((double)(L-mLow) +  (4096.0 * 1024.0 * 1024.0) * (double)(H-mHigh)) * mSecondsPerCycle;

	return timeStep;
}


inline double SystemDriver::GetTimeStep()
{
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
}

inline double SystemDriver::GetLastTimeStep()
{
	return mLastTimeStep;
}

inline void SystemDriver::GetPureTickCount( DWORD& l, DWORD& h )
{
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
}

inline void GetTickCountDifference( DWORD& l1, DWORD& h1, DWORD& l2, DWORD& h2, DWORD& lo, DWORD& ho )
{
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
}

inline DWORD TickCount_SecondHigherThanFirst( DWORD& l1, DWORD& h1, DWORD& l2, DWORD& h2 )
{
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
}

inline void SystemDriver::MilisecondsToTickCount( DWORD& ms, DWORD& lo, DWORD& ho )
{
	DWORD MS=ms,LO,HO;
	DWORD CPS = (DWORD)(0.001/mSecondsPerCycle);	// cycles per milisecond
	__asm
	{
		mov   eax,[MS]
		mov   ecx,[CPS]
		mul   ecx
		mov   [LO],eax
		mov   [HO],edx
	}
	lo=LO;ho=HO;
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
