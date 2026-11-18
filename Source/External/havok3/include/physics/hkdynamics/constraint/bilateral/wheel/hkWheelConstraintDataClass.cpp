/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/bilateral/wheel/hkWheelConstraintData.h>


// External pointer and enum types
extern const hkClass hkWheelConstraintDataConstraintBasisAClass;
extern const hkClass hkWheelConstraintDataConstraintBasisBClass;

//
// Class hkWheelConstraintData
//

//
// Class hkWheelConstraintData::ConstraintBasisA
//
static const hkInternalClassMember hkWheelConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisA,m_pivot) },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisA,m_axle) }
};
const hkClass hkWheelConstraintDataConstraintBasisAClass(
	"hkWheelConstraintDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkWheelConstraintData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkWheelConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkWheelConstraintData::ConstraintBasisB
//
static const hkInternalClassMember hkWheelConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_pivot) },
	{ "steeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_steeringAxis) },
	{ "perpToSteeringAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_perpToSteeringAxis) },
	{ "suspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_suspensionAxis) },
	{ "perpToSuspensionAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_perpToSuspensionAxis) },
	{ "referenceAxle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData::ConstraintBasisB,m_referenceAxle) }
};
const hkClass hkWheelConstraintDataConstraintBasisBClass(
	"hkWheelConstraintDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkWheelConstraintData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkWheelConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkWheelConstraintDataClass_Members[] =
{
	{ "basisA", &hkWheelConstraintDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_basisA) },
	{ "basisB", &hkWheelConstraintDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_basisB) },
	{ "suspensionMinLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_suspensionMinLimit) },
	{ "suspensionMaxLimit", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_suspensionMaxLimit) },
	{ "suspensionStrength", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_suspensionStrength) },
	{ "suspensionDamping", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkWheelConstraintData,m_suspensionDamping) }
};
extern const hkClass hkConstraintDataClass;

extern const hkClass hkWheelConstraintDataClass;
const hkClass hkWheelConstraintDataClass(
	"hkWheelConstraintData",
	&hkConstraintDataClass, // parent
	sizeof(hkWheelConstraintData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkWheelConstraintDataClass_Members),
	int(sizeof(hkWheelConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
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
