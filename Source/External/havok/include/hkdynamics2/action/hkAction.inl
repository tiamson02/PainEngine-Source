/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline hkAction::hkAction()
:	m_owner(HK_NULL),
	m_userData(HK_NULL)
{
}

inline hkSimulationIsland* hkAction::getSimulationIsland()
{ 
	return m_island; 
}

inline void hkAction::setSimulationIsland( hkSimulationIsland* isl )
{ 
	m_island = isl; 
}

inline void hkAction::setOwner( hkWorld* owner )
{
	m_owner = owner;
}

inline hkWorld* hkAction::getOwner()
{
	return m_owner;
}

inline void* hkAction::getUserData() const
{
	return m_userData;
}

inline void hkAction::setUserData( void* data )
{
	m_userData = data;
}

inline void hkAction::entityDeletedCallback( hkEntity* entity )
{
	HK_ASSERT2(0, "An action recieved an entity deleted callback. The entity should not have been deleted if the action was still alive.");
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
