/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/chain/hingelimits/hkHingeLimitsData.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/chain/hingelimits/hkHingeLimitsData.h>


// External pointer and enum types
extern const hkClass hkHingeLimitsDataConstraintBasisAClass;
extern const hkClass hkHingeLimitsDataConstraintBasisBClass;

//
// Class hkHingeLimitsData
//

//
// Class hkHingeLimitsData::ConstraintBasisA
//
static const hkInternalClassMember hkHingeLimitsData_ConstraintBasisAClass_Members[] =
{
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData::ConstraintBasisA,m_axle) },
	{ "perpToAxle1", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData::ConstraintBasisA,m_perpToAxle1) },
	{ "perpToAxle2", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData::ConstraintBasisA,m_perpToAxle2) }
};
const hkClass hkHingeLimitsDataConstraintBasisAClass(
	"hkHingeLimitsDataConstraintBasisA",
	HK_NULL, // parent
	sizeof(hkHingeLimitsData::ConstraintBasisA),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsData_ConstraintBasisAClass_Members),
	int(sizeof(hkHingeLimitsData_ConstraintBasisAClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkHingeLimitsData::ConstraintBasisB
//
static const hkInternalClassMember hkHingeLimitsData_ConstraintBasisBClass_Members[] =
{
	{ "axle", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData::ConstraintBasisB,m_axle) },
	{ "perp2FreeAxis", HK_NULL, HK_NULL, hkClassMember::TYPE_VECTOR4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData::ConstraintBasisB,m_perp2FreeAxis) }
};
const hkClass hkHingeLimitsDataConstraintBasisBClass(
	"hkHingeLimitsDataConstraintBasisB",
	HK_NULL, // parent
	sizeof(hkHingeLimitsData::ConstraintBasisB),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsData_ConstraintBasisBClass_Members),
	int(sizeof(hkHingeLimitsData_ConstraintBasisBClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);
static const hkInternalClassMember hkHingeLimitsDataClass_Members[] =
{
	{ "minAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_minAngle) },
	{ "maxAngle", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_maxAngle) },
	{ "maxFrictionTorque", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_maxFrictionTorque) },
	{ "angularLimitsTauFactor", HK_NULL, HK_NULL, hkClassMember::TYPE_REAL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_angularLimitsTauFactor) },
	{ "basisA", &hkHingeLimitsDataConstraintBasisAClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_basisA) },
	{ "basisB", &hkHingeLimitsDataConstraintBasisBClass, HK_NULL, hkClassMember::TYPE_STRUCT, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkHingeLimitsData,m_basisB) }
};
extern const hkClass hkConstraintDataClass;

extern const hkClass hkHingeLimitsDataClass;
const hkClass hkHingeLimitsDataClass(
	"hkHingeLimitsData",
	&hkConstraintDataClass, // parent
	sizeof(hkHingeLimitsData),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkHingeLimitsDataClass_Members),
	int(sizeof(hkHingeLimitsDataClass_Members)/sizeof(hkInternalClassMember)),
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
