/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkCollidable& hkPhantom::getCollidable()
{
	return m_collidable;
}

inline hkWorld* hkPhantom::getOwner()
{ 
	return m_owner; 
}

inline void hkPhantom::setOwner( hkWorld* world ) 
{ 
	m_owner = world; 
}

inline const hkAabb& hkPhantom::getAabb() const
{
	return m_aabb;
}


inline hkPropertyValue hkPhantom::getProperty( hkUint32 key ) const
{
	for (int i = 0; i < m_properties.getSize(); ++i)
	{
		if (m_properties[i].m_key == key)
		{
			return m_properties[i].m_value;
		}
	}

	hkPropertyValue returnValue;
	returnValue.asPtr = HK_NULL;

	return returnValue;
}

inline bool hkPhantom::hasProperty( hkUint32 key ) const
{
	for (int i = 0; i < m_properties.getSize(); ++i)
	{
		if (m_properties[i].m_key == key)
		{
			return true;
		}
	}
	return false;
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
