/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HAVOK_FILTER_REGISTRY__H
#define HAVOK_FILTER_REGISTRY__H

class hkTypeInfo;
class hkClass;
class hkVtableClassRegistry;
class hkFinishLoadedObjectRegistry;
class hkClassNameRegistry;

/// Class registry interface used by export filters.
class hkFilterClassRegistry
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_SERIALIZE, hkFilterClassRegistry);

		hkFilterClassRegistry();

		~hkFilterClassRegistry();

		const hkVtableClassRegistry& getVtableRegistry() const;
		hkVtableClassRegistry& getVtableRegistry();

		const hkFinishLoadedObjectRegistry& getFinishObjectRegistry() const;
		hkFinishLoadedObjectRegistry& getFinishObjectRegistry();

		const hkClassNameRegistry& getClassNameRegistry() const; 
		hkClassNameRegistry& getClassNameRegistry(); 

		const hkClass* getClassFromVirtualInstance(void*) const;

		void registerClass(const hkTypeInfo* ti, const hkClass* ci);

		void registerClasses(const hkTypeInfo* const * tis, const hkClass* const * cis);

		void copy( const hkFilterClassRegistry& copyFrom );

		struct Implementation;

	protected:

		Implementation* m_impl;
};

#endif // HAVOK_FILTER_REGISTRY__H

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
