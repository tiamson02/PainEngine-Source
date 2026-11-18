/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKMAP_H
#define HKBASE_HKMAP_H

#include <hkbase/hkBase.h>

/// A struct to hold a pair of elements.
template <typename F, typename S>
struct hkPair
{
	hkPair( const F& f, const S& s)
		: first(f), second(s)
	{
	}
	
	F first;
	S second;
};


/// The default havok map.<br>
///	<br>
/// This is <i>not</i> a complete replacement for the STL map, and should be used only with plain old data types
/// (or objects that have a trivial constructor, destructor,
/// copy constructor and assignment operator that are equivalent 
/// to memcopy), as constructors, destructors, etc. are not called.
/// If you want to create a map of objects, you should use hkObjectMap instead.
template <typename K, typename V>
class hkMap
{
	public:

		hkMap() { }

			/// Gets the value with the key k. If the key is not in the map, it is added and the returned reference can be used to add a
			/// corresponding value.
		V& operator[] (const K& k);

			///Finds the value with the key k.
		const V* find(const K& k) const;

			///Erases the key-value pair with the key k.
		int erase(const K& k);

	public:

			///The map's key-value pairs.
		hkArray< hkPair<K,V> > m_storage;
};

#include <hkbase/htl/hkMap.inl>

#endif // HKBASE_HKMAP_H

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
