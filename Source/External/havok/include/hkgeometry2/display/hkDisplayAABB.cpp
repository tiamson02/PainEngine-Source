/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayAABB.h>


hkDisplayAABB::hkDisplayAABB(const hkVector4& min, const hkVector4& max)
:	hkDisplayGeometry(HK_DISPLAY_AABB),
	m_minExtent(min),
	m_maxExtent(max)
{
}

hkDisplayAABB::hkDisplayAABB()
:	hkDisplayGeometry(HK_DISPLAY_AABB)
{
	m_minExtent.setZero4();
	m_maxExtent.setZero4();
}



void hkDisplayAABB::setExtents(const hkVector4& min, const hkVector4& max)
{
	m_minExtent = min;
	m_maxExtent = max;
}


void hkDisplayAABB::buildGeometry()
{
	// build triangle hkGeometry
	m_geometry = new hkGeometry();

	hkArray<hkVector4> verts;
	verts.setSize(8);

	verts[0].set(m_minExtent(0),m_minExtent(1),m_minExtent(2));
	verts[1].set(m_minExtent(0),m_minExtent(1),m_maxExtent(2));
	verts[2].set(m_maxExtent(0),m_minExtent(1),m_maxExtent(2));
	verts[3].set(m_maxExtent(0),m_minExtent(1),m_minExtent(2));
	verts[4].set(m_minExtent(0),m_maxExtent(1),m_minExtent(2));
	verts[5].set(m_minExtent(0),m_maxExtent(1),m_maxExtent(2));
	verts[6].set(m_maxExtent(0),m_maxExtent(1),m_maxExtent(2));
	verts[7].set(m_maxExtent(0),m_maxExtent(1),m_minExtent(2));	

	m_geometry->addVertices(&verts[0], verts.getSize());

	hkArray<hkTriangleIndices> tris;
	tris.setSize(12);
	// tris are CCW order -  is this right?
	tris[0].setTriangleIndices(0,3,1);
	tris[1].setTriangleIndices(1,3,2);
	tris[2].setTriangleIndices(2,6,5);
	tris[3].setTriangleIndices(5,1,2);
	tris[4].setTriangleIndices(5,6,4);
	tris[5].setTriangleIndices(4,6,7);
	tris[6].setTriangleIndices(7,3,0);
	tris[7].setTriangleIndices(0,4,7);
	tris[8].setTriangleIndices(0,1,4);
	tris[9].setTriangleIndices(4,1,5);
	tris[10].setTriangleIndices(2,3,6);
	tris[11].setTriangleIndices(6,3,7);

	m_geometry->addTriangles(&tris[0], tris.getSize());

	int bytessaved = m_geometry->optimizeGeometry();
}

const hkVector4& hkDisplayAABB::getMinExtent()
{
	return m_minExtent;
}


const hkVector4& hkDisplayAABB::getMaxExtent()
{
	return m_maxExtent;
}

void hkDisplayAABB::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	lines.setSize(24);
	
	lines[0].set(m_minExtent(0),m_minExtent(1),m_minExtent(2));
	lines[1].set(m_minExtent(0),m_maxExtent(1),m_minExtent(2));

	lines[2].set(m_minExtent(0),m_minExtent(1),m_minExtent(2));
	lines[3].set(m_minExtent(0),m_minExtent(1),m_maxExtent(2));

	lines[4].set(m_minExtent(0),m_minExtent(1),m_minExtent(2));
	lines[5].set(m_maxExtent(0),m_minExtent(1),m_minExtent(2));

	lines[6].set(m_maxExtent(0),m_maxExtent(1),m_maxExtent(2));
	lines[7].set(m_maxExtent(0),m_maxExtent(1),m_minExtent(2));

	lines[8].set(m_maxExtent(0),m_maxExtent(1),m_maxExtent(2));
	lines[9].set(m_minExtent(0),m_maxExtent(1),m_maxExtent(2));

	lines[10].set(m_maxExtent(0),m_maxExtent(1),m_maxExtent(2));
	lines[11].set(m_maxExtent(0),m_minExtent(1),m_maxExtent(2));

	lines[12].set(m_minExtent(0),m_maxExtent(1),m_minExtent(2));
	lines[13].set(m_maxExtent(0),m_maxExtent(1),m_minExtent(2));

	lines[14].set(m_minExtent(0),m_maxExtent(1),m_minExtent(2));
	lines[15].set(m_minExtent(0),m_maxExtent(1),m_maxExtent(2));

	lines[16].set(m_maxExtent(0),m_maxExtent(1),m_minExtent(2));
	lines[17].set(m_maxExtent(0),m_minExtent(1),m_minExtent(2));

	lines[18].set(m_minExtent(0),m_maxExtent(1),m_maxExtent(2));
	lines[19].set(m_minExtent(0),m_minExtent(1),m_maxExtent(2));

	lines[20].set(m_minExtent(0),m_minExtent(1),m_maxExtent(2));
	lines[21].set(m_maxExtent(0),m_minExtent(1),m_maxExtent(2));

	lines[22].set(m_maxExtent(0),m_minExtent(1),m_maxExtent(2));
	lines[23].set(m_maxExtent(0),m_minExtent(1),m_minExtent(2));
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
