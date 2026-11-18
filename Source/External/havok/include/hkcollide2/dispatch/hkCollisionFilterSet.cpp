/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>
#include <hkcollide2/dispatch/hkCollisionFilterSet.h>

hkCollisionFilterSet::~hkCollisionFilterSet()
{
	for ( int i = 0; i < m_collisionFilters.getSize(); ++i )
	{
		m_collisionFilters[i]->removeReference();
	}
}


bool hkCollisionFilterSet::isCollisionEnabled( const hkCollidable& a, const hkCollidable& b ) const
{
	 // check filters
	for ( int i = m_collisionFilters.getSize()-1; i>=0; i-- )
	{
		if(! m_collisionFilters[i]->isCollisionEnabled( a, b))
		{
			return false;
		}
	}
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
