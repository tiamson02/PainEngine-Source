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

//
// Class hkAabb
//
static hkInternalClassMember hkAabbClass_Members[] =
{
	{ "min", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "max", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkAabbClass_300;
hkClass hkAabbClass_300(
	"hkAabb",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAabbClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/phantom/hkAabbPhantom.h'



// External pointer and enum types
extern hkClass hkCollidableClass_300;

//
// Class hkAabbPhantom
//
static hkInternalClassMember hkAabbPhantomClass_Members[] =
{
	{ "aabb", &hkAabbClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "overlappingCollidables", &hkCollidableClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkPhantomClass_300;

extern hkClass hkAabbPhantomClass_300;
hkClass hkAabbPhantomClass_300(
	"hkAabbPhantom",
	&hkPhantomClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAabbPhantomClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/action/hkAction.h'



// External pointer and enum types
extern hkClass hkSimulationIslandClass_300;
extern hkClass hkWorldClass_300;

//
// Class hkAction
//
static hkInternalClassMember hkActionClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "island", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
extern hkClass hkReferencedObjectClass_300;

extern hkClass hkActionClass_300;
hkClass hkActionClass_300(
	"hkAction",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkActionClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/actions/angulardashpot/hkAngularDashpotAction.h'



//
// Class hkAngularDashpotAction
//
static hkInternalClassMember hkAngularDashpotActionClass_Members[] =
{
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkBinaryActionClass_300;

extern hkClass hkAngularDashpotActionClass_300;
hkClass hkAngularDashpotActionClass_300(
	"hkAngularDashpotAction",
	&hkBinaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAngularDashpotActionClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/hkAnimationBinding.h'



// External pointer and enum types
extern hkClass hkInt16Class_300;
extern hkClass hkSkeletalAnimationClass_300;

//
// Class hkAnimationBinding
//
static hkInternalClassMember hkAnimationBindingClass_Members[] =
{
	{ "animation", &hkSkeletalAnimationClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 }
};
extern hkClass hkAnimationBindingClass_300;
hkClass hkAnimationBindingClass_300(
	"hkAnimationBinding",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnimationBindingClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/hkAnimationContainer.h'



// External pointer and enum types
extern hkClass hkBoneAttachmentClass_300;
extern hkClass hkMeshBindingClass_300;
extern hkClass hkSkeletonClass_300;

//
// Class hkAnimationContainer
//
static hkInternalClassMember hkAnimationContainerClass_Members[] =
{
	{ "skeletons", &hkSkeletonClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "animations", &hkSkeletalAnimationClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindings", &hkAnimationBindingClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "attachments", &hkBoneAttachmentClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skins", &hkMeshBindingClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkAnimationContainerClass_300;
hkClass hkAnimationContainerClass_300(
	"hkAnimationContainer",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnimationContainerClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/hkAnimationTrack.h'



// External pointer and enum types
extern hkClass AnnotationClass_300;

//
// Class hkAnimationTrack
//

//
// Class hkAnimationTrack::Annotation
//
static hkInternalClassMember hkAnimationTrack_AnnotationClass_Members[] =
{
	{ "time", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "text", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
extern hkClass hkAnimationTrackAnnotationClass_300;
hkClass hkAnimationTrackAnnotationClass_300(
	"hkAnimationTrackhkAnimationTrackAnnotation",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnimationTrack_AnnotationClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkAnimationTrackClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "annotations", &hkAnimationTrackAnnotationClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkAnimationTrackClass_300;
hkClass hkAnimationTrackClass_300(
	"hkAnimationTrack",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnimationTrackClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/action/hkArrayAction.h'



// External pointer and enum types
extern hkClass hkEntityClass_300;

//
// Class hkArrayAction
//
static hkInternalClassMember hkArrayActionClass_Members[] =
{
	{ "entities", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkArrayActionClass_300;
hkClass hkArrayActionClass_300(
	"hkArrayAction",
	&hkActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkArrayActionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/ballandsocket/hkBallAndSocketConstraintData.h'



// External pointer and enum types
extern hkClass hkSolverResultsClass_300;

//
// Class hkBallAndSocketConstraintData
//

//
// Class hkBallAndSocketConstraintData::Runtime
//
static hkInternalClassMember hkBallAndSocketConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 3, 0, 0 }
};
extern hkClass hkBallAndSocketConstraintDataRuntimeClass_300;
hkClass hkBallAndSocketConstraintDataRuntimeClass_300(
	"hkBallAndSocketConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBallAndSocketConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkBallAndSocketConstraintDataClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConstraintDataClass_300;

extern hkClass hkBallAndSocketConstraintDataClass_300;
hkClass hkBallAndSocketConstraintDataClass_300(
	"hkBallAndSocketConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBallAndSocketConstraintDataClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkbase/baseobject/hkBaseObject.h'



//
// Class hkBaseObject
//
extern hkClass hkBaseObjectClass_300;
hkClass hkBaseObjectClass_300(
	"hkBaseObject",
	HK_NULL, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/action/hkBinaryAction.h'



// External pointer and enum types

//
// Class hkBinaryAction
//
static hkInternalClassMember hkBinaryActionClass_Members[] =
{
	{ "entityA", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entityB", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkBinaryActionClass_300(
	"hkBinaryAction",
	&hkActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBinaryActionClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/motor/hkBlendingMotor.h'



//
// Class hkBlendingMotor
//
static hkInternalClassMember hkBlendingMotorClass_Members[] =
{
	{ "targetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "invSpringConstant", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMotorControllerClass_300;

extern hkClass hkBlendingMotorClass_300;
hkClass hkBlendingMotorClass_300(
	"hkBlendingMotor",
	&hkMotorControllerClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBlendingMotorClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/rig/hkBoneAttachment.h'



// External pointer and enum types

//
// Class hkBoneAttachment
//
static hkInternalClassMember hkBoneAttachmentClass_Members[] =
{
	{ "skeleton", &hkSkeletonClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "boneIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "parentWorld", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "attachment", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoneAttachmentClass_300(
	"hkBoneAttachment",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoneAttachmentClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/rig/hkBone.h'



//
// Class hkBone
//
static hkInternalClassMember hkBoneClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "initialTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lockTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkBoneClass_300;
hkClass hkBoneClass_300(
	"hkBone",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoneClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkBoxMotion.h'



//
// Class hkBoxMotion
//
static hkInternalClassMember hkBoxMotionClass_Members[] =
{
	{ "inertiaAndMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkRigidMotionClass_300;

extern hkClass hkBoxMotionClass_300;
hkClass hkBoxMotionClass_300(
	"hkBoxMotion",
	&hkRigidMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoxMotionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/box/hkBoxShape.h'



//
// Class hkBoxShape
//
static hkInternalClassMember hkBoxShapeClass_Members[] =
{
	{ "halfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConvexShapeClass_300;

extern hkClass hkBoxShapeClass_300;
hkClass hkBoxShapeClass_300(
	"hkBoxShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoxShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/breakable/hkBreakableConstraintData.h'



// External pointer and enum types
extern hkClass hkBreakableListenerClass_300;

//
// Class hkBreakableConstraintData
//
static hkInternalClassMember hkBreakableConstraintDataClass_Members[] =
{
	{ "constraintData", &hkConstraintDataClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "solverResultLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "removeWhenBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "breakNonRemovedInstances", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "listener", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkBreakableConstraintDataClass_300;
hkClass hkBreakableConstraintDataClass_300(
	"hkBreakableConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBreakableConstraintDataClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkinternal/collide/broadphase/hkBroadPhaseHandle.h'



//
// Class hkBroadPhaseHandle
//
static hkInternalClassMember hkBroadPhaseHandleClass_Members[] =
{
	{ "id", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkBroadPhaseHandleClass_300;
hkClass hkBroadPhaseHandleClass_300(
	"hkBroadPhaseHandle",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBroadPhaseHandleClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/bv/hkBvShape.h'



// External pointer and enum types
extern hkClass hkShapeClass_300;

//
// Class hkBvShape
//
static hkInternalClassMember hkBvShapeClass_Members[] =
{
	{ "boundingVolumeShape", &hkShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childShape", &hkShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkBvShapeClass_300;
hkClass hkBvShapeClass_300(
	"hkBvShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBvShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/bvtree/hkBvTreeShape.h'



// External pointer and enum types
extern hkClass hkShapeCollectionClass_300;

//
// Class hkBvTreeShape
//
static hkInternalClassMember hkBvTreeShapeClass_Members[] =
{
	{ "shapeCollection", &hkShapeCollectionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkBvTreeShapeClass_300;
hkClass hkBvTreeShapeClass_300(
	"hkBvTreeShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBvTreeShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/phantom/hkCachingShapePhantom.h'



// External pointer and enum types
extern hkClass hkCollisionAgentClass_300;
extern hkClass hkCollisionDetailClass_300;

//
// Class hkCachingShapePhantom
//

//
// Class hkCachingShapePhantom::hkCollisionDetail
//
static hkInternalClassMember hkCachingShapePhantom_hkCollisionDetailClass_Members[] =
{
	{ "agent", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "collidable", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkCachingShapePhantomhkCollisionDetailClass_300;
hkClass hkCachingShapePhantomhkCollisionDetailClass_300(
	"hkCachingShapePhantomhkCollisionDetail",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCachingShapePhantom_hkCollisionDetailClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkCachingShapePhantomClass_Members[] =
{
	{ "collisionDetails", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
extern hkClass hkShapePhantomClass_300;

extern hkClass hkCachingShapePhantomClass_300;
hkClass hkCachingShapePhantomClass_300(
	"hkCachingShapePhantom",
	&hkShapePhantomClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCachingShapePhantomClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/capsule/hkCapsuleShape.h'



//
// Class hkCapsuleShape
//
static hkInternalClassMember hkCapsuleShapeClass_Members[] =
{
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkCapsuleShapeClass_300;
hkClass hkCapsuleShapeClass_300(
	"hkCapsuleShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCapsuleShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/agent/hkCdBody.h'



// External pointer and enum types
extern hkClass hkCdBodyClass_300;

//
// Class hkCdBody
//
static hkInternalClassMember hkCdBodyClass_Members[] =
{
	{ "shape", &hkShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motion", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "parent", &hkCdBodyClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkCdBodyClass_300(
	"hkCdBody",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCdBodyClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/charactercontrol/characterproxy/hkCharacterProxyCinfo.h'



// External pointer and enum types

//
// Class hkCharacterProxyCinfo
//
static hkInternalClassMember hkCharacterProxyCinfoClass_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dynamicFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "staticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraUpStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraDownStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shapePhantom", &hkShapePhantomClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "keepDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "userPlanes", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "conservativeWalking", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkCharacterProxyCinfoClass_300;
hkClass hkCharacterProxyCinfoClass_300(
	"hkCharacterProxyCinfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCharacterProxyCinfoClass_Members),
	12, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkbase/class/hkClass.h'



// External pointer and enum types
extern hkClass hkClassClass_300;
extern hkClass hkClassEnumClass_300;
extern hkClass hkClassMemberClass_300;

//
// Class hkClass
//
static hkInternalClassMember hkClassClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "parent", &hkClassClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "objectSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numImplementedInterfaces", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "declaredEnums", &hkClassEnumClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "declaredMembers", &hkClassMemberClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "hasVtable", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkClassClass_300(
	"hkClass",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkClassClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkbase/class/hkClassEnum.h'



// External pointer and enum types
extern hkClass ItemClass_300;

//
// Class hkClassEnum
//

//
// Class hkClassEnum::Item
//
static hkInternalClassMember hkClassEnum_ItemClass_Members[] =
{
	{ "value", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
extern hkClass hkClassEnumItemClass_300;
hkClass hkClassEnumItemClass_300(
	"hkClassEnumItem",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkClassEnum_ItemClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkClassEnumClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "items", &hkClassEnumItemClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkClassEnumClass_300(
	"hkClassEnum",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkClassEnumClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkbase/class/hkClassMember.h'



// External pointer and enum types

//
// Class hkClassMember
//

//
// Enum hkClassMember::Type
//
static const hkInternalClassEnumItem hkClassMemberTypeEnumItems_300[] =
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

//
// Enum hkClassMember::Flags
//
static const hkInternalClassEnumItem hkClassMemberFlagsEnumItems_300[] =
{
	{1, "POINTER_OPTIONAL"},
	{2, "POINTER_VOIDSTAR"},
	{8, "ENUM_8"},
	{16, "ENUM_16"},
	{32, "ENUM_32"},
	{64, "ARRAY_RAWDATA"},
};

//
// Enum hkClassMember::Range
//
static const hkInternalClassEnumItem hkClassMemberRangeEnumItems_300[] =
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
	{"Type", hkClassMemberTypeEnumItems_300, 30 },
	{"Flags", hkClassMemberFlagsEnumItems_300, 6 },
	{"Range", hkClassMemberRangeEnumItems_300, 7 }
};
extern const hkClassEnum* hkClassMemberTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[0]);
extern const hkClassEnum* hkClassMemberFlagsEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[1]);
extern const hkClassEnum* hkClassMemberRangeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[2]);
static hkInternalClassMember hkClassMemberClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "class", &hkClassClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "enum", &hkClassEnumClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "type", HK_NULL, hkClassMemberTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "subtype", HK_NULL, hkClassMemberTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "cArraySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkClassMemberClass_300(
	"hkClassMember",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkClassMemberEnums),
	3, // enums
	reinterpret_cast<const hkClassMember*>(hkClassMemberClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/agent/hkCollidable.h'



// External pointer and enum types
extern hkClass hkTypedBroadPhaseHandleClass_300;

//
// Class hkCollidable
//
static hkInternalClassMember hkCollidableClass_Members[] =
{
	{ "ownerOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseHandle", &hkTypedBroadPhaseHandleClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowedPenetrationDepth", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkCollidableClass_300(
	"hkCollidable",
	&hkCdBodyClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCollidableClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/hkCollidableCollidableFilter.h'



//
// Class hkCollidableCollidableFilter
//
extern hkClass hkCollidableCollidableFilterClass_300;
hkClass hkCollidableCollidableFilterClass_300(
	"hkCollidableCollidableFilter",
	HK_NULL, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/hkCollisionFilter.h'



//
// Class hkCollisionFilter
//

extern hkClass hkCollisionFilterClass_300;
hkClass hkCollisionFilterClass_300(
	"hkCollisionFilter",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	4, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/list/hkCollisionFilterList.h'



// External pointer and enum types

//
// Class hkCollisionFilterList
//
static hkInternalClassMember hkCollisionFilterListClass_Members[] =
{
	{ "collisionFilters", &hkCollisionFilterClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkCollisionFilterListClass_300;
hkClass hkCollisionFilterListClass_300(
	"hkCollisionFilterList",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCollisionFilterListClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/collide/filter/constrainedsystem/hkConstrainedSystemFilter.h'



//
// Class hkConstrainedSystemFilter
//

extern hkClass hkConstrainedSystemFilterClass_300;
hkClass hkConstrainedSystemFilterClass_300(
	"hkConstrainedSystemFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/hkConstraintData.h'


//
// Global
//

//
// Enum ::hkConstraintType
//
static const hkInternalClassEnumItem hkConstraintTypeEnumItems_300[] =
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
static const hkInternalClassEnum Enums[] = {
	{"hkConstraintType", hkConstraintTypeEnumItems_300, 15 }
};
extern const hkClassEnum* hkConstraintTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&Enums[0]);

//
// Class hkConstraintData
//

//
// Class hkConstraintData::RuntimeInfo
//
static hkInternalClassMember hkConstraintData_RuntimeInfoClass_Members[] =
{
	{ "sizeOfExternalRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numSolverResults", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConstraintDataRuntimeInfoClass_300;
hkClass hkConstraintDataRuntimeInfoClass_300(
	"hkConstraintDataRuntimeInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConstraintData_RuntimeInfoClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkConstraintDataClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkConstraintDataClass_300(
	"hkConstraintData",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConstraintDataClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkConstraintInfo
//
static hkInternalClassMember hkConstraintInfoClass_Members[] =
{
	{ "maxSizeOfJacobians", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sizeOfJacobians", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sizeOfSchemas", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numSolverResults", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConstraintInfoClass_300;
hkClass hkConstraintInfoClass_300(
	"hkConstraintInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConstraintInfoClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/hkConstraintInstance.h'



// External pointer and enum types
extern hkClass hkConstraintInternalClass_300;
extern hkClass hkConstraintOwnerClass_300;

//
// Class hkConstraintInstance
//

//
// Enum hkConstraintInstance::ConstraintPriority
//
static const hkInternalClassEnumItem hkConstraintInstanceConstraintPriorityEnumItems_300[] =
{
	{0, "PRIORITY_INVALID"},
	{1, "PRIORITY_PSI"},
	{2, "PRIORITY_TOI"},
	{3, "PRIORITY_TOI_HIGHER"},
};
static const hkInternalClassEnum hkConstraintInstanceEnums[] = {
	{"ConstraintPriority", hkConstraintInstanceConstraintPriorityEnumItems_300, 4 }
};
extern const hkClassEnum* hkConstraintInstanceConstraintPriorityEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[0]);
static hkInternalClassMember hkConstraintInstanceClass_Members[] =
{
	{ "owner", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "data", &hkConstraintDataClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entities", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 2, 0, 0 },
	{ "priority", HK_NULL, hkConstraintInstanceConstraintPriorityEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "wantRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internal", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkConstraintInstanceClass_300;
hkClass hkConstraintInstanceClass_300(
	"hkConstraintInstance",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkConstraintInstanceEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkConstraintInstanceClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/basetypes/hkContactPoint.h'



//
// Class hkContactPoint
//
static hkInternalClassMember hkContactPointClass_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "separatingNormal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkContactPointClass_300;
hkClass hkContactPointClass_300(
	"hkContactPoint",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkContactPointClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convexlist/hkConvexListShape.h'



//
// Class hkConvexListShape
//
static hkInternalClassMember hkConvexListShapeClass_Members[] =
{
	{ "minDistanceToUseConvexHullForGetClosestPoints", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkListShapeClass_300;

extern hkClass hkConvexListShapeClass_300;
hkClass hkConvexListShapeClass_300(
	"hkConvexListShape",
	&hkListShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexListShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convexpiecemesh/hkConvexPieceMeshShape.h'



// External pointer and enum types
extern hkClass hkConvexPieceStreamDataClass_300;

//
// Class hkConvexPieceMeshShape
//

//
// Class hkConvexPieceMeshShape::Stats
//
static hkInternalClassMember hkConvexPieceMeshShape_StatsClass_Members[] =
{
	{ "numTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numConvexPieces", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "avgNumTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxTrianglesPerConvexPiece", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConvexPieceMeshShapeStatsClass_300;
hkClass hkConvexPieceMeshShapeStatsClass_300(
	"hkConvexPieceMeshShapeStats",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexPieceMeshShape_StatsClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkConvexPieceMeshShapeClass_Members[] =
{
	{ "convexPieceStream", &hkConvexPieceStreamDataClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayMesh", &hkShapeCollectionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexPieceMeshShapeClass_300;
hkClass hkConvexPieceMeshShapeClass_300(
	"hkConvexPieceMeshShape",
	&hkShapeCollectionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexPieceMeshShapeClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkinternal/collide/convexpiecemesh/hkConvexPieceStreamData.h'



// External pointer and enum types
extern hkClass hkShapeKeyClass_300;
extern hkClass hkUint32Class_300;

//
// Class hkConvexPieceStreamData
//
static hkInternalClassMember hkConvexPieceStreamDataClass_Members[] =
{
	{ "convexPieceStream", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceOffsets", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceSingleTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};
hkClass hkConvexPieceStreamDataClass_300(
	"hkConvexPieceStreamData",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexPieceStreamDataClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convex/hkConvexShape.h'



//
// Class hkConvexShape
//
static hkInternalClassMember hkConvexShapeClass_Members[] =
{
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSphereRepShapeClass_300;

hkClass hkConvexShapeClass_300(
	"hkConvexShape",
	&hkSphereRepShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convextransform/hkConvexTransformShape.h'



// External pointer and enum types

//
// Class hkConvexTransformShape
//
static hkInternalClassMember hkConvexTransformShapeClass_Members[] =
{
	{ "childShape", &hkConvexShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexTransformShapeClass_300;
hkClass hkConvexTransformShapeClass_300(
	"hkConvexTransformShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexTransformShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convextranslate/hkConvexTranslateShape.h'



// External pointer and enum types

//
// Class hkConvexTranslateShape
//
static hkInternalClassMember hkConvexTranslateShapeClass_Members[] =
{
	{ "childShape", &hkConvexShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "translation", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexTranslateShapeClass_300;
hkClass hkConvexTranslateShapeClass_300(
	"hkConvexTranslateShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexTranslateShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/convexvertices/hkConvexVerticesShape.h'



// External pointer and enum types
extern hkClass FourVectorsClass_300;
extern hkClass hkVector4Class_300;

//
// Class hkConvexVerticesShape
//

//
// Class hkConvexVerticesShape::FourVectors
//
static hkInternalClassMember hkConvexVerticesShape_FourVectorsClass_Members[] =
{
	{ "x", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "y", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "z", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkConvexVerticesShapeFourVectorsClass_300;
hkClass hkConvexVerticesShapeFourVectorsClass_300(
	"hkConvexVerticesShapeFourVectors",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexVerticesShape_FourVectorsClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkConvexVerticesShapeClass_Members[] =
{
	{ "aabbHalfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotatedVertices", &hkConvexVerticesShapeFourVectorsClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeEquations", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};

extern hkClass hkConvexVerticesShapeClass_300;
hkClass hkConvexVerticesShapeClass_300(
	"hkConvexVerticesShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConvexVerticesShapeClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/cylinder/hkCylinderShape.h'



//
// Class hkCylinderShape
//
static hkInternalClassMember hkCylinderShapeClass_Members[] =
{
	{ "cylRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkCylinderShapeClass_300;
hkClass hkCylinderShapeClass_300(
	"hkCylinderShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCylinderShapeClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/actions/dashpot/hkDashpotAction.h'



//
// Class hkDashpotAction
//
static hkInternalClassMember hkDashpotActionClass_Members[] =
{
	{ "point", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "impulse", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkDashpotActionClass_300;
hkClass hkDashpotActionClass_300(
	"hkDashpotAction",
	&hkBinaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDashpotActionClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/motion/default/hkDefaultExtractedMotion.h'



// External pointer and enum types

//
// Class hkDefaultExtractedMotion
//

//
// Class hkDefaultExtractedMotion::MotionExtractionOptions
//
static hkInternalClassMember hkDefaultExtractedMotion_MotionExtractionOptionsClass_Members[] =
{
	{ "numSamples", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowUpDown", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowFrontBack", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowRightLeft", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowTurning", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forward", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkDefaultExtractedMotionMotionExtractionOptionsClass_300;
hkClass hkDefaultExtractedMotionMotionExtractionOptionsClass_300(
	"hkDefaultExtractedMotionMotionExtractionOptions",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDefaultExtractedMotion_MotionExtractionOptionsClass_Members),
	7, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkDefaultExtractedMotionClass_Members[] =
{
	{ "period", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forward", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "timeSlice", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motionTrack", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};
extern hkClass hkExtractedMotionClass_300;

extern hkClass hkDefaultExtractedMotionClass_300;
hkClass hkDefaultExtractedMotionClass_300(
	"hkDefaultExtractedMotion",
	&hkExtractedMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDefaultExtractedMotionClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/deltacompressed/hkDeltaCompressedAnimation.h'



// External pointer and enum types
extern hkClass hkDeltaCompressedAnimationQuantizationFormatClass_300;
extern hkClass hkRealClass_300;
extern hkClass hkUint16Class_300;
extern hkClass hkUint8Class_300;

//
// Class hkDeltaCompressedAnimation
//

//
// Class hkDeltaCompressedAnimation::CompressionParams
//
static hkInternalClassMember hkDeltaCompressedAnimation_CompressionParamsClass_Members[] =
{
	{ "quantizationBits", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absolutePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "relativePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotationTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scaleTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkDeltaCompressedAnimationCompressionParamsClass_300;
hkClass hkDeltaCompressedAnimationCompressionParamsClass_300(
	"hkDeltaCompressedAnimationCompressionParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedAnimation_CompressionParamsClass_Members),
	6, // members
	HK_NULL // defaults
	);

//
// Class hkDeltaCompressedAnimation::QuantizationFormat
//
static hkInternalClassMember hkDeltaCompressedAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkDeltaCompressedAnimationQuantizationFormatClass_300(
	"hkDeltaCompressedAnimationQuantizationFormat",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedAnimation_QuantizationFormatClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkDeltaCompressedAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkDeltaCompressedAnimationQuantizationFormatClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};

extern hkClass hkDeltaCompressedAnimationClass_300;
hkClass hkDeltaCompressedAnimationClass_300(
	"hkDeltaCompressedAnimation",
	&hkSkeletalAnimationClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedAnimationClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/collide/filter/disableentity/hkDisableEntityCollisionFilter.h'



// External pointer and enum types

//
// Class hkDisableEntityCollisionFilter
//
static hkInternalClassMember hkDisableEntityCollisionFilterClass_Members[] =
{
	{ "disabledEntities", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkDisableEntityCollisionFilterClass_300;
hkClass hkDisableEntityCollisionFilterClass_300(
	"hkDisableEntityCollisionFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDisableEntityCollisionFilterClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkEntity.h'



// External pointer and enum types
extern hkClass hkCollisionListenerClass_300;
extern hkClass hkEntityActivationListenerClass_300;
extern hkClass hkEntityDeactivatorClass_300;
extern hkClass hkEntityListenerClass_300;
extern hkClass hkMaterialClass_300;
extern hkClass hkMotionClass_300;

//
// Class hkEntity
//
static hkInternalClassMember hkEntityClass_Members[] =
{
	{ "motion", &hkMotionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simulationIsland", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkMaterialClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivator", &hkEntityDeactivatorClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
	{ "actions", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};
extern hkClass hkWorldObjectClass_300;

hkClass hkEntityClass_300(
	"hkEntity",
	&hkWorldObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkEntityClass_Members),
	17, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkEntityDeactivator.h'



//
// Class hkEntityDeactivator
//

hkClass hkEntityDeactivatorClass_300(
	"hkEntityDeactivator",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/motion/hkExtractedMotion.h'



//
// Class hkExtractedMotion
//

hkClass hkExtractedMotionClass_300(
	"hkExtractedMotion",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkFakeRigidBodyDeactivator.h'



//
// Class hkFakeRigidBodyDeactivator
//
extern hkClass hkRigidBodyDeactivatorClass_300;

extern hkClass hkFakeRigidBodyDeactivatorClass_300;
hkClass hkFakeRigidBodyDeactivatorClass_300(
	"hkFakeRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/fastmesh/hkFastMeshShape.h'



//
// Class hkFastMeshShape
//
extern hkClass hkMeshShapeClass_300;

extern hkClass hkFastMeshShapeClass_300;
hkClass hkFastMeshShapeClass_300(
	"hkFastMeshShape",
	&hkMeshShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkFixedRigidMotion.h'



//
// Class hkFixedRigidMotion
//
extern hkClass hkKeyframedRigidMotionClass_300;

extern hkClass hkFixedRigidMotionClass_300;
hkClass hkFixedRigidMotionClass_300(
	"hkFixedRigidMotion",
	&hkKeyframedRigidMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/constraintkit/hkGenericConstraintData.h'



// External pointer and enum types
extern hkClass hkGenericConstraintDataSchemeClass_300;

//
// Class hkGenericConstraintData
//
static hkInternalClassMember hkGenericConstraintDataClass_Members[] =
{
	{ "scheme", &hkGenericConstraintDataSchemeClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkGenericConstraintDataClass_300;
hkClass hkGenericConstraintDataClass_300(
	"hkGenericConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkGenericConstraintDataClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/constraintkit/hkGenericConstraintScheme.h'



// External pointer and enum types
extern hkClass hkConstraintModifierClass_300;
extern hkClass intClass_300;

//
// Class hkGenericConstraintDataScheme
//
static hkInternalClassMember hkGenericConstraintDataSchemeClass_Members[] =
{
	{ "info", &hkConstraintInfoClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "commands", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT32, 0, 0, 0 },
	{ "modifiers", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "motors", &hkMotorControllerClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkGenericConstraintDataSchemeClass_300(
	"hkGenericConstraintDataScheme",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkGenericConstraintDataSchemeClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/collide/filter/h1group/hkGroupCollisionFilter.h'



//
// Class hkGroupCollisionFilter
//
static hkInternalClassMember hkGroupCollisionFilterClass_Members[] =
{
	{ "noGroupCollisionEnabled", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionGroups", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 32, 0, 0 }
};

extern hkClass hkGroupCollisionFilterClass_300;
hkClass hkGroupCollisionFilterClass_300(
	"hkGroupCollisionFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkGroupCollisionFilterClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/group/hkGroupFilter.h'



//
// Class hkGroupFilter
//
static hkInternalClassMember hkGroupFilterClass_Members[] =
{
	{ "nextFreeSystemGroup", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionLookupTable", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 32, 0, 0 }
};

extern hkClass hkGroupFilterClass_300;
hkClass hkGroupFilterClass_300(
	"hkGroupFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkGroupFilterClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/heightfield/hkHeightFieldShape.h'



// External pointer and enum types
extern hkClass hkSphereClass_300;

//
// Class hkHeightFieldShape
//

//
// Class hkHeightFieldShape::CollideSpheresInput
//
static hkInternalClassMember hkHeightFieldShape_CollideSpheresInputClass_Members[] =
{
	{ "spheres", &hkSphereClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkHeightFieldShapeCollideSpheresInputClass_300;
hkClass hkHeightFieldShapeCollideSpheresInputClass_300(
	"hkHeightFieldShapeCollideSpheresInput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHeightFieldShape_CollideSpheresInputClass_Members),
	2, // members
	HK_NULL // defaults
	);

//
// Class hkHeightFieldShape::hkSphereCastInput
//
static hkInternalClassMember hkHeightFieldShape_hkSphereCastInputClass_Members[] =
{
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxExtraPenetration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkShapeRayCastInputClass_300;

extern hkClass hkHeightFieldShapehkSphereCastInputClass_300;
hkClass hkHeightFieldShapehkSphereCastInputClass_300(
	"hkHeightFieldShapehkSphereCastInput",
	&hkShapeRayCastInputClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHeightFieldShape_hkSphereCastInputClass_Members),
	2, // members
	HK_NULL // defaults
	);

extern hkClass hkHeightFieldShapeClass_300;
hkClass hkHeightFieldShapeClass_300(
	"hkHeightFieldShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/hinge/hkHingeConstraintData.h'



// External pointer and enum types
extern hkClass hkHingeConstraintDataConstraintBasisAClass_300;
extern hkClass hkHingeConstraintDataConstraintBasisBClass_300;

//
// Class hkHingeConstraintData
//

//
// Class hkHingeConstraintData::Runtime
//
static hkInternalClassMember hkHingeConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 5, 0, 0 }
};
extern hkClass hkHingeConstraintDataRuntimeClass_300;
hkClass hkHingeConstraintDataRuntimeClass_300(
	"hkHingeConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkHingeConstraintData::ConstraintBasisA
//
static hkInternalClassMember hkHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataConstraintBasisAClass_300(
	"hkHingeConstraintDataConstraintBasisA",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisAClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkHingeConstraintData::ConstraintBasisB
//
static hkInternalClassMember hkHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkHingeConstraintDataConstraintBasisBClass_300(
	"hkHingeConstraintDataConstraintBasisB",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisBClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkHingeConstraintDataClass_Members[] =
{
	{ "basisA", &hkHingeConstraintDataConstraintBasisAClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkHingeConstraintDataConstraintBasisBClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkHingeConstraintDataClass_300;
hkClass hkHingeConstraintDataClass_300(
	"hkHingeConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintDataClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/interleaved/hkInterleavedAnimation.h'



// External pointer and enum types
extern hkClass hkQsTransformClass_300;

//
// Class hkInterleavedAnimation
//
static hkInternalClassMember hkInterleavedAnimationClass_Members[] =
{
	{ "poses", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};

extern hkClass hkInterleavedAnimationClass_300;
hkClass hkInterleavedAnimationClass_300(
	"hkInterleavedAnimation",
	&hkSkeletalAnimationClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkInterleavedAnimationClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkKeyframedRigidMotion.h'



// External pointer and enum types

//
// Class hkKeyframedRigidMotion
//
static hkInternalClassMember hkKeyframedRigidMotionClass_Members[] =
{
	{ "savedMotion", &hkRigidMotionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkKeyframedRigidMotionClass_300(
	"hkKeyframedRigidMotion",
	&hkRigidMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkKeyframedRigidMotionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h'



// External pointer and enum types
extern hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_300;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_300;

//
// Class hkLimitedHingeConstraintData
//

//
// Class hkLimitedHingeConstraintData::Runtime
//
static hkInternalClassMember hkLimitedHingeConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "lastAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkLimitedHingeConstraintDataRuntimeClass_300;
hkClass hkLimitedHingeConstraintDataRuntimeClass_300(
	"hkLimitedHingeConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_RuntimeClass_Members),
	2, // members
	HK_NULL // defaults
	);

//
// Class hkLimitedHingeConstraintData::ConstraintBasisA
//
static hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_300(
	"hkLimitedHingeConstraintDataConstraintBasisA",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisAClass_Members),
	4, // members
	HK_NULL // defaults
	);

//
// Class hkLimitedHingeConstraintData::ConstraintBasisB
//
static hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perp2FreeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_300(
	"hkLimitedHingeConstraintDataConstraintBasisB",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisBClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkLimitedHingeConstraintDataClass_Members[] =
{
	{ "minAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisA", &hkLimitedHingeConstraintDataConstraintBasisAClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkLimitedHingeConstraintDataConstraintBasisBClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkLimitedHingeConstraintDataClass_300;
hkClass hkLimitedHingeConstraintDataClass_300(
	"hkLimitedHingeConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintDataClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/pointtopath/hkLinearParametricCurve.h'



// External pointer and enum types

//
// Class hkLinearParametricCurve
//
static hkInternalClassMember hkLinearParametricCurveClass_Members[] =
{
	{ "smoothingFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "closedLoop", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dirNotParallelToTangentAlongWholePath", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "points", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "distance", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
extern hkClass hkParametricCurveClass_300;

extern hkClass hkLinearParametricCurveClass_300;
hkClass hkLinearParametricCurveClass_300(
	"hkLinearParametricCurve",
	&hkParametricCurveClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLinearParametricCurveClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkinternal/collide/agent3/machine/nn/hkLinkedCollidable.h'



// External pointer and enum types
extern hkClass CollisionEntryClass_300;
extern hkClass hkAgentNnEntryClass_300;
extern hkClass hkLinkedCollidableClass_300;

//
// Class hkLinkedCollidable
//

//
// Class hkLinkedCollidable::CollisionEntry
//
static hkInternalClassMember hkLinkedCollidable_CollisionEntryClass_Members[] =
{
	{ "agentEntry", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "partner", &hkLinkedCollidableClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkLinkedCollidableCollisionEntryClass_300;
hkClass hkLinkedCollidableCollisionEntryClass_300(
	"hkLinkedCollidableCollisionEntry",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLinkedCollidable_CollisionEntryClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkLinkedCollidableClass_Members[] =
{
	{ "collisionEntries", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};

hkClass hkLinkedCollidableClass_300(
	"hkLinkedCollidable",
	&hkCollidableClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLinkedCollidableClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/list/hkListShape.h'



// External pointer and enum types
extern hkClass ChildInfoClass_300;

//
// Class hkListShape
//

//
// Class hkListShape::ChildInfo
//
static hkInternalClassMember hkListShape_ChildInfoClass_Members[] =
{
	{ "shape", &hkShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkListShapeChildInfoClass_300;
hkClass hkListShapeChildInfoClass_300(
	"hkListShapeChildInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkListShape_ChildInfoClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkListShapeClass_Members[] =
{
	{ "childInfo", &hkListShapeChildInfoClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkListShapeClass_300(
	"hkListShape",
	&hkShapeCollectionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkListShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/malleable/hkMalleableConstraintData.h'



// External pointer and enum types

//
// Class hkMalleableConstraintData
//
static hkInternalClassMember hkMalleableConstraintDataClass_Members[] =
{
	{ "constraintData", &hkConstraintDataClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkMalleableConstraintDataClass_300;
hkClass hkMalleableConstraintDataClass_300(
	"hkMalleableConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMalleableConstraintDataClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/common/hkMaterial.h'



//
// Class hkMaterial
//

//
// Enum hkMaterial::ResponseType
//
static const hkInternalClassEnumItem hkMaterialResponseTypeEnumItems_300[] =
{
	{0, "RESPONSE_INVALID"},
	{1, "RESPONSE_SIMPLE_CONTACT"},
	{2, "RESPONSE_REPORTING"},
	{3, "RESPONSE_NONE"},
	{4, "RESPONSE_MAX_ID"},
};
static const hkInternalClassEnum hkMaterialEnums[] = {
	{"ResponseType", hkMaterialResponseTypeEnumItems_300, 5 }
};
extern const hkClassEnum* hkMaterialResponseTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkMaterialEnums[0]);
static hkInternalClassMember hkMaterialClass_Members[] =
{
	{ "responseType", HK_NULL, hkMaterialResponseTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMaterialClass_300(
	"hkMaterial",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMaterialEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkMaterialClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/deform/skinning/hkMeshBinding.h'



// External pointer and enum types
extern hkClass hkTransformClass_300;
extern hkClass hkxMeshClass_300;

//
// Class hkMeshBinding
//
static hkInternalClassMember hkMeshBindingClass_Members[] =
{
	{ "mesh", &hkxMeshClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "inverseWorldBindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_TRANSFORM, 0, 0, 0 }
};
hkClass hkMeshBindingClass_300(
	"hkMeshBinding",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshBindingClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/mesh/hkMeshMaterial.h'



//
// Class hkMeshMaterial
//
static hkInternalClassMember hkMeshMaterialClass_Members[] =
{
	{ "filterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMeshMaterialClass_300;
hkClass hkMeshMaterialClass_300(
	"hkMeshMaterial",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshMaterialClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/mesh/hkMeshShape.h'



// External pointer and enum types
extern hkClass SubpartClass_300;

//
// Class hkMeshShape
//

//
// Enum hkMeshShape::IndexStridingType
//
static const hkInternalClassEnumItem hkMeshShapeIndexStridingTypeEnumItems_300[] =
{
	{0, "INDICES_INVALID"},
	{1, "INDICES_INT16"},
	{2, "INDICES_INT32"},
	{3, "INDICES_MAX_ID"},
};
static const hkInternalClassEnum hkMeshShapeEnums[] = {
	{"IndexStridingType", hkMeshShapeIndexStridingTypeEnumItems_300, 4 }
};
extern const hkClassEnum* hkMeshShapeIndexStridingTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[0]);

//
// Class hkMeshShape::Subpart
//
static hkInternalClassMember hkMeshShape_SubpartClass_Members[] =
{
	{ "vertexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "vertexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "stridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materialIndexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materialStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numMaterials", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMeshShapeSubpartClass_300;
hkClass hkMeshShapeSubpartClass_300(
	"hkMeshShapeSubpart",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshShape_SubpartClass_Members),
	12, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkMeshShapeClass_Members[] =
{
	{ "scaling", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numBitsForSubpartIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subparts", &hkMeshShapeSubpartClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkMeshShapeClass_300(
	"hkMeshShape",
	&hkShapeCollectionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMeshShapeEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshShapeClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/mopp/hkMoppBvTreeShape.h'



// External pointer and enum types
extern hkClass hkMoppCodeClass_300;

//
// Class hkMoppBvTreeShape
//
static hkInternalClassMember hkMoppBvTreeShapeClass_Members[] =
{
	{ "code", &hkMoppCodeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkMoppBvTreeShapeClass_300;
hkClass hkMoppBvTreeShapeClass_300(
	"hkMoppBvTreeShape",
	&hkBvTreeShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMoppBvTreeShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkinternal/collide/mopp/code/hkMoppCode.h'



// External pointer and enum types
extern hkClass hkMoppCodeCodeInfoClass_300;

//
// Class hkMoppCode
//

//
// Class hkMoppCode::CodeInfo
//
static hkInternalClassMember hkMoppCode_CodeInfoClass_Members[] =
{
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMoppCodeCodeInfoClass_300(
	"hkMoppCodeCodeInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMoppCode_CodeInfoClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkMoppCodeClass_Members[] =
{
	{ "info", &hkMoppCodeCodeInfoClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};

hkClass hkMoppCodeClass_300(
	"hkMoppCode",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMoppCodeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/hkMotion.h'



//
// Class hkMotion
//

//
// Enum hkMotion::MotionType
//
static const hkInternalClassEnumItem hkMotionMotionTypeEnumItems_300[] =
{
	{0, "MOTION_INVALID"},
	{1, "MOTION_DYNAMIC"},
	{2, "MOTION_SPHERE_INERTIA"},
	{3, "MOTION_STABILIZED_SPHERE_INERTIA"},
	{4, "MOTION_BOX_INERTIA"},
	{5, "MOTION_STABILIZED_BOX_INERTIA"},
	{6, "MOTION_KEYFRAMED"},
	{7, "MOTION_FIXED"},
	{8, "MOTION_MAX_ID"},
};
static const hkInternalClassEnum hkMotionEnums[] = {
	{"MotionType", hkMotionMotionTypeEnumItems_300, 9 }
};
extern const hkClassEnum* hkMotionMotionTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkMotionEnums[0]);
static hkInternalClassMember hkMotionClass_Members[] =
{
	{ "solverData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkMotionClass_300(
	"hkMotion",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMotionEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkMotionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/basetypes/hkMotionState.h'



// External pointer and enum types
extern hkClass hkSweptTransformClass_300;

//
// Class hkMotionState
//
static hkInternalClassMember hkMotionStateClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sweptTransform", &hkSweptTransformClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deltaAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationClass", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationCounter", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMotionStateClass_300;
hkClass hkMotionStateClass_300(
	"hkMotionState",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMotionStateClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/actions/motor/hkMotorAction.h'



//
// Class hkMotorAction
//
static hkInternalClassMember hkMotorActionClass_Members[] =
{
	{ "axis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spinRate", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "gain", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "active", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkUnaryActionClass_300;

extern hkClass hkMotorActionClass_300;
hkClass hkMotorActionClass_300(
	"hkMotorAction",
	&hkUnaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMotorActionClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/motor/hkMotorController.h'



//
// Class hkMotorController
//
static hkInternalClassMember hkMotorControllerClass_Members[] =
{
	{ "masterWeight", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkMotorControllerClass_300(
	"hkMotorController",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMotorControllerClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/mousespring/hkMouseSpringAction.h'



//
// Class hkMouseSpringAction
//
static hkInternalClassMember hkMouseSpringActionClass_Members[] =
{
	{ "positionInRbLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "mousePositionInWorld", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springElasticity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxRelativeForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkMouseSpringActionClass_300;
hkClass hkMouseSpringActionClass_300(
	"hkMouseSpringAction",
	&hkUnaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMouseSpringActionClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/multiray/hkMultiRayShape.h'



// External pointer and enum types
extern hkClass RayClass_300;

//
// Class hkMultiRayShape
//

//
// Class hkMultiRayShape::Ray
//
static hkInternalClassMember hkMultiRayShape_RayClass_Members[] =
{
	{ "start", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "end", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMultiRayShapeRayClass_300;
hkClass hkMultiRayShapeRayClass_300(
	"hkMultiRayShapeRay",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMultiRayShape_RayClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkMultiRayShapeClass_Members[] =
{
	{ "rays", &hkMultiRayShapeRayClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rayPenetrationDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkMultiRayShapeClass_300;
hkClass hkMultiRayShapeClass_300(
	"hkMultiRayShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMultiRayShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/multisphere/hkMultiSphereShape.h'



//
// Class hkMultiSphereShape
//

//
// Enum hkMultiSphereShape::test
//
static const hkInternalClassEnumItem hkMultiSphereShapetestEnumItems_300[] =
{
	{8, "MAX_SPHERES"},
};
static const hkInternalClassEnum hkMultiSphereShapeEnums[] = {
	{"test", hkMultiSphereShapetestEnumItems_300, 1 }
};
extern const hkClassEnum* hkMultiSphereShapetestEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkMultiSphereShapeEnums[0]);
static hkInternalClassMember hkMultiSphereShapeClass_Members[] =
{
	{ "numSpheres", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spheres", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 8, 0, 0 }
};

extern hkClass hkMultiSphereShapeClass_300;
hkClass hkMultiSphereShapeClass_300(
	"hkMultiSphereShape",
	&hkSphereRepShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMultiSphereShapeEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkMultiSphereShapeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/null/hkNullCollisionFilter.h'



//
// Class hkNullCollisionFilter
//

extern hkClass hkNullCollisionFilterClass_300;
hkClass hkNullCollisionFilterClass_300(
	"hkNullCollisionFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkserialize/packfile/binary/hkPackfileHeader.h'



//
// Class hkPackfileHeader
//
static hkInternalClassMember hkPackfileHeaderClass_Members[] =
{
	{ "magic", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "userTag", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "fileVersion", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "layoutRules", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 4, 0, 0 },
	{ "numSections", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsSectionIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsSectionOffest", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsClassSectionIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contentsClassSectionOffest", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pad", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 6, 0, 0 }
};
extern hkClass hkPackfileHeaderClass_300;
hkClass hkPackfileHeaderClass_300(
	"hkPackfileHeader",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPackfileHeaderClass_Members),
	10, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkserialize/packfile/binary/hkPackfileSectionHeader.h'



//
// Class hkPackfileSectionHeader
//
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
extern hkClass hkPackfileSectionHeaderClass_300;
hkClass hkPackfileSectionHeaderClass_300(
	"hkPackfileSectionHeader",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPackfileSectionHeaderClass_Members),
	9, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/collide/filter/pairwise/hkPairwiseCollisionFilter.h'



// External pointer and enum types
extern hkClass CollisionPairClass_300;

//
// Class hkPairwiseCollisionFilter
//

//
// Class hkPairwiseCollisionFilter::CollisionPair
//
static hkInternalClassMember hkPairwiseCollisionFilter_CollisionPairClass_Members[] =
{
	{ "a", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "b", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkPairwiseCollisionFilterCollisionPairClass_300;
hkClass hkPairwiseCollisionFilterCollisionPairClass_300(
	"hkPairwiseCollisionFilterCollisionPair",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPairwiseCollisionFilter_CollisionPairClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPairwiseCollisionFilterClass_Members[] =
{
	{ "disabledPairs", &hkPairwiseCollisionFilterCollisionPairClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkPairwiseCollisionFilterClass_300;
hkClass hkPairwiseCollisionFilterClass_300(
	"hkPairwiseCollisionFilter",
	&hkCollisionFilterClass_300, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPairwiseCollisionFilterClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/pointtopath/hkParametricCurve.h'



//
// Class hkParametricCurve
//

hkClass hkParametricCurveClass_300(
	"hkParametricCurve",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/phantomcallback/hkPhantomCallbackShape.h'



//
// Class hkPhantomCallbackShape
//

extern hkClass hkPhantomCallbackShapeClass_300;
hkClass hkPhantomCallbackShapeClass_300(
	"hkPhantomCallbackShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/phantom/hkPhantom.h'



// External pointer and enum types
extern hkClass hkPhantomListenerClass_300;
extern hkClass hkPhantomOverlapListenerClass_300;

//
// Class hkPhantom
//
static hkInternalClassMember hkPhantomClass_Members[] =
{
	{ "overlapListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "phantomListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};

hkClass hkPhantomClass_300(
	"hkPhantom",
	&hkWorldObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPhantomClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/serialize/hkPhysicsData.h'



// External pointer and enum types
extern hkClass hkPhysicsSystemClass_300;
extern hkClass hkWorldCinfoClass_300;

//
// Class hkPhysicsData
//
static hkInternalClassMember hkPhysicsDataClass_Members[] =
{
	{ "worldCinfo", &hkWorldCinfoClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "systems", &hkPhysicsSystemClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkPhysicsDataClass_300;
hkClass hkPhysicsDataClass_300(
	"hkPhysicsData",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPhysicsDataClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/world/hkPhysicsSystem.h'



// External pointer and enum types
extern hkClass hkRigidBodyClass_300;

//
// Class hkPhysicsSystem
//
static hkInternalClassMember hkPhysicsSystemClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "actions", &hkActionClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "phantoms", &hkPhantomClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkPhysicsSystemClass_300(
	"hkPhysicsSystem",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPhysicsSystemClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/plane/hkPlaneShape.h'



//
// Class hkPlaneShape
//
static hkInternalClassMember hkPlaneShapeClass_Members[] =
{
	{ "plane", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aabbHalfExtents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkPlaneShapeClass_300;
hkClass hkPlaneShapeClass_300(
	"hkPlaneShape",
	&hkHeightFieldShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPlaneShapeClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/pointtopath/hkPointToPathConstraintData.h'



// External pointer and enum types

//
// Class hkPointToPathConstraintData
//

//
// Enum hkPointToPathConstraintData::OrientationConstraintType
//
static const hkInternalClassEnumItem hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_300[] =
{
	{0, "CONSTRAIN_ORIENTATION_INVALID"},
	{1, "CONSTRAIN_ORIENTATION_NONE"},
	{2, "CONSTRAIN_ORIENTATION_ALLOW_SPIN"},
	{3, "CONSTRAIN_ORIENTATION_TO_PATH"},
	{4, "CONSTRAIN_ORIENTATION_MAX_ID"},
};
static const hkInternalClassEnum hkPointToPathConstraintDataEnums[] = {
	{"OrientationConstraintType", hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_300, 5 }
};
extern const hkClassEnum* hkPointToPathConstraintDataOrientationConstraintTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkPointToPathConstraintDataEnums[0]);

//
// Class hkPointToPathConstraintData::Runtime
//
static hkInternalClassMember hkPointToPathConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "parametricPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPointToPathConstraintDataRuntimeClass_300;
hkClass hkPointToPathConstraintDataRuntimeClass_300(
	"hkPointToPathConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPointToPathConstraintData_RuntimeClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPointToPathConstraintDataClass_Members[] =
{
	{ "path", &hkParametricCurveClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularConstrainedDOF", HK_NULL, hkPointToPathConstraintDataOrientationConstraintTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "transform_OS_KS", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 2, 0, 0 }
};

extern hkClass hkPointToPathConstraintDataClass_300;
hkClass hkPointToPathConstraintDataClass_300(
	"hkPointToPathConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkPointToPathConstraintDataEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkPointToPathConstraintDataClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/pointtoplane/hkPointToPlaneConstraintData.h'



// External pointer and enum types

//
// Class hkPointToPlaneConstraintData
//

//
// Class hkPointToPlaneConstraintData::Runtime
//
static hkInternalClassMember hkPointToPlaneConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 1, 0, 0 }
};
extern hkClass hkPointToPlaneConstraintDataRuntimeClass_300;
hkClass hkPointToPlaneConstraintDataRuntimeClass_300(
	"hkPointToPlaneConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPointToPlaneConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPointToPlaneConstraintDataClass_Members[] =
{
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeNormalA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkPointToPlaneConstraintDataClass_300;
hkClass hkPointToPlaneConstraintDataClass_300(
	"hkPointToPlaneConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPointToPlaneConstraintDataClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/poweredhinge/hkPoweredHingeConstraintData.h'



// External pointer and enum types

//
// Class hkPoweredHingeConstraintData
//

//
// Class hkPoweredHingeConstraintData::Runtime
//
static hkInternalClassMember hkPoweredHingeConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "lastAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotations", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPoweredHingeConstraintDataRuntimeClass_300;
hkClass hkPoweredHingeConstraintDataRuntimeClass_300(
	"hkPoweredHingeConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredHingeConstraintData_RuntimeClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPoweredHingeConstraintDataClass_Members[] =
{
	{ "motorActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ignoreLimits", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motor", &hkMotorControllerClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkPoweredHingeConstraintDataClass_300;
hkClass hkPoweredHingeConstraintDataClass_300(
	"hkPoweredHingeConstraintData",
	&hkLimitedHingeConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredHingeConstraintDataClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/poweredragdoll/hkPoweredRagdollConstraintData.h'



// External pointer and enum types

//
// Class hkPoweredRagdollConstraintData
//
static hkInternalClassMember hkPoweredRagdollConstraintDataClass_Members[] =
{
	{ "motorsActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "targetFrameAinB", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMotor", &hkMotorControllerClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "coneMotor", &hkMotorControllerClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "planeMotor", &hkMotorControllerClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkRagdollConstraintDataClass_300;

extern hkClass hkPoweredRagdollConstraintDataClass_300;
hkClass hkPoweredRagdollConstraintDataClass_300(
	"hkPoweredRagdollConstraintData",
	&hkRagdollConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredRagdollConstraintDataClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/prismatic/hkPrismaticConstraintData.h'



// External pointer and enum types
extern hkClass hkPrismaticConstraintDataConstraintBasisAClass_300;
extern hkClass hkPrismaticConstraintDataConstraintBasisBClass_300;

//
// Class hkPrismaticConstraintData
//

//
// Class hkPrismaticConstraintData::Runtime
//
static hkInternalClassMember hkPrismaticConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 }
};
extern hkClass hkPrismaticConstraintDataRuntimeClass_300;
hkClass hkPrismaticConstraintDataRuntimeClass_300(
	"hkPrismaticConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkPrismaticConstraintData::ConstraintBasisA
//
static hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "BtoAoffsetRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_ROTATION, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataConstraintBasisAClass_300(
	"hkPrismaticConstraintDataConstraintBasisA",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisAClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkPrismaticConstraintData::ConstraintBasisB
//
static hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToShaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPrismaticConstraintDataConstraintBasisBClass_300(
	"hkPrismaticConstraintDataConstraintBasisB",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisBClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPrismaticConstraintDataClass_Members[] =
{
	{ "basisA", &hkPrismaticConstraintDataConstraintBasisAClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkPrismaticConstraintDataConstraintBasisBClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkPrismaticConstraintDataClass_300;
hkClass hkPrismaticConstraintDataClass_300(
	"hkPrismaticConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintDataClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/common/hkProperty.h'



// External pointer and enum types
extern hkClass hkPropertyValueClass_300;

//
// Class hkPropertyValue
//
static hkInternalClassMember hkPropertyValueClass_Members[] =
{
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT64, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPropertyValueClass_300(
	"hkPropertyValue",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPropertyValueClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkProperty
//
static hkInternalClassMember hkPropertyClass_Members[] =
{
	{ "key", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "value", &hkPropertyValueClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPropertyClass_300;
hkClass hkPropertyClass_300(
	"hkProperty",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPropertyClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h'



// External pointer and enum types
extern hkClass hkRagdollConstraintDataConstraintBasisAClass_300;
extern hkClass hkRagdollConstraintDataConstraintBasisBClass_300;

//
// Class hkRagdollConstraintData
//

//
// Class hkRagdollConstraintData::Runtime
//
static hkInternalClassMember hkRagdollConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 9, 0, 0 }
};
extern hkClass hkRagdollConstraintDataRuntimeClass_300;
hkClass hkRagdollConstraintDataRuntimeClass_300(
	"hkRagdollConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkRagdollConstraintData::ConstraintBasisA
//
static hkInternalClassMember hkRagdollConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollConstraintDataConstraintBasisAClass_300(
	"hkRagdollConstraintDataConstraintBasisA",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisAClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkRagdollConstraintData::ConstraintBasisB
//
static hkInternalClassMember hkRagdollConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkRagdollConstraintDataConstraintBasisBClass_300(
	"hkRagdollConstraintDataConstraintBasisB",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisBClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkRagdollConstraintDataClass_Members[] =
{
	{ "basisA", &hkRagdollConstraintDataConstraintBasisAClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkRagdollConstraintDataConstraintBasisBClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkRagdollConstraintDataClass_300(
	"hkRagdollConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintDataClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/ragdoll/instance/hkRagdollInstance.h'



// External pointer and enum types

//
// Class hkRagdollInstance
//
static hkInternalClassMember hkRagdollInstanceClass_Members[] =
{
	{ "physicsSystem", &hkPhysicsSystemClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkRagdollInstanceClass_300;
hkClass hkRagdollInstanceClass_300(
	"hkRagdollInstance",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollInstanceClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/filter/hkRayCollidableFilter.h'



//
// Class hkRayCollidableFilter
//
extern hkClass hkRayCollidableFilterClass_300;
hkClass hkRayCollidableFilterClass_300(
	"hkRayCollidableFilter",
	HK_NULL, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/hkRayShapeCollectionFilter.h'



//
// Class hkRayShapeCollectionFilter
//
extern hkClass hkRayShapeCollectionFilterClass_300;
hkClass hkRayShapeCollectionFilterClass_300(
	"hkRayShapeCollectionFilter",
	HK_NULL, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkbase/baseobject/hkReferencedObject.h'



//
// Class hkReferencedObject
//
static hkInternalClassMember hkReferencedObjectClass_Members[] =
{
	{ "memSizeAndFlags", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "referenceCount", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_INT16, 0, 0, 0 }
};

hkClass hkReferencedObjectClass_300(
	"hkReferencedObject",
	&hkBaseObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkReferencedObjectClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/actions/reorient/hkReorientAction.h'



//
// Class hkReorientAction
//
static hkInternalClassMember hkReorientActionClass_Members[] =
{
	{ "rotationAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "upAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkReorientActionClass_300;
hkClass hkReorientActionClass_300(
	"hkReorientAction",
	&hkUnaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkReorientActionClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkconstraintsolver/accumulator/hkRigidAccumulator.h'



//
// Class hkRigidAccumulator
//
static hkInternalClassMember hkRigidAccumulatorClass_Members[] =
{
	{ "deactivationCounter", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationClass", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "matrixIsIdentity", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearVel", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularVel", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "invMasses", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scratch0", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scratch1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scratch2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scratch3", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVelocityAccumulatorClass_300;

extern hkClass hkRigidAccumulatorClass_300;
hkClass hkRigidAccumulatorClass_300(
	"hkRigidAccumulator",
	&hkVelocityAccumulatorClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRigidAccumulatorClass_Members),
	10, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkRigidBody.h'



//
// Class hkRigidBody
//

hkClass hkRigidBodyClass_300(
	"hkRigidBody",
	&hkEntityClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkRigidBodyDeactivator.h'



//
// Class hkRigidBodyDeactivator
//

//
// Enum hkRigidBodyDeactivator::DeactivatorType
//
static const hkInternalClassEnumItem hkRigidBodyDeactivatorDeactivatorTypeEnumItems_300[] =
{
	{0, "DEACTIVATOR_INVALID"},
	{1, "DEACTIVATOR_NEVER"},
	{2, "DEACTIVATOR_SPATIAL"},
	{3, "DEACTIVATOR_MAX_ID"},
};
static const hkInternalClassEnum hkRigidBodyDeactivatorEnums[] = {
	{"DeactivatorType", hkRigidBodyDeactivatorDeactivatorTypeEnumItems_300, 4 }
};
extern const hkClassEnum* hkRigidBodyDeactivatorDeactivatorTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkRigidBodyDeactivatorEnums[0]);

hkClass hkRigidBodyDeactivatorClass_300(
	"hkRigidBodyDeactivator",
	&hkEntityDeactivatorClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkRigidBodyDeactivatorEnums),
	1, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkRigidMotion.h'



// External pointer and enum types

//
// Class hkRigidMotion
//
static hkInternalClassMember hkRigidMotionClass_Members[] =
{
	{ "motionState", &hkMotionStateClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "massInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "particleMinInertiaDiagInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkRigidMotionClass_300(
	"hkRigidMotion",
	&hkMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRigidMotionClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkserialize/util/hkRootLevelContainer.h'



// External pointer and enum types
extern hkClass NamedVariantClass_300;

//
// Class hkRootLevelContainer
//

//
// Class hkRootLevelContainer::NamedVariant
//
static hkInternalClassMember hkRootLevelContainer_NamedVariantClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "variant", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkRootLevelContainerNamedVariantClass_300;
hkClass hkRootLevelContainerNamedVariantClass_300(
	"hkRootLevelContainerNamedVariant",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRootLevelContainer_NamedVariantClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkRootLevelContainerClass_Members[] =
{
	{ "namedVariants", &hkRootLevelContainerNamedVariantClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkRootLevelContainerClass_300;
hkClass hkRootLevelContainerClass_300(
	"hkRootLevelContainer",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRootLevelContainerClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/sampledheightfield/hkSampledHeightFieldBase.h'



//
// Class hkSampledHeightFieldBase
//
static hkInternalClassMember hkSampledHeightFieldBaseClass_Members[] =
{
	{ "xRes", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "zRes", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "triangleFlip", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "heightCenter", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "intToFloatScale", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "floatToIntScale", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "floatToIntOffsetFloorCorrected", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extents", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSampledHeightFieldBaseClass_300;
hkClass hkSampledHeightFieldBaseClass_300(
	"hkSampledHeightFieldBase",
	&hkHeightFieldShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSampledHeightFieldBaseClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/sampledheightfield/hkSampledHeightFieldShape.h'


// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/serialize/display/hkSerializedDisplayMarker.h'



//
// Class hkSerializedDisplayMarker
//
static hkInternalClassMember hkSerializedDisplayMarkerClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayMarkerClass_300;
hkClass hkSerializedDisplayMarkerClass_300(
	"hkSerializedDisplayMarker",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayMarkerClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/serialize/display/hkSerializedDisplayMarkerList.h'



// External pointer and enum types

//
// Class hkSerializedDisplayMarkerList
//
static hkInternalClassMember hkSerializedDisplayMarkerListClass_Members[] =
{
	{ "markers", &hkSerializedDisplayMarkerClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayMarkerListClass_300;
hkClass hkSerializedDisplayMarkerListClass_300(
	"hkSerializedDisplayMarkerList",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayMarkerListClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/serialize/display/hkSerializedDisplayRbTransforms.h'



// External pointer and enum types
extern hkClass DisplayTransformPairClass_300;

//
// Class hkSerializedDisplayRbTransforms
//

//
// Class hkSerializedDisplayRbTransforms::DisplayTransformPair
//
static hkInternalClassMember hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members[] =
{
	{ "rb", &hkRigidBodyClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "localToDisplay", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_300;
hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_300(
	"hkSerializedDisplayRbTransformsDisplayTransformPair",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkSerializedDisplayRbTransformsClass_Members[] =
{
	{ "transforms", &hkSerializedDisplayRbTransformsDisplayTransformPairClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayRbTransformsClass_300;
hkClass hkSerializedDisplayRbTransformsClass_300(
	"hkSerializedDisplayRbTransforms",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSerializedDisplayRbTransformsClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/hkShape.h'



//
// Class hkShape
//
static hkInternalClassMember hkShapeClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkShapeClass_300(
	"hkShape",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/collection/hkShapeCollection.h'



//
// Class hkShapeCollection
//
static hkInternalClassMember hkShapeCollectionClass_Members[] =
{
	{ "disableWelding", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkShapeCollectionClass_300(
	"hkShapeCollection",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkShapeCollectionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/agent/hkShapeCollectionFilter.h'



//
// Class hkShapeCollectionFilter
//
extern hkClass hkShapeCollectionFilterClass_300;
hkClass hkShapeCollectionFilterClass_300(
	"hkShapeCollectionFilter",
	HK_NULL, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/phantom/hkShapePhantom.h'



// External pointer and enum types

//
// Class hkShapePhantom
//
static hkInternalClassMember hkShapePhantomClass_Members[] =
{
	{ "motionState", &hkMotionStateClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkShapePhantomClass_300(
	"hkShapePhantom",
	&hkPhantomClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkShapePhantomClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/hkShapeRayCastInput.h'



// External pointer and enum types

//
// Class hkShapeRayCastInput
//
static hkInternalClassMember hkShapeRayCastInputClass_Members[] =
{
	{ "from", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "to", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "filterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rayShapeCollectionFilter", &hkRayShapeCollectionFilterClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkShapeRayCastInputClass_300(
	"hkShapeRayCastInput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkShapeRayCastInputClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/simplemesh/hkSimpleMeshShape.h'



// External pointer and enum types
extern hkClass TriangleClass_300;

//
// Class hkSimpleMeshShape
//

//
// Class hkSimpleMeshShape::Triangle
//
static hkInternalClassMember hkSimpleMeshShape_TriangleClass_Members[] =
{
	{ "a", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "b", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "c", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSimpleMeshShapeTriangleClass_300;
hkClass hkSimpleMeshShapeTriangleClass_300(
	"hkSimpleMeshShapeTriangle",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSimpleMeshShape_TriangleClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkSimpleMeshShapeClass_Members[] =
{
	{ "vertices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "triangles", &hkSimpleMeshShapeTriangleClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSimpleMeshShapeClass_300;
hkClass hkSimpleMeshShapeClass_300(
	"hkSimpleMeshShape",
	&hkShapeCollectionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSimpleMeshShapeClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/phantom/hkSimpleShapePhantom.h'



// External pointer and enum types

//
// Class hkSimpleShapePhantom
//

//
// Class hkSimpleShapePhantom::hkCollisionDetail
//
static hkInternalClassMember hkSimpleShapePhantom_hkCollisionDetailClass_Members[] =
{
	{ "collidable", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkSimpleShapePhantomhkCollisionDetailClass_300;
hkClass hkSimpleShapePhantomhkCollisionDetailClass_300(
	"hkSimpleShapePhantomhkCollisionDetail",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSimpleShapePhantom_hkCollisionDetailClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkSimpleShapePhantomClass_Members[] =
{
	{ "collisionDetails", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};

extern hkClass hkSimpleShapePhantomClass_300;
hkClass hkSimpleShapePhantomClass_300(
	"hkSimpleShapePhantom",
	&hkShapePhantomClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSimpleShapePhantomClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/hkSkeletalAnimation.h'



// External pointer and enum types

//
// Class hkSkeletalAnimation
//
static hkInternalClassMember hkSkeletalAnimationClass_Members[] =
{
	{ "period", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numberOfBoneTracks", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extractedMotion", &hkExtractedMotionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "animationTracks", &hkAnimationTrackClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

hkClass hkSkeletalAnimationClass_300(
	"hkSkeletalAnimation",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletalAnimationClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/rig/hkSkeleton.h'



// External pointer and enum types

//
// Class hkSkeleton
//
static hkInternalClassMember hkSkeletonClass_Members[] =
{
	{ "hierarchy", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "bones", &hkBoneClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkSkeletonClass_300(
	"hkSkeleton",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkconstraintsolver/solve/hkSolverResults.h'



//
// Class hkSolverResults
//
static hkInternalClassMember hkSolverResultsClass_Members[] =
{
	{ "impulseApplied", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internalSolverData", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSolverResultsClass_300(
	"hkSolverResults",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSolverResultsClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/entity/hkSpatialRigidBodyDeactivator.h'



// External pointer and enum types
extern hkClass hkSpatialRigidBodyDeactivatorSampleClass_300;

//
// Class hkSpatialRigidBodyDeactivator
//

//
// Class hkSpatialRigidBodyDeactivator::Sample
//
static hkInternalClassMember hkSpatialRigidBodyDeactivator_SampleClass_Members[] =
{
	{ "refPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "refRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSpatialRigidBodyDeactivatorSampleClass_300(
	"hkSpatialRigidBodyDeactivatorSample",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSpatialRigidBodyDeactivator_SampleClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkSpatialRigidBodyDeactivatorClass_Members[] =
{
	{ "highFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "radiusSqrd", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSpatialRigidBodyDeactivatorClass_300;
hkClass hkSpatialRigidBodyDeactivatorClass_300(
	"hkSpatialRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSpatialRigidBodyDeactivatorClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/basetypes/hkSphere.h'



//
// Class hkSphere
//
static hkInternalClassMember hkSphereClass_Members[] =
{
	{ "pos", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSphereClass_300(
	"hkSphere",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSphereClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkSphereMotion.h'



//
// Class hkSphereMotion
//

extern hkClass hkSphereMotionClass_300;
hkClass hkSphereMotionClass_300(
	"hkSphereMotion",
	&hkRigidMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/sphererep/hkSphereRepShape.h'



//
// Class hkSphereRepShape
//

//
// Class hkSphereRepShape::hkCollisionSpheresInfo
//
static hkInternalClassMember hkSphereRepShape_hkCollisionSpheresInfoClass_Members[] =
{
	{ "numSpheres", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "useBuffer", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSphereRepShapehkCollisionSpheresInfoClass_300;
hkClass hkSphereRepShapehkCollisionSpheresInfoClass_300(
	"hkSphereRepShapehkCollisionSpheresInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSphereRepShape_hkCollisionSpheresInfoClass_Members),
	2, // members
	HK_NULL // defaults
	);

hkClass hkSphereRepShapeClass_300(
	"hkSphereRepShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/sphere/hkSphereShape.h'



//
// Class hkSphereShape
//

extern hkClass hkSphereShapeClass_300;
hkClass hkSphereShapeClass_300(
	"hkSphereShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkutilities/actions/spring/hkSpringAction.h'



//
// Class hkSpringAction
//
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

extern hkClass hkSpringActionClass_300;
hkClass hkSpringActionClass_300(
	"hkSpringAction",
	&hkBinaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSpringActionClass_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/motor/hkSpringDamperMotor.h'



//
// Class hkSpringDamperMotor
//
static hkInternalClassMember hkSpringDamperMotorClass_Members[] =
{
	{ "targetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "invSpringConstant", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSpringDamperMotorClass_300;
hkClass hkSpringDamperMotorClass_300(
	"hkSpringDamperMotor",
	&hkMotorControllerClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSpringDamperMotorClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkStabilizedBoxMotion.h'



//
// Class hkStabilizedBoxMotion
//

extern hkClass hkStabilizedBoxMotionClass_300;
hkClass hkStabilizedBoxMotionClass_300(
	"hkStabilizedBoxMotion",
	&hkBoxMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/motion/rigid/hkStabilizedSphereMotion.h'



//
// Class hkStabilizedSphereMotion
//

extern hkClass hkStabilizedSphereMotionClass_300;
hkClass hkStabilizedSphereMotionClass_300(
	"hkStabilizedSphereMotion",
	&hkSphereMotionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/stiffspring/hkStiffSpringConstraintData.h'



// External pointer and enum types

//
// Class hkStiffSpringConstraintData
//

//
// Class hkStiffSpringConstraintData::Runtime
//
static hkInternalClassMember hkStiffSpringConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 1, 0, 0 }
};
extern hkClass hkStiffSpringConstraintDataRuntimeClass_300;
hkClass hkStiffSpringConstraintDataRuntimeClass_300(
	"hkStiffSpringConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStiffSpringConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkStiffSpringConstraintDataClass_Members[] =
{
	{ "springLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pivotInB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkStiffSpringConstraintDataClass_300;
hkClass hkStiffSpringConstraintDataClass_300(
	"hkStiffSpringConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStiffSpringConstraintDataClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/storagemesh/hkStorageMeshShape.h'



// External pointer and enum types
extern hkClass SubpartStorageClass_300;

//
// Class hkStorageMeshShape
//

//
// Class hkStorageMeshShape::SubpartStorage
//
static hkInternalClassMember hkStorageMeshShape_SubpartStorageClass_Members[] =
{
	{ "vertices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "materials", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};
extern hkClass hkStorageMeshShapeSubpartStorageClass_300;
hkClass hkStorageMeshShapeSubpartStorageClass_300(
	"hkStorageMeshShapeSubpartStorage",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStorageMeshShape_SubpartStorageClass_Members),
	5, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkStorageMeshShapeClass_Members[] =
{
	{ "storage", &hkStorageMeshShapeSubpartStorageClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkStorageMeshShapeClass_300;
hkClass hkStorageMeshShapeClass_300(
	"hkStorageMeshShape",
	&hkMeshShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStorageMeshShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/motor/hkStrongestMotor.h'



//
// Class hkStrongestMotor
//
static hkInternalClassMember hkStrongestMotorClass_Members[] =
{
	{ "targetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkStrongestMotorClass_300;
hkClass hkStrongestMotorClass_300(
	"hkStrongestMotor",
	&hkMotorControllerClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStrongestMotorClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/linear/hkSweptTransform.h'



//
// Class hkSweptTransform
//
static hkInternalClassMember hkSweptTransformClass_Members[] =
{
	{ "centerOfMass0", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "centerOfMass1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotation0", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotation1", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "centerOfMassLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSweptTransformClass_300(
	"hkSweptTransform",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSweptTransformClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/transform/hkTransformShape.h'



// External pointer and enum types

//
// Class hkTransformShape
//
static hkInternalClassMember hkTransformShapeClass_Members[] =
{
	{ "childShape", &hkShapeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTransformShapeClass_300;
hkClass hkTransformShapeClass_300(
	"hkTransformShape",
	&hkShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTransformShapeClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : NONE
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/tripatch/hkTriPatchTriangle.h'



//
// Class hkTriPatchTriangle
//
static hkInternalClassMember hkTriPatchTriangleClass_Members[] =
{
	{ "neighbours", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 3, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceCountAndConnectivity", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkTriPatchTriangleClass_300;
hkClass hkTriPatchTriangleClass_300(
	"hkTriPatchTriangle",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTriPatchTriangleClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/trisampledheightfield/hkTriSampledHeightFieldBvTreeShape.h'



//
// Class hkTriSampledHeightFieldBvTreeShape
//
static hkInternalClassMember hkTriSampledHeightFieldBvTreeShapeClass_Members[] =
{
	{ "wantAabbRejectionTest", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTriSampledHeightFieldBvTreeShapeClass_300;
hkClass hkTriSampledHeightFieldBvTreeShapeClass_300(
	"hkTriSampledHeightFieldBvTreeShape",
	&hkBvTreeShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTriSampledHeightFieldBvTreeShapeClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/trisampledheightfield/hkTriSampledHeightFieldCollection.h'



// External pointer and enum types

//
// Class hkTriSampledHeightFieldCollection
//
static hkInternalClassMember hkTriSampledHeightFieldCollectionClass_Members[] =
{
	{ "heightfield", &hkSampledHeightFieldBaseClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTriSampledHeightFieldCollectionClass_300;
hkClass hkTriSampledHeightFieldCollectionClass_300(
	"hkTriSampledHeightFieldCollection",
	&hkShapeCollectionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTriSampledHeightFieldCollectionClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/shape/triangle/hkTriangleShape.h'



//
// Class hkTriangleShape
//
static hkInternalClassMember hkTriangleShapeClass_Members[] =
{
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexC", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTriangleShapeClass_300;
hkClass hkTriangleShapeClass_300(
	"hkTriangleShape",
	&hkConvexShapeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTriangleShapeClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkcollide/dispatch/broadphase/hkTypedBroadPhaseHandle.h'



//
// Class hkTypedBroadPhaseHandle
//
static hkInternalClassMember hkTypedBroadPhaseHandleClass_Members[] =
{
	{ "type", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ownerOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectQualityType", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkTypedBroadPhaseHandleClass_300(
	"hkTypedBroadPhaseHandle",
	&hkBroadPhaseHandleClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTypedBroadPhaseHandleClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/tyremarks/hkTyremarksInfo.h'



// External pointer and enum types
extern hkClass hkTyremarkPointClass_300;
extern hkClass hkTyremarksWheelClass_300;

//
// Class hkTyremarkPoint
//
static hkInternalClassMember hkTyremarkPointClass_Members[] =
{
	{ "pointLeft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pointRight", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTyremarkPointClass_300(
	"hkTyremarkPoint",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTyremarkPointClass_Members),
	2, // members
	HK_NULL // defaults
	);

//
// Class hkTyremarksWheel
//
static hkInternalClassMember hkTyremarksWheelClass_Members[] =
{
	{ "currentPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numPoints", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tyremarkPoints", &hkTyremarkPointClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkTyremarksWheelClass_300(
	"hkTyremarksWheel",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTyremarksWheelClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkTyremarksInfo
//
static hkInternalClassMember hkTyremarksInfoClass_Members[] =
{
	{ "minTyremarkEnergy", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxTyremarkEnergy", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tyremarksWheel", &hkTyremarksWheelClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkTyremarksInfoClass_300;
hkClass hkTyremarksInfoClass_300(
	"hkTyremarksInfo",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkTyremarksInfoClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/action/hkUnaryAction.h'



// External pointer and enum types

//
// Class hkUnaryAction
//
static hkInternalClassMember hkUnaryActionClass_Members[] =
{
	{ "entity", &hkEntityClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkUnaryActionClass_300(
	"hkUnaryAction",
	&hkActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkUnaryActionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/aerodynamics/hkVehicleAerodynamics.h'



//
// Class hkVehicleAerodynamics
//

//
// Class hkVehicleAerodynamics::AerodynamicsDragOutput
//
static hkInternalClassMember hkVehicleAerodynamics_AerodynamicsDragOutputClass_Members[] =
{
	{ "aerodynamicsForce", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aerodynamicsTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleAerodynamicsAerodynamicsDragOutputClass_300;
hkClass hkVehicleAerodynamicsAerodynamicsDragOutputClass_300(
	"hkVehicleAerodynamicsAerodynamicsDragOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleAerodynamics_AerodynamicsDragOutputClass_Members),
	2, // members
	HK_NULL // defaults
	);

extern hkClass hkVehicleAerodynamicsClass_300;
hkClass hkVehicleAerodynamicsClass_300(
	"hkVehicleAerodynamics",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/brake/hkVehicleBrake.h'



// External pointer and enum types
extern hkClass hkBoolClass_300;

//
// Class hkVehicleBrake
//

//
// Class hkVehicleBrake::WheelBreakingOutput
//
static hkInternalClassMember hkVehicleBrake_WheelBreakingOutputClass_Members[] =
{
	{ "brakingTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "isFixed", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_BOOL, 0, 0, 0 },
	{ "wheelsTimeSinceMaxPedalInput", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleBrakeWheelBreakingOutputClass_300;
hkClass hkVehicleBrakeWheelBreakingOutputClass_300(
	"hkVehicleBrakeWheelBreakingOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleBrake_WheelBreakingOutputClass_Members),
	3, // members
	HK_NULL // defaults
	);

extern hkClass hkVehicleBrakeClass_300;
hkClass hkVehicleBrakeClass_300(
	"hkVehicleBrake",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/hkVehicleData.h'



// External pointer and enum types
extern hkClass WheelComponentParamsClass_300;
extern hkClass hkInt8Class_300;
extern hkClass hkVehicleFrictionDescriptionClass_300;

//
// Class hkVehicleData
//

//
// Class hkVehicleData::WheelComponentParams
//
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
extern hkClass hkVehicleDataWheelComponentParamsClass_300;
hkClass hkVehicleDataWheelComponentParamsClass_300(
	"hkVehicleDataWheelComponentParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleData_WheelComponentParamsClass_Members),
	9, // members
	HK_NULL // defaults
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
	{ "wheelParams", &hkVehicleDataWheelComponentParamsClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numWheelsPerAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT8, 0, 0, 0 },
	{ "frictionDescription", &hkVehicleFrictionDescriptionClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisFrictionInertiaInvDiag", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "alreadyInitialised", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleDataClass_300;
hkClass hkVehicleDataClass_300(
	"hkVehicleData",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDataClass_Members),
	18, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/aerodynamics/default/hkVehicleDefaultAerodynamics.h'



//
// Class hkVehicleDefaultAerodynamics
//
static hkInternalClassMember hkVehicleDefaultAerodynamicsClass_Members[] =
{
	{ "airDensity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "frontalArea", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dragCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "liftCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraGravityws", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleDefaultAerodynamicsClass_300;
hkClass hkVehicleDefaultAerodynamicsClass_300(
	"hkVehicleDefaultAerodynamics",
	&hkVehicleAerodynamicsClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAerodynamicsClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/driverinput/default/hkVehicleDefaultAnalogDriverInput.h'



//
// Class hkVehicleDriverInputAnalogStatus
//
static hkInternalClassMember hkVehicleDriverInputAnalogStatusClass_Members[] =
{
	{ "positionX", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "positionY", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "handbrakeButtonPressed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "reverseButtonPressed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleDriverInputStatusClass_300;

extern hkClass hkVehicleDriverInputAnalogStatusClass_300;
hkClass hkVehicleDriverInputAnalogStatusClass_300(
	"hkVehicleDriverInputAnalogStatus",
	&hkVehicleDriverInputStatusClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDriverInputAnalogStatusClass_Members),
	4, // members
	HK_NULL // defaults
	);

//
// Class hkVehicleDefaultAnalogDriverInput
//
static hkInternalClassMember hkVehicleDefaultAnalogDriverInputClass_Members[] =
{
	{ "slopeChangePointX", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "initialSlope", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deadZone", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "autoReverse", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleDriverInputClass_300;

extern hkClass hkVehicleDefaultAnalogDriverInputClass_300;
hkClass hkVehicleDefaultAnalogDriverInputClass_300(
	"hkVehicleDefaultAnalogDriverInput",
	&hkVehicleDriverInputClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultAnalogDriverInputClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/brake/default/hkVehicleDefaultBrake.h'



// External pointer and enum types
extern hkClass WheelBrakingPropertiesClass_300;

//
// Class hkVehicleDefaultBrake
//

//
// Class hkVehicleDefaultBrake::WheelBrakingProperties
//
static hkInternalClassMember hkVehicleDefaultBrake_WheelBrakingPropertiesClass_Members[] =
{
	{ "maxBreakingTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minPedalInputToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isConnectedToHandbrake", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_300;
hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_300(
	"hkVehicleDefaultBrakeWheelBrakingProperties",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultBrake_WheelBrakingPropertiesClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleDefaultBrakeClass_Members[] =
{
	{ "wheelBrakingProperties", &hkVehicleDefaultBrakeWheelBrakingPropertiesClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsMinTimeToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleDefaultBrakeClass_300;
hkClass hkVehicleDefaultBrakeClass_300(
	"hkVehicleDefaultBrake",
	&hkVehicleBrakeClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultBrakeClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/engine/default/hkVehicleDefaultEngine.h'



//
// Class hkVehicleDefaultEngine
//
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
extern hkClass hkVehicleEngineClass_300;

extern hkClass hkVehicleDefaultEngineClass_300;
hkClass hkVehicleDefaultEngineClass_300(
	"hkVehicleDefaultEngine",
	&hkVehicleEngineClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultEngineClass_Members),
	10, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/steering/default/hkVehicleDefaultSteering.h'



// External pointer and enum types

//
// Class hkVehicleDefaultSteering
//
static hkInternalClassMember hkVehicleDefaultSteeringClass_Members[] =
{
	{ "maxSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxSpeedFullSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "doesWheelSteer", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_BOOL, 0, 0, 0 }
};
extern hkClass hkVehicleSteeringClass_300;

extern hkClass hkVehicleDefaultSteeringClass_300;
hkClass hkVehicleDefaultSteeringClass_300(
	"hkVehicleDefaultSteering",
	&hkVehicleSteeringClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSteeringClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/suspension/default/hkVehicleDefaultSuspension.h'



// External pointer and enum types
extern hkClass WheelSpringSuspensionParametersClass_300;

//
// Class hkVehicleDefaultSuspension
//

//
// Class hkVehicleDefaultSuspension::WheelSpringSuspensionParameters
//
static hkInternalClassMember hkVehicleDefaultSuspension_WheelSpringSuspensionParametersClass_Members[] =
{
	{ "strength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dampingCompression", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "dampingRelaxation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_300;
hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_300(
	"hkVehicleDefaultSuspensionWheelSpringSuspensionParameters",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSuspension_WheelSpringSuspensionParametersClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleDefaultSuspensionClass_Members[] =
{
	{ "wheelSpringParams", &hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkVehicleSuspensionClass_300;

extern hkClass hkVehicleDefaultSuspensionClass_300;
hkClass hkVehicleDefaultSuspensionClass_300(
	"hkVehicleDefaultSuspension",
	&hkVehicleSuspensionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultSuspensionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/transmission/default/hkVehicleDefaultTransmission.h'



// External pointer and enum types

//
// Class hkVehicleDefaultTransmission
//
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
extern hkClass hkVehicleTransmissionClass_300;

extern hkClass hkVehicleDefaultTransmissionClass_300;
hkClass hkVehicleDefaultTransmissionClass_300(
	"hkVehicleDefaultTransmission",
	&hkVehicleTransmissionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultTransmissionClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/velocitydamper/default/hkVehicleDefaultVelocityDamper.h'



//
// Class hkVehicleDefaultVelocityDamper
//
static hkInternalClassMember hkVehicleDefaultVelocityDamperClass_Members[] =
{
	{ "normalSpinDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionSpinDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionThreshold", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleVelocityDamperClass_300;

extern hkClass hkVehicleDefaultVelocityDamperClass_300;
hkClass hkVehicleDefaultVelocityDamperClass_300(
	"hkVehicleDefaultVelocityDamper",
	&hkVehicleVelocityDamperClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDefaultVelocityDamperClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/driverinput/hkVehicleDriverInput.h'



//
// Class hkVehicleDriverInputStatus
//

hkClass hkVehicleDriverInputStatusClass_300(
	"hkVehicleDriverInputStatus",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);

//
// Class hkVehicleDriverInput
//

//
// Class hkVehicleDriverInput::FilteredDriverInputOutput
//
static hkInternalClassMember hkVehicleDriverInput_FilteredDriverInputOutputClass_Members[] =
{
	{ "acceleratorPedalInput", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "brakePedalInput", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "steeringWheelInput", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "handbrakeOn", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tryingToReverse", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleDriverInputFilteredDriverInputOutputClass_300;
hkClass hkVehicleDriverInputFilteredDriverInputOutputClass_300(
	"hkVehicleDriverInputFilteredDriverInputOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleDriverInput_FilteredDriverInputOutputClass_Members),
	5, // members
	HK_NULL // defaults
	);

hkClass hkVehicleDriverInputClass_300(
	"hkVehicleDriverInput",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/engine/hkVehicleEngine.h'



//
// Class hkVehicleEngine
//

//
// Class hkVehicleEngine::EngineOutput
//
static hkInternalClassMember hkVehicleEngine_EngineOutputClass_Members[] =
{
	{ "torque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rpm", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleEngineEngineOutputClass_300;
hkClass hkVehicleEngineEngineOutputClass_300(
	"hkVehicleEngineEngineOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleEngine_EngineOutputClass_Members),
	2, // members
	HK_NULL // defaults
	);

hkClass hkVehicleEngineClass_300(
	"hkVehicleEngine",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkconstraintsolver/vehiclefriction/hkVehicleFrictionSolver.h'



// External pointer and enum types
extern hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_300;
extern hkClass hkVehicleFrictionStatusAxisStatusClass_300;

//
// Class hkVehicleFrictionDescription
//

//
// Class hkVehicleFrictionDescription::Cinfo
//
static hkInternalClassMember hkVehicleFrictionDescription_CinfoClass_Members[] =
{
	{ "wheelPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "chassisCenterOfMass", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "directionRight", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "directionFront", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "directionUp", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisFrictionInertiaInv", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelAxleAngularInertia", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "wheelRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 2, 0, 0 },
	{ "frictionEqualizer", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleFrictionDescriptionCinfoClass_300;
hkClass hkVehicleFrictionDescriptionCinfoClass_300(
	"hkVehicleFrictionDescriptionCinfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescription_CinfoClass_Members),
	10, // members
	HK_NULL // defaults
	);

//
// Class hkVehicleFrictionDescription::AxisDescription
//
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
hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_300(
	"hkVehicleFrictionDescriptionAxisDescription",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescription_AxisDescriptionClass_Members),
	10, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleFrictionDescriptionClass_Members[] =
{
	{ "wheelDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axleDescr", &hkVehicleFrictionDescriptionAxisDescriptionClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionDescriptionClass_300(
	"hkVehicleFrictionDescription",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescriptionClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkVehicleFrictionStatus
//

//
// Class hkVehicleFrictionStatus::AxisStatus
//
static hkInternalClassMember hkVehicleFrictionStatus_AxisStatusClass_Members[] =
{
	{ "forward_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "skid_energy_density", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_force", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "delayed_forward_impulse", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sideRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkVehicleFrictionStatusAxisStatusClass_300(
	"hkVehicleFrictionStatusAxisStatus",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatus_AxisStatusClass_Members),
	7, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleFrictionStatusClass_Members[] =
{
	{ "axis", &hkVehicleFrictionStatusAxisStatusClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
extern hkClass hkVehicleFrictionStatusClass_300;
hkClass hkVehicleFrictionStatusClass_300(
	"hkVehicleFrictionStatus",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatusClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/hkVehicleInstance.h'



// External pointer and enum types
extern hkClass WheelInfoClass_300;
extern hkClass hkVehicleWheelCollideClass_300;

//
// Class hkVehicleInstance
//

//
// Class hkVehicleInstance::WheelInfo
//
static hkInternalClassMember hkVehicleInstance_WheelInfoClass_Members[] =
{
	{ "contactPoint", &hkContactPointClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactBody", &hkRigidBodyClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
extern hkClass hkVehicleInstanceWheelInfoClass_300;
hkClass hkVehicleInstanceWheelInfoClass_300(
	"hkVehicleInstanceWheelInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleInstance_WheelInfoClass_Members),
	17, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleInstanceClass_Members[] =
{
	{ "data", &hkVehicleDataClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "driverInput", &hkVehicleDriverInputClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "steering", &hkVehicleSteeringClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "engine", &hkVehicleEngineClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transmission", &hkVehicleTransmissionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "brake", &hkVehicleBrakeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "suspension", &hkVehicleSuspensionClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "aerodynamics", &hkVehicleAerodynamicsClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelCollide", &hkVehicleWheelCollideClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tyreMarks", &hkTyremarksInfoClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chassis", &hkRigidBodyClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "velocityDamper", &hkVehicleVelocityDamperClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsInfo", &hkVehicleInstanceWheelInfoClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "frictionStatus", &hkVehicleFrictionStatusClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deviceStatus", &hkVehicleDriverInputStatusClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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

extern hkClass hkVehicleInstanceClass_300;
hkClass hkVehicleInstanceClass_300(
	"hkVehicleInstance",
	&hkUnaryActionClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleInstanceClass_Members),
	26, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/wheelcollide/raycast/hkVehicleRaycastWheelCollide.h'



// External pointer and enum types
extern hkClass hkRejectRayChassisListenerClass_300;

//
// Class hkRejectRayChassisListener
//
static hkInternalClassMember hkRejectRayChassisListenerClass_Members[] =
{
	{ "chassis", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

hkClass hkRejectRayChassisListenerClass_300(
	"hkRejectRayChassisListener",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	1, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRejectRayChassisListenerClass_Members),
	1, // members
	HK_NULL // defaults
	);

//
// Class hkVehicleRaycastWheelCollide
//
static hkInternalClassMember hkVehicleRaycastWheelCollideClass_Members[] =
{
	{ "wheelCollisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "phantom", &hkAabbPhantomClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rejectRayChassisListener", &hkRejectRayChassisListenerClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleRaycastWheelCollideClass_300;
hkClass hkVehicleRaycastWheelCollideClass_300(
	"hkVehicleRaycastWheelCollide",
	&hkVehicleWheelCollideClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleRaycastWheelCollideClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/steering/hkVehicleSteering.h'



// External pointer and enum types

//
// Class hkVehicleSteering
//

//
// Class hkVehicleSteering::SteeringAnglesOutput
//
static hkInternalClassMember hkVehicleSteering_SteeringAnglesOutputClass_Members[] =
{
	{ "mainSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelsSteeringAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
extern hkClass hkVehicleSteeringSteeringAnglesOutputClass_300;
hkClass hkVehicleSteeringSteeringAnglesOutputClass_300(
	"hkVehicleSteeringSteeringAnglesOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleSteering_SteeringAnglesOutputClass_Members),
	2, // members
	HK_NULL // defaults
	);

hkClass hkVehicleSteeringClass_300(
	"hkVehicleSteering",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/suspension/hkVehicleSuspension.h'



// External pointer and enum types
extern hkClass SuspensionWheelParametersClass_300;

//
// Class hkVehicleSuspension
//

//
// Class hkVehicleSuspension::SuspensionWheelParameters
//
static hkInternalClassMember hkVehicleSuspension_SuspensionWheelParametersClass_Members[] =
{
	{ "hardpointCs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "directionCs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleSuspensionSuspensionWheelParametersClass_300;
hkClass hkVehicleSuspensionSuspensionWheelParametersClass_300(
	"hkVehicleSuspensionSuspensionWheelParameters",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleSuspension_SuspensionWheelParametersClass_Members),
	3, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleSuspensionClass_Members[] =
{
	{ "wheelParams", &hkVehicleSuspensionSuspensionWheelParametersClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkVehicleSuspensionClass_300(
	"hkVehicleSuspension",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleSuspensionClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/transmission/hkVehicleTransmission.h'



//
// Class hkVehicleTransmission
//

//
// Class hkVehicleTransmission::TransmissionOutput
//
static hkInternalClassMember hkVehicleTransmission_TransmissionOutputClass_Members[] =
{
	{ "transmissionRPM", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "mainTransmittedTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelsTransmittedTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "numWheelsTramsmittedTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isReversing", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "currentGear", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "delayed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clutchDelayCountdown", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleTransmissionTransmissionOutputClass_300;
hkClass hkVehicleTransmissionTransmissionOutputClass_300(
	"hkVehicleTransmissionTransmissionOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleTransmission_TransmissionOutputClass_Members),
	8, // members
	HK_NULL // defaults
	);

hkClass hkVehicleTransmissionClass_300(
	"hkVehicleTransmission",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/velocitydamper/hkVehicleVelocityDamper.h'



//
// Class hkVehicleVelocityDamper
//

hkClass hkVehicleVelocityDamperClass_300(
	"hkVehicleVelocityDamper",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	HK_NULL,
	0, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkvehicle/wheelcollide/hkVehicleWheelCollide.h'



// External pointer and enum types

//
// Class hkVehicleWheelCollide
//

//
// Class hkVehicleWheelCollide::CollisionDetectionWheelOutput
//
static hkInternalClassMember hkVehicleWheelCollide_CollisionDetectionWheelOutputClass_Members[] =
{
	{ "contactPoint", &hkContactPointClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactBody", &hkRigidBodyClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "contactShapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "currentSuspensionLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionRelativeVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clippedInvContactDotSuspension", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleWheelCollideCollisionDetectionWheelOutputClass_300;
hkClass hkVehicleWheelCollideCollisionDetectionWheelOutputClass_300(
	"hkVehicleWheelCollideCollisionDetectionWheelOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleWheelCollide_CollisionDetectionWheelOutputClass_Members),
	7, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkVehicleWheelCollideClass_Members[] =
{
	{ "alreadyUsed", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkVehicleWheelCollideClass_300(
	"hkVehicleWheelCollide",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleWheelCollideClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkconstraintsolver/accumulator/hkVelocityAccumulator.h'



//
// Class hkVelocityAccumulator
//

//
// Enum hkVelocityAccumulator::hkAccumulatorType
//
static const hkInternalClassEnumItem hkVelocityAccumulatorhkAccumulatorTypeEnumItems_300[] =
{
	{0, "HK_RIGID_BODY"},
	{1, "HK_KEYFRAMED_RIGID_BODY"},
	{2, "HK_END"},
};
static const hkInternalClassEnum hkVelocityAccumulatorEnums[] = {
	{"hkAccumulatorType", hkVelocityAccumulatorhkAccumulatorTypeEnumItems_300, 3 }
};
extern const hkClassEnum* hkVelocityAccumulatorhkAccumulatorTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkVelocityAccumulatorEnums[0]);
static hkInternalClassMember hkVelocityAccumulatorClass_Members[] =
{
	{ "type", HK_NULL, hkVelocityAccumulatorhkAccumulatorTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkVelocityAccumulatorClass_300(
	"hkVelocityAccumulator",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkVelocityAccumulatorEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkVelocityAccumulatorClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/motor/hkVelocityMotor.h'



//
// Class hkVelocityMotor
//
static hkInternalClassMember hkVelocityMotorClass_Members[] =
{
	{ "velocityTarget", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVelocityMotorClass_300;
hkClass hkVelocityMotorClass_300(
	"hkVelocityMotor",
	&hkMotorControllerClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVelocityMotorClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/animation/waveletcompressed/hkWaveletCompressedAnimation.h'



// External pointer and enum types
extern hkClass hkWaveletCompressedAnimationQuantizationFormatClass_300;

//
// Class hkWaveletCompressedAnimation
//

//
// Class hkWaveletCompressedAnimation::CompressionParams
//
static hkInternalClassMember hkWaveletCompressedAnimation_CompressionParamsClass_Members[] =
{
	{ "quantizationBits", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scaleTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserve", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "truncProp", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absolutePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "relativePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotationTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkWaveletCompressedAnimationCompressionParamsClass_300;
hkClass hkWaveletCompressedAnimationCompressionParamsClass_300(
	"hkWaveletCompressedAnimationCompressionParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletCompressedAnimation_CompressionParamsClass_Members),
	8, // members
	HK_NULL // defaults
	);

//
// Class hkWaveletCompressedAnimation::QuantizationFormat
//
static hkInternalClassMember hkWaveletCompressedAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkWaveletCompressedAnimationQuantizationFormatClass_300(
	"hkWaveletCompressedAnimationQuantizationFormat",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletCompressedAnimation_QuantizationFormatClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkWaveletCompressedAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkWaveletCompressedAnimationQuantizationFormatClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "blockIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};

extern hkClass hkWaveletCompressedAnimationClass_300;
hkClass hkWaveletCompressedAnimationClass_300(
	"hkWaveletCompressedAnimation",
	&hkSkeletalAnimationClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletCompressedAnimationClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h'



// External pointer and enum types
extern hkClass hkWheelConstraintDataConstraintBasisAClass_300;
extern hkClass hkWheelConstraintDataConstraintBasisBClass_300;

//
// Class hkWheelConstraintData
//

//
// Class hkWheelConstraintData::ConstraintBasisA
//
static hkInternalClassMember hkWheelConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataConstraintBasisAClass_300(
	"hkWheelConstraintDataConstraintBasisA",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisAClass_Members),
	2, // members
	HK_NULL // defaults
	);

//
// Class hkWheelConstraintData::ConstraintBasisB
//
static hkInternalClassMember hkWheelConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "steeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToSteeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpToSuspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkWheelConstraintDataConstraintBasisBClass_300(
	"hkWheelConstraintDataConstraintBasisB",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisBClass_Members),
	6, // members
	HK_NULL // defaults
	);

//
// Class hkWheelConstraintData::Runtime
//
static hkInternalClassMember hkWheelConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 6, 0, 0 }
};
extern hkClass hkWheelConstraintDataRuntimeClass_300;
hkClass hkWheelConstraintDataRuntimeClass_300(
	"hkWheelConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_RuntimeClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkWheelConstraintDataClass_Members[] =
{
	{ "basisA", &hkWheelConstraintDataConstraintBasisAClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkWheelConstraintDataConstraintBasisBClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMinLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMaxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkWheelConstraintDataClass_300;
hkClass hkWheelConstraintDataClass_300(
	"hkWheelConstraintData",
	&hkConstraintDataClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintDataClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/world/hkWorldCinfo.h'



// External pointer and enum types
extern hkClass hkWorldMemoryWatchDogClass_300;

//
// Class hkWorldCinfo
//

//
// Enum hkWorldCinfo::SolverType
//
static const hkInternalClassEnumItem hkWorldCinfoSolverTypeEnumItems_300[] =
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

//
// Enum hkWorldCinfo::SimulationType
//
static const hkInternalClassEnumItem hkWorldCinfoSimulationTypeEnumItems_300[] =
{
	{0, "SIMULATION_TYPE_INVALID"},
	{1, "SIMULATION_TYPE_DISCRETE"},
	{2, "SIMULATION_TYPE_ASYNCHRONOUS"},
	{3, "SIMULATION_TYPE_HALFSTEP"},
	{4, "SIMULATION_TYPE_CONTINUOUS"},
};

//
// Enum hkWorldCinfo::ContactPointGeneration
//
static const hkInternalClassEnumItem hkWorldCinfoContactPointGenerationEnumItems_300[] =
{
	{0, "CONTACT_POINT_ACCEPT_ALWAYS"},
	{1, "CONTACT_POINT_REJECT_DUBIOUS"},
	{2, "CONTACT_POINT_REJECT_MANY"},
};
static const hkInternalClassEnum hkWorldCinfoEnums[] = {
	{"SolverType", hkWorldCinfoSolverTypeEnumItems_300, 11 },
	{"SimulationType", hkWorldCinfoSimulationTypeEnumItems_300, 5 },
	{"ContactPointGeneration", hkWorldCinfoContactPointGenerationEnumItems_300, 3 }
};
extern const hkClassEnum* hkWorldCinfoSolverTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[0]);
extern const hkClassEnum* hkWorldCinfoSimulationTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[1]);
extern const hkClassEnum* hkWorldCinfoContactPointGenerationEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[2]);
static hkInternalClassMember hkWorldCinfoClass_Members[] =
{
	{ "gravity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseQuerySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseWorldAabb", &hkAabbClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilter", &hkCollisionFilterClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "expectedMaxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "expectedMinPsiDeltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "memoryWatchDog", &hkWorldMemoryWatchDogClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "broadPhaseNumMarkers", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactPointGeneration", HK_NULL, hkWorldCinfoContactPointGenerationEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "solverTau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "solverDamp", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "solverIterations", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boxBoxUseAggresiveCompletion", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "iterativeLinearCastEarlyOutDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "iterativeLinearCastMaxIterations", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "highFrequencyDeactivationPeriod", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencyDeactivationPeriod", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shouldActivateOnRigidBodyTransformChange", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "toiCollisionResponseRotateNormal", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "enableDeactivation", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "simulationType", HK_NULL, hkWorldCinfoSimulationTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "enableSimulationIslands", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkWorldCinfoClass_300(
	"hkWorldCinfo",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkWorldCinfoEnums),
	3, // enums
	reinterpret_cast<const hkClassMember*>(hkWorldCinfoClass_Members),
	23, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/world/memory/hkWorldMemoryWatchDog.h'



//
// Class hkWorldMemoryWatchDog
//
static hkInternalClassMember hkWorldMemoryWatchDogClass_Members[] =
{
	{ "memoryLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkWorldMemoryWatchDogClass_300(
	"hkWorldMemoryWatchDog",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWorldMemoryWatchDogClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/world/hkWorldObject.h'



// External pointer and enum types

//
// Class hkWorldObject
//

//
// Enum hkWorldObject::BroadPhaseType
//
static const hkInternalClassEnumItem hkWorldObjectBroadPhaseTypeEnumItems_300[] =
{
	{0, "BROAD_PHASE_INVALID"},
	{1, "BROAD_PHASE_ENTITY"},
	{2, "BROAD_PHASE_PHANTOM"},
	{3, "BROAD_PHASE_MAX_ID"},
};
static const hkInternalClassEnum hkWorldObjectEnums[] = {
	{"BroadPhaseType", hkWorldObjectBroadPhaseTypeEnumItems_300, 4 }
};
extern const hkClassEnum* hkWorldObjectBroadPhaseTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkWorldObjectEnums[0]);
static hkInternalClassMember hkWorldObjectClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "collidable", &hkLinkedCollidableClass_300, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "properties", &hkPropertyClass_300, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkWorldObjectClass_300(
	"hkWorldObject",
	&hkReferencedObjectClass_300, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkWorldObjectEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkWorldObjectClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/camera/hkxCamera.h'



//
// Class hkxCamera
//
static hkInternalClassMember hkxCameraClass_Members[] =
{
	{ "from", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "focus", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "fov", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "far", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "near", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxCameraClass_300;
hkClass hkxCameraClass_300(
	"hkxCamera",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxCameraClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/hkxIndexBuffer.h'



// External pointer and enum types

//
// Class hkxIndexBuffer
//

//
// Enum hkxIndexBuffer::IndexType
//
static const hkInternalClassEnumItem hkxIndexBufferIndexTypeEnumItems_300[] =
{
	{0, "INDEX_TYPE_INVALID"},
	{1, "INDEX_TYPE_TRI_LIST"},
	{2, "INDEX_TYPE_TRI_STRIP"},
	{3, "INDEX_TYPE_TRI_FAN"},
	{4, "INDEX_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxIndexBufferEnums[] = {
	{"IndexType", hkxIndexBufferIndexTypeEnumItems_300, 5 }
};
extern const hkClassEnum* hkxIndexBufferIndexTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkxIndexBufferEnums[0]);
static hkInternalClassMember hkxIndexBufferClass_Members[] =
{
	{ "indexType", HK_NULL, hkxIndexBufferIndexTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "vertexBaseOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxIndexBufferClass_300;
hkClass hkxIndexBufferClass_300(
	"hkxIndexBuffer",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkxIndexBufferEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkxIndexBufferClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/light/hkxLight.h'



//
// Class hkxLight
//

//
// Enum hkxLight::LightType
//
static const hkInternalClassEnumItem hkxLightLightTypeEnumItems_300[] =
{
	{0, "POINT_LIGHT"},
	{1, "DIRECTIONAL_LIGHT"},
	{2, "SPOT_LIGHT"},
};
static const hkInternalClassEnum hkxLightEnums[] = {
	{"LightType", hkxLightLightTypeEnumItems_300, 3 }
};
extern const hkClassEnum* hkxLightLightTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkxLightEnums[0]);
static hkInternalClassMember hkxLightClass_Members[] =
{
	{ "type", HK_NULL, hkxLightLightTypeEnum_300, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "direction", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "color", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxLightClass_300;
hkClass hkxLightClass_300(
	"hkxLight",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkxLightEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkxLightClass_Members),
	5, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/material/hkxMaterial.h'



// External pointer and enum types
extern hkClass TextureStageClass_300;
extern hkClass hkxMaterialClass_300;

//
// Class hkxMaterial
//

//
// Enum hkxMaterial::TextureType
//
static const hkInternalClassEnumItem hkxMaterialTextureTypeEnumItems_300[] =
{
	{0, "TEX_UNKNOWN"},
	{1, "TEX_DIFFUSE"},
	{2, "TEX_REFLECTION"},
	{3, "TEX_BUMP"},
	{4, "TEX_NORMAL"},
	{5, "TEX_DISPLACEMENT"},
};
static const hkInternalClassEnum hkxMaterialEnums[] = {
	{"TextureType", hkxMaterialTextureTypeEnumItems_300, 6 }
};
extern const hkClassEnum* hkxMaterialTextureTypeEnum_300 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEnums[0]);

//
// Class hkxMaterial::TextureStage
//
static hkInternalClassMember hkxMaterial_TextureStageClass_Members[] =
{
	{ "texture", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "usageHint", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxMaterialTextureStageClass_300;
hkClass hkxMaterialTextureStageClass_300(
	"hkxMaterialTextureStage",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxMaterial_TextureStageClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkxMaterialClass_Members[] =
{
	{ "stages", &hkxMaterialTextureStageClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "diffuseColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ambientColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "specularColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "emissiveColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subMaterials", &hkxMaterialClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMaterialClass_300(
	"hkxMaterial",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkxMaterialEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkxMaterialClass_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/hkxMesh.h'



// External pointer and enum types
extern hkClass hkxMeshSectionClass_300;

//
// Class hkxMesh
//
static hkInternalClassMember hkxMeshClass_Members[] =
{
	{ "sections", &hkxMeshSectionClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMeshClass_300(
	"hkxMesh",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxMeshClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/hkxMeshSection.h'



// External pointer and enum types
extern hkClass hkxVertexBufferClass_300;

//
// Class hkxMeshSection
//
static hkInternalClassMember hkxMeshSectionClass_Members[] =
{
	{ "vertexBuffer", &hkxVertexBufferClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "indexBuffers", &hkxIndexBufferClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkxMaterialClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxMeshSectionClass_300(
	"hkxMeshSection",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxMeshSectionClass_Members),
	3, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/graph/hkxNode.h'



// External pointer and enum types
extern hkClass AnnotationDataClass_300;
extern hkClass hkMatrix4Class_300;
extern hkClass hkxNodeClass_300;

//
// Class hkxNode
//

//
// Class hkxNode::AnnotationData
//
static hkInternalClassMember hkxNode_AnnotationDataClass_Members[] =
{
	{ "time", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "description", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
extern hkClass hkxNodeAnnotationDataClass_300;
hkClass hkxNodeAnnotationDataClass_300(
	"hkxNodeAnnotationData",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxNode_AnnotationDataClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkxNodeClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "object", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "keys", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "children", &hkxNodeClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "annotations", &hkxNodeAnnotationDataClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "userProperties", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "selected", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxNodeClass_300(
	"hkxNode",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxNodeClass_Members),
	7, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/scene/hkxScene.h'



// External pointer and enum types
extern hkClass hkxSkinBindingClass_300;
extern hkClass hkxTextureFileClass_300;
extern hkClass hkxTextureInplaceClass_300;

//
// Class hkxScene
//
static hkInternalClassMember hkxSceneClass_Members[] =
{
	{ "modeller", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "asset", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "sceneLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rootNode", &hkxNodeClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "cameras", &hkxCameraClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "lights", &hkxLightClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "meshes", &hkxMeshClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materials", &hkxMaterialClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "inplaceTextures", &hkxTextureInplaceClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "externalTextures", &hkxTextureFileClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skinBindings", &hkxSkinBindingClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkxSceneClass_300;
hkClass hkxSceneClass_300(
	"hkxScene",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxSceneClass_Members),
	11, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/skin/hkxSkinBinding.h'



// External pointer and enum types

//
// Class hkxSkinBinding
//
static hkInternalClassMember hkxSkinBindingClass_Members[] =
{
	{ "mesh", &hkxMeshClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", &hkxNodeClass_300, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "initSkinTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxSkinBindingClass_300(
	"hkxSkinBinding",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxSkinBindingClass_Members),
	4, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/material/hkxTextureFile.h'



//
// Class hkxTextureFile
//
static hkInternalClassMember hkxTextureFileClass_Members[] =
{
	{ "filename", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
hkClass hkxTextureFileClass_300(
	"hkxTextureFile",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxTextureFileClass_Members),
	1, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/material/hkxTextureInplace.h'



// External pointer and enum types

//
// Class hkxTextureInplace
//
static hkInternalClassMember hkxTextureInplaceClass_Members[] =
{
	{ "fileType", HK_NULL, HK_NULL, hkClassMember::TYPE_CHAR, hkClassMember::TYPE_VOID, 4, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};
hkClass hkxTextureInplaceClass_300(
	"hkxTextureInplace",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxTextureInplaceClass_Members),
	2, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/hkxVertexFormat.h'



//
// Class hkxVertexFormat
//
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
extern hkClass hkxVertexFormatClass_300;
hkClass hkxVertexFormatClass_300(
	"hkxVertexFormat",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexFormatClass_Members),
	12, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/formats/hkxVertexP4N4C1T2.h'



//
// Class hkxVertexP4N4C1T2
//
static hkInternalClassMember hkxVertexP4N4C1T2Class_Members[] =
{
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "normal", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "diffuse", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "u", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "v", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "padding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxVertexP4N4C1T2Class_300;
hkClass hkxVertexP4N4C1T2Class_300(
	"hkxVertexP4N4C1T2",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4C1T2Class_Members),
	6, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/formats/hkxVertexP4N4T4B4C1T2.h'



//
// Class hkxVertexP4N4T4B4C1T2
//
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
extern hkClass hkxVertexP4N4T4B4C1T2Class_300;
hkClass hkxVertexP4N4T4B4C1T2Class_300(
	"hkxVertexP4N4T4B4C1T2",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4C1T2Class_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/formats/hkxVertexP4N4T4B4W4I4C1Q2.h'



//
// Class hkxVertexP4N4T4B4W4I4C1Q2
//
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
extern hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_300;
hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_300(
	"hkxVertexP4N4T4B4W4I4C1Q2",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4W4I4C1Q2Class_Members),
	9, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/formats/hkxVertexP4N4T4B4W4I4Q4.h'



//
// Class hkxVertexP4N4T4B4W4I4Q4
//
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
extern hkClass hkxVertexP4N4T4B4W4I4Q4Class_300;
hkClass hkxVertexP4N4T4B4W4I4Q4Class_300(
	"hkxVertexP4N4T4B4W4I4Q4",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4T4B4W4I4Q4Class_Members),
	8, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/formats/hkxVertexP4N4W4I4C1Q2.h'



//
// Class hkxVertexP4N4W4I4C1Q2
//
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
extern hkClass hkxVertexP4N4W4I4C1Q2Class_300;
hkClass hkxVertexP4N4W4I4C1Q2Class_300(
	"hkxVertexP4N4W4I4C1Q2",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexP4N4W4I4C1Q2Class_Members),
	7, // members
	HK_NULL // defaults
	);

// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : ALL
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkscenedata/mesh/hkxVertexBuffer.hkclass'

// External pointer and enum types
extern const hkClass hkxVertexFormatClass;

//
// Class hkxVertexBuffer
//
static hkInternalClassMember hkxVertexBufferClass_Members_300[] =
{
	{ "vertexData", HK_NULL, HK_NULL, hkClassMember::TYPE_HOMOGENEOUSARRAY, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "format", &hkxVertexFormatClass_300, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, hkClassMember::POINTER_OPTIONAL, 0 }
};
hkClass hkxVertexBufferClass_300(
	"hkxVertexBuffer",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexBufferClass_Members_300),
	2, // members
	HK_NULL // defaults
	);


extern hkClass* const hkHavok300Classes[];
hkClass* const hkHavok300Classes[] =
{
	&hkAabbClass_300,
	&hkAabbPhantomClass_300,
	&hkActionClass_300,
	&hkAngularDashpotActionClass_300,
	&hkAnimationBindingClass_300,
	&hkAnimationContainerClass_300,
	&hkAnimationTrackAnnotationClass_300,
	&hkAnimationTrackClass_300,
	&hkArrayActionClass_300,
	&hkBallAndSocketConstraintDataRuntimeClass_300,
	&hkBallAndSocketConstraintDataClass_300,
	&hkBaseObjectClass_300,
	&hkBinaryActionClass_300,
	&hkBlendingMotorClass_300,
	&hkBoneAttachmentClass_300,
	&hkBoneClass_300,
	&hkBoxMotionClass_300,
	&hkBoxShapeClass_300,
	&hkBreakableConstraintDataClass_300,
	&hkBroadPhaseHandleClass_300,
	&hkBvShapeClass_300,
	&hkBvTreeShapeClass_300,
	&hkCachingShapePhantomhkCollisionDetailClass_300,
	&hkCachingShapePhantomClass_300,
	&hkCapsuleShapeClass_300,
	&hkCdBodyClass_300,
	&hkCharacterProxyCinfoClass_300,
	&hkClassClass_300,
	&hkClassEnumItemClass_300,
	&hkClassEnumClass_300,
	&hkClassMemberClass_300,
	&hkCollidableClass_300,
	&hkCollidableCollidableFilterClass_300,
	&hkCollisionFilterClass_300,
	&hkCollisionFilterListClass_300,
	&hkConstrainedSystemFilterClass_300,
	&hkConstraintDataRuntimeInfoClass_300,
	&hkConstraintDataClass_300,
	&hkConstraintInfoClass_300,
	&hkConstraintInstanceClass_300,
	&hkContactPointClass_300,
	&hkConvexListShapeClass_300,
	&hkConvexPieceMeshShapeStatsClass_300,
	&hkConvexPieceMeshShapeClass_300,
	&hkConvexPieceStreamDataClass_300,
	&hkConvexShapeClass_300,
	&hkConvexTransformShapeClass_300,
	&hkConvexTranslateShapeClass_300,
	&hkConvexVerticesShapeFourVectorsClass_300,
	&hkConvexVerticesShapeClass_300,
	&hkCylinderShapeClass_300,
	&hkDashpotActionClass_300,
	&hkDefaultExtractedMotionMotionExtractionOptionsClass_300,
	&hkDefaultExtractedMotionClass_300,
	&hkDeltaCompressedAnimationCompressionParamsClass_300,
	&hkDeltaCompressedAnimationQuantizationFormatClass_300,
	&hkDeltaCompressedAnimationClass_300,
	&hkDisableEntityCollisionFilterClass_300,
	&hkEntityClass_300,
	&hkEntityDeactivatorClass_300,
	&hkExtractedMotionClass_300,
	&hkFakeRigidBodyDeactivatorClass_300,
	&hkFastMeshShapeClass_300,
	&hkFixedRigidMotionClass_300,
	&hkGenericConstraintDataClass_300,
	&hkGenericConstraintDataSchemeClass_300,
	&hkGroupCollisionFilterClass_300,
	&hkGroupFilterClass_300,
	&hkHeightFieldShapeCollideSpheresInputClass_300,
	&hkHeightFieldShapehkSphereCastInputClass_300,
	&hkHeightFieldShapeClass_300,
	&hkHingeConstraintDataRuntimeClass_300,
	&hkHingeConstraintDataConstraintBasisAClass_300,
	&hkHingeConstraintDataConstraintBasisBClass_300,
	&hkHingeConstraintDataClass_300,
	&hkInterleavedAnimationClass_300,
	&hkKeyframedRigidMotionClass_300,
	&hkLimitedHingeConstraintDataRuntimeClass_300,
	&hkLimitedHingeConstraintDataConstraintBasisAClass_300,
	&hkLimitedHingeConstraintDataConstraintBasisBClass_300,
	&hkLimitedHingeConstraintDataClass_300,
	&hkLinearParametricCurveClass_300,
	&hkLinkedCollidableCollisionEntryClass_300,
	&hkLinkedCollidableClass_300,
	&hkListShapeChildInfoClass_300,
	&hkListShapeClass_300,
	&hkMalleableConstraintDataClass_300,
	&hkMaterialClass_300,
	&hkMeshBindingClass_300,
	&hkMeshMaterialClass_300,
	&hkMeshShapeSubpartClass_300,
	&hkMeshShapeClass_300,
	&hkMoppBvTreeShapeClass_300,
	&hkMoppCodeCodeInfoClass_300,
	&hkMoppCodeClass_300,
	&hkMotionClass_300,
	&hkMotionStateClass_300,
	&hkMotorActionClass_300,
	&hkMotorControllerClass_300,
	&hkMouseSpringActionClass_300,
	&hkMultiRayShapeRayClass_300,
	&hkMultiRayShapeClass_300,
	&hkMultiSphereShapeClass_300,
	&hkNullCollisionFilterClass_300,
	&hkPackfileHeaderClass_300,
	&hkPackfileSectionHeaderClass_300,
	&hkPairwiseCollisionFilterCollisionPairClass_300,
	&hkPairwiseCollisionFilterClass_300,
	&hkParametricCurveClass_300,
	&hkPhantomCallbackShapeClass_300,
	&hkPhantomClass_300,
	&hkPhysicsDataClass_300,
	&hkPhysicsSystemClass_300,
	&hkPlaneShapeClass_300,
	&hkPointToPathConstraintDataRuntimeClass_300,
	&hkPointToPathConstraintDataClass_300,
	&hkPointToPlaneConstraintDataRuntimeClass_300,
	&hkPointToPlaneConstraintDataClass_300,
	&hkPoweredHingeConstraintDataRuntimeClass_300,
	&hkPoweredHingeConstraintDataClass_300,
	&hkPoweredRagdollConstraintDataClass_300,
	&hkPrismaticConstraintDataRuntimeClass_300,
	&hkPrismaticConstraintDataConstraintBasisAClass_300,
	&hkPrismaticConstraintDataConstraintBasisBClass_300,
	&hkPrismaticConstraintDataClass_300,
	&hkPropertyValueClass_300,
	&hkPropertyClass_300,
	&hkRagdollConstraintDataRuntimeClass_300,
	&hkRagdollConstraintDataConstraintBasisAClass_300,
	&hkRagdollConstraintDataConstraintBasisBClass_300,
	&hkRagdollConstraintDataClass_300,
	&hkRagdollInstanceClass_300,
	&hkRayCollidableFilterClass_300,
	&hkRayShapeCollectionFilterClass_300,
	&hkReferencedObjectClass_300,
	&hkReorientActionClass_300,
	&hkRigidAccumulatorClass_300,
	&hkRigidBodyClass_300,
	&hkRigidBodyDeactivatorClass_300,
	&hkRigidMotionClass_300,
	&hkRootLevelContainerNamedVariantClass_300,
	&hkRootLevelContainerClass_300,
	&hkSampledHeightFieldBaseClass_300,
	&hkSerializedDisplayMarkerClass_300,
	&hkSerializedDisplayMarkerListClass_300,
	&hkSerializedDisplayRbTransformsDisplayTransformPairClass_300,
	&hkSerializedDisplayRbTransformsClass_300,
	&hkShapeClass_300,
	&hkShapeCollectionClass_300,
	&hkShapeCollectionFilterClass_300,
	&hkShapePhantomClass_300,
	&hkShapeRayCastInputClass_300,
	&hkSimpleMeshShapeTriangleClass_300,
	&hkSimpleMeshShapeClass_300,
	&hkSimpleShapePhantomhkCollisionDetailClass_300,
	&hkSimpleShapePhantomClass_300,
	&hkSkeletalAnimationClass_300,
	&hkSkeletonClass_300,
	&hkSolverResultsClass_300,
	&hkSpatialRigidBodyDeactivatorSampleClass_300,
	&hkSpatialRigidBodyDeactivatorClass_300,
	&hkSphereClass_300,
	&hkSphereMotionClass_300,
	&hkSphereRepShapehkCollisionSpheresInfoClass_300,
	&hkSphereRepShapeClass_300,
	&hkSphereShapeClass_300,
	&hkSpringActionClass_300,
	&hkSpringDamperMotorClass_300,
	&hkStabilizedBoxMotionClass_300,
	&hkStabilizedSphereMotionClass_300,
	&hkStiffSpringConstraintDataRuntimeClass_300,
	&hkStiffSpringConstraintDataClass_300,
	&hkStorageMeshShapeSubpartStorageClass_300,
	&hkStorageMeshShapeClass_300,
	&hkStrongestMotorClass_300,
	&hkSweptTransformClass_300,
	&hkTransformShapeClass_300,
	&hkTriPatchTriangleClass_300,
	&hkTriSampledHeightFieldBvTreeShapeClass_300,
	&hkTriSampledHeightFieldCollectionClass_300,
	&hkTriangleShapeClass_300,
	&hkTypedBroadPhaseHandleClass_300,
	&hkTyremarkPointClass_300,
	&hkTyremarksWheelClass_300,
	&hkTyremarksInfoClass_300,
	&hkUnaryActionClass_300,
	&hkVehicleAerodynamicsAerodynamicsDragOutputClass_300,
	&hkVehicleAerodynamicsClass_300,
	&hkVehicleBrakeWheelBreakingOutputClass_300,
	&hkVehicleBrakeClass_300,
	&hkVehicleDataWheelComponentParamsClass_300,
	&hkVehicleDataClass_300,
	&hkVehicleDefaultAerodynamicsClass_300,
	&hkVehicleDriverInputAnalogStatusClass_300,
	&hkVehicleDefaultAnalogDriverInputClass_300,
	&hkVehicleDefaultBrakeWheelBrakingPropertiesClass_300,
	&hkVehicleDefaultBrakeClass_300,
	&hkVehicleDefaultEngineClass_300,
	&hkVehicleDefaultSteeringClass_300,
	&hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_300,
	&hkVehicleDefaultSuspensionClass_300,
	&hkVehicleDefaultTransmissionClass_300,
	&hkVehicleDefaultVelocityDamperClass_300,
	&hkVehicleDriverInputStatusClass_300,
	&hkVehicleDriverInputFilteredDriverInputOutputClass_300,
	&hkVehicleDriverInputClass_300,
	&hkVehicleEngineEngineOutputClass_300,
	&hkVehicleEngineClass_300,
	&hkVehicleFrictionDescriptionCinfoClass_300,
	&hkVehicleFrictionDescriptionAxisDescriptionClass_300,
	&hkVehicleFrictionDescriptionClass_300,
	&hkVehicleFrictionStatusAxisStatusClass_300,
	&hkVehicleFrictionStatusClass_300,
	&hkVehicleInstanceWheelInfoClass_300,
	&hkVehicleInstanceClass_300,
	&hkRejectRayChassisListenerClass_300,
	&hkVehicleRaycastWheelCollideClass_300,
	&hkVehicleSteeringSteeringAnglesOutputClass_300,
	&hkVehicleSteeringClass_300,
	&hkVehicleSuspensionSuspensionWheelParametersClass_300,
	&hkVehicleSuspensionClass_300,
	&hkVehicleTransmissionTransmissionOutputClass_300,
	&hkVehicleTransmissionClass_300,
	&hkVehicleVelocityDamperClass_300,
	&hkVehicleWheelCollideCollisionDetectionWheelOutputClass_300,
	&hkVehicleWheelCollideClass_300,
	&hkVelocityAccumulatorClass_300,
	&hkVelocityMotorClass_300,
	&hkWaveletCompressedAnimationCompressionParamsClass_300,
	&hkWaveletCompressedAnimationQuantizationFormatClass_300,
	&hkWaveletCompressedAnimationClass_300,
	&hkWheelConstraintDataConstraintBasisAClass_300,
	&hkWheelConstraintDataConstraintBasisBClass_300,
	&hkWheelConstraintDataRuntimeClass_300,
	&hkWheelConstraintDataClass_300,
	&hkWorldCinfoClass_300,
	&hkWorldMemoryWatchDogClass_300,
	&hkWorldObjectClass_300,
	&hkxCameraClass_300,
	&hkxIndexBufferClass_300,
	&hkxLightClass_300,
	&hkxMaterialTextureStageClass_300,
	&hkxMaterialClass_300,
	&hkxMeshClass_300,
	&hkxMeshSectionClass_300,
	&hkxNodeAnnotationDataClass_300,
	&hkxNodeClass_300,
	&hkxSceneClass_300,
	&hkxSkinBindingClass_300,
	&hkxTextureFileClass_300,
	&hkxTextureInplaceClass_300,
	&hkxVertexFormatClass_300,
	&hkxVertexP4N4C1T2Class_300,
	&hkxVertexP4N4T4B4C1T2Class_300,
	&hkxVertexP4N4T4B4W4I4C1Q2Class_300,
	&hkxVertexP4N4T4B4W4I4Q4Class_300,
	&hkxVertexP4N4W4I4C1Q2Class_300,
	&hkxVertexBufferClass_300,
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
