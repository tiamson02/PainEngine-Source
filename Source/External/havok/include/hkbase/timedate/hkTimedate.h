/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKTIMEDATE_H
#define HKBASE_HKTIMEDATE_H


/// Provides date and time functions ususally found in libc
class hkTimeDate
{
	public:

		/// A struct to mirror libc's 'struct tm'
		struct hkTimeStruct
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkTimeStruct)
			
			int tm_sec;     /* seconds after the minute - [0,59] */
			int tm_min;     /* minutes after the hour - [0,59] */
			int tm_hour;    /* hours since midnight - [0,23] */
			int tm_mday;    /* day of the month - [1,31] */
			int tm_mon;     /* months since January - [0,11] */
			int tm_year;    /* years since 1900 */
			int tm_wday;    /* days since Sunday - [0,6] */
			int tm_yday;    /* days since January 1 - [0,365] */
			int tm_isdst;   /* daylight savings time flag */
		};


			/// Returns the number of seconds since 1970 for the date in 'thetime'.
		static hkSystemTime HK_CALL mktime(const hkTimeStruct& thetime);
			/// Returns the number of seconds since 1970.
		static hkSystemTime HK_CALL time();
			/// Converts the number of seconds since 1970 in 'thetime' to an equivalent hkTimeStruct.
		static hkTimeStruct HK_CALL gmtime(const hkSystemTime& thetime);
 
	private:

			/// No implementation
		hkTimeDate();
};

#endif // HKBASE_HKTIMEDATE_H

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
