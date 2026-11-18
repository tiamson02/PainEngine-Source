/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkArray<hkCollidable*>& hkAabbPhantom::getOverlappingCollidables()
{ 
	return m_overlappingCollidables;
}


inline bool hkAabbPhantom::addFilteredOverlappingCollidable( hkCollidable& collidable )
{
	if ( m_collisionFilter && !m_collisionFilter->isCollisionEnabled(m_collidable,collidable))
	{
		return false;
	}

	m_overlappingCollidables.pushBack( &collidable );
	return true;
	
}

inline bool hkAabbPhantom::removeFilteredOverlappingCollidable( hkCollidable& collidable )
{
	if ( m_collisionFilter && !m_collisionFilter->isCollisionEnabled(m_collidable,collidable))
	{
		return false;
	}

	int index = m_overlappingCollidables.indexOf( &collidable );
	
	if (index < 0) // not in the list. Can happen when filterinfo is changed before this remove.
		return false; 
	
	m_overlappingCollidables.removeAt( index );
	
	return true;
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
