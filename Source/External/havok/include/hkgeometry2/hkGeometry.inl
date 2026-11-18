/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// Returns the number of vertices in the hkGeometry.
inline int hkGeometry::getNumVertices() const
{
	return m_vertex_count;
}

// Returns the number of triangles.
inline int hkGeometry::getNumTriangles() const
{
	return m_triangle_count;
}

/// \param i The index of the vertex to return  
inline const hkVector4& hkGeometry::getVertex(int i) const
{
	HK_ASSERT2(i < m_vertex_count, "index passed to hkGeometry::getVertex exceeds number of geometry vertices");
	return m_vertices[i];
}

// Returns the ith triangle (3 indices into the vertex array) of this hkGeometry.
/// \param i The index of the triangle to be returned  
inline const hkTriangleIndices& hkGeometry::getTriangle(int i) const
{
	HK_ASSERT2(i < m_triangle_count, "index passed to hkGeometry::getTriangle exceeds number of geometry triangles");
	return m_triangles[i];
}

// Return the array of vertices representing this hkGeometry.
inline const hkVector4* hkGeometry::getVertices() const
{
	return m_vertices;
}

// Returns the number of vertices in the hkGeometry.
const hkTriangleIndices* hkGeometry::getTriangles() const 
{
	return m_triangles;
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
