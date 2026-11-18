/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkAabb::hkAabb(const hkVector4& min, const hkVector4& max)
	: m_min(min), m_max(max)
{
}

inline bool hkAabb::overlaps( const hkAabb& testAabb ) const
{
	HK_ASSERT2( isValid(), "Invalid aabb in hkAabb::overlaps." );
	HK_ASSERT2( testAabb.isValid(), "Invalid aabb in hkAabb::overlaps.");
	// must be careful about w components
#define XYZ_LESS(a,b) ((a.compareLessThanEqual4(b) & HK_VECTOR3_COMPARE_MASK_XYZ) == HK_VECTOR3_COMPARE_MASK_XYZ)
	return XYZ_LESS(m_min, testAabb.m_max) && XYZ_LESS(testAabb.m_min, m_max);
#undef XYZ_LESS
}


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
