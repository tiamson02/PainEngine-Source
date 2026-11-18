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

static void PositionConstraintMotor_330a2_330b1(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	hkClassMemberAccessor newMin(newObj, "minForce");
	hkClassMemberAccessor newMax(newObj, "maxForce");
	hkClassMemberAccessor oldMax(oldObj, "maxForce");

	if (newMin.isOk() && newMax.isOk() && oldMax.isOk())
	{
		newMin.asReal() = - oldMax.asReal();
		newMax.asReal() = + oldMax.asReal();
	}
	else
	{
		HK_ASSERT2(0xad7d77de, false, "member not found");
	}
}

static void VelocityConstraintMotor_330a2_330b1(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	hkVersionUtil::renameMember(oldObj, "maxNegForce", newObj, "minForce");
	hkVersionUtil::renameMember(oldObj, "maxPosForce", newObj, "maxForce");
}

static void SpringDamperConstraintMotor_330a2_330b1(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	hkVersionUtil::renameMember(oldObj, "maxNegForce", newObj, "minForce");
	hkVersionUtil::renameMember(oldObj, "maxPosForce", newObj, "maxForce");
}

#define REMOVED(TYPE) { 0,0, hkVersionUtil::VERSION_REMOVED, TYPE, HK_NULL }
#define BINARY_IDENTICAL(OLDSIG,NEWSIG,TYPE) { OLDSIG, NEWSIG, hkVersionUtil::VERSION_MANUAL, TYPE, HK_NULL }


static hkVersionUtil::ClassAction UpdateActions[] =
{
	{ 0x1c50563b, 0xead954fd, hkVersionUtil::VERSION_COPY, "hkPositionConstraintMotor", PositionConstraintMotor_330a2_330b1 }, // max/min forces moved to base class
	{ 0xa03b1417, 0x94d2e665, hkVersionUtil::VERSION_COPY, "hkVelocityConstraintMotor", VelocityConstraintMotor_330a2_330b1 }, // member added, defaults to false + max/min forces moved to base class
	{ 0x48377d86, 0xb29a4f46, hkVersionUtil::VERSION_COPY, "hkSpringDamperConstraintMotor", SpringDamperConstraintMotor_330a2_330b1 }, // max/min forces moved to base class

	{0xbfc428d3, 0xbf0e8138, hkVersionUtil::VERSION_COPY, "hkPoweredChainData", HK_NULL }, // Array<ConstraintInfo> changed
	{0x173a57ec, 0xf88aee25, hkVersionUtil::VERSION_COPY, "hkPoweredChainDataConstraintInfo", HK_NULL }, // removed m_cfm*, moved m_bTc

	REMOVED("hkConstraintChainDriverDataTarget"),
	REMOVED("hkConstraintChainDriverData"),
	REMOVED("hkTriPatchTriangle"),

	BINARY_IDENTICAL(0xda8c7d7d, 0x82ef3c01, "hkConstraintMotor"), // enum added
	BINARY_IDENTICAL(0xb368f9bd, 0xde4be9fc, "hkConstraintData"), // enum type_chain_driver removed
	
	{ 0x9dd3289c, 0x9dd3289c, hkVersionUtil::VERSION_VARIANT, "hkBoneAttachment", HK_NULL },
	{ 0x50fec527, 0x50fec527, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainer", HK_NULL },
	{ 0x0989ed98, 0x0989ed98, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainerNamedVariant", HK_NULL },
	{ 0x9d8f9b60, 0x9d8f9b60, hkVersionUtil::VERSION_VARIANT, "hkxMaterial", HK_NULL },
	{ 0xe085ba9f, 0xe085ba9f, hkVersionUtil::VERSION_VARIANT, "hkxMaterialTextureStage", HK_NULL },
	{ 0xf55b7691, 0xf55b7691, hkVersionUtil::VERSION_VARIANT, "hkxAttribute", HK_NULL },
	{ 0xb95bf591, 0xb95bf591, hkVersionUtil::VERSION_VARIANT, "hkxAttributeGroup", HK_NULL },
	{ 0x7ac89c0b, 0x7ac89c0b, hkVersionUtil::VERSION_VARIANT, "hkxNode", HK_NULL },

	BINARY_IDENTICAL(0xb330fa01, 0xff8ce40d, "hkPackfileHeader"), // contentsClass -> contentsClassName

	{ 0,0, 0, HK_NULL, HK_NULL }
};

extern hkClass* const hkHavok330b1Classes[];

extern const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_330a2_330b1;
const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_330a2_330b1 =
{
	HK_NULL, // no renames
	UpdateActions,
	hkHavok330b1Classes
};

static hkResult HK_CALL update_Havok330a2_Havok330b1(
	hkArray<hkVariant>& objectsInOut,
	hkObjectUpdateTracker& tracker )
{
	static hkBool computedOffsets;
	if( computedOffsets == false )
	{
		hkVersionUtil::recomputeClassMemberOffsets( hkVersionUpdateDescription_330a2_330b1.classes );
		computedOffsets = true;
	}
	return hkVersionUtil::updateSingleVersion( objectsInOut, tracker, hkVersionUpdateDescription_330a2_330b1 );
}

extern const hkVersionRegistry::Updater hkVersionUpdater_Havok330a2_Havok330b1;
const hkVersionRegistry::Updater hkVersionUpdater_Havok330a2_Havok330b1 =
{
	"Havok-3.3.0-a2",
	"Havok-3.3.0-b1",
	update_Havok330a2_Havok330b1
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
