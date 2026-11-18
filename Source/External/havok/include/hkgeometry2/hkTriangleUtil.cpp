/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkgeometry2/hkTriangleUtil.h>

/// Returns true if the given ray r=pos+t*dir intersects the triangle  Returns t,
/// and u,v (planar coordinates  on the triangle).
/// If perform_culling=true, back-face culling is performed, i.e. if t gets negative, the
/// method returns false.
///
/// \param t               (output) distance to triangle if hit.
/// \param u               (output) barycentric coordinate on triangle if hit.
/// \param v               (output) barycentric coordinate on triangle if hit.
/// \param pos             (input) origin of ray.
/// \param dir             (input) direction of ray.
/// \param a               First vertex.
/// \param b               Second vertex.
/// \param c               Third vertex.
/// \param perform_culling If true backface culling will be performed.
/// 
bool hkTriangleUtil::rayIntersect( hkReal &t, hkReal &u, hkReal &v, const hkVector4 &pos, const hkVector4 &dir,
										const hkVector4& a, const hkVector4& b, const hkVector4& c, bool perform_culling )
{

	/* find vectors for two edges sharing vert0 */

	hkVector4 edge1; edge1.setSub4(b,a);
	hkVector4 edge2; edge2.setSub4(c,a);

	/* begin calculating determinant - also used to calculate U parameter */
	hkVector4 pvec;	pvec.setCross(dir,edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	const hkReal det = edge1.dot3(pvec);

	/* Negatives values are valid if culling is off. Zero is invalid always */
	// if (perform_culling ? (det < 0.0001f) : ((det>-0.0001f)&&(det<0.0001f)))  return false;
	// (line rewritten since it caused a compiler error in MWERKS for DC
	if (perform_culling)
	{
		if (det<0.0001f) return false;
	}
	else
	{
		if ((det>-0.0001f)&&(det<0.0001f)) return false;
	}


	/* Now we can safely calculate this */
	const hkReal inv_det = 1.0f / det;

	/* calculate distance from vert0 to ray origin */
	hkVector4 tvec; tvec.setSub4(pos, a);

	/* calculate U parameter and test bounds */
	u = tvec.dot3(pvec);

	if (u < 0.0f || u > det)
	{
	   return false;
	}

	/* prepare to test V parameter */
	hkVector4 qvec;	qvec.setCross(tvec,edge1);

	/* calculate V parameter and test bounds */
	v = dir.dot3(qvec);

	if (v < 0.0f || (u + v) > det)
	{
	   return false;
	}

	/* calculate t */
	t = edge2.dot3(qvec);

	/* scale parameters */
	t *= inv_det;
	u *= inv_det;
	v *= inv_det;

	/* Ray intersects triangle  */
	return true;
}

// Returns true if the triangle is non-degenerate. Non-degenerate is assumed to be: it has non-zero area and every edge has non-zero squarelength (using tolerance for both zero checks)
/// \param a               First vertex.
/// \param b               Second vertex.
/// \param c               Third vertex.
/// \param tolerance	   Minimal acceptible area and squared edge length
///
bool hkTriangleUtil::isNonDegenerate(const hkVector4& a, const hkVector4& b, const hkVector4& c , hkReal tolerance)
{
	hkVector4	ab;
	hkVector4 ac;
	hkVector4 bc;
	ab.setSub4( b , a );
	ac.setSub4( c , a );
	bc.setSub4( c , b );
	
	bool edgesOk = false;
	bool areaOk = false;

	if ((hkReal)( ab.lengthSquared3() ) > tolerance &&
		(hkReal)( ac.lengthSquared3() ) > tolerance &&
		(hkReal)( bc.lengthSquared3() ) > tolerance)
	{
		edgesOk = true;

		hkVector4	area;
		area.setCross(ab,ac);
		areaOk = ( (hkReal)(area.length3()) > tolerance);
	}

	return (edgesOk & areaOk);
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
