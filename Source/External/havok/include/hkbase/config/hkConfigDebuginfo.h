/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
// AdvancedBuild configuration settings
// These are compile time settings for the libraries.
// Changing these values with compiled libs will not work,
// and will in fact lead to massive breakage.


////////////////////////////////////////////////////////////////////////////
// AUTO-GENERATED - DO NOT EDIT
////////////////////////////////////////////////////////////////////////////

// EQSELECT: HK_CONFIG_DEBUGINFO, Choose debug output option:
// Debug points and lines, console streaming and timerbanks can be globally disabled
#define HK_CONFIG_DEBUGINFO_DEFAULT 1 // Default
#define HK_CONFIG_DEBUGINFO_DISABLED 2 // Disabled

// VALUE:
#define HK_CONFIG_DEBUGINFO HK_CONFIG_DEBUGINFO_DISABLED

#if (HK_CONFIG_DEBUGINFO == HK_CONFIG_DEBUGINFO_DISABLED)
#	undef HK_USE_DEBUGDISPLAY
#	undef HK_USE_DEBUGCONSOLE
#	undef HK_USE_DEBUGTIMERS
#elif (HK_CONFIG_DEBUGINFO != HK_CONFIG_DEBUGINFO_DEFAULT)
#	error "SERIOUS ERROR - NO DEBUGINFO SETTING SPECIFIED"
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
