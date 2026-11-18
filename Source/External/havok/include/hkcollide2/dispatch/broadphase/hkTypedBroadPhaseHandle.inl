/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkTypedBroadPhaseHandle::hkTypedBroadPhaseHandle( int type )
{
	m_type = type;
	m_uniqueId = 0;
	m_collisionFilterInfo = 0;
}

int hkTypedBroadPhaseHandle::getType() const
{ 
	return m_type; 
}

hkUint32 hkTypedBroadPhaseHandle::getUniqueId() const
{ 
	return m_uniqueId; 
}

void hkTypedBroadPhaseHandle::setUniqueId( hkUint32 id ) 
{ 
	m_uniqueId = id; 
}


hkUint32 hkTypedBroadPhaseHandle::getCollisionFilterInfo() const
{
	return m_collisionFilterInfo;
}

void hkTypedBroadPhaseHandle::setCollisionFilterInfo( hkUint32 info )
{
	m_collisionFilterInfo = info;
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
