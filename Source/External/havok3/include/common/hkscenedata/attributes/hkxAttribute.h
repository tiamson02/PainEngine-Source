/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKSCENEDATA_HKX_ATTRIBUTE__H
#define HKSCENEDATA_HKX_ATTRIBUTE__H

// An attribute is nothing more than a named variant, i.e., a variant (type+value) with a name
// Expected types are d
struct hkxAttribute
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkxAttribute );
	HK_DECLARE_REFLECTION();

		// These hints are used by different attribute types to define how they are affected by transforms
	enum Hint 
	{
		HINT_NONE = 0, // no hint
		HINT_IGNORE = 1, // ignore this attribute (usually not seen after export)
		HINT_TRANSFORM = 2, // Can apply transforms to the attribute (it is a direction or postion)
		HINT_SCALE = 4, // Can apply scale to the attribute (it is measured in world units perhaps)
		HINT_TRANSFORM_AND_SCALE= 6, // Can scale and transform the attribute
		HINT_FLIP = 8 // It should be flipped (negated for floats) upon handedness flip (used for angles)
	};

		/// Name of the attribute. 
	char* m_name; 
		
		/// Type (hkClass pointer) and pointer to object holding the 
		/// data. Depending on the object type it may be an animated
		/// attribute. 
		/// See <hkscenedata/attributes/hkxAttributes.h> for some 
		/// commonly used attributes used by the Havok exporters.
	hkVariant m_value;
};

#endif // HKSCENEDATA_HKX_ATTRIBUTE__H

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
