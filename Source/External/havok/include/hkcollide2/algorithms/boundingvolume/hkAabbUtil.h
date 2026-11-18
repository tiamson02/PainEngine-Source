/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_AABB_UTIL_H
#define HK_COLLIDE2_AABB_UTIL_H

class hkAabb;
class hkTransform;
class hkVector4;


/// A utility class for creating AABBs that contain various geometric objects.
class hkAabbUtil
{
	public:

			/// Calculates an AABB from an array of vertices.
		static void HK_CALL calcAabb( const float* vertexArray, int numVertices, int striding, hkAabb& aabbOut );

			/// Calculates an AABB from an array of vertices, given a transform.
		static void HK_CALL calcAabb( const hkTransform& BvToWorld, const float* vertexArray, int numVertices, int striding, hkAabb& aabbOut );

			/// Calculates an AABB from an OBB specified by a transform, a center, and an halfExtents vector.
		static void HK_CALL calcAabb( const hkTransform& BvToWorld, const hkVector4& halfExtents, const hkVector4& center, hkAabb& aabbOut);

			/// Calculates an AABB from an OBB specified by a transform, a center, and an halfExtents vector and an extra radius
		static inline void HK_CALL calcAabb( const hkTransform& BvToWorld, const hkVector4& halfExtents, const hkVector4& center, float extraRadius, hkAabb& aabbOut);

			/// Calculates an AABB from an OBB specified by a transform, and an halfExtents vector.
		static inline void HK_CALL calcAabb( const hkTransform& BvToWorld, const hkVector4& halfExtents, float extraRadius, hkAabb& aabbOut);

};

#include <hkcollide2/algorithms/boundingvolume/hkAabbUtil.inl>

#endif // HK_COLLIDE2_AABB_UTIL_H

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
