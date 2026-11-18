/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/timedate/hkTimedate.h>
#include <dolphin.h>
/* // For reference:

typedef struct OSCalendarTime
{
    int sec;    // seconds after the minute [0, 61]
    int min;    // minutes after the hour [0, 59]
    int hour;   // hours since midnight [0, 23]
    int mday;   // day of the month [1, 31]
    int mon;    // month since January [0, 11]
    int year;   // years in AD [1, ...]
    int wday;   // days since Sunday [0, 6]
    int yday;   // days since January 1 [0, 365]

    int msec;   // milliseconds after the second [0,999]
    int usec;   // microseconds after the millisecond [0,999]
} OSCalendarTime;
*/

static const float g_fSecsPerYear = ( 365.25f * 24 * 60 * 60 ); // days * hours * mins * secs

hkSystemTime HK_CALL hkTimeDate::mktime(const hkTimeStruct& thetime)
{

	OSCalendarTime ct;
	ct.msec = 0;
	ct.usec = 0;
	ct.hour = thetime.tm_hour;
	ct.mday = thetime.tm_mday;
	ct.min = thetime.tm_min;
	ct.mon = thetime.tm_mon;
 	ct.sec = thetime.tm_sec;
 	ct.wday = thetime.tm_wday;
 	ct.yday = thetime.tm_yday;
 	ct.year = thetime.tm_year + 1900; // ct is AD, tm is from 1970

	OSTime t = OSCalendarTimeToTicks(&ct);	
	return static_cast<hkSystemTime>( (t / OS_TIMER_CLOCK) + (g_fSecsPerYear * 30) ); // convert to seconds from 1970
}

hkSystemTime HK_CALL hkTimeDate::time()
{
	return (OSGetTime() / OS_TIMER_CLOCK) + (g_fSecsPerYear * 30); // secs from 1970
}

hkTimeDate::hkTimeStruct HK_CALL hkTimeDate::gmtime(const hkSystemTime& timer)
{
	OSCalendarTime ct;
	hkTimeStruct returnStruct;
	
	// timer is secs from Jan 1st, 1970, so change to GameCube system time (Jan 1st 2000)
	// convert from secs to OSTime and then to calander time
	
	hkSystemTime ticksFrom2000 = (timer - (g_fSecsPerYear * 30) ) * OS_TIMER_CLOCK;
	OSTicksToCalendarTime(ticksFrom2000 , &ct);	
	
	returnStruct.tm_hour = ct.hour;
	returnStruct.tm_isdst = 0;
	returnStruct.tm_mday = ct.mday;;
	returnStruct.tm_min = ct.min;
	returnStruct.tm_mon = ct.mon;
	returnStruct.tm_sec = ct.sec;
	returnStruct.tm_wday = ct.wday;
	returnStruct.tm_yday = ct.yday;
	returnStruct.tm_year = ct.year - 1900; // ct is AD, tm is from 1970

	return returnStruct;
}


/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
