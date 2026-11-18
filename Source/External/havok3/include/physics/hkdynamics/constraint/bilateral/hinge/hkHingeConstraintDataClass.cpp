/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/bilateral/hinge/hkHingeConstraintData.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/bilateral/hinge/hkHingeConstraintData.h>


// External pointer and enum types
extern const hkClass hkHingeConstraintDataConstraintBasisAClass;
extern const hkClass hkHingeConstraintDataConstraintBasisBClass;

//
// Class hkHingeConstraintData
//

//
// Class hkHingeConstraintData::ConstraintBasisA
//
static const hkInternalClassMember hkHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData::ConstraintBasisA,m_pivot) },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData::ConstraintBasisA,m_perpToAxle1) },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData::ConstraintBasisA,m_perpToAxle2) }
};
const hkClass hkHingeConstraintDataConstraintBasisAClass(
	"hkHingeConstraintDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkHingeConstraintData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkHingeConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkHingeConstraintData::ConstraintBasisB
//
static const hkInternalClassMember hkHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData::ConstraintBasisB,m_pivot) },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData::ConstraintBasisB,m_axle) }
};
const hkClass hkHingeConstraintDataConstraintBasisBClass(
	"hkHingeConstraintDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkHingeConstraintData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkHingeConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkHingeConstraintDataClass_Members[] =
{
	{ "basisA", &hkHingeConstraintDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData,m_basisA) },
	{ "basisB", &hkHingeConstraintDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeConstraintData,m_basisB) }
};
extern const hkClass hkConstraintDataClass;

extern const hkClass hkHingeConstraintDataClass;
const hkClass hkHingeConstraintDataClass(
	"hkHingeConstraintData",
	&hkConstraintDataClass, // parent
	sizeof(hkHingeConstraintData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeConstraintDataClass_Members),
	int(sizeof(hkHingeConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
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
