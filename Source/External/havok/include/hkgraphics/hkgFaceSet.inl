/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline const hkgVertexSet*	hkgFaceSet::getVertices() const 
{ 
	return m_vertices; 
}

inline hkgVertexSet* hkgFaceSet::getVertices() 
{ 
	return m_vertices; 
} 

inline void hkgFaceSet::addPrimitive(hkgFaceSetPrimitive* p)
{
	HK_ASSERT( p->hasData() );
	
	if (p->getType() == HKG_TRI_LIST)
		m_flags = HKG_FACE_SET_UNOPTIMIZED; // status back open again 
	
	m_flags &= ~(HKG_FACE_SET_REORDERED | HKG_FACE_SET_PLATFORM_OPT);

	m_primitives.pushBack(p);
}

inline int hkgFaceSet::getNumPrimitives() const
{
	return m_primitives.getSize();
}

inline const hkgFaceSetPrimitive* hkgFaceSet::getPrimitive(int i) const
{
	HK_ASSERT( i>=0 && i < m_primitives.getSize());
	return m_primitives[i];
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
