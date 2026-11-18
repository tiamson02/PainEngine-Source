/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SCENE_EXPORT_ATTRIBUTES_H
#define HK_SCENE_EXPORT_ATTRIBUTES_H

#include <hkserialize/packfile/xml/hkXmlPackfileReader.h>

#include <hkmath/hkMath.h>

#include <hkscenedata/graph/hkxNode.h>

#include <hkscenedata/attributes/hkxAttribute.h>


class hkSceneExportAttributes
{
public:

		/// The havokPath is the absolute path to the Havok directory where the 
		/// attribute description file (hkSceneExportAttributes.xml) can be found, 
		/// usually the same location as the filter mananger. 
		/// Load will clear any already loaded data.
	hkBool init ( class hkFilterMemoryTracker* memoryTracker);
	
		/// Loads all attribute descriptions in the given path.
	hkBool loadAttributeDescriptions(const char* attributeDescriptionPath );	

		/// Given an attribute group, it processes the attributes inside it,
		/// following the instructions in the attribute description 
	void processAttributeGroup (hkxAttributeGroup& attrGroup);

		/// Given two attribute groups, merges them into a third one
	void mergeTwoAttributeGroups(const hkxAttributeGroup& groupOne, const hkxAttributeGroup& groupTwo, hkxAttributeGroup& mergedGroup);

protected:

	hkArray<struct hkAttributeDescriptionDatabase*> m_databases;
	hkXmlPackfileReader m_packfileReader;

	// To keep track of what we allocate.
	hkFilterMemoryTracker* m_trackedMemory; 

private:

	struct hkAttributeGroupDescription* searchForAttributeGroup(const char* group, struct hkAttributeGroupDescription* previousGroup);
	
	void mergeXYZToVector (hkArray<hkxAttribute>& attributes);
	void mergeTransAndRotToMatrix (hkArray<hkxAttribute>& attributes);
	void enforceTypes (hkArray<hkxAttribute>& attributes, const struct hkAttributeGroupDescription* attrGroupDesc);
	void enforceEnabledBy (hkArray<hkxAttribute>& attributes, const struct hkAttributeGroupDescription* attrGroupDesc);
	void enforceHints (hkArray<hkxAttribute>& attributes, const struct hkAttributeGroupDescription* attrGroupDesc);

	// Supported attribute conversions
	struct hkxAnimatedQuaternion* convertEulerToQuaternion (struct hkxAnimatedVector* animatedVector);
	struct hkxAnimatedQuaternion* convertMatrixToQuaternion (struct hkxAnimatedMatrix* animatedMatrix);
	struct hkxSparselyAnimatedEnum* convertIntToEnum (struct hkxSparselyAnimatedInt* animatedInt, const class hkClassEnum* enumClass);
	struct hkxSparselyAnimatedString* convertIntToString (struct hkxSparselyAnimatedInt* animatedInt, const class hkClassEnum* enumClass);
	struct hkxSparselyAnimatedString* convertEnumToString (struct hkxSparselyAnimatedEnum* animatedEnum);

};

#endif //HK_SCENE_EXPORT_ATTRIBUTES_H

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
