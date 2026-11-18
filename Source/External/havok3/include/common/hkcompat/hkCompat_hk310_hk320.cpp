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

static void Noop( hkVariant&, hkVariant&, hkObjectUpdateTracker& )
{
}

static void ZeroPointer( hkVariant& newObj, const char* memName )
{
	hkClassMemberAccessor userData(newObj, memName);
	if( userData.isOk() )
	{
		userData.asPointer() = HK_NULL;
	}
}
static void ZeroUserDataPointer( hkVariant& newObj )
{
	ZeroPointer(newObj, "userData");
}

static void ZeroUserDataPointer(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	ZeroUserDataPointer(newObj);
}

static void VehicleInstanceWheelInfo_310_320(
	hkVariant& oldObj,
	hkVariant& newObj,
	hkObjectUpdateTracker& )
{
	ZeroPointer( newObj, "contactBody" );
}

#define REMOVED(TYPE) { 0,0, hkVersionUtil::VERSION_REMOVED, TYPE, HK_NULL }
#define BINARY_IDENTICAL(OLDSIG,NEWSIG,TYPE) { OLDSIG, NEWSIG, hkVersionUtil::VERSION_MANUAL, TYPE, HK_NULL }

static hkVersionUtil::ClassAction UpdateActions[] =
{
	// World object
	{ 0x91149181, 0x4b9d0f79, hkVersionUtil::VERSION_INPLACE, "hkAabbPhantom", ZeroUserDataPointer }, // overlapping collidables zeroed
	{ 0x93184fe5, 0x35b30cc5, hkVersionUtil::VERSION_COPY, "hkEntity", ZeroUserDataPointer },	// new uid member added, userdata->zero
	{ 0xef940b7c, 0x4bcc8a37, hkVersionUtil::VERSION_COPY, "hkWorldCinfo", HK_NULL }, // New members + defaults
	{ 0x6b9c4f42, 0xa4dc1e15, hkVersionUtil::VERSION_INPLACE, "hkWorldObject", ZeroUserDataPointer },  // broadphase border enum added, userdata->zero

	// Constraints
	{ 0xb1c5a2d7, 0xda7599af, hkVersionUtil::VERSION_COPY, "hkBreakableConstraintData", HK_NULL }, // many additions
	{ 0xa487a781, 0x6553492d, hkVersionUtil::VERSION_COPY, "hkLimitedHingeConstraintData", HK_NULL }, // angularLimitsTauFactor added
	{ 0xc1f5d50d, 0x22baaa3f, hkVersionUtil::VERSION_COPY, "hkPoweredRagdollConstraintData", HK_NULL }, // copy as it actually got smaller and members moved (bool after matrix now instead of before)
	{ 0x09905d3a, 0x7ff383a1, hkVersionUtil::VERSION_COPY, "hkRagdollConstraintData", HK_NULL }, // angular tau added
	{ 0xb1c5a2d7, 0xda7599af, hkVersionUtil::VERSION_COPY, "hkBreakableConstraintData", HK_NULL }, // many additions

	// Misc
	{ 0x73004dbb, 0xed923a6e, hkVersionUtil::VERSION_MANUAL, "hkContactPointMaterial", ZeroUserDataPointer }, // userdata -> zero
	{ 0x0970366c, 0x4361c3e7, hkVersionUtil::VERSION_MANUAL, "hkPhysicsSystem", ZeroUserDataPointer },
	{ 0x51a73ef8, 0x1cd2a3e1, hkVersionUtil::VERSION_MANUAL, "hkMeshShapeSubpart", Noop }, // type of subpart members changed
		
	// Vehicle
	{ 0x0428f554, 0xefc66f9b, hkVersionUtil::VERSION_COPY, "hkVehicleInstance", HK_NULL }, // Changed friction Status
	{ 0xd8b13b98, 0x1c076a84, hkVersionUtil::VERSION_COPY, "hkVehicleFrictionStatus", HK_NULL }, // Changed friction Status
	{ 0x5854d53d, 0xe70e2bb4, hkVersionUtil::VERSION_COPY, "hkVehicleFrictionStatusAxisStatus", HK_NULL }, // Changed friction Status
	{ 0x5bf274c8, 0x8d5654ee, hkVersionUtil::VERSION_MANUAL, "hkVehicleInstanceWheelInfo", VehicleInstanceWheelInfo_310_320 }, // contactBody -> zero

	BINARY_IDENTICAL(0x20477afd, 0xa1fdd897, "hkClass"), // signature flags enum added
	BINARY_IDENTICAL(0x51a73ef8, 0x1cd2a3e1, "hkMeshShapeSubpart"), // vertices pointer void* -> float*
	BINARY_IDENTICAL(0x16df02fe, 0x8c11d1f2, "hkPackfileHeader"), // name added (size unchanged")
	BINARY_IDENTICAL(0xb33582ad, 0x953b00d8, "hkMotion"), // enum THIN_MOTION added
	BINARY_IDENTICAL(0x9eeaa6f5, 0xf5aa2dc6, "hkMeshShape"), // type of subpart members changed

	REMOVED("hkBallAndSocketConstraintDataRuntime"),
	REMOVED("hkCachingShapePhantomhkCollisionDetail"),
	REMOVED("hkConvexPieceMeshShapeStats"),
	REMOVED("hkConstraintDataRuntimeInfo"),
	REMOVED("hkDeltaCompressedSkeletalAnimationCompressionParams"),
	REMOVED("hkHeightFieldShapehkSphereCastInput"),
	REMOVED("hkHingeConstraintDataRuntime"),
	REMOVED("hkLimitedHingeConstraintDataRuntime"),
	REMOVED("hkLinkedCollidableCollisionEntry"),
	REMOVED("hkPointToPathConstraintDataRuntime"),
	REMOVED("hkPointToPlaneConstraintDataRuntime"),
	REMOVED("hkPoweredHingeConstraintDataRuntime"),
	REMOVED("hkPoweredRagdollConstraintDataRuntime"),
	REMOVED("hkPrismaticConstraintDataRuntime"),
	REMOVED("hkRagdollConstraintDataRuntime"),
	REMOVED("hkSimpleShapePhantomhkCollisionDetail"),
	REMOVED("hkSphereRepShapehkCollisionSpheresInfo"),
	REMOVED("hkStiffSpringConstraintDataRuntime"),
	REMOVED("hkTriPatchTriangle"),
	REMOVED("hkVehicleAerodynamicsAerodynamicsDragOutput"),
	REMOVED("hkVehicleBrakeWheelBreakingOutput"),
	REMOVED("hkVehicleDriverInputFilteredDriverInputOutput"),
	REMOVED("hkVehicleEngineEngineOutput"),
	REMOVED("hkVehicleSteeringSteeringAnglesOutput"),
	REMOVED("hkVehicleTransmissionTransmissionOutput"),
	REMOVED("hkVehicleWheelCollideCollisionDetectionWheelOutput"),
	REMOVED("hkVehicleFrictionDescriptionCinfo"),
	REMOVED("hkVehicleFrictionSolverAxleParams"),
	REMOVED("hkVehicleFrictionSolverParams"),
	REMOVED("hkHeightFieldShapeCollideSpheresInput"),
	REMOVED("hkPhysicsDataSplitPhysicsSystemsOutput"),
	REMOVED("hkVehicleStepInfo"),
	REMOVED("hkVelocityAccumulator"),
	REMOVED("hkRigidAccumulator"),
	REMOVED("hkSolverResults"),
	
	{ 0x4af7c559, 0x459311f7, hkVersionUtil::VERSION_COPY, "hkSkeleton", HK_NULL }, // added name
	{ 0xef964a74, 0xe39df839, hkVersionUtil::VERSION_COPY, "hkAnimationBinding", HK_NULL }, // added blendHint
	REMOVED("hkDefaultAnimatedReferenceFrameMotionExtractionOptions"),
	REMOVED("hkWaveletSkeletalAnimationCompressionParams"),
	REMOVED("hkWheelConstraintDataRuntime"),

	// Scenedata
	{ 0x50fec527, 0x50fec527, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainer", HK_NULL }, // variant
	{ 0x0989ed98, 0x0989ed98, hkVersionUtil::VERSION_VARIANT, "hkRootLevelContainerNamedVariant", HK_NULL }, // variant
	{ 0x9dd3289c, 0x9dd3289c, hkVersionUtil::VERSION_VARIANT, "hkBoneAttachment", HK_NULL }, // variant
	{ 0xe085ba9f, 0xe085ba9f, hkVersionUtil::VERSION_VARIANT, "hkxMaterialTextureStage", HK_NULL }, // variant
	{ 0x8b106e42, 0xd5c65fae, hkVersionUtil::VERSION_COPY, "hkxCamera", HK_NULL }, // lefthanded added
	{ 0x5ea0b80b, 0xbcaf7ac8, hkVersionUtil::VERSION_COPY | hkVersionUtil::VERSION_VARIANT, "hkxNode", HK_NULL }, // attribute groups added, variant
	{ 0x4a320b11, 0x70b51f14, hkVersionUtil::VERSION_COPY, "hkxScene", HK_NULL }, // applied transform
	{ 0x73b9eedd, 0x9d8f9b60, hkVersionUtil::VERSION_COPY | hkVersionUtil::VERSION_VARIANT, "hkxMaterial", HK_NULL }, // added extra data variant (can store effect shader ptr etc)
	BINARY_IDENTICAL(0x9c5ac7f1, 0x57061454, "hkxVertexBuffer"), // flags of format changed

	{ 0,0, 0, HK_NULL, HK_NULL }
};

extern hkClass* const hkHavok320Classes[];

extern const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_310_320;
const hkVersionUtil::UpdateDescription hkVersionUpdateDescription_310_320 =
{
	HK_NULL, // no renames
	UpdateActions,
	hkHavok320Classes
};

static hkResult HK_CALL update_Havok310_Havok320(
	hkArray<hkVariant>& objectsInOut,
	hkObjectUpdateTracker& tracker )
{
	static hkBool computedOffsets320;
	if( computedOffsets320 == false )
	{
		hkVersionUtil::recomputeClassMemberOffsets( hkVersionUpdateDescription_310_320.classes );
		computedOffsets320 = true;
	}
	return hkVersionUtil::updateSingleVersion( objectsInOut, tracker, hkVersionUpdateDescription_310_320 );
}

extern const hkVersionRegistry::Updater hkVersionUpdater_Havok310_Havok320;
const hkVersionRegistry::Updater hkVersionUpdater_Havok310_Havok320 =
{
	"Havok-3.1.0",
	"Havok-3.2.0",
	update_Havok310_Havok320
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
