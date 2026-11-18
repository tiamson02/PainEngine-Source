/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



inline const hkVector4 &hkBoxShape::getHalfExtents() const
{
	return m_halfExtents;
}

inline void hkBoxShape::setHalfExtents(const hkVector4& halfExtents)
{
	HK_ASSERT2( ((halfExtents(0) > 0.0f) && (halfExtents(1) > 0.0f) && (halfExtents(2) > 0.0f)), "hkBoxShape passed a NONPOSITIVE-valued extent");
	m_halfExtents = halfExtents;
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
