/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'hkfilterscene/hkFilterSceneAlterMeshOptions.h'

#include <hkfilterscene/hkFilterScene.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkInternalClassMember.h>
#include <hkfilterscene/hkFilterSceneAlterMeshOptions.h>


//
// Class hkFilterSceneAlterMeshOptions
//
static const hkInternalClassMember hkFilterSceneAlterMeshOptionsClass_Members[] =
{
	{ "removeIndices", HK_NULL, HK_NULL, hkClassMember::TYPE_BOOL, hkClassMember::TYPE_VOID, 0, 0, HK_OFFSET_OF(hkFilterSceneAlterMeshOptions,m_removeIndices) }
};
namespace
{
	struct hkFilterSceneAlterMeshOptions_DefaultStruct
	{
		int s_defaultOffsets[1];
		typedef hkInt8 _hkBool;
		typedef hkReal _hkVector4[4];
		typedef hkReal _hkQuaternion[4];
		typedef hkReal _hkMatrix3[12];
		typedef hkReal _hkRotation[12];
		typedef hkReal _hkQsTransform[12];
		typedef hkReal _hkMatrix4[16];
		typedef hkReal _hkTransform[16];
		_hkBool m_removeIndices;
	};
	const hkFilterSceneAlterMeshOptions_DefaultStruct hkFilterSceneAlterMeshOptions_Default =
	{
		{HK_OFFSET_OF(hkFilterSceneAlterMeshOptions_DefaultStruct,m_removeIndices)},
		true
	};
}
extern const hkClass hkFilterSceneAlterMeshOptionsClass;
const hkClass hkFilterSceneAlterMeshOptionsClass(
	"hkFilterSceneAlterMeshOptions",
	HK_NULL, // parent
	sizeof(hkFilterSceneAlterMeshOptions),
	HK_NULL,
	0, // interfaces
	HK_NULL,
	0, // enums
	reinterpret_cast<const hkClassMember*>(hkFilterSceneAlterMeshOptionsClass_Members),
	int(sizeof(hkFilterSceneAlterMeshOptionsClass_Members)/sizeof(hkInternalClassMember)),
	&hkFilterSceneAlterMeshOptions_Default
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
