/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayPlane.h>


hkDisplayPlane::hkDisplayPlane(const hkVector4& normal, const hkVector4& perpToNormal, 
							   const hkVector4& center, hkReal extent)
:	hkDisplayGeometry(HK_DISPLAY_PLANE),
	m_normal(normal),
	m_center(center),
	m_perpToNormal(perpToNormal),
	m_extent(extent)
{

}

hkDisplayPlane::hkDisplayPlane()
:	hkDisplayGeometry(HK_DISPLAY_PLANE) 
{
	m_extent = 0.0f;
	m_normal.setZero4();
	m_center.setZero4();
	m_perpToNormal.setZero4();
}

void hkDisplayPlane::createPlanePoints(hkVector4* points)
{
	hkVector4 otherDir;
	otherDir.setCross(m_normal, m_perpToNormal);
	
	for(int i = 0; i < 4; i++)
	{
		points[i] = m_center;
	}

	hkVector4 offset1;
	hkVector4 offset2;
	
	offset1.setMul4(m_extent, otherDir);
	offset2.setMul4(m_extent, m_perpToNormal);
	
	points[0].add4(offset1);
	points[0].add4(offset2);

	points[1].add4(offset1);
	points[1].sub4(offset2);

	points[2].sub4(offset1);
	points[2].add4(offset2);

	points[3].sub4(offset1);
	points[3].sub4(offset2);
}

hkVector4& hkDisplayPlane::getNormal()
{
	return m_normal;
}

hkVector4& hkDisplayPlane::getCenter()
{
	return m_center;
}

hkVector4& hkDisplayPlane::getPerpToNormal()
{
	return m_perpToNormal;
}

hkReal& hkDisplayPlane::getExtent()
{
	return m_extent;
}


void hkDisplayPlane::setParameters(const hkVector4& normal, const hkVector4& perpToNormal, 
								   const hkVector4& center, hkReal extent)
{
	m_normal = normal;
	m_center = center;
	m_perpToNormal = perpToNormal;
	m_extent = extent;
}

void hkDisplayPlane::buildGeometry()
{
	// build triangle hkGeometry

	m_geometry = new hkGeometry();

	hkVector4 points[5];
	points[4] = m_center;
	createPlanePoints(points);
	m_geometry->addVertices(points, 5);
	
	hkArray<hkTriangleIndices> triangleIndices;
	triangleIndices.setSize(4);

	triangleIndices[0].setTriangleIndices(2,4,3);
	triangleIndices[1].setTriangleIndices(0,4,2);
	triangleIndices[2].setTriangleIndices(1,4,0);
	triangleIndices[3].setTriangleIndices(3,4,1);

	m_geometry->addTriangles(&triangleIndices[0], 4);	
}


void hkDisplayPlane::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	lines.setSize(12);
	
	hkVector4 points[4];
	
	createPlanePoints(points);

	lines[0] = points[0];
	lines[1] = points[1];

	lines[2] = points[1];
	lines[3] = points[2];

	lines[4] = points[2];
	lines[5] = points[3];

	lines[6] = points[3];
	lines[7] = points[0];

	lines[8] = points[0];
	lines[9] = points[2];
	
	lines[10] = points[1];
	lines[11] = points[3];
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
