/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkgFaceSetPrimitive::hkgFaceSetPrimitive( HKG_FACE_RENDER_TYPE t )
: m_type(t), m_indices(HK_NULL), m_vertBufStartIndex(0), m_len(0)
{
}



inline hkgFaceSetPrimitive::~hkgFaceSetPrimitive()
{
	setLength(0);
}

inline int hkgFaceSetPrimitive::getLength() const
{
	return m_len;
}

inline HKG_FACE_RENDER_TYPE hkgFaceSetPrimitive::getType() const
{
	return m_type;
}

inline void hkgFaceSetPrimitive::setVertexBufferStartIndex( int n )
{
	m_vertBufStartIndex = n;
}

inline int hkgFaceSetPrimitive::getVertexBufferStartIndex() const
{
	return m_vertBufStartIndex;
}

inline const unsigned short* hkgFaceSetPrimitive::getIndices() const 
{ 
	return m_indices; 
}

inline unsigned short*	hkgFaceSetPrimitive::getIndices() 
{
	return m_indices; 
}

inline void hkgFaceSetPrimitive::setIndices(unsigned short* conn, int len)
{
	if (m_indices)
		setLength(0); // deallocate previous

	m_len = len;
	m_indices = conn;
}

inline bool hkgFaceSetPrimitive::hasData() const
{
	return (m_len > 0);
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
