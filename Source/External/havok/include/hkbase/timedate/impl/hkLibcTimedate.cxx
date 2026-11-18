/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/timedate/hkTimedate.h>

extern "C"
{
#include <time.h>
}

hkSystemTime HK_CALL hkTimeDate::mktime(const hkTimeStruct& thetime)
{
	tm argStruct;

	argStruct.tm_hour = thetime.tm_hour;
	argStruct.tm_isdst = thetime.tm_isdst;
	argStruct.tm_mday = thetime.tm_mday;
	argStruct.tm_min = thetime.tm_min;
	argStruct.tm_mon = thetime.tm_mon;
	argStruct.tm_sec = thetime.tm_sec;
	argStruct.tm_wday = thetime.tm_wday;
	argStruct.tm_yday = thetime.tm_yday;
	argStruct.tm_year = thetime.tm_year;
	
	return ::mktime(&argStruct);
}

hkSystemTime HK_CALL hkTimeDate::time()
{
	return ::time(HK_NULL);
}

hkTimeDate::hkTimeStruct HK_CALL hkTimeDate::gmtime(const hkSystemTime& timer)
{
	tm* timeStruct = ::gmtime(&timer);

	hkTimeStruct returnStruct;

	returnStruct.tm_hour = timeStruct->tm_hour;
	returnStruct.tm_isdst = timeStruct->tm_isdst;
	returnStruct.tm_mday = timeStruct->tm_mday;
	returnStruct.tm_min = timeStruct->tm_min;
	returnStruct.tm_mon = timeStruct->tm_mon;
	returnStruct.tm_sec = timeStruct->tm_sec;
	returnStruct.tm_wday = timeStruct->tm_wday;
	returnStruct.tm_yday = timeStruct->tm_yday;
	returnStruct.tm_year = timeStruct->tm_year;

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
