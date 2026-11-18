/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_TRIANGLEUTIL_H
#define HK_GEOMETRY2_TRIANGLEUTIL_H

/// Triangle utilities (ray intersection, normal calculation, point-in-triangle tests etc.) 
class hkTriangleUtil
{
	public:

	

		// Utilities...
			/// Method to calculate the (non-normalized) triangle normal.
		static inline void HK_CALL calcNormal( hkVector4& normal, const hkVector4& a, const hkVector4& b, const hkVector4& c );


			/// Method to calculate the triangle centroid.
		static inline void HK_CALL calcCentroid( hkVector4& centroid, const hkVector4& a, const hkVector4& b, const hkVector4& c );


			/// Returns true if the point is in front of the Triangle
		static inline bool HK_CALL inFront( const hkVector4& point, const hkVector4& a, const hkVector4& b, const hkVector4& c );


			/// Return true if the point is contained within the Triangle.
		static inline bool HK_CALL containsPoint( const hkVector4 &pt, const hkVector4& a, const hkVector4& b, const hkVector4& c );


			/// Returns the plane constant (d) for this Triangle
		static inline hkReal HK_CALL calcPlaneConstant( const hkVector4& a, const hkVector4& b, const hkVector4& c );


			/// Performs a ray-triangle intersection test
		static bool HK_CALL rayIntersect( hkReal &t, hkReal &u, hkReal &v, const hkVector4 &pos, const hkVector4 &dir,
								  const hkVector4& a, const hkVector4& b, const hkVector4& c, bool perform_culling=false );

			/// Returns true if the triangle is 'ok', 'ok' meaning that it has non-zero area and every edge has non-zero squarelength (using tolerance for both zero checks)
		static bool HK_CALL isNonDegenerate(const hkVector4& a, const hkVector4& b, const hkVector4& c , hkReal tolerance);

	protected:

		hkTriangleUtil();

};

#include <hkgeometry2/hkTriangleUtil.inl>

#endif // HK_GEOMETRY2_TRIANGLEUTIL_H

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
