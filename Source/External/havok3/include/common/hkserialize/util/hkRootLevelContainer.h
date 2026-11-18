/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_ROOT_CONTAINER_H
#define HK_ROOT_CONTAINER_H

#include <hkbase/hkBase.h>

extern const class hkClass hkRootLevelContainerClass;

/// This is the root level class exported by our tools.
/// It contains an array of Variants and associated names.
class hkRootLevelContainer
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SERIALIZE, hkRootLevelContainer);

		HK_DECLARE_REFLECTION();

	public:

		class  NamedVariant
		{
			HK_DECLARE_REFLECTION();

			public:
				char*     m_name;
				hkVariant m_variant;
		};

			/// Iterates over the objects held in the container returning a pointer
			/// to the first object after prevObject, with a type corresponding to 'typeName'.
			/// If prevObject is null then the search begins from the start of the container.
		void* findObjectByType( const char* typeName, const void* prevObject = HK_NULL ) const;

			/// Iterates over the objects held in the container returning a pointer
			/// to the first object after prevObject, with a name corresponding to 'objectName'.
			/// If prevObject is null then the search begins from the start of the container.
		void* findObjectByName( const char* objectName, const void* prevObject = HK_NULL ) const;

	public:

		class NamedVariant* m_namedVariants;
		int m_numNamedVariants;
};

#endif //HK_ROOT_CONTAINER_H

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
