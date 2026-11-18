/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// Given the vertices, set normal to be the non-unit normal.
/// \param normal   
/// \param a   
/// \param b   
/// \param c   
inline void HK_CALL hkTriangleUtil::calcNormal( hkVector4& normal, const hkVector4& a, const hkVector4& b, const hkVector4& c ) 
{
	hkVector4 cb;
	hkVector4 ab;
	cb.setSub4(c,b);
	ab.setSub4(a,b);

	normal.setCross( cb , ab);
}

/// Given the vertices list, set the centroid to be the centroid of the three 
/// vertices.
/// \param centroid   
/// \param a   
/// \param b   
/// \param c   
inline void HK_CALL hkTriangleUtil::calcCentroid( hkVector4& centroid, const hkVector4& a, const hkVector4& b, const hkVector4& c ) 
{
	centroid.setAdd4(a,b);
	centroid.add4(c);
	centroid.mul4(1.0f/3.0f);
}

/// Given the plane in which the triangle is embedded, the point is in front  iff 
/// the point is in the half space (defined by the plane) that the normal points 
/// into.
/// \param point   
/// \param a   
/// \param b   
/// \param c   
inline bool HK_CALL hkTriangleUtil::inFront( const hkVector4& point, const hkVector4& a, const hkVector4& b, const hkVector4& c ) 
{
	hkVector4 triangleNormal;
	calcNormal ( triangleNormal, a, b, c );
	triangleNormal.normalize3();

	hkVector4 d; d.setSub4(point, a);
	return 	d.dot3(triangleNormal) > 0;
}

/// Assumes that the point is on the plane containing the Triangle.
/// \param pt   
/// \param a   
/// \param b   
/// \param c   
inline bool HK_CALL hkTriangleUtil::containsPoint( const hkVector4 &pt, const hkVector4& a, const hkVector4& b, const hkVector4& c ) 
{
	hkVector4 triangleNormal;
	calcNormal ( triangleNormal, a, b, c );
	triangleNormal.normalize3();

	hkVector4 pta;
	pta.setSub4(pt,a);
	hkVector4 ba;
	ba.setSub4(b,a);
	pta.setCross(pta,ba);

	hkVector4 ptb;
	pta.setSub4(pt,b);
	hkVector4 cb;
	ba.setSub4(c,b);
	ptb.setCross(pta,cb);

	hkVector4 ptc;
	pta.setSub4(pt,c);
	hkVector4 ac;
	ba.setSub4(a,c);
	ptc.setCross(pta,ac);

	hkRotation simultaneousDots;

	//set column and then inverse mul
	//is actually faster than set row
	//and then mul
	simultaneousDots.setCols(pta,ptb,ptc);
	//pta now holds the dot products
	pta._setRotatedInverseDir(simultaneousDots,triangleNormal);
	
	// all dots less than zero means we are inside
	int mask = pta.compareLessThanZero4() & HK_VECTOR3_COMPARE_MASK_XYZ;
	return mask != HK_VECTOR3_COMPARE_MASK_XYZ;
}

/// The Triangle lies in the plane defined by x.N+d, where x is any point in the 
/// plane,  N is the normal to the plane and d is the distance from the plane to the 
/// origin (the planar distance)
/// \param a   
/// \param b   
/// \param c   
inline hkReal HK_CALL hkTriangleUtil::calcPlaneConstant( const hkVector4& a, const hkVector4& b, const hkVector4& c ) 
{
	hkVector4 n;
	calcNormal( n, a, b, c );
	n.normalize3();

	return n.dot3(a);
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
