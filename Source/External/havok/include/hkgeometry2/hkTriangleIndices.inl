/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


/// \class hkTriangleIndices
/// A hkTriangleIndices object is represented as three 32-bit indices into a vertex list.
/// We assume the vertices are given in counter-clockwise order (with the normal facing outward).
/// <br><img src="pix/Triangle2.gif">


// Construct the hkTriangleIndices with the given vertex indices.
inline hkTriangleIndices::hkTriangleIndices()
{
}


// Construct the hkTriangleIndices with the given vertex indices.
/// \param vertices Vertex indices to set from.
inline hkTriangleIndices::hkTriangleIndices( const hkUint32 vertices[3] )
{
	setTriangleIndices(vertices);
}


// Set the hkTriangleIndices with the given indices a, b and c (assumed to be indices
// into an array of Vector4s).
/// \param a First index.
/// \param b Second index.
/// \param c Third index.
inline hkTriangleIndices::hkTriangleIndices( hkUint32 a, hkUint32 b, hkUint32 c )
{
	setTriangleIndices( a, b, c );
}


// Construct the hkTriangleIndices with the given vertex indices.
/// \param o hkTriangleIndices to copy from
inline void hkTriangleIndices::setTriangleIndices( const hkTriangleIndices &o )
{
	m_verts[0] = o.m_verts[0];
	m_verts[1] = o.m_verts[1];
	m_verts[2] = o.m_verts[2];
}


/// \param vertices Vertex indices to copy from
inline void hkTriangleIndices::setTriangleIndices( const hkUint32 vertices[3] )
{
	m_verts[0] = vertices[0];
	m_verts[1] = vertices[1];
	m_verts[2] = vertices[2];
}


/// Set the hkTriangleIndices with the given indices a, b and c (assumed to be indices
/// into an array of Vector4s).
/// \param a First index.
/// \param b Second index.
/// \param c Third index.
inline void hkTriangleIndices::setTriangleIndices( hkUint32 a, hkUint32 b, hkUint32 c )
{
	m_verts[0] = a;
	m_verts[1] = b;
	m_verts[2] = c;
}


// Returns the ith vertex on this hkTriangleIndices.
/// \param i Range [0, 1 or 2].
///
inline hkUint32 hkTriangleIndices::getVertex( hkUint32 i ) const
{
	HK_ASSERT2( i < 3, "index passed to hkTriangleIndices::getVertex is not valid; must be between 0 and 2");
	return m_verts[i];
}


// Returns a read-only reference to the first index of this hkTriangleIndices.
///
inline const hkUint32* hkTriangleIndices::getIndexBase() const
{
	return static_cast<const hkUint32*>( m_verts );
}


// Sets the ith vertex on this hkTriangleIndices.
/// \param i Range [0, 1 or 2].
///
inline void hkTriangleIndices::setVertex( hkUint32 i, hkUint32 v )
{
	HK_ASSERT2( i < 3, "index passed to hkTriangleIndices::setVertex is not valid; must be between 0 and 2");
	m_verts[i] = v;
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
