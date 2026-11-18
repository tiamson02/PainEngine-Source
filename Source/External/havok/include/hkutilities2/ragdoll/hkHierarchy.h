/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKHIERARCHY_H
#define HKHIERARCHY_H


/// This small class stores the hierarchy used by an hkSkeleton.
/// The hierarchy is stored as an array of parent indices.
/// For performance each child index must be greater than that of its parent.
/// It is a referenced object as the same hierarchy may be used 
/// by multiple skeletons.
class hkHierarchy : public hkReferencedObject
{
	public:

		/// Construct a hierarchy with the specified structure.
		inline hkHierarchy( const hkArray<hkInt16>& parents );

		/// Get the number of parents for a specified child index.
		inline hkInt16 getDepth( hkInt16 i ) const;

		/// Get the parent index array
		inline const hkArray<hkInt16>& getParents() const;

	private:

		hkArray<hkInt16>	m_parents;
};

#include <hkutilities2/ragdoll/hkHierarchy.inl>

#endif
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
