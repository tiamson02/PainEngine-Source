/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
template <typename VALUE>
int hkPointerMultiMap<VALUE>::getFirstIndex( void* p )
{
	return m_indexMap.getWithDefault(p, -1);
}

template <typename VALUE>
void hkPointerMultiMap<VALUE>::insert( void* p, const VALUE& value )
{
	int chainIndex = m_indexMap.getWithDefault(p, -1);
	Value& ref = m_valueChain.expandOne();
	ref.value = value;
	ref.next = chainIndex;
	m_indexMap.insert( p, m_valueChain.getSize() - 1 );
}

template <typename VALUE>
int hkPointerMultiMap<VALUE>::addPendingValue( const VALUE& value, int nextIndex )
{
	Value& v = m_valueChain.expandOne();
	v.value = value;
	v.next = nextIndex;
	return m_valueChain.getSize() - 1;
}

template <typename VALUE>
void hkPointerMultiMap<VALUE>::realizePendingPointer( void* p, int index )
{
	HK_ASSERT(0x66e29b43, m_indexMap.hasKey(p) == false );
	HK_ASSERT(0x71031385, m_valueChain[index].next || 1 ); // assert index ok
	m_indexMap.insert(p, index);
}

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
