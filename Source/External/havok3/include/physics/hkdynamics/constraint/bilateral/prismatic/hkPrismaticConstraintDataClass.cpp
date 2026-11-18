/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/bilateral/prismatic/hkPrismaticConstraintData.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/bilateral/prismatic/hkPrismaticConstraintData.h>


// External pointer and enum types
extern const hkClass hkConstraintMotorClass;
extern const hkClass hkPrismaticConstraintDataConstraintBasisAClass;
extern const hkClass hkPrismaticConstraintDataConstraintBasisBClass;

//
// Class hkPrismaticConstraintData
//

//
// Class hkPrismaticConstraintData::ConstraintBasisA
//
static const hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisA,m_pivot) },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisA,m_shaft) },
	{ "BtoAoffsetRotation", HK_NULL, HK_NULL, hkClassMember::TYPE_ROTATION, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisA,m_BtoAoffsetRotation) }
};
const hkClass hkPrismaticConstraintDataConstraintBasisAClass(
	"hkPrismaticConstraintDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkPrismaticConstraintData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkPrismaticConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkPrismaticConstraintData::ConstraintBasisB
//
static const hkInternalClassMember hkPrismaticConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisB,m_pivot) },
	{ "shaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisB,m_shaft) },
	{ "perpToShaft", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData::ConstraintBasisB,m_perpToShaft) }
};
const hkClass hkPrismaticConstraintDataConstraintBasisBClass(
	"hkPrismaticConstraintDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkPrismaticConstraintData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkPrismaticConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkPrismaticConstraintDataClass_Members[] =
{
	{ "motor", &hkConstraintMotorClass, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_motor) },
	{ "basisA", &hkPrismaticConstraintDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_basisA) },
	{ "basisB", &hkPrismaticConstraintDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_basisB) },
	{ "minLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_minLimit) },
	{ "maxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_maxLimit) },
	{ "maxFrictionForce", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_maxFrictionForce) },
	{ "motorTargetPosition", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkPrismaticConstraintData,m_motorTargetPosition) }
};
extern const hkClass hkConstraintDataClass;

extern const hkClass hkPrismaticConstraintDataClass;
const hkClass hkPrismaticConstraintDataClass(
	"hkPrismaticConstraintData",
	&hkConstraintDataClass, // parent
	sizeof(hkPrismaticConstraintData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkPrismaticConstraintDataClass_Members),
	int(sizeof(hkPrismaticConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
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
