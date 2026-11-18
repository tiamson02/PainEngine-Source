/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h'

#include <hkinternal/hkInternal.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h>


// External pointer and enum types
extern const hkClass hkRagdollConstraintDataConstraintBasisAClass;
extern const hkClass hkRagdollConstraintDataConstraintBasisBClass;

//
// Class hkRagdollConstraintData
//

//
// Class hkRagdollConstraintData::ConstraintBasisA
//
static const hkInternalClassMember hkRagdollConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisA,m_pivot) },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisA,m_planeAxis) },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisA,m_twistAxis) }
};
const hkClass hkRagdollConstraintDataConstraintBasisAClass(
	"hkRagdollConstraintDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkRagdollConstraintData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkRagdollConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkRagdollConstraintData::ConstraintBasisB
//
static const hkInternalClassMember hkRagdollConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisB,m_pivot) },
	{ "planeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisB,m_planeAxis) },
	{ "twistAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData::ConstraintBasisB,m_twistAxis) }
};
const hkClass hkRagdollConstraintDataConstraintBasisBClass(
	"hkRagdollConstraintDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkRagdollConstraintData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkRagdollConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkRagdollConstraintDataClass_Members[] =
{
	{ "basisA", &hkRagdollConstraintDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_basisA) },
	{ "basisB", &hkRagdollConstraintDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_basisB) },
	{ "coneMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_coneMinAngle) },
	{ "planeMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_planeMinAngle) },
	{ "planeMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_planeMaxAngle) },
	{ "twistMinAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_twistMinAngle) },
	{ "twistMaxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_twistMaxAngle) },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_maxFrictionTorque) },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkRagdollConstraintData,m_angularLimitsTauFactor) }
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
extern const hkClass hkConstraintDataClass;

extern const hkClass hkRagdollConstraintDataClass;
const hkClass hkRagdollConstraintDataClass(
	"hkRagdollConstraintData",
	&hkConstraintDataClass, // parent
	sizeof(hkRagdollConstraintData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkRagdollConstraintDataClass_Members),
	int(sizeof(hkRagdollConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	&hkRagdollConstraintData_Default
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
