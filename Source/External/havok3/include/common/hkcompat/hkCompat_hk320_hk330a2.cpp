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

static void MalleableConstraintData_320_330(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	hkClassMemberAccessor newStrength(newObj, "strength");
	hkClassMemberAccessor oldTau(oldObj, "tau");
	if( newStrength.isOk() && oldTau.isOk() )
	{
		newStrength.asReal() = oldTau.asReal();
	}
	else
	{
		HK_ASSERT2(0xad7d77dd, false, "member not found");
	}
}

#define REMOVED(TYPE) { 0,0, hkVersionUtil::VERSION_REMOVED, TYPE, HK_NULL }
#define BINARY_IDENTICAL(OLDSIG,NEWSIG,TYPE) { OLDSIG, NEWSIG, hkVersionUtil::VERSION_MANUAL, TYPE, HK_NULL }

static hkVersionUtil::ClassAction UpdateActions[] =
{
	{ 0x7ff383a1, 0x7c518cec, hkVersionUtil::VERSION_COPY, "hkRagdollConstraintData", HK_NULL }, // removed m_anglesHavok30
	{ 0x47194132, 0xdd8f8ace, hkVersionUtil::VERSION_COPY, "hkMalleableConstraintData", MalleableConstraintData_320_330 }, // members removed and added: tau+dumping replaced by strength
	{ 0xda7599af, 0x6a0a45f1, hkVersionUtil::VERSION_COPY, "hkBreakableConstraintData", HK_NULL }, // members removed and added. revertBackVelocityOnBreak defaults to zero. childRuntimeSize and childNumSolverResults initialized in the finish-up construtctor.

	{ 0x448142a7, 0xb368f9bd, hkVersionUtil::VERSION_MANUAL, "hkConstraintData", HK_NULL }, // enum scope
	{ 0x28982f20, 0xa2e0c7c2, hkVersionUtil::VERSION_MANUAL, "hkConstraintInstance", HK_NULL }, // enumerations added, virtual function added, members changed from protected to public
	{ 0x19e37679, 0xa03b1417, hkVersionUtil::VERSION_COPY, "hkVelocityConstraintMotor", HK_NULL }, // member added, defaults to false
	
	BINARY_IDENTICAL(0x7e6045e5, 0x472ddf28, "hkMultiSphereShape"), // enum MAX_SPHERES made anonymous
	BINARY_IDENTICAL(0xc70eb5bb, 0x7d7692dc, "hkMultiThreadLock"), // AccessType global->scoped
	REMOVED("hkTriPatchTriangle"),

	{ 0x9dd3289c, 0x9dd3289c, hkVersionUtil::VERSION_VARIANT, "hkBoneAttachment", HK_NULL },
	{ 0x50fec527, 0x50fec527, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainer", HK_NULL },
	{ 0x0989ed98, 0x0989ed98, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainerNamedVariant", HK_NULL },
	{ 0x9d8f9b60, 0x9d8f9b60, hkVersionUtil::VERSION_VARIANT, "hkxMaterial", HK_NULL },
	{ 0xe085ba9f, 0xe085ba9f, hkVersionUtil::VERSION_VARIANT, "hkxMaterialTextureStage", HK_NULL },
	// enum global->local in hkxAttribute
	{ 0x779e0651, 0xf55b7691, hkVersionUtil::VERSION_MANUAL | hkVersionUtil::VERSION_VARIANT, "hkxAttribute", HK_NULL },
	{ 0xdae88b4d, 0xb95bf591, hkVersionUtil::VERSION_MANUAL | hkVersionUtil::VERSION_VARIANT, "hkxAttributeGroup", HK_NULL },
	{ 0xbcaf7ac8, 0x7ac89c0b, hkVersionUtil::VERSION_MANUAL | hkVersionUtil::VERSION_VARIANT, "hkxNode", HK_NULL },
	BINARY_IDENTICAL(0x8c11d1f2, 0xb330fa01, "hkPackfileHeader"), // version string 12 -> 16 chars

	{ 0xe6bd02ee, 0x9bb15af4, hkVersionUtil::VERSION_MANUAL, "hkxAnimatedFloat", HK_NULL }, // enum scope
	{ 0x1eba1f03, 0x95bd90ad, hkVersionUtil::VERSION_MANUAL, "hkxAnimatedMatrix", HK_NULL }, // enum scope
	{ 0xa9adb3a6, 0xfe98cabd, hkVersionUtil::VERSION_MANUAL, "hkxAnimatedVector", HK_NULL }, // enum scope

	REMOVED("hkMonitorStreamFrameInfo"),
	REMOVED("hkMonitorStreamStringMap"),
	REMOVED("hkMonitorStreamStringMapStringMap"),

	{ 0,0, 0, HK_NULL, HK_NULL }
};

extern hkClass* const hkHavok330a2Classes[];

extern const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_320_330a2;
const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_320_330a2 =
{
	HK_NULL, // no renames
	UpdateActions,
	hkHavok330a2Classes
};

static hkResult HK_CALL update_Havok320_Havok330a2(
	hkArray<hkVariant>& objectsInOut,
	hkObjectUpdateTracker& tracker )
{
	static hkBool computedOffsets;
	if( computedOffsets == false )
	{
		hkVersionUtil::recomputeClassMemberOffsets( hkVersionUpdateDescription_320_330a2.classes );
		computedOffsets = true;
	}
	return hkVersionUtil::updateSingleVersion( objectsInOut, tracker, hkVersionUpdateDescription_320_330a2 );
}

extern const hkVersionRegistry::Updater hkVersionUpdater_Havok320_Havok330a2;
const hkVersionRegistry::Updater hkVersionUpdater_Havok320_Havok330a2 =
{
	"Havok-3.2.0",
	"Havok-3.3.0-a2",
	update_Havok320_Havok330a2
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
