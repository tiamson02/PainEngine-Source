/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkgVertexSet::resetCounters() 
{	
	m_lastAddedPosIndex = 0;
	m_lastAddedUVIndex = 0;
	m_lastAddedColorIndex = 0;
	m_lastAddedNormalIndex = 0; 
}

inline hkgVertexSet::hkgVertexSet(hkgFaceSet* owner) 
: m_owner( owner), m_numVerts(0), m_stride( 3*sizeof(float) ), m_lockedBuf( HK_NULL ) 
{ 
	resetCounters(); 
}

inline hkgVertexSet::~hkgVertexSet() 
{
	
}		

inline int hkgVertexSet::getNumVerts() const 
{ 
	return m_numVerts; 
}

inline void hkgVertexSet::setPosAddCounter(int i)
{
	m_lastAddedPosIndex = i;
}

inline void hkgVertexSet::setUVAddCounter(int i)
{
	m_lastAddedUVIndex = i;
}

inline void hkgVertexSet::setColorAddCounter(int i)
{
	m_lastAddedColorIndex = i;
}

inline void hkgVertexSet::setNormalAddCounter(int i)
{
	m_lastAddedNormalIndex = i;
}

inline int hkgVertexSet::getPosAddCounter()
{
	return m_lastAddedPosIndex;
}

inline int hkgVertexSet::getUVAddCounter()
{
	return m_lastAddedUVIndex;
}

inline int hkgVertexSet::getColorAddCounter()
{
	return m_lastAddedColorIndex;
}

inline int hkgVertexSet::getNormalAddCounter()
{
	return m_lastAddedNormalIndex;
}

inline int hkgVertexSet::getStrideInBytes()
{
	return m_stride;
}

inline HKG_VERTEXSET_FORMAT hkgVertexSet::getVertexFormat() const
{
	return m_vertexFormat;
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
