/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkReal hkContactPointProperties::getFriction() const
{ 
	return m_friction; 
}

inline void hkContactPointProperties::setFriction( hkReal r )
{ 
	m_friction = r; 
}

inline hkReal hkContactPointProperties::getRestitution() const
{ 
	return m_restitution; 
}

inline void hkContactPointProperties::setRestitution( hkReal r )
{ 
	m_restitution = r; 
}

inline void* hkContactPointProperties::getUserData() const
{ 
	return m_userData; 
}

inline void hkContactPointProperties::setUserData( void* data )
{ 
	m_userData = data; 
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
