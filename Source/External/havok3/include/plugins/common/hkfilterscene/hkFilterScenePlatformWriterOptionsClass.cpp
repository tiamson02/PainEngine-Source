/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkfilterscene/hkFilterScenePlatformWriterOptions.h'

#include <hkfilterscene/hkFilterScene.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkfilterscene/hkFilterScenePlatformWriterOptions.h>


//
// Class hkFilterScenePlatformWriterOptions
//

//
// Enum hkFilterScenePlatformWriterOptions::Preset
//
static const hkInternalClassEnumItem hkFilterScenePlatformWriterOptionsPresetEnumItems[] =
{
	{0, "CUSTOM"},
	{1, "MSVC_WIN32"},
	{2, "MSVC_XBOX"},
	{3, "MSVC_AMD64"},
	{4, "CW_PS2"},
	{5, "CW_GAMECUBE"},
	{6, "GCC33_PS2"},
	{7, "GCC32_PS2"},
	{8, "GCC295_PS2"},
	{9, "SN31_PS2"},
	{10, "SN393_GAMECUBE"},
	{11, "SNC_PSP"},
	{12, "GCC151_PSP"},
	{13, "X360"},
	{14, "GCC_PS3"},
	{15, "XML"},
};
static const hkInternalClassEnum hkFilterScenePlatformWriterOptionsEnums[] = {
	{"Preset", hkFilterScenePlatformWriterOptionsPresetEnumItems, 16 }
};
extern const hkClassEnum* hkFilterScenePlatformWriterOptionsPresetEnum = reinterpret_cast<const hkClassEnum*>(&hkFilterScenePlatformWriterOptionsEnums[0]);
static const hkInternalClassMember hkFilterScenePlatformWriterOptionsClass_Members[] =
{
	{ "filename", HK_NULL, HK_NULL, hkClassMember::TYPE_POINTER, hkClassMember::TYPE_CHAR, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_filename) },
	{ "preset", HK_NULL, hkFilterScenePlatformWriterOptionsPresetEnum, hkClassMember::TYPE_ENUM, hkClassMember::TYPE_VOID, 0, hkClassMember::ENUM_8, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_preset) },
	{ "bytesInPointer", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_bytesInPointer) },
	{ "littleEndian", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_littleEndian) },
	{ "reusePaddingOptimized", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_reusePaddingOptimized) },
	{ "emptyBaseClassOptimized", HK_NULL, HK_NULL, hkClassMember::TYPE_INT8, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_emptyBaseClassOptimized) },
	{ "removeMetadata", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_removeMetadata) },
	{ "userTag", HK_NULL, HK_NULL, hkClassMember::TYPE_UINT32, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterScenePlatformWriterOptions,m_userTag) }
};
namespace
{
	struct hkFilterScenePlatformWriterOptions_DefaultStruct
	{
		int s_defaultOffsets[8];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		hkInt8 /* enum Preset */ m_preset;
		hkInt8 m_bytesInPointer;
		hkInt8 m_littleEndian;
		hkInt8 m_reusePaddingOptimized;
		hkInt8 m_emptyBaseClassOptimized;
		_hkBool m_removeMetadata;
		hkUint32 m_userTag;
	};
	const hkFilterScenePlatformWriterOptions_DefaultStruct hkFilterScenePlatformWriterOptions_Default =
	{
		{-1,HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_preset),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_bytesInPointer),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_littleEndian),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_reusePaddingOptimized),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_emptyBaseClassOptimized),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_removeMetadata),HK_OFFSET_OF(hkFilterScenePlatformWriterOptions_DefaultStruct,m_userTag)},
		hkFilterScenePlatformWriterOptions::MSVC_WIN32,4,1,0,1,true,0
	};
}
extern const hkClass hkFilterScenePlatformWriterOptionsClass;
const hkClass hkFilterScenePlatformWriterOptionsClass(
	"hkFilterScenePlatformWriterOptions",
	HK_NULL, // parent
	sizeof(hkFilterScenePlatformWriterOptions),
	HK_NULL,
	0, // interfaces
	reinterpret_cast<const hkClassEnum*>(hkFilterScenePlatformWriterOptionsEnums),
	1, // enums
	reinterpret_cast<const hkClassMember*>(hkFilterScenePlatformWriterOptionsClass_Members),
	int(sizeof(hkFilterScenePlatformWriterOptionsClass_Members)/sizeof(hkInternalClassMember)),
	&hkFilterScenePlatformWriterOptions_Default
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
