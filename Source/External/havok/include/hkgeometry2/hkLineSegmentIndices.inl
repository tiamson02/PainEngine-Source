/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/***
 *** INLINE FUNCTION DEFINITIONS
 ***/


///	In a debug build, the indices will be initialized to 0xFFFF, othewise they are uninitialized. 
inline hkLineSegmentIndices::hkLineSegmentIndices()
{
}

inline hkLineSegmentIndices::hkLineSegmentIndices( const unsigned short vertices[2] )
{
	setLineSegmentIndices(vertices);
}

inline hkLineSegmentIndices::hkLineSegmentIndices( const unsigned short a, const unsigned short b )
{
	setLineSegmentIndices( a, b );
}


inline void hkLineSegmentIndices::setLineSegmentIndices( const unsigned short vertices[2] )
{
	m_verts[0] = vertices[0];
	m_verts[1] = vertices[1];
}

inline void hkLineSegmentIndices::setLineSegmentIndices( const unsigned short a, const unsigned short b )
{
	m_verts[0] = a;
	m_verts[1] = b;
}

inline unsigned short hkLineSegmentIndices::getVertex(unsigned char i) const
{
	HK_ASSERT2( (i == 0) || (i == 1), "index passed to hkLineSegmentIndices::getVertex() is not valid; must be 0 or 1");
	return m_verts[i];
}

inline void hkLineSegmentIndices::setVertex( unsigned char i, unsigned short v)
{
	HK_ASSERT2( (i == 0) || (i == 1), "index passed to hkLineSegmentIndices::setVertex() is not valid; must be 0 or 1");
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
