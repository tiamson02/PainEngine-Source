/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkcompat/hkCompat.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>

extern hkClass hkAabbClass_320;
extern hkClass hkAabbPhantomClass_320;
extern hkClass hkActionClass_320;
extern hkClass hkAngularDashpotActionClass_320;
extern hkClass hkAnimatedReferenceFrameClass_320;
extern hkClass hkAnimationBindingClass_320;
extern hkClass hkAnimationContainerClass_320;
extern hkClass hkAnnotationTrackAnnotationClass_320;
extern hkClass hkAnnotationTrackClass_320;
extern hkClass hkArrayActionClass_320;
extern hkClass hkBallAndSocketConstraintDataClass_320;
extern hkClass hkBaseObjectClass_320;
extern hkClass hkBinaryActionClass_320;
extern hkClass hkBoneAttachmentClass_320;
extern hkClass hkBoneClass_320;
extern hkClass hkBoolClass_320;
extern hkClass hkBoxMotionClass_320;
extern hkClass hkBoxShapeClass_320;
extern hkClass hkBreakableConstraintDataClass_320;
extern hkClass hkBreakableListenerClass_320;
extern hkClass hkBroadPhaseHandleClass_320;
extern hkClass hkBvShapeClass_320;
extern hkClass hkBvTreeShapeClass_320;
extern hkClass hkCachingShapePhantomClass_320;
extern hkClass hkCapsuleShapeClass_320;
extern hkClass hkCdBodyClass_320;
extern hkClass hkCharacterProxyCinfoClass_320;
extern hkClass hkClassClass_320;
extern hkClass hkClassEnumClass_320;
extern hkClass hkClassEnumItemClass_320;
extern hkClass hkClassMemberClass_320;
extern hkClass hkCollidableClass_320;
extern hkClass hkCollidableCollidableFilterClass_320;
extern hkClass hkCollisionDetailClass_320;
extern hkClass hkCollisionFilterClass_320;
extern hkClass hkCollisionFilterListClass_320;
extern hkClass hkCollisionListenerClass_320;
extern hkClass hkConstrainedSystemFilterClass_320;
extern hkClass hkConstraintDataClass_320;
extern hkClass hkConstraintInfoClass_320;
extern hkClass hkConstraintInstanceClass_320;
extern hkClass hkConstraintInternalClass_320;
extern hkClass hkConstraintModifierClass_320;
extern hkClass hkConstraintMotorClass_320;
extern hkClass hkConstraintOwnerClass_320;
extern hkClass hkContactPointClass_320;
extern hkClass hkContactPointMaterialClass_320;
extern hkClass hkConvexListShapeClass_320;
extern hkClass hkConvexPieceMeshShapeClass_320;
extern hkClass hkConvexPieceStreamDataClass_320;
extern hkClass hkConvexShapeClass_320;
extern hkClass hkConvexTransformShapeClass_320;
extern hkClass hkConvexTranslateShapeClass_320;
extern hkClass hkConvexVerticesShapeClass_320;
extern hkClass hkConvexVerticesShapeFourVectorsClass_320;
extern hkClass hkCylinderShapeClass_320;
extern hkClass hkDashpotActionClass_320;
extern hkClass hkDefaultAnimatedReferenceFrameClass_320;
extern hkClass hkDeltaCompressedSkeletalAnimationClass_320;
extern hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_320;
extern hkClass hkDisableEntityCollisionFilterClass_320;
extern hkClass hkDisplayBindingDataClass_320;
extern hkClass hkEntityActivationListenerClass_320;
extern hkClass hkEntityClass_320;
extern hkClass hkEntityDeactivatorClass_320;
extern hkClass hkEntityListenerClass_320;
extern hkClass hkFakeRigidBodyDeactivatorClass_320;
extern hkClass hkFastMeshShapeClass_320;
extern hkClass hkFixedRigidMotionClass_320;
extern hkClass hkGenericConstraintDataClass_320;
extern hkClass hkGenericConstraintDataSchemeClass_320;
extern hkClass hkGroupCollisionFilterClass_320;
extern hkClass hkGroupFilterClass_320;
extern hkClass hkHeightFieldShapeClass_320;
extern hkClass hkHingeConstraintDataClass_320;
extern hkClass hkHingeConstraintDataConstraintBasisAClass_320;
extern hkClass hkHingeConstraintDataConstraintBasisBClass_320;
extern hkClass hkInt16Class_320;
extern hkClass hkInt8Class_320;
extern hkClass hkInterleavedSkeletalAnimationClass_320;
extern hkClass hkKeyframedRigidMotionClass_320;
extern hkClass hkLimitedHingeConstraintDataClass_320;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_320;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_320;
extern hkClass hkLinearParametricCurveClass_320;
extern hkClass hkLinkedCollidableClass_320;
extern hkClass hkListShapeChildInfoClass_320;
extern hkClass hkListShapeClass_320;
extern hkClass hkMalleableConstraintDataClass_320;
extern hkClass hkMaterialClass_320;
extern hkClass hkMatrix4Class_320;
extern hkClass hkMeshBindingClass_320;
extern hkClass hkMeshBindingMappingClass_320;
extern hkClass hkMeshMaterialClass_320;
extern hkClass hkMeshShapeClass_320;
extern hkClass hkMeshShapeSubpartClass_320;
extern hkClass hkMonitorStreamFrameInfoClass_320;
extern hkClass hkMonitorStreamStringMapClass_320;
extern hkClass hkMonitorStreamStringMapStringMapClass_320;
extern hkClass hkMoppBvTreeShapeClass_320;
extern hkClass hkMoppCodeClass_320;
extern hkClass hkMoppCodeCodeInfoClass_320;
extern hkClass hkMotionClass_320;
extern hkClass hkMotionStateClass_320;
extern hkClass hkMotorActionClass_320;
extern hkClass hkMouseSpringActionClass_320;
extern hkClass hkMultiRayShapeClass_320;
extern hkClass hkMultiRayShapeRayClass_320;
extern hkClass hkMultiSphereShapeClass_320;
extern hkClass hkMultiThreadLockClass_320;
extern hkClass hkNullCollisionFilterClass_320;
extern hkClass hkPackfileHeaderClass_320;
extern hkClass hkPackfileSectionHeaderClass_320;
extern hkClass hkPairwiseCollisionFilterClass_320;
extern hkClass hkPairwiseCollisionFilterCollisionPairClass_320;
extern hkClass hkParametricCurveClass_320;
extern hkClass hkPhantomCallbackShapeClass_320;
extern hkClass hkPhantomClass_320;
extern hkClass hkPhantomListenerClass_320;
extern hkClass hkPhantomOverlapListenerClass_320;
extern hkClass hkPhysicsDataClass_320;
extern hkClass hkPhysicsSystemClass_320;
extern hkClass hkPhysicsSystemDisplayBindingClass_320;
extern hkClass hkPlaneShapeClass_320;
extern hkClass hkPointToPathConstraintDataClass_320;
extern hkClass hkPointToPlaneConstraintDataClass_320;
extern hkClass hkPositionConstraintMotorClass_320;
extern hkClass hkPoweredHingeConstraintDataClass_320;
extern hkClass hkPoweredRagdollConstraintDataClass_320;
extern hkClass hkPrismaticConstraintDataClass_320;
extern hkClass hkPrismaticConstraintDataConstraintBasisAClass_320;
extern hkClass hkPrismaticConstraintDataConstraintBasisBClass_320;
extern hkClass hkPropertyClass_320;
extern hkClass hkPropertyValueClass_320;
extern hkClass hkQsTransformClass_320;
extern hkClass hkQuaternionClass_320;
extern hkClass hkRagdollConstraintDataClass_320;
extern hkClass hkRagdollConstraintDataConstraintBasisAClass_320;
extern hkClass hkRagdollConstraintDataConstraintBasisBClass_320;
extern hkClass hkRagdollInstanceClass_320;
extern hkClass hkRayCollidableFilterClass_320;
extern hkClass hkRayShapeCollectionFilterClass_320;
extern hkClass hkRealClass_320;
extern hkClass hkReferencedObjectClass_320;
extern hkClass hkRejectRayChassisListenerClass_320;
extern hkClass hkReorientActionClass_320;
extern hkClass hkRigidBodyClass_320;
extern hkClass hkRigidBodyDeactivatorClass_320;
extern hkClass hkRigidBodyDisplayBindingClass_320;
extern hkClass hkRigidMotionClass_320;
extern hkClass hkRootLevelContainerClass_320;
extern hkClass hkRootLevelContainerNamedVariantClass_320;
extern hkClass hkSampledHeightFieldShapeClass_320;
extern hkClass hkSerializedDisplayMarkerClass_320;
extern hkClass hkSerializedDisplayMarkerListClass_320;
extern hkClass hkSerializedDisplayRbTransformsClass_320;
extern hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_320;
extern hkClass hkShapeClass_320;
extern hkClass hkShapeCollectionClass_320;
extern hkClass hkShapeCollectionFilterClass_320;
extern hkClass hkShapeKeyClass_320;
extern hkClass hkShapePhantomClass_320;
extern hkClass hkShapeRayCastInputClass_320;
extern hkClass hkSimpleMeshShapeClass_320;
extern hkClass hkSimpleMeshShapeTriangleClass_320;
extern hkClass hkSimpleShapePhantomClass_320;
extern hkClass hkSimulationIslandClass_320;
extern hkClass hkSkeletalAnimationClass_320;
extern hkClass hkSkeletonClass_320;
extern hkClass hkSkeletonMapperClass_320;
extern hkClass hkSkeletonMapperDataChainMappingClass_320;
extern hkClass hkSkeletonMapperDataClass_320;
extern hkClass hkSkeletonMapperDataSimpleMappingClass_320;
extern hkClass hkSpatialRigidBodyDeactivatorClass_320;
extern hkClass hkSpatialRigidBodyDeactivatorSampleClass_320;
extern hkClass hkSphereClass_320;
extern hkClass hkSphereMotionClass_320;
extern hkClass hkSphereRepShapeClass_320;
extern hkClass hkSphereShapeClass_320;
extern hkClass hkSpringActionClass_320;
extern hkClass hkSpringDamperConstraintMotorClass_320;
extern hkClass hkStabilizedBoxMotionClass_320;
extern hkClass hkStabilizedSphereMotionClass_320;
extern hkClass hkStiffSpringConstraintDataClass_320;
extern hkClass hkStorageMeshShapeClass_320;
extern hkClass hkStorageMeshShapeSubpartStorageClass_320;
extern hkClass hkStorageSampledHeightFieldShapeClass_320;
extern hkClass hkSweptTransformClass_320;
extern hkClass hkThinBoxMotionClass_320;
extern hkClass hkTransformClass_320;
extern hkClass hkTransformShapeClass_320;
extern hkClass hkTriPatchTriangleClass_320;
extern hkClass hkTriSampledHeightFieldBvTreeShapeClass_320;
extern hkClass hkTriSampledHeightFieldCollectionClass_320;
extern hkClass hkTriangleShapeClass_320;
extern hkClass hkTypedBroadPhaseHandleClass_320;
extern hkClass hkTyremarkPointClass_320;
extern hkClass hkTyremarksInfoClass_320;
extern hkClass hkTyremarksWheelClass_320;
extern hkClass hkUnaryActionClass_320;
extern hkClass hkVehicleAerodynamicsClass_320;
extern hkClass hkVehicleBrakeClass_320;
extern hkClass hkVehicleDataClass_320;
extern hkClass hkVehicleDataWheelComponentParamsClass_320;
extern hkClass hkVehicleDefaultAerodynamicsClass_320;
extern hkClass hkVehicleDefaultAnalogDriverInputClass_320;
extern hkClass hkVehicleDefaultBrakeClass_320;
extern hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_320;
extern hkClass hkVehicleDefaultEngineClass_320;
extern hkClass hkVehicleDefaultSteeringClass_320;
extern hkClass hkVehicleDefaultSuspensionClass_320;
extern hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_320;
extern hkClass hkVehicleDefaultTransmissionClass_320;
extern hkClass hkVehicleDefaultVelocityDamperClass_320;
extern hkClass hkVehicleDriverInputAnalogStatusClass_320;
extern hkClass hkVehicleDriverInputClass_320;
extern hkClass hkVehicleDriverInputStatusClass_320;
extern hkClass hkVehicleEngineClass_320;
extern hkClass hkVehicleFrictionDescriptionClass_320;
extern hkClass hkVehicleFrictionStatusClass_320;
extern hkClass hkVehicleInstanceClass_320;
extern hkClass hkVehicleInstanceWheelInfoClass_320;
extern hkClass hkVehicleRaycastWheelCollideClass_320;
extern hkClass hkVehicleSteeringClass_320;
extern hkClass hkVehicleSuspensionClass_320;
extern hkClass hkVehicleSuspensionSuspensionWheelParametersClass_320;
extern hkClass hkVehicleTransmissionClass_320;
extern hkClass hkVehicleVelocityDamperClass_320;
extern hkClass hkVehicleWheelCollideClass_320;
extern hkClass hkVelocityConstraintMotorClass_320;
extern hkClass hkVersioningExceptionsArrayClass_320;
extern hkClass hkVersioningExceptionsArrayVersioningExceptionClass_320;
extern hkClass hkWaveletSkeletalAnimationClass_320;
extern hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_320;
extern hkClass hkWheelConstraintDataClass_320;
extern hkClass hkWheelConstraintDataConstraintBasisAClass_320;
extern hkClass hkWheelConstraintDataConstraintBasisBClass_320;
extern hkClass hkWorldCinfoClass_320;
extern hkClass hkWorldClass_320;
extern hkClass hkWorldMemoryWatchDogClass_320;
extern hkClass hkWorldObjectClass_320;
extern hkClass hkxAnimatedFloatClass_320;
extern hkClass hkxAnimatedMatrixClass_320;
extern hkClass hkxAnimatedQuaternionClass_320;
extern hkClass hkxAnimatedVectorClass_320;
extern hkClass hkxAttributeClass_320;
extern hkClass hkxAttributeGroupClass_320;
extern hkClass hkxCameraClass_320;
extern hkClass hkxIndexBufferClass_320;
extern hkClass hkxLightClass_320;
extern hkClass hkxMaterialClass_320;
extern hkClass hkxMaterialEffectClass_320;
extern hkClass hkxMaterialTextureStageClass_320;
extern hkClass hkxMeshClass_320;
extern hkClass hkxMeshSectionClass_320;
extern hkClass hkxNodeAnnotationDataClass_320;
extern hkClass hkxNodeClass_320;
extern hkClass hkxSceneClass_320;
extern hkClass hkxSkinBindingClass_320;
extern hkClass hkxSparselyAnimatedBoolClass_320;
extern hkClass hkxSparselyAnimatedEnumClass_320;
extern hkClass hkxSparselyAnimatedIntClass_320;
extern hkClass hkxSparselyAnimatedStringClass_320;
extern hkClass hkxSparselyAnimatedStringStringTypeClass_320;
extern hkClass hkxTextureFileClass_320;
extern hkClass hkxTextureInplaceClass_320;
extern hkClass hkxVertexBufferClass_320;
extern hkClass hkxVertexFormatClass_320;
extern hkClass hkxVertexP4N4C1T2Class_320;
extern hkClass hkxVertexP4N4T4B4C1T2Class_320;
extern hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_320;
extern hkClass hkxVertexP4N4T4B4W4I4Q4Class_320;
extern hkClass hkxVertexP4N4W4I4C1Q2Class_320;

hkClass hkBaseObjectClass_320(
	"hkBaseObject",
	HK_NULL, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkReferencedObjectClass_Members[] =
{
	{ "memSizeAndFlags", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "referenceCount", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_INT16, 0, 0, 0 }
};
hkClass hkReferencedObjectClass_320(
	"hkReferencedObject",
	&hkBaseObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkReferencedObjectClass_Members),
	int(sizeof(hkReferencedObjectClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkClassSignatureFlagsEnumItems_320[] =
{
	{1, "SIGNATURE_LOCAL"},
};
static const hkInternalClassEnum hkClassEnums[] = {
	{"SignatureFlags", hkClassSignatureFlagsEnumItems_320, 1 }
};
extern const hkClassEnum* hkClassSignatureFlagsEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkClassEnums[0]);
static hkInternalClassMember hkClassClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "parent", &hkClassClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "objectSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numImplementedInterfaces", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "declaredEnums", &hkClassEnumClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "declaredMembers", &hkClassMemberClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "defaults", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkClassClass_320(
	"hkClass",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkClassEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkClassClass_Members),
	int(sizeof(hkClassClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkClassEnum_ItemClass_Members[] =
{
	{ "value", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkClassEnumItemClass_320(
	"hkClassEnumItem",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkClassEnum_ItemClass_Members),
	int(sizeof(hkClassEnum_ItemClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkClassEnumClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "items", &hkClassEnumItemClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkClassEnumClass_320(
	"hkClassEnum",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkClassEnumClass_Members),
	int(sizeof(hkClassEnumClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkClassMemberTypeEnumItems_320[] =
{
	{0, "TYPE_VOID"},
	{1, "TYPE_BOOL"},
	{2, "TYPE_CHAR"},
	{3, "TYPE_INT8"},
	{4, "TYPE_UINT8"},
	{5, "TYPE_INT16"},
	{6, "TYPE_UINT16"},
	{7, "TYPE_INT32"},
	{8, "TYPE_UINT32"},
	{9, "TYPE_INT64"},
	{10, "TYPE_UINT64"},
	{11, "TYPE_REAL"},
	{12, "TYPE_VECTOR4"},
	{13, "TYPE_QUATERNION"},
	{14, "TYPE_MATRIX3"},
	{15, "TYPE_ROTATION"},
	{16, "TYPE_QSTRANSFORM"},
	{17, "TYPE_MATRIX4"},
	{18, "TYPE_TRANSFORM"},
	{19, "TYPE_ZERO"},
	{20, "TYPE_POINTER"},
	{21, "TYPE_FUNCTIONPOINTER"},
	{22, "TYPE_ARRAY"},
	{23, "TYPE_INPLACEARRAY"},
	{24, "TYPE_ENUM"},
	{25, "TYPE_STRUCT"},
	{26, "TYPE_SIMPLEARRAY"},
	{27, "TYPE_HOMOGENEOUSARRAY"},
	{28, "TYPE_VARIANT"},
	{29, "TYPE_MAX"},
};
static const hkInternalClassEnumItem hkClassMemberFlagsEnumItems_320[] =
{
	{1, "POINTER_OPTIONAL"},
	{2, "POINTER_VOIDSTAR"},
	{8, "ENUM_8"},
	{16, "ENUM_16"},
	{32, "ENUM_32"},
	{64, "ARRAY_RAWDATA"},
};
static const hkInternalClassEnumItem hkClassMemberRangeEnumItems_320[] =
{
	{0, "INVALID"},
	{1, "DEFAULT"},
	{2, "ABS_MIN"},
	{4, "ABS_MAX"},
	{8, "SOFT_MIN"},
	{16, "SOFT_MAX"},
	{32, "RANGE_MAX"},
};
static const hkInternalClassEnum hkClassMemberEnums[] = {
	{"Type", hkClassMemberTypeEnumItems_320, 30 },
	{"Flags", hkClassMemberFlagsEnumItems_320, 6 },
	{"Range", hkClassMemberRangeEnumItems_320, 7 }
};
extern const hkClassEnum* hkClassMemberTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[0]);
extern const hkClassEnum* hkClassMemberFlagsEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[1]);
extern const hkClassEnum* hkClassMemberRangeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[2]);
static hkInternalClassMember hkClassMemberClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "class", &hkClassClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "enum", &hkClassEnumClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "type", HK_NULL, hkClassMemberTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "subtype", HK_NULL, hkClassMemberTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "cArraySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkClassMemberClass_320(
	"hkClassMember",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkClassMemberEnums),
	3, 
	reinterpret_cast<const hkClassMember*>(hkClassMemberClass_Members),
	int(sizeof(hkClassMemberClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMonitorStreamStringMap_StringMapClass_Members[] =
{
	{ "id", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT64, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "string", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkMonitorStreamStringMapStringMapClass_320(
	"hkMonitorStreamStringMapStringMap",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMonitorStreamStringMap_StringMapClass_Members),
	int(sizeof(hkMonitorStreamStringMap_StringMapClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMonitorStreamStringMapClass_Members[] =
{
	{ "map", &hkMonitorStreamStringMapStringMapClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkMonitorStreamStringMapClass_320(
	"hkMonitorStreamStringMap",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMonitorStreamStringMapClass_Members),
	int(sizeof(hkMonitorStreamStringMapClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkMonitorStreamFrameInfoAbsoluteTimeCounterEnumItems_320[] =
{
	{0, "ABSOLUTE_TIME_TIMER_0"},
	{1, "ABSOLUTE_TIME_TIMER_1"},
	{0xffffffff, "ABSOLUTE_TIME_NOT_TIMED"},
};
static const hkInternalClassEnum hkMonitorStreamFrameInfoEnums[] = {
	{"AbsoluteTimeCounter", hkMonitorStreamFrameInfoAbsoluteTimeCounterEnumItems_320, 3 }
};
extern const hkClassEnum* hkMonitorStreamFrameInfoAbsoluteTimeCounterEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMonitorStreamFrameInfoEnums[0]);
static hkInternalClassMember hkMonitorStreamFrameInfoClass_Members[] =
{
	{ "heading", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "indexOfTimer0", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexOfTimer1", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absoluteTimeCounter", HK_NULL, hkMonitorStreamFrameInfoAbsoluteTimeCounterEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "timerFactor0", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "timerFactor1", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMonitorStreamFrameInfoClass_320(
	"hkMonitorStreamFrameInfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkMonitorStreamFrameInfoEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkMonitorStreamFrameInfoClass_Members),
	int(sizeof(hkMonitorStreamFrameInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkAabbClass_Members[] =
{
	{ "min", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "max", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkAabbClass_320(
	"hkAabb",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAabbClass_Members),
	int(sizeof(hkAabbClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkContactPointClass_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "separatingNormal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkContactPointClass_320(
	"hkContactPoint",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkContactPointClass_Members),
	int(sizeof(hkContactPointClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkContactPointMaterialFlagEnumEnumItems_320[] =
{
	{1, "CONTACT_IS_NEW_AND_POTENTIAL"},
	{2, "CONTACT_USES_SOLVER_PATH2"},
};
static const hkInternalClassEnum hkContactPointMaterialEnums[] = {
	{"FlagEnum", hkContactPointMaterialFlagEnumEnumItems_320, 2 }
};
extern const hkClassEnum* hkContactPointMaterialFlagEnumEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkContactPointMaterialEnums[0]);
static hkInternalClassMember hkContactPointMaterialClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkContactPointMaterialClass_320(
	"hkContactPointMaterial",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkContactPointMaterialEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkContactPointMaterialClass_Members),
	int(sizeof(hkContactPointMaterialClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMotionStateClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sweptTransform", &hkSweptTransformClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deltaAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationClass", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationCounter", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMotionStateClass_320(
	"hkMotionState",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMotionStateClass_Members),
	int(sizeof(hkMotionStateClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSphereClass_Members[] =
{
	{ "pos", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSphereClass_320(
	"hkSphere",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSphereClass_Members),
	int(sizeof(hkSphereClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSweptTransformClass_Members[] =
{
	{ "centerOfMass0", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "centerOfMass1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotation0", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotation1", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "centerOfMassLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSweptTransformClass_320(
	"hkSweptTransform",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSweptTransformClass_Members),
	int(sizeof(hkSweptTransformClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
extern const hkClassEnum* hkxAttributeHintEnum_320;
static hkInternalClassMember hkxAnimatedFloatClass_Members[] =
{
	{ "floats", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedFloatClass_320(
	"hkxAnimatedFloat",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAnimatedFloatClass_Members),
	int(sizeof(hkxAnimatedFloatClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
extern const hkClassEnum* hkxAttributeHintEnum_320;
static hkInternalClassMember hkxAnimatedMatrixClass_Members[] =
{
	{ "matrices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedMatrixClass_320(
	"hkxAnimatedMatrix",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAnimatedMatrixClass_Members),
	int(sizeof(hkxAnimatedMatrixClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxAnimatedQuaternionClass_Members[] =
{
	{ "quaternions", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QUATERNION, 0, 0, 0 }
};
hkClass hkxAnimatedQuaternionClass_320(
	"hkxAnimatedQuaternion",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAnimatedQuaternionClass_Members),
	int(sizeof(hkxAnimatedQuaternionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
extern const hkClassEnum* hkxAttributeHintEnum_320;
static hkInternalClassMember hkxAnimatedVectorClass_Members[] =
{
	{ "vectors", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedVectorClass_320(
	"hkxAnimatedVector",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAnimatedVectorClass_Members),
	int(sizeof(hkxAnimatedVectorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkscenedata_attributes_hkxAttributehkxAttributeHintEnumItems_320[] =
{
	{0, "HKX_HINT_NONE"},
	{1, "HKX_HINT_IGNORE"},
	{2, "HKX_HINT_TRANSFORM"},
	{4, "HKX_HINT_SCALE"},
	{6, "HKX_HINT_TRANSFORM_AND_SCALE"},
	{8, "HKX_HINT_FLIP"},
};
static const hkInternalClassEnum hkscenedata_attributes_hkxAttributeEnums[] = {
	{"hkxAttributeHint", hkscenedata_attributes_hkxAttributehkxAttributeHintEnumItems_320, 6 }
};
extern const hkClassEnum* hkxAttributeHintEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkscenedata_attributes_hkxAttributeEnums[0]);
static hkInternalClassMember hkxAttributeClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "value", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxAttributeClass_320(
	"hkxAttribute",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAttributeClass_Members),
	int(sizeof(hkxAttributeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxAttributeGroupClass_Members[] =
{
	{ "groupType", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "attributes", &hkxAttributeClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxAttributeGroupClass_320(
	"hkxAttributeGroup",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxAttributeGroupClass_Members),
	int(sizeof(hkxAttributeGroupClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSparselyAnimatedBoolClass_Members[] =
{
	{ "bools", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_BOOL, 0, 0, 0 },
	{ "times", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedBoolClass_320(
	"hkxSparselyAnimatedBool",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedBoolClass_Members),
	int(sizeof(hkxSparselyAnimatedBoolClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSparselyAnimatedEnumClass_Members[] =
{
	{ "type", &hkClassEnumClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedEnumClass_320(
	"hkxSparselyAnimatedEnum",
	&hkxSparselyAnimatedIntClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedEnumClass_Members),
	int(sizeof(hkxSparselyAnimatedEnumClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSparselyAnimatedIntClass_Members[] =
{
	{ "ints", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT32, 0, 0, 0 },
	{ "times", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedIntClass_320(
	"hkxSparselyAnimatedInt",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedIntClass_Members),
	int(sizeof(hkxSparselyAnimatedIntClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSparselyAnimatedString_StringTypeClass_Members[] =
{
	{ "string", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedStringStringTypeClass_320(
	"hkxSparselyAnimatedStringStringType",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedString_StringTypeClass_Members),
	int(sizeof(hkxSparselyAnimatedString_StringTypeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSparselyAnimatedStringClass_Members[] =
{
	{ "strings", &hkxSparselyAnimatedStringStringTypeClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "times", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedStringClass_320(
	"hkxSparselyAnimatedString",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSparselyAnimatedStringClass_Members),
	int(sizeof(hkxSparselyAnimatedStringClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxCameraClass_Members[] =
{
	{ "from", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "focus", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "fov", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "far", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "near", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "leftHanded", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxCameraClass_320(
	"hkxCamera",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxCameraClass_Members),
	int(sizeof(hkxCameraClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxNode_AnnotationDataClass_Members[] =
{
	{ "time", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "description", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkxNodeAnnotationDataClass_320(
	"hkxNodeAnnotationData",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxNode_AnnotationDataClass_Members),
	int(sizeof(hkxNode_AnnotationDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxNodeClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "object", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "keyFrames", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "children", &hkxNodeClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "annotations", &hkxNodeAnnotationDataClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "userProperties", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "selected", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "attributeGroups", &hkxAttributeGroupClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxNodeClass_320(
	"hkxNode",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxNodeClass_Members),
	int(sizeof(hkxNodeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkxLightLightTypeEnumItems_320[] =
{
	{0, "POINT_LIGHT"},
	{1, "DIRECTIONAL_LIGHT"},
	{2, "SPOT_LIGHT"},
};
static const hkInternalClassEnum hkxLightEnums[] = {
	{"LightType", hkxLightLightTypeEnumItems_320, 3 }
};
extern const hkClassEnum* hkxLightLightTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkxLightEnums[0]);
static hkInternalClassMember hkxLightClass_Members[] =
{
	{ "type", HK_NULL, hkxLightLightTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "direction", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "color", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxLightClass_320(
	"hkxLight",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkxLightEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkxLightClass_Members),
	int(sizeof(hkxLightClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkxMaterialTextureTypeEnumItems_320[] =
{
	{0, "TEX_UNKNOWN"},
	{1, "TEX_DIFFUSE"},
	{2, "TEX_REFLECTION"},
	{3, "TEX_BUMP"},
	{4, "TEX_NORMAL"},
	{5, "TEX_DISPLACEMENT"},
};
static const hkInternalClassEnum hkxMaterialEnums[] = {
	{"TextureType", hkxMaterialTextureTypeEnumItems_320, 6 }
};
extern const hkClassEnum* hkxMaterialTextureTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEnums[0]);
static hkInternalClassMember hkxMaterial_TextureStageClass_Members[] =
{
	{ "texture", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "usageHint", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxMaterialTextureStageClass_320(
	"hkxMaterialTextureStage",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxMaterial_TextureStageClass_Members),
	int(sizeof(hkxMaterial_TextureStageClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxMaterialClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "stages", &hkxMaterialTextureStageClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "diffuseColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ambientColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "specularColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "emissiveColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subMaterials", &hkxMaterialClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "extraData", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxMaterialClass_320(
	"hkxMaterial",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkxMaterialEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkxMaterialClass_Members),
	int(sizeof(hkxMaterialClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkxMaterialEffectEffectTypeEnumItems_320[] =
{
	{0, "EFFECT_TYPE_INVALID"},
	{1, "EFFECT_TYPE_UNKNOWN"},
	{2, "EFFECT_TYPE_HLSL_FX"},
	{3, "EFFECT_TYPE_CG_FX"},
	{4, "EFFECT_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxMaterialEffectEnums[] = {
	{"EffectType", hkxMaterialEffectEffectTypeEnumItems_320, 5 }
};
extern const hkClassEnum* hkxMaterialEffectEffectTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEffectEnums[0]);
static hkInternalClassMember hkxMaterialEffectClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "type", HK_NULL, hkxMaterialEffectEffectTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkxMaterialEffectClass_320(
	"hkxMaterialEffect",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkxMaterialEffectEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkxMaterialEffectClass_Members),
	int(sizeof(hkxMaterialEffectClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxTextureFileClass_Members[] =
{
	{ "filename", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkxTextureFileClass_320(
	"hkxTextureFile",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxTextureFileClass_Members),
	int(sizeof(hkxTextureFileClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxTextureInplaceClass_Members[] =
{
	{ "fileType", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 4, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkxTextureInplaceClass_320(
	"hkxTextureInplace",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxTextureInplaceClass_Members),
	int(sizeof(hkxTextureInplaceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkxIndexBufferIndexTypeEnumItems_320[] =
{
	{0, "INDEX_TYPE_INVALID"},
	{1, "INDEX_TYPE_TRI_LIST"},
	{2, "INDEX_TYPE_TRI_STRIP"},
	{3, "INDEX_TYPE_TRI_FAN"},
	{4, "INDEX_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxIndexBufferEnums[] = {
	{"IndexType", hkxIndexBufferIndexTypeEnumItems_320, 5 }
};
extern const hkClassEnum* hkxIndexBufferIndexTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkxIndexBufferEnums[0]);
static hkInternalClassMember hkxIndexBufferClass_Members[] =
{
	{ "indexType", HK_NULL, hkxIndexBufferIndexTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "vertexBaseOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxIndexBufferClass_320(
	"hkxIndexBuffer",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkxIndexBufferEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkxIndexBufferClass_Members),
	int(sizeof(hkxIndexBufferClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxMeshClass_Members[] =
{
	{ "sections", &hkxMeshSectionClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMeshClass_320(
	"hkxMesh",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxMeshClass_Members),
	int(sizeof(hkxMeshClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxMeshSectionClass_Members[] =
{
	{ "vertexBuffer", &hkxVertexBufferClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "indexBuffers", &hkxIndexBufferClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkxMaterialClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxMeshSectionClass_320(
	"hkxMeshSection",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxMeshSectionClass_Members),
	int(sizeof(hkxMeshSectionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexBufferClass_Members[] =
{
	{ "vertexData", HK_NULL, HK_NULL, hkClassMember::TYPE_HOMOGENEOUSARRAY, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "format", &hkxVertexFormatClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxVertexBufferClass_320(
	"hkxVertexBuffer",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexBufferClass_Members),
	int(sizeof(hkxVertexBufferClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexFormatClass_Members[] =
{
	{ "stride", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "positionOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normalOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tangentOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "binormalOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numBonesPerVertex", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneIndexOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneWeightOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTextureChannels", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tFloatCoordOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tQuantizedCoordOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "colorOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxVertexFormatClass_320(
	"hkxVertexFormat",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexFormatClass_Members),
	int(sizeof(hkxVertexFormatClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexP4N4C1T2Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "diffuse", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "u", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "v", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "padding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxVertexP4N4C1T2Class_320(
	"hkxVertexP4N4C1T2",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4C1T2Class_Members),
	int(sizeof(hkxVertexP4N4C1T2Class_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexP4N4T4B4C1T2Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tangent", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "binormal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "diffuse", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "u", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "v", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "padding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxVertexP4N4T4B4C1T2Class_320(
	"hkxVertexP4N4T4B4C1T2",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4C1T2Class_Members),
	int(sizeof(hkxVertexP4N4T4B4C1T2Class_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexP4N4T4B4W4I4C1Q2Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tangent", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "binormal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "weights", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indices", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "diffuse", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qu", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qv", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_320(
	"hkxVertexP4N4T4B4W4I4C1Q2",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4W4I4C1Q2Class_Members),
	int(sizeof(hkxVertexP4N4T4B4W4I4C1Q2Class_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexP4N4T4B4W4I4Q4Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tangent", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "binormal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "weights", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indices", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qu0", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "qu1", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkxVertexP4N4T4B4W4I4Q4Class_320(
	"hkxVertexP4N4T4B4W4I4Q4",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4W4I4Q4Class_Members),
	int(sizeof(hkxVertexP4N4T4B4W4I4Q4Class_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxVertexP4N4W4I4C1Q2Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "weights", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indices", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "diffuse", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qu", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qv", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxVertexP4N4W4I4C1Q2Class_320(
	"hkxVertexP4N4W4I4C1Q2",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4W4I4C1Q2Class_Members),
	int(sizeof(hkxVertexP4N4W4I4C1Q2Class_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkxSceneClass_Members[] =
{
	{ "modeller", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "asset", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "sceneLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rootNode", &hkxNodeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "cameras", &hkxCameraClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "lights", &hkxLightClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "meshes", &hkxMeshClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materials", &hkxMaterialClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "inplaceTextures", &hkxTextureInplaceClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "externalTextures", &hkxTextureFileClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skinBindings", &hkxSkinBindingClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "appliedTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkxScene_DefaultStruct
	{
		int s_defaultOffsets[12];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkMatrix4[16];
		_hkMatrix3 m_appliedTransform;
	};
	const hkxScene_DefaultStruct hkxScene_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkxScene_DefaultStruct,m_appliedTransform)},
		{ 1,0,0,0, 0,1,0,0, 0,0,1,0 }
	};
}
hkClass hkxSceneClass_320(
	"hkxScene",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSceneClass_Members),
	int(sizeof(hkxSceneClass_Members)/sizeof(hkInternalClassMember)),
	&hkxScene_Default
	);
static hkInternalClassMember hkxSkinBindingClass_Members[] =
{
	{ "mesh", &hkxMeshClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", &hkxNodeClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "initSkinTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxSkinBindingClass_320(
	"hkxSkinBinding",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkxSkinBindingClass_Members),
	int(sizeof(hkxSkinBindingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPackfileHeaderClass_Members[] =
{
	{ "magic", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "userTag", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "fileVersion", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "layoutRules", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 4, 0, 0 },
	{ "numSections", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsSectionIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsSectionOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsClassSectionIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsClassSectionOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsVersion", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 12, 0, 0 },
	{ "pad", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 3, 0, 0 }
};
hkClass hkPackfileHeaderClass_320(
	"hkPackfileHeader",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPackfileHeaderClass_Members),
	int(sizeof(hkPackfileHeaderClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPackfileSectionHeaderClass_Members[] =
{
	{ "sectionTag", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 19, 0, 0 },
	{ "nullByte", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absoluteDataStart", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "localFixupsOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "globalFixupsOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "virtualFixupsOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "externalFixupsOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spare", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPackfileSectionHeaderClass_320(
	"hkPackfileSectionHeader",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPackfileSectionHeaderClass_Members),
	int(sizeof(hkPackfileSectionHeaderClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRootLevelContainer_NamedVariantClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "variant", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRootLevelContainerNamedVariantClass_320(
	"hkRootLevelContainerNamedVariant",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRootLevelContainer_NamedVariantClass_Members),
	int(sizeof(hkRootLevelContainer_NamedVariantClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRootLevelContainerClass_Members[] =
{
	{ "namedVariants", &hkRootLevelContainerNamedVariantClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkRootLevelContainerClass_320(
	"hkRootLevelContainer",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRootLevelContainerClass_Members),
	int(sizeof(hkRootLevelContainerClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVersioningExceptionsArray_VersioningExceptionClass_Members[] =
{
	{ "className", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "oldSignature", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "newSignature", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVersioningExceptionsArrayVersioningExceptionClass_320(
	"hkVersioningExceptionsArrayVersioningException",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVersioningExceptionsArray_VersioningExceptionClass_Members),
	int(sizeof(hkVersioningExceptionsArray_VersioningExceptionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVersioningExceptionsArrayClass_Members[] =
{
	{ "exceptions", &hkVersioningExceptionsArrayVersioningExceptionClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVersioningExceptionsArrayClass_320(
	"hkVersioningExceptionsArray",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVersioningExceptionsArrayClass_Members),
	int(sizeof(hkVersioningExceptionsArrayClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkAnimationContainerClass_Members[] =
{
	{ "skeletons", &hkSkeletonClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "animations", &hkSkeletalAnimationClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindings", &hkAnimationBindingClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "attachments", &hkBoneAttachmentClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skins", &hkMeshBindingClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkAnimationContainerClass_320(
	"hkAnimationContainer",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAnimationContainerClass_Members),
	int(sizeof(hkAnimationContainerClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkAnimationBindingBlendHintEnumItems_320[] =
{
	{0, "NORMAL"},
	{1, "ADDITIVE"},
};
static const hkInternalClassEnum hkAnimationBindingEnums[] = {
	{"BlendHint", hkAnimationBindingBlendHintEnumItems_320, 2 }
};
extern const hkClassEnum* hkAnimationBindingBlendHintEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkAnimationBindingEnums[0]);
static hkInternalClassMember hkAnimationBindingClass_Members[] =
{
	{ "animation", &hkSkeletalAnimationClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "animationTrackToBoneIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "blendHint", HK_NULL, hkAnimationBindingBlendHintEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkAnimationBindingClass_320(
	"hkAnimationBinding",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkAnimationBindingEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkAnimationBindingClass_Members),
	int(sizeof(hkAnimationBindingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkAnnotationTrack_AnnotationClass_Members[] =
{
	{ "time", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "text", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkAnnotationTrackAnnotationClass_320(
	"hkAnnotationTrackAnnotation",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAnnotationTrack_AnnotationClass_Members),
	int(sizeof(hkAnnotationTrack_AnnotationClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkAnnotationTrackClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "annotations", &hkAnnotationTrackAnnotationClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkAnnotationTrackClass_320(
	"hkAnnotationTrack",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAnnotationTrackClass_Members),
	int(sizeof(hkAnnotationTrackClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletalAnimationClass_Members[] =
{
	{ "duration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numberOfTracks", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extractedMotion", &hkAnimatedReferenceFrameClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "annotationTracks", &hkAnnotationTrackClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkSkeletalAnimationClass_320(
	"hkSkeletalAnimation",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletalAnimationClass_Members),
	int(sizeof(hkSkeletalAnimationClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkDeltaCompressedSkeletalAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_320(
	"hkDeltaCompressedSkeletalAnimationQuantizationFormat",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedSkeletalAnimation_QuantizationFormatClass_Members),
	int(sizeof(hkDeltaCompressedSkeletalAnimation_QuantizationFormatClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkDeltaCompressedSkeletalAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkDeltaCompressedSkeletalAnimationClass_320(
	"hkDeltaCompressedSkeletalAnimation",
	&hkSkeletalAnimationClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedSkeletalAnimationClass_Members),
	int(sizeof(hkDeltaCompressedSkeletalAnimationClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkInterleavedSkeletalAnimationClass_Members[] =
{
	{ "transforms", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};
hkClass hkInterleavedSkeletalAnimationClass_320(
	"hkInterleavedSkeletalAnimation",
	&hkSkeletalAnimationClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkInterleavedSkeletalAnimationClass_Members),
	int(sizeof(hkInterleavedSkeletalAnimationClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWaveletSkeletalAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_320(
	"hkWaveletSkeletalAnimationQuantizationFormat",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWaveletSkeletalAnimation_QuantizationFormatClass_Members),
	int(sizeof(hkWaveletSkeletalAnimation_QuantizationFormatClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWaveletSkeletalAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkWaveletSkeletalAnimationQuantizationFormatClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "blockIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};
hkClass hkWaveletSkeletalAnimationClass_320(
	"hkWaveletSkeletalAnimation",
	&hkSkeletalAnimationClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWaveletSkeletalAnimationClass_Members),
	int(sizeof(hkWaveletSkeletalAnimationClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMeshBinding_MappingClass_Members[] =
{
	{ "mapping", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 }
};
hkClass hkMeshBindingMappingClass_320(
	"hkMeshBindingMapping",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMeshBinding_MappingClass_Members),
	int(sizeof(hkMeshBinding_MappingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMeshBindingClass_Members[] =
{
	{ "mesh", &hkxMeshClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mappings", &hkMeshBindingMappingClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "inverseWorldBindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_TRANSFORM, 0, 0, 0 }
};
hkClass hkMeshBindingClass_320(
	"hkMeshBinding",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMeshBindingClass_Members),
	int(sizeof(hkMeshBindingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletonMapperClass_Members[] =
{
	{ "mapping", &hkSkeletonMapperDataClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperClass_320(
	"hkSkeletonMapper",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperClass_Members),
	int(sizeof(hkSkeletonMapperClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletonMapperData_SimpleMappingClass_Members[] =
{
	{ "boneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperDataSimpleMappingClass_320(
	"hkSkeletonMapperDataSimpleMapping",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperData_SimpleMappingClass_Members),
	int(sizeof(hkSkeletonMapperData_SimpleMappingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletonMapperData_ChainMappingClass_Members[] =
{
	{ "startBoneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endBoneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "startBoneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endBoneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "startAFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endAFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperDataChainMappingClass_320(
	"hkSkeletonMapperDataChainMapping",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperData_ChainMappingClass_Members),
	int(sizeof(hkSkeletonMapperData_ChainMappingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletonMapperDataClass_Members[] =
{
	{ "skeletonA", &hkSkeletonClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeletonB", &hkSkeletonClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simpleMappings", &hkSkeletonMapperDataSimpleMappingClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chainMappings", &hkSkeletonMapperDataChainMappingClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "unmappedBones", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "keepUnmappedLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperDataClass_320(
	"hkSkeletonMapperData",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperDataClass_Members),
	int(sizeof(hkSkeletonMapperDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkAnimatedReferenceFrameClass_320(
	"hkAnimatedReferenceFrame",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkDefaultAnimatedReferenceFrameClass_Members[] =
{
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forward", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "duration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceFrameSamples", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};
hkClass hkDefaultAnimatedReferenceFrameClass_320(
	"hkDefaultAnimatedReferenceFrame",
	&hkAnimatedReferenceFrameClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDefaultAnimatedReferenceFrameClass_Members),
	int(sizeof(hkDefaultAnimatedReferenceFrameClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBoneClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "lockTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoneClass_320(
	"hkBone",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBoneClass_Members),
	int(sizeof(hkBoneClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBoneAttachmentClass_Members[] =
{
	{ "boneFromAttachment", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "attachment", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoneAttachmentClass_320(
	"hkBoneAttachment",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBoneAttachmentClass_Members),
	int(sizeof(hkBoneAttachmentClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSkeletonClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "parentIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "bones", &hkBoneClass_320, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "referencePose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};
hkClass hkSkeletonClass_320(
	"hkSkeleton",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSkeletonClass_Members),
	int(sizeof(hkSkeletonClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCdBodyClass_Members[] =
{
	{ "shape", &hkShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motion", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "parent", &hkCdBodyClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkCdBodyClass_320(
	"hkCdBody",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCdBodyClass_Members),
	int(sizeof(hkCdBodyClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCollidableClass_Members[] =
{
	{ "ownerOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseHandle", &hkTypedBroadPhaseHandleClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowedPenetrationDepth", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCollidableClass_320(
	"hkCollidable",
	&hkCdBodyClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCollidableClass_Members),
	int(sizeof(hkCollidableClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkShapeCollectionFilterClass_320(
	"hkShapeCollectionFilter",
	HK_NULL, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkTypedBroadPhaseHandleClass_Members[] =
{
	{ "type", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ownerOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectQualityType", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTypedBroadPhaseHandleClass_320(
	"hkTypedBroadPhaseHandle",
	&hkBroadPhaseHandleClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTypedBroadPhaseHandleClass_Members),
	int(sizeof(hkTypedBroadPhaseHandleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkCollidableCollidableFilterClass_320(
	"hkCollidableCollidableFilter",
	HK_NULL, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkCollisionFilterClass_320(
	"hkCollisionFilter",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	4, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkRayCollidableFilterClass_320(
	"hkRayCollidableFilter",
	HK_NULL, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkGroupFilterClass_Members[] =
{
	{ "nextFreeSystemGroup", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionLookupTable", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 32, 0, 0 }
};
hkClass hkGroupFilterClass_320(
	"hkGroupFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkGroupFilterClass_Members),
	int(sizeof(hkGroupFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCollisionFilterListClass_Members[] =
{
	{ "collisionFilters", &hkCollisionFilterClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkCollisionFilterListClass_320(
	"hkCollisionFilterList",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCollisionFilterListClass_Members),
	int(sizeof(hkCollisionFilterListClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkNullCollisionFilterClass_320(
	"hkNullCollisionFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkRayShapeCollectionFilterClass_320(
	"hkRayShapeCollectionFilter",
	HK_NULL, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkShapeClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkShapeClass_320(
	"hkShape",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkShapeClass_Members),
	int(sizeof(hkShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkShapeRayCastInputClass_Members[] =
{
	{ "from", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "to", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "filterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rayShapeCollectionFilter", &hkRayShapeCollectionFilterClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkShapeRayCastInputClass_320(
	"hkShapeRayCastInput",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkShapeRayCastInputClass_Members),
	int(sizeof(hkShapeRayCastInputClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBoxShapeClass_Members[] =
{
	{ "halfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoxShapeClass_320(
	"hkBoxShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBoxShapeClass_Members),
	int(sizeof(hkBoxShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBvShapeClass_Members[] =
{
	{ "boundingVolumeShape", &hkShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childShape", &hkShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBvShapeClass_320(
	"hkBvShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBvShapeClass_Members),
	int(sizeof(hkBvShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBvTreeShapeClass_Members[] =
{
	{ "shapeCollection", &hkShapeCollectionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBvTreeShapeClass_320(
	"hkBvTreeShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBvTreeShapeClass_Members),
	int(sizeof(hkBvTreeShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCapsuleShapeClass_Members[] =
{
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCapsuleShapeClass_320(
	"hkCapsuleShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCapsuleShapeClass_Members),
	int(sizeof(hkCapsuleShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkShapeCollectionClass_Members[] =
{
	{ "disableWelding", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkShapeCollectionClass_320(
	"hkShapeCollection",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkShapeCollectionClass_Members),
	int(sizeof(hkShapeCollectionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexShapeClass_Members[] =
{
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexShapeClass_320(
	"hkConvexShape",
	&hkSphereRepShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexShapeClass_Members),
	int(sizeof(hkConvexShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexListShapeClass_Members[] =
{
	{ "minDistanceToUseConvexHullForGetClosestPoints", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexListShapeClass_320(
	"hkConvexListShape",
	&hkListShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexListShapeClass_Members),
	int(sizeof(hkConvexListShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexPieceMeshShapeClass_Members[] =
{
	{ "convexPieceStream", &hkConvexPieceStreamDataClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayMesh", &hkShapeCollectionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexPieceMeshShapeClass_320(
	"hkConvexPieceMeshShape",
	&hkShapeCollectionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexPieceMeshShapeClass_Members),
	int(sizeof(hkConvexPieceMeshShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexTransformShapeClass_Members[] =
{
	{ "childShape", &hkConvexShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexTransformShapeClass_320(
	"hkConvexTransformShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexTransformShapeClass_Members),
	int(sizeof(hkConvexTransformShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexTranslateShapeClass_Members[] =
{
	{ "childShape", &hkConvexShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "translation", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexTranslateShapeClass_320(
	"hkConvexTranslateShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexTranslateShapeClass_Members),
	int(sizeof(hkConvexTranslateShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexVerticesShape_FourVectorsClass_Members[] =
{
	{ "x", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "y", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "z", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexVerticesShapeFourVectorsClass_320(
	"hkConvexVerticesShapeFourVectors",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexVerticesShape_FourVectorsClass_Members),
	int(sizeof(hkConvexVerticesShape_FourVectorsClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexVerticesShapeClass_Members[] =
{
	{ "aabbHalfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotatedVertices", &hkConvexVerticesShapeFourVectorsClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeEquations", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};
hkClass hkConvexVerticesShapeClass_320(
	"hkConvexVerticesShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexVerticesShapeClass_Members),
	int(sizeof(hkConvexVerticesShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkCylinderShapeVertexIdEncodingEnumItems_320[] =
{
	{7, "VERTEX_ID_ENCODING_IS_BASE_A_SHIFT"},
	{6, "VERTEX_ID_ENCODING_SIN_SIGN_SHIFT"},
	{5, "VERTEX_ID_ENCODING_COS_SIGN_SHIFT"},
	{4, "VERTEX_ID_ENCODING_IS_SIN_LESSER_SHIFT"},
	{0x0f, "VERTEX_ID_ENCODING_VALUE_MASK"},
};
static const hkInternalClassEnum hkCylinderShapeEnums[] = {
	{"VertexIdEncoding", hkCylinderShapeVertexIdEncodingEnumItems_320, 5 }
};
extern const hkClassEnum* hkCylinderShapeVertexIdEncodingEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkCylinderShapeEnums[0]);
static hkInternalClassMember hkCylinderShapeClass_Members[] =
{
	{ "cylRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCylinderShapeClass_320(
	"hkCylinderShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkCylinderShapeEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkCylinderShapeClass_Members),
	int(sizeof(hkCylinderShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkFastMeshShapeClass_320(
	"hkFastMeshShape",
	&hkMeshShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkHeightFieldShapeClass_320(
	"hkHeightFieldShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkListShape_ChildInfoClass_Members[] =
{
	{ "shape", &hkShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkListShapeChildInfoClass_320(
	"hkListShapeChildInfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkListShape_ChildInfoClass_Members),
	int(sizeof(hkListShape_ChildInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkListShapeClass_Members[] =
{
	{ "childInfo", &hkListShapeChildInfoClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkListShapeClass_320(
	"hkListShape",
	&hkShapeCollectionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkListShapeClass_Members),
	int(sizeof(hkListShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMeshMaterialClass_Members[] =
{
	{ "filterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMeshMaterialClass_320(
	"hkMeshMaterial",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMeshMaterialClass_Members),
	int(sizeof(hkMeshMaterialClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkMeshShapeIndexStridingTypeEnumItems_320[] =
{
	{0, "INDICES_INVALID"},
	{1, "INDICES_INT16"},
	{2, "INDICES_INT32"},
	{3, "INDICES_MAX_ID"},
};
static const hkInternalClassEnumItem hkMeshShapeMaterialIndexStridingTypeEnumItems_320[] =
{
	{0, "MATERIAL_INDICES_INVALID"},
	{1, "MATERIAL_INDICES_INT8"},
	{2, "MATERIAL_INDICES_INT16"},
	{3, "MATERIAL_INDICES_MAX_ID"},
};
static const hkInternalClassEnum hkMeshShapeEnums[] = {
	{"IndexStridingType", hkMeshShapeIndexStridingTypeEnumItems_320, 4 },
	{"MaterialIndexStridingType", hkMeshShapeMaterialIndexStridingTypeEnumItems_320, 4 }
};
extern const hkClassEnum* hkMeshShapeIndexStridingTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[0]);
extern const hkClassEnum* hkMeshShapeMaterialIndexStridingTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[1]);
static hkInternalClassMember hkMeshShape_SubpartClass_Members[] =
{
	{ "vertexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "vertexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "stridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "materialIndexStridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialBase", &hkMeshMaterialClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numMaterials", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMeshShapeSubpartClass_320(
	"hkMeshShapeSubpart",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMeshShape_SubpartClass_Members),
	int(sizeof(hkMeshShape_SubpartClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMeshShapeClass_Members[] =
{
	{ "scaling", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numBitsForSubpartIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subparts", &hkMeshShapeSubpartClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMeshShapeClass_320(
	"hkMeshShape",
	&hkShapeCollectionClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkMeshShapeEnums),
	2, 
	reinterpret_cast<const hkClassMember*>(hkMeshShapeClass_Members),
	int(sizeof(hkMeshShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMoppBvTreeShapeClass_Members[] =
{
	{ "code", &hkMoppCodeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkMoppBvTreeShapeClass_320(
	"hkMoppBvTreeShape",
	&hkBvTreeShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMoppBvTreeShapeClass_Members),
	int(sizeof(hkMoppBvTreeShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMultiRayShape_RayClass_Members[] =
{
	{ "start", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "end", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMultiRayShapeRayClass_320(
	"hkMultiRayShapeRay",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMultiRayShape_RayClass_Members),
	int(sizeof(hkMultiRayShape_RayClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMultiRayShapeClass_Members[] =
{
	{ "rays", &hkMultiRayShapeRayClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rayPenetrationDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMultiRayShapeClass_320(
	"hkMultiRayShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMultiRayShapeClass_Members),
	int(sizeof(hkMultiRayShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkMultiSphereShapetestEnumItems_320[] =
{
	{8, "MAX_SPHERES"},
};
static const hkInternalClassEnum hkMultiSphereShapeEnums[] = {
	{"test", hkMultiSphereShapetestEnumItems_320, 1 }
};
extern const hkClassEnum* hkMultiSphereShapetestEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMultiSphereShapeEnums[0]);
static hkInternalClassMember hkMultiSphereShapeClass_Members[] =
{
	{ "numSpheres", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spheres", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 8/*hkMultiSphereShape::MAX_SPHERES*/, 0, 0 }
};
hkClass hkMultiSphereShapeClass_320(
	"hkMultiSphereShape",
	&hkSphereRepShapeClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkMultiSphereShapeEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkMultiSphereShapeClass_Members),
	int(sizeof(hkMultiSphereShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkPhantomCallbackShapeClass_320(
	"hkPhantomCallbackShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkPlaneShapeClass_Members[] =
{
	{ "plane", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbHalfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPlaneShapeClass_320(
	"hkPlaneShape",
	&hkHeightFieldShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPlaneShapeClass_Members),
	int(sizeof(hkPlaneShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSampledHeightFieldShapeClass_Members[] =
{
	{ "xRes", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "zRes", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "heightCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "intToFloatScale", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "floatToIntScale", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "floatToIntOffsetFloorCorrected", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSampledHeightFieldShapeClass_320(
	"hkSampledHeightFieldShape",
	&hkHeightFieldShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSampledHeightFieldShapeClass_Members),
	int(sizeof(hkSampledHeightFieldShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSimpleMeshShape_TriangleClass_Members[] =
{
	{ "a", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "b", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "c", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSimpleMeshShapeTriangleClass_320(
	"hkSimpleMeshShapeTriangle",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSimpleMeshShape_TriangleClass_Members),
	int(sizeof(hkSimpleMeshShape_TriangleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSimpleMeshShapeClass_Members[] =
{
	{ "vertices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "triangles", &hkSimpleMeshShapeTriangleClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSimpleMeshShapeClass_320(
	"hkSimpleMeshShape",
	&hkShapeCollectionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSimpleMeshShapeClass_Members),
	int(sizeof(hkSimpleMeshShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkSphereShapeClass_320(
	"hkSphereShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkSphereRepShapeClass_320(
	"hkSphereRepShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkStorageMeshShape_SubpartStorageClass_Members[] =
{
	{ "vertices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "materials", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "materialIndices16", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 }
};
hkClass hkStorageMeshShapeSubpartStorageClass_320(
	"hkStorageMeshShapeSubpartStorage",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkStorageMeshShape_SubpartStorageClass_Members),
	int(sizeof(hkStorageMeshShape_SubpartStorageClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkStorageMeshShapeClass_Members[] =
{
	{ "storage", &hkStorageMeshShapeSubpartStorageClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkStorageMeshShapeClass_320(
	"hkStorageMeshShape",
	&hkMeshShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkStorageMeshShapeClass_Members),
	int(sizeof(hkStorageMeshShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkStorageSampledHeightFieldShapeClass_Members[] =
{
	{ "storage", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "triangleFlip", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkStorageSampledHeightFieldShapeClass_320(
	"hkStorageSampledHeightFieldShape",
	&hkSampledHeightFieldShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkStorageSampledHeightFieldShapeClass_Members),
	int(sizeof(hkStorageSampledHeightFieldShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTransformShapeClass_Members[] =
{
	{ "childShape", &hkShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTransformShapeClass_320(
	"hkTransformShape",
	&hkShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTransformShapeClass_Members),
	int(sizeof(hkTransformShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTriangleShapeClass_Members[] =
{
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexC", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriangleShapeClass_320(
	"hkTriangleShape",
	&hkConvexShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTriangleShapeClass_Members),
	int(sizeof(hkTriangleShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTriPatchTriangleClass_Members[] =
{
	{ "neighbours", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 3, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceCountAndConnectivity", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriPatchTriangleClass_320(
	"hkTriPatchTriangle",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTriPatchTriangleClass_Members),
	int(sizeof(hkTriPatchTriangleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTriSampledHeightFieldBvTreeShapeClass_Members[] =
{
	{ "wantAabbRejectionTest", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriSampledHeightFieldBvTreeShapeClass_320(
	"hkTriSampledHeightFieldBvTreeShape",
	&hkBvTreeShapeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTriSampledHeightFieldBvTreeShapeClass_Members),
	int(sizeof(hkTriSampledHeightFieldBvTreeShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTriSampledHeightFieldCollectionClass_Members[] =
{
	{ "heightfield", &hkSampledHeightFieldShapeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriSampledHeightFieldCollectionClass_320(
	"hkTriSampledHeightFieldCollection",
	&hkShapeCollectionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTriSampledHeightFieldCollectionClass_Members),
	int(sizeof(hkTriSampledHeightFieldCollectionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkActionClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "island", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkActionClass_320(
	"hkAction",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkActionClass_Members),
	int(sizeof(hkActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkArrayActionClass_Members[] =
{
	{ "entities", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkArrayActionClass_320(
	"hkArrayAction",
	&hkActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkArrayActionClass_Members),
	int(sizeof(hkArrayActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBinaryActionClass_Members[] =
{
	{ "entityA", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entityB", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBinaryActionClass_320(
	"hkBinaryAction",
	&hkActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBinaryActionClass_Members),
	int(sizeof(hkBinaryActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkUnaryActionClass_Members[] =
{
	{ "entity", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkUnaryActionClass_320(
	"hkUnaryAction",
	&hkActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkUnaryActionClass_Members),
	int(sizeof(hkUnaryActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkMaterialResponseTypeEnumItems_320[] =
{
	{0, "RESPONSE_INVALID"},
	{1, "RESPONSE_SIMPLE_CONTACT"},
	{2, "RESPONSE_REPORTING"},
	{3, "RESPONSE_NONE"},
	{4, "RESPONSE_MAX_ID"},
};
static const hkInternalClassEnum hkMaterialEnums[] = {
	{"ResponseType", hkMaterialResponseTypeEnumItems_320, 5 }
};
extern const hkClassEnum* hkMaterialResponseTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMaterialEnums[0]);
static hkInternalClassMember hkMaterialClass_Members[] =
{
	{ "responseType", HK_NULL, hkMaterialResponseTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMaterialClass_320(
	"hkMaterial",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkMaterialEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkMaterialClass_Members),
	int(sizeof(hkMaterialClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPropertyValueClass_Members[] =
{
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT64, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPropertyValueClass_320(
	"hkPropertyValue",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPropertyValueClass_Members),
	int(sizeof(hkPropertyValueClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPropertyClass_Members[] =
{
	{ "key", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "alignmentPadding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "value", &hkPropertyValueClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPropertyClass_320(
	"hkProperty",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPropertyClass_Members),
	int(sizeof(hkPropertyClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkdynamics_constraint_hkConstraintDatahkConstraintTypeEnumItems_320[] =
{
	{0, "HK_CONSTRAINT_TYPE_BALLANDSOCKET"},
	{1, "HK_CONSTRAINT_TYPE_HINGE"},
	{2, "HK_CONSTRAINT_TYPE_LIMITEDHINGE"},
	{3, "HK_CONSTRAINT_TYPE_POINTTOPATH"},
	{4, "HK_CONSTRAINT_TYPE_POWEREDHINGE"},
	{5, "HK_CONSTRAINT_TYPE_POWEREDRAGDOLL"},
	{6, "HK_CONSTRAINT_TYPE_PRISMATIC"},
	{7, "HK_CONSTRAINT_TYPE_RAGDOLL"},
	{8, "HK_CONSTRAINT_TYPE_STIFFSPRING"},
	{9, "HK_CONSTRAINT_TYPE_WHEEL"},
	{10, "HK_CONSTRAINT_TYPE_GENERIC"},
	{11, "HK_CONSTRAINT_TYPE_CONTACT"},
	{12, "HK_CONSTRAINT_TYPE_BREAKABLE"},
	{13, "HK_CONSTRAINT_TYPE_MALLEABLE"},
	{14, "HK_CONSTRAINT_TYPE_POINTTOPLANE"},
};
static const hkInternalClassEnum hkdynamics_constraint_hkConstraintDataEnums[] = {
	{"hkConstraintType", hkdynamics_constraint_hkConstraintDatahkConstraintTypeEnumItems_320, 15 }
};
extern const hkClassEnum* hkConstraintTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkdynamics_constraint_hkConstraintDataEnums[0]);
static hkInternalClassMember hkConstraintDataClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConstraintDataClass_320(
	"hkConstraintData",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConstraintDataClass_Members),
	int(sizeof(hkConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConstraintInfoClass_Members[] =
{
	{ "maxSizeOfJacobians", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sizeOfJacobians", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sizeOfSchemas", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numSolverResults", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConstraintInfoClass_320(
	"hkConstraintInfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConstraintInfoClass_Members),
	int(sizeof(hkConstraintInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkConstraintInstanceConstraintPriorityEnumItems_320[] =
{
	{0, "PRIORITY_INVALID"},
	{1, "PRIORITY_PSI"},
	{2, "PRIORITY_TOI"},
	{3, "PRIORITY_TOI_HIGHER"},
	{4, "PRIORITY_TOI_FORCED"},
};
static const hkInternalClassEnum hkConstraintInstanceEnums[] = {
	{"ConstraintPriority", hkConstraintInstanceConstraintPriorityEnumItems_320, 5 }
};
extern const hkClassEnum* hkConstraintInstanceConstraintPriorityEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[0]);
static hkInternalClassMember hkConstraintInstanceClass_Members[] =
{
	{ "owner", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "data", &hkConstraintDataClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entities", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 2, 0, 0 },
	{ "priority", HK_NULL, hkConstraintInstanceConstraintPriorityEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "wantRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internal", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkConstraintInstanceClass_320(
	"hkConstraintInstance",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkConstraintInstanceEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkConstraintInstanceClass_Members),
	int(sizeof(hkConstraintInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBallAndSocketConstraintDataClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBallAndSocketConstraintDataClass_320(
	"hkBallAndSocketConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBallAndSocketConstraintDataClass_Members),
	int(sizeof(hkBallAndSocketConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataConstraintBasisAClass_320(
	"hkHingeConstraintDataConstraintBasisA",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkHingeConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataConstraintBasisBClass_320(
	"hkHingeConstraintDataConstraintBasisB",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkHingeConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkHingeConstraintDataClass_Members[] =
{
	{ "basisA", &hkHingeConstraintDataConstraintBasisAClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkHingeConstraintDataConstraintBasisBClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataClass_320(
	"hkHingeConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintDataClass_Members),
	int(sizeof(hkHingeConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_320(
	"hkLimitedHingeConstraintDataConstraintBasisA",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkLimitedHingeConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perp2FreeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_320(
	"hkLimitedHingeConstraintDataConstraintBasisB",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkLimitedHingeConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkLimitedHingeConstraintDataClass_Members[] =
{
	{ "minAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisA", &hkLimitedHingeConstraintDataConstraintBasisAClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkLimitedHingeConstraintDataConstraintBasisBClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkLimitedHingeConstraintData_DefaultStruct
	{
		int s_defaultOffsets[6];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkMatrix4[16];
		hkReal m_angularLimitsTauFactor;
	};
	const hkLimitedHingeConstraintData_DefaultStruct hkLimitedHingeConstraintData_Default =
	{
		{-1,-1,-1,HK_OFFSET_OF(hkLimitedHingeConstraintData_DefaultStruct,m_angularLimitsTauFactor),-1,-1},
		1.0f
	};
}
hkClass hkLimitedHingeConstraintDataClass_320(
	"hkLimitedHingeConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintDataClass_Members),
	int(sizeof(hkLimitedHingeConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	&hkLimitedHingeConstraintData_Default
	);
static hkInternalClassMember hkLinearParametricCurveClass_Members[] =
{
	{ "smoothingFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "closedLoop", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dirNotParallelToTangentAlongWholePath", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "points", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "distance", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkLinearParametricCurveClass_320(
	"hkLinearParametricCurve",
	&hkParametricCurveClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkLinearParametricCurveClass_Members),
	int(sizeof(hkLinearParametricCurveClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkParametricCurveClass_320(
	"hkParametricCurve",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static const hkInternalClassEnumItem hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_320[] =
{
	{0, "CONSTRAIN_ORIENTATION_INVALID"},
	{1, "CONSTRAIN_ORIENTATION_NONE"},
	{2, "CONSTRAIN_ORIENTATION_ALLOW_SPIN"},
	{3, "CONSTRAIN_ORIENTATION_TO_PATH"},
	{4, "CONSTRAIN_ORIENTATION_MAX_ID"},
};
static const hkInternalClassEnum hkPointToPathConstraintDataEnums[] = {
	{"OrientationConstraintType", hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_320, 5 }
};
extern const hkClassEnum* hkPointToPathConstraintDataOrientationConstraintTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkPointToPathConstraintDataEnums[0]);
static hkInternalClassMember hkPointToPathConstraintDataClass_Members[] =
{
	{ "path", &hkParametricCurveClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularConstrainedDOF", HK_NULL, hkPointToPathConstraintDataOrientationConstraintTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "transform_OS_KS", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkPointToPathConstraintDataClass_320(
	"hkPointToPathConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkPointToPathConstraintDataEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkPointToPathConstraintDataClass_Members),
	int(sizeof(hkPointToPathConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPointToPlaneConstraintDataClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeNormalA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPointToPlaneConstraintDataClass_320(
	"hkPointToPlaneConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPointToPlaneConstraintDataClass_Members),
	int(sizeof(hkPointToPlaneConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPoweredHingeConstraintDataClass_Members[] =
{
	{ "motorActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ignoreLimits", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "targetAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motor", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPoweredHingeConstraintDataClass_320(
	"hkPoweredHingeConstraintData",
	&hkLimitedHingeConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPoweredHingeConstraintDataClass_Members),
	int(sizeof(hkPoweredHingeConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "BtoAoffsetRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_ROTATION, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataConstraintBasisAClass_320(
	"hkPrismaticConstraintDataConstraintBasisA",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkPrismaticConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToShaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataConstraintBasisBClass_320(
	"hkPrismaticConstraintDataConstraintBasisB",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkPrismaticConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPrismaticConstraintDataClass_Members[] =
{
	{ "motor", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "basisA", &hkPrismaticConstraintDataConstraintBasisAClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkPrismaticConstraintDataConstraintBasisBClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motorTargetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataClass_320(
	"hkPrismaticConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintDataClass_Members),
	int(sizeof(hkPrismaticConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkStiffSpringConstraintDataClass_Members[] =
{
	{ "springLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkStiffSpringConstraintDataClass_320(
	"hkStiffSpringConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkStiffSpringConstraintDataClass_Members),
	int(sizeof(hkStiffSpringConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWheelConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataConstraintBasisAClass_320(
	"hkWheelConstraintDataConstraintBasisA",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkWheelConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWheelConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "steeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToSteeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToSuspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataConstraintBasisBClass_320(
	"hkWheelConstraintDataConstraintBasisB",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkWheelConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWheelConstraintDataClass_Members[] =
{
	{ "basisA", &hkWheelConstraintDataConstraintBasisAClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkWheelConstraintDataConstraintBasisBClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMinLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMaxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataClass_320(
	"hkWheelConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintDataClass_Members),
	int(sizeof(hkWheelConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBreakableConstraintDataClass_Members[] =
{
	{ "constraintData", &hkConstraintDataClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childRuntimeSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "childNumSolverResults", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "solverResultLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "removeWhenBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "revertBackVelocityOnBreak", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "listener", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkBreakableConstraintDataClass_320(
	"hkBreakableConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBreakableConstraintDataClass_Members),
	int(sizeof(hkBreakableConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkGenericConstraintDataClass_Members[] =
{
	{ "scheme", &hkGenericConstraintDataSchemeClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkGenericConstraintDataClass_320(
	"hkGenericConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkGenericConstraintDataClass_Members),
	int(sizeof(hkGenericConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkGenericConstraintDataSchemeClass_Members[] =
{
	{ "info", &hkConstraintInfoClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "commands", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT32, 0, 0, 0 },
	{ "modifiers", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "motors", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkGenericConstraintDataSchemeClass_320(
	"hkGenericConstraintDataScheme",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkGenericConstraintDataSchemeClass_Members),
	int(sizeof(hkGenericConstraintDataSchemeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMalleableConstraintDataClass_Members[] =
{
	{ "constraintData", &hkConstraintDataClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMalleableConstraintDataClass_320(
	"hkMalleableConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMalleableConstraintDataClass_Members),
	int(sizeof(hkMalleableConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkConstraintMotorClass_320(
	"hkConstraintMotor",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkPositionConstraintMotorClass_Members[] =
{
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "proportionalRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "constantRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPositionConstraintMotorClass_320(
	"hkPositionConstraintMotor",
	&hkConstraintMotorClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPositionConstraintMotorClass_Members),
	int(sizeof(hkPositionConstraintMotorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSpringDamperConstraintMotorClass_Members[] =
{
	{ "springConstant", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxPosForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxNegForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpringDamperConstraintMotorClass_320(
	"hkSpringDamperConstraintMotor",
	&hkConstraintMotorClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSpringDamperConstraintMotorClass_Members),
	int(sizeof(hkSpringDamperConstraintMotorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVelocityConstraintMotorClass_Members[] =
{
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "velocityTarget", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxPosForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxNegForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVelocityConstraintMotorClass_320(
	"hkVelocityConstraintMotor",
	&hkConstraintMotorClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVelocityConstraintMotorClass_Members),
	int(sizeof(hkVelocityConstraintMotorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkEntityClass_Members[] =
{
	{ "motion", &hkMotionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simulationIsland", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkMaterialClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivator", &hkEntityDeactivatorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "constraintsMaster", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "constraintsSlave", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "constraintRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "storageIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "processContactCallbackDelay", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "autoRemoveLevel", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "fixed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isFixedOrKeyframed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internalCollideFlag", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "activationListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "entityListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "actions", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "uid", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkEntity_DefaultStruct
	{
		int s_defaultOffsets[18];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkMatrix4[16];
		hkUint32 m_uid;
	};
	const hkEntity_DefaultStruct hkEntity_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkEntity_DefaultStruct,m_uid)},
		hkUint32(-1)
	};
}
hkClass hkEntityClass_320(
	"hkEntity",
	&hkWorldObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkEntityClass_Members),
	int(sizeof(hkEntityClass_Members)/sizeof(hkInternalClassMember)),
	&hkEntity_Default
	);
hkClass hkEntityDeactivatorClass_320(
	"hkEntityDeactivator",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkFakeRigidBodyDeactivatorClass_320(
	"hkFakeRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkRigidBodyClass_320(
	"hkRigidBody",
	&hkEntityClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static const hkInternalClassEnumItem hkRigidBodyDeactivatorDeactivatorTypeEnumItems_320[] =
{
	{0, "DEACTIVATOR_INVALID"},
	{1, "DEACTIVATOR_NEVER"},
	{2, "DEACTIVATOR_SPATIAL"},
	{3, "DEACTIVATOR_MAX_ID"},
};
static const hkInternalClassEnum hkRigidBodyDeactivatorEnums[] = {
	{"DeactivatorType", hkRigidBodyDeactivatorDeactivatorTypeEnumItems_320, 4 }
};
extern const hkClassEnum* hkRigidBodyDeactivatorDeactivatorTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkRigidBodyDeactivatorEnums[0]);
hkClass hkRigidBodyDeactivatorClass_320(
	"hkRigidBodyDeactivator",
	&hkEntityDeactivatorClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkRigidBodyDeactivatorEnums),
	1, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkSpatialRigidBodyDeactivator_SampleClass_Members[] =
{
	{ "refPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "refRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpatialRigidBodyDeactivatorSampleClass_320(
	"hkSpatialRigidBodyDeactivatorSample",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSpatialRigidBodyDeactivator_SampleClass_Members),
	int(sizeof(hkSpatialRigidBodyDeactivator_SampleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSpatialRigidBodyDeactivatorClass_Members[] =
{
	{ "highFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "radiusSqrd", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpatialRigidBodyDeactivatorClass_320(
	"hkSpatialRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSpatialRigidBodyDeactivatorClass_Members),
	int(sizeof(hkSpatialRigidBodyDeactivatorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkMotionMotionTypeEnumItems_320[] =
{
	{0, "MOTION_INVALID"},
	{1, "MOTION_DYNAMIC"},
	{2, "MOTION_SPHERE_INERTIA"},
	{3, "MOTION_STABILIZED_SPHERE_INERTIA"},
	{4, "MOTION_BOX_INERTIA"},
	{5, "MOTION_STABILIZED_BOX_INERTIA"},
	{6, "MOTION_KEYFRAMED"},
	{7, "MOTION_FIXED"},
	{8, "MOTION_THIN_BOX_INERTIA"},
	{9, "MOTION_MAX_ID"},
};
static const hkInternalClassEnum hkMotionEnums[] = {
	{"MotionType", hkMotionMotionTypeEnumItems_320, 10 }
};
extern const hkClassEnum* hkMotionMotionTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkMotionEnums[0]);
static hkInternalClassMember hkMotionClass_Members[] =
{
	{ "solverData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMotionClass_320(
	"hkMotion",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkMotionEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkMotionClass_Members),
	int(sizeof(hkMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBoxMotionClass_Members[] =
{
	{ "inertiaAndMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoxMotionClass_320(
	"hkBoxMotion",
	&hkRigidMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBoxMotionClass_Members),
	int(sizeof(hkBoxMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkFixedRigidMotionClass_320(
	"hkFixedRigidMotion",
	&hkKeyframedRigidMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkKeyframedRigidMotionClass_Members[] =
{
	{ "savedMotion", &hkRigidMotionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "savedQualityTypeIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkKeyframedRigidMotionClass_320(
	"hkKeyframedRigidMotion",
	&hkRigidMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkKeyframedRigidMotionClass_Members),
	int(sizeof(hkKeyframedRigidMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRigidMotionClass_Members[] =
{
	{ "motionState", &hkMotionStateClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "massInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "particleMinInertiaDiagInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRigidMotionClass_320(
	"hkRigidMotion",
	&hkMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRigidMotionClass_Members),
	int(sizeof(hkRigidMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkSphereMotionClass_320(
	"hkSphereMotion",
	&hkRigidMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkStabilizedBoxMotionClass_320(
	"hkStabilizedBoxMotion",
	&hkBoxMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkStabilizedSphereMotionClass_320(
	"hkStabilizedSphereMotion",
	&hkSphereMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkThinBoxMotionClass_320(
	"hkThinBoxMotion",
	&hkBoxMotionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkAabbPhantomClass_Members[] =
{
	{ "aabb", &hkAabbClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "overlappingCollidables", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkAabbPhantomClass_320(
	"hkAabbPhantom",
	&hkPhantomClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAabbPhantomClass_Members),
	int(sizeof(hkAabbPhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCachingShapePhantomClass_Members[] =
{
	{ "collisionDetails", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkCachingShapePhantomClass_320(
	"hkCachingShapePhantom",
	&hkShapePhantomClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCachingShapePhantomClass_Members),
	int(sizeof(hkCachingShapePhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPhantomClass_Members[] =
{
	{ "overlapListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "phantomListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkPhantomClass_320(
	"hkPhantom",
	&hkWorldObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPhantomClass_Members),
	int(sizeof(hkPhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkShapePhantomClass_Members[] =
{
	{ "motionState", &hkMotionStateClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkShapePhantomClass_320(
	"hkShapePhantom",
	&hkPhantomClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkShapePhantomClass_Members),
	int(sizeof(hkShapePhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSimpleShapePhantomClass_Members[] =
{
	{ "collisionDetails", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkSimpleShapePhantomClass_320(
	"hkSimpleShapePhantom",
	&hkShapePhantomClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSimpleShapePhantomClass_Members),
	int(sizeof(hkSimpleShapePhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkdynamics_world_hkMultiThreadLockhkMultiThreadLockTypesEnumItems_320[] =
{
	{0, "HK_ACCESS_IGNORE"},
	{1, "HK_ACCESS_RO"},
	{2, "HK_ACCESS_RW"},
};
static const hkInternalClassEnum hkdynamics_world_hkMultiThreadLockEnums[] = {
	{"hkMultiThreadLockTypes", hkdynamics_world_hkMultiThreadLockhkMultiThreadLockTypesEnumItems_320, 3 }
};
extern const hkClassEnum* hkMultiThreadLockTypesEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkdynamics_world_hkMultiThreadLockEnums[0]);
static hkInternalClassMember hkMultiThreadLockClass_Members[] =
{
	{ "threadId", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lockCount", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMultiThreadLockClass_320(
	"hkMultiThreadLock",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMultiThreadLockClass_Members),
	int(sizeof(hkMultiThreadLockClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPhysicsSystemClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "actions", &hkActionClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "phantoms", &hkPhantomClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkPhysicsSystemClass_320(
	"hkPhysicsSystem",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPhysicsSystemClass_Members),
	int(sizeof(hkPhysicsSystemClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static const hkInternalClassEnumItem hkWorldCinfoSolverTypeEnumItems_320[] =
{
	{0, "SOLVER_TYPE_INVALID"},
	{1, "SOLVER_TYPE_2ITERS_SOFT"},
	{2, "SOLVER_TYPE_2ITERS_MEDIUM"},
	{3, "SOLVER_TYPE_2ITERS_HARD"},
	{4, "SOLVER_TYPE_4ITERS_SOFT"},
	{5, "SOLVER_TYPE_4ITERS_MEDIUM"},
	{6, "SOLVER_TYPE_4ITERS_HARD"},
	{7, "SOLVER_TYPE_8ITERS_SOFT"},
	{8, "SOLVER_TYPE_8ITERS_MEDIUM"},
	{9, "SOLVER_TYPE_8ITERS_HARD"},
	{10, "SOLVER_TYPE_MAX_ID"},
};
static const hkInternalClassEnumItem hkWorldCinfoSimulationTypeEnumItems_320[] =
{
	{0, "SIMULATION_TYPE_INVALID"},
	{1, "SIMULATION_TYPE_DISCRETE"},
	{2, "SIMULATION_TYPE_ASYNCHRONOUS"},
	{3, "SIMULATION_TYPE_HALFSTEP"},
	{4, "SIMULATION_TYPE_CONTINUOUS"},
	{5, "SIMULATION_TYPE_CONTINUOUS_HALFSTEP"},
	{6, "SIMULATION_TYPE_CONTINUOUS_ONE_THIRD_STEP"},
	{7, "SIMULATION_TYPE_BACKSTEP_SIMPLE"},
	{8, "SIMULATION_TYPE_BACKSTEP_NON_PENETRATING"},
	{9, "SIMULATION_TYPE_MULTITHREADED"},
};
static const hkInternalClassEnumItem hkWorldCinfoContactPointGenerationEnumItems_320[] =
{
	{0, "CONTACT_POINT_ACCEPT_ALWAYS"},
	{1, "CONTACT_POINT_REJECT_DUBIOUS"},
	{2, "CONTACT_POINT_REJECT_MANY"},
};
static const hkInternalClassEnumItem hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_320[] =
{
	{0, "BROADPHASE_BORDER_ASSERT"},
	{1, "BROADPHASE_BORDER_FIX_ENTITY"},
	{2, "BROADPHASE_BORDER_REMOVE_ENTITY"},
	{3, "BROADPHASE_BORDER_DO_NOTHING"},
};
static const hkInternalClassEnum hkWorldCinfoEnums[] = {
	{"SolverType", hkWorldCinfoSolverTypeEnumItems_320, 11 },
	{"SimulationType", hkWorldCinfoSimulationTypeEnumItems_320, 10 },
	{"ContactPointGeneration", hkWorldCinfoContactPointGenerationEnumItems_320, 3 },
	{"BroadPhaseBorderBehaviour", hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_320, 4 }
};
extern const hkClassEnum* hkWorldCinfoSolverTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[0]);
extern const hkClassEnum* hkWorldCinfoSimulationTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[1]);
extern const hkClassEnum* hkWorldCinfoContactPointGenerationEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[2]);
extern const hkClassEnum* hkWorldCinfoBroadPhaseBorderBehaviourEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[3]);
static hkInternalClassMember hkWorldCinfoClass_Members[] =
{
	{ "gravity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseQuerySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactRestingVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseBorderBehaviour", HK_NULL, hkWorldCinfoBroadPhaseBorderBehaviourEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "broadPhaseWorldAabb", &hkAabbClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilter", &hkCollisionFilterClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "expectedMaxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "expectedMinPsiDeltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "memoryWatchDog", &hkWorldMemoryWatchDogClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "broadPhaseNumMarkers", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactPointGeneration", HK_NULL, hkWorldCinfoContactPointGenerationEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "solverTau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "solverDamp", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "solverIterations", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "unusedPadding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "iterativeLinearCastEarlyOutDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "iterativeLinearCastMaxIterations", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "highFrequencyDeactivationPeriod", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencyDeactivationPeriod", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shouldActivateOnRigidBodyTransformChange", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "toiCollisionResponseRotateNormal", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "enableDeactivation", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "simulationType", HK_NULL, hkWorldCinfoSimulationTypeEnum_320, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "enableSimulationIslands", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "processActionsInSingleThread", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "synchronizeFrameAndPhysicsTime", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkWorldCinfo_DefaultStruct
	{
		int s_defaultOffsets[27];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkMatrix4[16];
		_hkBool m_processActionsInSingleThread;
		_hkBool m_synchronizeFrameAndPhysicsTime;
	};
	const hkWorldCinfo_DefaultStruct hkWorldCinfo_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkWorldCinfo_DefaultStruct,m_processActionsInSingleThread),HK_OFFSET_OF(hkWorldCinfo_DefaultStruct,m_synchronizeFrameAndPhysicsTime)},
		true,true
	};
}
hkClass hkWorldCinfoClass_320(
	"hkWorldCinfo",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkWorldCinfoEnums),
	4, 
	reinterpret_cast<const hkClassMember*>(hkWorldCinfoClass_Members),
	int(sizeof(hkWorldCinfoClass_Members)/sizeof(hkInternalClassMember)),
	&hkWorldCinfo_Default
	);
static const hkInternalClassEnumItem hkWorldObjectBroadPhaseTypeEnumItems_320[] =
{
	{0, "BROAD_PHASE_INVALID"},
	{1, "BROAD_PHASE_ENTITY"},
	{2, "BROAD_PHASE_PHANTOM"},
	{3, "BROAD_PHASE_BORDER"},
	{4, "BROAD_PHASE_MAX_ID"},
};
static const hkInternalClassEnum hkWorldObjectEnums[] = {
	{"BroadPhaseType", hkWorldObjectBroadPhaseTypeEnumItems_320, 5 }
};
extern const hkClassEnum* hkWorldObjectBroadPhaseTypeEnum_320 = reinterpret_cast<const hkClassEnum*>(&hkWorldObjectEnums[0]);
static hkInternalClassMember hkWorldObjectClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "collidable", &hkLinkedCollidableClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "properties", &hkPropertyClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkWorldObjectClass_320(
	"hkWorldObject",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassEnum*>(hkWorldObjectEnums),
	1, 
	reinterpret_cast<const hkClassMember*>(hkWorldObjectClass_Members),
	int(sizeof(hkWorldObjectClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkWorldMemoryWatchDogClass_Members[] =
{
	{ "memoryLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWorldMemoryWatchDogClass_320(
	"hkWorldMemoryWatchDog",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkWorldMemoryWatchDogClass_Members),
	int(sizeof(hkWorldMemoryWatchDogClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkLinkedCollidableClass_Members[] =
{
	{ "collisionEntries", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkLinkedCollidableClass_320(
	"hkLinkedCollidable",
	&hkCollidableClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkLinkedCollidableClass_Members),
	int(sizeof(hkLinkedCollidableClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkBroadPhaseHandleClass_Members[] =
{
	{ "id", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBroadPhaseHandleClass_320(
	"hkBroadPhaseHandle",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkBroadPhaseHandleClass_Members),
	int(sizeof(hkBroadPhaseHandleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConvexPieceStreamDataClass_Members[] =
{
	{ "convexPieceStream", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceOffsets", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceSingleTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};
hkClass hkConvexPieceStreamDataClass_320(
	"hkConvexPieceStreamData",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConvexPieceStreamDataClass_Members),
	int(sizeof(hkConvexPieceStreamDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMoppCode_CodeInfoClass_Members[] =
{
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMoppCodeCodeInfoClass_320(
	"hkMoppCodeCodeInfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMoppCode_CodeInfoClass_Members),
	int(sizeof(hkMoppCode_CodeInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMoppCodeClass_Members[] =
{
	{ "info", &hkMoppCodeCodeInfoClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkMoppCodeClass_320(
	"hkMoppCode",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMoppCodeClass_Members),
	int(sizeof(hkMoppCodeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPoweredRagdollConstraintDataClass_Members[] =
{
	{ "targetFrameAinB", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motorsActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMotor", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "coneMotor", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "planeMotor", &hkConstraintMotorClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPoweredRagdollConstraintDataClass_320(
	"hkPoweredRagdollConstraintData",
	&hkRagdollConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPoweredRagdollConstraintDataClass_Members),
	int(sizeof(hkPoweredRagdollConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRagdollConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollConstraintDataConstraintBasisAClass_320(
	"hkRagdollConstraintDataConstraintBasisA",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkRagdollConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRagdollConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollConstraintDataConstraintBasisBClass_320(
	"hkRagdollConstraintDataConstraintBasisB",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkRagdollConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRagdollConstraintDataClass_Members[] =
{
	{ "anglesAreNotStoredInHavok30Format", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisA", &hkRagdollConstraintDataConstraintBasisAClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkRagdollConstraintDataConstraintBasisBClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkRagdollConstraintData_DefaultStruct
	{
		int s_defaultOffsets[10];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkMatrix4[16];
		hkReal m_angularLimitsTauFactor;
	};
	const hkRagdollConstraintData_DefaultStruct hkRagdollConstraintData_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkRagdollConstraintData_DefaultStruct,m_angularLimitsTauFactor)},
		0.8f
	};
}
hkClass hkRagdollConstraintDataClass_320(
	"hkRagdollConstraintData",
	&hkConstraintDataClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintDataClass_Members),
	int(sizeof(hkRagdollConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	&hkRagdollConstraintData_Default
	);
static hkInternalClassMember hkAngularDashpotActionClass_Members[] =
{
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkAngularDashpotActionClass_320(
	"hkAngularDashpotAction",
	&hkBinaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkAngularDashpotActionClass_Members),
	int(sizeof(hkAngularDashpotActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkDashpotActionClass_Members[] =
{
	{ "point", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "impulse", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkDashpotActionClass_320(
	"hkDashpotAction",
	&hkBinaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDashpotActionClass_Members),
	int(sizeof(hkDashpotActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMotorActionClass_Members[] =
{
	{ "axis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinRate", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "gain", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "active", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMotorActionClass_320(
	"hkMotorAction",
	&hkUnaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMotorActionClass_Members),
	int(sizeof(hkMotorActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkReorientActionClass_Members[] =
{
	{ "rotationAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "upAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkReorientActionClass_320(
	"hkReorientAction",
	&hkUnaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkReorientActionClass_Members),
	int(sizeof(hkReorientActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSpringActionClass_Members[] =
{
	{ "lastForce", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "positionAinA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "positionBinB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "onCompression", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "onExtension", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpringActionClass_320(
	"hkSpringAction",
	&hkBinaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSpringActionClass_Members),
	int(sizeof(hkSpringActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkCharacterProxyCinfoClass_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dynamicFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "staticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "keepContactTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraUpStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraDownStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shapePhantom", &hkShapePhantomClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "keepDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "userPlanes", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxCharacterSpeedForSolver", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterMass", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxSlope", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "penetrationRecoverySpeed", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCharacterProxyCinfoClass_320(
	"hkCharacterProxyCinfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkCharacterProxyCinfoClass_Members),
	int(sizeof(hkCharacterProxyCinfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkDisableEntityCollisionFilterClass_Members[] =
{
	{ "disabledEntities", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkDisableEntityCollisionFilterClass_320(
	"hkDisableEntityCollisionFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDisableEntityCollisionFilterClass_Members),
	int(sizeof(hkDisableEntityCollisionFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkConstrainedSystemFilterClass_Members[] =
{
	{ "otherFilter", &hkCollisionFilterClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkConstrainedSystemFilterClass_320(
	"hkConstrainedSystemFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkConstrainedSystemFilterClass_Members),
	int(sizeof(hkConstrainedSystemFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkGroupCollisionFilterClass_Members[] =
{
	{ "noGroupCollisionEnabled", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionGroups", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 32, 0, 0 }
};
hkClass hkGroupCollisionFilterClass_320(
	"hkGroupCollisionFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkGroupCollisionFilterClass_Members),
	int(sizeof(hkGroupCollisionFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPairwiseCollisionFilter_CollisionPairClass_Members[] =
{
	{ "a", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "b", &hkEntityClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPairwiseCollisionFilterCollisionPairClass_320(
	"hkPairwiseCollisionFilterCollisionPair",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPairwiseCollisionFilter_CollisionPairClass_Members),
	int(sizeof(hkPairwiseCollisionFilter_CollisionPairClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPairwiseCollisionFilterClass_Members[] =
{
	{ "disabledPairs", &hkPairwiseCollisionFilterCollisionPairClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPairwiseCollisionFilterClass_320(
	"hkPairwiseCollisionFilter",
	&hkCollisionFilterClass_320, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPairwiseCollisionFilterClass_Members),
	int(sizeof(hkPairwiseCollisionFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkMouseSpringActionClass_Members[] =
{
	{ "positionInRbLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "mousePositionInWorld", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springElasticity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxRelativeForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMouseSpringActionClass_320(
	"hkMouseSpringAction",
	&hkUnaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkMouseSpringActionClass_Members),
	int(sizeof(hkMouseSpringActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRigidBodyDisplayBindingClass_Members[] =
{
	{ "rigidBody", &hkRigidBodyClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayObject", &hkxMeshClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rigidBodyFromDisplayObjectTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRigidBodyDisplayBindingClass_320(
	"hkRigidBodyDisplayBinding",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRigidBodyDisplayBindingClass_Members),
	int(sizeof(hkRigidBodyDisplayBindingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPhysicsSystemDisplayBindingClass_Members[] =
{
	{ "bindings", &hkRigidBodyDisplayBindingClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "system", &hkPhysicsSystemClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPhysicsSystemDisplayBindingClass_320(
	"hkPhysicsSystemDisplayBinding",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPhysicsSystemDisplayBindingClass_Members),
	int(sizeof(hkPhysicsSystemDisplayBindingClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkDisplayBindingDataClass_Members[] =
{
	{ "rigidBodyBindings", &hkRigidBodyDisplayBindingClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "physicsSystemBindings", &hkPhysicsSystemDisplayBindingClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkDisplayBindingDataClass_320(
	"hkDisplayBindingData",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkDisplayBindingDataClass_Members),
	int(sizeof(hkDisplayBindingDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkPhysicsDataClass_Members[] =
{
	{ "worldCinfo", &hkWorldCinfoClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "systems", &hkPhysicsSystemClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkPhysicsDataClass_320(
	"hkPhysicsData",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkPhysicsDataClass_Members),
	int(sizeof(hkPhysicsDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSerializedDisplayMarkerClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSerializedDisplayMarkerClass_320(
	"hkSerializedDisplayMarker",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayMarkerClass_Members),
	int(sizeof(hkSerializedDisplayMarkerClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSerializedDisplayMarkerListClass_Members[] =
{
	{ "markers", &hkSerializedDisplayMarkerClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkSerializedDisplayMarkerListClass_320(
	"hkSerializedDisplayMarkerList",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayMarkerListClass_Members),
	int(sizeof(hkSerializedDisplayMarkerListClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members[] =
{
	{ "rb", &hkRigidBodyClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "localToDisplay", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_320(
	"hkSerializedDisplayRbTransformsDisplayTransformPair",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members),
	int(sizeof(hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkSerializedDisplayRbTransformsClass_Members[] =
{
	{ "transforms", &hkSerializedDisplayRbTransformsDisplayTransformPairClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkSerializedDisplayRbTransformsClass_320(
	"hkSerializedDisplayRbTransforms",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayRbTransformsClass_Members),
	int(sizeof(hkSerializedDisplayRbTransformsClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleData_WheelComponentParamsClass_Members[] =
{
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "mass", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "width", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "viscosityFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forceFeedbackMultiplier", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxContactBodyAcceleration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDataWheelComponentParamsClass_320(
	"hkVehicleDataWheelComponentParams",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleData_WheelComponentParamsClass_Members),
	int(sizeof(hkVehicleData_WheelComponentParamsClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDataClass_Members[] =
{
	{ "gravity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numWheels", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisOrientation", HK_NULL, HK_NULL, hkClassMember::TYPE_ROTATION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torqueRollFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torquePitchFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torqueYawFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraTorqueFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxVelocityForPositionalFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisUnitInertiaYaw", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisUnitInertiaRoll", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisUnitInertiaPitch", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "frictionEqualizer", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normalClippingAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelParams", &hkVehicleDataWheelComponentParamsClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numWheelsPerAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT8, 0, 0, 0 },
	{ "frictionDescription", &hkVehicleFrictionDescriptionClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisFrictionInertiaInvDiag", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "alreadyInitialised", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDataClass_320(
	"hkVehicleData",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDataClass_Members),
	int(sizeof(hkVehicleDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleInstance_WheelInfoClass_Members[] =
{
	{ "contactPoint", &hkContactPointClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactBody", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "contactShapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "hardPointWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rayEndPointWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "currentSuspensionLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionDirectionWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinAxisCs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinAxisWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "steeringOrientationCs", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "skidEnergyDensity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sideForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardSlipVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sideSlipVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleInstanceWheelInfoClass_320(
	"hkVehicleInstanceWheelInfo",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleInstance_WheelInfoClass_Members),
	int(sizeof(hkVehicleInstance_WheelInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleInstanceClass_Members[] =
{
	{ "data", &hkVehicleDataClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "driverInput", &hkVehicleDriverInputClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "steering", &hkVehicleSteeringClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "engine", &hkVehicleEngineClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transmission", &hkVehicleTransmissionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "brake", &hkVehicleBrakeClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "suspension", &hkVehicleSuspensionClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "aerodynamics", &hkVehicleAerodynamicsClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelCollide", &hkVehicleWheelCollideClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tyreMarks", &hkTyremarksInfoClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chassis", &hkRigidBodyClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "velocityDamper", &hkVehicleVelocityDamperClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsInfo", &hkVehicleInstanceWheelInfoClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "frictionStatus", &hkVehicleFrictionStatusClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deviceStatus", &hkVehicleDriverInputStatusClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "isFixed", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_BOOL, 0, 0, 0 },
	{ "wheelsTimeSinceMaxPedalInput", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tryingToReverse", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rpm", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "mainSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelsSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "isReversing", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "currentGear", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "delayed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clutchDelayCountdown", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleInstanceClass_320(
	"hkVehicleInstance",
	&hkUnaryActionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleInstanceClass_Members),
	int(sizeof(hkVehicleInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleAerodynamicsClass_320(
	"hkVehicleAerodynamics",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultAerodynamicsClass_Members[] =
{
	{ "airDensity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "frontalArea", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dragCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "liftCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraGravityws", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultAerodynamicsClass_320(
	"hkVehicleDefaultAerodynamics",
	&hkVehicleAerodynamicsClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAerodynamicsClass_Members),
	int(sizeof(hkVehicleDefaultAerodynamicsClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleBrakeClass_320(
	"hkVehicleBrake",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultBrake_WheelBrakingPropertiesClass_Members[] =
{
	{ "maxBreakingTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minPedalInputToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isConnectedToHandbrake", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_320(
	"hkVehicleDefaultBrakeWheelBrakingProperties",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultBrake_WheelBrakingPropertiesClass_Members),
	int(sizeof(hkVehicleDefaultBrake_WheelBrakingPropertiesClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultBrakeClass_Members[] =
{
	{ "wheelBrakingProperties", &hkVehicleDefaultBrakeWheelBrakingPropertiesClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsMinTimeToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultBrakeClass_320(
	"hkVehicleDefaultBrake",
	&hkVehicleBrakeClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultBrakeClass_Members),
	int(sizeof(hkVehicleDefaultBrakeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleDriverInputStatusClass_320(
	"hkVehicleDriverInputStatus",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
hkClass hkVehicleDriverInputClass_320(
	"hkVehicleDriverInput",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDriverInputAnalogStatusClass_Members[] =
{
	{ "positionX", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "positionY", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "handbrakeButtonPressed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "reverseButtonPressed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDriverInputAnalogStatusClass_320(
	"hkVehicleDriverInputAnalogStatus",
	&hkVehicleDriverInputStatusClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDriverInputAnalogStatusClass_Members),
	int(sizeof(hkVehicleDriverInputAnalogStatusClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultAnalogDriverInputClass_Members[] =
{
	{ "slopeChangePointX", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "initialSlope", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deadZone", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "autoReverse", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultAnalogDriverInputClass_320(
	"hkVehicleDefaultAnalogDriverInput",
	&hkVehicleDriverInputClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAnalogDriverInputClass_Members),
	int(sizeof(hkVehicleDefaultAnalogDriverInputClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleEngineClass_320(
	"hkVehicleEngine",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultEngineClass_Members[] =
{
	{ "minRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "optRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torqueFactorAtMinRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "torqueFactorAtMaxRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "resistanceFactorAtMinRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "resistanceFactorAtOptRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "resistanceFactorAtMaxRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clutchSlipRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultEngineClass_320(
	"hkVehicleDefaultEngine",
	&hkVehicleEngineClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultEngineClass_Members),
	int(sizeof(hkVehicleDefaultEngineClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleSteeringClass_320(
	"hkVehicleSteering",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultSteeringClass_Members[] =
{
	{ "maxSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxSpeedFullSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "doesWheelSteer", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_BOOL, 0, 0, 0 }
};
hkClass hkVehicleDefaultSteeringClass_320(
	"hkVehicleDefaultSteering",
	&hkVehicleSteeringClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSteeringClass_Members),
	int(sizeof(hkVehicleDefaultSteeringClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleSuspension_SuspensionWheelParametersClass_Members[] =
{
	{ "hardpointCs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "directionCs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleSuspensionSuspensionWheelParametersClass_320(
	"hkVehicleSuspensionSuspensionWheelParameters",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleSuspension_SuspensionWheelParametersClass_Members),
	int(sizeof(hkVehicleSuspension_SuspensionWheelParametersClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleSuspensionClass_Members[] =
{
	{ "wheelParams", &hkVehicleSuspensionSuspensionWheelParametersClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVehicleSuspensionClass_320(
	"hkVehicleSuspension",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleSuspensionClass_Members),
	int(sizeof(hkVehicleSuspensionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultSuspension_WheelSpringSuspensionParametersClass_Members[] =
{
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dampingCompression", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dampingRelaxation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_320(
	"hkVehicleDefaultSuspensionWheelSpringSuspensionParameters",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSuspension_WheelSpringSuspensionParametersClass_Members),
	int(sizeof(hkVehicleDefaultSuspension_WheelSpringSuspensionParametersClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultSuspensionClass_Members[] =
{
	{ "wheelSpringParams", &hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVehicleDefaultSuspensionClass_320(
	"hkVehicleDefaultSuspension",
	&hkVehicleSuspensionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSuspensionClass_Members),
	int(sizeof(hkVehicleDefaultSuspensionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleTransmissionClass_320(
	"hkVehicleTransmission",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultTransmissionClass_Members[] =
{
	{ "downshiftRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "upshiftRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "primaryTransmissionRatio", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clutchDelayTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "reverseGearRatio", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "gearsRatio", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "wheelsTorqueRatio", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkVehicleDefaultTransmissionClass_320(
	"hkVehicleDefaultTransmission",
	&hkVehicleTransmissionClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultTransmissionClass_Members),
	int(sizeof(hkVehicleDefaultTransmissionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTyremarkPointClass_Members[] =
{
	{ "pointLeft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pointRight", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTyremarkPointClass_320(
	"hkTyremarkPoint",
	HK_NULL, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTyremarkPointClass_Members),
	int(sizeof(hkTyremarkPointClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTyremarksWheelClass_Members[] =
{
	{ "currentPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numPoints", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tyremarkPoints", &hkTyremarkPointClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkTyremarksWheelClass_320(
	"hkTyremarksWheel",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTyremarksWheelClass_Members),
	int(sizeof(hkTyremarksWheelClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkTyremarksInfoClass_Members[] =
{
	{ "minTyremarkEnergy", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxTyremarkEnergy", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tyremarksWheel", &hkTyremarksWheelClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkTyremarksInfoClass_320(
	"hkTyremarksInfo",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkTyremarksInfoClass_Members),
	int(sizeof(hkTyremarksInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
hkClass hkVehicleVelocityDamperClass_320(
	"hkVehicleVelocityDamper",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	HK_NULL,
	0,
	HK_NULL 
	);
static hkInternalClassMember hkVehicleDefaultVelocityDamperClass_Members[] =
{
	{ "normalSpinDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionSpinDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionThreshold", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultVelocityDamperClass_320(
	"hkVehicleDefaultVelocityDamper",
	&hkVehicleVelocityDamperClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultVelocityDamperClass_Members),
	int(sizeof(hkVehicleDefaultVelocityDamperClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleWheelCollideClass_Members[] =
{
	{ "alreadyUsed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleWheelCollideClass_320(
	"hkVehicleWheelCollide",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleWheelCollideClass_Members),
	int(sizeof(hkVehicleWheelCollideClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkRejectRayChassisListenerClass_Members[] =
{
	{ "chassis", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkRejectRayChassisListenerClass_320(
	"hkRejectRayChassisListener",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	1, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRejectRayChassisListenerClass_Members),
	int(sizeof(hkRejectRayChassisListenerClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
static hkInternalClassMember hkVehicleRaycastWheelCollideClass_Members[] =
{
	{ "wheelCollisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "phantom", &hkAabbPhantomClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rejectRayChassisListener", &hkRejectRayChassisListenerClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleRaycastWheelCollideClass_320(
	"hkVehicleRaycastWheelCollide",
	&hkVehicleWheelCollideClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkVehicleRaycastWheelCollideClass_Members),
	int(sizeof(hkVehicleRaycastWheelCollideClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);
//XXX
static hkInternalClassMember hkVehicleFrictionDescription_AxisDescriptionClass_Members_320[] =
{
	{ "frictionCircleYtab", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 16, 0, 0 },
	{ "xStep", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "xStart", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelSurfaceInertia", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelSurfaceInertiaInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelChassisMassRatio", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelRadiusInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelDownForceFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelDownForceSumFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_320(
	"hkVehicleFrictionDescriptionAxisDescription",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<hkClassMember*>(hkVehicleFrictionDescription_AxisDescriptionClass_Members_320),
	int(sizeof(hkVehicleFrictionDescription_AxisDescriptionClass_Members_320)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionDescriptionClass_Members_320[] =
{
	{ "wheelDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axleDescr", &hkVehicleFrictionDescriptionAxisDescriptionClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionDescriptionClass_320(
	"hkVehicleFrictionDescription",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<hkClassMember*>(hkVehicleFrictionDescriptionClass_Members_320),
	int(sizeof(hkVehicleFrictionDescriptionClass_Members_320)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionStatus_AxisStatusClass_Members_320[] =
{
	{ "forward_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "skid_energy_density", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_force", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "delayed_forward_impulse", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sideRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "relativeSideForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "relativeForwardForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleFrictionStatusAxisStatusClass_320(
	"hkVehicleFrictionStatusAxisStatus",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<hkClassMember*>(hkVehicleFrictionStatus_AxisStatusClass_Members_320),
	int(sizeof(hkVehicleFrictionStatus_AxisStatusClass_Members_320)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionStatusClass_Members_320[] =
{
	{ "axis", &hkVehicleFrictionStatusAxisStatusClass_320, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionStatusClass_320(
	"hkVehicleFrictionStatus",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<hkClassMember*>(hkVehicleFrictionStatusClass_Members_320),
	int(sizeof(hkVehicleFrictionStatusClass_Members_320)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
//XXX
static hkInternalClassMember hkRagdollInstanceClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_320, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_320, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkRagdollInstanceClass_320(
	"hkRagdollInstance",
	&hkReferencedObjectClass_320, 
	0,
	HK_NULL,
	0, 
	HK_NULL,
	0, 
	reinterpret_cast<const hkClassMember*>(hkRagdollInstanceClass_Members),
	int(sizeof(hkRagdollInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL 
	);

extern hkClass* const hkHavok320Classes[];
hkClass* const hkHavok320Classes[] =
{
	&hkAabbClass_320,
	&hkAabbPhantomClass_320,
	&hkActionClass_320,
	&hkAngularDashpotActionClass_320,
	&hkAnimatedReferenceFrameClass_320,
	&hkAnimationBindingClass_320,
	&hkAnimationContainerClass_320,
	&hkAnnotationTrackAnnotationClass_320,
	&hkAnnotationTrackClass_320,
	&hkArrayActionClass_320,
	&hkBallAndSocketConstraintDataClass_320,
	&hkBaseObjectClass_320,
	&hkBinaryActionClass_320,
	&hkBoneAttachmentClass_320,
	&hkBoneClass_320,
	&hkBoxMotionClass_320,
	&hkBoxShapeClass_320,
	&hkBreakableConstraintDataClass_320,
	&hkBroadPhaseHandleClass_320,
	&hkBvShapeClass_320,
	&hkBvTreeShapeClass_320,
	&hkCachingShapePhantomClass_320,
	&hkCapsuleShapeClass_320,
	&hkCdBodyClass_320,
	&hkCharacterProxyCinfoClass_320,
	&hkClassClass_320,
	&hkClassEnumClass_320,
	&hkClassEnumItemClass_320,
	&hkClassMemberClass_320,
	&hkCollidableClass_320,
	&hkCollidableCollidableFilterClass_320,
	&hkCollisionFilterClass_320,
	&hkCollisionFilterListClass_320,
	&hkConstrainedSystemFilterClass_320,
	&hkConstraintDataClass_320,
	&hkConstraintInfoClass_320,
	&hkConstraintInstanceClass_320,
	&hkConstraintMotorClass_320,
	&hkContactPointClass_320,
	&hkContactPointMaterialClass_320,
	&hkConvexListShapeClass_320,
	&hkConvexPieceMeshShapeClass_320,
	&hkConvexPieceStreamDataClass_320,
	&hkConvexShapeClass_320,
	&hkConvexTransformShapeClass_320,
	&hkConvexTranslateShapeClass_320,
	&hkConvexVerticesShapeClass_320,
	&hkConvexVerticesShapeFourVectorsClass_320,
	&hkCylinderShapeClass_320,
	&hkDashpotActionClass_320,
	&hkDefaultAnimatedReferenceFrameClass_320,
	&hkDeltaCompressedSkeletalAnimationClass_320,
	&hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_320,
	&hkDisableEntityCollisionFilterClass_320,
	&hkDisplayBindingDataClass_320,
	&hkEntityClass_320,
	&hkEntityDeactivatorClass_320,
	&hkFakeRigidBodyDeactivatorClass_320,
	&hkFastMeshShapeClass_320,
	&hkFixedRigidMotionClass_320,
	&hkGenericConstraintDataClass_320,
	&hkGenericConstraintDataSchemeClass_320,
	&hkGroupCollisionFilterClass_320,
	&hkGroupFilterClass_320,
	&hkHeightFieldShapeClass_320,
	&hkHingeConstraintDataClass_320,
	&hkHingeConstraintDataConstraintBasisAClass_320,
	&hkHingeConstraintDataConstraintBasisBClass_320,
	&hkInterleavedSkeletalAnimationClass_320,
	&hkKeyframedRigidMotionClass_320,
	&hkLimitedHingeConstraintDataClass_320,
	&hkLimitedHingeConstraintDataConstraintBasisAClass_320,
	&hkLimitedHingeConstraintDataConstraintBasisBClass_320,
	&hkLinearParametricCurveClass_320,
	&hkLinkedCollidableClass_320,
	&hkListShapeChildInfoClass_320,
	&hkListShapeClass_320,
	&hkMalleableConstraintDataClass_320,
	&hkMaterialClass_320,
	&hkMeshBindingClass_320,
	&hkMeshBindingMappingClass_320,
	&hkMeshMaterialClass_320,
	&hkMeshShapeClass_320,
	&hkMeshShapeSubpartClass_320,
	&hkMonitorStreamFrameInfoClass_320,
	&hkMonitorStreamStringMapClass_320,
	&hkMonitorStreamStringMapStringMapClass_320,
	&hkMoppBvTreeShapeClass_320,
	&hkMoppCodeClass_320,
	&hkMoppCodeCodeInfoClass_320,
	&hkMotionClass_320,
	&hkMotionStateClass_320,
	&hkMotorActionClass_320,
	&hkMouseSpringActionClass_320,
	&hkMultiRayShapeClass_320,
	&hkMultiRayShapeRayClass_320,
	&hkMultiSphereShapeClass_320,
	&hkMultiThreadLockClass_320,
	&hkNullCollisionFilterClass_320,
	&hkPackfileHeaderClass_320,
	&hkPackfileSectionHeaderClass_320,
	&hkPairwiseCollisionFilterClass_320,
	&hkPairwiseCollisionFilterCollisionPairClass_320,
	&hkParametricCurveClass_320,
	&hkPhantomCallbackShapeClass_320,
	&hkPhantomClass_320,
	&hkPhysicsDataClass_320,
	&hkPhysicsSystemClass_320,
	&hkPhysicsSystemDisplayBindingClass_320,
	&hkPlaneShapeClass_320,
	&hkPointToPathConstraintDataClass_320,
	&hkPointToPlaneConstraintDataClass_320,
	&hkPositionConstraintMotorClass_320,
	&hkPoweredHingeConstraintDataClass_320,
	&hkPoweredRagdollConstraintDataClass_320,
	&hkPrismaticConstraintDataClass_320,
	&hkPrismaticConstraintDataConstraintBasisAClass_320,
	&hkPrismaticConstraintDataConstraintBasisBClass_320,
	&hkPropertyClass_320,
	&hkPropertyValueClass_320,
	&hkRagdollConstraintDataClass_320,
	&hkRagdollConstraintDataConstraintBasisAClass_320,
	&hkRagdollConstraintDataConstraintBasisBClass_320,
	&hkRagdollInstanceClass_320,
	&hkRayCollidableFilterClass_320,
	&hkRayShapeCollectionFilterClass_320,
	&hkReferencedObjectClass_320,
	&hkRejectRayChassisListenerClass_320,
	&hkReorientActionClass_320,
	&hkRigidBodyClass_320,
	&hkRigidBodyDeactivatorClass_320,
	&hkRigidBodyDisplayBindingClass_320,
	&hkRigidMotionClass_320,
	&hkRootLevelContainerClass_320,
	&hkRootLevelContainerNamedVariantClass_320,
	&hkSampledHeightFieldShapeClass_320,
	&hkSerializedDisplayMarkerClass_320,
	&hkSerializedDisplayMarkerListClass_320,
	&hkSerializedDisplayRbTransformsClass_320,
	&hkSerializedDisplayRbTransformsDisplayTransformPairClass_320,
	&hkShapeClass_320,
	&hkShapeCollectionClass_320,
	&hkShapeCollectionFilterClass_320,
	&hkShapePhantomClass_320,
	&hkShapeRayCastInputClass_320,
	&hkSimpleMeshShapeClass_320,
	&hkSimpleMeshShapeTriangleClass_320,
	&hkSimpleShapePhantomClass_320,
	&hkSkeletalAnimationClass_320,
	&hkSkeletonClass_320,
	&hkSkeletonMapperClass_320,
	&hkSkeletonMapperDataChainMappingClass_320,
	&hkSkeletonMapperDataClass_320,
	&hkSkeletonMapperDataSimpleMappingClass_320,
	&hkSpatialRigidBodyDeactivatorClass_320,
	&hkSpatialRigidBodyDeactivatorSampleClass_320,
	&hkSphereClass_320,
	&hkSphereMotionClass_320,
	&hkSphereRepShapeClass_320,
	&hkSphereShapeClass_320,
	&hkSpringActionClass_320,
	&hkSpringDamperConstraintMotorClass_320,
	&hkStabilizedBoxMotionClass_320,
	&hkStabilizedSphereMotionClass_320,
	&hkStiffSpringConstraintDataClass_320,
	&hkStorageMeshShapeClass_320,
	&hkStorageMeshShapeSubpartStorageClass_320,
	&hkStorageSampledHeightFieldShapeClass_320,
	&hkSweptTransformClass_320,
	&hkThinBoxMotionClass_320,
	&hkTransformShapeClass_320,
	&hkTriPatchTriangleClass_320,
	&hkTriSampledHeightFieldBvTreeShapeClass_320,
	&hkTriSampledHeightFieldCollectionClass_320,
	&hkTriangleShapeClass_320,
	&hkTypedBroadPhaseHandleClass_320,
	&hkTyremarkPointClass_320,
	&hkTyremarksInfoClass_320,
	&hkTyremarksWheelClass_320,
	&hkUnaryActionClass_320,
	&hkVehicleAerodynamicsClass_320,
	&hkVehicleBrakeClass_320,
	&hkVehicleDataClass_320,
	&hkVehicleDataWheelComponentParamsClass_320,
	&hkVehicleDefaultAerodynamicsClass_320,
	&hkVehicleDefaultAnalogDriverInputClass_320,
	&hkVehicleDefaultBrakeClass_320,
	&hkVehicleDefaultBrakeWheelBrakingPropertiesClass_320,
	&hkVehicleDefaultEngineClass_320,
	&hkVehicleDefaultSteeringClass_320,
	&hkVehicleDefaultSuspensionClass_320,
	&hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_320,
	&hkVehicleDefaultTransmissionClass_320,
	&hkVehicleDefaultVelocityDamperClass_320,
	&hkVehicleDriverInputAnalogStatusClass_320,
	&hkVehicleDriverInputClass_320,
	&hkVehicleDriverInputStatusClass_320,
	&hkVehicleEngineClass_320,
	&hkVehicleInstanceClass_320,
	&hkVehicleInstanceWheelInfoClass_320,
	&hkVehicleRaycastWheelCollideClass_320,
	&hkVehicleSteeringClass_320,
	&hkVehicleSuspensionClass_320,
	&hkVehicleSuspensionSuspensionWheelParametersClass_320,
	&hkVehicleTransmissionClass_320,
	&hkVehicleVelocityDamperClass_320,
	&hkVehicleWheelCollideClass_320,
	//XXX
	&hkVehicleFrictionDescriptionAxisDescriptionClass_320,
	&hkVehicleFrictionDescriptionClass_320,
	&hkVehicleFrictionStatusAxisStatusClass_320,
	&hkVehicleFrictionStatusClass_320,
	//XXX
	&hkVelocityConstraintMotorClass_320,
	&hkVersioningExceptionsArrayClass_320,
	&hkVersioningExceptionsArrayVersioningExceptionClass_320,
	&hkWaveletSkeletalAnimationClass_320,
	&hkWaveletSkeletalAnimationQuantizationFormatClass_320,
	&hkWheelConstraintDataClass_320,
	&hkWheelConstraintDataConstraintBasisAClass_320,
	&hkWheelConstraintDataConstraintBasisBClass_320,
	&hkWorldCinfoClass_320,
	&hkWorldMemoryWatchDogClass_320,
	&hkWorldObjectClass_320,
	&hkxAnimatedFloatClass_320,
	&hkxAnimatedMatrixClass_320,
	&hkxAnimatedQuaternionClass_320,
	&hkxAnimatedVectorClass_320,
	&hkxAttributeClass_320,
	&hkxAttributeGroupClass_320,
	&hkxCameraClass_320,
	&hkxIndexBufferClass_320,
	&hkxLightClass_320,
	&hkxMaterialClass_320,
	&hkxMaterialEffectClass_320,
	&hkxMaterialTextureStageClass_320,
	&hkxMeshClass_320,
	&hkxMeshSectionClass_320,
	&hkxNodeAnnotationDataClass_320,
	&hkxNodeClass_320,
	&hkxSceneClass_320,
	&hkxSkinBindingClass_320,
	&hkxSparselyAnimatedBoolClass_320,
	&hkxSparselyAnimatedEnumClass_320,
	&hkxSparselyAnimatedIntClass_320,
	&hkxSparselyAnimatedStringClass_320,
	&hkxSparselyAnimatedStringStringTypeClass_320,
	&hkxTextureFileClass_320,
	&hkxTextureInplaceClass_320,
	&hkxVertexBufferClass_320,
	&hkxVertexFormatClass_320,
	&hkxVertexP4N4C1T2Class_320,
	&hkxVertexP4N4T4B4C1T2Class_320,
	&hkxVertexP4N4T4B4W4I4C1Q2Class_320,
	&hkxVertexP4N4T4B4W4I4Q4Class_320,
	&hkxVertexP4N4W4I4C1Q2Class_320,
	HK_NULL
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
