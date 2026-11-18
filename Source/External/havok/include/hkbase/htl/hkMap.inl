/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

template <typename K, typename V>
V& hkMap<K,V>::operator[] (const K& k)
{
	for(int i=0; i<m_storage.getSize(); ++i)
	{
		if( m_storage[i].first == k )
		{
			return m_storage[i].second;
		}
	}
	hkPair<K,V>& p = *m_storage.expandBy(1);
	p.first = k;
	return p.second;
}

template <typename K, typename V>
const V* hkMap<K,V>::find(const K& k) const
{
	for(int i=0; i<m_storage.getSize(); ++i)
	{
		if( m_storage[i].first == k )
		{
			return &m_storage[i].second;
		}
	}
	return HK_NULL;
}

template <typename K, typename V>
int hkMap<K,V>::erase(const K& k)
{
	int rems = 0;
	for(int i = m_storage.getSize() - 1; i >= 0; --i)
	{
		if( m_storage[i].first == k )
		{
			m_storage.removeAt(i);
			++rems;
		}
	}
	return rems;
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
