/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
// Build configuration settings
// These are compile time settings for the libraries.
// Changing these values with compiled libs will not work,
// and will in fact lead to massive breakage.

////////////////////////////////////////////////////////////////////////////
// AUTO-GENERATED - DO NOT EDIT
////////////////////////////////////////////////////////////////////////////

// EQSELECT: HK_CONFIG_MEMORY_STATS, Choose to enable or disable memory statistics;
// This determines whether or not we create static arrays of statistics information
#define HK_CONFIG_MEMORY_STATS_ENABLED 0	// Enabled
#define HK_CONFIG_MEMORY_STATS_DISABLED 1	// Disabled
#define HK_CONFIG_MEMORY_STATS_DEFAULT 2	// Disabled in release

// VALUE:
#define HK_CONFIG_MEMORY_STATS HK_CONFIG_MEMORY_STATS_DEFAULT

// NOTE: stats are turned on and off based on debug (the above code is ignored)
// for better control, remove the code below (basetypes.h must be included before
// this file)

#if HK_CONFIG_MEMORY_STATS == HK_CONFIG_MEMORY_STATS_DEFAULT

#	if  defined(HK_DEBUG)
#	undef HK_CONFIG_MEMORY_STATS
#	define HK_CONFIG_MEMORY_STATS HK_CONFIG_MEMORY_STATS_ENABLED

#	else
#	undef HK_CONFIG_MEMORY_STATS
#	define HK_CONFIG_MEMORY_STATS HK_CONFIG_MEMORY_STATS_DISABLED

#	endif

#endif
/*
* Havok Game Dynamics SDK - client RELEASE, BUILD(#20030622)
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
