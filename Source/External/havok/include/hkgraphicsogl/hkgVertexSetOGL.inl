/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkgVertexSetOGL::hkgVertexSetOGL(hkgFaceSet* owner) 
: hkgVertexSet(owner), m_vertsPos(HK_NULL),m_vertsUVs(HK_NULL),	
  m_vertsColors(HK_NULL), m_vertsNormals(HK_NULL)
{ 
}

inline const float*	hkgVertexSetOGL::getPosArray(int vertOffset) const  
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsPos + (vertOffset*3);	  
}

inline float*	hkgVertexSetOGL::getPosArray(int vertOffset) 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsPos+ (vertOffset*3);	  
}

inline const float*	hkgVertexSetOGL::getUVArray(int vertOffset) const    
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsUVs == HK_NULL? HK_NULL : m_vertsUVs + (vertOffset*2);
}

inline float*	hkgVertexSetOGL::getUVArray(int vertOffset) 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsUVs == HK_NULL? HK_NULL : m_vertsUVs + (vertOffset*2);	  
}

inline const float*	hkgVertexSetOGL::getColorArray(int vertOffset) const 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsColors == HK_NULL? HK_NULL : m_vertsColors + (vertOffset*3); 
}

inline float*	hkgVertexSetOGL::getColorArray(int vertOffset) 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsColors == HK_NULL? HK_NULL : m_vertsColors + (vertOffset*3); 
}

inline const float*	hkgVertexSetOGL::getNormalArray(int vertOffset) const 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsNormals + (vertOffset*3);
}

inline float* hkgVertexSetOGL::getNormalArray(int vertOffset) 
{ 
	HK_ASSERT(vertOffset < m_numVerts);
	return m_vertsNormals + (vertOffset*3);
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
