/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h>


// External pointer and enum types
extern const hkClass hkLimitedHingeConstraintDataConstraintBasisAClass;
extern const hkClass hkLimitedHingeConstraintDataConstraintBasisBClass;

//
// Class hkLimitedHingeConstraintData
//

//
// Class hkLimitedHingeConstraintData::ConstraintBasisA
//
static const hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisAClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisA,m_pivot) },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisA,m_axle) },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisA,m_perpToAxle1) },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisA,m_perpToAxle2) }
};
const hkClass hkLimitedHingeConstraintDataConstraintBasisAClass(
	"hkLimitedHingeConstraintDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkLimitedHingeConstraintData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisAClass_Members),
	int(sizeof(hkLimitedHingeConstraintData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkLimitedHingeConstraintData::ConstraintBasisB
//
static const hkInternalClassMember hkLimitedHingeConstraintData_ConstraintBasisBClass_Members[] =
{
	{ "pivot", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisB,m_pivot) },
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisB,m_axle) },
	{ "perp2FreeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData::ConstraintBasisB,m_perp2FreeAxis) }
};
const hkClass hkLimitedHingeConstraintDataConstraintBasisBClass(
	"hkLimitedHingeConstraintDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkLimitedHingeConstraintData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintData_ConstraintBasisBClass_Members),
	int(sizeof(hkLimitedHingeConstraintData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkLimitedHingeConstraintDataClass_Members[] =
{
	{ "minAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_minAngle) },
	{ "maxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_maxAngle) },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_maxFrictionTorque) },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_angularLimitsTauFactor) },
	{ "basisA", &hkLimitedHingeConstraintDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_basisA) },
	{ "basisB", &hkLimitedHingeConstraintDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkLimitedHingeConstraintData,m_basisB) }
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
extern const hkClass hkConstraintDataClass;

extern const hkClass hkLimitedHingeConstraintDataClass;
const hkClass hkLimitedHingeConstraintDataClass(
	"hkLimitedHingeConstraintData",
	&hkConstraintDataClass, // parent
	sizeof(hkLimitedHingeConstraintData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkLimitedHingeConstraintDataClass_Members),
	int(sizeof(hkLimitedHingeConstraintDataClass_Members)/sizeof(hkInternalClassMember)),
	&hkLimitedHingeConstraintData_Default
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
