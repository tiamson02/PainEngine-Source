/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKSCENEEXPORT_ATTRIBUTE_DESCRIPTION__H
#define HKSCENEEXPORT_ATTRIBUTE_DESCRIPTION__H

extern const hkClass hkAttributeDescriptionClass;
extern const hkClass hkAttributeGroupDescriptionClass;
extern const hkClass hkAttributeDescriptionDatabaseClass;

	// An attribute description object contains information about an attribute that can be used by
	// an exporter to complement the data given by the modeller regarding that attribute. This is useful
	// in order to support attribute types and hints not natively supported by the modeller.
struct hkAttributeDescription
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, hkAttributeDescription );
	HK_DECLARE_REFLECTION();

		// Name of the attribute we are describing
	const char* m_name;

		// If a boolean attribute with the given name is not set to true, then this attribute is ignored
	const char* m_enabledBy;

		// Sometimes the modeller may use a type that doesn't match the expected final type.
		// We can force a conversion
	enum ForcedType
	{
		LEAVE,
		FORCE_BOOL,
		FORCE_INT,
		FORCE_ENUM,
		FORCE_FLOAT,
		FORCE_STRING,
		FORCE_VECTOR,
		FORCE_MATRIX,
		FORCE_QUATERNION,
	};

		// (Possibly) force this attribute to be of the given type
	enum ForcedType m_forcedType;

		// For attributes with FORCE_ENUM; if so we would like to know the string values.
	class hkClassEnum* m_enum;

		// There should be a one to one mapping between this enum and the hkxattrib enum (serialization
		// doesn't like referencing it from here)
	enum Hint 
	{
		HINT_NONE = 0, // no hint
		HINT_IGNORE = 1, // ignore this attribute (usually not seen after export)
		HINT_TRANSFORM = 2, // Can apply transforms to the attribute (it is a direction or postion)
		HINT_SCALE = 4, // Can apply scale to the attribute (it is measured in world units perhaps)
		HINT_TRANSFORM_AND_SCALE= 6, // Can scale and transform the attribute
		HINT_FLIP = 8 // It should be flipped (negated for floats) upon handedness flip (used for angles)
	};

		// Want to know if it has a special meaning or usage
	enum Hint m_hint;
};

	// AttributeDescriptions are grouped in AttributeGroupDescriptions, just as Attributes are grouped in AttributeGroups
struct hkAttributeGroupDescription
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, hkAttributeGroupDescription );
	HK_DECLARE_REFLECTION();

		// Finds an attribute description in this attribute group description
	hkAttributeDescription* findAttributeDescriptionByName (const char* name) const;

	const char* m_name;
	struct hkAttributeDescription* m_attributeDescriptions;
	int m_numAttributeDescriptions;
};

	// An AttributeDescriptionDatabase stores descriptions for all attributes and attribute groups
struct hkAttributeDescriptionDatabase
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_EXPORT, hkAttributeDescriptionDatabase );
	HK_DECLARE_REFLECTION();

		/// Search for a description for a group by name
		/// If found (not NULL) then use hkAttributeGroupDescription::findAttributeDescriptionByName
		/// to look for specific attribute data
	struct hkAttributeGroupDescription* findAttributeDescriptionGroupByName (const char* name) const;


	struct hkAttributeGroupDescription* m_groupDescriptions;
	int m_numGroupDescriptions;
};


#endif // HKSCENEEXPORT_ATTRIBUTE_DESCRIPTION__H

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
