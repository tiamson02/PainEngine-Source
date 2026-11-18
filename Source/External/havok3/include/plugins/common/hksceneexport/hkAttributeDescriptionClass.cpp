/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hksceneexport/hkAttributeDescription.h'

#include <hksceneexport/hkSceneExport.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hksceneexport/hkAttributeDescription.h>


// External pointer and enum types
extern const hkClass hkAttributeDescriptionClass;
extern const hkClass hkAttributeGroupDescriptionClass;
extern const hkClass hkClassEnumClass;

//
// Class hkAttributeDescription
//

//
// Enum hkAttributeDescription::ForcedType
//
static const hkInternalClassEnumItem hkAttributeDescriptionForcedTypeEnumItems[] =
{
	{0, "LEAVE"},
	{1, "FORCE_BOOL"},
	{2, "FORCE_INT"},
	{3, "FORCE_ENUM"},
	{4, "FORCE_FLOAT"},
	{5, "FORCE_STRING"},
	{6, "FORCE_VECTOR"},
	{7, "FORCE_MATRIX"},
	{8, "FORCE_QUATERNION"},
};

//
// Enum hkAttributeDescription::Hint
//
static const hkInternalClassEnumItem hkAttributeDescriptionHintEnumItems[] =
{
	{0, "HINT_NONE"},
	{1, "HINT_IGNORE"},
	{2, "HINT_TRANSFORM"},
	{4, "HINT_SCALE"},
	{6, "HINT_TRANSFORM_AND_SCALE"},
	{8, "HINT_FLIP"},
};
static const hkInternalClassEnum hkAttributeDescriptionEnums[] = {
	{"ForcedType", hkAttributeDescriptionForcedTypeEnumItems, 9 },
	{"Hint", hkAttributeDescriptionHintEnumItems, 6 }
};
extern const hkClassEnum* hkAttributeDescriptionForcedTypeEnum = reinterpret_cast<const hkClassEnum*>(&hkAttributeDescriptionEnums[0]);
extern const hkClassEnum* hkAttributeDescriptionHintEnum = reinterpret_cast<const hkClassEnum*>(&hkAttributeDescriptionEnums[1]);
static const hkInternalClassMember hkAttributeDescriptionClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, HK_OFFSET_OF(hkAttributeDescription,m_name) },
	{ "enabledBy", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, HK_OFFSET_OF(hkAttributeDescription,m_enabledBy) },
	{ "forcedType", HK_NULL, hkAttributeDescriptionForcedTypeEnum, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, HK_OFFSET_OF(hkAttributeDescription,m_forcedType) },
	{ "enum", &hkClassEnumClass, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkAttributeDescription,m_enum) },
	{ "hint", HK_NULL, hkAttributeDescriptionHintEnum, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, HK_OFFSET_OF(hkAttributeDescription,m_hint) }
};
const hkClass hkAttributeDescriptionClass(
	"hkAttributeDescription",
	HK_NULL, // parent
	sizeof(hkAttributeDescription),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkAttributeDescriptionEnums),
	2, // enums
	reinterpret_cast<const hkClassMember*>(hkAttributeDescriptionClass_Members),
	int(sizeof(hkAttributeDescriptionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkAttributeGroupDescription
//
static const hkInternalClassMember hkAttributeGroupDescriptionClass_Members[] =
{
	{ "name", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, HK_OFFSET_OF(hkAttributeGroupDescription,m_name) },
	{ "attributeDescriptions", &hkAttributeDescriptionClass, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkAttributeGroupDescription,m_attributeDescriptions) }
};
const hkClass hkAttributeGroupDescriptionClass(
	"hkAttributeGroupDescription",
	HK_NULL, // parent
	sizeof(hkAttributeGroupDescription),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAttributeGroupDescriptionClass_Members),
	int(sizeof(hkAttributeGroupDescriptionClass_Members)/sizeof(hkInternalClassMember)),
	HK_NULL // defaults
	);

//
// Class hkAttributeDescriptionDatabase
//
static const hkInternalClassMember hkAttributeDescriptionDatabaseClass_Members[] =
{
	{ "groupDescriptions", &hkAttributeGroupDescriptionClass, HK_NULL, hkClassMember::TYPE_SIMPLEARRAY, hkClassMember::TYPE_STRUCT, 0, 0, HK_OFFSET_OF(hkAttributeDescriptionDatabase,m_groupDescriptions) }
};
extern const hkClass hkAttributeDescriptionDatabaseClass;
const hkClass hkAttributeDescriptionDatabaseClass(
	"hkAttributeDescriptionDatabase",
	HK_NULL, // parent
	sizeof(hkAttributeDescriptionDatabase),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkAttributeDescriptionDatabaseClass_Members),
	int(sizeof(hkAttributeDescriptionDatabaseClass_Members)/sizeof(hkInternalClassMember)),
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
