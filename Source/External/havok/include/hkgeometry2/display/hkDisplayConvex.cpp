/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplayConvex.h>


hkDisplayConvex::hkDisplayConvex(hkGeometry* geom)
:	hkDisplayGeometry(HK_DISPLAY_CONVEX)
{
	m_geometry = geom;
}


void hkDisplayConvex::buildGeometry()
{
  // Do nothing since geometry was passed in	
}


void hkDisplayConvex::getWireframeGeometry(hkArray<hkVector4>& lines)
{
    // This effectively means the lines will be drawn twice
	// but to filter out unique edges would be pretty costly
		
	if(m_geometry != HK_NULL)
	{
		const hkTriangleIndices* tIndices = m_geometry->getTriangles();
		const hkVector4* vertices = m_geometry->getVertices();
		
		long v[3];
	
		for(int i = m_geometry->getNumTriangles(); i >= 0;  i--)
		{
			v[0] = tIndices[i].getVertex(0);
			v[1] = tIndices[i].getVertex(1);
			v[2] = tIndices[i].getVertex(2);
			
			lines.pushBack(vertices[ v[0] ]);
			lines.pushBack(vertices[ v[1] ]);

			lines.pushBack(vertices[ v[0] ]);
			lines.pushBack(vertices[ v[2] ]);			

			lines.pushBack(vertices[ v[1] ]);
			lines.pushBack(vertices[ v[2] ]);
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
