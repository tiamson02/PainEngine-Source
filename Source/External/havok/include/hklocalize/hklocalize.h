/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_LOCALIZE_LOCALIZE_H
#define HK_LOCALIZE_LOCALIZE_H

#include <hkbase/types/hkBaseTypes.h>

#define HKLANGUAGE_EN 0
#define HKLANGUAGE_ES 1
#define HKLANGUAGE_JP 2
#define HKLANGUAGE_FR 3
#define HKLANGUAGE_IT 4
#define HKLANGUAGE_DE 5

#define HKLANGUAGE HKLANGUAGE_EN

#if HKLANGUAGE == HKLANGUAGE_EN

// In the case of english, we use an inline function
inline const char* LOC (const char* string_in, const char* notes = HK_NULL, bool differentiate=false) 
{
	return string_in;
}

#else

const char* LOC (const char* string_in, const char* notes = HK_NULL, bool differentiate=false);

#endif

#endif //HK_LOCALIZE_LOCALIZE_H


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
