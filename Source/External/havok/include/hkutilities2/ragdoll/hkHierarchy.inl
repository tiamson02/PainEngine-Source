/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkHierarchy::hkHierarchy( const hkArray<hkInt16>& parents )
{
	m_parents = parents;

	// parents must come before children in the list
	for( int i = 0; i < m_parents.getSize(); i++ )
	{
		HK_ASSERT( m_parents[i] < i );
	}
}

inline const hkArray<hkInt16>& hkHierarchy::getParents() const 
{ 
	return m_parents; 
}

inline hkInt16 hkHierarchy::getDepth( hkInt16 i ) const
{
	HK_ASSERT( i < m_parents.getSize() );

	hkInt16 depth = 0;
	
	i = m_parents[i];
	while( -1 != i )
	{
		i = m_parents[i];
		depth++;
	}

	return depth;
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
