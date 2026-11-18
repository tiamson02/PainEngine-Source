/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKOBJECTMAP_H
#define HKBASE_HKOBJECTMAP_H

#include <hkbase/hkBase.h>
#include <hkbase/htl/hkObjectArray.h>

template <typename F, typename S>
struct hkObjectPair
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_ARRAY, hkObjectPair );

	F first;
	S second;

	hkObjectPair() : first(), second() {}
};


/// The Havok object map.<br>
///	<br>
/// The following must be publicly accessible for each item in an object map:
/// <ul><li>default constructor, copy constructor, assignment, destructor
/// (either hand or compiler generated)</li>
/// <li>placement new, needed to construct objects in place. (You may need
///	to use HK_DECLARE_CLASS_ALLOCATOR or HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR
/// for structs, nested classes, etc)</li></ul>
template <typename K, typename V>
class hkObjectMap
{
public:

	typedef hkObjectMap<K,V> foo;
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_ARRAY, foo );

	hkObjectMap() { }

		/// Gets the value with the key k. If the key is not in the map, it is added and the returned reference can be used to add a
		/// corresponding value.
	V& operator[] (const K& k);

		///Finds the value with the key k.
	const V* find(const K& k) const;

		///Erases the key-value pair with the key k.
	int erase(const K& k);

public:

		///The map's pairs.
	hkObjectArray< hkObjectPair<K,V> > m_storage;
};

#include <hkbase/htl/hkObjectMap.inl>

#endif // HKBASE_HKOBJECTMAP_H

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
