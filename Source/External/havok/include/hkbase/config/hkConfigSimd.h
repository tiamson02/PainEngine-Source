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

// EQSELECT: HK_CONFIG_SIMD, Choose to enable or disable simd instructions:
// This determines whether or not we use simd instructions.
#define HK_CONFIG_SIMD_PLATFORM_DEFAULT 0  // Platform/Compiler Default
#define HK_CONFIG_SIMD_ENABLED 1 // Enabled
#define HK_CONFIG_SIMD_DISABLED 2 // Disabled

// VALUE:
#define HK_CONFIG_SIMD HK_CONFIG_SIMD_PLATFORM_DEFAULT

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_PLATFORM_DEFAULT
#	if !defined(HKBASE_HKBASETYPES_H)
#		error hkbase/hkBase.h needs to be included before this file
#	endif
#	undef HK_CONFIG_SIMD
#	if defined(HK_PLATFORM_XBOX)
#		define HK_CONFIG_SIMD HK_CONFIG_SIMD_ENABLED
#	elif defined(HK_ARCH_PS2)
#		define HK_CONFIG_SIMD HK_CONFIG_SIMD_ENABLED
#	else
#		define HK_CONFIG_SIMD HK_CONFIG_SIMD_DISABLED
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
