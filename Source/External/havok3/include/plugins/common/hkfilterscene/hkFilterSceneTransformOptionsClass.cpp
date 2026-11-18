/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkfilterscene/hkFilterSceneTransformOptions.h'

#include <hkfilterscene/hkFilterScene.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkfilterscene/hkFilterSceneTransformOptions.h>


//
// Class hkFilterSceneTransformOptions
//

//
// Enum hkFilterSceneTransformOptions::Preset
//
static const hkInternalClassEnumItem hkFilterSceneTransformOptionsPresetEnumItems[] =
{
	{0, "IDENTITY"},
	{1, "MIRROR_X"},
	{2, "MIRROR_Y"},
	{3, "MIRROR_Z"},
	{4, "SCALE_FEET_TO_METERS"},
	{5, "SCALE_INCHES_TO_METERS"},
	{6, "SCALE_CMS_TO_METERS"},
	{7, "CUSTOM"},
	{8, "PRESET_MAX_ID"},
};
static const hkInternalClassEnum hkFilterSceneTransformOptionsEnums[] = {
	{"Preset", hkFilterSceneTransformOptionsPresetEnumItems, 9 }
};
extern const hkClassEnum* hkFilterSceneTransformOptionsPresetEnum = reinterpret_cast<const hkClassEnum*>(&hkFilterSceneTransformOptionsEnums[0]);
static const hkInternalClassMember hkFilterSceneTransformOptionsClass_Members[] =
{
	{ "preset", HK_NULL, hkFilterSceneTransformOptionsPresetEnum, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_preset) },
	{ "applyToNodes", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_applyToNodes) },
	{ "applyToBuffers", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_applyToBuffers) },
	{ "applyToLights", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_applyToLights) },
	{ "applyToCameras", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_applyToCameras) },
	{ "flipWinding", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_flipWinding) },
	{ "matrix", HK_NULL, HK_NULL, hkClassMember::TYPE_MATRIX4, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneTransformOptions,m_matrix) }
};
namespace
{
	struct hkFilterSceneTransformOptions_DefaultStruct
	{
		int s_defaultOffsets[7];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		hkInt8 /* enum Preset */ m_preset;
		_hkBool m_applyToNodes;
		_hkBool m_applyToBuffers;
		_hkBool m_applyToLights;
		_hkBool m_applyToCameras;
		_hkBool m_flipWinding;
	};
	const hkFilterSceneTransformOptions_DefaultStruct hkFilterSceneTransformOptions_Default =
	{
		{HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_preset),HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_applyToNodes),HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_applyToBuffers),HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_applyToLights),HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_applyToCameras),HK_OFFSET_OF(hkFilterSceneTransformOptions_DefaultStruct,m_flipWinding),-1},
		hkFilterSceneTransformOptions::IDENTITY,true,true,true,true,false
	};
}
extern const hkClass hkFilterSceneTransformOptionsClass;
const hkClass hkFilterSceneTransformOptionsClass(
	"hkFilterSceneTransformOptions",
	HK_NULL, // parent
	sizeof(hkFilterSceneTransformOptions),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkFilterSceneTransformOptionsEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkFilterSceneTransformOptionsClass_Members),
	int(sizeof(hkFilterSceneTransformOptionsClass_Members)/sizeof(hkInternalClassMember)),
	&hkFilterSceneTransformOptions_Default
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
