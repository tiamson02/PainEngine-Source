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


extern hkClass hkBaseObjectClass_330b1;
extern hkClass hkReferencedObjectClass_330b1;
extern hkClass hkClassClass_330b1;
extern hkClass hkClassEnumItemClass_330b1;
extern hkClass hkClassEnumClass_330b1;
extern hkClass hkClassMemberClass_330b1;
extern hkClass hkMonitorStreamStringMapStringMapClass_330b1;
extern hkClass hkMonitorStreamStringMapClass_330b1;
extern hkClass hkMonitorStreamFrameInfoClass_330b1;
extern hkClass hkAabbClass_330b1;
extern hkClass hkContactPointClass_330b1;
extern hkClass hkContactPointMaterialClass_330b1;
extern hkClass hkMotionStateClass_330b1;
extern hkClass hkSphereClass_330b1;
extern hkClass hkSweptTransformClass_330b1;
extern hkClass hkxAnimatedFloatClass_330b1;
extern hkClass hkxAnimatedMatrixClass_330b1;
extern hkClass hkxAnimatedQuaternionClass_330b1;
extern hkClass hkxAnimatedVectorClass_330b1;
extern hkClass hkxAttributeClass_330b1;
extern hkClass hkxAttributeGroupClass_330b1;
extern hkClass hkxSparselyAnimatedBoolClass_330b1;
extern hkClass hkxSparselyAnimatedEnumClass_330b1;
extern hkClass hkxSparselyAnimatedIntClass_330b1;
extern hkClass hkxSparselyAnimatedStringStringTypeClass_330b1;
extern hkClass hkxSparselyAnimatedStringClass_330b1;
extern hkClass hkxCameraClass_330b1;
extern hkClass hkxNodeAnnotationDataClass_330b1;
extern hkClass hkxNodeClass_330b1;
extern hkClass hkxLightClass_330b1;
extern hkClass hkxMaterialTextureStageClass_330b1;
extern hkClass hkxMaterialClass_330b1;
extern hkClass hkxMaterialEffectClass_330b1;
extern hkClass hkxTextureFileClass_330b1;
extern hkClass hkxTextureInplaceClass_330b1;
extern hkClass hkxIndexBufferClass_330b1;
extern hkClass hkxMeshClass_330b1;
extern hkClass hkxMeshSectionClass_330b1;
extern hkClass hkxVertexBufferClass_330b1;
extern hkClass hkxVertexFormatClass_330b1;
extern hkClass hkxVertexP4N4C1T2Class_330b1;
extern hkClass hkxVertexP4N4T4B4C1T2Class_330b1;
extern hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_330b1;
extern hkClass hkxVertexP4N4T4B4W4I4Q4Class_330b1;
extern hkClass hkxVertexP4N4W4I4C1Q2Class_330b1;
extern hkClass hkxSceneClass_330b1;
extern hkClass hkxSkinBindingClass_330b1;
extern hkClass hkPackfileHeaderClass_330b1;
extern hkClass hkPackfileSectionHeaderClass_330b1;
extern hkClass hkRootLevelContainerNamedVariantClass_330b1;
extern hkClass hkRootLevelContainerClass_330b1;
extern hkClass hkVersioningExceptionsArrayVersioningExceptionClass_330b1;
extern hkClass hkVersioningExceptionsArrayClass_330b1;
extern hkClass hkAnimationContainerClass_330b1;
extern hkClass hkAnimationBindingClass_330b1;
extern hkClass hkAnnotationTrackAnnotationClass_330b1;
extern hkClass hkAnnotationTrackClass_330b1;
extern hkClass hkSkeletalAnimationClass_330b1;
extern hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_330b1;
extern hkClass hkDeltaCompressedSkeletalAnimationClass_330b1;
extern hkClass hkInterleavedSkeletalAnimationClass_330b1;
extern hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_330b1;
extern hkClass hkWaveletSkeletalAnimationClass_330b1;
extern hkClass hkMeshBindingMappingClass_330b1;
extern hkClass hkMeshBindingClass_330b1;
extern hkClass hkSkeletonMapperClass_330b1;
extern hkClass hkSkeletonMapperDataSimpleMappingClass_330b1;
extern hkClass hkSkeletonMapperDataChainMappingClass_330b1;
extern hkClass hkSkeletonMapperDataClass_330b1;
extern hkClass hkAnimatedReferenceFrameClass_330b1;
extern hkClass hkDefaultAnimatedReferenceFrameClass_330b1;
extern hkClass hkBoneClass_330b1;
extern hkClass hkBoneAttachmentClass_330b1;
extern hkClass hkSkeletonClass_330b1;
extern hkClass hkCdBodyClass_330b1;
extern hkClass hkCollidableClass_330b1;
extern hkClass hkShapeCollectionFilterClass_330b1;
extern hkClass hkTypedBroadPhaseHandleClass_330b1;
extern hkClass hkCollidableCollidableFilterClass_330b1;
extern hkClass hkCollisionFilterClass_330b1;
extern hkClass hkRayCollidableFilterClass_330b1;
extern hkClass hkGroupFilterClass_330b1;
extern hkClass hkCollisionFilterListClass_330b1;
extern hkClass hkNullCollisionFilterClass_330b1;
extern hkClass hkRayShapeCollectionFilterClass_330b1;
extern hkClass hkShapeClass_330b1;
extern hkClass hkShapeRayCastInputClass_330b1;
extern hkClass hkBoxShapeClass_330b1;
extern hkClass hkBvShapeClass_330b1;
extern hkClass hkBvTreeShapeClass_330b1;
extern hkClass hkCapsuleShapeClass_330b1;
extern hkClass hkShapeCollectionClass_330b1;
extern hkClass hkConvexShapeClass_330b1;
extern hkClass hkConvexListShapeClass_330b1;
extern hkClass hkConvexPieceMeshShapeClass_330b1;
extern hkClass hkConvexTransformShapeClass_330b1;
extern hkClass hkConvexTranslateShapeClass_330b1;
extern hkClass hkConvexVerticesShapeFourVectorsClass_330b1;
extern hkClass hkConvexVerticesShapeClass_330b1;
extern hkClass hkCylinderShapeClass_330b1;
extern hkClass hkFastMeshShapeClass_330b1;
extern hkClass hkHeightFieldShapeClass_330b1;
extern hkClass hkListShapeChildInfoClass_330b1;
extern hkClass hkListShapeClass_330b1;
extern hkClass hkMeshMaterialClass_330b1;
extern hkClass hkMeshShapeSubpartClass_330b1;
extern hkClass hkMeshShapeClass_330b1;
extern hkClass hkMoppBvTreeShapeClass_330b1;
extern hkClass hkMultiRayShapeRayClass_330b1;
extern hkClass hkMultiRayShapeClass_330b1;
extern hkClass hkMultiSphereShapeClass_330b1;
extern hkClass hkPhantomCallbackShapeClass_330b1;
extern hkClass hkPlaneShapeClass_330b1;
extern hkClass hkSampledHeightFieldShapeClass_330b1;
extern hkClass hkSimpleMeshShapeTriangleClass_330b1;
extern hkClass hkSimpleMeshShapeClass_330b1;
extern hkClass hkSphereShapeClass_330b1;
extern hkClass hkSphereRepShapeClass_330b1;
extern hkClass hkStorageMeshShapeSubpartStorageClass_330b1;
extern hkClass hkStorageMeshShapeClass_330b1;
extern hkClass hkStorageSampledHeightFieldShapeClass_330b1;
extern hkClass hkTransformShapeClass_330b1;
extern hkClass hkTriangleShapeClass_330b1;
extern hkClass hkTriSampledHeightFieldBvTreeShapeClass_330b1;
extern hkClass hkTriSampledHeightFieldCollectionClass_330b1;
extern hkClass hkActionClass_330b1;
extern hkClass hkArrayActionClass_330b1;
extern hkClass hkBinaryActionClass_330b1;
extern hkClass hkUnaryActionClass_330b1;
extern hkClass hkMaterialClass_330b1;
extern hkClass hkPropertyValueClass_330b1;
extern hkClass hkPropertyClass_330b1;
extern hkClass hkConstraintDataClass_330b1;
extern hkClass hkConstraintInfoClass_330b1;
extern hkClass hkConstraintInstanceClass_330b1;
extern hkClass hkBallAndSocketConstraintDataClass_330b1;
extern hkClass hkRelativeOrientationConstraintDataClass_330b1;
extern hkClass hkHingeConstraintDataConstraintBasisAClass_330b1;
extern hkClass hkHingeConstraintDataConstraintBasisBClass_330b1;
extern hkClass hkHingeConstraintDataClass_330b1;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_330b1;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_330b1;
extern hkClass hkLimitedHingeConstraintDataClass_330b1;
extern hkClass hkLinearParametricCurveClass_330b1;
extern hkClass hkParametricCurveClass_330b1;
extern hkClass hkPointToPathConstraintDataClass_330b1;
extern hkClass hkPointToPlaneConstraintDataClass_330b1;
extern hkClass hkPoweredHingeConstraintDataClass_330b1;
extern hkClass hkPrismaticConstraintDataConstraintBasisAClass_330b1;
extern hkClass hkPrismaticConstraintDataConstraintBasisBClass_330b1;
extern hkClass hkPrismaticConstraintDataClass_330b1;
extern hkClass hkStiffSpringConstraintDataClass_330b1;
extern hkClass hkWheelConstraintDataConstraintBasisAClass_330b1;
extern hkClass hkWheelConstraintDataConstraintBasisBClass_330b1;
extern hkClass hkWheelConstraintDataClass_330b1;
extern hkClass hkBreakableConstraintDataClass_330b1;
extern hkClass hkConstraintChainDataClass_330b1;
extern hkClass hkConstraintChainInstanceClass_330b1;
extern hkClass hkConstraintChainInstanceActionClass_330b1;
extern hkClass hkBallSocketChainDataConstraintInfoClass_330b1;
extern hkClass hkBallSocketChainDataClass_330b1;
extern hkClass hkHingeLimitsDataConstraintBasisAClass_330b1;
extern hkClass hkHingeLimitsDataConstraintBasisBClass_330b1;
extern hkClass hkHingeLimitsDataClass_330b1;
extern hkClass hkPoweredChainDataConstraintInfoClass_330b1;
extern hkClass hkPoweredChainDataClass_330b1;
extern hkClass hkStiffSpringChainDataConstraintInfoClass_330b1;
extern hkClass hkStiffSpringChainDataClass_330b1;
extern hkClass hkGenericConstraintDataClass_330b1;
extern hkClass hkGenericConstraintDataSchemeClass_330b1;
extern hkClass hkMalleableConstraintDataClass_330b1;
extern hkClass hkConstraintMotorClass_330b1;
extern hkClass hkLimitedForceConstraintMotorClass_330b1;
extern hkClass hkPositionConstraintMotorClass_330b1;
extern hkClass hkSpringDamperConstraintMotorClass_330b1;
extern hkClass hkVelocityConstraintMotorClass_330b1;
extern hkClass hkPulleyConstraintDataClass_330b1;
extern hkClass hkEntityClass_330b1;
extern hkClass hkEntityDeactivatorClass_330b1;
extern hkClass hkFakeRigidBodyDeactivatorClass_330b1;
extern hkClass hkRigidBodyClass_330b1;
extern hkClass hkRigidBodyDeactivatorClass_330b1;
extern hkClass hkSpatialRigidBodyDeactivatorSampleClass_330b1;
extern hkClass hkSpatialRigidBodyDeactivatorClass_330b1;
extern hkClass hkMotionClass_330b1;
extern hkClass hkBoxMotionClass_330b1;
extern hkClass hkFixedRigidMotionClass_330b1;
extern hkClass hkKeyframedRigidMotionClass_330b1;
extern hkClass hkRigidMotionClass_330b1;
extern hkClass hkSphereMotionClass_330b1;
extern hkClass hkStabilizedBoxMotionClass_330b1;
extern hkClass hkStabilizedSphereMotionClass_330b1;
extern hkClass hkThinBoxMotionClass_330b1;
extern hkClass hkAabbPhantomClass_330b1;
extern hkClass hkCachingShapePhantomClass_330b1;
extern hkClass hkPhantomClass_330b1;
extern hkClass hkShapePhantomClass_330b1;
extern hkClass hkSimpleShapePhantomClass_330b1;
extern hkClass hkMultiThreadLockClass_330b1;
extern hkClass hkPhysicsSystemClass_330b1;
extern hkClass hkWorldCinfoClass_330b1;
extern hkClass hkWorldObjectClass_330b1;
extern hkClass hkWorldMemoryWatchDogClass_330b1;
extern hkClass hkLinkedCollidableClass_330b1;
extern hkClass hkBroadPhaseHandleClass_330b1;
extern hkClass hkConvexPieceStreamDataClass_330b1;
extern hkClass hkMoppCodeCodeInfoClass_330b1;
extern hkClass hkMoppCodeClass_330b1;
extern hkClass hkPoweredRagdollConstraintDataClass_330b1;
extern hkClass hkRagdollConstraintDataConstraintBasisAClass_330b1;
extern hkClass hkRagdollConstraintDataConstraintBasisBClass_330b1;
extern hkClass hkRagdollConstraintDataClass_330b1;
extern hkClass hkRagdollLimitsDataConstraintBasisAClass_330b1;
extern hkClass hkRagdollLimitsDataConstraintBasisBClass_330b1;
extern hkClass hkRagdollLimitsDataClass_330b1;
extern hkClass hkAngularDashpotActionClass_330b1;
extern hkClass hkDashpotActionClass_330b1;
extern hkClass hkMotorActionClass_330b1;
extern hkClass hkReorientActionClass_330b1;
extern hkClass hkSpringActionClass_330b1;
extern hkClass hkCharacterProxyCinfoClass_330b1;
extern hkClass hkDisableEntityCollisionFilterClass_330b1;
extern hkClass hkConstrainedSystemFilterClass_330b1;
extern hkClass hkGroupCollisionFilterClass_330b1;
extern hkClass hkPairwiseCollisionFilterCollisionPairClass_330b1;
extern hkClass hkPairwiseCollisionFilterClass_330b1;
extern hkClass hkPoweredChainMapperTargetClass_330b1;
extern hkClass hkPoweredChainMapperLinkInfoClass_330b1;
extern hkClass hkPoweredChainMapperClass_330b1;
extern hkClass hkMouseSpringActionClass_330b1;
extern hkClass hkRigidBodyDisplayBindingClass_330b1;
extern hkClass hkPhysicsSystemDisplayBindingClass_330b1;
extern hkClass hkDisplayBindingDataClass_330b1;
extern hkClass hkPhysicsDataClass_330b1;
extern hkClass hkSerializedDisplayMarkerClass_330b1;
extern hkClass hkSerializedDisplayMarkerListClass_330b1;
extern hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_330b1;
extern hkClass hkSerializedDisplayRbTransformsClass_330b1;
extern hkClass hkVehicleDataWheelComponentParamsClass_330b1;
extern hkClass hkVehicleDataClass_330b1;
extern hkClass hkVehicleInstanceWheelInfoClass_330b1;
extern hkClass hkVehicleInstanceClass_330b1;
extern hkClass hkVehicleAerodynamicsClass_330b1;
extern hkClass hkVehicleDefaultAerodynamicsClass_330b1;
extern hkClass hkVehicleBrakeClass_330b1;
extern hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_330b1;
extern hkClass hkVehicleDefaultBrakeClass_330b1;
extern hkClass hkVehicleDriverInputStatusClass_330b1;
extern hkClass hkVehicleDriverInputClass_330b1;
extern hkClass hkVehicleDriverInputAnalogStatusClass_330b1;
extern hkClass hkVehicleDefaultAnalogDriverInputClass_330b1;
extern hkClass hkVehicleEngineClass_330b1;
extern hkClass hkVehicleDefaultEngineClass_330b1;
extern hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_330b1;
extern hkClass hkVehicleFrictionDescriptionClass_330b1;
extern hkClass hkVehicleFrictionStatusAxisStatusClass_330b1;
extern hkClass hkVehicleFrictionStatusClass_330b1;
extern hkClass hkVehicleSteeringClass_330b1;
extern hkClass hkVehicleDefaultSteeringClass_330b1;
extern hkClass hkVehicleSuspensionSuspensionWheelParametersClass_330b1;
extern hkClass hkVehicleSuspensionClass_330b1;
extern hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_330b1;
extern hkClass hkVehicleDefaultSuspensionClass_330b1;
extern hkClass hkVehicleTransmissionClass_330b1;
extern hkClass hkVehicleDefaultTransmissionClass_330b1;
extern hkClass hkTyremarkPointClass_330b1;
extern hkClass hkTyremarksWheelClass_330b1;
extern hkClass hkTyremarksInfoClass_330b1;
extern hkClass hkVehicleVelocityDamperClass_330b1;
extern hkClass hkVehicleDefaultVelocityDamperClass_330b1;
extern hkClass hkVehicleWheelCollideClass_330b1;
extern hkClass hkRejectRayChassisListenerClass_330b1;
extern hkClass hkVehicleRaycastWheelCollideClass_330b1;
extern hkClass hkRagdollInstanceClass_330b1;

hkClass hkBaseObjectClass_330b1(
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
hkClass hkReferencedObjectClass_330b1(
	"hkReferencedObject",
	&hkBaseObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkReferencedObjectClass_Members),
	int(sizeof(hkReferencedObjectClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkClassSignatureFlagsEnumItems_330b1[] =
{
	{1, "SIGNATURE_LOCAL"},
};
static const hkInternalClassEnum hkClassEnums[] = {
	{"SignatureFlags", hkClassSignatureFlagsEnumItems_330b1, 1 }
};
extern const hkClassEnum* hkClassSignatureFlagsEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkClassEnums[0]);
static hkInternalClassMember hkClassClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "parent", &hkClassClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "objectSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numImplementedInterfaces", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "declaredEnums", &hkClassEnumClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "declaredMembers", &hkClassMemberClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "defaults", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkClassClass_330b1(
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
hkClass hkClassEnumItemClass_330b1(
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
	{ "items", &hkClassEnumItemClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkClassEnumClass_330b1(
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
static const hkInternalClassEnumItem hkClassMemberTypeEnumItems_330b1[] =
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
static const hkInternalClassEnumItem hkClassMemberFlagsEnumItems_330b1[] =
{
	{1, "POINTER_OPTIONAL"},
	{2, "POINTER_VOIDSTAR"},
	{8, "ENUM_8"},
	{16, "ENUM_16"},
	{32, "ENUM_32"},
	{64, "ARRAY_RAWDATA"},
};
static const hkInternalClassEnumItem hkClassMemberRangeEnumItems_330b1[] =
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
	{"Type", hkClassMemberTypeEnumItems_330b1, 30 },
	{"Flags", hkClassMemberFlagsEnumItems_330b1, 6 },
	{"Range", hkClassMemberRangeEnumItems_330b1, 7 }
};
extern const hkClassEnum* hkClassMemberTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[0]);
extern const hkClassEnum* hkClassMemberFlagsEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[1]);
extern const hkClassEnum* hkClassMemberRangeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[2]);
static hkInternalClassMember hkClassMemberClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "class", &hkClassClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "enum", &hkClassEnumClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "type", HK_NULL, hkClassMemberTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "subtype", HK_NULL, hkClassMemberTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "cArraySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkClassMemberClass_330b1(
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
hkClass hkMonitorStreamStringMapStringMapClass_330b1(
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
	{ "map", &hkMonitorStreamStringMapStringMapClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkMonitorStreamStringMapClass_330b1(
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
static const hkInternalClassEnumItem hkMonitorStreamFrameInfoAbsoluteTimeCounterEnumItems_330b1[] =
{
	{0, "ABSOLUTE_TIME_TIMER_0"},
	{1, "ABSOLUTE_TIME_TIMER_1"},
	{0xffffffff, "ABSOLUTE_TIME_NOT_TIMED"},
};
static const hkInternalClassEnum hkMonitorStreamFrameInfoEnums[] = {
	{"AbsoluteTimeCounter", hkMonitorStreamFrameInfoAbsoluteTimeCounterEnumItems_330b1, 3 }
};
extern const hkClassEnum* hkMonitorStreamFrameInfoAbsoluteTimeCounterEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMonitorStreamFrameInfoEnums[0]);
static hkInternalClassMember hkMonitorStreamFrameInfoClass_Members[] =
{
	{ "heading", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "indexOfTimer0", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexOfTimer1", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absoluteTimeCounter", HK_NULL, hkMonitorStreamFrameInfoAbsoluteTimeCounterEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "timerFactor0", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "timerFactor1", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMonitorStreamFrameInfoClass_330b1(
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
hkClass hkAabbClass_330b1(
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
hkClass hkContactPointClass_330b1(
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
static const hkInternalClassEnumItem hkContactPointMaterialFlagEnumEnumItems_330b1[] =
{
	{1, "CONTACT_IS_NEW_AND_POTENTIAL"},
	{2, "CONTACT_USES_SOLVER_PATH2"},
};
static const hkInternalClassEnum hkContactPointMaterialEnums[] = {
	{"FlagEnum", hkContactPointMaterialFlagEnumEnumItems_330b1, 2 }
};
extern const hkClassEnum* hkContactPointMaterialFlagEnumEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkContactPointMaterialEnums[0]);
static hkInternalClassMember hkContactPointMaterialClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkContactPointMaterialClass_330b1(
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
	{ "sweptTransform", &hkSweptTransformClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deltaAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationClass", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationCounter", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMotionStateClass_330b1(
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
hkClass hkSphereClass_330b1(
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
hkClass hkSweptTransformClass_330b1(
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
extern const hkClassEnum* hkxAttributeHintEnum_330b1;
static hkInternalClassMember hkxAnimatedFloatClass_Members[] =
{
	{ "floats", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedFloatClass_330b1(
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
extern const hkClassEnum* hkxAttributeHintEnum_330b1;
static hkInternalClassMember hkxAnimatedMatrixClass_Members[] =
{
	{ "matrices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedMatrixClass_330b1(
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
hkClass hkxAnimatedQuaternionClass_330b1(
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
extern const hkClassEnum* hkxAttributeHintEnum_330b1;
static hkInternalClassMember hkxAnimatedVectorClass_Members[] =
{
	{ "vectors", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "hint", HK_NULL, hkxAttributeHintEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkxAnimatedVectorClass_330b1(
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
static const hkInternalClassEnumItem hkxAttributeHintEnumItems_330b1[] =
{
	{0, "HINT_NONE"},
	{1, "HINT_IGNORE"},
	{2, "HINT_TRANSFORM"},
	{4, "HINT_SCALE"},
	{6, "HINT_TRANSFORM_AND_SCALE"},
	{8, "HINT_FLIP"},
};
static const hkInternalClassEnum hkxAttributeEnums[] = {
	{"Hint", hkxAttributeHintEnumItems_330b1, 6 }
};
extern const hkClassEnum* hkxAttributeHintEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkxAttributeEnums[0]);
static hkInternalClassMember hkxAttributeClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "value", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxAttributeClass_330b1(
	"hkxAttribute",
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkxAttributeEnums),
	1,
	reinterpret_cast<const hkClassMember*>(hkxAttributeClass_Members),
	int(sizeof(hkxAttributeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkxAttributeGroupClass_Members[] =
{
	{ "groupType", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "attributes", &hkxAttributeClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxAttributeGroupClass_330b1(
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
hkClass hkxSparselyAnimatedBoolClass_330b1(
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
	{ "type", &hkClassEnumClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedEnumClass_330b1(
	"hkxSparselyAnimatedEnum",
	&hkxSparselyAnimatedIntClass_330b1,
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
hkClass hkxSparselyAnimatedIntClass_330b1(
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
hkClass hkxSparselyAnimatedStringStringTypeClass_330b1(
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
	{ "strings", &hkxSparselyAnimatedStringStringTypeClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "times", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkxSparselyAnimatedStringClass_330b1(
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
hkClass hkxCameraClass_330b1(
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
hkClass hkxNodeAnnotationDataClass_330b1(
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
	{ "children", &hkxNodeClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "annotations", &hkxNodeAnnotationDataClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "userProperties", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "selected", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "attributeGroups", &hkxAttributeGroupClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxNodeClass_330b1(
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
static const hkInternalClassEnumItem hkxLightLightTypeEnumItems_330b1[] =
{
	{0, "POINT_LIGHT"},
	{1, "DIRECTIONAL_LIGHT"},
	{2, "SPOT_LIGHT"},
};
static const hkInternalClassEnum hkxLightEnums[] = {
	{"LightType", hkxLightLightTypeEnumItems_330b1, 3 }
};
extern const hkClassEnum* hkxLightLightTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkxLightEnums[0]);
static hkInternalClassMember hkxLightClass_Members[] =
{
	{ "type", HK_NULL, hkxLightLightTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "direction", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "color", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxLightClass_330b1(
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
static const hkInternalClassEnumItem hkxMaterialTextureTypeEnumItems_330b1[] =
{
	{0, "TEX_UNKNOWN"},
	{1, "TEX_DIFFUSE"},
	{2, "TEX_REFLECTION"},
	{3, "TEX_BUMP"},
	{4, "TEX_NORMAL"},
	{5, "TEX_DISPLACEMENT"},
};
static const hkInternalClassEnum hkxMaterialEnums[] = {
	{"TextureType", hkxMaterialTextureTypeEnumItems_330b1, 6 }
};
extern const hkClassEnum* hkxMaterialTextureTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEnums[0]);
static hkInternalClassMember hkxMaterial_TextureStageClass_Members[] =
{
	{ "texture", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "usageHint", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxMaterialTextureStageClass_330b1(
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
	{ "stages", &hkxMaterialTextureStageClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "diffuseColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ambientColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "specularColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "emissiveColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subMaterials", &hkxMaterialClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "extraData", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxMaterialClass_330b1(
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
static const hkInternalClassEnumItem hkxMaterialEffectEffectTypeEnumItems_330b1[] =
{
	{0, "EFFECT_TYPE_INVALID"},
	{1, "EFFECT_TYPE_UNKNOWN"},
	{2, "EFFECT_TYPE_HLSL_FX"},
	{3, "EFFECT_TYPE_CG_FX"},
	{4, "EFFECT_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxMaterialEffectEnums[] = {
	{"EffectType", hkxMaterialEffectEffectTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkxMaterialEffectEffectTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEffectEnums[0]);
static hkInternalClassMember hkxMaterialEffectClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "type", HK_NULL, hkxMaterialEffectEffectTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkxMaterialEffectClass_330b1(
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
hkClass hkxTextureFileClass_330b1(
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
hkClass hkxTextureInplaceClass_330b1(
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
static const hkInternalClassEnumItem hkxIndexBufferIndexTypeEnumItems_330b1[] =
{
	{0, "INDEX_TYPE_INVALID"},
	{1, "INDEX_TYPE_TRI_LIST"},
	{2, "INDEX_TYPE_TRI_STRIP"},
	{3, "INDEX_TYPE_TRI_FAN"},
	{4, "INDEX_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxIndexBufferEnums[] = {
	{"IndexType", hkxIndexBufferIndexTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkxIndexBufferIndexTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkxIndexBufferEnums[0]);
static hkInternalClassMember hkxIndexBufferClass_Members[] =
{
	{ "indexType", HK_NULL, hkxIndexBufferIndexTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "vertexBaseOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxIndexBufferClass_330b1(
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
	{ "sections", &hkxMeshSectionClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMeshClass_330b1(
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
	{ "vertexBuffer", &hkxVertexBufferClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "indexBuffers", &hkxIndexBufferClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkxMaterialClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxMeshSectionClass_330b1(
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
	{ "format", &hkxVertexFormatClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxVertexBufferClass_330b1(
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
hkClass hkxVertexFormatClass_330b1(
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
hkClass hkxVertexP4N4C1T2Class_330b1(
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
hkClass hkxVertexP4N4T4B4C1T2Class_330b1(
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
hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_330b1(
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
hkClass hkxVertexP4N4T4B4W4I4Q4Class_330b1(
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
hkClass hkxVertexP4N4W4I4C1Q2Class_330b1(
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
	{ "rootNode", &hkxNodeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "cameras", &hkxCameraClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "lights", &hkxLightClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "meshes", &hkxMeshClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materials", &hkxMaterialClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "inplaceTextures", &hkxTextureInplaceClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "externalTextures", &hkxTextureFileClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skinBindings", &hkxSkinBindingClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "appliedTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkxScene_DefaultStruct
	{
		int s_defaultOffsets[12];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		_hkMatrix3 m_appliedTransform;
	};
	const hkxScene_DefaultStruct hkxScene_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkxScene_DefaultStruct,m_appliedTransform)},
		{1,0,0,0,0,1,0,0,0,0,1,0}
	};
}
hkClass hkxSceneClass_330b1(
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
	{ "mesh", &hkxMeshClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", &hkxNodeClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "initSkinTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxSkinBindingClass_330b1(
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
	{ "contentsClassNameSectionIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsClassNameSectionOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsVersion", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 16, 0, 0 },
	{ "pad", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkPackfileHeaderClass_330b1(
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
hkClass hkPackfileSectionHeaderClass_330b1(
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
hkClass hkRootLevelContainerNamedVariantClass_330b1(
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
	{ "namedVariants", &hkRootLevelContainerNamedVariantClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkRootLevelContainerClass_330b1(
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
hkClass hkVersioningExceptionsArrayVersioningExceptionClass_330b1(
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
	{ "exceptions", &hkVersioningExceptionsArrayVersioningExceptionClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVersioningExceptionsArrayClass_330b1(
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
	{ "skeletons", &hkSkeletonClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "animations", &hkSkeletalAnimationClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindings", &hkAnimationBindingClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "attachments", &hkBoneAttachmentClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skins", &hkMeshBindingClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkAnimationContainerClass_330b1(
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
static const hkInternalClassEnumItem hkAnimationBindingBlendHintEnumItems_330b1[] =
{
	{0, "NORMAL"},
	{1, "ADDITIVE"},
};
static const hkInternalClassEnum hkAnimationBindingEnums[] = {
	{"BlendHint", hkAnimationBindingBlendHintEnumItems_330b1, 2 }
};
extern const hkClassEnum* hkAnimationBindingBlendHintEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkAnimationBindingEnums[0]);
static hkInternalClassMember hkAnimationBindingClass_Members[] =
{
	{ "animation", &hkSkeletalAnimationClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "animationTrackToBoneIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "blendHint", HK_NULL, hkAnimationBindingBlendHintEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkAnimationBindingClass_330b1(
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
hkClass hkAnnotationTrackAnnotationClass_330b1(
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
	{ "annotations", &hkAnnotationTrackAnnotationClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkAnnotationTrackClass_330b1(
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
	{ "extractedMotion", &hkAnimatedReferenceFrameClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "annotationTracks", &hkAnnotationTrackClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkSkeletalAnimationClass_330b1(
	"hkSkeletalAnimation",
	&hkReferencedObjectClass_330b1,
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
hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_330b1(
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
	{ "qFormat", &hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkDeltaCompressedSkeletalAnimationClass_330b1(
	"hkDeltaCompressedSkeletalAnimation",
	&hkSkeletalAnimationClass_330b1,
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
hkClass hkInterleavedSkeletalAnimationClass_330b1(
	"hkInterleavedSkeletalAnimation",
	&hkSkeletalAnimationClass_330b1,
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
hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_330b1(
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
	{ "qFormat", &hkWaveletSkeletalAnimationQuantizationFormatClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "blockIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};
hkClass hkWaveletSkeletalAnimationClass_330b1(
	"hkWaveletSkeletalAnimation",
	&hkSkeletalAnimationClass_330b1,
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
hkClass hkMeshBindingMappingClass_330b1(
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
	{ "mesh", &hkxMeshClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mappings", &hkMeshBindingMappingClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "inverseWorldBindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_TRANSFORM, 0, 0, 0 }
};
hkClass hkMeshBindingClass_330b1(
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
	{ "mapping", &hkSkeletonMapperDataClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperClass_330b1(
	"hkSkeletonMapper",
	&hkReferencedObjectClass_330b1,
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
hkClass hkSkeletonMapperDataSimpleMappingClass_330b1(
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
hkClass hkSkeletonMapperDataChainMappingClass_330b1(
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
	{ "skeletonA", &hkSkeletonClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeletonB", &hkSkeletonClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simpleMappings", &hkSkeletonMapperDataSimpleMappingClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chainMappings", &hkSkeletonMapperDataChainMappingClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "unmappedBones", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "keepUnmappedLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperDataClass_330b1(
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
hkClass hkAnimatedReferenceFrameClass_330b1(
	"hkAnimatedReferenceFrame",
	&hkReferencedObjectClass_330b1,
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
hkClass hkDefaultAnimatedReferenceFrameClass_330b1(
	"hkDefaultAnimatedReferenceFrame",
	&hkAnimatedReferenceFrameClass_330b1,
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
hkClass hkBoneClass_330b1(
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
hkClass hkBoneAttachmentClass_330b1(
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
	{ "bones", &hkBoneClass_330b1, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "referencePose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};
hkClass hkSkeletonClass_330b1(
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
	{ "shape", &hkShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motion", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "parent", &hkCdBodyClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkCdBodyClass_330b1(
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
	{ "broadPhaseHandle", &hkTypedBroadPhaseHandleClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowedPenetrationDepth", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCollidableClass_330b1(
	"hkCollidable",
	&hkCdBodyClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkCollidableClass_Members),
	int(sizeof(hkCollidableClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkShapeCollectionFilterClass_330b1(
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
hkClass hkTypedBroadPhaseHandleClass_330b1(
	"hkTypedBroadPhaseHandle",
	&hkBroadPhaseHandleClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkTypedBroadPhaseHandleClass_Members),
	int(sizeof(hkTypedBroadPhaseHandleClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkCollidableCollidableFilterClass_330b1(
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
hkClass hkCollisionFilterClass_330b1(
	"hkCollisionFilter",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	4,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkRayCollidableFilterClass_330b1(
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
hkClass hkGroupFilterClass_330b1(
	"hkGroupFilter",
	&hkCollisionFilterClass_330b1,
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
	{ "collisionFilters", &hkCollisionFilterClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkCollisionFilterListClass_330b1(
	"hkCollisionFilterList",
	&hkCollisionFilterClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkCollisionFilterListClass_Members),
	int(sizeof(hkCollisionFilterListClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkNullCollisionFilterClass_330b1(
	"hkNullCollisionFilter",
	&hkCollisionFilterClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkRayShapeCollectionFilterClass_330b1(
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
hkClass hkShapeClass_330b1(
	"hkShape",
	&hkReferencedObjectClass_330b1,
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
	{ "rayShapeCollectionFilter", &hkRayShapeCollectionFilterClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkShapeRayCastInputClass_330b1(
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
hkClass hkBoxShapeClass_330b1(
	"hkBoxShape",
	&hkConvexShapeClass_330b1,
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
	{ "boundingVolumeShape", &hkShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childShape", &hkShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBvShapeClass_330b1(
	"hkBvShape",
	&hkShapeClass_330b1,
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
	{ "shapeCollection", &hkShapeCollectionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBvTreeShapeClass_330b1(
	"hkBvTreeShape",
	&hkShapeClass_330b1,
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
hkClass hkCapsuleShapeClass_330b1(
	"hkCapsuleShape",
	&hkConvexShapeClass_330b1,
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
hkClass hkShapeCollectionClass_330b1(
	"hkShapeCollection",
	&hkShapeClass_330b1,
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
hkClass hkConvexShapeClass_330b1(
	"hkConvexShape",
	&hkSphereRepShapeClass_330b1,
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
hkClass hkConvexListShapeClass_330b1(
	"hkConvexListShape",
	&hkListShapeClass_330b1,
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
	{ "convexPieceStream", &hkConvexPieceStreamDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayMesh", &hkShapeCollectionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexPieceMeshShapeClass_330b1(
	"hkConvexPieceMeshShape",
	&hkShapeCollectionClass_330b1,
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
	{ "childShape", &hkConvexShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexTransformShapeClass_330b1(
	"hkConvexTransformShape",
	&hkConvexShapeClass_330b1,
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
	{ "childShape", &hkConvexShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "translation", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConvexTranslateShapeClass_330b1(
	"hkConvexTranslateShape",
	&hkConvexShapeClass_330b1,
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
hkClass hkConvexVerticesShapeFourVectorsClass_330b1(
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
	{ "rotatedVertices", &hkConvexVerticesShapeFourVectorsClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeEquations", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};
hkClass hkConvexVerticesShapeClass_330b1(
	"hkConvexVerticesShape",
	&hkConvexShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkConvexVerticesShapeClass_Members),
	int(sizeof(hkConvexVerticesShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkCylinderShapeVertexIdEncodingEnumItems_330b1[] =
{
	{7, "VERTEX_ID_ENCODING_IS_BASE_A_SHIFT"},
	{6, "VERTEX_ID_ENCODING_SIN_SIGN_SHIFT"},
	{5, "VERTEX_ID_ENCODING_COS_SIGN_SHIFT"},
	{4, "VERTEX_ID_ENCODING_IS_SIN_LESSER_SHIFT"},
	{0x0f, "VERTEX_ID_ENCODING_VALUE_MASK"},
};
static const hkInternalClassEnum hkCylinderShapeEnums[] = {
	{"VertexIdEncoding", hkCylinderShapeVertexIdEncodingEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkCylinderShapeVertexIdEncodingEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkCylinderShapeEnums[0]);
static hkInternalClassMember hkCylinderShapeClass_Members[] =
{
	{ "cylRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCylinderShapeClass_330b1(
	"hkCylinderShape",
	&hkConvexShapeClass_330b1,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkCylinderShapeEnums),
	1,
	reinterpret_cast<const hkClassMember*>(hkCylinderShapeClass_Members),
	int(sizeof(hkCylinderShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkFastMeshShapeClass_330b1(
	"hkFastMeshShape",
	&hkMeshShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkHeightFieldShapeClass_330b1(
	"hkHeightFieldShape",
	&hkShapeClass_330b1,
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
	{ "shape", &hkShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkListShapeChildInfoClass_330b1(
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
	{ "childInfo", &hkListShapeChildInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkListShapeClass_330b1(
	"hkListShape",
	&hkShapeCollectionClass_330b1,
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
hkClass hkMeshMaterialClass_330b1(
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
static const hkInternalClassEnumItem hkMeshShapeIndexStridingTypeEnumItems_330b1[] =
{
	{0, "INDICES_INVALID"},
	{1, "INDICES_INT16"},
	{2, "INDICES_INT32"},
	{3, "INDICES_MAX_ID"},
};
static const hkInternalClassEnumItem hkMeshShapeMaterialIndexStridingTypeEnumItems_330b1[] =
{
	{0, "MATERIAL_INDICES_INVALID"},
	{1, "MATERIAL_INDICES_INT8"},
	{2, "MATERIAL_INDICES_INT16"},
	{3, "MATERIAL_INDICES_MAX_ID"},
};
static const hkInternalClassEnum hkMeshShapeEnums[] = {
	{"IndexStridingType", hkMeshShapeIndexStridingTypeEnumItems_330b1, 4 },
	{"MaterialIndexStridingType", hkMeshShapeMaterialIndexStridingTypeEnumItems_330b1, 4 }
};
extern const hkClassEnum* hkMeshShapeIndexStridingTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[0]);
extern const hkClassEnum* hkMeshShapeMaterialIndexStridingTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[1]);
static hkInternalClassMember hkMeshShape_SubpartClass_Members[] =
{
	{ "vertexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "vertexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "stridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "materialIndexStridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialBase", &hkMeshMaterialClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numMaterials", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMeshShapeSubpartClass_330b1(
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
	{ "subparts", &hkMeshShapeSubpartClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMeshShapeClass_330b1(
	"hkMeshShape",
	&hkShapeCollectionClass_330b1,
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
	{ "code", &hkMoppCodeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkMoppBvTreeShapeClass_330b1(
	"hkMoppBvTreeShape",
	&hkBvTreeShapeClass_330b1,
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
hkClass hkMultiRayShapeRayClass_330b1(
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
	{ "rays", &hkMultiRayShapeRayClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rayPenetrationDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMultiRayShapeClass_330b1(
	"hkMultiRayShape",
	&hkShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkMultiRayShapeClass_Members),
	int(sizeof(hkMultiRayShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkMultiSphereShapeClass_Members[] =
{
	{ "numSpheres", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spheres", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 8, 0, 0 }
};
hkClass hkMultiSphereShapeClass_330b1(
	"hkMultiSphereShape",
	&hkSphereRepShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkMultiSphereShapeClass_Members),
	int(sizeof(hkMultiSphereShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkPhantomCallbackShapeClass_330b1(
	"hkPhantomCallbackShape",
	&hkShapeClass_330b1,
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
hkClass hkPlaneShapeClass_330b1(
	"hkPlaneShape",
	&hkHeightFieldShapeClass_330b1,
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
hkClass hkSampledHeightFieldShapeClass_330b1(
	"hkSampledHeightFieldShape",
	&hkHeightFieldShapeClass_330b1,
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
hkClass hkSimpleMeshShapeTriangleClass_330b1(
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
	{ "triangles", &hkSimpleMeshShapeTriangleClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSimpleMeshShapeClass_330b1(
	"hkSimpleMeshShape",
	&hkShapeCollectionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkSimpleMeshShapeClass_Members),
	int(sizeof(hkSimpleMeshShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkSphereShapeClass_330b1(
	"hkSphereShape",
	&hkConvexShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkSphereRepShapeClass_330b1(
	"hkSphereRepShape",
	&hkShapeClass_330b1,
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
hkClass hkStorageMeshShapeSubpartStorageClass_330b1(
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
	{ "storage", &hkStorageMeshShapeSubpartStorageClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkStorageMeshShapeClass_330b1(
	"hkStorageMeshShape",
	&hkMeshShapeClass_330b1,
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
hkClass hkStorageSampledHeightFieldShapeClass_330b1(
	"hkStorageSampledHeightFieldShape",
	&hkSampledHeightFieldShapeClass_330b1,
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
	{ "childShape", &hkShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTransformShapeClass_330b1(
	"hkTransformShape",
	&hkShapeClass_330b1,
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
hkClass hkTriangleShapeClass_330b1(
	"hkTriangleShape",
	&hkConvexShapeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkTriangleShapeClass_Members),
	int(sizeof(hkTriangleShapeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkTriSampledHeightFieldBvTreeShapeClass_Members[] =
{
	{ "wantAabbRejectionTest", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriSampledHeightFieldBvTreeShapeClass_330b1(
	"hkTriSampledHeightFieldBvTreeShape",
	&hkBvTreeShapeClass_330b1,
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
	{ "heightfield", &hkSampledHeightFieldShapeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTriSampledHeightFieldCollectionClass_330b1(
	"hkTriSampledHeightFieldCollection",
	&hkShapeCollectionClass_330b1,
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
hkClass hkActionClass_330b1(
	"hkAction",
	&hkReferencedObjectClass_330b1,
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
	{ "entities", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkArrayActionClass_330b1(
	"hkArrayAction",
	&hkActionClass_330b1,
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
	{ "entityA", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entityB", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkBinaryActionClass_330b1(
	"hkBinaryAction",
	&hkActionClass_330b1,
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
	{ "entity", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkUnaryActionClass_330b1(
	"hkUnaryAction",
	&hkActionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkUnaryActionClass_Members),
	int(sizeof(hkUnaryActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkMaterialResponseTypeEnumItems_330b1[] =
{
	{0, "RESPONSE_INVALID"},
	{1, "RESPONSE_SIMPLE_CONTACT"},
	{2, "RESPONSE_REPORTING"},
	{3, "RESPONSE_NONE"},
	{4, "RESPONSE_MAX_ID"},
};
static const hkInternalClassEnum hkMaterialEnums[] = {
	{"ResponseType", hkMaterialResponseTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkMaterialResponseTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMaterialEnums[0]);
static hkInternalClassMember hkMaterialClass_Members[] =
{
	{ "responseType", HK_NULL, hkMaterialResponseTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMaterialClass_330b1(
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
hkClass hkPropertyValueClass_330b1(
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
	{ "value", &hkPropertyValueClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPropertyClass_330b1(
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
static const hkInternalClassEnumItem hkConstraintDataConstraintTypeEnumItems_330b1[] =
{
	{0, "CONSTRAINT_TYPE_BALLANDSOCKET"},
	{1, "CONSTRAINT_TYPE_HINGE"},
	{2, "CONSTRAINT_TYPE_LIMITEDHINGE"},
	{3, "CONSTRAINT_TYPE_POINTTOPATH"},
	{4, "CONSTRAINT_TYPE_POWEREDHINGE"},
	{5, "CONSTRAINT_TYPE_POWEREDRAGDOLL"},
	{6, "CONSTRAINT_TYPE_PRISMATIC"},
	{7, "CONSTRAINT_TYPE_RAGDOLL"},
	{8, "CONSTRAINT_TYPE_STIFFSPRING"},
	{9, "CONSTRAINT_TYPE_WHEEL"},
	{10, "CONSTRAINT_TYPE_GENERIC"},
	{11, "CONSTRAINT_TYPE_CONTACT"},
	{12, "CONSTRAINT_TYPE_BREAKABLE"},
	{13, "CONSTRAINT_TYPE_MALLEABLE"},
	{14, "CONSTRAINT_TYPE_POINTTOPLANE"},
	{15, "CONSTRAINT_TYPE_PULLEY"},
	{16, "CONSTRAINT_TYPE_RELATIVE_ORIENTATION"},
	{18, "CONSTRAINT_TYPE_HINGE_LIMITS"},
	{19, "CONSTRAINT_TYPE_RAGDOLL_LIMITS"},
	{100, "BEGIN_CONSTRAINT_CHAIN_TYPES"},
	{100, "CONSTRAINT_TYPE_STIFF_SPRING_CHAIN"},
	{101, "CONSTRAINT_TYPE_BALL_SOCKET_CHAIN"},
	{102, "CONSTRAINT_TYPE_POWERED_CHAIN"},
};
static const hkInternalClassEnum hkConstraintDataEnums[] = {
	{"ConstraintType", hkConstraintDataConstraintTypeEnumItems_330b1, 23 }
};
extern const hkClassEnum* hkConstraintDataConstraintTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkConstraintDataEnums[0]);
static hkInternalClassMember hkConstraintDataClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkConstraintDataClass_330b1(
	"hkConstraintData",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkConstraintDataEnums),
	1,
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
hkClass hkConstraintInfoClass_330b1(
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
static const hkInternalClassEnumItem hkConstraintInstanceConstraintPriorityEnumItems_330b1[] =
{
	{0, "PRIORITY_INVALID"},
	{1, "PRIORITY_PSI"},
	{2, "PRIORITY_TOI"},
	{3, "PRIORITY_TOI_HIGHER"},
	{4, "PRIORITY_TOI_FORCED"},
};
static const hkInternalClassEnumItem hkConstraintInstanceTypeEnumItems_330b1[] =
{
	{0, "TYPE_NORMAL"},
	{1, "TYPE_CHAIN"},
};
static const hkInternalClassEnumItem hkConstraintInstanceAddReferencesEnumItems_330b1[] =
{
	{0, "DO_NOT_ADD_REFERENCES"},
	{1, "DO_ADD_REFERENCES"},
};
static const hkInternalClassEnum hkConstraintInstanceEnums[] = {
	{"ConstraintPriority", hkConstraintInstanceConstraintPriorityEnumItems_330b1, 5 },
	{"Type", hkConstraintInstanceTypeEnumItems_330b1, 2 },
	{"AddReferences", hkConstraintInstanceAddReferencesEnumItems_330b1, 2 }
};
extern const hkClassEnum* hkConstraintInstanceConstraintPriorityEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[0]);
extern const hkClassEnum* hkConstraintInstanceTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[1]);
extern const hkClassEnum* hkConstraintInstanceAddReferencesEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[2]);
static hkInternalClassMember hkConstraintInstanceClass_Members[] =
{
	{ "owner", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "data", &hkConstraintDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entities", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 2, 0, 0 },
	{ "priority", HK_NULL, hkConstraintInstanceConstraintPriorityEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "wantRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internal", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkConstraintInstanceClass_330b1(
	"hkConstraintInstance",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkConstraintInstanceEnums),
	3,
	reinterpret_cast<const hkClassMember*>(hkConstraintInstanceClass_Members),
	int(sizeof(hkConstraintInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkBallAndSocketConstraintDataClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBallAndSocketConstraintDataClass_330b1(
	"hkBallAndSocketConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkBallAndSocketConstraintDataClass_Members),
	int(sizeof(hkBallAndSocketConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkRelativeOrientationConstraintDataClass_Members[] =
{
	{ "aTb", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRelativeOrientationConstraintDataClass_330b1(
	"hkRelativeOrientationConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRelativeOrientationConstraintDataClass_Members),
	int(sizeof(hkRelativeOrientationConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataConstraintBasisAClass_330b1(
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
hkClass hkHingeConstraintDataConstraintBasisBClass_330b1(
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
	{ "basisA", &hkHingeConstraintDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkHingeConstraintDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataClass_330b1(
	"hkHingeConstraintData",
	&hkConstraintDataClass_330b1,
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
hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_330b1(
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
hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_330b1(
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
	{ "basisA", &hkLimitedHingeConstraintDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkLimitedHingeConstraintDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkLimitedHingeConstraintData_DefaultStruct
	{
		int s_defaultOffsets[6];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		hkReal m_angularLimitsTauFactor;
	};
	const hkLimitedHingeConstraintData_DefaultStruct hkLimitedHingeConstraintData_Default =
	{
		{-1,-1,-1,HK_OFFSET_OF(hkLimitedHingeConstraintData_DefaultStruct,m_angularLimitsTauFactor),-1,-1},
		1.0
	};
}
hkClass hkLimitedHingeConstraintDataClass_330b1(
	"hkLimitedHingeConstraintData",
	&hkConstraintDataClass_330b1,
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
hkClass hkLinearParametricCurveClass_330b1(
	"hkLinearParametricCurve",
	&hkParametricCurveClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkLinearParametricCurveClass_Members),
	int(sizeof(hkLinearParametricCurveClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkParametricCurveClass_330b1(
	"hkParametricCurve",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
static const hkInternalClassEnumItem hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_330b1[] =
{
	{0, "CONSTRAIN_ORIENTATION_INVALID"},
	{1, "CONSTRAIN_ORIENTATION_NONE"},
	{2, "CONSTRAIN_ORIENTATION_ALLOW_SPIN"},
	{3, "CONSTRAIN_ORIENTATION_TO_PATH"},
	{4, "CONSTRAIN_ORIENTATION_MAX_ID"},
};
static const hkInternalClassEnum hkPointToPathConstraintDataEnums[] = {
	{"OrientationConstraintType", hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkPointToPathConstraintDataOrientationConstraintTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkPointToPathConstraintDataEnums[0]);
static hkInternalClassMember hkPointToPathConstraintDataClass_Members[] =
{
	{ "path", &hkParametricCurveClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularConstrainedDOF", HK_NULL, hkPointToPathConstraintDataOrientationConstraintTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "transform_OS_KS", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkPointToPathConstraintDataClass_330b1(
	"hkPointToPathConstraintData",
	&hkConstraintDataClass_330b1,
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
hkClass hkPointToPlaneConstraintDataClass_330b1(
	"hkPointToPlaneConstraintData",
	&hkConstraintDataClass_330b1,
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
	{ "motor", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPoweredHingeConstraintDataClass_330b1(
	"hkPoweredHingeConstraintData",
	&hkLimitedHingeConstraintDataClass_330b1,
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
hkClass hkPrismaticConstraintDataConstraintBasisAClass_330b1(
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
hkClass hkPrismaticConstraintDataConstraintBasisBClass_330b1(
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
	{ "motor", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "basisA", &hkPrismaticConstraintDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkPrismaticConstraintDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motorTargetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataClass_330b1(
	"hkPrismaticConstraintData",
	&hkConstraintDataClass_330b1,
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
hkClass hkStiffSpringConstraintDataClass_330b1(
	"hkStiffSpringConstraintData",
	&hkConstraintDataClass_330b1,
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
hkClass hkWheelConstraintDataConstraintBasisAClass_330b1(
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
hkClass hkWheelConstraintDataConstraintBasisBClass_330b1(
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
	{ "basisA", &hkWheelConstraintDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkWheelConstraintDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMinLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMaxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataClass_330b1(
	"hkWheelConstraintData",
	&hkConstraintDataClass_330b1,
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
	{ "constraintData", &hkConstraintDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childRuntimeSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "childNumSolverResults", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "solverResultLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "removeWhenBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "revertBackVelocityOnBreak", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "listener", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkBreakableConstraintDataClass_330b1(
	"hkBreakableConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkBreakableConstraintDataClass_Members),
	int(sizeof(hkBreakableConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkConstraintChainDataClass_330b1(
	"hkConstraintChainData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
static hkInternalClassMember hkConstraintChainInstanceClass_Members[] =
{
	{ "chainedEntities", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "action", &hkConstraintChainInstanceActionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkConstraintChainInstanceClass_330b1(
	"hkConstraintChainInstance",
	&hkConstraintInstanceClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkConstraintChainInstanceClass_Members),
	int(sizeof(hkConstraintChainInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkConstraintChainInstanceActionClass_Members[] =
{
	{ "constraintInstance", &hkConstraintChainInstanceClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkConstraintChainInstanceActionClass_330b1(
	"hkConstraintChainInstanceAction",
	&hkActionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkConstraintChainInstanceActionClass_Members),
	int(sizeof(hkConstraintChainInstanceActionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkBallSocketChainData_ConstraintInfoClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBallSocketChainDataConstraintInfoClass_330b1(
	"hkBallSocketChainDataConstraintInfo",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkBallSocketChainData_ConstraintInfoClass_Members),
	int(sizeof(hkBallSocketChainData_ConstraintInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkBallSocketChainDataClass_Members[] =
{
	{ "infos", &hkBallSocketChainDataConstraintInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "cfm", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxErrorDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBallSocketChainDataClass_330b1(
	"hkBallSocketChainData",
	&hkConstraintChainDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkBallSocketChainDataClass_Members),
	int(sizeof(hkBallSocketChainDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkHingeLimitsData_ConstraintBasisAClass_Members[] =
{
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeLimitsDataConstraintBasisAClass_330b1(
	"hkHingeLimitsDataConstraintBasisA",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsData_ConstraintBasisAClass_Members),
	int(sizeof(hkHingeLimitsData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkHingeLimitsData_ConstraintBasisBClass_Members[] =
{
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perp2FreeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeLimitsDataConstraintBasisBClass_330b1(
	"hkHingeLimitsDataConstraintBasisB",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsData_ConstraintBasisBClass_Members),
	int(sizeof(hkHingeLimitsData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkHingeLimitsDataClass_Members[] =
{
	{ "minAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisA", &hkHingeLimitsDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkHingeLimitsDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeLimitsDataClass_330b1(
	"hkHingeLimitsData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsDataClass_Members),
	int(sizeof(hkHingeLimitsDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPoweredChainData_ConstraintInfoClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aTc", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "bTc", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motors", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 3, 0, 0 },
	{ "switchBodies", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPoweredChainDataConstraintInfoClass_330b1(
	"hkPoweredChainDataConstraintInfo",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPoweredChainData_ConstraintInfoClass_Members),
	int(sizeof(hkPoweredChainData_ConstraintInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPoweredChainDataClass_Members[] =
{
	{ "infos", &hkPoweredChainDataConstraintInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "cfm", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxErrorDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPoweredChainDataClass_330b1(
	"hkPoweredChainData",
	&hkConstraintChainDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPoweredChainDataClass_Members),
	int(sizeof(hkPoweredChainDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkStiffSpringChainData_ConstraintInfoClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkStiffSpringChainDataConstraintInfoClass_330b1(
	"hkStiffSpringChainDataConstraintInfo",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkStiffSpringChainData_ConstraintInfoClass_Members),
	int(sizeof(hkStiffSpringChainData_ConstraintInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkStiffSpringChainDataClass_Members[] =
{
	{ "infos", &hkStiffSpringChainDataConstraintInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "cfm", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkStiffSpringChainDataClass_330b1(
	"hkStiffSpringChainData",
	&hkConstraintChainDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkStiffSpringChainDataClass_Members),
	int(sizeof(hkStiffSpringChainDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkGenericConstraintDataClass_Members[] =
{
	{ "scheme", &hkGenericConstraintDataSchemeClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkGenericConstraintDataClass_330b1(
	"hkGenericConstraintData",
	&hkConstraintDataClass_330b1,
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
	{ "info", &hkConstraintInfoClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "commands", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT32, 0, 0, 0 },
	{ "modifiers", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "motors", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkGenericConstraintDataSchemeClass_330b1(
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
	{ "constraintData", &hkConstraintDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMalleableConstraintDataClass_330b1(
	"hkMalleableConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkMalleableConstraintDataClass_Members),
	int(sizeof(hkMalleableConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkConstraintMotorTypeEnumItems_330b1[] =
{
	{0, "TYPE_INVALID"},
	{1, "TYPE_POSITION"},
	{2, "TYPE_VELOCITY"},
	{3, "TYPE_SPRING_DAMPER"},
	{4, "TYPE_MAX"},
};
static const hkInternalClassEnum hkConstraintMotorEnums[] = {
	{"Type", hkConstraintMotorTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkConstraintMotorTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkConstraintMotorEnums[0]);
hkClass hkConstraintMotorClass_330b1(
	"hkConstraintMotor",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkConstraintMotorEnums),
	1,
	HK_NULL,
	0,
	HK_NULL
	);
static hkInternalClassMember hkLimitedForceConstraintMotorClass_Members[] =
{
	{ "minForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkLimitedForceConstraintMotorClass_330b1(
	"hkLimitedForceConstraintMotor",
	&hkConstraintMotorClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkLimitedForceConstraintMotorClass_Members),
	int(sizeof(hkLimitedForceConstraintMotorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPositionConstraintMotorClass_Members[] =
{
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "proportionalRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "constantRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPositionConstraintMotorClass_330b1(
	"hkPositionConstraintMotor",
	&hkLimitedForceConstraintMotorClass_330b1,
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
	{ "springDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpringDamperConstraintMotorClass_330b1(
	"hkSpringDamperConstraintMotor",
	&hkLimitedForceConstraintMotorClass_330b1,
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
	{ "useVelocityTargetFromConstraintTargets", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVelocityConstraintMotorClass_330b1(
	"hkVelocityConstraintMotor",
	&hkLimitedForceConstraintMotorClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVelocityConstraintMotorClass_Members),
	int(sizeof(hkVelocityConstraintMotorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPulleyConstraintDataClass_Members[] =
{
	{ "ropeLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "leverageOnBodyB", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pulleyPivotAinWorld", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pulleyPivotBinWorld", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPulleyConstraintDataClass_330b1(
	"hkPulleyConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPulleyConstraintDataClass_Members),
	int(sizeof(hkPulleyConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkEntityClass_Members[] =
{
	{ "motion", &hkMotionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simulationIsland", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkMaterialClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivator", &hkEntityDeactivatorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		hkUint32 m_uid;
	};
	const hkEntity_DefaultStruct hkEntity_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkEntity_DefaultStruct,m_uid)},
		0xffffffff
	};
}
hkClass hkEntityClass_330b1(
	"hkEntity",
	&hkWorldObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkEntityClass_Members),
	int(sizeof(hkEntityClass_Members)/sizeof(hkInternalClassMember)),
	&hkEntity_Default
	);
hkClass hkEntityDeactivatorClass_330b1(
	"hkEntityDeactivator",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkFakeRigidBodyDeactivatorClass_330b1(
	"hkFakeRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkRigidBodyClass_330b1(
	"hkRigidBody",
	&hkEntityClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
static const hkInternalClassEnumItem hkRigidBodyDeactivatorDeactivatorTypeEnumItems_330b1[] =
{
	{0, "DEACTIVATOR_INVALID"},
	{1, "DEACTIVATOR_NEVER"},
	{2, "DEACTIVATOR_SPATIAL"},
	{3, "DEACTIVATOR_MAX_ID"},
};
static const hkInternalClassEnum hkRigidBodyDeactivatorEnums[] = {
	{"DeactivatorType", hkRigidBodyDeactivatorDeactivatorTypeEnumItems_330b1, 4 }
};
extern const hkClassEnum* hkRigidBodyDeactivatorDeactivatorTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkRigidBodyDeactivatorEnums[0]);
hkClass hkRigidBodyDeactivatorClass_330b1(
	"hkRigidBodyDeactivator",
	&hkEntityDeactivatorClass_330b1,
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
hkClass hkSpatialRigidBodyDeactivatorSampleClass_330b1(
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
	{ "highFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "radiusSqrd", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpatialRigidBodyDeactivatorClass_330b1(
	"hkSpatialRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkSpatialRigidBodyDeactivatorClass_Members),
	int(sizeof(hkSpatialRigidBodyDeactivatorClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkMotionMotionTypeEnumItems_330b1[] =
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
	{"MotionType", hkMotionMotionTypeEnumItems_330b1, 10 }
};
extern const hkClassEnum* hkMotionMotionTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMotionEnums[0]);
static hkInternalClassMember hkMotionClass_Members[] =
{
	{ "solverData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMotionClass_330b1(
	"hkMotion",
	&hkReferencedObjectClass_330b1,
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
hkClass hkBoxMotionClass_330b1(
	"hkBoxMotion",
	&hkRigidMotionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkBoxMotionClass_Members),
	int(sizeof(hkBoxMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkFixedRigidMotionClass_330b1(
	"hkFixedRigidMotion",
	&hkKeyframedRigidMotionClass_330b1,
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
	{ "savedMotion", &hkRigidMotionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "savedQualityTypeIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkKeyframedRigidMotionClass_330b1(
	"hkKeyframedRigidMotion",
	&hkRigidMotionClass_330b1,
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
	{ "motionState", &hkMotionStateClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "massInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "particleMinInertiaDiagInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRigidMotionClass_330b1(
	"hkRigidMotion",
	&hkMotionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRigidMotionClass_Members),
	int(sizeof(hkRigidMotionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkSphereMotionClass_330b1(
	"hkSphereMotion",
	&hkRigidMotionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkStabilizedBoxMotionClass_330b1(
	"hkStabilizedBoxMotion",
	&hkBoxMotionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkStabilizedSphereMotionClass_330b1(
	"hkStabilizedSphereMotion",
	&hkSphereMotionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkThinBoxMotionClass_330b1(
	"hkThinBoxMotion",
	&hkBoxMotionClass_330b1,
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
	{ "aabb", &hkAabbClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "overlappingCollidables", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
hkClass hkAabbPhantomClass_330b1(
	"hkAabbPhantom",
	&hkPhantomClass_330b1,
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
hkClass hkCachingShapePhantomClass_330b1(
	"hkCachingShapePhantom",
	&hkShapePhantomClass_330b1,
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
hkClass hkPhantomClass_330b1(
	"hkPhantom",
	&hkWorldObjectClass_330b1,
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
	{ "motionState", &hkMotionStateClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkShapePhantomClass_330b1(
	"hkShapePhantom",
	&hkPhantomClass_330b1,
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
hkClass hkSimpleShapePhantomClass_330b1(
	"hkSimpleShapePhantom",
	&hkShapePhantomClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkSimpleShapePhantomClass_Members),
	int(sizeof(hkSimpleShapePhantomClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkMultiThreadLockAccessTypeEnumItems_330b1[] =
{
	{0, "HK_ACCESS_IGNORE"},
	{1, "HK_ACCESS_RO"},
	{2, "HK_ACCESS_RW"},
};
static const hkInternalClassEnum hkMultiThreadLockEnums[] = {
	{"AccessType", hkMultiThreadLockAccessTypeEnumItems_330b1, 3 }
};
extern const hkClassEnum* hkMultiThreadLockAccessTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkMultiThreadLockEnums[0]);
static hkInternalClassMember hkMultiThreadLockClass_Members[] =
{
	{ "threadId", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lockCount", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMultiThreadLockClass_330b1(
	"hkMultiThreadLock",
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkMultiThreadLockEnums),
	1,
	reinterpret_cast<const hkClassMember*>(hkMultiThreadLockClass_Members),
	int(sizeof(hkMultiThreadLockClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPhysicsSystemClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "actions", &hkActionClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "phantoms", &hkPhantomClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkPhysicsSystemClass_330b1(
	"hkPhysicsSystem",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPhysicsSystemClass_Members),
	int(sizeof(hkPhysicsSystemClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static const hkInternalClassEnumItem hkWorldCinfoSolverTypeEnumItems_330b1[] =
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
static const hkInternalClassEnumItem hkWorldCinfoSimulationTypeEnumItems_330b1[] =
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
static const hkInternalClassEnumItem hkWorldCinfoContactPointGenerationEnumItems_330b1[] =
{
	{0, "CONTACT_POINT_ACCEPT_ALWAYS"},
	{1, "CONTACT_POINT_REJECT_DUBIOUS"},
	{2, "CONTACT_POINT_REJECT_MANY"},
};
static const hkInternalClassEnumItem hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_330b1[] =
{
	{0, "BROADPHASE_BORDER_ASSERT"},
	{1, "BROADPHASE_BORDER_FIX_ENTITY"},
	{2, "BROADPHASE_BORDER_REMOVE_ENTITY"},
	{3, "BROADPHASE_BORDER_DO_NOTHING"},
};
static const hkInternalClassEnum hkWorldCinfoEnums[] = {
	{"SolverType", hkWorldCinfoSolverTypeEnumItems_330b1, 11 },
	{"SimulationType", hkWorldCinfoSimulationTypeEnumItems_330b1, 10 },
	{"ContactPointGeneration", hkWorldCinfoContactPointGenerationEnumItems_330b1, 3 },
	{"BroadPhaseBorderBehaviour", hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_330b1, 4 }
};
extern const hkClassEnum* hkWorldCinfoSolverTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[0]);
extern const hkClassEnum* hkWorldCinfoSimulationTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[1]);
extern const hkClassEnum* hkWorldCinfoContactPointGenerationEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[2]);
extern const hkClassEnum* hkWorldCinfoBroadPhaseBorderBehaviourEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[3]);
static hkInternalClassMember hkWorldCinfoClass_Members[] =
{
	{ "gravity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseQuerySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactRestingVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseBorderBehaviour", HK_NULL, hkWorldCinfoBroadPhaseBorderBehaviourEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "broadPhaseWorldAabb", &hkAabbClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilter", &hkCollisionFilterClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "expectedMaxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "expectedMinPsiDeltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "memoryWatchDog", &hkWorldMemoryWatchDogClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "broadPhaseNumMarkers", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactPointGeneration", HK_NULL, hkWorldCinfoContactPointGenerationEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
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
	{ "simulationType", HK_NULL, hkWorldCinfoSimulationTypeEnum_330b1, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
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
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		_hkBool m_processActionsInSingleThread;
		_hkBool m_synchronizeFrameAndPhysicsTime;
	};
	const hkWorldCinfo_DefaultStruct hkWorldCinfo_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkWorldCinfo_DefaultStruct,m_processActionsInSingleThread),HK_OFFSET_OF(hkWorldCinfo_DefaultStruct,m_synchronizeFrameAndPhysicsTime)},
		true,true
	};
}
hkClass hkWorldCinfoClass_330b1(
	"hkWorldCinfo",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassEnum*>(hkWorldCinfoEnums),
	4,
	reinterpret_cast<const hkClassMember*>(hkWorldCinfoClass_Members),
	int(sizeof(hkWorldCinfoClass_Members)/sizeof(hkInternalClassMember)),
	&hkWorldCinfo_Default
	);
static const hkInternalClassEnumItem hkWorldObjectBroadPhaseTypeEnumItems_330b1[] =
{
	{0, "BROAD_PHASE_INVALID"},
	{1, "BROAD_PHASE_ENTITY"},
	{2, "BROAD_PHASE_PHANTOM"},
	{3, "BROAD_PHASE_BORDER"},
	{4, "BROAD_PHASE_MAX_ID"},
};
static const hkInternalClassEnum hkWorldObjectEnums[] = {
	{"BroadPhaseType", hkWorldObjectBroadPhaseTypeEnumItems_330b1, 5 }
};
extern const hkClassEnum* hkWorldObjectBroadPhaseTypeEnum_330b1 = reinterpret_cast<const hkClassEnum*>(&hkWorldObjectEnums[0]);
static hkInternalClassMember hkWorldObjectClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "collidable", &hkLinkedCollidableClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "properties", &hkPropertyClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkWorldObjectClass_330b1(
	"hkWorldObject",
	&hkReferencedObjectClass_330b1,
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
hkClass hkWorldMemoryWatchDogClass_330b1(
	"hkWorldMemoryWatchDog",
	&hkReferencedObjectClass_330b1,
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
hkClass hkLinkedCollidableClass_330b1(
	"hkLinkedCollidable",
	&hkCollidableClass_330b1,
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
hkClass hkBroadPhaseHandleClass_330b1(
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
hkClass hkConvexPieceStreamDataClass_330b1(
	"hkConvexPieceStreamData",
	&hkReferencedObjectClass_330b1,
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
hkClass hkMoppCodeCodeInfoClass_330b1(
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
	{ "info", &hkMoppCodeCodeInfoClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkMoppCodeClass_330b1(
	"hkMoppCode",
	&hkReferencedObjectClass_330b1,
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
	{ "twistMotor", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "coneMotor", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "planeMotor", &hkConstraintMotorClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPoweredRagdollConstraintDataClass_330b1(
	"hkPoweredRagdollConstraintData",
	&hkRagdollConstraintDataClass_330b1,
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
hkClass hkRagdollConstraintDataConstraintBasisAClass_330b1(
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
hkClass hkRagdollConstraintDataConstraintBasisBClass_330b1(
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
	{ "basisA", &hkRagdollConstraintDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkRagdollConstraintDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
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
		int s_defaultOffsets[9];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		hkReal m_angularLimitsTauFactor;
	};
	const hkRagdollConstraintData_DefaultStruct hkRagdollConstraintData_Default =
	{
		{-1,-1,-1,-1,-1,-1,-1,-1,HK_OFFSET_OF(hkRagdollConstraintData_DefaultStruct,m_angularLimitsTauFactor)},
		0.8f
	};
}
hkClass hkRagdollConstraintDataClass_330b1(
	"hkRagdollConstraintData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintDataClass_Members),
	int(sizeof(hkRagdollConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	&hkRagdollConstraintData_Default
	);
static hkInternalClassMember hkRagdollLimitsData_ConstraintBasisAClass_Members[] =
{
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollLimitsDataConstraintBasisAClass_330b1(
	"hkRagdollLimitsDataConstraintBasisA",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsData_ConstraintBasisAClass_Members),
	int(sizeof(hkRagdollLimitsData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkRagdollLimitsData_ConstraintBasisBClass_Members[] =
{
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollLimitsDataConstraintBasisBClass_330b1(
	"hkRagdollLimitsDataConstraintBasisB",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsData_ConstraintBasisBClass_Members),
	int(sizeof(hkRagdollLimitsData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkRagdollLimitsDataClass_Members[] =
{
	{ "basisA", &hkRagdollLimitsDataConstraintBasisAClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkRagdollLimitsDataConstraintBasisBClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollLimitsDataClass_330b1(
	"hkRagdollLimitsData",
	&hkConstraintDataClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsDataClass_Members),
	int(sizeof(hkRagdollLimitsDataClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkAngularDashpotActionClass_Members[] =
{
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkAngularDashpotActionClass_330b1(
	"hkAngularDashpotAction",
	&hkBinaryActionClass_330b1,
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
hkClass hkDashpotActionClass_330b1(
	"hkDashpotAction",
	&hkBinaryActionClass_330b1,
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
hkClass hkMotorActionClass_330b1(
	"hkMotorAction",
	&hkUnaryActionClass_330b1,
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
hkClass hkReorientActionClass_330b1(
	"hkReorientAction",
	&hkUnaryActionClass_330b1,
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
hkClass hkSpringActionClass_330b1(
	"hkSpringAction",
	&hkBinaryActionClass_330b1,
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
	{ "shapePhantom", &hkShapePhantomClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "keepDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "userPlanes", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxCharacterSpeedForSolver", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterMass", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxSlope", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "penetrationRecoverySpeed", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkCharacterProxyCinfoClass_330b1(
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
	{ "disabledEntities", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkDisableEntityCollisionFilterClass_330b1(
	"hkDisableEntityCollisionFilter",
	&hkCollisionFilterClass_330b1,
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
	{ "otherFilter", &hkCollisionFilterClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkConstrainedSystemFilterClass_330b1(
	"hkConstrainedSystemFilter",
	&hkCollisionFilterClass_330b1,
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
hkClass hkGroupCollisionFilterClass_330b1(
	"hkGroupCollisionFilter",
	&hkCollisionFilterClass_330b1,
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
	{ "a", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "b", &hkEntityClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPairwiseCollisionFilterCollisionPairClass_330b1(
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
	{ "disabledPairs", &hkPairwiseCollisionFilterCollisionPairClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPairwiseCollisionFilterClass_330b1(
	"hkPairwiseCollisionFilter",
	&hkCollisionFilterClass_330b1,
	0,
	HK_NULL,
	1,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPairwiseCollisionFilterClass_Members),
	int(sizeof(hkPairwiseCollisionFilterClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPoweredChainMapper_TargetClass_Members[] =
{
	{ "chain", &hkPoweredChainDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "infoIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPoweredChainMapperTargetClass_330b1(
	"hkPoweredChainMapperTarget",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPoweredChainMapper_TargetClass_Members),
	int(sizeof(hkPoweredChainMapper_TargetClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPoweredChainMapper_LinkInfoClass_Members[] =
{
	{ "firstTargetIdx", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTargets", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "limitConstraint", &hkConstraintInstanceClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPoweredChainMapperLinkInfoClass_330b1(
	"hkPoweredChainMapperLinkInfo",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPoweredChainMapper_LinkInfoClass_Members),
	int(sizeof(hkPoweredChainMapper_LinkInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkPoweredChainMapperClass_Members[] =
{
	{ "links", &hkPoweredChainMapperLinkInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "targets", &hkPoweredChainMapperTargetClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chains", &hkConstraintChainInstanceClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkPoweredChainMapperClass_330b1(
	"hkPoweredChainMapper",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkPoweredChainMapperClass_Members),
	int(sizeof(hkPoweredChainMapperClass_Members)/sizeof(hkInternalClassMember)),
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
hkClass hkMouseSpringActionClass_330b1(
	"hkMouseSpringAction",
	&hkUnaryActionClass_330b1,
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
	{ "rigidBody", &hkRigidBodyClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayObject", &hkxMeshClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rigidBodyFromDisplayObjectTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRigidBodyDisplayBindingClass_330b1(
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
	{ "bindings", &hkRigidBodyDisplayBindingClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "system", &hkPhysicsSystemClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkPhysicsSystemDisplayBindingClass_330b1(
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
	{ "rigidBodyBindings", &hkRigidBodyDisplayBindingClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "physicsSystemBindings", &hkPhysicsSystemDisplayBindingClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkDisplayBindingDataClass_330b1(
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
	{ "worldCinfo", &hkWorldCinfoClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "systems", &hkPhysicsSystemClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkPhysicsDataClass_330b1(
	"hkPhysicsData",
	&hkReferencedObjectClass_330b1,
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
hkClass hkSerializedDisplayMarkerClass_330b1(
	"hkSerializedDisplayMarker",
	&hkReferencedObjectClass_330b1,
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
	{ "markers", &hkSerializedDisplayMarkerClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkSerializedDisplayMarkerListClass_330b1(
	"hkSerializedDisplayMarkerList",
	&hkReferencedObjectClass_330b1,
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
	{ "rb", &hkRigidBodyClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "localToDisplay", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_330b1(
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
	{ "transforms", &hkSerializedDisplayRbTransformsDisplayTransformPairClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkSerializedDisplayRbTransformsClass_330b1(
	"hkSerializedDisplayRbTransforms",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDataWheelComponentParamsClass_330b1(
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
	{ "wheelParams", &hkVehicleDataWheelComponentParamsClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numWheelsPerAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT8, 0, 0, 0 },
	{ "frictionDescription", &hkVehicleFrictionDescriptionClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisFrictionInertiaInvDiag", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "alreadyInitialised", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDataClass_330b1(
	"hkVehicleData",
	&hkReferencedObjectClass_330b1,
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
	{ "contactPoint", &hkContactPointClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
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
hkClass hkVehicleInstanceWheelInfoClass_330b1(
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
	{ "data", &hkVehicleDataClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "driverInput", &hkVehicleDriverInputClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "steering", &hkVehicleSteeringClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "engine", &hkVehicleEngineClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transmission", &hkVehicleTransmissionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "brake", &hkVehicleBrakeClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "suspension", &hkVehicleSuspensionClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "aerodynamics", &hkVehicleAerodynamicsClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelCollide", &hkVehicleWheelCollideClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tyreMarks", &hkTyremarksInfoClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chassis", &hkRigidBodyClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "velocityDamper", &hkVehicleVelocityDamperClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsInfo", &hkVehicleInstanceWheelInfoClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "frictionStatus", &hkVehicleFrictionStatusClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deviceStatus", &hkVehicleDriverInputStatusClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
hkClass hkVehicleInstanceClass_330b1(
	"hkVehicleInstance",
	&hkUnaryActionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleInstanceClass_Members),
	int(sizeof(hkVehicleInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleAerodynamicsClass_330b1(
	"hkVehicleAerodynamics",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultAerodynamicsClass_330b1(
	"hkVehicleDefaultAerodynamics",
	&hkVehicleAerodynamicsClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAerodynamicsClass_Members),
	int(sizeof(hkVehicleDefaultAerodynamicsClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleBrakeClass_330b1(
	"hkVehicleBrake",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_330b1(
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
	{ "wheelBrakingProperties", &hkVehicleDefaultBrakeWheelBrakingPropertiesClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsMinTimeToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleDefaultBrakeClass_330b1(
	"hkVehicleDefaultBrake",
	&hkVehicleBrakeClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultBrakeClass_Members),
	int(sizeof(hkVehicleDefaultBrakeClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleDriverInputStatusClass_330b1(
	"hkVehicleDriverInputStatus",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL
	);
hkClass hkVehicleDriverInputClass_330b1(
	"hkVehicleDriverInput",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDriverInputAnalogStatusClass_330b1(
	"hkVehicleDriverInputAnalogStatus",
	&hkVehicleDriverInputStatusClass_330b1,
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
hkClass hkVehicleDefaultAnalogDriverInputClass_330b1(
	"hkVehicleDefaultAnalogDriverInput",
	&hkVehicleDriverInputClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAnalogDriverInputClass_Members),
	int(sizeof(hkVehicleDefaultAnalogDriverInputClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleEngineClass_330b1(
	"hkVehicleEngine",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultEngineClass_330b1(
	"hkVehicleDefaultEngine",
	&hkVehicleEngineClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultEngineClass_Members),
	int(sizeof(hkVehicleDefaultEngineClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionDescription_AxisDescriptionClass_Members[] =
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
hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_330b1(
	"hkVehicleFrictionDescriptionAxisDescription",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescription_AxisDescriptionClass_Members),
	int(sizeof(hkVehicleFrictionDescription_AxisDescriptionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionDescriptionClass_Members[] =
{
	{ "wheelDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axleDescr", &hkVehicleFrictionDescriptionAxisDescriptionClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionDescriptionClass_330b1(
	"hkVehicleFrictionDescription",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescriptionClass_Members),
	int(sizeof(hkVehicleFrictionDescriptionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionStatus_AxisStatusClass_Members[] =
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
hkClass hkVehicleFrictionStatusAxisStatusClass_330b1(
	"hkVehicleFrictionStatusAxisStatus",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatus_AxisStatusClass_Members),
	int(sizeof(hkVehicleFrictionStatus_AxisStatusClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionStatusClass_Members[] =
{
	{ "axis", &hkVehicleFrictionStatusAxisStatusClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionStatusClass_330b1(
	"hkVehicleFrictionStatus",
	HK_NULL,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatusClass_Members),
	int(sizeof(hkVehicleFrictionStatusClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleSteeringClass_330b1(
	"hkVehicleSteering",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultSteeringClass_330b1(
	"hkVehicleDefaultSteering",
	&hkVehicleSteeringClass_330b1,
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
hkClass hkVehicleSuspensionSuspensionWheelParametersClass_330b1(
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
	{ "wheelParams", &hkVehicleSuspensionSuspensionWheelParametersClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVehicleSuspensionClass_330b1(
	"hkVehicleSuspension",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_330b1(
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
	{ "wheelSpringParams", &hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkVehicleDefaultSuspensionClass_330b1(
	"hkVehicleDefaultSuspension",
	&hkVehicleSuspensionClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSuspensionClass_Members),
	int(sizeof(hkVehicleDefaultSuspensionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleTransmissionClass_330b1(
	"hkVehicleTransmission",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultTransmissionClass_330b1(
	"hkVehicleDefaultTransmission",
	&hkVehicleTransmissionClass_330b1,
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
hkClass hkTyremarkPointClass_330b1(
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
	{ "tyremarkPoints", &hkTyremarkPointClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkTyremarksWheelClass_330b1(
	"hkTyremarksWheel",
	&hkReferencedObjectClass_330b1,
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
	{ "tyremarksWheel", &hkTyremarksWheelClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkTyremarksInfoClass_330b1(
	"hkTyremarksInfo",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkTyremarksInfoClass_Members),
	int(sizeof(hkTyremarksInfoClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
hkClass hkVehicleVelocityDamperClass_330b1(
	"hkVehicleVelocityDamper",
	&hkReferencedObjectClass_330b1,
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
hkClass hkVehicleDefaultVelocityDamperClass_330b1(
	"hkVehicleDefaultVelocityDamper",
	&hkVehicleVelocityDamperClass_330b1,
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
hkClass hkVehicleWheelCollideClass_330b1(
	"hkVehicleWheelCollide",
	&hkReferencedObjectClass_330b1,
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
hkClass hkRejectRayChassisListenerClass_330b1(
	"hkRejectRayChassisListener",
	&hkReferencedObjectClass_330b1,
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
	{ "phantom", &hkAabbPhantomClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rejectRayChassisListener", &hkRejectRayChassisListenerClass_330b1, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleRaycastWheelCollideClass_330b1(
	"hkVehicleRaycastWheelCollide",
	&hkVehicleWheelCollideClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkVehicleRaycastWheelCollideClass_Members),
	int(sizeof(hkVehicleRaycastWheelCollideClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);
static hkInternalClassMember hkRagdollInstanceClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_330b1, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_330b1, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkRagdollInstanceClass_330b1(
	"hkRagdollInstance",
	&hkReferencedObjectClass_330b1,
	0,
	HK_NULL,
	0,
	HK_NULL,
	0,
	reinterpret_cast<const hkClassMember*>(hkRagdollInstanceClass_Members),
	int(sizeof(hkRagdollInstanceClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL
	);

extern hkClass* const hkHavok330b1Classes[];
hkClass* const hkHavok330b1Classes[] =
{
	&hkBaseObjectClass_330b1,
	&hkReferencedObjectClass_330b1,
	&hkClassClass_330b1,
	&hkClassEnumItemClass_330b1,
	&hkClassEnumClass_330b1,
	&hkClassMemberClass_330b1,
	&hkMonitorStreamStringMapStringMapClass_330b1,
	&hkMonitorStreamStringMapClass_330b1,
	&hkMonitorStreamFrameInfoClass_330b1,
	&hkAabbClass_330b1,
	&hkContactPointClass_330b1,
	&hkContactPointMaterialClass_330b1,
	&hkMotionStateClass_330b1,
	&hkSphereClass_330b1,
	&hkSweptTransformClass_330b1,
	&hkxAnimatedFloatClass_330b1,
	&hkxAnimatedMatrixClass_330b1,
	&hkxAnimatedQuaternionClass_330b1,
	&hkxAnimatedVectorClass_330b1,
	&hkxAttributeClass_330b1,
	&hkxAttributeGroupClass_330b1,
	&hkxSparselyAnimatedBoolClass_330b1,
	&hkxSparselyAnimatedEnumClass_330b1,
	&hkxSparselyAnimatedIntClass_330b1,
	&hkxSparselyAnimatedStringStringTypeClass_330b1,
	&hkxSparselyAnimatedStringClass_330b1,
	&hkxCameraClass_330b1,
	&hkxNodeAnnotationDataClass_330b1,
	&hkxNodeClass_330b1,
	&hkxLightClass_330b1,
	&hkxMaterialTextureStageClass_330b1,
	&hkxMaterialClass_330b1,
	&hkxMaterialEffectClass_330b1,
	&hkxTextureFileClass_330b1,
	&hkxTextureInplaceClass_330b1,
	&hkxIndexBufferClass_330b1,
	&hkxMeshClass_330b1,
	&hkxMeshSectionClass_330b1,
	&hkxVertexBufferClass_330b1,
	&hkxVertexFormatClass_330b1,
	&hkxVertexP4N4C1T2Class_330b1,
	&hkxVertexP4N4T4B4C1T2Class_330b1,
	&hkxVertexP4N4T4B4W4I4C1Q2Class_330b1,
	&hkxVertexP4N4T4B4W4I4Q4Class_330b1,
	&hkxVertexP4N4W4I4C1Q2Class_330b1,
	&hkxSceneClass_330b1,
	&hkxSkinBindingClass_330b1,
	&hkPackfileHeaderClass_330b1,
	&hkPackfileSectionHeaderClass_330b1,
	&hkRootLevelContainerNamedVariantClass_330b1,
	&hkRootLevelContainerClass_330b1,
	&hkVersioningExceptionsArrayVersioningExceptionClass_330b1,
	&hkVersioningExceptionsArrayClass_330b1,
	&hkAnimationContainerClass_330b1,
	&hkAnimationBindingClass_330b1,
	&hkAnnotationTrackAnnotationClass_330b1,
	&hkAnnotationTrackClass_330b1,
	&hkSkeletalAnimationClass_330b1,
	&hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_330b1,
	&hkDeltaCompressedSkeletalAnimationClass_330b1,
	&hkInterleavedSkeletalAnimationClass_330b1,
	&hkWaveletSkeletalAnimationQuantizationFormatClass_330b1,
	&hkWaveletSkeletalAnimationClass_330b1,
	&hkMeshBindingMappingClass_330b1,
	&hkMeshBindingClass_330b1,
	&hkSkeletonMapperClass_330b1,
	&hkSkeletonMapperDataSimpleMappingClass_330b1,
	&hkSkeletonMapperDataChainMappingClass_330b1,
	&hkSkeletonMapperDataClass_330b1,
	&hkAnimatedReferenceFrameClass_330b1,
	&hkDefaultAnimatedReferenceFrameClass_330b1,
	&hkBoneClass_330b1,
	&hkBoneAttachmentClass_330b1,
	&hkSkeletonClass_330b1,
	&hkCdBodyClass_330b1,
	&hkCollidableClass_330b1,
	&hkShapeCollectionFilterClass_330b1,
	&hkTypedBroadPhaseHandleClass_330b1,
	&hkCollidableCollidableFilterClass_330b1,
	&hkCollisionFilterClass_330b1,
	&hkRayCollidableFilterClass_330b1,
	&hkGroupFilterClass_330b1,
	&hkCollisionFilterListClass_330b1,
	&hkNullCollisionFilterClass_330b1,
	&hkRayShapeCollectionFilterClass_330b1,
	&hkShapeClass_330b1,
	&hkShapeRayCastInputClass_330b1,
	&hkBoxShapeClass_330b1,
	&hkBvShapeClass_330b1,
	&hkBvTreeShapeClass_330b1,
	&hkCapsuleShapeClass_330b1,
	&hkShapeCollectionClass_330b1,
	&hkConvexShapeClass_330b1,
	&hkConvexListShapeClass_330b1,
	&hkConvexPieceMeshShapeClass_330b1,
	&hkConvexTransformShapeClass_330b1,
	&hkConvexTranslateShapeClass_330b1,
	&hkConvexVerticesShapeFourVectorsClass_330b1,
	&hkConvexVerticesShapeClass_330b1,
	&hkCylinderShapeClass_330b1,
	&hkFastMeshShapeClass_330b1,
	&hkHeightFieldShapeClass_330b1,
	&hkListShapeChildInfoClass_330b1,
	&hkListShapeClass_330b1,
	&hkMeshMaterialClass_330b1,
	&hkMeshShapeSubpartClass_330b1,
	&hkMeshShapeClass_330b1,
	&hkMoppBvTreeShapeClass_330b1,
	&hkMultiRayShapeRayClass_330b1,
	&hkMultiRayShapeClass_330b1,
	&hkMultiSphereShapeClass_330b1,
	&hkPhantomCallbackShapeClass_330b1,
	&hkPlaneShapeClass_330b1,
	&hkSampledHeightFieldShapeClass_330b1,
	&hkSimpleMeshShapeTriangleClass_330b1,
	&hkSimpleMeshShapeClass_330b1,
	&hkSphereShapeClass_330b1,
	&hkSphereRepShapeClass_330b1,
	&hkStorageMeshShapeSubpartStorageClass_330b1,
	&hkStorageMeshShapeClass_330b1,
	&hkStorageSampledHeightFieldShapeClass_330b1,
	&hkTransformShapeClass_330b1,
	&hkTriangleShapeClass_330b1,
	&hkTriSampledHeightFieldBvTreeShapeClass_330b1,
	&hkTriSampledHeightFieldCollectionClass_330b1,
	&hkActionClass_330b1,
	&hkArrayActionClass_330b1,
	&hkBinaryActionClass_330b1,
	&hkUnaryActionClass_330b1,
	&hkMaterialClass_330b1,
	&hkPropertyValueClass_330b1,
	&hkPropertyClass_330b1,
	&hkConstraintDataClass_330b1,
	&hkConstraintInfoClass_330b1,
	&hkConstraintInstanceClass_330b1,
	&hkBallAndSocketConstraintDataClass_330b1,
	&hkRelativeOrientationConstraintDataClass_330b1,
	&hkHingeConstraintDataConstraintBasisAClass_330b1,
	&hkHingeConstraintDataConstraintBasisBClass_330b1,
	&hkHingeConstraintDataClass_330b1,
	&hkLimitedHingeConstraintDataConstraintBasisAClass_330b1,
	&hkLimitedHingeConstraintDataConstraintBasisBClass_330b1,
	&hkLimitedHingeConstraintDataClass_330b1,
	&hkLinearParametricCurveClass_330b1,
	&hkParametricCurveClass_330b1,
	&hkPointToPathConstraintDataClass_330b1,
	&hkPointToPlaneConstraintDataClass_330b1,
	&hkPoweredHingeConstraintDataClass_330b1,
	&hkPrismaticConstraintDataConstraintBasisAClass_330b1,
	&hkPrismaticConstraintDataConstraintBasisBClass_330b1,
	&hkPrismaticConstraintDataClass_330b1,
	&hkStiffSpringConstraintDataClass_330b1,
	&hkWheelConstraintDataConstraintBasisAClass_330b1,
	&hkWheelConstraintDataConstraintBasisBClass_330b1,
	&hkWheelConstraintDataClass_330b1,
	&hkBreakableConstraintDataClass_330b1,
	&hkConstraintChainDataClass_330b1,
	&hkConstraintChainInstanceClass_330b1,
	&hkConstraintChainInstanceActionClass_330b1,
	&hkBallSocketChainDataConstraintInfoClass_330b1,
	&hkBallSocketChainDataClass_330b1,
	&hkHingeLimitsDataConstraintBasisAClass_330b1,
	&hkHingeLimitsDataConstraintBasisBClass_330b1,
	&hkHingeLimitsDataClass_330b1,
	&hkPoweredChainDataConstraintInfoClass_330b1,
	&hkPoweredChainDataClass_330b1,
	&hkStiffSpringChainDataConstraintInfoClass_330b1,
	&hkStiffSpringChainDataClass_330b1,
	&hkGenericConstraintDataClass_330b1,
	&hkGenericConstraintDataSchemeClass_330b1,
	&hkMalleableConstraintDataClass_330b1,
	&hkConstraintMotorClass_330b1,
	&hkLimitedForceConstraintMotorClass_330b1,
	&hkPositionConstraintMotorClass_330b1,
	&hkSpringDamperConstraintMotorClass_330b1,
	&hkVelocityConstraintMotorClass_330b1,
	&hkPulleyConstraintDataClass_330b1,
	&hkEntityClass_330b1,
	&hkEntityDeactivatorClass_330b1,
	&hkFakeRigidBodyDeactivatorClass_330b1,
	&hkRigidBodyClass_330b1,
	&hkRigidBodyDeactivatorClass_330b1,
	&hkSpatialRigidBodyDeactivatorSampleClass_330b1,
	&hkSpatialRigidBodyDeactivatorClass_330b1,
	&hkMotionClass_330b1,
	&hkBoxMotionClass_330b1,
	&hkFixedRigidMotionClass_330b1,
	&hkKeyframedRigidMotionClass_330b1,
	&hkRigidMotionClass_330b1,
	&hkSphereMotionClass_330b1,
	&hkStabilizedBoxMotionClass_330b1,
	&hkStabilizedSphereMotionClass_330b1,
	&hkThinBoxMotionClass_330b1,
	&hkAabbPhantomClass_330b1,
	&hkCachingShapePhantomClass_330b1,
	&hkPhantomClass_330b1,
	&hkShapePhantomClass_330b1,
	&hkSimpleShapePhantomClass_330b1,
	&hkMultiThreadLockClass_330b1,
	&hkPhysicsSystemClass_330b1,
	&hkWorldCinfoClass_330b1,
	&hkWorldObjectClass_330b1,
	&hkWorldMemoryWatchDogClass_330b1,
	&hkLinkedCollidableClass_330b1,
	&hkBroadPhaseHandleClass_330b1,
	&hkConvexPieceStreamDataClass_330b1,
	&hkMoppCodeCodeInfoClass_330b1,
	&hkMoppCodeClass_330b1,
	&hkPoweredRagdollConstraintDataClass_330b1,
	&hkRagdollConstraintDataConstraintBasisAClass_330b1,
	&hkRagdollConstraintDataConstraintBasisBClass_330b1,
	&hkRagdollConstraintDataClass_330b1,
	&hkRagdollLimitsDataConstraintBasisAClass_330b1,
	&hkRagdollLimitsDataConstraintBasisBClass_330b1,
	&hkRagdollLimitsDataClass_330b1,
	&hkAngularDashpotActionClass_330b1,
	&hkDashpotActionClass_330b1,
	&hkMotorActionClass_330b1,
	&hkReorientActionClass_330b1,
	&hkSpringActionClass_330b1,
	&hkCharacterProxyCinfoClass_330b1,
	&hkDisableEntityCollisionFilterClass_330b1,
	&hkConstrainedSystemFilterClass_330b1,
	&hkGroupCollisionFilterClass_330b1,
	&hkPairwiseCollisionFilterCollisionPairClass_330b1,
	&hkPairwiseCollisionFilterClass_330b1,
	&hkPoweredChainMapperTargetClass_330b1,
	&hkPoweredChainMapperLinkInfoClass_330b1,
	&hkPoweredChainMapperClass_330b1,
	&hkMouseSpringActionClass_330b1,
	&hkRigidBodyDisplayBindingClass_330b1,
	&hkPhysicsSystemDisplayBindingClass_330b1,
	&hkDisplayBindingDataClass_330b1,
	&hkPhysicsDataClass_330b1,
	&hkSerializedDisplayMarkerClass_330b1,
	&hkSerializedDisplayMarkerListClass_330b1,
	&hkSerializedDisplayRbTransformsDisplayTransformPairClass_330b1,
	&hkSerializedDisplayRbTransformsClass_330b1,
	&hkVehicleDataWheelComponentParamsClass_330b1,
	&hkVehicleDataClass_330b1,
	&hkVehicleInstanceWheelInfoClass_330b1,
	&hkVehicleInstanceClass_330b1,
	&hkVehicleAerodynamicsClass_330b1,
	&hkVehicleDefaultAerodynamicsClass_330b1,
	&hkVehicleBrakeClass_330b1,
	&hkVehicleDefaultBrakeWheelBrakingPropertiesClass_330b1,
	&hkVehicleDefaultBrakeClass_330b1,
	&hkVehicleDriverInputStatusClass_330b1,
	&hkVehicleDriverInputClass_330b1,
	&hkVehicleDriverInputAnalogStatusClass_330b1,
	&hkVehicleDefaultAnalogDriverInputClass_330b1,
	&hkVehicleEngineClass_330b1,
	&hkVehicleDefaultEngineClass_330b1,
	&hkVehicleFrictionDescriptionAxisDescriptionClass_330b1,
	&hkVehicleFrictionDescriptionClass_330b1,
	&hkVehicleFrictionStatusAxisStatusClass_330b1,
	&hkVehicleFrictionStatusClass_330b1,
	&hkVehicleSteeringClass_330b1,
	&hkVehicleDefaultSteeringClass_330b1,
	&hkVehicleSuspensionSuspensionWheelParametersClass_330b1,
	&hkVehicleSuspensionClass_330b1,
	&hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_330b1,
	&hkVehicleDefaultSuspensionClass_330b1,
	&hkVehicleTransmissionClass_330b1,
	&hkVehicleDefaultTransmissionClass_330b1,
	&hkTyremarkPointClass_330b1,
	&hkTyremarksWheelClass_330b1,
	&hkTyremarksInfoClass_330b1,
	&hkVehicleVelocityDamperClass_330b1,
	&hkVehicleDefaultVelocityDamperClass_330b1,
	&hkVehicleWheelCollideClass_330b1,
	&hkRejectRayChassisListenerClass_330b1,
	&hkVehicleRaycastWheelCollideClass_330b1,
	&hkRagdollInstanceClass_330b1,
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
