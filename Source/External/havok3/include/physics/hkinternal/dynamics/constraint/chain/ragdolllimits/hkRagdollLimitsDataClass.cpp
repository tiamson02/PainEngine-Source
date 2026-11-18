/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkinternal/dynamics/constraint/chain/ragdolllimits/hkRagdollLimitsData.h'

#include <hkinternal/hkInternal.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkinternal/dynamics/constraint/chain/ragdolllimits/hkRagdollLimitsData.h>


// External pointer and enum types
extern const hkClass hkRagdollLimitsDataConstraintBasisAClass;
extern const hkClass hkRagdollLimitsDataConstraintBasisBClass;

//
// Class hkRagdollLimitsData
//

//
// Class hkRagdollLimitsData::ConstraintBasisA
//
static const hkInternalClassMember hkRagdollLimitsData_ConstraintBasisAClass_Members[] =
{
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData::ConstraintBasisA,m_planeAxis) },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData::ConstraintBasisA,m_twistAxis) }
};
const hkClass hkRagdollLimitsDataConstraintBasisAClass(
	"hkRagdollLimitsDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkRagdollLimitsData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsData_ConstraintBasisAClass_Members),
	int(sizeof(hkRagdollLimitsData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkRagdollLimitsData::ConstraintBasisB
//
static const hkInternalClassMember hkRagdollLimitsData_ConstraintBasisBClass_Members[] =
{
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData::ConstraintBasisB,m_planeAxis) },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData::ConstraintBasisB,m_twistAxis) }
};
const hkClass hkRagdollLimitsDataConstraintBasisBClass(
	"hkRagdollLimitsDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkRagdollLimitsData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsData_ConstraintBasisBClass_Members),
	int(sizeof(hkRagdollLimitsData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkRagdollLimitsDataClass_Members[] =
{
	{ "basisA", &hkRagdollLimitsDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_basisA) },
	{ "basisB", &hkRagdollLimitsDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_basisB) },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_coneMinAngle) },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_planeMinAngle) },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_planeMaxAngle) },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_twistMinAngle) },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_twistMaxAngle) },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_maxFrictionTorque) },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollLimitsData,m_angularLimitsTauFactor) }
};
extern const hkClass hkConstraintDataClass;

extern const hkClass hkRagdollLimitsDataClass;
const hkClass hkRagdollLimitsDataClass(
	"hkRagdollLimitsData",
	&hkConstraintDataClass, // parent
	sizeof(hkRagdollLimitsData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollLimitsDataClass_Members),
	int(sizeof(hkRagdollLimitsDataClass_Members)/sizeof(hkInternalClassMember)),
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
