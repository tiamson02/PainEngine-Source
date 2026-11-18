// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#include "Engine.h"
#include "LogBuffer.h"
#include "SystemDriver.h"

#include <time.h>
#include <mmsystem.h>

#define PKTV_MAXPATH 1024

SystemDriver::SystemDriver()
:   mOSType(EOS_None), mSecondsPerCycle(1), mLow(0), mHigh(0),mLastTimeStep(0)
{
	mTimerCapabilities = new TIMECAPS;
	mTimeCapsOk = ( timeGetDevCaps( (LPTIMECAPS)mTimerCapabilities, sizeof(TIMECAPS) ) == TIMERR_NOERROR );
	if( !mTimeCapsOk )
		GLog->Print( "Exact timer not available, exact sleep will be inaccurate!\n" );
	else
		GLog->Print( "Exact timer resolution: %d ms\n", ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
}

SystemDriver::~SystemDriver()
{
	delete (TIMECAPS*)mTimerCapabilities;
}

void SystemDriver::Initialize(void)
{
	char tmp[PKTV_MAXPATH];
    GetModuleFileName(NULL,tmp,PKTV_MAXPATH);

    mExeFileName=String::BaseName(tmp);
    tmp[::strlen(tmp)-mExeFileName.Len()]=0;
    mBaseDir=tmp;
        
    DWORD size=sizeof(tmp);
    GetComputerName(tmp,&size);
    mComputerName=tmp;

    GetCPUInfo();
    GetWindowsInfo();

    // init time
    _tzset();
    srand((unsigned)time(NULL));

	ResetTimer();
}

void SystemDriver::GetWindowsInfo()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
    if(FALSE==bOsVersionInfoEx) {
        // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if(!GetVersionEx((OSVERSIONINFO*) &osvi)) {
            // couldnt retrieve windows version
            mOSName="UNKNOWN";
            mOSType=EOS_None;
            return;
        }
    }

    switch(osvi.dwPlatformId) {
        case VER_PLATFORM_WIN32_NT:
            if(osvi.dwMajorVersion <= 4) {
                mOSName+="Microsoft Windows NT ";
                mOSType=EOS_WindowsNT;
            }

            if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
                mOSName+="Microsoft Windows 2000 ";
                mOSType=EOS_Windows2000;
            }

            if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
                mOSName+="Microsoft Windows XP ";
                mOSType=EOS_WindowsXP;
            }

            // product type.
            if(bOsVersionInfoEx) {
                if(osvi.wProductType == VER_NT_WORKSTATION) {
                    if(osvi.wSuiteMask & VER_SUITE_PERSONAL) {
                        mOSName+="Personal ";
                    }
                    else {
                        mOSName+="Professional ";
                    }
                }
                else 
                if(osvi.wProductType == VER_NT_SERVER) {
                    if(osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                        mOSName+="DataCenter Server ";
                    }
                    else
                    if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                        mOSName+="Advanced Server ";
                    }
                    else {
                        mOSName+="Server ";
                    }
                }
            }
            else {
                HKEY hKey;
                char szProductType[80];
                DWORD dwBufLen;

                RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                             "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
                             0, KEY_QUERY_VALUE, &hKey);

                RegQueryValueEx(hKey, "ProductType", NULL, NULL,
                                (LPBYTE) szProductType, &dwBufLen);
                RegCloseKey(hKey);
                if(lstrcmpi("WINNT", szProductType) == 0) {
                    mOSName+="Professional ";
                }
                else
                if(lstrcmpi("LANMANNT", szProductType) == 0) {
                    mOSName+="Server ";
                }
                else
                if(lstrcmpi("SERVERNT", szProductType) == 0) {
                    mOSName+="Advanced Server ";
                }
            }

            // get version, service pack (if any), and build number.
            if(osvi.dwMajorVersion <= 4) {
                char tmp[128];
                sprintf(tmp,"version %d.%d %s (Build %d)",
                        osvi.dwMajorVersion,
                        osvi.dwMinorVersion,
                        osvi.szCSDVersion,
                        osvi.dwBuildNumber & 0xFFFF);
                mOSName+=tmp;
            }
            else
            { 
                char tmp[128];
                sprintf(tmp,"%s (Build %d)",
                        osvi.szCSDVersion,
                        osvi.dwBuildNumber & 0xFFFF);
                mOSName+=tmp;
            }        

            break;

        case VER_PLATFORM_WIN32_WINDOWS:
            if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
                mOSName+="Microsoft Windows 95 ";
                mOSType=EOS_Windows95;
                if(osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B') {
                    mOSName+="OSR2 ";
                }
            } 

            if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
                mOSName+="Microsoft Windows 98 ";
                mOSType=EOS_Windows98;
                if(osvi.szCSDVersion[1] == 'A') {
                   mOSName+="SE ";
                }
            } 

            if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
                 mOSName+="Microsoft Windows Me ";
                 mOSType=EOS_WindowsMe;
            } 
            break;

        case VER_PLATFORM_WIN32s:

            mOSName+="Microsoft Win32s ";
            mOSType=EOS_WindowsWin32s;
            break;
    }
}
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

	__int64 start;
	{
		ulong32* pointer = (ulong32 *)&start;
		__asm
		{
			mov		ecx, pointer
			rdtsc
			mov		[ecx],     eax
			mov		[ecx+4],   edx
		}
	}

	do
	{
		QueryPerformanceCounter(&curCount);
	}
	while(curCount.QuadPart - startCount.QuadPart < waitTime.QuadPart);

	__int64 end;
	{
		ulong32* pointer = (ulong32 *)&end;
		__asm
		{
			mov		ecx, pointer
			rdtsc
			mov		[ecx],     eax
			mov		[ecx+4],   edx
		}
	}

	_int64 output = (end - start) << scale;
	mSecondsPerCycle = ((double)1.0/output);
}

const char* SystemDriver::GetTimeAsString() const
{
    static char tmp[64];
    time_t ltime;
    time(&ltime);
    struct tm* today=localtime(&ltime);
    strftime(tmp,sizeof(tmp),"%#c",today);
    return tmp;
}

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

void SystemDriver::Dump(LogBuffer& log)
{
    const char* null="<null>";
    log.Print("CPU MHZ         : %f\n", 1.0e-6/mSecondsPerCycle);
    log.Print("Module Name     : %s\n", mExeFileName.Size() ? mExeFileName.AsChar() : null);
    log.Print("Base Directory  : %s\n", mBaseDir.Size() ? mBaseDir.AsChar() : null);
    log.Print("Computer Name   : %s\n", mComputerName.Size() ? mComputerName.AsChar() : null);
    log.Print("Operating System: %s\n", mOSName.Size() ? mOSName.AsChar() : null);
}

void SystemDriver::ExactSleep( DWORD miliseconds )
{
	if( mTimeCapsOk ) timeBeginPeriod( ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
	Sleep( miliseconds );
	if( mTimeCapsOk ) timeEndPeriod( ((TIMECAPS*)mTimerCapabilities)->wPeriodMin );
}
