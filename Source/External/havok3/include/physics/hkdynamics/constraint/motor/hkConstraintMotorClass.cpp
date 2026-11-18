/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkdynamics/constraint/motor/hkConstraintMotor.h'

#include <hkdynamics/hkDynamics.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkdynamics/constraint/motor/hkConstraintMotor.h>


//
// Class hkConstraintMotor
//

//
// Enum hkConstraintMotor::Type
//
static const hkInternalClassEnumItem hkConstraintMotorTypeEnumItems[] =
{
	{0, "TYPE_INVALID"},
	{1, "TYPE_POSITION"},
	{2, "TYPE_VELOCITY"},
	{3, "TYPE_SPRING_DAMPER"},
	{4, "TYPE_MAX"},
};
static const hkInternalClassEnum hkConstraintMotorEnums[] = {
	{"Type", hkConstraintMotorTypeEnumItems, 5 }
};
extern const hkClassEnum* hkConstraintMotorTypeEnum = reinterpret_cast<const hkClassEnum*>(&hkConstraintMotorEnums[0]);
extern const hkClass hkReferencedObjectClass;

extern const hkClass hkConstraintMotorClass;
const hkClass hkConstraintMotorClass(
	"hkConstraintMotor",
	&hkReferencedObjectClass, // parent
	sizeof(hkConstraintMotor),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkConstraintMotorEnums),
	1, // enums
	HK_NULL,
	0,
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
