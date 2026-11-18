/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkcompat/hkCompat.h>
#include <hkbase/class/hkClassMemberAccessor.h>
#include <hkbase/htl/hkPointerMap.h>
#include <hkbase/class/hkClass.h>
#include <hkserialize/version/hkVersionRegistry.h>
#include <hkserialize/version/hkVersionUtil.h>
#include <hkserialize/util/hkBuiltinTypeRegistry.h>


#define REMOVED(TYPE) { 0,0, hkVersionUtil::VERSION_REMOVED, TYPE, HK_NULL }
#define BINARY_IDENTICAL(OLDSIG,NEWSIG,TYPE) { OLDSIG, NEWSIG, hkVersionUtil::VERSION_MANUAL, TYPE, HK_NULL }

static hkVersionUtil::ClassAction UpdateActions[] =
{
	// fixme { 0xbf0e8138, 0x86a24d53, hkVersionUtil::VERSION_VARIANT, "hkPoweredChainData" },

	REMOVED("hkMonitorStreamStringMapStringMap"),
	REMOVED("hkMonitorStreamStringMap"),
	REMOVED("hkMonitorStreamFrameInfo"),

	{ 0x9dd3289c, 0x9dd3289c, hkVersionUtil::VERSION_VARIANT, "hkBoneAttachment", HK_NULL },
	{ 0x50fec527, 0x50fec527, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainer", HK_NULL },
	{ 0x0989ed98, 0x0989ed98, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainerNamedVariant", HK_NULL },
	{ 0x9d8f9b60, 0x9d8f9b60, hkVersionUtil::VERSION_VARIANT, "hkxMaterial", HK_NULL },
	{ 0xe085ba9f, 0xe085ba9f, hkVersionUtil::VERSION_VARIANT, "hkxMaterialTextureStage", HK_NULL },
	{ 0xf55b7691, 0xf55b7691, hkVersionUtil::VERSION_VARIANT, "hkxAttribute", HK_NULL },
	{ 0xb95bf591, 0xb95bf591, hkVersionUtil::VERSION_VARIANT, "hkxAttributeGroup", HK_NULL },
	{ 0x7ac89c0b, 0x7ac89c0b, hkVersionUtil::VERSION_VARIANT, "hkxNode", HK_NULL },

	{ 0xa4dc1e15, 0xfa6d8952, hkVersionUtil::VERSION_COPY, "hkWorldObject", HK_NULL }, // hkMultiThreadLock added as member

	{ 0,0, 0, HK_NULL, HK_NULL }
};

static hkClass*const* hkHavok330b2Classes = const_cast<hkClass*const*>(hkBuiltinTypeRegistry::StaticLinkedClasses);

extern const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_330b1_330b2;
const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_330b1_330b2 =
{
	HK_NULL, // no renames
	UpdateActions,
	hkHavok330b2Classes
};

static hkResult HK_CALL update_Havok330b1_Havok330b2(
	hkArray<hkVariant>& objectsInOut,
	hkObjectUpdateTracker& tracker )
{
	/*static hkBool computedOffsets;
	if( computedOffsets == false )
	{
		hkVersionUtil::recomputeClassMemberOffsets( hkVersionUpdateDescription_330b1_330b2.classes );
		computedOffsets = true;
	}*/
	return hkVersionUtil::updateSingleVersion( objectsInOut, tracker, hkVersionUpdateDescription_330b1_330b2 );
}

extern const hkVersionRegistry::Updater hkVersionUpdater_Havok330b1_Havok330b2;
const hkVersionRegistry::Updater hkVersionUpdater_Havok330b1_Havok330b2 =
{
	"Havok-3.3.0-b1",
	"Havok-3.3.0-b2",
	update_Havok330b1_Havok330b2
};

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
