/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkAabb::hkAabb(const hkVector4& min, const hkVector4& max)
	: m_min(min), m_max(max)
{
}

hkBool hkAabb::overlaps( const hkAabb& testAabb ) const
{
	HK_ASSERT2(0x3f5578fc,  isValid(), "Invalid aabb in hkAabb::overlaps." );
	HK_ASSERT2(0x76dd800a,  testAabb.isValid(), "Invalid aabb in hkAabb::overlaps.");
	// must be careful about w components
#define XYZ_LESS(a,b) ((a.compareLessThanEqual4(b) & HK_VECTOR3_COMPARE_MASK_XYZ) == HK_VECTOR3_COMPARE_MASK_XYZ)
	return XYZ_LESS(m_min, testAabb.m_max) && XYZ_LESS(testAabb.m_min, m_max);
#undef XYZ_LESS
}

hkBool hkAabb::contains(const hkAabb& other) const
{
	int mincomp = m_min.compareLessThanEqual4(other.m_min);
	int maxcomp = other.m_max.compareLessThanEqual4(m_max);
	return ((mincomp & maxcomp & HK_VECTOR3_COMPARE_MASK_XYZ) == HK_VECTOR3_COMPARE_MASK_XYZ);
}

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
