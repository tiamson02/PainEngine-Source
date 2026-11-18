/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkLineSegment::hkLineSegment()
{
}

// Instantiate a line segment.
inline hkLineSegment::hkLineSegment( const hkVector4& a, const hkVector4& b )
{
	m_verts[0] = a;
	m_verts[1] = b;
}


/// \param a First vertex
/// \param b Second vertex
inline void hkLineSegment::setLineSegment( const hkVector4& a, const hkVector4& b )
{
	m_verts[0] = a;
	m_verts[1] = b;
}

// Returns the ith vertex on this Triangle (non-const).
/// \param i The index of the vertex to be returned
inline hkVector4& hkLineSegment::getVertex(int i)
{
	HK_ASSERT2( (i == 0) || (i == 1), "index passed to line segment getVertex() is not valid; must be 0 or 1");
	return m_verts[i];
}

// Returns the ith vertex on this Triangle (const).
/// \param i The index of the vertex to be returned
inline const hkVector4& hkLineSegment::getVertex(int i) const
{
	HK_ASSERT2( (i == 0) || (i == 1), "index passed to line segment getVertex() is not valid; must be 0 or 1");
	return m_verts[i];
}

// Set the ith vertex to Vector4 parameter x .
/// \param i The index of the vertex to be set. i should not be greater than 1 or less than 0
/// \param x The new value for the vertex
inline void hkLineSegment::setVertex( unsigned char i, const hkVector4& x )
{
	HK_ASSERT2( (i == 0) || (i == 1), "index passed to line segment setVertex() is not valid; must be 0 or 1");
	m_verts[i] = x;
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
