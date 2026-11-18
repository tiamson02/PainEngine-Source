/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SCENEDATA_HKX_ANIMATED_STRING_H
#define HK_SCENEDATA_HKX_ANIMATED_STRING_H

extern const hkClass hkxSparselyAnimatedStringClass;
struct hkxSparselyAnimatedString
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_SCENE_DATA, hkxSparselyAnimatedString );
	HK_DECLARE_REFLECTION();

		/// The array of sampled floats. The array can be of any length but
		/// the time stamps in the m_times array must be of the same length
		/// as there is a one to one mapping between the two arrays.

		/// Hack so that the serialization will know it is really a cstring
		/// not a simple array to char*s. See HVK-2584
	struct StringType
	{
		HK_DECLARE_REFLECTION();

		char* m_string;

		char* operator=(char* str)
		{
			m_string = str; 
			return m_string;
		}
	};

	struct StringType* m_strings;
	int m_numStrings;
	float* m_times;
	int m_numTimes; 
};


#endif // HK_SCENEDATA_HKX_ANIMATED_STRING_H

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
