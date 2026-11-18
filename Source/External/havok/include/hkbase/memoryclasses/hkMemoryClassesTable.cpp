/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/memoryclasses/hkMemoryClassesTable.h>
#include <hkbase/config/hkConfigMemoryStats.h>

#if HK_CONFIG_MEMORY_STATS == HK_CONFIG_MEMORY_STATS_ENABLED

//only if stats are on
#	define HK_MEMORY_CLASS_DEFINITION_START hkMemoryClassInfo hkMemoryClassesTable[] = {
#	define HK_MEMORY_CLASS_DEFINITION_END hkMemoryClassInfo(HK_NULL,0x0)};
#	define HK_MEMORY_CLASS(A,B) hkMemoryClassInfo("HK_MEMORY_CLASS_" #A, B),

#	include <hkbase/memoryclasses/hkMemoryClasses.h>

#	undef HK_MEMORY_CLASS_DEFINITION_START
#	undef HK_MEMORY_CLASS_DEFINITION_END
#	undef HK_MEMORY_CLASS

#endif //HK_CONFIG_MEMORY_STATS == HK_CONFIG_MEMORY_STATS_ENABLED
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
