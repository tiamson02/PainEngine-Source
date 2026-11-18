/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayBox.h>

hkDisplayBox::hkDisplayBox(const hkVector4& halfExtents)
:	hkDisplayGeometry(HK_DISPLAY_BOX),
	m_halfExtents(halfExtents)
{
	m_halfExtents = halfExtents;
}

hkDisplayBox::hkDisplayBox()
:	hkDisplayGeometry(HK_DISPLAY_BOX)
{
	m_halfExtents.setZero4();	
}

void hkDisplayBox::setParameters(const hkVector4& halfExtents, const hkTransform& t)
{
	m_halfExtents = halfExtents;
	m_transform = t;
}

/*
8
0.000000 1.000000 1.000000 
1.000000 1.000000 1.000000 
1.000000 0.000000 1.000000 
0.000000 0.000000 1.000000 
0.000000 1.000000 0.000000 
1.000000 1.000000 0.000000 
1.000000 0.000000 0.000000 
0.000000 0.000000 0.000000 
12
3 3 2 1 
3 3 1 0 
3 6 7 4 
3 6 4 5 
3 4 7 3 
3 4 3 0 
3 2 6 5 
3 2 5 1 
3 7 6 2 
3 7 2 3 
3 1 5 4 
3 1 4 0 
*/
void hkDisplayBox::buildGeometry()
{
	m_geometry = new hkGeometry();

	hkArray<hkVector4> verts;
	verts.setSize(8);

	verts[0].set(-m_halfExtents(0), m_halfExtents(1), m_halfExtents(2));
	verts[1].set(m_halfExtents(0), m_halfExtents(1), m_halfExtents(2));
	verts[2].set(m_halfExtents(0), -m_halfExtents(1), m_halfExtents(2));
	verts[3].set(-m_halfExtents(0), -m_halfExtents(1), m_halfExtents(2));
	verts[4].set(-m_halfExtents(0), m_halfExtents(1), -m_halfExtents(2));
	verts[5].set(m_halfExtents(0), m_halfExtents(1), -m_halfExtents(2));
	verts[6].set(m_halfExtents(0), -m_halfExtents(1), -m_halfExtents(2));
	verts[7].set(-m_halfExtents(0), -m_halfExtents(1), -m_halfExtents(2));

	for (int i=0; i < 8; i++)
	{
		verts[i].setTransformedPos( m_transform, verts[i] );
	}

	m_geometry->addVertices(&verts[0], verts.getSize());

	hkArray<hkTriangleIndices> tris;
	tris.setSize(12);

	tris[0].setTriangleIndices(3, 2, 1);
	tris[1].setTriangleIndices(3, 1, 0);
	tris[2].setTriangleIndices(6, 7, 4);
	tris[3].setTriangleIndices(6, 4, 5);
	tris[4].setTriangleIndices(4, 7, 3);
	tris[5].setTriangleIndices(4, 3, 0);
	tris[6].setTriangleIndices(2, 6, 5);
	tris[7].setTriangleIndices(2, 5, 1);
	tris[8].setTriangleIndices(7, 6, 2);
	tris[9].setTriangleIndices(7, 2, 3);
	tris[10].setTriangleIndices(1, 5, 4);
	tris[11].setTriangleIndices(1, 4, 0);

	m_geometry->addTriangles(&tris[0], tris.getSize());

	int bytessaved = m_geometry->optimizeGeometry();
}


void hkDisplayBox::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	lines.setSize(24);

	hkVector4 points[8];

	// points 
	{
		for (int i = 0; i < 8; i++)
		{
			hkVector4 v = m_halfExtents;
			if ( i & 1 )	v(0) *= -1.0f;
			if ( i & 2 )	v(1) *= -1.0f;
			if ( i & 4 )	v(2) *= -1.0f;
			points[i].setTransformedPos(m_transform, v);
		}
	}

	// edges
	
	{
		int k = 0;
		for (int i = 0; i < 8; i++)
		{
			for ( int bit = 1; bit < 8; bit <<= 1 )
			{
				int j = i ^ bit;

				if ( i < j )
				{
					lines[k++] = points[i];
					lines[k++] = points[j];
				}
			}
		}
	}	

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
