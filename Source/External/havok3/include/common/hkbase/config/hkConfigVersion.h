/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
// Build configuration settings
// These are compile time settings for the libraries.
// Changing these values with compiled libs will not work,

//
// Build number and version number
// The version number must match the version of the libraries
// but the build number is for informative reasons only as multiple
// builds will work with each other but they must have the same 
// version number, which is of the form XX.YY.ZZ stored as XXYYZZ
// and the build number is the date of release in the form 
// YEAR-MONTH-DAY stored YEARMNDY
//

#ifndef HK_BASE_CONFIGVERSION_H
#define HK_BASE_CONFIGVERSION_H

#define HAVOK_SDK_VERSION_MAJOR 0x3
#define HAVOK_SDK_VERSION_MINOR 0x3
#define HAVOK_SDK_VERSION_POINT 0x0
// release id must be less than 3 chars to fit in binary packfile header.
#define HAVOK_SDK_VERSION_RELEASE "b2"

#define HAVOK_SDK_VERSION_NUMBER ((10000*HAVOK_SDK_VERSION_MAJOR) + (100*HAVOK_SDK_VERSION_MINOR) + (HAVOK_SDK_VERSION_POINT))

#define HAVOK_BUILD_NUMBER 20060114

#endif // HK_BASE_CONFIGVERSION_H

/*
* Havok SDK - client RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
