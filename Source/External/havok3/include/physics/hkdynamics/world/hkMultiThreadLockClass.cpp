/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/world/hkMultiThreadLock.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/world/hkMultiThreadLock.h>


//
// Class hkMultiThreadLock
//

//
// Enum hkMultiThreadLock::AccessType
//
static const hkInternalClassEnumItem hkMultiThreadLockAccessTypeEnumItems[] =
{
	{0, "HK_ACCESS_IGNORE"},
	{1, "HK_ACCESS_RO"},
	{2, "HK_ACCESS_RW"},
};
static const hkInternalClassEnum hkMultiThreadLockEnums[] = {
	{"AccessType", hkMultiThreadLockAccessTypeEnumItems, 3 }
};
extern const hkClassEnum* hkMultiThreadLockAccessTypeEnum = reinterpret_cast<const hkClassEnum*>(&hkMultiThreadLockEnums[0]);
static const hkInternalClassMember hkMultiThreadLockClass_Members[] =
{
	{ "threadId", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkMultiThreadLock,m_threadId) },
	{ "lockCount", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkMultiThreadLock,m_lockCount) }
};
extern const hkClass hkMultiThreadLockClass;
const hkClass hkMultiThreadLockClass(
	"hkMultiThreadLock",
	HK_NULL, // parent
	sizeof(hkMultiThreadLock),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMultiThreadLockEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkMultiThreadLockClass_Members),
	int(sizeof(hkMultiThreadLockClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
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
