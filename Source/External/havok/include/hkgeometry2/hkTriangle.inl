/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

		
inline hkTriangle::hkTriangle()
{
}


// Instantiate a triangle.
inline hkTriangle::hkTriangle( const hkVector4& v1, const hkVector4& v2, const hkVector4& v3 )
{
	m_verts[0] = v1;
	m_verts[1] = v2;
	m_verts[2] = v3;
}


/// \param a First vertex
/// \param b Second vertex
/// \param c Third vertex
inline void hkTriangle::setTriangle( const hkVector4& a, const hkVector4& b, const hkVector4& c )
{
	m_verts[0] = a;
	m_verts[1] = b;
	m_verts[2] = c;
}


// Returns the ith vertex on this Triangle (const).
/// \param i The index of the vertex to be returned
inline const hkVector4& hkTriangle::getVertex(int i) const
{
	HK_ASSERT2( (i >= 0) && (i < 3), "index passed to hkTriangle::getVertex is not valid; must be between 0 and 2");
	return m_verts[i];
}

// Returns the ith vertex on this Triangle (non-const).
/// \param i The index of the vertex to be returned
inline hkVector4& hkTriangle::getVertex(int i)
{
	HK_ASSERT2( (i >= 0) && (i < 3), "index passed to hkTriangle::getVertex is not valid; must be between 0 and 2");
	return m_verts[i];
}


// Set the ith vertex to Vector4 parameter x .
/// \param i The index of the vertex to be set. i should not be greater than 2 or less than 0
/// \param x The new value for the vertex
inline void hkTriangle::setVertex( int i, const hkVector4& x )
{
	HK_ASSERT2( (i >= 0) && (i < 3), "index passed to hkTriangle::setVertex is not valid; must be between 0 and 2");
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
