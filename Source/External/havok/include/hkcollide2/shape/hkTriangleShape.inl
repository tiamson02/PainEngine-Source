/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

HK_FORCE_INLINE hkTriangleShape::hkTriangleShape()
{
}

HK_FORCE_INLINE hkTriangleShape::hkTriangleShape(const hkVector4& v0, const hkVector4& v1, const hkVector4& v2)
{
	m_vertices[0] = v0;
	m_vertices[1] = v1;
	m_vertices[2] = v2;
}


	/// Returns the hkTriangle.
const hkVector4* hkTriangleShape::getVertices() const
{
	return m_vertices;
}

const hkVector4& hkTriangleShape::getVertex(int i) const
{
	return m_vertices[i];
}

void hkTriangleShape::setVertex(int i, const hkVector4& vertex)
{
	m_vertices[i] = vertex;
}



hkUint32 hkTriangleShape::getUserIdA() const
{
	return m_userIdA;
}

hkUint32 hkTriangleShape::getUserIdB() const
{
	return m_userIdB;
}

void hkTriangleShape::setUserIdA( hkUint32 id )
{
	m_userIdA = id;
}

void hkTriangleShape::setUserIdB( hkUint32 id )
{
	m_userIdB = id;
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
