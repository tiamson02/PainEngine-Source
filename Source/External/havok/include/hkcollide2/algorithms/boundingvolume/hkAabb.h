/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_AABB_H
#define HK_COLLIDE2_AABB_H

#include <hkmath2/hkMath.h>

/// An axis aligned bounding box.
class hkAabb
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkAabb);

		hkAabb() { }

			///Creates a new AABB with the specified dimensions.
		hkAabb(const hkVector4& min, const hkVector4& max);
		
			/// Returns true if the given AABB overlaps with this one.
		inline bool overlaps( const hkAabb& testAabb ) const;

			/// Is this a valid aabb? I.e. no NaNs and min[i] <= max[i]
		bool isValid() const;

			/// Return true if 'other' is enclosed in this aabb.
			/// Boundaries are inclusive.
		bool contains(const hkAabb& other) const;

	public:

		hkVector4 m_min;
		hkVector4 m_max;
};

#include <hkcollide2/algorithms/boundingvolume/hkAabb.inl>

#endif // HK_COLLIDE2_AABB_H

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
