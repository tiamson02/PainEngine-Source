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

// Generated from 'hkmath/basetypes/hkAabb.h'


//
// Class hkAabb
//
static hkInternalClassMember hkAabbClass_Members[] =
{
	{ "min", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "max", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkAabbClass_310;
hkClass hkAabbClass_310(
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
extern hkClass hkCollidableClass_310;

//
// Class hkAabbPhantom
//
static hkInternalClassMember hkAabbPhantomClass_Members[] =
{
	{ "aabb", &hkAabbClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "overlappingCollidables", &hkCollidableClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkPhantomClass_310;

extern hkClass hkAabbPhantomClass_310;
hkClass hkAabbPhantomClass_310(
	"hkAabbPhantom",
	&hkPhantomClass_310, // parent
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
extern hkClass hkSimulationIslandClass_310;
extern hkClass hkWorldClass_310;

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
extern hkClass hkReferencedObjectClass_310;

extern hkClass hkActionClass_310;
hkClass hkActionClass_310(
	"hkAction",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkBinaryActionClass_310;

extern hkClass hkAngularDashpotActionClass_310;
hkClass hkAngularDashpotActionClass_310(
	"hkAngularDashpotAction",
	&hkBinaryActionClass_310, // parent
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

// Generated from 'hkanimation/motion/hkAnimatedReferenceFrame.h'



//
// Class hkAnimatedReferenceFrame
//

extern hkClass hkAnimatedReferenceFrameClass_310;
hkClass hkAnimatedReferenceFrameClass_310(
	"hkAnimatedReferenceFrame",
	&hkReferencedObjectClass_310, // parent
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

// Generated from 'hkanimation/animation/hkAnimationBinding.h'



// External pointer and enum types
extern hkClass hkInt16Class_310;
extern hkClass hkSkeletalAnimationClass_310;

//
// Class hkAnimationBinding
//
static hkInternalClassMember hkAnimationBindingClass_Members[] =
{
	{ "animation", &hkSkeletalAnimationClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "animationTrackToBoneIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 }
};
extern hkClass hkAnimationBindingClass_310;
hkClass hkAnimationBindingClass_310(
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
extern hkClass hkBoneAttachmentClass_310;
extern hkClass hkMeshBindingClass_310;
extern hkClass hkSkeletonClass_310;

//
// Class hkAnimationContainer
//
static hkInternalClassMember hkAnimationContainerClass_Members[] =
{
	{ "skeletons", &hkSkeletonClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "animations", &hkSkeletalAnimationClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindings", &hkAnimationBindingClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "attachments", &hkBoneAttachmentClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skins", &hkMeshBindingClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkAnimationContainerClass_310;
hkClass hkAnimationContainerClass_310(
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

// Generated from 'hkanimation/animation/hkAnnotationTrack.h'



// External pointer and enum types
extern hkClass AnnotationClass_310;

//
// Class hkAnnotationTrack
//

//
// Class hkAnnotationTrack::Annotation
//
static hkInternalClassMember hkAnnotationTrack_AnnotationClass_Members[] =
{
	{ "time", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "text", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 }
};
extern hkClass hkAnnotationTrackAnnotationClass_310;
hkClass hkAnnotationTrackAnnotationClass_310(
	"hkAnnotationTrackAnnotation",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnnotationTrack_AnnotationClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkAnnotationTrackClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "annotations", &hkAnnotationTrackAnnotationClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkAnnotationTrackClass_310;
hkClass hkAnnotationTrackClass_310(
	"hkAnnotationTrack",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAnnotationTrackClass_Members),
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
extern hkClass hkEntityClass_310;

//
// Class hkArrayAction
//
static hkInternalClassMember hkArrayActionClass_Members[] =
{
	{ "entities", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkArrayActionClass_310;
hkClass hkArrayActionClass_310(
	"hkArrayAction",
	&hkActionClass_310, // parent
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
extern hkClass hkSolverResultsClass_310;

//
// Class hkBallAndSocketConstraintData
//

//
// Class hkBallAndSocketConstraintData::Runtime
//
static hkInternalClassMember hkBallAndSocketConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 3, 0, 0 }
};
extern hkClass hkBallAndSocketConstraintDataRuntimeClass_310;
hkClass hkBallAndSocketConstraintDataRuntimeClass_310(
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
extern hkClass hkConstraintDataClass_310;

extern hkClass hkBallAndSocketConstraintDataClass_310;
hkClass hkBallAndSocketConstraintDataClass_310(
	"hkBallAndSocketConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass hkBaseObjectClass_310;
hkClass hkBaseObjectClass_310(
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
	{ "entityA", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entityB", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkBinaryActionClass_310(
	"hkBinaryAction",
	&hkActionClass_310, // parent
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
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/rig/hkBoneAttachment.h'



//
// Class hkBoneAttachment
//
static hkInternalClassMember hkBoneAttachmentClass_Members[] =
{
	{ "boneFromAttachment", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "attachment", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkBoneAttachmentClass_310(
	"hkBoneAttachment",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoneAttachmentClass_Members),
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

// Generated from 'hkanimation/rig/hkBone.h'



//
// Class hkBone
//
static hkInternalClassMember hkBoneClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "lockTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkBoneClass_310;
hkClass hkBoneClass_310(
	"hkBone",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkBoneClass_Members),
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

// Generated from 'hkdynamics/motion/rigid/hkBoxMotion.h'



//
// Class hkBoxMotion
//
static hkInternalClassMember hkBoxMotionClass_Members[] =
{
	{ "inertiaAndMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkRigidMotionClass_310;

extern hkClass hkBoxMotionClass_310;
hkClass hkBoxMotionClass_310(
	"hkBoxMotion",
	&hkRigidMotionClass_310, // parent
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
extern hkClass hkConvexShapeClass_310;

extern hkClass hkBoxShapeClass_310;
hkClass hkBoxShapeClass_310(
	"hkBoxShape",
	&hkConvexShapeClass_310, // parent
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
extern hkClass hkBreakableListenerClass_310;

//
// Class hkBreakableConstraintData
//
static hkInternalClassMember hkBreakableConstraintDataClass_Members[] =
{
	{ "constraintData", &hkConstraintDataClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "solverResultLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "isBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "removeWhenBroken", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "breakNonRemovedInstances", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "listener", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkBreakableConstraintDataClass_310;
hkClass hkBreakableConstraintDataClass_310(
	"hkBreakableConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass hkBroadPhaseHandleClass_310;
hkClass hkBroadPhaseHandleClass_310(
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
extern hkClass hkShapeClass_310;

//
// Class hkBvShape
//
static hkInternalClassMember hkBvShapeClass_Members[] =
{
	{ "boundingVolumeShape", &hkShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "childShape", &hkShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkBvShapeClass_310;
hkClass hkBvShapeClass_310(
	"hkBvShape",
	&hkShapeClass_310, // parent
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
extern hkClass hkShapeCollectionClass_310;

//
// Class hkBvTreeShape
//
static hkInternalClassMember hkBvTreeShapeClass_Members[] =
{
	{ "shapeCollection", &hkShapeCollectionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkBvTreeShapeClass_310;
hkClass hkBvTreeShapeClass_310(
	"hkBvTreeShape",
	&hkShapeClass_310, // parent
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
extern hkClass hkCollisionAgentClass_310;
extern hkClass hkCollisionDetailClass_310;

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
extern hkClass hkCachingShapePhantomhkCollisionDetailClass_310;
hkClass hkCachingShapePhantomhkCollisionDetailClass_310(
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
extern hkClass hkShapePhantomClass_310;

extern hkClass hkCachingShapePhantomClass_310;
hkClass hkCachingShapePhantomClass_310(
	"hkCachingShapePhantom",
	&hkShapePhantomClass_310, // parent
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

extern hkClass hkCapsuleShapeClass_310;
hkClass hkCapsuleShapeClass_310(
	"hkCapsuleShape",
	&hkConvexShapeClass_310, // parent
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
extern hkClass hkCdBodyClass_310;

//
// Class hkCdBody
//
static hkInternalClassMember hkCdBodyClass_Members[] =
{
	{ "shape", &hkShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "shapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motion", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "parent", &hkCdBodyClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkCdBodyClass_310(
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
	{ "keepContactTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraUpStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extraDownStaticFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "shapePhantom", &hkShapePhantomClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "keepDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "userPlanes", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxCharacterSpeedForSolver", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "characterMass", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxSlope", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "penetrationRecoverySpeed", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkCharacterProxyCinfo_DefaultStruct
	{
		int s_defaultOffsets[17];
		hkReal m_keepContactTolerance;
		hkReal m_maxCharacterSpeedForSolver;
		hkReal m_characterStrength;
		hkReal m_maxSlope;
		hkReal m_penetrationRecoverySpeed;
	};
	const hkCharacterProxyCinfo_DefaultStruct hkCharacterProxyCinfo_Default =
	{
		{ -1,-1,-1,-1, HK_OFFSET_OF(hkCharacterProxyCinfo_DefaultStruct,m_keepContactTolerance),
		-1,-1,-1,-1,-1,-1,-1, HK_OFFSET_OF(hkCharacterProxyCinfo_DefaultStruct,m_maxCharacterSpeedForSolver),
		HK_OFFSET_OF(hkCharacterProxyCinfo_DefaultStruct,m_characterStrength), -1,
		HK_OFFSET_OF(hkCharacterProxyCinfo_DefaultStruct,m_maxSlope),
		HK_OFFSET_OF(hkCharacterProxyCinfo_DefaultStruct,m_penetrationRecoverySpeed) },
		/*m_keepContactTolerance*/ 0.1f,
		/*m_maxCharacterSpeedForSolver*/ 10,
		/*m_characterStrength*/ 274877906944.0f,
		/*m_maxSlope*/ 1.5707963267948966f,
		/*m_penetrationRecoverySpeed*/ 1.0f
	};
}
extern hkClass hkCharacterProxyCinfoClass_310;
hkClass hkCharacterProxyCinfoClass_310(
	"hkCharacterProxyCinfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkCharacterProxyCinfoClass_Members),
	17, // members
	&hkCharacterProxyCinfo_Default // defaults
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
extern hkClass hkClassClass_310;
extern hkClass hkClassEnumClass_310;
extern hkClass hkClassMemberClass_310;

//
// Class hkClass
//
static hkInternalClassMember hkClassClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "parent", &hkClassClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "objectSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numImplementedInterfaces", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "declaredEnums", &hkClassEnumClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "declaredMembers", &hkClassMemberClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "defaults", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkClassClass_310(
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
extern hkClass ItemClass_310;

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
extern hkClass hkClassEnumItemClass_310;
hkClass hkClassEnumItemClass_310(
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
	{ "items", &hkClassEnumItemClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkClassEnumClass_310(
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
static const hkInternalClassEnumItem hkClassMemberTypeEnumItems_310[] =
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
static const hkInternalClassEnumItem hkClassMemberFlagsEnumItems_310[] =
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
static const hkInternalClassEnumItem hkClassMemberRangeEnumItems_310[] =
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
	{"Type", hkClassMemberTypeEnumItems_310, 30 },
	{"Flags", hkClassMemberFlagsEnumItems_310, 6 },
	{"Range", hkClassMemberRangeEnumItems_310, 7 }
};
extern const hkClassEnum* hkClassMemberTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[0]);
extern const hkClassEnum* hkClassMemberFlagsEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[1]);
extern const hkClassEnum* hkClassMemberRangeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkClassMemberEnums[2]);
static hkInternalClassMember hkClassMemberClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "class", &hkClassClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "enum", &hkClassEnumClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "type", HK_NULL, hkClassMemberTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "subtype", HK_NULL, hkClassMemberTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "cArraySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkClassMemberClass_310(
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
extern hkClass hkTypedBroadPhaseHandleClass_310;

//
// Class hkCollidable
//
static hkInternalClassMember hkCollidableClass_Members[] =
{
	{ "ownerOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseHandle", &hkTypedBroadPhaseHandleClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowedPenetrationDepth", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkCollidableClass_310(
	"hkCollidable",
	&hkCdBodyClass_310, // parent
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
extern hkClass hkCollidableCollidableFilterClass_310;
hkClass hkCollidableCollidableFilterClass_310(
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

extern hkClass hkCollisionFilterClass_310;
hkClass hkCollisionFilterClass_310(
	"hkCollisionFilter",
	&hkReferencedObjectClass_310, // parent
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
	{ "collisionFilters", &hkCollisionFilterClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkCollisionFilterListClass_310;
hkClass hkCollisionFilterListClass_310(
	"hkCollisionFilterList",
	&hkCollisionFilterClass_310, // parent
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



// External pointer and enum types

//
// Class hkConstrainedSystemFilter
//
static hkInternalClassMember hkConstrainedSystemFilterClass_Members[] =
{
	{ "otherFilter", &hkCollisionFilterClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkConstrainedSystemFilterClass_310;
hkClass hkConstrainedSystemFilterClass_310(
	"hkConstrainedSystemFilter",
	&hkCollisionFilterClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkConstrainedSystemFilterClass_Members),
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

// Generated from 'hkdynamics/constraint/hkConstraintData.h'


//
// Global
//

//
// Enum ::hkConstraintType
//
static const hkInternalClassEnumItem hkConstraintTypeEnumItems_310[] =
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
	{"hkConstraintType", hkConstraintTypeEnumItems_310, 15 }
};
extern const hkClassEnum* hkConstraintTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&Enums[0]);

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
extern hkClass hkConstraintDataRuntimeInfoClass_310;
hkClass hkConstraintDataRuntimeInfoClass_310(
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

hkClass hkConstraintDataClass_310(
	"hkConstraintData",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkConstraintInfoClass_310;
hkClass hkConstraintInfoClass_310(
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
extern hkClass hkConstraintInternalClass_310;
extern hkClass hkConstraintOwnerClass_310;

//
// Class hkConstraintInstance
//

//
// Enum hkConstraintInstance::ConstraintPriority
//
static const hkInternalClassEnumItem hkConstraintInstanceConstraintPriorityEnumItems_310[] =
{
	{0, "PRIORITY_INVALID"},
	{1, "PRIORITY_PSI"},
	{2, "PRIORITY_TOI"},
	{3, "PRIORITY_TOI_HIGHER"},
	{4, "PRIORITY_TOI_FORCED"},
};
static const hkInternalClassEnum hkConstraintInstanceEnums[] = {
	{"ConstraintPriority", hkConstraintInstanceConstraintPriorityEnumItems_310, 5 }
};
extern const hkClassEnum* hkConstraintInstanceConstraintPriorityEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkConstraintInstanceEnums[0]);
static hkInternalClassMember hkConstraintInstanceClass_Members[] =
{
	{ "owner", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "data", &hkConstraintDataClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "entities", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 2, 0, 0 },
	{ "priority", HK_NULL, hkConstraintInstanceConstraintPriorityEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "wantRuntime", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "internal", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkConstraintInstanceClass_310;
hkClass hkConstraintInstanceClass_310(
	"hkConstraintInstance",
	&hkReferencedObjectClass_310, // parent
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
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/constraint/motor/hkConstraintMotor.h'



//
// Class hkConstraintMotor
//

extern hkClass hkConstraintMotorClass_310;
hkClass hkConstraintMotorClass_310(
	"hkConstraintMotor",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkContactPointClass_310;
hkClass hkContactPointClass_310(
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
// PRODUCT    : HAVOK_2 MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/basetypes/hkContactPointMaterial.h'



//
// Class hkContactPointMaterial
//

//
// Enum hkContactPointMaterial::FlagEnum
//
static const hkInternalClassEnumItem hkContactPointMaterialFlagEnumEnumItems_310[] =
{
	{1, "CONTACT_IS_NEW_AND_POTENTIAL"},
	{2, "CONTACT_USES_SOLVER_PATH2"},
};
static const hkInternalClassEnum hkContactPointMaterialEnums[] = {
	{"FlagEnum", hkContactPointMaterialFlagEnumEnumItems_310, 2 }
};
extern const hkClassEnum* hkContactPointMaterialFlagEnumEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkContactPointMaterialEnums[0]);
static hkInternalClassMember hkContactPointMaterialClass_Members[] =
{
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "flags", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkContactPointMaterialClass_310;
hkClass hkContactPointMaterialClass_310(
	"hkContactPointMaterial",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkContactPointMaterialEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkContactPointMaterialClass_Members),
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

// Generated from 'hkcollide/shape/convexlist/hkConvexListShape.h'



//
// Class hkConvexListShape
//
static hkInternalClassMember hkConvexListShapeClass_Members[] =
{
	{ "minDistanceToUseConvexHullForGetClosestPoints", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkListShapeClass_310;

extern hkClass hkConvexListShapeClass_310;
hkClass hkConvexListShapeClass_310(
	"hkConvexListShape",
	&hkListShapeClass_310, // parent
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
extern hkClass hkConvexPieceStreamDataClass_310;

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
extern hkClass hkConvexPieceMeshShapeStatsClass_310;
hkClass hkConvexPieceMeshShapeStatsClass_310(
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
	{ "convexPieceStream", &hkConvexPieceStreamDataClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "displayMesh", &hkShapeCollectionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexPieceMeshShapeClass_310;
hkClass hkConvexPieceMeshShapeClass_310(
	"hkConvexPieceMeshShape",
	&hkShapeCollectionClass_310, // parent
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
extern hkClass hkShapeKeyClass_310;
extern hkClass hkUint32Class_310;

//
// Class hkConvexPieceStreamData
//
static hkInternalClassMember hkConvexPieceStreamDataClass_Members[] =
{
	{ "convexPieceStream", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceOffsets", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "convexPieceSingleTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};

hkClass hkConvexPieceStreamDataClass_310(
	"hkConvexPieceStreamData",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkSphereRepShapeClass_310;

hkClass hkConvexShapeClass_310(
	"hkConvexShape",
	&hkSphereRepShapeClass_310, // parent
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
	{ "childShape", &hkConvexShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexTransformShapeClass_310;
hkClass hkConvexTransformShapeClass_310(
	"hkConvexTransformShape",
	&hkConvexShapeClass_310, // parent
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
	{ "childShape", &hkConvexShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "translation", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkConvexTranslateShapeClass_310;
hkClass hkConvexTranslateShapeClass_310(
	"hkConvexTranslateShape",
	&hkConvexShapeClass_310, // parent
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
extern hkClass FourVectorsClass_310;
extern hkClass hkVector4Class_310;

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
extern hkClass hkConvexVerticesShapeFourVectorsClass_310;
hkClass hkConvexVerticesShapeFourVectorsClass_310(
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
	{ "rotatedVertices", &hkConvexVerticesShapeFourVectorsClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeEquations", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};

extern hkClass hkConvexVerticesShapeClass_310;
hkClass hkConvexVerticesShapeClass_310(
	"hkConvexVerticesShape",
	&hkConvexShapeClass_310, // parent
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

//
// Enum hkCylinderShape::VertexIdEncoding
//
static const hkInternalClassEnumItem hkCylinderShapeVertexIdEncodingEnumItems_310[] =
{
	{7, "VERTEX_ID_ENCODING_IS_BASE_A_SHIFT"},
	{6, "VERTEX_ID_ENCODING_SIN_SIGN_SHIFT"},
	{5, "VERTEX_ID_ENCODING_COS_SIGN_SHIFT"},
	{4, "VERTEX_ID_ENCODING_IS_SIN_LESSER_SHIFT"},
	{0x0f, "VERTEX_ID_ENCODING_VALUE_MASK"},
};
static const hkInternalClassEnum hkCylinderShapeEnums[] = {
	{"VertexIdEncoding", hkCylinderShapeVertexIdEncodingEnumItems_310, 5 }
};
extern const hkClassEnum* hkCylinderShapeVertexIdEncodingEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkCylinderShapeEnums[0]);
static hkInternalClassMember hkCylinderShapeClass_Members[] =
{
	{ "cylRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexA", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "vertexB", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "perpendicular2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkCylinderShapeClass_310;
hkClass hkCylinderShapeClass_310(
	"hkCylinderShape",
	&hkConvexShapeClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkCylinderShapeEnums),
	1, // enums
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

extern hkClass hkDashpotActionClass_310;
hkClass hkDashpotActionClass_310(
	"hkDashpotAction",
	&hkBinaryActionClass_310, // parent
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

// Generated from 'hkanimation/motion/default/hkDefaultAnimatedReferenceFrame.h'



// External pointer and enum types

//
// Class hkDefaultAnimatedReferenceFrame
//

//
// Class hkDefaultAnimatedReferenceFrame::MotionExtractionOptions
//
static hkInternalClassMember hkDefaultAnimatedReferenceFrame_MotionExtractionOptionsClass_Members[] =
{
	{ "numSamples", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowUpDown", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowFrontBack", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowRightLeft", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "allowTurning", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forward", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkDefaultAnimatedReferenceFrameMotionExtractionOptionsClass_310;
hkClass hkDefaultAnimatedReferenceFrameMotionExtractionOptionsClass_310(
	"hkDefaultAnimatedReferenceFrameMotionExtractionOptions",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDefaultAnimatedReferenceFrame_MotionExtractionOptionsClass_Members),
	7, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkDefaultAnimatedReferenceFrameClass_Members[] =
{
	{ "up", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forward", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "duration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "referenceFrameSamples", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 }
};

extern hkClass hkDefaultAnimatedReferenceFrameClass_310;
hkClass hkDefaultAnimatedReferenceFrameClass_310(
	"hkDefaultAnimatedReferenceFrame",
	&hkAnimatedReferenceFrameClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDefaultAnimatedReferenceFrameClass_Members),
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

// Generated from 'hkanimation/animation/deltacompressed/hkDeltaCompressedSkeletalAnimation.h'



// External pointer and enum types
extern hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_310;
extern hkClass hkRealClass_310;
extern hkClass hkUint16Class_310;
extern hkClass hkUint8Class_310;

//
// Class hkDeltaCompressedSkeletalAnimation
//

//
// Class hkDeltaCompressedSkeletalAnimation::CompressionParams
//
static hkInternalClassMember hkDeltaCompressedSkeletalAnimation_CompressionParamsClass_Members[] =
{
	{ "quantizationBits", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "absolutePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "relativePositionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotationTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "scaleTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkDeltaCompressedSkeletalAnimationCompressionParamsClass_310;
hkClass hkDeltaCompressedSkeletalAnimationCompressionParamsClass_310(
	"hkDeltaCompressedSkeletalAnimationCompressionParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedSkeletalAnimation_CompressionParamsClass_Members),
	6, // members
	HK_NULL // defaults
	);

//
// Class hkDeltaCompressedSkeletalAnimation::QuantizationFormat
//
static hkInternalClassMember hkDeltaCompressedSkeletalAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_310(
	"hkDeltaCompressedSkeletalAnimationQuantizationFormat",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedSkeletalAnimation_QuantizationFormatClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkDeltaCompressedSkeletalAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};

extern hkClass hkDeltaCompressedSkeletalAnimationClass_310;
hkClass hkDeltaCompressedSkeletalAnimationClass_310(
	"hkDeltaCompressedSkeletalAnimation",
	&hkSkeletalAnimationClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkDeltaCompressedSkeletalAnimationClass_Members),
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
	{ "disabledEntities", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkDisableEntityCollisionFilterClass_310;
hkClass hkDisableEntityCollisionFilterClass_310(
	"hkDisableEntityCollisionFilter",
	&hkCollisionFilterClass_310, // parent
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
extern hkClass hkCollisionListenerClass_310;
extern hkClass hkEntityActivationListenerClass_310;
extern hkClass hkEntityDeactivatorClass_310;
extern hkClass hkEntityListenerClass_310;
extern hkClass hkMaterialClass_310;
extern hkClass hkMotionClass_310;

//
// Class hkEntity
//
static hkInternalClassMember hkEntityClass_Members[] =
{
	{ "motion", &hkMotionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simulationIsland", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkMaterialClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivator", &hkEntityDeactivatorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
extern hkClass hkWorldObjectClass_310;

hkClass hkEntityClass_310(
	"hkEntity",
	&hkWorldObjectClass_310, // parent
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

hkClass hkEntityDeactivatorClass_310(
	"hkEntityDeactivator",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkRigidBodyDeactivatorClass_310;

extern hkClass hkFakeRigidBodyDeactivatorClass_310;
hkClass hkFakeRigidBodyDeactivatorClass_310(
	"hkFakeRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_310, // parent
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
extern hkClass hkMeshShapeClass_310;

extern hkClass hkFastMeshShapeClass_310;
hkClass hkFastMeshShapeClass_310(
	"hkFastMeshShape",
	&hkMeshShapeClass_310, // parent
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
extern hkClass hkKeyframedRigidMotionClass_310;

extern hkClass hkFixedRigidMotionClass_310;
hkClass hkFixedRigidMotionClass_310(
	"hkFixedRigidMotion",
	&hkKeyframedRigidMotionClass_310, // parent
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
extern hkClass hkGenericConstraintDataSchemeClass_310;

//
// Class hkGenericConstraintData
//
static hkInternalClassMember hkGenericConstraintDataClass_Members[] =
{
	{ "scheme", &hkGenericConstraintDataSchemeClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkGenericConstraintDataClass_310;
hkClass hkGenericConstraintDataClass_310(
	"hkGenericConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass hkConstraintModifierClass_310;
extern hkClass intClass_310;

//
// Class hkGenericConstraintDataScheme
//
static hkInternalClassMember hkGenericConstraintDataSchemeClass_Members[] =
{
	{ "info", &hkConstraintInfoClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_VECTOR4, 0, 0, 0 },
	{ "commands", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT32, 0, 0, 0 },
	{ "modifiers", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "motors", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkGenericConstraintDataSchemeClass_310(
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

extern hkClass hkGroupCollisionFilterClass_310;
hkClass hkGroupCollisionFilterClass_310(
	"hkGroupCollisionFilter",
	&hkCollisionFilterClass_310, // parent
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

extern hkClass hkGroupFilterClass_310;
hkClass hkGroupFilterClass_310(
	"hkGroupFilter",
	&hkCollisionFilterClass_310, // parent
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
extern hkClass hkSphereClass_310;

//
// Class hkHeightFieldShape
//

//
// Class hkHeightFieldShape::CollideSpheresInput
//
static hkInternalClassMember hkHeightFieldShape_CollideSpheresInputClass_Members[] =
{
	{ "spheres", &hkSphereClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkHeightFieldShapeCollideSpheresInputClass_310;
hkClass hkHeightFieldShapeCollideSpheresInputClass_310(
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
extern hkClass hkShapeRayCastInputClass_310;

extern hkClass hkHeightFieldShapehkSphereCastInputClass_310;
hkClass hkHeightFieldShapehkSphereCastInputClass_310(
	"hkHeightFieldShapehkSphereCastInput",
	&hkShapeRayCastInputClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHeightFieldShape_hkSphereCastInputClass_Members),
	2, // members
	HK_NULL // defaults
	);

extern hkClass hkHeightFieldShapeClass_310;
hkClass hkHeightFieldShapeClass_310(
	"hkHeightFieldShape",
	&hkShapeClass_310, // parent
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
extern hkClass hkHingeConstraintDataConstraintBasisAClass_310;
extern hkClass hkHingeConstraintDataConstraintBasisBClass_310;

//
// Class hkHingeConstraintData
//

//
// Class hkHingeConstraintData::Runtime
//
static hkInternalClassMember hkHingeConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 5, 0, 0 }
};
extern hkClass hkHingeConstraintDataRuntimeClass_310;
hkClass hkHingeConstraintDataRuntimeClass_310(
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
hkClass hkHingeConstraintDataConstraintBasisAClass_310(
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
hkClass hkHingeConstraintDataConstraintBasisBClass_310(
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
	{ "basisA", &hkHingeConstraintDataConstraintBasisAClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkHingeConstraintDataConstraintBasisBClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkHingeConstraintDataClass_310;
hkClass hkHingeConstraintDataClass_310(
	"hkHingeConstraintData",
	&hkConstraintDataClass_310, // parent
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

// Generated from 'hkanimation/animation/interleaved/hkInterleavedSkeletalAnimation.h'



// External pointer and enum types
extern hkClass hkQsTransformClass_310;

//
// Class hkInterleavedSkeletalAnimation
//
static hkInternalClassMember hkInterleavedSkeletalAnimationClass_Members[] =
{
	{ "transforms", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};

extern hkClass hkInterleavedSkeletalAnimationClass_310;
hkClass hkInterleavedSkeletalAnimationClass_310(
	"hkInterleavedSkeletalAnimation",
	&hkSkeletalAnimationClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkInterleavedSkeletalAnimationClass_Members),
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
	{ "savedMotion", &hkRigidMotionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "savedQualityTypeIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkKeyframedRigidMotionClass_310(
	"hkKeyframedRigidMotion",
	&hkRigidMotionClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkKeyframedRigidMotionClass_Members),
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

// Generated from 'hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h'



// External pointer and enum types
extern hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_310;
extern hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_310;

//
// Class hkLimitedHingeConstraintData
//

//
// Class hkLimitedHingeConstraintData::Runtime
//
static hkInternalClassMember hkLimitedHingeConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "lastAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkLimitedHingeConstraintDataRuntimeClass_310;
hkClass hkLimitedHingeConstraintDataRuntimeClass_310(
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
hkClass hkLimitedHingeConstraintDataConstraintBasisAClass_310(
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
hkClass hkLimitedHingeConstraintDataConstraintBasisBClass_310(
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
	{ "basisA", &hkLimitedHingeConstraintDataConstraintBasisAClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkLimitedHingeConstraintDataConstraintBasisBClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkLimitedHingeConstraintDataClass_310;
hkClass hkLimitedHingeConstraintDataClass_310(
	"hkLimitedHingeConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass hkParametricCurveClass_310;

extern hkClass hkLinearParametricCurveClass_310;
hkClass hkLinearParametricCurveClass_310(
	"hkLinearParametricCurve",
	&hkParametricCurveClass_310, // parent
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
extern hkClass CollisionEntryClass_310;
extern hkClass hkAgentNnEntryClass_310;
extern hkClass hkLinkedCollidableClass_310;

//
// Class hkLinkedCollidable
//

//
// Class hkLinkedCollidable::CollisionEntry
//
static hkInternalClassMember hkLinkedCollidable_CollisionEntryClass_Members[] =
{
	{ "agentEntry", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "partner", &hkLinkedCollidableClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkLinkedCollidableCollisionEntryClass_310;
hkClass hkLinkedCollidableCollisionEntryClass_310(
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

hkClass hkLinkedCollidableClass_310(
	"hkLinkedCollidable",
	&hkCollidableClass_310, // parent
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
extern hkClass ChildInfoClass_310;

//
// Class hkListShape
//

//
// Class hkListShape::ChildInfo
//
static hkInternalClassMember hkListShape_ChildInfoClass_Members[] =
{
	{ "shape", &hkShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "collisionFilterInfo", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkListShapeChildInfoClass_310;
hkClass hkListShapeChildInfoClass_310(
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
	{ "childInfo", &hkListShapeChildInfoClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkListShapeClass_310(
	"hkListShape",
	&hkShapeCollectionClass_310, // parent
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
	{ "constraintData", &hkConstraintDataClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkMalleableConstraintDataClass_310;
hkClass hkMalleableConstraintDataClass_310(
	"hkMalleableConstraintData",
	&hkConstraintDataClass_310, // parent
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
static const hkInternalClassEnumItem hkMaterialResponseTypeEnumItems_310[] =
{
	{0, "RESPONSE_INVALID"},
	{1, "RESPONSE_SIMPLE_CONTACT"},
	{2, "RESPONSE_REPORTING"},
	{3, "RESPONSE_NONE"},
	{4, "RESPONSE_MAX_ID"},
};
static const hkInternalClassEnum hkMaterialEnums[] = {
	{"ResponseType", hkMaterialResponseTypeEnumItems_310, 5 }
};
extern const hkClassEnum* hkMaterialResponseTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkMaterialEnums[0]);
static hkInternalClassMember hkMaterialClass_Members[] =
{
	{ "responseType", HK_NULL, hkMaterialResponseTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "friction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "restitution", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkMaterialClass_310(
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
extern hkClass MappingClass_310;
extern hkClass hkTransformClass_310;
extern hkClass hkxMeshClass_310;

//
// Class hkMeshBinding
//

//
// Class hkMeshBinding::Mapping
//
static hkInternalClassMember hkMeshBinding_MappingClass_Members[] =
{
	{ "mapping", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 }
};
extern hkClass hkMeshBindingMappingClass_310;
hkClass hkMeshBindingMappingClass_310(
	"hkMeshBindingMapping",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshBinding_MappingClass_Members),
	1, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkMeshBindingClass_Members[] =
{
	{ "mesh", &hkxMeshClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mappings", &hkMeshBindingMappingClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "inverseWorldBindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_TRANSFORM, 0, 0, 0 }
};
hkClass hkMeshBindingClass_310(
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
extern hkClass hkMeshMaterialClass_310;
hkClass hkMeshMaterialClass_310(
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
extern hkClass SubpartClass_310;

//
// Class hkMeshShape
//

//
// Enum hkMeshShape::IndexStridingType
//
static const hkInternalClassEnumItem hkMeshShapeIndexStridingTypeEnumItems_310[] =
{
	{0, "INDICES_INVALID"},
	{1, "INDICES_INT16"},
	{2, "INDICES_INT32"},
	{3, "INDICES_MAX_ID"},
};

//
// Enum hkMeshShape::MaterialIndexStridingType
//
static const hkInternalClassEnumItem hkMeshShapeMaterialIndexStridingTypeEnumItems_310[] =
{
	{0, "MATERIAL_INDICES_INVALID"},
	{1, "MATERIAL_INDICES_INT8"},
	{2, "MATERIAL_INDICES_INT16"},
	{3, "MATERIAL_INDICES_MAX_ID"},
};
static const hkInternalClassEnum hkMeshShapeEnums[] = {
	{"IndexStridingType", hkMeshShapeIndexStridingTypeEnumItems_310, 4 },
	{"MaterialIndexStridingType", hkMeshShapeMaterialIndexStridingTypeEnumItems_310, 4 }
};
extern const hkClassEnum* hkMeshShapeIndexStridingTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[0]);
extern const hkClassEnum* hkMeshShapeMaterialIndexStridingTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkMeshShapeEnums[1]);

//
// Class hkMeshShape::Subpart
//
static hkInternalClassMember hkMeshShape_SubpartClass_Members[] =
{
	{ "vertexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "vertexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numVertices", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "indexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "stridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "materialIndexStridingType", HK_NULL, hkMeshShapeIndexStridingTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numTriangles", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialIndexBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materialIndexStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "materialBase", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materialStriding", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numMaterials", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMeshShapeSubpartClass_310;
hkClass hkMeshShapeSubpartClass_310(
	"hkMeshShapeSubpart",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkMeshShape_SubpartClass_Members),
	13, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkMeshShapeClass_Members[] =
{
	{ "scaling", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numBitsForSubpartIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subparts", &hkMeshShapeSubpartClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkMeshShapeClass_310(
	"hkMeshShape",
	&hkShapeCollectionClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkMeshShapeEnums),
	2, // enums
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
extern hkClass hkMoppCodeClass_310;

//
// Class hkMoppBvTreeShape
//
static hkInternalClassMember hkMoppBvTreeShapeClass_Members[] =
{
	{ "code", &hkMoppCodeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkMoppBvTreeShapeClass_310;
hkClass hkMoppBvTreeShapeClass_310(
	"hkMoppBvTreeShape",
	&hkBvTreeShapeClass_310, // parent
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
extern hkClass hkMoppCodeCodeInfoClass_310;

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
hkClass hkMoppCodeCodeInfoClass_310(
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
	{ "info", &hkMoppCodeCodeInfoClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 }
};

hkClass hkMoppCodeClass_310(
	"hkMoppCode",
	&hkReferencedObjectClass_310, // parent
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
static const hkInternalClassEnumItem hkMotionMotionTypeEnumItems_310[] =
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
	{"MotionType", hkMotionMotionTypeEnumItems_310, 9 }
};
extern const hkClassEnum* hkMotionMotionTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkMotionEnums[0]);
static hkInternalClassMember hkMotionClass_Members[] =
{
	{ "solverData", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkMotionClass_310(
	"hkMotion",
	&hkReferencedObjectClass_310, // parent
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
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkmath/basetypes/hkMotionState.h'



// External pointer and enum types
extern hkClass hkSweptTransformClass_310;

//
// Class hkMotionState
//
static hkInternalClassMember hkMotionStateClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sweptTransform", &hkSweptTransformClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deltaAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "objectRadius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxAngularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationClass", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deactivationCounter", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT16, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkMotionStateClass_310;
hkClass hkMotionStateClass_310(
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
extern hkClass hkUnaryActionClass_310;

extern hkClass hkMotorActionClass_310;
hkClass hkMotorActionClass_310(
	"hkMotorAction",
	&hkUnaryActionClass_310, // parent
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

extern hkClass hkMouseSpringActionClass_310;
hkClass hkMouseSpringActionClass_310(
	"hkMouseSpringAction",
	&hkUnaryActionClass_310, // parent
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
extern hkClass RayClass_310;

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
extern hkClass hkMultiRayShapeRayClass_310;
hkClass hkMultiRayShapeRayClass_310(
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
	{ "rays", &hkMultiRayShapeRayClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rayPenetrationDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkMultiRayShapeClass_310;
hkClass hkMultiRayShapeClass_310(
	"hkMultiRayShape",
	&hkShapeClass_310, // parent
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
static const hkInternalClassEnumItem hkMultiSphereShapetestEnumItems_310[] =
{
	{8, "MAX_SPHERES"},
};
static const hkInternalClassEnum hkMultiSphereShapeEnums[] = {
	{"test", hkMultiSphereShapetestEnumItems_310, 1 }
};
extern const hkClassEnum* hkMultiSphereShapetestEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkMultiSphereShapeEnums[0]);
static hkInternalClassMember hkMultiSphereShapeClass_Members[] =
{
	{ "numSpheres", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "spheres", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 8, 0, 0 }
};

extern hkClass hkMultiSphereShapeClass_310;
hkClass hkMultiSphereShapeClass_310(
	"hkMultiSphereShape",
	&hkSphereRepShapeClass_310, // parent
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

extern hkClass hkNullCollisionFilterClass_310;
hkClass hkNullCollisionFilterClass_310(
	"hkNullCollisionFilter",
	&hkCollisionFilterClass_310, // parent
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
extern hkClass hkPackfileHeaderClass_310;
hkClass hkPackfileHeaderClass_310(
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
extern hkClass hkPackfileSectionHeaderClass_310;
hkClass hkPackfileSectionHeaderClass_310(
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
extern hkClass CollisionPairClass_310;

//
// Class hkPairwiseCollisionFilter
//

//
// Class hkPairwiseCollisionFilter::CollisionPair
//
static hkInternalClassMember hkPairwiseCollisionFilter_CollisionPairClass_Members[] =
{
	{ "a", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "b", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkPairwiseCollisionFilterCollisionPairClass_310;
hkClass hkPairwiseCollisionFilterCollisionPairClass_310(
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
	{ "disabledPairs", &hkPairwiseCollisionFilterCollisionPairClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkPairwiseCollisionFilterClass_310;
hkClass hkPairwiseCollisionFilterClass_310(
	"hkPairwiseCollisionFilter",
	&hkCollisionFilterClass_310, // parent
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

hkClass hkParametricCurveClass_310(
	"hkParametricCurve",
	&hkReferencedObjectClass_310, // parent
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

extern hkClass hkPhantomCallbackShapeClass_310;
hkClass hkPhantomCallbackShapeClass_310(
	"hkPhantomCallbackShape",
	&hkShapeClass_310, // parent
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
extern hkClass hkPhantomListenerClass_310;
extern hkClass hkPhantomOverlapListenerClass_310;

//
// Class hkPhantom
//
static hkInternalClassMember hkPhantomClass_Members[] =
{
	{ "overlapListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 },
	{ "phantomListeners", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_ARRAY, 0, 0, 0 }
};

hkClass hkPhantomClass_310(
	"hkPhantom",
	&hkWorldObjectClass_310, // parent
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
extern hkClass hkPhysicsSystemClass_310;
extern hkClass hkWorldCinfoClass_310;

//
// Class hkPhysicsData
//

//
// Class hkPhysicsData::SplitPhysicsSystemsOutput
//
static hkInternalClassMember hkPhysicsData_SplitPhysicsSystemsOutputClass_Members[] =
{
	{ "unconstrainedFixedBodies", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "unconstrainedKeyframedBodies", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "unconstrainedMovingBodies", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "phantoms", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "constrainedSystems", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkPhysicsDataSplitPhysicsSystemsOutputClass_310;
hkClass hkPhysicsDataSplitPhysicsSystemsOutputClass_310(
	"hkPhysicsDataSplitPhysicsSystemsOutput",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPhysicsData_SplitPhysicsSystemsOutputClass_Members),
	5, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPhysicsDataClass_Members[] =
{
	{ "worldCinfo", &hkWorldCinfoClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "systems", &hkPhysicsSystemClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkPhysicsDataClass_310;
hkClass hkPhysicsDataClass_310(
	"hkPhysicsData",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkRigidBodyClass_310;

//
// Class hkPhysicsSystem
//
static hkInternalClassMember hkPhysicsSystemClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "actions", &hkActionClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "phantoms", &hkPhantomClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkPhysicsSystemClass_310(
	"hkPhysicsSystem",
	&hkReferencedObjectClass_310, // parent
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

extern hkClass hkPlaneShapeClass_310;
hkClass hkPlaneShapeClass_310(
	"hkPlaneShape",
	&hkHeightFieldShapeClass_310, // parent
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
static const hkInternalClassEnumItem hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_310[] =
{
	{0, "CONSTRAIN_ORIENTATION_INVALID"},
	{1, "CONSTRAIN_ORIENTATION_NONE"},
	{2, "CONSTRAIN_ORIENTATION_ALLOW_SPIN"},
	{3, "CONSTRAIN_ORIENTATION_TO_PATH"},
	{4, "CONSTRAIN_ORIENTATION_MAX_ID"},
};
static const hkInternalClassEnum hkPointToPathConstraintDataEnums[] = {
	{"OrientationConstraintType", hkPointToPathConstraintDataOrientationConstraintTypeEnumItems_310, 5 }
};
extern const hkClassEnum* hkPointToPathConstraintDataOrientationConstraintTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkPointToPathConstraintDataEnums[0]);

//
// Class hkPointToPathConstraintData::Runtime
//
static hkInternalClassMember hkPointToPathConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "parametricPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPointToPathConstraintDataRuntimeClass_310;
hkClass hkPointToPathConstraintDataRuntimeClass_310(
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
	{ "path", &hkParametricCurveClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularConstrainedDOF", HK_NULL, hkPointToPathConstraintDataOrientationConstraintTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "transform_OS_KS", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 2, 0, 0 }
};

extern hkClass hkPointToPathConstraintDataClass_310;
hkClass hkPointToPathConstraintDataClass_310(
	"hkPointToPathConstraintData",
	&hkConstraintDataClass_310, // parent
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
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 1, 0, 0 }
};
extern hkClass hkPointToPlaneConstraintDataRuntimeClass_310;
hkClass hkPointToPlaneConstraintDataRuntimeClass_310(
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

extern hkClass hkPointToPlaneConstraintDataClass_310;
hkClass hkPointToPlaneConstraintDataClass_310(
	"hkPointToPlaneConstraintData",
	&hkConstraintDataClass_310, // parent
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

// Generated from 'hkdynamics/constraint/motor/position/hkPositionConstraintMotor.h'



//
// Class hkPositionConstraintMotor
//
static hkInternalClassMember hkPositionConstraintMotorClass_Members[] =
{
	{ "maxForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "damping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "proportionalRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "constantRecoveryVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkPositionConstraintMotorClass_310;
hkClass hkPositionConstraintMotorClass_310(
	"hkPositionConstraintMotor",
	&hkConstraintMotorClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPositionConstraintMotorClass_Members),
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
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "lastAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rotations", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motorRuntimeData", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPoweredHingeConstraintDataRuntimeClass_310;
hkClass hkPoweredHingeConstraintDataRuntimeClass_310(
	"hkPoweredHingeConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredHingeConstraintData_RuntimeClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPoweredHingeConstraintDataClass_Members[] =
{
	{ "motorActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ignoreLimits", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "targetAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motor", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkPoweredHingeConstraintDataClass_310;
hkClass hkPoweredHingeConstraintDataClass_310(
	"hkPoweredHingeConstraintData",
	&hkLimitedHingeConstraintDataClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredHingeConstraintDataClass_Members),
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

// Generated from 'hkinternal/dynamics/constraint/bilateral/poweredragdoll/hkPoweredRagdollConstraintData.h'



// External pointer and enum types

//
// Class hkPoweredRagdollConstraintData
//

//
// Class hkPoweredRagdollConstraintData::Runtime
//
static hkInternalClassMember hkPoweredRagdollConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 9, 0, 0 },
	{ "motorRuntimeData", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 3, 0, 0 }
};
extern hkClass hkPoweredRagdollConstraintDataRuntimeClass_310;
hkClass hkPoweredRagdollConstraintDataRuntimeClass_310(
	"hkPoweredRagdollConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPoweredRagdollConstraintData_RuntimeClass_Members),
	2, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkPoweredRagdollConstraintDataClass_Members[] =
{
	{ "motorsActive", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "targetFrameAinB", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX3, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMotor", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "coneMotor", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "planeMotor", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkRagdollConstraintDataClass_310;

extern hkClass hkPoweredRagdollConstraintDataClass_310;
hkClass hkPoweredRagdollConstraintDataClass_310(
	"hkPoweredRagdollConstraintData",
	&hkRagdollConstraintDataClass_310, // parent
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
extern hkClass hkPrismaticConstraintDataConstraintBasisAClass_310;
extern hkClass hkPrismaticConstraintDataConstraintBasisBClass_310;

//
// Class hkPrismaticConstraintData
//

//
// Class hkPrismaticConstraintData::Runtime
//
static hkInternalClassMember hkPrismaticConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 7, 0, 0 },
	{ "motorRuntimeData", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPrismaticConstraintDataRuntimeClass_310;
hkClass hkPrismaticConstraintDataRuntimeClass_310(
	"hkPrismaticConstraintDataRuntime",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_RuntimeClass_Members),
	2, // members
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
hkClass hkPrismaticConstraintDataConstraintBasisAClass_310(
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
hkClass hkPrismaticConstraintDataConstraintBasisBClass_310(
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
	{ "motor", &hkConstraintMotorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "basisA", &hkPrismaticConstraintDataConstraintBasisAClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkPrismaticConstraintDataConstraintBasisBClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "motorTargetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkPrismaticConstraintDataClass_310;
hkClass hkPrismaticConstraintDataClass_310(
	"hkPrismaticConstraintData",
	&hkConstraintDataClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintDataClass_Members),
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

// Generated from 'hkdynamics/common/hkProperty.h'



// External pointer and enum types
extern hkClass hkPropertyValueClass_310;

//
// Class hkPropertyValue
//
static hkInternalClassMember hkPropertyValueClass_Members[] =
{
	{ "data", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT64, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkPropertyValueClass_310(
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
	{ "alignmentPadding", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "value", &hkPropertyValueClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkPropertyClass_310;
hkClass hkPropertyClass_310(
	"hkProperty",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPropertyClass_Members),
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

// Generated from 'hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h'



// External pointer and enum types
extern hkClass hkRagdollConstraintDataConstraintBasisAClass_310;
extern hkClass hkRagdollConstraintDataConstraintBasisBClass_310;

//
// Class hkRagdollConstraintData
//

//
// Class hkRagdollConstraintData::Runtime
//
static hkInternalClassMember hkRagdollConstraintData_RuntimeClass_Members[] =
{
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 9, 0, 0 }
};
extern hkClass hkRagdollConstraintDataRuntimeClass_310;
hkClass hkRagdollConstraintDataRuntimeClass_310(
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
hkClass hkRagdollConstraintDataConstraintBasisAClass_310(
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
hkClass hkRagdollConstraintDataConstraintBasisBClass_310(
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
	{ "anglesAreNotStoredInHavok30Format", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisA", &hkRagdollConstraintDataConstraintBasisAClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkRagdollConstraintDataConstraintBasisBClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkRagdollConstraintDataClass_310(
	"hkRagdollConstraintData",
	&hkConstraintDataClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintDataClass_Members),
	9, // members
	HK_NULL // defaults
	);
// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2+MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkragdoll/instance/hkRagdollInstance.h'



// External pointer and enum types

//
// Class hkRagdollInstance
//
static hkInternalClassMember hkRagdollInstanceClass_Members[] =
{
	{ "rigidBodies", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "constraints", &hkConstraintInstanceClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skeleton", &hkSkeletonClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkRagdollInstanceClass_310;
hkClass hkRagdollInstanceClass_310(
	"hkRagdollInstance",
	&hkReferencedObjectClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollInstanceClass_Members),
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

// Generated from 'hkcollide/filter/hkRayCollidableFilter.h'



//
// Class hkRayCollidableFilter
//
extern hkClass hkRayCollidableFilterClass_310;
hkClass hkRayCollidableFilterClass_310(
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
extern hkClass hkRayShapeCollectionFilterClass_310;
hkClass hkRayShapeCollectionFilterClass_310(
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

hkClass hkReferencedObjectClass_310(
	"hkReferencedObject",
	&hkBaseObjectClass_310, // parent
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

extern hkClass hkReorientActionClass_310;
hkClass hkReorientActionClass_310(
	"hkReorientAction",
	&hkUnaryActionClass_310, // parent
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

// Generated from 'hkdynamics/entity/hkRigidBody.h'



//
// Class hkRigidBody
//

hkClass hkRigidBodyClass_310(
	"hkRigidBody",
	&hkEntityClass_310, // parent
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
static const hkInternalClassEnumItem hkRigidBodyDeactivatorDeactivatorTypeEnumItems_310[] =
{
	{0, "DEACTIVATOR_INVALID"},
	{1, "DEACTIVATOR_NEVER"},
	{2, "DEACTIVATOR_SPATIAL"},
	{3, "DEACTIVATOR_MAX_ID"},
};
static const hkInternalClassEnum hkRigidBodyDeactivatorEnums[] = {
	{"DeactivatorType", hkRigidBodyDeactivatorDeactivatorTypeEnumItems_310, 4 }
};
extern const hkClassEnum* hkRigidBodyDeactivatorDeactivatorTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkRigidBodyDeactivatorEnums[0]);

hkClass hkRigidBodyDeactivatorClass_310(
	"hkRigidBodyDeactivator",
	&hkEntityDeactivatorClass_310, // parent
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
	{ "motionState", &hkMotionStateClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "massInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "particleMinInertiaDiagInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "linearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angularVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkRigidMotionClass_310(
	"hkRigidMotion",
	&hkMotionClass_310, // parent
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
extern hkClass NamedVariantClass_310;

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
extern hkClass hkRootLevelContainerNamedVariantClass_310;
hkClass hkRootLevelContainerNamedVariantClass_310(
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
	{ "namedVariants", &hkRootLevelContainerNamedVariantClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkRootLevelContainerClass_310;
hkClass hkRootLevelContainerClass_310(
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

// Generated from 'hkcollide/shape/sampledheightfield/hkSampledHeightFieldShape.h'



//
// Class hkSampledHeightFieldShape
//
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

extern hkClass hkSampledHeightFieldShapeClass_310;
hkClass hkSampledHeightFieldShapeClass_310(
	"hkSampledHeightFieldShape",
	&hkHeightFieldShapeClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSampledHeightFieldShapeClass_Members),
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

// Generated from 'hkutilities/serialize/display/hkSerializedDisplayMarker.h'



//
// Class hkSerializedDisplayMarker
//
static hkInternalClassMember hkSerializedDisplayMarkerClass_Members[] =
{
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayMarkerClass_310;
hkClass hkSerializedDisplayMarkerClass_310(
	"hkSerializedDisplayMarker",
	&hkReferencedObjectClass_310, // parent
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
	{ "markers", &hkSerializedDisplayMarkerClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayMarkerListClass_310;
hkClass hkSerializedDisplayMarkerListClass_310(
	"hkSerializedDisplayMarkerList",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass DisplayTransformPairClass_310;

//
// Class hkSerializedDisplayRbTransforms
//

//
// Class hkSerializedDisplayRbTransforms::DisplayTransformPair
//
static hkInternalClassMember hkSerializedDisplayRbTransforms_DisplayTransformPairClass_Members[] =
{
	{ "rb", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "localToDisplay", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_310;
hkClass hkSerializedDisplayRbTransformsDisplayTransformPairClass_310(
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
	{ "transforms", &hkSerializedDisplayRbTransformsDisplayTransformPairClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

extern hkClass hkSerializedDisplayRbTransformsClass_310;
hkClass hkSerializedDisplayRbTransformsClass_310(
	"hkSerializedDisplayRbTransforms",
	&hkReferencedObjectClass_310, // parent
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

hkClass hkShapeClass_310(
	"hkShape",
	&hkReferencedObjectClass_310, // parent
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

hkClass hkShapeCollectionClass_310(
	"hkShapeCollection",
	&hkShapeClass_310, // parent
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
extern hkClass hkShapeCollectionFilterClass_310;
hkClass hkShapeCollectionFilterClass_310(
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
	{ "motionState", &hkMotionStateClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

hkClass hkShapePhantomClass_310(
	"hkShapePhantom",
	&hkPhantomClass_310, // parent
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
	{ "rayShapeCollectionFilter", &hkRayShapeCollectionFilterClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkShapeRayCastInputClass_310(
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
extern hkClass TriangleClass_310;

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
extern hkClass hkSimpleMeshShapeTriangleClass_310;
hkClass hkSimpleMeshShapeTriangleClass_310(
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
	{ "triangles", &hkSimpleMeshShapeTriangleClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "materialIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSimpleMeshShapeClass_310;
hkClass hkSimpleMeshShapeClass_310(
	"hkSimpleMeshShape",
	&hkShapeCollectionClass_310, // parent
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
extern hkClass hkSimpleShapePhantomhkCollisionDetailClass_310;
hkClass hkSimpleShapePhantomhkCollisionDetailClass_310(
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

extern hkClass hkSimpleShapePhantomClass_310;
hkClass hkSimpleShapePhantomClass_310(
	"hkSimpleShapePhantom",
	&hkShapePhantomClass_310, // parent
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
	{ "duration", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "numberOfTracks", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "extractedMotion", &hkAnimatedReferenceFrameClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "annotationTracks", &hkAnnotationTrackClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

hkClass hkSkeletalAnimationClass_310(
	"hkSkeletalAnimation",
	&hkReferencedObjectClass_310, // parent
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
// PRODUCT    : MAYHEM
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkanimation/rig/hkSkeleton.h'



// External pointer and enum types

//
// Class hkSkeleton
//
static hkInternalClassMember hkSkeletonClass_Members[] =
{
	{ "parentIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "bones", &hkBoneClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "referencePose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_QSTRANSFORM, 0, 0, 0 }
};
hkClass hkSkeletonClass_310(
	"hkSkeleton",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonClass_Members),
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

// Generated from 'hkanimation/mapper/hkSkeletonMapper.h'



// External pointer and enum types
extern hkClass hkSkeletonMapperDataClass_310;

//
// Class hkSkeletonMapper
//
static hkInternalClassMember hkSkeletonMapperClass_Members[] =
{
	{ "mapping", &hkSkeletonMapperDataClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSkeletonMapperClass_310;
hkClass hkSkeletonMapperClass_310(
	"hkSkeletonMapper",
	&hkReferencedObjectClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperClass_Members),
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

// Generated from 'hkanimation/mapper/hkSkeletonMapperData.h'



// External pointer and enum types
extern hkClass ChainMappingClass_310;
extern hkClass SimpleMappingClass_310;

//
// Class hkSkeletonMapperData
//

//
// Class hkSkeletonMapperData::SimpleMapping
//
static hkInternalClassMember hkSkeletonMapperData_SimpleMappingClass_Members[] =
{
	{ "boneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "boneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "aFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSkeletonMapperDataSimpleMappingClass_310;
hkClass hkSkeletonMapperDataSimpleMappingClass_310(
	"hkSkeletonMapperDataSimpleMapping",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperData_SimpleMappingClass_Members),
	3, // members
	HK_NULL // defaults
	);

//
// Class hkSkeletonMapperData::ChainMapping
//
static hkInternalClassMember hkSkeletonMapperData_ChainMappingClass_Members[] =
{
	{ "startBoneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endBoneA", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "startBoneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endBoneB", HK_NULL, HK_NULL, hkClassMember::TYPE_INT16, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "startAFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "endAFromBTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_QSTRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkSkeletonMapperDataChainMappingClass_310;
hkClass hkSkeletonMapperDataChainMappingClass_310(
	"hkSkeletonMapperDataChainMapping",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperData_ChainMappingClass_Members),
	6, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkSkeletonMapperDataClass_Members[] =
{
	{ "skeletonA", &hkSkeletonClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "skeletonB", &hkSkeletonClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "simpleMappings", &hkSkeletonMapperDataSimpleMappingClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chainMappings", &hkSkeletonMapperDataChainMappingClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "unmappedBones", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT16, 0, 0, 0 },
	{ "keepUnmappedLocal", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSkeletonMapperDataClass_310(
	"hkSkeletonMapperData",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSkeletonMapperDataClass_Members),
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

// Generated from 'hkdynamics/entity/hkSpatialRigidBodyDeactivator.h'



// External pointer and enum types
extern hkClass hkSpatialRigidBodyDeactivatorSampleClass_310;

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
hkClass hkSpatialRigidBodyDeactivatorSampleClass_310(
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
	{ "highFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "lowFrequencySample", &hkSpatialRigidBodyDeactivatorSampleClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "radiusSqrd", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minHighFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyTranslation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "minLowFrequencyRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSpatialRigidBodyDeactivatorClass_310;
hkClass hkSpatialRigidBodyDeactivatorClass_310(
	"hkSpatialRigidBodyDeactivator",
	&hkRigidBodyDeactivatorClass_310, // parent
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
hkClass hkSphereClass_310(
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

extern hkClass hkSphereMotionClass_310;
hkClass hkSphereMotionClass_310(
	"hkSphereMotion",
	&hkRigidMotionClass_310, // parent
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
extern hkClass hkSphereRepShapehkCollisionSpheresInfoClass_310;
hkClass hkSphereRepShapehkCollisionSpheresInfoClass_310(
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

hkClass hkSphereRepShapeClass_310(
	"hkSphereRepShape",
	&hkShapeClass_310, // parent
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

extern hkClass hkSphereShapeClass_310;
hkClass hkSphereShapeClass_310(
	"hkSphereShape",
	&hkConvexShapeClass_310, // parent
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

extern hkClass hkSpringActionClass_310;
hkClass hkSpringActionClass_310(
	"hkSpringAction",
	&hkBinaryActionClass_310, // parent
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

// Generated from 'hkdynamics/constraint/motor/springdamper/hkSpringDamperConstraintMotor.h'



//
// Class hkSpringDamperConstraintMotor
//
static hkInternalClassMember hkSpringDamperConstraintMotorClass_Members[] =
{
	{ "springConstant", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "springDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxPosForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxNegForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkSpringDamperConstraintMotorClass_310;
hkClass hkSpringDamperConstraintMotorClass_310(
	"hkSpringDamperConstraintMotor",
	&hkConstraintMotorClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkSpringDamperConstraintMotorClass_Members),
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

extern hkClass hkStabilizedBoxMotionClass_310;
hkClass hkStabilizedBoxMotionClass_310(
	"hkStabilizedBoxMotion",
	&hkBoxMotionClass_310, // parent
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

extern hkClass hkStabilizedSphereMotionClass_310;
hkClass hkStabilizedSphereMotionClass_310(
	"hkStabilizedSphereMotion",
	&hkSphereMotionClass_310, // parent
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
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 1, 0, 0 }
};
extern hkClass hkStiffSpringConstraintDataRuntimeClass_310;
hkClass hkStiffSpringConstraintDataRuntimeClass_310(
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

extern hkClass hkStiffSpringConstraintDataClass_310;
hkClass hkStiffSpringConstraintDataClass_310(
	"hkStiffSpringConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass SubpartStorageClass_310;

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
	{ "materials", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "materialIndices16", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 }
};
extern hkClass hkStorageMeshShapeSubpartStorageClass_310;
hkClass hkStorageMeshShapeSubpartStorageClass_310(
	"hkStorageMeshShapeSubpartStorage",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStorageMeshShape_SubpartStorageClass_Members),
	6, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkStorageMeshShapeClass_Members[] =
{
	{ "storage", &hkStorageMeshShapeSubpartStorageClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkStorageMeshShapeClass_310;
hkClass hkStorageMeshShapeClass_310(
	"hkStorageMeshShape",
	&hkMeshShapeClass_310, // parent
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

// Generated from 'hkcollide/shape/storagesampledheightfield/hkStorageSampledHeightFieldShape.h'



// External pointer and enum types

//
// Class hkStorageSampledHeightFieldShape
//
static hkInternalClassMember hkStorageSampledHeightFieldShapeClass_Members[] =
{
	{ "storage", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "triangleFlip", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkStorageSampledHeightFieldShapeClass_310;
hkClass hkStorageSampledHeightFieldShapeClass_310(
	"hkStorageSampledHeightFieldShape",
	&hkSampledHeightFieldShapeClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkStorageSampledHeightFieldShapeClass_Members),
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
hkClass hkSweptTransformClass_310(
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
	{ "childShape", &hkShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rotation", HK_NULL, HK_NULL, hkClassMember::TYPE_QUATERNION, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "transform", HK_NULL, HK_NULL, hkClassMember::TYPE_TRANSFORM, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTransformShapeClass_310;
hkClass hkTransformShapeClass_310(
	"hkTransformShape",
	&hkShapeClass_310, // parent
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
extern hkClass hkTriPatchTriangleClass_310;
hkClass hkTriPatchTriangleClass_310(
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

extern hkClass hkTriSampledHeightFieldBvTreeShapeClass_310;
hkClass hkTriSampledHeightFieldBvTreeShapeClass_310(
	"hkTriSampledHeightFieldBvTreeShape",
	&hkBvTreeShapeClass_310, // parent
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
	{ "heightfield", &hkSampledHeightFieldShapeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "radius", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkTriSampledHeightFieldCollectionClass_310;
hkClass hkTriSampledHeightFieldCollectionClass_310(
	"hkTriSampledHeightFieldCollection",
	&hkShapeCollectionClass_310, // parent
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

extern hkClass hkTriangleShapeClass_310;
hkClass hkTriangleShapeClass_310(
	"hkTriangleShape",
	&hkConvexShapeClass_310, // parent
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

hkClass hkTypedBroadPhaseHandleClass_310(
	"hkTypedBroadPhaseHandle",
	&hkBroadPhaseHandleClass_310, // parent
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
extern hkClass hkTyremarkPointClass_310;
extern hkClass hkTyremarksWheelClass_310;

//
// Class hkTyremarkPoint
//
static hkInternalClassMember hkTyremarkPointClass_Members[] =
{
	{ "pointLeft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "pointRight", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkTyremarkPointClass_310(
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
	{ "tyremarkPoints", &hkTyremarkPointClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkTyremarksWheelClass_310(
	"hkTyremarksWheel",
	&hkReferencedObjectClass_310, // parent
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
	{ "tyremarksWheel", &hkTyremarksWheelClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

extern hkClass hkTyremarksInfoClass_310;
hkClass hkTyremarksInfoClass_310(
	"hkTyremarksInfo",
	&hkReferencedObjectClass_310, // parent
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
	{ "entity", &hkEntityClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkUnaryActionClass_310(
	"hkUnaryAction",
	&hkActionClass_310, // parent
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
extern hkClass hkVehicleAerodynamicsAerodynamicsDragOutputClass_310;
hkClass hkVehicleAerodynamicsAerodynamicsDragOutputClass_310(
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

extern hkClass hkVehicleAerodynamicsClass_310;
hkClass hkVehicleAerodynamicsClass_310(
	"hkVehicleAerodynamics",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkBoolClass_310;

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
extern hkClass hkVehicleBrakeWheelBreakingOutputClass_310;
hkClass hkVehicleBrakeWheelBreakingOutputClass_310(
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

extern hkClass hkVehicleBrakeClass_310;
hkClass hkVehicleBrakeClass_310(
	"hkVehicleBrake",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass WheelComponentParamsClass_310;
extern hkClass hkInt8Class_310;
extern hkClass hkVehicleFrictionDescriptionClass_310;

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
extern hkClass hkVehicleDataWheelComponentParamsClass_310;
hkClass hkVehicleDataWheelComponentParamsClass_310(
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
	{ "wheelParams", &hkVehicleDataWheelComponentParamsClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "numWheelsPerAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_INT8, 0, 0, 0 },
	{ "frictionDescription", &hkVehicleFrictionDescriptionClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisFrictionInertiaInvDiag", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "alreadyInitialised", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleDataClass_310;
hkClass hkVehicleDataClass_310(
	"hkVehicleData",
	&hkReferencedObjectClass_310, // parent
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

extern hkClass hkVehicleDefaultAerodynamicsClass_310;
hkClass hkVehicleDefaultAerodynamicsClass_310(
	"hkVehicleDefaultAerodynamics",
	&hkVehicleAerodynamicsClass_310, // parent
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
extern hkClass hkVehicleDriverInputStatusClass_310;

extern hkClass hkVehicleDriverInputAnalogStatusClass_310;
hkClass hkVehicleDriverInputAnalogStatusClass_310(
	"hkVehicleDriverInputAnalogStatus",
	&hkVehicleDriverInputStatusClass_310, // parent
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
extern hkClass hkVehicleDriverInputClass_310;

extern hkClass hkVehicleDefaultAnalogDriverInputClass_310;
hkClass hkVehicleDefaultAnalogDriverInputClass_310(
	"hkVehicleDefaultAnalogDriverInput",
	&hkVehicleDriverInputClass_310, // parent
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
extern hkClass WheelBrakingPropertiesClass_310;

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
extern hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_310;
hkClass hkVehicleDefaultBrakeWheelBrakingPropertiesClass_310(
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
	{ "wheelBrakingProperties", &hkVehicleDefaultBrakeWheelBrakingPropertiesClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsMinTimeToBlock", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleDefaultBrakeClass_310;
hkClass hkVehicleDefaultBrakeClass_310(
	"hkVehicleDefaultBrake",
	&hkVehicleBrakeClass_310, // parent
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
extern hkClass hkVehicleEngineClass_310;

extern hkClass hkVehicleDefaultEngineClass_310;
hkClass hkVehicleDefaultEngineClass_310(
	"hkVehicleDefaultEngine",
	&hkVehicleEngineClass_310, // parent
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
extern hkClass hkVehicleSteeringClass_310;

extern hkClass hkVehicleDefaultSteeringClass_310;
hkClass hkVehicleDefaultSteeringClass_310(
	"hkVehicleDefaultSteering",
	&hkVehicleSteeringClass_310, // parent
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
extern hkClass WheelSpringSuspensionParametersClass_310;

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
extern hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_310;
hkClass hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_310(
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
	{ "wheelSpringParams", &hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
extern hkClass hkVehicleSuspensionClass_310;

extern hkClass hkVehicleDefaultSuspensionClass_310;
hkClass hkVehicleDefaultSuspensionClass_310(
	"hkVehicleDefaultSuspension",
	&hkVehicleSuspensionClass_310, // parent
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
extern hkClass hkVehicleTransmissionClass_310;

extern hkClass hkVehicleDefaultTransmissionClass_310;
hkClass hkVehicleDefaultTransmissionClass_310(
	"hkVehicleDefaultTransmission",
	&hkVehicleTransmissionClass_310, // parent
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
extern hkClass hkVehicleVelocityDamperClass_310;

extern hkClass hkVehicleDefaultVelocityDamperClass_310;
hkClass hkVehicleDefaultVelocityDamperClass_310(
	"hkVehicleDefaultVelocityDamper",
	&hkVehicleVelocityDamperClass_310, // parent
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

hkClass hkVehicleDriverInputStatusClass_310(
	"hkVehicleDriverInputStatus",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkVehicleDriverInputFilteredDriverInputOutputClass_310;
hkClass hkVehicleDriverInputFilteredDriverInputOutputClass_310(
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

hkClass hkVehicleDriverInputClass_310(
	"hkVehicleDriverInput",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkVehicleEngineEngineOutputClass_310;
hkClass hkVehicleEngineEngineOutputClass_310(
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

hkClass hkVehicleEngineClass_310(
	"hkVehicleEngine",
	&hkReferencedObjectClass_310, // parent
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

// Generated from 'hkvehicle/hkVehicleInstance.h'



// External pointer and enum types
extern hkClass WheelInfoClass_310;
extern hkClass hkVehicleFrictionStatusClass_310;
extern hkClass hkVehicleWheelCollideClass_310;

//
// Class hkVehicleInstance
//

//
// Class hkVehicleInstance::WheelInfo
//
static hkInternalClassMember hkVehicleInstance_WheelInfoClass_Members[] =
{
	{ "contactPoint", &hkContactPointClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactBody", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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
extern hkClass hkVehicleInstanceWheelInfoClass_310;
hkClass hkVehicleInstanceWheelInfoClass_310(
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
	{ "data", &hkVehicleDataClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "driverInput", &hkVehicleDriverInputClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "steering", &hkVehicleSteeringClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "engine", &hkVehicleEngineClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "transmission", &hkVehicleTransmissionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "brake", &hkVehicleBrakeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "suspension", &hkVehicleSuspensionClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "aerodynamics", &hkVehicleAerodynamicsClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelCollide", &hkVehicleWheelCollideClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "tyreMarks", &hkTyremarksInfoClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "chassis", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "velocityDamper", &hkVehicleVelocityDamperClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "wheelsInfo", &hkVehicleInstanceWheelInfoClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "frictionStatus", &hkVehicleFrictionStatusClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "deviceStatus", &hkVehicleDriverInputStatusClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
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

extern hkClass hkVehicleInstanceClass_310;
hkClass hkVehicleInstanceClass_310(
	"hkVehicleInstance",
	&hkUnaryActionClass_310, // parent
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
extern hkClass hkRejectRayChassisListenerClass_310;

//
// Class hkRejectRayChassisListener
//
static hkInternalClassMember hkRejectRayChassisListenerClass_Members[] =
{
	{ "chassis", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};

hkClass hkRejectRayChassisListenerClass_310(
	"hkRejectRayChassisListener",
	&hkReferencedObjectClass_310, // parent
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
	{ "phantom", &hkAabbPhantomClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "rejectRayChassisListener", &hkRejectRayChassisListenerClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVehicleRaycastWheelCollideClass_310;
hkClass hkVehicleRaycastWheelCollideClass_310(
	"hkVehicleRaycastWheelCollide",
	&hkVehicleWheelCollideClass_310, // parent
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
extern hkClass hkVehicleSteeringSteeringAnglesOutputClass_310;
hkClass hkVehicleSteeringSteeringAnglesOutputClass_310(
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

hkClass hkVehicleSteeringClass_310(
	"hkVehicleSteering",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass SuspensionWheelParametersClass_310;

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
extern hkClass hkVehicleSuspensionSuspensionWheelParametersClass_310;
hkClass hkVehicleSuspensionSuspensionWheelParametersClass_310(
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
	{ "wheelParams", &hkVehicleSuspensionSuspensionWheelParametersClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkVehicleSuspensionClass_310(
	"hkVehicleSuspension",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkVehicleTransmissionTransmissionOutputClass_310;
hkClass hkVehicleTransmissionTransmissionOutputClass_310(
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

hkClass hkVehicleTransmissionClass_310(
	"hkVehicleTransmission",
	&hkReferencedObjectClass_310, // parent
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

hkClass hkVehicleVelocityDamperClass_310(
	"hkVehicleVelocityDamper",
	&hkReferencedObjectClass_310, // parent
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
	{ "contactPoint", &hkContactPointClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactBody", &hkRigidBodyClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "contactShapeKey", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "currentSuspensionLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionRelativeVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "clippedInvContactDotSuspension", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleWheelCollideCollisionDetectionWheelOutputClass_310;
hkClass hkVehicleWheelCollideCollisionDetectionWheelOutputClass_310(
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

hkClass hkVehicleWheelCollideClass_310(
	"hkVehicleWheelCollide",
	&hkReferencedObjectClass_310, // parent
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

// Generated from 'hkdynamics/constraint/motor/velocity/hkVelocityConstraintMotor.h'



//
// Class hkVelocityConstraintMotor
//
static hkInternalClassMember hkVelocityConstraintMotorClass_Members[] =
{
	{ "tau", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "velocityTarget", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxPosForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxNegForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkVelocityConstraintMotorClass_310;
hkClass hkVelocityConstraintMotorClass_310(
	"hkVelocityConstraintMotor",
	&hkConstraintMotorClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVelocityConstraintMotorClass_Members),
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

// Generated from 'hkanimation/animation/waveletcompressed/hkWaveletSkeletalAnimation.h'



// External pointer and enum types
extern hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_310;

//
// Class hkWaveletSkeletalAnimation
//

//
// Class hkWaveletSkeletalAnimation::CompressionParams
//
static hkInternalClassMember hkWaveletSkeletalAnimation_CompressionParamsClass_Members[] =
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
extern hkClass hkWaveletSkeletalAnimationCompressionParamsClass_310;
hkClass hkWaveletSkeletalAnimationCompressionParamsClass_310(
	"hkWaveletSkeletalAnimationCompressionParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletSkeletalAnimation_CompressionParamsClass_Members),
	8, // members
	HK_NULL // defaults
	);

//
// Class hkWaveletSkeletalAnimation::QuantizationFormat
//
static hkInternalClassMember hkWaveletSkeletalAnimation_QuantizationFormatClass_Members[] =
{
	{ "bitWidth", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "preserved", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "offset", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "scale", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 }
};
hkClass hkWaveletSkeletalAnimationQuantizationFormatClass_310(
	"hkWaveletSkeletalAnimationQuantizationFormat",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletSkeletalAnimation_QuantizationFormatClass_Members),
	4, // members
	HK_NULL // defaults
	);
static hkInternalClassMember hkWaveletSkeletalAnimationClass_Members[] =
{
	{ "numberOfPoses", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "blockSize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "qFormat", &hkWaveletSkeletalAnimationQuantizationFormatClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "quantizedData", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT8, 0, 0, 0 },
	{ "staticMask", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "staticDOFs", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_REAL, 0, 0, 0 },
	{ "blockIndex", HK_NULL, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 }
};

extern hkClass hkWaveletSkeletalAnimationClass_310;
hkClass hkWaveletSkeletalAnimationClass_310(
	"hkWaveletSkeletalAnimation",
	&hkSkeletalAnimationClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWaveletSkeletalAnimationClass_Members),
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
extern hkClass hkWheelConstraintDataConstraintBasisAClass_310;
extern hkClass hkWheelConstraintDataConstraintBasisBClass_310;

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
hkClass hkWheelConstraintDataConstraintBasisAClass_310(
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
hkClass hkWheelConstraintDataConstraintBasisBClass_310(
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
	{ "solverResults", &hkSolverResultsClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 6, 0, 0 }
};
extern hkClass hkWheelConstraintDataRuntimeClass_310;
hkClass hkWheelConstraintDataRuntimeClass_310(
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
	{ "basisA", &hkWheelConstraintDataConstraintBasisAClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "basisB", &hkWheelConstraintDataConstraintBasisBClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMinLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionMaxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "suspensionDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};

extern hkClass hkWheelConstraintDataClass_310;
hkClass hkWheelConstraintDataClass_310(
	"hkWheelConstraintData",
	&hkConstraintDataClass_310, // parent
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
extern hkClass hkWorldMemoryWatchDogClass_310;

//
// Class hkWorldCinfo
//

//
// Enum hkWorldCinfo::SolverType
//
static const hkInternalClassEnumItem hkWorldCinfoSolverTypeEnumItems_310[] =
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
static const hkInternalClassEnumItem hkWorldCinfoSimulationTypeEnumItems_310[] =
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
};

//
// Enum hkWorldCinfo::ContactPointGeneration
//
static const hkInternalClassEnumItem hkWorldCinfoContactPointGenerationEnumItems_310[] =
{
	{0, "CONTACT_POINT_ACCEPT_ALWAYS"},
	{1, "CONTACT_POINT_REJECT_DUBIOUS"},
	{2, "CONTACT_POINT_REJECT_MANY"},
};

//
// Enum hkWorldCinfo::BroadPhaseBorderBehaviour
//
static const hkInternalClassEnumItem hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_310[] =
{
	{0, "BROADPHASE_BORDER_ASSERT"},
	{1, "BROADPHASE_BORDER_FIX_ENTITY"},
	{2, "BROADPHASE_BORDER_REMOVE_ENTITY"},
	{3, "BROADPHASE_BORDER_DO_NOTHING"},
};
static const hkInternalClassEnum hkWorldCinfoEnums[] = {
	{"SolverType", hkWorldCinfoSolverTypeEnumItems_310, 11 },
	{"SimulationType", hkWorldCinfoSimulationTypeEnumItems_310, 9 },
	{"ContactPointGeneration", hkWorldCinfoContactPointGenerationEnumItems_310, 3 },
	{"BroadPhaseBorderBehaviour", hkWorldCinfoBroadPhaseBorderBehaviourEnumItems_310, 4 }
};
extern const hkClassEnum* hkWorldCinfoSolverTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[0]);
extern const hkClassEnum* hkWorldCinfoSimulationTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[1]);
extern const hkClassEnum* hkWorldCinfoContactPointGenerationEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[2]);
extern const hkClassEnum* hkWorldCinfoBroadPhaseBorderBehaviourEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkWorldCinfoEnums[3]);
static hkInternalClassMember hkWorldCinfoClass_Members[] =
{
	{ "gravity", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseQuerySize", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactRestingVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "broadPhaseBorderBehaviour", HK_NULL, hkWorldCinfoBroadPhaseBorderBehaviourEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "broadPhaseWorldAabb", &hkAabbClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionTolerance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "collisionFilter", &hkCollisionFilterClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "expectedMaxLinearVelocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "expectedMinPsiDeltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "memoryWatchDog", &hkWorldMemoryWatchDogClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "broadPhaseNumMarkers", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "contactPointGeneration", HK_NULL, hkWorldCinfoContactPointGenerationEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
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
	{ "simulationType", HK_NULL, hkWorldCinfoSimulationTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "enableSimulationIslands", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
namespace
{
	struct hkWorldCinfo_DefaultStruct
	{
		int s_defaultOffsets[25];
		hkReal m_contactRestingVelocity;
	};
	const hkWorldCinfo_DefaultStruct hkWorldCinfo_Default =
	{
		{ -1,-1, HK_OFFSET_OF(hkWorldCinfo_DefaultStruct,m_contactRestingVelocity),
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },
		274877906944.0f
	};
}
hkClass hkWorldCinfoClass_310(
	"hkWorldCinfo",
	&hkReferencedObjectClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkWorldCinfoEnums),
	4, // enums
	reinterpret_cast<const hkClassMember*>(hkWorldCinfoClass_Members),
	25, // members
	&hkWorldCinfo_Default // defaults
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

hkClass hkWorldMemoryWatchDogClass_310(
	"hkWorldMemoryWatchDog",
	&hkReferencedObjectClass_310, // parent
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
static const hkInternalClassEnumItem hkWorldObjectBroadPhaseTypeEnumItems_310[] =
{
	{0, "BROAD_PHASE_INVALID"},
	{1, "BROAD_PHASE_ENTITY"},
	{2, "BROAD_PHASE_PHANTOM"},
	{3, "BROAD_PHASE_MAX_ID"},
};
static const hkInternalClassEnum hkWorldObjectEnums[] = {
	{"BroadPhaseType", hkWorldObjectBroadPhaseTypeEnumItems_310, 4 }
};
extern const hkClassEnum* hkWorldObjectBroadPhaseTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkWorldObjectEnums[0]);
static hkInternalClassMember hkWorldObjectClass_Members[] =
{
	{ "world", HK_NULL, HK_NULL, hkClassMember::TYPE_ZERO, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "userData", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "collidable", &hkLinkedCollidableClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "properties", &hkPropertyClass_310, HK_NULL, hkClassMember::TYPE_ARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};

hkClass hkWorldObjectClass_310(
	"hkWorldObject",
	&hkReferencedObjectClass_310, // parent
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
extern hkClass hkxCameraClass_310;
hkClass hkxCameraClass_310(
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
static const hkInternalClassEnumItem hkxIndexBufferIndexTypeEnumItems_310[] =
{
	{0, "INDEX_TYPE_INVALID"},
	{1, "INDEX_TYPE_TRI_LIST"},
	{2, "INDEX_TYPE_TRI_STRIP"},
	{3, "INDEX_TYPE_TRI_FAN"},
	{4, "INDEX_TYPE_MAX_ID"},
};
static const hkInternalClassEnum hkxIndexBufferEnums[] = {
	{"IndexType", hkxIndexBufferIndexTypeEnumItems_310, 5 }
};
extern const hkClassEnum* hkxIndexBufferIndexTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkxIndexBufferEnums[0]);
static hkInternalClassMember hkxIndexBufferClass_Members[] =
{
	{ "indexType", HK_NULL, hkxIndexBufferIndexTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "indices16", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT16, 0, 0, 0 },
	{ "indices32", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_UINT32, 0, 0, 0 },
	{ "vertexBaseOffset", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "length", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxIndexBufferClass_310;
hkClass hkxIndexBufferClass_310(
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
static const hkInternalClassEnumItem hkxLightLightTypeEnumItems_310[] =
{
	{0, "POINT_LIGHT"},
	{1, "DIRECTIONAL_LIGHT"},
	{2, "SPOT_LIGHT"},
};
static const hkInternalClassEnum hkxLightEnums[] = {
	{"LightType", hkxLightLightTypeEnumItems_310, 3 }
};
extern const hkClassEnum* hkxLightLightTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkxLightEnums[0]);
static hkInternalClassMember hkxLightClass_Members[] =
{
	{ "type", HK_NULL, hkxLightLightTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 },
	{ "position", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "direction", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "color", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "angle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxLightClass_310;
hkClass hkxLightClass_310(
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
extern hkClass TextureStageClass_310;
extern hkClass hkxMaterialClass_310;

//
// Class hkxMaterial
//

//
// Enum hkxMaterial::TextureType
//
static const hkInternalClassEnumItem hkxMaterialTextureTypeEnumItems_310[] =
{
	{0, "TEX_UNKNOWN"},
	{1, "TEX_DIFFUSE"},
	{2, "TEX_REFLECTION"},
	{3, "TEX_BUMP"},
	{4, "TEX_NORMAL"},
	{5, "TEX_DISPLACEMENT"},
};
static const hkInternalClassEnum hkxMaterialEnums[] = {
	{"TextureType", hkxMaterialTextureTypeEnumItems_310, 6 }
};
extern const hkClassEnum* hkxMaterialTextureTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkxMaterialEnums[0]);

//
// Class hkxMaterial::TextureStage
//
static hkInternalClassMember hkxMaterial_TextureStageClass_Members[] =
{
	{ "texture", HK_NULL, HK_NULL, hkClassMember::TYPE_VARIANT, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "usageHint", HK_NULL, HK_NULL, hkClassMember::TYPE_INT32, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkxMaterialTextureStageClass_310;
hkClass hkxMaterialTextureStageClass_310(
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
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "stages", &hkxMaterialTextureStageClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "diffuseColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "ambientColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "specularColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "emissiveColor", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "subMaterials", &hkxMaterialClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMaterialClass_310(
	"hkxMaterial",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkxMaterialEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkxMaterialClass_Members),
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

// Generated from 'hkscenedata/mesh/hkxMesh.h'



// External pointer and enum types
extern hkClass hkxMeshSectionClass_310;

//
// Class hkxMesh
//
static hkInternalClassMember hkxMeshClass_Members[] =
{
	{ "sections", &hkxMeshSectionClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
hkClass hkxMeshClass_310(
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
extern hkClass hkxVertexBufferClass_310;

//
// Class hkxMeshSection
//
static hkInternalClassMember hkxMeshSectionClass_Members[] =
{
	{ "vertexBuffer", &hkxVertexBufferClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "indexBuffers", &hkxIndexBufferClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "material", &hkxMaterialClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 }
};
hkClass hkxMeshSectionClass_310(
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
extern hkClass AnnotationDataClass_310;
extern hkClass hkMatrix4Class_310;
extern hkClass hkxNodeClass_310;

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
extern hkClass hkxNodeAnnotationDataClass_310;
hkClass hkxNodeAnnotationDataClass_310(
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
	{ "keyFrames", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "children", &hkxNodeClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "annotations", &hkxNodeAnnotationDataClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "userProperties", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "selected", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxNodeClass_310(
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
extern hkClass hkxSkinBindingClass_310;
extern hkClass hkxTextureFileClass_310;
extern hkClass hkxTextureInplaceClass_310;

//
// Class hkxScene
//
static hkInternalClassMember hkxSceneClass_Members[] =
{
	{ "modeller", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "asset", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, 0 },
	{ "sceneLength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "rootNode", &hkxNodeClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "cameras", &hkxCameraClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "lights", &hkxLightClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "meshes", &hkxMeshClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "materials", &hkxMaterialClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "inplaceTextures", &hkxTextureInplaceClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "externalTextures", &hkxTextureFileClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "skinBindings", &hkxSkinBindingClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 }
};
extern hkClass hkxSceneClass_310;
hkClass hkxSceneClass_310(
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
	{ "mesh", &hkxMeshClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "mapping", &hkxNodeClass_310, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_POINTER, 0, 0, 0 },
	{ "bindPose", HK_NULL, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_MATRIX4, 0, 0, 0 },
	{ "initSkinTransform", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkxSkinBindingClass_310(
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
hkClass hkxTextureFileClass_310(
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
hkClass hkxTextureInplaceClass_310(
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
extern hkClass hkxVertexFormatClass_310;
hkClass hkxVertexFormatClass_310(
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
extern hkClass hkxVertexP4N4C1T2Class_310;
hkClass hkxVertexP4N4C1T2Class_310(
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
extern hkClass hkxVertexP4N4T4B4C1T2Class_310;
hkClass hkxVertexP4N4T4B4C1T2Class_310(
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
extern hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_310;
hkClass hkxVertexP4N4T4B4W4I4C1Q2Class_310(
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
extern hkClass hkxVertexP4N4T4B4W4I4Q4Class_310;
hkClass hkxVertexP4N4T4B4W4I4Q4Class_310(
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
extern hkClass hkxVertexP4N4W4I4C1Q2Class_310;
hkClass hkxVertexP4N4W4I4C1Q2Class_310(
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

//
// Class hkxVertexBuffer
//
static hkInternalClassMember hkxVertexBufferClass_Members_310[] =
{
	{ "vertexData", HK_NULL, HK_NULL, hkClassMember::TYPE_HOMOGENEOUSARRAY, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "format", &hkxVertexFormatClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, hkClassMember::POINTER_OPTIONAL, 0 }
};
hkClass hkxVertexBufferClass_310(
	"hkxVertexBuffer",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkxVertexBufferClass_Members_310),
	2, // members
	HK_NULL // defaults
	);

//
// Class hkSolverResults
//
static hkInternalClassMember hkSolverResultsClass_Members_310[] =
{
    { "impulseApplied", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
    { "internalSolverData", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
hkClass hkSolverResultsClass_310(
    "hkSolverResults",
    HK_NULL, // parent
    0,
    HK_NULL,
    0, // interfaces
    HK_NULL,
    0, // enums
    reinterpret_cast<const hkClassMember*>(hkSolverResultsClass_Members_310),
    2, // members
    HK_NULL
    );


// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/classes/constraintsolver/hkVehicleFrictionSolver.hkclass'

// External pointer and enum types
extern hkClass hkRigidAccumulatorClass_310;
extern hkClass hkVehicleFrictionDescriptionAxisDescriptionClass_310;
extern hkClass hkVehicleFrictionStatusAxisStatusClass_310;

//
// Class hkVehicleStepInfo
//
static hkInternalClassMember hkVehicleStepInfoClass_Members_310[] =
{
	{ "deltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "invDeltaTime", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleStepInfoClass_310;
hkClass hkVehicleStepInfoClass_310(
	"hkVehicleStepInfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleStepInfoClass_Members_310),
	2, // members
	HK_NULL
	);

//
// Class hkVehicleFrictionDescription
//

//
// Class hkVehicleFrictionDescription::Cinfo
//
static hkInternalClassMember hkVehicleFrictionDescription_CinfoClass_Members_310[] =
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
extern hkClass hkVehicleFrictionDescription_CinfoClass_310;
hkClass hkVehicleFrictionDescription_CinfoClass_310(
	"hkVehicleFrictionDescriptionCinfo",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescription_CinfoClass_Members_310),
	10, // members
	HK_NULL
	);

//
// Class hkVehicleFrictionDescription::AxisDescription
//
static hkInternalClassMember hkVehicleFrictionDescription_AxisDescriptionClass_Members_310[] =
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
extern hkClass hkVehicleFrictionDescription_AxisDescriptionClass_310;
hkClass hkVehicleFrictionDescription_AxisDescriptionClass_310(
	"hkVehicleFrictionDescriptionAxisDescription",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescription_AxisDescriptionClass_Members_310),
	10, // members
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionDescriptionClass_Members_310[] =
{
	{ "wheelDistance", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassisMassInv", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "axleDescr", &hkVehicleFrictionDescription_AxisDescriptionClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionDescriptionClass_310(
	"hkVehicleFrictionDescription",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionDescriptionClass_Members_310),
	3, // members
	HK_NULL
	);

//
// Class hkVehicleFrictionStatus
//

//
// Class hkVehicleFrictionStatus::AxisStatus
//
static hkInternalClassMember hkVehicleFrictionStatus_AxisStatusClass_Members_310[] =
{
	{ "forward_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_slip_velocity", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "skid_energy_density", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "side_force", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "delayed_forward_impulse", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "sideRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardRhs", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleFrictionStatus_AxisStatusClass_310;
hkClass hkVehicleFrictionStatus_AxisStatusClass_310(
	"hkVehicleFrictionStatusAxisStatus",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatus_AxisStatusClass_Members_310),
	7, // members
	HK_NULL
	);
static hkInternalClassMember hkVehicleFrictionStatusClass_Members_310[] =
{
	{ "axis", &hkVehicleFrictionStatus_AxisStatusClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 2, 0, 0 }
};
hkClass hkVehicleFrictionStatusClass_310(
	"hkVehicleFrictionStatus",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionStatusClass_Members_310),
	1, // members
	HK_NULL
	);

//
// Class hkVehicleFrictionSolverAxleParams
//
static hkInternalClassMember hkVehicleFrictionSolverAxleParamsClass_Members_310[] =
{
	{ "contactPointWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "constraintNormalWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardDirWs", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "groundObject", &hkRigidAccumulatorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "groundObjectAtLastIntegration", &hkRigidAccumulatorClass_310, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, 0 },
	{ "frictionCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "viscosityFrictionCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "maxFrictionCoefficient", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelDownForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "forwardForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "wheelFixed", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT8, hkClassMember::TYPE_VOID, 0, 0, 0 }
};
extern hkClass hkVehicleFrictionSolverAxleParamsClass_310;
hkClass hkVehicleFrictionSolverAxleParamsClass_310(
	"hkVehicleFrictionSolverAxleParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionSolverAxleParamsClass_Members_310),
	11, // members
	HK_NULL
	);

//
// Class hkVehicleFrictionSolverParams
//
hkInternalClassMember hkVehicleFrictionSolverParamsClass_Members_310[] =
{
	{ "maxVelocityForPositionalFriction", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, 0 },
	{ "chassis", &hkRigidAccumulatorClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, 0, 0, 0, 0 },
	{ "chassisAtLastIntegration", &hkRigidAccumulatorClass_310, HK_NULL, hkClassMember::TYPE_STRUCT, 0, 0, 0, 0 }
};
extern hkClass hkVehicleFrictionSolverParamsClass_310;
hkClass hkVehicleFrictionSolverParamsClass_310(
	"hkVehicleFrictionSolverParams",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkVehicleFrictionSolverParamsClass_Members_310),
	3, // members
	HK_NULL
	);

// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/classes/constraintsolver/hkRigidAccumulator.hkclass'

// Class hkRigidAccumulator
//
static hkInternalClassMember hkRigidAccumulatorClass_Members_310[] =
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
extern hkClass hkVelocityAccumulatorClass_310;

hkClass hkRigidAccumulatorClass_310(
	"hkRigidAccumulator",
	&hkVelocityAccumulatorClass_310, // parent
	0,
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRigidAccumulatorClass_Members_310),
	10, // members
	HK_NULL
	);

// TKBMS v1.0 -----------------------------------------------------
//
// PLATFORM   : ALL
// PRODUCT    : HAVOK_2
// VISIBILITY : PUBLIC
//
// ------------------------------------------------------TKBMS v1.0

// Generated from 'hkdynamics/classes/constraintsolver/hkVelocityAccumulator.hkclass'

//
// Class hkVelocityAccumulator
//
static const hkInternalClassEnumItem hkVelocityAccumulator_hkAccumulatorTypeEnumItems_310[] =
{
	{0, "HK_RIGID_BODY"},
	{1, "HK_KEYFRAMED_RIGID_BODY"},
	{2, "HK_END"},
};
static const hkInternalClassEnum hkVelocityAccumulator_Enums_310[] = {
	{"hkAccumulatorType", hkVelocityAccumulator_hkAccumulatorTypeEnumItems_310, 3 }
};
extern const hkClassEnum* hkVelocityAccumulator_hkAccumulatorTypeEnum_310 = reinterpret_cast<const hkClassEnum*>(&hkVelocityAccumulator_Enums_310[0]);
hkInternalClassMember hkVelocityAccumulatorClass_Members_310[] =
{
	{ "type", HK_NULL, hkVelocityAccumulator_hkAccumulatorTypeEnum_310, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, 0 }
};
hkClass hkVelocityAccumulatorClass_310(
	"hkVelocityAccumulator",
	HK_NULL, // parent
	0,
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkVelocityAccumulator_Enums_310),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkVelocityAccumulatorClass_Members_310),
	1, // members
	HK_NULL
	);

extern hkClass* const hkHavok310Classes[];
hkClass* const hkHavok310Classes[] =
{
	&hkAabbClass_310,
	&hkAabbPhantomClass_310,
	&hkActionClass_310,
	&hkAngularDashpotActionClass_310,
	&hkAnimatedReferenceFrameClass_310,
	&hkAnimationBindingClass_310,
	&hkAnimationContainerClass_310,
	&hkAnnotationTrackAnnotationClass_310,
	&hkAnnotationTrackClass_310,
	&hkArrayActionClass_310,
	&hkBallAndSocketConstraintDataRuntimeClass_310,
	&hkBallAndSocketConstraintDataClass_310,
	&hkBaseObjectClass_310,
	&hkBinaryActionClass_310,
	&hkBoneAttachmentClass_310,
	&hkBoneClass_310,
	&hkBoxMotionClass_310,
	&hkBoxShapeClass_310,
	&hkBreakableConstraintDataClass_310,
	&hkBroadPhaseHandleClass_310,
	&hkBvShapeClass_310,
	&hkBvTreeShapeClass_310,
	&hkCachingShapePhantomhkCollisionDetailClass_310,
	&hkCachingShapePhantomClass_310,
	&hkCapsuleShapeClass_310,
	&hkCdBodyClass_310,
	&hkCharacterProxyCinfoClass_310,
	&hkClassClass_310,
	&hkClassEnumItemClass_310,
	&hkClassEnumClass_310,
	&hkClassMemberClass_310,
	&hkCollidableClass_310,
	&hkCollidableCollidableFilterClass_310,
	&hkCollisionFilterClass_310,
	&hkCollisionFilterListClass_310,
	&hkConstrainedSystemFilterClass_310,
	&hkConstraintDataRuntimeInfoClass_310,
	&hkConstraintDataClass_310,
	&hkConstraintInfoClass_310,
	&hkConstraintInstanceClass_310,
	&hkConstraintMotorClass_310,
	&hkContactPointClass_310,
	&hkContactPointMaterialClass_310,
	&hkConvexListShapeClass_310,
	&hkConvexPieceMeshShapeStatsClass_310,
	&hkConvexPieceMeshShapeClass_310,
	&hkConvexPieceStreamDataClass_310,
	&hkConvexShapeClass_310,
	&hkConvexTransformShapeClass_310,
	&hkConvexTranslateShapeClass_310,
	&hkConvexVerticesShapeFourVectorsClass_310,
	&hkConvexVerticesShapeClass_310,
	&hkCylinderShapeClass_310,
	&hkDashpotActionClass_310,
	&hkDefaultAnimatedReferenceFrameMotionExtractionOptionsClass_310,
	&hkDefaultAnimatedReferenceFrameClass_310,
	&hkDeltaCompressedSkeletalAnimationCompressionParamsClass_310,
	&hkDeltaCompressedSkeletalAnimationQuantizationFormatClass_310,
	&hkDeltaCompressedSkeletalAnimationClass_310,
	&hkDisableEntityCollisionFilterClass_310,
	&hkEntityClass_310,
	&hkEntityDeactivatorClass_310,
	&hkFakeRigidBodyDeactivatorClass_310,
	&hkFastMeshShapeClass_310,
	&hkFixedRigidMotionClass_310,
	&hkGenericConstraintDataClass_310,
	&hkGenericConstraintDataSchemeClass_310,
	&hkGroupCollisionFilterClass_310,
	&hkGroupFilterClass_310,
	&hkHeightFieldShapeCollideSpheresInputClass_310,
	&hkHeightFieldShapehkSphereCastInputClass_310,
	&hkHeightFieldShapeClass_310,
	&hkHingeConstraintDataRuntimeClass_310,
	&hkHingeConstraintDataConstraintBasisAClass_310,
	&hkHingeConstraintDataConstraintBasisBClass_310,
	&hkHingeConstraintDataClass_310,
	&hkInterleavedSkeletalAnimationClass_310,
	&hkKeyframedRigidMotionClass_310,
	&hkLimitedHingeConstraintDataRuntimeClass_310,
	&hkLimitedHingeConstraintDataConstraintBasisAClass_310,
	&hkLimitedHingeConstraintDataConstraintBasisBClass_310,
	&hkLimitedHingeConstraintDataClass_310,
	&hkLinearParametricCurveClass_310,
	&hkLinkedCollidableCollisionEntryClass_310,
	&hkLinkedCollidableClass_310,
	&hkListShapeChildInfoClass_310,
	&hkListShapeClass_310,
	&hkMalleableConstraintDataClass_310,
	&hkMaterialClass_310,
	&hkMeshBindingMappingClass_310,
	&hkMeshBindingClass_310,
	&hkMeshMaterialClass_310,
	&hkMeshShapeSubpartClass_310,
	&hkMeshShapeClass_310,
	&hkMoppBvTreeShapeClass_310,
	&hkMoppCodeCodeInfoClass_310,
	&hkMoppCodeClass_310,
	&hkMotionClass_310,
	&hkMotionStateClass_310,
	&hkMotorActionClass_310,
	&hkMouseSpringActionClass_310,
	&hkMultiRayShapeRayClass_310,
	&hkMultiRayShapeClass_310,
	&hkMultiSphereShapeClass_310,
	&hkNullCollisionFilterClass_310,
	&hkPackfileHeaderClass_310,
	&hkPackfileSectionHeaderClass_310,
	&hkPairwiseCollisionFilterCollisionPairClass_310,
	&hkPairwiseCollisionFilterClass_310,
	&hkParametricCurveClass_310,
	&hkPhantomCallbackShapeClass_310,
	&hkPhantomClass_310,
	&hkPhysicsDataSplitPhysicsSystemsOutputClass_310,
	&hkPhysicsDataClass_310,
	&hkPhysicsSystemClass_310,
	&hkPlaneShapeClass_310,
	&hkPointToPathConstraintDataRuntimeClass_310,
	&hkPointToPathConstraintDataClass_310,
	&hkPointToPlaneConstraintDataRuntimeClass_310,
	&hkPointToPlaneConstraintDataClass_310,
	&hkPositionConstraintMotorClass_310,
	&hkPoweredHingeConstraintDataRuntimeClass_310,
	&hkPoweredHingeConstraintDataClass_310,
	&hkPoweredRagdollConstraintDataRuntimeClass_310,
	&hkPoweredRagdollConstraintDataClass_310,
	&hkPrismaticConstraintDataRuntimeClass_310,
	&hkPrismaticConstraintDataConstraintBasisAClass_310,
	&hkPrismaticConstraintDataConstraintBasisBClass_310,
	&hkPrismaticConstraintDataClass_310,
	&hkPropertyValueClass_310,
	&hkPropertyClass_310,
	&hkRagdollConstraintDataRuntimeClass_310,
	&hkRagdollConstraintDataConstraintBasisAClass_310,
	&hkRagdollConstraintDataConstraintBasisBClass_310,
	&hkRagdollConstraintDataClass_310,
	&hkRagdollInstanceClass_310,
	&hkRayCollidableFilterClass_310,
	&hkRayShapeCollectionFilterClass_310,
	&hkReferencedObjectClass_310,
	&hkReorientActionClass_310,
	&hkRigidBodyClass_310,
	&hkRigidBodyDeactivatorClass_310,
	&hkRigidMotionClass_310,
	&hkRootLevelContainerNamedVariantClass_310,
	&hkRootLevelContainerClass_310,
	&hkSampledHeightFieldShapeClass_310,
	&hkSerializedDisplayMarkerClass_310,
	&hkSerializedDisplayMarkerListClass_310,
	&hkSerializedDisplayRbTransformsDisplayTransformPairClass_310,
	&hkSerializedDisplayRbTransformsClass_310,
	&hkShapeClass_310,
	&hkShapeCollectionClass_310,
	&hkShapeCollectionFilterClass_310,
	&hkShapePhantomClass_310,
	&hkShapeRayCastInputClass_310,
	&hkSimpleMeshShapeTriangleClass_310,
	&hkSimpleMeshShapeClass_310,
	&hkSimpleShapePhantomhkCollisionDetailClass_310,
	&hkSimpleShapePhantomClass_310,
	&hkSkeletalAnimationClass_310,
	&hkSkeletonClass_310,
	&hkSkeletonMapperClass_310,
	&hkSkeletonMapperDataSimpleMappingClass_310,
	&hkSkeletonMapperDataChainMappingClass_310,
	&hkSkeletonMapperDataClass_310,
	&hkSpatialRigidBodyDeactivatorSampleClass_310,
	&hkSpatialRigidBodyDeactivatorClass_310,
	&hkSphereClass_310,
	&hkSphereMotionClass_310,
	&hkSphereRepShapehkCollisionSpheresInfoClass_310,
	&hkSphereRepShapeClass_310,
	&hkSphereShapeClass_310,
	&hkSpringActionClass_310,
	&hkSpringDamperConstraintMotorClass_310,
	&hkStabilizedBoxMotionClass_310,
	&hkStabilizedSphereMotionClass_310,
	&hkStiffSpringConstraintDataRuntimeClass_310,
	&hkStiffSpringConstraintDataClass_310,
	&hkStorageMeshShapeSubpartStorageClass_310,
	&hkStorageMeshShapeClass_310,
	&hkStorageSampledHeightFieldShapeClass_310,
	&hkSweptTransformClass_310,
	&hkTransformShapeClass_310,
	&hkTriPatchTriangleClass_310,
	&hkTriSampledHeightFieldBvTreeShapeClass_310,
	&hkTriSampledHeightFieldCollectionClass_310,
	&hkTriangleShapeClass_310,
	&hkTypedBroadPhaseHandleClass_310,
	&hkTyremarkPointClass_310,
	&hkTyremarksWheelClass_310,
	&hkTyremarksInfoClass_310,
	&hkUnaryActionClass_310,
	&hkVehicleAerodynamicsAerodynamicsDragOutputClass_310,
	&hkVehicleAerodynamicsClass_310,
	&hkVehicleBrakeWheelBreakingOutputClass_310,
	&hkVehicleBrakeClass_310,
	&hkVehicleDataWheelComponentParamsClass_310,
	&hkVehicleDataClass_310,
	&hkVehicleDefaultAerodynamicsClass_310,
	&hkVehicleDriverInputAnalogStatusClass_310,
	&hkVehicleDefaultAnalogDriverInputClass_310,
	&hkVehicleDefaultBrakeWheelBrakingPropertiesClass_310,
	&hkVehicleDefaultBrakeClass_310,
	&hkVehicleDefaultEngineClass_310,
	&hkVehicleDefaultSteeringClass_310,
	&hkVehicleDefaultSuspensionWheelSpringSuspensionParametersClass_310,
	&hkVehicleDefaultSuspensionClass_310,
	&hkVehicleDefaultTransmissionClass_310,
	&hkVehicleDefaultVelocityDamperClass_310,
	&hkVehicleDriverInputStatusClass_310,
	&hkVehicleDriverInputFilteredDriverInputOutputClass_310,
	&hkVehicleDriverInputClass_310,
	&hkVehicleEngineEngineOutputClass_310,
	&hkVehicleEngineClass_310,
	&hkVehicleInstanceWheelInfoClass_310,
	&hkVehicleInstanceClass_310,
	&hkRejectRayChassisListenerClass_310,
	&hkVehicleRaycastWheelCollideClass_310,
	&hkVehicleSteeringSteeringAnglesOutputClass_310,
	&hkVehicleSteeringClass_310,
	&hkVehicleSuspensionSuspensionWheelParametersClass_310,
	&hkVehicleSuspensionClass_310,
	&hkVehicleTransmissionTransmissionOutputClass_310,
	&hkVehicleTransmissionClass_310,
	&hkVehicleVelocityDamperClass_310,
	&hkVehicleWheelCollideCollisionDetectionWheelOutputClass_310,
	&hkVehicleWheelCollideClass_310,
	&hkVelocityConstraintMotorClass_310,
	&hkWaveletSkeletalAnimationCompressionParamsClass_310,
	&hkWaveletSkeletalAnimationQuantizationFormatClass_310,
	&hkWaveletSkeletalAnimationClass_310,
	&hkWheelConstraintDataConstraintBasisAClass_310,
	&hkWheelConstraintDataConstraintBasisBClass_310,
	&hkWheelConstraintDataRuntimeClass_310,
	&hkWheelConstraintDataClass_310,
	&hkWorldCinfoClass_310,
	&hkWorldMemoryWatchDogClass_310,
	&hkWorldObjectClass_310,
	&hkxCameraClass_310,
	&hkxIndexBufferClass_310,
	&hkxLightClass_310,
	&hkxMaterialTextureStageClass_310,
	&hkxMaterialClass_310,
	&hkxMeshClass_310,
	&hkxMeshSectionClass_310,
	&hkxNodeAnnotationDataClass_310,
	&hkxNodeClass_310,
	&hkxSceneClass_310,
	&hkxSkinBindingClass_310,
	&hkxTextureFileClass_310,
	&hkxTextureInplaceClass_310,
	&hkxVertexFormatClass_310,
	&hkxVertexP4N4C1T2Class_310,
	&hkxVertexP4N4T4B4C1T2Class_310,
	&hkxVertexP4N4T4B4W4I4C1Q2Class_310,
	&hkxVertexP4N4T4B4W4I4Q4Class_310,
	&hkxVertexP4N4W4I4C1Q2Class_310,
	&hkxVertexBufferClass_310,
	&hkSolverResultsClass_310,
	&hkVehicleStepInfoClass_310,
	&hkVehicleFrictionDescription_CinfoClass_310,
	&hkVehicleFrictionDescription_AxisDescriptionClass_310,
	&hkVehicleFrictionDescriptionClass_310,
	&hkVehicleFrictionStatus_AxisStatusClass_310,
	&hkVehicleFrictionStatusClass_310,
	&hkVehicleFrictionSolverAxleParamsClass_310,
	&hkVehicleFrictionSolverParamsClass_310,
	&hkRigidAccumulatorClass_310,
	&hkVelocityAccumulatorClass_310,
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
